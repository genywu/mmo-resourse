#include "stdafx.h"
#include "..\nets\networld\message.h"
#include "..\player.h"
#include "..\..\WorldServer\Game.h"
#include "../DBentity/entityManager.h"
#include "../GlobalRgnManager.h"
#include "../WorldRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnRegionDetailSaveMessage(CMessage *pMsg)
{
	char pszSaveInfo[512]="";
	switch(pMsg->GetType())
	{
	case MSG_S2W_REGION_DETAIL_SAVE_PROPERTY:
		{
			//CGUID regionGUID;
			//pMsg->GetGUID(regionGUID);
			//tagRegionParam stRegionParam;
			//pMsg->GetEx(&stRegionParam,(long)sizeof(tagRegionParam));
			//CWorldRegion* pRegion=GetGame()->GetGlobalRgnManager()->FindRgnByGUID(regionGUID);
			//CEntityManager::DBEntityComponentMapItr rgnMapItr = GetGame()->GetEntityManager()->GetBaseEntityMap().find(COM_REGIONGROUP);
			//if(rgnMapItr == GetGame()->GetEntityManager()->GetBaseEntityMap().end()) return;

			//CEntity* pDBRegion=NULL;
			//map<CGUID, CBaseEntity*>::iterator rgnItr = rgnMapItr->second.find(regionGUID);
			//if(rgnItr == rgnMapItr->second.end())
			//{
			//	pDBRegion=GetGame()->GetEntityManager()->NewBaseEntity(COM_REGION, regionGUID, COM_REGION, regionGUID);
			//	rgnMapItr->second[regionGUID] = pDBRegion;
			//	memcpy(&(pRegion->GetRegionParam()),&stRegionParam,sizeof(tagRegionParam));

			//	pDBRegion->SetGuidAttr(string("RegionID"), 0, pRegion->GetExID());
			//	pDBRegion->SetGuidAttr(string("OwnedFactionID"), 0, pRegion->GetRegionParam().OwnedFactionID);
			//	pDBRegion->SetGuidAttr(string("OwnedUnionID"), 0, pRegion->GetRegionParam().OwnedUnionID);
			//	pDBRegion->SetLongAttr(string("CurTaxRate"), 0, pRegion->GetRegionParam().lCurrentTaxRate);

			//	pDBRegion->SetLongAttr(string("TodayTotalTax"), 0, pRegion->GetRegionParam().dwTodayTotalTax);
			//	pDBRegion->SetLongAttr(string("TotalTax"), 0, pRegion->GetRegionParam().dwTotalTax);
			//	pDBRegion->SetLongAttr(string("BusinessTotalTax"), 0, pRegion->GetRegionParam().dwBusinessTotalTax);
			//	pDBRegion->SetLongAttr(string("BusinessTaxRate"), 0, pRegion->GetRegionParam().lBusinessTaxRate);
			//	pDBRegion->SetLongAttr(string("YuanbaoTaxRate"), 0, pRegion->GetRegionParam().lYuanbaoTaxRate);

			//	_snprintf(pszSaveInfo,512,"----DETAIL SAVE Region ID: %d------", pRegion->GetID());
			//	PutStringToFile("Save_WS_Info",pszSaveInfo);

			//	GetGame()->GetEntityManager()->CreateCreateRegionSession(pDBRegion);
			//}
			//else
			//{
			//	pDBRegion = (CEntity*)rgnItr->second;
			//	memcpy(&(pRegion->GetRegionParam()),&stRegionParam,sizeof(tagRegionParam));

			//	pDBRegion->SetGuidAttr(string("RegionID"), 0, pRegion->GetExID());
			//	pDBRegion->SetGuidAttr(string("OwnedFactionID"), 0, pRegion->GetRegionParam().OwnedFactionID);
			//	pDBRegion->SetGuidAttr(string("OwnedUnionID"), 0, pRegion->GetRegionParam().OwnedUnionID);
			//	pDBRegion->SetLongAttr(string("CurTaxRate"), 0, pRegion->GetRegionParam().lCurrentTaxRate);

			//	pDBRegion->SetLongAttr(string("TodayTotalTax"), 0, pRegion->GetRegionParam().dwTodayTotalTax);
			//	pDBRegion->SetLongAttr(string("TotalTax"), 0, pRegion->GetRegionParam().dwTotalTax);
			//	pDBRegion->SetLongAttr(string("BusinessTotalTax"), 0, pRegion->GetRegionParam().dwBusinessTotalTax);
			//	pDBRegion->SetLongAttr(string("BusinessTaxRate"), 0, pRegion->GetRegionParam().lBusinessTaxRate);
			//	pDBRegion->SetLongAttr(string("YuanbaoTaxRate"), 0, pRegion->GetRegionParam().lYuanbaoTaxRate);

			//	_snprintf(pszSaveInfo,512,"----DETAIL SAVE Region ID: %d------", pRegion->GetID());
			//	PutStringToFile("Save_WS_Info",pszSaveInfo);

			//	GetGame()->GetEntityManager()->CreateSaveRegionSession(pDBRegion);
			//}
		}
		break;
	default:
		break;
	}
}