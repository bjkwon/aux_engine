// AUXLAB 
//
// Copyright (c) 2009-2019 Bomjun Kwon (bjkwon at gmail)
// Licensed under the Academic Free License version 3.0
//
// Project: sigproc
// Signal Generation and Processing Library
// Platform-independent (hopefully) 
// 
// Version: 1.7
// Date: 5/24/2020 

// Time sequence with nSamples > 1 must set snap 1
// so the data doesn't go with time but it is about at that particular tmark
// 5/25/2020 

#include "AuxScope.h"
#include <cstring> // for g++

#include <math.h>
#include <limits> 
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <complex>
#include <math.h> // ceil

#define CRIT  100. // Threshold for computing rms is above 100th of its max

#define RETURN_0_MSG(str) {	strcpy(errstr, str);		return 0;	}

void filter(int nTabs, double *num, double *den, int length, double *in, double *out);
void filter(int nTabs, double *num, int length, double *in, double *out);

static double quantizetmark(double delta, int fs)
{
	//quantize delta based on the fs grid.  3/31/2016. Rev. 5/19/2018
	if (fs > 1)
	{
		int pt = (int)round(delta / 1000 * fs);
		return 1000.f * pt / fs;
	}
	return delta;
}

/*bufBlockSize is 1 (for logical, string), sizeof(double) (regular), or sizeof(double)*2 (complex).     4/4/2017 bjkwon */
body::body()
	: bufType(0), nSamples(0), bufBlockSize(sizeof(auxtype)), buf(NULL), nGroups(1), ghost(false)
{
}

body::body(auxtype value)
	: bufType('R'), nSamples(0), buf(NULL), ghost(false)
{
	SetValue(value);
}

body::body(complex<auxtype> value)
	: bufType('C'), nSamples(0), buf(NULL), ghost(false)
{
	cbuf[0] = value;
	bufBlockSize = 2 * sizeof(auxtype);
}

body::body(const body& src)
{
	*this = src;
}

body::body(auxtype *y, int len)
	: nSamples(len), bufBlockSize(sizeof(auxtype)), buf(new auxtype[len]), nGroups(1), ghost(false)
{
	memcpy(buf, y, bufBlockSize*len);
}

body::body(bool *y, int len)
	: nSamples(len), bufBlockSize(1), logbuf(new bool[len]), nGroups(1), ghost(false)
{
	memcpy(buf, y, bufBlockSize*len);
}

body::body(const vector<auxtype> & src)
	: nSamples((unsigned int)src.size()), bufBlockSize(sizeof(auxtype)), buf(new auxtype[src.size()]), nGroups(1), ghost(false)
{
	memcpy(buf, src.data(), sizeof(auxtype)*nSamples);
}

body& body::operator=(const vector<auxtype> & rhs)
{
	nSamples = (unsigned int)rhs.size();
	bufBlockSize = 8;
	nGroups = 1;
	ghost = false;
	buf = new auxtype[nSamples];
	return *this;
}

body& body::operator=(const body & rhs)
{
	if (this != &rhs)
	{
		unsigned int currentBufSize = bufBlockSize * nSamples;
		unsigned int reqBufSize = rhs.bufBlockSize * rhs.nSamples;
		nSamples = rhs.nSamples;
		bufBlockSize = rhs.bufBlockSize;
		nGroups = rhs.nGroups;
		if (reqBufSize > currentBufSize)
		{
			if (!ghost && buf)
				delete[] buf;
			// If both LHS and RHS were already ghost, keep both ghosts
			// otherwise, ghost on RHS cannot be transferred to ghost on the LHS. 1/6/2022
			if (!ghost || !rhs.ghost)
				logbuf = new bool[reqBufSize];
		}
		if (ghost && rhs.ghost)
			buf = rhs.buf;// shallow (ghost) copy
		else
			memcpy(buf, rhs.buf, nSamples * bufBlockSize);
		bufType = rhs.bufType;
	}
	return *this;
}

// DO NOT CALL this function with empty buf
auxtype body::_max(unsigned int id, int unsigned len, void* pind) const
{
	if (len == 0) len = nSamples;
	auto it = max_element(buf + id, buf + id + len);
	if (pind)
		*(int*)pind = it - buf;
	return *it;
}

auxtype body::_min(unsigned int id, unsigned int len, void* pind) const
{
	if (len == 0) len = nSamples;
	auto it = min_element(buf + id, buf + id + len);
	if (pind)
		*(int*)pind = it - buf;
	return *it;
}

body& body::MakeLogical()
{
	if (bufBlockSize == 1) {
		bufType = 'L';
		return *this;
	}
	body out;
	out.bufBlockSize = 1; // logical array
	out.UpdateBuffer(nSamples); // This over-reserve the memory, but there's no harm. 4/4/2017 bjk
	for (unsigned int k = 0; k < nSamples; k++)
	{
		if (buf[k] == 0.) out.logbuf[k] = false;
		else			out.logbuf[k] = true;
	}
	out.nGroups = nGroups;
	out.bufType = 'L';
	return (*this = out);
}

CSignal& CSignal::operator=(const body& rhs)
{
	Reset();
	body::operator=(rhs);
	return *this;
}

CSignal& CSignal::operator=(const CSignal& rhs)
{   // Make a deep copy only for buf, but not for sc, because sc is not necessarily dynamically allocated.
	// Thus, BE Extra careful when making writing an assignment statement about the scaling..
	if (this != &rhs)
	{
		Reset(rhs.fs);
		if (rhs.fs > 0) // Don't change to the line below... it affects just about everything 12/13/2020
		// if (rhs.type() & TYPEBIT_TSEQ) 
			tmark = quantizetmark(rhs.tmark, rhs.fs);
		else // this way, quantize is skipped for t-seq with relative tmarks.
		{
			tmark = rhs.tmark;
			SetFs(0);
		}
		snap = rhs.snap;
		body::operator=(rhs);
	}
	return *this;
}

CVar& CVar::operator=(const CSignals& rhs)
{
	Reset();
	CSignals::operator=(rhs);
	return *this;
}

CVar& CVar::operator=(const CVar& rhs)
{   // Make a deep copy only for buf, but not for sc, because sc is not necessarily dynamically allocated.
	// Thus, BE Extra careful when making writing an assignment statement about the scaling..
	if (this != &rhs)
	{
		CSignals::operator=(rhs);
		bufType = rhs.bufType;
		cell = rhs.cell;
		strut = rhs.strut;
		struts = rhs.struts;
	}
	return *this;
}

CVar & CVar::operator=(CVar * rhs)
{
	return *this = *rhs;
}

bool CVar::operator==(std::string rhstr)
{
	return *this == CSignals(rhstr);
}
bool CVar::operator==(auxtype val)
{
	return *this == CSignals(val);
}

bool CSignals::operator==(auxtype rhs)
{
	return *this == CSignals(rhs);
}

bool CSignals::operator==(std::string rhstr)
{
	return *this == CSignals(rhstr);
}

CTimeSeries& CTimeSeries::operator=(const CSignal& rhs)
{
	Reset();
	if (ghost)
		CSignal::operator=(rhs);
	else
		CSignal::operator=(rhs);
	return *this;
}

bool CSignal::operator==(auxtype rhs)
{
	return *this == CSignal(rhs);
}

bool CSignal::operator==(std::string rhstr)
{
	return *this == CSignal(rhstr);
}

CTimeSeries& CTimeSeries::operator=(const CTimeSeries& rhs)
{
	if (this != &rhs)
	{
		CSignal::operator=(rhs);
//		outarg = rhs.outarg;
		if (rhs.chain)
		{
			chain = new CTimeSeries;
			*chain = *rhs.chain;
		}
		else
		{
			if (!ghost) delete chain;
			chain = NULL;
		}
	}
	return *this;
}

CSignals& CSignals::operator=(const CTimeSeries& rhs)
{
	Reset();
	CTimeSeries::operator=(rhs);
	return *this;
}

CSignals& CSignals::operator=(const CSignals& rhs)
{   // Make a deep copy only for buf, but not for sc, because sc is not necessarily dynamically allocated.
	// Thus, BE Extra careful when making writing an assignment statement about the scaling..
	if (this != &rhs)
	{
		CTimeSeries::operator=(rhs);
		if (rhs.next) 
			SetNextChan(*rhs.next);
		else
		{
			delete next;
			next = NULL;
		}
	}
	return *this;
}

body& body::operator+=(auxtype con)
{
	unsigned int blockSize = bufBlockSize / sizeof(auxtype);
	for (unsigned int k = 0; k < nSamples; k++) buf[k*blockSize] += con;
	return *this;
}

body& body::operator*=(auxtype con)
{
	unsigned int blockSize = bufBlockSize / sizeof(auxtype);
	switch (blockSize)
	{
	case 1:
		for (unsigned int k = 0; k < nSamples; k++) buf[k] *= con;
		break;
	case 2:
		for (unsigned int k = 0; k < nSamples; k++) cbuf[k] *= con;
		break;
	}
	return *this;
}

body& body::operator/=(auxtype con)
{
	return *this *= 1. / con;
}

body::~body()
{
	if (buf) delete[] buf;
	nSamples = 0;
}

void body::SetValue(auxtype v)
{ // why did I change it this way?
 // this way I can just replace a value with the existing value 
 // (i.e., if it was already CSIG_SCALR, just reuse the buffer) 8/15/2018
	if (bufBlockSize != sizeof(auxtype) || nSamples != 1)
	{ 
		bufBlockSize = sizeof(auxtype);
		if (buf) delete[] buf;
		buf = new auxtype[1];
		nSamples = 1;
	}
	nGroups = 1;
	buf[0] = v;
	bufType = 'R';
}

void body::SetValue(complex<auxtype> v)
{
	if (bufBlockSize != 2 * sizeof(auxtype) || nSamples != 1)
	{
		bufBlockSize = 2 * sizeof(auxtype);
		if (buf) delete[] buf;
		buf = new auxtype[2];
		nSamples = 1;
	}
	nGroups = 1;
	cbuf[0] = v;
	bufType = 'C';
}

body& body::UpdateBuffer(uint64_t length, uint64_t offset)	// Set nSamples. Re-allocate buf if necessary to accommodate new length.
{
	if (!ghost)
	{
		unsigned int currentBufsize = bufBlockSize * nSamples;
		unsigned int reqBufSize = bufBlockSize * length;
		if (length < 0 || currentBufsize == reqBufSize)
			return *this;
		if (length > nSamples) {
			bool *newlogbuf = new bool[reqBufSize];
			if (nSamples > 0)
				memcpy(newlogbuf + offset * bufBlockSize, buf, nSamples*bufBlockSize);
			delete[] buf;
			logbuf = newlogbuf;
		}
		//initializing with zeros for the rest
		if (length > nSamples)
			memset(logbuf + (nSamples + offset) * bufBlockSize, 0, (length - nSamples - offset)*bufBlockSize);
		memset(logbuf, 0, offset * bufBlockSize);
	}
	//For ghost, the rationale for this call is unclear; but just update nSamples with length
	nSamples = length;
	return *this;
}

void body::Reset()
{
	if (!ghost && buf)
		delete[] buf;
	buf = NULL;
	bufType = ' ';
	nSamples = 0;
	nGroups = 1;
	ghost = false;
	bufBlockSize = sizeof(auxtype);
}

string body::valuestr(int digits) const
{ //this doesn't throw. Used for showvar.cpp (avoiding unhandled exceptions)
	ostringstream out;
	out.unsetf(ios::floatfield);
	out.precision(digits);
	if (nSamples >= 1)
	{
		if (bufBlockSize == sizeof(auxtype))	out << buf[0];
		else if (bufBlockSize == 1)	out << (auxtype)logbuf[0];
		else out << "complex";
	}
	else if (nSamples == 0)
		out << "(emptybuffer)";
	if (nSamples > 1)
		out << "(more)";
	return out.str();
}

auxtype body::value() const
{
	if (nSamples == 1)
	{
		if (bufBlockSize == sizeof(auxtype))	return buf[0];
		else if (bufBlockSize == 1)	return (auxtype)logbuf[0];
		else throw "value( ) on complex value. Use cvalue instead.";
	}
	else if (nSamples == 0)
		throw "value( ) on null.";
	else
		throw "value( ) on vector/array.";
}

complex<auxtype> body::cvalue() const
{
	if (nSamples == 1)
		return cbuf[0];
	else if (nSamples == 0)
		throw "value( ) on null.";
	else
		throw "value( ) on vector/array.";
}

//Equivalent to real part
void body::SetReal()
{ // This converts a complex array into real (decimating the imaginary part).
  // Must not be used when the buf is properly prepared--in that case just bufBlockSize = 1; is sufficient.
	unsigned int k = 0;
	bufType = 'R';
	if (IsComplex())
	{
		bufBlockSize = sizeof(auxtype);
		for (; k < nSamples; k++) buf[k] = buf[2 * k];
	}
	//if logical or string, it will turn to regular 8 
	else if (bufBlockSize == 1)
	{
		body out;
		out.UpdateBuffer(nSamples);
		for (auto &v : *this) out.buf[k++] = (auxtype)v;
		*this = out;
	}
}
void body::SetByte()
{
	bufType = 'B';
}
void body::SetComplex()
{
	bufType = 'C';
	if (bufBlockSize != sizeof(auxtype) * 2)
	{
		bufBlockSize = sizeof(auxtype) * 2;
		if (nSamples > 0)
		{
			auxtype*newbuf = new auxtype[2 * nSamples];
			memset(newbuf, 0, sizeof(auxtype) * 2 * nSamples);
			for (unsigned int k = 0; k < nSamples; k++) newbuf[2 * k] = buf[k];
			delete[] buf;
			buf = newbuf;
		}
	}
	else if (bufBlockSize == 1)
	{
		body out;
		out.bufBlockSize = sizeof(auxtype) * 2;
		out.UpdateBuffer(nSamples);
		for (unsigned int k = 0; k < nSamples; k++) out.buf[2 * k] = buf[k];
		*this = out;
	}
}

void body::SwapContents1node(body &sec)
{	// swaps fs, buf, nSamples & bufBlockSize (rev. 5/19/2018)
	body tmp;
	tmp.buf = buf, tmp.nSamples = nSamples, tmp.bufBlockSize = bufBlockSize, tmp.nGroups = nGroups;
	buf = sec.buf, nSamples = sec.nSamples, bufBlockSize = sec.bufBlockSize, nGroups = sec.nGroups;
	sec.buf = tmp.buf, sec.nSamples = tmp.nSamples, sec.bufBlockSize = tmp.bufBlockSize, sec.nGroups = tmp.nGroups;
	// Mark buf NULL so that destructor won't try to destroy buf which is used by sec
	tmp.buf = NULL;
}

body &body::addmult(char op, body &sec, unsigned int id0, uint64_t len)
{
	if (nSamples / nGroups < sec.nSamples / sec.nGroups) SwapContents1node(sec);
	auto lenL = nSamples / nGroups;
	auto lenR = sec.nSamples / sec.nGroups;
	auto vertID = id0 / lenL;
	auto minlen = min(lenL, lenR);
	//Assume that is this is Complex, sec is also Complex
	if (lenR == 1)
	{
		if (IsComplex())
		{
			auto rhs = sec.nSamples == 1 ? sec.cbuf[0] : sec.cbuf[vertID];
			auto len2 = len == 0 ? lenL : len;
			if (op == '+')
				for (auto k = id0; k < id0 + len2; k++)		cbuf[k] += rhs;
			else
				for (auto k = id0; k < id0 + len2; k++)		cbuf[k] *= rhs;
			len = len2;
		}
		else
		{
			auto rhs = sec.nSamples == 1 ? sec.buf[0] : sec.buf[vertID];
			auto len2 = len == 0 ? lenL : len;
			if (op == '+')
				for (auto k = id0; k < id0 + len2; k++)		buf[k] += rhs;
			else
				for (auto k = id0; k < id0 + len2; k++)		buf[k] *= rhs;
		}
	}
	else
	{
		if (len == 0)	len = minlen;
		else			len = min(len, minlen);
		if (IsComplex())
		{
			if (op == '+')
				for (auto k = id0; k < id0 + len; k++)		cbuf[k] += sec.cbuf[vertID*lenR + k - id0];
			else
				for (auto k = id0; k < id0 + len; k++)		cbuf[k] *= sec.cbuf[vertID*lenR + k - id0];
		}
		else
		{
			if (op == '+')
				for (auto k = id0; k < id0 + len; k++)		buf[k] += sec.buf[vertID*lenR + k - id0];
			else
				for (auto k = id0; k < id0 + len; k++)		buf[k] *= sec.buf[vertID*lenR + k - id0];
		}
	}
	if (IsComplex())
	{
		for (auto k = (unsigned)0; k < len; k++)
			if (imag(cbuf[k]) != 0.) return *this;
		//if it survives this far, it should be set real
		SetReal();
	}
	return *this;
}


body &body::each(auxtype (*fn)(auxtype))
{
	if (bufBlockSize == 1)
		for (unsigned int k = 0; k < nSamples; ++k)	logbuf[k] = fn(logbuf[k]) != 0;
	else
		for (unsigned int k = 0; k < nSamples; ++k)	buf[k] = fn(buf[k]);
	return *this;
}

body& body::each_sym(auxtype (*fn)(auxtype))
{
	for (unsigned int k = 0; k < nSamples; ++k)	
		if (buf[k] >= 0) buf[k] = fn(buf[k]);
		else buf[k] = -fn(-buf[k]);
	return *this;
}

body &body::each(auxtype (*fn)(complex<auxtype>))
{
	auxtype *out = new auxtype [nSamples];
	for (unsigned int k = 0; k < nSamples; ++k)	out[k] = fn(cbuf[k]);
	delete[] buf;
	buf = out;
	bufBlockSize = sizeof(auxtype);
	return *this;
}

body &body::each(complex<auxtype>(*fn)(auxtype))
{
	for (unsigned int k = 0; k < nSamples; ++k)	cbuf[k] = fn(buf[k]);
	return *this;
}

body &body::each(complex<auxtype>(*fn)(complex<auxtype>))
{
	for (unsigned int k = 0; k < nSamples; ++k)	cbuf[k] = fn(cbuf[k]);
	return *this;
}

body &body::each(auxtype (*fn)(auxtype, auxtype), const body &arg)
{
	if (arg.nSamples == 1)
	{
		auxtype val = arg.value();
		if (bufBlockSize == 1 && arg.bufBlockSize == 1)
			for (unsigned int k = 0; k < nSamples; k++)	logbuf[k] = fn(logbuf[k], arg.logbuf[0]) != 0;
		else if (bufBlockSize == 1 && arg.bufBlockSize != 1)
			for (unsigned int k = 0; k < nSamples; k++)	logbuf[k] = fn(logbuf[k], val) != 0;
		else
			for (unsigned int k = 0; k < nSamples; k++)	buf[k] = fn(buf[k], val);
	}
	else if (nSamples == 1)
	{
		auxtype baseval = buf[0];
		UpdateBuffer(arg.nSamples);
		if (bufBlockSize == 1 && arg.bufBlockSize == 1)
			for (unsigned int k = 0; k < arg.nSamples; k++) logbuf[k] = fn(baseval, arg.logbuf[k]) != 0;
		else if (bufBlockSize == 1 && arg.bufBlockSize != 1)
			for (unsigned int k = 0; k < arg.nSamples; k++) logbuf[k] = fn(baseval, arg.buf[k]) != 0;
		else
			for (unsigned int k = 0; k < arg.nSamples; k++) buf[k] = fn(baseval, arg.buf[k]);
	}
	else
	{
		nSamples = min(nSamples, arg.nSamples);
		if (bufBlockSize == 1 && arg.bufBlockSize == 1)
			for (unsigned int k = 0; k < nSamples; k++) logbuf[k] = fn(logbuf[k], arg.logbuf[k]) != 0;
		else if (bufBlockSize == 1 && arg.bufBlockSize != 1)
			for (unsigned int k = 0; k < nSamples; k++) logbuf[k] = fn(logbuf[k], arg.buf[k]) != 0;
		else
			for (unsigned int k = 0; k < nSamples; k++) buf[k] = fn(buf[k], arg.buf[k]);
	}
	return *this;
}

body& body::each_sym2(auxtype (*fn)(auxtype, auxtype), const body& arg)
{
	if (arg.nSamples == 1)
	{
		auxtype val = arg.value();
		for (unsigned int k = 0; k < nSamples; k++)	
			if (buf[k] >= 0) buf[k] = fn(buf[k], val);
			else buf[k] = -fn(-buf[k], val);
	}
	else if (nSamples == 1)
	{
		auxtype baseval = buf[0];
		UpdateBuffer(arg.nSamples);
		for (unsigned int k = 0; k < arg.nSamples; k++)
			if (buf[k] >= 0) buf[k] = fn(baseval, arg.buf[k]);
			else buf[k] = -fn(-baseval, arg.buf[k]);
	}
	else
	{
		nSamples = min(nSamples, arg.nSamples);
		for (unsigned int k = 0; k < nSamples; k++) 
			if (buf[k] >= 0) buf[k] = fn(buf[k], arg.buf[k]);
			else buf[k] = -fn(-buf[k], arg.buf[k]);
	}
	return *this;
}



body &body::each(complex<auxtype >(*fn)(complex<auxtype >, complex<auxtype >), const body &arg)
{
	
	if (arg.nSamples == 1)
	{
		complex<auxtype > val = arg.IsComplex() ? arg.cbuf[0] : arg.buf[0];
		for (unsigned int k = 0; k < nSamples; k++)	cbuf[k] = fn(cbuf[k], val);
	}
	else if (nSamples == 1)
	{
		complex<auxtype > val = arg.IsComplex() ? cbuf[0] : buf[0];
		UpdateBuffer(arg.nSamples);
		for (unsigned int k = 0; k < arg.nSamples; k++) cbuf[k] = fn(val, arg.cbuf[k]);
	}
	else
	{
		nSamples = min(nSamples, arg.nSamples);
		for (unsigned int k = 0; k < nSamples; k++) cbuf[k] = fn(cbuf[k], arg.cbuf[k]);
	}
	return *this;
}

auxtype lt(auxtype a, auxtype b)
{
	return a < b;
}
static auxtype le(auxtype a, auxtype b)
{
	return a <= b;
}
static auxtype gt(auxtype a, auxtype b)
{
	return a > b;
}
static auxtype ge(auxtype a, auxtype b)
{
	return a >= b;
}
static auxtype eq(auxtype a, auxtype b)
{
	return a == b;
}
static auxtype ne(auxtype a, auxtype b)
{
	return a != b;
}
static auxtype and_(auxtype a, auxtype b)
{
	return a && b;
}
static auxtype or_(auxtype a, auxtype b)
{
	return a || b;
}
static auxtype not_(auxtype a)
{
	if (a == 0) return 1.;
	else  return 0.;
}


body& body::LogOp(body &rhs, int type)
{
	auxtype(*fn)(auxtype, auxtype);
	switch (type)
	{
	case '<':
		fn = lt;
		break;
	case '>':
		fn = gt;
		break;
	case T_LOGIC_LE:
		fn = le;
		break;
	case T_LOGIC_GE:
		fn = ge;
		break;
	case T_LOGIC_EQ:
		fn = eq;
		break;
	case T_LOGIC_NE:
		fn = ne;
		break;
	case T_LOGIC_AND:
		fn = and_;
		break;
	case T_LOGIC_OR:
		fn = or_ ;
		break;
	case T_LOGIC_NOT:
		each(not_);
		return *this;
	default:
		return *this;
	}
	each(fn, rhs);
	MakeLogical();
	return *this;
}


/* If consecutive, RHS with different length can be applied to LHS, updating this length.
If not consecutive, RHS must have the same length as LHS.
11/28/2020
*/

body& body::replacebyindex(vector<unsigned int>::iterator idBegin, vector<unsigned int>::iterator idEnd, const body & RHS)
{ // non-consecutive
	// ASSUME: 1) ind.size -- RHS.length; or RHS is scalar
	// 2) all values in ind must be in the range of this buf
	if (RHS.nSamples == 1)
	{
		if (bufBlockSize == 1)
			for (auto it = idBegin; it!=idEnd; it++) logbuf[*it] = RHS.logbuf[0];
		else if (bufBlockSize == 8)
			for (auto it = idBegin; it!=idEnd; it++) buf[*it] = RHS.buf[0];
		else // if (bufBlockSize == 16)
			for (auto it = idBegin; it!=idEnd; it++) cbuf[*it] = RHS.cbuf[0];
	}
	else
	{
		unsigned int k = 0;
		if (bufBlockSize == 1)
			for (auto it = idBegin; it!=idEnd; it++) logbuf[*it] = RHS.logbuf[k++];
		else if (bufBlockSize == 8)
			for (auto it = idBegin; it!=idEnd; it++) buf[*it] = RHS.buf[k++];
		else // if (bufBlockSize == 16)
			for (auto it = idBegin; it!=idEnd; it++) cbuf[*it] = RHS.cbuf[k++];
	}
	return *this;
}

body& body::replacebyindex(unsigned int id0, unsigned int len, const body & RHS)
{ // consecutive
	// ASSUME: RHS.bufBlockSize is the same as this bufBlockSize
// if RHS.length  > len --> update buffer
// else just update buffer content and update the length of this
	if (RHS.nSamples > len || id0 == nSamples)
	{
		bool* newbuf = new bool[(nSamples + RHS.nSamples - len) * bufBlockSize];
		memcpy(newbuf, logbuf, id0* bufBlockSize);
		memcpy(newbuf + id0 * bufBlockSize, RHS.buf, RHS.nSamples * bufBlockSize);
		memcpy(newbuf + (id0 + RHS.nSamples) * bufBlockSize, logbuf + (id0 + len) * bufBlockSize, (nSamples - id0 - len) * bufBlockSize);
		nSamples += RHS.nSamples - len;
		delete[] buf;
		logbuf = newbuf;
	}
	else
	{
		memmove(logbuf + id0 * bufBlockSize, RHS.buf, RHS.nSamples * bufBlockSize);
		if (RHS.nSamples != len)
			memmove(logbuf + (id0 + RHS.nSamples) * bufBlockSize, logbuf + (id0 + len) * bufBlockSize, (nSamples - id0 - len) * bufBlockSize);
		nSamples += RHS.nSamples - len;
	}
	return *this;
} 

body &body::insert(body &sec, int id)
{
	if (sec.nSamples == 0) return *this;
	if (id < 0) throw "insert index cannot be negative.";
	if (sec.bufBlockSize != bufBlockSize) throw "insert must be between the same data structure.";
	int nToMove = nSamples - id;
	UpdateBuffer(nSamples + sec.nSamples);
	bool *temp = new bool[nToMove*bufBlockSize];
	memcpy(temp, logbuf + id * bufBlockSize, nToMove*bufBlockSize);
	memcpy(logbuf + id * bufBlockSize, sec.buf, sec.nSamples*bufBlockSize);
	memcpy(logbuf + (id + sec.nSamples)*bufBlockSize, temp, nToMove*bufBlockSize);
	delete[] temp;
	return *this;
}

body& body::transpose() {
	int   rows = nGroups;                 // # of rows
	int   cols = nSamples / nGroups;      // # of cols
	int   N = nSamples;
	vector<char> done(N, 0);
	int nextID = 1;

	auto index_to_rc = [&](int idx, int& r, int& c) {
		r = idx / cols;
		c = idx % cols;
	};
	auto rc_to_index = [&](int r, int c) {
		return r * rows + c;
	};
	auto next_unmarked = [&]() -> int {
		for (int k = nextID; k < N - 1; ++k) {
			if (!done[k]) return k;
		}
		return -1;  // nothing left
	};

	while (1) {
		int startID = next_unmarked();
		if (startID < 0) break;          // all non-diagonal entries done

		// start a new cycle
		double cache = buf[startID];
		int curr = startID;
		done[curr] = 1; // this is not "done" in a strict sense until the cycle is closed, without this marking, it gets tricky. Safe to mark it done, because it comes back to this for sure,

		// follow the cycle
		while (1) {
			int r, c;
			index_to_rc(curr, r, c);
			nextID = rc_to_index(c, r);
			if (nextID == startID) {
				// close the cycle
				buf[startID] = cache;
				break;
			}
			std::swap(cache, buf[nextID]);
			curr = nextID;
			done[curr] = 1; // this is "done" in a strict sense.
		}
	}

	// update for next operations
	nGroups = cols;
	return *this;
}

CSignal::CSignal()
	:fs(1), tmark(0.), snap(0)
{
}

CTimeSeries::CTimeSeries()
	: chain(NULL)
{
}

CSignal::CSignal(int sampleRate)
	: fs(max(sampleRate, 0)), tmark(0.), snap(0)
{
	if (fs == 2) bufBlockSize = 1;
}

CSignal::CSignal(int sampleRate, unsigned int len)
	: fs(max(sampleRate, 0)), tmark(0.), snap(0)
{
	if (fs == 2) bufBlockSize = 1;
	UpdateBuffer(len);
}

CTimeSeries::CTimeSeries(int sampleRate)
	: chain(NULL)
{
	fs = max(sampleRate, 0);
	tmark = 0.;
	if (fs == 2) bufBlockSize = 1;
}

CTimeSeries::CTimeSeries(int sampleRate, unsigned int len)
	: chain(NULL)
{
	fs = max(sampleRate, 0);
	tmark = 0.;
	if (fs == 2) bufBlockSize = 1;
	UpdateBuffer(len);
}


CSignal::CSignal(auxtype value)
	:fs(1), tmark(0.), snap(0)
{
	SetValue(value);
}

CTimeSeries::CTimeSeries(auxtype value)
	: chain(NULL)
{
	fs = 1;
	tmark = 0;
	SetValue(value);
}

CSignal::CSignal(vector<auxtype> vv)
{
	fs = 1;
	tmark = 0;
	snap = 0;
	UpdateBuffer((unsigned int)vv.size());
	for (unsigned int k = 0; k < (unsigned int)vv.size(); k++)
		buf[k] = vv[k];
}

CSignal::CSignal(std::string str)
	:snap(0)
{
	SetString(str.c_str());
}

CSignal::CSignal(const body& src)
{
	*this = src;
}

CSignal::CSignal(const CSignal& src)
{
	*this = src;
}

CTimeSeries::CTimeSeries(const CSignal& src)
	: chain(NULL) //REQUIRED!!!!
{
	*this = src;
}

CTimeSeries::CTimeSeries(const CTimeSeries& src)
	: chain(NULL) //REQUIRED!!!!  otherwise, copied object return has uninitialized chain and causes a crash later. 5/24/2048
{
	*this = src;
}

CSignal::CSignal(auxtype *y, int len)
	: fs(1), tmark(0.), snap(0)
{
	UpdateBuffer(len);
	memcpy((void*)buf, (void*)y, sizeof(auxtype)*len);
}

vector<auxtype> body::ToVector() const
{
	vector<auxtype> out;
	out.resize((size_t)nSamples);
	int k = 0;
	for (vector<auxtype>::iterator it = out.begin(); it != out.end(); it++)
		*it = buf[k++];
	return out;
}

CSignal::~CSignal()
{
	if (buf && !ghost) 
		delete[] buf;
	buf = nullptr;
	nSamples = 0;
}

CTimeSeries::~CTimeSeries()
{
	if (chain) {
//		if (!ghost) 
			delete chain;
		chain = NULL;
	}
}

void CSignals::SetFs(int newfs)
{
	CSignal::SetFs(newfs);
	if (next)
		next->CSignal::SetFs(newfs);
}

void CSignal::SetFs(int newfs)
{  // the old data (the content of buf) should be retained; don't call Reset() carelessly.
	if (bufBlockSize == 1 && newfs != fs)
	{// Trying to convert a data type with a byte size to auxtype size
		auxtype *newbuf = new auxtype[nSamples];
		memset(newbuf, 0, sizeof(auxtype)*nSamples);
		if (IsLogical())
			for (unsigned int k = 0; k < nSamples; k++)
				if (logbuf[k]) newbuf[k] = 1.;
		delete buf;
		buf = newbuf;
		bufBlockSize = sizeof(auxtype);
	}
	fs = newfs; 
}


CSignal& CSignal::Reset(int fs2set)	// Empty all data fields - sets nSamples to 0.
{
	body::Reset();
	if (fs2set)	// if fs2set == 0 (default), keep the current fs.
		fs = max(fs2set, 1);	// not to allow minus.
	tmark = 0;
	snap = 0;
	return *this;
}

CTimeSeries& CTimeSeries::Reset(int fs2set)	// Empty all data fields - sets nSamples to 0.
{
	CSignal::Reset(fs2set);
	if (chain) {
		if (chain->ghost)
		{
			while (chain)
			{
				//clean from the last chain
				CTimeSeries * p = this, *pOneB4Last = NULL;
				for (; p->chain; p = p->chain)
				{
					if (p->chain && !p->chain->chain)
						pOneB4Last = p;
				}
				delete p;
				if (pOneB4Last)
					pOneB4Last->chain = NULL;
			}
		}
		else
		{
			delete chain;
			chain = NULL;
		}
	}
	return *this;
}


void CTimeSeries::SwapContents1node(CTimeSeries &sec)
{	// Swaps chain & tmark.	Leaves "next" intact!!!
	CTimeSeries tmp(fs);
	// tmp = *this
	tmp.body::SwapContents1node(*this);
	tmp.chain = chain, tmp.tmark = tmark;
	// *this = sec
	body::SwapContents1node(sec);
	chain = sec.chain, tmark = sec.tmark; fs = sec.fs;
	// sec = tmp
	sec.body::SwapContents1node(tmp);
	sec.chain = tmp.chain, sec.tmark = tmp.tmark; sec.fs = tmp.fs;
	// Mark chain NULL so that destructor won't try to destroy chain which is used by sec
	tmp.chain = NULL;
}


CSignal& CSignal::operator+=(CSignal *yy)
{ // Do you still need this? 11/14/2021
	if (yy->IsComplex()) SetComplex();
	if (IsComplex()) yy->SetComplex();
	unsigned int nSamples0(nSamples);
	UpdateBuffer(yy->nSamples + nSamples);
	if (IsString())
		strcat(strbuf, yy->strbuf);
	else
		memcpy(&logbuf[nSamples0*bufBlockSize], yy->buf, bufBlockSize*yy->nSamples);
	return *this;
}

CTimeSeries& CTimeSeries::operator+=(CTimeSeries *yy)
{
	if (yy == NULL) return *this;
	if (IsEmpty()) return (*this = *yy);
	if (type() == TYPEBIT_NULL) return *this = *yy >>= tmark;
	// Concatenation of CTimeSeries
    // yy might lose its value.
	CTimeSeries *ptail = GetDeepestChain();
	if ((ISAUDIO(type()) && ISAUDIO(yy->type())) && (yy->chain || yy->tmark)) {
		// when insertee has chain(s), chain it instead of copying. yy loses its value.
		CTimeSeries *pNew = new CTimeSeries(fs);
		if (chain && !yy->chain) // I don't know what would be the consequences of this leaving out  5/12/2016 ---check later??
			*pNew = *yy;	// make a copy, leaving the original intact for the next channel
		else
			pNew->SwapContents1node(*yy);		// pNew takes the insertee. As yy lost all its pointers, yy can be destroyed later without destroying any data it used to have.
		*pNew >>= ptail->tmark + ptail->_dur();	// to make the insertee the next chain(s) of ptail.
		ptail->chain = pNew;					// link it to *this.
	}
	else {
		// otherwise, just copy at the end.
		if (yy->nSamples > 0)
		{
			// If yy->nSamples==1 && fs > 3, i.e., TSEQ, reset it so that it is a regular vector
			if (yy->nSamples == 1) ptail->SetFs(1);
			int nSamples0 = ptail->nSamples;
			if (yy->IsComplex()) ptail->SetComplex();
			if (IsComplex()) yy->SetComplex();
			ptail->UpdateBuffer(yy->nSamples + nSamples0);
			if (ptail->IsString())
				strcat(ptail->strbuf, yy->strbuf);
			else
				memcpy(&ptail->logbuf[nSamples0*bufBlockSize], yy->buf, bufBlockSize*yy->nSamples);
		}
		if (!IsAudio() && IsComplex())
		{
			for (unsigned int k = 0; k < nSamples; k++)
				if (imag(cbuf[k]) != 0.) return *this;
			//if it survives this far, it should be set real
			SetReal();
		}
	}
	return *this;
}

CSignal CTimeSeries::TSeries2CSignal()
{
	//if (GetType() != CSIG_TSERIES)
	//	throw "Internal error---must be called by a member type of CSIG_TSERIES";
	CTimeSeries out;
	//unsigned int k(0), count = CountChains();
	//out.UpdateBuffer(count);
	//for (CTimeSeries *p = this; p; p = p->chain, k++)
	//	out.buf[k] = p->value();
	return out;
}

CTimeSeries CTimeSeries::evoke_getval(auxtype (CSignal::*fp)(unsigned int, unsigned int, void*) const, void *popt)
{
	// As of 12/19/2020, there's no need to worry about using a function going through evoke_getval for string or logical type.
	int _fs = fs == 2 ? 1 : fs;
	CTimeSeries out(_fs);
	// aout output argument from evoke_getval function pointer is always a scalar, i.e., auxtype
	auxtype outcarrier; 
	//if (GetType() == CSIG_TSERIES)
	//{
	//	out.Reset(1); // 1 means new fs
	//	CSignal tp = TSeries2CSignal();
	//	out.SetValue((tp.*fp)(0, 0, popt));
	//}
	//else
	{
		CTimeSeries tp(_fs);
		out.tmark = tmark;
		unsigned int len = Len();
		CVar aout(_fs); // additional output
		if (popt)
		{
			aout.UpdateBuffer(nGroups);
			aout.nGroups = nGroups;
		}
		// For audio, grouped data output is a chained output
		// For non-audio, grouped data output is a column vector
		if (fs > 2) // if audio
		{
			for (unsigned int k = 0; k < nGroups; k++)
			{
				tp.tmark = tmark + round(1000.*k*Len() / fs);
				if (popt)
				{ // popt is always a pointer to auxtype
					tp.SetValue((this->*fp)(k * len, len, &outcarrier));
					aout.buf[k] = outcarrier; // fed from individual function
				}
				else
					tp.SetValue((this->*fp)(k * len, len, NULL));
				out.AddChain(tp);
			}
			for (CTimeSeries* p = chain; p; p = p->chain)
			{
				for (unsigned int k = 0; k < p->nGroups; k++)
				{
					tp.tmark = p->tmark + round(1000. * k * p->Len() / fs);
					if (popt)
					{ // popt is always a pointer to auxtype
						tp.SetValue((p->*fp)(k * p->Len(), p->Len(), &outcarrier));
						CTimeSeries tp2(_fs);
						tp2.buf[k] = outcarrier;
						aout.AddChain(tp2);
					}
					else
						tp.SetValue((p->*fp)(k * p->Len(), p->Len(), NULL));
					out.AddChain(tp);
				}
			}
		}
		else
		{
			// This assumes that popt from the calling party is a pointer to a CVar object 
			if (popt && ((CVar*)popt)->nSamples	) outcarrier = ((CVar*)popt)->value();
			if (nGroups == nSamples) 
			{ // computing a column vector should yield a scalar. 7/7/2020
				nGroups = 1; 
				len = nSamples;
			}
			out.UpdateBuffer(nGroups);
			out.nGroups = nGroups;
			for (unsigned int k = 0; k < nGroups; k++)
			{
				if (popt)
				{
					out.buf[k] = (this->*fp)(k * len, len, &outcarrier);
					aout.buf[k] = outcarrier; // fed from individual function
				}
				else
					out.buf[k] = (this->*fp)(k * len, len, NULL);
			}
		}
		if (popt)	*(CVar*)popt = aout;
	}
	return out;
}

CSignal& CSignal::evoke_modsig(fmodify fp, void* pargin, void* pargout)
{
	auto len = Len();
	for (unsigned int k = 0; k < nGroups; k++)
	{
		(fp)((auxtype*)(strbuf + k * bufBlockSize * len), len, pargin, pargout);
	}
	return *this;
}

CSignal CSignal::evoke_modsig2(CSignal(*func) (const CSignal&, void*, void*), void* pargin, void* pargout)
{
	auto len = Len();
	CSignal out(fs);
	out.UpdateBuffer(nSamples);
	CSignal indy(fs);
	uint64_t lastcount = 0, count = 0;
	for (unsigned int k = 0; k < nGroups; k++)
	{
		CSignal bit(fs);
		bit.UpdateBuffer(len);
		memcpy(bit.buf, buf + k * len, sizeof(auxtype) * len);
		indy = (func)(bit, pargin, pargout);
		count += indy.nSamples;
		if (count > out.nSamples)
			out.UpdateBuffer(count);
		memcpy(out.buf + lastcount, indy.buf, sizeof(auxtype) * indy.nSamples);
		lastcount = count;
	}
	out.nSamples = count;
	return out;
}


CTimeSeries& CTimeSeries::evoke_modsig(fmodify fp, void* pargin, void* pargout)
{
	for (CTimeSeries* p = this; p; p = p->chain)
		p->CSignal::evoke_modsig(fp, pargin, pargout);
	return *this;
}

CTimeSeries CTimeSeries::evoke_modsig2(CSignal(*func) (const CSignal&, void*, void*), void* pargin, void* pargout)
{
	CTimeSeries out(fs);
	CTimeSeries outExt(fs);
	for (CTimeSeries* p = this; p; p = p->chain)
	{
		void* _pargin = NULL;
		void* _pargout = NULL;
		if (pargin)
			_pargin = pargin;
		if (pargout)
			_pargout = (void*)&outExt;
		CTimeSeries outtp = p->CSignal::evoke_modsig2(func, _pargin, _pargout);
		if (pargout)
		{
			outExt.tmark = p->tmark;
			((CTimeSeries*)pargout)->AddChain(*(CTimeSeries*)_pargout);
		}
		outtp.tmark = p->tmark;
		out.AddChain(outtp);
		out.nGroups = p->nGroups;
	}	return out;
}

CTimeSeries CTimeSeries::evoke_group2chain(CSignal(*func) (auxtype*, unsigned int, void*, void*), void* pargin, void* pargout)
{
	auto len = Len();
	uint16_t tp = type();
	CTimeSeries extra, extra0;
	void* temp = &extra0;
	CTimeSeries out0 = func(buf, len, pargin, temp);
	auto len0 = out0.nSamples;
	CTimeSeries out = out0;
	uint64_t extralen0;
	if (pargout)
	{
		extralen0 = extra0.nSamples;
		if (!ISTEMPORAL(tp))
			extra.UpdateBuffer(extralen0 * nGroups);
		else
			extra.UpdateBuffer(extralen0);
		extra.SetFs(extra0.GetFs());
		memcpy(extra.buf, extra0.buf, extralen0 * bufBlockSize);
	}
	// assumption1: output of (func) does not change its characteristics while looping.
	// assumption2: additional output of (func), temp above, does not change its characteristics while looping.
	// If this is AUDIO or TEMPORAL, group structure will turn to chain structure
	// to show the timestamp of each group
	if (!ISTEMPORAL(tp)) {
		// therefore, the outputs of (func) are simply stacked thru the loop and make the final output
		auto newLength = out0.nSamples * nGroups;
		if (newLength > out.nSamples)
			out.UpdateBuffer(newLength);
	}
	for (unsigned int k = 1; k < nGroups; k++)
	{
		auto outrow = func((auxtype*)(strbuf + len * k * bufBlockSize), len, pargin, temp);
		if (!ISTEMPORAL(tp)) {
			memcpy(out.strbuf + len0 * k * outrow.bufBlockSize, outrow.buf, len0 * outrow.bufBlockSize);
			if (pargout)
				memcpy(extra.strbuf + extralen0 * k * outrow.bufBlockSize, extra0.buf, extralen0 * outrow.bufBlockSize);
		}
		else {
			outrow.tmark = tmark + 1000.f * k * len / fs;
			out.AddChain(outrow);
			if (pargout) {
				extra0.tmark = outrow.tmark;
				extra.AddChain(extra0);
			}
		}
	}
	if (!ISTEMPORAL(tp)) {
		out.nSamples = out0.nSamples * nGroups; // necessary for [b,c]=a.max
		out.nGroups = nGroups; // correct for necessary for [b,c]=a.max; ---nGroups should just follow nGroups, if adjustment overall is needed, should be done inside the gate function
	}
	if (pargout) // 
	{
		if (!ISTEMPORAL(tp))
			extra.nGroups = nGroups;
		*(CTimeSeries*)pargout = extra;
	}
	return out;
} 

CTimeSeries CTimeSeries::evoke_getsig2(CSignal(*func) (auxtype*, unsigned int, void*, void*), void* pargin, void* pargout)
{
	CTimeSeries out(fs);
	CTimeSeries outExt(fs);
	for (CTimeSeries* p = this; p; p = p->chain)
	{
		void* _pargin = NULL;
		void* _pargout = NULL;
		if (pargin)
			_pargin = pargin;
		if (pargout)
			_pargout = (void*)&outExt;
		CTimeSeries outtp = p->evoke_group2chain(func, _pargin, _pargout);
		if (pargout) 
		{
			outExt.tmark = p->tmark;
			((CTimeSeries*)pargout)->AddChain(*(CTimeSeries*)_pargout);
		}
		outtp.tmark = p->tmark;
		out.AddChain(outtp);
	}
	return out;
}

CTimeSeries CTimeSeries::evoke_getsig(CTimeSeries(*func) (const CTimeSeries&, void*), void* popt)
{
	CTimeSeries out(fs);
	for (CTimeSeries* p = this, *q = &out, *r = (CTimeSeries*)popt; p; p = p->chain)
	{
		void* tp = NULL;
		if (r)
		{
			tp = r;
			r = r->chain;
		}
		CTimeSeries outtp = func(*p, tp);
		outtp.tmark = p->tmark;
		out.AddChain(outtp);
	}
	return out;
}

CSignal &CSignal::matrixmult(CSignal *arg)
{
	unsigned int col = Len();
	unsigned int col2 = arg->Len();
	if (col != arg->nGroups)
		throw "Column count must be the same as row count in the argument.";
	body out;
	out.UpdateBuffer(nGroups * col2);
	out.nGroups = nGroups;
	for (unsigned int m = 0; m < nGroups; m++)
		for (unsigned int n = 0; n < col2; n++)
		{
			auxtype tp = 0;
			for (unsigned int k = 0; k < col; k++)
			{
				tp += buf[m*col + k] * arg->buf[k*col2 + n];
			}
			out.buf[m*col2 + n] = tp;
		}
	return *this = out;
}

void CTimeSeries::AddMultChain(char op, CTimeSeries *sec)
{
	if (fs > 1 && sec->fs > 1 && fs != sec->fs)  throw "The sampling rates of both operands must be the same.";

	CTimeSeries *p;
	if (IsComplex() || sec->IsComplex())
	{
		SetComplex(); sec->SetComplex();
	}
	auto tp = type();
	auto sectp = sec->type();
	if (ISTEMPORAL(tp) || ISTSEQ(sectp)) // Check if the logic is right 11/25/2021
	{
		if (chained_scalar()) //special case: point by point operation
		{
			if (CountChains() != sec->CountChains())
				throw "Both tseq must be the same length.";
			for (CTimeSeries *p = this, *pp = sec; p; p = p->chain, pp = pp->chain)
			{
				if (p->nSamples != pp->nSamples)
					throw "Both tseq must have the same number of elements.";
			}
			for (CTimeSeries *p = this, *pp = sec; p; p = p->chain, pp = pp->chain)
			{
				if (op == '*')
					for (unsigned int k = 0; k < p->nSamples; k++)	p->buf[k] *= pp->buf[k];
				else
					for (unsigned int k = 0; k < p->nSamples; k++)	p->buf[k] += pp->buf[k];
			}
			return;
		}
		else if (op == '+')
			SwapContents1node(*sec); //if sec is tseq and this is not, swap here
	}
	if (!IsAudio() || !sec->IsAudio())
	{
		//special case: multiplication after interpolation
		if (op == '*' && (chained_scalar() || sec->chained_scalar()))
		{
			// need to pass down the case of scalar * tseq
			if ((IsAudio() && sec->chained_scalar()) || (sec->IsAudio() && chained_scalar()))
			{
				if (chained_scalar())
					SwapContents1node(*sec);
				bool relTime = sec->fs == 0;
				vector<double> tpoints, tvals;
				for (CTimeSeries *p = sec; p; p = p->chain)
				{
					tpoints.push_back(p->tmark);
					tvals.push_back(p->value());
				}
				if (relTime)
				{
					for (auto &tp : tpoints)
						tp *= dur();
				}
				//for (CTimeSeries *p = this; p; p = p->chain)
				//	p->CSignal::Modulate(tpoints, tvals);
				return;
			}
		}
		for (p = this; p; (p = p->chain) && (sec->nSamples > 0))
		{
			for (unsigned int k = 0; k < p->nGroups; k++)
				p->addmult(op, *sec, k*p->Len(), p->Len());
		}
		if (sec->IsAudio())
			SetFs(sec->fs);
		return;
	}

	vector<double> thistmark;
	vector<double> sectmark;
	for (p = this; p; p = p->chain)
	{
		thistmark.push_back(p->tmark);
		thistmark.push_back(p->tmark + p->_dur());
	}
	for (p = sec; p; p = p->chain)
	{
		sectmark.push_back(p->tmark);
		sectmark.push_back(p->tmark + p->_dur());
	}
	unsigned int k, im(0), is(0); // im: index for master (this), is: index for sec
	double anc1, anc0;
	short status(0);
	vector<double> anc; // just for debugging/tracking purposes... OK to delete.
	unsigned int count;
	anc0 = min(thistmark[im], sectmark[is]);
	anc.push_back(anc0);
	CTimeSeries out(sec->GetFs()), part(sec->GetFs());
	if (IsComplex() || sec->IsComplex()) part.SetComplex();
	do {
		if (im == thistmark.size() && is == sectmark.size())
			break;
		if (is == sectmark.size()) //only thistmark is left
			status += (short)(pow(-1., (int)im)), im++;
		else if (im == thistmark.size()) //only sectmark is left
			status += (short)(2 * pow(-1., (int)is)), is++;
		else if (thistmark[im] <= sectmark[is]) // both are available. check them
			status += (short)(pow(-1., (int)im)), im++;
		else
			status += (short)(2 * pow(-1., (int)is)), is++;
		if (im == thistmark.size() && is == sectmark.size())
			break;
		else if (im < thistmark.size() && is < sectmark.size())
			anc1 = min(thistmark[im], sectmark[is]);
		else // this means only one of the two, thistmark and sectmark, is left, just pick the available one.
			if (sectmark.size() - is > 0)
				anc1 = sectmark[is];
			else
				anc1 = thistmark[im];
		anc.push_back(anc1);

		if (status > 0) // if status is 0, skip this cycle--no need to add chain
		{
			CTimeSeries *pm = this;
			CTimeSeries *ps = sec;
			// status indicates in the time range between the last two values of anc, what signal is present
			// 1 means this, 2 means sec, 3 means both
			count = (unsigned int)round((anc1 - anc0) / 1000.*fs);
			unsigned int blockSize = bufBlockSize / sizeof(auxtype);
			{
				part.UpdateBuffer(count);
				part.tmark = anc0;
				part.nGroups = nGroups;
				if (status & 1)
				{
					for (k = 0, p = pm; k < im / 2; k++/*, p=p->chain*/)
					{
						if (p->chain != NULL) p = p->chain;
						else				break;
					}
					int id = (int)round((anc0 - p->tmark) / 1000.*fs);
					memcpy(part.buf, p->logbuf + id * bufBlockSize, part.nSamples*bufBlockSize);
				}
				if (status & 2)
				{
					for (k = 0, p = ps; k < is / 2; k++)
						if (p->chain != NULL) p = p->chain;
					int id = (int)round((anc0 - p->tmark) / 1000.*fs);
					if (status & 1) // op=='+' add the two,  op=='*' multiply
					{
						if (op == '+')
						{
							if (part.IsComplex()) for (unsigned int k = 0; k < count; k++)	part.cbuf[k] += p->cbuf[k + id];
							else				 for (unsigned int k = 0; k < count; k++)	part.buf[k] += p->buf[k + id];
						}
						else if (op == '*')
						{
							if (part.IsComplex()) for (unsigned int k = 0; k < count; k++)	part.cbuf[k] *= p->cbuf[k + id];
							else				 for (unsigned int k = 0; k < count; k++)	part.buf[k] *= p->buf[k + id];
						}
					}
					else
					{
						memcpy(part.buf, p->logbuf + id * bufBlockSize, part.nSamples*bufBlockSize);
					}
				}
			}
			if (out.tmark == 0. && out.nSamples == 0) // the very first time
				out = part;
			else if (count > 0 && part.nSamples > 0) // if count==0, don't add part (which might be leftover from the previous round)
				out.AddChain(part);
		}
		anc0 = anc1;
	} while (im < thistmark.size() || is < sectmark.size());
	*this = out.ConnectChains();
}

CTimeSeries& CTimeSeries::ConnectChains()
{
	CTimeSeries *p(this), out(fs);
	if (p == NULL || p->chain == NULL) return *this;
	if (tmark > 0)
	{
		double shift(tmark);
		for (; p; p = p->chain)	p->tmark -= shift;
		ConnectChains();
		p = this;
		for (; p; p = p->chain)	p->tmark += shift;
		return *this;
	}
	while (1)
	{
		CTimeSeries part(fs);
		if (IsComplex()) part.SetComplex();

		part.tmark = p->tmark;
		unsigned int count(0);
		// Get the count needed to consolidate 
		for (CTimeSeries *tp(p); ;) {
			count += tp->nSamples;
			//at this point tmark should have been adjusted to be very close to align with the fs grid, i.e., tp->chain->tmark/1000.*fs would be very close to an integer
			// if it is an integer, tmark was already aligned with the grid so no need to adjust
			// if it is not an integer (but very close to one), it should be forced to be the one.. now I'm adding .01 
			// this raises a possibility to make tp->chain->tmark/1000.*fs overrun (end up making one greater than it should be) if its decimal is 0.49, but that's not likely...
			// 6/4/2016 bjk
			// if nSamples so far is the same as the following chain tmark, it means continuing (i.e., the chain consolidated)
			// "Being the same" is too restrictive. Allow error margin of 2.
			// 7/18/2016 bjk
			if (tp->chain != NULL && fabs(tp->chain->tmark / 1000.*fs - count) <= 2)
				tp = tp->chain;
			else
				break;
		}
		// Consolidate 
		part.UpdateBuffer(count);
		int offset(0);
		while (1) {
			memcpy(part.logbuf + offset, p->buf, p->nSamples*bufBlockSize);
			offset += p->nSamples*bufBlockSize;
			if (p->chain != NULL && offset == (int)(p->chain->tmark / 1000.*fs + .1)*bufBlockSize)
				p = p->chain;
			else
				break;
		}
		if (out.tmark == 0. && out.nSamples == 0) // the very first time
			out = part;
		else
			out.AddChain(part);
		if (p->chain == NULL) break;
		p = p->chain;
	}
	return (*this = out);
}


CTimeSeries& CTimeSeries::operator/=(CTimeSeries &scaleArray)
{
	operate(scaleArray, '/');
	return *this;
}

CTimeSeries& CTimeSeries::operator/=(auxtype con)
{ // used in fft.cpp
	operate(CTimeSeries(con), '/');
	return *this;
}

CSignal& CSignal::reciprocal(void)
{
	for (unsigned int k = 0; k < nSamples; k++)
		buf[k] = 1.f / buf[k];
	return *this;
}

CTimeSeries& CTimeSeries::reciprocal(void)
{
	CSignal::reciprocal();
	return *this;
}

CSignal& CSignal::operator>>=(double delta)
{
	if (delta == 0)		return *this;
	double newtmark = quantizetmark(tmark + delta, fs);
	if (newtmark < 0) // cut off negative time domain
	{
		auto remainingDur = quantizetmark(endt() + delta, fs);
		if (remainingDur<=0.)
		{ // nothing left
			Reset(1); // needs to specify 1 to make it NULL with fs=1
			return *this;
		}
		// at this point, pts2remove cannot exceed nSamples
		auto pts2remove = (unsigned int)(nSamples - remainingDur / 1000. * fs + .5);
		tmark = 0;
		nSamples -= pts2remove;
		memmove(logbuf, logbuf + pts2remove * bufBlockSize, nSamples * bufBlockSize);
	}
	else
		tmark += delta;
	return *this;
}

CTimeSeries& CTimeSeries::operator>>=(double delta)
{
	for (CTimeSeries *p = this; p; p = p->chain)
	{ // CSignal with the face of CTimeSeries. No need to worry about chain processing
		CSignal *pp = (CSignal*)p;
		pp->CSignal::operator>>=(delta);
		if (pp->nSamples == 0)
		{
			Reset();
		}
	}
	return *this;
}

CTimeSeries& CTimeSeries::AddChain(const CTimeSeries &sec)
{ // MAKE SURE sec is not empty
	auto tp = type();
	if ( tp == TYPEBIT_NULL || tp == TYPEBIT_SIZE1) // NULL or empty string
		return *this = sec;
	if (chain == NULL)
	{
		auto _dur = (double)nSamples / fs;
		if (1000.f * nSamples / fs >= sec.tmark)
			operate(sec, '+');
		else
			return *(chain = new CTimeSeries(sec));
		return *this;
	}
	else
		return chain->AddChain(sec);
}

CTimeSeries * CTimeSeries::GetDeepestChain()
{
	CTimeSeries *p(this);
	for (; p->chain; p = p->chain)
		;
	return p;
}

uint64_t CTimeSeries::CountChains(uint64_t *maxlength) const
{
	// verify again all involved with this 9/18/2018
	uint64_t maxy(0), res(1);
	for (const CTimeSeries* p = this; p->chain; p = p->chain, res++)
		maxy = max(p->nSamples, maxy);
	if (maxlength) *maxlength = maxy;
	return res;
}

double CSignals::alldur() const
{
	double out = CTimeSeries::alldur();
	if (next)
		out = max(out, next->alldur());
	return out;
}

double CTimeSeries::alldur() const
{
	double out;
	for (CTimeSeries *p = (CTimeSeries *)this; p; p = p->chain)
		out = p->CSignal::endt();
	return out;
}

double CTimeSeries::MakeChainless()
{ //This converts the null intervals of the signal to zero.
	fs = max(fs, 1);
	double newdur = alldur();	// doing this here as fs might have changed.
	if (!tmark && !chain)	// already chainless && no padding required.
		return newdur;

	CTimeSeries nsig(fs);
	nsig.UpdateBuffer((unsigned int)round(newdur / 1000.*fs));
	for (CTimeSeries *p = this; p; p = p->chain) {
		if (p->tmark + p->_dur() <= 0)
			continue;
		unsigned int iorg = (unsigned int)((p->tmark < 0) ? round(-p->tmark / 1000.*fs) : 0);
		unsigned int inew = (unsigned int)((p->tmark > 0) ? round(p->tmark / 1000.*fs) : 0);
		unsigned int cplen = p->nSamples - iorg;
		if (inew + cplen > nsig.nSamples) {
			if (p->chain == NULL && inew + cplen == nsig.nSamples + 1 && newdur / 1000.*fs - nSamples > 0.499999)	// last chain, only 1 sample difference, possile rounding error.
				nsig.UpdateBuffer(nsig.nSamples + 1);
			else
				throw "Internal error: Buffer overrun detected.";
		}
		memcpy(nsig.buf + inew, p->buf + iorg, cplen * sizeof(*buf));
	}
	SwapContents1node(nsig);	// *this gets the chainless signal, nsig gets the chained signal and will be destroyed soon.
	return newdur;
}

//Remove from the class... make it static AuxFunc 11/14/2021
CTimeSeries& CTimeSeries::Squeeze()
{
	int nSamplesTotal(0), nSamples0(nSamples);
	for (CTimeSeries* p(this); p; p = p->chain)
		nSamplesTotal += p->nSamples;
	UpdateBuffer(nSamplesTotal);
	nSamplesTotal = nSamples0;
	for (CTimeSeries* p(chain); p; p = p->chain)
		memcpy(&buf[nSamplesTotal], p->buf, p->nSamples * sizeof(p->buf[0])), nSamplesTotal += p->nSamples;
	delete chain;
	chain = NULL;
	return *this;
}

CTimeSeries& CTimeSeries::MergeChains()
{// This tidy things up by removing unnecessary chains and rearranging them.
	CTimeSeries temp;
	if (nSamples == 0 && chain) { temp = *chain; chain = NULL; *this = temp; }

	for (CTimeSeries* p(this); p && p->chain; p = p->chain)
	{
		double et = p->tmark + p->_dur();
		if (et >= p->chain->tmark) // consolidate 
		{
			temp = *p->chain;
			int id1 = (int)round((temp.tmark - p->tmark) / 1000.*fs);
			int common = p->nSamples - id1;
			int id2 = temp.nSamples - common;
			int oldnSamples(p->nSamples);
			p->UpdateBuffer(p->nSamples + temp.nSamples - common);
			/* overlapping portion */
			for (int i = 0; i < common; i++)
				p->buf[oldnSamples - common + i] += temp.buf[i];
			for (unsigned int k = 0; k < temp.nSamples - common; k++)
				p->buf[oldnSamples + k] = temp.buf[common + k];
			if (temp.chain == NULL)	p->chain = NULL;
			else *p->chain = *temp.chain; // deep copy is necessary (we are losing p).
		}
	}
	return *this;
}

CTimeSeries& CTimeSeries::LogOp(CTimeSeries &rhs, int type)
{
	for (CTimeSeries *p = this; p; p = p->chain)
		p->body::LogOp(rhs, type);
	return *this;
}

int mceil(auxtype x)
{ // modified ceil with reasonable tolerance
  // This function prevents uncessarily rounding-up
  // if x has long trailing zeros followed by one (due to rounding) such as 513.0000000001 
  // In that case we don't want 514...
  // 5/26/2018
	int implicitFloor = (int)x;
	auxtype ceiled = ceil(x);
	if (ceiled - implicitFloor == 0.)
		return implicitFloor;
	if (x - implicitFloor < 1.e-6)
		return implicitFloor;
	else
		return (int)ceiled;
}

CTimeSeries& CTimeSeries::removeafter(double timems)
{ // if timems is in the grid, the point is removed (but dur will be until that grid point)
	CTimeSeries *last = NULL;
	for (CTimeSeries *p(this); p; p = p->chain)
	{
		if (timems > p->tmark + p->_dur()) { last = p; continue; }
		else if (timems > p->tmark)
		{
			// no need to worry about p->tmark
			// all points occuring on and after timems will be removed (dur will still be timems)
			// if timems exceeds the grid of p->tmark, it won't be removed, so it is ceil.
			p->nSamples = mceil((timems - p->tmark)*fs / 1000.);
		}
		else if (last)
		{
			if (!ghost)	delete[] p;
			last->chain = NULL;
			break;
		}
		else
		{ // if p->tmark is less than timems AND last is NULL, which means the very first p happening after timems
			Reset(fs);
			break;
		}
		last = p;
	}
	return *this;
}

CTimeSeries& CTimeSeries::timeshift(double timems)
{ // if timems is in the grid, the point is kept.
	int chainlevel(0);
	CTimeSeries *p(this);
	for (; p; p = p->chain)
	{
		if (timems > p->tmark + p->_dur()) { chainlevel++; continue; }
		if (timems < p->tmark)
			p->tmark -= timems;
		else
		{
			// p->tmark should be zero, except for the small off-granular timing based on the difference between timems and p->tmark
			// the decrease of points will step-up if timems exceeds the grid, so it is ceil.
			int pointsless = mceil((timems - p->tmark)*fs / 1000.);
			if (pointsless > 0)
			{
				p->nSamples -= pointsless;
				if (!ghost)
				{
					bool *tbuf = new bool[p->nSamples*bufBlockSize];
					memcpy(tbuf, p->buf + pointsless, p->nSamples*bufBlockSize);
					delete[] p->logbuf;
					p->logbuf = tbuf;
				}
				else
					p->buf += pointsless;
				p->tmark = 0;
			}
		}
	}
	//all chains at and prior to chainlevel are cleared here.
	p = this;
	for (int k(0); k < chainlevel; k++, p = p->chain)
		if (!ghost) delete[] p->buf;
	if (p != this) // or if chainlevel is non-zero 
	{
		// Make new chain after chainlevel. If p is NULL (make an empty CTimeSeries object to return);
		if (!p) { p = new CTimeSeries; }
		nSamples = p->nSamples;
		tmark = p->tmark;
		buf = p->buf;
		chain = NULL;
	}
	return *this;
}

CTimeSeries& CTimeSeries::Crop(double begin_ms, double end_ms)
{
	if (begin_ms == end_ms) { Reset(); return *this; }
	if (begin_ms > end_ms) {
		Crop(end_ms, begin_ms);
		ReverseTime();
		return *this;
	}
	removeafter(end_ms);
	if (nSamples>0)
		timeshift(begin_ms);
	return *this;
}

#ifndef NO_FFTW

CSignal& CSignal::movespec(unsigned int id0, unsigned int len, void *parg)
{
	if (len == 0) len = nSamples;
	CSignals shift = *(CSignals*)parg;
	CSignals copy(*this);
//	Hilbert();
	auxtype t(0), grid(1.f / fs);
	const complex<auxtype> j(0.0, 1.0);
	complex<auxtype> datum;
	auxtype val = shift.value();
	for (unsigned int k = 0; k < nSamples; k++)
	{
		datum = (copy.buf[k] + buf[k] * j) * exp(j * val *  2. * PI * t);
		buf[k] = real(datum);
		t += grid;
	}
	return *this;
}

#endif 

pair<CTimeSeries*, int> CTimeSeries::FindChainAndID(double timept, bool begin)
{ 
	CTimeSeries *body = NULL;
	CTimeSeries* p = this;
	int id;
	for (; p; p = p->chain)
	{
		auto ent = p->CSignal::endt();
		if (timept >= p->tmark &&  (ent- timept) > 1./fs) {
			body = p;
			id = round((timept - p->tmark) / 1000. * fs);
			if (!begin) id--;
			break;
		}
		if (begin) {
			if (timept < p->tmark) {
				body = p;
				id = round((timept - p->tmark) / 1000. * fs);
				break;
			}
		}
		else {
			if (timept >= p->CSignal::endt()) {
				body = p;
				id = p->nSamples-1;
			}
		}
	}
	return make_pair(body, id);
}

CTimeSeries& CTimeSeries::ReplaceBetweenTPs(const CTimeSeries &newsig, double t1, double t2)
{ // signal portion between t1 and t2 is replaced by newsig
 // t1 and t2 are in ms
//	float lastendtofnewsig = newsig.GetDeepestChain()->endt();

	CTimeSeries *p(this);
	CTimeSeries copy(*this);
	double samplegrid = 1.f / fs;
	double deviationfromgrid = t1 - ((double)(int)(t1*fs)) / fs;
	bool inbet(false); // true means t1 is in between chains.
	for (p = this; p && !inbet; p = p->chain)
	{
		if (t1 > p->tmark + p->_dur() && p->chain && t1 < p->chain->tmark)
			inbet = true;
	}
	if (t1 > 0. && fabs(deviationfromgrid) > 1.e-8 && deviationfromgrid > -samplegrid && deviationfromgrid < samplegrid)
		t1 -= 1000.f*samplegrid;  // because its in ms
	Crop(0, t1);
	//if (inbet)
	//	newsig >>= t1 - (tmark + _dur());
	//if (newsig.chain) { delete newsig.chain; newsig.chain = NULL; }
	*this += (CTimeSeries*)&newsig;
	//if t2 coincides with the sampling grid, don't take that point here (it will be taken twice)
	deviationfromgrid = t2 - ((double)(int)(t2*fs)) / fs;
	// deviationfromgrid of zero can masquerade as a very small number
	if (fabs(deviationfromgrid) > 1.e-8 && deviationfromgrid > -samplegrid && deviationfromgrid < samplegrid)
		t2 += 1000.f*samplegrid;  // because its in ms
	copy.Crop(t2, std::numeric_limits<double>::infinity());
	*this += &copy;
	MergeChains();
	return *this;
}

//Move it to auxfunc
CTimeSeries& CTimeSeries::NullIn(double tpoint)
{
	double tp = quantizetmark(tpoint, fs);
	for (CTimeSeries *p = this; p; p = p->chain)
	{
		if (p->endt() < tp) continue;
		int count = (int)((tp - p->tmark) * fs / 1000 + .5);
		CTimeSeries *temp = p->chain;
		CTimeSeries *newchain = new CTimeSeries(fs);
		newchain->UpdateBuffer(p->nSamples - count);
		memcpy(newchain->buf, p->buf + count, sizeof(auxtype)*count);
		newchain->tmark = tp;
		newchain->chain = temp;
		p->nSamples = count;
		p->chain = newchain;
		return *this;
	}
	return *this;
}

// Remove from the class; move it to auxFunc 11/14/2021
bool CTimeSeries::IsAudioOnAt(double timept)
{
	if (!IsAudio()) return false;
	CTimeSeries *p(this);
	for (; p; p = p->chain)
	{
		if (timept < p->tmark)
			return false;
		if (timept >= p->tmark && timept <= p->tmark + p->dur())
			return true;
	}
	return false;
}

int CSignal::GetType() const
{
		return 0;
}

// CTimeSeries GetType() returning CSIG_TSERIES in the old code means
// (nSamples = 1 and non-zero tmark) or (nSamples = 1 and non-NULL chain)
bool CTimeSeries::chained_scalar() const
{
	if (tmark > 0 && nSamples == 1) return true;
	if (chain && nSamples == 1)	return true;
	return false;
}

CTimeSeries * CTimeSeries::AtTimePoint(double timept)
{ // This retrieves CSignal at the specified time point. If no CSignal exists, return NULL.
	for (CTimeSeries *p = this; p; p = p->chain)
	{
		if (p->CSignal::endt() < timept) continue;
		if (timept < p->tmark) return NULL;
		return p;
	}
	return NULL;
}
static int getSIH(int len, float r1, float r2, int *outlength)
{
	float _r1 = 1. / r1;
	float _r2 = 1. / r2;
	float ratio_mean = 2. / (1. / _r1 + 1. / _r2);
	int N = (int)round(len*ratio_mean);
	*outlength = N;
	float sum = 0;
	float ratio;
	for (int k = 0; k < N; k++)
	{
		ratio = _r1 + (_r2 - _r1)*k / N;
		sum += 1. / ratio;
	}
	float  leftover = len - sum;
	*outlength += (int)round(leftover * ratio);
	return (int)round(leftover);
}

CSignal& CSignal::resample(unsigned int id0, unsigned int len, void *parg)
{
	//This doesn't mean real "resampling" because this does not change fs.
	//pratio < 1 means generate more samples (interpolation)-->longer duration and lower pitch
	//pratio > 1 means downsample--> shorter duration and higher pitch
	//On return, pratio is set with the actual ratio (incoming size / result size) for each chain
	if (len == 0) len = nSamples;
	CSignals *pratio = (CSignals *)parg;
	char errstr[256] = {};
	//SRC_DATA conv;
	//float *data_out, *data_in = new float[nSamples];
	//int errcode;
	//SRC_STATE* handle = src_new(SRC_SINC_MEDIUM_QUALITY, 1, &errcode);
	//if (errcode)
	//{
	//	pratio->SetString(src_strerror(errcode));
	//	return *this;
	//}
	//for (unsigned int k = 0; k < nSamples; k++) data_in[k] = (float)buf[k];
	//conv.data_in = data_in;
	//if (!pratio->chained_scalar())
	//{
	//	conv.src_ratio = 1. / pratio->value();
	//	conv.input_frames = nSamples;
	//	conv.output_frames = (long)(nSamples * conv.src_ratio + .5);
	//	conv.data_out = data_out = new float[conv.output_frames];
	//	conv.end_of_input = 1;
	//	errcode = src_process(handle, &conv);
	//	if (errcode)
	//	{
	//		pratio->SetString(src_strerror(errcode));
	//		delete[] data_in;	delete[] data_out;
	//		return *this;
	//	}
	//	UpdateBuffer(conv.output_frames);
	//	long k;
	//	for (k = 0; k < conv.output_frames_gen; k++)
	//		buf[k] = conv.data_out[k];
	//	for (k = conv.output_frames_gen; k < conv.output_frames; k++)
	//		buf[k] = 0;
	//}
	//else
	//{
	//	int blockCount = 0;
	//	vector<float> outbuffer;
	//	//inspect pratio to estimate the output length
	//	int cum = 0, cumID = 0;
	//	for (CTimeSeries *p = pratio; p && p->chain; p = p->chain)
	//		cum += (int)((p->chain->tmark - p->tmark) * fs / 1000 * p->value());
	//	outbuffer.reserve(cum);
	//	int lastSize = 1, lastPt = 0;
	//	data_out = new float[lastSize];
	//	long inputSamplesLeft = (long)nSamples;
	//	int orgSampleCounts = 0;
	//	//assume that pratio time sequence is well prepared--
	//	for (CTimeSeries *p = pratio; p && p->chain; p = p->chain)
	//	{
	//		conv.end_of_input = 0;
	//		unsigned int i1, i2;
	//		float ratio_mean;
	//		int inBuffersize, outBuffersize;
	//		if (p->value() == p->chain->value())
	//			src_set_ratio(handle, conv.src_ratio = ratio_mean = 1. / p->value());
	//		else
	//		{
	//			src_set_ratio(handle, 1. / p->value());
	//			conv.src_ratio = 1. / p->chain->value();
	//			ratio_mean = (2 * 1. / p->value()*1. / p->chain->value() / (1. / p->value() + 1. / p->chain->value())); // harmonic mean
	//		}
	//		//current p covers from p->tmark to p->chain->tmark
	//		if (!p->chain->chain)
	//			conv.input_frames = inputSamplesLeft;
	//		else
	//		{
	//			//current p covers from p->tmark to p->chain->tmark
	//			i1 = (int)(p->tmark * fs / 1000);
	//			i2 = (int)(p->chain->tmark * fs / 1000);
	//			conv.input_frames = i2 - i1;
	//		}
	//		conv.output_frames = (long)(conv.input_frames * ratio_mean + .5); // when the begining and ending ratio is different, use the harmonic mean for the estimate.
	//		if (conv.output_frames > lastSize)
	//		{
	//			delete[] data_out;
	//			data_out = new float[lastSize = conv.output_frames + 20000];//reserve the buffer size big enough to avoid memory crash, but find out a better than this.... 3/20/2019
	//		}
	//		conv.data_out = data_out;
	//		int harmean;
	//		int out2 = getSIH(conv.input_frames, p->value(), p->chain->value(), &harmean);
	//		int harmean0 = harmean;
	//		int newlen = harmean + out2 / 2;
	//		errcode = src_process(handle, &conv);
	//		inBuffersize = conv.input_frames_used;
	//		if ( errcode)
	//		{
	//			std::string errout;
	//			sformat(errout, "Error in block %d--%s", blockCount++, src_strerror(errcode));
	//			pratio->SetString(errout.c_str());
	//			delete[] data_in;	delete[] data_out;
	//			return *this;
	//		}
	//		outBuffersize = conv.output_frames_gen;
	//		for (int k = 0; k < conv.output_frames_gen; k++)
	//			outbuffer.push_back(data_out[k]);
	//		lastPt += conv.input_frames_used;
	//		if (p->chain->chain)
	//		{
	//			conv.data_in = &data_in[lastPt];
	//			inputSamplesLeft -= conv.input_frames_used;
	//		}
	//		while (conv.input_frames)
	//		{
	//			conv.src_ratio = 1. / p->chain->value();
	//			conv.data_in = &data_in[lastPt];
	//			conv.input_frames -= conv.input_frames_used;
	//			conv.end_of_input = conv.input_frames == 0 ? 1 : 0;
	//			errcode = src_process(handle, &conv);
	//			inBuffersize += conv.input_frames_used;
	//			outBuffersize += conv.output_frames_gen;
	//			for (int k = 0; k < conv.output_frames_gen; k++)
	//				outbuffer.push_back(data_out[k]);
	//			lastPt += conv.input_frames_used;
	//		}
	//		src_reset(handle);
	//		p->chain->tmark = p->tmark + 1000. / fs * outBuffersize;
	//	}
	//	UpdateBuffer((unsigned int)outbuffer.size());
	//	memcpy(buf, &outbuffer[0], sizeof(float)*outbuffer.size());
	//}
	//src_delete(handle);
	//delete[] data_in;
	//delete[] data_out;
	return *this;
}

void CTimeSeries::ReverseTime()
{
	for (CTimeSeries *p = this; p; p = p->chain)
		p->CSignal::ReverseTime();
}

void CSignal::ReverseTime()
{
	CSignal temp(*this);
	double *tempBuf = temp.GetBuffer();
	for (unsigned int i = 0; i < nSamples; i++)
		tempBuf[nSamples - i - 1] = buf[i];
	*this = temp;
}

string CSignal::str() const
{
	unsigned int k;
	string out;
	out.resize(nSamples);
	for (k = 0; k < nSamples && strbuf[k]; k++) {
		out[k] = *(strbuf + k);
	}
	out.resize(k);
	return out;
}

char *CSignal::getString(char *str, const int size)
{
	int len = min((int)nSamples, size - 1);
	memcpy(str, strbuf, len);
	str[len] = '\0';
	return str;
}

CSignal &CSignal::SetString(const char *str)
{
	Reset(2);
	bufType = 'S';
	bufBlockSize = 1;
	UpdateBuffer((int)strlen(str) + 1);
	strcpy(strbuf, str);
	return *this;
}

CSignal &CSignal::SetString(const char c)
{
	Reset(2);
	bufType = 'S';
	bufBlockSize = 1;
	if (c == 0) return *this;
	UpdateBuffer(2);
	memset(strbuf, 0, 2);
	strbuf[0] = c;
	nSamples = 1;
	return *this;
}

CTimeSeries& CTimeSeries::each(auxtype(*fn)(auxtype))
{
	for (CTimeSeries* p = this; p; p = p->chain)
		p->body::each(fn);
	return *this;
}

CTimeSeries& CTimeSeries::each_allownegative(auxtype(*fn)(auxtype))
{
	for (CTimeSeries* p = this; p; p = p->chain)
		p->body::each_sym(fn);
	return *this;
}

CTimeSeries& CTimeSeries::each(complex<auxtype>(*fn)(complex<auxtype>))
{
	if (IsAudio())
	{
		throw "No audio object allowed.";
	}
	else
	{
		for (CTimeSeries* p = this; p; p = p->chain)
			p->body::each(fn);
	}
	return *this;
}

CTimeSeries& CTimeSeries::each(auxtype(*fn)(complex<auxtype>))
{
	for (CTimeSeries* p = this; p; p = p->chain)
		p->body::each(fn);
	return *this;
}

CTimeSeries& CTimeSeries::each(auxtype(*fn)(auxtype, auxtype), const CSignal &arg2)
{
	if (IsAudio())
	{
		for (CTimeSeries* p = this; p; p = p->chain)
			p->body::each_sym2(fn, arg2);
	}
	else
	{
		for (CTimeSeries* p = this; p; p = p->chain)
			p->body::each(fn, arg2);
	}

	return *this;
}

CTimeSeries& CTimeSeries::each(complex<auxtype>(*fn)(complex<auxtype>, complex<auxtype>), const CSignal& arg2)
{
	for (CTimeSeries* p = this; p; p = p->chain)
		p->body::each(fn, arg2);
	return *this;
}

CTimeSeries& CTimeSeries::transpose1()
{
	if (nSamples == 1)
		return *this;
	CTimeSeries t(buf[0]);
	SwapContents1node(t);
	return *this;
}

int CTimeSeries::WriteAXL(FILE* fp)
{
	CTimeSeries *p(this);
	size_t res;
	unsigned int nChains = CountChains();
	res = fwrite((void*)&fs, sizeof(fs), 1, fp);
	res = fwrite((void*)&nChains, sizeof(nChains), 1, fp);
	for (unsigned int k = 0; k < nChains; k++, p = p->chain)
	{
		res = fwrite((void*)&p->nSamples, sizeof(nSamples), 1, fp);
		res = fwrite((void*)&p->tmark, sizeof(tmark), 1, fp);
		res = fwrite((void*)p->buf, p->bufBlockSize, p->nSamples, fp);
	}
	return (int)res;
}

CSignals::CSignals()
	: next(NULL)
{
}

CSignals::CSignals(bool *b, unsigned int len)
	: next(NULL)
{
	Reset(1);
	bufBlockSize = 1; // logical array
	UpdateBuffer(len);
	bool bv = *b;
	for_each(logbuf, logbuf + len, [bv](bool &v) { v = bv; });
}

CSignals::CSignals(int sampleRate)
	:next(NULL)
{
	SetFs(max(sampleRate, 0));
}

CSignals::CSignals(auxtype value)
	: next(NULL)
{
	SetFs(1);
	SetValue(value);
}

CSignals::CSignals(const CSignals& src)
	:next(NULL)
{
	*this = src;
}

CSignals::CSignals(const CTimeSeries& src)
	: next(NULL)
{
	*this = src;
}

CSignals::CSignals(auxtype* y, int len)
	: next(NULL)
{
	SetFs(1);
	UpdateBuffer(len);
	memcpy(buf, y, sizeof(auxtype) * len);
}

CSignals::~CSignals()
{
	if (ghost) {
		next = nullptr; buf = nullptr; }
	if (next) {
		delete next;
		next = NULL;
	}
}

CSignals::CSignals(std::string str)
	:next(NULL)
{
	SetString(str.c_str());
}

bool CTimeSeries::fill_short_buffer(double tpoint_sec, int len, vector<auxtype>& out)
{ // Make a data buffer at specified time_point in sec. 
	// finder is a pair of (CTimeSeries object that covers the time point, index at that time point)
	auto ent = alldur();
	auto finder = FindChainAndID(tpoint_sec * 1000., true);
	if (finder.first == NULL && (ent/1000. - tpoint_sec) <= 1. / fs) {
		// the object is not found(time point is beyond the time of the object)
		out.resize(len);
		return true;
	}
	auto begin_id = finder.second;
	if (len == 0) {
		if (finder.first->nSamples - begin_id > 0)
			return false;
		else
			return true;
	}
	bool nomoredata;
	int k = 0;
	if (begin_id >= 0) {
		for (; k < min(len, (int)(finder.first->nSamples - begin_id)); k++)
			out.push_back(finder.first->buf[begin_id + k]);
		int left_in_buffer = finder.first->nSamples - begin_id - k;
		if (left_in_buffer == 0) {
			if (chain) {
				if (len == k)
					return false;
				auto timeadvance = round(1000. * k / fs) / 1000.;
				auto newtimepoint = tpoint_sec + timeadvance;
				vector<auxtype> out2;
				nomoredata = fill_short_buffer(tpoint_sec + timeadvance, len - k, out2);
				for (auto v : out2)
					out.push_back(v);
			}
			else {
				int need_to_fill_more = len - k;
				nomoredata = true;
				 for (k=0; k < need_to_fill_more; k++)
					out.push_back(0.);
			}
		}
		else {
			if (len == k)
				return false;
			auto timeadvance = round(1000. * k / fs) / 1000.;
			auto newtimepoint = tpoint_sec + timeadvance;
			vector<auxtype> out2;
			nomoredata = fill_short_buffer(tpoint_sec + timeadvance, len - k, out2);
			for (auto v : out2)
				out.push_back(v);
		}
		return nomoredata;
	}
	else {
		// zeros filled in and stopped before reaching the end of the next... 
		out.resize(min(len, -begin_id));
		fill(out.begin(), out.begin() + min(len, -begin_id), 0);
		if (len < -begin_id) {
			// Return true or false depending on it's done or more coming.
			nomoredata = chain == NULL ? true : false; // wait.. chain shnouldn't be NULL... 
		}
		else {
			// zeros filled in, now about to hit the next one
			auto timeadvance = round(1000. * -begin_id / fs) / 1000.;
			auto newtimepoint = tpoint_sec + timeadvance;
			vector<auxtype> out2;
			nomoredata = fill_short_buffer(newtimepoint, len - out.size(), out2);
			for (auto v : out2)
				out.push_back(v);
		}
		return nomoredata;
	}
}
/* fill_short_buffer
* Make a copy of the buf data at the specified time point and the length.
* len is meant to be small enough to make copying of the data buffer easy.
* Used playCallback in audio_play.cpp
* If null at the time point, the data is zero buffer until a non-null portion is available.
* The output :
        CTimeSeries::fill_short_buffer : vector 
		CSignals::fill_short_buffer : pair of vectors (for mono, the first item is the buffer, the second is a vector of size zero
* Return value: true if there's no further data after this buffer, false otherwise
*/
bool CSignals::fill_short_buffer(double tpoint_sec, int len, vector<auxtype>& out1, vector<auxtype>& out2)
{
	out1.clear();
	out2.clear();
	bool res1, res2 = true;
	res1 = CTimeSeries::fill_short_buffer(tpoint_sec, len, out1);
	if (next) 
		 res2 = next->CTimeSeries::fill_short_buffer(tpoint_sec, len, out2);
	return res1 & res2;
}

void CSignals::SetNextChan(const CSignals& second, bool need2makeghost)
{
	if (next == &second) return; // Check if this is valid. &second ?
	if (fs != second.GetFs() && second.nSamples > 0 && nSamples > 0)
	{
		// tseq and scalar can be mixed--i.e., if neither of this nor second is tseq throw
		if (!ISTSEQ(type()) && !ISTSEQ(second.type()))
		{
			char errstr[256];
			sprintf(errstr, "SetNextChan attempted on different fs: fs1=%d, fs2=%d", GetFs(), second.GetFs());
			throw errstr;
		}
	}
	if (next) {
			delete next;
		next = NULL;
	}
	next = new CSignals;
	if (need2makeghost)
		*next <= second;
	else
		*next = second;
}

CSignals& CSignals::Reset(int fs2set)	// Empty all data fields - sets nSamples to 0.
{
	CTimeSeries::Reset(fs2set);
	if (next) {
		if (!next->ghost)
			delete next;
		next = NULL;
	}
	return *this;
}

void CSignals::SetValue(auxtype v)
{
	Reset(1);
	body::SetValue(v);
}

void CSignals::SetValue(complex<auxtype> v)
{
	Reset(1);
	body::SetValue(v);
}

CSignals CSignals::evoke_getval(auxtype (CSignal::*fp)(unsigned int, unsigned int, void *) const, void *popt)
{
	CSignals newout = CTimeSeries::evoke_getval(fp, popt);
	if (next)
		newout.SetNextChan(next->evoke_getval(fp, popt));
	return newout;
}

CSignals& CSignals::evoke_modsig(fmodify fp, void* pargin, void* pargout)
{
	CTimeSeries::evoke_modsig(fp, pargin, pargout);
	if (next)
		next->evoke_modsig(fp, pargin, pargout);
	return *this;
}

CSignals CSignals::evoke_modsig2(CSignal(*func) (const CSignal&, void*, void*), void* pargin, void* pargout)
{
	CSignals newout = CTimeSeries::evoke_modsig2(func, pargin, pargout);
	if (next)
		newout.SetNextChan(next->CTimeSeries::evoke_modsig2(func, pargin, pargout));
	return newout;
}

CSignals CSignals::evoke_getsig2(CSignal(*func) (auxtype*, unsigned int, void*, void*), void* pargin, void* pargout)
{
	CSignals newout = CTimeSeries::evoke_getsig2(func, pargin, pargout);
	if (next)
		newout.SetNextChan(next->CTimeSeries::evoke_getsig2(func, pargin, pargout));
	return newout;
}

CSignals CVar::evoke_getsig2(CSignal(*func) (auxtype*, unsigned int, void*, void*), void* pargin, void* pargout)
{
	CSignals newout = CSignals::evoke_getsig2(func, pargin, pargout);
	auto tp = type();
	if (ISTEMPORAL(tp) && !ISAUDIO(tp))
	{
		newout.setsnap();
		if (pargout) ((CVar*)pargout)->setsnap();
	}
	return newout;
}

CSignals CSignals::evoke_getsig(CTimeSeries(*func) (const CTimeSeries&, void*) , void * popt)
{
	CSignals newout = CTimeSeries::evoke_getsig(func, popt);
	if (next)
		newout.SetNextChan(next->evoke_getsig(func, popt));
	return newout;
}

CSignals& CSignals::NullIn(auxtype tpoint)
{
	CTimeSeries::NullIn(tpoint);
	if (next != NULL)
	{
		next->CTimeSeries::NullIn(tpoint);
	}
	return *this;
}


CSignals& CSignals::operator+=(auxtype con)
{
	operate(CSignals(con), '+');
	return *this ;
}
CSignals& CSignals::operator*=(auxtype con)
{
	operate(CSignals(con), '*');
	return *this;
}
CSignals& CSignals::operator/=(auxtype con)
{
	operate(CSignals(con), '/');
	return *this;
}

const CSignals& CSignals::operator+=(CSignals *yy)
{
	CTimeSeries::operator+=(yy);
	if (next) {
		if (yy->next)
			*next += yy->next;	// stereo += stereo
		else
			*next += yy;		// stereo += mono
	}
	return *this;
}

CVar& CVar::initcell(const CVar &sec)
{
	Reset();
	cell.push_back(sec);
	return *this;
}

CVar& CVar::appendcell(const CVar &sec)
{
	auto tp = type();
	auto mask2noncell = tp & !TYPEBIT_CELL;
	if (mask2noncell)
		throw "attempting to add a cell member to a non-cell variable.";
	cell.push_back(sec);
	return *this;
}

CVar& CVar::setcell(unsigned int id, const CVar &sec)
{ // id one-based index
	if (!(type() & TYPEBIT_CELL))
		throw "cannot add a cell member to a non-cell variable.";
	if (id < cell.size() + 1)
		cell[id - 1] = sec; //replace existing cell 
	else
	{
		CVar tp;
		for (size_t k = cell.size(); k < id - 1; k++)
			cell.push_back(tp);
		appendcell(sec);
	}
	return *this;
}

CVar& CVar::bringnext()
{
	if (!next) Reset();
	else
	{
		CTimeSeries::Reset();
		*this <= next; // ghost copy
		next = NULL;
	}
	return *this;
}


CSignals& CSignals::reciprocal(void)
{
	CTimeSeries::reciprocal();
	if (next)	next->reciprocal();
	return *this;
}

CSignals& CSignals::operator>>=(auxtype delta)
{
	CTimeSeries::operator>>=(delta);
	if (next)	*next >>= delta;
	return *this;
}

CSignals& CSignals::Crop(const CSignals& timepoints)
{
	CTimeSeries::Crop(timepoints.buf[0], timepoints.buf[1]);
	if (next)		next->CTimeSeries::Crop(timepoints.next->buf[0], timepoints.next->buf[1]);
	return *this;
}

CSignals& CSignals::ReplaceBetweenTPs(const CSignals &newsig, const CSignals& timepoints)
{
	CTimeSeries::ReplaceBetweenTPs(newsig, timepoints.buf[0], timepoints.buf[1]);
	if (next) {
		if (newsig.next)
			next->CTimeSeries::ReplaceBetweenTPs(*newsig.next, timepoints.next->buf[0], timepoints.next->buf[1]);
		else
			next->CTimeSeries::ReplaceBetweenTPs(newsig, timepoints.next->buf[0], timepoints.next->buf[1]);
	}
	return *this;
}

CSignals &CSignals::transpose1()
{
	CTimeSeries::transpose1();
	if (next)
		next->transpose1();
	return *this;
}

CSignals& CSignals::LogOp(CSignals &rhs, int type)
{
	CTimeSeries::LogOp(rhs, type);
	if (next) next->LogOp(rhs, type);
	return *this;
}

auxtype CSignals::MakeChainless()
{
	if (next != NULL && nSamples == 0)
	{
		UpdateBuffer(next->nSamples);
		fs = next->GetFs();
	}
	CTimeSeries::MakeChainless();
	if (next != NULL)
	{
		next->MakeChainless();
		int diff = nSamples - next->nSamples;
		CSignals silenc(fs);
		if (diff > 0) // next is shorter (next needs padding)
			next->UpdateBuffer(nSamples);
		else
			UpdateBuffer(next->nSamples);
	}
	return alldur();
}

CSignals &CSignals::each(auxtype(*fn)(auxtype))
{
	CTimeSeries::each(fn);
	if (next) 	next->each(fn);
	return *this;
}

CSignals& CSignals::each_allownegative(auxtype(*fn)(auxtype))
{
	CTimeSeries::each_allownegative(fn);
	if (next) 	next->each(fn);
	return *this;
}

CSignals &CSignals::each(complex<auxtype>(*fn)(complex<auxtype>))
{
	CTimeSeries::each(fn);
	if (next) 	next->each(fn);
	return *this;
}

CSignals &CSignals::each(auxtype(*fn)(complex<auxtype>))
{
	CTimeSeries::each(fn);
	if (next) 	next->each(fn);
	return *this;
}

#ifdef _WINDOWS


#endif

#ifndef NO_FFTW

#endif

// CSignals GetType() returning CSIG_TSERIES in the old code means
// For stereo, nSamples = 1 and next->nSamples == 1
// For mono, the saem as CTimeSeries::GetType()
bool CSignals::stereo_scalar() const
{
	if (next && nSamples == 1 && next->nSamples == 1)
		return true;
	if (nSamples > 0 || !next)
		return CTimeSeries::chained_scalar();
	else
		return false;
}

int CSignals::ReadAXL(FILE* fp, bool logical, char *errstr)
{
	unsigned int nChains, check = sizeof(nSamples);
	size_t res;
	int res2;
	res = fread((void*)&fs, sizeof(fs), 1, fp);
	res2 = ftell(fp);
	Reset(fs);
	res = fread((void*)&nChains, sizeof(nChains), 1, fp);
	res2 = ftell(fp);
	fseek(fp, 0, SEEK_END);
	unsigned int endp = ftell(fp);
	fseek(fp, res2, SEEK_SET);
	for (unsigned int k = 0; k < nChains; k++)
	{
		unsigned int cum(0), _nSamples;
		CTimeSeries readsig(fs);
		res = fread((void*)&_nSamples, sizeof(nSamples), 1, fp); // readsig.nSamples shouldn't be directly modified, it should be done inside UpdateBuffer()
		res2 = ftell(fp);
		readsig.UpdateBuffer(_nSamples);
		if (logical) readsig.MakeLogical();
		res = fread((void*)&readsig.tmark, sizeof(tmark), 1, fp);
		res2 = ftell(fp);
		while (cum < readsig.nSamples)
		{
			res = fread((void*)&readsig.logbuf[cum], readsig.bufBlockSize, readsig.nSamples - cum, fp);
			cum += (int)res;
			if (!res)
			{
				res2 = ftell(fp);
				if (res2 == endp)
				{
					sprintf(errstr, "expecting %d bytes, data terminated at %d bytes", (int)readsig.nSamples, cum);
					readsig.nSamples = cum;
					break;
				}
			}
		}
		AddChain(readsig);
		res2 = ftell(fp);
	}
	MergeChains();
	errstr[0] = 0;
	return 1;
}

int CSignals::WriteAXL(FILE* fp)
{
	int res = CTimeSeries::WriteAXL(fp);
	if (next)
		res += next->CTimeSeries::WriteAXL(fp);
	return res;
}

bool CVar::IsGO() const
{ //Is this Graphic Object?
	if (fs == 3) return true;
	if (strut.empty()) return false;
	if (strut.find("type") == strut.end()) return false;
	if (strut.find("color") == strut.end()) return false;
	if (strut.find("userdata") == strut.end()) return false;
	if (strut.find("tag") == strut.end()) return false;
	if (strut.find("visible") == strut.end()) return false;
	//if (strut.find("parent") == strut.end()) return false;
	//if (strut.find("children") == strut.end()) return false;

	// Add rejection for other handles (e.g., audio handle)
	return true;
}

CVar&  CVar::length()
{
	if (IsGO())
	{
		if (fs == 3) SetValue((auxtype)nSamples);
		else SetValue(1.);
	}
//	else // checkcheckcheck
//		evoke_getval(&CSignal::length);
	return *this;
}


CVar::CVar()
{
}

CVar::~CVar()
{
}

CVar::CVar(const CSignals& src)
	:functionEvalRes(false)
{
	*this = src;
}

CVar::CVar(const CVar& src)
	: functionEvalRes(false)
{
	*this = src;
}
CVar::CVar(CVar * src)
: functionEvalRes(false)
{
	*this = *src;
}

CVar& CVar::Reset(int fs2set)
{ // calling Reset for a CVar object will erase cell, strut and struts members
	CSignals::Reset(fs2set);
	cell.clear();
	strut.clear();
	struts.clear();
	return *this;
}

void CVar::set_class_head(const CSignals & rhs)
{
	CSignals::operator=(rhs);
}

int CSignals::getBufferLength(double& lasttp, double& lasttp_with_silence, double blockDur) const
{
	double nullportion = tmark;
	if (nullportion >= blockDur)
	{
		if (!next || next->tmark >= blockDur)
		{
			if (next && next->tmark < nullportion) nullportion = next->tmark;
			lasttp = lasttp_with_silence = 0.;
			return (int)round(nullportion / 1000. * fs);
		}
	}
	const CSignals* p = this;
	multimap<double, int> timepoints;
	for (int k = 0; p && k < 2; k++, p = (CSignals*)p->next)
	{
		for (const CTimeSeries* q = p; q; q = q->chain)
		{
			if (q->IsEmpty()) continue;
			timepoints.insert(pair<double, int>(q->tmark, 1));
			timepoints.insert(pair<double, int>(q->CSignal::endt(), -1));
		}
	}
	auto it = timepoints.begin();
	for (int sum = 0; it != timepoints.end(); it++)
	{
		sum += it->second;
		if (sum == 0)
		{
			//			auto jt = it;
			//			if (++jt == timepoints.end())
			break;
		}
	}
	lasttp = it->first;
	lasttp_with_silence = lasttp;
	if (++it != timepoints.end())
		lasttp_with_silence = it->first;
	// If the null portion is long enough, treat it as a separate, blank audio block in the subsequent call
	if (lasttp_with_silence - lasttp >= blockDur)
		lasttp_with_silence = lasttp;
	return (int)round(lasttp_with_silence / 1000. * fs);
}

void CSignals::nextCSignals(double lasttp, double lasttp_with_silence, CSignals& ghcopy)
{
	CSignals* p = &ghcopy;
	CTimeSeries* q, * q1 = NULL;
	CSignals* q2 = NULL;
	for (int k = 0; p && k < 2; k++, p = p->next)
	{
		for (q = p; q; q = q->chain)
		{
			if (q->tmark >= lasttp)
			{
				q->tmark -= lasttp_with_silence;
				CSignals* tempNext = p->next;
				if (k == 0)
				{
					p->next = tempNext;
					q1 = q;
				}
				else
				{
					p->next = nullptr;
					q2 = (CSignals*)q;
				}
				break;
			}
		}
	}
	if (q1)
		ghcopy = *q1;
	if (q2) // what's the point of this? q2, if available, should be the same as next at this point. 5/24/2020
		ghcopy.SetNextChan(*q2, true);
	if (!q1 && !q2)
		ghcopy.Reset();
}

static auxtype RMS_concatenated(const CTimeSeries& sig)
{
	// Compute the "overall" RMS of entire chain as if all chains were concatenated.
	// input sig represents RMS value of each chain (nSamples of each chain is 1)
	// nGroups information is ignored.
	auto cum = 0.;
	unsigned int len = 0;
	for (auto p = &sig; p; p = p->chain)
	{
		cum += pow(10, (p->value() - 3.0103) / 10.) * p->nSamples;
		len += p->nSamples;
	}
	return 10.f * log10f(cum / len) + 3.0103f;
}

CSignal __rms(auxtype* buf, unsigned int len, void* pargin, void* pargout); // from rmsandothers.cpp

CSignals& CSignals::RMS()
{ // calculating the RMS of the entire CSignals as if all chain's were concatenated.
	// CAUTION--This function will replace the existing data with computed RMS.
	CSignals rmsComputed = evoke_getsig2(__rms, (void*)&fs);
	// at this point rmsComputed is chain'ed with next (also possibly chain'ed) and nSamples = 1 for each of them 
	CSignals out(1);
	auxtype rmsnow;
	rmsnow = RMS_concatenated(rmsComputed);
	out.SetValue(rmsnow);
	if (rmsComputed.next)
	{
		out.SetNextChan(CSignals(RMS_concatenated(*rmsComputed.next)));
	}
	return *this = out;
}
