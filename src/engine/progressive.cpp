#include "AuxScope.h"
#include "filter_block.h"
#include "AuxScope_exception.h"
#include <atomic>
#include <chrono>
#include <cstring>
#include <stdexcept>
#include <thread>

// Only the job's worker writes frames. Publication makes all earlier frames
// visible together. No worker ever owns a pointer to an evaluator or Qt object.
class FilterOutput final : public auxProgressiveAudio {
public:
    std::unique_ptr<double[]> frames;
    std::atomic<size_t> count{0};
    std::atomic<bool> stopped{false};
    size_t ready() const override { return count.load(); }
    bool canceled() const override { return stopped.load(); }
    bool finished() const override { return ready() == totalFrames || canceled(); }
    const double* samples() const override { return frames.get(); }
};

class ProgressiveFilterJob {
public:
    std::shared_ptr<FilterOutput> out = std::make_shared<FilterOutput>();
    std::shared_ptr<const auxProgressiveAudio> upstream;
    std::vector<double> input, num, den, state;
    size_t block = 4096, row = 0, published = 0;
    std::thread worker;
    std::atomic<bool> cancelRequested{false};
    double id = 0;
    void cancel() {
        cancelRequested = true;
        if (worker.joinable()) worker.join();
        if (out->ready() < out->totalFrames) out->stopped = true;
    }
    ~ProgressiveFilterJob() {
        cancel();
    }
    void step() {
        size_t i = out->ready();
        const size_t available = upstream ? upstream->ready() : input.size();
        const size_t end = std::min(i + block, available);
        const double* src = upstream ? upstream->samples() : input.data();
        filter_block(src+i, out->frames.get()+i, end-i, num, den, state);
        out->count = end;
        if (upstream && upstream->canceled() && end == available) out->stopped = true;
    }
    void run() {
        while (!out->finished() && !cancelRequested.load()) {
            if (upstream && upstream->ready() == out->ready() && !upstream->finished())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            else step();
        }
    }
    const double* workspaceBuffer = nullptr;
    void publish(CVar& value) {
        const auto end = out->ready();
        // Reserve address space once in the bound workspace variable. Touch
        // only the suffix actually published. The launch preview stays small.
        if (published && (!workspaceBuffer || workspaceBuffer != value.buf)) {
            std::unique_ptr<double[]> storage(new double[out->totalFrames]);
            std::copy_n(out->samples(), published, storage.get());
            value.body::Reset();
            value.buf = storage.release();
            workspaceBuffer = value.buf;
        }
        if (workspaceBuffer) value.nSamples = end;
        else value.UpdateBuffer(end);
        std::copy(out->samples()+published, out->samples()+end, value.buf+published);
        if (row) value.nGroups = std::max<size_t>(1, end/row);
        describe(value.strut["h"], end);
        published = end;
    }
    void describe(CVar& h, size_t end) const {
        h.strut["status"].SetString(out->canceled() ? "canceled" : end == out->totalFrames ? "done" : "running");
        h.strut["progress"].SetValue(100.0*end/out->totalFrames);
        h.strut["completed"].SetValue(end);
        h.strut["id"].SetValue(id);
        h.strut["type"].SetString("progressive_filter");
        h.strut["error"].SetString("");
        const size_t last = end ? ((end-1)/block)*block : 0;
        auto& tail = h.strut["block"];
        tail.Reset(out->sampleRate);
        tail.tmark = out->sampleRate > 1 ? out->startMs + 1000.0*last/out->sampleRate : 0;
        tail.UpdateBuffer(end-last);
        std::copy(out->samples()+last,out->samples()+end,tail.buf);
    }
};

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
    if (!source->nSamples || source->next || source->chain || source->bufBlockSize != sizeof(double) ||
        source->IsString() || (source->GetFs()!=1 && source->GetFs()<=500))
        fail("Progressive filt requires a contiguous real mono signal or vector.");
    auto job = std::make_shared<ProgressiveFilterJob>();
    static std::atomic<uint64_t> sequence{1};
    job->id = sequence++;
    job->out->sampleRate = source->GetFs();
    job->out->startMs = source->tmark;
    job->upstream = source->progressive;
    if (!job->upstream) job->input.assign(source->buf, source->buf + source->nSamples);
    job->out->totalFrames = job->upstream ? job->upstream->totalFrames : source->nSamples;
    job->row = job->upstream ? job->upstream->rowFrames : (source->nGroups > 1 ? source->Len() : 0);
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
    job->out->frames.reset(new double[job->out->totalFrames]);
    job->step();
    Sig.Reset(job->out->sampleRate);
    Sig.tmark = job->out->startMs;
    Sig.progressive = job->out;
    job->publish(Sig);
    if (!job->out->finished()) job->worker = std::thread(&ProgressiveFilterJob::run, job.get());
    SetVar(lhs->str,&Sig);
    if (progressiveJobs.count(lhs->str)) publish_progressive();
    progressiveJobs[lhs->str] = job;
    return &Sig;
}

// A saved operation handle continues to report its job's terminal state.
static void syncHandle(CVar& value, const CVar& handle) {
    const auto type = value.strut.find("type");
    const auto id = value.strut.find("id");
    if (type != value.strut.end() && type->second.IsString() &&
        type->second.str() == "progressive_filter" && id != value.strut.end() &&
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
        type->second.str() != "progressive_filter" || !handle.strut.count("id")) return false;
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
        if (job.published != job.out->ready() || job.out->canceled()) {
            job.publish(var->second);
            const auto& handle = var->second.strut.at("h");
            for (auto& v : Vars) if (&v.second != &var->second) syncHandle(v.second, handle);
            ++changed;
        }
        if (job.published == job.out->totalFrames || job.out->canceled()) it = progressiveJobs.erase(it);
        else ++it;
    }
    return changed;
}

// A reducer runs on the evaluator thread in bounded batches. Its cursor is
// independent of both source publication blocks and screen refreshes.
class BlockReducer {
public:
    explicit BlockReducer(EngineRuntime* env) : frame(env) {}
    ~BlockReducer() {
        if (output && !output->finished()) output->stopped = true;
        if (frame.node) yydeleteAstNode(frame.node, 0);
        frame.node = nullptr;
    }
    AuxScope frame;
    std::shared_ptr<const auxProgressiveAudio> source;
    std::shared_ptr<FilterOutput> output;
    std::vector<double> input;
    size_t cursor = 0, windowIndex = 0;
    double windowFrames = 0, origin = 0;
    int fs = 0;
    double id = 0;

    void publish_audio(CVar& handle) {
        auto flag = handle.strut.find("stream");
        if (!output && (flag == handle.strut.end() || !ISSCALARG(flag->second.type()) || flag->second.value() == 0))
            return;
        auto item = handle.strut.find("completed");
        if (item == handle.strut.end())
            throw std::runtime_error("A streaming callback must publish h.completed audio for every block.");
        CVar& audio = item->second;
        const size_t capacity = source ? source->totalFrames : input.size();
        if (audio.GetFs() != fs || audio.next || audio.chain || audio.bufBlockSize != sizeof(double) ||
            audio.nSamples != cursor || audio.nSamples > capacity)
            throw std::runtime_error("h.completed must be contiguous mono audio with one output sample per processed input sample and the same sample rate.");
        if (!output) {
            output = std::make_shared<FilterOutput>();
            output->totalFrames = capacity;
            output->sampleRate = fs;
            output->startMs = origin;
            output->frames.reset(new double[capacity]);
        }
        const size_t previous = output->ready();
        if (previous > cursor || (previous && std::memcmp(output->samples(), audio.buf, previous * sizeof(double)) != 0))
            throw std::runtime_error("A streaming callback must preserve all previously published samples.");
        std::copy(audio.buf + previous, audio.buf + cursor, output->frames.get() + previous);
        output->count = cursor;
        audio.progressive = output;
    }

    bool advance(CVar& dest) {
        const size_t limit = source ? source->ready() : input.size();
        const bool terminal = !source || (source->finished() && limit == source->ready());
        const double* data = source ? source->samples() : input.data();
        // A callback may do substantial interpreted DSP work. Yield after each
        // window so the frontend can publish progress and process input.
        for (int budget = 1; cursor < limit && budget > 0; --budget) {
            size_t end = std::max(cursor+1, static_cast<size_t>(std::llround((windowIndex+1)*windowFrames)));
            if (end > limit && !terminal) break;
            end = std::min(end, limit);
            CVar& block = frame.Vars["chunk"];
            block.Reset(fs);
            block.UpdateBuffer(end-cursor);
            std::copy(data+cursor,data+end,block.buf);
            frame.Vars["timestamp"].SetValue(origin + 1000.0*cursor/fs);
            frame.Vars["first"].SetValue(windowIndex == 0);
            frame.Compute();
            cursor = end;
            ++windowIndex;
        }
        dest = frame.Vars["acc"];
        auto& handle = dest.strut["h"];
        publish_audio(handle);
        handle.strut["id"].SetValue(id);
        handle.strut["type"].SetString("progressive_reducer");
        handle.strut["samples_completed"].SetValue(cursor);
        bool done = terminal && cursor == limit;
        if (done && output && cursor < output->totalFrames) output->stopped = true;
        handle.strut["status"].SetString(done ? (source && source->canceled() ? "canceled" : "done") : "running");
        return done;
    }
};

CVar* AuxScope::start_reducer(const AstNode* lhs, const AstNode* call) {
    auto fail = [&](const char* msg) { throw exception_etc(*this, call, msg).raise(); };
    const AstNode* arg = call->child;
    if (!arg || !arg->next || !arg->next->next)
        fail("Use progblocks(audio, window_ms, callback_name[, arguments...]).");
    CVar src = *Compute(arg);
    if (src.GetFs() <= 500 || !src.nSamples || src.chain || src.next || src.bufBlockSize != sizeof(double))
        fail("progblocks requires contiguous real mono audio.");
    auto job = std::make_shared<BlockReducer>(pEnv);
    job->source = src.progressive;
    if (!job->source) job->input.assign(src.buf,src.buf+src.nSamples);
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
            if (job.cursor) var->second = job.frame.Vars["acc"];
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
