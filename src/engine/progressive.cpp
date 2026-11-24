#include "AuxScope.h"
#include "filter_block.h"
#include "audio_file_reader.h"
#include <samplerate.h>
#include "AuxScope_exception.h"
#include <atomic>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <thread>

// Only the job's worker writes frames. Publication makes all earlier frames
// visible together. No worker ever owns a pointer to an evaluator or Qt object.
class ProgressiveOutput final : public auxProgressiveAudio {
public:
    std::unique_ptr<double[]> frames, right;
    std::atomic<size_t> count{0};
    std::atomic<bool> stopped{false};
    size_t ready() const override { return count.load(); }
    bool canceled() const override { return stopped.load(); }
    bool finished() const override { return ready() == totalFrames || canceled(); }
    const double* samples() const override { return frames.get(); }
};

const double* aux_progressive_right_samples(const auxProgressiveAudio& source) {
    auto* output = dynamic_cast<const ProgressiveOutput*>(&source);
    return output ? output->right.get() : nullptr;
}

// A frame contains one sample per channel. Both channels of a live source
// become readable together under the producer's shared ready() count.
static bool contiguous_real_channels(const CSignals& signal) {
    if (!signal.nSamples || signal.chain || signal.bufBlockSize != sizeof(double) || signal.IsString())
        return false;
    const auto* right = signal.next;
    return !right || (!right->next && !right->chain && right->bufBlockSize == sizeof(double) &&
        !right->IsString() && right->GetFs() == signal.GetFs() && right->tmark == signal.tmark &&
        right->nSamples == signal.nSamples && right->nGroups == signal.nGroups);
}

class ProgressiveInput {
public:
    explicit ProgressiveInput(const CVar& value) : stream(value.progressive) {
        if (!stream) {
            left.assign(value.buf, value.buf + value.nSamples);
            if (value.next) right.assign(value.next->buf, value.next->buf + value.next->nSamples);
        }
    }
    size_t ready() const { return stream ? stream->ready() : left.size(); }
    size_t totalFrames() const { return stream ? stream->totalFrames : left.size(); }
    const double* leftSamples() const { return stream ? stream->samples() : left.data(); }
    const double* rightSamples() const {
        return stream ? aux_progressive_right_samples(*stream) : (right.empty() ? nullptr : right.data());
    }
    std::shared_ptr<const auxProgressiveAudio> stream;
private:
    std::vector<double> left, right;
};

class ProgressiveAudioJob {
public:
    explicit ProgressiveAudioJob(const char* type) : type(type) {
        static std::atomic<uint64_t> sequence{1};
        id = sequence++;
    }
    std::shared_ptr<ProgressiveOutput> out = std::make_shared<ProgressiveOutput>();
    size_t block = 4096, row = 0, published = 0;
    std::thread worker;
    std::atomic<bool> cancelRequested{false};
    const char* type;
    double id;
    std::string error;
    void cancel() {
        cancelRequested = true;
        if (worker.joinable()) worker.join();
        if (out->ready() < out->totalFrames) out->stopped = true;
    }
    virtual ~ProgressiveAudioJob() = default;
    virtual void step() = 0;
    void start(CVar& value) {
        out->frames.reset(new double[out->totalFrames]);
        while (!out->ready() && !out->finished()) step();
        value.Reset(out->sampleRate);
        value.tmark = out->startMs;
        value.progressive = out;
        publish(value);
    }
    void run_remaining() {
        if (!out->finished()) worker = std::thread([this] {
            try {
                while (!out->finished() && !cancelRequested.load()) step();
            } catch (const std::exception& e) {
                error = e.what();
                out->stopped = true;
            }
        });
    }
    const double* workspaceBuffer[2]{};
    void publishChannel(CSignals& value, const double* src, size_t end, int channel) {
        auto& storage = workspaceBuffer[channel];
        if (published && (!storage || storage != value.buf)) {
            std::unique_ptr<double[]> buffer(new double[out->totalFrames]);
            std::copy_n(src, published, buffer.get());
            value.body::Reset();
            value.buf = buffer.release();
            storage = value.buf;
        }
        if (storage) value.nSamples = end;
        else value.UpdateBuffer(end);
        value.bufType = 'R';
        if (end > published) std::copy(src+published, src+end, value.buf+published);
    }
    void publish(CVar& value) {
        const auto end = out->ready();
        publishChannel(value, out->samples(), end, 0);
        if (out->right) {
            if (!value.next) value.next = new CSignals(out->sampleRate);
            value.next->tmark = value.tmark;
            publishChannel(*value.next, out->right.get(), end, 1);
        }
        if (row) {
            value.nGroups = std::max<size_t>(1, end/row);
            if (value.next) value.next->nGroups = value.nGroups;
        }
        describe(value.strut["h"], end);
        published = end;
    }
    void describe(CVar& h, size_t end) const {
        bool stopped = out->canceled();
        h.strut["status"].SetString(stopped ? (error.empty() ? "canceled" : "error") :
            end == out->totalFrames ? "done" : "running");
        h.strut["progress"].SetValue(out->totalFrames ? 100.0*end/out->totalFrames : 100.0);
        h.strut["completed"].SetValue(end);
        h.strut["id"].SetValue(id);
        h.strut["type"].SetString(type);
        h.strut["error"].SetString(stopped ? error.c_str() : "");
        const size_t last = end ? ((end-1)/block)*block : 0;
        auto& tail = h.strut["block"];
        tail.Reset(out->sampleRate);
        tail.tmark = out->sampleRate > 1 ? out->startMs + 1000.0*last/out->sampleRate : 0;
        tail.UpdateBuffer(end-last);
        if (end > last) std::copy(out->samples()+last,out->samples()+end,tail.buf);
        if (out->right) {
            tail.next = new CSignals(out->sampleRate);
            tail.next->tmark = tail.tmark;
            tail.next->UpdateBuffer(end-last);
            if (end > last) std::copy(out->right.get()+last, out->right.get()+end, tail.next->buf);
        }
    }
};

class ProgressiveFilterJob final : public ProgressiveAudioJob {
public:
    explicit ProgressiveFilterJob(const CVar& source)
        : ProgressiveAudioJob("progressive_filter"), input(source) {}
    ~ProgressiveFilterJob() override { cancel(); }
    ProgressiveInput input;
    std::vector<double> num, den, state, rightState;
    void step() override {
        size_t i = out->ready();
        const size_t available = input.ready();
        if (input.stream && available == i && !input.stream->finished()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return;
        }
        const size_t end = std::min(i + block, available);
        filter_block(input.leftSamples()+i, out->frames.get()+i, end-i, num, den, state);
        if (out->right)
            filter_block(input.rightSamples()+i, out->right.get()+i, end-i, num, den, rightState);
        out->count = end;
        if (input.stream && input.stream->canceled() && end == input.ready()) out->stopped = true;
    }
};

// Keep a single decoder and converter across input blocks so range boundaries,
// stereo alignment and the sinc filter history match a single synchronous read.
class FileAudioJob final : public ProgressiveAudioJob {
public:
    FileAudioJob(AudioFileReader source, int fs)
        : ProgressiveAudioJob("progressive_file"), reader(std::move(source)) {
        out->sampleRate = fs;
        ratio = 1.0 / (static_cast<float>(reader.sampleRate) / fs);
        if (!src_is_valid_ratio(ratio)) throw std::runtime_error("Unsupported file sample-rate ratio.");
        double frames = std::round(reader.frames * ratio);
        if (frames > body::maxBufferBytes / sizeof(double))
            throw std::length_error("Requested signal size is too large.");
        out->totalFrames = static_cast<size_t>(frames);
        if (reader.channels == 2) out->right.reset(new double[out->totalFrames]);
        if (reader.sampleRate != fs) {
            int error = 0;
            converter.reset(src_new(SRC_SINC_MEDIUM_QUALITY, reader.channels, &error));
            if (!converter) throw std::runtime_error(src_strerror(error));
            inputBlock = std::min(block, static_cast<size_t>(std::max(1.0, block / ratio)));
            converted.resize(block * reader.channels);
        }
    }
    ~FileAudioJob() override { cancel(); }

    void step() override {
        const size_t want = std::min(inputBlock, reader.frames - consumed);
        if (reader.read(want, input) != want)
            throw std::runtime_error("Audio file ended before its declared frame count.");
        consumed += want;
        if (!converter) {
            append(input.data(), want);
            return;
        }
        SRC_DATA data{};
        data.src_ratio = ratio;
        data.end_of_input = consumed == reader.frames;
        data.data_in = input.data();
        data.input_frames = want;
        data.data_out = converted.data();
        data.output_frames = block;
        do {
            int error = src_process(converter.get(), &data);
            if (error) throw std::runtime_error(src_strerror(error));
            append(converted.data(), data.output_frames_gen);
            data.input_frames -= data.input_frames_used;
            data.data_in += data.input_frames_used;
        } while (data.input_frames || (data.end_of_input && data.output_frames_gen == data.output_frames));
        if (data.end_of_input && out->ready() < out->totalFrames) {
            // The ordinary resampler zero-pads to the rounded output length.
            auto pos = out->ready();
            std::fill(out->frames.get()+pos, out->frames.get()+out->totalFrames, 0.0);
            if (out->right) std::fill(out->right.get()+pos, out->right.get()+out->totalFrames, 0.0);
            out->count = out->totalFrames;
        }
    }
private:
    void append(const float* data, size_t frames) {
        size_t pos = out->ready();
        frames = std::min(frames, out->totalFrames - pos);
        for (size_t i = 0; i < frames; ++i) {
            out->frames[pos+i] = *data++;
            if (out->right) out->right[pos+i] = *data++;
        }
        out->count = pos + frames;
    }
    AudioFileReader reader;
    std::unique_ptr<SRC_STATE, decltype(&src_delete)> converter{nullptr, src_delete};
    std::vector<float> input, converted;
    double ratio;
    size_t consumed = 0, inputBlock = 4096;
};

void AuxScope::prepare_file_audio(AudioFileReader reader) {
    auto job = std::make_shared<FileAudioJob>(std::move(reader), GetFs());
    job->start(Sig);
    preparedFileJob = std::move(job);
}

void AuxScope::bind_progressive_job(const string& name, std::shared_ptr<ProgressiveAudioJob> job) {
    if (progressiveJobs.count(name)) publish_progressive();
    job->run_remaining();
    progressiveJobs[name] = std::move(job);
}

CVar* AuxScope::try_start_progressive(const AstNode* lhs, const AstNode* rhs) {
    auto fail = [&](const char* msg) { throw exception_etc(*this, rhs, msg).raise(); };
    // A chain dispatches on its last call. A surrounding expression or a
    // subsequent method consumes the complete, ordinarily evaluated value.
    const AstNode* function = rhs;
    while (function->alt && (function->alt->type != N_ARGS || function->alt->alt))
        function = function->alt;
    if (function->type != T_ID && function->type != N_CALL && function->type != N_STRUCT)
        return nullptr;
    const AstNode* args = function->alt;
    if (!(args && args->type == N_ARGS) && function->type != N_CALL &&
        !(function->suppress & AST_FLAG_EXPLICIT_ZERO_ARG_CALL))
        return nullptr;
    if (function == rhs && (!strcmp(function->str, "wave") || !strcmp(function->str, "file"))) {
        // Prime an immediately usable prefix for a simple local WAV. Downloads,
        // MP3 frame counting and computed paths use the deferred evaluator.
        const AstNode* path = args ? args->child : nullptr;
        std::string filename;
        if (path && !path->alt) {
            if (path->type == T_STRING) filename = path->str;
            else if (path->type == T_ID) {
                auto it = Vars.find(path->str);
                if (it != Vars.end() && it->second.IsString()) filename = it->second.str();
            }
        }
        if (filename.empty() || !is_local_wave_source(filename, !strcmp(function->str, "wave")))
            return start_deferred(lhs, rhs, true);
        progressiveFileCall = rhs;
        try { Compute(rhs); }
        catch (...) { progressiveFileCall = nullptr; preparedFileJob.reset(); throw; }
        progressiveFileCall = nullptr;
        SetVar(lhs->str, &Sig);
        if (preparedFileJob) bind_progressive_job(lhs->str, std::move(preparedFileJob));
        return &Sig;
    }
    if (function == rhs && !strcmp(function->str, "progblocks")) {
        if (!args) fail("Use progblocks(audio, window_ms, callback_name[, arguments...]).");
        return start_reducer(lhs, args);
    }
    if (strcmp(function->str, "filt")) return nullptr;
    const AstNode* coeff = args ? args->child : nullptr;
    const AstNode* input = nullptr;
    if (function == rhs && coeff && coeff->type == T_ID && !coeff->alt) {
        input = coeff;
        coeff = coeff->next;
    } else if (rhs->type == T_ID && rhs->alt == function && function->type == N_STRUCT) {
        input = rhs;
    }
    const CVar* source = nullptr;
    if (input) {
        auto it = Vars.find(input->str);
        if (it != Vars.end()) source = &it->second;
    }
    if (!source || !coeff) fail("Progressive assignment supports x.filt(b[, a[, state]]) on a signal variable.");
    const bool audio = source->GetFs() > 500;
    if (!contiguous_real_channels(*source) || (!audio && (source->GetFs() != 1 || source->next)))
        fail("Progressive filt requires a contiguous real vector or aligned mono or stereo audio.");
    auto job = std::make_shared<ProgressiveFilterJob>(*source);
    job->out->sampleRate = source->GetFs();
    job->out->startMs = source->tmark;
    job->out->totalFrames = job->input.totalFrames();
    job->row = job->input.stream ? job->input.stream->rowFrames : (source->nGroups > 1 ? source->Len() : 0);
    job->out->rowFrames = job->row;
    if (job->row) job->block = job->row;
    job->den = {1};
    int n = 0;
    for (; coeff; coeff = coeff->next, ++n) {
        if (n == 3) fail("Too many progressive filter arguments.");
        CVar v = *Compute(coeff);
        if (n == 2 && v.IsEmpty()) continue;
        if (!v.nSamples || v.bufBlockSize != sizeof(double) || v.fs != 1 || v.next || v.chain || v.nGroups != 1)
            fail("Filter coefficients and state must be real scalars or vectors.");
        auto& dst = n == 0 ? job->num : n == 1 ? job->den : job->state;
        dst.assign(v.buf, v.buf+v.nSamples);
    }
    size_t order = std::max(job->num.size(),job->den.size())-1;
    if (job->state.size() > order) fail("Filter state exceeds filter order.");
    job->state.resize(order);
    if (job->input.rightSamples()) {
        job->rightState = job->state;
        job->out->right.reset(new double[job->out->totalFrames]);
    }
    job->start(Sig);
    SetVar(lhs->str,&Sig);
    bind_progressive_job(lhs->str, job);
    return &Sig;
}

// A saved operation handle continues to report its job's terminal state.
static void syncHandle(CVar& value, const CVar& handle) {
    const auto type = value.strut.find("type");
    const auto id = value.strut.find("id");
    if (type != value.strut.end() && type->second.IsString() &&
        type->second.str() == handle.strut.at("type").str() && id != value.strut.end() &&
        id->second.value() == handle.strut.at("id").value()) {
        value = handle;
        return;
    }
    for (auto& item : value.strut) syncHandle(item.second, handle);
    for (auto& item : value.cell) syncHandle(item, handle);
}

bool AuxScope::cancel_progressive(const CVar& handle) {
    auto type = handle.strut.find("type");
    if (type == handle.strut.end() || !type->second.IsString() ||
        (type->second.str() != "progressive_filter" && type->second.str() != "progressive_file") || !handle.strut.count("id")) return false;
    for (auto& entry : progressiveJobs)
        if (entry.second->id == handle.strut.at("id").value()) entry.second->cancel();
    publish_progressive();
    return true;
}

int AuxScope::publish_progressive() {
    int changed = 0;
    for (auto it = progressiveJobs.begin(); it != progressiveJobs.end();) {
        auto& job = *it->second;
        auto var = Vars.find(it->first);
        if (var == Vars.end() || var->second.progressive != job.out) {
            job.cancel();
            CVar terminal;
            job.describe(terminal, job.out->ready());
            for (auto& v : Vars) syncHandle(v.second, terminal);
            ++changed;
            it = progressiveJobs.erase(it);
            continue;
        }
        const bool stopped = job.out->canceled();
        if (job.published != job.out->ready() || stopped) {
            job.publish(var->second);
            const auto& handle = var->second.strut.at("h");
            for (auto& v : Vars) if (&v.second != &var->second) syncHandle(v.second, handle);
            ++changed;
        }
        if (job.published == job.out->totalFrames || stopped) it = progressiveJobs.erase(it);
        else ++it;
    }
    return changed;
}

// A reducer runs on the evaluator thread in bounded batches. Its cursor is
// independent of both source publication blocks and screen refreshes.
class BlockReducer {
public:
    BlockReducer(EngineRuntime* env, const CVar& src) : frame(env), input(src) {}
    ~BlockReducer() {
        if (output && !output->finished()) output->stopped = true;
        if (frame.node) yydeleteAstNode(frame.node, 0);
        frame.node = nullptr;
    }
    AuxScope frame;
    ProgressiveInput input;
    std::shared_ptr<ProgressiveOutput> output;
    size_t cursor = 0, windowIndex = 0;
    double windowFrames = 0, origin = 0;
    int fs = 0;
    double id = 0;

    void publish_audio(const CVar& acc, size_t end) {
        const auto handle = acc.strut.find("h");
        if (handle == acc.strut.end()) {
            if (output) throw std::runtime_error("A streaming callback must publish h.completed audio for every block.");
            return;
        }
        const auto& members = handle->second.strut;
        auto flag = members.find("stream");
        if (!output && (flag == members.end() || !ISSCALARG(flag->second.type()) || flag->second.value() == 0))
            return;
        auto item = members.find("completed");
        if (item == members.end())
            throw std::runtime_error("A streaming callback must publish h.completed audio for every block.");
        const CVar& audio = item->second;
        const size_t capacity = input.totalFrames();
        if (!contiguous_real_channels(audio) || audio.GetFs() != fs ||
            audio.nSamples != end || end > capacity)
            throw std::runtime_error("h.completed must be aligned mono or stereo audio with one frame per processed input frame at the source sample rate.");
        if (!output) {
            auto stream = std::make_shared<ProgressiveOutput>();
            stream->totalFrames = capacity;
            stream->sampleRate = fs;
            stream->startMs = origin;
            stream->frames.reset(new double[capacity]);
            if (audio.next) stream->right.reset(new double[capacity]);
            output = std::move(stream);
        }
        if (bool(output->right) != bool(audio.next))
            throw std::runtime_error("A streaming callback must preserve its output channel count.");
        const size_t previous = output->ready();
        if (previous > end || std::memcmp(output->samples(), audio.buf, previous * sizeof(double)) ||
            (audio.next && std::memcmp(output->right.get(), audio.next->buf, previous * sizeof(double))))
            throw std::runtime_error("A streaming callback must preserve all previously published samples.");
        std::copy(audio.buf + previous, audio.buf + end, output->frames.get() + previous);
        if (audio.next)
            std::copy(audio.next->buf + previous, audio.next->buf + end, output->right.get() + previous);
        output->count = end;
    }

    bool advance(CVar& dest) {
        const size_t limit = input.ready();
        const bool terminal = !input.stream || (input.stream->finished() && limit == input.ready());
        // Run one callback per poll so the frontend can publish between windows.
        if (cursor < limit) {
            size_t end = std::max(cursor+1, static_cast<size_t>(std::llround((windowIndex+1)*windowFrames)));
            if (end > limit && !terminal) return false;
            end = std::min(end, limit);
            CVar& block = frame.Vars["chunk"];
            block.Reset(fs);
            block.UpdateBuffer(end-cursor);
            std::copy(input.leftSamples()+cursor, input.leftSamples()+end, block.buf);
            if (const double* right = input.rightSamples()) {
                block.next = new CSignals(fs);
                block.next->UpdateBuffer(end-cursor);
                std::copy(right+cursor, right+end, block.next->buf);
            }
            frame.Vars["timestamp"].SetValue(origin + 1000.0*cursor/fs);
            frame.Vars["first"].SetValue(windowIndex == 0);
            frame.Compute();
            const CVar& acc = frame.Vars.at("acc");
            publish_audio(acc, end);
            dest = acc;
            if (output) dest.strut.at("h").strut.at("completed").progressive = output;
            cursor = end;
            ++windowIndex;
        }
        auto& handle = dest.strut["h"];
        handle.strut["id"].SetValue(id);
        handle.strut["type"].SetString("progressive_reducer");
        handle.strut["samples_completed"].SetValue(cursor);
        bool done = terminal && cursor == limit;
        if (done && output && cursor < output->totalFrames) output->stopped = true;
        handle.strut["status"].SetString(done ? (input.stream && input.stream->canceled() ? "canceled" : "done") : "running");
        return done;
    }
};

CVar* AuxScope::start_reducer(const AstNode* lhs, const AstNode* call) {
    auto fail = [&](const char* msg) { throw exception_etc(*this, call, msg).raise(); };
    const AstNode* arg = call->child;
    if (!arg || !arg->next || !arg->next->next)
        fail("Use progblocks(audio, window_ms, callback_name[, arguments...]).");
    CVar src = *Compute(arg);
    if (src.GetFs() <= 500 || !contiguous_real_channels(src))
        fail("progblocks requires contiguous real mono or aligned stereo audio.");
    auto job = std::make_shared<BlockReducer>(pEnv, src);
    job->fs = src.GetFs();
    job->origin = src.tmark;
    CVar duration = *Compute(arg = arg->next);
    if (!ISSCALARG(duration.type()) || !std::isfinite(duration.value()) || duration.value() <= 0 ||
        duration.value() > 2147483647) fail("Invalid progblocks window duration.");
    job->windowFrames = std::max(1.0, duration.value()*job->fs/1000.0);
    CVar callback = *Compute(arg = arg->next);
    if (!callback.IsString()) fail("progblocks callback name must be a string.");
    const auto name = callback.str();
    if (name.empty() || name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != string::npos ||
        isdigit(static_cast<unsigned char>(name.front())) || pEnv->IsValidBuiltin(name))
        fail("Expected a UDF name for progblocks.");
    std::string expr = "acc="+name+"(chunk,timestamp,acc,first";
    for (int i = 0; (arg = arg->next); ++i) {
        const std::string key = "param"+std::to_string(i);
        job->frame.Vars[key] = *Compute(arg);
        expr += ","+key;
    }
    job->frame.node = job->frame.makenodes(expr+")");
    if (!job->frame.node) fail("Invalid progblocks callback invocation.");
    job->frame.Vars["acc"] = CVar();
    static std::atomic<uint64_t> sequence{1};
    job->id = sequence++;
    Sig.Reset();
    Sig.strut["h"].strut["id"].SetValue(job->id);
    Sig.strut["h"].strut["type"].SetString("progressive_reducer");
    Sig.strut["h"].strut["status"].SetString("running");
    SetVar(lhs->str,&Sig);
    reducers[lhs->str] = job;
    return &Sig;
}

int AuxScope::publish_reducers() {
    int changes = 0;
    for (auto it = reducers.begin(); it != reducers.end();) {
        auto var = Vars.find(it->first);
        auto& job = *it->second;
        if (var == Vars.end() || !var->second.strut.count("h") ||
            !var->second.strut.at("h").strut.count("type") ||
            var->second.strut.at("h").strut.at("type").str() != "progressive_reducer" ||
            !var->second.strut.at("h").strut.count("id") ||
            var->second.strut.at("h").strut.at("id").value() != job.id) {
            it = reducers.erase(it);
            continue;
        }
        bool done = true;
        size_t before = job.cursor;
        std::string error;
        try { done = job.advance(var->second); }
        catch (const AuxScope_exception& e) { error = e.outstr; }
        catch (const std::exception& e) { error = e.what(); }
        catch (const char* msg) { error = msg ? msg : "Callback failed"; }
        if (!error.empty()) {
            auto& h = var->second.strut["h"];
            h.strut["id"].SetValue(job.id);
            h.strut["type"].SetString("progressive_reducer");
            h.strut["samples_completed"].SetValue(job.cursor);
            h.strut["status"].SetString("error");
            h.strut["error"].SetString(error.c_str());
        }
        if (before != job.cursor || done) ++changes;
        if (done) it = reducers.erase(it);
        else ++it;
    }
    return changes;
}
