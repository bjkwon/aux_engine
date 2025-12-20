#include "functions_common.h"

Cfunction set_builtin_function_onezero(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "length" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 1,};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_diff(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array or signal" };
	vector<string> desc_arg_opt = { "n-th diff" };
	vector<CVar> default_arg = { CVar(1.f) };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES, FFT_RESULTS_TYPES, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_cumsum(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array or signal" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES, FFT_RESULTS_TYPES, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _onezero(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	int len = (int)round(past->Sig.value());
	past->Sig.Reset(1);
	if (len <= 0) return;
	past->Sig.UpdateBuffer(len);	
	if (fname == "ones") {
		for (int k = 0; k < len; k++)
			past->Sig.buf[k] = 1;
	}
	past->Sig.bufType = 'R';
	// no need for "zeros" because the buffer was already initialized with zeros
}

void _diff(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int order = (int)args.front().value();
	if (order < 1) 
		throw exception_func(*past, pnode, "argument must be positive", pnode->str, 1).raise();
	uint64_t q, len = past->Sig.Len();
	for (uint64_t p = 0; p < past->Sig.nGroups; p++)
	{
		q = p * (len - order);
		for (; q < (p + 1) * (len - order); q++)
			past->Sig.buf[q] = past->Sig.buf[q + order + p] - past->Sig.buf[q + p];
	}
	if (past->Sig.nGroups > 1)
		past->Sig.UpdateBuffer(past->Sig.nSamples - (len - 1) * order);
	else
		past->Sig.UpdateBuffer(past->Sig.nSamples - 1);
}

void _cumsum(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	uint64_t len = past->Sig.Len();
	for (uint64_t p = 0; p < past->Sig.nGroups; p++)
	{
		unsigned int q = 1 + p * len;
		for (; q < (p + 1) * len; q++)
			past->Sig.buf[q] = past->Sig.buf[q - 1] + past->Sig.buf[q];
	}
}
