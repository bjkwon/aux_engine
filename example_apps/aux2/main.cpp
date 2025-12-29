// ppp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cassert>
#include <auxe/aux2_core.h>
#include <filesystem> // Include the C++17 filesystem library
#include "portaudio.h"
#include "console.h"
#include "utils.h"
#ifndef _WIN32
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif

using namespace std;

#define AUXENV_FILE "auxenv.json"
#define DEFAULT_FS 22050
#define PRECISION 6 // the default precision tested in Windows11, Visual Studio 2019

void appcontrol(auxContext* ctx, int precision, const string& cmd);

auxConfig cfg;

//extern vector<AuxScope*> xscope;

void filesystem_call(const vector<string> cmd)
{
	if (cmd.front() == "pwd") {
		try {
			cout << filesystem::current_path().string() << endl;
		}
		catch (filesystem::filesystem_error& e) {
			cerr << "Error getting current path: " << e.what() << endl;
		}
	}
	else { // cmd.front() must be "cd"
		error_code ec;
		std::filesystem::current_path(cmd[1].c_str(), ec);
		if (ec) {
			std::cerr << "Error changing directory: " << ec.message() << std::endl;
		}
	}
}

// Any input is one of the following:
// # shell command prompter
// / debugging prompter, only valid during debugging
// > app control--precision, udfpath, udf, debug, etc
// or a regular aux syntax
// interpreter() handles the first dispatch.
auxDebugAction interpreter(auxContext* ctx, int display_precision, const string& instr, bool show)
{
	auto cmd = instr;
	triml(cmd, " ");
	vector<string> shellcmd;
	string res;
	size_t pos;
	switch (cmd.front()) {
	case '#': // shell command
		pos = cmd.find_first_of('#');
		str2vector(shellcmd, cmd.substr(pos + 1), string(" "));
		if (shellcmd.size() > 0) {
			if (shellcmd.front() == "cd" || shellcmd.front() == "pwd")
				filesystem_call(shellcmd);
			else
				system(instr.substr(pos + 1).c_str());
		}
		break;
	case '>': //aux system prompter
		pos = cmd.find_first_of('>');
		appcontrol(ctx, cfg.display_precision, cmd.substr(pos + 1));
		break;
	case '/':
		pos = cmd.find_first_of('/');
		//if (sc.level < 2)
		//	throw exception_etc(sc, nodes, "Only during debugging /(debugging_control_key) can be used.").raise();
		return aux_handle_debug_key(ctx, instr.substr(pos + 1));
	default:
		cout << "input: " << cmd << endl;
		aux_eval(ctx, cmd, cfg, res);
		cout << res << endl;
		break;
	}
	return auxDebugAction::NoDebug;
}

// While debugging in progress, 
// the console input should be should be one of the following:
// 1) regular aux syntax
// 2) shell or system command: # or >
// 3) debugging key: /s /i /o /x /v
// For 1-3, no progress in CallUDF, stay in the while loop. 
// For 4, exit the loop and return the proper DebugAction

auxDebugAction console_debug_shell(const auxDebugInfo& ev) {
	string input;
	bool programExit = false;
	while (1) {
		// Think about how to display the udfname
//		cout << ev.node->str << " " << ev.line << "> ";
		getline(cin, input);
		if (!input.empty()) {
			auto res = interpreter(ev.ctx, 3, input, true);
			if (res != auxDebugAction::NoDebug)
				return res;
		}
	}
	return auxDebugAction::Continue;
}

int main(int argc, char** argv)
{
	srand((unsigned)time(0));
	int fs0;
	vector<string> auxpathfromenv;
	int precision;
	read_auxenv(fs0, auxpathfromenv, precision, AUXENV_FILE);
	if (fs0 == 0) fs0 = DEFAULT_FS;
	if (precision == 0) precision = PRECISION;

	cfg.display_limit_x = 10;
	cfg.display_limit_y = 10;
	cfg.display_precision = precision;
	cfg.search_paths = auxpathfromenv;
	cfg.sample_rate = fs0;
	cfg.debug_hook = console_debug_shell;
	auxContext *ctx = aux_init(&cfg);
	if (!ctx) {
		cout << "AUX Engine failed to initialize." << endl;
		return 0;
	}
	cout << " AUX Console version " << aux_version(ctx) << " aux2 core version " << AUX2_VERSION << endl;
//	xscope.push_back(&sc);
	string input;

	if (argc == 1) {
		PaError err = Pa_Initialize();
		if (err != paNoError) {
			printf("error play()\n");
		}
		string line;
		bool programExit = false;
#ifndef _WIN32
		ifstream historyfstream("aux2.history", istream::in);
		while (getline(historyfstream, line))
		{
			add_history(line.c_str());
		}
		historyfstream.close();
#endif
		int commandid = 0;
		while (1)
		{
			try {
#ifdef _WIN32
				printf("AUX> ");
				getline(cin, input);
#else
				input.clear();
				char* readbuf = programExit ? readline("") : readline("AUX> ");
				if (readbuf == NULL)
				{
					cout << "Internal Error: readline; Program will exit.\n";
					break;
				}
				if (strlen(readbuf) > 0)
				{
					add_history(readbuf);
					ofstream historyfstream2("aux2.history", ostream::out | ios::app);
					historyfstream2 << readbuf << endl;
					historyfstream2.close();
					input = readbuf;
					free(readbuf);
				}
#endif
				if (!input.empty())
				{
					interpreter(ctx, cfg.display_precision, input, true);
					programExit = false;
				}
				else
				{
					if (programExit) break;
					programExit = true;
					cout << "Press [ENTER] to quit" << endl;
				}
			}
			//catch (AuxScope* ast) {
			//	// When aborting from udf. 
			//	assert(ast->u.debugstatus == abort2base);
			//	sc.son.reset();
			//}
			//catch (AuxScope_exception e) {
			//	cout << "Error: " << e.getErrMsg() << endl;
			//}
			catch (const char* msg) {
				cout << "Error: " << msg << endl;
			}
		}
	}
	else {
		input = argv[1];
		auto pos = input.find(".aux");
		input += "(";
		if (pos!= string::npos)
			input.replace(pos, strlen(".aux"), "");
		for (int k = 2; k < argc; k++) {
			input += "\"";
			input += argv[k];
			input += "\"";
			if (k < argc - 1) input += ", ";
		}
		input += ")";
		try {
			interpreter(ctx, cfg.display_precision, input, false);
		}
		//catch (AuxScope_exception e) {
		//	cout << "Error: " << e.getErrMsg() << endl;
		//}
		catch (const char* msg) {
			cout << "Error: " << msg << endl;
		}
	}
	save_auxenv(ctx, cfg.display_precision, AUXENV_FILE);
	Pa_Terminate();
	aux_close(ctx);
	return 0;
}

