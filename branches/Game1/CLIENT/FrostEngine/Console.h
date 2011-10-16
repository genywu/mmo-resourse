/*

            ○○○○○○
			 ○                                      ○
			○○○    ○○ ○     ○○    ○○○  ○○○
		   ○  ○      ○○    ○    ○  ○        ○
		  ○          ○      ○    ○     ○     ○
	  ○○○      ○○○       ○○    ○○○    ○○

			"FROST" 3D GRAPHICS RENDERING ENGINE.

					 WRITTEN BY JIATAO.
--------------------------------------------------------------------
					COPYRIGHT 2002-2003. 

描述: 控制台模块
修改: 2003/10/31
*/


#pragma once

class console											  
{
	
public:
	enum OUTPUTFILTER
	{
		CONSOLE_ERROR   = 0x0001,
        CONSOLE_WARRING = 0x0002,
        CONSOLE_OUTPUT  = 0x0004
	};

	class Logfile
	{
		//
		char *m_pszLogFileName;

	public:
		const char* GetName()	{return m_pszLogFileName;}
		void Create();
		void Destroy();
		void Output(const char *pszString);
	};

	class Manager
	{
		DECLARE_SINGLETON(Manager);

		DWORD m_dwFilter;

		Logfile * m_pLogfile;

	public:
		Manager(void);
		~Manager(void);

		const char* GetLogFileName()
		{
			if(m_pLogfile)
				return m_pLogfile->GetName();
			else
				return NULL;
		}
		void Create(DWORD dwFilter);
		void Destroy();

		void RemoveHtmlTag(char *pszText,char *pszHtml);

		//void OutputError(const void *szString , ...);
		void Output(const void *szString , ...);		
		void Outpute(const void *szString , ...);
		void Outputw(const void *szString , ...);
	};
	
};
