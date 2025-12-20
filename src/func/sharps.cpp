#include "functions_common.h"

Cfunction set_builtin_function_constant(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _imaginary_unit(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	complex<float> x(0, 1);
	past->Sig.SetValue(x);
}

void _pi(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig.SetValue(asin(1) * 2);
}

void _natural_log_base(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig.SetValue(exp(1));
}

void _boolconst(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "false"))
		past->Sig.SetValue(0);
	else
		past->Sig.SetValue(1.);
	past->Sig.MakeLogical();
}