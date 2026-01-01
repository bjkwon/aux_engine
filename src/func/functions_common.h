#include "AuxScope.h"
#include "AuxScope_exception.h"
#include "typecheck.h"

// NULL                                   0
// Scalar                                 1
// Scalar bool                            TYPEBIT_SIZE1+1
// Scalar complex                         TYPEBIT_COMPLEX+1
// Vector                                 2
// Vector bool                            TYPEBIT_SIZE1+2
// Vector complex                         TYPEBIT_COMPLEX+2
// Byte                                   TYPEBIT_BYTE+2
// Matrix                                 3
// Matrix bool                            TYPEBIT_SIZE1+3
// Matrix complex                         TYPEBIT_COMPLEX+3
// Temporal Null                          TYPEBIT_TEMPO_ONE
// Audio chainless                        TYPEBIT_TEMPO_ONE+2
// Audio chainless 2D                     TYPEBIT_TEMPO_ONE+3
// Tseq null (no general use)             TYPEBIT_TEMPO_CHAINS
// Tseq (chained by def)                  TYPEBIT_TEMPO_CHAINS+1
// Audio chained                          TYPEBIT_TEMPO_CHAINS+2
// Tshot                                  TYPEBIT_TEMPO_CHAINS_SNAP+2
// Tshot 2D                               TYPEBIT_TEMPO_CHAINS_SNAP+3
// String null (no general use)           TYPEBIT_STRING
// String empty                           TYPEBIT_STRING+1
// String                                 TYPEBIT_STRING+2
// String temporal (no general use)       add TYPEBIT_TEMPO_CHAINS_SNAP to the generic string type value
// Stereo audio chainless                 TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE+2
// Stereo audio chained                   TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS+2
// Stereo audio chained  2D               TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS+3
// Stereo tseq                            TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS+1
// Stereo Tshot                           TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS_SNAP+2
// Stereo Tshot 2D                        TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS_SNAP+3
//
// Don't forget to add TYPEBIT_COMPLEX for complex: FFT output should be 
//     TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP+3 or
//     TYPEBIT_MULTICHANS + TYPEBIT_COMPLEX + TYPEBIT_TEMPO_CHAINS_SNAP+3 for FFT of stereo audio
//
// For "general" type, i.e., type of surface obj of a abstract object (such as cell, struct or struts)
// add the corresponding type bit.

typedef void(*fGate) (AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

static
Cfunction set_builtin_function_XXX(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "arg1", "arg2" };
	vector<string> desc_arg_opt = { "optional_arg1", "optional_arg2" };
	vector<CVar> default_arg = { /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/ };
	set<uint16_t> allowedTypes1 = { TYPEBIT_TEMPO_ONE + 2, TYPEBIT_TEMPO_CHAINS + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 50, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

/* These functions would be useful in gate functions
* find_parentnode_alt:
*	returns the parent node of a dot-call; or pnode itself if it's not a dot-call
* arg0node: 
*	returns the principal argument node (i.e., the very first arg); regardless of dot-call or not.
*/

const AstNode* find_parentnode_alt(const AstNode* pnode, const AstNode* pRoot0);
const AstNode* arg0node(const AstNode* pnode, const AstNode* pRoot0);
