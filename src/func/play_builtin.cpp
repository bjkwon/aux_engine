#include "functions_common.h"

#include <atomic>

namespace {

Cfunction make_play_builtin(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	ft.desc_arg_req = { "audio_or_handle" };
	ft.desc_arg_opt = { "audio_or_repeat", "repeat" };
	ft.defaultarg = { CVar(1.), CVar(1.) };
	for (int i = 0; i < 3; ++i)
		ft.allowed_arg_types.push_back({ 0xFFFF });
	ft.narg1 = 1;
	ft.narg2 = 3;
	return ft;
}

bool is_playback_handle(const CVar& value)
{
	return value.IsRuntimeHandle() &&
		value.strut.find("fs") != value.strut.end() &&
		value.strut.find("dur") != value.strut.end() &&
		value.strut.find("repeat_left") != value.strut.end() &&
		value.strut.find("prog") != value.strut.end();
}

bool is_audio_value(const CVar& value)
{
	const auto tp = value.type();
	return ISAUDIO(tp) || ISAUDIOG(tp);
}

const AstNode* first_arg_node(const AstNode* pnode)
{
	if (!pnode) return nullptr;
	return get_first_arg(pnode, pnode->type == N_STRUCT);
}

int parse_repeat_count(const CVar& value)
{
	if (!ISSCALARG(value.type()))
		return -1;
	const double rounded = std::round(value.value());
	if (rounded < 1.0 || std::fabs(value.value() - rounded) > 1e-9)
		return -1;
	return static_cast<int>(rounded);
}

double audio_duration_ms(const CVar& audio)
{
	return audio.alldur();
}

int audio_sample_rate(const CVar& audio)
{
	if (audio.GetFs() > 0)
		return audio.GetFs();
	if (audio.next && audio.next->GetFs() > 0)
		return audio.next->GetFs();
	return 1;
}

void set_playback_member(CVar& target, const char* name, double value)
{
	CVar member;
	member.SetValue(static_cast<auxtype>(value));
	target.strut[name] = member;
}

void set_playback_handle_result(CVar& out, uint64_t handle_id, const CVar& audio, int repeat_count)
{
	out.Reset(1);
	out.SetValue(static_cast<auxtype>(handle_id));
	out.MarkHandle(true);
	set_playback_member(out, "fs", audio_sample_rate(audio));
	set_playback_member(out, "dur", audio_duration_ms(audio));
	set_playback_member(out, "repeat_left", std::max(0, repeat_count - 1));
	set_playback_member(out, "prog", 0.0);
}

uint64_t next_playback_handle_id()
{
	static std::atomic<uint64_t> next_id{100000};
	return next_id.fetch_add(1, std::memory_order_relaxed);
}

struct PlayCall
{
	const CVar* handle_arg = nullptr;
	const CVar* audio_arg = nullptr;
	int repeat_count = 1;
};

bool try_parse_play_call(const vector<const CVar*>& actuals, PlayCall& out)
{
	if (actuals.empty() || actuals.size() > 3)
		return false;

	out = PlayCall{};
	if (actuals.size() == 1) {
		if (!actuals[0] || !is_audio_value(*actuals[0]))
			return false;
		out.audio_arg = actuals[0];
		return true;
	}

	if (actuals.size() == 2) {
		if (actuals[0] && is_playback_handle(*actuals[0])) {
			if (!actuals[1] || !is_audio_value(*actuals[1]))
				return false;
			out.handle_arg = actuals[0];
			out.audio_arg = actuals[1];
			return true;
		}
		if (!actuals[0] || !is_audio_value(*actuals[0]))
			return false;
		const int repeat = parse_repeat_count(*actuals[1]);
		if (repeat < 1)
			return false;
		out.audio_arg = actuals[0];
		out.repeat_count = repeat;
		return true;
	}

	if (!actuals[0] || !is_playback_handle(*actuals[0]) || !actuals[1] || !is_audio_value(*actuals[1]))
		return false;
	const int repeat = parse_repeat_count(*actuals[2]);
	if (repeat < 1)
		return false;
	out.handle_arg = actuals[0];
	out.audio_arg = actuals[1];
	out.repeat_count = repeat;
	return true;
}

} // namespace

Cfunction set_builtin_function_play(fGate fp)
{
	return make_play_builtin(fp);
}

Cfunction set_builtin_function_stop_pause_resume(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	ft.desc_arg_req = { "handle" };
	ft.narg1 = 1;
	ft.narg2 = 1;
	ft.allowed_arg_types.push_back({ 0xFFFF });
	return ft;
}

void _play(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv || !past->pEnv->playback_backend.start)
		throw exception_etc(*past, pnode, "Audio playback backend not available in this frontend.").raise();

	PlayCall parsed;
	bool parsed_ok = false;

	if (pnode && pnode->type == N_STRUCT) {
		if (is_playback_handle(past->Sig)) {
			if (!args.empty() && is_audio_value(args[0])) {
				parsed.handle_arg = &past->Sig;
				parsed.audio_arg = &args[0];
				parsed.repeat_count = 1;
				if (args.size() > 1) {
					const int repeat = parse_repeat_count(args[1]);
					if (repeat >= 1)
						parsed.repeat_count = repeat;
				}
				parsed_ok = true;
			}
		}
		else if (is_audio_value(past->Sig)) {
			parsed.audio_arg = &past->Sig;
			parsed.repeat_count = 1;
			if (!args.empty()) {
				const int repeat = parse_repeat_count(args[0]);
				if (repeat >= 1)
					parsed.repeat_count = repeat;
			}
			parsed_ok = true;
		}
	}

	int actualCount = 0;
	for (const AstNode* an = first_arg_node(pnode); an; an = an->next)
		++actualCount;
	if (actualCount <= 0 || actualCount > 3)
		throw exception_etc(*past, pnode, "play() requires an audio object.").raise();

	if (!parsed_ok) {
		vector<const CVar*> receiverFirst;
		receiverFirst.reserve(static_cast<size_t>(actualCount));
		receiverFirst.push_back(&past->Sig);
		for (int i = 1; i < actualCount && i - 1 < static_cast<int>(args.size()); ++i)
			receiverFirst.push_back(&args[static_cast<size_t>(i - 1)]);
		if (receiverFirst.size() == static_cast<size_t>(actualCount))
			parsed_ok = try_parse_play_call(receiverFirst, parsed);
	}

	if (!parsed_ok) {
		vector<const CVar*> argsOnly;
		argsOnly.reserve(static_cast<size_t>(actualCount));
		for (int i = 0; i < actualCount && i < static_cast<int>(args.size()); ++i)
			argsOnly.push_back(&args[static_cast<size_t>(i)]);
		if (argsOnly.size() == static_cast<size_t>(actualCount))
			parsed_ok = try_parse_play_call(argsOnly, parsed);
	}

	if (!parsed_ok) {
		const bool receiverLooksLikeHandle = is_playback_handle(past->Sig);
		const bool argsLookLikeHandle = !args.empty() && is_playback_handle(args[0]);
		if ((receiverLooksLikeHandle || argsLookLikeHandle) && actualCount >= 2)
			throw exception_etc(*past, pnode, "play(handle, x, repeat) requires an active playback handle.").raise();
		if ((actualCount == 2 || actualCount == 3) && !args.empty()) {
			const CVar& maybeRepeat = args[std::min<int>(static_cast<int>(args.size()) - 1, actualCount - 2)];
			if (parse_repeat_count(maybeRepeat) < 1)
				throw exception_etc(*past, pnode, "repeat must be a positive integer.").raise();
		}
		throw exception_etc(*past, pnode, "play() requires an audio object.").raise();
	}

	const CVar audio_copy = *parsed.audio_arg;

	const uint64_t handle_id = parsed.handle_arg
		? static_cast<uint64_t>(std::llround(parsed.handle_arg->value()))
		: next_playback_handle_id();
	set_playback_handle_result(past->Sig, handle_id, audio_copy, parsed.repeat_count);

	string err;
	const int ok = past->pEnv->playback_backend.start(
		past->pEnv->playback_backend.userdata,
		handle_id,
		reinterpret_cast<AuxObj>(&audio_copy),
		parsed.repeat_count,
		parsed.handle_arg ? 1 : 0,
		err);
	if (ok == 0) {
		past->Sig.Reset(1);
		past->Sig.SetValue(-1.);
		return;
	}
}

void _stop_pause_resume(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv || !past->pEnv->playback_backend.control)
		throw exception_etc(*past, pnode, "Audio playback control backend not available in this frontend.").raise();

	const char* fname = (pnode && pnode->str) ? pnode->str : "";
	auxPlaybackCommand command = auxPlaybackCommand::AUX_PLAYBACK_STOP;
	if (strcmp(fname, "stop") == 0) {
		command = auxPlaybackCommand::AUX_PLAYBACK_STOP;
	}
	else if (strcmp(fname, "pause") == 0) {
		command = auxPlaybackCommand::AUX_PLAYBACK_PAUSE;
	}
	else if (strcmp(fname, "resume") == 0) {
		command = auxPlaybackCommand::AUX_PLAYBACK_RESUME;
	}
	else {
		throw exception_etc(*past, pnode, string(fname) + "() is not available yet.").raise();
	}

	const CVar* handle_arg = nullptr;
	if (is_playback_handle(past->Sig)) {
		handle_arg = &past->Sig;
	}
	else if (!args.empty() && is_playback_handle(args[0])) {
		handle_arg = &args[0];
	}

	if (!handle_arg || !ISSCALARG(handle_arg->type()))
		throw exception_etc(*past, pnode, string(fname) + "() requires a playback handle.").raise();

	const double handleValue = handle_arg->value();
	const double rounded = std::round(handleValue);
	if (rounded <= 0 || std::fabs(handleValue - rounded) > 1e-9)
		throw exception_etc(*past, pnode, string(fname) + "() requires a playback handle.").raise();

	string err;
	const int ok = past->pEnv->playback_backend.control(
		past->pEnv->playback_backend.userdata,
		static_cast<uint64_t>(rounded),
		command,
		err);
	past->Sig.Reset(1);
	past->Sig.SetValue(ok != 0 ? 1.0 : -1.0);
}
