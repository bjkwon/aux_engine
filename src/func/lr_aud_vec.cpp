#include "functions_common.h"

Cfunction set_builtin_function_audio(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "vector/matrix" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 2, 3 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_vector(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0, 1, 2, AUDIO_TYPES_1D, TYPEBIT_TEMPO_ONE + 1, TYPEBIT_TEMPO_ONE + 3, TYPEBIT_TEMPO_CHAINS +1, TYPEBIT_MULTICHANS + 1, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_ONE + 3, TYPEBIT_MULTICHANS + TYPEBIT_TEMPO_CHAINS + 1, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_leftright(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_obj" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsSTEREOG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _audio(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	switch (past->Sig.nGroups)
	{
	case 1:
		past->Sig.SetFs(past->GetFs());
		break;
	case 2:
	{
		past->Sig.SetFs(past->GetFs());
		past->Sig.nSamples /= 2;
		past->Sig.nGroups = 1;
		CSignals nextgh(past->Sig.GetFs());
		nextgh <= past->Sig;
		nextgh.buf += past->Sig.nSamples;
		past->Sig.SetNextChan(nextgh);
	}
	break;
	default:
		exception_func(*past, pnode, "Cannot apply to a matrix with rows > 2.", "audio").raise();
		break;
	}
}

static void chain2vector(CTimeSeries* psig)
{
	int count = 1;
	CTimeSeries* p = psig->chain;
	for (; p; p = p->chain) {
		count++;
	}
	psig->UpdateBuffer(count);
	p = psig->chain;
	for (int k = 1; k < count; k++, p = p->chain)
		psig->buf[k] = p->buf[0];
	delete psig->chain;
	psig->chain = NULL;
}

static void vector_mono(CTimeSeries* psig)
{
	auto tp = ((CSignals*)psig)->type();
	if ((tp & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 1) {
		chain2vector(psig);
	}
	psig->SetFs(1);
	psig->snap = 0;
	psig->tmark = 0.;
}

void _vector(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "vector") {
		vector_mono(&past->Sig);
		if (past->Sig.next)
		{
			vector_mono(past->Sig.next);
			auto len = past->Sig.nSamples;
			past->Sig.UpdateBuffer(len * 2);
			past->Sig.nGroups = 2;
			memcpy(past->Sig.logbuf + len * past->Sig.bufBlockSize, past->Sig.next->logbuf, len * past->Sig.bufBlockSize);
			delete past->Sig.next;
			past->Sig.next = NULL;
		}
	}
	else if (fname == "squeeze") {
		past->Sig.Squeeze();
	}
}

void _leftright(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string fname = pnode->str;
	if (fname == "left")
	{
		delete past->Sig.next;
		past->Sig.next = NULL;
	}
	else if (fname == "right")
	{
		if (past->Sig.next)
			past->Sig.bringnext();
	}
}