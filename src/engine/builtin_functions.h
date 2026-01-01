#include "AuxScope.h"

/* How to add new functions to the list of built in functions

typedef void(*fGate) (AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

Case 1) One gate function to one built-in, Cfunction decl (exclusive)
Given gate function decl:
   void _tone(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
and Cfunction decl:
   Cfunction set_builtin_function_tone(fGate fp); --> arg number/types of tone() are registered here.

In EngineRuntime::InitBuiltInFunctions(),
	builtin.emplace("tone", set_builtin_function_tone(&_tone)); 
simplified to -->
	SET_BUILTIN_FUNC("tone", tone)
    (1st arg: aux func name in quotation;  2nd arg: gate function in the source code, no quotation)

    DECL_GATE(_tone) in builtin_functions.h

Case 2) Multiple gate functions to multiple built-in's, shared Cfunction decl
Multiple gate functions:
    void _imaginary_unit(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
    void _pi(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
    void _natural_log_base(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
One Cfunction decl:
    Cfunction set_builtin_function_constant(fGate fp);

In EngineRuntime::InitBuiltInFunctions(),
	pseudo_vars.emplace("i", set_builtin_function_constant(&_imaginary_unit));
	pseudo_vars.emplace("pi", set_builtin_function_constant(&_pi));
	pseudo_vars.emplace("e", set_builtin_function_constant(&_natural_log_base));

	DECL_GATE(_constant) in builtin_functions.h

Case 3) One gate function to multiple built-in's, shared Cfunction decl
Gate function:
    void _tparamonly(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
One Cfunction decl:
	Cfunction set_builtin_function_tparamonly(fGate fp)
In EngineRuntime::InitBuiltInFunctions(),
	builtin.emplace("noise", set_builtin_function_tone(&_tparamonly));
	builtin.emplace("gnoise", set_builtin_function_tone(&_tparamonly));
Simplify -->
	SET_BUILTIN_FUNC("noise", tparamonly);
	SET_BUILTIN_FUNC("gnoise", tparamonly);

    DECL_GATE(_tparamonly) in builtin_functions.h

Case 4) One AUX build-in function, multiple calling prototype (i.e., multiple gate functions)
If an aux built-in function offers multiple prototypes---
Gate functions:
void _andor(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
void _andor2(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
Cfunction decls:
Cfunction set_builtin_function_andor(fGate fp)
Cfunction set_builtin_function_andor2(fGate fp)
In EngineRuntime::InitBuiltInFunctions(),
	builtin.emplace("and", set_builtin_function_tone(&_andor));
	builtin.emplace("and", set_builtin_function_tone(&_andor2));
Simplify -->
	SET_BUILTIN_FUNC("and", andor);
	SET_BUILTIN_FUNC("and", andor2);
Add to builtin_functions.h
	DECL_GATE(_andor)
	DECL_GATE(_andor2) 

	9/27/2022
*/

/* types
* 0 NULL
* 1 scalar
* 2 vector
* TYPEBIT_AUDIO + 2 audio
* TYPEBIT_SIZE1 + TYPEBIT_FS2 string
*/

//these functions are defined in _file.cpp
void _fopen(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _fclose(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _fprintf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _fread(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _fwrite(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _write(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_wavwrite)
DECL_GATE(_wave)
void _file(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

map<string, Cfunction> dummy_pseudo_vars;
map<string, Cfunction> EngineRuntime::pseudo_vars = dummy_pseudo_vars;

//#ifdef NO_PLAYSND // for aux_builtin_ext
//AuxScope::play_block_ms = 0;
//AuxScope::record_block_ms = 0;
//AuxScope::record_bytes = 0;
//#endif


DECL_GATE(_run)

DECL_GATE(_movespec)

DECL_GATE(_tparamonly)
DECL_GATE(_rand)
DECL_GATE(_irand)
DECL_GATE(_randperm)
DECL_GATE(_andor)
DECL_GATE(_andor2)
DECL_GATE(_mostleast)
DECL_GATE(_sort)
void _sprintf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _inputdlg(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void aux_input(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_error(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_warning(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void udf_rethrow(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _msgbox(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _include(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_eval)
DECL_GATE(_include)
DECL_GATE(_group)
DECL_GATE(_ungroup)
DECL_GATE(_clear)
DECL_GATE(_dir)
DECL_GATE(_veq)
DECL_GATE(_datatype)
DECL_GATE(_noargs)
void _interp1(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _fdelete(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _isaudioat(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_play)
DECL_GATE(_play2)
DECL_GATE(_stop_pause_resume)
DECL_GATE(_fft)
DECL_GATE(_ifft)
DECL_GATE(_tone)
DECL_GATE(_diff)
DECL_GATE(_cumsum)
DECL_GATE(_cellstruct)
DECL_GATE(_structbase)
DECL_GATE(_tseqget)
DECL_GATE(_tseqset)
DECL_GATE(_error_warning)
DECL_GATE(_test)
DECL_GATE(_str2num)
DECL_GATE(_rend)

void _fm(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _str2num(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _esc(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _varcheck(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
DECL_GATE(_ramp)
void _std(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _size(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _mostleast(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _setnextchan(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _setfs(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _conv(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _imaginary_unit(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _pi(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _natural_log_base(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _boolconst(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

DECL_GATE(_pow)
DECL_GATE(_mod)
DECL_GATE(_minmax)
DECL_GATE(_sums)
DECL_GATE(_lens)
DECL_GATE(_rmsetc)
DECL_GATE(_onezero)
DECL_GATE(_sam)
DECL_GATE(_blackman)
DECL_GATE(_hamming)
DECL_GATE(_filt)
DECL_GATE(_iir)
DECL_GATE(_conv)
DECL_GATE(_audio)
DECL_GATE(_vector)
DECL_GATE(_leftright)
DECL_GATE(_hilbenvlope)
DECL_GATE(_printf)
DECL_GATE(_fprintf)
DECL_GATE(_fopen)
DECL_GATE(_fclose)
DECL_GATE(_fwrite)
DECL_GATE(_fread)
DECL_GATE(_write)
DECL_GATE(_file)
DECL_GATE(_json)
DECL_GATE(_objchecker)
DECL_GATE(_setnextchan)

DECL_GATE(_resample)

DECL_GATE(_constant)

void _sqrt(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);
void _sin(AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

DECL_GATE(_pitchtime)