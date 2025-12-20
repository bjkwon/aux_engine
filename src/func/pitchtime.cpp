#include "functions_common.h"

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
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj", "factor"};
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, 2, TYPEBIT_TEMPO_CHAINS, TYPEBIT_TEMPO_CHAINS + 1};
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

int timeargument(CVar& ratio, double audioDur, int fs)
{
	CTimeSeries* pratio = (CTimeSeries*)&ratio;
	if (pratio->fs == 0) // relative
	{
		for (CTimeSeries* p = pratio; p; p = p->chain)
		{
			p->tmark *= audioDur;
			p->SetFs(fs);
		}
	}
	//If the first tmark is not 0, make one with 0 tmark and bring the value at zero
	if (pratio->tmark != 0.)
	{
		CTimeSeries newParam(fs);
		newParam.tmark = 0.;
		newParam.SetValue(pratio->value());
		newParam.chain = new CTimeSeries;
		*newParam.chain = *pratio; // this way the copied version goes to chain
		*pratio = newParam;
	}
	//If the last tmark is not the end of the signal, make one with 0 tmark and bring the value at zero
	CTimeSeries* pLast = NULL;
	for (CTimeSeries* p = pratio; p; p = p->chain)
		if (!p->chain)
			pLast = p;
	if (pLast && fabs(pLast->tmark - audioDur) > 10.) // give a 10 milliseconds margin to the end edge time point.
	{
		CTimeSeries newParam(fs);
		newParam.tmark = audioDur;
		newParam.SetValue(pLast->value());
		pLast->chain = new CTimeSeries;
		*pLast->chain = newParam; // this way the copied version goes to chain
	}
	// reject 
	for (CTimeSeries* p = pratio; p; p = p->chain)
	{
		if (p->tmark < 0 || p->tmark > audioDur)
		{
			throw "Time arguments set out of range of the audio block.";
		}
	}
	return 1;
}

static inline int maxcc(double* x1, int len1, double* x2, int len2, int prevmaxid)
{
	const int len = len1 + len2 - 1;
	double* buffer = new double[len];
	for (int k = 0; k < len; k++)
	{
		double tp = 0.;
		for (int q, p = 0; p <= k && p < len1; p++)
		{
			q = k - p;
			int p2 = len1 - p - 1;
			if (p2 < len1 && q < len2)
				tp += x1[p2] * x2[q];
		}
		buffer[k] = tp;
	}
	CSignal temp(buffer + len2, len - 2 * len2 + 1);
	delete[] buffer;
	int maxid;
	temp._max(0, len - 2 * len2 + 1, &maxid);
	return (int)maxid;
}

static inline double harmonicmean(double x1, double x2)
{
	return 2 * x1 * x2 / (x1 + x2);
}

static inline double cal_ingrid(double prev, int id1, int hop, double ratio)
{
	double out = prev + hop / ratio;
	return out;
}

static inline double cal_harmonic_serise(int length, double r1, double r2)
{
	if (length == 1)
		return 1. / r1 + 1. / r2;
	double increment = (r2 - r1) / (length - 1);
	if (increment == 0.)
		return length / r1;
	double r = r1 - (r2 - r1) / (length - 1);
	double out = 0;
	int k = 0;
	for (; k < length; k++)
	{
		r += increment;
		out += 1. / r;
	}
	return out;
}

static inline vector<double> set_synHop_vector(int length, double r1, double r2)
{
	vector<double> out;
	if (length == 1)
	{
		out.push_back(1. / r1 + 1. / r2);
		return out;
	}
	double increment = (r2 - r1) / (length - 1);
	double r = r1 - (r2 - r1) / (length - 1);
	double last = 0;
	int k = 0;
	for (; k < length; k++)
	{
		r += increment;
		last += 1. / r;
		out.push_back(last);
	}
	return out;
}

static double adjust_hop(int length, double ratio, int hop, int& leftover)
{ // length: the length of the input array
  // ratio: ratio (such as 1.5 or 2.; for the case of dynamic ratios, put the harmonic mean
  // hop : [in] nominal hop (384 or 512)
// returns adjusted hop
	// new hop shall be (returned_hop)+1 for the first leftover, (returned_hop) for the rest (i.e., leftover+1 through L0
	int L0 = (int)(round((length * ratio)) / hop);
	int r0 = (int)(length * ratio) - L0 * hop;
	//now adjusting hop
	int a = r0 / L0;
	leftover = r0 - a * L0;
	hop += a;
	return (double)hop;
}

static inline int spreader(int nSamples, int nBlocks, int tol, double ratio1, double ratio2, int synHop, int* ingrid, int* outgrid)
{
	double hmean = harmonicmean(ratio1, ratio2);
	if (nBlocks <= 1)
	{
		ingrid[0] = nSamples + tol;
		outgrid[0] = (int)((ingrid[0] - ingrid[-1]) * hmean);
		return 1;
	}
	int winLen = min((int)nSamples / 5, 512);
	int leftover = 0;
	double hop, ratio, cum2 = 0., cum1 = (double)tol;
	double increment = 0.;
	hop = nSamples / cal_harmonic_serise(nBlocks, ratio1, ratio2);
	if (ratio1 != ratio2)
	{
		adjust_hop(nSamples, hmean, synHop, leftover); // hop computed earlier taken; output from adjust_hop is ignored
		ratio = ratio1 - (ratio2 - ratio1) / (nBlocks - 1);
		increment = (ratio2 - ratio1) / (nBlocks - 1);
	}
	else
	{
		ratio = ratio1;
		increment = 0;
		hop = adjust_hop(nSamples, ratio, synHop, leftover);
	}
	if (nBlocks == 1)
		nBlocks++;
	for (int k = 0; k < nBlocks; k++)
	{
		cum2 += hop; // must be round-up, and keep nBlocks the same and deduct if remainder is negative
		ratio += increment;
		double in_diff = hop / ratio;
		cum1 += in_diff;
		if (leftover > 0 && k < leftover)
		{
			cum2++;
			if (ratio1 == ratio2) cum1 += 1 / hmean;
		}
		else if (leftover > 0 && k - leftover >= nBlocks)
		{
			cum2--;
			if (ratio1 == ratio2) cum1 -= 1 / hmean;
		}
		outgrid[k] = (int)cum2;
		ingrid[k] = (int)round(cum1);
	}
	return nBlocks;
}

static inline int set_time_grids(bool lengthadjust, int id1, int id2, double ratio1, double ratio2, int synHop, int* ingrid, int* outgrid, int outgridoffset)
{ // outgrid[0] is always 0
	if (lengthadjust) ratio2 = ratio1;
	double lastInGrid = (double)id1;
	int blocksizeIn = id2 - id1;
	int cumOutTP = 0;
	double harmean = harmonicmean(ratio1, ratio2);
	int nBlocks = (int)((id2 - id1) * harmean / synHop); // this should not be round to make consistent with L0 in adjust_hop()
	double* _in = new double[nBlocks + 50];
	double* _out = new double[nBlocks + 50];
	_in[0] = (double)id1;
	_out[0] = 0.;
	nBlocks = spreader(id2 - id1, nBlocks, id1, ratio1, ratio2, synHop, ingrid, outgrid);
	if (outgridoffset > 0)
		for (int k = 0; k < nBlocks; k++)
			outgrid[k] += outgridoffset;
	delete[] _in;
	delete[] _out;
	return nBlocks;
}

static inline void stretch(bool nostretch, unsigned int nSamples, double* pout, double* overlapWind, const CSignal& input2,
	int winLen, int synHop, size_t blockBegin, size_t blockEnd, int* ingr, int* outgr,
	int& targetSize, int& nextOutIndex, int& del, size_t gridsize)
{
	winLen = 2 * (outgr[blockBegin + 1] - outgr[blockBegin]);
	double* wind = new double[winLen];
	for (int k = 0; k < winLen; k++)
		wind[k] = .5 * (1 - cos(2.0 * PI * k / (winLen - 1.0))); //hanning

// timestretch_log.py #0
	const int winLenHalf = (int)(winLen / 2. + .5);
	int tolerance = ingr[0];
	int lastInPoint = ingr[blockEnd] + synHop;
	// timestretch_log.py #1
	int xid0, yid0;
	// timestretch_log.py #2
	int nOverlap2 = 0;
	nextOutIndex = 0;
	int len1 = winLen + 2 * tolerance;
	int maxid = -1;
	if (nostretch)
	{
		int _synHop = outgr[1] - outgr[0];
		int winLenHalf = winLen / 2;
		for (size_t m = blockBegin; m < blockEnd - 1; m++)
		{
			xid0 = ingr[m] + del;
			yid0 = outgr[m];
			int xid, yid, k = 0;
			for (; k < winLen; k++)
			{
				xid = xid0 + k;
				yid = yid0 + k;
				pout[yid] += input2.buf[xid] * wind[(k + winLenHalf) % winLen];
				overlapWind[yid] += wind[(k + winLenHalf) % winLen];
				pout[yid] += input2.buf[xid] * wind[k % winLen];
				overlapWind[yid] += wind[k % winLen];
			}
			nextOutIndex = yid0 + k;
			if (m == blockEnd - 2)
			{
				for (; k < winLen + winLenHalf; k++)
				{
					xid = xid0 + k;
					yid = yid0 + k;
					pout[yid] += input2.buf[xid] * wind[(k + winLenHalf) % winLen];
					overlapWind[yid] += wind[(k + winLenHalf) % winLen];
				}
				int _synHop = outgr[m + 1] - outgr[m];
				// This is crosscorrelation between the next input block including tolerance regions before & after
				// and "natural progression of the last copied input segment (from Jonathan Driedger)"
				int corrIDX1 = ingr[m + 1] - tolerance;
				int corrIDX2 = ingr[m] + _synHop + del;
				maxid = maxcc(&input2.buf[corrIDX1], len1, &input2.buf[corrIDX2], winLen, maxid);
				del = tolerance - maxid + 1;
			}
		}
	}
	else
	{
		for (size_t m = blockBegin; ; m++)
		{
			xid0 = ingr[m] + del;
			yid0 = outgr[m];
			int xid, yid, k = 0;
			for (; k < winLen; k++)
			{
				xid = xid0 + k;
				yid = yid0 + k;
				pout[yid] += input2.buf[xid] * wind[k];
				overlapWind[yid] += wind[k];
				if (blockEnd == gridsize && xid0 + k == lastInPoint - 1)
				{
					nOverlap2++;
					break;
				}
			}
			nextOutIndex = yid0 + k;
			//		if (m >= blockEnd-1) break;
			if (m == blockEnd)
				break;
			if (1) //(m < gridsize - 1)
			{
				int _synHop = outgr[m + 1] - outgr[m];
				// This is crosscorrelation between the next input block including tolerance regions before & after
				// and "natural progression of the last copied input segment (from Jonathan Driedger)"
				int corrIDX1 = ingr[m + 1] - tolerance;
				int corrIDX2 = ingr[m] + _synHop + del;
				// timestretch_log.py #3
				int len2 = winLen;
				if (m >= gridsize - 1)
				{
					if (len1 > (int)input2.nSamples - corrIDX1 + 1)
						len1 = input2.nSamples - corrIDX1;
					if (len1 < 1) break;
					if (len2 > (int)input2.nSamples - corrIDX2 + 1)
						len2 = input2.nSamples - corrIDX2;
					if (len2 < 1) break;
				}
				maxid = maxcc(&input2.buf[corrIDX1], len1, &input2.buf[corrIDX2], winLen, maxid);
				// timestretch_log.py #4
				del = tolerance - maxid + 1;
				if (m == gridsize - 1)
				{
					if (del > 0 || outgr[m + 1] > nextOutIndex)
						break;
				}
				// timestretch_log.py #5
			}
			// timestretch_log.py #6
		}
	}
	// timestretch_log.py #7
	targetSize = lastInPoint + outgr[blockEnd] - ingr[blockEnd] - synHop - outgr[blockBegin];
	if (blockEnd == gridsize && nextOutIndex < targetSize + outgr[blockBegin])
		nextOutIndex = targetSize + outgr[blockBegin] - 1;
	delete[] wind;
}


static CSignal __tsbase(const CSignal& base, void* parg, void* parg2)
{
	vector<CVar>* pargs = (vector<CVar>*)parg;
	CVar ratio = pargs->front();
	int fs = pargs->back().GetFs();
	std::map<std::string, CVar> opt;
	int winLen = min((int)base.nSamples / 5, 512);
	if (!ratio.strut.empty())
	{
		auto finder = ratio.strut.find("windowsize");
		if (finder != ratio.strut.end())
			winLen = (int)(*finder).second.value();
		if (winLen < 50 || winLen>4096 * 2)
			throw "windowsize must be >= 50 or <= 8192";
		opt = ratio.strut;
		ratio.strut.clear();
	}
	//pratio is either a constant or time sequence of scalars (not relative time)
	int synHop = winLen / 2;
	//	int tolerance = 1024;
	int tolerance = synHop;
	//pratio must be either real constant or T_SEQ then value at each time point is the ratio for that segment
	map<int, double> anchor;
	vector<int> vanchor;
	bool lengthadjust = false;
	bool returnTmarks = false;
	//Prepare--if time grid in tmarks for the task are too narrow, give them a rasonable margin.
	if (Cfunction::IsTEMPORALG(ratio.GetType()))
	{
		//temporary hack---used only in pitchscale()
		//treating as if CSIG_TSERIES with constant ratio in each interval
		if (ratio.tmark < 0)
			lengthadjust = true, ratio.tmark = 0.;

		for (CTimeSeries* p = &ratio; p; p = p->chain)
		{
			if (p->tmark < 0) // from pitchscale
			{
				vanchor.push_back(vanchor.back() + synHop);
				returnTmarks = true;
			}
			else
				vanchor.push_back((int)ceil(p->tmark * fs / 1000) + tolerance);
			anchor[vanchor.back()] = p->value();
		}
	}
	else
	{
		vanchor.push_back(tolerance);
		anchor[vanchor.back()] = ratio.value();
		vanchor.push_back(base.nSamples + tolerance);
		anchor[vanchor.back()] = ratio.value();
	}
	int outputLength;
	CTimeSeries* p;
	double cumTpointsY = 0.;
	unsigned int nTSLayers = 1;
	if (!Cfunction::IsTEMPORALG(ratio.GetType()))
	{
		outputLength = (int)ceil(ratio.value() * base.nSamples);
		cumTpointsY = (double)outputLength;
	}
	else
	{
		nTSLayers = ratio.CountChains();
		p = &ratio;
		for (unsigned int k = 0; k < nTSLayers - 1; k++, p = p->chain)
			cumTpointsY += harmonicmean(p->chain->value(), p->value()) * (p->chain->tmark - p->tmark) * fs / 1000;
		outputLength = (int)ceil(cumTpointsY);
	}
	int nBlocks = (int)ceil(cumTpointsY / synHop);
	int* ingrid = new int[nTSLayers * (nBlocks + 50)]; // give some margin for CSIG_TSERIES pratio
	int* outgrid = new int[nTSLayers * (nBlocks + 50)];
	ingrid[0] = tolerance;
	outgrid[0] = 0;
	vector<int> newtpoints; // new sample indices corresponding to the input indices for the ratio; begins with the second index (i.e., for a constant ratio, only the last index is shown)
	vector<size_t> chainIDX(1, 0);
	if (!Cfunction::IsTEMPORALG(ratio.GetType()))
	{
		nBlocks = set_time_grids(lengthadjust, tolerance, base.nSamples + tolerance, ratio.value(), ratio.value(), synHop, ingrid + 1, outgrid + 1, 0);
		chainIDX.push_back(nBlocks);
	}
	else
	{
		int nBlocksCum = 0;
		int last_outgridID = 0;
		for (auto it = vanchor.begin(); it != vanchor.end() - 1; it++)
		{
			nBlocks = set_time_grids(lengthadjust, *it, *(it + 1), anchor[*it], anchor[*(it + 1)], synHop,
				ingrid + last_outgridID + 1, outgrid + last_outgridID + 1, outgrid[last_outgridID]);
			last_outgridID += nBlocks;
			chainIDX.push_back(last_outgridID);
		}
	}
	CSignal out;
	out.tmark = 0; // make sure to do it
	out.UpdateBuffer(outgrid[chainIDX.back()] + synHop + 3 * winLen);
	int filledID = 0;
	int cumProcessed = 0, del = 0;
	const int nOutReserve = outgrid[chainIDX.back() - 1] + 2 * winLen;
	double* pout = new double[nOutReserve];
	double* overlapWind = new double[nOutReserve];
	memset(pout, 0, sizeof(double) * nOutReserve);
	memset(overlapWind, 0, sizeof(double) * nOutReserve);
	CSignal input(fs, synHop + tolerance + base.nSamples); // making the input with zero padding1 at the front, copy the current buffer
	memcpy(input.buf + synHop + tolerance, base.buf, base.nSamples * sizeof(auxtype));
	CSignal secondzeropadds(fs, 2 * tolerance);
	input += &secondzeropadds; // zero padding2
	int targetSize = 0, lastOutIndex = 0;
	p = &ratio;
	CTimeSeries* pchain = ratio.chain;
	for (auto it = chainIDX.begin() + 1; it != chainIDX.end(); it++)
	{
		int target;
		bool nostretch = p->value() == 1. && pchain->value() == 1;
		stretch(nostretch, ingrid[*it] - ingrid[0], pout, overlapWind,
			input, winLen, synHop, *(it - 1), *it, ingrid, outgrid,
			target, lastOutIndex, del, chainIDX.back());
		targetSize += target;
		if (pchain) pchain->tmark = targetSize * 1000. / fs;
		// remove zeropading at the beginning and ending. Begin at winLenHalf and take outputLength elements
		// memcpy is done to make the target size from the end of the actual end; i.e., lastOutIndex
		cumProcessed += ingrid[*it] - ingrid[*(it - 1)];
		filledID += target;
		p = pchain;
		if (pchain) {
			pchain = pchain->chain;
		}
	}
	for (int p = 0; p < targetSize; p++)
	{
		if (overlapWind[p] > .001)
			pout[p] /= overlapWind[p];
	}
	memcpy(out.buf, pout + lastOutIndex - targetSize + 1, sizeof(double) * targetSize);

	// timestretch_log.py #8
	out.SetFs(fs);
	out.nSamples = filledID;
	delete[] ingrid;
	delete[] outgrid;
	delete[] overlapWind;
	delete[] pout;
	if (returnTmarks)
	{
		CVar tmarks(1);
		tmarks.UpdateBuffer((unsigned int)vanchor.size());
		int jj = 0;
		for (auto it = vanchor.begin(); it != vanchor.end(); it++)
		{
			tmarks.buf[jj++] = (*it - vanchor.front()) * 1000. / fs;
		}
		ratio.strut["tmarks"] = tmarks;
	}
	return out;
}

static void __ps(auxtype* buf, uint64_t len, void* parg, void* parg2)
{

}

CSignal __resample(const CSignal& base, void* pargin, void* pargout); // from movespec.cpp; pargin is vector<CVar>

void _pitchtime(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	double audioDur = past->Sig.dur();
	int fs = past->Sig.GetFs();
	CVar sig = past->Sig;

	if (fname == "timestretch") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs); // factor argument
		vector<CVar> args;
		args.push_back(factor);
		args.push_back((CVar)fs);
		past->Sig = past->Sig.evoke_modsig2(__tsbase, &args);
	}
	else if (fname == "pitchscale") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs); // factor argument
		vector<CVar> args;
		args.push_back(factor);
		args.push_back((CVar)fs);
		past->Sig = past->Sig.evoke_modsig2(__tsbase, &args);
		past->Sig = past->Sig.evoke_modsig2(__resample, &args);
	}
	else if (fname == "respeed") {
		CVar factor = args.front();
		timeargument(factor, audioDur, fs); // factor argument
		vector<CVar> args;
		args.push_back(factor);
		past->Sig = past->Sig.evoke_modsig2(__resample, &args);
	}
}
