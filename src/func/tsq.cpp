#include "functions_common.h"

Cfunction set_builtin_function_tseqget(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "tseq" };
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsTEMPORALG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::IsAUDIOG, }; // No audio object for tseq functions
	ft.reject.push_back(prohibitFunc);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_tseqset(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "tseq", "object to set"};
	vector<string> desc_arg_opt = { "" };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_NULL }; // not used
	ft.allowed_arg_types.push_back(allowedTypes1); // not used (but need this line)
	set<pfunc_typecheck> allowedCheckFunc = { Cfunction::IsTEMPORALG };
	ft.qualify.push_back(allowedCheckFunc);
	set<pfunc_typecheck> allowedCheckFunc1 = { Cfunction::IsVectorG, Cfunction::Is2DG };
	ft.qualify.push_back(allowedCheckFunc1);
	set<pfunc_typecheck> prohibitFunc = { Cfunction::IsAUDIOG, }; // No audio object for tseq functions
	ft.reject.push_back(prohibitFunc);
	set<pfunc_typecheck> prohibitFunc1 = { Cfunction::AllFalse, }; // prohibit false (none)
	ft.reject.push_back(prohibitFunc1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _tseqget(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!strcmp(pnode->str, "tsq_getvalues")) {
		int k = 0, nItems = past->Sig.CountChains();
		CTimeSeries out(1);
		out.UpdateBuffer(nItems * past->Sig.nSamples);
		//	out.nGroups = nItems;
		for (CTimeSeries* p = &past->Sig; p; p = p->chain)
			memmove(out.buf + k++ * p->nSamples, p->buf, sizeof(auxtype) * p->nSamples); // assuming that p->nSamples is always the same
		past->Sig = out;
	}
	else if (!strcmp(pnode->str, "tsq_gettimes")) { 
		//get the item count; i.e., the number of chains
		int nItems = past->Sig.CountChains();
		auxtype* dbuf = new auxtype[nItems];
		int k = 0;
		int nChains = 1;
		bool relative = past->Sig.GetFs() == 0;
		for (CTimeSeries* q = &past->Sig; q; q = q->chain)
			dbuf[k++] = q->tmark;
		past->Sig.Reset(1);
		past->Sig.UpdateBuffer(nItems * nChains);
		past->Sig.nGroups = 1;// nChains;
		memmove(past->Sig.buf, dbuf, sizeof(auxtype) * past->Sig.nSamples);
		delete[] dbuf;
	}
	else { // tsq_isrel
		int type = past->Sig.GetType();
		bool res = past->Sig.GetFs() == 0;
		auxtype dres = res ? 1. : 0.;
		past->Sig.SetValue(dres);
		past->Sig.MakeLogical();
	}
}

void _tseqset(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	// args.front() : new values or times
	int nItems = past->Sig.CountChains();
	if (!strcmp(pnode->str, "tsq_setvalues")) {
		if (args.front().nSamples != nItems)
			throw exception_func(*past, pnode, "Argument vector must have the same number of groups as the TSEQ length.", pnode->str).raise();
		if (Cfunction::IsVectorG(args.front().type()))
			throw exception_func(*past, pnode, "Argument must be a matrix (Use colon).", pnode->str).raise();
		int id = 0;
		for (CTimeSeries* p = &past->Sig; p; p = p->chain)
		{
			p->UpdateBuffer(args.front().Len());
			memmove(p->buf, args.front().buf + id++ * args.front().Len(), sizeof(auxtype) * args.front().Len());
		}
	}
	else { // "tsq_settimes"
		if (args.front().nSamples != nItems)
			throw exception_func(*past, pnode, "Argument vector must have the same number of elements as the TSEQ.", pnode->str).raise();
		int id = 0;
		for (CTimeSeries* p = &past->Sig; p; p = p->chain)
		{
			p->tmark = args.front().buf[id++];
			if (args.front().GetFs() == 0) p->SetFs(0);
		}
	}
	const AstNode* arg0 = arg0node(pnode, past->node);
	CVar* psig = past->GetVariable(arg0->str, arg0);
	if (!psig) {
		throw exception_func(*past, pnode, "Must be a variable", pnode->str, 1).raise();
	}
	past->SetVar(arg0->str, &past->Sig);
}
