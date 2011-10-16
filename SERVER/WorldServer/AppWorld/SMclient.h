#pragma once
class CPlayer;
typedef struct tagReportedPlayer:public tagBaseResultInfo
{
	CGUID _OptGuid;//操作GUID,返回给SCC
}REPORTPLAYER;
class CSMClient
{
public:
	CSMClient(void);
	~CSMClient(void);
	void SendAITick(bool b = false);
	void ExitOk();
	void InitOk();
	static void	ReleaseSM();
	void	SendRefreshInfo();
	void	ReloadFileNotify(char* szfile,BYTE bSuccess);
	void	SendInfo(char* szInfo,...);
	void	ProcessMessage();
	bool	LoadMapFileSetup(CDataBlockAllocator *pDBAllocator);
	void	SendReportInfo(CPlayer* pPlayer,CPlayer* pObjPlayer,WORD wType);
	bool	BanPlayer(tagBaseResultInfo& tgInfo,CGUID& guid);
	bool	OnProcessBanPlayer(const char* name,bool bSucc=true);
	//参数:处理成功标志,返回给SCC的操作GUID
	void	SendBanResult(BYTE bSucc,CGUID& guid);
	void	GetBroadCastInfo(REPORTPLAYER& tgInfo,char* szInfo);
	void	SetWatchedPlayer(const CGUID& guid, const char* name, long GsSocket)
	{m_WatchedGuid=guid; strcpy(m_WatchedName,name);m_WatchedGsSocket=GsSocket;}
	CGUID&	GetWatchedGuid(){return m_WatchedGuid;}
	const	char* GetWatchedName(){if(0 != m_WatchedName[0])return m_WatchedName;return NULL;}
	long	GetWatchedGsSocket(){return m_WatchedGsSocket;}
	void	ClearWatch(void){m_WatchedGuid == NULL_GUID; memset(m_WatchedName, 0, 32); m_WatchedGsSocket=0;}
private:
	static CSMClient* g_SMInstance;
	bool m_bMapFile;
	map<string,REPORTPLAYER>mapBanPlayer;
	CGUID m_WatchedGuid;//被监视玩家的GUID
	char  m_WatchedName[32];//被监视玩家的姓名
	long m_WatchedGsSocket;//被监视玩家所在的GsSocket
public:
	static CSMClient* GetSM();
	
};
