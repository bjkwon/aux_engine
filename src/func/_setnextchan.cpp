#include "functions_common.h"

Cfunction set_builtin_function_setnextchan(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "Mono signal", "Mono signal to make the next chan", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsTEMPORALG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::IsSTEREOG, }; // mono only.. no stereo
	ft.reject.push_back(prohibitFunc);
	ft.qualify.push_back(allowedCheckFunc);
	ft.reject.push_back(prohibitFunc);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _setnextchan(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
    // setnextchan is used only when a right channel is added to a mono (and the original signal becomes left)
	// Modifying the first argument object
	CVar* second = new CVar;
	past->Sig.SetNextChan(args.front());
	const AstNode* arg0 = arg0node(pnode, past->node);
	CVar* psig = past->GetVariable(arg0->str, arg0);
	if (!psig) {
		throw exception_func(*past, pnode, "Must be a variable", pnode->str, 1).raise();
	}
	*psig = past->Sig;

}

