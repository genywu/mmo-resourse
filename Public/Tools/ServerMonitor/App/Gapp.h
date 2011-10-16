#pragma once
#include "StdAfx.h"
#include "../../../SCMPublic/def/InfoDef.h"
#include "../../../SCMPublic/class/UpdatePack.h"
#include "patch.h"
class CGapp : public tagGappBaseInfo
{
public:
	CGapp(void);
	~CGapp(void);
	BOOL	Init(TiXmlElement* p);
	
	BOOL	Start();
	BOOL	ShutDown();
	void	SetInfo(const char* strTitle);
	void	SetState(DWORD sdwstate);
	inline  DWORD	GetState()				{return _dwGappState;}
	DWORD	GetID()					{return _dwGappID;}
	DWORD	GetProcessID()			{return m_dwProcessID;}
	bool	SetProcessID();
	const	char* GetPhysName()			{return m_szPhysName;}
	const	char* GetDisplayName()		{return _szGappName;}
	void	DelayTest();
	long	SetDelayTime(DWORD dwtime);
	long	GetDelayTime();
	void	AddToByteArray(DBWriteSet& dbWriteSet);
	BOOL	InitMemServer();
	BOOL	UpdateGapp(const char* szPatchName);
	DWORD	GetType()				{return _dwGappType;}
	BOOL	SetVersion();
	void	SetMainTick(DWORD tick);
	const char* GetVersion()		{return _szGappVersion;}
	BOOL	CmpVersion(stringstream& stream);//与SCC上的版本比较
	BOOL	GetNextVersionPatch(stringstream& stream);//获取下一个版本的更新包
	BOOL	GetFile(DBReadSet& dbreadset);//获取SCC发过来的文件
	char*	GetTitle()				{return m_szTitle;}
	BOOL    CheckVersion(const char* szVersion = NULL);	
	BOOL	GetPackRes();//获取原始包
	BOOL	WritePackRes(DBReadSet& dbreadset);//将原始包文件写到磁盘
	BOOL	Backup();//备份原有的包文件
	bool	GetHardWareLoad();
	void	OnTimeOut(DWORD dwTimerID,DWORD dwcurTime,const void* pargs);
	void	OnTimerCancel(DWORD dwTdwTimeID,const void* parg);
	void	CancelLoadResTimer();
	void	CancelPingTimer();
	void	RequestAITick();//发送到GAPP请求AITick
	void	RequestFile(char* filename);
	void	CmpAITick(DWORD tick);
	bool	CheckStart();
	bool	FindGappWindow();
	bool	IsShutDownOk();
	void	LSSendRefreshInfo(CMessage* pMsg){};
	void	DBSSendRefreshInfo(CMessage* pMsg){};
	void	WSSendRefreshInfo(CMessage* pMsg){};
	void	GSSendRefreshInfo(CMessage* pMsg){};
	void	SetIsLatest(BYTE b)	{m_bLatest=b;}
	bool	SetLSPort(DWORD dwPort);
	bool	Reset();//重设状态
	bool	CheckProcessNum();
	bool	FindProcByProcID();//正常启动关闭的时候检查进程
	bool	EnumGappWindow(DWORD dwProcID);
private:
	DWORD			m_dwProcessID;//进程号
	char			m_szPath[BASE_TEXT_SIZE];//路径
	char			m_szTitle[BASE_TEXT_SIZE];//窗口名称
	char			m_szPhysName[BASE_TEXT_SIZE];//物理文件名
	list<long>		m_listDelayTime;
	//内存映射文件
	char			m_szMapFileName[BASE_NAME_SIZE];//影射文件名
	char			m_szMapObjName[BASE_NAME_SIZE];//影射对象名
	char			m_szOutPutDir[BASE_NAME_SIZE];//输出日志文件夹
	
	//更新包
	CPatch*			m_pPatch;//更新包
	char			m_szNextVersion[BASE_NAME_SIZE];//下一个要更新的版本
	DWORD			m_MainAITick;
	DWORD			m_dwLoadResTimerID;
	DWORD			m_dwMainLoopTimerID;
	DWORD			m_dwShutDownTimerId;
	DWORD			m_dwPingTimerID;
	BYTE			m_bLatest;//是否更新到最新版本
	HWND			m_hHandle;//GAPP句柄
};

typedef struct tagWndInfo
{
	DWORD _dwProcID;
	HWND _hWnd;
}WNDINFO,*PWNDINFO;
