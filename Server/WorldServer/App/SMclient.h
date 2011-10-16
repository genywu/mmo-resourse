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
	void	ReloadFileNotify(char* szfile,BYTE bSuccess);
	void	SendInfo(char* szInfo,...);
	void ProcessMessage();
	bool LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
private:
	static CSMClient* g_SMInstance;
	bool m_bMapFile;
public:
	static CSMClient* GetSM();
	
};
