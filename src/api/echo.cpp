#include "echo.h"
#include <iomanip>      // std::setw

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static string echo(int depth, const CVar& sig, int display_precision, int display_limit_x, int display_limit_y, int display_limit_bytes)
{
	echo_object EO("", display_limit_x, display_limit_y, display_limit_bytes);
	EO.precision = display_precision;
	return EO.print(sig, 0);
}

//[ 5  3 2 -1 9 83 7 62 9 7 6 8 9 7 3 2 -1]
string show_preview(const CVar& sig, int display_precision, int display_limit_x, int display_limit_y, int display_limit_bytes)
{
	int dt = 1;
	string out = echo(dt, sig, display_precision, display_limit_x, display_limit_y, display_limit_bytes);
	return out;
}

string show_preview(const AuxScope* ctx, int display_precision, int display_limit_x, int display_limit_y, int display_limit_bytes)
{
	int dt = 1;
	string out;
	if (ctx->SigExt.empty())
		out = echo(dt, ctx->Sig, display_precision, display_limit_x, display_limit_y, display_limit_bytes);
	else {
		auto it = ctx->SigExt.begin();
		while (it != ctx->SigExt.end()) {
			out += echo(dt, it->get(), display_precision, display_limit_x, display_limit_y, display_limit_bytes);
			it++;
		};
	}
	return out;
}

string echo_object::tmarks(const CTimeSeries& sig, bool unit)
{
	// unit is to be used in the future 8/15/2018
	// Get the timepoints
	ostringstream out;
	streamsize org_precision = out.precision();
	out.setf(ios::fixed);
	out.precision(1);
	int kk(0), tint(sig.CountChains());
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain) {
		out << "(" << xp->tmark;
		if (unit) out << "ms";
		out << "~" << xp->tmark + 1000. * xp->nSamples / xp->GetFs();
		if (unit) out << "ms";
		out << ") ";
	}
	kk = 0;
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain) {
		out << " " << xp->nSamples;
		if (xp->nGroups > 1)
			out << " (" << xp->nGroups << "x" << xp->Len() << ")";
	}
	out << endl;
	out.unsetf(ios::floatfield);
	out.precision(org_precision);
	return out.str();
}

string echo_object::make(const CTimeSeries& sig, bool unit, int offset)
{
	ostringstream out;
	streamsize org_precision = out.precision();
	out.setf(ios::fixed);
	if (unit) out.precision(1);
	else out.precision(2);
	int kk(0), tint(sig.CountChains());
	for (const CTimeSeries* xp = &sig; kk < tint; kk++, xp = xp->chain)
	{
		for (int k = 0; k < offset + 1; k++) out << " ";
		out << "(" << xp->tmark;
		if (unit) out << "ms";
		out << ") ";
		out << print_vector(*xp, offset);
	}
	out.unsetf(ios::floatfield);
	out.precision(org_precision);
	return out.str();
}

string echo_struct::print(const CVar& obj, const string& head)
{
	string out = "[Structure] ...\n";
	for (map<string, CVar>::const_iterator it = obj.strut.begin(); it != obj.strut.end(); it++)
	{
		ostringstream var0;
		var0 << '.' << it->first;
		out += var0.str();
		out += echo_object::print(it->second, offset + 1);
	}
	return out;
}

string echo_cell::print(const CVar& obj, const string& head)
{
	string out;
//	echo_object::header(head);
	out += "[Cell] ...\n";
	auto j = 1;
	for (vector<CVar>::const_iterator it = obj.cell.begin(); it != obj.cell.end(); it++)
	{
		ostringstream oss;
		oss << '{' << j++ << "} ";
		oss << echo_object::print(*it, offset);
		out += oss.str();
	}
	return out;
}

string echo_object::row(const CTimeSeries& obj, unsigned int id0, int offset, int prec)
{
	ostringstream out;
	streamsize org_precision = out.precision();
	out.precision(prec);
	unsigned int k = 0;
	vector<int> idx;
	idx.reserve(obj.Len());
	if (tbht == "head")
		for (int k = 0; k < min(display_limit_x, obj.Len()); k++) idx.push_back(k);
	else if (tbht == "tail")
		for (int k = obj.Len() - display_limit_x; k < obj.Len(); k++) idx.push_back(k);
	else
		for (int k = 0; k < min(10, obj.Len()); k++) idx.push_back(k);
	if (idx.front() != 0) {
		if (ISSTRINGG(obj.type())) out << "...";
		else	out << "... ";
	}
	else if (ISSTRINGG(obj.type()))
		out << "\"";
	if (obj.IsComplex()) {
		for (; k < min(10, obj.Len()); k++, out << " ")
		{
			for (int m = 0; m < offset; m++) out << " ";
			out << obj.cbuf[k + id0];
		}
	}
	else
	{
		for (int m = 0; m < offset; m++) out << " ";
		if (obj.IsLogical())
		{
			for (auto k : idx)
				out << obj.logbuf[k + id0] << " ";
		}
		else if (ISSTRINGG(obj.type())) {
			for (auto k : idx)
				out << obj.strbuf[k + id0];
		}
		else 
			for (auto k : idx)
				out << obj.buf[k + id0] << " ";
	}
	if (idx.back() < obj.Len() - 1) {
		if (ISSTRINGG(obj.type())) out << "... ";
		else out << " ... ";
	}
	else if (ISSTRINGG(obj.type()))
		out << "\"";
	out.precision(org_precision);	
	out << postscript << "\n";
	return out.str();
}

string echo_object::print_vector(const CTimeSeries& obj, int offset)
{
	stringstream ss;
	vector<int> idx;
	idx.reserve(obj.nGroups);
	if (tbht == "top")
		for (int k = 0; k < min(display_limit_x, obj.nGroups); k++) idx.push_back(k);
	else if (tbht == "bottom")
		for (int k = obj.nGroups - display_limit_x; k < obj.nGroups; k++) idx.push_back(k);
	else
		for (int k = 0; k < min(display_limit_y, obj.nGroups); k++) idx.push_back(k);
	if (idx.front() != 0)
	{
		ss << endl;
		for (int m = 0; m < offset; m++) ss << " ";
		ss << "... ";
	}
	if (obj.nGroups == 1) {
		if (obj.IsLogical()) cout << "(bool) ";
		ss << row(obj, 0, 0, precision);
	}
	else
	{
		if (obj.IsLogical()) cout << "(bool) ";
		ss << endl;
		for (auto k : idx)
			ss << row(obj, obj.Len() * k, offset + 1, precision);
		if (idx.back() < obj.nGroups - 1)
			ss << " ... ";
	}
	return ss.str();
}

string echo_object::print_temporal(const string& title, const CVar& obj, int offset)
{
	string out;
	for (int k = 0; k < offset + 1; k++) out += " ";
	out += title + " " + tmarks(obj, true);
	if (display_limit_x > 1) {
		int k = 0;
		for (const CTimeSeries* xp = &obj; xp ; k++, xp = xp->chain) {
//			for (int m = 0; m < offset + 1; m++) out += " ";
//			out += print_vector(*xp, offset);
		}
	}
	return out;
}

static void dump_hex(std::ostream& oss, const void* data, size_t nbytes) {
	auto p = static_cast<const unsigned char*>(data);
	// Save stream state (important for library code)
	std::ios old_state(nullptr);
	old_state.copyfmt(oss);
	oss << std::hex << std::setfill('0');
	for (size_t i = 0; i < nbytes; ++i) {
		oss << std::setw(2)
			<< static_cast<unsigned int>(p[i]) << ' ';
	}
	oss << '\n';
	// Restore stream state
	oss.copyfmt(old_state);
}

string echo_object::print(const CVar& obj, int offset)
{
	string head = "(no head)";
	ostringstream oss;
	auto tp = obj.type();
	for (int k = 0; k < offset; k++) oss << " ";
	oss << "type = " << "0x" << setw(4) << setfill('0') << hex << tp << ", " << dec;
	if (tp & TYPEBIT_STRUT)
		oss << echo_struct(head, offset).print(obj, head);
	else if (tp & TYPEBIT_CELL)
		oss << echo_cell(head, offset).print(obj, head);
	else if (ISAUDIO(tp))
	{
//		header(name=head);
		oss << endl;
		if (obj.IsStereo())
		{
			oss << print_temporal("audio(L)", obj, offset);
			oss << print_temporal("audio(R)", *(obj.next), offset);
		}
		else
		{
			oss << print_temporal("audio", obj, offset);
		}
	}
	else if (ISTEMPORAL(tp) || ISSTEREO(tp))
	{
//		header(head);
		oss << endl;
		if (obj.next) oss << "[L] " << endl;
		oss << make(obj, obj.GetFs() > 0, offset);
		if (obj.next)
		{
			oss << "[R] " << endl;
			oss << make(*(obj.next), obj.next->GetFs() > 0, offset);
		}
	}
	else if (!tp) // Don't do (type & TYPEBIT_NULL) unless you want to be funny!
	{
		header(head);
		oss << "[]" << postscript << endl;
	}
	else if (ISSTRING(tp) || ISBYTE(tp))
	{
//		header(head);
		//if (display_limit_x < 0) {
		ostringstream oss2;
		if (obj.bufType == 'S')
			oss << "\"" << obj.str() << "\"" << postscript << endl;
		else if (obj.bufType == 'B') {
			dump_hex(oss2, obj.buf, min(display_limit_bytes, obj.nSamples));
			postscript = oss2.str();
			oss << "(" << obj.nSamples << " bytes) " << postscript << endl;
		}
		//}
		//else 
		//	oss << print_vector(obj, offset);
	}
	else if (ISSCALAR(tp) || ISVECTOR(tp) || IS2D(tp))
	{
		if (obj.nSamples > 1) {
			oss << "size=";
			if (IS2D(tp)) oss << obj.nGroups << "X";
			oss << obj.Len() << endl;
		}
//		header(head);
		oss << print_vector(obj, offset);
	}
	return oss.str();
}