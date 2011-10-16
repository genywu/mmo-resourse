

#include "StdAfx.h"
#include "GSAutoSupply.h"
#include "Player.h"


CTimerQueue<GSAutoSupply*>		GSAutoSupply::s_TimerQueue(4, Auto_Supply_Space);

//! GSAutoSupply::得到定时器对象
CTimerQueue<GSAutoSupply*>& GSAutoSupply::GetTimerQueue(void)
{
	return s_TimerQueue;
}

GSAutoSupply::GSAutoSupply(void)
:m_pOwner(NULL)
,m_bTimerRunning(FALSE)
{
	
}

GSAutoSupply::~GSAutoSupply(void)
{
	CloseTimer();
}

//! 设置所有者
void GSAutoSupply::SetOwner(CPlayer *pMainPlayer)
{
	m_pOwner = pMainPlayer;
	if (NULL != m_pOwner)
	{
		BOOL bOpenTimer = FALSE;
		//! 记录设置物品的位置备用
		for (LONG i = eAST_HP1; i < AotoSupplyTypeNum; ++i)
		{
			if(GoodsReady(i))
				bOpenTimer = TRUE;
		}

		if (bOpenTimer)
		{
			OpenTimer();
		}
	}
}

//! 改变补给设置
BOOL GSAutoSupply::ChangeSetup(LONG eSupplyType, tagAotoSupplyNode &AotoSupplyNode, BOOL bSysOpt)
{
	if (NULL != m_pOwner)
	{
		if(eASET_NotErr == SetOneSetup(eSupplyType, AotoSupplyNode) || bSysOpt)
		{
			if(GoodsReady(eSupplyType))
			{
				OpenTimer();
			}
			else
			{
				GoOnTimer();
			}

			return TRUE;
		}
	}
	return FALSE;
}

//! 自动补给执行
void GSAutoSupply::Supply(LONG eSupplyType)
{
	assert(NULL != m_pOwner);
	const tagAotoSupplyNode* pAotoSupplyNode = GetOneSetup(eSupplyType);
	assert(NULL != pAotoSupplyNode);

	if (!pAotoSupplyNode->bActive || 0 == pAotoSupplyNode->lGoodsIndex)
		return;
	
	//! ----------------------------使用条件判断----------------------------
	LONG lCurrScale = A_Hundred_Percent;
	switch(eSupplyType)
	{
	case eAST_HP1:
	case eAST_HP2:
		{
			if(0 != m_pOwner->GetMaxHP())
				lCurrScale = m_pOwner->GetHP() * 100 / m_pOwner->GetMaxHP();
		}
		break;
	case eAST_MP:
		{
			if(0 != m_pOwner->GetMaxMP())
				lCurrScale = m_pOwner->GetMP() * 100 / m_pOwner->GetMaxMP();
		}
		break;
	case eAST_ArmPower:
		{
			lCurrScale = m_pOwner->GetArmCurrPower();
		}
		break;
	case eAST_SpriteMP:
		{
			// 10.14.2009 此栏位改为玩家PK CP栏位，置于此栏位的物品自动补给玩家CP
			// 三转职业且80级以上才会补给
			if( m_pOwner->GetLevel() < 80 ||
				!( m_pOwner->GetOccupation() >= OCC_Paladin &&
				m_pOwner->GetOccupation() <= OCC_Prophet ) )
			{
				break;
			}
			long lMaxCP = m_pOwner->GetMaxPKCP();
			if( lMaxCP != 0 )
			{
				lCurrScale = 100 * m_pOwner->GetPKCP() / lMaxCP;
			}
		}
		break;
	}

	if((eAST_ArmPower != eSupplyType && pAotoSupplyNode->lSpringScale <= lCurrScale)
		|| (eAST_ArmPower == eSupplyType && (0 < lCurrScale || -1 == lCurrScale))
		)
		return;

	//! ----------------------------使用物品----------------------------

	//! 物品使用间隔
	DWORD lNow = timeGetTime();
	if((DWORD)(m_arrGoodsParam[eSupplyType].lLastTime + m_arrGoodsParam[eSupplyType].lCoolingTime) > lNow)
		return;

	//! 使用
	if(!m_pOwner->AutoUseItem(
		m_arrGoodsParam[eSupplyType].wPlace, 
		m_arrGoodsParam[eSupplyType].lPos,
		m_arrGoodsParam[eSupplyType].lTargetType,
		m_arrGoodsParam[eSupplyType].TargetGuid
		)
		)
	{
		//! 使用缓存的物品失败，说明物品用完或物品位置已变更
		//! 更新缓存物品
		if(m_pOwner->FindFirstGoodsByIndex(pAotoSupplyNode->lGoodsIndex, m_arrGoodsParam[eSupplyType]))
		{
			//! 再次使用物品
			if(!m_pOwner->AutoUseItem(
				m_arrGoodsParam[eSupplyType].wPlace, 
				m_arrGoodsParam[eSupplyType].lPos,
				m_arrGoodsParam[eSupplyType].lTargetType,
				m_arrGoodsParam[eSupplyType].TargetGuid
				)
				)
			{
				return;//! 失败了不记录使用时间，直接返回了
			}
		}
		else
		{
			GoOnTimer();
			return;
		}
	}

	CMessage msg(MSG_S2C_SPRITE_AutoSupply_UseItem_Notify);
	msg.Add(eSupplyType);
	msg.SendToPlayer(m_pOwner->GetExID(), false);

	m_arrGoodsParam[eSupplyType].lLastTime = lNow;
}

//!								定时器超时响应
long GSAutoSupply::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	if (NULL != m_pOwner)
	{
		BOOL bOpenTimer = FALSE;
		//! 记录设置物品的位置备用
		for (LONG i = eAST_HP1; i < AotoSupplyTypeNum; ++i)
		{
			Supply(i);
		}
	}

	return 0;
}

//! 当定时器被取消的调用
void GSAutoSupply::OnTimerCancel(DWORD timerid,const void* var)
{

}

//! 打开自动补给定时器
void GSAutoSupply::OpenTimer(void)
{
	//! 创建留言周期任务定时器
	if(!m_bTimerRunning)
	{
		GetTimerQueue().Schedule(this, NULL, Auto_Supply_Space, Auto_Supply_Space);
		m_bTimerRunning = TRUE;
	}
}

//! 关闭自动补给定时器
void GSAutoSupply::CloseTimer(void)
{
	//! 取消任务定时
	m_bTimerRunning = FALSE;
	GetTimerQueue().Cancel(this);
}

void GSAutoSupply::GoOnTimer(void)
{
	BOOL bGoOnTimer = FALSE;
	for (LONG i = eAST_HP1; i < AotoSupplyTypeNum; ++i)
	{
		bGoOnTimer |= GoodsReady(i);
	}

	if(bGoOnTimer)
		return;

	CloseTimer();
}

//! 准备物品
BOOL GSAutoSupply::GoodsReady(LONG eSupplyType)
{
	assert(NULL != m_pOwner);
	const tagAotoSupplyNode* pAotoSupplyNode = GetOneSetup(eSupplyType);
	assert(NULL != pAotoSupplyNode);
	if (0 != pAotoSupplyNode->lGoodsIndex)
	{
		//! 即使没有找到补给物品，也不影响返回结果
		m_pOwner->FindFirstGoodsByIndex(pAotoSupplyNode->lGoodsIndex, m_arrGoodsParam[eSupplyType]);
		//! 返回值影响补给定时器是否启动，因此返回的是[激活状态]和[物品是否设置]的与值
		return pAotoSupplyNode->bActive;
	}
	return FALSE;
}