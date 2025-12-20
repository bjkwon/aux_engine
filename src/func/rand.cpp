#include "functions_common.h"
#include <time.h>

Cfunction set_builtin_function_rand(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "output_size" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {};
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

Cfunction set_builtin_function_randperm(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "range_from_1_to_" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {};
	set<uint16_t> allowedTypes1 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_irand(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "range_from_1_to_" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {};
	set<uint16_t> allowedTypes1 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
// cell is piggy-bagging on _rand 10/1/2022
void _rand(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float val = past->Sig.value();
	if (val < 0)
		throw exception_func(*past, pnode, "argument must be positive", pnode->str, 1).raise();
	int ival = (int)round(val);
	if (!strcmp(pnode->str, "rand")) {

		static bool initialized(false);
		if (!initialized)
		{
			srand((unsigned)time(0));
			initialized = true;
		}
		past->Sig.UpdateBuffer(ival);
		for (int k = 0; k < ival; k++)
			past->Sig.buf[k] = (float)rand() / RAND_MAX;
		past->Sig.bufType = 'R';
	}
	else // cell 
	{
		past->Sig.Reset();
		CVar tp;
		for (int k = 0; k < ival; k++)
			past->Sig.appendcell(tp);
	}
}

void _irand(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	float val = past->Sig.value();
	if (val < 0)
		throw exception_func(*past, pnode, "argument must be positive", pnode->str, 1).raise();
	static bool initialized(false);
	if (!initialized)
	{
		srand((unsigned)time(0));
		initialized = true;
	}
	past->Sig.UpdateBuffer(1);
	past->Sig.SetValue((float)ceil((float)rand() / (float)RAND_MAX * val));
	past->Sig.bufType = 'R';
}

void _randperm(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int ival = (int)round(past->Sig.value());
	if (ival < 1)
		throw exception_func(*past, pnode, "argument must be positive", pnode->str, 1).raise();
	static bool initialized(false);
	if (!initialized)
	{
		srand((unsigned)time(0));
		initialized = true;
	}
	past->Sig.Reset(1);
	past->Sig.UpdateBuffer((size_t)ival);
	int m, n;
	float hold;
	for (int i = 0; i < ival; i++)past->Sig.buf[i] = (float)(i + 1);
	int repeat = (int)sqrt(ival * 100.); // swapping sqrt(ival*100.) times
	for (int i = 0; i < repeat; i++)
	{
		m = (int)((float)rand() / (float)RAND_MAX * ival);
		do { n = (int)((float)rand() / (float)RAND_MAX * ival); } while (m == n);
		hold = past->Sig.buf[m];
		past->Sig.buf[m] = past->Sig.buf[n];
		past->Sig.buf[n] = hold;
	}
	past->Sig.bufType = 'R';
}

