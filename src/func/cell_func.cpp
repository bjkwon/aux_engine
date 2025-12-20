#include "functions_common.h"

Cfunction set_builtin_function_cellstruct(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "struct_object", "prop_name"};
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { TYPEBIT_CELL, TYPEBIT_STRUT, TYPEBIT_STRUTS, }; // no more need because of qualify/reject
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2}; // no more need because of qualify/reject
	ft.allowed_arg_types.push_back(allowedTypes2);
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsSTRUT, Cfunction::IsCell };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc);
	set<pfunc_typecheck> allowedCheckFunc1 = { Cfunction::AllTrue }; // Allow all
	ft.qualify.push_back(allowedCheckFunc1);
	set<pfunc_typecheck> prohibitFunc1 = { Cfunction::IsSTRUT, Cfunction::IsCell }; // except for these
	ft.reject.push_back(prohibitFunc1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_structbase(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "struct_object"};
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used, but shouldn't be empty
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsSTRUT, Cfunction::IsCell };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _cellstruct(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "face")) {
		const AstNode* arg0 = arg0node(pnode, past->node);
		CVar* psig = past->GetVariable(arg0->str, arg0);
		if (!psig) {
			throw exception_func(*past, pnode, "Must be a variable", pnode->str, 1).raise();
		}
		psig->set_class_head(args.front());
		past->Sig = args.front();
	}
	else if (!strcmp(pnode->str, "erase") || !strcmp(pnode->str, "ismember")) {
		past->Sig.Reset();
		const AstNode* arg0 = arg0node(pnode, past->node);
		CVar* psig = past->GetVariable(arg0->str, arg0);
		if (!psig) {
			throw exception_func(*past, pnode, "Must be a variable", pnode->str, 1).raise();
		}
		auto it = psig->strut.find(args.front().str());
		if (it != psig->strut.end()) {
			if (!strcmp(pnode->str, "erase"))
				psig->strut.erase(it);
			past->Sig.SetValue(1);
		}
		else {
			past->Sig.SetValue(0);
		}
		past->Sig.MakeLogical();
	}
	else /* ismember */ {
	}
}

void _structbase(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "face")) {
		past->Sig.strut.clear();
		past->Sig.cell.clear();
	}
	else if (!strcmp(pnode->str, "members")) {
		CVar out;
		for (auto v : past->Sig.strut) {
			auto sss = v.first;
			CVar temp(sss);
			out.cell.push_back(temp);
		}
		past->Sig = out;
	}
}