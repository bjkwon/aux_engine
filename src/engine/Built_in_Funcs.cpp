// AUXLAB 
//
// Copyright (c) 2009-2018 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.495
// Date: 12/13/2018
// 

#include <math.h>
#include <map>
#include <string>
#include <time.h>
#include "aux_classes.h"
#include "AuxScope.h"
using namespace std;

typedef void(*fGate) (AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

complex<auxtype> cmpexp(complex<auxtype> x) { return exp(x); }
complex<auxtype> cmpcos(complex<auxtype> x) { return cos(x); }
complex<auxtype> cmpcosh(complex<auxtype> x) { return cosh(x); }
complex<auxtype> cmplog(complex<auxtype> x) { return log(x); }
complex<auxtype> cmplog10(complex<auxtype> x) { return log10(x); }
complex<auxtype> cmpsin(complex<auxtype> x) { return sin(x); }
complex<auxtype> cmpsinh(complex<auxtype> x) { return sinh(x); }
complex<auxtype> cmptan(complex<auxtype> x) { return tan(x); }
complex<auxtype> cmptanh(complex<auxtype> x) { return tanh(x); }

auxtype cmpreal(complex<auxtype> x) { return real(x); }
auxtype cmpimag(complex<auxtype> x) { return imag(x); }
auxtype cmpabs(complex<auxtype> x) { return abs(x); }
complex<auxtype> cmpconj(complex<auxtype> x) { return conj(x); }
complex<auxtype> cmpsqrt(complex<auxtype> x) { return sqrt(x); }
//auxtype cmpnorm(complex<auxtype> x) { return norm(x); }
auxtype cmpangle(complex<auxtype> x) { return arg(x); }

auxtype aux_db(auxtype x)
{
	return powf(10.f, x / 20.f);
}

auxtype aux_sign(auxtype x)
{
	return (x == 0.f) ? 0.f : ((x>0.f) ? 1.f : -1.f);
}

auxtype aux_round(auxtype x)
{
	if (x >= 0.f)
		return (auxtype)(int)(x + .5f);
	else
		return -aux_round(-x);
}

auxtype aux_fix(auxtype x)
{
	return (auxtype)(int)x;
}
complex<auxtype> aux_cexp(complex<auxtype> base, complex<auxtype> exponent)
{
	return pow(base, exponent);
}

auxtype aux_passthru(auxtype number)
{
	return number;
}

auxtype aux_angle(auxtype number)
{
	return number;
}

auxtype aux_angle_4_real(auxtype number)
{
	return number > 0.f ? acosf(1.f) : acosf(-1.f);
}

static inline complex<auxtype> r2c_sqrt(complex<auxtype> x) { return sqrt(x); }
static inline complex<auxtype> r2c_log(complex<auxtype> x) { return log(x); }
static inline complex<auxtype> r2c_log10(complex<auxtype> x) { return log10(x); }

void AuxScope::HandleMathFunc(string& fname, const body& arg)
{
	auxtype(*fn1)(auxtype) = NULL;
	auxtype(*fn2)(auxtype, auxtype) = NULL;
	auxtype(*cfn0)(complex<auxtype>) = NULL;
	complex<auxtype>(*cfn1)(complex<auxtype>) = NULL;
	complex<auxtype>(*cfn2)(complex<auxtype>, complex<auxtype>) = NULL;
	if (fname == "abs")
	{
		if (Sig.IsComplex())		cfn0 = cmpabs, Sig.each(cfn0);
		else						fn1 = fabs, Sig.each(fn1);
	}
	else if (fname == "conj") { if (Sig.IsComplex()) cfn1 = cmpconj; 	else	fn1 = fabs; }
	else if (fname == "real") { if (Sig.IsComplex()) cfn0 = cmpreal; 	else fn1 = aux_passthru; }
	else if (fname == "imag") {
		if (Sig.IsComplex()) cfn0 = cmpimag;
		else {
			Sig.SetReal();
			for (auto& val : Sig) val = 0.;
			fn1 = aux_passthru;
		}
	}
	else if (fname == "angle") { if (Sig.IsComplex()) cfn0 = cmpangle;	else fn1 = aux_angle_4_real; }
#ifdef FLOAT
	else if (fname == "sin")	if (Sig.IsComplex())	cfn1 = cmpsin; else fn1 = sinf;
	else if (fname == "cos")	if (Sig.IsComplex())	cfn1 = cmpcos; else fn1 = cosf;
	else if (fname == "tan")	if (Sig.IsComplex())	cfn1 = cmptan; else fn1 = tanf;
	else if (fname == "exp")	if (Sig.IsComplex())	cfn1 = cmpexp; else fn1 = expf;
	else if (fname == "db")		fn1 = aux_db;
	else if (fname == "sign")	fn1 = aux_sign;
	else if (fname == "asin")	fn1 = asinf;
	else if (fname == "acos")	fn1 = acosf;
	else if (fname == "atan")	fn1 = atanf;
	else if (fname == "round")	fn1 = aux_round;
	else if (fname == "fix")	fn1 = aux_fix;
	else if (fname == "ceil")	fn1 = ceilf;
	else if (fname == "floor")	fn1 = floorf;
	else if (fname == "log")
	{
		fn1 = logf, cfn1 = r2c_log;
	}
	else if (fname == "log10")
	{
		fn1 = log10f, cfn1 = r2c_log10;
	}
	else if (fname == "sqrt")
	{
		fn1 = sqrtf; cfn1 = r2c_sqrt;
	}
#else
	else if (fname == "sin")	if (Sig.IsComplex())	cfn1 = cmpsin; else fn1 = sin;
	else if (fname == "cos")	if (Sig.IsComplex())	cfn1 = cmpcos; else fn1 = cos;
	else if (fname == "tan")	if (Sig.IsComplex())	cfn1 = cmptan; else fn1 = tan;
	else if (fname == "exp")	if (Sig.IsComplex())	cfn1 = cmpexp; else fn1 = exp;
	else if (fname == "db")		fn1 = aux_db;
	else if (fname == "sign")	fn1 = aux_sign;
	else if (fname == "asin")	fn1 = asin;
	else if (fname == "acos")	fn1 = acos;
	else if (fname == "atan")	fn1 = atan;
	else if (fname == "round")	fn1 = aux_round;
	else if (fname == "fix")	fn1 = aux_fix;
	else if (fname == "ceil")	fn1 = ceil;
	else if (fname == "floor")	fn1 = floor;
	else if (fname == "log")
	{
		fn1 = log, cfn1 = r2c_log;
	}
	else if (fname == "log10")
	{
		fn1 = log10, cfn1 = r2c_log10;
	}
	else if (fname == "sqrt")
	{
		fn1 = sqrt; cfn1 = r2c_sqrt;
	}
#endif
	if (fname == "sqrt" || fname == "log10" || fname == "log")
	{
		if (Sig.IsComplex())
		{
			Sig.each(cfn1);
		}
		else if (Sig._min() < 0)
		{
			if (ISAUDIO(Sig.type()))
				Sig.each_allownegative(fn1);
			else
			{
				Sig.SetComplex();
				Sig.each(cfn1);
			}
		}
		else
			Sig.each(fn1);
		return;
	}
	Sig.each(fn1);
}
