#include "functions_common.h"

int GetFileText(FILE* fp, string& strOut); // utils.cpp

Cfunction set_builtin_function_include(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "filename", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
Cfunction set_builtin_function_eval(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "expression or commands", };
	vector<string> desc_arg_opt = {  };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING+1, TYPEBIT_STRING+2 }; 
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}
void _eval(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
    // eval() is one of the functions where echoing in the xcom command window doesn't make sense.
  // but the new variables created or modified within the eval call should be transported back to the calling scope
	// As of 5/17/2020, there is no return of eval (null returned if assigned) for when there's no error
	// If there's an error, the error is caught here and the error message is sent to the calling function.
	string emsg, exp = past->Sig.str();
	AuxScope qscope(past);
	qscope.node = qscope.makenodes(exp.c_str());
	if (!qscope.node) // syntax error in the expression
		throw exception_etc(*past, pnode, qscope.emsg).raise();
	qscope.process_statement(qscope.node);
	//transporting variables
	for (map<string, CVar>::iterator it = qscope.Vars.begin(); it != qscope.Vars.end(); it++)
		past->SetVar(it->first.c_str(), &it->second);
	past->Sig = qscope.Sig; // temp hack; just to port out the last result during the eval call
}

void _include(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string dummy, errmsg;
	string filename = past->Sig.str();
	if (FILE* auxfile = past->fopen_from_path(filename, "", dummy)) {
		try {
			AuxScope qscope(past);
			string filecontent;
			if (GetFileText(auxfile, filecontent) <= 0)
			{ // File reading error or empty file
				fclose(auxfile);
				qscope.emsg = "Cannot read specified file";
				throw exception_etc(*past, pnode, qscope.emsg).raise();
			}
			fclose(auxfile);
			qscope.node = qscope.makenodes(filecontent.c_str());
			if (!qscope.node) 
				throw exception_etc(*past, pnode, qscope.emsg).raise();
			vector<CVar*> res = qscope.Compute();
			past->Sig = res.back();
			for (map<string, CVar>::iterator it = qscope.Vars.begin(); it != qscope.Vars.end(); it++)
				past->Vars[it->first] = it->second;
			for (auto it = qscope.GOvars.begin(); it != qscope.GOvars.end(); it++)
				past->GOvars[it->first] = it->second;
		}
		catch (const AuxScope_exception &e) {
			errmsg = string("Error from include ") + filename + ": ";
			if (!e.sourceloc.empty()) errmsg += e.sourceloc + ", ";
			errmsg += e.msgonly;
			throw exception_etc(*past, pnode, errmsg).raise();
		}
	}
	else {
		string emsg = string("Cannot read file: ") + filename;
		throw exception_etc(*past, pnode, emsg).raise();
	}
}