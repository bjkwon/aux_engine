#pragma once
#include "AuxScope.h"
#include <sstream>

typedef void(*fGate) (AuxScope* past, const AstNode* pnode, const vector<CVar>& args);

/* Error message must be on of the following patterns:
* 1) 2nd arg must be ______ in func()
* 2) >> requires a scalar operand
* 3) Out of range: 2nd index of TID 
* 4) any other error message: for example, "var" not defined or not a built_in function
*/

class AuxScope_exception
{
public:
	AuxScope_exception() {
		pnode = NULL;  pCtx = NULL;
	};
	AuxScope_exception(const AuxScope& base, const AstNode* _pnode) {
		pnode = _pnode;  pCtx = &base;
		line = pnode->line;
		col = pnode->col;
	};
	virtual ~AuxScope_exception() {};
	AuxScope_exception& raise(); 
	void addLineCol();
	string getErrMsg() const { return outstr; };
	int line, col;

	const AstNode* pnode;
	const AuxScope* pCtx; // pointer to the context, AKA AstSig, that threw the exception
	string basemsg, tidstr;
	string msgonly; // including basemsg, tidstr, and extra
	string sourceloc; // source location; where the error occurred (line, col and file)
	string udffile; // udf file name, if applicable
	string outstr; // msgonly \n sourceloc
};

class exception_func : public AuxScope_exception
{
public:
	exception_func(const AuxScope& base, const AstNode* _pnode, const string& msg, const string& fname="", int id = 0) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "Invalid arg";
		if (id > 0)
			oss << id;
		oss << ": " << msg;
		if (!fname.empty())
			oss << " in " << fname << "()";
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_func() {};
};

class exception_misuse : public AuxScope_exception
{
public:
	exception_misuse(const AuxScope& base, const AstNode* _pnode, const string& msg, int id = 0) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "invalid operand";
		if (id > 0)
			 oss << id;
		oss << " " << msg;
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_misuse() {};
};

class exception_range : public AuxScope_exception
{
public:
	exception_range(const AuxScope& base, const AstNode* _pnode, const string& idpos, const string& msg) {
		ostringstream oss;
		pnode = _pnode;  pCtx = &base;
		oss << "Out of range: " << idpos << " " << msg;
		msgonly = oss.str().c_str();
		line = pnode->line;
		col = pnode->col;
	};
	~exception_range() {};
};

class exception_etc : public AuxScope_exception
{
public:
	exception_etc(const AuxScope& base, const AstNode* _pnode, const string& msg) {
		pnode = _pnode;  pCtx = &base;
		msgonly = msg;
		line = pnode->line;
		col = pnode->col;
	};
	~exception_etc() {};
};

