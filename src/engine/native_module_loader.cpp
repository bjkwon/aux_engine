#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include "nlohmann/json.hpp"

#include "AuxScope.h"
#include "AuxScope_exception.h"
#include "_file_mp3.h"
#include "_file_wav.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

using json = nlohmann::json;

namespace {

constexpr size_t kErrCap = 1024;

static auxNativeValue to_native_value(const CVar& value)
{
	return auxNativeValue{ reinterpret_cast<const void*>(&value) };
}

static auxNativeMutableValue to_native_mutable_value(CVar& value)
{
	return auxNativeMutableValue{ reinterpret_cast<void*>(&value) };
}

static const CVar* native_as_cvar(auxNativeValue value)
{
	return reinterpret_cast<const CVar*>(value.impl);
}

static CVar* native_as_mutable_cvar(auxNativeMutableValue value)
{
	return reinterpret_cast<CVar*>(value.impl);
}

static const CSignals* native_get_channel(const CVar* value, int channel_index)
{
	const CSignals* ch = value;
	for (int i = 0; ch && i < channel_index; ++i)
		ch = ch->next;
	return ch;
}

static uint16_t native_value_type(auxNativeValue value)
{
	const CVar* v = native_as_cvar(value);
	return v ? v->type() : TYPEBIT_NULL;
}

static int native_value_get_scalar(auxNativeValue value, auxtype* out)
{
	const CVar* v = native_as_cvar(value);
	if (!v || !out || !ISSCALARG(v->type()))
		return 1;
	*out = v->value();
	return 0;
}

static size_t native_value_vector_length(auxNativeValue value)
{
	const CVar* v = native_as_cvar(value);
	if (!v) return 0;
	const uint16_t t = v->type();
	if (ISSCALARG(t)) return 1;
	if (!ISVECTORG(t) || ISSTRING(t) || ISAUDIO(t) || ISAUDIOG(t)) return 0;
	return static_cast<size_t>(v->nSamples);
}

static size_t native_value_copy_vector(auxNativeValue value, auxtype* out, size_t max_len)
{
	const CVar* v = native_as_cvar(value);
	if (!v || !out) return 0;
	const size_t len = native_value_vector_length(value);
	if (len == 0 || max_len < len) return 0;
	if (len == 1) {
		out[0] = v->value();
		return 1;
	}
	const vector<auxtype> values = v->ToVector();
	if (values.size() != len) return 0;
	memcpy(out, values.data(), len * sizeof(auxtype));
	return len;
}

static size_t native_value_string_length(auxNativeValue value)
{
	const CVar* v = native_as_cvar(value);
	if (!v || !ISSTRING(v->type())) return 0;
	const string s = v->str();
	return s.size();
}

static size_t native_value_copy_string(auxNativeValue value, char* out, size_t max_len)
{
	const CVar* v = native_as_cvar(value);
	if (!v || !out || max_len == 0 || !ISSTRING(v->type())) return 0;
	const string s = v->str();
	const size_t n = std::min(max_len - 1, s.size());
	memcpy(out, s.data(), n);
	out[n] = '\0';
	return n;
}

static int native_value_num_channels(auxNativeValue value)
{
	const CVar* v = native_as_cvar(value);
	if (!v) return 0;
	int count = 0;
	for (const CSignals* ch = v; ch; ch = ch->next)
		++count;
	return count;
}

static int native_value_sample_rate(auxNativeValue value)
{
	const CVar* v = native_as_cvar(value);
	return v ? v->GetFs() : 0;
}

static size_t native_value_flatten_channel_length(auxNativeValue value, int channel_index)
{
	const CVar* v = native_as_cvar(value);
	if (!v || channel_index < 0) return 0;
	const CSignals* ch = native_get_channel(v, channel_index);
	size_t total = 0;
	for (const CTimeSeries* seg = ch; seg; seg = seg->chain)
		total += static_cast<size_t>(seg->nSamples);
	return total;
}

static size_t native_value_flatten_channel(auxNativeValue value, int channel_index, auxtype* out, size_t max_len)
{
	const CVar* v = native_as_cvar(value);
	if (!v || !out || channel_index < 0) return 0;
	const CSignals* ch = native_get_channel(v, channel_index);
	size_t written = 0;
	for (const CTimeSeries* seg = ch; seg; seg = seg->chain) {
		const size_t n = static_cast<size_t>(seg->nSamples);
		if (written + n > max_len) return written;
		memcpy(out + written, seg->buf, n * sizeof(auxtype));
		written += n;
	}
	return written;
}

static int native_result_set_null(auxNativeMutableValue result)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out) return 1;
	out->Reset();
	return 0;
}

static int native_result_set_scalar(auxNativeMutableValue result, auxtype value)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out) return 1;
	out->SetValue(value);
	return 0;
}

static int native_result_set_vector(auxNativeMutableValue result, const auxtype* values, size_t len)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out || (!values && len > 0)) return 1;
	out->Reset(1);
	out->UpdateBuffer(static_cast<uint64_t>(len));
	if (len > 0)
		memcpy(out->buf, values, len * sizeof(auxtype));
	return 0;
}

static int native_result_set_string(auxNativeMutableValue result, const char* value)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out || !value) return 1;
	out->SetString(value);
	return 0;
}

static int native_result_set_audio_mono(auxNativeMutableValue result, const auxtype* values, size_t frames, int sample_rate)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out || (!values && frames > 0) || sample_rate <= 0) return 1;
	out->Reset(sample_rate);
	out->UpdateBuffer(static_cast<uint64_t>(frames));
	if (frames > 0)
		memcpy(out->buf, values, frames * sizeof(auxtype));
	return 0;
}

static int native_result_set_audio_stereo(auxNativeMutableValue result, const auxtype* left, const auxtype* right, size_t frames, int sample_rate)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out || (!left && frames > 0) || (!right && frames > 0) || sample_rate <= 0) return 1;
	out->Reset(sample_rate);
	out->UpdateBuffer(static_cast<uint64_t>(frames));
	if (frames > 0)
		memcpy(out->buf, left, frames * sizeof(auxtype));
	CSignals second(sample_rate);
	second.UpdateBuffer(static_cast<uint64_t>(frames));
	if (frames > 0)
		memcpy(second.buf, right, frames * sizeof(auxtype));
	out->SetNextChan(second);
	return 0;
}

static int native_result_set_bytes(auxNativeMutableValue result, const unsigned char* values, size_t len)
{
	CVar* out = native_as_mutable_cvar(result);
	if (!out || (!values && len > 0)) return 1;
	out->Reset(1);
	out->bufBlockSize = 1;
	out->UpdateBuffer(static_cast<uint64_t>(len));
	if (len > 0)
		memcpy(out->strbuf, values, len);
	out->SetByte();
	return 0;
}

static bool read_file_bytes(const string& path, vector<unsigned char>& bytes)
{
	ifstream in(path, std::ios::binary);
	if (!in.is_open())
		return false;
	in.seekg(0, std::ios::end);
	const std::streamoff size = in.tellg();
	if (size < 0)
		return false;
	in.seekg(0, std::ios::beg);
	bytes.resize(static_cast<size_t>(size));
	if (!bytes.empty())
		in.read(reinterpret_cast<char*>(bytes.data()), size);
	return in.good() || in.eof();
}

static int native_filetype(const string& path)
{
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) return 0;
	char buffer[16] = {};
	const auto n = fread(buffer, 1, sizeof(buffer), fp);
	fclose(fp);
	if (n < 12) return 4;
	if (!memcmp(buffer, "RIFF", 4) && !memcmp(buffer + 8, "WAVE", 4))
		return 1;
	if (!memcmp(buffer, "ID3", 3) || (buffer[0] == static_cast<char>(0xFF) && static_cast<char>(0xE0) <= buffer[1] && buffer[1] <= static_cast<char>(0xFF)))
		return 2;
	return 4;
}

static bool looks_like_text(const vector<unsigned char>& bytes)
{
	for (unsigned char ch : bytes) {
		if (ch == 0)
			return false;
		if (ch < 0x20 && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\f' && ch != '\b')
			return false;
	}
	return true;
}

static int set_audio_from_interleaved(CVar* out, const vector<float>& input, size_t frames, int channels, int sample_rate)
{
	if (!out || sample_rate <= 0 || channels <= 0)
		return 1;
	out->Reset(sample_rate);
	out->bufType = 'R';
	if (channels == 1) {
		out->UpdateBuffer(static_cast<uint64_t>(frames));
		for (size_t i = 0; i < frames; ++i)
			out->buf[i] = static_cast<auxtype>(input[i]);
		return 0;
	}
	if (channels == 2) {
		out->UpdateBuffer(static_cast<uint64_t>(frames));
		CSignals right(sample_rate);
		right.UpdateBuffer(static_cast<uint64_t>(frames));
		for (size_t i = 0; i < frames; ++i) {
			out->buf[i] = static_cast<auxtype>(input[i * 2]);
			right.buf[i] = static_cast<auxtype>(input[i * 2 + 1]);
		}
		out->SetNextChan(right);
		return 0;
	}
	return 1;
}

static int native_result_set_file(auxNativeMutableValue result, const char* path, int preferred_sample_rate)
{
	(void)preferred_sample_rate;
	CVar* out = native_as_mutable_cvar(result);
	if (!out || !path || !*path) return 1;

	const string filename(path);
	const int type = native_filetype(filename);
	string err;
	if (type == 1) {
		WavInfo info{};
		const int offset = wav_read_header(filename, info, err);
		if (offset < 0 || !err.empty())
			return 1;
		FILE* fp = fopen(filename.c_str(), "rb");
		if (!fp)
			return 1;
		fseek(fp, offset, SEEK_SET);
		const uint64_t frames = info.block_align ? (info.data_size / info.block_align) : 0;
		vector<float> buffer;
		const uint64_t count = wav_read_float32(fp, frames, info, buffer, err);
		fclose(fp);
		if (!err.empty())
			return 1;
		return set_audio_from_interleaved(out, buffer, static_cast<size_t>(count), static_cast<int>(info.num_channels), static_cast<int>(info.sample_rate));
	}
	if (type == 2) {
		Mp3Info info{};
		vector<float> buffer;
		const uint64_t frames = mp3_read_float32(filename, 0.0, -1.0, info, buffer, err);
		if (!err.empty())
			return 1;
		return set_audio_from_interleaved(out, buffer, static_cast<size_t>(frames), static_cast<int>(info.num_channels), static_cast<int>(info.sample_rate));
	}

	vector<unsigned char> bytes;
	if (!read_file_bytes(filename, bytes))
		return 1;
	if (looks_like_text(bytes)) {
		string text(bytes.begin(), bytes.end());
		out->SetString(text.c_str());
		return 0;
	}
	return native_result_set_bytes(result, bytes.empty() ? nullptr : bytes.data(), bytes.size());
}

static const auxNativeModuleHost kNativeHost = {
	AUXE_NATIVE_MODULE_ABI_VERSION,
	sizeof(auxNativeModuleHost),
	&native_value_type,
	&native_value_get_scalar,
	&native_value_vector_length,
	&native_value_copy_vector,
	&native_value_string_length,
	&native_value_copy_string,
	&native_value_num_channels,
	&native_value_flatten_channel_length,
	&native_value_flatten_channel,
	&native_result_set_null,
	&native_result_set_scalar,
	&native_result_set_vector,
	&native_result_set_string,
	&native_result_set_audio_mono,
	&native_result_set_audio_stereo,
	&native_value_sample_rate,
	&native_result_set_bytes,
	&native_result_set_file
};

static string path_join2(const string& a, const string& b)
{
	if (a.empty()) return b;
	if (b.empty()) return a;
	if (b.size() > 0 && (b.front() == '/' || b.front() == '\\')) return b;
#ifdef _WIN32
	if (b.size() > 2 && b[1] == ':') return b;
#endif
	const char last = a.back();
	if (last == '/' || last == '\\') return a + b;
#ifdef _WIN32
	return a + "\\" + b;
#else
	return a + "/" + b;
#endif
}

static bool file_exists(const string& path)
{
	ifstream f(path);
	return f.good();
}

static vector<string> split_path_list(const char* raw)
{
	vector<string> out;
	if (!raw || !*raw) return out;
#ifdef _WIN32
	const char delim = ';';
#else
	const char delim = ':';
#endif
	string item;
	for (const char* p = raw; ; ++p) {
		if (*p == delim || *p == '\0') {
			if (!item.empty()) out.push_back(item);
			item.clear();
			if (*p == '\0') break;
		} else {
			item.push_back(*p);
		}
	}
	return out;
}

static string default_registry_root()
{
#ifdef _WIN32
	const char* local = getenv("LOCALAPPDATA");
	if (local && *local)
		return path_join2(path_join2(local, "auxe"), "modules");
	return "";
#else
	const char* home = getenv("HOME");
	if (home && *home)
		return path_join2(path_join2(home, ".auxe"), "modules");
	return "";
#endif
}

static string platform_id()
{
#ifdef _WIN32
#if defined(_M_ARM64) || defined(__aarch64__)
	return "windows-arm64";
#else
	return "windows-x64";
#endif
#elif defined(__APPLE__)
#if defined(__aarch64__) || defined(__arm64__)
	return "macos-arm64";
#else
	return "macos-x64";
#endif
#else
#if defined(__aarch64__)
	return "linux-arm64";
#else
	return "linux-x64";
#endif
#endif
}

static bool read_manifest(const string& manifest_path, json& manifest, string& errstr)
{
	ifstream in(manifest_path);
	if (!in.is_open()) {
		errstr = "Module manifest not found: " + manifest_path;
		return false;
	}
	try {
		in >> manifest;
		return true;
	} catch (const std::exception& e) {
		errstr = "Invalid module manifest " + manifest_path + ": " + e.what();
		return false;
	}
}

static vector<string> module_registry_roots()
{
	vector<string> roots = split_path_list(getenv("AUXE_MODULE_PATH"));
	const string fallback = default_registry_root();
	if (!fallback.empty())
		roots.push_back(fallback);
	return roots;
}

static string find_module_dir(const string& module_name)
{
	for (const string& root : module_registry_roots()) {
		const string dir = path_join2(root, module_name);
		if (file_exists(path_join2(dir, "auxe-module.json")))
			return dir;
	}
	return "";
}

static void* open_library(const string& path, string& errstr)
{
#ifdef _WIN32
	HMODULE h = LoadLibraryA(path.c_str());
	if (!h) {
		errstr = "Failed to load module library: " + path;
		return nullptr;
	}
	return reinterpret_cast<void*>(h);
#else
	void* h = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (!h) {
		const char* e = dlerror();
		errstr = "Failed to load module library: " + path;
		if (e) errstr += " (" + string(e) + ")";
		return nullptr;
	}
	return h;
#endif
}

static void close_library(void* handle)
{
	if (!handle) return;
#ifdef _WIN32
	FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
	dlclose(handle);
#endif
}

static auxNativeModuleInit find_module_init(void* handle)
{
	if (!handle) return nullptr;
#ifdef _WIN32
	return reinterpret_cast<auxNativeModuleInit>(GetProcAddress(reinterpret_cast<HMODULE>(handle), "auxe_module_init"));
#else
	return reinterpret_cast<auxNativeModuleInit>(dlsym(handle, "auxe_module_init"));
#endif
}

static bool manifest_function_allows(const json& manifest, const string& name)
{
	if (!manifest.contains("functions"))
		return true;
	if (!manifest["functions"].is_array())
		return false;
	for (const auto& item : manifest["functions"]) {
		if (item.is_object() && item.value("name", string()) == name)
			return true;
	}
	return false;
}

} // namespace

int EngineRuntime::InvokeNativeModuleFunction(const string& funcname, AuxScope* past, bool has_receiver, bool dot_call, const vector<CVar>& args, CVar& result, string& errstr)
{
	auto it = native_module_functions.find(funcname);
	if (it == native_module_functions.end()) {
		errstr = "Native module function is not registered: " + funcname;
		return 1;
	}
	const auto cb = it->second.desc.callback;
	if (!cb) {
		errstr = "Native module function has no callback: " + funcname;
		return 1;
	}
	if (dot_call && !it->second.desc.allow_dot_call) {
		errstr = funcname + "(): static native module function cannot be called with dot notation.";
		return 1;
	}
	const int total_args = static_cast<int>(args.size()) + (has_receiver ? 1 : 0);
	if (total_args < it->second.desc.min_args) {
		errstr = funcname + "(): too few arguments.";
		return 1;
	}
	if (it->second.desc.max_args >= 0 && total_args > it->second.desc.max_args) {
		errstr = funcname + "(): too many arguments.";
		return 1;
	}
	vector<auxNativeValue> native_args;
	native_args.reserve(args.size());
	for (const CVar& arg : args)
		native_args.push_back(to_native_value(arg));
	char err[kErrCap] = {};
	const int rc = cb(reinterpret_cast<auxContext*>(past),
	                  has_receiver ? to_native_value(past->Sig) : auxNativeValue{},
	                  native_args.empty() ? nullptr : native_args.data(),
	                  native_args.size(),
	                  to_native_mutable_value(result),
	                  err,
	                  sizeof(err));
	if (rc != 0) {
		errstr = err[0] ? string(err) : (funcname + "(): native module callback returned an error.");
		return rc;
	}
	return 0;
}

int EngineRuntime::LoadNativeModule(const string& module_name, string& errstr)
{
	if (module_name.empty()) {
		errstr = "Module name is empty.";
		return 1;
	}
	if (native_modules.find(module_name) != native_modules.end())
		return 0;

	const string module_dir = find_module_dir(module_name);
	if (module_dir.empty()) {
		errstr = "Module not found: " + module_name;
		return 1;
	}
	const string manifest_path = path_join2(module_dir, "auxe-module.json");
	json manifest;
	if (!read_manifest(manifest_path, manifest, errstr))
		return 1;
	const string manifest_name = manifest.value("name", string());
	if (manifest_name != module_name) {
		errstr = "Module manifest name mismatch for " + module_name + ".";
		return 1;
	}
	if (manifest.value("abi_version", 0u) != AUXE_NATIVE_MODULE_ABI_VERSION) {
		errstr = "Module ABI mismatch for " + module_name + ".";
		return 1;
	}
	string library_rel;
	if (manifest.contains("platforms") && manifest["platforms"].is_object())
		library_rel = manifest["platforms"].value(platform_id(), string());
	if (library_rel.empty())
		library_rel = manifest.value("library", string());
	if (library_rel.empty()) {
		errstr = "Module manifest does not specify a library for " + platform_id() + ".";
		return 1;
	}
	const string library_path = path_join2(module_dir, library_rel);
	void* library = open_library(library_path, errstr);
	if (!library)
		return 1;

	auxNativeModuleInit init = find_module_init(library);
	if (!init) {
		close_library(library);
		errstr = "Module library does not export auxe_module_init: " + library_path;
		return 1;
	}

	auxNativeModuleInfo info{};
	char err[kErrCap] = {};
	const int init_rc = init(&kNativeHost, &info, err, sizeof(err));
	if (init_rc != 0) {
		close_library(library);
		errstr = err[0] ? string(err) : ("Module initialization failed: " + module_name);
		return 1;
	}
	if (info.abi_version != AUXE_NATIVE_MODULE_ABI_VERSION) {
		close_library(library);
		errstr = "Module ABI mismatch after initialization for " + module_name + ".";
		return 1;
	}
	if (!info.name || module_name != info.name) {
		close_library(library);
		errstr = "Module entrypoint name mismatch for " + module_name + ".";
		return 1;
	}
	if (!info.functions || info.function_count == 0) {
		close_library(library);
		errstr = "Module exports no functions: " + module_name;
		return 1;
	}

	for (size_t i = 0; i < info.function_count; ++i) {
		const auxNativeFunctionDesc& desc = info.functions[i];
		if (!desc.name || !*desc.name || !desc.callback) {
			close_library(library);
			errstr = "Module contains an invalid function descriptor: " + module_name;
			return 1;
		}
		const string fname = desc.name;
		const string qualified_name = module_name + "." + fname;
		if (!manifest_function_allows(manifest, fname)) {
			close_library(library);
			errstr = "Module function is not declared in manifest: " + fname;
			return 1;
		}
		if (native_module_functions.find(qualified_name) != native_module_functions.end()) {
			close_library(library);
			errstr = "Module function is already registered: " + qualified_name;
			return 1;
		}
	}

	NativeModuleHandle handle;
	handle.name = module_name;
	handle.path = library_path;
	handle.library = library;
	native_modules[module_name] = handle;
	for (size_t i = 0; i < info.function_count; ++i) {
		NativeModuleFunction fn;
		fn.module_name = module_name;
		fn.desc = info.functions[i];
		native_module_functions[module_name + "." + string(fn.desc.name)] = fn;
	}
	return 0;
}

int EngineRuntime::ImportNativeModule(const string& module_name, const string& alias, AuxScope* frame, string& errstr)
{
	const string binding = alias.empty() ? module_name : alias;
	if (binding.empty()) {
		errstr = "Module alias is empty.";
		return 1;
	}
	auto ait = native_module_aliases.find(binding);
	if (ait != native_module_aliases.end()) {
		if (ait->second == module_name)
			return 0;
		errstr = "Module alias already refers to another module: " + binding;
		return 1;
	}
	if (builtin.find(binding) != builtin.end() || pseudo_vars.find(binding) != pseudo_vars.end()) {
		errstr = "Module alias collides with an existing builtin or pseudo variable: " + binding;
		return 1;
	}
	if (frame && (frame->Vars.find(binding) != frame->Vars.end() || frame->GOvars.find(binding) != frame->GOvars.end())) {
		errstr = "Module alias collides with an existing variable: " + binding;
		return 1;
	}
	if (LoadNativeModule(module_name, errstr) != 0)
		return 1;
	native_module_aliases[binding] = module_name;
	return 0;
}

bool EngineRuntime::ResolveNativeModuleFunction(const string& alias, const string& funcname, string& qualified_name) const
{
	auto ait = native_module_aliases.find(alias);
	if (ait == native_module_aliases.end())
		return false;
	qualified_name = ait->second + "." + funcname;
	return native_module_functions.find(qualified_name) != native_module_functions.end();
}

void auxe_close_native_modules(EngineRuntime& runtime)
{
	for (auto& entry : runtime.native_modules)
		close_library(entry.second.library);
	runtime.native_modules.clear();
	runtime.native_module_functions.clear();
	runtime.native_module_aliases.clear();
}
