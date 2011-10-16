#pragma once

class CSMClient
{
public:
	CSMClient(void);
	~CSMClient(void);
	void SendAITick(bool b = false);
	void ExitOk();
	void InitOk();   
	bool LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
	static void		ReleaseSM();
	void ProcessMessage();
private:
	static CSMClient* g_SMInstance;
	bool m_bMapFile;//是否已打开内存映射文件
public:
	static CSMClient* GetSM();

};
