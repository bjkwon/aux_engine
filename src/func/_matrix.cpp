#include "functions_common.h"

Cfunction set_builtin_function_group(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array", "nGroups",  };
	vector<string> desc_arg_opt = { "overlap=0" };
	vector<CVar> default_arg = { CVar(0.f) };
	set<uint16_t> allowedTypes1 = { 1, 2, TYPEBIT_SIZE1 + 2, AUDIO_TYPES_1D };
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_ungroup(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array" };
	vector<string> desc_arg_opt = { "overlap=0" };
	vector<CVar> default_arg = { CVar(0.f) };
	set<uint16_t> allowedTypes1 = { 1, 3, AUDIO_TYPES_2D };
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_buffer(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array", "blocklen" };
	vector<string> desc_arg_opt = { "overlap"};
	vector<CVar> default_arg = { CVar(0.f) };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

static CTimeSeries group_buffer(const auxtype* buf, uint64_t len, int nGroups, int overlap, int fs)
{
	CTimeSeries out(fs);
	const uint64_t numerator = len + (uint64_t)overlap * (uint64_t)(nGroups - 1);
	const uint64_t groupLen = (numerator + (uint64_t)nGroups - 1) / (uint64_t)nGroups;
	out.UpdateBuffer(groupLen * (uint64_t)nGroups);
	out.nGroups = (unsigned int)nGroups;
	for (int k = 0; k < nGroups; k++)
	{
		const uint64_t srcOffset = (uint64_t)k * (groupLen - (uint64_t)overlap);
		const uint64_t nCopy = (srcOffset < len) ? min(groupLen, len - srcOffset) : 0;
		if (nCopy > 0)
			memcpy(out.buf + (uint64_t)k * groupLen, buf + srcOffset, nCopy * sizeof(auxtype));
	}
	return out;
}

CSignal __group(auxtype* buf, unsigned int len, void* pargin, void* pargout)
{
	(void)pargout;
	auto in = *(vector<CVar>*)pargin;
	int nGroups = (int)in[0].value();
	int overlap = (int)in[1].value();
	int fs = (int)in[2].value();
	return group_buffer(buf, len, nGroups, overlap, fs);
}

void _group(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auxtype val = args[0].value();
	if (val != (auxtype)(int)val)
		exception_func(*past, pnode, "argument must be an integer.", "group", 2).raise();
	if (args[1].value() != (auxtype)(int)args[1].value())
		exception_func(*past, pnode, "argument must be an integer.", "group", 3).raise();
	int nGroups = (int)val;
	int overlap = (int)args[1].value();
	if (nGroups < 1)
		exception_func(*past, pnode, "The row/group count must be a positive integer.", "group", 2).raise();
	if (overlap < 0)
		exception_func(*past, pnode, "Overlap must be a non-negative integer.", "group", 3).raise();
	auto tp = past->Sig.type();
	const uint64_t paddedNumerator = past->Sig.nSamples + (uint64_t)overlap * (uint64_t)(nGroups - 1);
	const uint64_t paddedNCols = (paddedNumerator + (uint64_t)nGroups - 1) / (uint64_t)nGroups;
	if (overlap > 0 && (uint64_t)overlap > paddedNCols)
		exception_func(*past, pnode, "Overlap cannot exceed the size on the row/group.", "group", 3).raise();
	if (ISSCALAR(tp) || ISVECTOR(tp))
	{
		const uint64_t numerator = past->Sig.nSamples + (uint64_t)overlap * (uint64_t)(nGroups - 1);
		if (numerator % (uint64_t)nGroups != 0)
			exception_func(*past, pnode, "The length of array must be compatible with the requested row count and overlap.", "group").raise();
		if (overlap > 0)
			past->Sig = CSignals(group_buffer(past->Sig.buf, past->Sig.nSamples, nGroups, overlap, past->Sig.GetFs()));
	}
	else
	{
		vector<CVar> argin(args);
		argin.push_back(CVar((auxtype)past->Sig.GetFs()));
		past->Sig = past->Sig.evoke_getsig2(__group, (void*)&argin);
		//taking care of nGroups for chains
		if (past->Sig.chain)
			past->Sig.setsnap(0);
		for (auto p = past->Sig.chain; p; p = p->chain)
			p->nGroups = (int)val;
		//taking care of nGroups for next
		for (auto p = past->Sig.next; p; p = p->next)
			p->nGroups = (int)val;
	}
	//
	past->Sig.nGroups = (int)nGroups;
	if (!strcmp(pnode->str, "matrix"))
		past->statusMsg = "(NOTE) matrix() is superseded by group() and will be removed in future versions.";
}
//
//void _buffer(AuxScope* past, const AstNode* pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	past->blockCell(pnode, past->Sig);
//	past->blockString(pnode, past->Sig);
//	//	past->blockGO(pnode, past->Sig); // need this 7/11/2020
//	CVar second, third;
//	try {
//		AuxScope tp(past);
//		second = tp.Compute(p);
//		if (p->next) third = tp.Compute(p->next);
//	}
//	catch (const CAstException& e) {
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc(e.getErrMsg().c_str());
//	}
//	if (!second.IsScalar())
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc("2nd argument must be a scalar.");
//	auxtype _overlap, _blocklen = second.value();
//	if (_blocklen != (auxtype)(int)_blocklen)
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc("2nd argument must be an integer.");
//	if (third.type() == TYPEBIT_NULL) _overlap = 0.;
//	else
//	{
//		if (!third.IsScalar())
//			throw CAstException(FUNC_SYNTAX, *past, pnode).proc("3rd argument must be a scalar.");
//		_overlap = third.value();
//		if (_overlap != (auxtype)(int)_overlap)
//			throw CAstException(FUNC_SYNTAX, *past, pnode).proc("3rd argument must be an integer.");
//	}
//	unsigned int nGroups;
//	auto type = past->Sig.type();
//	auto blocklen = (unsigned int)_blocklen;
//	auto overlap = (unsigned int)_overlap;
//	if (past->Sig.IsAudio() || (type & 0x0003) <= 2) // audio, vector or constant
//	{
//		if (past->Sig.chain)
//			throw CAstException(FUNC_SYNTAX, *past, pnode).proc("To make a matrix from an audio signal, null portions should be filled with zeros. Call contig().");
//		nGroups = (unsigned int)ceil(past->Sig.nSamples / (_blocklen - _overlap));
//		unsigned int newlength = nGroups * (unsigned int)blocklen;
//		CVar out(past->Sig.GetFs());
//		CVar *pout;
//		auto nSamples0 = past->Sig.nSamples;
//		if (newlength == past->Sig.nSamples)
//		{
//			past->Sig.nGroups = nGroups;
//			return;
//		}
//		else if (newlength > past->Sig.nSamples)
//		{
//			out.UpdateBuffer(newlength);
//			pout = &out;
//		}
//		else
//		{
//			pout = &past->Sig;
//			pout->nSamples = newlength;
//		}
//		unsigned int m = 0;
//		for (; m < nGroups-1; m++)
//			memcpy(pout->buf + m * blocklen, past->Sig.buf + m * (blocklen - overlap), sizeof(past->Sig.buf) * blocklen);
//		auto k = m * blocklen;
//		//auto n = k - m * overlap; // the index for past->Sig.buf
//		for (; k < nSamples0 + m * overlap; k++)
//			pout->buf[k] = past->Sig.buf[k - m * overlap];
//		pout->nGroups = nGroups;
//		if (newlength > past->Sig.nSamples) 
//			past->Sig = out;
//	}
//	else
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc("Vector or audio signals required.");
//}

void _ungroup(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auxtype _overlap = args[0].value();
	if (_overlap != (auxtype)(int)_overlap)
		exception_func(*past, pnode, "argument must be an integer.", "group", 1).raise();
	auto overlap = (int)_overlap;
	if (_overlap > 0. && overlap > past->Sig.Len())
		exception_func(*past, pnode, "Overlap cannot exceed the size on the row/group.", "group", 2).raise();
	CVar out(past->Sig.GetFs());
	auto blocklen = past->Sig.Len();
	out.UpdateBuffer(past->Sig.nSamples - (uint64_t)overlap * (past->Sig.nGroups - 1));
	uint64_t id = 0;
	for (unsigned int k = 0, id2 = 0; k < past->Sig.nGroups; k++)
	{
		const uint64_t skip = k == 0 ? 0 : (uint64_t)overlap;
		const uint64_t nMoves = blocklen - skip;
		memmove(out.logbuf + id * past->Sig.bufBlockSize, past->Sig.logbuf + (id2 + skip) * past->Sig.bufBlockSize, past->Sig.bufBlockSize * nMoves);
		id += nMoves;
		id2 += blocklen;
	}
	past->Sig = out;
}
