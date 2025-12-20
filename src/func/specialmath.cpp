#include "functions_common.h"

Cfunction set_builtin_function_pow(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "value_or_array", "value_or_array" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, ALL_AUDIO_TYPES, TYPEBIT_COMPLEX + 1, TYPEBIT_COMPLEX + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, TYPEBIT_COMPLEX + 1, TYPEBIT_COMPLEX + 2 };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_mod(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "value_or_array", "value_or_array" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

complex<double> comp_pow(complex<double> x, complex<double> y)
{
	return pow(x, y);
}

CTimeSeries __pow(const CTimeSeries& base, void* p)
{
	CTimeSeries out(base);
	CSignal operand = *(CSignal*)p;
	auto tp = base.type();
	if (ISAUDIO(tp))
	{
#ifdef FLOAT
		out.each_sym2(powf, operand);
#else
		out.each_sym2(pow, operand);
#endif
	}
	else if (base._min() < 0)
	{
		CTimeSeries copy(base);
		copy.SetComplex();
		out.SetComplex();
		if (operand.nSamples == 1)
		{
			auto op = operand.value();
			complex<auxtype> temp;
			for (auto k = 0; k < base.nSamples; k++)
				out.cbuf[k] = pow(copy.cbuf[k], operand.value());
		}
		else if (base.nSamples == 1)
		{
			auto baseval = copy.cvalue();
			out.UpdateBuffer(operand.nSamples);
			for (auto k = 0; k < operand.nSamples; k++)
				out.cbuf[k] = pow(baseval, operand.buf[k]);
		}
		else
		{
			for (auto k = 0; k < operand.nSamples; k++)
				out.cbuf[k] = pow(copy.cbuf[k], operand.buf[k]);
		}
	}
	else if (ISCOMPLEX(tp) || ISCOMPLEX(operand.type()))
	{
		out.each(comp_pow, operand);
	}
	else
	{
#ifdef FLOAT
		out.each(powf, operand);
#else
		out.each(pow, operand);
#endif		
	}
	return out;
}

void _pow(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	past->Sig = past->Sig.evoke_getsig(&__pow, (void*)&args[0]);
}

CTimeSeries __mod(const CTimeSeries& base, void* p)
{
	CTimeSeries out(base);
	body operand = *(body*)p;
	out.each_sym2(fmod, operand);
	return out;
}

void _mod(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar base = past->Sig;
	CVar oper = args[0];
	past->Sig = past->Sig.evoke_getsig(&__mod, (void*)&oper);
}

