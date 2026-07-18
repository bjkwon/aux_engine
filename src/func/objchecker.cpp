#include "functions_common.h"

Cfunction set_builtin_function_objchecker(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _objchecker(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auto tp = past->Sig.type();
	auto set_logical = [&](bool value) {
		past->Sig.SetValue(value ? 1.0 : 0.0);
		past->Sig.MakeLogical();
	};
	if (!strcmp(pnode->str, "isempty")) {
		set_logical(past->Sig.IsEmpty());
	}
	else if (!strcmp(pnode->str, "isaudio")) {
		set_logical(ISAUDIO(tp));
	}
	else if (!strcmp(pnode->str, "isvector")) {
		set_logical(ISVECTOR(tp));
	}
	else if (!strcmp(pnode->str, "isstring")) {
		set_logical(ISSTRING(tp));
	}
	else if (!strcmp(pnode->str, "isstereo")) {
		set_logical(ISSTEREO(tp));
	}
	else if (!strcmp(pnode->str, "isbool")) {
		set_logical(ISBOOL(tp));
	}
	else if (!strcmp(pnode->str, "iscell")) {
		set_logical(Cfunction::IsCell(tp));
	}
	else if (!strcmp(pnode->str, "isclass")) {
		set_logical(ISSTRUT(tp));
	}
	else if (!strcmp(pnode->str, "istseq")) {
		set_logical(ISTEMPORAL(tp));
	}
}
