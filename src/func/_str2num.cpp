#include "functions_common.h"
#include "psycon.tab.h"

Cfunction set_builtin_function_str2num(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "string" };
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/ };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

bool isAllNodeT_NUM(const AstNode* p)
{
	if (p->type == T_NUMBER)
	{
		if (p->next)
			return isAllNodeT_NUM(p->next);
		return true;
	}
	return false;
}
void _str2num(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string instr = string("[") + past->Sig.str() + "]";
	AuxScope tast(past->pEnv);
	string emsg;
	auto nodes = tast.makenodes(instr);
	tast.node = nodes;
	vector<CVar*> res = tast.Compute();
	past->Sig = res.back();
}

//void _esc(CAstSig* past, const AstNode* pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	if (!past->Sig.IsString())
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc("argument must be a text string.");
//	char estr[256];
//	string str = past->Sig.string();
//	char* instr = new char[str.size()+1];
//	memcpy(instr, str.c_str(), str.size() + 1);
//	process_esc_chars(instr, str.size(), estr);
//	past->Sig.SetString(instr);
//	delete[] instr;
//}
