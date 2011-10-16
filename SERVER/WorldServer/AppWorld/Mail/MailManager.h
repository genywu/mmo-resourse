///////////////////////////////////////////////////////////////////////////////
///  MailManager.h
///
///  E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\WorldServer\AppWorld\Mail
///
///  CREATED: 09/12/2008  by 陈先进
///
///  PURPOSE:   邮件管理类
///
//////////////////////////////////////////////////////////////////////
#ifndef _MAILSYSTEM_H_
#define _MAILSYSTEM_H_
#pragma once

#include "Mail.h"

typedef CTimerQueue<CGameEvent*> MailTimer;

class CMailSystem :public CGameEvent
{
public:
	struct stTimerParam
	{
		//定时器ID
		long lTimerID;
		//预定触发时间
		long lFuture;
		//调用次数
		long lCount;
		//已调用次数
		long lCalledCount;
		//开始时间
		DWORD nStartTime;			
	};
public:
	CMailSystem();
	~CMailSystem();
	void	SendMail( CMail *pMail,long lSysID=-1);										//转发邮件
	void	SendMailToGS(CMail *pMail,CPlayer *pPlayer);								//发邮件给收信人	
	CMail   *GetMail(const CGUID &guid);												//获取邮件对象
	CMail	*GetTempMail(const CGUID &guid);											//获取临时邮件
	bool	SaveMailToDB(CPlayer *pPlayer,CMail *pMail);								//邮件保存到DB
	bool	SaveMailToTempMail(CMail *pMail);											//邮件保存到临时邮件
	void	DeleteTempMail(CGUID &guid);												//删除临时邮件
	void	SendAllMailToGs(CPlayer *pPlayer);											//发送所有玩家邮件到GS
	void	SendDeleteMailToDB(const CGUID &PlayerGuid, list<CGUID> &lMailGuid);		//发送删除邮件to DB
	void    SendDeleteMailToGS(const CGUID &PlayerGuid, list<CGUID> &lMailGuid);		//发送删除邮件to GS
	void	GetGoodsFromMail(CGUID &guid,long lIndex);									//从信中取物品
	void	SaveAndSendMailToReceiver(CMail *pMail);
	void	GetGoldFromMail(CGUID &guid);												//从邮件中取金币
	void	ReadMail(CGUID &guid);														//阅读邮件
	void	PayMail(CGUID &guid);														//付款收信
	void	SendPayMailMsgToGs(CPlayer *pPlayer, CGUID &MailGuid);						//向GS返回付款收信
	bool	OnChangeMailDest(CMail *pMail);												//转发邮件	
	void	RejectMail(CPlayer *pPlayer, CGUID &guid);									//退信
	bool	SaveMailToMailSys(CPlayer *pPlayer,CMail *pMail);							//保存邮件到邮件系统
	bool	StartRegisterTimer(const CGUID &PlayerGuid, CMail *pMail);					//启动定时器
	void	RegisterTimer(const CGUID &PlayerGuid,const  CGUID &MailGuid,long lFuture);	//注册定时器					
	void	UnRegisterTimer(long lTimerID);												//注消定时器
	bool	RegisterSysMailTimer(DWORD dwID,CMail *pMail);								//注册系统邮件定时器
	void	UnRegisterSysMailTimer(long lTimerID);										//注消系统邮件定时器
	void	RegisterCheckTime(const CGUID &PlayerGuid,long lFuture);					//注册邮件检查定时器
	void	UnRegisterCheckTime(const CGUID &PlayerGuid);								//注册邮件检查定时器
	void	SendMailMsgToGS(const CGUID &MailGuid,int nRet);							//向GS返回寄信结果
	void	SuccessSaveMail(CGUID &MailGuid);											//邮件保存成功
	void	FailedSendMail(CPlayer *pPlayer,long lValue);								//邮件发送失败
	void	UnReisterTimerOnReject(const CGUID &MailGuid);								//退信后注销定时器
	void	DeleteMailOnPlayerExit(CPlayer *pPlayer);									//玩家下线后删除ws上的邮件
	void	FindNoReceiver(CGUID &MailGuid);											//没有找到收信人
	void	FailedSaveMail(CGUID &MailGuid);											//邮件保存失败
	void	SendSaveMailRet(CGUID &MailGuid,long lRet);									//返回保存邮件结果
	void	Run();
	long	Schedule(	CGameEvent* const pEvent,const void *arg,unsigned long futuretime,unsigned long interval = 0);
	int		Cancel(CGameEvent* const pEvent);
	int		Cancel(long timerid, const void** arg = 0);
	

	//网络消息响应
	virtual void OnMessage(CMessage*);
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var);
	//处理实体间消息函数
	virtual long Proc(eEventType type,const tagProcVar *pVar);
	static CMailSystem *GetInstance();


	void	InitSysMail();															//初始化系统邮件
	void	InitMailParam(tagMailParam *pParam,tagMailParam *pPamramList);			//初始化邮件参数
	void	AddSysMail(CMail *pMail,DWORD dwID);									//添加系统邮件
	CMail	*GetSysMail(long lID);													//通过id得到系统邮件
	void	ClearSysMail();															//清除系统邮件
	void	ClearSysMail(CMail *pMail);												//清除系统邮件
	void	SendSysMail(long lSysID);												//发送系统邮件	
	void	InitSysMailParam(tagMailParam *pParam,tagMailParam *pPamramList);		//初始化系统邮件参数
	void	AddPlayerMail(CGUID &PlayetGuid,list<CMail*> &lMail);					//保存从DB初始化获取的邮件
	void	ReleasePlayerMail(CPlayer* pPlayer);									//玩家离开释放邮件
	void	OutMailInfo(VOID);

private:
	map<CGUID,CMail*>	m_mapMail;
	map<CGUID,CMail*>   m_mapTempMail;												
	map<DWORD,CMail*>   m_mapSysMail;
	map<CGUID,list<CMail*>>	m_mapPlayerMail;
	map<DWORD,stTimerParam*> m_mapTimerParam;
	map<long, tagTimerVar*>	m_mapTimer;
	map<CGUID,long>		m_mapCheckMailTimer;
	static MailTimer	*m_pMailTimer;	
	static CMailSystem	*instance;
};
#endif//_MAILSYSTEM_H_