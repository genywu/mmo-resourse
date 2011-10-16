#pragma once

class CDataBlockAllocator;
class CSMClient
{
public:
	CSMClient(void);
	~CSMClient(void);
	void SendAITick(bool b = false);
	void SendRefreshInfo();
	void ExitOk();
	void InitOk();
	static void		ReleaseSM();
	bool LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
	void ProcessMessage();
private:
	static CSMClient* g_SMInstance;
	bool m_bMapFile;//是否已打开内存映射文件
public:
	static CSMClient* GetSM();

};
