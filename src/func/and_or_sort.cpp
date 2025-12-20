#include "functions_common.h"

Cfunction set_builtin_function_andor(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 0, 1, 2, 3, TYPEBIT_LOGICAL + 1, TYPEBIT_LOGICAL + 2, TYPEBIT_LOGICAL + 3, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_andor2(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array", "array"  };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0, TYPEBIT_LOGICAL + 1, TYPEBIT_LOGICAL + 2, TYPEBIT_LOGICAL + 3, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_sort(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// The count of default_arg: same as desc_arg_opt.size()
	// Each of allowedTypes_n should list all allowed types
	// ft.allowed_arg_types.push_back(allowedTypes_n) should be called as many times as desc_arg_req.size() + desc_arg_opt.size()
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 0xFFFF };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}


Cfunction set_builtin_function_mostleast(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "array1", "array2" };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = {  };
	set<uint16_t> allowedTypes1 = { 1, 2,};
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _andor2(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{ // two arguments
	CVar arg0 = past->Sig;
	past->Sig.Reset(1);
	past->Sig.UpdateBuffer(min(arg0.nSamples, args.front().nSamples));
	past->Sig.MakeLogical();
	if (!strcmp(pnode->str, "and")) {
		for (uint64_t k = 0; k < min(arg0.nSamples, args.front().nSamples); k++)
			past->Sig.logbuf[k] = arg0.logbuf[k] && args.front().logbuf[k];
	}
	else
	{
		for (uint64_t k = 0; k < min(arg0.nSamples, args.front().nSamples); k++)
			past->Sig.logbuf[k] = arg0.logbuf[k] || args.front().logbuf[k];
	}
}
void _andor(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{ // single arguments
	auxtype res;
	if (!strcmp(pnode->str, "and")) {
		res = 1.f;
		if (past->Sig.IsLogical())
		{
			for (uint64_t k = 0; k < past->Sig.nSamples; k++)
				if (!past->Sig.logbuf[k]) { res = 0.; break; }
		}
		else
		{
			for (uint64_t k = 0; k < past->Sig.nSamples; k++)
				if (past->Sig.buf[k] == 0.) { res = 0.;	break; }
		}
	}
	else {
		res = 0.;
		if (past->Sig.IsLogical())
		{
			for (uint64_t k = 0; k < past->Sig.nSamples; k++)
				if (past->Sig.logbuf[k]) { res = 1.f; break; }
		}
		else
		{
			for (uint64_t k = 0; k < past->Sig.nSamples; k++)
				if (past->Sig.buf[k] != 0.) { res = 1.f;	break; }
		}
	}
	past->Sig.SetValue(res);
	past->Sig.MakeLogical();
}

template <class T>
int dcomp(const void* arg1, const void* arg2)
{
	if (*(T*)arg1 > *(T*)arg2)	return 1;
	else if (*(T*)arg1 == *(T*)arg2) return 0;
	else	return -1;
}

template <class T>
int dcompR(const void* arg1, const void* arg2)
{
	if (*(T*)arg1 < *(T*)arg2)	return 1;
	else if (*(T*)arg1 == *(T*)arg2) return 0;
	else	return -1;
}

static void __sort(auxtype* buf, uint64_t len, void* parg, void* parg2)
{
	int8_t bufBlockSize = *(int8_t*)parg;
	int8_t order = *(int8_t*)parg2;
	if (bufBlockSize == sizeof(auxtype))
	{
		if (order > 0)
			qsort((void*)buf, len, bufBlockSize, dcomp<auxtype>);
		else
			qsort((void*)buf, len, bufBlockSize, dcompR<auxtype>);
	}
	else if (bufBlockSize == 1)
	{
		if (order > 0)
			qsort((void*)buf, len, bufBlockSize, dcomp<unsigned char>);
		else
			qsort((void*)buf, len, bufBlockSize, dcompR<unsigned char>);
	}
}

void _sort(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int8_t order = 1;
	int8_t bufBlockSize = past->Sig.bufBlockSize;
	past->Sig.evoke_modsig(__sort, &bufBlockSize, &order);
}

static void __atmost(auxtype* buf, uint64_t len, void* parg, void* parg2)
{
	CVar *param = (CVar*)parg;
	if (param->IsScalar()) {
		auxtype limit = param->value();
		for (uint64_t k = 0; k < len; k++)
			if (buf[k] > limit) buf[k] = limit;
	}
	else {
		for (uint64_t k = 0; k < len; k++)
			if (buf[k] > param->buf[k]) buf[k] = param->buf[k];
	}
}

static void __atleast(auxtype* buf, uint64_t len, void* parg, void* parg2)
{
	CVar *param = (CVar*)parg;
	if (param->IsScalar()) {
		auxtype limit = param->value();
		for (uint64_t k = 0; k < len; k++)
			if (buf[k] < limit) buf[k] = limit;
	}
	else {
		for (uint64_t k = 0; k < len; k++)
			if (buf[k] < param->buf[k]) buf[k] = param->buf[k];
	}
}

void _mostleast(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	uint64_t len = args.front().nSamples;
	if (len > 1 && len != past->Sig.nSamples)
		throw exception_func(*past, pnode, "must be a scalar or array with the same length of arg1", pnode->str, 2).raise();
	if (!strcmp(pnode->str, "atmost"))
		past->Sig.evoke_modsig(__atmost, (void*)&args.front());
	else
		past->Sig.evoke_modsig(__atleast, (void*)&args.front());
}


