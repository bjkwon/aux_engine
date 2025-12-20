#include "functions_common.h"

Cfunction set_builtin_function_veq(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", "object", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 0xFFFF }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_datatype(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { 0xFFFF }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _datatype(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	uint16_t out;
	if ((out = past->Sig.type()) == 0xffff)
		throw exception_func(*past, pnode, "this particular data type has not been ready to handle.", "otype()", 1).raise();
	past->Sig.SetValue((double)out);
}

void _veq(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	AuxScope br(past);
	CVar arg1 = past->Sig;
	CVar arg2 = args[0];
	uint16_t type1 = arg1.type();
	uint16_t type2 = args[0].type();
	try {
		// throw 0 for false
		if (type1 != type2) {
			uint16_t diff = type1 > type2 ? type1 - type2 : type2 - type1;
			if (diff != TYPEBIT_COMPLEX) throw 0;
			if (type1 & TYPEBIT_COMPLEX) {
				for (unsigned k = 0; k < arg1.nSamples; k++)
					if (imag(arg1.cbuf[k]) != 0.) throw 0;
				arg1.SetReal();
			}
			else if (type2 & TYPEBIT_COMPLEX) {
				for (unsigned k = 0; k < arg2.nSamples; k++)
					if (imag(arg2.cbuf[k]) != 0.) throw 0;
				arg2.SetReal();
			}
		}
		if (arg1.nSamples != arg2.nSamples) throw 0;
		else if (type1 & 0x2000) // GO
		{
			if (arg1.value() != arg2.value()) throw 0;
		}
		else
		{
			if (arg1.bufBlockSize == sizeof(auxtype))
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.buf[k] != arg2.buf[k]) throw 0;
				}
			else if (arg1.bufBlockSize == 2 * sizeof(auxtype))
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.cbuf[k] != arg2.cbuf[k]) throw 0;
				}
			else
				for (unsigned k = 0; k < arg1.nSamples; k++)
				{
					if (arg1.logbuf[k] != arg2.logbuf[k]) throw 0;
				}
		}
		past->Sig.Reset(1);
		past->Sig.MakeLogical();
		past->Sig.UpdateBuffer(1);
		past->Sig.logbuf[0] = true;
		return;
	}
	catch (int k)
	{
		//k should be 0 and it doesn't matter what k is.
		k = 0; // just to avoid warning C4101
		past->Sig.Reset(1);
		past->Sig.MakeLogical();
		past->Sig.UpdateBuffer(1);
		past->Sig.logbuf[0] = false;
		return;
	}
}

