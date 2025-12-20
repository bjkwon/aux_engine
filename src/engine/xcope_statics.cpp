#include "AuxScope.h"

const AstNode* AuxScope::find_parent(const AstNode* p, const AstNode* a)
{
	// last changed 6/12/2022 to accommodate [out,state]=x.filt
	if (p->type == N_VECTOR)
		return (const AstNode*)p->str;
	// search a from nodes begining at p
	if (p->child == a) return p;
	if (p->alt == a) return p;
	if (p->next == a) return p;
	const AstNode* q = NULL;
	if (p->child)
		if ((q = find_parent(p->child, a))) return q;
	if (p->alt)
		if ((q = find_parent(p->alt, a))) return q;
	if (p->next)
		if ((q = find_parent(p->next, a))) return q;
	return NULL;
}

AstNode* AuxScope::goto_line(const AstNode* pnode, int line)
{
	AstNode* pp, * p = (AstNode*)pnode;
	for (; p; p = p->next)
	{
		if (p->line == line) return p;
		if (p->type == T_FOR || p->type == T_WHILE) // line should be inside of block, i.e., T_FOR T_IF or T_WHILE
		{
			pp = AuxScope::goto_line((const AstNode*)p->alt, line);
			if (pp) return pp;
		}
		else if (p->type == T_IF || p->type == T_TRY)
		{
			pp = AuxScope::goto_line((const AstNode*)p->child->next, line);
			if (pp) return pp;
		}
	}
	return p;
}

bool AuxScope::IsLooping(const AstNode* pnode)
{
	if (pnode->type == T_IF || pnode->type == T_FOR || pnode->type == T_WHILE) return true;
	return false;
}
