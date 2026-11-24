#include "AuxScope.h"
#include "AuxScope_exception.h"
#include "utils.h"
#include <assert.h>
#include <deque>
#include <iostream>
#include <chrono>
#include <future>
#include <atomic>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <limits>

static std::string lower_copy_process(const std::string& in)
{
	std::string out = in;
	std::transform(out.begin(), out.end(), out.begin(), ::tolower);
	return out;
}

static bool is_reserved_class_member_name(const std::string& name)
{
	return lower_copy_process(name) == "__class";
}

static bool get_object_class_key_process(const CVar* pBase, std::string& class_key)
{
	if (!pBase) return false;
	auto it = pBase->strut.find("__class");
	if (it == pBase->strut.end()) return false;
	if (!ISSTRINGG(it->second.type())) return false;
	class_key = lower_copy_process(it->second.str());
	return !class_key.empty();
}

static bool class_has_declared_member_process(const EngineRuntime* env, const CVar* pBase, const std::string& member_name)
{
	std::string class_key;
	if (!get_object_class_key_process(pBase, class_key))
		return true;
	if (!env)
		return false;
	auto cfd = env->classes.find(class_key);
	if (cfd == env->classes.end())
		return false;
	return cfd->second.defaults.find(member_name) != cfd->second.defaults.end();
}

// The evaluator can mutate its AST and the caller may parse another command
// immediately. Give the worker its own RHS, preserving shared tail pointers.
class DeferredSyntax {
public:
	AstNode* copy(const AstNode* src) {
		if (!src) return nullptr;
		auto [it, inserted] = nodes.try_emplace(src, *src);
		auto& dst = it->second;
		if (!inserted) return &dst;
		dst.str = nullptr;
		if (src->type == N_MATRIX || src->type == N_VECTOR)
			dst.str = reinterpret_cast<char*>(copy(reinterpret_cast<const AstNode*>(src->str)));
		else if (src->str || src->type == N_TSEQ) {
			// TSeq writes a diagnostic name into this parser-owned buffer.
			size_t size = src->str ? strlen(src->str) + 1 : 0;
			if (src->type == N_TSEQ) size = std::max(size, sizeof("TSEQ"));
			dst.str = static_cast<char*>(calloc(size, 1));
			if (!dst.str) throw std::bad_alloc();
			if (src->str) strcpy(dst.str, src->str);
		}
		dst.child = copy(src->child);
		dst.alt = copy(src->alt);
		dst.next = copy(src->next);
		dst.tail = copy(src->tail);
		return &dst;
	}

	void copyUdfTrees(UDF& udf) {
		udf.uxtree = copy(udf.uxtree);
		udf.DebugBreaks.clear();
		for (auto& entry : udf.switch_case) entry.second = copy(entry.second);
		for (auto& entry : udf.local) copyUdfTrees(entry.second);
	}

	~DeferredSyntax() {
		for (auto& entry : nodes) {
			auto& node = entry.second;
			if (node.type != N_MATRIX && node.type != N_VECTOR) free(node.str);
		}
	}
private:
	std::map<const AstNode*, AstNode> nodes;
};

struct DeferredResult {
	CVar value;
	std::shared_ptr<ProgressiveAudioJob> fileJob;
};

class DeferredAssignment {
public:
	string destination;
	std::future<DeferredResult> result;
	std::map<uint64_t, map<string, double>> handleUpdates;
};

CVar* AuxScope::start_deferred(const AstNode* lhs, const AstNode* rhs, bool progressiveFile)
{
	static std::atomic<uint64_t> sequence{1};
	const auto id = sequence++;
	auto syntax = std::make_shared<DeferredSyntax>();
	auto* root = syntax->copy(rhs);
	// UDF registration, mutable UDF syntax and try/catch state belong to one
	// evaluator. Snapshot them before the caller is free to edit its workspace.
	auto env = std::make_unique<EngineRuntime>(pEnv->Fs);
	env->AuxPath = pEnv->AuxPath;
	env->builtin = pEnv->builtin;
	env->classes = pEnv->classes;
	env->udf.insert(pEnv->udf.begin(), pEnv->udf.end());
	for (auto& entry : env->udf) syntax->copyUdfTrees(entry.second);
	env->files = pEnv->files;
	env->graphics_backend = pEnv->graphics_backend;
	env->playback_backend = pEnv->playback_backend;
	env->stream_start = pEnv->stream_start;
	env->native_module_functions = pEnv->native_module_functions;
	env->native_module_aliases = pEnv->native_module_aliases;
	// The launching runtime owns these libraries until all workers have joined.
	env->native_modules = pEnv->native_modules;
	for (auto& entry : env->native_modules) entry.second.library = nullptr;
	auto job = std::make_shared<DeferredAssignment>();
	job->destination = lhs->str;
	// Capture inputs before replacing the destination (x <- x + 1).
	job->result = std::async(std::launch::async, [syntax, root, env = std::move(env), vars = Vars, id, progressiveFile]() mutable -> DeferredResult {
		AuxScope worker(env.get());
		worker.Vars.swap(vars);
		worker.node = root;
		if (progressiveFile) worker.progressiveFileCall = root;
		string error;
		try {
			CVar value = *worker.Compute(root);
			if (!worker.preparedFileJob && value.progressive) {
				value.progressive.reset();
				value.strut.erase("h");
			}
			return {std::move(value), std::move(worker.preparedFileJob)};
		} catch (const AuxScope_exception& e) {
			error = e.outstr;
		} catch (const std::exception& e) {
			error = e.what();
		} catch (const char* e) {
			error = e ? e : "Deferred assignment failed.";
		} catch (...) {
			error = "Deferred assignment failed.";
		}
		CVar result;
		result.strut["status"].SetString("error");
		result.strut["message"].SetString(error.c_str());
		result.strut["job_id"].SetValue(id);
		return {std::move(result), nullptr};
	});
	CVar pending;
	pending.strut["status"].SetString("requested");
	pending.strut["job_id"].SetValue(id);
	SetVar(lhs->str, &pending);
	Sig = pending;
	deferredJobs.push_back(std::move(job));
	return &Sig;
}

void AuxScope::forget_assignment(const string& name)
{
	for (auto& job : deferredJobs)
		if (job->destination == name) job->destination.clear();
	reducers.erase(name);
}

// A short playback or recording can finish before the worker returns its
// handle. Replay host updates on publication, including handles inside structs.
void AuxScope::remember_deferred_handle_members(uint64_t id, const map<string, double>& members)
{
	for (auto& job : deferredJobs) {
		if (job->destination.empty()) continue;
		auto& latest = job->handleUpdates[id];
		for (const auto& member : members) latest[member.first] = member.second;
	}
}

int AuxScope::publish_deferred()
{
	int changed = 0;
	for (auto it = deferredJobs.begin(); it != deferredJobs.end();) {
		auto& job = **it;
		if (job.result.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
			++it;
			continue;
		}
		string destination = std::move(job.destination);
		if (!destination.empty() && Vars.count(destination)) {
			auto result = job.result.get();
			for (const auto& update : job.handleUpdates)
				apply_handle_members(result.value, update.first, update.second);
			SetVar(destination.c_str(), &result.value);
			if (result.fileJob) bind_progressive_job(destination, std::move(result.fileJob));
			++changed;
		}
		it = deferredJobs.erase(it);
	}
	return changed;
}

int AuxScope::drain_async_jobs(bool& filterUpdatesOnly)
{
	const int assignments = publish_deferred();
	const int filters = publish_progressive();
	const int reducers = publish_reducers();
	filterUpdatesOnly = assignments == 0 && reducers == 0;
	return assignments + filters + reducers;
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

static void copy_index_prefix(CTimeSeries& destination, const CTimeSeries& source, uint64_t length)
{
	destination.Reset(source.GetFs());
	destination.bufType = source.bufType;
	destination.bufBlockSize = source.bufBlockSize;
	destination.UpdateBuffer(length);
	destination.nGroups = source.nGroups;
	destination.tmark = source.tmark;
	destination.snap = source.snap;
	if (length > 0)
		memcpy(destination.logbuf, source.logbuf, length * source.bufBlockSize);
}

void AuxScope::eval_index(const AstNode* pInd, const CVar &varLHS, CVar &index, bool* firstIndexWasLogical, CVar* logicalSelector)
{
	// input: pInd, psigBase
	// output: index -- sig holding all indices
	if (firstIndexWasLogical)
		*firstIndexWasLogical = false;
	if (logicalSelector)
		logicalSelector->Reset();

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
		if (index.IsLogical())
		{
			if (index.nSamples == 1 && !index.chain)
				throw exception_etc(*this, pInd, "A scalar Boolean value cannot be used as an index.").raise();

			CVar normalized;
			const bool perBlockSelector = index.chain != NULL;
			const CTimeSeries* sourcePart = &index;
			const CTimeSeries* targetPart = &varLHS;
			CTimeSeries* normalizedPart = &normalized;
			while (targetPart)
			{
				if (!sourcePart)
					throw exception_etc(*this, pInd, "Boolean selector chain does not match the indexed object.").raise();
				const uint64_t requiredLength = pInd->next ? varLHS.nGroups : targetPart->nSamples;
				if (sourcePart->nSamples < requiredLength)
					throw exception_etc(*this, pInd,
						"Boolean selector must contain at least as many entries as the indexed object.").raise();

				copy_index_prefix(*normalizedPart, *sourcePart, requiredLength);
				targetPart = pInd->next ? NULL : targetPart->chain;
				if (perBlockSelector)
					sourcePart = sourcePart->chain;
				if (targetPart)
				{
					normalizedPart->chain = new CTimeSeries;
					normalizedPart = normalizedPart->chain;
				}
			}
			if (perBlockSelector && sourcePart)
				throw exception_etc(*this, pInd, "Boolean selector chain does not match the indexed object.").raise();
			index = normalized;
			if (firstIndexWasLogical)
				*firstIndexWasLogical = true;
			if (logicalSelector)
				*logicalSelector = index;
			index_array_satisfying_condition(index);
		}
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
			if (isig2.IsLogical())
			{
				if (isig2.nSamples == 1 && !isig2.chain)
					throw exception_etc(*this, p, "A scalar Boolean value cannot be used as an index.").raise();
				if (isig2.chain)
					throw exception_etc(*this, p, "A matrix-dimension Boolean selector cannot be chained.").raise();
				if (isig2.nSamples < varLHS.Len())
					throw exception_etc(*this, p,
						"Boolean selector must contain at least as many entries as the indexed dimension.").raise();
				CVar normalizedSecond;
				copy_index_prefix(normalizedSecond, isig2, varLHS.Len());
				isig2 = normalizedSecond;
				index_array_satisfying_condition(isig2);
			}
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
	size_t cellind = (size_t)(plhs->alt->dval); // check the validity of ind...probably it will be longer than this.
	ostringstream oss;
	if (!(cellobj.type() & TYPEBIT_CELL))
		throw exception_etc(*this, plhs, "Temporal block selection is read-only.").raise();
	if (cellind > cellobj.cell.size())
	{
		oss << "Cell index " << cellind;
		throw exception_range(*this, plhs->alt, oss.str().c_str(), plhs->str);
	}
	return &cellobj.cell[cellind - 1];
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
		for (const AstNode* suffix = plhs->alt; suffix; suffix = suffix->alt)
			if (suffix->type == N_STRUCT && suffix->str && !strcmp(suffix->str, "blockat"))
				throw exception_etc(*this, suffix, "Temporal block selection is read-only.").raise();
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
		if (plhs->alt->type == N_CELL && !(pvarLHS->type() & TYPEBIT_CELL))
		{
			if (ISTEMPORAL(pvarLHS->type()))
				throw exception_etc(*this, plhs, "Temporal block selection is read-only.").raise();
			throw exception_etc(*this, plhs, "Brace selection requires a cell or mono temporal object.").raise();
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
			// Advance plhs past the resolved struct/channel-selector chain now, before the
			// CELL/type-check/N_TIME_EXTRACT dispatch below, so those checks see the actual
			// following node (e.g. N_TIME_EXTRACT) instead of the N_STRUCT link itself.
			// pstruct->alt is guaranteed non-null here (checked above).
			plhs = pstruct;
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
        if (!pstruct || pstruct->alt) {
            const bool indexedElementWrite = plhs->alt &&
                                             plhs->alt->type != N_STRUCT &&
                                             plhs->alt->type != N_TIME_EXTRACT;
            uint16_t typeMask = indexedElementWrite ? (uint16_t)0xFFF0 : (uint16_t)0xFFF4;
            // A .left/.right-scoped LHS write always compares as mono: pvarLHS may still be the
            // whole (genuinely stereo) variable itself (see get_available_struct_item), so its
            // type() carries TYPEBIT_MULTICHANS even though only one channel is being written.
            if (channelWriteSel)
                typeMask &= (uint16_t)~TYPEBIT_MULTICHANS;
            const bool scalarTimeFill = plhs->alt->type == N_TIME_EXTRACT &&
                                        RHS.nSamples == 1 && !RHS.chain &&
                                        RHS.bufBlockSize == pvarLHS->bufBlockSize;
            if (typerhs > 0 && plhs->alt->type != N_STRUCT && !scalarTimeFill &&
                (typelhs & typeMask) != (typerhs & typeMask)) {
                if (!ISAUDIO(typelhs) || !ISAUDIO(typerhs)) // if one is single chain audio and the other is chained audio, it should't throw
                    throw exception_etc(*this, plhs, "LHS and RHS have different object type.").raise();
            }
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
		// x(ind): process ind
		eval_index(plhs->alt->child, *pvarLHS, lhs_index);
		if (pvarLHS->chain)
		{
			const bool perBlockIndex = lhs_index.chain != NULL;
			const CTimeSeries* sourcePart = pvarLHS;
			const CTimeSeries* indexPart = &lhs_index;
			for (; sourcePart; sourcePart = sourcePart->chain)
			{
				if (!indexPart)
					throw exception_etc(*this, plhs, "Index chain does not match the source chain.").raise();
				if (indexPart->nSamples > 0 &&
					(indexPart->_max() > sourcePart->nSamples || indexPart->_min() < 1))
					throw exception_range(*this, plhs, "Index exceeds at least one chained block.", "").raise();
				if (perBlockIndex)
					indexPart = indexPart->chain;
			}
			if (perBlockIndex && indexPart)
				throw exception_etc(*this, plhs, "Index chain does not match the source chain.").raise();
		}
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
	channelWriteSel = 0;
	auto it = Vars.find(plhs->str);
	if (it != Vars.end()) {
		map<std::string, CVar>::iterator itvar;
		for (pvarLHS = &(it->second); plhs->alt && plhs->alt->type == N_STRUCT; plhs = plhs->alt) {
			*pstruct = plhs->alt;
			if (is_reserved_class_member_name(plhs->alt->str))
				throw exception_misuse(*this, plhs->alt, string(".") + plhs->alt->str + " is reserved for internal class metadata.").raise();
			itvar = ((CVar*)pvarLHS)->strut.find(plhs->alt->str);
			if (itvar == pvarLHS->strut.end()) {
				if (!class_has_declared_member_process(pEnv, pvarLHS, plhs->alt->str))
					throw exception_misuse(*this, plhs->alt, string("Cannot add undeclared member .") + plhs->alt->str + " to class object.").raise();
				bool wantRight = !strcmp(plhs->alt->str, "right");
				if (ISAUDIO(pvarLHS->type()) && (wantRight || !strcmp(plhs->alt->str, "left"))) {
					// .left/.right on the LHS select a single channel to write into directly
					// (not a copy, unlike the read-side left()/right() builtins which mutate Sig).
					if (!plhs->alt->alt)
						throw exception_etc(*this, plhs, string(".") + plhs->alt->str + " must be used with an index on the LHS, e.g. x." + plhs->alt->str + "(t1~t2) = ...").raise();
					if (plhs->alt->alt->type == N_CELL)
						throw exception_etc(*this, plhs, "Temporal block selection is read-only.").raise();
					if (plhs->alt->alt->type != N_ARGS && plhs->alt->alt->type != N_TIME_EXTRACT)
						throw exception_etc(*this, plhs, string(".") + plhs->alt->str + " on the LHS must be followed directly by an index, e.g. x." + plhs->alt->str + "(t1~t2) = ...").raise();
					if (!pvarLHS->next)
						throw exception_etc(*this, plhs, string(".") + plhs->alt->str + " requires a stereo signal.").raise();
					// Keep pvarLHS pointing at the whole (real) stereo CVar, for both .left and .right:
					// pvarLHS->next is allocated as a plain CSignals (see SetNextChan), not a CVar, so
					// it must never be reinterpreted as a CVar*. Length/endpoint/type computations use
					// pvarLHS as-is (left and right channels share sample count); the actual buffer
					// write is redirected to the real pvarLHS->next pointer only in adjust_buf/insertreplace.
					channelWriteSel = wantRight ? 2 : 1;
					continue;
				}
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
void AuxScope::adjust_buf(body& lvar, const body& lhs_index, const body& robj, bool contig, const AstNode* pn)
{
	const size_t elemSize = lvar.bufBlockSize;
	if (robj.nSamples == 0)
	{ // truncate the LHS var buffer
		if (contig)
		{
			memmove(lvar.logbuf + elemSize * ((uint64_t)lhs_index.buf[0] - 1),
			        lvar.logbuf + elemSize * ((uint64_t)lhs_index.buf[lhs_index.nSamples - 1]),
			        (lvar.nSamples - (uint64_t)lhs_index.buf[0] - lhs_index.nSamples + 1) * elemSize);
			lvar.nSamples -= lhs_index.nSamples;
		}
		else
		{
			vector<uint64_t> positions;
			positions.reserve(lhs_index.nSamples);
			for (uint64_t k = 0; k < lhs_index.nSamples; ++k)
				positions.push_back((uint64_t)lhs_index.buf[k] - 1);
			sort(positions.begin(), positions.end());
			positions.erase(unique(positions.begin(), positions.end()), positions.end());
			for (auto it = positions.rbegin(); it != positions.rend(); ++it)
			{
				const uint64_t pos = *it;
				memmove(lvar.logbuf + elemSize * pos,
				        lvar.logbuf + elemSize * (pos + 1),
				        (lvar.nSamples - pos - 1) * elemSize);
				--lvar.nSamples;
			}
		}
	}
	else if (robj.nSamples == 1)
	{ // fill the buffer with the RHS value 
		for (uint64_t k = 0; k < lhs_index.nSamples; k++)
			memcpy(lvar.logbuf + elemSize * ((uint64_t)lhs_index.buf[k] - 1), robj.logbuf, elemSize);
	}
	else if (lhs_index.nSamples == 1)
	{
		auto nCopied = lvar.nSamples + 1 - (uint16_t)lhs_index.buf[0];
		lvar.UpdateBuffer(lvar.nSamples + robj.nSamples);
		auto j = (uint64_t)lhs_index.buf[0] - 1;
		memmove(lvar.logbuf + elemSize * (j + robj.nSamples),
		        lvar.logbuf + elemSize * j,
		        nCopied * elemSize);
		memcpy(lvar.logbuf + elemSize * j, robj.logbuf, robj.nSamples * elemSize);
	}
	else if (lhs_index.nSamples == robj.nSamples) {
		if (contig)
			memmove(lvar.logbuf + elemSize * ((uint64_t)lhs_index.buf[0] - 1), robj.logbuf, elemSize * robj.nSamples);
		else
			for (uint64_t k = 0; k < robj.nSamples; k++)
				memcpy(lvar.logbuf + elemSize * ((uint64_t)lhs_index.buf[k] - 1),
				       robj.logbuf + elemSize * k,
				       elemSize);
	}
	else
		throw exception_etc(*this, pn, "Unexpected case").raise();
}

static bool is_contiguous_increasing_index(const body& index)
{
	for (uint64_t k = 1; k < index.nSamples; ++k)
		if (index.buf[k - 1] + 1. != index.buf[k])
			return false;
	return true;
}

static void extract_indexed_block(CTimeSeries& out, const body& index,
	const CTimeSeries& source, bool preserveAudio, bool preserveTimeline)

{
	out.Reset(preserveAudio ? source.GetFs() : 1);
	out.bufType = source.bufType;
	out.bufBlockSize = source.bufBlockSize;
	out.UpdateBuffer(index.nSamples);
	out.nGroups = index.nGroups;
	out.snap = source.snap;
	if (preserveAudio && index.nSamples > 0)
		out.tmark = source.tmark + (index.buf[0] - 1.) * 1000. / source.GetFs();
	else if (preserveTimeline)
		out.tmark = source.tmark;

	if (index.nSamples == 0)
		return;
	if (index._max() > source.nSamples || index._min() < 1)
		throw "Index exceeds at least one chained block.";

	const bool contiguous = is_contiguous_increasing_index(index);
	const size_t firstOffset = source.bufBlockSize * ((uint64_t)index.buf[0] - 1);
	if (contiguous)
	{
		memmove(out.logbuf, source.logbuf + firstOffset, source.bufBlockSize * index.nSamples);
	}
	else
	{
		for (uint64_t k = 0; k < index.nSamples; k++)
		{
			const size_t srcOffset = source.bufBlockSize * ((uint64_t)index.buf[k] - 1);
			memcpy(out.logbuf + k * out.bufBlockSize, source.logbuf + srcOffset, out.bufBlockSize);
		}
	}

}

static void extract_indexed_channel(CTimeSeries& out, const CVar& index,
	const CTimeSeries& source, bool preserveAudio)
{
	const bool perBlockIndex = index.chain != NULL;
	const bool preserveTimeline = source.chain != NULL;
	const CTimeSeries* sourcePart = &source;
	const CTimeSeries* indexPart = &index;
	CTimeSeries* outPart = &out;
	while (sourcePart)
	{
		if (!indexPart)
			throw "Index chain does not match the source chain.";
		extract_indexed_block(*outPart, *indexPart, *sourcePart,
			preserveAudio && is_contiguous_increasing_index(*indexPart), preserveTimeline);
		sourcePart = sourcePart->chain;
		if (perBlockIndex)
			indexPart = indexPart->chain;
		if (sourcePart)
		{
			outPart->chain = new CTimeSeries;
			outPart = outPart->chain;
		}
	}
	if (perBlockIndex && indexPart)
		throw "Index chain does not match the source chain.";

}

void AuxScope::extract_by_index(CVar& out, const CVar& index, const CVar& obj, bool contig)
{
	(void)contig;
	out.Reset();
	extract_indexed_channel(out, index, obj, ISAUDIO(obj.type()));
	if (obj.next)
	{
		CSignals right;
		extract_indexed_channel(right, index, *obj.next, ISAUDIO(obj.type()));
		out.SetNextChan(right);
	}
}

static bool extract_audio_channel_by_logical_condition(CSignals& out, const CTimeSeries& selector, const CTimeSeries& source)
{
	if (selector.GetFs() != source.GetFs())
		return false;

	CSignals selected(source.GetFs());
	CTimeSeries* tail = NULL;
	const CTimeSeries* maskPart = &selector;
	const CTimeSeries* sourcePart = &source;
	while (maskPart && sourcePart)
	{
		if (!maskPart->IsLogical() || maskPart->nSamples != sourcePart->nSamples ||
			fabs(maskPart->tmark - sourcePart->tmark) > .5 * 1000. / source.GetFs())
			return false;

		uint64_t begin = 0;
		while (begin < maskPart->nSamples)
		{
			while (begin < maskPart->nSamples && !maskPart->logbuf[begin])
				++begin;
			if (begin == maskPart->nSamples)
				break;
			uint64_t end = begin + 1;
			while (end < maskPart->nSamples && maskPart->logbuf[end])
				++end;

			CTimeSeries part(source.GetFs());
			part.bufType = sourcePart->bufType;
			part.bufBlockSize = sourcePart->bufBlockSize;
			part.UpdateBuffer(end - begin);
			part.nGroups = 1;
			part.tmark = sourcePart->tmark + begin * 1000. / source.GetFs();
			part.snap = sourcePart->snap;
			memcpy(part.logbuf, sourcePart->logbuf + begin * sourcePart->bufBlockSize,
				(end - begin) * sourcePart->bufBlockSize);

			if (!tail)
			{
				selected = part;
				tail = &selected;
			}
			else
			{
				tail->chain = new CTimeSeries(part);
				tail = tail->chain;
			}
			begin = end;
		}
		maskPart = maskPart->chain;
		sourcePart = sourcePart->chain;
	}
	if (maskPart || sourcePart)
		return false;

	out = selected;
	return true;
}

bool AuxScope::extract_audio_by_logical_condition(CVar& out, const CVar& selector, const CVar& obj)
{
	if (!ISAUDIO(obj.type()) || !selector.IsLogical())
		return false;

	CSignals left;
	if (!extract_audio_channel_by_logical_condition(left, selector, obj))
		return false;

	CVar selected(left);
	if (obj.next)
	{
		CSignals right;
		if (!extract_audio_channel_by_logical_condition(right, selector, *obj.next))
			return false;
		selected.SetNextChan(right);
	}
	out = selected;
	return true;
}

static bool fill_time_range_with_scalar(CTimeSeries& signal, double beginMs, double endMs, const body& scalar)
{
	if (beginMs > endMs)
		swap(beginMs, endMs);
	bool wrote = false;
	for (CTimeSeries* part = &signal; part; part = part->chain)
	{
		if (part->GetFs() <= 0 || part->nSamples == 0)
			continue;
		const double partBegin = part->tmark;
		const double partEnd = partBegin + part->nSamples * 1000. / part->GetFs();
		const double overlapBegin = max(beginMs, partBegin);
		const double overlapEnd = min(endMs, partEnd);
		if (overlapBegin >= overlapEnd)
			continue;
		const double samplesPerMs = part->GetFs() / 1000.;
		uint64_t first = (uint64_t)ceil((overlapBegin - partBegin) * samplesPerMs - 1.e-9);
		uint64_t last = (uint64_t)ceil((overlapEnd - partBegin) * samplesPerMs - 1.e-9);
		first = min(first, part->nSamples);
		last = min(last, part->nSamples);
		for (uint64_t k = first; k < last; ++k)
			memcpy(part->logbuf + k * part->bufBlockSize, scalar.logbuf, part->bufBlockSize);
		wrote |= first < last;
	}
	return wrote;
}

/* Assume that lvar is either a scalar, vector, audiosig, or tseq and has already been evaluated
* lvar is being modified with robj according to lhs_index, which is either vector indices (1D or 2D) or time indices
* (also assume that type checking of robj and lvar has been done)
* Note that if RHS has a replica, robj is not an input (it should be NULL) and Compute(prhs) is done here
*/
void AuxScope::mod_sig(CVar& lvar, const CVar& lhs_index, const CVar& robj, bool contig, const AstNode* plhs, const AstNode* prhs)
{
	(void)contig;
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
		if (!isreplica && robj.nSamples == 1 && !robj.chain)
		{
			const double beginMs = lhs_index.buf[0];
			const double endMs = lhs_index.buf[1];
			bool wrote = false;
			if (channelWriteSel != 2)
				wrote |= fill_time_range_with_scalar(lvar, beginMs, endMs, robj);
			if (lvar.next && channelWriteSel != 1)
			{
				const body& rightScalar = robj.next ? static_cast<const body&>(*robj.next) : static_cast<const body&>(robj);
				const CTimeSeries* rightIndex = lhs_index.next ? lhs_index.next : &lhs_index;
				wrote |= fill_time_range_with_scalar(*lvar.next, rightIndex->buf[0], rightIndex->buf[1], rightScalar);
			}
			if (!wrote)
				throw exception_etc(*this, plhs, "Time indexing out of range").raise();
			return;
		}
		if (isreplica) { //RL-T
			if (channelWriteSel == 2)
				replica = *lvar.next;
			else
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
		auto applyDirect = [&](CTimeSeries& target, const CTimeSeries& rhsRoot)
		{
			const bool perBlockIndex = lhs_index.chain != NULL;
			const bool perBlockRhs = rhsRoot.chain != NULL;
			CTimeSeries* targetPart = &target;
			const CTimeSeries* indexPart = &lhs_index;
			const CTimeSeries* rhsPart = &rhsRoot;
			while (targetPart)
			{
				if (!indexPart || !rhsPart)
					throw exception_etc(*this, plhs, "Indexed assignment chain dimensions do not agree.").raise();
				if (rhsPart->nSamples > 1 && indexPart->nSamples > 1 &&
					(indexPart->nSamples != rhsPart->nSamples || indexPart->nGroups != rhsPart->nGroups))
					throw exception_etc(*this, plhs, "LHS and RHS have different dimension (lengths).").raise();
				adjust_buf(*targetPart, *indexPart, *rhsPart,
					is_contiguous_increasing_index(*indexPart), pn);
				targetPart = targetPart->chain;
				if (perBlockIndex) indexPart = indexPart->chain;
				if (perBlockRhs) rhsPart = rhsPart->chain;
			}
			if ((perBlockIndex && indexPart) || (perBlockRhs && rhsPart))
				throw exception_etc(*this, plhs, "Indexed assignment chain dimensions do not agree.").raise();
		};

		auto applyReplica = [&](CTimeSeries& target)
		{
			const bool perBlockIndex = lhs_index.chain != NULL;
			CTimeSeries* targetPart = &target;
			const CTimeSeries* indexPart = &lhs_index;
			while (targetPart)
			{
				if (!indexPart)
					throw exception_etc(*this, plhs, "Index chain does not match the source chain.").raise();
				CVar sourceBlock;
				sourceBlock.CTimeSeries::operator=(static_cast<const CSignal&>(*targetPart));
				CVar indexBlock;
				indexBlock.CTimeSeries::operator=(static_cast<const CSignal&>(*indexPart));
				extract_by_index(replica, indexBlock, sourceBlock,
					is_contiguous_increasing_index(*indexPart));
				CVar computed = *Compute(prhs);
				adjust_buf(*targetPart, *indexPart, computed,
					is_contiguous_increasing_index(*indexPart), pn);
				replica.Reset();
				targetPart = targetPart->chain;
				if (perBlockIndex) indexPart = indexPart->chain;
			}
			if (perBlockIndex && indexPart)
				throw exception_etc(*this, plhs, "Index chain does not match the source chain.").raise();
		};

		if (isreplica)
		{
			if (channelWriteSel != 2)
				applyReplica(lvar);
			if (lvar.next && channelWriteSel != 1)
				applyReplica(*lvar.next);
		}
		else
		{
			if (channelWriteSel != 2)
				applyDirect(lvar, robj);
			if (lvar.next && channelWriteSel != 1)
				applyDirect(*lvar.next, robj.next ? static_cast<const CTimeSeries&>(*robj.next)
				                                  : static_cast<const CTimeSeries&>(robj));
		}
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
		if (!ISSCALARG(Sig.type()) || !std::isfinite(Sig.value()) ||
			Sig.value() < 1. || std::floor(Sig.value()) != Sig.value() ||
			Sig.value() >= std::ldexp(1., std::numeric_limits<size_t>::digits))
			throw exception_etc(*this, p, "Brace index must be a positive integer scalar.").raise();
		p->alt->dval = Sig.value();
	}
}

CVar* AuxScope::process_statement(const AstNode* pnode)
{
	publish_deferred();
	const AstNode* plhs;
	const AstNode* prhs;
	bool isreplica = get_nodes_left_right_sides(pnode, &plhs, &prhs);
	if (pnode->suppress & AST_SUPPRESS_ASYNC_ASSIGN) {
		if (!plhs || plhs->type != T_ID || plhs->alt || level != 1 || dad)
			throw exception_etc(*this, pnode, "<- requires a plain top-level destination.").raise();
		u.pending_assign_lhs = nullptr;
		u.pending_assign_rhs_call = nullptr;
		if (auto* value = try_start_progressive(plhs, prhs)) return value;
		return start_deferred(plhs, prhs);
	}
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
		if (RHS.progressive) { // Ordinary assignment freezes the current prefix.
			RHS.progressive.reset();
			RHS.strut.erase("h");
			Sig = RHS;
		}
		uint16_t typelhs;
		bool contig;
		CVar index;
		// channelWriteSel is per-assignment state set only on the .left/.right LHS path; clear it
		// before and after (including on throw) so it never leaks into a later plain indexed write.
		struct ChannelWriteSelReset {
			int& sel;
			~ChannelWriteSelReset() { sel = 0; }
		} channelWriteSelReset{ channelWriteSel };
		channelWriteSel = 0;
		sanitize_cell_node(plhs);
		eval_lhs(plhs, prhs, index, RHS, typelhs, contig, isreplica);
		right_to_left(plhs, index, RHS, typelhs, contig, isreplica ? prhs:NULL);
		if (plhs->type == T_ID && plhs->alt) {
			forget_assignment(plhs->str);
			auto dest = Vars.find(plhs->str);
			if (dest != Vars.end() && dest->second.progressive) {
				dest->second.progressive.reset();
				dest->second.strut.erase("h");
			}
		}
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
			if (channelWriteSel)
				((CTimeSeries*)(channelWriteSel == 2 ? lobj->next : lobj))->CTimeSeries::ReplaceBetweenTPs(robj, indsig.buf[0], indsig.buf[1]);
			else
				replace(*lobj, indsig, robj, *this, plhs);
		}
		else if (channelWriteSel)
			// .left/.right-scoped write: lobj is always the whole stereo variable itself (see
			// get_available_struct_item), and lobj->next is the real *other* channel of that same
			// variable (not a copy) -- whichever side isn't targeted must stay untouched. Bypass
			// CSignals::ReplaceBetweenTPs (which fans a mono RHS out to both channels) and call the
			// base CTimeSeries implementation directly on just the targeted channel's CSignals node.
			// (lobj->next is a plain CSignals*, never a CVar*, so it's used as CTimeSeries* only.)
			((CTimeSeries*)(channelWriteSel == 2 ? lobj->next : lobj))->CTimeSeries::ReplaceBetweenTPs(robj, indsig.buf[0], indsig.buf[1]);
		else
			lobj->ReplaceBetweenTPs(robj, indsig);
	}
}
