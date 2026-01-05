#include "functions_common.h"
#include <sstream>

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

Cfunction set_builtin_function_rend(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", "format"};
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 1, TYPEBIT_BYTE + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
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

template <class T>
static T read_unaligned(const uint8_t*& p) {
	T v;
	std::memcpy(&v, p, sizeof(T));
	p += sizeof(T);
	return v;
}

static std::vector<double> read_values_as_double(const uint8_t*& p,
	const std::string& type,
	size_t byte_count)
{
	std::vector<double> out;

	auto require_multiple = [&](size_t sz) {
		if (byte_count % sz != 0) {
			throw std::runtime_error(
				"Byte count not aligned with element size for type '" + type + "'");
		}
		return byte_count / sz;
	};

	if (type == "int8") {
		size_t n = require_multiple(sizeof(int8_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<int8_t>(p)));
	}
	else if (type == "int16") {
		size_t n = require_multiple(sizeof(int16_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<int16_t>(p)));
	}
	else if (type == "int32") {
		size_t n = require_multiple(sizeof(int32_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<int32_t>(p)));
	}
	else if (type == "int64") {
		size_t n = require_multiple(sizeof(int64_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<int64_t>(p)));
	}
	else if (type == "uint8") {
		size_t n = require_multiple(sizeof(uint8_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<uint8_t>(p)));
	}
	else if (type == "uint16") {
		size_t n = require_multiple(sizeof(uint16_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<uint16_t>(p)));
	}
	else if (type == "uint32") {
		size_t n = require_multiple(sizeof(uint32_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<uint32_t>(p)));
	}
	else if (type == "uint64") {
		size_t n = require_multiple(sizeof(uint64_t));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<uint64_t>(p)));
	}
	else if (type == "float") {
		size_t n = require_multiple(sizeof(float));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(static_cast<double>(read_unaligned<float>(p)));
	}
	else if (type == "double") {
		size_t n = require_multiple(sizeof(double));
		out.reserve(n);
		for (size_t i = 0; i < n; ++i)
			out.push_back(read_unaligned<double>(p));
	}
	else {
		throw std::invalid_argument(
			"read_values_as_double: unsupported type '" + type + "'");
	}

	return out;
}

void _rend(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string estr;
	CVar arg = args.front();
	string param = arg.str();
	if (param == "str") {
		//most likely the argument is from fread("byte") and the buffer may be big enough, but just for the safety
		past->Sig.bufType = 'S';
		past->Sig.UpdateBuffer(past->Sig.nSamples+1);
	}
	else {
		try {
			const uint8_t* p = reinterpret_cast<const uint8_t*>(past->Sig.strbuf);
			vector<double> content_read = read_values_as_double(p, param, past->Sig.nSamples);
			CVar out(1);
			out.UpdateBuffer(content_read.size());
			uint64_t k = 0;
			for (auto v : content_read)
				out.buf[k++] = v;
			past->Sig = out;
		}
		catch (const std::runtime_error& e) {
			estr = e.what();
			throw exception_etc(past, pnode, estr).raise();
		}
		catch (const std::invalid_argument& e) {
			estr = e.what();
			estr += "accepted types are int8, int16, int32, uint8, uint16, uint32, float, double, or str.";
			throw exception_etc(past, pnode, estr).raise();
		}
	}
}