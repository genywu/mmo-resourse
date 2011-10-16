#include "stdafx.h"
#include "..\Player.h"
#include "..\ServerRegion.h"
#include "..\Pet.h"

void OnPetMessage(CMessage *pMsg )
{	
	CPlayer* pPlayer = pMsg->GetPlayer();
	if( NULL == pPlayer )
	{
		return;
	}

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(pMsg->GetRegion());
	if( NULL == pRegion )
	{
		return;
	}

	long lType = pMsg->GetLong();
	CPet* pPet = pPlayer->GetActivePet(lType);
	if( NULL == pPet )
	{
		return;
	}

	switch( pMsg->GetType() )
	{
	case MSG_C2S_PET_MOVE:
		{
			float fCurX = pMsg->GetFloat();
			float fCurY = pMsg->GetFloat();
			float fDesX = pMsg->GetFloat();
			float fDesY = pMsg->GetFloat();
			pPet->OnQuestMoveStep(fCurX, fCurY, fDesX, fDesY);
		}
		break;

	case MSG_C2S_PET_STOP:
		{
			float fDesX = pMsg->GetFloat();
			float fDesY = pMsg->GetFloat();
			short sDir = pMsg->GetShort();
			pPet->OnQuestStopMove(fDesX, fDesY, sDir);
		}
		break;

	case MSG_C2S_PET_CHANGE_MODE:
		{
			long lType = pMsg->GetLong();
			pPet->SetMoveType(lType);
		}
		break;

	case MSG_C2S_PET_CHANGE_ACTION:
		{
			long lType = pMsg->GetLong();
			pPet->SetFightType(lType);
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
			GetInst(CPetCtrl).DelPet(pPet, PET_OT_HOUSE);
		}
		break;

	case MSG_C2S_PET_CANCEL:
		{
			GetInst(CPetCtrl).DelPet(pPet, PET_OT_CANCEL);
		}
		break;
	}
}