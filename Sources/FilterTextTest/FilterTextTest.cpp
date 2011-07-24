// FilterTextTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "iostream.h"
#include "fstream.h"
#include "..\FilterText\FilterText.h"
#include <Shellapi.h>

typedef HRESULT (*LPFNCREATETEXTFILTER)(ITextFilter** ppTextFilter);
ITextFilter* pTextFilter;

void AddFilterWords(char *szFilterWordFile)
{
	ifstream ifs(szFilterWordFile, ios::nocreate);
	if(!ifs)
	{
		cout<<"Can't Open FilterWordFile: "<<szFilterWordFile<<"!"<<endl;
		return;
	}
	char szLine[1024];
	while(!ifs.eof())
	{
		ifs.getline(szLine, 1023);
		if(ifs.bad())
		{
			cout<<"Read file error!"<<szFilterWordFile<<"!"<<endl;
			return;
		}
		pTextFilter->AddExpression(szLine);
	}
}

void DoTest(char *szTestFile)
{
	ifstream ifs(szTestFile, ios::nocreate);
	if(!ifs)
	{
		cout<<"Can't Open FilterTestFile: "<<szTestFile<<"!"<<endl;
		return;
	}

	ofstream ofs("result.txt");
	if(!ofs)
	{
		cout<<"Can't create ResultFile: result.txt!"<<endl;
		return;
	}

	char szLine[1024];
	while(!ifs.eof())
	{
		ifs.getline(szLine, 1023);
		if(ifs.bad())
		{
			cout<<"Read file error!"<<szTestFile<<"!"<<endl;
			return;
		}

		if(pTextFilter->IsTextPass(szLine))
		{
			ofs<<"Pass "<<szLine<<endl;
		}
		else
		{
			ofs<<"Filt "<<szLine<<endl;
		}
	}

	ShellExecute(NULL, "open", "result.txt", NULL, NULL, SW_SHOW);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"Usage: FilterTextTest 过滤信息文件 要测试的文本文件!"<<endl;
		return FALSE;
	}

	HMODULE m_hFilterText;
	LPFNCREATETEXTFILTER lpfnCreateTextFilter;

	if (!(m_hFilterText = ::LoadLibrary("FilterText.dll")))
	{
		cout<<"Can't load FilterText.dll!"<<endl;
		return FALSE;
	}

	if (!(lpfnCreateTextFilter = (LPFNCREATETEXTFILTER)GetProcAddress(m_hFilterText, "CreateTextFilter")))
	{
		FreeLibrary(m_hFilterText);
		cout<<"Can't find function CreateTextFilter!"<<endl;
		return FALSE;
	}
	
	lpfnCreateTextFilter(&pTextFilter);

	AddFilterWords(argv[1]);
	DoTest(argv[2]);

	FreeLibrary(m_hFilterText);

	return 0;
}



