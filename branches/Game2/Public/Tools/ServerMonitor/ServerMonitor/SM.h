#pragma once
#include "StdAfx.h"
#include "CmdLine.h"
#include "../App/Gapp.h"
#include "../App/HardWareInfo.h"
#include "../App/SMDef.h"
#include "../net/Message.h"
#include "../../../SCMPublic/def/InfoDef.h"

class CServer;
class CMemServer;
class CDataBlockAllocator;
typedef CTimerQueue<CGapp*> GappTimer;
enum eNetFlag
{
	NF_SCC_Client=0x001,
	NF_SM_Client=0x010,
	NF_DBServer_Client=0x100,// [AHC 080809 DBSMSG]
	NF_BillingServer_Client=0x1000,
	NF_LogServer_Client = 0x10000,
};

class CSM
{
public:
	CSM(void);
	~CSM(void);
	
	struct tagSetup
	{
		//long	lUseLogServer;		// 是否使用LogServer；0=不使用，1=使用
		//string	strLogIP;			// LogServerIP
	//	DWORD	dwLogPort;			// LogServerPort
	//	string  strErrFile;			// 错误文件路径+文件名
	//	string  strLogFile;			// 本地日志文件路径+文件名（SQL格式）
	//	string  strTxtLogFile;		// 本地日志文件路径+文件名（明文格式）
	//	DWORD	dClientNo;			// 客户端编号
		//long	lUseBillingServer;		//是否连接BillingServers
		

		//string	strLoginIP;			// 登录IP
		//long	dwLoginPort;		// 登录PORT
		//DWORD	dwLoginPassword;	//登录SCC密码
		long	lSwitch;			//开关,表示是否自动连接SCC

		long	lMaxBlockConNum;
		long	lValidDelayRecDataTime;

		DWORD	dwDataBlockNum;			//数据块数量
		DWORD	dwDataBlockSize;		//数据块大小
		DWORD	dwFreeMsgNum;			//预分配的消息数量

		DWORD	dwFreeSockOperNum;		//网络命令操作预分配数量
		DWORD	dwFreeIOOperNum;		//完成端口上IO操作预分配数量
		long	lIOOperDataBufNum;		//默认IO操作的DataBuf数量

		bool	bCheckNet;				// 是否对网络进行检测
		DWORD	dwBanIPTime;			// 禁止IP的时间
		DWORD	dwMaxMsgLen;			// 允许传输的最大消息长度		
		long	lMaxConnectNum;			// 客户端的最大连接数量
		long	lMaxClientsNum;			// 最大的客户端发送缓冲区大小
		long	lPendingWrBufNum;		// 最大的发送IO操作Buf总大小
		long	lPendingRdBufNum;		// 最大的接受IO操作Buf总大小
		long	lMaxSendSizePerSecond;	// 向客户端每秒发送的最大字节数
		long	lMaxRecvSizePerSecond;	// 从客户端接受的每秒最大字节数
		long	lMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
		long	lConPendingWrBufNum;	// 客户端最大的发送IO操作Buf总大小
		long	lConPendingRdBufNum;	// 客户端最大的接受IO操作Buf总大小
		long	lConMaxSendSizePerSecond;//向服务器发送的每秒最大字节数
		long	lConMaxRecvSizePerSecond;//从服务器接受的每秒最大字节数
		long	lConMaxBlockedSendMsgNum;//最大阻塞的发送消息数量

		DWORD	dwRefeashInfoTime;	// 刷新一次信息的时间
		DWORD	dwSaveInfoTime;		// 保存一次信息的间隔时间
		DWORD	dwGoodsLogTime;		// 物品Log多少分钟清除一次
		bool	bWatchRunTimeInfo;	// 是否监控运行的时间信息
		DWORD	dwWatchRunTime;		// 监控间隔
		DWORD	dwMsgValidateTime;	// 消息验证时间（毫秒，0为关闭）
	};
	tagScc2SmSetup m_SCCPrama;
	//申请共享数据块
	BYTE* AllockShareDB(long lSize);
	//释放共享数据块
	void FreeShareDB(BYTE* pData);


	// SCC信息
	struct tagSCC
	{
		long			lAuto;
		DWORD			dwIndex;			// 服务器编号
		string			strIP;				// IP
		DWORD			dwPort;				//登录端口
		char			cPassward[MD5_16_PASSWORD_SIZE];		// 密码
	};

	BOOL Init();
	BOOL InitNetServer(void);			//建立网络服务器端
	BOOL InitMemServer(void);			//初始化内存映射文件,和GAPP通信
	BOOL InitSccPrama(CMessage* pMsg=NULL);				//用SCC发来的数据初始化
	BOOL LoadSccSetup(const char *filename);
	BOOL LoadSetup();
	BOOL MainLoop(void);
	BOOL ProcessMessage();
	BOOL Release();
	BOOL OnSCCClose();
	tagSCC* GetSCC(DWORD dwIndex){return (s_mapSCC.find(dwIndex)!=s_mapSCC.end())?&s_mapSCC[dwIndex]:NULL;}
	tagSCC* GetSCC(const char* strIP,const char* strPass);			// 获取GameServer的数据
	CGapp*	FindGapp(DWORD dwID){return (m_mapGappInfo.find(dwID) != m_mapGappInfo.end()) ? m_mapGappInfo[dwID] : NULL;}
	bool	IsSameProcID(DWORD dwProcID);//GAPP的执行文件名字一样,枚举时检查是否是同一进程
	DWORD	GetGappNumByType(eGappType eType);
	bool 	UpdateGappByType(DWORD dwType,DBReadSet& dbreadset);
	unsigned	 FindGappThreadID(DWORD dwID);
	tagSetup*	 GetSetup()	{return &m_Setup;}
	tagScc2SmSetup*	GetSccPrama(){return &m_SCCPrama;}
	BOOL		 StartAccept();
	BOOL		 LoadGappInfo(const char* filename);
	BOOL		 InitMemUsageName();
	BOOL		 AddGapp(CGapp* pGapp);
	const char* GetMemUsageFileName()	{return m_pszMemUsageFile;}
	void SendGappInfo();
	void SendHardWareInfo();
	void DelayTest();
	BOOL CheckGappVersion(long lGappID = -1);
	long GetGappSize()	{return (long)m_mapGappInfo.size();}
	bool ConnScc();
	//提供给个部输入命令用
	BOOL ShutDown(long id);							//关闭GAPP
	BOOL Start(long id);							//开启GAPP
	BOOL ShowGapp();
	BOOL GetPackRes(long id);						//从SCC上获取GAPP原始包
	void SendError(const char* Info,...);
private:
	long				m_lSocketID;				//连接SCC
	map<DWORD,long>		m_mapVersion;
	map<long,CGapp*>	m_mapGappInfo;				//key  配置文件中的ID,所有GAPP唯一标识
	map<long,tagSCC>	s_mapSCC;					//scc列表,key 是SCC ID
	tagSetup			m_Setup;				//配置
	CDataBlockAllocator	*m_pDBAllocator;
	CServer				*s_pNetServer;				
	// 是否初次连接SCC
	bool					b_IsFirstConnectionSCC;
	//内存文件日志名称
	char m_pszMemUsageFile[MAX_PATH];
	//CPU文件日志名称
	char m_pszLogFile[MAX_PATH];
	tagSCC*				m_CurrScc;				//当前连接的SCC
	map<DWORD,CGapp*>m_TimeOut;//GAPP定时处理器
	GappTimer  m_GappTimer;
	list<tagSMTimerVar*> m_listTimerManager;
public:
	typedef	map<long,CGapp*>::iterator	GAPPITER;
	typedef map<long,tagSCC>::iterator  SCCITER;
	typedef deque<string>::iterator		CMDITER;	
	void RunCmdLine();
	GappTimer* GetGappTimer(void) { return &m_GappTimer;}
	tagSMTimerVar* GetTimerVar();
	void FreeTimerVar(tagSMTimerVar* p);
	void ReleaseTimerVarManager();
};


extern HANDLE g_hSMThreadExitEvent;		// 主线程退出事件
extern bool	g_bSMThreadExit;				// 是否退出主线程
BOOL CreateSM();
CSM* GetSM();
BOOL DeleteSM();
unsigned __stdcall		SMThreadFunc(void* pArguments);	// SM主线程
unsigned __stdcall		StartGappThreadFunc(void* pArguments);//自动启动GAPP
