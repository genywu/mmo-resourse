#pragma once
#include "stdafx.h"

#include "Message.h"
#include "Player.h"
#include "Monster.h"
#include "ClientRegion.h"
#include "PetSystem/Pet.h"
#include "../GameClient/Game.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void OnPetMessage( CMessage *pMsg )
{	
	CPlayer* pPlayer = pMsg -> GetPlayer();
	CClientRegion *pRegion = (CClientRegion *)pMsg->GetRegion();
	if( pPlayer == NULL )
	{
		return;
	}

	switch(pMsg -> GetType())
	{
        //宠物改变状态
    case MSG_S2C_PET_CHANGE_STATE:
        {
            CGUID petGuid;
            pMsg->GetGUID(petGuid);
            PET_STATE petState = (PET_STATE)pMsg->GetLong();

            CPet* pet = pPlayer->GetPet(petGuid);
            if (pet)
                pet->SetPetState(petState);
        }
        break;

	case MSG_S2C_PET_CHANGE_MASTER:
		{
			//CGUID MonsterID,MasterID;
			//LONG lMonsterType=pMsg->GetLong();
			//pMsg->GetGUID(MonsterID);
			//LONG lMasterType=pMsg->GetLong();
			//pMsg->GetGUID(MasterID);
			//char strName[17];
			//pMsg->GetStr(strName,17);
			//DWORD dwLevel=pMsg->GetDWord();
			//DWORD dwExp=pMsg->GetDWord();
			//DWORD dwHp=pMsg->GetDWord();
			//DWORD dwMaxHp = pMsg->GetDWord();
			//CMonster *pMonster=(CMonster*)(pRegion->FindChildObject(lMonsterType,MonsterID));
			//if(pMonster)
			//{
			//	pMonster->GetPetInfo()->lMasterType=lMasterType;
			//	pMonster->GetPetInfo()->MasterID=MasterID;
			//	pMonster->GetPetInfo()->dwExp=dwExp;
			//	pMonster->GetPetInfo()->dwLevel=dwLevel;
			//	memcpy(pMonster->GetPetInfo()->strMasterName,strName,sizeof(strName));
			//	pMonster->SetHp( dwHp );
			//	pMonster->SetMaxHp( dwMaxHp );
			//}
			//if( MasterID == GetGame()->GetMainPlayer()->GetExID() )
			//{
			//	GetGame()->GetMainPlayer()->GetPetIDSet()->clear();
			//	GetGame()->GetMainPlayer()->GetPetIDSet()->insert( MonsterID );
			//}
			//else
			//{
			//	GetGame()->GetMainPlayer()->GetPetIDSet()->erase(MonsterID );
			//}
		}
		break;
		// 创建宠物
	case MSG_S2C_PET_CREATE_PET:
		{
			long lType = pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			pPlayer->SetCurPetGUID(guid);
			long lPicID = pMsg->GetLong();

			CPet* pet = pPlayer->CreatePet(guid);
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			pet->DecodeAttrFromDataBlock(setReadDB);

			// 界面添加
			FireUIEvent("Servant/Pets", "AddPet");
		}
		break;
		// 删除宠物
	case MSG_S2C_PET_CANCEL:
		{
			BYTE b = pMsg->GetByte();
			if (b)
			{
				CGUID guid;
				pMsg->GetGUID(guid);
				pPlayer->DelPet(guid);

				// 界面更新
				FireUIEvent("Servant/Pets", "DelPet");
			}
		}
		break;
	case MSG_S2C_PET_CHANGE_MODE:
		{
			DWORD dwMode=pMsg->GetDWord();
			//GetGame()->GetInterface()->GetPetBar()->SetCurrentPetMode(dwMode);
			break;
		}
		//case MSG_S2C_PET_CHANGE_EXPERIENCE:
		//	{
		//		//LONG lMonsterType=pMsg->GetLong();
		//		//CGUID MonsterID;
		//		//pMsg->GetGUID(MonsterID);
		//		//DWORD dwLevel=pMsg->GetDWord();
		//		//DWORD dwExp=pMsg->GetDWord();
		//		//DWORD dwMaxHp=pMsg->GetDWord();
		//		//DWORD dwHp=pMsg->GetDWord();
		//		//CMonster *pMonster=(CMonster*)(pRegion->FindChildObject(lMonsterType,MonsterID));
		//		//if(pMonster)
		//		//{
		//		//	pMonster->GetPetInfo()->dwExp=dwExp;
		//		//	pMonster->GetPetInfo()->dwLevel=dwLevel;
		//		//	pMonster->SetHp(dwHp);
		//		//	pMonster->SetMaxHp(dwMaxHp);
		//		//}
		//	}
		//	break;
	case MSG_S2C_PET_AI:
		{
			CGUID PetGuid;
			pMsg->GetGUID(PetGuid);

			long lOpType = pMsg->GetLong();

			CClientRegion *pRegion = GetGame()->GetRegion();
			CPet *pPet = dynamic_cast<CPet*>(pRegion->FindChildObject(TYPE_PET,PetGuid));
			if (pPet == NULL)
			{
				//MessageBox(g_hWnd,"没宠物的情况下 收到了寻怪消息。MSG_S2C_PET_AI ","Info",MB_OK);
				return;
			}
			switch( lOpType )
			{
			case PET_OT_AI_SET_TARGET:		//自动寻敌  (主动攻击模式)
				{
					long lTargetType = pMsg->GetLong();

					CGUID TargetGuid;
					pMsg->GetGUID(TargetGuid);

					if (lTargetType == 0 || pRegion == NULL)
					{
						pPet->SetCurAttackShape(NULL);
						return;	
					}
					//找到执行AI的宠物
					
					CMoveShape*	pTargetShpae = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(lTargetType,TargetGuid));
					pPet->SetCurAttackShape(pTargetShpae);
					if (pTargetShpae)
					{
						if (pTargetShpae != pPet->GetCurAttackShape())
						{
							pPet->UpdateTargetPos(pTargetShpae);
							pPet->SetState(CShape::STATE_FIGHT);
						}
					}
					else
						pPet->SetCurAttackShape(TargetGuid);
				}
				break;
			//case PET_OT_DETECT:
			//	{

			//	}
			//	break;
			case PET_OT_AI_ADD_SKILL:		//开启技能插槽
				{
					long lPos = pMsg->GetLong();
					long lColor = pMsg->GetLong();
					
					if (pPet->IsMainPlayerPet())
					{
						//CPetPageEx *pPage = GetGame()->GetCGuiEx()->GetPetPageEx();
						//pPage->EnabledNewSkillSlot(pPet,lPos,lColor);
					}
				}
				break;
			default:
				break;
			}

		}
		break;
	}
}