#include "StdAfx.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../Pet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnPetMessage(CMessage *pMsg )
{	
	CPlayer* pPlayer = pMsg->GetPlayer();
	if(NULL == pPlayer || pPlayer->IsDied())
	{
		return;
	}

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(pMsg->GetRegion());
	if( NULL == pRegion )
	{
		return;
	}

	switch( pMsg->GetType() )
	{
	case MSG_C2S_PET_MOVE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			// 玩家不存在该宠物
			if( NULL == pet ) return;
			float fCurX = pMsg->GetFloat();
			float fCurY = pMsg->GetFloat();
			float fDesX = pMsg->GetFloat();
			float fDesY = pMsg->GetFloat();
			pet->OnQuestMoveStep(fCurX, fCurY, fDesX, fDesY);
		}
		break;

	case MSG_C2S_PET_STOP:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			// 玩家不存在该宠物
			if( NULL == pet ) return;
			float fDesX = pMsg->GetFloat();
			float fDesY = pMsg->GetFloat();
			short sDir = pMsg->GetShort();
			pet->OnQuestStopMove(fDesX, fDesY, sDir);
		}
		break;

	case MSG_C2S_PET_CHANGE_MODE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			// 玩家不存在该宠物
			if( NULL == pet ) return;
			long lMoveType = pMsg->GetLong();
			pet->SetMoveType(lMoveType);
		}
		break;

	case MSG_C2S_PET_CHANGE_ACTION:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			// 玩家不存在该宠物
			if( NULL == pet ) return;
			long lType = pMsg->GetLong();
			pet->SetFightType(lType);
		}
		break;

	case MSG_C2S_PET_CHANGE_SPEED:
		{
			//long lType = pMsg->GetLong();
			//float fSpeed = pMsg->GetFloat();
			//pPet->ChangeSpeed(lType);
		}
		break;

	case MSG_C2S_PET_HOUSE:
		{
			//GetInst(CPetCtrl).DelPet(pPet, PET_OT_HOUSE);
		}
		break;
		
		// 删除宠物
	case MSG_C2S_PET_CANCEL:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CMessage msg(MSG_S2C_PET_CANCEL);
			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet )
			{
				msg.Add((BYTE)0);
			}
			CGUID sendID = pet->GetExID();
			long lState = pet->GetStateType();
			if (lState!=PET_STATE_AWAIT && lState != PET_STATE_SLEEP)
			{
				pPlayer->SendNotifyMessage("[Colour 'ffff0000']宠物状态不是待机状态，不能进行放生操作！");
				return;
			}

			if (pPlayer->DelPet(guid))
			{
				msg.Add((BYTE)1);
				msg.Add(sendID);
				msg.SendToPlayer(pPlayer->GetExID());
			}
			else
			{
				msg.Add((BYTE)0);
				msg.SendToPlayer(pPlayer->GetExID());
			}
		}
		break;
	case MSG_C2S_PET_CHANG_NAME:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			char name[64];
			pMsg->GetStr(name, 64);
			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet ) return;
			pet->SetPetName(name);
		}
		break;
		// 宠物技能镶嵌
	case MSG_C2S_PET_ADDSKILL:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			long lContainerId=pMsg->GetLong();
			long lContainerPos = pMsg->GetLong();
			long lHoleType = pMsg->GetLong();
			long lSkillPos = pMsg->GetLong();
			CGoods* pGoods=pPlayer->FindGoods(lContainerId, lContainerPos);

			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet ) return;
			
			if (pet->AddSkill(pGoods, lSkillPos, lHoleType))
			{
				// 成功消息
				//CMessage msg(MSG_S2C_PET_ADDSKILL);
				//msg.Add(...);
				//msg.Send();
			}
		}
		break;

		// 宠物升级
	case MSG_C2S_PET_UPGRADE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet ) return;
			if (pet->OnUpgradeLev())
			{
			}
		}
		break;

	case MSG_C2S_PET_UPGRADE_DLEV:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			if(  NULL == pet ) return;
			if (pet->OnUpgradeDLev())
			{
			}
		}
		break;
	case MSG_C2S_PET_CHANGE_STATE:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			long lNewState = pMsg->GetLong();
			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet ) return;

			CPet* activePet = pPlayer->GetActivePet();
			if (activePet!=NULL && lNewState==PET_STATE_ACTIVE)
			{
				activePet->ChangeStateType(PET_STATE_FIGHT);
			}
			pet->ChangeStateType(lNewState);
		}
		break;

	case MSG_C2S_PET_SET_FOCUS:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CPet* pet = pPlayer->GetPet(guid);
			if( NULL == pet ) return;

			pPlayer->SetFocusPetGUID(guid);
		}
		break;
	}
}