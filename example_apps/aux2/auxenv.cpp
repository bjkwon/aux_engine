#include <iostream>
#include <fstream>
#include <sstream>
#include "console.h"
#include "portaudio.h"
#include <auxe/auxe.h>
#include "utils.h"
#include <array>

#ifdef _WIN32
#include <direct.h>
#else
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <iomanip>
#endif

using namespace std;

#include "nlohmann/json.hpp"
using json = nlohmann::json;


int str2vector(vector<string>& out, const string& in, const string& delim_chars); // utils.cpp
string get_current_dir(); // utils.cpp
int play_audio(auxContext* ctx, const AuxObj& obj); //audioplay.cpp

std::string base_name(const std::string& path)
{
#ifdef _WIN32
	const char sep = '\\';
#else
	const char sep = '/';
#endif

	size_t pos = path.find_last_of(sep);
	if (pos == std::string::npos) return path;
	return path.substr(pos + 1);
}

std::string remove_ext(const std::string& filename)
{
	size_t pos = filename.find_last_of('.');
	if (pos == std::string::npos)
		return filename;  // no extension
	return filename.substr(0, pos);
}
static string set_udf(auxContext* ctx, const string& udffullpath)
{
	auto base = base_name(udffullpath);
	auto udfname = remove_ext(base);
	auto pt = udffullpath.find_last_of(base) - base.size();
	auto pathonly = udffullpath.substr(0, pt);
	string estr;
	aux_define_udf(ctx, udfname, pathonly, estr);
	return estr;
}

static int appcontrol_udfpath(auxContext* ctx, const string& args, string& out)
{
	size_t nItems;
	vector<string> parsed; // command delimitered with space
	nItems = str2vector(parsed, args.c_str(), " ");
	ostringstream outstream;
	if (nItems == 0) {
		out = aux_get_udfpath(ctx);
		return 0;
	}
	if (parsed[1].front() == ':') {
		if (parsed[1].size() < 2)
			return -1;
		if (nItems == 1)
			return -2; // need additional arg
		// treat the trailing directory marker
		string rest = args.substr(args.find_first_of(parsed[2]));
		switch (parsed[1][1]) {
		case 'a': // add path
			aux_add_udfpath(ctx, rest);
			break;
		case 'r': // remove path
			if (aux_remove_udfpath(ctx, rest))
			{
				if (!rest.empty())
					outstream << "Invalid path or directory not found: " << rest << endl;
				out = outstream.str();
				return -3; //directory not found
			}
			break;
		}
		return 0;
	}
	return -1;
}

// > udf {full_path}
// > udfpath --> displays current pathsappcontrol_udfpath
// > udfpath :a mypath --> add path
// > udfpath :r thispath --> remove path
// > vars --> display variables
// > version --> show version
// > precision --> no arg: show precision, one arg: set precision
void appcontrol(auxContext* ctx, int precision, const string& cmd)
{
	size_t nItems, k(0);
	vector<string> parsed; // command delimitered with space
	nItems = str2vector(parsed, cmd.c_str(), " ");
	if (nItems == 0) return;
	if (parsed.front() == "udfpath") {
		string out;
		if (!appcontrol_udfpath(ctx, cmd.substr(0, parsed[0].size() + 1), out))
		{
			cout << out;
		}
	}
	else if (parsed.front() == "udf") {
		auto estr = set_udf(ctx, cmd.substr(0, parsed[0].size() + 1));
		if (!estr.empty())
			cout << estr;
	}
	else if (parsed.front() == "vars") {
		auto varlist = aux_enum_vars(ctx);
		for (auto v : varlist) {
			AuxObj obj = aux_get_var(ctx, v.c_str());
			cout << v << ' ' << "0x" << setw(4) << setfill('0') << hex << aux_type(obj) << ' ' << endl;
		}
	}
	else if (parsed.front() == "version") {
		cout << "aux2 core v." << aux_version(ctx) << " aux2 console v." << AUX2_VERSION << endl;
	}
	else if (parsed.front() == "precision") {
		if (nItems == 1) {
			// view precision 
			cout << "Current number display precision:" << precision << endl;
		}
		else {
			try {
				auto val = stoi(parsed[1]);
				if (val > 64)
					throw std::out_of_range("Value must be less than 64.");
				cout << "Number display precision adjusted to " << precision << endl;
			}
			catch (const std::invalid_argument& e) {
				std::cout << e.what() << "Invalid argument: not a valid integer for displace precision.\n";
			}
			catch (const std::out_of_range& e) {
				std::cout << e.what() << "Out of range: value too large for int\n";
			}
			catch (const std::exception&) {
				throw;  // rethrow for caller
			}
		}
	}
	else if (parsed.front() == "debug") {
		if (nItems == 1) {
			cout << "To set/clear debugger breakpoints: >debug udf_name line1 line2 line2...  (negatiev line number to clear it; 0 to clear all):" << endl;
		}
		else {
			auto udfname = parsed[1];
			auto res = aux_register_udf(ctx, udfname);
			if (res) {
				cout << "Cannot register udf. (is the udf file available?) " << endl;
			}
			else {
				auto pt = cmd.find(udfname);
				auto rest_str = cmd.substr(pt + udfname.size() + 1);
				istringstream iss(rest_str);
				vector<int> breakpoints;
				int val;
				while (true) {
					if (!(iss >> val)) break;
					breakpoints.push_back(val);
				}
				if (!iss.eof()) {
					throw std::invalid_argument("Invalid integer for debug breakpoints");
				}
				if (breakpoints.size() == 1 && breakpoints.front() == 0)
					aux_debug_del_breakpoints(ctx, udfname.c_str(), breakpoints); // remove all breakpoints
				//separate negative from positive
				vector<int> breakpoints_positive;
				vector<int> breakpoints_negative;
				for (auto v : breakpoints) {
					if (v > 0) breakpoints_positive.push_back(v);
					else if (v < 0) breakpoints_negative.push_back(v);
					else throw std::invalid_argument("debug breakpoints array must not contain zero.");
				}
				aux_debug_add_breakpoints(ctx, udfname.c_str(), breakpoints_positive);
				aux_debug_del_breakpoints(ctx, udfname.c_str(), breakpoints_negative);
			}
		}
	}
	else if (parsed.front() == "play") {

		if (nItems == 2) {
			auto varname = parsed[1];
			auto obj = aux_get_var(ctx, varname);
			if (obj != nullptr && aux_is_audio(obj)) {
				play_audio(ctx, obj);


			}

		}

	}
}
#define AUXENV_FILE "auxenv.json"

int read_auxenv(int& fs0, vector<string>& auxpathfromenv, int& precision, const string& envfilename)
{
	ifstream envfstream;
	json jenv;
	try {
		envfstream.open(envfilename);
		if (envfstream.fail())
			throw envfilename;
		stringstream buffer;
		buffer << envfstream.rdbuf();
		jenv = json::parse(buffer.str());
		auto fd = jenv.find("fs");
		if (fd != jenv.end())
			fs0 = jenv["fs"].get<int>();
		fd = jenv.find("precision");
		if (fd != jenv.end())
			precision = jenv["precision"].get<int>();
		fd = jenv.find("AuxPath");
		if (fd != jenv.end()) {
			if ((*fd).type_name() == "array") {
				for (auto el : (*fd)) {
					string item = el;
					if (item.back() != DIRMARKER)
						item += DIRMARKER;
					auxpathfromenv.push_back(item);
				}
			}
		}
		return 1;
	}
	catch (const string& fname) {
		cout << "Environment file not found--- " << fname << endl;
	}
	catch (json::parse_error e) {
		cout << e.what() << endl;
	}
	catch (json::type_error e) {
		cout << e.what() << endl;
	}
	return 0;
}

void save_auxenv(auxContext* ctx, int display_precision, const string& envfilename)
{
	ofstream envfstream;
	json jenv; 
	jenv["fs"] = aux_get_fs(ctx);
	jenv["precision"] = display_precision;
	vector<string> auxpath;
	string udfpaths = aux_get_udfpath(ctx);
	auto nItems = str2vector(auxpath, udfpaths.c_str(), "\n");
	if (udfpaths.size()==0 || nItems==0)
		auxpath.push_back("");
	for (auto s : auxpath)
		jenv["AuxPath"].push_back(s);
	try {
		envfstream.open(envfilename);
		if (envfstream.fail())
			throw envfilename;
		envfstream << jenv << endl;
	}
	catch (const string& fname) {
		cout << "Environment file cannot be opened for writing--- " << fname << endl;
	}
	catch (json::parse_error e) {
		cout << e.what() << endl;
	}
	catch (json::type_error e) {
		cout << e.what() << endl;
	}
}