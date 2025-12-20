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
	if (!strcmp(pnode->str, "isempty")) {
		past->Sig.SetValue(past->Sig.IsEmpty());
	}
	else if (!strcmp(pnode->str, "isaudio")) {
		past->Sig.SetValue(ISAUDIO(tp));
	}
	else if (!strcmp(pnode->str, "isvector")) {
		past->Sig.SetValue(ISVECTOR(tp));
	}
	else if (!strcmp(pnode->str, "isstring")) {
		past->Sig.SetValue(ISSTRING(tp));
	}
	else if (!strcmp(pnode->str, "isstereo")) {
		past->Sig.SetValue(ISSTEREO(tp));
	}
	else if (!strcmp(pnode->str, "isbool")) {
		past->Sig.SetValue(ISBOOL(tp));
	}
	else if (!strcmp(pnode->str, "iscell")) {
		past->Sig.SetValue(Cfunction::IsCell(tp));
	}
	else if (!strcmp(pnode->str, "isclass")) {
		past->Sig.SetValue(ISSTRUT(tp));
	}
	else if (!strcmp(pnode->str, "istseq")) {
		past->Sig.SetValue(ISTEMPORAL(tp));
	}
}

