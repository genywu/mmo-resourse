#pragma once

class CCmdLine
{
public:
	CCmdLine(void);
	~CCmdLine(void);
private:
	deque<string>m_qCmdLines;
	CRITICAL_SECTION m_CSection;
	static CCmdLine*		 m_Instance;
public:
	bool Init();
	bool Release();
	bool InsertCmd(const char* cmd);
	bool GetAllCmd(deque<string>& dequecmd);
public:
	static CCmdLine* GetInstance();
};
