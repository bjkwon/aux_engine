#include "functions_common.h"
#include <algorithm>
#include <cassert>
#include <string.h> // aux_file
#include "_file_wav.h"

int str2vector(vector<string>& out, const string& in, const string& delim_chars);
int GetFileText(const char* fname, const char* mod, string& strOut); // utils.cpp
CSignal __resample(const CSignal& base, void* pargin, void* pargout); // from movespec.cpp; pargin is vector<CVar>

Cfunction set_builtin_function_wave(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "filename", };
	vector<string> desc_arg_opt = { "start_time", "end_time", };
	vector<CVar> default_arg = { CVar(0.f), CVar(-1.f) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 1, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_file(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "filename", };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_wavwrite(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "audio_signal", "filename" };
	vector<string> desc_arg_opt = { "option" };
	vector<CVar> default_arg = { CVar(string("")) };
	set<uint16_t> allowedTypes1 = { ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fopen(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "filename", "mode"};
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes1);
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fclose(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID" };
	vector<string> desc_arg_opt = { };
	vector<CVar> default_arg = { };
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2};
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fread(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID", "precision", };
	vector<string> desc_arg_opt = { "additional_arg"};
	vector<CVar> default_arg = { CVar(string("")) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_fwrite(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "FILE_ID", "object", "precision", };
	vector<string> desc_arg_opt = { "additional_arg" };
	vector<CVar> default_arg = { CVar(string("")) };
	ft.defaultarg = default_arg;
	set<uint16_t> allowedTypes1 = { TYPEBIT_BYTE + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { 0xFFFF }; // accepting all
	ft.allowed_arg_types.push_back(allowedTypes2);
	set<uint16_t> allowedTypes3 = { TYPEBIT_STRING, TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes3);
	ft.allowed_arg_types.push_back(allowedTypes3);
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

Cfunction set_builtin_function_write(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = false;
	vector<string> desc_arg_req = { "object", "filename" };
	vector<string> desc_arg_opt = { "option" };
	vector<CVar> default_arg = { CVar(string("")) };
	set<uint16_t> allowedTypes1 = { 1, 2, 3, TYPEBIT_COMPLEX + 1, TYPEBIT_COMPLEX + 2, TYPEBIT_COMPLEX + 3, 
		TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, TYPEBIT_STRING + 3,
		TYPEBIT_SIZE1 + 1, TYPEBIT_SIZE1 + 2, TYPEBIT_SIZE1 + 3,
		ALL_AUDIO_TYPES };
	ft.allowed_arg_types.push_back(allowedTypes1);
	set<uint16_t> allowedTypes2 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2, };
	ft.allowed_arg_types.push_back(allowedTypes2);
	ft.allowed_arg_types.push_back(allowedTypes2);
	// til this line ==============
	ft.desc_arg_req = desc_arg_req;
	ft.desc_arg_opt = desc_arg_opt;
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}

static inline bool isnumeric(const char* buf)
{
	for (size_t k = 0; k < strlen(buf); k++)
	{
		if (buf[k] <= 0 && buf[k] >= 9)
			continue;
		else
		{
			if (buf[k] != '\0' && buf[k] != '\t')
				return false;
		}
	}
	return true;
}

static void EnumAudioVariables(AuxScope* past, vector<string>& var)
{
	var.clear();
	for (map<string, CVar>::iterator it = past->Vars.begin(); it != past->Vars.end(); it++)
		if (ISAUDIO(it->second.type())) var.push_back(it->first);
}

void _fopen(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = past->Sig.str();
	string mode = args[0].str();
	FILE* fl;
	if (!(fl = fopen(filename.c_str(), mode.c_str())))
	{
		throw exception_etc(*past, pnode, "fopen failed.").raise();
	}
	else
	{
		past->Sig.Reset(2);
		past->Sig.strbuf = new char[sizeof(uint64_t)];
		memcpy(past->Sig.buf, &fl, sizeof(uint64_t));
		past->Sig.nSamples = sizeof(uint64_t);
		past->Sig.bufType = 'B';
	}
}

void _fclose(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	auto fp = (FILE*)*(uint64_t*)past->Sig.buf;
	if (fclose(fp) == EOF)
	{
		past->Sig.SetValue(-1.);
	}
	else
	{
		past->Sig.SetValue(0);
	}
}

/* fwrite: audio signal --> rescale from -1 to 1 to each integer range corresponding to the format,
   e.g., -32768 to 32767 for int16, 0 to 65535 for uint16
   if it is stereo, writes the data in an interleaved manner for each channel
   nonaudio signal --> write as is.. don't care whether it is outside of the range.

   fread: reads the data according to the format, e.g., -2^31 to 2^31 for int32, makes a non-audio object
   if the last arg is "a" or "audio," it rescales in the range and makes the object audio (mono)
   if the last arg is "a2" or "audio2," it rescales in the range and makes the object audio (stereo)
*/

static FILE* prepare_freadwrite(AuxScope* past, const AstNode* pnode, const vector<CVar>& args, int& bytes, string& prec, char* additional)
{
	//first arg is always file identifier
	//second arg is the signal to write to file
	//third arg is precision--one of the following: int8 int16 int32 uint8 uint16 uint32 char float double

	auto fp = (FILE*)*(uint64_t*)past->Sig.buf;
	string fname = pnode->str;
	auto arg = args.begin();
	if (fname == "fwrite") arg++;
	prec = (*arg).str();
	if (prec == "int8" || prec == "uint8" || prec == "char" || prec == "byte" || prec == "void")
		bytes = 1;
	else if (prec == "int16" || prec == "uint16")
		bytes = 2;
	else if (prec == "int24")
		bytes = 3;
	else if (prec == "float" || prec == "int32" || prec == "uint32")
		bytes = 4;
	else if (prec == "double")
		bytes = 8;
	else
		throw exception_func(*past, pnode, "Second arg must be one of the following: char byte int16 uint16 float int32 uint32 double", fname).raise();
	if (fname == "fread")
		strcpy(additional, (*++arg).str().c_str());
	return fp;
}

template<typename T>
size_t fwrite_general_floating(T var, const CVar& sig, string prec, FILE* file)
{
	int k = 0;
	T* pvar = &var;
	if (sig.next)
	{
		auxtype* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, file, &k](auxtype v) {
				*pvar = (T)v; fwrite(pvar, sizeof(T), 1, file);
				*pvar = (T)buf2[k++]; fwrite(pvar, sizeof(T), 1, file); });
	}
	else
	{
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, file](auxtype v) { *pvar = (T)v; fwrite(pvar, sizeof(T), 1, file); });
	}
	return sig.nSamples;
}

template<typename T>
size_t fwrite_general(T var, const CVar& sig, string prec, FILE* file, int bytes, uint64_t factor)
{
	auto tp = sig.type();
	if (!ISAUDIO(tp))
		factor = 1;
	int k = 0;
	T* pvar = &var;
	if (sig.next)
	{
		auxtype* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, factor, bytes, file, &k](auxtype v) {
				*pvar = (T)(factor * v - .5); fwrite(pvar, bytes, 1, file);
				*pvar = (T)(factor * buf2[k++] - .5); fwrite(pvar, bytes, 1, file); });
	}
	else
	{
		if (ISAUDIO(tp))
			for_each(sig.buf, sig.buf + sig.nSamples,
				[pvar, bytes, factor, file](auxtype v) { *pvar = (T)(factor * v - .5); fwrite(pvar, bytes, 1, file); });
		else
			for_each(sig.buf, sig.buf + sig.nSamples,
				[pvar, bytes, factor, file](auxtype v) { *pvar = (T)(v - .5); fwrite(pvar, bytes, 1, file); });
	}
	return sig.nSamples;
}


// As of 12/22/2022, if aux2 was built with float, fwrite may generate double, but that's not true double (it's float in double format)
// Likewise, fread with double may succeed, but the data is stored in the float buffer
void _fwrite(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	CVar firstarg = args[0];
	int bytes;
	string prec;
	size_t res=-1;
	FILE* file = prepare_freadwrite(past, pnode, args, bytes, prec, NULL);
	const CVar* pobj = NULL;
	auto tp = args[0].type();
	if (ISAUDIO(tp))
	{
		if (tp & TYPEBIT_TEMPO_CHAINS || (tp & TYPEBIT_TEMPO_ONE && args[0].tmark > 0))
		{
			CVar copy = args[0];
			copy.MakeChainless();
			pobj = &copy;
		}
	}
	if (!pobj) pobj = &args[0];
	if (prec == "void" || prec == "byte")
	{
		res = fwrite(args[0].buf, 1, args[0].nSamples, file);
	}
	else if (prec == "char") 
	{
		if (pobj->IsString())
			res = fwrite(pobj->strbuf, 1, pobj->nSamples, file);
		else
		{
			char temp = 0;
			res = fwrite_general(temp, *pobj, prec, file, bytes, 0x100);
		}
	}
	else if (prec == "int8")
	{
		int8_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x80);
	}
	else if (prec == "uint8")
	{
		uint8_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x100);
	}
	else if (prec == "int16")
	{
		int16_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x8000);
	}
	else if (prec == "uint16")
	{
		uint16_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x10000);
	}
	//else if (prec == "int24")
	//{
	//	int32_t temp = 0; // in24_t doesn't exist
	//	res = fwrite_general(temp, *pobj, prec, file, bytes, 0x800000);
	//}
	else if (prec == "int32")
	{
		int32_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0x80000000);
	}
	else if (prec == "uint32")
	{
		uint32_t temp = 0;
		res = fwrite_general(temp, *pobj, prec, file, bytes, 0xffffffff);
	}
	else if (prec == "float")
	{ // No automatic scaling
		float temp = 0;
		res = fwrite_general_floating(temp, *pobj, prec, file);
	}
	else if (prec == "double")
	{ // No automatic scaling
		double temp = 0;
		res = fwrite_general_floating(temp, *pobj, prec, file);
	}
	past->Sig.SetValue((float)res);
}

template<typename T>
void fread_general(T var, CVar& sig, FILE* file, int bytes, char* addarg, uint64_t factor)
{
	T* pvar = &var;
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a"))
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, bytes, file, factor](auxtype& v) { fread(pvar, bytes, 1, file); v = *pvar / (auxtype)factor; });
	else if (!strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
	{
		int k = 0;
		CSignals next(CSignal(sig.GetFs(), sig.nSamples));
		sig.SetNextChan(next);
		auxtype* buf2 = sig.next->buf;
		for_each(sig.buf, sig.buf + sig.nSamples,
			[buf2, pvar, bytes, file, factor, &k](auxtype& v) {
				fread(pvar, bytes, 1, file); v = *pvar / (auxtype)factor;
				fread(pvar, bytes, 1, file); buf2[k++] = *pvar / (auxtype)factor; });
	}
	else
		for_each(sig.buf, sig.buf + sig.nSamples,
			[pvar, bytes, file](auxtype& v) { fread(pvar, bytes, 1, file); v = *pvar; });
}

void _fread(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	int bytes;
	string prec;
	char addarg[16] = {};
	FILE* file = prepare_freadwrite(past, pnode, args, bytes, prec, addarg);

	auto res = fseek(file, 0L, SEEK_END);
	size_t sz = ftell(file);
	res = fseek(file, 0L, SEEK_SET);

	size_t nItems = sz / bytes;
	if (prec == "char" || prec == "byte" || prec == "void")
	{ // Treat it separately just to make the code neat.
		past->Sig.SetString('\0');
		past->Sig.UpdateBuffer((unsigned int)nItems);
		fread(past->Sig.strbuf, bytes, nItems, file);
		if (prec == "byte" || prec == "void") past->Sig.bufType = 'B';
		return;
	}
	past->Sig.Reset(1); // always make it non-audio
	if (!strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
	{
		if (nItems / 2 * 2 != nItems)
			throw exception_etc( *past, pnode, "attempting to read stereo audio data but data count is not even.").raise();
		nItems /= 2;
	}
	past->Sig.UpdateBuffer((unsigned int)nItems);
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a") || !strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
		past->Sig.SetFs(past->pEnv->Fs);
	if (prec == "int8" || prec == "uint8")
	{
		int8_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80);
	}
	else if (prec == "int16" || prec == "uint16")
	{
		int16_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x8000);
	}
	else if (prec == "int24")
	{
		int32_t temp = 0; // in24_t doesn't exist
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80000000); // check
	}
	else if (prec == "int32" || prec == "uint32")
	{
		int32_t temp = 0;
		fread_general(temp, past->Sig, file, bytes, addarg, 0x80000000);
	}
	else if (prec == "float")
	{
		float temp = 0.;
		fread_general(temp, past->Sig, file, bytes, addarg, 1);
	}
	else if (prec == "double")
	{
		double temp = 0.;
		fread_general(temp, past->Sig, file, bytes, addarg, 1);
	}
	if (!strcmp(addarg, "audio") || !strcmp(addarg, "a") | !strcmp(addarg, "audio2") || !strcmp(addarg, "a2"))
		past->Sig.SetFs(past->pEnv->Fs);
}


static void write2textfile(FILE* fid, CVar* psig)
{
	if (psig->bufBlockSize == 1)
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%c ", (char)psig->logbuf[k]);
		fprintf(fid, "\n");
	}
	else if (ISAUDIO(psig->type())) // audio
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%7.4f ", psig->buf[k]);
		if (psig->next)
		{
			fprintf(fid, "\n");
			for (unsigned int k = 0; k < psig->nSamples; k++)
				fprintf(fid, "%7.4f ", psig->next->buf[k]);
		}
		fprintf(fid, "\n");
	}
	else if (!psig->cell.empty())
	{
		for (auto cel : psig->cell)
			write2textfile(fid, &cel);
	}
	else
	{
		for (unsigned int k = 0; k < psig->nSamples; k++)
			fprintf(fid, "%g ", psig->buf[k]);
		fprintf(fid, "\n");
	}
}

void _wavwrite(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = args[0].str();
	string option = args[1].str();

	string fullfilename = past->makefullfile(filename, ".wav");
	char errStr[256];
	past->Sig.MakeChainless();

	// As of 12/27/2025, write only in PCM16sle format
	WavInfo wavinfo = { 0 };
	wavinfo.audio_format = 1; // PCM only for now
	wavinfo.num_channels = (past->Sig.next) ? 2 : 1;
	wavinfo.sample_rate = past->Sig.GetFs();
	wavinfo.bits_per_sample = 2 * 8; // 16 bit for now
	wavinfo.block_align = 2 * wavinfo.num_channels; // 16 bits (2 bytes) per channel for now
	wavinfo.byte_rate = wavinfo.sample_rate * wavinfo.block_align;
	wavinfo.data_offset = 44; // ok for PCM16sle
	wavinfo.data_size = past->Sig.nSamples * wavinfo.block_align;
	wavinfo.riff_size = wavinfo.data_size + 44 - 8; // WAVE header size is 44
	FILE* fp;
	string estr;
	if ((fp = fopen(fullfilename.c_str(), "wb")) == NULL) {
		estr = string("Unable to open/write audio file:") + fullfilename;
		throw exception_etc(*past, pnode, estr.c_str()).raise();
	}
	char buffer[44];
	make_wav_header(buffer, wavinfo, past->Sig.nSamples);
	int res = fwrite(buffer, 1, 44, fp);
	if (res != 44) {
		estr = string("Error in fwrite (header): ") + fullfilename;
		throw exception_etc(*past, pnode, estr.c_str()).raise();
	}
	uint64_t k;
	uint16_t val, val2(0);
	bool complete = false;
	if (past->Sig.next) {
		for (k = 0; k < past->Sig.nSamples; k++) {
			val = (uint16_t)(past->Sig.buf[k] * 32768);
			val2 = (uint16_t)(past->Sig.next->buf[k] * 32768);
			if (fwrite(&val, 1, 2, fp) != 2) break;
			if (fwrite(&val2, 1, 2, fp) != 2) break;
			complete = k == past->Sig.nSamples-1;
		}
	}
	else {
		for (uint64_t k = 0; k < past->Sig.nSamples; k++) {
			val = (uint16_t)(past->Sig.buf[k] * 32768);
			if (fwrite(&val, 1, 2, fp) != 2) break;
			complete = k == past->Sig.nSamples-1;
		}
	}
	res = fclose(fp);
	if (!complete) {
		estr = string("Error in fwrite: ") + fullfilename;
		throw exception_etc(*past, pnode, estr.c_str()).raise();
	}
	if (res) {
		estr = string("Error in fclose: ") + fullfilename;
		throw exception_etc(*past, pnode, estr.c_str()).raise();
	}
}

void _write(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = args[0].str();
	string option = args[1].str();
//	trim(filename, ' '); // is it necessary? 1/9/2022
	size_t pdot = filename.rfind('.');
	string extension = filename.substr(pdot + 1);
	if (extension == "wav")
	{
		_wavwrite(past, pnode, args);
	}
	else if (extension == "txt")
	{
		FILE* fid = fopen(filename.c_str(), "wt");
		if (!fid)
			throw exception_etc(*past, pnode, "File creation error").raise();
		write2textfile(fid, &past->Sig);
		fclose(fid);
	}
	else
		throw exception_func(*past, pnode, "The extension must be specified .wav .mp3 or .txt").raise();
}

void _wave(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string estr;
	string filename = past->Sig.str();
	double beginMs = args[0].value();
	double durMs = args[1].value();
	WavInfo wavinfo;
	int res = wav_read_header(filename, wavinfo, estr);
	if (!res)
		throw exception_etc(past, pnode, estr).raise();

	size_t count;
	past->Sig.Reset(wavinfo.sample_rate);
	past->Sig.bufType = 'R';
	size_t id1 = (size_t)(beginMs / 1000.f * wavinfo.sample_rate + .5);
	FILE* fp = fopen(filename.c_str(), "rb"); // most likely success
	res = fseek(fp, res + id1 * wavinfo.block_align, SEEK_SET);
	int _frames2read;
	size_t frames = wavinfo.data_size / wavinfo.block_align;
	if (durMs < 0)
		_frames2read = frames - id1;
	else
		_frames2read = (size_t)(durMs / 1000.f * wavinfo.sample_rate + .5) - id1;
	size_t frames2read = max(_frames2read, 0);
	if (wavinfo.num_channels == 1)
	{
		past->Sig.UpdateBuffer((uint64_t)frames2read);
		vector<float> buffer;
		count = wav_read_float32(fp, frames2read, wavinfo, buffer, estr);
		//what if count is less than expected?
		uint64_t id = 0;
		for (auto v : buffer) {
			past->Sig.buf[id++] = v;
		}
	}
	else
	{
		vector<float> buffer;
		count = wav_read_float32(fp, frames2read, wavinfo, buffer, estr);
		//what if count is less than expected?
		auto frames = reinterpret_cast<const float(*)[2]>(buffer.data());
		size_t framesRead = count;
		past->Sig.next = new CSignals((int)wavinfo.sample_rate);
		past->Sig.UpdateBuffer((int)framesRead);
		past->Sig.next->UpdateBuffer((int)framesRead);
		for (size_t k = 0; k < framesRead; ++k) {
			past->Sig.buf[k] = frames[k][0];
			past->Sig.next->buf[k] = frames[k][1];
		}
	}
	fclose(fp);
	//Resampling if the sampling rate is different from the current environment
	int envFs = past->GetFs();
	if (envFs != past->Sig.GetFs()) {
		CVar ratio(1), argout(1);
		ratio.SetValue((float)past->Sig.GetFs()/ envFs);
		vector<CVar> arg(1, ratio);
		past->Sig = past->Sig.evoke_modsig2(__resample, &arg, &argout);
		if (ISSTRING(argout.type())) // this means there was an error during resample
			throw exception_etc(*past, pnode, argout.str()).raise();
		past->Sig.SetFs(envFs);
	}
}

static int filetype(const string& fname, string& errstr)
{
	FILE* fp = fopen(fname.c_str(), "rb");
	if (!fp) { errstr = "File not found or cannot be opened.";  return 0; }
	char buffer[16];
	auto res = fread(buffer, 1, sizeof(buffer), fp);
	if (res < sizeof(buffer)) { errstr = "File type header is corrupt or cannot be read.";  fclose(fp); return 4; } // treat it as a text file
	errstr = "";
	auto ret = buffer[0] == (char)0xFF;
	auto ret2 = buffer[1] >= (char)0xE0;
	auto ret3 = buffer[1] <= (char)0xFF;
	if (!memcmp(buffer, "RIFF", 4) && !memcmp(buffer + 8, "WAVE", 4))
		res = 1; // WAV
	else if (!memcmp(buffer, "ID3", 3) || (buffer[0] == (char)0xFF && (char)0xE0 <= buffer[1] && buffer[1] <= (char)0xFF))
		res = 2; // MP3
	else if (!strncmp(buffer, "FORM", 5))
		res = 3; // AIFF
	else
		res = 4;
	fclose(fp);
	return res;
}

void _file(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	char errStr[16];
	string filename = past->Sig.str();
	string errstr, content;
	int res = filetype(filename, errstr);
	int ret, fs, nChans;
	size_t len, nLines;
	switch (res)
	{
	case 1:
		//as of now Jan 2022, args is empty
		// supplying default args for _wave()
		((vector<CVar>*) & args)->push_back(CVar(0.));
		((vector<CVar>*) & args)->push_back(CVar(-1.));
		_wave(past, pnode, args);
		break;
	case 2:
	case 3:
		throw exception_func(*past, pnode, "mp3, aiff files are not supported currently in auxe.").raise();
		break;
	case 4:
		if (GetFileText(filename.c_str(), "rb", content))
		{
			past->Sig.Reset();
			vector<string> line;
			nLines = str2vector(line, content, "\r\n");
			for (size_t k = 0; k < nLines; k++)
			{
				//if there's at least one non-numeric character except for space and tab, treat the whole line as a string.
				if (!isnumeric(line[k].c_str())) {
					past->Sig.appendcell((CVar)line[k]);
				}
				else {
					vector<string> datavect;
					res = str2vector(datavect, line[k], " \t");
					auxtype* data = new auxtype[res];
					k = 0;
					for (auto str : datavect)
					{
#ifdef FLOAT
						if (sscanf(str.c_str(), "%f", data + k++) == EOF)
#else
						if (sscanf(str.c_str(), "%lf", data + k++) == EOF)
#endif						
							throw exception_func(*past, pnode, "Error in reading and converting to auxtype data").raise();
					}
					CSignals tp(data, res);
					if (nLines == 1)
						past->Sig = tp;
					delete[] data;
				}
			}
		}
		else
			throw exception_func(*past, pnode, "Cannot read file").raise();
		break;
	}
}
