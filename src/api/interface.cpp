#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include "AuxScope.h"
#include "AuxScope_exception.h"
#include <auxe/auxe.h>
#include "utils.h"

struct auxContext : AuxScope {};

string path_join(const vector<string>& parts); // support.cpp

string show_preview(const CVar& sig, int display_precision, int display_limit_x, int display_limit_y);
string show_preview(const AuxScope* ctx, int display_precision, int display_limit_x, int display_limit_y);

using namespace std;

#define DEFAULT_FS 22050

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

auxContext* aux_init(auxConfig* cfg)
{
    srand((unsigned)time(0));
    if (cfg->sample_rate == 0) cfg->sample_rate = DEFAULT_FS;
    pglobalEnv = new EngineRuntime(cfg->sample_rate);
    pglobalEnv->AuxPath = cfg->search_paths;
    pglobalEnv->InitBuiltInFunctions();

    AuxScope * psc = new AuxScope(pglobalEnv);
//    xscope.push_back(psc);
    return (auxContext*)psc;
}

void aux_close(auxContext* ctx)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return ;  // null pointer or environment not initialized
    }
    delete frame->pEnv;
    delete frame;
}

string aux_version(auxContext* ctx)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return "";  // null pointer or environment not initialized
    }
    auto& env = *frame->pEnv;
    return env.version;
}

int aux_eval(auxContext* ctx, const string& script, const auxConfig& cfg, string& preview_or_error)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    try {
        frame->statusMsg.clear();
        auto nodes = frame->makenodes(script);
        if (!nodes)
            throw frame->emsg.c_str();
        frame->node = nodes;
        frame->Compute();
        preview_or_error = show_preview(frame, cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y);
        return 0;
    } 
    catch (AuxScope* ast) {
        // When aborting from udf. 
        // ??? 
        assert(ast->u.debugstatus == abort2base);
        frame->son.reset();
        return 1;
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
    auto* frame = static_cast<AuxScope*>(ctx);
    if (!frame) return -1;
    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return 1;
    frame->Vars.erase(it);
    return 0;
}

int aux_get_vars(auxContext* ctx, vector<string>& vars)
{
    auto* frame = static_cast<AuxScope*>(ctx);
    if (!frame) return -1;
    for (auto v : frame->Vars)
        vars.push_back(v.first);
    return 0;
}

AuxObj aux_get_var(auxContext* ctx, const string& varname)
{
    auto* frame = static_cast<AuxScope*>(ctx);
    if (!frame) return nullptr;

    auto it = frame->Vars.find(varname);
    if (it == frame->Vars.end()) return nullptr;
    const CVar* cv = &it->second;
    return reinterpret_cast<AuxObj>(cv);
}

vector<AuxObj> aux_get_cell(auxContext* ctx, const string& varname)
{
    vector<AuxObj> out;
    auto* frame = static_cast<AuxScope*>(ctx);
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
    auto* frame = static_cast<AuxScope*>(ctx);
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


int aux_describe_var(auxContext* ctx, const AuxObj& v, uint16_t& typeName, const auxConfig& cfg, string& preview)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    if (v == nullptr)
        return 1;
    else
    {
        typeName = asCVar(v)->type();
        preview = show_preview(frame->Sig, cfg.display_precision, cfg.display_limit_x, cfg.display_limit_y);
        return 0;
    }
}

/*
-1 : bad ctx / udfname / env
2 : UDF not found
1 : no new breakpoints were added(all already existed)
0 : at least one breakpoint was added successfully
*/
int aux_debug_add_breakpoints(auxContext* ctx, const string& udfname, const vector<int>& lines)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
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
    AuxScope* frame = static_cast<AuxScope*>(ctx);
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
        auto jt = std::find(breaks.begin(), breaks.end(), line);
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
    AuxScope* frame = static_cast<AuxScope*>(ctx);
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
    AuxScope* frame = static_cast<AuxScope*>(ctx);
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
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    frame->pEnv->AddPath(udfpath);
    return 0;
}

int aux_remove_udfpath(auxContext* ctx, const string& udfpath)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    return frame->pEnv->RemovePath(udfpath);
}

string aux_get_udfpath(auxContext* ctx)
{
    string out;
    AuxScope* frame = static_cast<AuxScope*>(ctx);
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
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return out;  // null pointer or environment not initialized
    }
    for (auto v : frame->Vars)
        out.push_back(v.first);
    return out;
}

int aux_get_fs(auxContext* ctx)
{
    AuxScope* frame = static_cast<AuxScope*>(ctx);
    if (!ctx || !frame->pEnv) {
        return -1;  // null pointer or environment not initialized
    }
    return frame->pEnv->Fs;
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
		return auxDebugAction::Step;
	case 'i':
	case 'I':
		return auxDebugAction::StepIn;
	case 'o':
	case 'O':
		return auxDebugAction::StepOut;
	case 'c':
	case 'C':
		return auxDebugAction::Continue;
	case 'x':
	case 'X':
		return auxDebugAction::AbortToBase;
	default:
		msg = "Invalid debugger contol key";
		//throw exception_etc(sc, sc.node, msg).raise();
        return auxDebugAction::NoDebug;
    }
}
