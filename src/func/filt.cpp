#include "functions_common.h"
//from ellf.c
extern "C" int design_iir(double* num, double* den, int fs, int kind, int type, int n, double* freqs, double dbr /*rippledB*/, double dbd /*stopEdgeFreqORattenDB*/);

Cfunction set_builtin_function_filt(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "numerator_array" };
	vector<string> desc_arg_opt = { "denominator_array", "initial_condition_array"};
	vector<CVar> default_arg = { CVar(1.f), CVar() };
	set<uint16_t> allowedTypes1 = { 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2 };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_conv(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "signal1", "signal2" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_iir(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "cut_off_freq" };
	vector<string> desc_arg_opt = { "order[=8]", "type(Butterword[=1], Chebyshev=2, Elliptic=3)", "passRipple_dB=0.5", "AttenDB=-40" };
	vector<CVar> default_arg = { CVar(8.f), CVar(1.f), CVar(.5f), CVar(-40.f), };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsAUDIOG, Cfunction::IsVectorG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc);
	set<pfunc_typecheck> allowedCheckFunc1 = { Cfunction::IsScalarG, Cfunction::IsVectorG, };
	set<pfunc_typecheck> allowedCheckFunc2 = { Cfunction::IsScalarG, };
	ft.qualify.push_back(allowedCheckFunc1);
	ft.reject.push_back(prohibitFunc); // arg 1 
	ft.qualify.push_back(allowedCheckFunc2);
	ft.reject.push_back(prohibitFunc); // arg 2 
	ft.qualify.push_back(allowedCheckFunc2);
	ft.reject.push_back(prohibitFunc);  // arg 3
	ft.qualify.push_back(allowedCheckFunc2);
	ft.reject.push_back(prohibitFunc);  // arg 4
	ft.qualify.push_back(allowedCheckFunc2);
	ft.reject.push_back(prohibitFunc);  // arg 5
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}


int get_output_count(const AstNode* ptree, const AstNode* pnode);  // support.cpp

static void filterbase(auxtype* buf, size_t len, const vector<auxtype>& num, const vector<auxtype>& den, vector<auxtype>& initial, vector<auxtype>& state)
{
	if (initial.size() < max(num.size(), den.size()) - 1)
	{
		for (auto k = initial.size(); k < max(num.size(), den.size()) - 1; k++)
			initial.push_back(0.);
	}
	state = initial;
	//if (IsComplex())
	//{
	//	complex<auxtype>* out = new complex<auxtype>[len];
	//	for (unsigned int m = 0; m < len; m++)
	//	{
	//		out[m] = num[0] * cbuf[m] + state.front();
	//		//DO THIS--------------------

	//		//size of state is always one less than num or den
	//		//int k = 1;
	//		//for (auto& v : state)
	//		//{
	//		//	v = num[k] * cbuf[m] - den[k] * out[m];
	//		//	if (k < initial.size())
	//		//		v += *((&v) + 1);
	//		//	k++;
	//		//}
	//	}
	//	delete[] cbuf;
	//	cbuf = out;
	//}
	//else
	{
		auxtype* out = new auxtype[len];
		for (unsigned int m = 0; m < len; m++)
		{
			out[m] = num[0] * buf[m] + state.front();
			//size of state is always one less than num or den
			int k = 1;
			for (auto& v : state)
			{
				// den and num may have different size
				if (k < num.size())
				{
					v = num[k] * buf[m];
					if (k < den.size()) v += -den[k] * out[m];
				}
				else
					v = -den[k] * out[m]; // should be k < den.size()
				if (k < state.size())
					v += *((&v) + 1);
				k++;
			}
		}
		memcpy(buf, out, sizeof(auxtype) * len);
		delete[] out;
	}
}

CSignal __filt(const CSignal& base, void* parg1, void* parg2)
{
	vector<CVar> argin = *(vector<CVar>*)parg1;
	vector<double> num(argin[0].buf, argin[0].buf + argin[0].nSamples);
	vector<double> den(argin[1].buf, argin[1].buf + argin[1].nSamples);
	vector<double> init(argin[2].buf, argin[2].buf + argin[2].nSamples);
	vector<double> fin;
	filterbase(base.buf, base.nSamples, num, den, init, fin);
	// parg2 was declared as CVar in _filt
	CVar* cvarfin = (CVar*)parg2;
	cvarfin->UpdateBuffer(fin.size());
	memcpy(cvarfin->buf, fin.data(), sizeof(double) * fin.size());
	// final state after filtering is not only copied to parg2, but also used to update parg1
	// updating init won't do anything (init is a local variable)
	// instead update the content of the pointer used to initialize init
	// So that final state at this group can be used as an initial state in the successive call inside of CSignal::evoke_modsig2
	// Even if it's not necessary because nGroup=1, updating the content of this pointer won't do any harm. 01/20/2022
	if (argin[2].nSamples == 0)
		(*(((vector<CVar>*)parg1)->begin() + 2)).UpdateBuffer(fin.size());
	memcpy((*(((vector<CVar>*)parg1)->begin() + 2)).buf, fin.data(), sizeof(double) * fin.size());
	return base;
}

void _filt(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (args[2].nSamples >= max(args[0].nSamples, args[1].nSamples))
		throw exception_func(*past, pnode, "the size of state array must be less than the coefficient array").raise();
	if (fname == "filt")
	{
		CVar* extraOut = new CVar; // to carry a state array
		past->Sig = past->Sig.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
		if (get_output_count(past->node, pnode) > 1)
		{
			past->SigExt.push_back(move(make_unique<CVar>(past->Sig)));
			unique_ptr<CVar> pt = make_unique<CVar>(*extraOut);
			past->SigExt.push_back(move(pt));
		}
		else
			delete extraOut;
	}
	else // filtfilt
	{
			CSignal temp(past->Sig.GetFs()), out(past->Sig.GetFs());
			size_t nfact = (size_t)(3 * (max(args[0].nSamples, args[1].nSamples) - 1));
			temp.UpdateBuffer(nfact + past->Sig.nSamples);
			memset(temp.buf, 0, sizeof(auxtype) * nfact);
			memcpy(temp.buf + nfact, past->Sig.buf, sizeof(auxtype) * past->Sig.nSamples);
			CSignal temp2(temp);
			temp += &temp2;
			CVar* extraOut = new CVar; // to carry a state array
			temp.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
			temp.ReverseTime();
			extraOut->ReverseTime();
			((vector<CVar>*) & args)->back().UpdateBuffer(extraOut->nSamples);
			memcpy(((vector<CVar>*) & args)->back().buf, extraOut->buf, sizeof(auxtype) * extraOut->nSamples);
			temp.evoke_modsig2(__filt, (void*)&args, (void*)extraOut);
			temp.ReverseTime();
			extraOut->ReverseTime();
			memcpy(past->Sig.buf, temp.buf + nfact, sizeof(auxtype) * past->Sig.nSamples);
			if (get_output_count(past->node, pnode) > 1)
			{
				past->SigExt.push_back(move(make_unique<CVar>(past->Sig)));
				unique_ptr<CVar> pt = make_unique<CVar>(*extraOut);
				past->SigExt.push_back(move(pt));
			}
			else
				delete extraOut;
	}
}

CSignal __conv(const CSignal& base, void* parg1, void* parg2)
{
	vector<CVar> argin = *(vector<CVar>*)parg1;
	CSignal out(base.GetFs());
	out.UpdateBuffer(base.nSamples + argin.front().nSamples - 1);
	for (unsigned int k = 0; k < out.nSamples; k++)
	{
		auxtype tp = 0.f;
		for (int p(0), q(0); p < base.nSamples; p++)
		{
			if ((q = k - p) < 0) continue;
			if (p < base.nSamples && q < argin.front().nSamples)
				tp += base.buf[p] * argin.front().buf[q];
		}
		out.buf[k] = tp;
	}
	return out;
}

void _conv(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig = past->Sig.evoke_modsig2(__conv, (void*)&args, NULL);
}

void _iir(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	int type(0), kind(1), norder(4);
	double freqs[2], rippledB(0.5), stopbandFreqORAttenDB(-40.);
	norder = args.at(1).value();
	kind = args.at(2).value();
	rippledB = args.at(3).value();
	stopbandFreqORAttenDB = args.at(4).value();
	if (fname == "lpf") type = 1;
	else if (fname == "bpf") type = 2;
	else if (fname == "hpf") type = 3;
	else if (fname == "bsf") type = 4;

	vector<double> den(norder + 1, 0);
	vector<double> num(norder + 1, 0);
	freqs[0] = args.at(0).buf[0];
	if (type == 2 || type == 4) {
		if (freqs[1] = args.at(0).nSamples !=2) 
			throw exception_func(*past, pnode, "Two-element array needed for cut-off frequencies.", fname).raise();
		freqs[1] = args.at(0).buf[1];
		den.resize(2 * norder + 1);
		num.resize(2 * norder + 1);
	}
	else {
		if (freqs[1] = args.at(0).nSamples != 1)
			throw exception_func(*past, pnode, "A scalar needed for cut-off frequencies.", fname).raise();
	}
	int res = design_iir(num.data(), den.data(), past->Sig.GetFs(), kind, type, norder, freqs, (double)rippledB, (double)stopbandFreqORAttenDB);
	if (res <= 0) {
		// handle error
	}
	else {
		vector<auxtype> coeff(norder + 1, 0);
		auto it = num.begin();
		for (auto& v : coeff) {
			v = *it;
			it++;
		}
		CVar tp1 = CSignals(coeff.data(), norder + 1);
		vector<CVar> argsnew(1, tp1);
		auto it2 = den.begin();
		for (auto& v : coeff) {
			v = *it2;
			it2++;
		}
		CVar tp2 = CSignals(coeff.data(), norder + 1);
		argsnew.push_back(tp2);
		argsnew.push_back(CVar()); // init condition not used (a Null object) from the user. Internally init condition is used properly to generate results for nGroups>1)

		CVar* extraOut = new CVar; // to carry a state array
		past->Sig = past->Sig.evoke_modsig2(__filt, (void*)&argsnew, (void*)extraOut);
		if (get_output_count(past->node, pnode) > 1)
		{
			past->SigExt.push_back(move(make_unique<CVar>(past->Sig)));
			unique_ptr<CVar> pt = make_unique<CVar>(*extraOut);
			past->SigExt.push_back(move(pt));
		}
		else
			delete extraOut;
	}
}