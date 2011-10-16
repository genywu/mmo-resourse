#include "stdafx.h"

#include "futility.h"
#include "fconsole.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SINGLETON(console::Manager);

//////////////////////////////////////
//CLASS___(console::Logfile)__________
void console::Logfile::Create()
{
	struct _finddata_t c_file;
	intptr_t hFile;
	bool bExist = ((hFile = _findfirst( "log", &c_file )) != -1L );

	if (!bExist)	
		CreateDirectory("log",NULL);

	static SYSTEMTIME st;
	GetLocalTime(&st);
	static char szLogFileName[_MAX_PATH];

	_snprintf(szLogFileName,256,"log\\%d_%d_%d[%2.2d_%2.2d_%2.2d].html",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
    	
	int iNeedLen = GetCurrentDirectory(0,NULL);

	m_pszLogFileName = new char [strlen(szLogFileName)+ 1];
	//GetCurrentDirectory(iNeedLen,m_pszLogFileName);
	strcpy(m_pszLogFileName,szLogFileName);

	FILE *fp = fopen(m_pszLogFileName,"wb");	
	if (fp) 
	{
		//write header
		fprintf(fp,"<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n");
		fprintf(fp,"<meta name=\"GENERATOR\" content=\"Frost Engine Log Html File\">\n");
		fprintf(fp,"<style type=\"text/css\">\n");
		//fprintf(fp,"A:link {COLOR: #808080; FONT-SIZE: 9pt; TEXT-DECORATION: none}\n");
		//fprintf(fp,"A:visited {COLOR: #a0a0a0; FONT-SIZE: 9pt; TEXT-DECORATION: none}\n");
		fprintf(fp,"</style><title>Frost Engine Log</title></head>\n");
		fprintf(fp,"<table width= 800 bgcolor=\"#dfdfe5\" cellspacing=\"0\" ><TR><td  bgcolor=\"#dfdfe5\" align=\"middle\" valign=\"center\"><FONT face=\"Times New Roman\" color=#7f9c8b size=6><STRONG>Frost Engine Log</STRONG></FONT></td></TR></table>\n");
		fprintf(fp,"<pre>");		
		fprintf(fp,"<p><FONT color=\"#7f9c8b\"><strong>BEGIN</strong></FONT></p>");
		fprintf(fp,"<HR COLOR=\"#dfdfe5\">");
		fclose(fp);
	}
}
void console::Logfile::Destroy()
{
//open file
	FILE *fp = fopen(m_pszLogFileName,"ab");	
	if (fp) 
	{
		//write header
		fprintf(fp,"<HR COLOR=\"#dfdfe5\">");
		fprintf(fp,"<p><FONT color=\"#7f9c8b\"><strong>END</strong></font></p>");
		fprintf(fp,"</pre>");
		fprintf(fp,"</html>");
		fclose(fp);
	}	

	SAFEDELETE(m_pszLogFileName);
}
void console::Logfile::Output(const char *pszString)
{
	FILE *fp = fopen(m_pszLogFileName,"ab");	
	if (!fp) return;
	fprintf(fp,pszString);
	fclose(fp);
}


////////////////////////////////////////
//CLASS___(console::Manager)___________
//控制台管理器
void console::Manager::Outpute(const void *szFormat,...)
{
	//输出信息到ConsoleWnd  + LogFile + Output Wnd
	static char szBuffer0[2048],szBuffer1[2048];
	if (m_dwFilter & CONSOLE_ERROR)
	{
		va_list vapt;
		va_start(vapt,szFormat);
		_vsnprintf((char*)szBuffer0,2047,(const char*)szFormat,vapt);	
		va_end(vapt);

		//Graphices CONSOLE Wnd
		//RemoveHtmlTag(szBuffer1,szBuffer0);
		//FConsole::GetInstance()->Output(DEBUG_OUTPUT_COLOR_INFO_DW,szTextBuffer);
		
		//LOGFILE
		strcpy(szBuffer1,"<FONT color=\"#FF0000\"><strong>Error: </strong></font>");
		strcat(szBuffer1,szBuffer0);
		m_pLogfile->Output(szBuffer1);

		//Win32 Output Wnd
		RemoveHtmlTag(szBuffer1,szBuffer0);
		strcpy(szBuffer0,"ERROR: ");
		strcat(szBuffer0,szBuffer1);		
		::OutputDebugString(szBuffer0);
	}
}


console::Manager::Manager(void)
{
	m_dwFilter = 0xffff;
}

console::Manager::~Manager(void)
{
}

void console::Manager::Create(DWORD dwFilter)
{
	m_dwFilter = dwFilter;
	m_pLogfile = new Logfile;
	m_pLogfile->Create();
}

void console::Manager::Destroy()
{
	SAFEDESTROY(m_pLogfile);
}


void console::Manager::RemoveHtmlTag(char *pszText,char *pszHtml)
{
	//Remove Html Tag <>之间内容
	int i = 0,c = 0,n = 0;
	int iLen = lstrlen(pszHtml);
	while(i < iLen)
	{
		c = pszHtml[i++];
		if (c == '<')
		{
			while( ((c = pszHtml[i++]) != '>') && i < iLen);
		}
		else
			pszText[n++] = c;
	}
	pszText[n] = '\0';
}

void console::Manager::Output(const void *szString,...)
{
	//输出信息到ConsoleWnd  + LogFile + Output Wnd
	static char szBuffer0[2048],szBuffer1[2048];
	if (m_dwFilter & CONSOLE_OUTPUT)
	{
		va_list vapt;
		va_start(vapt,szString);
		_vsnprintf((char*)szBuffer0,2047,(const char*)szString,vapt);	
		va_end(vapt);

		//Graphices CONSOLE Wnd
		//	RemoveHtmlTag(szBuffer1,szBuffer0);
		//FConsole::GetInstance()->Output(DEBUG_OUTPUT_COLOR_INFO_DW,szTextBuffer);

		//LOGFILE
		m_pLogfile->Output(szBuffer0);
		//Win32 Output Wnd
		RemoveHtmlTag(szBuffer1,szBuffer0);
		::OutputDebugString(szBuffer1);
	}
}


void console::Manager::Outputw(const void *szString,...)
{
	if (m_dwFilter & CONSOLE_WARRING)
	{
		//输出信息到ConsoleWnd  + LogFile + Output Wnd
		static char szBuffer0[2048],szBuffer1[2048];

		va_list vapt;
		va_start(vapt,szString);
		_vsnprintf((char*)szBuffer0,2047,(const char*)szString,vapt);	
		va_end(vapt);

		//Graphices CONSOLE Wnd
		//RemoveHtmlTag(szBuffer1,szBuffer0);
		//FConsole::GetInstance()->Output(DEBUG_OUTPUT_COLOR_INFO_DW,szTextBuffer);
		
		//LOGFILE
		strcpy(szBuffer1,"<FONT color=\"#ff00ff\"><strong>Waring: </strong></font>");
		strcat(szBuffer1,szBuffer0);
		m_pLogfile->Output(szBuffer1);

		//Win32 Output Wnd
		RemoveHtmlTag(szBuffer1,szBuffer0);
		strcpy(szBuffer0,"Warring: ");
		strcat(szBuffer0,szBuffer1);		
		::OutputDebugString(szBuffer0);
	}
}