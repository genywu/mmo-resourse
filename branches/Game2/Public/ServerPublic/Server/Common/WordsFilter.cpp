
#include "StdAfx.h"
#include <Windows.h>
#include "WordsFilter.h"

bool IsCursorInHz(long cursorPos, string strText)
{
	if( cursorPos>0 && cursorPos<=(long)strText.length() && (uchar)strText[cursorPos-1]>127 )
	{
		int num=0;
		int i;
		for(i=cursorPos-1;i>=0;i--)
		{
			if( (uchar)strText[i]<127)
				break;
			num++;
		}
		// 汉字中间,如果为奇数，则在汉字中间
		if( num % 2 == 1 )
			return true;
	}
	return false;
}

int FindSubString(string& str, string& strSub)
{
	int iPos = 0;
	int iSize = (int)str.size() - (int)strSub.size();
	while (iPos <= iSize)
	{
		if(!IsCursorInHz(iPos, str))
			if (strnicmp(str.c_str()+ iPos, strSub.c_str(), strSub.size()) == 0)
				return iPos;
		iPos ++;
	}

	return -1;
}



CWordsFilter* CWordsFilter::instance = NULL;

CWordsFilter::CWordsFilter()
{
	m_lstFilter.clear();
}

bool	CWordsFilter::Initial(string& strFileName)
{
	m_strFilterFileName = strFileName;
	return LoadFilter(m_strFilterFileName.c_str());
}
bool CWordsFilter::LoadFilter(const char* pszFile)
{
	assert(pszFile);


	FILE* fp = fopen(pszFile, "r");
	if (NULL == fp)
		return false;

	char szTemp[1024];
	while(fgets(szTemp,1024, fp ))
	{
		szTemp[lstrlen(szTemp)-1] = '\0';
		if ( szTemp[0] != 0)
		{
			if(-1 != FindSubString(string(szTemp), string("*")))
				break;
			m_lstFilter.push_back(szTemp);
		}
	}

	return true;
}

//!		编码
bool	CWordsFilter::AddToByteArray(vector<uchar>& ByteArray)
{
	_AddToByteArray(&ByteArray, (long)m_lstFilter.size());
	for (list<string>::iterator ite = m_lstFilter.begin(); ite != m_lstFilter.end(); ++ite)
	{
		_AddToByteArray(&ByteArray, (*ite).c_str());
	}
	return true;
}

//!		解码
bool	CWordsFilter::Decode(uchar* pData, long &lPos)
{
	if (NULL == pData)
		return false;
	
	long lWordsNum = _GetLongFromByteArray(pData, lPos);
	m_lstFilter.clear();
	for (long i = 0; i < lWordsNum; ++i)
	{
		char szTmp[1024] = {0};
		_GetStringFromByteArray(pData, lPos, szTmp);
		if(-1 != FindSubString(string((char*)pData), string("*")))
			break;
		m_lstFilter.push_back(string(szTmp));
	}
	
	return true;
}

bool	CWordsFilter::ReloadFilter()
{
	m_lstFilter.clear();
	return LoadFilter(m_strFilterFileName.c_str());
}



void CWordsFilter::AddWords(const char* pszWords)
{
	assert(pszWords);
	if(-1 != FindSubString(string(pszWords), string("*")))
		return;
	m_lstFilter.push_back(pszWords);
}



bool CWordsFilter::DelWords(const char* pszWords)
{
	assert(pszWords);

	list<string>::iterator iterBeg = m_lstFilter.begin();
	list<string>::iterator iterEnd = m_lstFilter.end();

	for (; iterBeg != iterEnd; ++ iterBeg)
	{
		if ((*iterBeg) == pszWords)
		{
			m_lstFilter.erase(iterBeg);
			return true;
		}
	}

	return false;
}

CWordsFilter* CWordsFilter::GetInstance()
{
	if(instance==NULL)
	{
		instance = new CWordsFilter();
	}
	return instance;
}
void CWordsFilter::Release()
{
	SAFE_DELETE(instance);
}

bool CWordsFilter::Check(string& str,bool bReplace)
{
	list<string>::iterator iterBeg = m_lstFilter.begin();
	list<string>::iterator iterEnd = m_lstFilter.end();

	bool bReturn = true;
	for (; iterBeg != iterEnd; ++ iterBeg)
	{
		if (str.find((*iterBeg)) != string::npos)
		{ 
			//if(bReplace==false)
			//{
				bReturn = false;
				break;
			//}
		}
	}
	return bReturn;

}




bool CWordsFilter::Check(string& str,bool bReplace , bool bCaseSensitive)
{
	list<string>::iterator	iterBeg = m_lstFilter.begin();
	list<string>::iterator	iterEnd = m_lstFilter.end();

	bool bPass = true;
	string strReplace;
	int iPos = -1;
	string strTemp;

	for (; iterBeg != iterEnd; ++iterBeg)
	{
		strTemp = (*iterBeg);
		iPos = FindSubString(str, strTemp );
		while (iPos  != -1)
		{
			if (bReplace==false)
				return false;

			bPass = false;
			strReplace.clear();
			for (int i = 0; i < (int)iterBeg->size(); ++i)
				strReplace += "*";
			str.replace(iPos, iterBeg->size(), strReplace.c_str());
			iPos = FindSubString(str, *iterBeg );
		}
	}

	return bPass;
}

