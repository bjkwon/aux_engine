#include "functions_common.h"

int get_output_count(const AstNode* ptree, const AstNode* pnode);  // support.cpp

Cfunction set_builtin_function_test(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "tpoint_sec", "len"};
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 1, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
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

void _test(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auto arg1 = args.front();
	auto arg2 = *(args.begin()+1);
	vector<auxtype> buf1;
	vector<auxtype> buf2;
	int nOutVars = get_output_count(past->node, pnode);
	if (nOutVars > 1) {
		CVar* extraOut = new CVar;
		void* popt = (void*)extraOut;
		auto nomore = past->Sig.fill_short_buffer(arg1.value(), arg2.value(), buf1, buf2);
		past->SigExt.push_back(move(make_unique<CVar>(past->Sig)));
		unique_ptr<CVar> pt1 = make_unique<CVar>(CSignals(&nomore, 1));
		past->SigExt.push_back(move(pt1));
		unique_ptr<CVar> pt2 = make_unique<CVar>(CSignals(CSignal(buf1)));
		past->SigExt.push_back(move(pt2));
		unique_ptr<CVar> pt3 = make_unique<CVar>(CSignals(CSignal(buf2)));
		past->SigExt.push_back(move(pt3));
	}
	else {
		auto nomore = past->Sig.fill_short_buffer(arg1.value(), arg2.value(), buf1, buf2);
		past->Sig = CSignals(&nomore, 1);
	}
}
