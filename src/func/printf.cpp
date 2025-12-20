#include "functions_common.h"
#include <regex>
#include <iostream>

extern map<double, FILE*> file_ids;

Cfunction set_builtin_function_printf(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "string format", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 0xFFFF, }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = -1; // no limit in output arg count
	return ft;
}

Cfunction set_builtin_function_fprintf(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID", "string format", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2};
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	set<uint16_t> allowedTypes3 = { 0xFFFF, }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes3);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = -1; // no limit in output arg count
	return ft;
}

static void processEscapes(string& str)
{
	size_t pos;
	for (size_t start = 0; (pos = str.find('\\', start)) != string::npos; start = pos + 1)
		switch (str[pos + 1]) {
		case 'n':
			str.replace(pos, 2, "\n");
			break;
		case 't':
			str.replace(pos, 2, "\t");
			break;
		default:
			str.erase(pos, 1);
		}
}

int gen_formatted_output(string &output, const string &prefixstr, string formatstr, CVar obj)
{
	output += prefixstr;
	vector<char> forconverted(100);
	auto tp = obj.type();
	char outbuffer[256], buffer[256];
	strcpy(buffer, formatstr.c_str());
	if (ISSTRING(tp))
		sprintf(outbuffer, buffer, obj.str().c_str());
	else if (ISSCALAR(tp))
	{
		if (*(formatstr.end() - 1) == 's')
			return -1;
		else if (*(formatstr.end()-1)=='d' || *(formatstr.end() - 1) == 'i' || 
			*(formatstr.end() - 1) == 'x' || *(formatstr.end() - 1) == 'X')
			sprintf(outbuffer, buffer, (int)obj.value());
		else
			sprintf(outbuffer, buffer, obj.value());
	}
	output += outbuffer;
	return 1;
}

void __printf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string output;
	string fname = pnode->str;
	regex regexp("%([-+ #0]+)?[0-9]?(\\.[0-9])?[hlL]?[cuoxXideEgGfs]");
	auto argit = args.begin();
	if (fname == "fprintf")
		argit++; // to skip FILE_ID
	string fmtstring = (*argit++).str();

	past->Sig.Reset(2);	// to get the output string
	AuxScope tast(past);	// to preserve this->Sig
	processEscapes(fmtstring);
	smatch mm;
	regex_search(fmtstring, mm, regexp);
	string nextstr = mm.suffix().str();
	string formatstr = mm.str(0);
	if (mm.empty())
		nextstr = fmtstring;
	else
	{
		if (argit== args.end())
			throw exception_func(*past, pnode, "--Insufficient argument", fname).raise();
		int res = gen_formatted_output(output, mm.prefix(), mm.str(0), *argit++);
		if (res < 0)
			throw exception_func(*past, pnode, "string arg requires tring object", "printf").raise();
		int k = 0;
		while (res > 0 && regex_search(nextstr, mm, regexp))
		{
			if (argit == args.end())
				throw exception_func(*past, pnode, "--Insufficient argument", fname).raise();
			res = gen_formatted_output(output, mm.prefix(), mm.str(0), *argit++);
			if (res < 0)
				throw exception_func(*past, pnode, "--String requires tring object", fname).raise();
			nextstr = mm.suffix().str();
		}
	}
	output += nextstr;
	if (fname == "printf")
		cout << output;
	else if (fname == "sprintf")
		past->Sig.SetString(output.c_str());
	else if (fname == "fprintf")
	{
		auto fp = (FILE*)*(uint64_t*)(args.begin())->buf;
		if (fprintf(fp, "%s", output.c_str()) < 0)
		{
			throw exception_func(*past, pnode, "-- Invalid file identifier", fname).raise();
		}
		past->Sig.SetValue((float)output.size());
	}
}

void _printf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	//fname should be either  "printf" || "sprintf"
	vector<CVar> _args;
	_args.push_back(past->Sig);
	for (auto v : args)
		_args.push_back(v);
	__printf(past, pnode, _args);
}

void _fprintf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	vector<CVar> _args;
	_args.push_back(past->Sig);
	for (auto v : args)
		_args.push_back(v);
	__printf(past, pnode, _args);
}

#ifndef NO_FILES
//
//void _fprintf(AuxScope* past, const AstNode* pnode)
//{
//	const AstNode* p = get_first_arg(pnode, (*(past->pEnv->builtin.find(pnode->str))).second.alwaysstatic);
//	CVar firstarg = past->Sig;
//	_sprintf(past, pnode);
//	string buffer;
//	buffer = past->Sig.string();
//	bool openclosehere(1);
//	FILE* file = nullptr;
//	//is first argument string?
////	past->Compute(p);
//	if (firstarg.IsString())
//	{
//		string filename = past->makefullfile(firstarg.string());
//		file = fopen(filename.c_str(), "at");
//	}
//	else
//	{
//		if (!firstarg.IsScalar())
//		{
//			past->Sig.SetValue(-2.);
//			return;
//		}
//		if (firstarg.value() == 0.)
//		{
//			printf(buffer.c_str());
//			return;
//		}
//		file = file_ids[firstarg.value()];
//		openclosehere = false;
//	}
//	if (!file)
//	{
//		throw CAstException(FUNC_SYNTAX, *past, pnode).proc("First arg must be either a file identifider, filename or 0 (for console)");
//	}
//	if (fprintf(file, buffer.c_str()) < 0)
//		past->Sig.SetValue(-3.);
//	else
//	{
//		if (openclosehere)
//		{
//			if (fclose(file) == EOF)
//			{
//				past->Sig.SetValue(-4.);
//				return;
//			}
//		}
//		past->Sig.SetValue((double)buffer.length());
//	}
//}

#endif // NO_FILES
