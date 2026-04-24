#include "AuxScope.h"
#include "AuxScope_exception.h"
#include "utils.h"
#include <assert.h>
#include <deque>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <sstream>
#include <stdexcept>

static const int AST_FLAG_ASYNC_ASSIGN = AST_SUPPRESS_ASYNC_ASSIGN;

namespace {
struct AsyncAssignJob {
	uint64_t id = 0;
	EngineRuntime* env = nullptr;
	AuxScope* owner = nullptr;
	std::string lhs;
	std::string rhs_expr;
	std::map<std::string, CVar> vars_snapshot;
	std::atomic<bool> done{ false };
	bool ok = false;
	CVar value;
	std::string error;
};

std::mutex g_async_jobs_mtx;
std::vector<std::shared_ptr<AsyncAssignJob>> g_async_jobs;
std::atomic<uint64_t> g_async_job_id{ 1 };
}

static bool get_line_text(const std::string& script, int line, std::string& out)
{
	if (line <= 0) return false;
	std::stringstream ss(script);
	for (int k = 1; std::getline(ss, out); ++k)
		if (k == line) return true;
	return false;
}

static bool extract_async_rhs_from_script(const AuxScope& ths, const AstNode* pnode, std::string& rhs)
{
	std::string line;
	if (!get_line_text(ths.script, pnode->line, line))
		return false;
	auto pos = line.find("<-");
	if (pos == std::string::npos)
		return false;
	rhs = line.substr(pos + 2);
	trim(rhs, std::string(" \t\r;"));
	return !rhs.empty();
}

static void start_async_assign_job(AuxScope& ths, const std::string& lhs, const std::string& rhs_expr, uint64_t id)
{
	auto job = std::make_shared<AsyncAssignJob>();
	job->id = id;
	job->env = ths.pEnv;
	job->owner = &ths;
	job->lhs = lhs;
	job->rhs_expr = rhs_expr;
	job->vars_snapshot = ths.Vars;
	{
		std::lock_guard<std::mutex> lk(g_async_jobs_mtx);
		g_async_jobs.push_back(job);
	}
	std::thread([job, env = ths.pEnv]() {
		try {
			AuxScope worker(env);
			worker.Vars = job->vars_snapshot;
			const std::string expr = "__async_tmp__=" + job->rhs_expr;
			AstNode* n = worker.makenodes(expr);
			if (!n)
				throw std::runtime_error("Invalid async RHS expression.");
			worker.node = n;
			worker.Compute();
			auto it = worker.Vars.find("__async_tmp__");
			if (it == worker.Vars.end())
				throw std::runtime_error("Async RHS did not produce a value.");
			job->value = it->second;
			job->ok = true;
		}
		catch (const AuxScope_exception& e) {
			job->error = e.outstr;
			job->ok = false;
		}
		catch (const std::exception& e) {
			job->error = e.what();
			job->ok = false;
		}
		catch (const char* e) {
			job->error = e ? e : "Async job failed.";
			job->ok = false;
		}
		catch (...) {
			job->error = "Async job failed.";
			job->ok = false;
		}
		job->done.store(true);
	}).detach();
}

static int drain_async_assign_jobs(AuxScope& ths)
{
	int changed = 0;
	std::lock_guard<std::mutex> lk(g_async_jobs_mtx);
	for (auto it = g_async_jobs.begin(); it != g_async_jobs.end(); )
	{
		auto& job = *it;
		if (job->env != ths.pEnv || !job->done.load())
		{
			++it;
			continue;
		}
		if (job->ok)
		{
			if (job->owner && job->owner->pEnv == ths.pEnv)
				job->owner->SetVar(job->lhs.c_str(), &job->value);
			else
				ths.SetVar(job->lhs.c_str(), &job->value);
			++changed;
		}
		else
		{
			CVar carrier, status, msg, jid;
			status.SetString("error");
			msg.SetString(job->error.c_str());
			jid.SetValue((auxtype)job->id);
			carrier.strut["status"] = status;
			carrier.strut["message"] = msg;
			carrier.strut["job_id"] = jid;
			if (job->owner && job->owner->pEnv == ths.pEnv)
				job->owner->SetVar(job->lhs.c_str(), &carrier);
			else
				ths.SetVar(job->lhs.c_str(), &carrier);
			++changed;
		}
		it = g_async_jobs.erase(it);
	}
	return changed;
}

int AuxScope::drain_async_jobs()
{
	return drain_async_assign_jobs(*this);
}

bool AuxScope::get_nodes_left_right_sides(const AstNode* pnode, const AstNode** plhs, const AstNode** prhs)
{
	replica.Reset();
	*plhs = NULL;
	*prhs = pnode;
	if (find(pEnv->type_arith_op.begin(), pEnv->type_arith_op.end(), pnode->type) != pEnv->type_arith_op.end())
		return false;
	if (find(pEnv->type_condition.begin(), pEnv->type_condition.end(), pnode->type) != pEnv->type_condition.end())
		return false;
	if (find(pEnv->type_blockflow.begin(), pEnv->type_blockflow.end(), pnode->type) != pEnv->type_blockflow.end())
		return false;
	if (!pnode)
		return false;
	if (pnode->child) 
	{
		*plhs = pnode;
		*prhs = pnode->child;
	}
	// if LHS is NULL, don't search replica on the RHS
	if (!*plhs) return false;
	// Check if RHS includes the replica
	return searchtree(*prhs, T_REPLICA) != NULL;
}

void AuxScope::eval_index(const AstNode* pInd, const CVar &varLHS, CVar &index)
{
	// input: pInd, psigBase
	// output: index -- sig holding all indices

	// process the first index
	ostringstream oss;
	unsigned int len;
	ends.push_back(find_endpoint(pInd, varLHS));
	try {
		AuxScope tp(this);
		if (pInd->type == T_FULLRANGE)
		{ // x(:,ids) or x(:)
			index.UpdateBuffer((unsigned int)ends.back());
			for (int k = 0; k < (int)index.nSamples; k++)	index.buf[k] = k + 1;
		}
		else
			index = Compute(pInd);
		if (index.IsLogical()) index_array_satisfying_condition(index);
		// process the second index, if it exists
		if (pInd->next)
		{
			if (varLHS.nGroups > 1 && index.nSamples > 1)
				index.nGroups = index.nSamples;
			AstNode* p = pInd->next;
			CVar isig2;
			if (p->type == T_FULLRANGE)
			{// x(ids,:)
				len = varLHS.Len();
				isig2.UpdateBuffer(len);
				for (unsigned int k = 0; k < len; k++)	isig2.buf[k] = k + 1;
			}
			else // x(ids1,ids2)
			{
				//endpoint for the second arg in 2D is determined here
				ends.push_back((double)varLHS.Len());
				isig2 = Compute(p);
				ends.pop_back(); // pop 2-D end value from the stack here
			}
			if (isig2.IsLogical()) index_array_satisfying_condition(isig2);
			if (isig2.nSamples > 0 && isig2._max() > (double)varLHS.Len())
			{
				oss << "max of 2nd index " << isig2._max() << " exceeds" << varLHS.Len() << ".";
				throw exception_range(this, pInd, oss.str().c_str(), "");
			}
			interweave_indices(index, isig2, varLHS.Len());
		}
	}
	catch (AuxScope_exception e) {
		e.outstr += "invalid indexing... code to be refactored";
		throw e;
	}
	ends.pop_back();
	//Check if index is within range 
	if (index.nSamples > 0 && (index._max() > varLHS.nSamples || index._min() < 1)) {
		oss << index._max();
		throw exception_range(*this, pInd, oss.str(), "").raise();
	}
}

const CVar* AuxScope::get_cell_item(const AstNode* plhs, const CVar &cellobj)
{ // from CNodeProbe::cell_indexing
	CVar* out;
	size_t cellind = (size_t)(plhs->alt->dval); // check the validity of ind...probably it will be longer than this.
	ostringstream oss;
	if (cellobj.type() & TYPEBIT_CELL)
	{
		if (cellind > cellobj.cell.size())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*this, plhs->alt, oss.str().c_str(), plhs->str);
		}
		return &cellobj.cell[cellind - 1];
		//if (plhs->child && root->child && pbase->searchtree(root->child, T_REPLICA))
		//	pbase->replica_prep(&pbase->Sig);
	}
	else
	{ // in this case x{2} means second chain
		printf("********************************\n");
		if (cellind > cellobj.CountChains())
		{
			oss << "Cell index " << cellind;
			throw exception_range(*this, plhs->alt, oss.str().c_str(), plhs->str).raise();
		}
		CTimeSeries* pout = (CTimeSeries*)&cellobj;
		for (size_t k = 0; k < cellind; k++, pout = pout->chain) {}
		out = (CVar*)pout;
	}
	return out;
}
// left var is available through Vars.find
void AuxScope::eval_lhs(const AstNode* plhs, const AstNode* prhs, CVar &lhs_index, CVar& RHS, uint16_t &typelhs, bool &contig, bool isreplica, const CVar* cell_item)
{
	// in: plhs, prhs
	// out: indices of left var, right CVar object
	ostringstream out;
	// must be eihter TID or N_VECTOR
	contig = true;
	if (plhs->type == N_VECTOR) {
		lhs = (AstNode*)plhs;
		const AstNode* pvector = (const AstNode*)plhs->str;
		// pvector->type should be N_VECTOR
		for (auto p = pvector->alt; p; p = p->next) {
			if (p->alt && p->alt->type==N_ARGS) {
				out << "Items in the LHS vector must not have indexing: " << p->str;
				throw exception_etc(*this, plhs, out.str()).raise();
			}
		}
		outputbinding_for_eval_lhs(plhs);
	}
	else // assume TID
	{
		lhs = NULL;
		const CVar* pvarLHS;
		const CVar* struct_item = NULL;
		const AstNode* pstruct = NULL;
		// if lvalue is not eligible for a statement, throw here
		if (prhs) {
			if (pEnv->IsValidBuiltin(plhs->str))
			{
				out << "LHS must be an l-value. " << plhs->str << " is a built-in function.";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
			// if var is not defined, type is null
			auto itvar = Vars.find(plhs->str);
			if (itvar == Vars.end())
			{ // if left var is not defined
				if (isreplica) {
					out << "Replicator (..) cannot be used with undefined LHS.";
					throw exception_etc(*this, plhs, out.str()).raise();
				}
				// For N_STRUCT, in a....propn = RHS anything a to propn can be undefined
				// but a....propn(id) = RHS should issue an error
				bool alert_undefinedLHS = false;
				if (plhs->alt) {
					switch (plhs->alt->type) {
					case N_STRUCT:
						for (auto p = plhs; p; p = p->alt) {
							pstruct = p;
							if (!p->alt) 
								break;
						}
						if (pstruct->type== N_ARGS || pstruct->type == N_TIME_EXTRACT || pstruct->type == N_CELL)
							alert_undefinedLHS = true;
						break;
					case N_CELL:
						if (plhs->alt->dval != 0.)
							alert_undefinedLHS = true;
						break;
					case N_ARGS:
					case N_TIME_EXTRACT:
						alert_undefinedLHS = true;
						break;
					}
				}
				if (alert_undefinedLHS) {
					out << "The object on the LHS has not been defined and cannot be indexed.";
					throw exception_etc(*this, plhs, out.str()).raise();
				}
				typelhs = TYPEBIT_NULL;
				return;
			}
			pvarLHS = &(itvar->second);
		}
		else {
			pvarLHS = cell_item;
		}
		// I think this was done--- x....prop = RHS; this works whether .prop was defined or not
		// Todo-- x....prop(id) = RHS   Assume .prop is available, then we need to do type-check and length-check for .prop 
		if (!plhs->alt)
		{ // left var is available but no indexing is indicated--> replacing it with a new RHS
			typelhs = TYPEBIT_NULL;
			return;
		}
		if (plhs->alt->type == N_STRUCT) // look at the prop item, not the base one
		{
			// x.prop1.prop2...propk...propn = RHS
			//  propk is the last item available as struct (i.e., .prop(k+1) has not been defined)
			//  struct_item is the CVar of .propk
			//  plhs (input) points to x
			//  pstruct (output) points to the node of .prop(k+1)
			//go down to the prop item. x.prop1.prop2.prop3....prop7 
			// If prop1 is undefined, return with TYPEBIT_NULL
			struct_item = get_available_struct_item(plhs, &pstruct);
			// struct_item NULL means nothing has been defined from prop1 through propn
			// struct_item not NULL but if it doesn't have strut means prop(k+1) is defined without indexing 
			// pstruct->alt->type == N_STRUCT means .prop(k+1) has a decendent node that was not defined--> no need to check
			// (pstruct->alt->type == T_ID or N_TIME_EXTRACT) means .prop(k+1) has a decendent node accessed with indexing--need to check
			if (!struct_item || !pstruct->alt || pstruct->alt->type == N_STRUCT) {
				typelhs = TYPEBIT_NULL;
				return;
			}
			pvarLHS = struct_item;
		}
		typelhs = pvarLHS->type();
		if ((typelhs & TYPEBIT_CELL)) {
			// check if ind is valid in x{ind} -- todo 9/5/2022
			if (plhs->alt->type == N_ARGS)
				throw exception_etc(*this, plhs, "Items in the cell array on LHS should be accessed with {}").raise();
			lhs_index.SetValue(plhs->alt->dval);
			return;
		}
		// check type.. mask with 0xFFF4 -- to mask the last two bits zero (clean the length bits) to check the type only (no length)
		auto typerhs = RHS.type();
		if (!pstruct || pstruct->alt)
			if (typerhs > 0 && plhs->alt->type != N_STRUCT && (typelhs & (uint16_t)0xFFF4) != (typerhs & (uint16_t)0xFFF4)) {
				if (!ISAUDIO(typelhs) || !ISAUDIO(typerhs)) // if one is single chain audio and the other is chained audio, it should't throw
					throw exception_etc(*this, plhs, "LHS and RHS have different object type.").raise();
			}
		if (plhs->alt->type == N_TIME_EXTRACT)
		{
			// if lhs var is not audio, throw
			if (!ISAUDIO(typelhs))	{
				out << "LHS must be audio.";
				throw exception_etc(*this, plhs, out.str()).raise();
			}
			lhs_index = gettimepoints((CTimeSeries*)pvarLHS, plhs->alt);
			if (pvarLHS->next)
				lhs_index.SetNextChan(gettimepoints(pvarLHS->next, plhs->alt));
			return;
		}
		// if pstruct is not NULL, .prop is being checked 
		if (pstruct) {
			if (pstruct->alt)
				plhs = pstruct;
			else
				return;
		}
		// x(ind): process ind
		eval_index(plhs->alt->child, *pvarLHS, lhs_index);
		if (lhs_index.nSamples == 0)
			return;
		//check size
		if (RHS.nSamples>1 && lhs_index.nSamples > 1)
		if (lhs_index.nSamples != RHS.nSamples || lhs_index.nGroups != RHS.nGroups)
			throw exception_etc(*this, plhs, "LHS and RHS have different dimension (lengths).").raise();
		for (uint64_t k = 0; k < lhs_index.nSamples-1; k++)
		{
			if (lhs_index.buf[k] + 1. != lhs_index.buf[k + 1])
			{
				contig = false;
				break;
			}
		}
	}
}

void AuxScope::assign_struct(CVar* lobj, const AstNode* plhs, const AstNode* pstruct, const CVar& robj)
{
	deque<string> strchain;
	deque<CVar> cvarchain;
	const AstNode* p = pstruct;
	for (auto q = pstruct->alt; q; q = q->alt)
	{
		strchain.push_back(q->str);
		cvarchain.push_back(CVar());
	}
	auto pvar = (CVar*)&robj;
	for (auto rit = cvarchain.end(); !strchain.empty(); strchain.pop_back())
	{
		rit--;
		auto str = strchain.back();
		(*rit).strut[str] = pvar;
		pvar = &(*rit);
	}
	if (!lobj)
		SetVar(plhs->str, pvar);
	else
		lobj->strut[pstruct->str] = pvar;
}
CVar* AuxScope::get_available_struct_item(const AstNode* plhs, const AstNode** pstruct)
{ // x.p1 defined, but the statement is x.q = RHS --> OK, but x.q(2) = RHS --> NOT OK, throw here
	CVar* pvarLHS = NULL;
	*pstruct = plhs;
	auto it = Vars.find(plhs->str);
	if (it != Vars.end()) {
		map<std::string, CVar>::iterator itvar;
		for (pvarLHS = &(it->second); plhs->alt && plhs->alt->type == N_STRUCT; plhs = plhs->alt) {
			*pstruct = plhs->alt;
			itvar = ((CVar*)pvarLHS)->strut.find(plhs->alt->str);
			if (itvar == pvarLHS->strut.end()) {
				if (plhs->alt->type==N_STRUCT && plhs->alt->alt && plhs->alt->alt->type == N_ARGS)
					throw exception_etc(*this, plhs, string("Trying to index an undefined member variable .") + plhs->alt->str  + " on LHS").raise();
				break;
			}
			if (plhs->alt->alt)
				pvarLHS = &(itvar->second);
		}
	}
	return pvarLHS;
}

/* Adjust lvar, a CVar object on the LHS, with robj, a CVar object on the RHS, according to lhs_index
/* contig: true if a contiguous buffer block is represented by lhs_index
/* pn: pointer to AstNode, only used for exception handling
*/
void AuxScope::adjust_buf(CVar& lvar, const CVar& lhs_index, const CVar& robj, bool contig, const AstNode* pn)
{
	if (robj.nSamples == 0)
	{ // truncate the LHS var buffer
		if (contig)
		{
			memmove(lvar.buf + (uint64_t)lhs_index.buf[0] - 1, lvar.buf + (uint64_t)lhs_index.buf[lhs_index.nSamples - 1], (lvar.nSamples - (uint64_t)lhs_index.buf[0] - lhs_index.nSamples + 1) * sizeof(auxtype));
			lvar.nSamples -= lhs_index.nSamples;
		}
		else
		{
			for (uint64_t k = 0; k < lhs_index.nSamples; k++) {
				memmove(lvar.buf + (uint64_t)lhs_index.buf[k] - 1, lvar.buf + (uint64_t)lhs_index.buf[k], (lvar.nSamples - (uint64_t)lhs_index.buf[k])  * sizeof(auxtype));
				--lvar.nSamples;
				for (uint64_t p = k; p < lhs_index.nSamples; p++) lhs_index.buf[p]--;
			}
		}
	}
	else if (robj.nSamples == 1)
	{ // fill the buffer with the RHS value 
		for (uint64_t k = 0; k < lhs_index.nSamples; k++)
			lvar.buf[(uint64_t)lhs_index.buf[k] - 1] = robj.buf[0];
	}
	else if (lhs_index.nSamples == 1)
	{
		auto nCopied = lvar.nSamples + 1 - (uint16_t)lhs_index.buf[0];
		lvar.UpdateBuffer(lvar.nSamples + robj.nSamples);
		auxtype* pv = lvar.buf;
		auto j = (uint64_t)lhs_index.buf[0] - 1;
		memmove(&pv[j + robj.nSamples], &pv[j], nCopied * sizeof(auxtype));
		auto pval = robj.buf;
		memcpy(&pv[j], pval, robj.nSamples * sizeof(auxtype));
	}
	else if (lhs_index.nSamples == robj.nSamples) {
		if (contig)
			memmove(lvar.logbuf + lvar.bufBlockSize * ((uint64_t)lhs_index.buf[0] - 1), robj.buf, lvar.bufBlockSize * robj.nSamples);
		else
			for (uint64_t k = 0; k < robj.nSamples; k++)
				lvar.buf[(uint64_t)lhs_index.buf[k] - 1] = robj.buf[k];
	}
	else
		throw exception_etc(*this, pn, "Unexpected case").raise();
}

void AuxScope::extract_by_index(CVar& out, const CVar& index, const CVar& obj, bool contig)
{
	// Clear
	out.Reset();
	out.bufType = obj.bufType;
	out.bufBlockSize = obj.bufBlockSize;
	//allocate the output buffer
	out.UpdateBuffer(index.nSamples);
	if (out.bufBlockSize == 1) {
		if (contig)
			memmove(out.logbuf + out.bufBlockSize * ((uint64_t)index.buf[0] - 1), obj.buf, out.bufBlockSize * obj.nSamples);
		else
			for (uint64_t k = 0; k < index.nSamples; k++)
				out.strbuf[k] = obj.strbuf[(uint64_t)index.buf[k] - 1];
	}
	else {
		if (contig)
			memmove(out.logbuf + out.bufBlockSize * ((uint64_t)index.buf[0] - 1), obj.buf, out.bufBlockSize * obj.nSamples);
		else
			for (uint64_t k = 0; k < index.nSamples; k++)
				out.buf[k] = obj.buf[(uint64_t)index.buf[k] - 1];
	}
	out.nGroups = index.nGroups;
	if (obj.next) {
		CSignals sec;
		sec.UpdateBuffer(index.nSamples);
		sec.bufType = obj.next->bufType;
		if (contig)
			memmove(sec.logbuf + sec.bufBlockSize * ((uint64_t)index.buf[0] - 1), obj.next->buf, sec.bufBlockSize * obj.next->nSamples);
		else
			for (uint64_t k = 0; k < index.nSamples; k++)
				sec.buf[k] = obj.next->buf[(uint64_t)index.buf[k] - 1];
		out.SetNextChan(sec);
	}
}

/* Assume that lvar is either a scalar, vector, audiosig, or tseq and has already been evaluated
* lvar is being modified with robj according to lhs_index, which is either vector indices (1D or 2D) or time indices
* (also assume that type checking of robj and lvar has been done)
* Note that if RHS has a replica, robj is not an input (it should be NULL) and Compute(prhs) is done here
*/
void AuxScope::mod_sig(CVar& lvar, const CVar& lhs_index, const CVar& robj, bool contig, const AstNode* plhs, const AstNode* prhs)
{
	bool isreplica = prhs != NULL;
	if (lhs_index.nSamples == 0)
	{
		CVar rhs_eval;
		const CVar* prhs_obj = &robj;
		if (isreplica)
		{
			rhs_eval = Compute(prhs);
			prhs_obj = &rhs_eval;
		}
		if (prhs_obj->type() == TYPEBIT_NULL || prhs_obj->nSamples == 0)
			return;
		throw exception_etc(*this, plhs, "Empty index on LHS can only be assigned a null value.").raise();
	}
	if (plhs->alt->type == N_TIME_EXTRACT)
	{
		if (isreplica) { //RL-T
			replica = lvar;
			replica.Crop(lhs_index);
			insertreplace(plhs, Compute(prhs), lhs_index, &lvar, isreplica);
			replica.Reset();
		}
		else
			insertreplace(plhs, robj, lhs_index, &lvar, isreplica);
	}
	else
	{
		const AstNode* pn = plhs->alt;
		if (isreplica) { //RL-X
			replica.UpdateBuffer(lhs_index.nSamples);
			if (contig)
				memmove(replica.logbuf, lvar.logbuf + (size_t)(lvar.bufBlockSize * (lhs_index.buf[0] - 1)), lvar.bufBlockSize * lhs_index.nSamples);
			else
				for (uint64_t k = 0; k < lhs_index.nSamples; k++)
					replica.buf[k] = lvar.buf[(uint64_t)lhs_index.buf[k] - 1];
			adjust_buf(lvar, lhs_index, Compute(prhs), contig, pn);
			replica.Reset();
		}
		else
			adjust_buf(lvar, lhs_index, robj, contig, pn);
	}
}

void AuxScope::right_to_left(const AstNode* plhs, const CVar& lhs_index, CVar& robj, uint16_t typelhs, bool contig, const AstNode* prhs, CVar* lobj)
{
	bool isreplica = prhs != NULL;
	ostringstream out;
	if (plhs->type == N_VECTOR) {
		if (lhs) { // NULL lhs means outputbinding was done at PrepareAndCallUDF
			outputbinding(plhs);
			lhs = nullptr;
		}
	}
	else if (typelhs == TYPEBIT_NULL) {
		if (plhs->alt && plhs->alt->type == N_STRUCT) {
			const AstNode* pstruct;
			CVar* struct_item = get_available_struct_item(plhs, &pstruct);
			if (isreplica) { //RL-NS
				replica = GetVariable(pstruct->str, pstruct, (CVar*)struct_item);
				robj = Compute(prhs);
			}
			// p.prop1.prop2.prop3=[3 5 2] 
			assign_struct((CVar*)struct_item, plhs, pstruct, robj);
		}
		else {
			if (isreplica) { //RL-N
				if (!plhs->alt) {
					replica = &Vars.find(plhs->str)->second;
					Compute(prhs);
				}
			}
			SetVar(plhs->str, &Sig);
		}
	}
	else
	{
		lobj = &Vars.find(plhs->str)->second;
		if (typelhs & TYPEBIT_CELL) {
			const CVar* cellitem = get_cell_item(plhs, *lobj);
			CVar index;
			if (plhs->alt->alt) {
				//  x{1}(2:3)=[222 333]
				eval_lhs(plhs->alt, NULL, index, (CVar&)robj, typelhs, contig, isreplica, cellitem); // robj is not updated, just provided only as a reference
				mod_sig(*(CVar*)cellitem, index, robj, contig, plhs->alt, prhs);
			}
			else {
				//  x{1}=1:2:20;
				if (isreplica) { //RL-C
					replica = *cellitem;
					robj = Compute(prhs);
				}
				*(CVar*)cellitem = robj;
			}
		}
		else {
			if (plhs->alt->type == N_STRUCT) { // RL-S
			// go til the tip of prop item and call assign_adjust
				CVar* struct_item = get_available_struct_item(plhs, &plhs);
				lobj = struct_item;
			}
			//at this point lobj is either a scalar, vector, audiosig, or tseq 
			mod_sig(*lobj, lhs_index, robj, contig, plhs, prhs);
		}
	}
}

void AuxScope::sanitize_cell_node(const AstNode* p)
{ // evaluate cell indexing in child into dval
	if (p->alt && p->alt->type == N_CELL)
	{
		Compute(p->alt->child);
		if (!ISSCALARG(Sig.type()))
			throw exception_etc(*this, p, "Cell index must be a scalar").raise();
		p->alt->dval = Sig.value();
	}
}

CVar* AuxScope::process_statement(const AstNode* pnode)
{
	drain_async_assign_jobs(*this);
	const AstNode* plhs;
	const AstNode* prhs;
	bool isreplica = get_nodes_left_right_sides(pnode, &plhs, &prhs);
	const bool is_async_assign = (pnode && (pnode->suppress & AST_FLAG_ASYNC_ASSIGN));
	if (is_async_assign)
	{
		if (!plhs || plhs->type != T_ID || plhs->alt)
			throw exception_etc(*this, pnode, "Async assignment currently supports only plain variables (x <- expr).").raise();
		if (level != 1 || dad != nullptr)
			throw exception_etc(*this, pnode, "Async assignment is allowed only at top-level scope.").raise();
		std::string rhs_expr;
		if (!extract_async_rhs_from_script(*this, pnode, rhs_expr))
			throw exception_etc(*this, pnode, "Cannot parse async RHS from source line.").raise();
		const uint64_t job_id = g_async_job_id.fetch_add(1);
		CVar carrier, status, jid;
		status.SetString("requested");
		jid.SetValue((auxtype)job_id);
		carrier.strut["status"] = status;
		carrier.strut["job_id"] = jid;
		SetVar(plhs->str, &carrier);
		Sig = carrier;
		start_async_assign_job(*this, plhs->str, rhs_expr, job_id);
		u.pending_assign_lhs = nullptr;
		u.pending_assign_rhs_call = nullptr;
		return &Sig;
	}
	u.pending_assign_lhs = nullptr;
	u.pending_assign_rhs_call = nullptr;
	// If RHS evaluation is interrupted by debugger pause, complete assignment on resume.
	// Covers simple scalar assignment (x = udf(...)) and vector output binding
	// ([out1,out2] = udf(...)).
	if (plhs && ((plhs->type == T_ID && !plhs->alt) || plhs->type == N_VECTOR)) {
		u.pending_assign_lhs = plhs;
		u.pending_assign_rhs_call = prhs;
	}
	CVar RHS;
	if (!isreplica)
		RHS = Compute(prhs);
	if (plhs)
	{
		uint16_t typelhs;
		bool contig;
		CVar index;
		sanitize_cell_node(plhs);
		eval_lhs(plhs, prhs, index, RHS, typelhs, contig, isreplica);
		right_to_left(plhs, index, RHS, typelhs, contig, isreplica ? prhs:NULL);
	}
	u.pending_assign_lhs = nullptr;
	u.pending_assign_rhs_call = nullptr;
	return &Sig;
}

static void replace(CVar& lobj, const CVar& indsig, const CVar& robj, const AuxScope& ths, const AstNode* plhs)
{
	// find out where robj begins and ends (index and tmark for the chain of interest)
	auto begin = lobj.FindChainAndID(indsig.buf[0], true);
	auto end = lobj.FindChainAndID(indsig.buf[1], false);
	if (begin.first == NULL)
		throw exception_etc(ths, plhs, "Time indexing out of range").raise();
	int len;
	CTimeSeries* q = (CTimeSeries*)&robj;
	int offset = begin.second;
	for (auto p = begin.first; p; p = p->chain, q = q->chain) {
		if (begin.first == end.first)
			len = end.second - begin.second + 1;
		else
			len = q->nSamples;
		memcpy(p->logbuf + offset * p->bufBlockSize, q->buf, len * q->bufBlockSize);
		offset = 0;
	}
	if (lobj.next)
		replace(*(CVar*)(lobj.next), *indsig.next, *robj.next, ths, plhs);
}

// N_IDLIST here is probably outdated. 7/26/2023
void AuxScope::insertreplace(const AstNode* plhs, const CVar& robj, const CVar& indsig, CVar *lobj, bool isreplica)
{
	const AstNode* p = plhs;
	if ((p->alt && p->alt->type == N_TIME_EXTRACT) || // x{id}(t1~t2) = ...sqrt
		p->type == N_TIME_EXTRACT || (p->next && p->next->type == N_IDLIST))  // s(repl_RHS1~repl_RHS2)   or  cel{n}(repl_RHS1~repl_RHS2)
	{
		if (isreplica) // direct update of buf
		{
			replace(*lobj, indsig, robj, *this, plhs);
		}
		else
			lobj->ReplaceBetweenTPs(robj, indsig);
	}
}
