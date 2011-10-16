////////////////////////////////////////////////////////////////////////////
//CityGate.cpp/////////////////////////////////////////////////////////////
//城门类
//Author:Joe
//Create Time:2004.721
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\citygate.h"
#include "Player.h"
#include "Monster.h"
#include "ServerRegion.h"
#include "script/Script.h"



/***********************************
/门
/
/**********************************/
CGate::CGate()
{
	m_lType=TYPE_CITYGATE;
}
CGate::~CGate()
{
	DelectAllScripByShape(GetExID());
}
BOOL CGate::InitProperty(CGateList::tagGate *p)
{
	SetName(p->cName);//名字
	SetLenth(p->wLenth);//长		
	SetWidth(p->wWidth);//宽
	SetLevel(p->wLevel);//等级
	SetGraphicsID(p->dwModelId);//模型ID
	SetMaxHP(p->wMaxHp);//最大HP
	SetHP(p->wMaxHp);	//当前HP
	SetUnAttack(p->bIsunbeatable);//是否无敌
	SetBlockType(p->byBlockType);//阻挡类型
	SetDef(p->lDef);//物防	
	SetMagicDef(p->lMagicDef);//魔防
	SetNameColor(p->wNameColor);//名字颜色
	SetHitSound(string(p->cHitSound));//击中音效
	SetDestorySound(string(p->cDestorySound));//损毁音效
	SetOpenSound(string(p->cOpenSound));//打开音效
	SetCloseSound(string(p->cCloseSound));//关闭音效
	return TRUE;
}

BOOL CGate::InitSetProperty(CServerRegion::tagGateProperty *p)
{
	SetIndex(p->lIndex);//编号
	CShape::SetPosXY( (float)(p->lxPos + 0.5),(float)(p->lyPos + 0.5) );
	SetDir(p->lDir);//方向
	CShape::SetState(p->wOpenState);//打开状态
	CShape::SetAction(p->wOpenState);//动作
	SetAttackScp(p->strAttack);//受攻击脚本
	
	SetOpen(p->strOpen);//打开脚本
	SetClose(p->strClose);//关闭脚本
	SetDestoryScp(p->strDestory);//损毁脚本	
	SetRefreshScp(p->strRefresh);//重刷脚本
	return TRUE;
}

//// 添加到CByteArray
bool CGate::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	CBuild::CodeToDataBlock(setWriteDB,bExData);
	setWriteDB.AddToByteArray(m_strOpenSound.c_str());
	setWriteDB.AddToByteArray(m_strCloseSound.c_str());
	return true;
}

// 解码
bool CGate::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CBuild::DecodeFromDataBlock(setReadDB, bExData);
	char strVal[256];
	setReadDB.GetStringFromByteArray( strVal,256);
	m_strOpenSound=strVal;
	setReadDB.GetStringFromByteArray( strVal,256);
	m_strCloseSound=strVal;
	return true;
}

void CGate::SetGate(WORD l)
{
	CBuild::SetBuild(l);
	switch (l)
	{
	case STATE_CLOSE:			//设置为关闭状态	
		{
			//当前状态不为损毁
			if (m_wState!=STATE_DIED)
			{
				MoveAroundPlayer();
				//设置阻挡
				SetAttribute("Action",ACT_IDLE);
				SetAttribute("State",STATE_FIGHT);
				SetAction(ACT_IDLE);				
			}
		}
		break;
	case STATE_OPEN:			//设置为打开状态	
		{
			if (m_wState!=STATE_DIED)
			{
				SetAttribute("Action",ACT_OPEN);
				SetAttribute("State",STATE_PEACE);
				SetAction(ACT_OPEN);			
			}
		}
		break;
	default:
		break;
	}
}

// 可否攻击
bool CGate::IsAttackAble(CMoveShape* pAttackShape)
{
	if(!GetUnAttack()) return false;
	if(pAttackShape == NULL)	return false;

	if(GetState() == STATE_DIED || IsDied()) return false;
	//城门打开时不可攻击
	if(GetAction()==ACT_OPEN) return false;

	CServerRegion* pRegion = (CServerRegion*)GetFather();
	if(pRegion == NULL) return false;

	//城门在非战斗时间不能被攻击
	bool bResult = false;

	switch( pAttackShape -> GetType() )
		{
		case TYPE_PLAYER:
			{
			
			}
			break;


		}

	return bResult;
}

void CGate::OnDied()
{
	CBuild::OnDied();
}




