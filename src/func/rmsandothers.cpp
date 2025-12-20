#include <numeric>
#include "functions_common.h"

Cfunction set_builtin_function_rmsetc(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "arrray"};
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/ };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

inline static double _getdB(double x)
{
	// 3 dB is added to make rms of full scale sinusoid 0 dB
	return 20 * log10f(x) + 3.0103;
}

CSignal __rms(double *buf, unsigned int len, void* pargin, void* pargout)
{
	CSignal out(*(int*)pargin); // fs
	out.UpdateBuffer(1);
	if (len == 0) out.buf[0] = std::numeric_limits<double>::infinity();
	else
	{
		double val = 0.f;
		for_each(buf, buf + len, [&val](double& v) {val += v * v; });
		out.buf[0] = _getdB(sqrt(val / len));
	}
	return out;
}

void _rmsetc(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "rms")
		past->Sig = past->Sig.evoke_getsig2(__rms, (void*)&past->Sig.fs);
	else if (fname == "begint")
		past->Sig = past->Sig.evoke_getval(&CSignal::begint);
	else if (fname == "endt")
		past->Sig = past->Sig.evoke_getval(&CSignal::endt);
	else if (fname == "dur")
		past->Sig = past->Sig.evoke_getval(&CSignal::dur);
	else if (fname == "rmsall")
		past->Sig = past->Sig.RMS(); // overall RMS from artificially concatenated chain's 
}

