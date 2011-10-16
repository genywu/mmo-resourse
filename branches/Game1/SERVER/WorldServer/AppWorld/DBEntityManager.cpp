#include "StdAfx.h"
#include "DBentityManager.h"
#include "..\nets\networld\Message.h"

#include "DBCard.h"
#include "DBGood.h"
#include "DBLoginPlayer.h"
#include "DBPlayer.h"
#include "DBFaction.h"
#include "DBCityWar.h"
#include "DBVillageWar.h"
#include "DBUnion.h"
#include "DBCountry.h"
#include "DBEnemyFaction.h"
#include "DBGenVar.h"
#include "DBSetup.h"
#include "DBRegion.h"
#include "dbDuprgn.h"
#include "DBRgnGoods.h"
#include "DBLinkMan.h"
#include "DBLinkManGroup.h"

#include "Player.h"
#include "goods\CGoods.h"
#include "Country\Country.h"
#include "Country\CountryHandler.h"
#include "goods\CGoodsFactory.h"
#include "goods/CGoodsBaseProperties.h"
#include "organizingsystem\faction.h"
#include "organizingsystem\union.h"
#include "cardsdealingsystem\card.h"
#include "organizingsystem\AttackCitySys.h"
#include "organizingsystem\FactionWarSys.h"
#include "organizingsystem\OrganizingCtrl.h"
#include "organizingsystem\VillageWarSys.h"
#include "Country\Country.h"
#include "script\VariableList.h"
#include "Container\CEquipmentContainer.h"
#include "Container\CSubpackContainer.h"

#include "PlayerDepot.h"

///[TEST]//////////////////////////////////////////////////////////////////////////
#include "../WorldServer/WorldServer.h"
#include "Linkman/LinkmanSystem.h"
#include "../../../public/entityproperty.h"
#include "dbgoodsgroup.h"
#include "DBRgnGoodsGroup.h"
#include "DBFactionApplyGroup.h"
#include "DBFactionMemberGroup.h"
#include "Mail/Mail.h"
#include "DBMail.h"
#include "DBMailGoods.h"
#include "DBMailGoodsGroup.h"
#include "DBMailGroup.h"
#include "DBGenVar.h"
#include "DBGenVarGroup.h"
#include "Mail/Mail.h"
#include "Mail/MailManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*************************
//输出SomeThing log信息
void PutSomeThingLogInfo(const char *msg)
{
	char strFile[MAX_PATH];
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(strFile, "log/SomeThing %04d-%02d-%02d.txt", stTime.wYear, stTime.wMonth, stTime.wDay);

	//创建一个log的保存目录
	CreateDirectory( "log",	NULL);

	FILE *fp;
	if( (fp=fopen(strFile, "a+")) == NULL )
	{
		return;
	}

	fseek(fp, 0, SEEK_END);
	fwrite(msg, lstrlen(msg), 1, fp);
	fclose(fp);
}
//*************************
// 保存日志
void SaveSomeThingLogText(bool bForce)
{
	const int lMaxLength = 64000;	// log信息最大长度
	static char str[lMaxLength];

	// 写日志文件
	static DWORD dwLastSaveTime = timeGetTime();

	if (bForce ||											// 强制写
		timeGetTime() - dwLastSaveTime > GetGame()->GetSetup()->dwSaveInfoTime ||		// 大于
		GetWindowTextLength(g_hLogText) >= lMaxLength		// 内容超过64K
		)
	{
		dwLastSaveTime = timeGetTime();

		PutSomeThingLogInfo("\r\n=============================== Start SomeThing Save Log ===============================\r\n");

		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d\r\n\r\n", stTime.wYear, stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);
		PutSomeThingLogInfo(str);

		GetWindowText(g_hInfoText, str, lMaxLength);
		PutSomeThingLogInfo(str);

		PutSomeThingLogInfo("\r\n");

		//	GetWindowText(g_hLogText, str, lMaxLength);
		//	PutLogInfo(str);
		SetWindowText(g_hLogText, "");

		PutSomeThingLogInfo("================================ End SomeThing Save Log ================================\r\n");
	}
}

// 加入一条记录
void AddSomeThingLogText(char* msg, ...)
{
	const int lMaxLength = 64000;	// log信息最大长度
	static char str[lMaxLength];

	// 写日志文件
	SaveSomeThingLogText(false);

	// 写服务器界面
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(str, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va,msg);
	vsprintf(&str[strlen(str)],msg,va);
	va_end(va);

	strcat(str, "\r\n");

	PutSomeThingLogInfo(str);

	SendMessage(g_hLogText, EM_SETSEL, 0, -1);
	SendMessage(g_hLogText, EM_SETSEL, -1, 0);
	SendMessage(g_hLogText, EM_REPLACESEL, FALSE, (LPARAM)str);
}


CDBEntityManager::tagLoginInfo::tagLoginInfo(void)
:lSignCode(0),lLastLoginTime(0)
{

}
CDBEntityManager::tagLoginInfo::~tagLoginInfo(void)
{

}

CDBEntityManager::CDBEntityManager()
{
	m_mapDBCards.clear();
	m_mapDBPlayers.clear();
	m_mapDBFactions.clear();
	//m_mapDBCityWars.clear();

	//m_mapDBVillageWars.clear();
	m_mapDBUnions.clear();
	m_mapDBCountrys.clear();
	//m_mapDBEnemyFactions.clear();
	m_mapDBGenVars.clear();
	m_mapDBSetups.clear();
	m_mapDBRegions.clear();

	m_pTmpGenVarGroup = new CDBGenVarGroup(NULL_GUID);
}
CDBEntityManager::~CDBEntityManager()
{
	SAFE_DELETE(m_pTmpGenVarGroup);

	std::map<string, CDataEntityManager*>::iterator deItr = m_mapObjAttrDef.begin();
	for(; deItr != m_mapObjAttrDef.end(); deItr++)
	{
		delete deItr->second;
	}
	m_mapObjAttrDef.clear();

	DBCardMapItr itr = m_mapDBCards.begin();
	for(; itr != m_mapDBCards.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}
	m_mapDBCards.clear();
	DBPlayerMapItr itr2 = m_mapDBPlayers.begin();
	for(; itr2 != m_mapDBPlayers.end(); itr2++)
	{
		SAFE_DELETE((*itr2).second);
	}
	m_mapDBPlayers.clear();
	DBLoginPlayerMapItr itr21 = m_mapDBLoginPlayers.begin();
	for(; itr21 != m_mapDBLoginPlayers.end(); itr21++)
	{
		SAFE_DELETE((*itr21).second);
	}
	m_mapDBLoginPlayers.clear();
	CdkeyDBLoginPlayerMapItr itrCdkey = m_mapCdkeyLoginPlayers.begin();
	for(; itrCdkey != m_mapCdkeyLoginPlayers.end(); itrCdkey++)
	{
		SAFE_DELETE((*itrCdkey).second);
	}
	m_mapCdkeyLoginPlayers.clear();
	DBFactionMapItr itr3 = m_mapDBFactions.begin();
	for(; itr3 != m_mapDBFactions.end(); itr3++)
	{
		SAFE_DELETE((*itr3).second);
	}
	m_mapDBFactions.clear();

	DBUnionMapItr itr6 = m_mapDBUnions.begin();
	for(; itr6 != m_mapDBUnions.end(); itr6++)
	{
		SAFE_DELETE((*itr6).second);
	}
	m_mapDBUnions.clear();
	DBCountryMapItr itr7 = m_mapDBCountrys.begin();
	for(; itr7 != m_mapDBCountrys.end(); itr7++)
	{
		SAFE_DELETE((*itr7).second);
	}
	m_mapDBCountrys.clear();
	
	DBGenVarMapItr itr9 = m_mapDBGenVars.begin();
	for(; itr9 != m_mapDBGenVars.end(); itr9++)
	{
		SAFE_DELETE((*itr9).second);
	}
	m_mapDBGenVars.clear();
	DBSetupMapItr itr10 = m_mapDBSetups.begin();
	for(; itr10 != m_mapDBSetups.end(); itr10++)
	{
		SAFE_DELETE((*itr10).second);
	}
	m_mapDBSetups.clear();
	DBRegionMapItr itr11 = m_mapDBRegions.begin();
	for(; itr11 != m_mapDBRegions.end(); itr11++)
	{
		SAFE_DELETE((*itr11).second);
	}
	m_mapDBRegions.clear();

	DBDupRgnMapItr itr12 = m_mapDBDupRgns.begin();
	for(; itr12 != m_mapDBDupRgns.end(); itr12++)
	{
		SAFE_DELETE((*itr12).second);
	}
	m_mapDBDupRgns.clear();

	DBGenVarMapItr genVarItr = m_mapDBGenVars.begin();
	for(; genVarItr != m_mapDBGenVars.end(); genVarItr++)
	{
		SAFE_DELETE((*genVarItr).second);
	}
	m_mapDBGenVars.clear();
}
CDBGenVarGroup*   CDBEntityManager::GetGenVarGroup(void) 
{ 
	return m_pTmpGenVarGroup;
}
// 从消息取得操作对象
CBaseDBEntity* CDBEntityManager::FindEntityFromMsg(DB_OBJ_TYPE type, const CGUID& entityid, const CGUID& ownerID)
{
	char szGUID[128];
	entityid.tostring(szGUID);
#ifdef _RUNSTACKINFO_
	//输出运行栈标记
	char pszStackInfo[5120]="";
	_snprintf(pszStackInfo,5120,"CDBEntityManager::FindEntityFromMsg(msgtype:%d, ID[%s]!) Start.", (long)type, szGUID);
	OutputStackInfo(pszStackInfo);
#endif

	CBaseDBEntity* retEntity = NULL;
	switch(type)
	{
	case DB_GENVARGROUP:
		retEntity = (CBaseDBEntity*)m_pTmpGenVarGroup;
		break;
	case DB_MAILGROUP:
		{
			CDBPlayer* pPlayer = FindPlayer(ownerID);
			if(pPlayer)
			{
				CDBMailGroup* pGroup = pPlayer->GetMailGroupPtr();
				if(pGroup->GetOwnerGUID() != pPlayer->GetExID())
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					pPlayer->GetExID().tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() MailGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg MailGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_FACTIONAPPLYGROUP:
		{
			CDBFaction* pFac = FindFaction(ownerID);
			if(pFac)
			{
				CDBFactionApplyGroup* pGroup = pFac->GetFactionApplyGroupPtr();
				if(pGroup->GetOwnerGUID() != pFac->GetExID())
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					pFac->GetExID().tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() FacApplyGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg FactionApplyGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_FACTIONMEMBERGROUP:
		{
			CDBFaction* pFac = FindFaction(ownerID);
			if(pFac)
			{
				CDBFactionMemberGroup* pGroup = pFac->GetFactionMemberGroupPtr();
				if(pGroup->GetOwnerGUID() != pFac->GetExID())
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					pFac->GetExID().tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() FacMemberGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg FactionMemberGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_LINKMANGROUP:
		{
			CDBPlayer* pPlayer = FindPlayer(ownerID);
			if(pPlayer)
			{
				CDBLinkmanGroup* pGroup = pPlayer->GetLinkmanGroupPtr();
				if(pGroup->GetOwnerGUID() != pPlayer->GetExID())
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					pPlayer->GetExID().tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() LinkManGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg CDBLinkmanGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_PHRGNGOODSGROUP:
		{
			DBDupRgnMapItr itr = GetDBDupRgnMap().find(ownerID);
			if(itr != GetDBDupRgnMap().end())
			{
				CDBRgnGoodsGroup* pGroup = itr->second->GetGoodsGroupPtr();
				if(pGroup->GetOwnerGUID() != itr->first)
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					itr->first.tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() PhRgnGoodsGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg RgnGoodsGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_GOODSGROUP:
		{
			CDBPlayer* pPlayer = FindPlayer(ownerID);
			if(pPlayer)
			{
				CDBGoodsGroup* pGroup = pPlayer->GetGoodsGroupPtr();
				if(pGroup->GetOwnerGUID() != pPlayer->GetExID())
				{
					char szOwnerID[128];
					pGroup->GetOwnerGUID().tostring(szOwnerID);
					char szOwnerID1[128];
					pPlayer->GetExID().tostring(szOwnerID1);
					AddLogText("FindEntityFromMsg() GoodsGroup's OwnerID[%s] is Err, OwnerID[%s]!", szOwnerID, szOwnerID1);
				}
				retEntity = (CBaseDBEntity*)pGroup;
			}
			else
			{
				char szOwnerID[128];
				ownerID.tostring(szOwnerID);
				AddLogText("CDBEntityManager::FindEntityFromMsg GoodsGroup is NULL, owner[%s]!", szOwnerID);
			}
		}
		break;
	case DB_COUNTRY:
		{
			DBCountryMapItr itr = m_mapDBCountrys.find(entityid);
			if(itr != m_mapDBCountrys.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_FACTION:
		{
			DBFactionMapItr itr = m_mapDBFactions.find(entityid);
			if(itr != m_mapDBFactions.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_GEN_VAR:
		{
			DBGenVarMapItr itr = m_mapDBGenVars.find(entityid);
			if(itr != m_mapDBGenVars.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}		
		}
		break;
	case DB_PLAYER:
		{
			DBPlayerMapItr itr = m_mapDBPlayers.find(entityid);
			if(itr != m_mapDBPlayers.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_LOGINPLAYER:
		{
			DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(entityid);
			if(itr != m_mapDBLoginPlayers.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_REGION:
		{
			DBRegionMapItr it=m_mapDBRegions.find(entityid);
			if(it!=m_mapDBRegions.end())
			{
				retEntity = (CBaseDBEntity*)(it->second);
			}
		}
		break;
	case DB_PHDUPRGN:
		{
			DBDupRgnMapItr itr = m_mapDBDupRgns.find(entityid);
			if(itr != m_mapDBDupRgns.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_SETUP:
		{
			if(m_mapDBSetups.size())
			{
				DBSetupMapItr itr = m_mapDBSetups.begin();
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	case DB_UNION:
		{
			DBUnionMapItr itr = m_mapDBUnions.find(entityid);
			if(itr != m_mapDBUnions.end())
			{
				retEntity = (CBaseDBEntity*)(itr->second);
			}
		}
		break;
	}
#ifdef _RUNSTACKINFO_
	//输出运行栈标记
	_snprintf(pszStackInfo,5120,"CDBEntityManager::FindEntityFromMsg(msgtype:%d, ID[%s], ptr[%d]!) end.", (long)type, szGUID,(long)retEntity);
	OutputStackInfo(pszStackInfo);
#endif
	return retEntity;
}
void CDBEntityManager::ProcessMsg(CMessage* pMsg)
{
	long entityType = pMsg->GetLong();

	// 取得Oper Entity Flag 0:1个, 1:多个
	BYTE opEntityFlag = pMsg->GetByte();

	// DB操作类型
	DB_OPERATION_TYPE op = (DB_OPERATION_TYPE)pMsg->GetByte();

	// MSGID
	long msgid = pMsg->GetLong();
	// 清除定时器
	//GetGame()->DelMsgTimer(msgid);

	// Entity Num
	long entityNum = pMsg->GetWord();

	long bufSize = pMsg->GetLong();
	// 消息数据块
	BYTE* msgBuf = new BYTE[bufSize];
	// pos
	long msgBufPos = 0;
	// 获取MsgBuf
	pMsg->GetEx(msgBuf, bufSize);

#ifdef _RUNSTACKINFO_
	//输出运行栈标记
	char pszStackInfo[1024]="";
	_snprintf(pszStackInfo,1024,"CDBEntityManager::ProcessMsg(msgtype:%d, oper:%d, enNum:%d, bufsize:%d) Start.",pMsg->GetType(), op, entityNum, bufSize);
	OutputStackInfo(pszStackInfo);
#endif

	// 清除DBCountry
	if(entityType == DB_COUNTRY)
	{
		if(entityNum != 0)
		{
			// 清除CDBCountry
			map<CGUID, CDBCountry*>::iterator dbItr = GetGame()->GetDBEntityManager()->GetDBCountryMap().begin();
			for(; dbItr != GetGame()->GetDBEntityManager()->GetDBCountryMap().end(); dbItr++)
			{
				SAFE_DELETE(dbItr->second);
			}
			GetGame()->GetDBEntityManager()->GetDBCountryMap().clear();

			// 清除CCountry
			GetCountryHandler()->Release();
		}
	}

	CBaseDBEntity* entity = NULL;
	for(int i=0; i<entityNum; i++)
	{
		CGUID entityid;
		_GetBufferFromByteArray(msgBuf, msgBufPos, entityid, bufSize);
		char szEntityID[128];
		entityid.tostring(szEntityID);
#ifdef _RUNSTACKINFO_
		_snprintf(pszStackInfo,256,"ProcessMsg::_GetBufferFromByteArray() end, ID[%s].", szEntityID);
		OutputStackInfo(pszStackInfo);
#endif
		CGUID ownerID;
		if(entityType == DB_MAILGROUP || entityType == DB_GOODSGROUP || entityType == DB_PHRGNGOODSGROUP || entityType == DB_GENVARGROUP
			|| entityType == DB_LINKMANGROUP || entityType == DB_FACTIONMEMBERGROUP || entityType == DB_FACTIONAPPLYGROUP)
		{
			_GetBufferFromByteArray(msgBuf, msgBufPos, ownerID, bufSize); // 拥有者ID
		}
		entity = FindEntityFromMsg((DB_OBJ_TYPE)entityType, entityid, ownerID);
#ifdef _RUNSTACKINFO_
		_snprintf(pszStackInfo,256,"ProcessMsg::FindEntityFromMsg() end, ID[%s].", szEntityID);
		OutputStackInfo(pszStackInfo);
#endif
		if(!entity)
		{
			if(op != DB_OPER_SAVE) // 保存回应消息,没有对象表示该对象应该被删除,不再处理
			{
				entity = CreateEntityByMsg((DB_OBJ_TYPE)entityType, entityid);
#ifdef _RUNSTACKINFO_
				_snprintf(pszStackInfo,256,"ProcessMsg::CreateEntityByMsg() end, ID[%s].", szEntityID);
				OutputStackInfo(pszStackInfo);
#endif
				if(entity)
				{
#ifdef _RUNSTACKINFO_
					_snprintf(pszStackInfo,256,"ProcessMsg::ProcessMsg{entity:%s} Start.", szEntityID);
					OutputStackInfo(pszStackInfo);
#endif				
					entity->ProcessMsg(op, msgid, msgBuf, msgBufPos, bufSize);
#ifdef _RUNSTACKINFO_
					_snprintf(pszStackInfo,256,"ProcessMsg::ProcessMsg{entity:%s} end.", szEntityID);
					OutputStackInfo(pszStackInfo);
#endif
				}
			}
		}
		else
		{
			entity->ProcessMsg(op, msgid, msgBuf, msgBufPos, bufSize);
		}
	}
#ifdef _RUNSTACKINFO_
	_snprintf(pszStackInfo,256,"CDBEntityManager::ProcessMsg end.");
	OutputStackInfo(pszStackInfo);
#endif
	SAFE_DELETE_ARRAY(msgBuf);
}
// 创建对象
CBaseDBEntity* CDBEntityManager::CreateEntityByMsg(DB_OBJ_TYPE type, const CGUID& guid)
{
	CBaseDBEntity* retEntity = NULL;
	switch(type)
	{
	case DB_GENVARGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Don't Create GenVarGroup!");
		}
		break;
	case DB_MAILGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create MailGroup!");
		}
		break;
	case DB_FACTIONAPPLYGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create DB_FACTIONAPPLYGROUP!");
		}
		break;
	case DB_FACTIONMEMBERGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create DB_FACTIONMEMBERGROUP!");
		}
		break;
	case DB_LINKMANGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create DB_LINKMANGROUP!");
		}
		break;
	case DB_PHRGNGOODSGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create RgnGoodsGroup!");
		}
		break;
	case DB_GOODSGROUP:
		{
			//Err
			AddLogText("CDBEntityManager::CreateEntityByMsg Can't Create GoodsGroup!");
		}
		break;
	case DB_COUNTRY:
		{
			retEntity = new CDBCountry(guid);
			AddOneDBCountry((CDBCountry*)retEntity);
		}
		break;
	case DB_FACTION:
		{
			retEntity = new CDBFaction(guid);
			AddOneDBFaction((CDBFaction*)retEntity);
		}
		break;
	case DB_GEN_VAR:
		{
			retEntity = new CDBGenVar(guid);	
			AddOneDBGenVar((CDBGenVar*)retEntity);
		}
		break;
	case DB_PLAYER:
		{
			retEntity = new CDBPlayer(guid);
			AddOneDBPlayer((CDBPlayer*)retEntity);
		}
		break;
	case DB_LOGINPLAYER:
		{
			retEntity = new CDBLoginPlayer(guid);
			AddOneDBLoginPlayer((CDBLoginPlayer*)retEntity);
		}
		break;
	case DB_REGION:
		{
			retEntity=new CDBRegion(guid);
			AddOneDBRegion((CDBRegion*)retEntity);
		}
		break;
	case DB_PHDUPRGN:
		{
			retEntity = new CDBDupRgn(guid);
			AddOneDBDupRgn((CDBDupRgn*)retEntity);
		}
		break;
	case DB_SETUP:
		{
			retEntity = new CDBSetup(guid);
			AddOneDBSetup((CDBSetup*)retEntity);
		}
		break;
	case DB_UNION:
		{
			retEntity = new CDBUnion(guid);
			AddOneDBUnion((CDBUnion*)retEntity);
		}
		break;
	}

	return retEntity;
}
// interfaces
//DBCard////////////////////////////////////////////////////////////////////////
// 添加一个DBCard到链表末尾
void CDBEntityManager::AddOneDBCard(CDBCard* card)
{
	if(card)
		m_mapDBCards[card->GetExID()] = card;
}
// 删除一个DBCard
void CDBEntityManager::DelOneDBCard(CDBCard* card)
{
	if(card)
	{
		DBCardMapItr itr = m_mapDBCards.find(card->GetExID());
		if(itr != m_mapDBCards.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBCards.erase(itr);
		}
	}
}
// 删除一个DBCard
void CDBEntityManager::DelOneDBCard(const CGUID& cardID)
{
	DBCardMapItr itr = m_mapDBCards.find(cardID);
	if(itr != m_mapDBCards.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBCards.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearCardMap(void)
{
	DBCardMapItr itr = m_mapDBCards.begin();
	for(; itr != m_mapDBCards.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBCards.clear();
}
// CDBLoginPlayer////////////////////////////////////////////////////////////////////////
// 添加一个CDBLoginPlayer到Map末尾 同时添加到CDKEY的map
void CDBEntityManager::AddOneDBLoginPlayer(CDBLoginPlayer* player)
{
	if(player)
	{
		// 先清除原有数据
		DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(player->GetExID());
		if(itr != m_mapDBLoginPlayers.end())
		{
			SAFE_DELETE(itr->second);
			m_mapDBLoginPlayers.erase(itr);
		}
		m_mapDBLoginPlayers[player->GetExID()] = player;
	}
}
// 删除一个CDBLoginPlayer 删除CDKEY的map
void CDBEntityManager::DelOneDBLoginPlayer(CDBLoginPlayer* player)
{
	if(player)
	{
		DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(player->GetExID());
		if(itr != m_mapDBLoginPlayers.end())
		{
			// 重复删除
			//DelLoginPlayerByCdkey(player);

			// 同时清除CDBLoginPlayerMap
			m_mapDBLoginPlayers.erase(itr);
		}

		SAFE_DELETE(player);
	}
}
// 删除一个CDBLoginPlayer 删除CDKEY的map
void CDBEntityManager::DelOneDBLoginPlayer(const CGUID& playerID)
{
	DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(playerID);
	if(itr != m_mapDBLoginPlayers.end())
	{
		// 重复删除
		//DelLoginPlayerByCdkey(itr->second);

		SAFE_DELETE(itr->second);
		// 同时清除CDBLoginPlayerMap
		m_mapDBLoginPlayers.erase(itr);
	}
}
CDBLoginPlayer* CDBEntityManager::FindLoginPlayer(const CGUID& guid)
{
	CDBLoginPlayer* ret = NULL;
	DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(guid);
	if(itr != m_mapDBLoginPlayers.end())
		ret = itr->second;

	return ret;
}
// 清空链表
void CDBEntityManager::ClearLoginPlayerMap(void)
{
	DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.begin();
	for(; itr != m_mapDBLoginPlayers.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBLoginPlayers.clear();
}
// 玩家第一次登录时初始化
bool CDBEntityManager::InitLoginPlayer(const char* szCdkey)
{
	if(szCdkey)
	{
		// 现在vector中查找是否已经存在
		CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(szCdkey);
		if(itr == m_mapCdkeyLoginPlayers.end())
		{
			tagLoginInfo* tLoginInfo = new tagLoginInfo;
			m_mapCdkeyLoginPlayers[string(szCdkey)] = tLoginInfo;
			tLoginInfo->lLastLoginTime = timeGetTime();
			return true;
		}
	}
	return false;
}
// 添加一个CDBLoginPlayer到Map末尾 同时操作另一个map(CGUID为key)
void CDBEntityManager::AddLoginPlayerByCdkey(CDBLoginPlayer* player)
{
	if(player)
	{
		if(!player->GetAccount())
		{
			char szGuid[128];
			player->GetExID().tostring(szGuid);
			AddLogText("AddLoginPlayerByCdkey:[%s] CDKEY为空!", szGuid);
		}
		tagLoginInfo* tLoginInfo = NULL;
		// 现在vector中查找是否已经存在
		string strAccount = player->GetAccount();
		CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(strAccount);
		if(itr != m_mapCdkeyLoginPlayers.end())
		{
			tLoginInfo = itr->second;
		}
		else
		{
			tLoginInfo = new tagLoginInfo;
			m_mapCdkeyLoginPlayers[strAccount] = tLoginInfo;
			tLoginInfo->lLastLoginTime = timeGetTime();
		}
		tLoginInfo->setLoginGuid.insert(player->GetExID());
	}
}
// 删除一个CDBLoginPlayer
void CDBEntityManager::DelLoginPlayerByCdkey(CDBLoginPlayer* player)
{
	if(player)
	{
		// 现在vector中查找是否已经存在
		CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(player->GetAccount());
		if(itr != m_mapCdkeyLoginPlayers.end())
		{
			tagLoginInfo* tLoginInfo = itr->second;
			if(tLoginInfo)
			{
				tLoginInfo->setLoginGuid.erase(player->GetExID());
			}
			if(tLoginInfo && tLoginInfo->setLoginGuid.size()==0)
			{
				delete tLoginInfo;
				m_mapCdkeyLoginPlayers.erase(itr);
			}
		}
	}
}
// 清空链表
void CDBEntityManager::ClearCdkeyLoginPlayerMap(void)
{
	CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.begin();
	for(; itr != m_mapCdkeyLoginPlayers.end(); itr++)
		delete itr->second;
	m_mapCdkeyLoginPlayers.clear();
}
// 取得个数
CDBEntityManager::tagLoginInfo* CDBEntityManager::FindLoginPlayerByCdkey(const char* szCdkey)
{
	if(!szCdkey)
		return NULL;

	tagLoginInfo* tLoginInfo = NULL;
	CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(szCdkey);
	if(itr != m_mapCdkeyLoginPlayers.end())
		tLoginInfo = itr->second;
	return tLoginInfo;
}
//DBPlayer////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBPlayer(CDBPlayer* player)
{
	if(player)
	{
		// 先清除原有数据
		DBPlayerMapItr itr = m_mapDBPlayers.find(player->GetExID());
		if(itr != m_mapDBPlayers.end())
		{
			SAFE_DELETE(itr->second);
			m_mapDBPlayers.erase(itr);
		}

		m_mapDBPlayers[player->GetExID()] = player;
	}
}
// 删除一个DBPlayer
void CDBEntityManager::DelOneDBPlayer(const CGUID& playerID)
{
	DBPlayerMapItr itr = m_mapDBPlayers.find(playerID);
	if(itr != m_mapDBPlayers.end())
	{
		// 清除联系人
		LinkmanSystem::GetInstance().DelPlayerLinkman(playerID);

		SAFE_DELETE((*itr).second);
		m_mapDBPlayers.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearPlayerMap(void)
{
	DBPlayerMapItr itr = m_mapDBPlayers.begin();
	for(; itr != m_mapDBPlayers.end(); itr++)
	{
		LinkmanSystem::GetInstance().DelPlayerLinkman(itr->second->GetExID());
		SAFE_DELETE((*itr).second);
	}

	m_mapDBPlayers.clear();
}
CDBPlayer* CDBEntityManager::FindPlayer(const CGUID& guid)
{
	CDBPlayer* ret = NULL;
	DBPlayerMapItr itr = m_mapDBPlayers.find(guid);
	if(itr != m_mapDBPlayers.end())
		ret = itr->second;

	return ret;
}

CDBRegion* CDBEntityManager::FindRegion(const CGUID& guid)
{	
	DBRegionMapItr it=m_mapDBRegions.find(guid);
	if(it!=m_mapDBRegions.end())
		return it->second;
	return NULL;
}

//DBFaction////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBFaction(CDBFaction* faction)
{
	if(faction)
	{
		DBFactionMapItr itr = m_mapDBFactions.find(faction->GetExID());
		if(itr != m_mapDBFactions.end())
		{
			if(faction == itr->second) return;

			SAFE_DELETE((*itr).second);
			m_mapDBFactions.erase(itr);
		}
		m_mapDBFactions[faction->GetExID()] = faction;
	}
}
// 删除一个DBGood
void CDBEntityManager::DelOneDBFaction(CDBFaction* faction)
{
	if(faction)
	{
		DBFactionMapItr itr = m_mapDBFactions.find(faction->GetExID());
		if(itr != m_mapDBFactions.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBFactions.erase(itr);
		}
	}
}
// 删除一个DBFaction
void CDBEntityManager::DelOneDBFaction(const CGUID& factionID)
{
	DBFactionMapItr itr = m_mapDBFactions.find(factionID);
	if(itr != m_mapDBFactions.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBFactions.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearFactionMap(void)
{
	DBFactionMapItr itr = m_mapDBFactions.begin();
	for(; itr != m_mapDBFactions.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBFactions.clear();
}
CDBFaction* CDBEntityManager::FindFaction(const CGUID& guid)
{
	CDBFaction* ret = NULL;
	DBFactionMapItr itr = m_mapDBFactions.find(guid);
	if(itr != m_mapDBFactions.end())
	{
		ret = itr->second;
	}
	return ret;
}
//DBEnemyFaction////////////////////////////////////////////////////////////////////////
//void CDBEntityManager::AddOneDBEnemyFaction(CDBEnemyFaction* Enemyfaction)
//{
//	if(Enemyfaction)
//		m_mapDBEnemyFactions[Enemyfaction->GetExID()] = Enemyfaction;
//}
// 删除一个DBGood
//void CDBEntityManager::DelOneDBEnemyFaction(CDBEnemyFaction* Enemyfaction)
//{
//	if(Enemyfaction)
//	{
//		DBEnemyFactionMapItr itr = m_mapDBEnemyFactions.find(Enemyfaction->GetExID());
//		if(itr != m_mapDBEnemyFactions.end())
//		{
//			SAFE_DELETE((*itr).second);
//			m_mapDBEnemyFactions.erase(itr);
//		}
//	}
//}
//// 删除一个DBCityWar
//void CDBEntityManager::DelOneDBEnemyFaction(const CGUID& EnemyFactionID)
//{
//	DBEnemyFactionMapItr itr = m_mapDBEnemyFactions.find(EnemyFactionID);
//	if(itr != m_mapDBEnemyFactions.end())
//	{
//		SAFE_DELETE((*itr).second);
//		m_mapDBEnemyFactions.erase(itr);
//	}
//}
//// 清空链表
//void CDBEntityManager::ClearEnemyFactionMap(void)
//{
//	DBEnemyFactionMapItr itr = m_mapDBEnemyFactions.begin();
//	for(; itr != m_mapDBEnemyFactions.end(); itr++)
//	{
//		SAFE_DELETE((*itr).second);
//	}
//
//	m_mapDBEnemyFactions.clear();
//}
//DBCityWar////////////////////////////////////////////////////////////////////////
//void CDBEntityManager::AddOneDBCityWar(CDBCityWar* CityWar)
//{
//	if(CityWar)
//		m_mapDBCityWars[CityWar->GetExID()] = CityWar;
//}
//// 删除一个DBCityWar
//void CDBEntityManager::DelOneDBCityWar(CDBCityWar* CityWar)
//{
//	if(CityWar)
//	{
//		DBCityWarMapItr itr = m_mapDBCityWars.find(CityWar->GetExID());
//		if(itr != m_mapDBCityWars.end())
//		{
//			SAFE_DELETE((*itr).second);
//			m_mapDBCityWars.erase(itr);
//		}
//	}
//}
//// 删除一个DBCityWar
//void CDBEntityManager::DelOneDBCityWar(const CGUID& CityWarID)
//{
//	DBCityWarMapItr itr = m_mapDBCityWars.find(CityWarID);
//	if(itr != m_mapDBCityWars.end())
//	{
//		SAFE_DELETE((*itr).second);
//		m_mapDBCityWars.erase(itr);
//	}
//}
//// 清空链表
//void CDBEntityManager::ClearCityWarMap(void)
//{
//	DBCityWarMapItr itr = m_mapDBCityWars.begin();
//	for(; itr != m_mapDBCityWars.end(); itr++)
//	{
//		SAFE_DELETE((*itr).second);
//	}
//
//	m_mapDBCityWars.clear();
//}
////DBVillageWar////////////////////////////////////////////////////////////////////////
//void CDBEntityManager::AddOneDBVillageWar(CDBVillageWar* VillageWar)
//{
//	if(VillageWar)
//		m_mapDBVillageWars[VillageWar->GetExID()] = VillageWar;
//}
//// 删除一个DBGood
//void CDBEntityManager::DelOneDBVillageWar(CDBVillageWar* VillageWar)
//{
//	if(VillageWar)
//	{
//		DBVillageWarMapItr itr = m_mapDBVillageWars.find(VillageWar->GetExID());
//		if(itr != m_mapDBVillageWars.end())
//		{
//			SAFE_DELETE((*itr).second);
//			m_mapDBVillageWars.erase(itr);
//		}
//	}
//}
//// 删除一个DBVillageWar
//void CDBEntityManager::DelOneDBVillageWar(const CGUID& VillageWarID)
//{
//	DBVillageWarMapItr itr = m_mapDBVillageWars.find(VillageWarID);
//	if(itr != m_mapDBVillageWars.end())
//	{
//		SAFE_DELETE((*itr).second);
//		m_mapDBVillageWars.erase(itr);
//	}
//}
//// 清空链表
//void CDBEntityManager::ClearVillageWarMap(void)
//{
//	DBVillageWarMapItr itr = m_mapDBVillageWars.begin();
//	for(; itr != m_mapDBVillageWars.end(); itr++)
//	{
//		SAFE_DELETE((*itr).second);
//	}
//
//	m_mapDBVillageWars.clear();
//}

//DBUnion////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBUnion(CDBUnion* Union)
{
	if(Union)
		m_mapDBUnions[Union->GetExID()] = Union;
}
// 删除一个DBGood
void CDBEntityManager::DelOneDBUnion(CDBUnion* Union)
{
	if(Union)
	{
		DBUnionMapItr itr = m_mapDBUnions.find(Union->GetExID());
		if(itr != m_mapDBUnions.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBUnions.erase(itr);
		}
	}
}
// 删除一个DBUnion
void CDBEntityManager::DelOneDBUnion(const CGUID& UnionID)
{
	DBUnionMapItr itr = m_mapDBUnions.find(UnionID);
	if(itr != m_mapDBUnions.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBUnions.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearUnionMap(void)
{
	DBUnionMapItr itr = m_mapDBUnions.begin();
	for(; itr != m_mapDBUnions.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBUnions.clear();
}
//DBCountry////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBCountry(CDBCountry* Country)
{
	if(Country)
	{
		// 先清除原有数据
		DBCountryMapItr itr;
		for(itr = m_mapDBCountrys.begin(); itr != m_mapDBCountrys.end(); itr++)
		{
			if( itr->second->GetExID() == Country->GetExID() 
				|| itr->second->GetID() == Country->GetID() )
			{
				SAFE_DELETE(itr->second);
				m_mapDBCountrys.erase(itr);
				break;
			}
		}

		m_mapDBCountrys[Country->GetExID()] = Country;
	}
}
// 删除一个DBGood
void CDBEntityManager::DelOneDBCountry(CDBCountry* Country)
{
	if(Country)
	{
		DBCountryMapItr itr = m_mapDBCountrys.find(Country->GetExID());
		if(itr != m_mapDBCountrys.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBCountrys.erase(itr);
		}
	}
}
// 删除一个DBCountry
void CDBEntityManager::DelOneDBCountry(const CGUID& CountryID)
{
	DBCountryMapItr itr = m_mapDBCountrys.find(CountryID);
	if(itr != m_mapDBCountrys.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBCountrys.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearCountryMap(void)
{
	DBCountryMapItr itr = m_mapDBCountrys.begin();
	for(; itr != m_mapDBCountrys.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBCountrys.clear();
}


//DBGenVar////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBGenVar(CDBGenVar* GenVar)
{
	if(GenVar)
		m_mapDBGenVars[GenVar->GetExID()] = GenVar;
}
// 删除一个DBGenVar
void CDBEntityManager::DelOneDBGenVar(CDBGenVar* GenVar)
{
	if(GenVar)
	{
		DBGenVarMapItr itr = m_mapDBGenVars.find(GenVar->GetExID());
		if(itr != m_mapDBGenVars.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBGenVars.erase(itr);
		}
	}
}
// 删除一个DBGenVar
void CDBEntityManager::DelOneDBGenVar(const CGUID& GenVarID)
{
	DBGenVarMapItr itr = m_mapDBGenVars.find(GenVarID);
	if(itr != m_mapDBGenVars.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBGenVars.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearGenVarMap(void)
{
	DBGenVarMapItr itr = m_mapDBGenVars.begin();
	for(; itr != m_mapDBGenVars.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBGenVars.clear();
}

//DBSetup////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBSetup(CDBSetup* Setup)
{
	if(Setup)
		m_mapDBSetups[Setup->GetExID()] = Setup;
}
// 删除一个DBSetup
void CDBEntityManager::DelOneDBSetup(CDBSetup* Setup)
{
	if(Setup)
	{
		DBSetupMapItr itr = m_mapDBSetups.find(Setup->GetExID());
		if(itr != m_mapDBSetups.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBSetups.erase(itr);
		}
	}
}
// 删除一个DBSetup
void CDBEntityManager::DelOneDBSetup(const CGUID& SetupID)
{
	DBSetupMapItr itr = m_mapDBSetups.find(SetupID);
	if(itr != m_mapDBSetups.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBSetups.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearSetupMap(void)
{
	DBSetupMapItr itr = m_mapDBSetups.begin();
	for(; itr != m_mapDBSetups.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBSetups.clear();
}

//DBRegion////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBRegion(CDBRegion* Region)
{
	if(Region)
	{
		// 先清除原有数据
		DBRegionMapItr itr;
		for(itr = m_mapDBRegions.begin(); itr != m_mapDBRegions.end(); itr++)
		{
			if( itr->second->GetExID() == Region->GetExID() 
				|| itr->second->GetID() == Region->GetID() )
			{
				SAFE_DELETE(itr->second);
				m_mapDBRegions.erase(itr);
				break;
			}
		}

		m_mapDBRegions[Region->GetExID()] = Region;
	}
}
// 删除一个DBRegion
void CDBEntityManager::DelOneDBRegion(CDBRegion* Region)
{
	if(Region)
	{
		DBRegionMapItr itr = m_mapDBRegions.find(Region->GetExID());
		if(itr != m_mapDBRegions.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBRegions.erase(itr);
		}
	}
}
// 删除一个DBRegion
void CDBEntityManager::DelOneDBRegion(const CGUID& RegionID)
{
	DBRegionMapItr itr = m_mapDBRegions.find(RegionID);
	if(itr != m_mapDBRegions.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBRegions.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearRegionMap(void)
{
	DBRegionMapItr itr = m_mapDBRegions.begin();
	for(; itr != m_mapDBRegions.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBRegions.clear();
}
//DBDupRgn////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBDupRgn(CDBDupRgn* Region)
{
	if(Region)
	{
		// 先清除原有数据
		DBDupRgnMapItr itr = m_mapDBDupRgns.find(Region->GetExID());

		if( itr != m_mapDBDupRgns.end() )
		{
			SAFE_DELETE(itr->second);
			m_mapDBDupRgns.erase(itr);
		}

		m_mapDBDupRgns[Region->GetExID()] = Region;
	}
}
// 删除一个DBDupRgn
void CDBEntityManager::DelOneDBDupRgn(CDBDupRgn* Region)
{
	if(Region)
	{
		DBDupRgnMapItr itr = m_mapDBDupRgns.find(Region->GetExID());
		if(itr != m_mapDBDupRgns.end())
		{
			SAFE_DELETE((*itr).second);
			m_mapDBDupRgns.erase(itr);
		}
	}
}
// 删除一个DBDupRgn
void CDBEntityManager::DelOneDBDupRgn(const CGUID& RegionID)
{
	DBDupRgnMapItr itr = m_mapDBDupRgns.find(RegionID);
	if(itr != m_mapDBDupRgns.end())
	{
		SAFE_DELETE((*itr).second);
		m_mapDBDupRgns.erase(itr);
	}
}
// 清空链表
void CDBEntityManager::ClearDupRgnMap(void)
{
	DBDupRgnMapItr itr = m_mapDBDupRgns.begin();
	for(; itr != m_mapDBDupRgns.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBDupRgns.clear();
}
//DBLinkman////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBLinkman(CDBLinkMan* good)
{
	if(good)
	{
		// 先清除原有数据
		DBLinkmanMapItr itr = m_mapDBLinkman.find(good->GetExID());
		if(itr != m_mapDBLinkman.end())
		{
			SAFE_DELETE(itr->second);
			m_mapDBLinkman.erase(itr);
		}

		m_mapDBLinkman[good->GetExID()] = good;
	}
}
// 删除一个DBRgnGood
void CDBEntityManager::DelOneDBLinkman(CDBLinkMan* good)
{
	if(good)
	{
		DBLinkmanMapItr itr = m_mapDBLinkman.find(good->GetExID());
		if(itr != m_mapDBLinkman.end())
			m_mapDBLinkman.erase(itr);

		SAFE_DELETE(good);
	}
}
// 删除一个DBRgnGood
void CDBEntityManager::DelOneDBLinkman(const CGUID& goodID)
{
	DBLinkmanMapItr itr = m_mapDBLinkman.find(goodID);
	if(itr != m_mapDBLinkman.end())
	{
		SAFE_DELETE(itr->second);
		m_mapDBLinkman.erase(itr);
	}
}
// 清空所属玩家的信息
void CDBEntityManager::ClearDBLinkmanMap(const CGUID& PlayerGuid)
{
	//DBLinkmanMapItr itr = m_mapDBLinkman.begin();
	//while( itr != m_mapDBLinkman.end() )
	//{
	//	if(PlayerGuid == itr->second->GetPlayerID())
	//	{
	//		SAFE_DELETE(itr->second);
	//		itr = m_mapDBLinkman.erase(itr);
	//	}
	//	else
	//		itr++;
	//}
}
// 清空链表
void CDBEntityManager::ClearDBLinkmanMap(void)
{
	DBLinkmanMapItr itr = m_mapDBLinkman.begin();
	for(; itr != m_mapDBLinkman.end(); itr++)
	{
		SAFE_DELETE(itr->second);
	}

	m_mapDBLinkman.clear();
}

bool CDBEntityManager::HasDBLinkman(const CGUID& ownerID, long place)
{
	bool ret = false;
	DBLinkmanMapItr itr = m_mapDBLinkman.begin();
	for(;itr != m_mapDBLinkman.end(); itr++)
	{
		CEntityProperty* epOwnerID = itr->second->GetDataEntityManager().GetEntityProperty(string("OwnerGUID"));
		if( epOwnerID)
		{
			CGUID tGUID;
			epOwnerID->GetGuidAttr(0, tGUID);
			if ( ownerID == tGUID )
			{
				ret = true;
			}
		}
	}
	return ret;
}

// 产生ID
bool CDBEntityManager::GenerateGUID(CGUID& guid)
{
	return CGUID::CreateGUID(guid);
}

void CDBEntityManager::CGoodsToCDBGood(CGoods* good, CDBGoods* dbGood, const CGUID& ownerID)
{
	char szGUID[128];
	char goodSZGUID[128];
	good->GetExID().tostring(goodSZGUID);

	ownerID.tostring(szGUID);

	string tableName = "equipment";
	dbGood->SetGuidAttr(string("goodsID"), tableName, 0, dbGood->GetExID());
	dbGood->SetLongAttr(string("goodsIndex"), tableName, 0, good->GetBasePropertiesIndex());
	dbGood->SetGuidAttr(string("PlayerID"), tableName, 0, ownerID);
	dbGood->SetStringAttr(string("name"), tableName, 0, good->GetName());
	dbGood->SetLongAttr(string("price"), tableName, 0, good->GetPrice());
	dbGood->SetLongAttr(string("amount"), tableName, 0, good->GetAmount());
	
	DWORD dwMaxHole=good->GetMaxEnchaseHoleNum();
	if(dwMaxHole>0)
	{
		LONG* pHoleData=good->GetEnchaseHoleData();
		if(pHoleData)
		{
			
			dbGood->SetLongAttr(string("holeNum"), tableName, 0,dwMaxHole);
			dbGood->SetLongAttr(string("hole1Idx"), tableName, 0,pHoleData[0]);
			dbGood->SetLongAttr(string("hole2Idx"), tableName, 0,pHoleData[1]);
			dbGood->SetLongAttr(string("hole3Idx"), tableName, 0,pHoleData[2]);
			dbGood->SetLongAttr(string("hole4Idx"), tableName, 0,pHoleData[3]);
			dbGood->SetLongAttr(string("hole5Idx"), tableName, 0,pHoleData[4]);
		}
		SAFE_DELETE_ARRAY(pHoleData);
	}

	vector<BYTE> m_vecAddonProperty;
	vector<CGoods::tagAddonProperty>& vecAddonProperty=good->GetAllAddonProperties();
	/*
	DWORD dwAddonSize=vecAddonProperty.size();	
	_AddToByteArray(&m_vecAddonProperty,dwAddonSize);
	for(int i=0;i<dwAddonSize;i++)
	{

		//addon id
		_AddToByteArray(&m_vecAddonProperty,(DWORD)vecAddonProperty[i].gapType);
		_AddToByteArray(&m_vecAddonProperty,vecAddonProperty[i].vValues[0].lBaseValue);
		_AddToByteArray(&m_vecAddonProperty,vecAddonProperty[i].vValues[1].lBaseValue);
	}*/
	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(good->GetBasePropertiesIndex());
	if(!pBaseProperty)
		return;
	DWORD dwAddonNum=0;
	DWORD dwPropertyId=0;
	vector<BYTE> vecAddonData;
	for( size_t i = 0; i < vecAddonProperty.size(); i ++ )
	{		
		
		//如果物编里面有该属性,
		if(pBaseProperty->IsHasAddonPropertie(vecAddonProperty[i].gapType))	
		{	
			dwPropertyId=vecAddonProperty[i].gapType;
			if(CGoodsFactory::s_GoodsAttrDBSetup[dwPropertyId][0]==1)
			{			
				dwAddonNum++;
				_AddToByteArray(&vecAddonData,(DWORD)vecAddonProperty[i].gapType);
				_AddToByteArray(&vecAddonData,vecAddonProperty[i].vValues[0].lBaseValue);
				_AddToByteArray(&vecAddonData,vecAddonProperty[i].vValues[1].lBaseValue);
			}
		}
		/* 没有的属性就不存了
		else
		{
			dwAddonNum++;
			_AddToByteArray(&vecAddonData,(DWORD)vecAddonProperty[i].gapType);
			_AddToByteArray(&vecAddonData,vecAddonProperty[i].vValues[0].lBaseValue);
			_AddToByteArray(&vecAddonData,vecAddonProperty[i].vValues[1].lBaseValue);
		}*/
	}
	_AddToByteArray( &m_vecAddonProperty,dwAddonNum);	
	
	if(vecAddonData.size()>0)
		_AddToByteArray(&m_vecAddonProperty,&vecAddonData[0],vecAddonData.size());	
	
	dbGood->SetBufAttr(string("AddonProperty"),tableName,0,&m_vecAddonProperty[0],m_vecAddonProperty.size());
}
void CDBEntityManager::CPlayerToCDBLoginPlayer(CPlayer* pPlayer, CDBLoginPlayer* dbLPlayer)
{
	if(!dbLPlayer->GetDataEntityManager().GetEntityPropertyMap().size())
		AddLogText("CPlayerToCDBLoginPlayer() db对象的属性配置信息为空!");

	string tableName = "csl_player_base";
	dbLPlayer->SetAccount(pPlayer->GetAccount(), 20);
	dbLPlayer->SetGuidAttr(string("guid"), tableName, 0, pPlayer->GetExID());
	dbLPlayer->SetStringAttr(string("Name"), tableName, 0, pPlayer->GetName());
	dbLPlayer->SetStringAttr(string("Account"), tableName, 0, pPlayer->GetAccount());
	dbLPlayer->SetLongAttr(string("Levels"), tableName, 0, pPlayer->GetLevel());
	dbLPlayer->SetLongAttr(string("Occupation"), tableName, 0, pPlayer->GetOccupation());
	dbLPlayer->SetLongAttr(string("Sex"), tableName, 0, pPlayer->GetSex());
	dbLPlayer->SetLongAttr(string("Country"), tableName, 0, pPlayer->GetCountry());
	dbLPlayer->SetLongAttr(string("Region"), tableName, 0, pPlayer->GetRegionID());
	
	//	头发
	dbLPlayer->SetLongAttr(string("HEAD"), tableName, 0, pPlayer->GetHeadPic());

	//	头发
	dbLPlayer->SetLongAttr(string("HAIR"), tableName, 0, pPlayer->GetHeadPic());
	//	头发
	dbLPlayer->SetLongAttr(string("FACE"), tableName, 0, pPlayer->GetFacePic());

	BYTE dwNull = 0;
	CGoods* pGoods = NULL;
	// ----------------------------
	//	1：头盔
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_HEAD );
	// ID
	//dbLPlayer->SetLongAttr(string("HEAD"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull));
	dbLPlayer->SetLongAttr(string("HeadLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	2：项链
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_NECKLACE );
	dbLPlayer->SetLongAttr(string("NECKLACE"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("NecklaceLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	3：翅膀
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WING );
	dbLPlayer->SetLongAttr(string("WING"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("WingLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	4：盔甲
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BODY );
	dbLPlayer->SetLongAttr(string("BODY"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("BodyLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	5：腰带
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BACK );
	dbLPlayer->SetLongAttr(string("BACK"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("BackLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	6：手套	
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_GLOVE );
	dbLPlayer->SetLongAttr(string("GLOVE"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("GloveLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	7：鞋子
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_BOOT );
	dbLPlayer->SetLongAttr(string("BOOT"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("BootLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	8：头饰
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_HEADGEAR );
	dbLPlayer->SetLongAttr(string("HEADGEAR"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("HeadgearLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	9：外套		
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_FROCK );
	dbLPlayer->SetLongAttr(string("FROCK"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("FrockLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	10：左戒指
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_LRING );
	dbLPlayer->SetLongAttr(string("LRING"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("LRingLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	11：右戒指		
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_RRING );
	dbLPlayer->SetLongAttr(string("RRING"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("RRingLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	12：勋章1
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL1 );
	dbLPlayer->SetLongAttr(string("MEDAL1"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("Medal1Level"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	13	勋章2
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL2 );
	dbLPlayer->SetLongAttr(string("MEDAL2"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("Medal2Level"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	14	勋章3
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_MEDAL3 );
	dbLPlayer->SetLongAttr(string("MEDAL3"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("Medal3Level"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	15：小精灵
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_FAIRY );
	dbLPlayer->SetLongAttr(string("FAIRY"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("FairyLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	16：武器
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON );
	dbLPlayer->SetLongAttr(string("WEAPON"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) );// ID
	dbLPlayer->SetLongAttr(string("WeaponLevel"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level
	//	17：副手武器
	pGoods = pPlayer -> m_cEquipment.GetGoods(CEquipmentContainer::EC_WEAPON2 );
	dbLPlayer->SetLongAttr(string("WEAPON2"), tableName, 0,(DWORD)(pGoods?pGoods->GetBasePropertiesIndex():dwNull) ); // ID
	dbLPlayer->SetLongAttr(string("Weapon2Level"), tableName, 0,(DWORD)(pGoods?pGoods->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL,1):dwNull) ); // Level

	dbLPlayer->SetLongAttr(string("Region"), tableName, 0,  (long)pPlayer->GetRegionID() );
}
void CDBEntityManager::CPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(!dbPlayer->GetDataEntityManager().GetEntityPropertyMap().size())
		AddLogText("CPlayerToCDBPlayer() db对象的属性配置信息为空!");

	if(pPlayer && dbPlayer)
	{
		// 清除上次物品记录
		DetailPropertyCPlayerToCDBPlayer(pPlayer, dbPlayer);
		DetailGoodsGroupCPlayerToCDBPlayer(pPlayer, dbPlayer);

		map<DWORD,DWORD>::iterator creditIt=pPlayer->GetCreditMap()->begin();
		long creditNum = pPlayer->GetCreditMap()->size();
		long bufSize = creditNum*sizeof(DWORD)*2+sizeof(long);
		if(bufSize)
		{
			BYTE* buf = new BYTE[bufSize];
			memcpy((BYTE*)buf, &creditNum, sizeof(long));
			long count = 0;
			for(;creditIt!=pPlayer->GetCreditMap()->end();creditIt++)
			{
				long value1 = creditIt->first;
				long value2 = creditIt->second;
				memcpy(&(((BYTE*)buf)[sizeof(long)+sizeof(DWORD)*2*count]), &value1, sizeof(DWORD));
				memcpy(&(((BYTE*)buf)[sizeof(long)+sizeof(DWORD)*2*count+sizeof(DWORD)]), &value2, sizeof(DWORD));
				count++;
			}
			dbPlayer->SetBufAttr(string("AreaCredit"), string("baseproperty"), 0, buf, bufSize);
			SAFE_DELETE_ARRAY(buf);
		}

		//! 钱包
		//DetailWalletCPlayerToCDBPlayer(pPlayer, dbPlayer);
		//DetailSilverCPlayerToCDBPlayer(pPlayer,dbPlayer);

		//! 宝石
	
		//! 新仓库
		//GoodsContainerList ContainerList;
		//pPlayer->m_PlayerDepot.GetGoodsContainerList(ContainerList);
		//for (GoodsContainerList::iterator ite = ContainerList.begin(); ite != ContainerList.end(); ++ite)
		//{
		//	dbPlayer->ClearContainerMap((PLAYER_EXTEND_ID)ite->second);
		//	long lNum = 0;
		//	for (long i = 0; lNum < ite->first->GetGoodsAmount(); ++i)
		//	{
		//		long lPos = i;
		//		if (eDCT_Secondary == ite->second)
		//		{
		//			lPos += 10001;
		//		}
		//		CGoods *pGoods = ite->first->GetGoods(lPos);
		//		if (NULL != pGoods)
		//		{
		//			CDBGoods* tGoods = new CDBGoods(pGoods->GetExID());
		//			dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pGoods->GetExID()] = tGoods;
		//			tGoods->SetLongAttr(string("place"), string("equipment"), 0,ite->second);
		//			tGoods->SetLongAttr(string("position"), string("equipment"), 0,lPos);

		//			GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pGoods, tGoods, pPlayer->GetExID());

		//			map<long, set<CGUID>>::iterator itr = dbPlayer->GetGoodsGroupPtr()->GetDirtyGoodsMap().find(ite->second);
		//			if(itr != dbPlayer->GetGoodsGroupPtr()->GetDirtyGoodsMap().end())
		//				itr->second.insert(pGoods->GetExID());
		//			else
		//			{
		//				set<CGUID> setGoods;
		//				setGoods.insert(pGoods->GetExID());
		//				dbPlayer->GetGoodsGroupPtr()->GetDirtyGoodsMap()[ite->second] = setGoods;
		//			}

		//			++lNum;
		//		}
		//		//! 逻辑上来说，i若超过256（仓库和子背包的最大容量都远小于这个数），一定是出了错，不需要在继续找物品了
		//		assert(256 > i);
		//		if (256 < i) break;
		//	}
		//}
		//! 新仓库信息
		dbPlayer->SetLongAttr(string("DepotFrostDate"),	string("baseproperty"), 0,pPlayer->m_PlayerDepot.GetThawdate());
		dbPlayer->SetStringAttr(string("DepotPwd"),			string("baseproperty"), 0, pPlayer->m_PlayerDepot.GetPassword());
		dbPlayer->SetLongAttr(string("DepotSubFlag"),		string("baseproperty"), 0,pPlayer->m_PlayerDepot.GetSubpackFlag());

		dbPlayer->SetLongAttr(string("ticket"),		string("baseproperty"), 0,pPlayer->GetTicket());

		DetailSkillCPlayerToCDBPlayer(pPlayer, dbPlayer);

		DetailVarDataCPlayerToCDBPlayer(pPlayer, dbPlayer);

		DetailStateCPlayerToCDBPlayer(pPlayer, dbPlayer);

		DetailQuestCPlayerToCDBPlayer(pPlayer, dbPlayer);

		DetailMerQuestCPlayerToCDBPlayer(pPlayer, dbPlayer);

		DetailMedalCPlayerToCDBPlayer(pPlayer, dbPlayer);
		//-------------------------------------------------------------------------------
		// 最近购买的10种商品列表. by Fox.		2008-02-25
		list<DWORD> lstIncShopCur10 = pPlayer->GetIncShopCur10();
		long lSize = lstIncShopCur10.size();
		if(lSize)
		{
			BYTE* incBuf = new BYTE[lSize*sizeof(DWORD)+sizeof(DWORD)];
			memcpy(&((BYTE*)incBuf)[0], &lSize, sizeof(DWORD));
			long incNum = 0;
			for( list<DWORD>::iterator it=lstIncShopCur10.begin(); it!=lstIncShopCur10.end(); ++it )
			{
				DWORD value = *it;
				memcpy(&((BYTE*)incBuf)[incNum*sizeof(DWORD)+sizeof(DWORD)], &value, sizeof(DWORD));
				incNum++;
			}
			dbPlayer->SetBufAttr(string("IncShopCur10"), string("baseproperty"), 0, incBuf, lSize*sizeof(DWORD)+sizeof(DWORD));
			SAFE_DELETE_ARRAY(incBuf);
		}

		// new Occupation
		vector<BYTE> doPBA;
		pPlayer->AddDOccuDataToByteArray(&doPBA);
		dbPlayer->SetBufAttr(string("DOccuData"), string("baseproperty"), 0, &doPBA[0], (long)doPBA.size());

		doPBA.clear();
		pPlayer->m_SpriteSystem.AddToByteArray(doPBA);
		dbPlayer->SetBufAttr(string("AutoSupplyData"), string("CSL_Player_Sprite_System"), 0, &doPBA[0], (long)doPBA.size());

		doPBA.clear();
		pPlayer->m_SetupOnServer.AddToByteArray(doPBA);
		dbPlayer->SetBufAttr(string("PlayerSetup"), string("CSL_Player_Sprite_System"),0, &doPBA[0], (long)doPBA.size());
	}
}
// 邮件对象转化
void CDBEntityManager::CMailToCDBMail(CMail* pMail, CDBMail* pDBMail)
{
	string tblName = "CSL_MAIL";

	if(pMail && pDBMail)
	{
		pDBMail->SetGuidAttr(string("PlayerID"), tblName, 0, pMail->GetWriterExID());
		pDBMail->SetGuidAttr(string("guid"), tblName, 0, pMail->GetExID());
		pDBMail->SetLongAttr(string("type"), tblName, 0, pMail->GetType());
		pDBMail->SetStringAttr(string("Subject"), tblName, 0, pMail->GetSubject().c_str());
		pDBMail->SetStringAttr(string("Writer"), tblName, 0, pMail->GetWriter().c_str());
		pDBMail->SetBufAttr(string("Text"), tblName, 0, (VOID*)pMail->GetText().c_str(), pMail->GetText().size());
		pDBMail->SetBufAttr(string("ExText"), tblName, 0, (VOID*)pMail->GetExText().c_str(), pMail->GetExText().size());
		pDBMail->SetLongAttr(string("RemainTime"), tblName, 0, pMail->GetRemainTime());
//		pDBMail->SetLongAttr(string("Date"), tblName, 0, pMail->GetDate());
		pDBMail->SetLongAttr(string("Gold"), tblName, 0, pMail->GetGold());
		pDBMail->SetLongAttr(string("Read"), tblName, 0, pMail->GetRead());
		
		long IncGoodsNum = 0;
		long incGoodsBufSize = pMail->GetSGood().size() * sizeof(tagSGoods)+sizeof(long);
		BYTE* incGoodsBuf = new BYTE[incGoodsBufSize];
		BYTE* ptr = incGoodsBuf;
		list<tagSGoods*>::iterator IncGoodsItr = pMail->GetSGood().begin();
		*(long*)ptr = (long)pMail->GetSGood().size();
		ptr += sizeof(long);
		for(; IncGoodsItr != pMail->GetSGood().end(); IncGoodsItr++)
		{
			memcpy(&(((BYTE*)ptr)[IncGoodsNum*sizeof(tagSGoods)]), &(*IncGoodsItr), sizeof(tagSGoods));
			IncGoodsNum++;
		}
		pDBMail->SetBufAttr(string("IncGoods"), tblName, 0, incGoodsBuf, incGoodsBufSize);
		SAFE_DELETE_ARRAY(incGoodsBuf);

		// 物品转换
		pDBMail->ClearGoodsSet();
		vector<CGoods*>::iterator goodsItr = pMail->GetMGoodsContainer().begin();
		for(; goodsItr!=pMail->GetMGoodsContainer().end(); goodsItr++)
		{
			CDBMailGoods* pDBMailGoods = new CDBMailGoods((*goodsItr)->GetExID());
			CGoodsToCDBMailGoods(*goodsItr, pDBMailGoods, pMail->GetExID());
			pDBMail->GetGoodsGroupPtr()->GetGoodsGroupSet().insert(pDBMailGoods);
		}
	}
}
// 邮件物品对象转换
void CDBEntityManager::CGoodsToCDBMailGoods(CGoods* pGoods, CDBMailGoods* pDBMailGoods, const CGUID& ownerID)
{
	if(pGoods && pDBMailGoods)
	{
		string tableName = "equipment";
		pDBMailGoods->SetGuidAttr(string("goodsID"), tableName, 0, pGoods->GetExID());
		pDBMailGoods->SetLongAttr(string("goodsIndex"), tableName, 0, pGoods->GetBasePropertiesIndex());
		pDBMailGoods->SetGuidAttr(string("MailID"), tableName, 0, ownerID);
		pDBMailGoods->SetStringAttr(string("name"), tableName, 0, pGoods->GetName());
		pDBMailGoods->SetLongAttr(string("price"), tableName, 0, pGoods->GetPrice());
		pDBMailGoods->SetLongAttr(string("amount"), tableName, 0, pGoods->GetAmount());

		DWORD dwMaxHole=pGoods->GetMaxEnchaseHoleNum();
		if(dwMaxHole>0)
		{
			LONG* pHoleData=pGoods->GetEnchaseHoleData();
			if(pHoleData)
			{

				pDBMailGoods->SetLongAttr(string("holeNum"), tableName, 0,dwMaxHole);
				pDBMailGoods->SetLongAttr(string("hole1Idx"), tableName, 0,pHoleData[0]);
				pDBMailGoods->SetLongAttr(string("hole2Idx"), tableName, 0,pHoleData[1]);
				pDBMailGoods->SetLongAttr(string("hole3Idx"), tableName, 0,pHoleData[2]);
				pDBMailGoods->SetLongAttr(string("hole4Idx"), tableName, 0,pHoleData[3]);
				pDBMailGoods->SetLongAttr(string("hole5Idx"), tableName, 0,pHoleData[4]);
			}
			SAFE_DELETE_ARRAY(pHoleData);
		}

		vector<BYTE> m_vecAddonProperty;
		vector<CGoods::tagAddonProperty>& vecAddonProperty=pGoods->GetAllAddonProperties();
		DWORD dwAddonSize=vecAddonProperty.size();	
		_AddToByteArray(&m_vecAddonProperty,dwAddonSize);
		for(int i=0;i<dwAddonSize;i++)
		{
			//addon id
			_AddToByteArray(&m_vecAddonProperty,(DWORD)vecAddonProperty[i].gapType);
			_AddToByteArray(&m_vecAddonProperty,vecAddonProperty[i].vValues[0].lBaseValue);
			_AddToByteArray(&m_vecAddonProperty,vecAddonProperty[i].vValues[1].lBaseValue);
		}

		pDBMailGoods->SetBufAttr(string("AddonProperty"), tableName,0, &m_vecAddonProperty[0], m_vecAddonProperty.size());
	}
}
void CDBEntityManager::CPlayerToCDBPlayerMails(CPlayer* player, CDBPlayer* dbPlayer)
{
	dbPlayer->ClearDBMailMap();
	map<CGUID,CMail*>::iterator itr = player->m_mapMail.begin();
	for(; itr != player->m_mapMail.end(); itr++)
	{
		CDBMail* dbMail = new CDBMail(itr->first);
		CMailToCDBMail(itr->second, dbMail);
		dbPlayer->AddOneDBMail(dbMail);
	}
}
void CDBEntityManager::CDBPlayerToCPlayerMails(CDBPlayer* dbPlayer, CPlayer* player)
{
	map<CGUID,CMail*>::iterator itr = player->m_mapMail.begin();
	for(; itr != player->m_mapMail.end(); itr++)
	{
		delete itr->second;
	}
	player->m_mapMail.clear();

	set<CDBMail*>::iterator mailItr = dbPlayer->GetMailGroupPtr()->GetMailGroupSet().begin();
	for(; mailItr != dbPlayer->GetMailGroupPtr()->GetMailGroupSet().end(); mailItr++)
	{
		CMail* mail = new CMail;
		CDBMailToCMail(*mailItr, mail);
		player->m_mapMail[mail->GetExID()] = mail;
	}
}
void CDBEntityManager::CFactionToCDBFaction(CFaction* fac, CDBFaction* dbFac)
{
	if (NULL == fac || NULL == dbFac) return;

	dbFac->Release();
	//! 基本属性表
	if( 0 != (fac->m_dwDataChanged & eFDCF_BaseInfo))
	{
		string strBaseTableName = "CSL_Faction_BaseProperty";
		dbFac->SetExID(fac->GetExID());
		dbFac->SetGuidAttr(string("FactionGuid"), strBaseTableName, 0, fac->GetExID());
		dbFac->SetStringAttr(string("Name"), strBaseTableName, 0, fac->GetName());
		//! 当前使用的技能ID
		dbFac->SetLongAttr(string("CurrSkillID"), strBaseTableName, 0,fac->GetPublicData().lCurrSkillID);
		
		//! 工会等级
		dbFac->SetLongAttr(string("Levels"), strBaseTableName, 0,fac->GetLvl());
		//! 基本信息数据
		dbFac->SetLongAttr(string("CountryID"), strBaseTableName, 0, fac->GetBaseData().lCountryID);
		dbFac->SetGuidAttr(string("MasterGuid"), strBaseTableName, 0, fac->GetBaseData().MasterGuid);
		dbFac->SetStringAttr(string("MasterName"), strBaseTableName, 0, fac->GetBaseData().szMasterName);
		dbFac->SetLongAttr(string("CreateTime"), strBaseTableName, 0, fac->GetBaseData().CreateTime);
		dbFac->SetLongAttr(string("CurrRes"), strBaseTableName, 0, fac->GetBaseData().lCurrRes);
		dbFac->SetGuidAttr(string("SuperiorGuid"), strBaseTableName, 0,fac->GetBaseData().SuperiorGuid);
		dbFac->SetLongAttr(string("IsRecruiting"), strBaseTableName, 0, fac->GetBaseData().bIsRecruiting);

		dbFac->SetStringAttr(string("JobName1"), strBaseTableName, 0, fac->GetBaseData().arr_szJobName[0]);
		dbFac->SetStringAttr(string("JobName2"), strBaseTableName, 0, fac->GetBaseData().arr_szJobName[1]);
		dbFac->SetStringAttr(string("JobName3"), strBaseTableName, 0, fac->GetBaseData().arr_szJobName[2]);
		dbFac->SetStringAttr(string("JobName4"), strBaseTableName, 0, fac->GetBaseData().arr_szJobName[3]);
		dbFac->SetStringAttr(string("JobName5"), strBaseTableName, 0, fac->GetBaseData().arr_szJobName[4]);

		dbFac->SetLongAttr(string("JobPurview1"), strBaseTableName, 0, fac->GetBaseData().arrJobPurview[0]);
		dbFac->SetLongAttr(string("JobPurview2"), strBaseTableName, 0, fac->GetBaseData().arrJobPurview[1]);
		dbFac->SetLongAttr(string("JobPurview3"), strBaseTableName, 0, fac->GetBaseData().arrJobPurview[2]);
		dbFac->SetLongAttr(string("JobPurview4"), strBaseTableName, 0, fac->GetBaseData().arrJobPurview[3]);
		dbFac->SetLongAttr(string("JobPurview5"), strBaseTableName, 0, fac->GetBaseData().arrJobPurview[4]);
				
		//! 宣言
		dbFac->SetGuidAttr(string("Pronounce_MemberGuid"), strBaseTableName, 0,fac->GetPronounceWord().MemberGuid);
		dbFac->SetStringAttr(string("Pronounce_MemberName"), strBaseTableName, 0, fac->GetPronounceWord().szName);
		dbFac->SetBufAttr(string("Pronounce_Body"), strBaseTableName, 0,&(fac->GetPronounceWord().szBody[0]), MAX_PronounceCharNum);
		dbFac->SetLongAttr(string("Pronounce_TimeSpouseParam"), strBaseTableName, 0, fac->GetPronounceWord().Time);
		
		//! 会徽
		vector<BYTE> vecIconData;
		fac->GetIcon(vecIconData);
		if(vecIconData.size())
			dbFac->SetBufAttr(string("Icon_Data"), strBaseTableName, 0, &vecIconData[0], vecIconData.size());
	}
	//! 成员表
	if( 0 != (fac->m_dwDataChanged & eFDCF_Member))
	{
		string strMemberTableName = "CSL_Faction_Members";
		CDBFactionMemberGroup* tMemberGroup = dbFac->GetFactionMemberGroupPtr();
		if(tMemberGroup)
		{
			tMemberGroup->Clear();
			tMemberGroup->SetOwnerGUID(fac->GetExID());
			
			map<CGUID, tagFacMemInfo>::iterator memItr = fac->GetMemberMap().begin();
			for(; memItr != fac->GetMemberMap().end(); memItr++)
			{
				//! 成员信息列表
				CDBFactionMember* tFacMember = new CDBFactionMember(memItr->second.MemberGuid);
				tFacMember->SetGuidAttr(string("FactionGuid_members"), strMemberTableName, 0,fac->GetExID());
				tFacMember->SetGuidAttr(string("MemberGuid"), strMemberTableName, 0, memItr->second.MemberGuid);
				tFacMember->SetStringAttr(string("Name_Members"), strMemberTableName, 0, memItr->second.szName);
				tFacMember->SetStringAttr(string("Title"), strMemberTableName, 0, memItr->second.szTitle);
				tFacMember->SetLongAttr(string("Levels_Members"), strMemberTableName, 0, memItr->second.lLvl);
				tFacMember->SetLongAttr(string("Occu_Members"), strMemberTableName, 0, memItr->second.lOccu);
				tFacMember->SetLongAttr(string("JobLvl"), strMemberTableName, 0, memItr->second.lJobLvl);
				tFacMember->SetLongAttr(string("LastOnlineTime"), strMemberTableName, 0, memItr->second.LastOnlineTime);
				tFacMember->SetLongAttr(string("Contribute"), strMemberTableName, 0, memItr->second.lContribute);
				tMemberGroup->GetMemberGroupSet().insert(tFacMember);
			}
		}
	}
	//! 申请加入列表
	if( 0 != (fac->m_dwDataChanged & eFDCF_Apply))
	{
		string strApplyTableName = "CSL_Faction_Apply";
		//! 申请列表
		CDBFactionApplyGroup* tApplyGroup = dbFac->GetFactionApplyGroupPtr();
		if(tApplyGroup)
		{
			tApplyGroup->Clear();
			tApplyGroup->SetOwnerGUID(fac->GetExID());
			map<CGUID, tagFacApplyPerson>::iterator applyItr = fac->GetApplyMap().begin();
			for(; applyItr != fac->GetApplyMap().end(); applyItr++)
			{
				//! 成员信息列表
				CDBFactionApply* tFacApply = new CDBFactionApply(applyItr->second.PlayerGuid);
				tFacApply->SetGuidAttr(string("FactionGuid_Apply"), strApplyTableName, 0,fac->GetExID());
				tFacApply->SetGuidAttr(string("PlayerGuid"), strApplyTableName, 0, applyItr->second.PlayerGuid);
				tFacApply->SetStringAttr(string("Name_Apply"), strApplyTableName, 0, applyItr->second.szName);
				tFacApply->SetLongAttr(string("Levels_Apply"), strApplyTableName, 0, applyItr->second.lLvl);
				tFacApply->SetLongAttr(string("Occu_Apply"), strApplyTableName, 0, applyItr->second.lOccu);
				tFacApply->SetLongAttr(string("Time"), strApplyTableName, 0, applyItr->second.lTime);
				tApplyGroup->GetApplayGroupSet().insert(tFacApply);
			}
		}
	}
}



void CDBEntityManager::CCardToCDBCard(CCard* card, CDBCard* dbCard)
{
	dbCard->SetSellerAccount(card->GetSellerAccount(), strlen((card->GetSellerAccount())));
	dbCard->SetPrice(card->GetPrice());
}


//! 辅助CDBGoodToCGoods接口使用的函数
inline void _UpdateGoodsPropertiesValue(CGoods *pGoods, CDBGoods *pDbGoods, CGoodsBaseProperties::GOODS_ADDON_PROPERTIES eBasePropertiesType, long lValue, long Pos = 1)
{
	bool bAddonProperyExist = pGoods->IsAddonProperyExist(eBasePropertiesType);
	if (-1 != lValue)
	{
		if (!bAddonProperyExist)
			pGoods->AddAddonPropertyBaseValues(eBasePropertiesType, Pos, lValue);
		else
			pGoods->SetAddonPropertyBaseValues(eBasePropertiesType, Pos, lValue);
	}
	else
	{
		if (bAddonProperyExist)
			pGoods->DelAddonPropertyBaseValues(eBasePropertiesType);
	}
}

void CDBEntityManager::CDBGoodToCGoods(CDBGoods* dbGood, CGoods* good, const CGUID& ownerID)
{
	char szGUID[128];
	ownerID.tostring(szGUID);
	char goodSZGUID[128];
	CGUID tempGuid = dbGood->GetExID();
	tempGuid.tostring(goodSZGUID);

	string goodsTameName = "equipment";
	good->SetExID(tempGuid);
	good->SetName(dbGood->GetDataEntityManager().GetStringAttr(string("name"), 0));
	good->SetPrice(dbGood->GetDataEntityManager().GetLongAttr(string("price"), 0));
	good->SetAmount(dbGood->GetDataEntityManager().GetLongAttr(string("amount"), 0));
	//镶嵌
	good->InitEnchaseHole();
	LONG lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole1Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(0,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(0,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(0,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(32);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,0);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole2Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(1,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(1,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(1,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(33);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,1);
	}
	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole3Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(2,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(2,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(2,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(34);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,2);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole4Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(3,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(3,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(3,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(35);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,3);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole5Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(4,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(4,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(4,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(36);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,4);
	}

	CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(good->GetBasePropertiesIndex());
	if(pProperty)
	{
		good->SetGoodsBaseType(pProperty->GetGoodsBaseType());
	}

	CEntityProperty* ep = dbGood->GetDataEntityManager().GetEntityProperty(string("AddonProperty"));
	if(ep)
	{
		if(ep->GetBufSize(0))
		{
			BYTE* addonBuf = new BYTE[ep->GetBufSize(0)];
			ep->GetBufAttr(0,addonBuf, ep->GetBufSize(0));
			// 将addonBuf数据解码到CGoods
			good->AddAddonProperty(addonBuf);
			SAFE_DELETE_ARRAY(addonBuf);
		}
	}
}
void CDBEntityManager::CDBLoginPlayerToCPlayer(CDBLoginPlayer* dbLPlayer, CPlayer* player)
{
	player->SetName(dbLPlayer->GetStringAttr(string("Name"), 0));
	player->SetAccount(dbLPlayer->GetStringAttr(string("Account"), 0));
	player->SetLevel(dbLPlayer->GetLongAttr(string("Levels"), 0));
	player->SetOccupation(dbLPlayer->GetLongAttr(string("Occupation"), 0));
	player->SetSex(dbLPlayer->GetLongAttr(string("Sex"), 0));
	player->SetCountry(dbLPlayer->GetLongAttr(string("Country"), 0));
	player->SetRegionID(dbLPlayer->GetLongAttr(string("Region"), 0));

	player->SetHeadPic(dbLPlayer->GetLongAttr(string("HEAD"), 0));
	player->SetFacePic(dbLPlayer->GetLongAttr(string("FACE"), 0));
}
void CDBEntityManager::CDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		string playerTableName = "baseproperty";
		string goodsTableName = "equipment";
		DetailPropertyCDBPlayerToCPlayer(dbPlayer, pPlayer);
		//! 清空仓库
		//pPlayer->m_PlayerDepot.Clear();
		//! 仓库信息
		pPlayer->m_PlayerDepot.SetThawdate(dbPlayer->GetDataEntityManager().GetLongAttr(string("DepotFrostDate"), 0));
		pPlayer->m_PlayerDepot.SetSubpackFlag(dbPlayer->GetDataEntityManager().GetLongAttr(string("DepotSubFlag"), 0));
		pPlayer->m_PlayerDepot.SetPassword(dbPlayer->GetDataEntityManager().GetStringAttr(string("DepotPwd"), 0));

		pPlayer->SetTicket(dbPlayer->GetDataEntityManager().GetLongAttr(string("ticket"), 0));

		CEntityProperty* creditEP = dbPlayer->GetDataEntityManager().GetEntityProperty(string("AreaCredit"));
		if(creditEP)
		{
			long creditBufSize = creditEP->GetBufSize(0);
			if(creditBufSize)
			{
				BYTE* creditBuf = new BYTE[creditBufSize];
				creditEP->GetBufAttr(0, (void*)creditBuf, creditBufSize);
				pPlayer->ClearCredit();
				DWORD creditNum = ((DWORD*)creditBuf)[0];
				for(int i=0; i<creditNum; i++)
				{
					pPlayer->AddCredit(creditBuf[sizeof(DWORD)+sizeof(DWORD)*2*i], creditBuf[sizeof(DWORD)+sizeof(DWORD)*2*i+sizeof(DWORD)]);
				}
				SAFE_DELETE_ARRAY(creditBuf);
			}
		}

		DetailGoodsGroupCDBPlayerToCPlayer(dbPlayer, pPlayer);
		//! 装备物品
		//DetailEquipmentCDBPlayerToCPlayer(dbPlayer, pPlayer);

		//! 原始背包
		//DetailOrignalPackCDBPlayerToCPlayer(dbPlayer, pPlayer);
		// subpack 1 goods
		//for (int subpackNum=0; subpackNum < 5; subpackNum++)
		//{
		//	DetailSubpackCDBPlayerToCPlayer(dbPlayer, pPlayer, subpackNum);
		//}
	
		// wallet goods
		//DetailWalletCDBPlayerToCPlayer(dbPlayer, pPlayer);
		//DetailSilverCDBPlayerToCPlayer(dbPlayer,pPlayer);

		// skill data
		DetailSkillCDBPlayerToCPlayer(dbPlayer, pPlayer);
		// Vars data
		DetailVarDataCDBPlayerToCPlayer(dbPlayer, pPlayer);
		// states Data
		DetailStateCDBPlayerToCPlayer(dbPlayer, pPlayer);
		// Quest data [任务系统数据库修改]
		DetailQuestCDBPlayerToCPlayer(dbPlayer, pPlayer);
		// 俑兵任务数据
		DetailMerQuestCDBPlayerToCPlayer(dbPlayer, pPlayer);

		DetailMedalCDBPlayerToCPlayer(dbPlayer, pPlayer);

		//-------------------------------------------------------------------------------
		// 最近购买的10种商品列表. by Fox.		2008-02-26
		CEntityProperty* incEP = dbPlayer->GetDataEntityManager().GetEntityProperty(string("IncShopCur10"));
		if(incEP)
		{
			long lSize = incEP->GetBufSize(0);
			if(lSize)
			{
				BYTE* incBuf = new BYTE[lSize];
				incEP->GetBufAttr(0, (void*)incBuf, lSize);
				pPlayer->GetIncShopCur10().clear();
				long incNum = ((DWORD*)incBuf)[0];
				for( int i=0; i<incNum; i++ )
				{
					pPlayer->GetIncShopCur10().push_back(((DWORD*)incBuf)[1+i]);
				}
				SAFE_DELETE_ARRAY(incBuf);
			}
		}

		// new Occupation
		CEntityProperty* occuEP = dbPlayer->GetDataEntityManager().GetEntityProperty(string("DOccuData"));
		if(occuEP)
		{
			long lSize = occuEP->GetBufSize(0);
			if(lSize)
			{
				BYTE* occuBuf = new BYTE[lSize];
				occuEP->GetBufAttr(0, (void*)occuBuf, lSize);
				long doccuPos = 0;
				if(lSize)
					pPlayer->DecordDOccuDataFromByteArray(occuBuf, doccuPos);
				SAFE_DELETE_ARRAY(occuBuf);
			}
		}
		
		CEntityProperty* spriteEP = dbPlayer->GetDataEntityManager().GetEntityProperty(string("AutoSupplyData"));
		if(spriteEP)
		{
			LONG lSpriteDataPos = 0;
			long lSize = spriteEP->GetBufSize(0);
			if(lSize)
			{
				BYTE* spriteBuf = new BYTE[lSize];
				spriteEP->GetBufAttr(0, (void*)spriteBuf, lSize);
				long doccuPos = 0;
				if(lSize)
					pPlayer->m_SpriteSystem.GetFormByteArray(spriteBuf, lSpriteDataPos);
				SAFE_DELETE_ARRAY(spriteBuf);
			}
		}

		CEntityProperty* setupEP = dbPlayer->GetDataEntityManager().GetEntityProperty(string("PlayerSetup"));
		if(setupEP)
		{
			LONG lSpriteDataPos = 0;
			long lSize = setupEP->GetBufSize(0);
			if(lSize)
			{
				BYTE* setupBuf = new BYTE[lSize];
				setupEP->GetBufAttr(0, (void*)setupBuf, lSize);
				long doccuPos = 0;
				if(lSize)
					pPlayer->m_SetupOnServer.GetFormByteArray(setupBuf, lSpriteDataPos);
				SAFE_DELETE_ARRAY(setupBuf);
			}
		}
	}
}
void CDBEntityManager::CDBFactionToCFaction(CDBFaction* dbFac, CFaction* fac)
{
	if (NULL == fac || NULL == dbFac) return;

	//! 基本属性表
	CGUID tGuid;
	dbFac->GetGuidAttr(string("FactionGuid"), 0, tGuid);
	fac->SetExID(tGuid);
	fac->SetName(dbFac->GetStringAttr(string("Name"), 0));
	//! 当前使用的技能ID
	fac->GetPublicData().lCurrSkillID = dbFac->GetLongAttr(string("CurrSkillID"),  0);

	//! 工会等级
	fac->SetLvl(dbFac->GetLongAttr(string("Levels"), 0));

	//! 基本信息数据
	fac->GetBaseData().lCountryID = dbFac->GetLongAttr(string("CountryID"), 0 );
	dbFac->GetGuidAttr(string("MasterGuid"), 0, fac->GetBaseData().MasterGuid );
	strcpy( fac->GetBaseData().szMasterName, dbFac->GetStringAttr(string("MasterName"), 0) );
	fac->GetBaseData().CreateTime = dbFac->GetLongAttr(string("CreateTime"),0);
	fac->GetBaseData().lCurrRes = dbFac->GetLongAttr(string("CurrRes"), 0);
	dbFac->GetGuidAttr(string("SuperiorGuid"), 0, fac->GetBaseData().SuperiorGuid);
	fac->GetBaseData().bIsRecruiting = dbFac->GetLongAttr(string("IsRecruiting"),  0);

	strcpy(fac->GetBaseData().arr_szJobName[0], dbFac->GetStringAttr(string("JobName1"), 0));
	strcpy(fac->GetBaseData().arr_szJobName[1], dbFac->GetStringAttr(string("JobName2"), 0));
	strcpy(fac->GetBaseData().arr_szJobName[2], dbFac->GetStringAttr(string("JobName3"), 0));
	strcpy(fac->GetBaseData().arr_szJobName[3], dbFac->GetStringAttr(string("JobName4"), 0));
	strcpy(fac->GetBaseData().arr_szJobName[4], dbFac->GetStringAttr(string("JobName5"), 0));

	fac->GetBaseData().arrJobPurview[0] = dbFac->GetLongAttr(string("JobPurview1"),  0);
	fac->GetBaseData().arrJobPurview[1] = dbFac->GetLongAttr(string("JobPurview2"),  0);
	fac->GetBaseData().arrJobPurview[2] = dbFac->GetLongAttr(string("JobPurview3"),  0);
	fac->GetBaseData().arrJobPurview[3] = dbFac->GetLongAttr(string("JobPurview4"),  0);
	fac->GetBaseData().arrJobPurview[4] = dbFac->GetLongAttr(string("JobPurview5"),  0);


	//! 宣言
	dbFac->GetGuidAttr(string("Pronounce_MemberGuid"), 0, fac->GetPronounceWord().MemberGuid);
	strcpy(fac->GetPronounceWord().szName, dbFac->GetStringAttr(string("Pronounce_MemberName"), 0));
	dbFac->GetBufAttr(string("Pronounce_Body"),  0,&(fac->GetPronounceWord().szBody[0]), MAX_PronounceCharNum);
	fac->GetPronounceWord().Time = dbFac->GetLongAttr(string("Pronounce_TimeSpouseParam"), 0);

	//! 会徽
	CEntityProperty* iconEP = dbFac->GetDataEntityManager().GetEntityProperty(string("Icon_Data"));
	if(iconEP)	
	{
		long iconDataSize = iconEP->GetBufSize(0);
		if(iconDataSize)
		{
			BYTE* IconData = new BYTE[iconDataSize];
			iconEP->GetBufAttr(0, IconData, iconDataSize);
			fac->SetIcon(IconData, iconDataSize);
		}
	}
	//! 成员表
	
	CDBFactionMemberGroup* tMemberGroup = dbFac->GetFactionMemberGroupPtr();
	if(tMemberGroup)
	{
		set<CDBFactionMember*>::iterator memItr = tMemberGroup->GetMemberGroupSet().begin();
		for(; memItr != tMemberGroup->GetMemberGroupSet().end(); memItr++)
		{
			tagFacMemInfo tFacMemInfo;
			//! 成员信息列表
			(*memItr)->GetGuidAttr(string("MemberGuid"), 0, tFacMemInfo.MemberGuid);
			strcpy(tFacMemInfo.szName, (*memItr)->GetStringAttr(string("Name_Members"), 0));
			strcpy(tFacMemInfo.szTitle, (*memItr)->GetStringAttr(string("Title"), 0));
			tFacMemInfo.lLvl = (*memItr)->GetLongAttr(string("Levels_Members"),  0 );
			tFacMemInfo.lOccu = (*memItr)->GetLongAttr(string("Occu_Members"),  0 );
			tFacMemInfo.lJobLvl = (*memItr)->GetLongAttr(string("JobLvl"),  0 );
			tFacMemInfo.LastOnlineTime = (*memItr)->GetLongAttr(string("LastOnlineTime"),  0 );
			tFacMemInfo.lContribute = (*memItr)->GetLongAttr(string("Contribute"),  0 );
			fac->GetMemberMap()[tFacMemInfo.MemberGuid] = tFacMemInfo;
		}
	}
	//! 申请加入列表
	CDBFactionApplyGroup* tApplyGroup = dbFac->GetFactionApplyGroupPtr();
	if(tApplyGroup)
	{
		set<CDBFactionApply*>::iterator applyItr = tApplyGroup->GetApplayGroupSet().begin();
		for(; applyItr != tApplyGroup->GetApplayGroupSet().end(); applyItr++)
		{
			tagFacApplyPerson tFacAppyPerson;
			//! 成员信息列表
			(*applyItr)->GetGuidAttr(string("PlayerGuid"),  0, tFacAppyPerson.PlayerGuid);
			strcpy(tFacAppyPerson.szName, (*applyItr)->GetStringAttr(string("Name_Apply"),  0));
			tFacAppyPerson.lLvl = (*applyItr)->GetLongAttr(string("Levels_Apply"),  0);
			tFacAppyPerson.lOccu = (*applyItr)->GetLongAttr(string("Occu_Apply"),  0);
			tFacAppyPerson.lTime = (*applyItr)->GetLongAttr(string("Time"),  0);
			fac->GetApplyMap()[tFacAppyPerson.PlayerGuid] = tFacAppyPerson;
		}
	}
}
void CDBEntityManager::CDBCardToCCard(CDBCard* dbCard, CCard* card)
{
	card->SetSellerAccount(dbCard->GetSellerAccount());
	card->SetPrice(dbCard->GetPrice());
}

void CDBEntityManager::CCountryToCDBCountry(CCountry* country, CDBCountry* dbCountry)
{
	dbCountry->SetID(country->GetID());
	dbCountry->SetKing(country->GetKing());
	dbCountry->SetPower(country->GetPower());
	dbCountry->SetTechLevel(country->GetTechLevel());
}
void CDBEntityManager::CDBCountryToCCountry(CDBCountry* dbCountry, CCountry* country)
{
	country->SetID(dbCountry->GetID());
	country->SetKing(dbCountry->GetKing());
	country->SetPower(dbCountry->GetPower());
	country->SetTechLevel(dbCountry->GetTechLevel());
}

void CDBEntityManager::CVarsListToCDBGenVar(CVariableList* vl, CDBGenVarGroup* pGenVarGroup)
{
	if(!vl || !pGenVarGroup) return;

	pGenVarGroup->Clear();

	string tblName = "CSL_SCRIPTVAR";
	for(int i=0; i<vl->GetVarNum(); i++)
	{
		CGUID tGuid;
		CGUID::CreateGUID(tGuid);
		CDBGenVar* pGenVar = new CDBGenVar(tGuid);
		pGenVar->SetStringAttr(string("varName"), tblName, 0, vl->GetVarList()[i]->Name);
		if(vl->GetVarList()[i]->Array < 0) // 字符串
		{
			pGenVar->SetStringAttr(string("SValue"), tblName, 0, vl->GetVarList()[i]->strSValue);
			pGenVar->SetStringAttr(string("CValue"), tblName, 0, vl->GetVarList()[i]->strValue);
		}
		else if(vl->GetVarList()[i]->Array > 0) // 数组
		{
			char* szArray = new char[64*vl->GetVarList()[i]->Array];
		
			memset(szArray, 0, 64*vl->GetVarList()[i]->Array);
			for(int j=0; j<vl->GetVarList()[i]->Array; j++)
			{
				char szTmp[128];
				gcvt(*(vl->GetVarList()[i]->SValue), 10, szTmp);
				strcat(szArray, szTmp);
				strcat(szArray, ",");
			}
			pGenVar->SetStringAttr(string("SValue"), tblName, 0, szArray);

			memset(szArray, 0, 64*vl->GetVarList()[i]->Array);
			for(int m=0; m<vl->GetVarList()[i]->Array; m++)
			{
				char szTmp[128];
				gcvt(*(vl->GetVarList()[i]->Value), 10, szTmp);
				strcat(szArray, szTmp);
				strcat(szArray, ",");
			}
			pGenVar->SetStringAttr(string("CValue"), tblName, 0, szArray);
			SAFE_DELETE_ARRAY(szArray);
		}
		else if(vl->GetVarList()[i]->Array == 0) // 整数
		{
			char szTmp[128];
			gcvt(*(vl->GetVarList()[i]->SValue), 10, szTmp);
			pGenVar->SetStringAttr(string("SValue"), tblName, 0, szTmp);
			gcvt(*(vl->GetVarList()[i]->Value), 10, szTmp);
			pGenVar->SetStringAttr(string("CValue"), tblName, 0, szTmp);
		}
		pGenVarGroup->GetGenVarGroupSet().insert(pGenVar);
	}

	CVariableList::GuidList::iterator itr = vl->GetGuidList().begin();
	for(; itr!=vl->GetGuidList().end(); itr++)
	{
		CGUID tGuid;
		CGUID::CreateGUID(tGuid);
		CDBGenVar* pGenVar = new CDBGenVar(tGuid);
		pGenVar->SetStringAttr(string("varName"), tblName, 0, itr->first.c_str());
		char szGuid[128];
		itr->second->tostring(szGuid);
		pGenVar->SetStringAttr(string("SValue"), tblName, 0, szGuid);
		pGenVar->SetStringAttr(string("CValue"), tblName, 0, szGuid);
		pGenVarGroup->GetGenVarGroupSet().insert(pGenVar);
	}
}
void CDBEntityManager::CCDBGenVarToVarsList(CDBGenVarGroup* pGenVarGroup, CVariableList* vl)
{
	if(!vl || !pGenVarGroup) return;
	//vl->Release();
	set<CDBGenVar*>::iterator itr = pGenVarGroup->GetGenVarGroupSet().begin();
	for(; itr != pGenVarGroup->GetGenVarGroupSet().end(); itr++)
	{
		char* pStr = (char*)(*itr)->GetStringAttr(string("varName"), 0);
		if(pStr)
		{
			if(pStr[0] == '#') // 字符串
			{
				char* pValue = (char*)(*itr)->GetStringAttr(string("SValue"), 0);
				if(pValue)
					vl->AddVar(pStr, pValue);
			}
			else if(pStr[0] == '$') // 数组
			{
				char* pValue = (char*)(*itr)->GetStringAttr(string("SValue"), 0);
				if(pValue)
				{
					int pos = 0;
					int arrayPos = 0;
					char value[64];
					memset(value, 0, sizeof(value));
					int tmpPos = 0;
					// 先查找是否是数组
					bool isArray = false;
					while(pValue[pos] != '\0')
					{
						if(pValue[pos] == ',')
						{
							isArray = true;
							break;
						}
						pos++;
					}
					pos = 0;
					if(isArray)
					{
						// 统计数组长度
						while(pValue[pos] != '\0')
						{
							if(pValue[pos] == ',')
							{
								arrayPos++;
							}
							pos++;
						}

						vl->AddVar(pStr, arrayPos, 0.0f);

						// 为数组赋值
						pos = 0;
						arrayPos = 0;
						while(pValue[pos] != '\0')
						{
							if(pValue[pos] != ',')
							{
								value[tmpPos] = pValue[pos];
								tmpPos++;
							}
							else if(pValue[pos] == ',')
							{
								vl->SetVarValue(pStr, arrayPos, atof(value));
								arrayPos++;
								memset(value, 0, sizeof(value));
								tmpPos = 0;
							}
							pos++;
						}
					}
					else
					{
						vl->AddVar(pStr, atof(pValue));
					}
				}
			}
			else if(pStr[0] == '@') // 数组
			{
				char* pValue = (char*)(*itr)->GetStringAttr(string("SValue"), 0);
				if(pValue)
				{
					vl->AddGuid(pStr, CGUID(pValue));
				}
			}
		}
	}
}
void CDBEntityManager::CWorldRgnToCDBRgn(CWorldRegion* wRgn, CDBRegion* dbRgn)
{
	tagRegionParam RegionParam = wRgn->GetRegionParam();
	memcpy_s(&dbRgn->GetRegionParam(), sizeof(CDBRegion::tagRegionParam), &RegionParam, sizeof(tagRegionParam));
}
// 副本场景转换
void CDBEntityManager::CWorldRgnToCDBDupRgn(CWorldRegion* wRgn, CDBDupRgn* dbDupRgn)
{
	if(wRgn && dbDupRgn)
	{
		string tblName = "CSL_DUPRGN_PROPERTY";
		dbDupRgn->SetExID(wRgn->GetExID());
		// 转换GUID
		dbDupRgn->SetGuidAttr(string("guid"), tblName, 0, wRgn->GetExID());
		// 转换Owner GUID
		dbDupRgn->SetGuidAttr(string("OwnerGUID"), tblName, 0, wRgn->GetOwnerGUID());

		// 转换Name
		dbDupRgn->SetStringAttr(string("name"), tblName, 0, wRgn->GetName());
		// 转换变量数据
		vector<BYTE> pBA;
		wRgn->GetVariableList()->AddToByteArray(&pBA);
		dbDupRgn->SetBufAttr(string("VarList"), tblName, 0, &pBA[0], (long)pBA.size());
		// 转换TemplateRgnID
		dbDupRgn->SetLongAttr(string("TemplateRgnID"), tblName, 0, wRgn->GetID());
		// GSID
		dbDupRgn->SetLongAttr(string("GSID"), tblName, 0, wRgn->GetGsid());
	}
}

void CDBEntityManager::CCDBRgnToWorldRgn(CDBRegion* dbRgn, CWorldRegion* wRgn)
{
	CDBRegion::tagRegionParam RegionParam = dbRgn->GetRegionParam();
	wRgn->SetParamFromDB(RegionParam.OwnedFactionID, RegionParam.OwnedUnionID,
		RegionParam.lCurrentTaxRate, RegionParam.dwTodayTotalTax, RegionParam.dwTotalTax);
}
// 副本场景转换
void CDBEntityManager::CDBDupRgnToCWorldRgn(CDBDupRgn* dbDupRgn, CWorldRegion* wRgn)
{
	// 转换GUID

	// 转换Owner GUID

	// 转换Name

	// 转换变量数据,第一个long为VarNum,第二个long为GuidNum,后面是VarData

	// 转换房间物品数据
}
void CDBEntityManager::CSetupToCDBSetup(CDBSetup* dbSetup)
{
	dbSetup->SetLeaveWordID(GetGame()->GetDBData()->nLeaveWordID);
	dbSetup->SetPlayerID(GetGame()->GetDBData()->PlayerGuid);
}
void CDBEntityManager::CDBSetupToCSetup(CDBSetup* dbSetup)
{
	GetGame()->GetDBData()->nLeaveWordID = dbSetup->GetLeaveWordID();
	GetGame()->GetDBData()->PlayerGuid = dbSetup->GetPlayerID();
}
void CDBEntityManager::CUnionToCDBUnion(CUnion* u, CDBUnion* dbU)
{

}
//void CDBEntityManager::CVillageWarToCDBVillageWar(tagVilWarSetup* vws, CDBVillageWar* dbs)
//{
//	dbs->SetDecWarFactions(vws->DecWarFactions);
//	dbs->SetRegionID(vws->lVilRegionID);
//	dbs->SetWarStartTime(vws->DeclarWarTime);
//}

void CDBEntityManager::CDBUnionToCUnion(CDBUnion* dbU, CUnion* u)
{

}
//void CDBEntityManager::CCDBVillageWarToVillageWar(CDBVillageWar* dbs)
//{
//	GetVilWarSys()->SetDecWarFactionsFromDB(dbs->GetRegionID(), dbs->GetWarStartTime(), dbs->GetDecWarFactions());
//}

// 邮件对象转化
void CDBEntityManager::CDBMailToCMail(CDBMail* pDBMail, CMail* pMail)
{
	if(pMail && pDBMail)
	{
		CGUID ownerID;
		CGUID guid;
		pDBMail->GetGuidAttr(string("guid"), 0, guid);
		pMail->SetExID(guid);
		pDBMail->GetGuidAttr(string("PlayerID"), 0, ownerID);
		pMail->SetType(pDBMail->GetLongAttr(string("Type"), 0));
		pMail->SetSubject(pDBMail->GetStringAttr(string("Subject"), 0));
		pMail->SetWriter(pDBMail->GetStringAttr(string("Writer"), 0));
		
		CEntityProperty* ep = pDBMail->GetDataEntityManager().GetEntityProperty(string("Text"));
		if(ep)
		{
			long bufSize = ep->GetBufSize(0);
			if(bufSize)
			{
				char* buf = new char[bufSize+1];
				ep->GetBufAttr(0, (void*)buf, bufSize);
				buf[bufSize] = '\0';
				pMail->SetText(buf);
				SAFE_DELETE_ARRAY(buf);
			}
		}
	
		ep = pDBMail->GetDataEntityManager().GetEntityProperty(string("ExText"));
		if(ep)
		{
			long bufSize = ep->GetBufSize(0);
			if(bufSize)
			{
				char* buf = new char[bufSize+1];
				ep->GetBufAttr(0, (void*)buf, bufSize);
				buf[bufSize] = '\0';
				pMail->SetExText(buf);
				SAFE_DELETE_ARRAY(buf);
			}
		}

		pMail->SetRemainTime(pDBMail->GetLongAttr(string("RemainTime"), 0));
//		pMail->SetDate(pDBMail->GetLongAttr(string("Date"), 0));
		pMail->SetGold(pDBMail->GetLongAttr(string("Gold"), 0));
		pMail->SetRead(pDBMail->GetLongAttr(string("Read"), 0));
		pDBMail->GetBufAttr(string("Text"), 0, (VOID*)pMail->GetText().c_str(), pMail->GetText().size());
		
		ep = pDBMail->GetDataEntityManager().GetEntityProperty(string("IncGoods"));
		if(ep)
		{
			long bufSize = ep->GetBufSize(0);
			if(bufSize)
			{
				char* buf = new char[bufSize];
				ep->GetBufAttr(0, (void*)buf, bufSize);
				char* ptr = buf;
				pMail->ClearSGoods();	
				long goodsNum = *(long*)ptr;
				ptr += sizeof(long);
				for(int i=0; i<goodsNum; i++)
				{
					tagSGoods* tSGoods = new tagSGoods;
					memcpy(tSGoods, &ptr[i*sizeof(tagSGoods)], sizeof(tagSGoods));
					pMail->GetSGood().push_back(tSGoods);
				}
				SAFE_DELETE_ARRAY(buf);
			}
		}

		// 物品转换
		pMail->ClearMailGoodsContainer();
		set<CDBMailGoods*>::iterator goodsItr = pDBMail->GetGoodsGroupPtr()->GetGoodsGroupSet().begin();
		for(; goodsItr!=pDBMail->GetGoodsGroupPtr()->GetGoodsGroupSet().end(); goodsItr++)
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(37);
#endif
			CGoods* pGoods = CGoodsFactory::CreateGoods((*goodsItr)->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
			if(pGoods)
				pGoods->SetExID(const_cast<CGUID&>((*goodsItr)->GetExID()));
			CDBMailGoodsToCGoods(*goodsItr, pGoods, ownerID);
			pMail->GetMGoodsContainer().push_back(pGoods);
		}
	}
}
// 邮件物品对象转换
void CDBEntityManager::CDBMailGoodsToCGoods(CDBMailGoods* dbGood, CGoods* good, const CGUID& ownerID)
{
	char szGUID[128];
	ownerID.tostring(szGUID);
	char goodSZGUID[128];
	CGUID tempGuid = dbGood->GetExID();
	tempGuid.tostring(goodSZGUID);

	good->SetExID(tempGuid);
	good->SetName(dbGood->GetDataEntityManager().GetStringAttr(string("name"), 0));
	good->SetPrice(dbGood->GetDataEntityManager().GetLongAttr(string("price"), 0));
	good->SetAmount(dbGood->GetDataEntityManager().GetLongAttr(string("amount"), 0));
	//镶嵌
	good->InitEnchaseHole();
	LONG lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole1Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(0,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(0,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(0,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(38);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,0);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole2Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(1,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(1,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(1,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(39);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,1);
	}
	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole3Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(2,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(2,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(2,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(40);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,2);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole4Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(3,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(3,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(3,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(41);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,3);
	}

	lHoleIdx=dbGood->GetDataEntityManager().GetLongAttr(string("hole5Idx"), 0);
	if(lHoleIdx==-1)
	{
		good->SetEnchaseHoleState(4,FALSE);
	}
	else if(lHoleIdx==0)
	{
		good->SetEnchaseHoleState(4,TRUE);
	}
	else if(lHoleIdx>0)
	{
		good->SetEnchaseHoleState(4,TRUE);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(42);
#endif
		CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx);
		good->Enchase(pCard,4);
	}

	CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(good->GetBasePropertiesIndex());
	if(pProperty)
	{
		good->SetGoodsBaseType(pProperty->GetGoodsBaseType());
	}

	CEntityProperty* ep = dbGood->GetDataEntityManager().GetEntityProperty(string("AddonProperty"));
	if(ep)
	{
		if(ep->GetBufSize(0))
		{
			BYTE* addonBuf = new BYTE[ep->GetBufSize(0)];
			ep->GetBufAttr(0,addonBuf, ep->GetBufSize(0));
			// 将addonBuf数据解码到CGoods
			good->AddAddonProperty(addonBuf);
			SAFE_DELETE_ARRAY(addonBuf);
		}
	}
}


// 分层转换 CPlayer 到 CDBPlayer
void CDBEntityManager::DetailPropertyCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		if(0 > pPlayer->GetRegionID() || pPlayer->GetRegionID() >= 100000)
		{
			AddLogText("CShape::SetRegionID RegionID[%d] is Error!", pPlayer->GetRegionID());
		}
		string tableName = "baseproperty";
		//// player BaseProperties
		dbPlayer->SetGuidAttr(string("guid"), tableName, 0, pPlayer->GetExID());
		dbPlayer->SetAccount(pPlayer->GetAccount(), 64);
		dbPlayer->SetStringAttr(string("Name"), tableName, 0, pPlayer->GetName());
		dbPlayer->SetLongAttr(string("Levels"), tableName, 0,(long)pPlayer->GetLevel());
		dbPlayer->SetLongAttr(string("ShowCountry"), tableName, 0,(long)pPlayer->GetShowCountry());
		dbPlayer->SetLongAttr(string("Occupation"), tableName, 0, (long)pPlayer->GetOccupation());
		dbPlayer->SetLongAttr(string("byAssOccu"), tableName, 0, (long)pPlayer->GetDOccupation());
		dbPlayer->SetLongAttr(string("Sex"), tableName, 0, (long)pPlayer->GetSex());
		dbPlayer->SetLongAttr(string("Country"), tableName, 0, (long)pPlayer->GetCountry());
		dbPlayer->SetLongAttr(string("HeadPic"), tableName, 0, (long)pPlayer->GetHeadPic());
		dbPlayer->SetLongAttr(string("FacePic"), tableName, 0,(long)pPlayer->GetGraphicsID());
		dbPlayer->SetLongAttr(string("RegionID"), tableName, 0,(long)pPlayer->GetRegionID());
		dbPlayer->SetLongAttr(string("PosX"), tableName, 0,(long)pPlayer->GetPosX());
		dbPlayer->SetLongAttr(string("PosY"), tableName, 0,(long)pPlayer->GetPosY());

		//// player ablities
		dbPlayer->SetLongAttr(string("Hp"), tableName, 0,(long)pPlayer->GetHP());
		dbPlayer->SetLongAttr(string("Mp"), tableName, 0, (long)pPlayer->GetMP());
		dbPlayer->SetLongAttr(string("Energy"), tableName, 0, (long)pPlayer->GetEnergy());
		dbPlayer->SetLongAttr(string("Stamina"), tableName, 0, (long)pPlayer->GetStamina());
		dbPlayer->SetLongAttr(string("Levels"), tableName, 0, (long)pPlayer->GetLevel());
		dbPlayer->SetLongAttr(string("Exp"), tableName, 0,(long)pPlayer->GetExp());
		dbPlayer->SetLongAttr(string("PresentExp"), tableName, 0,(long)pPlayer->GetPresentExp());
		////
		dbPlayer->SetLongAttr(string("RankOfNobility"), tableName, 0, (long)pPlayer->GetRankOfNobility());
		dbPlayer->SetLongAttr(string("RankOfNobCredit"), tableName, 0,(long)pPlayer->GetRankOfNobCredit());
		dbPlayer->SetLongAttr(string("RankOfMercenary"), tableName, 0, (long)pPlayer->GetRankOfMercenary());
		dbPlayer->SetLongAttr(string("MercenaryCredit"), tableName, 0,(long)pPlayer->GetMercenaryCredit());
		dbPlayer->SetLongAttr(string("CountryContribute"), tableName, 0,(long)pPlayer->GetCountryContribute());
		dbPlayer->SetLongAttr(string("SpouseID"), tableName, 0,(long)pPlayer->GetSpouseId());
		dbPlayer->SetLongAttr(string("SpouseParam"), tableName, 0,(long)pPlayer->GetSpouseParam());	
		dbPlayer->SetLongAttr(string("BusinessLevel"), tableName, 0, (long)pPlayer->GetBusinessLevel());
		dbPlayer->SetLongAttr(string("BusinessExp"), tableName, 0,(long)pPlayer->GetBusinessExp());
		dbPlayer->SetLongAttr(string("ArtisanLevel"), tableName, 0,(long)pPlayer->GetArtisanLevel());
		dbPlayer->SetLongAttr(string("ArtisanExp"), tableName, 0,(long)pPlayer->GetArtisanExp());
		dbPlayer->SetLongAttr(string("PKCount"), tableName, 0,(long)pPlayer->GetPkCount());
		dbPlayer->SetLongAttr(string("PKValue"), tableName, 0,(long)pPlayer->GetPkValue());
		dbPlayer->SetLongAttr(string("PVPCount"), tableName, 0,(long)pPlayer->GetPVPCount());	
		dbPlayer->SetLongAttr(string("PVPValue"), tableName, 0,(long)pPlayer->GetPVPValue());
		dbPlayer->SetLongAttr(string("PKOnOff"), tableName, 0, (long)pPlayer->GetPKOnOff());
		dbPlayer->SetLongAttr(string("SkillPoint"), tableName, 0,(long)pPlayer->GetSkillPoint());
		dbPlayer->SetLongAttr(string("LastExitGameTime"), tableName, 0,(long)pPlayer->GetLastExitGameTime());
		dbPlayer->SetGuidAttr(string("PHGUID"), tableName, 0, pPlayer->GetPersonalHouseRgnGUID());

		dbPlayer->SetLongAttr(string("DepotFrostDate"),	string("baseproperty"), 0,pPlayer->m_PlayerDepot.GetThawdate());
		dbPlayer->SetStringAttr(string("DepotPwd"),		string("baseproperty"), 0, pPlayer->m_PlayerDepot.GetPassword());
		dbPlayer->SetLongAttr(string("DepotSubFlag"),	string("baseproperty"), 0,pPlayer->m_PlayerDepot.GetSubpackFlag());

		//! 玩家当前的称号类型
		dbPlayer->SetLongAttr(string("TileType"), tableName, 0, (LONG)pPlayer->GetTitleType());
		dbPlayer->SetBufAttr(string("HotKey"), tableName, 0, (BYTE*)pPlayer->GetHotKeyArray(), sizeof(DWORD)*24);
		dbPlayer->SetLongAttr(string("OccuLevel"), tableName, 0,(long)pPlayer->GetOccuLevel());
	}
}
void CDBEntityManager::DetailGoodsGroupCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		dbPlayer->GetGoodsGroupPtr()->Clear();	
		if(dbPlayer)
		{
			//装备物品
			map<CEquipmentContainer::EQUIPMENT_COLUMN,CGoods*>::iterator itr4 = pPlayer->GetEquipmentGoodsMap()->begin();
			CDBGoods* tGoods = NULL;
			for(; itr4 != pPlayer->GetEquipmentGoodsMap()->end(); itr4++) //
			{
				tGoods = new CDBGoods(itr4->second->GetExID());
				tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)PEI_EQUIPMENT);
				tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)itr4->first);
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood((*itr4).second, tGoods, dbPlayer->GetExID());
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[itr4->second->GetExID()] = tGoods;
			}

			// 原始背包物品(除了子背包对象)
			hash_map<CGUID,CGoods*,hash_guid_compare>::iterator itr5 = pPlayer->GetOriginPackGoodsMap()->begin();
			tGoods = NULL;
			for(; itr5 != pPlayer->GetOriginPackGoodsMap()->end(); itr5++) //
			{
				tGoods = new CDBGoods(itr5->second->GetExID());
				DWORD pos = 0;
				pPlayer->getOriginPack()->QueryGoodsPosition(itr5->second, pos);
				tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)PEI_PACKET);
				tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)pos);
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood((*itr5).second, tGoods, dbPlayer->GetExID());
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[itr5->second->GetExID()] = tGoods;
			}

			// 子背包对象容器
			for(int i=0; i< 5; i++)
			{
				CSubpackContainer* pSubpackContainer = pPlayer->GetSubpackContainer();
				CGoods *pGoods = pSubpackContainer->GetGoods(10001+i);
				if(pGoods)
				{
					tGoods = new CDBGoods(pGoods->GetExID());
					tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)PEI_PACK);
					tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)PEI_PACK1+i);
					GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pGoods, tGoods, dbPlayer->GetExID());
					dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pGoods->GetExID()] = tGoods;
				}

				// 再添加子背包物品
				hash_map<CGUID,CGoods*,hash_guid_compare>::iterator itr13 =pPlayer->GetSubPackGoodsMap(i)->begin();
				for(; itr13 != pPlayer->GetSubPackGoodsMap(i)->end(); itr13++) //
				{
					tGoods = NULL;
					tGoods = new CDBGoods(itr13->second->GetExID());
					DWORD pos = 0;
					pPlayer->getSubpackContainer()->GetSubpack(i)->pSubpackContainer->QueryGoodsPosition(itr13->second, pos);
					tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)PEI_PACK1+i);
					tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)pos);
					GetGame()->GetDBEntityManager()->CGoodsToCDBGood((*itr13).second, tGoods, dbPlayer->GetExID());
					dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[itr13->second->GetExID()] = tGoods;
				}
			}

			// 金币
			CGoods *pMoneyGoods = pPlayer->m_cWallet.GetGoodsCoins();
			if(NULL != pMoneyGoods) // 钱包为空,清除DBGoods对象
			{
				CDBGoods* tGoods = new CDBGoods(pMoneyGoods->GetExID());
				string tableName = "eqipment";
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pMoneyGoods, tGoods, dbPlayer->GetExID());
				tGoods->SetLongAttr(string("place"), tableName, 0, (long)PEI_WALLET);
				tGoods->SetLongAttr(string("position"), tableName, 0, (long)0);
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pMoneyGoods->GetExID()] = tGoods;
			}	
			// 银币
			pMoneyGoods = pPlayer->m_cSilverWallet.GetGoodsCoins();
			if(NULL != pMoneyGoods) // 钱包为空,清除DBGoods对象
			{
				CDBGoods* tGoods = new CDBGoods(pMoneyGoods->GetExID());
				string tableName = "eqipment";
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pMoneyGoods, tGoods, dbPlayer->GetExID());
				tGoods->SetLongAttr(string("place"), tableName, 0, (long)PEI_SILVERWALLET);
				tGoods->SetLongAttr(string("position"), tableName, 0, (long)0);
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pMoneyGoods->GetExID()] = tGoods;
			}

			// 主仓库物品
			CVolumeLimitGoodsContainer* pContauner = (CVolumeLimitGoodsContainer*)pPlayer->m_PlayerDepot.FindContainer(eDCT_Primary);
			hash_map<CGUID,CGoods*,hash_guid_compare>::iterator itrDepot5 = pContauner->GetGoodsMap()->begin();
			tGoods = NULL;
			for(; itrDepot5 != pContauner->GetGoodsMap()->end(); itrDepot5++) //
			{
				tGoods = new CDBGoods(itrDepot5->second->GetExID());
				DWORD pos = 0;
				pContauner->QueryGoodsPosition(itrDepot5->second, pos);
				tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)eDCT_Primary);
				tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)pos);
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood((*itrDepot5).second, tGoods, dbPlayer->GetExID());
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[itrDepot5->second->GetExID()] = tGoods;
			}

			// 子仓库容器
			for(int i=0; i< 5; i++)
			{
				CSubpackContainer* pSubpackContainer = (CSubpackContainer*)pPlayer->m_PlayerDepot.FindContainer(eDCT_Secondary);
				CGoods *pGoods = pSubpackContainer->GetGoods(10001+i);
				if(pGoods)
				{
					tGoods = new CDBGoods(pGoods->GetExID());
					tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)eDCT_Secondary);
					tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)PEI_PACK1+i);
					GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pGoods, tGoods, dbPlayer->GetExID());
					dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pGoods->GetExID()] = tGoods;
				}

				// 再添加子背包物品
				CSubpackContainer::tagSubpack* pSubpack=pSubpackContainer->GetSubpack(i);
				if(pSubpack)
				{
					hash_map<CGUID,CGoods*,hash_guid_compare>::iterator itr13 = pSubpack->pSubpackContainer->GetGoodsMap()->begin();
					for(; itr13 != pSubpack->pSubpackContainer->GetGoodsMap()->end(); itr13++) //
					{
						tGoods = NULL;
						tGoods = new CDBGoods(itr13->second->GetExID());
						DWORD pos = 0;
						pSubpack->pSubpackContainer->QueryGoodsPosition(itr13->second, pos);
						tGoods->SetLongAttr(string("place"), string("equipment"), 0,(long)eDCT_Secondary1+i);
						tGoods->SetLongAttr(string("position"), string("equipment"), 0,(long)pos);
						GetGame()->GetDBEntityManager()->CGoodsToCDBGood((*itr13).second, tGoods, dbPlayer->GetExID());
						dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[itr13->second->GetExID()] = tGoods;
					}
				}
			}

			// 仓库金币
			pMoneyGoods = ((CWallet*)(pPlayer->m_PlayerDepot.FindContainer(eDCT_Gold)))->GetGoodsCoins();
			if(NULL != pMoneyGoods) // 钱包为空,清除DBGoods对象
			{
				CDBGoods* tGoods = new CDBGoods(pMoneyGoods->GetExID());
				string tableName = "eqipment";
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pMoneyGoods, tGoods, dbPlayer->GetExID());
				tGoods->SetLongAttr(string("place"), tableName, 0, (long)eDCT_Gold);
				tGoods->SetLongAttr(string("position"), tableName, 0, (long)0);
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pMoneyGoods->GetExID()] = tGoods;
			}	
			// 仓库银币
			pMoneyGoods = ((CSilverWallet*)(pPlayer->m_PlayerDepot.FindContainer(eDCT_Gem)))->GetGoodsCoins();
			if(NULL != pMoneyGoods) // 钱包为空,清除DBGoods对象
			{
				CDBGoods* tGoods = new CDBGoods(pMoneyGoods->GetExID());
				string tableName = "eqipment";
				GetGame()->GetDBEntityManager()->CGoodsToCDBGood(pMoneyGoods, tGoods, dbPlayer->GetExID());
				tGoods->SetLongAttr(string("place"), tableName, 0, (long)eDCT_Gem);
				tGoods->SetLongAttr(string("position"), tableName, 0, (long)0);
				dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap()[pMoneyGoods->GetExID()] = tGoods;
			}

		}
	}
}
void CDBEntityManager::DetailQuestCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// Quest data [任务系统数据库操作] 
		vector<BYTE> pqd;
		pqd.clear();
		pPlayer->AddQuestDataToByteArray(&pqd);
		if(pqd.size())
			dbPlayer->SetBufAttr(string("ListQuest"), string("baseproperty"), 0, (BYTE*)&pqd[0], (long)pqd.size());
	}
}
void CDBEntityManager::DetailMerQuestCPlayerToCDBPlayer	(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// Quest data [任务系统数据库操作] 
		vector<BYTE> pqd;
		pqd.clear();
		pPlayer->AddMerQuestDataByteArray(&pqd);
		if(pqd.size())
			dbPlayer->SetBufAttr(string("MercenaryQuest"), string("baseproperty"), 0, (BYTE*)&pqd[0], (long)pqd.size());
	}
}
void CDBEntityManager::DetailSkillCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		//skills id field
		list<CPlayer::tagSkill>& psl = pPlayer->GetSkillList();
		long skillNum = psl.size();
		long bufSize = pPlayer->GetSkillList().size() * sizeof(CPlayer::tagSkill) + sizeof(long);
		if(skillNum)
		{
			BYTE* buf = new BYTE[bufSize];
			memcpy(&buf[0], &skillNum, sizeof(long));
			list<CPlayer::tagSkill>::iterator itr = psl.begin();
			long num = 0;
			for(; itr != psl.end(); itr++)
			{
				memcpy(&(buf[sizeof(long)+sizeof(CPlayer::tagSkill)*num]), &(*itr), sizeof(CPlayer::tagSkill));
				num++;
			}
			dbPlayer->SetBufAttr(string("ListSkill"), string("baseproperty"), 0, (BYTE*)&buf[0], (long)bufSize);
			SAFE_DELETE_ARRAY(buf);
		}
	}
}
void CDBEntityManager::DetailStateCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// states Data
		if(pPlayer->GetExStates().size())
		{
			dbPlayer->SetBufAttr(string("ListState"), string("baseproperty"), 0, (BYTE*)&pPlayer->GetExStates()[0], (long)pPlayer->GetExStates().size());
		}
	}
}
void CDBEntityManager::DetailVarDataCPlayerToCDBPlayer(CPlayer* pPlayer, CDBPlayer* dbPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// 先删除CDBPlayer的老数据 
		// Vars num
		long vNum = pPlayer->GetVariableNum();
		// Vars guid num
		long gNum = pPlayer->GetGuidNum();
		// vardata len
		long vLen = pPlayer->GetVariableDataLength();
		
		// Vars Data
		long bufSize = vLen + sizeof(long)*3;
		BYTE* buf = new BYTE[bufSize];
		memcpy(&buf[0], &vNum, sizeof(long));
		memcpy(&buf[sizeof(long)], &gNum, sizeof(long));
		memcpy(&buf[sizeof(long)+sizeof(long)], &vLen, sizeof(long));
		char* pVariableList = pPlayer->GetVariableData();
		if(pVariableList && vLen)
			memcpy(&buf[sizeof(long)*3], pVariableList, vLen);
			
		dbPlayer->SetBufAttr(string("VariableList"), string("baseproperty"), 0, (BYTE*)buf, (long)bufSize);
		SAFE_DELETE_ARRAY(buf);

	}
}

void CDBEntityManager::DetailMedalCPlayerToCDBPlayer(CPlayer* player, CDBPlayer* dbPlayer)
{
	if(dbPlayer && player)
	{
		vector<BYTE> vMedalData;
		player->m_MedalContainer.AddToByteArray(vMedalData);
		if(vMedalData.size())
		{
			dbPlayer->SetBufAttr(string("MedalData"), string("baseproperty"), 0, (BYTE*)&vMedalData[0], (long)vMedalData.size());
		}
	}
}
// 分层转换 CDBPlayer 到 CPlayer
void CDBEntityManager::DetailPropertyCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// player BaseProperties
		pPlayer->SetName(dbPlayer->GetDataEntityManager().GetStringAttr(string("Name"), 0));
		pPlayer->SetAccount(dbPlayer->GetAccount());
		pPlayer->SetLevel(dbPlayer->GetDataEntityManager().GetLongAttr(string("Levels"), 0));
		pPlayer->SetOccupation(dbPlayer->GetDataEntityManager().GetLongAttr(string("Occupation"), 0));
		pPlayer->SetDOccupation(eDeputyOccu(dbPlayer->GetDataEntityManager().GetLongAttr(string("byAssOccu"), 0)));

		pPlayer->SetShowCountry((bool)dbPlayer->GetDataEntityManager().GetLongAttr(string("ShowCountry"), 0));

	

		pPlayer->SetSex(dbPlayer->GetDataEntityManager().GetLongAttr(string("Sex"), 0));
		pPlayer->SetCountry(dbPlayer->GetDataEntityManager().GetLongAttr(string("Country"), 0));
		pPlayer->SetRegionID(dbPlayer->GetDataEntityManager().GetLongAttr(string("RegionID"), 0));
		pPlayer->SetGraphicsID(pPlayer->GetSex()+1);
		//// player ablities
		pPlayer->SetPosX(dbPlayer->GetDataEntityManager().GetLongAttr(string("PosX"), 0));
		pPlayer->SetPosY(dbPlayer->GetDataEntityManager().GetLongAttr(string("PosY"), 0));
		pPlayer->SetDir(dbPlayer->GetDataEntityManager().GetLongAttr(string("Dir"), 0));
		pPlayer->SetHeadPic(dbPlayer->GetDataEntityManager().GetLongAttr(string("HeadPic"), 0));
		pPlayer->SetFacePic(dbPlayer->GetDataEntityManager().GetLongAttr(string("FacePic"), 0));
		pPlayer->SetHP(dbPlayer->GetDataEntityManager().GetLongAttr(string("Hp"), 0));
		pPlayer->SetMP(dbPlayer->GetDataEntityManager().GetLongAttr(string("Mp"), 0));
		pPlayer->SetEnergy(dbPlayer->GetDataEntityManager().GetLongAttr(string("Energy"), 0));
		pPlayer->SetStamina(dbPlayer->GetDataEntityManager().GetLongAttr(string("Stamina"), 0));
		pPlayer->SetExp(dbPlayer->GetDataEntityManager().GetLongAttr(string("Exp"), 0));
		pPlayer->SetPresentExp(dbPlayer->GetDataEntityManager().GetLongAttr(string("PresentExp"), 0));
		pPlayer->SetRankOfMercenary(dbPlayer->GetDataEntityManager().GetLongAttr(string("RankOfNobility"), 0));
		pPlayer->SetRankOfNobCredit(dbPlayer->GetDataEntityManager().GetLongAttr(string("RankOfNobCredit"), 0));
		pPlayer->SetMercenaryCredit(dbPlayer->GetDataEntityManager().GetLongAttr(string("MercenaryCredit"), 0));
		pPlayer->SetCountryContribute(dbPlayer->GetDataEntityManager().GetLongAttr(string("CountryContribute"), 0));
		pPlayer->SetSpouseId(dbPlayer->GetDataEntityManager().GetLongAttr(string("SpouseID"), 0));
		pPlayer->SetSpouseParam(dbPlayer->GetDataEntityManager().GetLongAttr(string("SpouseParam"), 0));
		pPlayer->SetBusinessLevel(dbPlayer->GetDataEntityManager().GetLongAttr(string("BusinessLevel"), 0));
		pPlayer->SetBusinessExp(dbPlayer->GetDataEntityManager().GetLongAttr(string("BusinessExp"), 0));
		pPlayer->SetArtisanLevel(dbPlayer->GetDataEntityManager().GetLongAttr(string("ArtisanLevel"), 0));
		pPlayer->SetArtisanExp(dbPlayer->GetDataEntityManager().GetLongAttr(string("ArtisanExp"), 0));
		pPlayer->SetPkCount(dbPlayer->GetDataEntityManager().GetLongAttr(string("PKCount"), 0));
		pPlayer->SetPkValue(dbPlayer->GetDataEntityManager().GetLongAttr(string("PKValue"), 0));
		pPlayer->SetPVPCount(dbPlayer->GetDataEntityManager().GetLongAttr(string("PVPCount"), 0));
		pPlayer->SetPVPValue(dbPlayer->GetDataEntityManager().GetLongAttr(string("PVPValue"), 0));
		pPlayer->SetPKOnOff(dbPlayer->GetDataEntityManager().GetLongAttr(string("PKOnOff"), 0));
		pPlayer->SetSkillPoint(dbPlayer->GetDataEntityManager().GetLongAttr(string("SkillPoint"), 0));
		pPlayer->SetLastExitGameTime(dbPlayer->GetDataEntityManager().GetLongAttr(string("LastExitGameTime"), 0));
		pPlayer->SetTitleType(dbPlayer->GetDataEntityManager().GetLongAttr(string("TileType"), 0));
		CGUID tGuid;
		dbPlayer->GetDataEntityManager().GetGuidAttr(string("PHGUID"), 0, tGuid);
		pPlayer->SetPersonalHouseRgnGUID(tGuid);
		BYTE* hotKeyBuf = new BYTE[sizeof(DWORD)*24];
		dbPlayer->GetDataEntityManager().GetBufAttr(string("HotKey"), 0, (void*)hotKeyBuf, sizeof(DWORD)*24);
		for(int i = 0; i < 24; i++)
		{
			pPlayer->SetHotKey(i, ((DWORD*)hotKeyBuf)[i]);
		}
		SAFE_DELETE_ARRAY(hotKeyBuf);
		pPlayer->SetOccuLevel(dbPlayer->GetDataEntityManager().GetLongAttr(string("OccuLevel"), 0));
	}
}

void CDBEntityManager::DetailGoodsGroupCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		// 清空CPlayer装备物品
		pPlayer->m_cEquipment.Clear();

		// 先创建子背包对象本身
		map<CGUID, CDBGoods*>::iterator placeItr = dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap().begin();
		for(; placeItr != dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap().end(); placeItr++)
		{
			CGoods* tGoods = NULL;
			CDBGoods* dbGoods = placeItr->second;
			if(dbGoods)
			{
				long GPlace = dbGoods->GetDataEntityManager().GetLongAttr(string("place"),0);
				switch(GPlace)
				{
				case PEI_PACK:// 是背包子背包对象
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));
						pPlayer->getSubpackContainer()->AddFromDB(tGoods, GPos);

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->getSubpackContainer()->AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case eDCT_Secondary:// 是背包子背包对象
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));
						pPlayer->m_PlayerDepot.FindContainer(eDCT_Secondary)->AddFromDB(tGoods, GPos);

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->m_PlayerDepot.FindContainer(eDCT_Secondary)->AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				}
			}
		}

		// 再创建物品本身
		placeItr = dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap().begin();
		for(; placeItr != dbPlayer->GetGoodsGroupPtr()->GetGoodsGroupMap().end(); placeItr++)
		{
			CGoods* tGoods = NULL;
			CDBGoods* dbGoods = placeItr->second;
			if(dbGoods)
			{
				long GPlace = dbGoods->GetDataEntityManager().GetLongAttr(string("place"),0);
				switch(GPlace)
				{
				case PEI_PACKET://普通背包
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->getOriginPack()->AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case PEI_EQUIPMENT://装备栏
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->m_cEquipment.AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case PEI_WALLET://金钱
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->m_cWallet.AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case PEI_SILVERWALLET://银币钱包
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->m_cSilverWallet.AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case PEI_PACK1:					//子背包栏位1
				case PEI_PACK2:					//子背包栏位2
				case PEI_PACK3:					//子背包栏位3
				case PEI_PACK4:					//子背包栏位4
				case PEI_PACK5:					//子背包栏位5
					{
						long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);
#ifdef __MEMORY_LEAK_CHECK__
						CBaseObject::SetConstructFlag(43);
#endif
						tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
						if(tGoods != NULL)
							tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));

						if(NULL == tGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CGoods指针为空");
							AddLogText(outStr);
							break;
						}
						if(NULL == dbGoods)
						{
							char outStr[256];
							sprintf(outStr, "CDBPlayerToCPlayer() Equipment CDBGoods指针为空");
							AddLogText(outStr);
							break;
						}
						else
						{
							pPlayer->getSubpackContainer()->GetSubpack(GPlace-10001)->pSubpackContainer->AddFromDB(tGoods, GPos);
							GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
						}
					}
					break;
				case eDCT_Primary:
				case eDCT_Gold:
				case eDCT_Gem:
				case eDCT_Secondary1:
				case eDCT_Secondary2:
				case eDCT_Secondary3:
				case eDCT_Secondary4:
				{
					long GPos = dbGoods->GetDataEntityManager().GetLongAttr(string("position"),0);

#ifdef __MEMORY_LEAK_CHECK__
					CBaseObject::SetConstructFlag(46);
#endif
					tGoods=CGoodsFactory::CreateGoods(dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0));
					if(tGoods != NULL)
					{
						tGoods->SetExID(const_cast<CGUID&>(dbGoods->GetExID()));
						pPlayer->m_PlayerDepot.FindContainer(GPlace)->AddFromDB(tGoods, GPos);
						GetGame()->GetDBEntityManager()->CDBGoodToCGoods(dbGoods, tGoods, dbPlayer->GetExID());
					}
					else
					{
						char szGoodsID[128];
						dbGoods->GetExID().tostring(szGoodsID);
						AddLogText("创建物品ID[%s],物品ID[%d]时失败!", szGoodsID, dbGoods->GetDataEntityManager().GetLongAttr(string("goodsIndex"), 0) );
					}
				}
			}
			}
		}
	}
}

void CDBEntityManager::DetailQuestCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("ListQuest"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* buf = new BYTE[size];
				ep->GetBufAttr(0, (void*)buf, size);
				long pos = 0;
				pPlayer->DecordQuestDataFromByteArray(buf, pos);
				SAFE_DELETE_ARRAY(buf);
			}
		}
	}
}

void CDBEntityManager::DetailMerQuestCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("MercenaryQuest"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* buf = new BYTE[size];
				ep->GetBufAttr(0, (void*)buf, size);
				long pos = 0;
				pPlayer->DecordMerQuestDataFromByteArray(buf,pos);//DecordQuestDataFromByteArray(buf, pos);
				SAFE_DELETE_ARRAY(buf);
			}
		}
	}

}

void CDBEntityManager::DetailSkillCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		//skills id field
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("ListSkill"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* buf = new BYTE[size];
				ep->GetBufAttr(0, (void*)buf, size);
				list<CPlayer::tagSkill>& psl = pPlayer->GetSkillList();
				psl.clear();
				long skillNum = ((long*)buf)[0];
				for(int i=0; i<skillNum; i++)
				{
					CPlayer::tagSkill tSkill;
					memcpy(&tSkill, &buf[sizeof(long)+sizeof(CPlayer::tagSkill)*i], sizeof(CPlayer::tagSkill));
					pPlayer->GetSkillList().push_back(tSkill);
				}
				SAFE_DELETE_ARRAY(buf);
			}
		}
	}
}
void CDBEntityManager::DetailStateCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("ListState"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* buf = new BYTE[size];
				ep->GetBufAttr(0, (void*)buf, size);
				pPlayer->SetExStates(&buf[0], size);
				SAFE_DELETE_ARRAY(buf);
			}
		}
	}
}
void CDBEntityManager::DetailVarDataCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("VariableList"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* buf = new BYTE[size];
				ep->GetBufAttr(0, (void*)buf, size);

				// 先删除Cplayer的老数据 
				char* tmpPtr = NULL;
				pPlayer->SetVariableData(tmpPtr);
				long varNum = ((long*)buf)[0];
				pPlayer->SetVariableNum(varNum);

				// Vars guid num
				long guidNum = ((long*)buf)[1];
				pPlayer->SetGuidNum(guidNum);

				// vardata len
				long varLen = ((long*)buf)[2];
				pPlayer->SetVariableDataLength(varLen);

				// Vars Data
				tmpPtr = new char[varLen];
				memset(tmpPtr, 0, varLen);
				memcpy(tmpPtr, &buf[sizeof(long)*3], varLen);
				pPlayer->SetVariableData(tmpPtr);
				SAFE_DELETE_ARRAY(buf);
			}
		}
	}
}

void CDBEntityManager::DetailMedalCDBPlayerToCPlayer(CDBPlayer* dbPlayer, CPlayer* pPlayer)
{
	if(dbPlayer && pPlayer)
	{
		CEntityProperty* ep = dbPlayer->GetDataEntityManager().GetEntityProperty(string("MedalData"));
		if(ep)
		{
			long size = ep->GetBufSize(0);
			if(size)
			{
				BYTE* pMedalData = new BYTE[size];
				ep->GetBufAttr(0, (void*)pMedalData, size);
				LONG lPos = 0;
				pPlayer->m_MedalContainer.Decode(pMedalData, lPos);
				SAFE_DELETE_ARRAY(pMedalData);
			}
		}
	}
}

