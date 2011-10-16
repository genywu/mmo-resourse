/*
*	file:		GSAutoSupply.h
*	Brief:		自动战斗类
*	detail:		实现自动战斗的功能
*					
*					
*	Authtor:    袁洪俊
*	Date:		2011-02-14
*/

#include "../public/AutoFightData.h"

#pragma once


class GSAutoFight : public CAutoFightData
{
public:
    GSAutoFight();
    ~GSAutoFight();

public:
    //设置所有者
    void        SetOwner(CPlayer* pMainPlayer);
    //得到所有者
    CPlayer*    GetOwner(void)   { return  m_pOwner;}
    //改变辅助挂机设置
    LONG        ChangeSetup(LONG lType, tagAutoFightNode &AutoFightNode, BOOL bSysOpt = FALSE);
    //!         自动修理装备的银币判断
    bool        CanAutoRepair(void);

public:
    //!								定时器超时响应
    long							OnTimeOut			(DWORD timerid,DWORD curtime,const void* var);
    //!								当定时器被取消的调用
    void							OnTimerCancel		(DWORD timerid,const void* var);

private:
    //!								打开自动减少周期定时器
    void							OpenTimer			(void);
    //!                             打开具体时间定时器
    void                            OpenDieReLiveTimer  (DWORD timeValue);
    //!                             打开共有定时器
    void                            OpenCommonTimer     (void);
    //!								关闭自动定时器
    void							CloseTimer			(void);
    //!                             关闭共有定时器         
    void                            CloseCommonTimer    (void);
    //!								自动判断是否还要继续定时器触发
    void							GoOnTimer			(void);

    //!                             共有定时器响应执行
    void                            BeginWork(void);

public:
    //!										得到定时器对象
    static	CTimerQueue<GSAutoFight*>&		GetTimerQueue(void);
    static  CTimerQueue<GSAutoFight*>&      GetCommonTimerQueue(void);
private:

    //所有者
    CPlayer*       m_pOwner;
    //!                                     定时器开启标识
    BOOL           m_bTimerRunning;
    //!                                     共有定时器开启标识
    BOOL           m_bCommonRunning;
    //!                                     保护状态标识
    BOOL           m_bProtected;
    //!                                     定时器响应次数
    DWORD          m_dwTimerAI;
    //!                                     稳定挂机死亡原地复活时间控制
    DWORD          m_dwStableHookReliveAI;

    //!                                     耐久为0的响应次数
    DWORD          m_dwDurabilityZeroAI;
    //!                                     共有定时器ID
    LONG           m_CommonTimerID;
    //!                                     稳定和普通挂机共有定时器
    static  CTimerQueue<GSAutoFight*>      s_CommonTimerQueue;
    //!										定时器
    static	CTimerQueue<GSAutoFight*>		s_TimerQueue;
    //!                                     自动挂机中死亡复活定时器
    //static  CTimerQueue<GSAutoFight*>
};