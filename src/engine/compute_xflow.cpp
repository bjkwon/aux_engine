#include <algorithm>
#include "AuxScope.h"
#include "AuxScope_exception.h"
#include "typecheck.h"
#include "utils.h"
#include <assert.h>

CVar* EngineRuntime::BLOCK(AuxScope* psk, const AstNode* pnode)
{
	psk->linebyline(pnode->next);
	return &psk->Sig;
}

CVar* EngineRuntime::FOR(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	psk->fExit = psk->fBreak = false;
	CVar isig = psk->Compute(p->child);
	//isig must be a vector
	ensureVector3(*psk, p, isig, "For-loop index variable must be a vector.");
	//If index variable already exists in the scope, throw
	if (psk->GetVariable(p->str, NULL))
		exception_etc(*psk, p, " ""for"" Index variable already exists outside the for loop").raise();
	for (unsigned int i = 0; i < isig.nSamples && !psk->fExit && !psk->fBreak; i++)
	{
		CVar tp(isig.buf[i]);
		psk->SetVar(p->str, &tp); // This is OK, SetVar of non-GO object always makes a duplicate object (as opposed to SetVar of Go obj grabbing the reference)
		//	assuming that (pnode->alt->type == N_BLOCK)
		// Now, not going through N_BLOCK 1/4/2020
		// 1) When running in a debugger, it must go through N_BLOCK
		// 2) check if looping through pa->next is bullet-proof
		psk->linebyline(pnode->alt->next);
	}
	psk->fBreak = false;
	return &psk->Sig;
}

CVar* EngineRuntime::IF(AuxScope* psk, const AstNode* pnode)
{
	if (pnode) {
		AstNode* p = pnode->child;
		psk->pLast = p;
		if (psk->checkcond(p))
			psk->process_statement(p->next);
		else if (pnode->alt)
			psk->process_statement(pnode->alt);
	}
	return &psk->Sig;
}

CVar* EngineRuntime::WHILE(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	psk->fExit = psk->fBreak = false;
	while (psk->checkcond(p) && !psk->fExit && !psk->fBreak)
		psk->process_statement(pnode->alt);
	psk->fBreak = false;
	return &psk->Sig;
}

// switch variable should be one of the following:
// string
// a scalar (real or imaginary)
// TYPEBIT_TEMPO_ONE with nSamples=1
// 1. Compute the switch var. If the type is not allowed, exception.
// 2. Compute the case vars in the order listed. 
//    If the type is not allowed, exception.
//    If the type is not the same as switch var, skip
//    If the case var value is the same as switch value, continue N_BLOCK there
//        For TYPEBIT_TEMPO_ONE, fs is ignored
//    If all case vars are exhausted and a match is not found, continue otherwise N_BLOCK. If otherwise is not there, just skip.

CVar* EngineRuntime::SWITCH(AuxScope* psk, const AstNode* pnode)
{
	if (pnode) {
		AstNode* p = pnode->child;
		CVar sw_var = psk->Compute(p);
		uint16_t stype = sw_var.type();
		if (stype == TYPEBIT_REAL+1 || stype == TYPEBIT_STRING + 1 || stype == TYPEBIT_STRING + 2 || stype == TYPEBIT_COMPLEX+1 || stype == TYPEBIT_TEMPO_ONE + 1) {
			p = pnode->alt; // case statement
			for (; p; p = p->next->alt) {
				if (p->type == T_OTHERWISE) {
					psk->process_statement(p->next);
					break;
				}
				else {
					CVar cs_var = psk->Compute(p);
					uint16_t ctype = cs_var.type();
					if (ctype != TYPEBIT_REAL + 1 && ctype != TYPEBIT_STRING + 1 && ctype != TYPEBIT_STRING + 2 && ctype != TYPEBIT_COMPLEX + 1 && ctype != TYPEBIT_TEMPO_ONE + 1) {
						ostringstream oss;
						oss << "TYPE=" << ctype << " not allowed for case statement in switch";
						throw exception_etc(psk, pnode, oss.str()).raise();
					}
					if (ctype == TYPEBIT_STRING + 1 || ctype == TYPEBIT_STRING + 2) {
						if (sw_var.str() == cs_var.str()) {
							psk->process_statement(p->next);
							break;
						}
						else 
							continue;
					}
					else {
						if (ctype != stype || sw_var.nSamples != cs_var.nSamples || sw_var.value() != cs_var.value())
							continue;
					}
					if (ctype == TYPEBIT_COMPLEX && sw_var.cvalue() != cs_var.cvalue())
						continue;
					// if it is still here, execute the current case block
						psk->process_statement(p->next);
					break;
				}
			}
		}
		else {
			ostringstream oss;
			oss << "TYPE=" << stype << " not allowed for switch statement";
			throw exception_etc(psk, pnode, oss.str()).raise();
		}
	}
	return &psk->Sig;
}

CVar* EngineRuntime::TRY(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	inTryCatch++;
	psk->Try_here(pnode, p);
	return &psk->Sig;
}

CVar* EngineRuntime::CATCH(AuxScope* psk, const AstNode* pnode)
{
	// AstNode* p = pnode->child; // not necessary
	// p is T_ID for the catch variable (exception message caught), handled in catch{} in Try_here
	inTryCatch--;
	psk->process_statement(pnode->next);
	return &psk->Sig;
}

CVar* EngineRuntime::ID(AuxScope* psk, const AstNode* pnode)
{
	return psk->TID((AstNode*)pnode, pnode->child);
}

CVar* EngineRuntime::TSEQ(AuxScope* psk, const AstNode* pnode)
{
	return psk->TSeq((AstNode*)pnode, pnode->child);
}

CVar* EngineRuntime::NUMBER(AuxScope* psk, const AstNode* pnode)
{
	psk->Sig.SetValue(pnode->dval);
	return psk->TID(pnode->alt, pnode->child, &psk->Sig);
}

CVar* EngineRuntime::STRING(AuxScope* psk, const AstNode* pnode)
{
	psk->Sig.Reset();
	psk->Sig.SetString(pnode->str);
	return psk->TID(pnode->alt, pnode->child, &psk->Sig);
}

CVar* EngineRuntime::MATRIX(AuxScope* psk, const AstNode* pnode)
{
	if (pnode->child) psk->throw_LHS_lvalue(pnode, false);
	psk->NodeMatrix(pnode);
	return psk->Dot(pnode->alt, &psk->Sig);
}

CVar* EngineRuntime::VECTOR(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	if (p)
	{ // if p (RHS exists), evaluate RHS first; then go to LHS (different from usual ways)
		string emsg;
		string funcname;
		if (p->type == T_ID)
			funcname = p->str;
		if (p->alt && p->alt->type == N_STRUCT)
			funcname = p->alt->str;
		// Now, evaluate RHS
		// why not TID(((AstNode*)pnode->str), p), which might be more convenient? (that's the "inner" N_VECTOR node)
		// Because then there's no way to catch [out1 out2].sqrt = func
		return psk->TID((AstNode*)pnode, p);
	}
	else
	{
		psk->NodeVector(pnode);
		return psk->Dot(pnode->alt, &psk->Sig);
	}
}

CVar* EngineRuntime::REPLICA(AuxScope* psk, const AstNode* pnode)
{
	return psk->TID((AstNode*)pnode, NULL, &psk->replica); //Make sure replica has been prepared prior to this
}

CVar* EngineRuntime::ENDPOINT(AuxScope* psk, const AstNode* pnode)
{
	CVar tsig;
	tsig.SetValue(psk->ends.back());
	return psk->TID((AstNode*)pnode, NULL, &tsig); //Make sure endpoint has been prepared prior to this
}

CVar* EngineRuntime::ARITH_PLUS(AuxScope* psk, const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	psk->Sig += tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::ARITH_MINUS(AuxScope* psk, const AstNode* pnode)
{
	CVar tsig;
	AstNode* p = pnode->child;
	tsig = -*psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.);
	if (tsig.type() == 0) tsig.SetValue(0.);
	psk->Sig += tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::ARITH_MULT(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	psk->Sig *= tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::ARITH_DIV(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	if (psk->Sig.type() == 0) psk->Sig.SetValue(0.f);
	if (tsig.type() == 0) tsig.SetValue(0.f);
	// reciprocal should be after blocking string (or it would corrupt the heap) 6/3/2020
	psk->Sig.reciprocal();
	psk->Sig *= tsig;
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::MATRIXMULT(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p);
	blockCellStruct2(*psk, pnode, psk->Sig);
	blockString2(*psk, pnode, psk->Sig);
	ensureVector1(*psk, pnode, tsig);
	psk->Compute(p->next);
	ensureVector1(*psk, pnode, psk->Sig);
	psk->Sig = (CSignals)tsig.matrixmult(&psk->Sig);
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* EngineRuntime::ARITH_MOD(AuxScope* psk, const AstNode* pnode)
{
	//only in the format of A %= B
	AstNode* p = pnode->child;
	((AstNode*)pnode)->type = N_CALL;
	((AstNode*)pnode)->str = strdup("mod");
	((AstNode*)pnode)->alt = p->next;
	p->next = NULL;
	psk->Sig = psk->replica;
	psk->HandleAuxFunction(pnode); // Assuming that current body content (psk->Sig) is already prepared...is it true? 8/23/2018
	return &psk->Sig;
}

CVar* EngineRuntime::TRANSPOSE(AuxScope* psk, const AstNode* pnode)
{
	psk->Transpose(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::NEGATIVE(AuxScope* psk, const AstNode* pnode)
{
	-*psk->Compute(pnode->child);
	blockString2(*psk, pnode, psk->Sig);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::TIMESHIFT(AuxScope* psk, const AstNode* pnode)
{
	AstNode* p = pnode->child;
	CVar tsig = psk->Compute(p->next);
	blockCellStruct2(*psk, pnode, psk->Sig);
	ensureScalar1(*psk, pnode, psk->Sig, ">>");
	psk->Compute(p);
	ensureAudio1(*psk, pnode, psk->Sig, ">>");
	psk->Sig >>= tsig.value();
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* EngineRuntime::CONCAT(AuxScope* psk, const AstNode* pnode)
{
	psk->Concatenate(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::LOGIC(AuxScope* psk, const AstNode* pnode)
{
	psk->ConditionalOperation(pnode, pnode->child);
	return psk->TID((AstNode*)pnode, NULL, &psk->Sig);
}

CVar* EngineRuntime::LEVELAT(AuxScope* psk, const AstNode* pnode)
{
	psk->SetLevel(pnode, pnode->child);
	return psk->TID((AstNode*)pnode->alt, NULL, &psk->Sig);
}

CVar* EngineRuntime::INITCELL(AuxScope* psk, const AstNode* pnode)
{
	return psk->InitCell(pnode, pnode->child);
}

CVar* EngineRuntime::BREAK(AuxScope* psk, const AstNode* pnode)
{
	psk->fBreak = true;
	return &psk->Sig;
}

CVar* EngineRuntime::RETURN(AuxScope* psk, const AstNode* pnode)
{
	psk->fExit = true;
	return &psk->Sig;
}
