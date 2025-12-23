// AUXLAB
//
// Copyright (c) 2009-2022 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent
//
// Version: 1.7
// Date: 12/22/2022
//
#include <sstream>
#include <stdbool.h>

#include "aux_classes.h"
#include "AuxScope.h"
#include "AuxScope_exception.h"

// 10/1/2020 TO DO-----separate these graffy function somehow to add its own features
// such as blockNULL etc.

#include "builtin_functions.h"

bool EngineRuntime::IsValidBuiltin(const string& funcname)
{
	if (pseudo_vars.find(funcname) != pseudo_vars.end())
		return false;
	return builtin.find(funcname) != builtin.end();
}

void _contig(AuxScope *past, const AstNode *pnode)
{
	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
	past->Sig.MakeChainless();
}

Cfunction set_builtin_function_colon(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "(scalar:scalar) or (scalar:scalar:scalar)", "(scalar:scalar) or (scalar:scalar:scalar)" };
	vector<string> desc_arg_opt = { "(scalar:scalar) or (scalar:scalar:scalar)" };
	vector<CVar> default_arg = { CVar(string("doesn't matter")), /*CVar(default_value), CVar(string("default_str")), same number of desc_arg_opt*/};
	set<uint16_t> allowedTypes1 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _colon(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auxtype val1 = past->Sig.value();
	auxtype val2 = args[0].value();
	auxtype step;
	if (ISSCALAR(args.back().type()))
		step = args[1].value();
	else
		step = (val1 > val2) ? -1.f : 1.f;
	int nItems = max(1, (int)((val2 - val1) / step) + 1);
	past->Sig.UpdateBuffer(nItems);
	for (int k = 0; k < nItems; k++)
		past->Sig.buf[k] = val1 + step * k;
}

//vector<string> EngineRuntime::InitBuiltInFunctionsExt(const vector<string>& externalModules)
//{
//	// If there's an error (including file not found), it's reported back to the caller.
//	vector<string> out;
//	char ext[256];
//	for (auto libname : externalModules)
//	{
//		string fname = libname;
//		_splitpath(libname.c_str(), NULL, NULL, NULL, ext);
//		if (!strcmp(ext,""))
//			fname += ".dll";
//		HANDLE hLib = LoadLibrary(fname.c_str());
//		if (hLib)
//		{
//			auto pt = (map<string, Cfunction>(_cdecl*)()) GetProcAddress((HMODULE)hLib, (LPCSTR)MAKELONG(1, 0)); // Init()
//			if (pt)
//			{
//				map<string, Cfunction> res = pt();
//				for (auto it = res.begin(); it != res.end(); it++)
//				{
//					builtin[(*it).first] = (*it).second;
//				}
//			}
//			else
//				out.push_back(fname + string(" does not have Init()."));
//		}
//		else
//			out.push_back(fname + string(" not found."));
//	}
//	return out;
//}

#define SET_BUILTIN_FUNC(AUXNAME,GATENAME) builtin.emplace(AUXNAME, set_builtin_function_##GATENAME(&_##GATENAME));
#define SET_PSEUDO_VARS(AUXNAME,GATENAME) pseudo_vars.emplace(AUXNAME, set_builtin_function_constant(&_##GATENAME));

void EngineRuntime::InitBuiltInFunctions()
{
	//Think about this 11/19/2021
//	srand((unsigned)time(0) ^ (unsigned int)GetCurrentThreadId());

	string name;
	Cfunction ft;
	set<uint16_t> allowedTypes;
	vector<string> arg_desc;

	SET_BUILTIN_FUNC("tone", tone);
	SET_BUILTIN_FUNC("noise", tparamonly);
	SET_BUILTIN_FUNC("gnoise", tparamonly);
	SET_BUILTIN_FUNC("dc", tparamonly);
	SET_BUILTIN_FUNC("silence", tparamonly);
	SET_BUILTIN_FUNC("ramp", ramp);
	SET_BUILTIN_FUNC("sam", sam);
	SET_BUILTIN_FUNC("pow", pow);
	SET_BUILTIN_FUNC("^", pow);
	SET_BUILTIN_FUNC("mod", mod);
	SET_BUILTIN_FUNC("%", mod);
	SET_BUILTIN_FUNC("group", group);
	SET_BUILTIN_FUNC("ungroup", ungroup);
	SET_BUILTIN_FUNC("matrix", ungroup);
	SET_BUILTIN_FUNC("fft", fft);
	SET_BUILTIN_FUNC("ifft", ifft);
	SET_BUILTIN_FUNC(":", colon);
	SET_BUILTIN_FUNC("min", minmax);
	SET_BUILTIN_FUNC("max", minmax);
	SET_BUILTIN_FUNC("sum", sums);
	SET_BUILTIN_FUNC("mean", sums);
	SET_BUILTIN_FUNC("stdev", sums); // to do---support unnormalized stdev (break out of sums and make one for stdev)
	SET_BUILTIN_FUNC("_max", minmax); // excluding inf, nan
	SET_BUILTIN_FUNC("_min", minmax); // excluding inf, nan
	SET_BUILTIN_FUNC("_sum", sums); // excluding inf, nan
	SET_BUILTIN_FUNC("_mean", sums); // excluding inf, nan
	SET_BUILTIN_FUNC("_stdev", sums); // excluding inf, nan, to do---support unnormalized stdev (break out of sums and make one for stdev)
	SET_BUILTIN_FUNC("length", lens);
	SET_BUILTIN_FUNC("size", lens);
	SET_BUILTIN_FUNC("rmsall", rmsetc);
	SET_BUILTIN_FUNC("rms", rmsetc);
	SET_BUILTIN_FUNC("begint", rmsetc);
	SET_BUILTIN_FUNC("endt", rmsetc);
	SET_BUILTIN_FUNC("dur", rmsetc);
	SET_BUILTIN_FUNC("ones", onezero);
	SET_BUILTIN_FUNC("zeros", onezero);
	SET_BUILTIN_FUNC("and", andor);
	SET_BUILTIN_FUNC("and", andor2);
	SET_BUILTIN_FUNC("or", andor);
	SET_BUILTIN_FUNC("or", andor2);
	SET_BUILTIN_FUNC("sort", sort);
	SET_BUILTIN_FUNC("atmost", mostleast);
	SET_BUILTIN_FUNC("atleast", mostleast);
	SET_BUILTIN_FUNC("hamming", hamming);
	SET_BUILTIN_FUNC("blackman", blackman);
	SET_BUILTIN_FUNC("hann", blackman);
	SET_BUILTIN_FUNC("filt", filt);
	SET_BUILTIN_FUNC("filtfilt", filt);
	SET_BUILTIN_FUNC("conv", conv);
	SET_BUILTIN_FUNC("lpf", iir);
	SET_BUILTIN_FUNC("hpf", iir);
	SET_BUILTIN_FUNC("bpf", iir);
	SET_BUILTIN_FUNC("bps", iir);
	SET_BUILTIN_FUNC("audio", audio);
	SET_BUILTIN_FUNC("vector", vector);
	SET_BUILTIN_FUNC("left", leftright);
	SET_BUILTIN_FUNC("right", leftright);
	SET_BUILTIN_FUNC("hilbert", hilbenvlope);
	SET_BUILTIN_FUNC("envlope", hilbenvlope);
	SET_BUILTIN_FUNC("printf", printf);
	SET_BUILTIN_FUNC("sprintf", printf);
	SET_BUILTIN_FUNC("json", json);

	SET_BUILTIN_FUNC("clear", clear);

	SET_BUILTIN_FUNC("resample", resample);
	SET_BUILTIN_FUNC("cell", rand);
	SET_BUILTIN_FUNC("rand", rand);
	SET_BUILTIN_FUNC("irand", irand);
	SET_BUILTIN_FUNC("randperm", randperm);
	SET_BUILTIN_FUNC("dir", dir);
	SET_BUILTIN_FUNC("issame", veq);
	SET_BUILTIN_FUNC("otype", datatype);
	SET_BUILTIN_FUNC("eval", eval);
	SET_BUILTIN_FUNC("str2num", str2num);
	SET_BUILTIN_FUNC("include", include);
	SET_BUILTIN_FUNC("diff", diff);
	SET_BUILTIN_FUNC("cumsum", cumsum);
	
	// Functions with no argument
	SET_BUILTIN_FUNC("getfs", noargs);
	SET_BUILTIN_FUNC("tic", noargs);
	SET_BUILTIN_FUNC("toc", noargs);

	SET_BUILTIN_FUNC("face", cellstruct);
	SET_BUILTIN_FUNC("face", structbase);
	SET_BUILTIN_FUNC("ismember", cellstruct);
	SET_BUILTIN_FUNC("erase", cellstruct);
	SET_BUILTIN_FUNC("members", structbase);

	SET_BUILTIN_FUNC("squeeze", vector);

	SET_BUILTIN_FUNC("error", error_warning);
	SET_BUILTIN_FUNC("warning", error_warning);

	SET_BUILTIN_FUNC("isempty", objchecker);
	SET_BUILTIN_FUNC("isaudio", objchecker);
	SET_BUILTIN_FUNC("isvector", objchecker);
	SET_BUILTIN_FUNC("isstring", objchecker);
	SET_BUILTIN_FUNC("isstereo", objchecker);
	SET_BUILTIN_FUNC("isbool", objchecker);
	SET_BUILTIN_FUNC("iscell", objchecker);
	SET_BUILTIN_FUNC("isclass", objchecker);
	SET_BUILTIN_FUNC("istseq", objchecker);

	SET_BUILTIN_FUNC("setnextchan", setnextchan);

	SET_BUILTIN_FUNC("test", test);

	SET_BUILTIN_FUNC("tsq_isrel", tseqget);
	SET_BUILTIN_FUNC("tsq_getvalues", tseqget);
	SET_BUILTIN_FUNC("tsq_gettimes", tseqget);
	SET_BUILTIN_FUNC("tsq_setvalues", tseqset);
	SET_BUILTIN_FUNC("tsq_settimes", tseqset);

	SET_BUILTIN_FUNC("timestretch", pitchtime);
	SET_BUILTIN_FUNC("pitchscale", pitchtime);
	SET_BUILTIN_FUNC("respeed", pitchtime);

	SET_PSEUDO_VARS("i", imaginary_unit);
	SET_PSEUDO_VARS("e", natural_log_base);
	SET_PSEUDO_VARS("pi", pi);
	SET_PSEUDO_VARS("false", boolconst);
	SET_PSEUDO_VARS("true", boolconst);

//	name = "isaudioat";
//	ft.funcsignature = "(audio_signal, time_pt)";
//	ft.func =  &_isaudioat;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 2;
//	name = "std";
//	ft.funcsignature = "(length, [, bias])";
//	ft.func =  &_std;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "setnextchan";
//	ft.func = &_setnextchan;
//	builtin[name] = ft;

//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(graphic_handle)";
//	name = "replicate";
//	ft.func = &_replicate;
//	builtin[name] = ft;
//
//
//	ft.funcsignature = "(audio_signal)";
//	ft.narg1 = 1;	ft.narg2 = 1;
//	name = "contig";
//	ft.func = &_contig;
//	builtin[name] = ft;
//
//	ft.narg1 = 2;	ft.narg2 = 2;
//	name = "respeed";
//	ft.funcsignature = "(audio_signal, playback_rate_change_ratio)";
//	ft.func = &_time_freq_manipulate;
//	builtin[name] = ft;
//
//	ft.narg1 = 3;	ft.narg2 = 3;
//	name = "interp";
//	ft.funcsignature = "(refX, refY, query_x_points)";
//	ft.func = &_interp1;
//	builtin[name] = ft;
//
//	ft.narg1 = 1;	ft.narg2 = 1;
//	ft.funcsignature = "(filename)";
//	name = "include";
//	ft.func =  &_include; // check
//	builtin[name] = ft;
//	name = "fdelete";
//	ft.func =  &_fdelete; // check
//	builtin[name] = ft;
//

	ft.narg1 = 1;	ft.narg2 = 1;
	ft.funcsignature = "(value_or_array)";
	const char *fmath1[] = { "abs", "sqrt", "conj", "real", "imag", "angle", "sign", "sin", "cos", "tan", "asin", "acos", "atan", "log", "log10", "exp", "round", "fix", "ceil", "floor", 0 };
	for (int k = 0; fmath1[k]; k++)
	{
		name = fmath1[k];
		ft.func =  NULL;
		builtin.emplace(name, ft);
	}
}

static inline complex<auxtype> r2c_sqrt(complex<auxtype> x) { return sqrt(x); }
static inline complex<auxtype> r2c_log(complex<auxtype> x) { return log(x); }
static inline complex<auxtype> r2c_log10(complex<auxtype> x) { return log10(x); }
static inline complex<auxtype> r2c_pow(complex<auxtype> x, complex<auxtype> y) { return pow(x, y); }

bool isgraphicfunc(string fname)
{
	if (fname == "figure") return true;
	if (fname == "text") return true;
	if (fname == "delete") return true;
	if (fname == "plot") return true;
	if (fname == "axes") return true;
	return false;
}

static bool this_is_one_of_allowedset(uint16_t thistype, const vector<set<uint16_t>>::const_iterator& allowed)
{
	for (auto it = allowed->begin(); it != allowed->end(); it++)
	{
		if (thistype == *it) return true;
	}
	return false;
}

void AuxScope::make_check_args_math(const AstNode* pnode)
{
	ostringstream ostr;
	set<uint16_t> allowed = { 1, 2, 3, ALL_AUDIO_TYPES };
	vector<set<uint16_t>> allowedvector;
	allowedvector.push_back(allowed);
	auto it = allowedvector.begin();
	if (!this_is_one_of_allowedset(Sig.type(), it)) {
		ostr << "type " << Sig.type();
		throw exception_func(*this, pnode, ostr.str(), pnode->str).raise();
	}
}

static bool check_allowed(const vector<set<uint16_t>>::const_iterator& allowedset, uint16_t type)
{
	// 0xFFFF accepts all type; i.e., 0xFFFF bypasses checking
	if (*allowedset->begin() == 0xFFFF) return true;
	if (this_is_one_of_allowedset(type, allowedset)) return true;
	return false;
}

static bool check_pass_fail(uint16_t tipe, const set<pfunc_typecheck>& fpset, bool pass)
{
	for (auto fp : fpset) {
		if ((fp)(tipe)) {
			return !pass;
		}
	}
	return pass;
}

static bool check_with_type(const AstNode* pnode, bool struct_call, const CVar& obj, vector<CVar>& out, const set<pfunc_typecheck>& fp_qual_set, const set<pfunc_typecheck>& fp_reject_set, int& argind, string& errmsg)
{
	ostringstream ostr;
	auto tp = obj.type();
	bool Sigpass = false;
	Sigpass = check_pass_fail(tp, fp_qual_set, Sigpass);
	if (!Sigpass) {
		ostr << "type " << tp;
		errmsg = ostr.str();
		argind = 1;
		return Sigpass;
	}
	Sigpass = check_pass_fail(tp, fp_reject_set, Sigpass);
	if (!Sigpass) {
		ostr << "type " << tp;
		errmsg = ostr.str();
		argind = 1;
	}
	return Sigpass;
}

static bool check_more2(const AstNode* pnode, bool struct_call, AuxScope& smallAuxScope, const Cfunction& func, vector<CVar>& out, const CVar& Sig, int& argind, int& count, string& errmsg)
{
	// Check Sig
	auto it_fp_qual_set = func.qualify.begin();
	auto it_fp_reject_set = func.reject.begin();
	if (!check_with_type(pnode, struct_call, Sig, out, *it_fp_qual_set, *it_fp_reject_set, argind, errmsg)) {
		return false;
	}
	// Check other args
	ostringstream ostr;
	const AstNode* pn = get_second_arg(pnode, struct_call);
	for (; pn; pn = pn->next, count++) {
		if (func.narg2 >= 0 && count > func.narg2)
		{
			ostr << pnode->str << "(): too many args; maximum number of args is " << func.narg2 << ".";
			errmsg = ostr.str();
			argind = -1;
			return false;
		}
		try {
			smallAuxScope.Compute(pn);
			if (func.narg2 < 0) // unspecified max arg count, no type checking
				out.push_back(smallAuxScope.Sig);
			else
			{
				argind = count;
				it_fp_qual_set++;
				it_fp_reject_set++;
				if (!check_with_type(pnode, struct_call, smallAuxScope.Sig, out, *it_fp_qual_set, *it_fp_reject_set, argind, errmsg)) {
					return false;
				}
				else {
					out.push_back(smallAuxScope.Sig);
				}
			}
		}
		catch (AuxScope_exception e) {
			throw e;
		}
	}
	count--;
	if (count < func.narg1)
	{
		ostr << pnode->str << "(): the number of arg should be at least " << func.narg1 << "; Only " << count << " given.";
		errmsg = ostr.str();
		argind = count;
		return false;
	}
	return true;
}

static bool check_more(const AstNode* pnode, bool struct_call, AuxScope& smallAuxScope, const Cfunction& func, vector<CVar>& out, const CVar& Sig, int& argind, int& count, string& errmsg)
{
	ostringstream ostr;
	vector<set<uint16_t>>::const_iterator allowedset = func.allowed_arg_types.begin();
	if (!check_allowed(allowedset, Sig.type()))
	{
		ostr << "type " << Sig.type();
		errmsg = ostr.str();
		argind = 1;
		return false;
	}
	allowedset++;
	const AstNode* pn = get_second_arg(pnode, struct_call);
	for (; pn; pn = pn->next, count++)
	{
		if (func.narg2 >= 0 && count > func.narg2)
		{
			ostr << pnode->str << "(): too many args; maximum number of args is " << func.narg2 << ".";
			errmsg = ostr.str();
			argind = -1;
			return false;
		}
		try {
			smallAuxScope.Compute(pn);
			if (func.narg2 < 0) // unspecified max arg count, no type checking
				out.push_back(smallAuxScope.Sig);
			else
			{
				// for arguments with any type (e.g., fwrite), bypass checking with 0xFFFF and add the result of Compute to the out vector
				if (check_allowed(allowedset, smallAuxScope.Sig.type()))
				{
					out.push_back(smallAuxScope.Sig);
				}
				else
				{
					ostr << "type " << smallAuxScope.Sig.type();
					errmsg = ostr.str();
					argind = count;
					return false;
				}
				allowedset++;
			}
		}
		catch (AuxScope_exception e) {
			throw e;
		}
	}
	count--;
	if (count < func.narg1)
	{
		ostr << pnode->str << "(): the number of arg should be at least " << func.narg1 << "; Only " << count << " given.";
		errmsg = ostr.str();
		argind = count;
		return false;
	}
	return true;
}

vector<CVar> AuxScope::make_check_args(const AstNode* pnode, multimap<string, Cfunction>::iterator& ftlist)
{ // Goal: make args vector for the function gate
  // the first arg is always Sig. The second arg is the first output vector.
  // check the arg types; if a type of a given arg is not one of allowed ones, throw.
  // check minimum, maximum number of args; if the number of given args is outside the range, throw. 
  // fill default arguments in the arg vector if not specified
	vector<CVar> out;
	ostringstream ostr;
	bool struct_call = pnode->type == N_STRUCT;
	string fname = pnode->str;
	// if a ftlist item has multiple gate functions, the first one should be the one with Sig and no other args.
	// and it is taken care of here with an empty out.
	// if any of Cfunction in ftlist allows empty and pnode->alt and pnode->child are NULL, return immediately
	for (; ftlist != pEnv->builtin.end() && (*ftlist).first == fname; ftlist++)
		if ((*ftlist).second.allowed_arg_types.empty() && !pnode->alt && !pnode->child) 
			return out;
	ftlist = pEnv->builtin.find(fname);
	Cfunction func = (*ftlist).second;
	if (func.alwaysstatic && struct_call)
	{
		ostr << "function " << fname << " does not allow . (dot) notation call.";
		throw exception_func(*this, pnode, ostr.str(), fname).raise();
	}
	if (Sig.type() == TYPEBIT_NULL && func.narg1 == 0 && func.narg2 > 0)
		Sig = func.defaultarg.front();
	AuxScope smallAuxScope(this);
	int argind;
	string errmsg;
	int count_ftlist = 0;
	for (; ftlist != pEnv->builtin.end() && (*ftlist).first == fname; count_ftlist++, ftlist++) {
		int count = 2;
		func = (*ftlist).second;
		if (func.qualify.empty() && func.reject.empty()) {
			if (check_more(pnode, struct_call, smallAuxScope, func, out, Sig, argind, count, errmsg)) {
				for (; count < func.narg2; count++)
					out.push_back(CVar(func.defaultarg[count - func.narg1]));
				return out;
			}
		}
		else {
			if (check_more2(pnode, struct_call, smallAuxScope, func, out, Sig, argind, count, errmsg)) {
				for (; count < func.narg2; count++)
					out.push_back(CVar(func.defaultarg[count - func.narg1]));
				return out;
			}
		}
	}
	// at this point, none of function signatures fit the given argument set
	if (count_ftlist==1) // for single function signatures, it's straightforward
		throw exception_func(*this, pnode, errmsg, fname, argind).raise();
	throw exception_func(*this, pnode, errmsg, fname).raise();
}

const AstNode* find_parentnode_alt(const AstNode* pnode, const AstNode* pRoot0)
{
	// determine if pnode is part of N_VECTOR nodes from pRoot0
	for (auto p = pRoot0; p; p = p->alt)
	{
		if (p->type==N_VECTOR && p->alt== pnode)
			return p;
		if (p->alt == pnode)
			return p;
	}
	return pnode;
}

const AstNode* arg0node(const AstNode* pnode, const AstNode* pRoot0)
{
	if (pnode->type == N_STRUCT)
	{
		return find_parentnode_alt(pnode, pRoot0);
	}
	else
	{
		if (pnode->alt)
		{
			if (pnode->alt->type == N_ARGS)
				return pnode->alt->child;
			else if (pnode->alt->type == N_HOOK)
				return pnode->alt;
		}
		return NULL;
	}
}

void AuxScope::HandleAuxFunction(const AstNode *pnode)
{
	string fnsigs;
	string fname = pnode->str;
	multimap<string, Cfunction>::iterator ftlist = pEnv->builtin.find(fname);
	bool structCall = pnode->type == N_STRUCT;
	const AstNode* arg0 = arg0node(pnode, node);
	if (!structCall)
	{
		if (arg0)
			Compute(arg0); // Update Sig with arg0; For strucCall, it was already done in read_node()
		else
			Sig.Reset();
	}
	if ((*ftlist).second.func)
	{
		vector<CVar> args = make_check_args(pnode, ftlist);
		/* IMPORTANT--
		* When the gate function is called, Sig is always the first argument (this is to avoid going through a copy constructor when used inside a gate function)
		* Just make sure to avoid calling Compute() inside the gate function before Sig is used for actual builtin function operation
		* If you still need to call Compute(); make a copy of Sig and do so and accept that a tiny performance degradation may occur
		*/
		Cfunction builtinFunCarrier = (*ftlist).second;
		// if arg0 is the only arg and is null, bypass everything and the output is null
		if (args.size()!=0 || 
			Sig.type()!=TYPEBIT_NULL ||
			(builtinFunCarrier.narg1==0 && args.empty()) ||
			(*builtinFunCarrier.allowed_arg_types.front().begin())==0xFFFF)
			builtinFunCarrier.func(this, pnode, args);
	}
	else
	{
		if (Sig.type() == 0) return;
		make_check_args_math(pnode);
		const AstNode* p2 = get_second_arg(pnode, pnode->type == N_STRUCT);
		if (p2)
		{ // only for those math functions taking two args, e.g., mod, pow
			AuxScope smallAuxScope(this);
			smallAuxScope.Compute(p2);
			smallAuxScope.make_check_args_math(p2);
			HandleMathFunc(fname, smallAuxScope.Sig);
		}
		else
			HandleMathFunc(fname, CVar()); // just a dummy arg
	}
	Sig.functionEvalRes = true;
	return;
}

Cfunction set_builtin_function_clear(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object"  };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _clear(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	AstNode* arg0 = (AstNode*)pnode;
	if (arg0->type==T_ID)
		past->ClearVar(arg0->str);
	else if (arg0->type == N_STRUCT)
		past->ClearVar(past->pheadthisline->str);
	else
		throw exception_etc(*past, pnode, "Only variables can be cleared.").raise();
	auto tp = past->Sig.type();
	// Cleaning up pgo
	if (past->pgo != nullptr && ISSCALARG(tp) && ISSTRUT(tp) && past->pgo.get()->value() == past->Sig.value())
	{
		past->pgo.reset();
		past->pgo = nullptr;
	}
	past->Sig.Reset();
}

