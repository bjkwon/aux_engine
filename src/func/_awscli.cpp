#include "functions_common.h"
#include "_file_wav.h"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

using json = nlohmann::json;

// awst(x, v): Upload audio x to S3, start Amazon Transcribe job, return struct with job info.
// Struct v members:
//   .region   string  S3/Transcribe region (default "us-east-1")
//   .bucket   string  S3 bucket name (required)
//   .langcode cell    Language codes; empty=single-id, 1=manual, 2+=multi-id
//   .outputkey string S3 prefix for transcript JSON (default "temp/transcripts/")
//   .basename  string Optional prefix for job name (default "auxlab")
//   .id        string Optional custom job prefix; if text and len<=32 use id_timestamp, else first 32 chars

static string shell_quote_posix(const string& s)
{
	string out("'");
	for (char c : s) {
		if (c == '\'')
			out += "'\\''";
		else
			out += c;
	}
	out += "'";
	return out;
}

static bool run_aws_cmd(const string& cmd, string& errstr)
{
#ifndef _WIN32
	// On macOS, GUI apps inherit minimal PATH; prepend common aws locations.
	string fullCmd = "PATH=\"/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin\" " + cmd + " 2>&1";
#else
	string fullCmd = cmd + " 2>&1";
#endif
	int rc = system(fullCmd.c_str());
	if (rc != 0) {
		errstr = "aws command failed (exit " + std::to_string(rc) + ")";
		return false;
	}
	return true;
}

static bool run_aws_cmd_capture(const string& cmd, string& output, string& errstr)
{
#ifndef _WIN32
	string fullCmd = "PATH=\"/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin\" " + cmd + " 2>/dev/null";
#else
	string fullCmd = cmd + " 2>nul";
#endif
	FILE* fp = popen(fullCmd.c_str(), "r");
	if (!fp) {
		errstr = "popen failed";
		return false;
	}
	output.clear();
	char buf[4096];
	while (fgets(buf, sizeof(buf), fp))
		output += buf;
	int rc = pclose(fp);
	if (rc != 0) {
		errstr = "aws command failed (exit " + std::to_string(rc) + ")";
		return false;
	}
	return true;
}

static void json_obj_to_cvar(CVar& out, const json& j)
{
	if (!j.is_object()) return;
	for (auto it = j.begin(); it != j.end(); ++it) {
		const string& k = it.key();
		const json& v = it.value();
		if (v.is_string())
			out.strut[k] = v.get<string>();
		else if (v.is_number_float())
			out.strut[k] = (float)v.get<double>();
		else if (v.is_number_integer())
			out.strut[k] = (float)v.get<int64_t>();
		else if (v.is_boolean()) {
			CVar c;
			c.SetValue(v.get<bool>() ? 1.f : 0.f);
			c.MakeLogical();
			out.strut[k] = c;
		}
		else if (v.is_null())
			out.strut[k] = CVar();
		else if (v.is_object()) {
			CVar child;
			json_obj_to_cvar(child, v);
			out.strut[k] = child;
		}
	}
}

static bool upload_to_s3(const string& localPath, const string& bucket, const string& key, string& errstr)
{
	string qLocal = shell_quote_posix(localPath);
	string s3Uri = "s3://" + bucket + "/" + key;
	string qS3 = shell_quote_posix(s3Uri);
	string cmd = "aws s3 cp " + qLocal + " " + qS3;
	return run_aws_cmd(cmd, errstr);
}

static string get_struct_str(const map<string, CVar>& strut, const string& key, const string& defaultVal)
{
	auto it = strut.find(key);
	if (it == strut.end()) return defaultVal;
	return it->second.str();
}

Cfunction set_builtin_function_awst(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_signal", "params_struct" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRUT, TYPEBIT_STRUTS };
	ft.allowed_arg_types.push_back(allowedTypes2);
	// qualify/reject per arg: arg1=audio, arg2=struct (when qualify present, allowedTypes is bypassed)
	set<pfunc_typecheck> qualifyAudio = { Cfunction::IsAUDIO };
	ft.qualify.push_back(qualifyAudio);
	ft.reject.push_back({ Cfunction::AllFalse });
	set<pfunc_typecheck> qualifyStruct = { Cfunction::IsSTRUT };
	ft.qualify.push_back(qualifyStruct);
	ft.reject.push_back({ Cfunction::AllFalse });
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

void _awst(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	const CVar& audio = past->Sig;
	const CVar& v = args[0];

	if (!ISAUDIO(audio.type()))
		throw exception_func(*past, pnode, "First argument must be audio", "awst", 1).raise();
	if (!Cfunction::IsSTRUT(v.type()))
		throw exception_func(*past, pnode, "Second argument must be a struct", "awst", 2).raise();

	string region = get_struct_str(v.strut, "region", "us-east-1");
	string bucket = get_struct_str(v.strut, "bucket", "");
	string outputkey = get_struct_str(v.strut, "outputkey", "temp/transcripts/");
	string basename = get_struct_str(v.strut, "basename", "auxlab");

	// v.id overrides basename when it's a text; max 32 chars
	auto idIt = v.strut.find("id");
	if (idIt != v.strut.end() && ISSTRING(idIt->second.type())) {
		string idStr = idIt->second.str();
		basename = (idStr.size() > 32) ? idStr.substr(0, 32) : idStr;
	}

	if (bucket.empty())
		throw exception_func(*past, pnode, "Struct must have .bucket (S3 bucket name)", "awst").raise();

	// Build language codes from .langcode cell
	vector<string> langCodes;
	auto it = v.strut.find("langcode");
	if (it != v.strut.end() && !it->second.cell.empty()) {
		for (const auto& c : it->second.cell)
			langCodes.push_back(c.str());
	}

	// Create temp wav file
#ifndef _WIN32
	char tmpl[] = "/tmp/aux2_awst_XXXXXX.wav";
	int fd = mkstemp(tmpl);
	if (fd < 0)
		throw exception_etc(*past, pnode, "Unable to create temporary file for awst").raise();
	close(fd);
	string tempPath(tmpl);
#else
	char tempPathBuf[L_tmpnam];
	if (!tmpnam(tempPathBuf))
		throw exception_etc(*past, pnode, "Unable to create temporary filename for awst").raise();
	string tempPath = string(tempPathBuf) + ".wav";
#endif

	// Write audio to temp wav (PCM16, same as _wavwrite)
	CVar audioCopy = audio;
	audioCopy.MakeChainless();
	WavInfo wavinfo = { 0 };
	wavinfo.audio_format = 1;
	wavinfo.num_channels = (audioCopy.next) ? 2 : 1;
	wavinfo.sample_rate = audioCopy.GetFs();
	wavinfo.bits_per_sample = 16;
	wavinfo.block_align = 2 * wavinfo.num_channels;
	wavinfo.byte_rate = wavinfo.sample_rate * wavinfo.block_align;
	wavinfo.data_offset = 44;
	wavinfo.data_size = audioCopy.nSamples * wavinfo.block_align;
	wavinfo.riff_size = wavinfo.data_size + 44 - 8;

	FILE* fp = fopen(tempPath.c_str(), "wb");
	if (!fp) {
		remove(tempPath.c_str());
		throw exception_etc(*past, pnode, "Unable to open temp file for writing").raise();
	}
	char buffer[44];
	make_wav_header(buffer, wavinfo, audioCopy.nSamples);
	if (fwrite(buffer, 1, 44, fp) != 44) {
		fclose(fp);
		remove(tempPath.c_str());
		throw exception_etc(*past, pnode, "Error writing wav header").raise();
	}
	uint16_t val, val2 = 0;
	bool ok = true;
	if (audioCopy.next) {
		for (uint64_t k = 0; k < audioCopy.nSamples && ok; k++) {
			val = (int16_t)(audioCopy.buf[k] * 32768);
			val2 = (int16_t)(audioCopy.next->buf[k] * 32768);
			if (fwrite(&val, 1, 2, fp) != 2 || fwrite(&val2, 1, 2, fp) != 2) ok = false;
		}
	} else {
		for (uint64_t k = 0; k < audioCopy.nSamples && ok; k++) {
			val = (int16_t)(audioCopy.buf[k] * 32768);
			if (fwrite(&val, 1, 2, fp) != 2) ok = false;
		}
	}
	fclose(fp);
	if (!ok) {
		remove(tempPath.c_str());
		throw exception_etc(*past, pnode, "Error writing wav data").raise();
	}

	// Generate jobname: basename_MM-DD_HH_MM_SS
	time_t now = time(nullptr);
	struct tm* t = localtime(&now);
	char ts[32];
	snprintf(ts, sizeof(ts), "%02d-%02d_%02d_%02d_%02d",
		t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	string jobname = basename + "_" + ts;
	string s3Key = jobname + ".wav";

	// Upload to S3
	string errstr;
	if (!upload_to_s3(tempPath, bucket, s3Key, errstr)) {
		remove(tempPath.c_str());
		throw exception_etc(*past, pnode, ("awst: " + errstr).c_str()).raise();
	}
	remove(tempPath.c_str());

	// Build aws transcribe start-transcription-job command
	string mediaUri = "s3://" + bucket + "/" + s3Key;
	string outJsonKey = outputkey;
	if (!outJsonKey.empty() && outJsonKey.back() != '/')
		outJsonKey += "/";
	outJsonKey += jobname + ".json";  // use jobname (not basename) so each job has unique transcript

	string transcribeCmd = "aws transcribe start-transcription-job "
		"--region " + shell_quote_posix(region) + " "
		"--transcription-job-name " + shell_quote_posix(jobname) + " "
		"--media MediaFileUri=" + shell_quote_posix(mediaUri) + " "
		"--media-format wav "
		"--output-bucket-name " + shell_quote_posix(bucket) + " "
		"--output-key " + shell_quote_posix(outJsonKey);

	// Language identification: 0=single-id, 1=manual, 2+=multi-id
	if (langCodes.empty()) {
		transcribeCmd += " --identify-language";
	} else if (langCodes.size() == 1) {
		transcribeCmd += " --language-code " + shell_quote_posix(langCodes[0]);
	} else {
		transcribeCmd += " --identify-multiple-languages";
		string lo;
		for (size_t i = 0; i < langCodes.size(); i++) {
			if (i > 0) lo += ",";
			lo += "LanguageCode=" + langCodes[i];
		}
		transcribeCmd += " --language-options " + shell_quote_posix(lo);
	}
	transcribeCmd += " --settings ChannelIdentification=true";

	string jsonOutput;
	if (!run_aws_cmd_capture(transcribeCmd, jsonOutput, errstr)) {
		throw exception_etc(*past, pnode, ("awst: " + errstr).c_str()).raise();
	}

	// Parse JSON and extract TranscriptionJob inner members as struct
	CVar out;
	try {
		json j = json::parse(jsonOutput);
		if (j.contains("TranscriptionJob")) {
			json_obj_to_cvar(out, j["TranscriptionJob"]);
		} else {
			// Fallback: return minimal struct with jobname
			out.strut["TranscriptionJobName"] = jobname;
			out.strut["TranscriptionJobStatus"] = string("UNKNOWN");
		}
	} catch (const json::exception& e) {
		out.strut["TranscriptionJobName"] = jobname;
		out.strut["TranscriptionJobStatus"] = string("UNKNOWN");
		out.strut["_parse_error"] = string(e.what());
	}
	past->Sig = out;
}
