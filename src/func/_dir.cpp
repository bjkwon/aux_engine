#include "functions_common.h"
#include "utils.h"

/* Use cases:
1)  dir
2)  dir("")
3)  dir("*.wav")
4)  dir("str")
5)  dir("str0\str")
6)  dir("drive:str")
   
Cases 1, 2, 3, 4, 5: Use path from get_current_dir()
     cases 1, 2: dir [path]\*.*
	 cases 3, 4, 5: dir [path]\*.wav   dir [path]\str

Case 4, 5, 6: if str indicates a directory, dir should be applied to that directory,
           if str indicates not a directory, dir is only for that file

Test cases
dir
dir("")
dir("*.wav")
dir("str") when str doesn't exists
dir("str") when str exists as a directory
dir("str") when str exists as a file
dir("str\") when str exists as a directory
dir("str\") when str doesn't exist as a directory
dir("str\*.wav") str exists as a directory
dir("str\*.wav") str doesn't exists as a directory
dir("X:str") drive X doesn't exist
dir("C:str") drive C exists, str exists as a directory from pwd
dir("C:str") drive C exists, str exists as a file at pwd
dir("C:str") drive C exists, str doesn't exists as a directory or file from pwd

*/

string get_current_dir(); // from utils.cpp

#ifdef _WIN32
#define DIRMARKER '\\'
#define DIRMARKERSTR "\\"
#else
#include <glob.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define DIRMARKER '/'
#define DIRMARKERSTR "/"
#endif
Cfunction set_builtin_function_dir(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	// Edit from this line ==============
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { };
	vector<string> desc_arg_opt = { "filename" };
	vector<CVar> default_arg = { CVar(string("*.*"))};
	set<uint16_t> allowedTypes1 = { TYPEBIT_STRING + 1, TYPEBIT_STRING + 2 };
	ft.allowed_arg_types.push_back(allowedTypes1);
	// til this line ==============
	ft.defaultarg = default_arg;
	ft.narg1 = desc_arg_req.size();
	ft.narg2 = ft.narg1 + default_arg.size();
	return ft;
}



#ifdef _WIN32

static void update_dir_info2CVar(CVar& out, const WIN32_FIND_DATA& ls, const char *fname, const char* ext, const char* pathonly)
{
	CVar obj;
	string pwd = get_current_dir();
	obj.strut["name"] = string(fname);
	obj.strut["ext"] = string(ext);
	char fullname[256];
	if (pathonly[0])
		obj.strut["path"] = string(pathonly);
	else
	{
		char* pt = strstr(fullname, fname);
		if (pt) *pt = 0;
		if (fullname[0])
			obj.strut["path"] = string(fullname);
		else
			obj.strut["path"] = pwd;
	}
	obj.strut["bytes"] = CVar((float)(ls.nFileSizeHigh * ((uint64_t)MAXDWORD + 1) + ls.nFileSizeLow));
	FILETIME ft = ls.ftLastWriteTime;
	SYSTEMTIME lt;
	FileTimeToSystemTime(&ft, &lt);
	sprintf(fullname, "%02d/%02d/%4d, %02d:%02d:%02d", lt.wMonth, lt.wDay, lt.wYear, lt.wHour, lt.wMinute, lt.wSecond);
	obj.strut["time"] = string(fullname);
	CVar b(float(ls.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? 1. : 0.));
	obj.strut["isdir"] = b;
	out.appendcell(obj);
}
#else
static void update_dir_info2CVar(CVar& out, const string& fname, const string& pathonly)
{
	CVar obj;
	obj.strut["name"] = fname;
	obj.strut["path"] = pathonly;
	struct stat st;
	stat(fname.c_str(), &st);
	obj.strut["bytes"] = CVar((float)st.st_size);
	CVar b((float)S_ISDIR(st.st_mode));
	obj.strut["isdir"] = b;
	obj.strut["time"] = string(ctime(&st.st_mtime));
	out.appendcell(obj);
}
#endif

void _dir(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string pwd = get_current_dir();
	string arg = past->Sig.str();
	bool arg_ending_back_slash = false;

	if (arg.empty()) arg += ".";
#ifdef _WINDOWS
	bool dot_or_dotdot = false;
	if (arg == "." || "..") dot_or_dotdot = true;
	if (arg.back() == DIRMARKER)
	{
		arg_ending_back_slash = true;
		arg += ".";
	}
	if (arg == "..") { arg += DIRMARKER; arg += "."; }
	WIN32_FIND_DATA ls;
	HANDLE hFind = FindFirstFile(arg.c_str(), &ls);	
	char curDirPath[MAX_PATH]; // where the FindFirstFile operation is based

	char drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH], pathonly[MAX_PATH] = {};
	_splitpath(arg.c_str(), drive, dir, fname, ext);
	string drivestr;
	if (!drive[0]) 
	{
		_splitpath(pwd.c_str(), drive, NULL, NULL, NULL);
	}
	// if dir begins with \ --> absolute path 
	//          if drive is empty, get current drive (from get_current_dir()) and make curDirPath
	//          otherwise, make curDirPath from drive, dir
	// otherwise (relative path)
	//          if both drive and dir are empty, curDirPath is get_current_dir()
	//          if only drive is empty, get current drive (from get_current_dir()) followed by dir
	//          if only dir is empty, drive followed by current directory of the folder using GetFullPathName
	// In addition, if arg ends with \ and ls indicates a file, 
	// make that statement illegal (that's the behavior in the Windows cmd).
	// for example, dir ("name\") is called when name is a file, not a directory
	auto bb = ls.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	bool path3;
	if (hFind == INVALID_HANDLE_VALUE || (arg_ending_back_slash  && !(ls.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
	{
		// no file found. return NULL
		past->Sig.Reset(1);
		return;
	}	
	auto specified_arg_is_directory = ls.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && !dot_or_dotdot;
	if (dir[0] && dir[0] == DIRMARKER) // absolute path
	{
		if (!drive[0]) // drive empty
		{
			_splitpath(pwd.c_str(), curDirPath, NULL, NULL, NULL);
			strcat(curDirPath, dir);
		}
		else
		{
			strcpy(curDirPath, drive);
			strcat(curDirPath, dir);
		}
	}
	else // relative path
	{
		if (!drive[0]) // drive empty
		{
			if (!dir[0]) // dir empty
			{
				strcpy(curDirPath, pwd.c_str());
				if (curDirPath[strlen(curDirPath) - 1] != '\\') strcat(curDirPath, "\\");
			}
			else
			{
				char currentDir[MAX_PATH];
				_splitpath(pwd.c_str(), curDirPath, currentDir, NULL, NULL);
				strcat(curDirPath, currentDir);
			}
		}
		else
		{
			path3 = true;
			if (!dir[0]) // dir empty
			{
				strcpy(curDirPath, drive);
				char** pt = { NULL };
				GetFullPathName(drive, sizeof(dir), dir, pt);
				strcpy(curDirPath, dir);
			}
		}
	}
	while (strcmp(ls.cFileName, ".") && ls.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{ // the first find is a directory, need to update curDirPath
		if (specified_arg_is_directory)
		{
			strcat(curDirPath, ls.cFileName);
			strcat(curDirPath, "\\");
		}
		arg += "\\*.*";
		hFind = FindFirstFile(arg.c_str(), &ls);
	}
	past->Sig.Reset();
	bool pathonly_corrected_dotdot = false;
	do {
		_splitpath(ls.cFileName, NULL, NULL, fname, ext);
		if (strcmp(ls.cFileName, ".") && strcmp(ls.cFileName, ".."))
		{
			if (!pathonly_corrected_dotdot && arg.size() > 1 && arg.substr(0, 2) == "..")
			{
				// if arg begins with dotdot .. , curDirPath should be go up by one
				if (curDirPath[strlen(curDirPath) - 1] == '\\') curDirPath[strlen(curDirPath) - 1] = 0;
				auto pt = strrchr(curDirPath, '\\');
				if (pt) *(pt+1) = 0;
				pathonly_corrected_dotdot = true;
			}
			update_dir_info2CVar(past->Sig, ls, fname, ext, curDirPath);
		}
	} while (FindNextFile(hFind, &ls));
#else
	if (arg.empty()) arg += "*";
	if (arg == "..") { arg += DIRMARKER; arg += "*"; }
	string cwd = get_current_dir();
	// does arg include DIRMARKER; if so, cd to the specified path
	string pathonly = get_path_only(arg.c_str());
	string name;
	if (!pathonly.empty())
	{
		chdir(pathonly.c_str());
		name = get_name_only(arg.c_str());
		pathonly = get_current_dir();
	}
	else
	{
		pathonly = cwd;
		name = arg;
	}
	glob_t gbuf = { 0 };
	glob(arg.c_str(), GLOB_DOOFFS, NULL, &gbuf);
	for (size_t k = 0; k < gbuf.gl_pathc; k++)
	{
		update_dir_info2CVar(past->Sig, gbuf.gl_pathv[k], pathonly);
	}
	chdir(cwd.c_str());
	globfree(&gbuf);
#endif
}
