#include "StdAfx.h"
#include "Condition.h"
#include "Expressions.h"
#include "BuffSkillDef.h"
#include "../MoveShape.h"
#include "../Monster.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../PKSys.h"
#include "../script/Script.h"
#include "../goods//CGoods.h"
#include "..\script\VariableList.h"

CCondition::CCondition(stModuParam *pParam)
:m_pParam(pParam)
{
}

CCondition::~CCondition(void)
{
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConProperty::CConProperty(stModuParam *pParam)
:CCondition(pParam)
{
}

CConProperty::~CConProperty(void)
{
}

bool CConProperty::Result(CMoveShape* pDest)
{
	int value1 = 0;
	itCheck it = m_Checks.begin();
	for(;it != m_Checks.end();it++)
	{
		tagCheckes &check = (*it);
		if(check.Obj == Obj_Self && m_pParam->pUser)
			value1 = m_pParam->pUser->GetAttribute(check.strName);
		else if(check.Obj == Obj_Dest && pDest)
			value1 = pDest->GetAttribute(check.strName);

		if(check.strVariaName!="")
		{
			//  [5/4/2009 chenxianj]		
			check.nValue = m_pParam->pVariableList->GetVarValue(check.strVariaName.c_str(),0);		
			//  [5/4/2009 chenxianj]
			
		}
		if(!check.pCmpFun(value1,check.nValue))
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConGoods::CConGoods(stModuParam *pParam)
:CCondition(pParam)
{
}

CConGoods::~CConGoods(void)
{
	itCheck it=m_Checkes.begin();
	for (;it!=m_Checkes.end();++it)
	{
		for (list<tagAddProperty*>::iterator pit=(*it).lAddProperty.begin();
			pit!=(*it).lAddProperty.end();++pit)
		{
			MP_DELETE(*pit);
		}
		(*it).lAddProperty.clear();

	}
}

bool CConGoods::GetGoodAddProperty(CGoods* pEquip,list<tagAddProperty*> &lAddProperty)
{
	for (list<tagAddProperty*>::iterator it= lAddProperty.begin(); it!=lAddProperty.end();++it)
	{
		if ((*it)->dwValue1!=0)
		{
			if(pEquip->GetAddonPropertyValues((GOODS_ADDON_PROPERTIES)((*it)->dwAddProType),1)!=(*it)->dwValue1)
			{
				return false;
			}
		}
		if ((*it)->dwValue2!=0)
		{
			if(pEquip->GetAddonPropertyValues((GOODS_ADDON_PROPERTIES)((*it)->dwAddProType),2)!=(*it)->dwValue2)
			{
				return false;
			}
		}
	}
	return true;
}
//得到物品检测结果
bool CConGoods::Result(CMoveShape* pDest)
{
	int value1 = 0;
	itCheck it = m_Checkes.begin();
	for(;it != m_Checkes.end();it++)
	{
		tagCheck &chek = (*it);
		if (chek.nPlace==CS2CContainerObjectMove::PEI_EQUIPMENT)
		{ 
			//装备栏
			if (chek.strOrigName.size()>0)
			{
				//原始名
				if(chek.Obj == Obj_Self && m_pParam->pUser)
				{
					//自己
					CGoods* pGoods=m_pParam->pUser->FindGoods(chek.nPlace,chek.nPos);
					if (pGoods!=NULL&&pGoods->HasAddonProperty(GAP_GOODS_MAXIMUM_DURABILITY)&&pGoods->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY,2)==0)
					{
						value1 = 0;
					}
					else
					{
						value1 = m_pParam->pUser->FindGoods(chek.nPlace,chek.nPos,chek.strOrigName.c_str());
					}		

				}
				else if(chek.Obj == Obj_Dest && pDest)
				{
					//目标
					value1 = pDest->FindGoods(chek.nPlace,chek.nPos,chek.strOrigName.c_str());
				}
			} 
			else
			{
				//类型
				if(chek.Obj == Obj_Self && m_pParam->pUser)
				{
					//自己
					CGoods* pEquip = m_pParam->pUser->FindGoods(chek.nPlace,chek.nPos);
					if (pEquip)
					{
						if (pEquip->GetGoodsBaseType() ==chek.nType)
						{
							if (pEquip!=NULL&&pEquip->HasAddonProperty(GAP_GOODS_MAXIMUM_DURABILITY)&&pEquip->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY,2)==0)
							{
								value1 = 0;
							}
							else
							{
								if (chek.lAddProperty.size()>0)
								{
									if (GetGoodAddProperty(pEquip,chek.lAddProperty))
									{
										value1 = pEquip->GetAmount();
									} 
									else
									{
										value1 = 0;
									}
								} 
								else
								{
									value1 = pEquip->GetAmount();
								}
							}
						}
					}

				}
				else if(chek.Obj == Obj_Dest && pDest)
				{	
					//目标
					CGoods* pEquip = pDest->FindGoods(chek.nPlace,chek.nPos);	
					if (pEquip)
					{
						if (pEquip->GetGoodsBaseType() ==chek.nType)
						{
							if (chek.lAddProperty.size()>0)
							{
								if (GetGoodAddProperty(pEquip,chek.lAddProperty))
								{
									value1 = pEquip->GetAmount();
								} 
								else
								{
									value1 = 0;
								}
							} 
							else
							{
								value1 = pEquip->GetAmount();
							}

						}
					}
				}

			}
		} 
		else
		{
	
			if (chek.strOrigName.size()>0)
			{
				if(chek.Obj == Obj_Self && m_pParam->pUser)
				{
					value1 = m_pParam->pUser->FindGoodsByName(chek.nPlace,chek.strOrigName.c_str());		

				}
				else if(chek.Obj == Obj_Dest && pDest)
				{
					value1 = pDest->FindGoodsByName(chek.nPlace,chek.strOrigName.c_str());
				}
			} 
			else
			{
				if(chek.Obj == Obj_Self && m_pParam->pUser)
				{
					
							value1 = m_pParam->pUser->FindGoodsByType(chek.nPlace,chek.nType); 			
				}
				else if(chek.Obj == Obj_Dest && pDest)
				{					
					value1 = pDest->FindGoodsByType(chek.nPlace,chek.nType);
				}

			}
		}
		
		

		if(chek.strVariaName!="")
		{
			//  [5/4/2009 chenxianj]		
				chek.nValue = m_pParam->pVariableList->GetVarValue(chek.strVariaName.c_str(),0);		
			//  [5/4/2009 chenxianj]

			
		}
		if(!chek.pCmpFun(value1,chek.nValue))
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConBuff::CConBuff(stModuParam *pParam)
:CCondition(pParam)
{
}

CConBuff::~CConBuff(void)
{
}

bool CConBuff::Result(CMoveShape* pDest)
{
	int nRet = 0;
	itCheck it = m_Checkes.begin();
	for(;it != m_Checkes.end();it++)
	{
		tagCheck &check  = (*it);
		
		if (check.nLevel==0 && check.nNum==0)
		{
			if(check.Obj == Obj_Self && m_pParam->pUser)
				nRet = m_pParam->pUser->GetStateParamByID(check.nBuffID,"Num");
			else if(check.Obj == Obj_Dest && pDest)
				nRet = pDest->GetStateParamByID(check.nBuffID,"Num");
			if(nRet <= 0)	return false;
		}
		else if (check.nLevel>0)
		{
			int nNum = 0;
			if(check.Obj == Obj_Self && m_pParam->pUser)
			{
				nNum = m_pParam->pUser->GetStateParamByID(check.nBuffID,"Num");
				nRet = m_pParam->pUser->GetStateParamByID(check.nBuffID,"Lv");
			}
			else if(check.Obj == Obj_Dest && pDest)
			{
				nRet = pDest->GetStateParamByID(check.nBuffID,"Lv");
				nNum = pDest->GetStateParamByID(check.nBuffID,"Num");
			}
			if (nRet<check.nLevel && nNum>0)
				return false;
		}
		if (check.nNum>0)
		{
			if(check.Obj == Obj_Self && m_pParam->pUser)
				nRet = m_pParam->pUser->GetStateParamByID(check.nBuffID,"Num");
			else if(check.Obj == Obj_Dest && pDest)
				nRet = pDest->GetStateParamByID(check.nBuffID,"Num");
			if (nRet<check.nNum)
				return false;
		}		
	}
	return true;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConDistance::CConDistance(stModuParam *pParam)
:CCondition(pParam)
{
}

CConDistance::~CConDistance(void)
{
}


bool CConDistance::Result(CMoveShape* pDest)
{
	if(NULL == pDest)	return false;	
	
	//作用对象是自己就不进行距离判断
	list<stDestObj*>::iterator it = m_pParam->OperObjs.begin();
	for(;it!=m_pParam->OperObjs.end();++it)
	{
		if ((*it)->DestID==m_pParam->pUser->GetExID() && (*it)->nDestType==m_pParam->pUser->GetType() && m_pParam->OperObjs.size()==1)
		{
			return true;
		}
	}

	int value=0;
	//得到相距的距离
	if (m_refObj == Obj_Mouse)
	{
		//
		value = Distance(m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),m_pParam->nMouseX,m_pParam->nMouseY); 
		if(m_strVariaName!="")
		{
			//  [5/4/2009 chenxianj]		
			m_nValue = m_pParam->pVariableList->GetVarValue(m_strVariaName.c_str(),0); 	
			//  [5/4/2009 chenxianj]
			
		}
	}
	else if (m_refObj == Obj_Firer)
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{
			CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nFirerType,m_pParam->FirerID);
			if(NULL==pObj)
				return false;
			CMoveShape *pMoveShape = dynamic_cast<CMoveShape*>(pObj);
			value = m_pParam->pUser->RealDistance(pMoveShape);
		}
	}
	else
	{
		value = m_pParam->pUser->RealDistance(pDest);
		if(m_strVariaName!="")
		{
			//  [5/4/2009 chenxianj]		
			m_nValue = m_pParam->pVariableList->GetVarValue(m_strVariaName.c_str(),0);			
			//  [5/4/2009 chenxianj]
				
		}
	}
	if (value<0)
	{
		value = 0;
	}	
	return m_CmpFun(value,m_nValue);
}

///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
CConScript::CConScript(stModuParam *pParam)
:CCondition(pParam)
{
}


CConScript::~CConScript(void)
{
}

bool CConScript::Result(CMoveShape* pDest)
{	
	//  [5/4/2009 chenxianj]
	CScript *pTempScript =  PopFreeScript(m_strContent.c_str());	
	//  [5/4/2009 chenxianj]

	if(pTempScript)
	{
		pTempScript->SetVariableList(m_pParam->pVariableList);	
		pTempScript->SetUsedItemID(m_pParam->GoodsGuid);
		pTempScript->SetSrcShapeEx(m_pParam->pUser);
		if(m_pParam->pUser)
			pTempScript->SetRegion(dynamic_cast<CRegion*>(m_pParam->pUser->GetFather()));
		else
			pTempScript->SetRegion(NULL);	
		
		pTempScript->RunStr(pDest,m_strContent.c_str());
		PushFreeScript(pTempScript);

		int nRet = m_pParam->pVariableList->GetVarValue("$Ret",0); 

		if(nRet==1)
		{
			return true;
		}
		else
		{
			return false;
		}	
	}
	return false;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConValidObj::CConValidObj(stModuParam *pParam)
:CCondition(pParam)
{
}
CConValidObj::~CConValidObj(void)
{
}

bool CConValidObj::Result(CMoveShape* pDest)
{
	string str = "Country";
	list<eValidObjType>::iterator it = m_lValidObjType.begin();
	if (m_lType == ePlayer_Type)
	{		
		for (;it!=m_lValidObjType.end();++it)
		{
			if ((*it)==SelfPlayer_Type)	 //
			{
				//自已
				if (pDest==m_pParam->pUser)
				{
					return true;
				}
			}
			else if ((*it)==EnemyPlayer_Type)
			{
				//敌国
				if (pDest->GetType()==TYPE_PLAYER)
				{
					if (m_pParam->pUser->GetAttribute(str)!=pDest->GetAttribute(str))
					{
						return true;
					}
				}
			}
			else if ((*it)==NormalPlayer_Type)
			{
				//表示本国普通玩家
				if (pDest->GetType()==TYPE_PLAYER)
				{
					if (m_pParam->pUser->GetAttribute(str)==pDest->GetAttribute(str) && m_pParam->pUser!=pDest)
					{
						CPlayer *pPlayer = dynamic_cast<CPlayer*>(pDest);
						if (pPlayer->GetPkValue()==0)
						{
							return true;
						}
					}
				}				
			}
			else if ((*it)==RedPlayer_Type)
			{
				//表示本国红名玩家
				if (pDest->GetType()==TYPE_PLAYER)
				{
					if (m_pParam->pUser->GetAttribute(str)==pDest->GetAttribute(str))
					{
						CPlayer *pPlayer = dynamic_cast<CPlayer*>(pDest);
						if (pPlayer->GetPkValue()>20)
						{
							return true;
						}
					}
				}		
			}
			else if ((*it)==PurplePlayer_Type)
			{
				//表示本国紫名玩家
				if (pDest->GetType()==TYPE_PLAYER)
				{
					if (m_pParam->pUser->GetAttribute(str)==pDest->GetAttribute(str))
					{
						CPlayer *pPlayer = dynamic_cast<CPlayer*>(pDest);
						if (pPlayer->GetPkValue()>0 && pPlayer->GetPkValue()<=20)
						{
							return true;
						}
					}
				}		
			}
			else if ((*it)==TeamPlayer_Type)
			{
				//队伍
				if (pDest->GetType()==TYPE_PLAYER && m_pParam->pUser->GetType()==TYPE_PLAYER)
				{
					//
					CPlayer *pDestPlayer = dynamic_cast<CPlayer*>(pDest);
					CPlayer *pUser = dynamic_cast<CPlayer*>(m_pParam->pUser);
					if (pDestPlayer->GetTeamID()!=NULL_GUID)
					{
						if (pDestPlayer->GetTeamID()==pUser->GetTeamID())
						{
							return true;
						}
					}
				}
			}
			else if ((*it)==TongPlayer_Type)
			{
				//帮会
				if (pDest->GetType()==TYPE_PLAYER && m_pParam->pUser->GetType()==TYPE_PLAYER)
				{
					//
					CPlayer *pDestPlayer = dynamic_cast<CPlayer*>(pDest);
					CPlayer *pUser = dynamic_cast<CPlayer*>(m_pParam->pUser);
					if (pDestPlayer->GetFactionID()!=NULL_GUID)
					{
						if (pDestPlayer->GetFactionID()==pUser->GetFactionID())
						{
							return true;
						}
					}
				}

			}
			else if ((*it)==LeaguePlayer_Type)
			{
				//同盟
				if (GetPKSys()->IsSameCamp(pDest,m_pParam->pUser) && pDest->GetType()==TYPE_PLAYER)
				{
					return true;
				}
			}
		}
	} 
	else if (m_lType == eGate_Type)
	{
		if (pDest->GetType() == TYPE_CITYGATE)
		{
			return true;
		}
	}
	else if(m_lType == eMonster_Type)
	{
		//怪物类型
		if (pDest->GetType()==TYPE_MONSTER)
		{
			CMonster *pMonster = dynamic_cast<CMonster*>(pDest);
			for (;it!=m_lValidObjType.end();++it)
			{
			

				if ((*it)==Monster_Type)
				{
					if (NT_Monster==pMonster->GetNpcType())
					{
						return true;
					}					
				}
				else if ((*it)==Npc_Type)
				{
					if (NT_Normal==pMonster->GetNpcType())
					{
						return true;
					}
				}
				else if ((*it)==Guard_Type)
				{
					if (NT_Guard==pMonster->GetNpcType())
					{
						return true;
					}
				}
				else if((*it)==SMonster_Type)
				{
					if (NT_S_Monster==pMonster->GetNpcType())
					{
						return true;
					}
				}
			}
		}
	}
	else if (m_lType==ePet_Type)
	{
		if (pDest->GetType()==TYPE_PET)
		{
			return true;
		}
	}
	return false;
		
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConTimer::CConTimer(stModuParam *pParam)
:CCondition(pParam)
{
}

CConTimer::~CConTimer(void)
{
}

bool CConTimer::Result(CMoveShape* pDest)
{
	stTimerParam *pTimerParam = m_pParam->FindTimerParam(m_pParam->lCurTimerID);
	if(pTimerParam)
	{
		if(pTimerParam->lFlag == m_lFlag)
			return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConEvent::CConEvent(stModuParam *pParam)
:CCondition(pParam)
{
}

CConEvent::~CConEvent(void)
{
}

bool CConEvent::Result(CMoveShape* pDest)
{
	if(m_lEventType != m_pParam->lCurEventType)
		return false;
	if(m_pParam->pEventVar)
	{
		if( m_lVar1!=0 && m_lVar1 != m_pParam->pEventVar->lVar1)
			return false;
		if(m_lVar2!=0 &&  m_lVar2 != m_pParam->pEventVar->lVar2)
			return false;
		if(m_lVar3!=0 && m_lVar3!=m_pParam->pEventVar->lVar3)
			return false;
		if (m_lVar4!=0 && m_lVar4!=m_pParam->pEventVar->lVar4)
			return false;
		
		
		if( m_strVar3 !="" && 
			m_strVar3 != (char*)m_pParam->pEventVar->pVar)
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CConCooldown::CConCooldown(stModuParam *pParam)
:CCondition(pParam)
{
}

CConCooldown::~CConCooldown(void)
{
}

//得到条件结果
bool CConCooldown::Result(CMoveShape* pDest)
{
	if(NULL==pDest)	return false;

	return  pDest->CooldownPast(m_pParam->nID);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//场景类型判断
CConRegion::CConRegion(stModuParam *pParam)
:CCondition(pParam)
{
}

CConRegion::~CConRegion(void)
{
}

bool CConRegion::Result(CMoveShape* pDest)
{
	CServerRegion *pRegion =dynamic_cast<CServerRegion*>(pDest->GetFather());
	if(pRegion->GetAttribute((char*)(m_strName.c_str()))==m_nRegionValue)
	{
		return true;
	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//判断面向条件类
CConDir::CConDir(stModuParam *pParam):CCondition(pParam)
{
}
CConDir::~CConDir()
{
}
//根据条件得到面向结果
bool CConDir::Result(CMoveShape* pDest)
{
	if (NULL==pDest || pDest==m_pParam->pUser)
	{
		return false;
	}
	int nDestToSrcDir = GetLineDir(pDest->GetTileX(),pDest->GetTileY(),m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY());
	
	int nBackDir[3];
	for (int i=0; i<3; ++i)
	{
		nBackDir[i] = pDest->GetDir() + 3 + i;
		if (nBackDir[i]>7)
		{
			nBackDir[i] = nBackDir[i] - 8;
		}
	}		
	if ((nBackDir[0]==nDestToSrcDir || nBackDir[1]==nDestToSrcDir || nBackDir[2]==nDestToSrcDir) && m_nFaceType==eFace_Back)
	{
		//背面
		return true;
	} 
	else if((nBackDir[0]!=nDestToSrcDir && nBackDir[1]!=nDestToSrcDir && nBackDir[2]!=nDestToSrcDir) && m_nFaceType==eFace_Front)
	{
		//正面
		return true;
	}
	else
	{
		return false;
	}

	
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//判断阻挡条件类
CConBlock::CConBlock(stModuParam *pParam)
:CCondition(pParam)
{
}

CConBlock::~CConBlock(void)
{
}

//没有任何阻挡判断
bool CConBlock::NoAnyBlock(BYTE bBlock)
{
	if(bBlock != CRegion::BLOCK_NO)	return false;
	return true;
}

//没有飞行阻挡判断
bool CConBlock::NoFullBlock(BYTE bBlock)
{
	if(bBlock == CRegion::BLOCK_UNFLY)	
		return false;
	return true;
}
//没有地面阻挡
bool NoHalfBlock(BYTE bBlock)
{
	if(bBlock== CRegion::BLOCK_CANFLY)	
		return false;
	return true;
}

//没有半阻挡判断
bool CConBlock::NoShapeBlock(BYTE bBlock,long lType)
{
	if((bBlock == CRegion::BLOCK_SHAPE && m_nShapeType == lType) || (m_NoBlockType==CRegion::BLOCK_SHAPE && m_nShapeType == lType) )	
		return false;
	return true;
}

//得到条件结果
bool CConBlock::Result(CMoveShape* pDest)
{
	//如果没有目标或者目标等于自己，返回true|| m_pParam->pUser == pDest
	if(NULL==pDest)	return true;
	if(m_pParam->pUser->GetFather() != pDest->GetFather() )	return false;

	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(pServerRegion)
	{
		if (m_nType==e_Line)
		{
			list<POINT> listPos;
			if (m_nDir==Direct_MousePos)
			{
				long lDir = m_pParam->pUser->GetDestDir(m_pParam->pUser->GetTileX(),
					m_pParam->pUser->GetTileY(),
					m_pParam->nMouseX,m_pParam->nMouseY);
				long lDistance = m_pParam->pUser->Distance(m_pParam->nMouseX,m_pParam->nMouseY);
				
				//得到与鼠标点之间的格子
				GetLinePointToDest(listPos,m_pParam->pUser->GetTileX(),
					m_pParam->pUser->GetTileY(),m_pParam->nMouseX,m_pParam->nMouseY,lDistance+1);
			}
			else
			{
				long lDir = m_pParam->pUser->GetDestDir(m_pParam->pUser->GetTileX(),
					m_pParam->pUser->GetTileY(),
					pDest->GetTileX(),pDest->GetTileY());
				long lDistance = m_pParam->pUser->Distance(pDest->GetTileX(),pDest->GetTileY());
				
				//得到与目标之间的格子
				GetLinePointToDest(listPos,m_pParam->pUser->GetTileX(),
					m_pParam->pUser->GetTileY(),pDest->GetTileX(),pDest->GetTileY(),lDistance);
			}


			list<POINT>::iterator it = listPos.begin();
			for(;it != listPos.end();it++)
			{
				POINT& pos = (*it);
				CRegion::tagCell* pCell = pServerRegion->GetCell(pos.x,pos.y);
				


				if(pCell==NULL)	
					return false;
				if((m_NoBlockType&BT_NoAnyBlock) && !NoAnyBlock(pCell->bBlock))
					return false;
				if((m_NoBlockType&BT_NoFullBlock) && !NoFullBlock(pCell->bBlock))
					return false;

				CMoveShape *pShape = dynamic_cast<CMoveShape*>(pServerRegion->GetShape(pos.x,pos.y));
				long lShapeType = 0;
				if (pShape)
				{
					lShapeType = pShape->GetType();
				}				
				if((m_NoBlockType&BT_NoShapeBlock) && !NoShapeBlock(pCell->bBlock,lShapeType))
					return false;
			}

		} 
		else if(m_nType==e_Point)
		{
			LONG x=0,y=0;
			

			if (m_nDir==DIREC_Front)
			{	
				//前面
				long lDistance = m_pParam->pUser->RealDistance(m_pParam->pMouseShape);
				if (lDistance<0)
				{
					lDistance = 0;
				}
				GetLineFront(x,y,m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
					m_pParam->pMouseShape->GetTileX(),m_pParam->pMouseShape->GetTileY(),lDistance);
			
			}
			else if (m_nDir==DIRECT_Back)
			{	
				//后面				
				GetLineBack(x,y,m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
					m_pParam->pMouseShape->GetTileX(),m_pParam->pMouseShape->GetTileY(),m_pParam->pMouseShape->GetFigure());
			}
			else if (m_nDir==Direct_MousePos)
			{
				x = m_pParam->nMouseX;
				y = m_pParam->nMouseY;
			}
			CRegion::tagCell* pCell = pServerRegion->GetCell(x,y);
			if(pCell==NULL)	return false;
			if((m_NoBlockType&BT_NoAnyBlock) && !NoAnyBlock(pCell->bBlock))
				return false;
			if((m_NoBlockType&BT_NoFullBlock) && !NoFullBlock(pCell->bBlock))
				return false;

			CMoveShape *pShape = dynamic_cast<CMoveShape*>(pServerRegion->GetShape(x,y));
			long lShapeType = 0;
			if (pShape)
			{
				lShapeType = pShape->GetType();
			}				
			if((m_NoBlockType&BT_NoShapeBlock) && !NoShapeBlock(pCell->bBlock,lShapeType))
				return false;
		}
		
	}
	return true;
}



//下边是比较操作
bool Less(int val1,int val2)
{
	return val1<val2;
}
bool Equal(int val1,int val2)
{
	return val1==val2;
}
bool LessEqual(int val1,int val2)
{
	return val1<=val2;
}
bool Greater(int val1,int val2)
{
	return val1>val2;
}
bool GreaterEqual(int val1,int val2)
{
	return val1>=val2;
}
bool NotEqual(int val1,int val2)
{
	return val1!=val2;
}

