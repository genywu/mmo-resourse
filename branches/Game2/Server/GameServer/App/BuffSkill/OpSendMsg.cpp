#include "StdAfx.h"
#include "OpSendMsg.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"

#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COpSendMsg::COpSendMsg(stModuParam *pParam)
:COperator(pParam)
{
}
COpSendMsg::COpSendMsg(void * pParam)
:COperator((stModuParam*)pParam)
{

}
COpSendMsg::~COpSendMsg(void)
{
}

//锁定物品消息
void COpSendMsg::SendLockGoodsMsg()
{
	if (!m_pParam->pUser->IsDied())
	{
		CMessage msg(MSG_S2C_SKILL_LOCK_GOODS);
		msg.Add(m_pParam->lContainerID);
		msg.Add(m_pParam->lPos);
		msg.SendToPlayer(m_pParam->pUser->GetExID(),false);
	}

}

//发送技能开始消息
void COpSendMsg::CreateSkillBeginMsg()
{
	//发送锁定物品信息
	if (m_pParam->lContainerID!=0)
	{
		SendLockGoodsMsg();	
	}
	if (m_pParam->pUser->GetState()==CShape::STATE_PEACE && m_pParam->eAddSubType==eSub_Type)
	{
		m_pParam->pUser->ChangeStateTo(CShape::STATE_FIGHT);
	}
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_BEGIN);//"##表示条件满足，开始使用技能"
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( m_pParam->pUser->GetDirEx());
	//全局id
	msg.Add(m_pParam->dwGlobleID);
	//id和等级
	msg.Add((long)m_pParam->nID);
	msg.Add((BYTE)m_pParam->nLvl );
	//当前技能执行到的阶段
	msg.Add((BYTE)(m_pParam->nProceNum+1));
	//阶段耗时
	msg.Add((long)m_pParam->lUseTime);
	//冷却时间
	msg.Add((long)m_pParam->lCoolDownTime);

	//添加鼠标点位置
	if (m_pParam->pMouseShape)
	{
		msg.Add((short)m_pParam->pMouseShape->GetTileX());
		msg.Add((short)m_pParam->pMouseShape->GetTileY());	
	}
	else
	{
		msg.Add((short)m_pParam->nMouseX);
		msg.Add((short)m_pParam->nMouseY);	

	}

	//添加作用目标对象集合
	DWORD dwSize = (DWORD)m_pParam->OperObjs.size();
	msg.Add((BYTE)dwSize);
	itDestObj it = m_pParam->OperObjs.begin();
	for(;it != m_pParam->OperObjs.end();it++)
	{
		msg.Add((*it)->nDestType);
		msg.Add((*it)->DestID);
	}
	if(m_eRange == MsgRange_Single )
		msg.SendToPlayer(m_pParam->pUser -> GetExID(),false);
	else if(m_eRange == MsgRange_Around)
	{
		CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
		if(pRegion)
		{
			pRegion->FindLimitPlayersAround(m_pParam->pUser,GlobalSetup::GetSetup()->lSendSkillMsgLimitPlayerNum,m_pParam->SendMsgPlayers,NULL_GUID);
			GetGame()->RemoveTransferCongPlayerID(m_pParam->SendMsgPlayers);
		}
		msg.SendToPlayers(m_pParam->SendMsgPlayers);
	}
#ifdef _DEBUG
	long lTime = timeGetTime();
	Log4c::Trace(ROOT_MODULE, "发送技能开始消息:技能ID:%6d,全局ID:%6d,时间%d,阶段号:%d",m_pParam->nID,m_pParam->dwGlobleID,timeGetTime(),m_pParam->nProceNum);
#endif

}
//发送技能结束消息
void COpSendMsg::CreateSkillEndMsg()
{
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_END);
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( m_pParam->pUser->GetDirEx());
	//全局id
	msg.Add(m_pParam->dwGlobleID);
	//id和等级
	msg.Add((long)m_pParam->nID);
	msg.Add((BYTE)m_pParam->nLvl );
	//当前技能执行到的阶段
	msg.Add((BYTE)(m_pParam->nProceNum+1));

	if(m_eRange == MsgRange_Single)
		msg.SendToPlayer(m_pParam->pUser -> GetExID(),false);
	else if(m_eRange == MsgRange_Around)
		msg.SendToPlayers(m_pParam->SendMsgPlayers);
#ifdef _DEBUG
	char str[256]="";
	_snprintf(str, 256, "发送技能结束消息:技能ID:%6d,全局ID:%6d,阶段号:%d\n",m_pParam->nID,m_pParam->dwGlobleID, m_pParam->nProceNum);
	OutputDebugString(str);
#endif
}
//发送技能失败消息
void COpSendMsg::CreateSKillFailMsg()
{
	CMoveShape* pUser = m_pParam->pUser;

	if(m_strVariaName !="")
	{
		//  [5/4/2009 chenxianj]	
		m_lValue = (long)m_pParam->pVariableList->GetVarValue<double>(m_strVariaName.c_str(),0);		
		//  [5/4/2009 chenxianj]
		
	}
	//技能失败重置吟唱时间
	if (pUser)
	{
		m_pParam->pUser->SetIntonateTime(timeGetTime(),0);
	}
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
	msg.Add( pUser->GetExID());
	//id和等级
	msg.Add((long)m_pParam->nID);
	msg.Add((BYTE)m_pParam->nLvl );
	//全局id
	msg.Add(m_pParam->dwGlobleID);
	//错误类型
	msg.Add((BYTE)m_lValue);
	CMoveShape* pPlayer = pUser;
	if( TYPE_PET == pUser->GetType() )
	{
		pPlayer = pUser->GetHost();
	}

	if(m_eRange == MsgRange_Single)
		msg.SendToPlayer(pPlayer->GetExID(), false);
	else if(m_eRange == MsgRange_Around)
		msg.SendToPlayers(m_pParam->SendMsgPlayers);
#ifdef _DEBUG
	char str[256]="";
	_snprintf(str, 256, "发送技能失败消息:技能ID:%6d,全局ID:%6d,阶段号:%d\n",m_pParam->nID,m_pParam->dwGlobleID, m_pParam->nProceNum);
	OutputDebugString(str);
#endif

}

//发送buff开始消息
void COpSendMsg::CreateBuffBeginMsg()
{
	//计算剩余时间
	long lRemainTime = 0;
	stModuParam::itTimer it = m_pParam->Timers.begin();
	if(it != m_pParam->Timers.end())
	{
		stTimerParam *pTimerParam = (*it);
		lRemainTime += pTimerParam->lFuture+pTimerParam->lInter*pTimerParam->nCount;
	}

	CMessage msg(MSG_S2C_SKILL_ADD_EX_STATE);

	//使用者type和ID
	msg.Add(m_pParam->pUser->GetType());
	msg.Add(m_pParam->pUser->GetExID());

	//buff的等级和ID
	msg.Add((DWORD)m_pParam->nID);
	msg.Add((DWORD)m_pParam->nLvl);
	msg.Add(lRemainTime);
	msg.Add(m_bUpdate);

	if(m_eRange == MsgRange_Single)
		msg.SendToPlayer(m_pParam->pUser -> GetExID(),false);
	else if(m_eRange == MsgRange_Around)
		msg.SendToAround(m_pParam->pUser);
}



//发送buff结束消息
void COpSendMsg::CreateBuffEndMsg()
{
#ifdef _DEBUG
	//char str1[64]="";
	//_snprintf(str1, 64, "状态%d,键值%d结束\n",m_pParam->nID,m_pParam->dwGlobleID);
	//OutputDebugString(str1);
#endif
	CMessage msg( MSG_S2C_SKILL_END_EX_STATE );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add((DWORD)m_pParam->nID);
	msg.Add((DWORD)m_pParam->nLvl);
	if(m_eRange == MsgRange_Single)
		msg.SendToPlayer(m_pParam->pUser -> GetExID(),false);
	else if(m_eRange == MsgRange_Around)
		msg.SendToAround(m_pParam->pUser);
}

//执行
void COpSendMsg::Execute(CMoveShape* pShape)
{
	if(m_eRange == MsgRange_Single && m_pParam->pUser->GetType() != TYPE_PLAYER && m_pParam->pUser->GetType() != TYPE_PET)
		return;
	if(m_pCreateMsg)
		(this->*m_pCreateMsg)();
}


COpSendDamageMsg::COpSendDamageMsg(stModuParam *pParam)
:COperator(pParam)
{
}

COpSendDamageMsg::~COpSendDamageMsg(void)
{
}

//普通减血消息
void COpSendDamageMsg::CreateSelfDamageMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_DAMAGE );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	int nSize = (int)m_pParam->SelfDamages.size();
	msg.Add((char)nSize);
	stModuParam::itDamage it = m_pParam->SelfDamages.begin();
	for(;it!=m_pParam->SelfDamages.end();it++)
	{
		msg.Add((char)(*it).first);
		msg.Add((*it).first);
	}
	msg.Add( m_pParam->pUser ->GetHP());
	msg.SendToNearestPlayer(m_pParam->pUser);

}
//普通减血消息
void COpSendDamageMsg::CreateDestDamageMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_BREAK );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	int nSize = (int)m_pParam->DestDamages.size();
	msg.Add((char)nSize);
	stModuParam::itDamage it = m_pParam->DestDamages.begin();
	for(;it!=m_pParam->DestDamages.end();it++)
	{
		msg.Add((char)(*it).first);
		msg.Add((*it).first);
	}
	msg.Add( pShape ->GetHP());
	msg.Add( (char)m_lBlastAttack);
	msg.Add((DWORD)m_pParam->nID);
	msg.Add((char)m_pParam->nLvl);
	msg.SendToNearestPlayer(pShape,true);
}
//创建闪避消息
void COpSendDamageMsg::CreateDestDodgeMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_FULLMISS );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//创建格挡消息
void COpSendDamageMsg::CreateDestBlockMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_BLOCK );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	int nSize = (int)m_pParam->DestDamages.size();
	msg.Add((char)nSize);
	stModuParam::itDamage it = m_pParam->DestDamages.begin();
	for(;it!=m_pParam->DestDamages.end();it++)
	{
		msg.Add((char)(*it).first);
		msg.Add((*it).first);
	}
	msg.Add( pShape ->GetHP());
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//招架消息
void COpSendDamageMsg::CreateDestParryMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_PARRY );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//抵抗消息
void COpSendDamageMsg::CreateDestResistMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_RESIST );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//吸收消息
void COpSendDamageMsg::CreateDestSuckMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_SUCK );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	int nSize = (int)m_pParam->DestDamages.size();
	msg.Add((char)nSize);
	stModuParam::itDamage it = m_pParam->DestDamages.begin();
	for(;it!=m_pParam->DestDamages.end();it++)
	{
		msg.Add((char)(*it).first);
		msg.Add((*it).first);
	}
	msg.Add( pShape ->GetHP());
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//反弹消息
void COpSendDamageMsg::CreateDestReboundMsg(CMoveShape* pShape)
{
	CMessage msg( MSG_S2C_SHAPE_ATK_REBOUND );
	msg.Add( m_pParam->pUser -> GetType() );
	msg.Add( m_pParam->pUser -> GetExID() );
	msg.Add( pShape -> GetType() );
	msg.Add( pShape -> GetExID() );
	int nSize = (int)m_pParam->DestDamages.size();
	msg.Add((char)nSize);
	stModuParam::itDamage it = m_pParam->DestDamages.begin();
	for(;it!=m_pParam->DestDamages.end();it++)
	{
		msg.Add((char)(*it).first);
		msg.Add((*it).first);
	}
	msg.Add( pShape ->GetHP());
	msg.Add( (char)m_lBlastAttack);
	msg.SendToNearestPlayer(pShape);
}

//执行
void COpSendDamageMsg::Execute(CMoveShape* pShape)
{
	if(m_strBlastVariaName !="")
	{
		//  [5/4/2009 chenxianj]		
		m_lBlastAttack = (long)m_pParam->pVariableList->GetVarValue<double>(m_strBlastVariaName.c_str(),0);
		//  [5/4/2009 chenxianj]		
	}
	if(m_pCreateMsg)
		m_pCreateMsg(pShape);
}
