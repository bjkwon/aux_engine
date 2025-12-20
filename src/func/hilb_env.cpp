#include "fftw3.h"
#include "functions_common.h"

Cfunction set_builtin_function_hilbenvlope(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 2, ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
#ifdef FLOAT
CSignal __hilbert(float* buf, unsigned int len, void* pargin, void* pargout)
{//This calculates the imaginary part of the analytic signal (Hilbert) transform and updates buf with it.
//To get the envelope, get the sqrt of x*x (original signal) plus hilbertx*hilbertx

	CSignal output;

	fftwf_complex* in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * len);
	fftwf_complex* mid = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * len);
	fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * len);
	memset(in, 0, len * sizeof(fftwf_complex));
	memset(mid, 0, len * sizeof(fftwf_complex));
	memset(out, 0, len * sizeof(fftwf_complex));

	// FFT
	for (unsigned int k = 0; k < len; k++) in[k][0] = buf[k];
	fftwf_plan p1 = fftwf_plan_dft_1d(len, in, mid, FFTW_FORWARD, FFTW_ESTIMATE);
	fftwf_execute(p1);

	memset(in, 0, len * sizeof(fftwf_complex));
	// converting halfcomplex array to complex array
	int half = len / 2 + len % 2;
	in[0][0] = mid[0][0];

	for (int k(1); k < half; ++k)
	{
		in[k][0] = 2 * mid[k][0];
		in[k][1] = 2 * mid[k][1];
	}

	if (len % 2 == 0)	// len is even
		in[half][0] = mid[half][0];
	// leave the rest zero

	// iFFT
	fftwf_plan p2 = fftwf_plan_dft_1d(len, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftwf_execute(p2);

	fftwf_destroy_plan(p1);
	fftwf_destroy_plan(p2);

	for (unsigned int k = 0; k < len; k++)
	{// scale back down since the resulting array is scaled by len.
//		buf[k+id0] = out[k][0] / len;	// This line fills buf with the identical signal with the input
		buf[k] = out[k][1] / len;	// This line is about the imaginary part of the analytic signal.
	}
	fftwf_free(in);
	fftwf_free(out);
	output.UpdateBuffer(len);
	memcpy(output.buf, buf, len * sizeof(float));
	return output;
}
#else
CSignal __hilbert(double* buf, unsigned int len, void* pargin, void* pargout)
{//This calculates the imaginary part of the analytic signal (Hilbert) transform and updates buf with it.
//To get the envelope, get the sqrt of x*x (original signal) plus hilbertx*hilbertx

	CSignal output;

	fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * len);
	fftw_complex* mid = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * len);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * len);
	memset(in, 0, len * sizeof(fftw_complex));
	memset(mid, 0, len * sizeof(fftw_complex));
	memset(out, 0, len * sizeof(fftw_complex));

	// FFT
	for (unsigned int k = 0; k < len; k++) in[k][0] = buf[k];
	fftw_plan p1 = fftw_plan_dft_1d(len, in, mid, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p1);

	memset(in, 0, len * sizeof(fftw_complex));
	// converting halfcomplex array to complex array
	int half = len / 2 + len % 2;
	in[0][0] = mid[0][0];

	for (int k(1); k < half; ++k)
	{
		in[k][0] = 2 * mid[k][0];
		in[k][1] = 2 * mid[k][1];
	}

	if (len % 2 == 0)	// len is even
		in[half][0] = mid[half][0];
	// leave the rest zero

	// iFFT
	fftw_plan p2 = fftw_plan_dft_1d(len, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p2);

	fftw_destroy_plan(p1);
	fftw_destroy_plan(p2);

	for (unsigned int k = 0; k < len; k++)
	{// scale back down since the resulting array is scaled by len.
//		buf[k+id0] = out[k][0] / len;	// This line fills buf with the identical signal with the input
		buf[k] = out[k][1] / len;	// This line is about the imaginary part of the analytic signal.
	}
	fftw_free(in);
	fftw_free(out);
	output.UpdateBuffer(len);
	memcpy(output.buf, buf, len * sizeof(double));
	return output;
}
#endif

CSignal __envelope(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{
	int fs = *(int*)pargin;
	CSignal copy(fs), copy2(fs), out(fs);
	copy.UpdateBuffer(len);
	copy2.UpdateBuffer(len);
	memcpy(copy.buf, buf, len * sizeof(auxtype));
	memcpy(copy2.buf, buf, len * sizeof(auxtype));
	__hilbert(copy.buf, len, NULL, NULL); // making it a phase-shifted version
	copy2.SetComplex();
	for (unsigned int k = 0; k < len; k++) copy2.buf[2 * k + 1] = copy.buf[k];
	out.UpdateBuffer(len);
	for (unsigned int k = 0; k < len; k++) out.buf[k] = abs(copy2.cbuf[k]);
	return out;
}

void _hilbenvlope(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "hilbert")
		past->Sig = past->Sig.evoke_getsig2(__hilbert, NULL);
	else if (fname == "envelope")
	{
		int fs[1] = { past->Sig.GetFs() };
		past->Sig = past->Sig.evoke_getsig2(__envelope, (void*)&fs);
	}
}
