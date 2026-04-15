#include "functions_common.h"

namespace {

void set_handle_result(CVar& out, std::uint64_t id)
{
	out.Reset(1);
	out.SetValue(static_cast<auxtype>(id));
	out.MarkHandle(true);
}

const AstNode* first_arg_node(const AstNode* pnode)
{
	if (!pnode || !pnode->alt) return nullptr;
	if (pnode->alt->type == N_ARGS) return pnode->alt->child;
	if (pnode->alt->type == N_HOOK) return pnode->alt;
	return nullptr;
}

const AstNode* nth_arg_node(const AstNode* pnode, int index)
{
	if (index < 0 || !pnode || !pnode->alt) return nullptr;
	const AstNode* node = nullptr;
	if (pnode->alt->type == N_ARGS) node = pnode->alt->child;
	else if (pnode->alt->type == N_HOOK) node = pnode->alt;
	else return nullptr;
	for (int i = 0; node && i < index; ++i)
		node = node->next;
	return node;
}

string simple_arg_source(const AstNode* node)
{
	if (!node || !node->str || !*node->str || node->child || node->alt)
		return {};
	return string(node->str);
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
	set_handle_result(past->Sig, id);
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
	string err;

	if (args.empty()) {
		if (!past->pEnv->graphics_backend.create_figure)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide figure creation yet.").raise();

		const uint64_t id = past->pEnv->graphics_backend.create_figure(past->pEnv->graphics_backend.userdata, err);
		if (id == 0) {
			if (err.empty()) err = "Failed to create figure.";
			throw exception_etc(*past, pnode, err).raise();
		}
		set_handle_result(past->Sig, id);
		return;
	}

	if (args.size() != 1)
		throw exception_etc(*past, pnode, "figure() requires a handle, position, or source name.").raise();

	const CVar& arg = args.front();
	const uint16_t tp = arg.type();

	if (ISSCALAR(tp)) {
		if (!past->pEnv->graphics_backend.figure_from_handle)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide figure(handle) support yet.").raise();

		const double handleValue = arg.value();
		const double rounded = std::round(handleValue);
		if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
			throw exception_etc(*past, pnode, "invalid figure argument").raise();

		const uint64_t id = past->pEnv->graphics_backend.figure_from_handle(
			past->pEnv->graphics_backend.userdata,
			static_cast<uint64_t>(rounded),
			err);
		if (id == 0) {
			if (err.empty()) err = "Failed to resolve figure(handle).";
			throw exception_etc(*past, pnode, err).raise();
		}
		set_handle_result(past->Sig, id);
		return;
	}

	if (ISVECTOR(tp) && !ISSTRING(tp) && !ISAUDIO(tp) && arg.nSamples == 4) {
		if (!past->pEnv->graphics_backend.figure_at_pos)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide figure(pos) support yet.").raise();

		const vector<auxtype> posVec = arg.ToVector();
		if (posVec.size() != 4)
			throw exception_etc(*past, pnode, "figure() requires a 4-element position vector.").raise();
		double pos[4] = { posVec[0], posVec[1], posVec[2], posVec[3] };
		const uint64_t id = past->pEnv->graphics_backend.figure_at_pos(
			past->pEnv->graphics_backend.userdata,
			pos,
			err);
		if (id == 0) {
			if (err.empty()) err = "Failed to create figure.";
			throw exception_etc(*past, pnode, err).raise();
		}
		set_handle_result(past->Sig, id);
		return;
	}

	if (ISSTRING(tp)) {
		if (!past->pEnv->graphics_backend.named_figure)
			throw exception_etc(*past, pnode, "The active graphics backend does not provide figure(name) support yet.").raise();

		const string sourceName = arg.str();
		if (sourceName.empty())
			throw exception_etc(*past, pnode, "figure() requires a non-empty source name.").raise();

		const uint64_t id = past->pEnv->graphics_backend.named_figure(
			past->pEnv->graphics_backend.userdata,
			sourceName.c_str(),
			err);
		if (id == 0) {
			if (err.empty()) err = "Failed to create named figure.";
			throw exception_etc(*past, pnode, err).raise();
		}
		set_handle_result(past->Sig, id);
		return;
	}

	throw exception_etc(*past, pnode, "figure() requires a handle, position, or source name.").raise();
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
		set_handle_result(past->Sig, id);
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
		set_handle_result(past->Sig, id);
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
		set_handle_result(past->Sig, id);
		return;
	}

	throw exception_etc(*past, pnode, "axes() requires a handle or position.").raise();
}

void _plot(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	if (!past->pEnv->graphics_backend.plot)
		throw exception_etc(*past, pnode, "The active graphics backend does not provide plot() support yet.").raise();

	int actualCount = 0;
	for (const AstNode* an = first_arg_node(pnode); an; an = an->next)
		++actualCount;
	if (actualCount <= 0 || actualCount > 3 || args.size() < static_cast<size_t>(actualCount))
		throw exception_etc(*past, pnode, "currently supported forms are plot(x), plot(x,\"style\"), plot(h,x), and plot(h,x,\"style\").").raise();

	const CVar* plotObj = nullptr;
	uint64_t targetHandle = 0;
	string styleText;
	int plotArgIndex = 0;
	const CVar* firstActual = &past->Sig;

	if (actualCount == 1) {
		plotObj = firstActual;
		plotArgIndex = 0;
	}
	else if (actualCount == 2) {
		if (ISSTRING(args[0].type())) {
			plotObj = firstActual;
			styleText = args[0].str();
			plotArgIndex = 0;
		}
		else if (ISSCALAR(firstActual->type())) {
			const double handleValue = firstActual->value();
			const double rounded = std::round(handleValue);
			if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
				throw exception_etc(*past, pnode, "currently supported forms are plot(x), plot(x,\"style\"), plot(h,x), and plot(h,x,\"style\").").raise();
			targetHandle = static_cast<uint64_t>(rounded);
			plotObj = &args[0];
			plotArgIndex = 1;
		}
		else {
			throw exception_etc(*past, pnode, "currently supported forms are plot(x), plot(x,\"style\"), plot(h,x), and plot(h,x,\"style\").").raise();
		}
	}
	else {
		if (!ISSCALAR(firstActual->type()) || !ISSTRING(args[1].type()))
			throw exception_etc(*past, pnode, "currently supported forms are plot(x), plot(x,\"style\"), plot(h,x), and plot(h,x,\"style\").").raise();
		const double handleValue = firstActual->value();
		const double rounded = std::round(handleValue);
		if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
			throw exception_etc(*past, pnode, "currently supported forms are plot(x), plot(x,\"style\"), plot(h,x), and plot(h,x,\"style\").").raise();
		targetHandle = static_cast<uint64_t>(rounded);
		plotObj = &args[0];
		styleText = args[1].str();
		plotArgIndex = 1;
	}

	string sourceExpr = simple_arg_source(nth_arg_node(pnode, plotArgIndex));
	string err;
	const uint64_t id = past->pEnv->graphics_backend.plot(
		past->pEnv->graphics_backend.userdata,
		targetHandle,
		reinterpret_cast<AuxObj>(plotObj),
		sourceExpr.empty() ? nullptr : sourceExpr.c_str(),
		styleText.empty() ? nullptr : styleText.c_str(),
		err);
	if (id == 0) {
		if (err.empty()) err = "Failed to create plot.";
		throw exception_etc(*past, pnode, err).raise();
	}
	set_handle_result(past->Sig, id);
}

void _line(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv || !past->pEnv->graphics_backend.notify)
		throw exception_etc(*past, pnode, "Graphics backend not available in this frontend.").raise();
	if (!past->pEnv->graphics_backend.line)
		throw exception_etc(*past, pnode, "The active graphics backend does not provide line() support yet.").raise();

	int actualCount = 0;
	for (const AstNode* an = first_arg_node(pnode); an; an = an->next)
		++actualCount;
	if (actualCount <= 0 || actualCount > 3 || args.size() < static_cast<size_t>(actualCount))
		throw exception_etc(*past, pnode, "supported forms are line(x), line(x,y), line(h,x), and line(h,x,y).").raise();

	uint64_t targetHandle = 0;
	const CVar* xObj = nullptr;
	const CVar* yObj = nullptr;
	const CVar* firstActual = &past->Sig;

	if (actualCount == 1) {
		xObj = firstActual;
	}
	else if (actualCount == 2) {
		if (ISSCALAR(firstActual->type())) {
			const double handleValue = firstActual->value();
			const double rounded = std::round(handleValue);
			if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
				throw exception_etc(*past, pnode, "supported forms are line(x), line(x,y), line(h,x), and line(h,x,y).").raise();
			targetHandle = static_cast<uint64_t>(rounded);
			xObj = &args[0];
		}
		else {
			xObj = firstActual;
			yObj = &args[0];
		}
	}
	else {
		if (!ISSCALAR(firstActual->type()))
			throw exception_etc(*past, pnode, "supported forms are line(x), line(x,y), line(h,x), and line(h,x,y).").raise();
		const double handleValue = firstActual->value();
		const double rounded = std::round(handleValue);
		if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
			throw exception_etc(*past, pnode, "supported forms are line(x), line(x,y), line(h,x), and line(h,x,y).").raise();
		targetHandle = static_cast<uint64_t>(rounded);
		xObj = &args[0];
		yObj = &args[1];
	}

	string err;
	const uint64_t id = past->pEnv->graphics_backend.line(
		past->pEnv->graphics_backend.userdata,
		targetHandle,
		reinterpret_cast<AuxObj>(xObj),
		reinterpret_cast<AuxObj>(yObj),
		err);
	if (id == 0) {
		if (err.empty()) err = "Failed to create line.";
		throw exception_etc(*past, pnode, err).raise();
	}
	set_handle_result(past->Sig, id);
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
