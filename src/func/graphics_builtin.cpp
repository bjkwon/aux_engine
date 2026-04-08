#include "functions_common.h"

namespace {

Cfunction make_graphics_builtin(fGate fp,
                                const vector<string>& desc_arg_req,
                                const vector<string>& desc_arg_opt)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg.assign(desc_arg_opt.size(), CVar());
	for (size_t i = 0; i < desc_arg_req.size() + desc_arg_opt.size(); ++i)
		ft.allowed_arg_types.push_back({ 0xFFFF });
	ft.narg1 = static_cast<int>(desc_arg_req.size());
	ft.narg2 = ft.narg1 + static_cast<int>(ft.defaultarg.size());
	return ft;
}

[[noreturn]] void throw_graphics_backend_error(AuxScope* past, const AstNode* pnode)
{
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	throw exception_etc(*past, pnode, string(pnode->str) + " is registered in auxe, but runtime graphics semantics have not been migrated yet.").raise();
}

} // namespace

Cfunction set_builtin_function_figure(fGate fp)
{
	return make_graphics_builtin(fp, {}, { "handle_or_pos_or_name" });
}

Cfunction set_builtin_function_axes(fGate fp)
{
	return make_graphics_builtin(fp, {}, { "handle_or_pos" });
}

Cfunction set_builtin_function_plot(fGate fp)
{
	return make_graphics_builtin(fp, { "x" }, { "y_or_style", "style", "target_handle" });
}

Cfunction set_builtin_function_line(fGate fp)
{
	return make_graphics_builtin(fp, { "x" }, { "y", "target_handle" });
}

Cfunction set_builtin_function_text(fGate fp)
{
	return make_graphics_builtin(fp, { "x", "y", "string" }, { "target_handle" });
}

Cfunction set_builtin_function_delete(fGate fp)
{
	return make_graphics_builtin(fp, { "object" }, {});
}

void _figure(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _axes(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _plot(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _line(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _text(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _delete(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _gcf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}

void _gca(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	throw_graphics_backend_error(past, pnode);
}
