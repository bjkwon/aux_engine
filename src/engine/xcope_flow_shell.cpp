// AUXLAB 
//
// Copyright (c) 2009-2019 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.704
// Date: 1/1/2021

#include <iostream>
#include <sstream>
#include <list>
#include <algorithm>
#include <exception>
#include <math.h>
#include <time.h>
#include "aux_classes.h"
#include "AuxScope.h"
#include "AuxScope_exception.h"
//#include "console.h"
#include "psycon.tab.h"
#include <thread>

using namespace std;

static const AstNode* get_try_node(const AstNode* pnode)
{
	const AstNode* p = pnode;
	// for a base udf, pnode is N_BLOCK; otherwise, it is T_FUNCTION
	if (p->type == T_FUNCTION) p = p->child->next;
	for (; p; p = p->next)
		if (p->type == T_TRY)
			return p;
	return NULL;
}

static const AstNode* get_base_node_for_try (const AstNode* pnode, int line)
{// Get the node of try from the t_func node of the base udf
	// First, find out in which (local) function try was called
	const AstNode* p = pnode;
	for (; p; p = p->next)
	{
		if (!p->next)
			return p;
		else if (line < p->next->line)
			return p;
	}
	return NULL;
}

/* If an exception is thrown from eval(), errline or errcol value is for the eval, not the location of the eval call in the udf
* i.e., errline or errcol is not really useful in that case. 12/21/2022
*/
CVar* AuxScope::Try_here(const AstNode* pnode, AstNode* p)
{ // to be used only for udf 
	try {
		process_statement(p);
	}
	catch (const AuxScope_exception& e) {
		// If an exception is thrown inside of a try in a udf, e carries the information 
		// at the time of exception, including where it occurred, the xtree node info, etc,
		// with inTryCatch set. That information is copied to the variable specified in
		// catch, and the udf proceeds to the bottom of CallUDF(), to the next line
		// i.e., the first line of catch.
		// For other exceptions, this is bypassed and e is further thrown to the user
		// to be captured by xcom 1/1/2021
		if (pEnv->inTryCatch)
		{ // Make a new struct variable from child of e.pTarget (which is T_CATCH)
			auto baseudf = get_base_node_for_try(u.t_func_base, p->line);
			auto pnode_try = get_try_node(baseudf);
			const char* name = pnode_try->alt->child->str; // the variable name of catch (as "catchme" in catch "catchme")
			string errmsg = e.outstr;
			size_t id = errmsg.find("[GOTO_BASE]");
			if (id != string::npos) errmsg = errmsg.substr(id + string("[GOTO_BASE]").size());
			CVar msg(errmsg);
			SetVar("full", &msg, &Vars[name]);
			msg = e.basemsg;
			SetVar("base", &msg, &Vars[name]);
			errmsg = e.msgonly;
			if (id != string::npos) errmsg = errmsg.substr(id + string("[GOTO_BASE]").size());
			msg = errmsg;
			SetVar("body", &msg, &Vars[name]);
			msg = e.udffile;
			SetVar("source", &msg, &Vars[name]);
			msg.SetValue((auxtype)e.line);
			SetVar("errline", &msg, &Vars[name]);
			msg.SetValue((auxtype)e.col);
			SetVar("errcol", &msg, &Vars[name]);
			process_statement(pnode_try->alt);
			if (pnode_try->alt->type == T_CATCHBACK)
			{
				auto tblock = make_unique<AstNode>();
				memset(tblock.get(), 0, sizeof(AstNode));
				tblock->type = N_BLOCK;
				tblock->next = pTryLast->next;
				auto ttry = make_unique<AstNode>();
				memset(ttry.get(), 0, sizeof(AstNode));
				ttry->type = T_TRY;
				ttry->line = pTryLast->line;
				ttry->col = pTryLast->col;
				ttry->child = tblock.get();
				ttry->alt = pnode_try->alt;
				pEnv->TRY((AuxScope*)this, ttry.get());
			}
		}
		else
			throw e;
	}
	return &Sig;
}

static bool isItBreakPoint(const vector<int>& breakpoints, int currentLine)
{
	return find(breakpoints.begin(), breakpoints.end(), currentLine) != breakpoints.end();
}

void AuxScope::hold_at_break_point(const AstNode* pnode)
{
	// if the current line is one of breakpoints
	// if currently stepping
	if (isItBreakPoint(pEnv->udf[u.title].DebugBreaks, pnode->line) ||
		u.debugstatus == step ||
		u.debugstatus == step_in
		)
	{
		if (u.debugstatus == step_in)  u.debugstatus = progress;
		if (pEnv->debug_hook) {
			DebugEvent ev{ this, pnode, pnode->line };
			DebugAction action = pEnv->debug_hook(ev);

			switch (action) {
			case DebugAction::AbortToBase:
				u.debugstatus = abort2base;
				break;
			case DebugAction::StepIn:
				u.debugstatus = step_in;
				break;
			case DebugAction::Step:
				u.debugstatus = step;
				break;
			case DebugAction::Continue:
			default:
				// leave as progress
				break;
			}
		}
	}
}

const AstNode* AuxScope::linebyline(const AstNode* p)
{
	while (p)
	{
		pLast = p;
		// T_IF, T_WHILE, T_FOR are checked here to break right at the beginning of the loop
		u.currentLine = p->line;
		// N_IDLIST here is probably outdated. 7/26/2023
		if (p->type == T_ID || p->type == T_FOR || p->type == T_IF || p->type == T_WHILE || p->type == T_SWITCH || p->type == N_IDLIST || p->type == N_VECTOR)
			hold_at_break_point(p);
		if (u.debugstatus == abort2base)
		{
			u.currentLine = -1;
			throw this;
		}
		if (pEnv->inTryCatch)
			pTryLast = p;
		process_statement(p);
		Sig.Reset(1); // without this, fs=3 lingers on the next line; if Sig is a cell or struct, it lingers on the next line and may cause an error
		if (fExit) return p;
		p = p->next;
	}
	return NULL;
}

void AuxScope::CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase, size_t nargout_requested)
{
	// Returns the number of output arguments requested in the call
	// 
	// t_func: the T_FUNCTION node pointer for the current UDF call, created after ReadUDF ("formal" context--i.e., how the udf file was read with variables used in the file)
	// pOutParam: AstNode for formal output variable (or LHS), just used inside of this function.
	// Output parameter dispatching (sending the output back to the calling worksapce) is done with pOutParam and lhs at the bottom.

	// u.debugstatus is set when debug key is pressed (F5, F10, F11), prior to this call.
	// For an initial entry UDF, u.debugstatus should be null
	CVar nargin((auxtype)u.nargin);
	CVar nargout((auxtype)nargout_requested);
	SetVar("nargin", &nargin);
	SetVar("nargout", &nargout);
	// If the udf has multiple statements, p->type is N_BLOCK), then go deeper
	// If it has a single statement, take it from there.
	AstNode* pFirst = u.t_func->child->next;
	if (pFirst->type == N_BLOCK)	pFirst = pFirst->next;
	//Get the range of lines for the current udf
	u.currentLine = pFirst->line;
	linebyline(pFirst);
}
