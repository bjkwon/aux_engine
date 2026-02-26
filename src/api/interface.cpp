#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "AuxScope.h"
#include "AuxScope_exception.h"
#include <auxe/auxe.h>
#include "utils.h"

struct auxContext : AuxScope {};

string path_join(const vector<string>& parts); // support.cpp

string show_preview(const CVar& sig, int display_precision, int display_limit_x, int display_limit_y, int display_limit_bytes);
string show_preview(const AuxScope* ctx, int display_precision, int display_limit_x, int display_limit_y, int display_limit_bytes);

using namespace std;

#define DEFAULT_FS 22050
constexpr int DEFAULT_DISPLAY_LIMIT_STR = 32;

//extern vector<AuxScope*> xscope;
EngineRuntime* pglobalEnv = nullptr;

// AuxObj is an opaque handle in the public API.
// Inside the library we treat it as a pointer to CVar (which also behaves like CSignals/CTimeSeries).
static inline const CVar* asCVar(AuxObj h) {
    return reinterpret_cast<const CVar*>(h);
}

static inline const CSignals* asCSignals(AuxObj h) {
    return reinterpret_cast<const CSignals*>(h);
}

// Defined in src/func/fft.cpp
CSignal __fft2(auxtype* buf, unsigned int len, void* pargin, void* pargout);

static DebugAction to_internal_action(auxDebugAction a)
{
    switch (a)
    {
    case auxDebugAction::AUX_DEBUG_NO_DEBUG:   return DebugAction::NoDebug;
    case auxDebugAction::AUX_DEBUG_CONTINUE:   return DebugAction::Continue;
    case auxDebugAction::AUX_DEBUG_STEP:       return DebugAction::Step;
    case auxDebugAction::AUX_DEBUG_STEP_OUT:   return DebugAction::StepOut;
    case auxDebugAction::AUX_DEBUG_STEP_IN:    return DebugAction::StepIn;
    case auxDebugAction::AUX_DEBUG_ABORT_BASE: return DebugAction::AbortToBase;
    default:                   return DebugAction::NoDebug; // safe fallback
    }
}

static string resolve_pause_file_for_frame(const AuxScope* scope)
{
    if (!scope || !scope->pEnv) return "";

    auto it = scope->pEnv->udf.find(scope->u.title);
    if (it != scope->pEnv->udf.end() && !it->second.fullname.empty())
        return it->second.fullname;

    auto ibase = scope->pEnv->udf.find(scope->u.base);
    if (ibase != scope->pEnv->udf.end()) {
        auto ilocal = ibase->second.local.find(scope->u.title);
        if (ilocal != ibase->second.local.end() && !ilocal->second.fullname.empty())
            return ilocal->second.fullname;
        if (!ibase->second.fullname.empty())
            return ibase->second.fullname;
    }

    return scope->u.title;
}

auxContext* aux_init(auxConfig* cfg)
{
    srand((unsigned)time(0));
    if (cfg->sample_rate == 0) cfg->sample_rate = DEFAULT_FS;
    if (cfg->display_limit_str <= 0) cfg->display_limit_str = DEFAULT_DISPLAY_LIMIT_STR;
    pglobalEnv = new EngineRuntime(cfg->sample_rate);
    pglobalEnv->AuxPath = cfg->search_paths;
    pglobalEnv->InitBuiltInFunctions();

    // Wrap the public hook (function pointer) into internal DebugHook (std::function)
    if (cfg->debug_hook)
    {
        auxDebugHook user_hook = cfg->debug_hook; // copy the function pointer
        pglobalEnv->debug_hook = [user_hook](const DebugEvent& ev) -> DebugAction
        {
            auxDebugInfo info;
            auxContext* frame_ctx = reinterpret_cast<auxContext*>(ev.frame);
            info.ctx = &frame_ctx;
            info.line = ev.line;
            info.filename = ev.filename;
            auxDebugAction user_action = user_hook(info);
            return to_internal_action(user_action);
        };
    }
    else
    {
        pglobalEnv->debug_hook = DebugHook{};
    }

    AuxScope * psc = new AuxScope(pglobalEnv);
//    xscope.push_back(psc);
    return (auxContext*)psc;
}

void aux_close(auxContext* ctx)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return ;  // null pointer or environment not initialized
    }
    delete frame->pEnv;
    delete frame;
}

string aux_version(auxContext* ctx)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return "";  // null pointer or environment not initialized
    }
    auto& env = *frame->pEnv;
    return env.version;
}

int aux_eval(auxContext** ctx, const string& script, const auxConfig& cfg, string& preview_or_error)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(*ctx);
    if (!*ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    try {
        frame->statusMsg.clear();
        auto nodes = frame->makenodes(script);
        if (!nodes)
            throw frame->emsg.c_str();
        frame->node = nodes;
        frame->Compute();
        preview_or_error = show_preview(frame, cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y, cfg.display_limit_bytes);
        return 0;
    } 
    catch (AuxScope* ast) {
        if (ast->u.debugstatus == abort2base) {
            frame->son.reset();
            preview_or_error = "Aborted to base.";
            return (int)auxEvalStatus::AUX_EVAL_ERROR;
        } else if (ast->u.debugstatus == paused) {
            // paused due to breakpoint
            preview_or_error = "(dbg)";
            *ctx = reinterpret_cast<auxContext*>(ast);
            return (int)auxEvalStatus::AUX_EVAL_PAUSED;
        }
        // fallback
        preview_or_error = "Execution interrupted.";
        return (int)auxEvalStatus::AUX_EVAL_ERROR;
    }
    catch (AuxScope_exception e) {
        preview_or_error = e.getErrMsg();
        return 1;
    }
    catch (const char* msg) {
        preview_or_error = msg;
        return 1;
    }
}

// ===== Helpers using your real classes =====

// Get pointer to channel (CSignals) from a CVar/AuxObj.
// channel_index is 0-based.
static const CSignals* get_channel(const AuxObj& v, int channel_index)
{
    const CSignals* ch = asCSignals(v);
    for (int i = 0; ch && i < channel_index; ++i) {
        ch = ch->next;        // follow CSignals::next
    }
    return ch;
}

static std::string format_time_readable(double ms)
{
    std::ostringstream os;
    os << std::fixed << std::setprecision(1);
    const double v = std::fabs(ms);
    if (v >= 3600000.0)
        os << (ms / 3600000.0) << "h";
    else if (v >= 60000.0)
        os << (ms / 60000.0) << "m";
    else if (v >= 1000.0)
        os << (ms / 1000.0) << "s";
    else
        os << ms << "ms";
    return os.str();
}

static std::string format_ms_range(const CTimeSeries* ch)
{
    if (!ch) return "";
    std::ostringstream oss;
    for (const CTimeSeries* seg = ch; seg; seg = seg->chain) {
        double seg_end = seg->tmark;
        if (seg->GetFs() > 0) {
            seg_end += 1000.0 * static_cast<double>(seg->nSamples) / static_cast<double>(seg->GetFs());
        }
        if (oss.tellp() > 0) oss << " ";
        oss << "(" << format_time_readable(seg->tmark) << "~" << format_time_readable(seg_end) << ")";
    }
    return oss.str();
}

static std::string format_channel_sizes(const CTimeSeries* ch)
{
    if (!ch) return "";
    std::ostringstream oss;
    for (const CTimeSeries* seg = ch; seg; seg = seg->chain) {
        if (oss.tellp() > 0) oss << " ";
        oss << seg->nSamples;
    }
    return oss.str();
}

static std::string format_non_audio_size(const CVar* v)
{
    if (!v) return "";

    const uint16_t t = v->type();
    if (ISNULL(t)) return "0";

    if (ISCELL(t)) {
        return std::to_string(v->cell.size());
    }
    if (ISSTRUT(t)) {
        return std::to_string(v->strut.size());
    }

    const uint64_t len = v->Len();
    if (v->nGroups > 1) {
        return std::to_string(v->nGroups) + "x" + std::to_string(len);
    }
    return std::to_string(len);
}

static std::string strip_preview_header(const std::string& raw)
{
    const auto nl = raw.find('\n');
    if (nl == std::string::npos) {
        return raw;
    }

    std::string body = raw.substr(nl + 1);
    while (!body.empty() && (body.front() == '\n' || body.front() == '\r')) {
        body.erase(body.begin());
    }
    return body;
}

static std::string collapse_preview_lines(std::string text)
{
    auto trim_line = [](std::string s) {
        const auto b = s.find_first_not_of(" \t\r\n");
        if (b == std::string::npos) return std::string{};
        const auto e = s.find_last_not_of(" \t\r\n");
        return s.substr(b, e - b + 1);
    };

    std::vector<std::string> lines;
    std::string cur;
    for (char c : text) {
        if (c == '\n' || c == '\r') {
            const std::string t = trim_line(cur);
            if (!t.empty()) lines.push_back(t);
            cur.clear();
            continue;
        }
        cur.push_back(c);
    }
    const std::string last = trim_line(cur);
    if (!last.empty()) lines.push_back(last);

    if (lines.empty()) return {};
    std::ostringstream oss;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) oss << " ; ";
        oss << lines[i];
    }
    return oss.str();
}

static std::string truncate_text(const std::string& s, int limit)
{
    if (limit <= 0) return s;
    if (static_cast<int>(s.size()) <= limit) return s;
    return s.substr(0, static_cast<size_t>(limit)) + "...";
}

// ===== Implementations of public API =====

uint16_t aux_type(const AuxObj& v)
{
    return asCVar(v)->type();
}

bool aux_is_audio(const AuxObj& v)
{
    auto t = asCVar(v)->type();
    return ISAUDIO(t) || ISAUDIOG(t);
}

int aux_num_channels(const AuxObj& v)
{
    const CSignals* ch = asCSignals(v);
    int n = 0;
    while (ch) {
        ++n;
        ch = ch->next;
    }
    return n;
}

int aux_num_segments(const AuxObj& v, int channel_index)
{
    const CSignals* ch = get_channel(v, channel_index);
    if (!ch) return 0;

    const CTimeSeries* seg = ch;    // CSignals : CTimeSeries
    int n = 0;
    while (seg) {
        ++n;
        seg = seg->chain;          // follow CTimeSeries::chain
    }
    return n;
}

bool aux_get_segment(const AuxObj& v, int channel_index, int segment_index, AuxSignal& out)
{
    const CSignals* ch = get_channel(v, channel_index);
    if (!ch) return false;

    const CTimeSeries* seg = ch;
    for (int i = 0; seg && i < segment_index; ++i) {
        seg = seg->chain;
    }
    if (!seg) return false;

    out.tmark = seg->tmark;
    out.fs = seg->fs;
    out.nSamples = static_cast<size_t>(seg->nSamples);
    out.buf = seg->buf;      // from body::buf (auxtype*)
    out.nSamples = seg->nSamples;
    out.nGroups = seg->nGroups;
    out.bufType = seg->bufType;
    return true;
}

bool aux_fft_power_db(const AuxObj& v, int channel_index, int start_timeline_sample, int num_timeline_samples, int offset_samples, vector<double>& out_db)
{
    out_db.clear();
    if (num_timeline_samples <= 0) return true;

    const size_t flatLen = aux_flatten_channel_length(v, channel_index);
    if (flatLen == 0) return true;
    vector<auxtype> flat(flatLen, 0.0);
    aux_flatten_channel(v, channel_index, flat.data(), flat.size());

    int fs = 0;
    AuxSignal seg0{};
    if (aux_get_segment(v, channel_index, 0, seg0)) {
        fs = seg0.fs;
    }
    if (fs <= 0) fs = 1;

    CVar sig(fs);
    sig.UpdateBuffer((uint64_t)num_timeline_samples);
    for (int i = 0; i < num_timeline_samples; ++i) {
        const int di = start_timeline_sample + i - offset_samples;
        if (di >= 0 && di < static_cast<int>(flatLen)) {
            sig.buf[i] = flat[static_cast<size_t>(di)];
        } else {
            sig.buf[i] = 0.0;
        }
    }

    vector<CVar> fftArgs;
    fftArgs.emplace_back(CVar(0.));               // use full signal length
    fftArgs.emplace_back(CVar((auxtype)fs));      // keep fs in output object
    CSignals fftOut = sig.evoke_getsig2(__fft2, (void*)&fftArgs, nullptr);

    if (!fftOut.IsComplex() || fftOut.nSamples == 0) {
        return false;
    }

    const size_t bins = static_cast<size_t>(fftOut.nSamples / 2 + 1);
    out_db.assign(bins, -80.0);
    const double n = std::max(1.0, static_cast<double>(sig.nSamples));
    constexpr double kFloor = 1e-12;
    for (size_t k = 0; k < bins; ++k) {
        const double mag = std::abs(fftOut.cbuf[k]);
        double amp = mag / n;
        if (k > 0 && k + 1 < bins) {
            amp *= 2.0; // one-sided amplitude for non-DC/non-Nyquist bins
        }
        double db = 20.0 * std::log10(std::max(kFloor, amp));
        db = std::clamp(db, -80.0, 0.0);
        out_db[k] = db;
    }
    return true;
}

/*
* Make a copy of the buf data at the specified time point and the length.
* len is meant to be small enough to make copying of the data buffer easy.
* Used playCallback in audio_play.cpp

bool aux_get_segment_buffer(const AuxObj& v, double tpoint_sec, int len, vector<auxtype>& out1, vector<auxtype>& out2)
{

}
*/
size_t aux_flatten_channel_length(const AuxObj& v, int channel_index)
{
    const CSignals* ch = get_channel(v, channel_index);
    if (!ch) return 0;

    size_t total = 0;
    const CTimeSeries* seg = ch;
    while (seg) {
        total += static_cast<size_t>(seg->nSamples);
        seg = seg->chain;
    }
    return total;
}

size_t aux_flatten_channel(const AuxObj& v, int channel_index, auxtype* out, size_t max_len)
{
    const CSignals* ch = get_channel(v, channel_index);
    if (!ch || !out) return 0;

    size_t written = 0;
    const CTimeSeries* seg = ch;
    while (seg) {
        size_t n = static_cast<size_t>(seg->nSamples);
        if (written + n > max_len) break;

        memcpy(out + written, seg->buf, n * sizeof(auxtype));
        written += n;
        seg = seg->chain;
    }
    return written;
}

int aux_del_var(auxContext* ctx, const string& varname)
{
    auto* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame) return -1;
    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return 1;
    frame->Vars.erase(it);
    return 0;
}

int aux_get_vars(auxContext* ctx, vector<string>& vars)
{
    auto* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame) return -1;
    for (auto v : frame->Vars)
        vars.push_back(v.first);
    return 0;
}

AuxObj aux_get_var(auxContext* ctx, const string& varname)
{
    auto* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame) return nullptr;

    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return nullptr;
    const CVar* cv = &it->second;
    return reinterpret_cast<AuxObj>(cv);
}

vector<AuxObj> aux_get_cell(auxContext* ctx, const string& varname)
{
    vector<AuxObj> out;
    auto* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame) return out;

    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return out;
    const CVar* cv = &it->second;
    auto t = cv->type();
    if (ISCELL(t)) {
        for (auto v : cv->cell)
            out.push_back(reinterpret_cast<AuxObj>(cv));
    }
    return out;
}

map<string, AuxObj> aux_get_struct(auxContext* ctx, const string& varname)
{
    map<string, AuxObj> out;
    auto* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame) return out;

    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return out;
    const CVar* cv = &it->second;
    auto t = cv->type();

    if (ISSTRUT(t)) {
        for (auto v : cv->strut)
            out[v.first] = reinterpret_cast<AuxObj>(cv);
    }
    return out;
}


int aux_describe_var(auxContext* ctx, const AuxObj& v, const auxConfig& cfg, uint16_t& type, string& size, string& preview)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    size.clear();
    if (v == nullptr)
        return 1;
    else
    {
        type = asCVar(v)->type();
        bool string_preview = false;
        if (aux_is_audio(v)) {
            const CSignals* ch1 = get_channel(v, 0);
            const CSignals* ch2 = get_channel(v, 1);

            const std::string s1 = ch1 ? format_channel_sizes(ch1) : "";
            const std::string s2 = ch2 ? format_channel_sizes(ch2) : "";
            if (!s1.empty() && !s2.empty()) size = s1 + " ; " + s2;
            else if (!s1.empty()) size = s1;
            else size = s2;

            const std::string r1 = format_ms_range(ch1);
            const std::string r2 = format_ms_range(ch2);
            if (!r1.empty() && !r2.empty()) {
                preview = r1 + "; " + r2;
            } else if (!r1.empty()) {
                preview = r1;
            } else {
                preview = show_preview(*asCVar(v), cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y, cfg.display_limit_bytes);
            }
        } else {
            const CVar* cv = asCVar(v);
            size = format_non_audio_size(cv);
            if ((type & 0xFFF0) == TYPEBIT_STRING) {
                preview = truncate_text(cv->str(), cfg.display_limit_str);
                string_preview = true;
            } else {
                const std::string raw = show_preview(*cv, cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y, cfg.display_limit_bytes);
                preview = strip_preview_header(raw);
                // Scalar previews can be a single-line output where header stripping removes the value.
                // In that case, keep the raw scalar expression result.
                if (preview.empty() && (type & 0x000F) == 1) {
                    preview = raw;
                }
            }
        }
        if (!string_preview) {
            preview = collapse_preview_lines(preview);
        }
        return 0;
    }
}

int aux_preview_current(auxContext* ctx, const auxConfig& cfg, string& preview)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;
    }
    preview = show_preview(frame, cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y, cfg.display_limit_bytes);
    return 0;
}

/*
-1 : bad ctx / udfname / env
2 : UDF not found
1 : no new breakpoints were added(all already existed)
0 : at least one breakpoint was added successfully
*/
int aux_debug_add_breakpoints(auxContext* ctx, const string& udfname, const vector<int>& lines)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    auto& env = *frame->pEnv;
    auto it = env.udf.find(udfname);
    if (it == env.udf.end()) {
        return 2; // udfname not found
    }

    if (lines.empty()) {
        return 0; // nothing to add
    }

    auto& breaks = it->second.DebugBreaks;

    int added = 0;
    for (int line : lines) {
        // Assume line values are valid, but avoid duplicates
        if (std::find(breaks.begin(), breaks.end(), line) == breaks.end()) {
            breaks.push_back(line);
            ++added;
        }
    }

    // 0 = success (at least one added), 1 = nothing changed (all already present)
    return (added == 0) ? 1 : 0;
}

// In order to clear all breakpoints, make lines[0] 0
int aux_debug_del_breakpoints(auxContext* ctx, const string& udfname, const vector<int>& lines)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    auto& env = *frame->pEnv;
    auto it = env.udf.find(udfname);
    if (it == env.udf.end()) {
        return 2; // udfname not found
    }

    auto& breaks = it->second.DebugBreaks;
    if (lines.empty()) {
        return 0; // nothing to do
    }
    // Special case: clear all breakpoints for this UDF
    if (lines[0] == 0) {
        if (breaks.empty()) {
            return 1; // nothing was set to begin with (optional semantics)
        }
        breaks.clear();
        return 0; // success
    }
    int removed = 0;
    for (int line : lines) {
        auto jt = std::find(breaks.begin(), breaks.end(), -line);
        if (jt != breaks.end()) {
            breaks.erase(jt);
            ++removed;
        }
    }
    if (removed == 0) {
        return 1; // no items in 'lines' matched existing breakpoints
    }
    return 0; // success
}

vector<int> aux_debug_view_breakpoints(auxContext* ctx, const string& udfname, vector<int>& lines)
{
    vector<int> out;
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return out;  // null pointer or environment not initialized; returns with zero length
    }
    auto& env = *frame->pEnv;
    auto it = env.udf.find(udfname);
    if (it == env.udf.end()) {
        return out; // udfname not found
    }
    return it->second.DebugBreaks;
}

int aux_define_udf(auxContext* ctx, const string& udfname, const string& udfpath, string& errstr)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    vector<string> inpath = { udfpath };
    inpath.push_back(udfname + ".aux");
    auto fullpath = path_join(inpath);
    ifstream file(fullpath);
    if (!file.is_open()) {
        errstr = "Error: cannot open ";
        errstr += fullpath;
        return 1;
    }
    string filecontent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    if (file.bad()) {
        errstr = "Error: failed while reading ";
        errstr += fullpath;
        return 1;
    }

    auto udftree = frame->makenodes(filecontent);
    if (udftree)
    {
        frame->node = udftree;
        AstNode* pout = frame->RegisterUDF(frame->node, fullpath.c_str(), filecontent);
        // The following should be after all the throws. Otherwise, the UDF AST will be dangling.
        // To prevent de-allocation of the AST of the UDF when qscope goes out of AuxScope.
        if (frame->node->type == N_BLOCK)
            frame->node->next = NULL;
        else
            frame->node = NULL;
        return 0; // success
    }
    else {
        errstr = "Error: UDF parsing failed. ";
        errstr += frame->emsg;
        return 1;
    }
}

int aux_add_udfpath(auxContext* ctx, const string& udfpath)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    frame->pEnv->AddPath(udfpath);
    return 0;
}

int aux_remove_udfpath(auxContext* ctx, const string& udfpath)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    return frame->pEnv->RemovePath(udfpath);
}

string aux_get_udfpath(auxContext* ctx)
{
    string out;
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return out;  // null pointer or environment not initialized
    }
    for (auto s : frame->pEnv->AuxPath)
        out += s + "\n";
    return out;
}

vector<string> aux_enum_vars(auxContext* ctx)
{
    vector<string> out;
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return out;  // null pointer or environment not initialized
    }
    for (auto v : frame->Vars)
        out.push_back(v.first);
    return out;
}

int aux_get_fs(auxContext* ctx)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    return frame->pEnv->Fs;
}

int aux_set_fs(auxContext* ctx, int fs)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    if (fs <= 0) {
        return -2;  // invalid argument
    }
    frame->pEnv->Fs = fs;
    return 0;
}

auxDebugAction  aux_handle_debug_key(auxContext* ctx, const string& instr)
{
	size_t first = instr.find_first_not_of(" \t\n\r");
	size_t last = instr.find_last_not_of(" \t\n\r");
	//if (first == string::npos || last == string::npos)
	//	throw exception_etc(sc, sc.node, "Invalid debugger contol key.").raise();
	//if (first != last)
	//	throw exception_etc(sc, sc.node, "Invalid Debugger contol key.").raise();
	string msg;
	switch (instr.substr(first).front()) {
	case 's':
	case 'S':
		return auxDebugAction::AUX_DEBUG_STEP;
	case 'i':
	case 'I':
		return auxDebugAction::AUX_DEBUG_STEP_IN;
	case 'o':
	case 'O':
		return auxDebugAction::AUX_DEBUG_STEP_OUT;
	case 'c':
	case 'C':
		return auxDebugAction::AUX_DEBUG_CONTINUE;
	case 'x':
	case 'X':
		return auxDebugAction::AUX_DEBUG_ABORT_BASE;
	default:
		msg = "Invalid debugger contol key";
		//throw exception_etc(sc, sc.node, msg).raise();
        return auxDebugAction::AUX_DEBUG_NO_DEBUG;
    }
}

AUXE_API int aux_register_udf(auxContext* ctx, const string& udfname)
{
    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    string estr;
    AstNode* t_func = frame->ReadUDF(estr, udfname);
    if (t_func) 
        return 0; // inspect t_func for other information.. keep it for later use
    else 
        return 1;
}

auxDebugAction aux_debug_resume(auxContext** ctx, auxDebugAction act)
{
    if (!ctx || !*ctx) return auxDebugAction::AUX_DEBUG_ABORT_BASE;

    AuxScope* frame = reinterpret_cast<AuxScope*>(*ctx);
    if (!frame->pEnv) return auxDebugAction::AUX_DEBUG_ABORT_BASE;

    const bool step_to_caller_next =
        (act == auxDebugAction::AUX_DEBUG_STEP_OUT || act == auxDebugAction::AUX_DEBUG_STEP);
    // Map public action to internal debugstatus
    switch (act) {
    case auxDebugAction::AUX_DEBUG_STEP:
        frame->u.debugstatus = step;
        break;
    case auxDebugAction::AUX_DEBUG_STEP_IN:
        frame->u.debugstatus = step_in;
        break;
    case auxDebugAction::AUX_DEBUG_STEP_OUT:
        frame->u.debugstatus = step_out;
        break;
    case auxDebugAction::AUX_DEBUG_ABORT_BASE:
        frame->u.debugstatus = abort2base;
        break;
    case auxDebugAction::AUX_DEBUG_CONTINUE:
    default:
        frame->u.debugstatus = progress;
        break;
    }

    try {
        frame->ResumePausedUDF();
        AuxScope* parent = frame->dad;
        if (parent && parent->son.get() == frame) {
            parent->FinalizeChildUDFCall();
            parent->CompletePendingAssignmentAfterDebugResume();
            if (step_to_caller_next && parent->pLast && parent->pLast->next) {
                const AstNode* next = parent->pLast->next;
                parent->u.paused_line = next->line;
                parent->u.paused_file = resolve_pause_file_for_frame(parent);
                parent->u.paused_node = next;
                parent->u.debugstatus = paused;
                *ctx = reinterpret_cast<auxContext*>(parent);
                return auxDebugAction::AUX_DEBUG_NO_DEBUG;
            }
            *ctx = reinterpret_cast<auxContext*>(parent);
        }
        return auxDebugAction::AUX_DEBUG_CONTINUE;
    }
    catch (AuxScope* ast) {
        if (ast->u.debugstatus == paused) {
            // hit another breakpoint or step pause
            *ctx = reinterpret_cast<auxContext*>(ast);
            return auxDebugAction::AUX_DEBUG_NO_DEBUG;
        }
        if (ast->u.debugstatus == abort2base) {
            AuxScope* base = ast;
            while (base->dad) base = base->dad;
            base->son.reset();
            *ctx = reinterpret_cast<auxContext*>(base);
            return auxDebugAction::AUX_DEBUG_ABORT_BASE;
        }
        *ctx = reinterpret_cast<auxContext*>(ast);
        return auxDebugAction::AUX_DEBUG_ABORT_BASE;
    }
}

int aux_debug_get_pause_info(auxContext* ctx, auxDebugInfo& out)
{
    if (!ctx) return -1;

    AuxScope* frame = reinterpret_cast<AuxScope*>(ctx);
    if (!frame || !frame->pEnv) return -1;

    // Not paused ? nothing to report
    if (frame->u.debugstatus != paused || frame->u.paused_node == nullptr)
        return -1;

    static thread_local auxContext* paused_frame = nullptr;
    paused_frame = reinterpret_cast<auxContext*>(frame);
    out.ctx = &paused_frame;

    // UDF name
    out.filename = frame->u.paused_file.c_str();

    // Line number
    out.line = frame->u.paused_line;

    return 0;
}
