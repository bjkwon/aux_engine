#include "functions_common.h"

namespace {

const AstNode* first_arg_node(const AstNode* pnode)
{
	if (!pnode || !pnode->alt) return nullptr;
	if (pnode->alt->type == N_ARGS) return pnode->alt->child;
	if (pnode->alt->type == N_HOOK) return pnode->alt;
	return nullptr;
}

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

void set_current_handle_result(AuxScope* past,
                               const AstNode* pnode,
                               auxGraphicsCurrentHandleHook hook,
                               const char* name)
{
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	if (!hook)
		throw exception_etc(*past, pnode, string(name) + " is registered in auxe, but the active graphics backend does not provide current-handle queries yet.").raise();

	const uint64_t id = hook(past->pEnv->graphics_backend.userdata);
	if (id == 0) {
		past->Sig.Reset();
		return;
	}
	past->Sig.SetValue(static_cast<auxtype>(id));
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
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	if (!args.empty())
		throw exception_etc(*past, pnode, "Only figure() without arguments has migrated to auxe so far.").raise();
	if (!past->pEnv->graphics_backend.create_figure)
		throw exception_etc(*past, pnode, "The active graphics backend does not provide figure creation yet.").raise();

	string err;
	const uint64_t id = past->pEnv->graphics_backend.create_figure(past->pEnv->graphics_backend.userdata, err);
	if (id == 0) {
		if (err.empty()) err = "Failed to create figure.";
		throw exception_etc(*past, pnode, err).raise();
	}
	past->Sig.SetValue(static_cast<auxtype>(id));
}

void _axes(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	string err;

	if (args.empty()) {
		if (!past->pEnv->graphics_backend.create_axes)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide axes creation yet.").raise();

		const uint64_t id = past->pEnv->graphics_backend.create_axes(past->pEnv->graphics_backend.userdata, err);
		if (id == 0) {
			if (err.empty()) err = "Failed to create axes.";
			throw exception_etc(*past, pnode, err).raise();
		}
		past->Sig.SetValue(static_cast<auxtype>(id));
		return;
	}

	if (args.size() != 1)
		throw exception_etc(*past, pnode, "axes() requires a handle or position.").raise();

	const CVar& arg = args.front();
	const uint16_t tp = arg.type();

	if (ISSCALAR(tp)) {
		if (!past->pEnv->graphics_backend.axes_from_handle)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide axes(handle) support yet.").raise();

		const double handleValue = arg.value();
		const double rounded = std::round(handleValue);
		if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
			throw exception_etc(*past, pnode, "invalid axes argument").raise();

		const uint64_t id = past->pEnv->graphics_backend.axes_from_handle(
			past->pEnv->graphics_backend.userdata,
			static_cast<uint64_t>(rounded),
			err);
		if (id == 0) {
			if (err.empty()) err = "Failed to resolve axes(handle).";
			throw exception_etc(*past, pnode, err).raise();
		}
		past->Sig.SetValue(static_cast<auxtype>(id));
		return;
	}

	if (ISVECTOR(tp) && !ISSTRING(tp) && !ISAUDIO(tp) && arg.nSamples == 4) {
		if (!past->pEnv->graphics_backend.axes_at_pos)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide axes(pos) support yet.").raise();

		const vector<auxtype> posVec = arg.ToVector();
		if (posVec.size() != 4)
			throw exception_etc(*past, pnode, "axes() requires a 4-element position vector.").raise();
		double pos[4] = { posVec[0], posVec[1], posVec[2], posVec[3] };
		const uint64_t id = past->pEnv->graphics_backend.axes_at_pos(
			past->pEnv->graphics_backend.userdata,
			pos,
			err);
		if (id == 0) {
			if (err.empty()) err = "Failed to create axes.";
			throw exception_etc(*past, pnode, err).raise();
		}
		past->Sig.SetValue(static_cast<auxtype>(id));
		return;
	}

	throw exception_etc(*past, pnode, "axes() requires a handle or position.").raise();
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
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	if (args.size() != 1)
		throw exception_etc(*past, pnode, "delete() requires a graphics handle.").raise();
	if (!past->pEnv->graphics_backend.delete_handle)
		throw exception_etc(*past, pnode, "The active graphics backend does not provide delete(handle) support yet.").raise();

	const CVar& arg = args.front();
	const uint16_t tp = arg.type();
	if (!ISSCALAR(tp))
		throw exception_etc(*past, pnode, "delete() requires a graphics handle.").raise();

	const double handleValue = arg.value();
	const double rounded = std::round(handleValue);
	if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
		throw exception_etc(*past, pnode, "delete() requires a graphics handle.").raise();

	string err;
	const int ok = past->pEnv->graphics_backend.delete_handle(
		past->pEnv->graphics_backend.userdata,
		static_cast<uint64_t>(rounded),
		err);
	if (!ok) {
		if (err.empty()) err = "Failed to delete graphics handle.";
		throw exception_etc(*past, pnode, err).raise();
	}

	const AstNode* argNode = first_arg_node(pnode);
	if (argNode && argNode->str && *argNode->str && !argNode->child && !argNode->alt &&
		past->pEnv->pseudo_vars.find(argNode->str) == past->pEnv->pseudo_vars.end()) {
		CVar empty;
		past->SetVar(argNode->str, &empty);
	}
	past->Sig.Reset();
}

void _gcf(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	(void)args;
	set_current_handle_result(past,
	                         pnode,
	                         (past && past->pEnv) ? past->pEnv->graphics_backend.current_figure : nullptr,
	                         "gcf");
}

void _gca(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	(void)args;
	set_current_handle_result(past,
	                         pnode,
	                         (past && past->pEnv) ? past->pEnv->graphics_backend.current_axes : nullptr,
	                         "gca");
}
