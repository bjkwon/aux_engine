#include "AuxScope.h"
#include "AuxScope_exception.h"

AuxScope_exception& AuxScope_exception::raise()
{
	addLineCol();
	outstr = msgonly + " " + sourceloc;
	return *this;
}

void AuxScope_exception::addLineCol()
{
	ostringstream oss;
	if (pCtx->level == pCtx->baselevel.back()) {
		if (line != 1 || col != 1) {
			oss << "line " << line << ", col " << col;
			sourceloc = oss.str();
		}
		return;
	}
	vector<int> lines;
	vector<string> strs;
	const AuxScope* tp = pCtx;
	char* pstr = NULL;
	while (tp)
	{
		if (!tp->u.base.empty())
		{
			if (tp->u.base == tp->u.title) // base udf (or "other" udf)
				strs.push_back(tp->pEnv->udf[tp->u.base].fullname);
			else
				strs.push_back(string("function \"") + tp->u.title + "\"");
			if (tp->pLast)
				lines.push_back(tp->pLast->line);
		}
		tp = tp->dad;
	}
	if (!strs.empty())
	{
		udffile = strs.front();
		vector<string>::iterator it2 = strs.begin();
		//at this point strs can have more items than lines and cols, because son->son is son during CallUDF()
		//so don't use strs iterator for for 
		for (vector<int>::iterator it = lines.begin(); it != lines.end(); it++, it2++)
		{
			oss << '\n' << "line " << *it;
			if (it == lines.begin()) oss << ", col " << pnode->col;
			oss << " in " << *it2;
		}
	}
	sourceloc = oss.str();
}
