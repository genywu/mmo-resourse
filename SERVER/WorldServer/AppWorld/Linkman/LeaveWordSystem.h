
/*
*	file:		LeaveWordSystem.h
*	Brief:		留言系统
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-03-06
*/

#include "../../public/LeaveWordDef.h"

#pragma once


class LeaveWordSystem
{
public:
	LeaveWordSystem(void);
	~LeaveWordSystem(void);

public:
	//!								获得唯一实例
	static		LeaveWordSystem&	GetInstance					(void);

public:
	//!								初始化
	BOOL							Init						(const char *pSetupFile);
	//!								发送配置到DBS
	void							SendSetupToDBS				(void);
	//!								释放
	void							Release						(void);

	//!								玩家留言
	BOOL							LeaveWord					(const CGUID &SenderGuid, const char *pReceiverName, char *pWord);

	//!								响应玩家上线
	void							OnPlayerLogin				(CPlayer *pReceiver);

	//!								响应玩家留言取出
	void							OnLeaveWordPop				(CMessage *pMsg);
	//!								响应留言保存结果
	void							OnLeaveWordResult			(CMessage *pMsg);

public:
	//!								定时器超时响应
	long							OnTimeOut					(DWORD timerid,DWORD curtime,const void* var);
	//!								当定时器被取消的调用
	void							OnTimerCancel				(DWORD timerid,const void* var);
	//!								定时器检测触发
	void							TimerExpire					(LONG lCurrTime);

private:
	//!								读取配置
	BOOL							LoadSetup					(const char *pSetupFile);
	//!								启动工作定时器
	BOOL							StartupTimer				(void);

	//!								通知玩家密聊发送失败
	void							RePlayerSendFailed			(CPlayer *pSender, const char *pReceiverName);
	//!								获得一个缓存的ID
	LONG							GetBufID					(void);

	//!								创建WSDB结构
	tagLeaveWord_W2DB*				CreateLeaveWord_W2DB		(void);
	//!								释放WSDB结构
	void							ReleaseWord_W2DB			(tagLeaveWord_W2DB* pLeaveWord_W2DB);

private:
	//!								定时器
	CTimerQueue<LeaveWordSystem*>	*m_pTimerQueue;
	//!								配置信息
	tagLeaveWordSetup				m_LeaveWordSetup;

	//!								等待初始化的玩家
	set<CGUID>						m_WaitPlayerGuid;
	//!								玩家留言次数记录
	map<CGUID, LONG>				m_PlayerLvwdCount;

	//!								写入留言时的缓冲
	map<LONG, tagLeaveWord_W2DB*>	m_LeaveWriteBuf;
	//!								缓冲ID的种子
	LONG							m_BufIdSeed;
};