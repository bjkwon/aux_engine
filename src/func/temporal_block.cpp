#include "functions_common.h"

Cfunction set_builtin_function_blockat(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	ft.desc_arg_req = { "temporal_obj", "timepoint" };
	ft.allowed_arg_types.push_back({ 0xFFFF });
	ft.allowed_arg_types.push_back({ 1 });
	ft.narg1 = 2;
	ft.narg2 = 2;
	return ft;
}

void _blockat(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (pnode->type != N_STRUCT)
		throw exception_etc(*past, pnode, "blockat() must be called with dot notation.").raise();
	past->Sig = past->temporal_block_at(past->Sig, args.front().value(), pnode);
}
