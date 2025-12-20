#include "functions_common.h"
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

Cfunction set_builtin_function_json(fGate fp)
{
	Cfunction ft;
	set<uint16_t> allowedTypes;
	ft.func = fp;
	ft.alwaysstatic = true;
	vector<string> desc_arg_req = { "json_filename", };
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

void json2CVar(CVar& out, const json& in, AuxScope* past, const AstNode* pnode, const string& fname)
{
	auto umap = in.get<unordered_map<string, json>>();
	for (auto v : umap) {
		if (v.second.type_name() == "object") {
			CVar childobj;
			json2CVar(childobj, v.second, past, pnode, fname);
			out.strut[v.first] = childobj;
		}
		else if (v.second.type_name() == "array") {
			CVar cellarrayobj;
			for (auto element : v.second) {
				CVar tempobj;
				if (element.type_name() == "object" || element.type_name() == "array") {
					json2CVar(tempobj, element, past, pnode, fname);
					cellarrayobj.cell.push_back(tempobj);
					out.strut[v.first] = cellarrayobj;
				}
				else {
					if (element.type_name() == "string")
						out.strut[v.first].cell.push_back(CVar(element.get<string>()));
					else if (element.type_name() == "number")
						out.strut[v.first].cell.push_back(CVar(element.get<float>()));
					else if (element.type_name() == "boolean")
						out.strut[v.first].cell.push_back(CVar(element.get<bool>())); // check
					else if (element.type_name() == "null")
						out.strut[v.first].cell.push_back(CVar());
				}
			}
		}
		else if (v.second.type_name() == "string")
			out.strut[v.first] = v.second.get<string>();
		else if (v.second.type_name() == "number")
			out.strut[v.first] = v.second.get<float>();
		else if (v.second.type_name() == "boolean") {
			out.strut[v.first] = (float)v.second.get<bool>();
			out.strut[v.first].MakeLogical();
		}
		else if (v.second.type_name() == "null")
			out.strut[v.first] = CVar();
		else
			exception_func(*past, pnode, "Error reading file", fname).raise();
	}
}

void _json(AuxScope* past, const AstNode* pnode, const vector<CVar>& args)
{
	string filename = past->Sig.str();
	string errstr, content;
	CVar out;

	ifstream infc; // input file stream carrier
	try {
		infc.open(filename);
		if (infc.fail())
			throw filename; 
		stringstream buffer;
		buffer << infc.rdbuf();
		json jdata = json::parse(buffer.str());
		json2CVar(out, jdata, past, pnode, filename);
		past->Sig = out;
	}
	catch (const string& fname) {
		exception_func(*past, pnode, "Error reading file", fname).raise();
	}
}

