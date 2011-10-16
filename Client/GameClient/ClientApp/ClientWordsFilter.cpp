#include "StdAfx.h"
#include ".\clientwordsfilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




CClientWordsFilter* CClientWordsFilter::instance = NULL;

CClientWordsFilter::CClientWordsFilter()
{
	m_lstFilter.clear();
}

bool CClientWordsFilter::IsCursorInHz(long cursorPos, string strText)
{
	if( cursorPos>0 && cursorPos<=(long)strText.length() && (unsigned char)strText[cursorPos-1]>127 )
	{
		int num=0;
		int i;
		for(i=cursorPos-1;i>=0;i--)
		{
			if( (unsigned char)strText[i]<127)
				break;
			num++;
		}
		// 汉字中间,如果为奇数，则在汉字中间
		if( num % 2 == 1 )
			return true;
	}
	return false;
}

bool CClientWordsFilter::Initial(string& strFileName)
{
	m_strFilterFileName = strFileName;
	return LoadFilter(m_strFilterFileName.c_str());
}


bool CClientWordsFilter::LoadFilter(const char* pszFile)
{
	assert(pszFile);

	CRFile *pFile;
	stringstream ssTemp;
	char szTemp[1024];
	pFile = rfOpen(pszFile);

	if( !pFile ) return false;

	pFile->ReadToStream(ssTemp);

	while( ssTemp.getline(szTemp, 1024) )
	{
		szTemp[lstrlen(szTemp)-1] = '\0';
		m_lstFilter.push_back(szTemp);
	}
	rfClose(pFile);
	return true;
}

CClientWordsFilter* CClientWordsFilter::GetInstance()
{
	if(instance==NULL)
	{
		instance = new CClientWordsFilter();
	}
	return instance;
}
void CClientWordsFilter::Release()
{
	delete instance;
	instance = NULL;
}

bool CClientWordsFilter::Check(string& str,bool bReplace)
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

int CClientWordsFilter::FindSubString(string& str, string& strSub)
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


bool CClientWordsFilter::Check(string& str,bool bReplace , bool bCaseSensitive)
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

