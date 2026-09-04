#include "functions_common.h"

Cfunction set_builtin_function_import(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = true;
	ft.desc_arg_req = { "module_name" };
	ft.desc_arg_opt = { "alias" };
	ft.narg1 = 1;
	ft.narg2 = 2;
	ft.allowed_arg_types.push_back({ TYPEBIT_STRING + 1, TYPEBIT_STRING + 2 });
	ft.allowed_arg_types.push_back({ TYPEBIT_STRING + 1, TYPEBIT_STRING + 2 });
	CVar default_alias;
	default_alias.SetString("");
	ft.defaultarg = { default_alias };
	return ft;
}

void _import(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv)
		throw exception_etc(*past, pnode, "AUX engine is not initialized.").raise();
	if (!ISSTRING(past->Sig.type()))
		throw exception_func(*past, pnode, "type " + std::to_string(past->Sig.type()), pnode->str, 1).raise();

	const string module_name = past->Sig.str();
	string alias;
	if (!args.empty()) {
		if (!ISSTRING(args.front().type()))
			throw exception_func(*past, pnode, "type " + std::to_string(args.front().type()), pnode->str, 2).raise();
		alias = args.front().str();
	}
	string err;
	if (past->pEnv->ImportNativeModule(module_name, alias, past, err) != 0) {
		if (err.empty())
			err = "Failed to import module: " + module_name;
		throw exception_etc(*past, pnode, err).raise();
	}
	past->Sig.SetString((alias.empty() ? module_name : alias).c_str());
}
