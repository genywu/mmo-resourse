#ifndef	__WORDS_FILTER__
#define __WORDS_FILTER__

#include <string>
#include <list>


using namespace std;

class CWordsFilter
{
private:
	CWordsFilter();
	~CWordsFilter() {}
public:

	bool	LoadFilter(const char* pszFile);
	bool	ReloadFilter();
	void	AddWords(const char* pszWords);
	bool	DelWords(const char* pszWords);

	bool	Check(string& str,bool bReplace=false);
	bool	Check(string& str,bool bReplace , bool bCaseSensitive);
	bool	Initial(string& strFileName);

	//!		±àÂë¹Ø¼ü×Ö
	BOOL	AddToByteArray(vector<BYTE>& ByteArray);
	//!		½âÂë¹Ø¼ü×Ö
	BOOL	Decode(BYTE* pData, LONG &lPos);

	static CWordsFilter* instance;
	static CWordsFilter* GetInstance();
	static void Release();
	
private:
	string			m_strFilterFileName;
	list<string>	m_lstFilter;

};

#endif	//__WORDS_FILTER__