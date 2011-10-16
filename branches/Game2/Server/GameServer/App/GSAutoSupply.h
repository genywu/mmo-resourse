/*
*	file:		GSAutoSupply.h
*	Brief:		自动补给类
*	detail:		实现自动补给的功能
*					
*					
*	Authtor:	张科智
*	Datae:		2008-05-22
*/

#pragma once

#include "../../../Public/ServerPublic/Server/Common/AutoSupplyData.h"

//! 用于记录使用物品参数的结构
struct tagGoodsParam
{
	WORD	wPlace;
	BYTE	lPos;
	LONG	lTargetType;
	CGUID	TargetGuid;
	LONG	lCoolingTime;	//! 物品冷却时间（毫秒）
	LONG	lCoolingType;	//! 物品冷却类型
	LONG	lLastTime;		//! 上次喝药时间（毫秒）
};


class GSAutoSupply : public AutoSupplyData
{
public:
	GSAutoSupply(void);
	~GSAutoSupply(void);


public:
	//!								设置所有者
	void							SetOwner			(CPlayer *pMainPlayer);
	//!								改变补给设置
	BOOL							ChangeSetup			(LONG eSupplyType, tagAotoSupplyNode &AotoSupplyNode, BOOL bSysOpt = FALSE);
	//!								自动补给执行
	void							Supply				(LONG eSupplyType);

public:
	//!								定时器超时响应
	long							OnTimeOut			(DWORD timerid,DWORD curtime,const void* var);
	//!								当定时器被取消的调用
	void							OnTimerCancel		(DWORD timerid,const void* var);
	//!
	
private:
	//!								打开自动补给定时器
	void							OpenTimer			(void);
	//!								关闭自动补给定时器
	void							CloseTimer			(void);
	//!								自动判断是否还要继续定时器触发
	void							GoOnTimer			(void);
	//!								准备物品
	BOOL							GoodsReady			(LONG eSupplyType);

public:
	//!										得到定时器对象
	static	CTimerQueue<GSAutoSupply*>&		GetTimerQueue(void);

private:
	
	//!										定时器开启标志
	BOOL									m_bTimerRunning;
	//!										所有者
	CPlayer									*m_pOwner;
	//!										缓存可使用的第一个物品
	tagGoodsParam							m_arrGoodsParam[AotoSupplyTypeNum];

	//!										定时器
	static	CTimerQueue<GSAutoSupply*>		s_TimerQueue;

	
};