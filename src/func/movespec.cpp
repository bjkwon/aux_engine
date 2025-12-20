#include "functions_common.h"
#include "samplerate.h"

Cfunction set_builtin_function_movespec(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio obj", "arg2" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_resample(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio obj", "arg2" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, TYPEBIT_TEMPO_CHAINS + 1, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 1 };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void __movespec(auxtype* buf, uint64_t len, void* parg, void* parg2)
{
	auxtype shift = *(auxtype*)parg;
	auxtype fs = *((auxtype*)parg + 1);
	vector<auxtype> copy(buf, buf + len);
	//	Hilbert();
	auxtype t(0), grid(1.f / fs);
	const complex<auxtype> j(0.0, 1.0);
	complex<auxtype> datum;
	for (unsigned int k = 0; k < len; k++)
	{
		datum = (copy.data()[k] + buf[k] * j) * exp(j * shift * 2. * PI * t);
		buf[k] = real(datum);
		t += grid;
	}
}

void _movespec(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auxtype shift_fs[2] = { args[0].value(), (auxtype)past->Sig.GetFs() };
	past->Sig.evoke_modsig(__movespec, &shift_fs);
}

static int getSIH(int len, float r1, float r2, int* outlength)
{
	float _r1 = 1. / r1;
	float _r2 = 1. / r2;
	float ratio_mean = 2. / (1. / _r1 + 1. / _r2);
	int N = (int)round(len * ratio_mean);
	*outlength = N;
	float sum = 0;
	float ratio;
	for (int k = 0; k < N; k++)
	{
		ratio = _r1 + (_r2 - _r1) * k / N;
		sum += 1. / ratio;
	}
	float  leftover = len - sum;
	*outlength += (int)round(leftover * ratio);
	return (int)round(leftover);
}


CSignal __resample(const CSignal& base, void* pargin, void* pargout)
{
	CSignal out(base.GetFs());
	auto in = *(const vector<CVar>*)pargin;
	CVar arg = in.front();
	CSignals* pratio = (CSignals*)&arg;

	//This doesn't mean real "resampling" because this does not change fs.
	//pratio < 1 means generate more samples (interpolation)-->longer duration and lower pitch
	//pratio > 1 means downsample--> shorter duration and higher pitch
	//On return, pratio is set with the actual ratio (incoming size / result size) for each chain
	char errstr[256] = {};
	SRC_DATA conv;
	float* data_out = NULL;
	float* data_in = new float[base.nSamples];
	int errcode;
	SRC_STATE* handle = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &errcode);
	if (errcode)
	{
		CVar outarg = *(CVar*)pargout;
		outarg.SetString(src_strerror(errcode));
		return out;
	}
	for (unsigned int k = 0; k < base.nSamples; k++) data_in[k] = (float)base.buf[k];
	conv.data_in = data_in;
	auto tp = arg.type();
	if (ISSCALAR(tp))
	{
		conv.src_ratio = 1. / arg.value();
		conv.input_frames = base.nSamples;
		conv.output_frames = (long)(base.nSamples * conv.src_ratio + .5);
		conv.data_out = data_out = new float[conv.output_frames];
		conv.end_of_input = 1;
		errcode = src_process(handle, &conv);
		if (errcode)
		{
			CVar outarg = *(CVar*)pargout;
			outarg.SetString(src_strerror(errcode));
			return out;
		}
		out.UpdateBuffer(conv.output_frames);
		long k;
		for (k = 0; k < conv.output_frames_gen; k++)
			out.buf[k] = conv.data_out[k];
		for (k = conv.output_frames_gen; k < conv.output_frames; k++)
			out.buf[k] = 0;
	}
	else
	{
		int blockCount = 0;
		vector<auxtype> outbuffer;
		//inspect pratio to estimate the output length
		int cum = 0, cumID = 0;
		int fs = base.GetFs();
		for (CTimeSeries* p = pratio; p && p->chain; p = p->chain)
			cum += (int)((p->chain->tmark - p->tmark) * fs / 1000 * p->value());
		outbuffer.reserve(cum);
		int lastSize = 1, lastPt = 0;
		data_out = new float[lastSize];
		long inputSamplesLeft = (long)base.nSamples;
		int orgSampleCounts = 0;
		//assume that pratio time sequence is well prepared--
		for (CTimeSeries* p = pratio; p && p->chain; p = p->chain)
		{
			conv.end_of_input = 0;
			unsigned int i1, i2;
			auxtype ratio_mean;
			int inBuffersize, outBuffersize;
			if (p->value() == p->chain->value())
				src_set_ratio(handle, conv.src_ratio = ratio_mean = 1. / p->value());
			else
			{
				src_set_ratio(handle, 1. / p->value());
				conv.src_ratio = 1. / p->chain->value();
				ratio_mean = (2 * 1. / p->value() * 1. / p->chain->value() / (1. / p->value() + 1. / p->chain->value())); // harmonic mean
			}
			//current p covers from p->tmark to p->chain->tmark
			if (!p->chain->chain)
				conv.input_frames = inputSamplesLeft;
			else
			{
				//current p covers from p->tmark to p->chain->tmark
				i1 = (int)(p->tmark * fs / 1000);
				i2 = (int)(p->chain->tmark * fs / 1000);
				conv.input_frames = i2 - i1;
			}
			conv.output_frames = (long)(conv.input_frames * ratio_mean + .5); // when the begining and ending ratio is different, use the harmonic mean for the estimate.
			if (conv.output_frames > lastSize)
			{
				delete[] data_out;
				data_out = new float[lastSize = conv.output_frames + 20000];//reserve the buffer size big enough to avoid memory crash, but find out a better than this.... 3/20/2019
			}
			conv.data_out = data_out;
			int harmean;
			int out2 = getSIH(conv.input_frames, p->value(), p->chain->value(), &harmean);
			int harmean0 = harmean;
			int newlen = harmean + out2 / 2;
			errcode = src_process(handle, &conv);
			inBuffersize = conv.input_frames_used;
			if (errcode)
			{
				char errout[512];
				snprintf(errout, sizeof(errout), "Error in block %d--%s", blockCount++, src_strerror(errcode));
				pratio->SetString(errout);
				delete[] data_in;	delete[] data_out;
				return out;
			}
			outBuffersize = conv.output_frames_gen;
			for (int k = 0; k < conv.output_frames_gen; k++)
				outbuffer.push_back(data_out[k]);
			lastPt += conv.input_frames_used;
			if (p->chain->chain)
			{
				conv.data_in = &data_in[lastPt];
				inputSamplesLeft -= conv.input_frames_used;
			}
			while (conv.input_frames)
			{
				conv.src_ratio = 1. / p->chain->value();
				conv.data_in = &data_in[lastPt];
				conv.input_frames -= conv.input_frames_used;
				conv.end_of_input = conv.input_frames == 0 ? 1 : 0;
				errcode = src_process(handle, &conv);
				inBuffersize += conv.input_frames_used;
				outBuffersize += conv.output_frames_gen;
				for (int k = 0; k < conv.output_frames_gen; k++)
					outbuffer.push_back(data_out[k]);
				lastPt += conv.input_frames_used;
			}
			src_reset(handle);
			p->chain->tmark = p->tmark + 1000. / fs * outBuffersize;
		}
		out.UpdateBuffer((unsigned int)outbuffer.size());
		memcpy(out.buf, &outbuffer[0], sizeof(auxtype) * outbuffer.size());
	}
	src_delete(handle);
	if (data_in) delete[] data_in;
	if (data_out) delete[] data_out;
	return out;
}

void _resample(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar outarg;
	past->Sig = past->Sig.evoke_modsig2(__resample, (void*)&args, (void*)&outarg);

}
