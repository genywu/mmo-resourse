#pragma once

#ifndef	__CClientWordsFilter__
#define __CClientWordsFilter__

#include <string>
#include <list>
#include <sstream>

using namespace std;

class CClientWordsFilter
{
private:
	CClientWordsFilter();
	~CClientWordsFilter() {}
public:

	bool	LoadFilter(const char* pszFile);

	bool	IsCursorInHz(long cursorPos, string strText);
	int		FindSubString(string& str, string& strSub);

	bool	Check(string& str,bool bReplace=false);
	bool	Check(string& str,bool bReplace , bool bCaseSensitive);

	bool	Initial(string& strFileName);

	static CClientWordsFilter* instance;
	static CClientWordsFilter* GetInstance();
	static void Release();

private:
	string			m_strFilterFileName;
	list<string>	m_lstFilter;

};

#endif	//__WORDS_FILTER__