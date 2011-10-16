#pragma once

class CSMClient
{
public:
	CSMClient(void);
	~CSMClient(void);
	void SendAITick(bool b = false);
	void ExitOk();
	void InitOk();
	static void		ReleaseSM();
	void	SendRefreshInfo();
	bool	LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
	void	ProcessMessage();
private:
	static CSMClient* g_SMInstance;
	bool	m_bMapFile;
public:
	static CSMClient* GetSM();

};