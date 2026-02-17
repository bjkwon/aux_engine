#pragma once

// Public auxe (AUX Engine) API.
// NOTE: This is a C++ API (uses std::string / std::vector / std::map).

#include <string>
#include <vector>
#include <map>
#include <complex>

using std::string;
using std::vector;
using std::map;
using std::complex;

// ---- DLL export/import (Windows) ----
#if defined(_WIN32) || defined(__CYGWIN__)
  #if defined(AUXE_BUILD_DLL)
    #define AUXE_API __declspec(dllexport)
  #else
    #define AUXE_API __declspec(dllimport)
  #endif
#else
  #define AUXE_API
#endif

typedef struct auxContext auxContext;
// AuxObj is opaque, but actually a pointer type
struct _AuxObj;
using AuxObj = const _AuxObj*;

enum class auxDebugAction {
    AUX_DEBUG_NO_DEBUG = -1,
    AUX_DEBUG_CONTINUE = 0,
    AUX_DEBUG_STEP,
    AUX_DEBUG_STEP_OUT,
    AUX_DEBUG_STEP_IN,
    AUX_DEBUG_ABORT_BASE
};

enum class auxEvalStatus {
    AUX_EVAL_OK = 0,
    AUX_EVAL_ERROR = 1,
    AUX_EVAL_PAUSED = 2
};

struct auxDebugInfo {
    auxContext** ctx;   // current context / frame
    string filename;
    int line;          // current line number in UDF (or -1 if unknown)
};

using auxDebugHook = auxDebugAction(*)(const auxDebugInfo&);

typedef double auxtype;

struct auxStruct {
    const void* data;         // pointer to interleaved samples
    size_t         numFrames;    // number of frames (per channel)
    int            numChannels;  // 1 = mono, 2 = stereo, ...
    double         sampleRate;   // e.g. 8000.0
};

struct AuxSignal {
    uint64_t nSamples;
    uint64_t nGroups;
    union
    {
        auxtype* buf;
        complex<auxtype>* cbuf;
        char* strbuf;
        bool* logbuf;
    };
    char bufType;
    int fs;
    double tmark;
};

typedef struct {
    int sample_rate;
    int display_precision;
    int display_limit_x;
    int display_limit_y;
    int display_limit_bytes;
    vector<string> search_paths;
    auxDebugHook debug_hook;
} auxConfig;

AUXE_API string aux_version(auxContext* ctx);

AUXE_API auxContext* aux_init(auxConfig* cfg);
AUXE_API void        aux_close(auxContext* ctx);

AUXE_API int         aux_eval(auxContext** ctx, const string& script, const auxConfig& cfg, string& preview); // returns 0 for success, preview shows the outcome; returns 1 for error preview shows the error
AUXE_API int         aux_del_var(auxContext* ctx, const string& varname);
AUXE_API uint16_t    aux_type(const AuxObj& v);
AUXE_API bool        aux_is_audio(const AuxObj& v);
AUXE_API int         aux_num_channels(const AuxObj& v);
AUXE_API int         aux_num_segments(const AuxObj& v, int channel_index);
AUXE_API bool        aux_get_segment(const AuxObj& v, int channel_index, int segment_index, AuxSignal& out);
AUXE_API size_t      aux_flatten_channel_length(const AuxObj& v, int channel_index);
AUXE_API size_t      aux_flatten_channel(const AuxObj& v, int channel_index, auxtype* out, size_t max_len);
AUXE_API int         aux_get_vars(auxContext* ctx, vector<string>& vars);
AUXE_API AuxObj      aux_get_var(auxContext* ctx, const string& varname);
AUXE_API vector<AuxObj> aux_get_cell(auxContext* ctx, const string& varname);
AUXE_API map<string, AuxObj> aux_get_struct(auxContext* ctx, const string& varname);
AUXE_API int        aux_describe_var(auxContext* ctx, const AuxObj& v, uint16_t& typeName, const auxConfig& cfg, string& preview);

AUXE_API int         aux_debug_add_breakpoints(auxContext* ctx, const string& udfpath, const vector<int>& lines);
AUXE_API int         aux_debug_del_breakpoints(auxContext* ctx, const string& udfpath, const vector<int>& lines);
AUXE_API vector<int> aux_debug_view_breakpoints(auxContext* ctx, const string& udfpath, vector<int>& lines);

AUXE_API int         aux_define_udf(auxContext* ctx, const string& udfname, const string& udfpath, string& errstr);

AUXE_API int aux_get_fs(auxContext* ctx);

AUXE_API string aux_get_udfpath(auxContext* ctx);
AUXE_API int aux_remove_udfpath(auxContext* ctx, const string& udfpath);
AUXE_API int aux_add_udfpath(auxContext* ctx, const string& udfpath);

AUXE_API vector<string> aux_enum_vars(auxContext* ctx);
AUXE_API auxDebugAction aux_handle_debug_key(auxContext* ctx, const string& instr);

AUXE_API int aux_register_udf(auxContext* ctx, const string& udfname);
AUXE_API auxDebugAction aux_debug_resume(auxContext** ctx, auxDebugAction act);

// Optional: query where we are paused
AUXE_API int aux_debug_get_pause_info(auxContext* ctx, auxDebugInfo& out);
