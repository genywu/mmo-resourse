//////////////////////////////////////////////////////////////////////////
//PKsys.cpp
//Fun:PK系统的先手攻击、杀人、死亡规则
//Author:Joe
//Create Time:2005.6.24
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "PKSys.h"
#include "Player.h"
#include "Monster.h"
#include "ServerRegion.h"
#include "Country/CountryHandler.h"
#include "Country/Country.h"

#include "../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//class CCountry;
CPKSys* CPKSys::pInstance = NULL;
CPKSys::CPKSys(void)
{
}

CPKSys::~CPKSys(void)
{
}

CPKSys* CPKSys::getInstance()
{
	if(pInstance == NULL)
	{
		pInstance = OBJ_CREATE(CPKSys);
	}
	return pInstance;
}

void CPKSys::Release()
{
	if(pInstance)
	{
		OBJ_RELEASE(CPKSys, pInstance);
	}
}

void CPKSys::SetPKValue(CMoveShape *pAttacker,CMoveShape *pBeenAttacker,int nAddSub)
{
	//主动攻击（包含使用减益技能）本国白名玩家，每次攻击将增加20点PK值，因此方法获得的PK值上限为20，若PK值已经达到或超过20，则不会再因此方法而增加。	

	if (pAttacker==NULL || pBeenAttacker==NULL)
	{
		return;
	}
	if (pAttacker->GetType()==TYPE_MONSTER)
	{
		return;		
	}
	if (pBeenAttacker->GetType()==TYPE_MONSTER)
	{
		if (((CMonster*)pBeenAttacker)->GetNpcType()!=NT_Guard)
		{
			return;
		}	
	}

	CMoveShape* pAtker = pAttacker;
	CMoveShape* pDefer = pBeenAttacker;
	if( TYPE_PET == pAtker->GetType() )
	{
		pAtker = pAtker->GetHost();
	}

	if( TYPE_PET == pDefer->GetType() )
	{
		pDefer = pDefer->GetHost();
	}

	if( NULL == pAtker || NULL == pDefer )
	{
		//PutoutLog("Pet", LT_ERROR, "...");
		return;
	}

	// [8/24/2009 chenxianj]
	//////////////////////////////////////////////////////////////////////////
	//竞技区域不进行pk值设定
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pAttacker->GetFather() );
	if( !pRegion ) 
	{
		return;
	}

	CRegion::eSecurity eSecurityType = pRegion->GetSecurity(pAttacker->GetTileX(),pAttacker->GetTileY());

	if (eSecurityType==CRegion::SECURTIY_FIGHT)
	{
		return;
	}
	//////////////////////////////////////////////////////////////////////////




	long lNum = pAtker->GetPkValue() + GlobalSetup::GetSetup()->dwPkCountPerAttack;

	//攻击本阵营PK值为0的角色（白名角色）/攻击本国或本阵营PK值大于0小于等于20的角色（紫名角色）
	if (nAddSub==2				&&
		IsSameCamp(pAtker,pDefer)&&
		(pDefer->GetPkValue()>=0 && pDefer->GetPkValue()<=20 )&&
		(pAtker->GetPkValue()<20))
	{
		if (lNum>20)
		{
			lNum = 20;
		}
		pAtker->SetPkValue(lNum);		
	}
	//对紫名、红名玩家释放增益技能，每次使用增益技能将增加20点PK值，因此方法获得的PK值上限为20，若PK值已经达到或超过20，则PK值不会再因此方法增加
	else if (nAddSub==1 && (pDefer->GetPkValue()>0) && (pAtker->GetPkValue()<20) && pAtker!=pDefer)
	{
		if (lNum>20)
		{
			lNum = 20;
		}		
		pAtker->SetPkValue(lNum);
	}
	else if (nAddSub==2 && !IsSameCamp(pAtker,pDefer) && pDefer->GetPVPValue()==0 && pDefer->GetPkValue()<=20 && pAtker->GetPVPValue()<20)
	{
		if(pRegion->GetStrongPointFlag()==false) // 未开启据点战标志
		{
			//攻击非本阵营恶名值为0且PK值小于等于20的角色
			long lNum = pAtker->GetPVPValue() + GlobalSetup::GetSetup()->dwPkCountPerAttack;
			if( lNum > (long) GlobalSetup::GetSetup()->dwPkCountPerAttack)
			{
				lNum = GlobalSetup::GetSetup()->dwPkCountPerAttack;
			}
			pAtker->SetAttribute("PVPValue",lNum);
			pAtker->UpdateAttribute();
		}
	}
	else if (nAddSub==1  && pDefer->GetPVPValue()>0 && pAtker->GetPVPValue()<20 && pAtker!=pDefer)
	{
		if(pRegion->GetStrongPointFlag()==false) // 未开启据点战标志
		{
			//对已有恶名值的角色使用增益技能&& !IsSameCamp(pAtker,pDefer)
			long lNum = pAtker->GetPVPValue() + GlobalSetup::GetSetup()->dwPkCountPerAttack;
			if (lNum>20)
			{
				lNum = 20;
			}
			pAtker->SetAttribute("PVPValue",lNum);	
			pAtker->UpdateAttribute();
		}
	}



}
bool CPKSys::IsSameCamp(CMoveShape *pAttacker,CMoveShape *pBeenAttacker)
{
	string str="Country";
	CCountry *pAttackerCountry = GetCountryHandler()->GetCountry((BYTE)pAttacker->GetAttribute(str));
	CCountry *pBeenAttackerCountry = GetCountryHandler()->GetCountry((BYTE)pBeenAttacker->GetAttribute(str));
	if(pAttackerCountry->GetCampID((BYTE)pAttacker->GetAttribute(str)) == pAttackerCountry->GetCampID((BYTE)pBeenAttacker->GetAttribute(str)))
	{
		return true;
	}
	else
	{
		return false;
	}
}
//根据同盟ID得到国家列表
void CPKSys::GetCountryList(long lCountyID,list<BYTE> &lCountryIDList)
{
	CCountry *pCountry = GetCountryHandler()->GetCountry((BYTE)lCountyID);
	pCountry->GetCountryList(pCountry->GetCampID((BYTE)lCountyID),lCountryIDList);
}
//1表示增益
//2表示减益
BOOL CPKSys::JudgeAttackRelation(CMoveShape *pAttacker,CMoveShape *pBeenAttacker,int nAddSub)
{
	if (pBeenAttacker->IsGod())
	{
		//((CPlayer*)pAttacker)->SendNotifyMessage(AppFrame::GetText("STR_WS_SKILL_FIFTEENTH"),0xffff0000,0,eNOTIFYPOS_CENTER);
		return FALSE;
	}

	if (!IsAttackAble(pAttacker,pBeenAttacker))
	{
		return FALSE;
	}
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pAttacker->GetFather() );
	if( !pRegion ) 
		return FALSE;
	//PVP
	if (pAttacker->GetType()==TYPE_PLAYER && pBeenAttacker->GetType()==TYPE_PLAYER && nAddSub==2)
	{		
		CPlayer* pPlayerAttacker	 = dynamic_cast<CPlayer*>( pAttacker );
		CPlayer* pPlayerBeenAttacker = dynamic_cast<CPlayer*>( pBeenAttacker );

		// 如果在nopk场景，玩家不能PK的
		if(0 == pRegion->IsNoPk())
			return FALSE;

		CRegion::eSecurity eBSecurityType =
			pRegion->GetSecurity(pPlayerBeenAttacker->GetTileX(),pPlayerBeenAttacker->GetTileY());
		CRegion::eSecurity eSecurityType = pRegion->GetSecurity(pPlayerAttacker->GetTileX(),pPlayerAttacker->GetTileY());
		if (eSecurityType == CRegion::SECURTIY_SAFE || eBSecurityType == CRegion::SECURTIY_SAFE)
		{
			return FALSE;
		}
		//组队
		if (pPlayerAttacker->IsPk_Team())
		{
			if (pPlayerAttacker->GetTeamID()!=NULL_GUID && pPlayerAttacker->GetTeamID()==pPlayerBeenAttacker->GetTeamID())
			{
				return FALSE;
			}
		}
		//帮会
		if (pPlayerAttacker->IsPk_Union())
		{
			if (pPlayerAttacker->GetFactionID()!=NULL_GUID && pPlayerAttacker->GetFactionID()==pPlayerBeenAttacker->GetFactionID())
			{
				return FALSE;
			}
		}
		//本国
		if (pPlayerAttacker->IsPk_Country())
		{
			//  [9/7/2009 chenxianj]
			//只保护本国白名玩家
			if (pPlayerAttacker->GetCountry()==pPlayerBeenAttacker->GetCountry() && pPlayerBeenAttacker->GetPkValue()==0)
			{
				return FALSE;
			}
		}
		//阵营
		if (pPlayerAttacker->IsPk_Camp())
		{
			//  [9/7/2009 chenxianj]
			//只保护本阵营白名玩家
			if (IsSameCamp(pPlayerAttacker,pPlayerBeenAttacker) && pPlayerBeenAttacker->GetPkValue()==0)
			{
				return FALSE;
			}
		}
		//全体
		if (pPlayerAttacker->IsPk_Normal())
		{
			if(!IsSameCamp(pPlayerAttacker,pPlayerBeenAttacker) && pPlayerBeenAttacker->GetPkValue()==0 && pPlayerBeenAttacker->GetPVPValue()==0)
			{
				return FALSE;
			}			
		}
		//红名
		if (pPlayerAttacker->IsPk_Badman())
		{
			if (pPlayerBeenAttacker->GetPkValue()>0)
			{
				return FALSE;
			}
		}
		//海盗
		if ( pPlayerAttacker->IsPk_Pirate())
		{
			if (pPlayerBeenAttacker->GetPVPValue()>0)
			{
				return FALSE;
			}
		}		
	}
	//PVM
	if (pAttacker->GetType()==TYPE_PLAYER && pBeenAttacker->GetType()==TYPE_MONSTER)
	{
		CPlayer* pPlayerAttacker	 = dynamic_cast<CPlayer*>( pAttacker );
		CMonster* pPlayerBeenAttacker = dynamic_cast<CMonster*>( pBeenAttacker );

		CRegion::eSecurity SecurityType =
			pRegion->GetSecurity(pPlayerBeenAttacker->GetTileX(),pPlayerBeenAttacker->GetTileY());

		if (SecurityType == CRegion::SECURTIY_SAFE && nAddSub==1)
		{
			//人对怪在安全区不能使用增益技能
			return FALSE;
		}	
		if (((CMonster*)pBeenAttacker)->GetNpcType()==NT_Guard)
		{
			//国家
			if (pPlayerAttacker->IsPk_Country())
			{
				if (pAttacker->GetCountry()==pBeenAttacker->GetCountry())
				{
					return FALSE;
				}
			}
			//阵营
			if (pPlayerAttacker->IsPk_Camp())
			{
				if (IsSameCamp(pPlayerAttacker,pPlayerBeenAttacker))
				{
					return FALSE;
				}
			}
			//全体
			if (pPlayerAttacker->IsPk_Normal())
			{
				return FALSE;
			}
		}	
	}
	//MVP
	if (pAttacker->GetType()==TYPE_MONSTER && pBeenAttacker->GetType()==TYPE_PLAYER)
	{
		CMonster* pPlayerAttacker	 = dynamic_cast<CMonster*>( pAttacker );
		CPlayer* pPlayerBeenAttacker = dynamic_cast<CPlayer*>( pBeenAttacker );
		CRegion::eSecurity SecurityType =
			pRegion->GetSecurity(pPlayerBeenAttacker->GetTileX(),pPlayerBeenAttacker->GetTileY());
		if (SecurityType == CRegion::SECURTIY_SAFE && nAddSub==1)
		{
			//怪对人在安全区不能使用增益技能
			return FALSE;
		}	

	}
	return TRUE;
}
//判断一个MoveShape是否可以攻击另外一个MoveShape
bool	CPKSys::IsAttackAble(CMoveShape* pAttacker,CMoveShape* pBeAttacker)
{	
	if (pAttacker->IsDied() || pBeAttacker->IsDied())
	{
		return false;
	}
	if(pAttacker == NULL && pAttacker->IsAttackAble() == false)	
		return false;
	if(pBeAttacker == NULL && pBeAttacker->IsBeAttackAble() == false)	
		return false;

	bool bRet = true;
	if(pAttacker->GetType() == TYPE_PLAYER && pBeAttacker->GetType() == TYPE_PLAYER)
	{
	}
	else if(pAttacker->GetType() == TYPE_MONSTER && pBeAttacker->GetType() == TYPE_MONSTER)
	{
	}
	else if(pAttacker->GetType() == TYPE_PLAYER && pBeAttacker->GetType() == TYPE_MONSTER)
	{
	}
	else if(pAttacker->GetType() == TYPE_MONSTER && pBeAttacker->GetType() == TYPE_PLAYER)
	{
	}
	else if(pAttacker->GetType() == TYPE_MONSTER && pBeAttacker->GetType() == TYPE_CITYGATE)
	{
	}
	else if(pAttacker->GetType() == TYPE_PLAYER && pBeAttacker->GetType() == TYPE_CITYGATE)
	{
	}
	else if(pAttacker->GetType() == TYPE_PLAYER && pBeAttacker->GetType() == TYPE_PET &&
		pAttacker != pBeAttacker->GetHost() )
	{
	}
	else if(pAttacker->GetType() == TYPE_PET && pBeAttacker->GetType() == TYPE_PLAYER &&
		pBeAttacker != pAttacker->GetHost() )
	{
	}
	else
	{
		bRet = false;
	}
	return bRet;
}

//判断是否是敌对关系
bool CPKSys::IsEnemy(CMoveShape* pShape1,CMoveShape* pShape2)
{
	return true;
}

//先手攻击
void CPKSys::OnFirstAttack(CPlayer* pAttackPlayer,CPlayer* pBeAttackedPlayer,
						   CServerRegion* pRegion,CRegion::eSecurity SecurityType)
{
	if(pAttackPlayer == NULL || pBeAttackedPlayer == NULL || pRegion == NULL)
		return;

	//在所有的情况下,如果被攻击者处于灰名或红名状态,攻击者都不会犯罪
	if(pBeAttackedPlayer->IsBadman() == true)
		return;

	switch(SecurityType)
	{
		//城战区域
	case CRegion::SECURITY_CITYWAR:
		break;
		//安全区
	case CRegion::SECURTIY_SAFE:
		break;
		//竞技区
	case CRegion::SECURTIY_FIGHT:
		break;
		//普通区
	case CRegion::SECURITY_FREE:
		{

			//如果处于城战状态
			if(IsCityWarState(pAttackPlayer,pBeAttackedPlayer) == true)
			{
				//不犯罪
			}
			//如果处于帮战状态
			else if(ISFactionWarState(pAttackPlayer,pBeAttackedPlayer) == true)
			{
				//不犯罪
			}			
			else
			{
				// 攻击他国都不会犯罪，只有攻击自己国家人才犯罪了。
				if( pAttackPlayer->GetCountry() == pBeAttackedPlayer->GetCountry() ) 
				{
					//攻击者进入犯罪
					pAttackPlayer->EnterCriminalState();
				}				
			}

		}
		break;
	}
}


//先手技能
void CPKSys::OnFirstSkill(CPlayer* pAttackPlayer,CPlayer* pBeAttackedPlayer,
						  CServerRegion* pRegion,CRegion::eSecurity SecurityType)
{
	if(pAttackPlayer == NULL || pBeAttackedPlayer == NULL || pRegion == NULL)
		return;

	//在所有的情况下,如果被攻击者处于灰名或红名状态,攻击者都不会犯罪
	if(pBeAttackedPlayer->IsBadman() == true)
		return;

	switch(SecurityType)
	{
		//城战区域
	case CRegion::SECURITY_CITYWAR:
		break;
		//安全区
	case CRegion::SECURTIY_SAFE:
		break;
		//竞技区
	case CRegion::SECURTIY_FIGHT:
		break;
		//普通区
	case CRegion::SECURITY_FREE:
		{
			//如果处于城战状态
			if(IsCityWarState(pAttackPlayer,pBeAttackedPlayer) == true)
			{
				//不犯罪
			}
			//如果处于帮战状态
			else if(ISFactionWarState(pAttackPlayer,pBeAttackedPlayer) == true)
			{
				//不犯罪
			}		
			else
			{
				//攻击者进入犯罪
				if( pAttackPlayer->GetCountry() == pBeAttackedPlayer->GetCountry() ) 
				{
					pAttackPlayer->EnterCriminalState();
				}
			}
		}
		break;
	}
}
//杀死后处理
void CPKSys::OnKill(CPlayer* pKillPlyaer,CPlayer* pBeKilledPlayer,CServerRegion* pRegion)
{
	if(pKillPlyaer == NULL || pBeKilledPlayer == NULL || pRegion == NULL)
		return;

	//在所有的情况下,如果被杀者处于灰名或红名状态,杀者都不会增加Pk数
	if(pBeKilledPlayer->IsBadman() == true)
		return;

	CRegion::eSecurity SecurityType =
		pRegion->GetSecurity(pBeKilledPlayer->GetTileX(),pBeKilledPlayer->GetTileY());

	switch(SecurityType)
	{
		//城战区域
	case CRegion::SECURITY_CITYWAR:
		break;
		//安全区
	case CRegion::SECURTIY_SAFE:
		break;
		//竞技区
	case CRegion::SECURTIY_FIGHT:
		break;
		//普通区
	case CRegion::SECURITY_FREE:
		{
			//如果处于城战状态
			if(IsCityWarState(pKillPlyaer,pBeKilledPlayer) == true)
			{
				//不增加PK数
			}
			//如果处于帮战状态
			else if(ISFactionWarState(pKillPlyaer,pBeKilledPlayer) == true)
			{
				//不增加PK数
			}			
			else
			{				
				{
					//增加PK数
					ReportMurderer(pKillPlyaer,pBeKilledPlayer);
				}
			}
		}
		break;
	}
}

//死亡后处理
void CPKSys::OnDeath(CPlayer* pPlayer,CServerRegion* pRegion)
{
	if(pPlayer == NULL || pRegion == NULL)
		return;
}

//得到死亡掉的经验
long CPKSys::GetDiedLostExp(CPlayer* pBeKilledPlayer,CServerRegion* pRegion,
							CRegion::eSecurity SecurityType)
{
	long lLostExp = 0;
	switch(SecurityType)
	{
		//城战区域
	case CRegion::SECURITY_CITYWAR:
		{
			lLostExp = (long)GlobalSetup::GetSetup()->fLossExp_War;
		}
		break;
		//安全区
	case CRegion::SECURTIY_SAFE:
		break;
		//竞技区
	case CRegion::SECURTIY_FIGHT:
		{
			lLostExp = (long)GlobalSetup::GetSetup()->fLossExp_Game;
		}
		break;
		//普通区
	case CRegion::SECURITY_FREE:
		{
			lLostExp = (long)GlobalSetup::GetSetup()->fLossExp_Normal;
		}
		break;
	}
	return lLostExp;
}
//得到死亡的爆率
float* CPKSys::GetDiedLostGoods(CPlayer* pBeKilledPlayer,long lMurdererType,const CGUID& MurdererID,CServerRegion* pRegion,
							  CRegion::eSecurity SecurityType,long lNameColor)
{
    return NULL;
}

//判断两个玩家是否出于城战状态
bool CPKSys::IsCityWarState(CPlayer* pPlayerA,CPlayer* pPlayerB)
{
	if(pPlayerA == NULL || pPlayerB == NULL)
		return false;

	//都有帮会
	if(NULL_GUID != pPlayerA->GetFactionID() && NULL_GUID != pPlayerB->GetFactionID())
	{
		//if(pPlayerA->IsCityWarEneymyFactionMemeber(pPlayerB->GetFactionID()) != false ||
		//	pPlayerB->IsCityWarEneymyFactionMemeber(pPlayerA->GetFactionID()) != false)
		//{
		//	return true;
		//}
	}
	return false;
}
//判断两个玩家是否出于敌对帮战状态
bool CPKSys::ISFactionWarState(CPlayer* pPlayerA,CPlayer* pPlayerB)
{
	if(pPlayerA == NULL || pPlayerB == NULL)
		return false;

	//都有帮会
	if(NULL_GUID != pPlayerA->GetFactionID() && NULL_GUID != pPlayerB->GetFactionID() )
	{
		//if(pPlayerA->IsEnemyFactionMember(pPlayerB->GetFactionID()) != false ||
		//	pPlayerB->IsEnemyFactionMember(pPlayerA->GetFactionID()) != false)
		//{
		//	return true;
		//}
	}
	return false;
}
// 是否在国战状态
bool CPKSys::IsCountryWarState(CPlayer* pPlayerA,CPlayer* pPlayerB)
{
	if(pPlayerA == NULL || pPlayerB == NULL)
		return false;

	return false;
}
//##报告凶手
void CPKSys::ReportMurderer(CPlayer* pMurderer,CPlayer* pBMurderer)
{
	long lNum=0;
	if( pMurderer && pBMurderer )
	{
		if( IsSameCamp(pMurderer,pBMurderer)) 
		{
			if (pBMurderer->GetPkValue()==0)
			{
				lNum = pMurderer -> GetPkValue() + GlobalSetup::GetSetup()->dwPkCountPerKill;
				if (lNum > 65535)
					lNum = 65535;
				pMurderer->SetPkValue(lNum);
			} 
			else if(pBMurderer->GetPkValue()>0 && pBMurderer->GetPkValue()<=20 && pMurderer->GetPkValue()<20)
			{
				lNum = pMurderer -> GetPkValue() + GlobalSetup::GetSetup()->dwPkCountPerAttack;
				if (lNum>20)
				{
					lNum = 20;
				}
				pMurderer->SetPkValue(lNum);
			}			
		}
		else
		{
			if (pBMurderer->GetPkValue()<=20 && pBMurderer->GetPVPValue()==0)
			{
				CServerRegion* pRgn = dynamic_cast<CServerRegion*>(pBMurderer->GetFather());
				if(pRgn && pRgn->GetStrongPointFlag()==false) // 未开启据点战标志
				{
					lNum = pMurderer->GetPVPValue() + GlobalSetup::GetSetup()->dwPkCountPerKill;
					pMurderer->SetAttribute("PVPValue",lNum);
					pMurderer->UpdateAttribute();
				}
			}
		}		
		pMurderer -> OnUpdateMurdererSign();
	}
}
