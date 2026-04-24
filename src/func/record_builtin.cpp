#include "functions_common.h"

#include <atomic>
#include <cmath>

namespace {

const AstNode* first_arg_node(const AstNode* pnode)
{
	if (!pnode) return nullptr;
	return get_first_arg(pnode, pnode->type == N_STRUCT);
}

int count_actual_args(const AstNode* pnode)
{
	int count = 0;
	for (const AstNode* an = first_arg_node(pnode); an; an = an->next)
		++count;
	return count;
}

bool try_parse_int_arg(const CVar& value, int& out)
{
	if (!ISSCALARG(value.type()))
		return false;
	const double rounded = std::round(value.value());
	if (std::fabs(value.value() - rounded) > 1e-9)
		return false;
	out = static_cast<int>(rounded);
	return true;
}

bool try_parse_scalar_arg(const CVar& value, double& out)
{
	if (!ISSCALARG(value.type()))
		return false;
	out = value.value();
	return true;
}

const CVar& get_actual_arg(const AuxScope* past, const vector<CVar>& args, int index)
{
	if (index == 0)
		return past->Sig;
	return args.at(static_cast<size_t>(index - 1));
}

void set_numeric_member(CVar& target, const char* name, double value)
{
	CVar member;
	member.SetValue(static_cast<auxtype>(value));
	target.strut[name] = member;
}

void set_recording_handle_result(CVar& out,
	uint64_t handle_id,
	int device_id,
	int sample_rate,
	int channels,
	double duration_ms,
	double block_ms)
{
	out.Reset(1);
	out.SetValue(static_cast<auxtype>(handle_id));
	out.MarkHandle(true);
	out.strut["type"] = CVar(string("audio_record"));
	set_numeric_member(out, "id", static_cast<double>(handle_id));
	set_numeric_member(out, "devID", static_cast<double>(device_id));
	set_numeric_member(out, "fs", static_cast<double>(sample_rate));
	set_numeric_member(out, "channels", static_cast<double>(channels));
	set_numeric_member(out, "dur", duration_ms);
	set_numeric_member(out, "block", block_ms);
	set_numeric_member(out, "durRec", 0.0);
	set_numeric_member(out, "durLeft", duration_ms > 0.0 ? duration_ms : 0.0);
	set_numeric_member(out, "prog", 0.0);
	set_numeric_member(out, "active", 1.0);
	set_numeric_member(out, "paused", 0.0);
}

bool is_async_record_call(const AstNode* pnode)
{
	if (!pnode)
		return false;
	if (pnode->type == T_ID && pnode->alt && pnode->alt->alt)
		return true;
	if (pnode->type == N_STRUCT && pnode->alt)
		return true;
	return false;
}

const AstNode* callback_node(const AstNode* pnode)
{
	if (!pnode)
		return nullptr;
	if (pnode->type == T_ID && pnode->alt)
		return pnode->alt->alt;
	if (pnode->type == N_STRUCT)
		return pnode->alt;
	return nullptr;
}

string callback_name_from_node(const AstNode* pnode)
{
	const AstNode* cb = callback_node(pnode);
	if (!cb || !cb->str)
		return "";
	return string(cb->str);
}

uint64_t next_recording_handle_id()
{
	static std::atomic<uint64_t> next_id{200000};
	return next_id.fetch_add(1, std::memory_order_relaxed);
}

Cfunction make_record_builtin(fGate fp)
{
	Cfunction ft;
	ft.func = fp;
	ft.alwaysstatic = false;
	ft.desc_arg_opt = { "device_id", "duration_ms", "channels", "block_ms" };
	ft.defaultarg = { CVar(0.), CVar(1000.), CVar(1.), CVar(100.) };
	for (int i = 0; i < 4; ++i)
		ft.allowed_arg_types.push_back({ 0xFFFF });
	ft.narg1 = 0;
	ft.narg2 = 4;
	return ft;
}

} // namespace

Cfunction set_builtin_function_record(fGate fp)
{
	return make_record_builtin(fp);
}

void _record(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	if (!past || !past->pEnv)
		throw exception_etc(*past, pnode, "Audio recording backend not available in this frontend.").raise();

	const bool async_call = is_async_record_call(pnode);
	if (async_call) {
		if (!past->pEnv->playback_backend.record_async_start)
			throw exception_etc(*past, pnode, "Async audio recording backend not available in this frontend.").raise();
	}
	else if (!past->pEnv->playback_backend.record) {
		throw exception_etc(*past, pnode, "Audio recording backend not available in this frontend.").raise();
	}

	const int actualCount = count_actual_args(pnode);
	const int maxArgs = async_call ? 4 : 3;
	if (actualCount > maxArgs)
		throw exception_etc(*past, pnode, async_call
			? "record(...).callback takes at most 4 arguments: device_id, duration_ms, channels, block_ms."
			: "record() takes at most 3 arguments: device_id, duration_ms, channels.").raise();

	int deviceId = 0;
	double durationMs = async_call ? -1.0 : 1000.0;
	int channelCount = 1;
	double blockMs = 100.0;

	if (actualCount >= 1 && !try_parse_int_arg(get_actual_arg(past, args, 0), deviceId))
		throw exception_etc(*past, pnode, "device_id must be an integer.").raise();
	if (actualCount >= 2 && !try_parse_scalar_arg(get_actual_arg(past, args, 1), durationMs))
		throw exception_etc(*past, pnode, "duration_ms must be a scalar.").raise();
	if (actualCount >= 3 && !try_parse_int_arg(get_actual_arg(past, args, 2), channelCount))
		throw exception_etc(*past, pnode, "channels must be 1 or 2.").raise();
	if (actualCount >= 4 && !try_parse_scalar_arg(get_actual_arg(past, args, 3), blockMs))
		throw exception_etc(*past, pnode, "block_ms must be a scalar.").raise();

	if (deviceId < 0)
		throw exception_etc(*past, pnode, "device_id must be 0 or greater.").raise();
	if (channelCount != 1 && channelCount != 2)
		throw exception_etc(*past, pnode, "channels must be 1 or 2.").raise();
	if (!std::isfinite(blockMs) || blockMs <= 0.0)
		throw exception_etc(*past, pnode, "block_ms must be a positive finite value.").raise();

	const int requestedFs = past->GetFs() > 0 ? past->GetFs() : 22050;

	if (async_call) {
		if (!std::isfinite(durationMs) || durationMs == 0.0 || durationMs < -1.0)
			throw exception_etc(*past, pnode, "duration_ms must be positive or -1 for indefinite async recording.").raise();
		const string callbackName = callback_name_from_node(pnode);
		if (callbackName.empty())
			throw exception_etc(*past, pnode, "record(...).callback requires a callback name.").raise();

		const uint64_t handle_id = next_recording_handle_id();
		set_recording_handle_result(past->Sig, handle_id, deviceId, requestedFs, channelCount, durationMs, blockMs);

		auxAsyncRecordSpec spec;
		spec.device_id = deviceId;
		spec.sample_rate = requestedFs;
		spec.num_channels = channelCount;
		spec.duration_ms = durationMs;
		spec.block_ms = blockMs;
		spec.callback_name = callbackName;

		string err;
		const int ok = past->pEnv->playback_backend.record_async_start(
			past->pEnv->playback_backend.userdata,
			handle_id,
			spec,
			err);
		if (ok == 0) {
			past->Sig.Reset(1);
			past->Sig.SetValue(-1.0);
		}
		return;
	}

	if (!std::isfinite(durationMs) || durationMs <= 0.0)
		throw exception_etc(*past, pnode, "duration_ms must be a positive finite value.").raise();

	auxRecordResult recorded;
	string err;
	const int ok = past->pEnv->playback_backend.record(
		past->pEnv->playback_backend.userdata,
		deviceId,
		requestedFs,
		channelCount,
		durationMs,
		recorded,
		err);
	if (ok == 0) {
		if (err.empty())
			err = "record() failed.";
		throw exception_etc(*past, pnode, err).raise();
	}

	if (recorded.sample_rate <= 0)
		throw exception_etc(*past, pnode, "record() backend returned an invalid sample rate.").raise();
	if (recorded.num_channels != 1 && recorded.num_channels != 2)
		throw exception_etc(*past, pnode, "record() backend returned an unsupported channel count.").raise();
	if (recorded.interleaved.empty()) {
		past->Sig.Reset(recorded.sample_rate);
		return;
	}
	if (recorded.interleaved.size() % static_cast<size_t>(recorded.num_channels) != 0)
		throw exception_etc(*past, pnode, "record() backend returned malformed interleaved audio data.").raise();

	const size_t frames = recorded.interleaved.size() / static_cast<size_t>(recorded.num_channels);
	past->Sig.Reset(recorded.sample_rate);
	past->Sig.UpdateBuffer(static_cast<unsigned int>(frames));
	if (recorded.num_channels == 1) {
		for (size_t i = 0; i < frames; ++i)
			past->Sig.buf[i] = recorded.interleaved[i];
		return;
	}

	CSignals right(recorded.sample_rate);
	right.UpdateBuffer(static_cast<unsigned int>(frames));
	for (size_t i = 0; i < frames; ++i) {
		past->Sig.buf[i] = recorded.interleaved[i * 2];
		right.buf[i] = recorded.interleaved[i * 2 + 1];
	}
	past->Sig.SetNextChan(right);
}
