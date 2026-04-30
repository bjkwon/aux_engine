#include "functions_common.h"
#include "fftw3.h"

/* An audio handle has the following members:
*	fs: sampling rate
*	chan: the number of channels
*	format: audio format (char, int16, float32, double, etc)
*	dur: duration in sec
*	dur_prog: duration played/recorded in sec
*	repeat: repeat count (default 1)
*	dev: device id's for the audio event, can be an array
*	block: the length of the recording/playing buffer per channel
*	buffer; a CSignals object (valid while active--a short life cycle
*	active: bool (true, or false).
*/

Cfunction set_builtin_function_pitchtime(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "factor" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, TYPEBIT_TEMPO_CHAINS, TYPEBIT_TEMPO_CHAINS + 1 };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

int timeargument(CVar& ratio, double audioDur, int fs)
{
	constexpr double kEndMarginMs = 10.0;
	CTimeSeries* pratio = (CTimeSeries*)&ratio;
	if (pratio->fs == 0) // relative
	{
		for (CTimeSeries* p = pratio; p; p = p->chain)
		{
			p->tmark *= audioDur;
			p->SetFs(fs);
		}
	}
	for (CTimeSeries* p = pratio; p; p = p->chain)
	{
		if (p->value() <= 0.)
			throw "Pitch/time factor must be positive.";
	}
	if (pratio->tmark != 0.)
	{
		CTimeSeries newParam(fs);
		newParam.tmark = 0.;
		newParam.SetValue(pratio->value());
		newParam.chain = new CTimeSeries;
		*newParam.chain = *pratio;
		*pratio = newParam;
	}
	CTimeSeries* pLast = nullptr;
	for (CTimeSeries* p = pratio; p; p = p->chain)
		if (!p->chain)
			pLast = p;
	if (pLast && fabs(pLast->tmark - audioDur) > kEndMarginMs)
	{
		CTimeSeries newParam(fs);
		newParam.tmark = audioDur;
		newParam.SetValue(pLast->value());
		pLast->chain = new CTimeSeries;
		*pLast->chain = newParam;
	}
	for (CTimeSeries* p = pratio; p; p = p->chain)
	{
		if (p->tmark < 0 || p->tmark > audioDur)
			throw "Time arguments set out of range of the audio block.";
	}
	return 1;
}

namespace {

constexpr int kMinWindowSize = 64;
constexpr int kMaxWindowSize = 8192;
constexpr double kTransientFluxThreshold = 0.35;
constexpr double kPhaseLockPeakFloor = 0.05;

struct RatioProfile
{
	vector<int> samples;
	vector<double> values;
};

static inline double wrap_phase(double x)
{
	while (x <= -PI) x += 2.0 * PI;
	while (x > PI) x -= 2.0 * PI;
	return x;
}

static inline int next_pow2(int value)
{
	int out = 1;
	while (out < value)
		out <<= 1;
	return out;
}

static RatioProfile build_ratio_profile(const CVar& ratio, int fs, int nSamples)
{
	RatioProfile profile;
	if (!Cfunction::IsTEMPORALG(ratio.GetType()))
	{
		const double v = ratio.value();
		profile.samples = { 0, nSamples };
		profile.values = { v, v };
		return profile;
	}

	for (const CTimeSeries* p = (const CTimeSeries*)&ratio; p; p = p->chain)
	{
		int sample = (int)llround(p->tmark * fs / 1000.0);
		sample = max(0, min(sample, nSamples));
		if (!profile.samples.empty() && sample == profile.samples.back())
		{
			profile.values.back() = p->value();
		}
		else
		{
			profile.samples.push_back(sample);
			profile.values.push_back(p->value());
		}
	}

	if (profile.samples.empty())
	{
		profile.samples = { 0, nSamples };
		profile.values = { 1.0, 1.0 };
		return profile;
	}
	if (profile.samples.front() > 0)
	{
		profile.samples.insert(profile.samples.begin(), 0);
		profile.values.insert(profile.values.begin(), profile.values.front());
	}
	if (profile.samples.back() < nSamples)
	{
		profile.samples.push_back(nSamples);
		profile.values.push_back(profile.values.back());
	}
	return profile;
}

static double ratio_at_sample(const RatioProfile& profile, double samplePos, size_t& cursor)
{
	if (profile.samples.size() == 1)
		return profile.values.front();

	while (cursor + 1 < profile.samples.size() - 1 && samplePos > profile.samples[cursor + 1])
		++cursor;

	const int x0 = profile.samples[cursor];
	const int x1 = profile.samples[cursor + 1];
	const double y0 = profile.values[cursor];
	const double y1 = profile.values[cursor + 1];
	if (x1 <= x0)
		return y1;
	const double t = (samplePos - x0) / (double)(x1 - x0);
	return y0 + (y1 - y0) * t;
}

static int estimate_output_length(const RatioProfile& profile)
{
	double total = 0.;
	for (size_t i = 0; i + 1 < profile.samples.size(); ++i)
	{
		const int x0 = profile.samples[i];
		const int x1 = profile.samples[i + 1];
		if (x1 <= x0)
			continue;
		total += 0.5 * (profile.values[i] + profile.values[i + 1]) * (x1 - x0);
	}
	return max(1, (int)ceil(total));
}

static vector<double> make_hann_window(int winLen)
{
	vector<double> window(winLen);
	if (winLen == 1)
	{
		window[0] = 1.;
		return window;
	}
	for (int i = 0; i < winLen; ++i)
		window[i] = 0.5 * (1.0 - cos(2.0 * PI * i / (winLen - 1.0)));
	return window;
}

static vector<double> make_sqrt_hann_window(int winLen)
{
	vector<double> window = make_hann_window(winLen);
	for (double& v : window)
		v = sqrt(max(0.0, v));
	return window;
}

static vector<int> find_spectral_peaks(const vector<double>& magnitude, double floorRatio)
{
	vector<int> peaks;
	if (magnitude.size() < 3)
		return peaks;
	double peakMax = 0.;
	for (double v : magnitude)
		peakMax = max(peakMax, v);
	const double floor = peakMax * floorRatio;
	for (size_t k = 1; k + 1 < magnitude.size(); ++k)
	{
		if (magnitude[k] >= floor && magnitude[k] > magnitude[k - 1] && magnitude[k] >= magnitude[k + 1])
			peaks.push_back((int)k);
	}
	return peaks;
}

static double spectral_flux(const vector<double>& magnitude, const vector<double>& previousMagnitude)
{
	if (magnitude.size() != previousMagnitude.size())
		return 0.;
	double flux = 0.;
	double energy = 0.;
	for (size_t i = 0; i < magnitude.size(); ++i)
	{
		const double delta = magnitude[i] - previousMagnitude[i];
		if (delta > 0.)
			flux += delta;
		energy += magnitude[i];
	}
	if (energy <= 1.e-12)
		return 0.;
	return flux / energy;
}

#ifdef FLOAT
using PvPlan = fftwf_plan;
using PvComplex = fftwf_complex;
static inline auxtype* pv_alloc_real(size_t n) { return (auxtype*)fftwf_malloc(sizeof(auxtype) * n); }
static inline PvComplex* pv_alloc_complex(size_t n) { return (PvComplex*)fftwf_malloc(sizeof(PvComplex) * n); }
static inline void pv_free(void* p) { fftwf_free(p); }
static inline PvPlan pv_plan_r2c(int n, auxtype* in, PvComplex* out) { return fftwf_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE); }
static inline PvPlan pv_plan_c2r(int n, PvComplex* in, auxtype* out) { return fftwf_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE); }
static inline void pv_execute(PvPlan p) { fftwf_execute(p); }
static inline void pv_destroy(PvPlan p) { fftwf_destroy_plan(p); }
#else
using PvPlan = fftw_plan;
using PvComplex = fftw_complex;
static inline auxtype* pv_alloc_real(size_t n) { return (auxtype*)fftw_malloc(sizeof(auxtype) * n); }
static inline PvComplex* pv_alloc_complex(size_t n) { return (PvComplex*)fftw_malloc(sizeof(PvComplex) * n); }
static inline void pv_free(void* p) { fftw_free(p); }
static inline PvPlan pv_plan_r2c(int n, auxtype* in, PvComplex* out) { return fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE); }
static inline PvPlan pv_plan_c2r(int n, PvComplex* in, auxtype* out) { return fftw_plan_dft_c2r_1d(n, in, out, FFTW_ESTIMATE); }
static inline void pv_execute(PvPlan p) { fftw_execute(p); }
static inline void pv_destroy(PvPlan p) { fftw_destroy_plan(p); }
#endif

struct PvWorkspace
{
	auxtype* timeIn = nullptr;
	auxtype* timeOut = nullptr;
	PvComplex* spectrum = nullptr;
	PvPlan forward = nullptr;
	PvPlan inverse = nullptr;

	~PvWorkspace()
	{
		if (forward) pv_destroy(forward);
		if (inverse) pv_destroy(inverse);
		if (timeIn) pv_free(timeIn);
		if (timeOut) pv_free(timeOut);
		if (spectrum) pv_free(spectrum);
	}
};

static CSignal phase_vocoder_timestretch(const CSignal& base, const CVar& ratio, int fs)
{
	int winLen = min((int)base.nSamples / 5, 512);
	if (!ratio.strut.empty())
	{
		auto finder = ratio.strut.find("windowsize");
		if (finder != ratio.strut.end())
			winLen = (int)finder->second.value();
	}
	winLen = max(kMinWindowSize, min(winLen, kMaxWindowSize));
	if (winLen % 2 != 0)
		++winLen;

	const int anaHop = max(1, winLen / 4);
	const int fftSize = next_pow2(winLen);
	const int bins = fftSize / 2 + 1;
	const int halfWin = winLen / 2;
	const vector<double> window = make_sqrt_hann_window(winLen);
	const RatioProfile profile = build_ratio_profile(ratio, fs, (int)base.nSamples);
	const int estimatedOutputSamples = estimate_output_length(profile);
	const int outputPad = winLen;

	PvWorkspace ws;
	ws.timeIn = pv_alloc_real(fftSize);
	ws.timeOut = pv_alloc_real(fftSize);
	ws.spectrum = pv_alloc_complex(bins);
	ws.forward = pv_plan_r2c(fftSize, ws.timeIn, ws.spectrum);
	ws.inverse = pv_plan_c2r(fftSize, ws.spectrum, ws.timeOut);
	if (!ws.timeIn || !ws.timeOut || !ws.spectrum || !ws.forward || !ws.inverse)
		throw std::runtime_error("Failed to initialize phase vocoder FFT workspace.");

	vector<double> prevPhase(bins, 0.);
	vector<double> synthPhase(bins, 0.);
	vector<double> analysisPhase(bins, 0.);
	vector<double> propagatedPhase(bins, 0.);
	vector<double> lockedPhase(bins, 0.);
	vector<double> magnitude(bins, 0.);
	vector<double> prevMagnitude(bins, 0.);
	vector<double> omega(bins, 0.);
	for (int k = 0; k < bins; ++k)
		omega[k] = 2.0 * PI * k / fftSize;

	vector<double> output(estimatedOutputSamples + 4 * winLen, 0.);
	vector<double> norm(output.size(), 0.);
	double synthCenter = 0.;
	int lastWritten = outputPad;
	size_t ratioCursor = 0;
	bool firstFrame = true;

	for (int analysisCenter = 0; analysisCenter < (int)base.nSamples + halfWin; analysisCenter += anaHop)
	{
		memset(ws.timeIn, 0, sizeof(*ws.timeIn) * fftSize);
		for (int n = 0; n < winLen; ++n)
		{
			const int src = analysisCenter - halfWin + n;
			const double sample = (src >= 0 && src < (int)base.nSamples) ? base.buf[src] : 0.;
			ws.timeIn[n] = (auxtype)(sample * window[n]);
		}

		pv_execute(ws.forward);
		for (int k = 0; k < bins; ++k)
		{
			const double re = ws.spectrum[k][0];
			const double im = ws.spectrum[k][1];
			magnitude[k] = hypot(re, im);
			analysisPhase[k] = atan2(im, re);
		}

		const double frameRatio = ratio_at_sample(profile, analysisCenter, ratioCursor);
		const double synHop = max(1.0, anaHop * frameRatio);
		const bool transientFrame = !firstFrame && spectral_flux(magnitude, prevMagnitude) > kTransientFluxThreshold;

		if (firstFrame || transientFrame)
		{
			synthPhase = analysisPhase;
			firstFrame = false;
		}
		else
		{
			for (int k = 0; k < bins; ++k)
			{
				const double delta = wrap_phase(analysisPhase[k] - prevPhase[k] - omega[k] * anaHop);
				const double trueFreq = omega[k] + delta / anaHop;
				propagatedPhase[k] = synthPhase[k] + trueFreq * synHop;
			}

			lockedPhase = propagatedPhase;
			const vector<int> peaks = find_spectral_peaks(magnitude, kPhaseLockPeakFloor);
			if (!peaks.empty())
			{
				for (size_t i = 0; i < peaks.size(); ++i)
				{
					const int peak = peaks[i];
					const int left = (i == 0) ? 1 : (peaks[i - 1] + peak) / 2 + 1;
					const int right = (i + 1 == peaks.size()) ? (bins - 2) : (peak + peaks[i + 1]) / 2;
					lockedPhase[peak] = propagatedPhase[peak];
					for (int k = left; k <= right; ++k)
					{
						if (k == peak)
							continue;
						lockedPhase[k] = propagatedPhase[peak] + wrap_phase(analysisPhase[k] - analysisPhase[peak]);
					}
				}
			}
			synthPhase = lockedPhase;
		}

		prevPhase = analysisPhase;
		prevMagnitude = magnitude;
		for (int k = 0; k < bins; ++k)
		{
			ws.spectrum[k][0] = (auxtype)(magnitude[k] * cos(synthPhase[k]));
			ws.spectrum[k][1] = (auxtype)(magnitude[k] * sin(synthPhase[k]));
		}

		pv_execute(ws.inverse);
		const int synthStart = outputPad + (int)llround(synthCenter) - halfWin;
		if (synthStart + winLen >= (int)output.size())
		{
			const int growTo = synthStart + winLen + 2 * winLen;
			output.resize(growTo, 0.);
			norm.resize(growTo, 0.);
		}
		for (int n = 0; n < winLen; ++n)
		{
			const int dst = synthStart + n;
			if (dst < 0 || dst >= (int)output.size())
				continue;
			const double sample = (ws.timeOut[n] / fftSize) * window[n];
			output[dst] += sample;
			norm[dst] += window[n] * window[n];
		}
		lastWritten = max(lastWritten, synthStart + winLen);
		synthCenter += synHop;
	}

	const int maxAvailable = max(1, lastWritten - outputPad);
	const int renderedSamples = max(1, (int)llround(synthCenter));
	const int outSamples = min(maxAvailable, max(renderedSamples, min(maxAvailable, estimatedOutputSamples)));
	CSignal out(fs);
	out.UpdateBuffer(outSamples);
	out.tmark = 0.;
	for (int i = 0; i < outSamples; ++i)
	{
		const int src = outputPad + i;
		double sample = output[src];
		if (norm[src] > 1.e-9)
			sample /= norm[src];
		out.buf[i] = (auxtype)sample;
	}
	out.nSamples = outSamples;
	out.SetFs(fs);
	return out;
}

static CSignal __tsbase(const CSignal& base, void* parg, void* /*parg2*/)
{
	vector<CVar>* pargs = (vector<CVar>*)parg;
	const CVar ratio = pargs->front();
	const int fs = pargs->back().GetFs();
	return phase_vocoder_timestretch(base, ratio, fs);
}

} // namespace

CSignal __resample(const CSignal& base, void* pargin, void* pargout); // from movespec.cpp; pargin is vector<CVar>

void _pitchtime(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	double audioDur = past->Sig.dur();
	int fs = past->Sig.GetFs();

	if (fname == "timestretch") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs);
		vector<CVar> pvArgs;
		pvArgs.push_back(factor);
		pvArgs.push_back((CVar)fs);
		past->Sig = past->Sig.evoke_modsig2(__tsbase, &pvArgs);
	}
	else if (fname == "pitchscale") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs);
		vector<CVar> pvArgs;
		pvArgs.push_back(factor);
		pvArgs.push_back((CVar)fs);
		past->Sig = past->Sig.evoke_modsig2(__tsbase, &pvArgs);
		past->Sig = past->Sig.evoke_modsig2(__resample, &pvArgs);
	}
	else if (fname == "respeed") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs);
		vector<CVar> rsArgs;
		rsArgs.push_back(factor);
		past->Sig = past->Sig.evoke_modsig2(__resample, &rsArgs);
	}
}
