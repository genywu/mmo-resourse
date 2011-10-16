#include "StdAfx.h"

#include "DBentityManager.h"
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
#include "DBDupRgn.h"
#include "DBRgnGoods.h"
#include "DBLinkMan.h"
#include "DBLinkManGroup.h"
#include "..\..\..\nets\netdb\message.h"

#include "DBAdoBase.h"
#include "..\..\..\dbaccess\worlddb\RSCards.h"
#include "..\..\..\dbaccess\worlddb\RsCityWar.h"
#include "..\..\..\dbaccess\worlddb\RsCountry.h"
#include "..\..\..\dbaccess\worlddb\RsEnemyFactions.h"
#include "..\..\..\dbaccess\worlddb\RsFaction.h"
#include "..\..\..\dbaccess\worlddb\RsGenVar.h"
#include "..\..\..\dbaccess\worlddb\RsGoods.h"
#include "..\..\..\dbaccess\worlddb\RsPlayer.h"
#include "..\..\..\dbaccess\worlddb\RsRegion.h"
#include "..\..\..\dbaccess\worlddb\RsSetup.h"
#include "..\..\..\dbaccess\worlddb\RsUnion.h"
#include "..\..\..\dbaccess\worlddb\RsVillageWar.h"
#include "../DBServer/DBServer.h"

#include "DBLoadThread.h"
#include "DBSaveThread.h"
#include "DBAccessThread.h"
#include "../../../public/entityproperty.h"
#include "../../../public/tools.h"
#include "DBGoodsGroup.h"
#include "DBRgnGoodsGroup.h"
#include "DBFactionApplyGroup.h"
#include "DBFactionMemberGroup.h"
#include "DBMail.h"
#include "DBMailGoods.h"
#include "DBMailGroup.h"
#include "DBGenVarGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE   CDBEntityManager::m_hLoadEvent;
HANDLE   CDBEntityManager::m_hSaveEvent;

///[TEST]//////////////////////////////////////////////////////////////////////////
#include "../DBServer/game.h"
//*************************
//输出goods log信息
void CDBEntityManager::PutSomeThingLogInfo(const char *msg)
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
void CDBEntityManager::SaveSomeThingLogText(bool bForce)
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
void CDBEntityManager::AddSomeThingLogText(char* msg, ...)
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

//CDBEntityManager g_DBEntityManager;
DBLoadThread* CDBEntityManager::FindLoadThread(long id)
{
	DBLoadThread* ret = NULL;
	map<long, DBLoadThread*>::iterator itr = m_mapLoadThread.find(id);
	if(itr != m_mapLoadThread.end())
		ret = itr->second;

	return ret;
}
DBSaveThread* CDBEntityManager::FindSaveThread(long id)
{
	DBSaveThread* ret = NULL;
	map<long, DBSaveThread*>::iterator itr = m_mapSaveThread.find(id);
	if(itr != m_mapSaveThread.end())
		ret = itr->second;

	return ret;
}
// 是否还在运行
bool bDBSExit;

CDBEntityManager::CDBEntityManager()
{
	m_hLoadEvent =	CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSaveEvent =	CreateEvent(NULL, TRUE, FALSE, NULL);

	bDBSExit= true;

	m_mapDBLoginPlayers.clear();
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

	m_pAccessThread = NULL;

	InitializeCriticalSection(&m_LoadCriticalSection);
	InitializeCriticalSection(&m_SaveCriticalSection);
	InitializeCriticalSection(&m_DisposedLoadCriticalSection);
	InitializeCriticalSection(&m_DisposedSaveCriticalSection);

	m_pTmpGenVarGroup = new CDBGenVarGroup(NULL_GUID);
}
CDBEntityManager::~CDBEntityManager()
{
	SAFE_DELETE(m_pTmpGenVarGroup);

	m_pAccessThread->GetAdoBasePtr()->CloseCn();
	CoUninitialize();

	bDBSExit = false;

	ClearLoginPlayerMap();
	ClearPlayerMap();
	ClearFactionMap();
	ClearCountryMap();
	ClearUnionMap();
	ClearFactionMap();
	ClearGenVarMap();
	ClearDupRgnMap();
	ClearRegionMap();
	ClearSetupMap();

	std::map<string, CDataEntityManager*>::iterator deItr = m_mapObjAttrDef.begin();
	for(; deItr != m_mapObjAttrDef.end(); deItr++)
	{
		delete deItr->second;
	}
	m_mapObjAttrDef.clear();

	// 创建读线程退出数据
	for (map<long, DBLoadThread*>::iterator ite = m_mapLoadThread.begin(); ite != m_mapLoadThread.end(); ++ite)
	{
		LoadThreadData* tLoadData = new LoadThreadData;
		GetGame()->GetDBEntityManager()->PushLoadThreadData(tLoadData);
	}
	// 等待读线程退出
	for(map<long, DBLoadThread*>::iterator ite = m_mapLoadThread.begin(); ite != m_mapLoadThread.end(); ite++)
	{
		WaitForSingleObject(ite->second->GetQuitWaitHandle(), INFINITE);
	}
	// 删除读线程
	for (map<long, DBLoadThread*>::iterator ite = m_mapLoadThread.begin(); ite != m_mapLoadThread.end(); ++ite)
	{
		ite->second->Exit();
		SAFE_DELETE(ite->second);
	}
	m_mapLoadThread.clear();
	CloseHandle(m_hLoadEvent);

	// 创建写线程退出数据
	for(map<long, DBSaveThread*>::iterator ite = m_mapSaveThread.begin(); ite != m_mapSaveThread.end(); ite++)
	{
		SaveThreadData* tSaveData = new SaveThreadData;
		GetGame()->GetDBEntityManager()->PushSaveThreadData(tSaveData);
	}
	// 等待写线程退出
	for(map<long, DBSaveThread*>::iterator ite = m_mapSaveThread.begin(); ite != m_mapSaveThread.end(); ite++)
	{
		WaitForSingleObject(ite->second->GetQuitWaitHandle(), INFINITE);
	}
	// 删除写线程
	for (map<long, DBSaveThread*>::iterator ite = m_mapSaveThread.begin(); ite != m_mapSaveThread.end(); ++ite)
	{	
		ite->second->Exit();
		SAFE_DELETE(ite->second);
	}
	m_mapSaveThread.clear();
	CloseHandle(m_hSaveEvent);

	SAFE_DELETE(m_pAccessThread);
	DeleteCriticalSection(&m_LoadCriticalSection);
	DeleteCriticalSection(&m_SaveCriticalSection);
	DeleteCriticalSection(&m_DisposedLoadCriticalSection);
	DeleteCriticalSection(&m_DisposedSaveCriticalSection);
}
// 初始化DBThread
void CDBEntityManager::InitDBThread(string strProvider, 
									string strDataSource, 
									string strInitialCatalog,
									string strUserID, 
									string strPassword)
{
	// 创建读写线程
	for(int i=0; i<GetGame()->GetSetup()->dwPlayerLoadThreadNumber; i++)
	{
		DBLoadThread* loadThread1 = new DBLoadThread(strProvider, strDataSource, 
		strInitialCatalog, strUserID, strPassword);
		m_mapLoadThread[i] = loadThread1;
		loadThread1->Begin();
	}

	DBSaveThread* saveThread = new DBSaveThread(strProvider, 
		strDataSource, strInitialCatalog, strUserID, strPassword);
	m_mapSaveThread[1] = saveThread;
	saveThread->Begin();

	m_pAccessThread = new DBAccessThread(strProvider, strDataSource, 
		strInitialCatalog, strUserID, strPassword);

	CMyAdoBase::Initialize(strProvider, strDataSource, strInitialCatalog, strUserID, strPassword);
	m_pAccessThread->GetAdoBasePtr()->OpenCn();
	InitPropertyIniData();
	// 预读策略
	PreLoadPlayerBase();
	PreLoadPlayerAttrs();
}
CDBGenVarGroup*   CDBEntityManager::GetGenVarGroup(void) 
{ 
	return m_pTmpGenVarGroup;
}
// 从消息取得操作对象
CBaseDBEntity* CDBEntityManager::FindEntityFromMsg(DB_OBJ_TYPE type, const CGUID& entityid, const CGUID& ownerID)
{
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

	// Entity Num
	long entityNum = pMsg->GetWord();

	long bufSize = pMsg->GetLong();
	// 消息数据块
	BYTE* msgBuf = new BYTE[bufSize];
	// pos
	long msgBufPos = 0;
	// 获取MsgBuf
	pMsg->GetEx(msgBuf, bufSize);

	CBaseDBEntity* entity = NULL;
	for(int i=0; i<entityNum; i++)
	{
		CGUID entityid;
		_GetBufferFromByteArray(msgBuf, msgBufPos, entityid, bufSize);

#ifdef _RUNSTACKINFO_
		char szObjGuid[128];
		entityid.tostring(szObjGuid);
		char outStr[1024];
		_snprintf(outStr, 1024, "CDBEntityManager::ProcessMsg() ID[%s].", szObjGuid);
		OutputStackInfo(outStr);
#endif

		char entityID[128];
		entityid.tostring(entityID);

		CGUID ownerID;
		if(entityType == DB_MAILGROUP || entityType == DB_GOODSGROUP || entityType == DB_PHRGNGOODSGROUP || entityType == DB_GENVARGROUP
			|| entityType == DB_LINKMANGROUP || entityType == DB_FACTIONMEMBERGROUP || entityType == DB_FACTIONAPPLYGROUP)
		{
			_GetBufferFromByteArray(msgBuf, msgBufPos, ownerID, bufSize); // 拥有者ID
		}

		entity = FindEntityFromMsg((DB_OBJ_TYPE)entityType, entityid, ownerID);
		if(!entity)
		{
			entity = CreateEntityByMsg((DB_OBJ_TYPE)entityType, entityid);
			if(entity)
			{
				entity->ProcessMsg(op, msgid, msgBuf, msgBufPos, bufSize);
				// 为LoginPlayer单独处理添加map映射
				if(entityType == DB_LOGINPLAYER)
				{
					if( ((CDBLoginPlayer*)entity)->GetAccount()[0] != '\0') // 帐号部位空
						AddLoginPlayerByCdkey((CDBLoginPlayer*)entity);
				}
			}
		}
		else
			entity->ProcessMsg(op, msgid, msgBuf, msgBufPos, bufSize);
	}

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
			retEntity = new CDBRegion(guid);
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
// CDBLoginPlayer////////////////////////////////////////////////////////////////////////
// 添加一个CDBLoginPlayer到Map末尾
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
// 删除一个CDBLoginPlayer
void CDBEntityManager::DelOneDBLoginPlayer(CDBLoginPlayer* player)
{
	if(player)
	{
		DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(player->GetExID());
		if(itr != m_mapDBLoginPlayers.end())
		{
			// 重复删除
			DelLoginPlayerByCdkey(player);

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
		SAFE_DELETE(itr->second);
		// 同时清除CDBLoginPlayerMap
		m_mapDBLoginPlayers.erase(itr);
	}
}
CDBLoginPlayer* CDBEntityManager::FindLoginPlayer(const CGUID& guid)
{
	CDBLoginPlayer* lPlayer = NULL;
	DBLoginPlayerMapItr itr = m_mapDBLoginPlayers.find(guid);
	if(itr != m_mapDBLoginPlayers.end())
		lPlayer = itr->second;

	return lPlayer;
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

		// 现在vector中查找是否已经存在
		CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(player->GetAccount());
		if(itr != m_mapCdkeyLoginPlayers.end())
		{
			(itr->second).insert(player->GetExID());
		}
		else
		{
			set<CGUID> vecLPlayer;
			vecLPlayer.insert(player->GetExID());
			m_mapCdkeyLoginPlayers[player->GetAccount()] = vecLPlayer;
		}
	}
}
// 删除一个CDBLoginPlayer 同时操作另一个map(CGUID为key)
void CDBEntityManager::DelLoginPlayerByCdkey(CDBLoginPlayer* player)
{
	if(player)
	{
		// 现在vector中查找是否已经存在
		CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(player->GetAccount());
		if(itr != m_mapCdkeyLoginPlayers.end())
		{
			itr->second.erase(player->GetExID());
			// set个数为零,没有角色则清空set
			if(itr->second.size() == 0)
				m_mapCdkeyLoginPlayers.erase(itr);
		}
	}
}
// 清空链表
void CDBEntityManager::ClearCdkeyLoginPlayerMap(void)
{
	m_mapCdkeyLoginPlayers.clear();
}
// 取得个数
set<CGUID>* CDBEntityManager::FindLoginPlayerByCdkey(const char* szCdkey)
{
	set<CGUID>* ret = NULL;
	CdkeyDBLoginPlayerMapItr itr = m_mapCdkeyLoginPlayers.find(szCdkey);
	if(itr != m_mapCdkeyLoginPlayers.end())
		ret = &itr->second;

	return ret;
}
//DBPlayer////////////////////////////////////////////////////////////////////////
CDBPlayer* CDBEntityManager::FindPlayer(const CGUID& guid)
{
	CDBPlayer* ret = NULL;
	DBPlayerMapItr itr = GetDBPlayerMap().find(guid);
	if(itr != GetDBPlayerMap().end())
		ret = itr->second;

	return ret;
}
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
		lMapPlayers++;
	}
}
// 删除一个DBPlayer
void CDBEntityManager::DelOneDBPlayer(const CGUID& playerID)
{
	DBPlayerMapItr itr = m_mapDBPlayers.find(playerID);
	if(itr != m_mapDBPlayers.end())
	{
		SAFE_DELETE(itr->second);
		m_mapDBPlayers.erase(itr);
		lMapPlayers--;
	}
}
// 清空链表
void CDBEntityManager::ClearPlayerMap(void)
{
	DBPlayerMapItr itr = m_mapDBPlayers.begin();
	for(; itr != m_mapDBPlayers.end(); itr++)
	{
		SAFE_DELETE((*itr).second);
	}

	m_mapDBPlayers.clear();
}


//DBFaction////////////////////////////////////////////////////////////////////////
void CDBEntityManager::AddOneDBFaction(CDBFaction* faction)
{
	if(faction)
		m_mapDBFactions[faction->GetExID()] = faction;
}
// 删除一个DBGood
void CDBEntityManager::DelOneDBFaction(CDBFaction* faction)
{
	if(faction)
	{
		DBFactionMapItr itr = m_mapDBFactions.find(faction->GetExID());
		if(itr != m_mapDBFactions.end())
			m_mapDBFactions.erase(itr);

		SAFE_DELETE(faction);
	}
}
// 删除一个DBFaction
void CDBEntityManager::DelOneDBFaction(const CGUID& factionID)
{
	DBFactionMapItr itr = m_mapDBFactions.find(factionID);
	if(itr != m_mapDBFactions.end())
	{
		SAFE_DELETE(itr->second);
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
			m_mapDBCountrys.erase(itr);
		}

		SAFE_DELETE(itr->second);
	}
}
// 删除一个DBCountry
void CDBEntityManager::DelOneDBCountry(const CGUID& CountryID)
{
	DBCountryMapItr itr = m_mapDBCountrys.find(CountryID);
	if(itr != m_mapDBCountrys.end())
	{
		SAFE_DELETE(itr->second);
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
	{
		DBGenVarMapItr itr = m_mapDBGenVars.find(GenVar->GetExID());
		if(itr!=m_mapDBGenVars.end())
		{
			delete itr->second;
			m_mapDBGenVars.erase(itr);
		}
		m_mapDBGenVars[GenVar->GetExID()] = GenVar;
	}
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
		// 先查找是否有该Region
		DBRegionMapItr rItr;
		for (rItr = m_mapDBRegions.begin(); rItr != m_mapDBRegions.end(); rItr++)
		{
			if( rItr->second->GetExID() == Region->GetExID()
				|| rItr->second->GetID() == Region->GetID() )
			{
				SAFE_DELETE(rItr->second);
				m_mapDBRegions.erase(rItr);
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
			m_mapDBRegions.erase(itr);
		}


		SAFE_DELETE(Region);
	}
}
// 删除一个DBRegion
void CDBEntityManager::DelOneDBRegion(const CGUID& RegionID)
{
	DBRegionMapItr itr = m_mapDBRegions.find(RegionID);
	if(itr != m_mapDBRegions.end())
	{
		SAFE_DELETE(itr->second);
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
			SAFE_DELETE(Region);
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
		SAFE_DELETE(itr->second);
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
void CDBEntityManager::ClearDBLinkmanMap(const CGUID& ownerID)
{
	DBLinkmanMapItr itr = m_mapDBLinkman.begin();
	while( itr != m_mapDBLinkman.end() )
	{
		CGUID OwnerGUID;
		itr->second->GetGuidAttr(string("OwnerGUID"), 0, OwnerGUID);
		if(OwnerGUID == ownerID)
		{
			SAFE_DELETE(itr->second);
			itr = m_mapDBLinkman.erase(itr);
		}
		else
			itr++;
	}
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

// 在内存中验证玩家Account是否存在
bool CDBEntityManager::ValidatePlayerIDInCdkey(const char* szCDKEY, const CGUID& guid)
{
	bool ret = false;
	set<CGUID>* setLPlayer = FindLoginPlayerByCdkey(szCDKEY);
	if(setLPlayer && setLPlayer->size()!=0)
	{
		set<CGUID>::iterator itr;
		for(itr = setLPlayer->begin(); itr != setLPlayer->end(); itr++)
		{
			if((*itr) == guid)
			{
				ret = true;
				break;
			}
		}
	}
	else // 未找到该CDKEY
		ret = true;

	return ret;
}

// 多线程读取处理所需信息
LoadObj::LoadObj()
: objGUID(NULL_GUID),objID(0),msgID(0),objNum(0),
objType((DB_OBJ_TYPE)-1),dwIP(0),lQueryTime(0)
{
	memset(szCDKEY, 0, sizeof(szCDKEY));
}
LoadObj::~LoadObj()
{

}
LoadObj& LoadObj::operator=(const LoadObj& right)
{
	if(this != &right)
	{
		objGUID = right.objGUID;
		objID = right.objID;
		msgID = right.msgID;
		objNum = right.objNum;
		objType = right.objType;
		memcpy(szCDKEY, right.szCDKEY, 64);
		opType = right.opType;
		dwIP = right.dwIP;
	}
	return *this;
}



// 多线程写入处理所需信息
SaveObj::SaveObj()
: objGUID(NULL_GUID),objID(0),
msgID(0),objNum(0),objType((DB_OBJ_TYPE)-1),
dwIP(0)
{
}
SaveObj::~SaveObj()
{

}
SaveObj& SaveObj::operator=(const SaveObj& right)
{
	if(this != &right)
	{
		objGUID = right.objGUID;
		objID = right.objID;
		msgID = right.msgID;
		objNum = right.objNum;
		objType = right.objType;
		objAttrEnum.clear();
		if(right.objAttrEnum.size())
		{
			for(int i=0; i<right.objAttrEnum.size(); i++)
				objAttrEnum.push_back(right.objAttrEnum[i]);
		}
		opType = right.opType;
		dwIP = right.dwIP;
		opResultType = right.opResultType;
	}
	return *this;
}

//!	压入一个LoadThreadData
bool CDBEntityManager::PushLoadThreadData(LoadThreadData* loadData)
{
	if(NULL == loadData) return false;

	EnterCriticalSection(&m_LoadCriticalSection);
	if(m_listLoadThreadData.size() == 0)
	{
		// 设置事件
		OutputDebugStr("-------设置读取线程事件-------\r\n");
		CDBEntityManager::SetLoadEvent();
	}
	m_listLoadThreadData.push_back(loadData);
	LeaveCriticalSection(&m_LoadCriticalSection);

	return true;
}
//!	弹出一个LoadThreadData
bool CDBEntityManager::PopLoadThreadData(LoadThreadData** loadData,long dwThreadID)
{
	EnterCriticalSection(&m_LoadCriticalSection);
	while(m_listLoadThreadData.size() == 0)
	{	
		CDBEntityManager::ResetLoadEvent();
		//如果没有操作命令，则等待。
		LeaveCriticalSection(&m_LoadCriticalSection);

		CDBEntityManager::WaitLoadEvent();	
		//等待通知事件
		char outStr[256];
		sprintf(outStr, "ID[%d]:LOAD退出事件等待...\r\n", dwThreadID);
		OutputDebugStr(outStr);

		EnterCriticalSection(&m_LoadCriticalSection);
	}
	*loadData = (*m_listLoadThreadData.begin());
	m_listLoadThreadData.erase(m_listLoadThreadData.begin());
	LeaveCriticalSection(&m_LoadCriticalSection);
	return true;
}
//!	压入一个已被处理的LoadThreadData
bool CDBEntityManager::PushDisposedLoadThreadData(LoadThreadData* loadData)
{
	if(NULL == loadData) return false;

	EnterCriticalSection(&m_DisposedLoadCriticalSection);	
	m_listDisposedLoadThreadData.push_back(loadData);
	LeaveCriticalSection(&m_DisposedLoadCriticalSection);
	return true;
}
bool CDBEntityManager::PushDisposedLoadThreadData(std::list<LoadThreadData*>& loadDataList)
{
	if(loadDataList.size() == 0) return false;

	EnterCriticalSection(&m_DisposedLoadCriticalSection);	
	m_listDisposedLoadThreadData.insert(m_listDisposedLoadThreadData.end(), loadDataList.begin(), loadDataList.end());
	LeaveCriticalSection(&m_DisposedLoadCriticalSection);
	return true;
}
//! 弹出一个已被处理的LoadThreadData
bool CDBEntityManager::PopDisposedLoadThreadData(list<LoadThreadData*>& loadDataList)
{
	EnterCriticalSection(&m_DisposedLoadCriticalSection);
	if(m_listDisposedLoadThreadData.size() == 0)
	{
		LeaveCriticalSection(&m_DisposedLoadCriticalSection);
		return false;
	}
	loadDataList = m_listDisposedLoadThreadData;
	m_listDisposedLoadThreadData.clear();
	LeaveCriticalSection(&m_DisposedLoadCriticalSection);
	return true;
}


//!	压入一个SaveThreadData
bool CDBEntityManager::PushSaveThreadData(SaveThreadData* saveData)
{
	if(NULL == saveData) return false;

	EnterCriticalSection(&m_SaveCriticalSection);
	if(m_listSaveThreadData.size() == 0)
	{
		// 设置事件
		OutputDebugStr("-------设置保存线程事件-------\r\n");
		CDBEntityManager::SetSaveEvent();
	}
	m_listSaveThreadData.push_back(saveData);
	LeaveCriticalSection(&m_SaveCriticalSection);
	return true;
}
//!	弹出一个SaveThreadData
bool CDBEntityManager::PopSaveThreadData(list<SaveThreadData*>& saveDataList)
{
	EnterCriticalSection(&m_SaveCriticalSection);
	while(m_listSaveThreadData.size() == 0)
	{
		CDBEntityManager::ResetSaveEvent();
		//如果没有操作命令，则等待。
		LeaveCriticalSection(&m_SaveCriticalSection);
		//等待通知事件
		CDBEntityManager::WaitSaveEvent();

		//等待通知事件
		char outStr[256];
		sprintf(outStr, "SAVE退出事件等待...\r\n");
		OutputDebugStr(outStr);

		EnterCriticalSection(&m_SaveCriticalSection);
	}
	saveDataList = m_listSaveThreadData;
	m_listSaveThreadData.clear();
	LeaveCriticalSection(&m_SaveCriticalSection);
	return true;
}
//!	压入多个已被处理的SaveThreadData
bool CDBEntityManager::PushDisposedSaveThreadData(SaveThreadData* saveData)
{
	if(saveData == 0) return false;

	EnterCriticalSection(&m_DisposedSaveCriticalSection);	
	m_listDisposedSaveThreadData.push_back(saveData);
	LeaveCriticalSection(&m_DisposedSaveCriticalSection);
	return true;
}
//! 弹出一个已被处理的SaveThreadData
bool CDBEntityManager::PopDisposedSaveThreadData(SaveThreadData** saveData)
{
	if(0 != m_listDisposedSaveThreadData.size())
	{
		EnterCriticalSection(&m_DisposedSaveCriticalSection);
		if(m_listDisposedSaveThreadData.size() == 0)
		{
			LeaveCriticalSection(&m_DisposedSaveCriticalSection);
			return false;
		}
		*saveData = *(m_listDisposedSaveThreadData.begin());
		m_listDisposedSaveThreadData.pop_front();
		LeaveCriticalSection(&m_DisposedSaveCriticalSection);	
	}
	return true;
}
void CDBEntityManager::SetLoadEvent(void)
{
	SetEvent(m_hLoadEvent);
}
void CDBEntityManager::WaitLoadEvent(void)
{
	WaitForSingleObject(m_hLoadEvent, INFINITE);
}

void CDBEntityManager::ResetLoadEvent(void)
{
	ResetEvent(m_hLoadEvent);
}

void CDBEntityManager::SetSaveEvent(void)
{
	SetEvent(m_hSaveEvent);
}
void CDBEntityManager::WaitSaveEvent(void)
{
	WaitForSingleObject(m_hSaveEvent, INFINITE);
}

void CDBEntityManager::ResetSaveEvent(void)
{
	ResetEvent(m_hSaveEvent);
}

// 初始化属性配置
void CDBEntityManager::InitPropertyIniData()
{
	// 初始化player对象
	LoadObjStructInfo("structinfo\\loginplayer.ini");

	// 创建表对象
	_RecordsetPtr basers(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM csl_player_base");
		CMyAdoBase::CreateRs(basers);
		if(!CMyAdoBase::OpenRs(sql, basers, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_PLAYER_BASE"), string("loginplayer"));
		}
		CMyAdoBase::ReleaseRs(basers);
	}

	// 初始化player对象
	LoadObjStructInfo("structinfo\\player.ini");
	
	// 创建表对象
	_RecordsetPtr rs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM baseproperty");
		CMyAdoBase::CreateRs(rs);
		if(!CMyAdoBase::OpenRs(sql, rs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("baseproperty"), string("player"));
		}
		CMyAdoBase::ReleaseRs(rs);
	}

	// 创建表对象
	_RecordsetPtr rs1(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_Player_Sprite_System");
		CMyAdoBase::CreateRs(rs1);
		if(!CMyAdoBase::OpenRs(sql, rs1, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_Player_Sprite_System"), string("player"));
		}
		CMyAdoBase::ReleaseRs(rs1);
	}

	// 创建表对象
	_RecordsetPtr rs2(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM FriendsMember");
		CMyAdoBase::CreateRs(rs2);
		if(!CMyAdoBase::OpenRs(sql, rs2, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("FriendsMember"), string("player"));
		}
		CMyAdoBase::ReleaseRs(rs2);
	}

	// 初始化goods对象
	LoadObjStructInfo("structinfo\\goods.ini");
	// 创建表对象
	_RecordsetPtr goodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM equipment");
		CMyAdoBase::CreateRs(goodsrs);
		if(!CMyAdoBase::OpenRs(sql, goodsrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("equipment"), string("goods"));
		}
		CMyAdoBase::ReleaseRs(goodsrs);
	}

	// 初始化Duprgn对象
	LoadObjStructInfo("structinfo\\duprgn.ini");
	// 创建表对象
	_RecordsetPtr duprgnrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_DUPRGN_PROPERTY");
		CMyAdoBase::CreateRs(duprgnrs);
		if(!CMyAdoBase::OpenRs(sql, duprgnrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_DUPRGN_PROPERTY"), string("duprgn"));
		}
		CMyAdoBase::ReleaseRs(duprgnrs);
	}

	// 初始化RgnGoods对象
	LoadObjStructInfo("structinfo\\phrgnobj.ini");
	// 创建表对象
	_RecordsetPtr rgngoodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_DUPRGN_GOODS");
		CMyAdoBase::CreateRs(rgngoodsrs);
		if(!CMyAdoBase::OpenRs(sql, rgngoodsrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_DUPRGN_GOODS"), string("phrgnobj"));
		}
		CMyAdoBase::ReleaseRs(rgngoodsrs);
	}

	// 初始化Faction对象
	LoadObjStructInfo("structinfo\\faction.ini");
	// 创建表对象
	_RecordsetPtr facBaseDataRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_Faction_BaseProperty");
		CMyAdoBase::CreateRs(facBaseDataRs);
		if(!CMyAdoBase::OpenRs(sql, facBaseDataRs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_Faction_BaseProperty"), string("faction"));
		}
		CMyAdoBase::ReleaseRs(facBaseDataRs);
	}

	// 初始化Faction_Apply对象
	LoadObjStructInfo("structinfo\\faction_apply.ini");
	// 创建表对象
	_RecordsetPtr facApplyRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_Faction_Apply");
		CMyAdoBase::CreateRs(facApplyRs);
		if(!CMyAdoBase::OpenRs(sql, facApplyRs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_Faction_Apply"), string("faction_apply"));
		}
		CMyAdoBase::ReleaseRs(facApplyRs);
	}

	// 初始化Faction_Member对象
	LoadObjStructInfo("structinfo\\faction_member.ini");
	// 创建表对象
	_RecordsetPtr facMembersRs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_Faction_Members");
		CMyAdoBase::CreateRs(facMembersRs);
		if(!CMyAdoBase::OpenRs(sql, facMembersRs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_Faction_Members"), string("faction_member"));
		}
		CMyAdoBase::ReleaseRs(facMembersRs);
	}

	// 初始化mail对象
	LoadObjStructInfo("structinfo\\mail.ini");

	// 创建表对象
	_RecordsetPtr mailrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_MAIL");
		CMyAdoBase::CreateRs(mailrs);
		if(!CMyAdoBase::OpenRs(sql, mailrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_MAIL"), string("mail"));
		}
		CMyAdoBase::ReleaseRs(mailrs);
	}

	// 初始化mailgoods对象
	LoadObjStructInfo("structinfo\\mailgoods.ini");

	// 创建表对象
	_RecordsetPtr mailgoodsrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_MAIL_GOODS");
		CMyAdoBase::CreateRs(mailgoodsrs);
		if(!CMyAdoBase::OpenRs(sql, mailgoodsrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_MAIL_GOODS"), string("mailgoods"));
		}
		CMyAdoBase::ReleaseRs(mailgoodsrs);
	}

	// 初始化Global Var对象
	LoadObjStructInfo("structinfo\\scriptvar.ini");

	// 创建表对象
	_RecordsetPtr varrs(0);
	if(m_pAccessThread)
	{
		char sql[1024];
		sprintf(sql,"SELECT * FROM CSL_SCRIPTVAR");
		CMyAdoBase::CreateRs(varrs);
		if(!CMyAdoBase::OpenRs(sql, varrs, m_pAccessThread->GetConnPtr())) // 创建表
		{
			CreateOtherTableInDb(string("CSL_SCRIPTVAR"), string("scriptvar"));
		}
		CMyAdoBase::ReleaseRs(varrs);
	}
}
void CDBEntityManager::LoadObjStructInfo(const char* szPath)
{
	if(NULL == szPath) return;

	CRFile* prfile = rfOpen(szPath);
	if( prfile )
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		CDataEntityManager* attrManager = NULL;
		string deManagerName = "";
		string strPath = szPath;
		long ret = strPath.find("loginplayer", 0);
		if(ret != string::npos)
		{
			deManagerName = "loginplayer";
			goto NEXT;
		}
		ret = strPath.find("player", 0);
		if(ret != string::npos)
		{
			deManagerName = "player";
			goto NEXT;
		}
		ret = strPath.find("phrgnobj", 0);
		if(ret != string::npos)
		{
			deManagerName = "phrgnobj";
			goto NEXT;
		}
		ret = strPath.find("mailgoods", 0);
		if(ret != string::npos)
		{
			deManagerName = "mailgoods";
			goto NEXT;
		}
		ret = strPath.find("goods", 0);
		if(ret != string::npos)
		{
			deManagerName = "goods";
			goto NEXT;
		}
		ret = strPath.find("duprgn", 0);
		if(ret != string::npos)
		{
			deManagerName = "duprgn";
			goto NEXT;
		}
		ret = strPath.find("faction_apply", 0);
		if(ret != string::npos)
		{
			deManagerName = "faction_apply";
			goto NEXT;
		}
		ret = strPath.find("faction_member", 0);
		if(ret != string::npos)
		{
			deManagerName = "faction_member";
			goto NEXT;
		}
		ret = strPath.find("faction", 0);
		if(ret != string::npos)
		{
			deManagerName = "faction";
			goto NEXT;
		}
		ret = strPath.find("mail", 0);
		if(ret != string::npos)
		{
			deManagerName = "mail";
			goto NEXT;
		}
		ret = strPath.find("scriptvar", 0);
		if(ret != string::npos)
		{
			deManagerName = "scriptvar";
			goto NEXT;
		}

NEXT:
		std::map<string, CDataEntityManager*>::iterator itr = m_mapObjAttrDef.find(deManagerName);
		if(itr == m_mapObjAttrDef.end())
		{
			attrManager = new CDataEntityManager;
			m_mapObjAttrDef[deManagerName] = attrManager;
		}
		else
			attrManager = itr->second;

	
		while(ReadTo(stream, "#"))
		{
			
			string attrName = "";
			long attrType = 0;
			string dbTableName = "";
			long pkNameFlag = -1;
			stream >> attrName >> attrType >> dbTableName >> pkNameFlag;
			CEntityProperty* ep = attrManager->CreateEntityProperty(attrName, dbTableName, (DATA_OBJECT_TYPE)attrType);
			if(ep)
			{
				ep->SetDBTableName(dbTableName);
				ep->AddDataObject((DATA_OBJECT_TYPE)attrType, 0);
				ep->SetDbFlag(pkNameFlag);
			}
		}

		return;
	}
}

void CDBEntityManager::CreateOtherTableInDb(const string& tableName, const string& otherIniName, bool isIndexed)
{
	//! SQL语句的头			= 1 + 表名 + 3
	string strHead_1		= "\r\n\r\nCREATE TABLE [dbo].[";
	string strHead_3		= "] (\r\n";

	//! 8位整数类型字段	= 1 + 字段名 + 3
	string strInt8_1		= "\t[";
	string strInt8_3		= "] [tinyint] NOT NULL ,\r\n";

	//! 16位整数类型字段	= 1 + 字段名 + 3
	string strInt16_1		= "\t[";
	string strInt16_3		= "] [smallint] NOT NULL ,\r\n";

	//! 32位整数类型字段	= 1 + 字段名 + 3
	string strInt32_1		= "\t[";
	string strInt32_3		= "] [int] NOT NULL ,\r\n";
	//! 浮点类型字段		= 1 + 字段名 + 3
	string strFloat_1		= "\t[";
	string strFloat_3		= "] [float] NOT NULL ,\r\n";
	//! 日期类型字段		= 1 + 字段名 + 3
	string strDate_1		= "\t[";
	string strDate_3		= "] [datetime] NULL ,\r\n";
	//!	GUID类型字段		= 1 + 字段名 + 3
	string strGuid_1		= "\t[";
	string strGuid_3		= "] [uniqueidentifier] NOT NULL ,\r\n";
	//!	50位字符串类型字段 = 1 + 字段名 + 3
	string strStr50_1		= "\t[";
	string strStr50_3		= "] [nvarchar] (256) COLLATE Chinese_PRC_CI_AS NOT NULL ,\r\n";

	//! IMAGE类型字段	= 1 + 字段名 + 3
	string image_1		= "\t[";
	string image_3		= "] [image] NULL , \r\n";

	//!	SQL语句结尾
	string strFinality		= ") ON [PRIMARY]";

	//! 创建聚集索引					= 1 + tablename + 3 + tablename + 5 + itemname + 7;
	string strCreateCongregateIndex_1	= "\r\n\r\nCONSTRAINT [PK_";
	string strCreateCongregateIndex_3	= "] PRIMARY KEY CLUSTERED\r\n(\r\n[";
	string strCreateCongregateIndex_5	= "] ASC\r\n";
	string strCreateCongregateIndex_7	= ")WITH (IGNORE_DUP_KEY = OFF) ON [PRIMARY]\r\n";

	//! 数组顺序参考eDataTypeCode顺序
	string arrItemName[] = 
	{
		strInt8_1,		strInt8_3,
		strInt16_1,		strInt16_3,
		strInt32_1,		strInt32_3,
		strFloat_1,		strFloat_3,
		strDate_1,		strDate_3,
		strGuid_1,		strGuid_3,
		strStr50_1,		strStr50_3,
		image_1,		image_3,
	};


	string strCreateTableSql;
	//! 生成语句头
	strCreateTableSql += strHead_1 + tableName + strHead_3;
	CDataEntityManager* pObjAttrDef = NULL;
	std::map<string, CDataEntityManager*>::iterator itr = m_mapObjAttrDef.find(otherIniName);
	if(itr != m_mapObjAttrDef.end())
		pObjAttrDef = itr->second;

	if(!pObjAttrDef) return;

	bool hasUseImageFlag = false;

	// 物品表单独处理一下
	if(otherIniName == "goods" && tableName == "equipment")
	{
		strCreateTableSql += 
			arrItemName[2 * 5] + 
			string("id") +
			arrItemName[2 * 5 + 1];
	}

	//! 生成语句身
	CDataEntityManager::EntityPropertyMapItr attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
	for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
	{
		if(attrItr->second->GetDBTableName() == tableName)
		{
			CWrapDataObject* wdo = attrItr->second->GetDataObject(0);
			if(wdo)
			{
				switch(wdo->GetDataType())
				{
				case DATA_OBJECT_LONG:
				case DATA_OBJECT_DWORD:
					{
						strCreateTableSql += 
							arrItemName[2 * 2] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 2 + 1];
					}
					break;
				case DATA_OBJECT_SHORT:
				case DATA_OBJECT_WORD:
					{
						strCreateTableSql += 
							arrItemName[2 * 1] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 1 + 1];
					}
					break;
				case DATA_OBJECT_CHAR:
				case DATA_OBJECT_BYTE:
					{
						strCreateTableSql += 
							arrItemName[2 * 1] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 1 + 1];
					}
					break;
				case DATA_OBJECT_FLOAT:
					{
						strCreateTableSql += 
							arrItemName[2 * 3] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 3 + 1];
					}
					break;
				case DATA_OBJECT_STRING:
					{
						strCreateTableSql += 
							arrItemName[2 * 6] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 6 + 1];
					}
					break;
				case DATA_OBJECT_TIME:
					{
						strCreateTableSql += 
							arrItemName[2 * 4] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 4 + 1];
					}
					break;
				case DATA_OBJECT_BUFFER:
					{
						hasUseImageFlag = true;
						strCreateTableSql += 
							arrItemName[2 * 7] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 7 + 1];
					}
					break;
				case DATA_OBJECT_GUID:
					{
						strCreateTableSql += 
							arrItemName[2 * 5] + 
							attrItr->second->GetEPName() +
							arrItemName[2 * 5 + 1];
					}
					break;
				}
			}
		}
	}

	//! 创建字段的聚集索引
	if(isIndexed)
	{
		string strCreateCongregateIndex = 
		strCreateCongregateIndex_1 + 
		tableName + 
		strCreateCongregateIndex_3;

		bool bHasIdxAttrFlag = false;
		CDataEntityManager::EntityPropertyMapItr pkItr = pObjAttrDef->GetEntityPropertyMap().begin();
		for(; pkItr != pObjAttrDef->GetEntityPropertyMap().end(); pkItr++)
		{
			if(NULL == pkItr->second) continue;
			if(pkItr->second->GetDBTableName() == tableName)
			{
				if(pkItr->second->GetDbFlag() == 1) // 0:非主键,1:主键
				{
					strCreateCongregateIndex = strCreateCongregateIndex + pkItr->first + strCreateCongregateIndex_5 + ",";
					bHasIdxAttrFlag = true;
				}
			}
		}
		if(bHasIdxAttrFlag)
		{
			// 去掉最后的一个','
			string::iterator chItr = strCreateCongregateIndex.end();
			chItr--;
			strCreateCongregateIndex.erase(chItr);
			strCreateCongregateIndex += strCreateCongregateIndex_7;
			strCreateTableSql += strCreateCongregateIndex;
		}
	}
	

	//! 生成语句尾
	if(hasUseImageFlag)
		strCreateTableSql += strFinality + " TEXTIMAGE_ON [PRIMARY]";
	else
		strCreateTableSql += strFinality;

	ExecSqlString(strCreateTableSql);
}
void CDBEntityManager::ExecSqlString(const string& sqlStr)
{
	//! 执行语句
	//! 在配置的数据库中执行
	try
	{	
		m_pAccessThread->GetConnPtr()->Execute(sqlStr.c_str(), NULL, adCmdText);
		return;
	}
	catch(...)
	{
		try
		{	
			CMyAdoBase::CloseCn(m_pAccessThread->GetConnPtr());
			if (CMyAdoBase::OpenCn(m_pAccessThread->GetConnPtr()))
			{
				m_pAccessThread->GetConnPtr()->Execute(sqlStr.c_str(), NULL, adCmdText);
				return;
			}
		}
		catch(_com_error &e)
		{
			CMyAdoBase::PrintErr("CDBEntityManager - ExecSqlString error", e);
		}

		return;
	}
}

void CDBEntityManager::PreLoadPlayerBase()
{
	GetGame()->GetRsPlayer()->PreLoadPlayerBase(GetAccessThread()->GetConnPtr());
}
// 预读策略
void CDBEntityManager::PreLoadPlayerAttrs()
{
	GetGame()->GetRsPlayer()->PreLoadPlayerAttrs(GetAccessThread()->GetConnPtr());
}