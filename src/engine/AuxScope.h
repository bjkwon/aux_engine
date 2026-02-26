#pragma once
#include <cstring>
#include <memory> // unique_ptr
#include <set>
#include "aux_classes.h"
#include "psycon.yacc.h"
#include "psycon.tab.h"

#include <algorithm>
#define PI	3.141592

#ifdef _WIN32
#include <windows.h>
#else
typedef uint64_t INT_PTR;
#endif

using namespace std;

class AuxScope;
class CNodeProbe;

const AstNode* get_first_arg(const AstNode* pnode, bool staticfunc = false); // from support.cpp
const AstNode* get_second_arg(const AstNode* , bool); // from support.cpp


enum class DebugAction {
	NoDebug = -1,
	Continue = 0,
	Step,
	StepOut,
	StepIn,
	AbortToBase
};

struct DebugEvent {
	AuxScope* frame;           // current frame / workspace
	string filename;    // udf file name
	int line;               // convenience
};

using DebugHook = std::function<DebugAction(const DebugEvent&)>;

enum DEBUG_STATUS
{
	null = -1,
	entering,
	progress,
	step,
	step_in,
	step_out,
	continu,
	abort2base,
	cleanup,
	purgatory,
	refresh,
	typed_line,
	paused,
};

class UDF
{
public:
	AstNode* uxtree; // the syntax tree for the content of the UDF; begining with T_FUNCTION
	string fullname;
	string content;
	vector<int> DebugBreaks;
	vector<string> source;
	map<int, string> lines;
	map<string, UDF> local;
	bool newrecruit;
	UDF& operator=(const UDF& rhs);
	UDF() { uxtree = NULL; newrecruit = false; };
	multimap<CVar, AstNode*> switch_case; // why multimap instead of map? Because the key for case may appear in multiple switch blocks
	vector<int> switch_case_undefined;
	virtual ~UDF() {};
};

typedef bool (*pfunc_typecheck)(uint16_t tp);

class Cfunction
{
public:
	static bool IsSTRING(uint16_t tp);
	static bool IsSTRINGG(uint16_t tp);
	static bool IsAUDIO(uint16_t tp);
	static bool IsAUDIOG(uint16_t tp);
	static bool IsCellG(uint16_t tp);
	static bool IsNULL(uint16_t tp) { return tp == TYPEBIT_NULL; };
	static bool IsNULLG(uint16_t tp) { return !(tp & 3); };
	static bool IsScalar(uint16_t tp) { return (tp & 0b1111) == 1; };
	static bool IsNotScalar(uint16_t tp) { return (tp & 0b1111) != 1; };
	static bool IsScalarG(uint16_t tp) { return (((tp & 0b1111) & 0xF003)) == 1; };
	static bool IsNotScalarG(uint16_t tp) { return (((tp & 0b1111) & 0xF003)) != 1; };
	static bool IsVector(uint16_t tp) { return (tp & 0b1111) == 2; };
	static bool IsVectorG(uint16_t tp) { return (((tp & 0b1111) & 0xF003)) == 2; };
	static bool Is2D(uint16_t tp) { return (tp & 0xFF0F) == 3; };
	static bool Is2DG(uint16_t tp) { return (tp & 0x000F) == 3; };
	static bool IsBYTE(uint16_t tp) { return (tp & 0xFFF0) == TYPEBIT_BYTE; };
	static bool IsBYTEG(uint16_t tp) { return (tp & 0x00F0) == TYPEBIT_BYTE; };
	static bool IsBOOL(uint16_t tp) { return (tp & 0xFFF0) == TYPEBIT_SIZE1; };
	static bool IsBOOLG(uint16_t tp) { return (tp & 0x00F0) == TYPEBIT_SIZE1; };
	static bool IsCOMPLEX(uint16_t tp) { return (tp & 0b01100000) == TYPEBIT_COMPLEX; };
	static bool IsCOMPLEXG(uint16_t tp) { return (tp & (0xFF00 + 0b01100000)) == TYPEBIT_COMPLEX; };
	static bool IsTSEQ(uint16_t tp) { return (tp & 0xF0FF) == TYPEBIT_TEMPO_ONE + 1 || (tp & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 1; };
	static bool IsTSEQG(uint16_t tp) { return (tp & 0x00FF) == TYPEBIT_TEMPO_ONE + 1 || (tp & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 1; };
	static bool IsTSHOT(uint16_t tp) { return (tp & 0xF0FF) == TYPEBIT_TEMPO_ONE + 2 || (tp & 0xF0FF) == TYPEBIT_TEMPO_CHAINS + 2; };
	static bool IsTSHOTG(uint16_t tp) { return (tp & 0x00FF) == TYPEBIT_TEMPO_ONE + 2 || (tp & 0x00FF) == TYPEBIT_TEMPO_CHAINS + 2; };
	static bool IsSTEREO(uint16_t tp) { return (tp & (TYPEBIT_MULTICHANS + 0xF000)) != 0; };
	static bool IsSTEREOG(uint16_t tp) { return (tp & TYPEBIT_MULTICHANS) != 0; };
	static bool IsTEMPORAL(uint16_t tp) { return (tp & 0xFF0C) == TYPEBIT_TEMPO_ONE || (tp & 0xFF0C) == TYPEBIT_TEMPO_CHAINS || (tp & 0xFF0C) == TYPEBIT_TEMPO_CHAINS_SNAP; };
	static bool IsTEMPORALG(uint16_t tp) { return (tp & 0x000C) == TYPEBIT_TEMPO_ONE || (tp & 0x000C) == TYPEBIT_TEMPO_CHAINS || (tp & 0x000C) == TYPEBIT_TEMPO_CHAINS_SNAP; };
	static bool IsCell(uint16_t tp) { return (tp & TYPEBIT_CELL) != 0; };
	static bool IsSTRUT(uint16_t tp) { return  (( tp & TYPEBIT_STRUT) | (tp & TYPEBIT_STRUTS)) != 0; };
	static bool IsPureCell(uint16_t tp) { return (tp & TYPEBIT_CELL) != 0 && (tp & 0x00FF) == 0; };
	static bool IsCellWithFace(uint16_t tp) { return (tp & TYPEBIT_CELL) != 0 && (tp & 0x00FF) != 0; };
	static bool AllTrue(uint16_t tp) { return true; };
	static bool AllFalse(uint16_t tp) { return false; };

	static vector<uint16_t> audiotype_real;
	static vector<uint16_t> stringtype;
	static vector<uint16_t> celltype;
	vector<uint16_t> audiotype_complex;
	vector<uint16_t> audiotype_bool;
	Cfunction();
	virtual ~Cfunction() {};
	
	vector<set<pfunc_typecheck>> qualify;
	vector<set<pfunc_typecheck>> reject;

	string funcsignature;
	vector<string> desc_arg_req;
	vector<string> desc_arg_opt;
	bool alwaysstatic; // true means the function cannot be called as a member function
	int narg1, narg2; // narg1 is min number of args; narg2 is max number of args
	vector<CVar> defaultarg;
	vector<set<uint16_t>> allowed_arg_types;
	void(*func)(AuxScope*, const AstNode*, const vector<CVar>&);
};

struct FileEntry {
	FILE* fp;
	bool open;
};

class EngineRuntime
{
public:
	string version;
	static map<string, vector<CVar*>> glovar;
	static map<string, Cfunction> pseudo_vars;
	DebugHook debug_hook;
	map<string, UDF> udf;
	unordered_map<uint64_t, FileEntry> fileTable;
	uint64_t nextFD = 1;
	int Fs;
	int curLine; // used for control F10
	int inTryCatch;
	vector<string> AuxPath;
	bool shutdown;
	void InitBuiltInFunctions();
	bool IsValidBuiltin(const string& funcname);
	void InitErrorCodes();
	string path_delimited_semicolon();
	vector<string> InitBuiltInFunctionsExt(const vector<string>& externalModules);
	multimap<string, Cfunction> builtin;
	EngineRuntime(const int fs = 1);
	virtual ~EngineRuntime();
	EngineRuntime& operator=(const EngineRuntime& rhs);
	void AddPath(const string& path);
	int RemovePath(const string& path); // 0 for success; 1 if path is not found or invalid
	AstNode* checkout_udf(const string& udf_filename, const string& filecontent);
	AstNode* checkin_udf(const string& udf_filename, const string& fullpath, const string& filecontent, string& emsg);
	typedef CVar* (EngineRuntime::* xflow_func)(AuxScope* psk, const AstNode* pnode);
	map<int, xflow_func> xff;
	vector<int> type_arith_op;
	vector<int> type_condition;
	vector<int> type_blockflow;
	CVar* BLOCK(AuxScope* psk, const AstNode* p);
	CVar* FOR(AuxScope* psk, const AstNode* p);
	CVar* IF(AuxScope* psk, const AstNode* p);
	CVar* WHILE(AuxScope* psk, const AstNode* p);
	CVar* SWITCH(AuxScope* psk, const AstNode* p);
	CVar* TRY(AuxScope* psk, const AstNode* p);
	CVar* CATCH(AuxScope* psk, const AstNode* p);
	CVar* ID(AuxScope* psk, const AstNode* pnode);
	CVar* TSEQ(AuxScope* psk, const AstNode* pnode);
	CVar* NUMBER(AuxScope* psk, const AstNode* pnode);
	CVar* STRING(AuxScope* psk, const AstNode* pnode);

	CVar* MATRIX(AuxScope* psk, const AstNode* pnode);
	CVar* VECTOR(AuxScope* psk, const AstNode* pnode);
	CVar* REPLICA(AuxScope* psk, const AstNode* pnode);
	CVar* ENDPOINT(AuxScope* psk, const AstNode* pnode);
	CVar* ARITH_PLUS(AuxScope* psk, const AstNode* pnode);
	CVar* ARITH_MINUS(AuxScope* psk, const AstNode* pnode);
	CVar* ARITH_MULT(AuxScope* psk, const AstNode* pnode);
	CVar* ARITH_DIV(AuxScope* psk, const AstNode* pnode);
	CVar* MATRIXMULT(AuxScope* psk, const AstNode* pnode);
	CVar* ARITH_MOD(AuxScope* psk, const AstNode* pnode);
	CVar* TRANSPOSE(AuxScope* psk, const AstNode* pnode);
	CVar* NEGATIVE(AuxScope* psk, const AstNode* pnode);
	CVar* TIMESHIFT(AuxScope* psk, const AstNode* pnode);
	CVar* CONCAT(AuxScope* psk, const AstNode* pnode);
	CVar* LOGIC(AuxScope* psk, const AstNode* pnode);
	CVar* LEVELAT(AuxScope* psk, const AstNode* pnode);
	CVar* INITCELL(AuxScope* psk, const AstNode* pnode);
	CVar* BREAK(AuxScope* psk, const AstNode* pnode);
	CVar* RETURN(AuxScope* psk, const AstNode* pnode);
};

class CUDF
{
public:
	string title;
	string base;
	AstNode* t_func; // T_FUNCTION node
	AstNode* t_func_base; // T_FUNCTION node for base udf
	vector<string> argout; // formal output argument list; to be filled out in PrepareAndCallUDF()
	int nextBreakPoint;
	int currentLine;
	int nargin;
	const AstNode* pending_assign_lhs;
	const AstNode* pending_assign_rhs_call;
	const AstNode* paused_node;
	int paused_line;
	string paused_file;
	const AstNode* pending_catchback_next;
	const AstNode* pending_catchback_alt;
	int pending_catchback_line;
	int pending_catchback_col;
	DEBUG_STATUS debugstatus;
	CUDF() { nextBreakPoint = currentLine = -1; pending_assign_lhs = nullptr; pending_assign_rhs_call = nullptr; paused_node = nullptr; paused_line = -1; pending_catchback_next = nullptr; pending_catchback_alt = nullptr; pending_catchback_line = -1; pending_catchback_col = -1; pLastRead = NULL; };
	virtual ~CUDF() {};
	AstNode* pLastRead; //used for isthisUDFscope only, to mark the last pnode processed in 

};

class AuxScope
{
public:
	static const AstNode* find_parent(const AstNode* p, const AstNode* pME);
	static AstNode* goto_line(const AstNode* pnode, int line);
	static bool IsLooping(const AstNode* p);
	static bool IsConditional(const AstNode* pnode);

	AuxScope(string instr = ""); // check if this is still necessary 11/12/2022
	AuxScope(EngineRuntime* env);
	AuxScope(const AuxScope* src);
	virtual ~AuxScope();
	void init();
	unsigned long tic();
	unsigned long toc(const AstNode* p);
	void setstring(const string& instr) { script = instr; };
	AstNode* makenodes(const string& instr);
	CVar* Compute(const AstNode* pnode);
	vector<CVar*> Compute();
	CVar* TID(AstNode* pnode, AstNode* pRHS, CVar* psig = NULL);
	AuxScope& SetVar(const char* name, CVar* psig, CVar* pBase = NULL);
	AstNode* read_node(CVar** psigBase, AstNode* ptree);
	AstNode* read_nodes(CVar** psigBase, AstNode *pnode);
	void throw_LHS_lvalue(const AstNode* pn, bool udf);
	AstNode* searchtree(const AstNode* pTarget, AstNode* pStart);
	const AstNode* searchtree(const AstNode* p, int type, int line2check = -1);
	multimap<CVar, AstNode*> register_switch_cvars(const AstNode* pnode, vector<int>& undefined);
	AstNode* ReadUDF(string& emsg, const string& udf_filename);
	AstNode* RegisterUDF(const AstNode* p, const char* fullfilename, const string& filecontent);
	CVar* GetGlobalVariable(const AstNode* pnode, const char* varname);
	CVar* GetVariable(const char* varname, const AstNode* pnode, CVar* pvar = NULL);
	void PrepareAndCallUDF(const AstNode* pCalling, CVar* pBase, CVar* pStaticVars = NULL);
	void CallUDF(const AstNode* pnode4UDFcalled, CVar* pBase, size_t nargout_requested);
	void FinalizeChildUDFCall();
	void CompletePendingAssignmentAfterDebugResume();
	const AstNode* linebyline(const AstNode* p, bool skip_first_break_check = false, bool step_once = false, bool suppress_breakpoints = false);
	void hold_at_break_point(const AstNode* pnode);
	void ResumePausedUDF();
	FILE* fopen_from_path(const string& fname, const string& ext, string& fullfilename);
	void HandleAuxFunction(const AstNode* pnode);
	CVar* TSeq(const AstNode* pnode, AstNode* p);
	void HandleMathFunc(string& fname, const body& arg);
	CSignals gettimepoints(CTimeSeries* psig, const AstNode* pnode);
	CVar* NodeVector(const AstNode* pn);
	CVar* NodeMatrix(const AstNode* pnode);
	CVar* Dot(AstNode* p, CVar* psig);
	void Transpose(const AstNode* pnode, AstNode* p);
	CVar* Try_here(const AstNode* pnode, AstNode* p);
	void Concatenate(const AstNode* pnode, AstNode* p);
	CVar* ConditionalOperation(const AstNode* pnode, AstNode* p);
	CVar* SetLevel(const AstNode* pnode, AstNode* p);
	double find_endpoint(const AstNode* p, const CVar& var);
	void interweave_indices(CVar& isig, CVar& isig2, unsigned int len);
	void index_array_satisfying_condition(CVar& isig);
	CVar* InitCell(const AstNode* pnode, AstNode* p);
	bool checkcond(const AstNode* p);
	void switch_case_handler(const AstNode* pnode);
	int GetFs(void) { return pEnv->Fs; }
	void outputbinding_for_eval_lhs(const AstNode* pnode);
	void outputbinding(const AstNode* pnode);
	void outputbinding(const AstNode* pnode, size_t nArgout);
	void bind_psig(AstNode* pn, CVar* psig);
	string makefullfile(const string& fname, string extension = "");
	string ComputeString(const AstNode* p);
	vector<string> ClearVar(const char* var, CVar* psigBase=NULL);

	AstNode* pheadthisline;

	string script;
	string emsg;
	CVar Sig; // placeholder for the output of Compute(); used in various ways
	vector<unique_ptr<CVar>> SigExt; // placeholder for multiple output arguments
	AstNode* node;
	unique_ptr<CVar> pgo; // pointer, not a copy, of the last computed object; used for audio or graphics handles
	int level;
	vector<int> baselevel;
	map<string, CVar> Vars;
	map<string, vector<CVar*>> GOvars;
	AstNode* lhs;
	EngineRuntime* pEnv;
	CUDF u;
	CVar replica;
	unique_ptr<AuxScope> son;
	AuxScope* dad;
	const AstNode* pLast;
	const AstNode* pTryLast;
	string statusMsg; // to display any message during processing (e.g., "Sampling rate set to 22050 Hz" "This function will be obsolete.")
	unsigned long Tick0, Tick1;
	vector<float> ends;
	CVar* process_statement(const AstNode* pnode);

	bool fBreak;
	bool fExit;

	bool get_nodes_left_right_sides(const AstNode* pnode, const AstNode** plhs, const AstNode** prhs);

private:
	bool done;
	bool nodeAllocated;
	vector<CVar> make_check_args(const AstNode* pnode, multimap<string, Cfunction>::iterator& ftlist);
	void make_check_args_math(const AstNode* pnode);
	void eval_lhs(const AstNode* plhs, const AstNode* prhs, CVar& lhs_index, CVar& RHS, uint16_t& typelhs, bool& contig, bool isreplica, const CVar* cell_item = NULL);
	void mod_sig(CVar& lvar, const CVar& lhs_index, const CVar& robj, bool contig, const AstNode* plhs, const AstNode* prhs);
	void extract_by_index(CVar& out, const CVar& index, const CVar& obj, bool contig);
	void right_to_left(const AstNode* plhs, const CVar& lhs_index, CVar& robj, uint16_t typelhs, bool contig, const AstNode* prhs = NULL, CVar* lobj = NULL);
	void eval_index(const AstNode* pInd, const CVar& varLHS, CVar& index);
	void insertreplace(const AstNode* pnode, const CVar& sec, const CVar& indsig, CVar* lobj, bool isreplica);
	const CVar* get_cell_item(const AstNode* plhs, const CVar& cellobj);
	void adjust_buf(CVar& lobj, const CVar& lhs_index, const CVar& robj, bool contig, const AstNode* pn);
	void assign_struct(CVar* lobj, const AstNode* plhs, const AstNode* pstruct, const CVar& robj);
	CVar* get_available_struct_item(const AstNode* plhs, const AstNode** pstruct);
	void sanitize_cell_node(const AstNode* p);
};
