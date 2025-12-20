#include "functions_common.h"

Cfunction set_builtin_function_tone(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "f", "dur", };
	vector<string> desc_arg_opt = { "phase", };
	vector<CVar> default_arg = { CVar(0.f), };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

// DOCUMENT initPhase --- 1 coresponds to PI (180 degrees)
// If you want to make 1 corresponding to 360 degrees.... go ahead, change and document it 12/10/2020
// CHECK!!!!!!!!!!!!!!! 1/22/2021
static inline void __tone(auxtype *buf, auxtype freq, unsigned int length, int fs, auxtype initPhase)
{
	auto k = (unsigned int)0;
	initPhase *= 2 * PI;
	for_each(buf, buf + length, [&k, freq, fs, initPhase](auxtype& v)
		{ v = sinf(freq / (auxtype)fs * k++ + initPhase); });
}

static inline void __tone_glide(auxtype* buf, auxtype f1, auxtype f2, unsigned int length, int fs)
{
	auto t(0.f), tgrid(1.f / fs);
	auto duration = (auxtype)length / fs;
	auto gld = (f2 - f1) / 2. / duration;
	for_each(buf, buf + length, [&t, tgrid, f1, gld](auxtype& v)
	{
		t += tgrid;
		v = sinf(2.f * PI * t * (f1 + gld * t));
	});
}

static inline void __fm(auxtype* buf, int fs, auxtype midFreq, auxtype fmWidth, auxtype fmRate, unsigned int length, auxtype beginFMPhase)
{   // beginFMPhase is to be set. (But the beginning phase of the sine wave is not meaningful, so skipped)
	auto t(0.), tgrid(1. / fs);
	for_each(buf, buf + length, [&t, tgrid, midFreq, fmWidth, fmRate, beginFMPhase](auxtype& v)
	{
		t += tgrid;
		v = sinf(2.f * PI * t * midFreq - fmWidth / fmRate * cos(2.f * PI * (fmRate * t + beginFMPhase)));
	});
}

void _tone(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar freq = past->Sig;
	if (freq._max() >= past->GetFs() / 2)
		throw exception_func(*past, pnode, "Frequency exceeds Nyquist frequency.", "tone()", 1).raise();
	auxtype duration = args[0].value();
	if (duration <= 0.)
		throw exception_func(*past, pnode, "duration must be positive.", "tone()", 2).raise();
	auxtype initPhase = args[1].value();
	past->Sig.Reset(past->GetFs());
	auto nSamplesNeeded = (unsigned int)round(duration / 1000.f * past->Sig.GetFs());
	past->Sig.Reset();
	past->Sig.UpdateBuffer(nSamplesNeeded); //allocate memory if necessary
	if (freq.nSamples == 1)
	{
		__tone(past->Sig.buf, 2.f * PI * freq.value(), nSamplesNeeded, past->Sig.GetFs(), initPhase);
	}
	else // len == 2
	{
		// For now, initPhase is ignored when a two-element freq is given.
		__tone_glide(past->Sig.buf, freq.buf[0], freq.buf[1], nSamplesNeeded, past->Sig.GetFs());
	}
}
