#include "functions_common.h"
#include <iostream>

Cfunction set_builtin_function_error_warning(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "message", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _error_warning(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "error"))
		throw exception_etc(*past, pnode, past->Sig.str()).raise();
	else {
		cout << "WARNING: " << past->Sig.str() << endl;
	}
}