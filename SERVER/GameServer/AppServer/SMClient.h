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
	bool LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
	void ProcessMessage();
	void ChangeWatchedPlayer(const char* name,BOOL bDel);
	void SendWatchResult(const char* name,BYTE byType,char* szInfo="");
	bool FindWatchedPlayer(const char* name);
	bool IsSendTo(BYTE byType);//是否要发出此次信息
private:
	static CSMClient* g_SMInstance;
	bool m_bMapFile;
	std::set<string>WatchedSet;//被监视玩家列表
public:
	static CSMClient* GetSM();

};
