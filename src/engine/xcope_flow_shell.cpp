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

static void run_pending_catchback_reentry(AuxScope* scope);


static bool subtree_has_line(const AstNode* root, int line)
{
	if (!root || line <= 0) return false;
	for (const AstNode* p = root; p; p = p->next) {
		if (p->line == line)
			return true;
		if (subtree_has_line(p->child, line))
			return true;
		if (subtree_has_line(p->alt, line))
			return true;
	}
	return false;
}

static const AstNode* find_enclosing_loop_by_line(const AstNode* root, int line)
{
	if (!root || line <= 0) return nullptr;
	const AstNode* best = nullptr;
	for (const AstNode* p = root; p; p = p->next) {
		if (p->type == T_WHILE || p->type == T_FOR) {
			if (subtree_has_line(p->alt, line)) {
				best = p;
				if (const AstNode* inner = find_enclosing_loop_by_line(p->alt, line))
					best = inner;
			}
		}
		if (const AstNode* inner = find_enclosing_loop_by_line(p->child, line))
			best = inner;
		if (const AstNode* inner = find_enclosing_loop_by_line(p->alt, line))
			best = inner;
	}
	return best;
}

static const AstNode* find_enclosing_if_by_line(const AstNode* root, int line)
{
	if (!root || line <= 0) return nullptr;
	const AstNode* best = nullptr;
	for (const AstNode* p = root; p; p = p->next) {
		if (p->type == T_IF && p->child) {
			AstNode* hit_true = AuxScope::goto_line(p->child->next, line);
			AstNode* hit_else = p->alt ? AuxScope::goto_line(p->alt, line) : nullptr;
			if ((hit_true && hit_true->line == line) || (hit_else && hit_else->line == line)) {
				best = p;
				if (const AstNode* inner = find_enclosing_if_by_line(p->child->next, line))
					best = inner;
				else if (p->alt) {
					if (const AstNode* inner_else = find_enclosing_if_by_line(p->alt, line))
						best = inner_else;
				}
			}
		}
		if (const AstNode* inner = find_enclosing_if_by_line(p->child, line))
			best = inner;
		if (const AstNode* inner = find_enclosing_if_by_line(p->alt, line))
			best = inner;
	}
	return best;
}

static const AstNode* find_enclosing_switch_by_line(const AstNode* root, int line)
{
	if (!root || line <= 0) return nullptr;
	const AstNode* best = nullptr;
	for (const AstNode* p = root; p; p = p->next) {
		if (p->type == T_SWITCH) {
			for (const AstNode* c = p->alt; c; c = (c->next ? c->next->alt : nullptr)) {
				const AstNode* body = c->next;
				if (!body) continue;
				AstNode* hit = AuxScope::goto_line(body, line);
				if (hit && hit->line == line) {
					best = p;
					break;
				}
			}
		}
		if (const AstNode* inner = find_enclosing_switch_by_line(p->child, line))
			best = inner;
		if (const AstNode* inner = find_enclosing_switch_by_line(p->alt, line))
			best = inner;
	}
	return best;
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
			if (pnode_try->alt->type == T_CATCHBACK)
			{
				u.pending_catchback_next = pTryLast ? pTryLast->next : nullptr;
				u.pending_catchback_alt = pnode_try->alt;
				u.pending_catchback_line = pTryLast ? pTryLast->line : p->line;
				u.pending_catchback_col = pTryLast ? pTryLast->col : p->col;
			}
			process_statement(pnode_try->alt);
			if (pnode_try->alt->type == T_CATCHBACK)
				run_pending_catchback_reentry(this);
		}
		else
			throw e;
	}
	return &Sig;
}

static bool isItBreakPoint(const vector<int>& breakpoints, int currentLine)
{
	const int line = std::abs(currentLine);
	for (int bp : breakpoints) {
		if (std::abs(bp) == line) return true;
	}
	return false;
}

static std::string to_lower_copy(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

static const std::vector<int>* resolve_breakpoints_for_scope(const AuxScope* scope)
{
	if (!scope || !scope->pEnv) return nullptr;
	auto& udf = scope->pEnv->udf;
	std::string title = to_lower_copy(scope->u.title);
	auto it = udf.find(title);
	if (it != udf.end())
		return &it->second.DebugBreaks;

	std::string base = scope->u.base;
	if (base.empty() && scope->u.t_func_base && scope->u.t_func_base->str)
		base = scope->u.t_func_base->str;
	base = to_lower_copy(base);
	if (base.empty()) return nullptr;

	auto ibase = udf.find(base);
	if (ibase == udf.end()) return nullptr;
	auto ilocal = ibase->second.local.find(title);
	if (ilocal != ibase->second.local.end() && !ilocal->second.DebugBreaks.empty())
		return &ilocal->second.DebugBreaks;
	if (!ibase->second.DebugBreaks.empty())
		return &ibase->second.DebugBreaks;
	if (ilocal != ibase->second.local.end())
		return &ilocal->second.DebugBreaks;
	return nullptr;
}

static bool has_breakpoint_for_scope_line(const AuxScope* scope, int line)
{
	if (!scope || !scope->pEnv || line <= 0) return false;
	auto& udf = scope->pEnv->udf;
	const std::string title = to_lower_copy(scope->u.title);
	auto it = udf.find(title);
	if (it != udf.end() && isItBreakPoint(it->second.DebugBreaks, line))
		return true;

	std::string base = scope->u.base;
	if (base.empty() && scope->u.t_func_base && scope->u.t_func_base->str)
		base = scope->u.t_func_base->str;
	base = to_lower_copy(base);
	if (base.empty()) return false;

	auto ibase = udf.find(base);
	if (ibase == udf.end()) return false;
	if (isItBreakPoint(ibase->second.DebugBreaks, line))
		return true;
	for (const auto& kv : ibase->second.local) {
		if (isItBreakPoint(kv.second.DebugBreaks, line))
			return true;
	}
	return false;
}

static string resolve_paused_file_path(const AuxScope* scope)
{
	if (!scope || !scope->pEnv) return "";

	// Regular UDF call path (title matches a registered UDF key).
	auto it = scope->pEnv->udf.find(scope->u.title);
	if (it != scope->pEnv->udf.end() && !it->second.fullname.empty())
		return it->second.fullname;

	// Local function path (title may be local while base points to the owning file).
	auto ibase = scope->pEnv->udf.find(scope->u.base);
	if (ibase != scope->pEnv->udf.end()) {
		auto ilocal = ibase->second.local.find(scope->u.title);
		if (ilocal != ibase->second.local.end() && !ilocal->second.fullname.empty())
			return ilocal->second.fullname;
		if (!ibase->second.fullname.empty())
			return ibase->second.fullname;
	}

	// Fallback keeps previous behavior.
	return scope->u.title;
}

static void run_pending_catchback_reentry(AuxScope* scope)
{
	if (!scope || !scope->u.pending_catchback_alt)
		return;

	const AstNode* next = scope->u.pending_catchback_next;
	AstNode* alt = (AstNode*)scope->u.pending_catchback_alt;
	int line = scope->u.pending_catchback_line;
	int col = scope->u.pending_catchback_col;

	scope->u.pending_catchback_next = nullptr;
	scope->u.pending_catchback_alt = nullptr;
	scope->u.pending_catchback_line = -1;
	scope->u.pending_catchback_col = -1;

	if (!next || !alt)
		return;

	auto tblock = make_unique<AstNode>();
	memset(tblock.get(), 0, sizeof(AstNode));
	tblock->type = N_BLOCK;
	tblock->next = (AstNode*)next;

	auto ttry = make_unique<AstNode>();
	memset(ttry.get(), 0, sizeof(AstNode));
	ttry->type = T_TRY;
	ttry->line = line;
	ttry->col = col;
	ttry->child = tblock.get();
	ttry->alt = alt;
	scope->pEnv->TRY(scope, ttry.get());
}

void AuxScope::hold_at_break_point(const AstNode* pnode)
{
	if (!has_breakpoint_for_scope_line(this, pnode->line))
		return;

	// If stepping-in just landed, switch back to progress (your current behavior)
	if (u.debugstatus == step_in) u.debugstatus = progress;

	// Record pause location somewhere accessible (new fields you add)
	u.paused_line = pnode->line;
	u.paused_file = resolve_paused_file_path(this);      // or ev.filename
	u.paused_node = pnode;        // store pointer to resume from
	u.debugstatus = paused;

	// Instead of calling debug_hook synchronously, unwind to aux_eval caller.
	throw this; // You already use throw this for abort2base; we'll distinguish by debugstatus
}

void AuxScope::ResumePausedUDF()
{
	const AstNode* resume = u.paused_node;
	if (!resume) return;

	const bool resume_has_next = (resume->next != nullptr);
	const AstNode* root = (u.t_func ? (const AstNode*)u.t_func : (const AstNode*)u.t_func_base);
	const AstNode* resume_loop = find_enclosing_loop_by_line(root, resume->line);
	const AstNode* resume_switch = find_enclosing_switch_by_line(root, resume->line);
	const bool do_continue = (u.debugstatus == progress || u.debugstatus == continu);

	u.paused_node = nullptr;
	u.paused_line = -1;
	u.paused_file.clear();

	// Execute the paused line once, without stopping on the same breakpoint again.
	if (u.debugstatus == step_out)
		linebyline(resume, true, false, true);
	else if (u.debugstatus == step || u.debugstatus == step_in)
		linebyline(resume, true, true);
	else
		linebyline(resume, true, false);

	run_pending_catchback_reentry(this);

	// Continue from inside switch-case should first continue to the statement
	// after switch (e.g., lines 67/68).
	if (do_continue && resume_switch && resume_switch->next) {
		linebyline(resume_switch->next, true, false);
		run_pending_catchback_reentry(this);
	}
	// Then continue loop progression from enclosing loop header.
	if (do_continue && resume_loop) {
		linebyline(resume_loop, true, false);
		run_pending_catchback_reentry(this);
	}
}

const AstNode* AuxScope::linebyline(const AstNode* p, bool skip_first_break_check, bool step_once, bool suppress_breakpoints)
{
	bool first = true;
	while (p)
	{
		pLast = p;
		// T_IF, T_WHILE, T_FOR are checked here to break right at the beginning of the loop
		u.currentLine = p->line;
		// N_IDLIST here is probably outdated. 7/26/2023
		if (!suppress_breakpoints
			&& p->line > 0
			&& !(skip_first_break_check && first))
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

		if (step_once) {
			const AstNode* next = p->next;
			if (next) {
				u.paused_line = next->line;
				u.paused_file = resolve_paused_file_path(this);
				u.paused_node = next;
				u.debugstatus = paused;
				throw this;
			}
			// End of a nested block in step mode: if we are inside a loop body,
			// move to the next logical execution point.
			const AstNode* root = (u.t_func ? (const AstNode*)u.t_func : (const AstNode*)u.t_func_base);
			const AstNode* sw_parent0 = find_enclosing_switch_by_line(root, p->line);
			if (sw_parent0 && sw_parent0->next) {
				u.paused_line = sw_parent0->next->line;
				u.paused_file = resolve_paused_file_path(this);
				u.paused_node = sw_parent0->next;
				u.debugstatus = paused;
				throw this;
			}
			const AstNode* loop_parent = find_enclosing_loop_by_line(root, p->line);
			if (loop_parent) {
				const AstNode* target = nullptr;
				if (loop_parent->type == T_WHILE) {
					const bool cond_true = checkcond(loop_parent->child);
					if (cond_true) {
						if (loop_parent->alt && loop_parent->alt->type == N_BLOCK)
							target = loop_parent->alt->next;
						else
							target = loop_parent->alt;
					} else {
						target = loop_parent->next;
					}
				} else {
					auto it_idx = u.debug_for_index.find(loop_parent);
					auto it_cnt = u.debug_for_count.find(loop_parent);
					if (it_idx != u.debug_for_index.end() && it_cnt != u.debug_for_count.end()) {
						const unsigned int next_idx = it_idx->second + 1;
						it_idx->second = next_idx;
						if (!fExit && !fBreak && next_idx < it_cnt->second) {
							// Next step should return to for-header, matching user expectation
							// and keeping iteration advancement deterministic.
							target = loop_parent;
						}
						else {
							u.debug_for_index.erase(loop_parent);
							u.debug_for_count.erase(loop_parent);
							target = loop_parent->next;
						}
					}
					else {
						// No persisted for-loop debug state (e.g., pause came from breakpoint
						// inside loop body). Return to loop header and recover state there.
						target = loop_parent;
					}
				}
				if (target) {
					u.paused_line = target->line;
					u.paused_file = resolve_paused_file_path(this);
					u.paused_node = target;
					u.debugstatus = paused;
					throw this;
				}
			}
			const AstNode* if_parent = find_enclosing_if_by_line(root, p->line);
			if (if_parent && if_parent->next) {
				u.paused_line = if_parent->next->line;
				u.paused_file = resolve_paused_file_path(this);
				u.paused_node = if_parent->next;
				u.debugstatus = paused;
				throw this;
			}
			const AstNode* sw_parent = find_enclosing_switch_by_line(root, p->line);
			if (sw_parent && sw_parent->next) {
				u.paused_line = sw_parent->next->line;
				u.paused_file = resolve_paused_file_path(this);
				u.paused_node = sw_parent->next;
				u.debugstatus = paused;
				throw this;
			}
		}

		p = p->next;
		first = false;
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
	// Enter callee frame at first executable line on Step In.
	if (u.debugstatus == step_in && pFirst) {
		u.paused_line = pFirst->line;
		u.paused_file = resolve_paused_file_path(this);
		u.paused_node = pFirst;
		u.debugstatus = paused;
		throw this;
	}
	//Get the range of lines for the current udf
	u.currentLine = pFirst->line;
	u.paused_node = nullptr;  
	u.paused_line = -1;
	u.pending_catchback_next = nullptr;
	u.pending_catchback_alt = nullptr;
	u.pending_catchback_line = -1;
	u.pending_catchback_col = -1;
	u.debug_for_index.clear();
	u.debug_for_count.clear();
	linebyline(pFirst);
}
