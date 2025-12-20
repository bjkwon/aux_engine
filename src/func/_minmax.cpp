#include "functions_common.h"

int get_output_count(const AstNode* ptree, const AstNode* pnode);  // support.cpp

Cfunction set_builtin_function_minmax(fGate fp)
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
	set<uint16_t> allowedTypes1 = { 2, 3, TYPEBIT_TEMPO_ONE + 2, TYPEBIT_TEMPO_CHAINS + 2, TYPEBIT_TEMPO_CHAINS_SNAP + 2, TYPEBIT_TEMPO_ONE + 3, TYPEBIT_TEMPO_CHAINS + 3, TYPEBIT_TEMPO_CHAINS_SNAP + 3};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

CSignal __max_aux2(auxtype *buf, unsigned int len, void* pargin, void* pargout)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min 
	auto maxy = max_element(buf, buf + len);
	CSignal out(*maxy);
	out.SetFs(*(int*)pargin);
	if (pargout)
	{
		int ind = (int)(maxy - buf);
		((CTimeSeries*)pargout)->SetValue((auxtype)ind + 1); // one-based index
		((CTimeSeries*)pargout)->SetFs(*(int*)pargin);
	}
	return out;
}

CSignal __min_aux2(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min 
	auto miny = min_element(buf, buf + len);
	CTimeSeries out(*miny);
	out.SetFs(*(int*)pargin);
	if (pargout)
	{
		int ind = (int)(miny - buf);
		((CTimeSeries*)pargout)->SetValue((auxtype)ind + 1); // one-based index
		((CTimeSeries*)pargout)->SetFs(*(int*)pargin);
	}
	return out;
}

CSignal __max_aux3(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min
	double max_value = -std::numeric_limits<double>::infinity();
	int max_index = -1;
	for (unsigned int k = 0; k < len; ++k) {
		if (std::isfinite(buf[k]) && buf[k] > max_value) {
			max_value = buf[k];
			max_index = k;
		}
	}
	CSignal out(max_value);
	out.SetFs(*(int*)pargin);
	if (pargout)
	{
		((CTimeSeries*)pargout)->SetValue((auxtype)max_index + 1); // one-based index
		((CTimeSeries*)pargout)->SetFs(*(int*)pargin);
	}
	return out;
}

CSignal __min_aux3(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min 
	double min_value = std::numeric_limits<double>::infinity();
	int min_index = -1;
	for (unsigned int k = 0; k < len; ++k) {
		if (std::isfinite(buf[k]) && buf[k] < min_value) {
			min_value = buf[k];
			min_index = k;
		}
	}
	CSignal out(min_value);
	out.SetFs(*(int*)pargin);
	if (pargout)
	{
		((CTimeSeries*)pargout)->SetValue((auxtype)min_index + 1); // one-based index
		((CTimeSeries*)pargout)->SetFs(*(int*)pargin);
	}
	return out;
}

/* delete ?? */
CTimeSeries __max_aux(const CTimeSeries& base, void* parg)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min 
	auto maxy = max_element(base.buf, base.buf + base.nSamples);
	CTimeSeries out(*maxy);
	if (parg)
	{
		int ind = (int)(maxy - base.buf);
		((CTimeSeries*)parg)->SetValue((auxtype)ind+1); // one-based index
	}
	return out;
}

/* delete ?? */
CTimeSeries __min_aux(const CTimeSeries& base, void* parg)
{ // assume: parg is a pointer to a CTimeSeries obj; used to store the index of max/min 
	auto miny = min_element(base.buf, base.buf + base.nSamples);
	CTimeSeries out(*miny);
	if (parg)
	{
		int ind = (int)(miny - base.buf);
		((CTimeSeries*)parg)->SetValue((auxtype)ind+1); // one-based index
	}
	return out;
}

void _minmax(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int nOutVars = get_output_count(past->node, pnode);
	string fname = pnode->str;
	void* popt = NULL;
	auto fp = __max_aux2;
	if (fname == "min")
		fp = __min_aux2;
	else if (fname == "_max")
		fp = __max_aux3;
	else if (fname == "_min")
		fp = __min_aux3;
	if (nOutVars > 1)
	{
		CVar extraOut;
		popt = (void*)&extraOut;
		past->Sig = past->Sig.evoke_getsig2(fp, (void*)&past->Sig.fs, popt);
		past->SigExt.push_back(move(make_unique<CVar>(past->Sig)));
		unique_ptr<CVar> pt = make_unique<CVar>(extraOut); // popt carries maximum/minimum indices
		past->SigExt.push_back(move(pt));
	}
	else
	{
		past->Sig = past->Sig.evoke_getsig2(fp, (void*)&past->Sig.fs, popt);
	}
}

