#include "stdafx.h"
#include ".\rsplayer.h"
#include "..\..\server\dbserver\appworld\dbplayer.h"
#include "..\..\server\DBServer\AppWorld\DBLinkMan.h"
#include "..\..\server\DBServer\AppWorld\DBLinkManGroup.h"
#include "..\..\nets\netdb\message.h"
#include "..\..\server\dbserver\dbserver\game.h"
#include "..\..\server\dbserver\appworld\dbgood.h"
#include "..\..\server\DBServer\AppWorld\DBGoodsGroup.h"
#include "RSGoods.h"
#include"../../public/Date.h"
#include "RSCardsLargess.h"
#include "..\..\server\dbserver\appworld\dbentitymanager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../../server/DBServer/AppWorld/DBLoginPlayer.h"
#include "../../server/DBServer/AppWorld/DBLoadThread.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"
#include"../../public/LinkmanDef.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"

#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  

int nReadPlayerDataNum = 0;

CRsPlayer::CRsPlayer(void)
{
}

CRsPlayer::~CRsPlayer(void)
{
}
DWORD dwTestCountLoadNum = 0;
bool CRsPlayer::OpenPlayerBaseInDB(LPCSTR szCdkey, std::list<LoadThreadData*>& loadDataList, long &cnt, _ConnectionPtr& cn)
{
	bool IsLocalConnection = false;
	if(NULL == cn)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}
	char sql[512];
	time_t tDelDate = 0;

	

	CGUID lPlayerID;
	char nRemainDays = -1;
	string strName("");

	// 再装载数据库中的角色
	_RecordsetPtr rs;
	try
	{
		// 在获取数据库中的
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT * FROM CSL_PLAYER_BASE WHERE Account='%s'", szCdkey);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly));

		CDBLoginPlayer *pPlayer = NULL;
		LoadThreadData* loadData = NULL;
	
		rs->MoveFirst();
		char szGuidStr[128];
		while( !rs->GetadoEOF() )
		{
			CGUID tGuid((_bstr_t)rs->GetCollect("guid"));
			lPlayerID = tGuid;
			tGuid.tostring(szGuidStr);

			char pszLogingInfo[512]="";
			_snprintf(pszLogingInfo,512,"#%d-RsPlayer::Load LoginPlayer,tGuid[%s].",dwTestCountLoadNum, szGuidStr);
			PutStringToFile("Login_DBS_Info",pszLogingInfo);
						
			loadData = new LoadThreadData(lPlayerID, DB_LOGINPLAYER);
			pPlayer = (CDBLoginPlayer*)loadData->dbObj;

			//再从数据库中读删除时间
			if(!tDelDate) tDelDate = GetPlayerDeletionDateByRs( lPlayerID , rs );

			if(tDelDate)
			{
				time_t tCurTime = time(0);
				tm *pCurTime = localtime(&tCurTime);
				double dDiffSecond = difftime(tCurTime, tDelDate);
				short days = -short(dDiffSecond/86400.0);
				nRemainDays = days+1;
				if(nRemainDays < 0) nRemainDays = 0; 
			}
			else
			{
				nRemainDays = -1;
			}
		
			CDataEntityManager::EntityPropertyMapItr epItr = pPlayer->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pPlayer->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_PLAYER_BASE")
				{
					switch(epItr->second->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							_variant_t var = rs->GetCollect(epItr->first.c_str());
							long timeBuf[6] = {0};
							if( var.vt != VT_NULL ) 
							{
								SYSTEMTIME st;
								VariantTimeToSystemTime(var.date, &st);

								timeBuf[0] = st.wYear;
								timeBuf[1] = st.wMonth;
								timeBuf[2] = st.wDay;
								timeBuf[3] = st.wHour;
								timeBuf[4] = st.wMinute;
								timeBuf[5] = st.wSecond;
								epItr->second->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
							}
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							string strValue = (_bstr_t)rs->GetCollect(epItr->first.c_str());
							epItr->second->SetStringAttr(0, strValue.c_str());
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							//LoadBufferField(pPlayer, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID(_bstr_t(rs->GetCollect(epItr->first.c_str())));
							epItr->second->SetGuidAttr(0, tGUID);
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_DWORD:
						{
							long value = rs->GetCollect(epItr->first.c_str());
							epItr->second->SetLongAttr(0, value);
						}
						break;
					}
				}
			}

			loadData->loadObj->msgID = -1;
			loadData->loadObj->objType = DB_LOGINPLAYER;
			loadData->loadObj->objGUID = lPlayerID;
			loadData->dbObj->SetExID(lPlayerID);
			strcpy(loadData->loadObj->szCDKEY, ((CDBLoginPlayer*)loadData->dbObj)->GetAccount());
			loadDataList.insert(loadDataList.end(), loadData);

			loadData->dbObj->GetExID().tostring(szGuidStr);
			_snprintf(pszLogingInfo,512,"#%d-RsPlayer::Load LoginPlayer,dbObj->GetExID[%s].",dwTestCountLoadNum,szGuidStr);
			PutStringToFile("Login_DBS_Info",pszLogingInfo);

			rs->MoveNext();
			++cnt;
			++dwTestCountLoadNum;
		}

		ReleaseRs(rs);

		return true;
	}// end try
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_OPENBASEERR), e);
		ReleaseRs(rs);
		return false;
	}
}
// 设置登录时间,在WS发送MSG_W2DB_PASS_LOG_PLAYERBASELIST消息时处理
void CRsPlayer::UpdateLogoutTime(const CGUID& nID, WORD* LogoutTime, _ConnectionPtr& cn)
{
	if( nID == NULL_GUID ) 
		return;

	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return;
	}

	char sql[128];
	_RecordsetPtr rs;
	try
	{
		char szID[128];
		nID.tostring(szID);

		tm tt;
		memset(&tt,0, sizeof(tm));
		tt.tm_year= LogoutTime[0];
		tt.tm_mon = LogoutTime[1]; 
		tt.tm_mday = LogoutTime[2];
		tt.tm_hour = LogoutTime[3];
		tt.tm_min = LogoutTime[4]; 
		tt.tm_sec = LogoutTime[5];
		time_t res = mktime(&tt);

		sprintf(sql, "UPDATE CSL_PLAYER_BASE SET LogoutTime=%d WHERE guid='%s'", (DWORD)res, szID);
		TESTHR(ExecuteCn(sql, cn));
		return;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_UPDATELOGOUTTIMEERR),e);
		ReleaseRs(rs);
		return;
	}
}
//打开玩家基本属性
bool CRsPlayer::OpenPlayerBase(LPCSTR szCdkey, std::list<LoadThreadData*>& loadDataList, _ConnectionPtr& cn)
{
	BYTE nDBRoleCnt = 0,
		nListRoleCnt = 0,
		nRoleCnt =0;
	long cnt = 0;

	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	nDBRoleCnt = GetPlayerCountInDBbyCdkey(szCdkey, cn); // from db
	if(nDBRoleCnt == 0xff) return false;
	nListRoleCnt = 0;

	nRoleCnt = nDBRoleCnt + nListRoleCnt;//total 

	if(!nRoleCnt)
	{
		return true;
	}

	if( !OpenPlayerBaseInDB(szCdkey, loadDataList, cnt, cn) )
	{
		return false;
	}

	return true;
}

bool CRsPlayer::DeletePlayer(const CGUID& nID, time_t tDelDate, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn == NULL) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	char sql[128];

	// 删除时间+7天
	tDelDate += 7*24*3600;
	tm *now;
	now = localtime(&tDelDate);
	
	char szID[128];
	nID.tostring(szID);

	sprintf(sql, "UPDATE CSL_PLAYER_BASE SET DelDate = '%d-%d-%d %d:%d:%d' WHERE guid='%s'", 
		now->tm_year+1900, now->tm_mon+1, now->tm_mday,
		now->tm_hour, now->tm_min, now->tm_sec,
		szID);
	try
	{
		TESTHR(ExecuteCn(sql, cn));
		return true;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_DELPROPERTYERR), e);
		return false;
	}
}

bool CRsPlayer::IsNameExist(LPCSTR szName)
{
	if( !szName ) return false;

	// 如果名字包含了单引号，返回失败。
	if( HasSingleQuotes( szName ) ) return false;

	char sql[512];
	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql, "SELECT guid FROM CSL_PLAYER_BASE WHERE name='%s'", szName);

		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));

		// 记录存在
		if(!rs->GetadoEOF()) 
		{ 
			ReleaseRs(rs);
			return true;	
		}

		ReleaseRs(rs);
		return false;	
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CHECKNAMEERR), e);
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_NAMEIS),szName);
		while(*szName)
		{
			AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_X),(unsigned char)*szName);
			++szName;
		}
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_N));
		return true;
	}
}

void CRsPlayer::GetPlayerNameByID(const CGUID& dwPlayerID, char *szPlayerName)
{
	char sql[128];
	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		char szPID[128];
		dwPlayerID.tostring(szPID);
		sprintf(sql, "SELECT Name FROM CSL_PLAYER_BASE WHERE guid='%s'", szPID);
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));

		// 记录存在
		if(!rs->GetadoEOF()) 
		{ 
			lstrcpy( szPlayerName,  (const char *)(_bstr_t(rs->GetCollect("Name"))) );
		}

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_GETNAMEBYIDERR), e);
		ReleaseRs(rs);
	}
}

CGUID CRsPlayer::GetPlayerExID(const char* strName)
{
	char sql[128];
	_RecordsetPtr rs;
	
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql, "SELECT guid FROM CSL_PLAYER_BASE WHERE name='%s'", strName);
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));

		// 没记录
		if(rs->GetadoEOF()) 
		{
			ReleaseRs(rs);
			return NULL_GUID;
		}
		CGUID nPlayerID((_bstr_t)rs->GetCollect("guid"));
		ReleaseRs(rs);
		return nPlayerID;
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_GETIDERR), e);
		return NULL_GUID;
	}
}

string CRsPlayer::GetCDKey(const char* strName)
{
	char sql[256];
	string cdkey("");
	
	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql,"SELECT Account FROM CSL_PLAYER_BASE WHERE name='%s'",strName);
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));

		// 记录存在
		if(!rs->GetadoEOF()) cdkey = _bstr_t(rs->GetCollect("Account"));

		ReleaseRs(rs);
		return cdkey;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_GETCDKEYERR), e);

		ReleaseRs(rs);
		return cdkey;
	}	
}
bool CRsPlayer::CreatePlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	CDBEntityManager::DBLoginPlayerMapItr lpItr = GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().find(pPlayer->GetExID());
	if(lpItr != GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().end())
	{
		if( !CreatePlayerBase(lpItr->second, cn) )
		{
			return false;
		}
	}

	if( !CreatePlayerAbilities(pPlayer, cn) )
	{
		return false;
	}
	return true;
}

bool CRsPlayer::CreatePlayerBase(CDBLoginPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer)
	{
		return false;
	}

	// 存盘线程传过来的连接不存在
	if( cn==NULL ) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}
	CGUID nID;
	nID = pPlayer->GetExID();
	char sznID[128];
	nID.tostring(sznID);
	long nTransactionCounter = 0;
	string iniName   = "loginplayer";
	string tableName = "CSL_PLAYER_BASE";
	//_RecordsetPtr rs;
	try
	{
		char *sql = new char[1024 * 16];

		// 生成语句头
		strcpy(sql, "INSERT INTO CSL_PLAYER_BASE(");

		CDataEntityManager* pObjAttrDef = NULL;
		std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(iniName);
		if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
			pObjAttrDef = itr->second;

		if(!pObjAttrDef) return false;

		//! 生成语句身
		CDataEntityManager::EntityPropertyMapItr attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
		for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
		{
			if(attrItr->second->GetDBTableName() == tableName)
			{
				CWrapDataObject* wdo = attrItr->second->GetDataObject(0);
				if(wdo)
				{
					// 添加属性字段名
					strcat(sql, attrItr->second->GetEPName().c_str());
					strcat(sql, ",");
				}
			}
		}

		long sqlLen = strlen(sql);
		sql[sqlLen-1] = '\0';
		strcat(sql, ") VALUES(");

		//! 生成语句身
		attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
		for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
		{
			if(attrItr->second)
			{
				if(attrItr->second->GetDBTableName() == tableName)
				{
					// 添加属性字段值
					CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(attrItr->second->GetEPName());
					if(ep)
					{
						if(ep->GetDBTableName() == tableName)
						{
							switch(ep->GetDataType())
							{
							case DATA_OBJECT_TIME:
								{
									strcat(sql, "NULL,");
									continue;
								}
								break;
							case  DATA_OBJECT_STRING:
								{
									//rs->PutCollect((*epItr).c_str(), );
									strcat(sql, "'");
									const char* pStr = ep->GetStringAttr(0);
									if(pStr)
										strcat(sql, pStr);
									else
										strcat(sql, "");
									strcat(sql, "',");
								}
								break;
							case DATA_OBJECT_BUFFER:
								{
									//SaveBufferField((*goodsItr), (*epItr).c_str(), rs);
								}
								break;
							case DATA_OBJECT_GUID:
								{
									CGUID tGUID;
									ep->GetGuidAttr(0, tGUID);
									char szGuid[128];
									tGUID.tostring(szGuid);
									//rs->PutCollect((*epItr).c_str(), szGuid);
									strcat(sql, "'");
									strcat(sql, szGuid);
									strcat(sql, "',");
								}
								break;
							case DATA_OBJECT_BOOL:
							case DATA_OBJECT_CHAR:
							case DATA_OBJECT_BYTE:
								{
									//rs->PutCollect((*epItr).c_str(), (BYTE)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((BYTE)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							case DATA_OBJECT_SHORT:
							case DATA_OBJECT_WORD:
							case DATA_OBJECT_USHORT:
								{
									//rs->PutCollect((*epItr).c_str(), (WORD)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((WORD)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							case DATA_OBJECT_FLOAT:
							case DATA_OBJECT_LONG:
							case DATA_OBJECT_ULONG:
							case DATA_OBJECT_DWORD:
								{
									//rs->PutCollect((*epItr).c_str(), (DWORD)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((DWORD)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							}
						}
					}
				}
			}
		}

		long szSqlLen = strlen(sql);
		sql[szSqlLen-1] = ')';

		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CREATEPROPERTYERR));
			return false;
		}

		SAFE_DELETE_ARRAY(sql);
		return true;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CREATEPROPERTYERR), e);	
		//ReleaseRs(rs);

		return false;
	}
}


bool CRsPlayer::CreatePlayerAbilities(CDBPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer)
	{
		return false;
	}

	// 存盘线程传过来的连接不存在
	if( cn == NULL ) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	_RecordsetPtr rs;
	long nTransactionCounter = 0;
	char szPlayerID[128];
	pPlayer->GetExID().tostring(szPlayerID);
	try
	{
		TESTHR(CreateRs(rs));

		// 插入csl_player_ability表
		TESTHR(OpenRs("baseproperty", rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));

		rs->AddNew();

		char szGuid[40] = {0};
		pPlayer->GetExID().tostring(szGuid);
		CDataEntityManager::EntityPropertyMapItr epItr = pPlayer->GetDataEntityManager().GetEntityPropertyMap().begin();
		for(; epItr != pPlayer->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
		{
			CEntityProperty* ep = epItr->second;
			if(ep)
			{
				if(ep->GetDBTableName() == "baseproperty")
				{
					switch(ep->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							tm *now;
							time_t tDate;
							time(&tDate);
							now = localtime(&tDate);
							char szTimeValue[1024];
							sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", now->tm_year+1900, now->tm_mon+1, now->tm_mday,0,0,0);
							rs->PutCollect(epItr->first.c_str(), szTimeValue);
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							const char* strPtr = ep->GetStringAttr(0);
							if(strPtr)
								rs->PutCollect(epItr->first.c_str(), strPtr);
							else
								rs->PutCollect(epItr->first.c_str(), "");
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							SaveBufferField(pPlayer, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID;
							ep->GetGuidAttr(0, tGUID);
							char szGuid[128];
							tGUID.tostring(szGuid);
							rs->PutCollect(epItr->first.c_str(), szGuid);
						}
						break;
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
						{
							rs->PutCollect(epItr->first.c_str(), (BYTE)ep->GetLongAttr(0));
						}
						break;
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_USHORT:
						{
							rs->PutCollect(epItr->first.c_str(), (WORD)ep->GetLongAttr(0));
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_DWORD:
						{
							rs->PutCollect(epItr->first.c_str(), (DWORD)ep->GetLongAttr(0));
						}
						break;
					}
				}
			}
		}

		rs->Update();
		ReleaseRs(rs);
		return true;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CREATEPROPERTYERR), e);	
		ReleaseRs(rs);
		
		return false;
	}

}

//读玩家属性
bool CRsPlayer::LoadPlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer) return false;
	char sql[128];
	CGUID nID = pPlayer->GetExID();
	_RecordsetPtr rs;

	if(NULL == cn)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	char szPlayerID[128];
	nID.tostring(szPlayerID);

	try
	{
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT * FROM baseproperty WHERE guid='%s' ORDER BY guid", szPlayerID);

		if(!OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly))
		{
			PrintErr(sql);
			TESTHR(FALSE);
		}
		if(rs->GetadoEOF()) // 无该记录
		{
			AddLogText("Load Player[%s] RS is NULL!", szPlayerID);
			ReleaseRs(rs);
			return false;
		}
		else // 记录存在
		{
			rs->MoveFirst();
			CDataEntityManager::EntityPropertyMapItr epItr = pPlayer->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pPlayer->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "baseproperty")
				{
					switch(epItr->second->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							_variant_t var = rs->GetCollect(epItr->first.c_str());
							long timeBuf[6] = {0};
							if( var.vt != VT_NULL ) 
							{
								SYSTEMTIME st;
								VariantTimeToSystemTime(var.date, &st);

								timeBuf[0] = st.wYear;
								timeBuf[1] = st.wMonth;
								timeBuf[2] = st.wDay;
								timeBuf[3] = st.wHour;
								timeBuf[4] = st.wMinute;
								timeBuf[5] = st.wSecond;
								epItr->second->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
							}
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							string strValue = (_bstr_t)rs->GetCollect(epItr->first.c_str());
							epItr->second->SetStringAttr(0, strValue.c_str());
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							LoadBufferField(pPlayer, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID(_bstr_t(rs->GetCollect(epItr->first.c_str())));
							epItr->second->SetGuidAttr(0, tGUID);
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_DWORD:
						{
							long value = rs->GetCollect(epItr->first.c_str());
							epItr->second->SetLongAttr(0, value);
						}
						break;
					}
				}
			}
			ReleaseRs(rs);
			
			// 载入物品记录
			// 读取数据
			if(!GetGame()->GetRSGoods()->LoadGoods(pPlayer, pPlayer->GetGoodsGroupPtr(), cn))
			{
				AddLogText("Load Player[%s] Goods Err!", szPlayerID);
				return false;
			}


			// 载入联系人记录
			// 读取数据
			if(!GetGame()->GetRsPlayer()->LoadLinkmanData(pPlayer, pPlayer->GetLinkmanGroupPtr(), cn))
			{
				AddLogText("Load Player[%s] LinkMan Err!", szPlayerID);
				return false;
			}
		
			LoadSetupOnServer(pPlayer, cn);
			LoadSpriteSystem(pPlayer, cn);
			return true;
		}	
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADPROPERTYERR), e);	
		ReleaseRs(rs);
		return false;
	}
}

//统计玩家排行榜
bool CRsPlayer::StatRanks(CPlayerRanks* pPlayerRanks,_ConnectionPtr& cn)
{
	if(pPlayerRanks == NULL)	return false;
	return true;
}

//存玩家属性
bool CRsPlayer::SavePlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer)
	{
		return false;
	}

	// 存盘线程传过来的连接不存在
	if( cn == NULL ) 
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	CDBEntityManager::DBLoginPlayerMapItr lpItr = GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().find(pPlayer->GetExID());
	if(lpItr != GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().end())
	{
		if( !SavePlayerBase(lpItr->second, cn) )
		{
			return false;
		}
	}

	if( !SavePlayerAbilities(pPlayer, cn) )
	{
		return false;
	}

	return true;
}
// 保存玩家基本属性
bool CRsPlayer::SavePlayerBase(CDBLoginPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer)
	{
		return false;
	}

	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	long nTransactionCounter = 0;

	char szPlayerID[128];
	pPlayer->GetExID().tostring(szPlayerID);

	string iniName   = "loginplayer";
	string tableName = "CSL_PLAYER_BASE";

	// 设置玩家下线时间
	SYSTEMTIME lastTime;
	GetLocalTime(&lastTime);
	tm tt;
	memset(&tt,0, sizeof(tm));
	tt.tm_year= lastTime.wYear-1900; tt.tm_mon = lastTime.wMonth-1; tt.tm_mday = lastTime.wDay;
	tt.tm_hour = lastTime.wHour; tt.tm_min = lastTime.wMinute; tt.tm_sec = lastTime.wSecond;
	time_t res = mktime(&tt);
	if (res == (time_t)-1) res = 0;
	pPlayer->SetLongAttr(string("LogoutTime"), tableName, 0, res);
	
	try
	{
		char *sql = new char[1024 * 16];

		// 生成语句头
		strcpy(sql, "UPDATE CSL_PLAYER_BASE SET ");

		CDataEntityManager* pObjAttrDef = NULL;
		std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(iniName);
		if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
		pObjAttrDef = itr->second;

		if(!pObjAttrDef) return false;

		//! 生成语句身
		CDataEntityManager::EntityPropertyMapItr attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
		for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
		{
			if(attrItr->second)
			{
				if(attrItr->second->GetDBTableName() == tableName)
				{
					// 添加属性字段值
					CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(attrItr->second->GetEPName());
					if(ep)
					{
						if(ep->GetDBTableName() == tableName)
						{
							// 添加属性字段名
							strcat(sql, attrItr->second->GetEPName().c_str());
							strcat(sql, " = ");

							// 添加属性值
							switch(ep->GetDataType())
							{
							case DATA_OBJECT_TIME:
								{
									char szTimeValue[2048];
									DWORD timeValue[6] = {0};
									ep->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
									if(timeValue[0] != 0 && timeValue[1] != 0 && timeValue[2] != 0
										&& timeValue[0] != 0 && timeValue[1] != 0 && timeValue[2] != 0)
									{
										sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
											timeValue[3], timeValue[4], timeValue[5]);
										strcat(sql, "'");
										strcat(sql, szTimeValue);
										strcat(sql, "',");
									}
									else
									{
										strcat(sql, "NULL,");
									}
								}
								break;
							case  DATA_OBJECT_STRING:
								{
									//rs->PutCollect((*epItr).c_str(), );
									strcat(sql, "'");
									const char* pStr = ep->GetStringAttr(0);
									if(pStr)
										strcat(sql, pStr);
									else
										strcat(sql, "");
									strcat(sql, "',");
								}
								break;
							case DATA_OBJECT_BUFFER:
								{
									//SaveBufferField((*goodsItr), (*epItr).c_str(), rs);
								}
								break;
							case DATA_OBJECT_GUID:
								{
									CGUID tGUID;
									ep->GetGuidAttr(0, tGUID);
									char szGuid[128];
									tGUID.tostring(szGuid);
									//rs->PutCollect((*epItr).c_str(), szGuid);
									strcat(sql, "'");
									strcat(sql, szGuid);
									strcat(sql, "',");
								}
								break;
							case DATA_OBJECT_BOOL:
							case DATA_OBJECT_CHAR:
							case DATA_OBJECT_BYTE:
								{
									//rs->PutCollect((*epItr).c_str(), (BYTE)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((BYTE)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							case DATA_OBJECT_SHORT:
							case DATA_OBJECT_WORD:
							case DATA_OBJECT_USHORT:
								{
									//rs->PutCollect((*epItr).c_str(), (WORD)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((WORD)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							case DATA_OBJECT_FLOAT:
							case DATA_OBJECT_LONG:
							case DATA_OBJECT_ULONG:
							case DATA_OBJECT_DWORD:
								{
									//rs->PutCollect((*epItr).c_str(), (DWORD)ep->GetLongAttr(0));
									char value[32];
									memset(value, 0, sizeof(value));
									itoa((DWORD)ep->GetLongAttr(0), value, 10);
									strcat(sql, value);
									strcat(sql, ",");
								}
								break;
							}
						}
					}
				}
			}
		}

		long szSqlLen = strlen(sql);
		sql[szSqlLen-1] = '\0';
	
		// 添加sql条件部分
		strcat(sql, " WHERE guid='");
		strcat(sql, szPlayerID);
		strcat(sql, "'");

		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CREATEPROPERTYERR));
			return false;
		}

		SAFE_DELETE_ARRAY(sql);
		return true;
	}
	catch(_com_error &e)
	{	
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_SAVEBASEERR), e);
		return false;
	}
}

// 保存玩家普通能力属性(除技能,状态,好友名字,脚本数据等)
bool CRsPlayer::SavePlayerAbilities(CDBPlayer *pPlayer, _ConnectionPtr& cn)
{
	if(!pPlayer)
	{
		return false;
	}

	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	// 更新set里没有baseproperty的属性
	if(!pPlayer->FindTableUpdateFlag(string("baseproperty"))) return true;

	char sql[512];
	_RecordsetPtr rs;
	long nTransactionCounter = 0;

	char szPlayerID[128];
	pPlayer->GetExID().tostring(szPlayerID);

	try
	{
		TESTHR(CreateRs(rs));

		// 更新csl_player_ability表
		sprintf(sql, "select * from baseproperty where guid = '%s'", szPlayerID);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		if(rs->GetadoEOF()) // 无该记录
		{
			ReleaseRs(rs);
			return false;
		}
		////////////////////..............................
		//*  已经修改的行列************************起 
		set<string>::iterator epItr = pPlayer->GetDirtyAttrDataMap().begin();
		for(; epItr != pPlayer->GetDirtyAttrDataMap().end(); epItr++)
		{
			CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(*epItr);
			if(ep)
			{
				if(ep->GetDBTableName() == "baseproperty")
				{
					switch(ep->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							char szTimeValue[2048];
							DWORD timeValue[6] = {0};
							ep->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
							sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
																		timeValue[3], timeValue[4], timeValue[5]);
							rs->PutCollect((*epItr).c_str(), szTimeValue);
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							rs->PutCollect((*epItr).c_str(), ep->GetStringAttr(0));
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							SaveBufferField(pPlayer, (*epItr).c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tEpGuid;
							ep->GetGuidAttr(0, tEpGuid);
							char szEpGuid[128];
							tEpGuid.tostring(szEpGuid);
							if(ep->GetDbFlag() == 0) // 0:非主键,1:主键
								rs->PutCollect((*epItr).c_str(), szEpGuid);
						}
						break;
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
						{
							rs->PutCollect((*epItr).c_str(), (BYTE)ep->GetLongAttr(0));
						}
						break;
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_USHORT:
						{
							rs->PutCollect((*epItr).c_str(), (WORD)ep->GetLongAttr(0));
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_DWORD:
						{
							rs->PutCollect((*epItr).c_str(), (DWORD)ep->GetLongAttr(0));
						}
						break;
					}
				}
			}
		}

		rs->Update();
		ReleaseRs(rs);

		SaveSpriteSystem(pPlayer, cn);
		SaveSetupOnServer(pPlayer, cn);
		return true;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_SAVEALLERR), e);
		ReleaseRs(rs);
		return false;
	}
}

//存Buffer
bool CRsPlayer::SaveBufferField(CDBPlayer *pPlayer, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pPlayer == NULL)	return false;
	if(szFiledName == NULL) return false;

	VARIANT varBLOB;
	SAFEARRAY* psa;

	CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(string(szFiledName));
	if(NULL == ep) return false;

	long dataSize = ep->GetBufSize(0);
	BYTE* buffer = NULL;
	if(dataSize)
	{
		buffer = new BYTE[dataSize];
		ep->GetBufAttr(0, (void*)buffer, dataSize);

		long lSize=(long)dataSize;
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound=0;
		rgsabound[0].cElements=lSize;
		psa=SafeArrayCreate(VT_UI1,1,rgsabound);

		for(long i=0;i<lSize;i++)
		{
			SafeArrayPutElement(psa,&i,&buffer[i]);
		}
		varBLOB.vt=VT_ARRAY|VT_UI1;
		varBLOB.parray=psa;

		try
		{
			rs->GetFields()->GetItem(szFiledName)->AppendChunk(varBLOB);
		}
		catch(_com_error e)
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_THREADSAVESTATEOK), e);
			SAFE_DELETE_ARRAY(buffer);
			return false;
		}

		SafeArrayDestroy(psa);
	}
	SAFE_DELETE_ARRAY(buffer);
	return true;
}
//读Buffer
bool CRsPlayer::LoadBufferField(CDBPlayer *pPlayer, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pPlayer == NULL)	return false;

	try
	{
		if(rs->GetadoEOF())
		{
			ReleaseRs(rs);
			return false;
		}

		long lSize=0;
		FieldsPtr fldPtr = rs->GetFields();
		FieldPtr itemPtr = fldPtr->GetItem(szFiledName);
		lSize = itemPtr->ActualSize;

		if(lSize > 0)
		{
			_variant_t varBLOB;
			varBLOB = itemPtr->GetChunk(lSize);

			BYTE *pBuf=NULL;
			SafeArrayAccessData(varBLOB.parray,(void**)&pBuf);
			SafeArrayUnaccessData(varBLOB.parray);

			// 清空SKILL
			CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(string(szFiledName));
			if(ep)
				ep->SetBufAttr(0, pBuf, lSize);
		}
	}
	catch(_com_error e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADSTATEFAIL), e);
		return false;
	}
	return true;
}


// 返回数据库中，改帐户下，角色数目。
BYTE CRsPlayer::GetLocationStateInCdkey(LPCSTR szCdkey)
{
	
	if(!szCdkey) return 0;
	BYTE res = 0; 
		
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));

		char sql[128];
		sprintf(sql, "SELECT ID,Account FROM CSL_PLAYER_BASE WHERE Account='%s'", szCdkey);
		TESTHR(OpenRs(sql, rs, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr(), adOpenStatic));

		if(!rs->GetadoEOF())
		{
			res = rs->GetRecordCount();
			CloseRs(rs);	
		}
		ReleaseRs(rs);
		return res;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADSCRIPTFAIL), e);
		ReleaseRs(rs);	
		return res;
	}
	return 0;
}

BYTE CRsPlayer::GetPlayerCountInCdkey(LPCSTR szCdkey)
{
	BYTE res = 0;

	// 数据库连接失败，异常，这里返回-1(0xff)
	BYTE dbCont = GetPlayerCountInDBbyCdkey( szCdkey, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr() );
	if( dbCont == 0xff )
	{
		return dbCont;
	}

	res += dbCont;
	return res;
}

BYTE CRsPlayer::GetPlayerCountInDBbyCdkey(LPCSTR szCdkey, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	bool ret = false;

	BYTE res = 0;
	char sql[128];
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT guid FROM CSL_PLAYER_BASE WHERE Account='%s'", szCdkey);
		TESTHR(OpenRs(sql, rs, cn));

		if(!rs->GetadoEOF())
		{
			res = rs->GetRecordCount();
			CloseRs(rs);
		}
		ReleaseRs(rs);
		return res;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADNULLPLAYER), e);
		res=0xff;
		ReleaseRs(rs);
		return res;

	}
}
time_t CRsPlayer::GetPlayerDeletionDate(const CGUID& nID, _ConnectionPtr& cn)
{
	if( nID == NULL_GUID ) 
		return 0;

	bool IsLocalConnection = false;
	time_t res = 0;
	_RecordsetPtr rs;
	char sql[1024];
	char szGuid[128];
	nID.tostring(szGuid);
	try
	{
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT DelDate FROM CSL_PLAYER_BASE WHERE guid='%s'", szGuid);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly));

		if(rs->GetadoEOF())
		{
			ReleaseRs(rs);
			return 0;
		}

		_variant_t var = rs->GetCollect("DelDate");
		if( var.vt == VT_NULL ) 
		{
			ReleaseRs(rs);
			return 0;
		}

		SYSTEMTIME st;
		VariantTimeToSystemTime(var.date, &st);

		tm tt;
		memset(&tt,0, sizeof(tm));
		tt.tm_year= st.wYear-1900; tt.tm_mon = st.wMonth-1; tt.tm_mday = st.wDay;
		res = mktime(&tt);

		if (res == (time_t)-1) res = 0;
		return res;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADPLAYERNUMFAIL),e);
		ReleaseRs(rs);
		return -1;
	}
}

time_t CRsPlayer::GetPlayerDeletionDateByRs(const CGUID& nID, _RecordsetPtr& rs)
{
	time_t res = 0;
	try
	{
		if(rs->GetadoEOF())
		{
			return -1;
		}

		_variant_t var = rs->GetCollect("DelDate");
		if( var.vt == VT_NULL ) 
		{
			return -1;
		}

		SYSTEMTIME st;
		VariantTimeToSystemTime(var.date, &st);

		tm tt;
		memset(&tt,0, sizeof(tm));
		tt.tm_year= st.wYear-1900; tt.tm_mon = st.wMonth-1; tt.tm_mday = st.wDay;
		res = mktime(&tt);

		if (res == (time_t)-1) res = 0;
		return res;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADPLAYERNUMFAIL),e);
		return -1;
	}
}

// 获取标记了登录的角色的登录时间
time_t CRsPlayer::GetPlayerLastLogoutDate(const CGUID& nID, _ConnectionPtr& cn)
{
	if( nID == NULL_GUID ) 
		return 0;

	bool IsLocalConnection = false;

	time_t res = 0;
	char sql[128];

	_RecordsetPtr rs;
	try
	{;
		TESTHR(CreateRs(rs));

		char szID[128];
		nID.tostring(szID);
		sprintf(sql,"SELECT FROM CSL_PLAYER_BASE WHERE guid='%s'", szID);
		TESTHR(OpenRs(sql, rs, cn));
		if(rs->GetadoEOF())
		{
			ReleaseRs(rs);
			return 0;
		}

		res = (time_t)rs->GetCollect("LogoutTime");

		if (res == (time_t)-1) res = 0;

		ReleaseRs(rs);
		return res;
	}
	catch(_com_error &e) 
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_GETDELTIMEFAIL),e);
		ReleaseRs(rs);
		return -1;
	}
}
bool CRsPlayer::RestorePlayer(const CGUID& nPlayerID, _ConnectionPtr& cn)
{
	bool IsLocalConnection = false;
	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return false;
	}

	char sql[128];

	char szID[128];
	nPlayerID.tostring(szID);
	sprintf(sql, "UPDATE CSL_PLAYER_BASE SET DelDate = NULL WHERE guid='%s'", szID);
	long nTransactionCounter = 0;

	try
	{
		TESTHR(ExecuteCn(sql, cn));
		return true;
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_RESTORECHARSTARTTRANSIMITFAIL), e);
		return false;
	}
}

bool CRsPlayer::ValidatePlayerIDInCdkey(LPCSTR szCdkey, const CGUID& nID, _ConnectionPtr& cn)
{
	if(!szCdkey) return false;
	if(NULL_GUID == nID) return false;

	char sql[256];
	bool IsLocalConnection = false;

	_RecordsetPtr rs;

	CGUID nPlayerID;
	bool res = false;
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql, "SELECT guid FROM CSL_PLAYER_BASE WHERE account='%s'", szCdkey);
		TESTHR(OpenRs(sql, rs, cn));

		while( !rs->GetadoEOF() )
		{
			nPlayerID = ((CGUID)((bstr_t)rs->GetCollect("guid")));
			if( nID == nPlayerID )
			{
				res = true;
				break;
			}
			rs->MoveNext();
		}
		ReleaseRs(rs);
		return res;
	}// end try
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_RESTORECHARERR), e);
		ReleaseRs(rs);
		return false;
	}
}

//读取联系人数据
bool CRsPlayer::LoadLinkmanData(CDBPlayer* pPlayer, CDBLinkmanGroup* linkmanGroup, _ConnectionPtr cn)
{
	if(pPlayer == NULL)	return false;
	if(linkmanGroup == NULL) return true;

	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		char szGuid[64] = {0};
		pPlayer->GetExID().tostring(szGuid);

		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM FriendsMember WHERE OwnerGUID='%s' OR AimGUID='%s' ORDER BY OwnerGUID, AimGUID;", szGuid, szGuid);
		TESTHR(OpenRs(szSql, rs, cn));

		while(!rs->GetadoEOF())
		{
			CGUID lmCGUID;
			CGUID::CreateGUID(lmCGUID);
			CDBLinkMan* linkMan = new CDBLinkMan(lmCGUID);
			linkmanGroup->GetLinkmanGroupSet().insert(linkMan);

			_variant_t vOwnerGUID	= rs->GetCollect("OwnerGUID");
			_variant_t vAimGUID		= rs->GetCollect("AimGUID");

			CGUID OwnerGUID		= CGUID(((char*)(_bstr_t)vOwnerGUID));
			CGUID AimGUID		= CGUID(((char*)(_bstr_t)vAimGUID));
			long uGroupID		= rs->GetCollect("GroupID");
			long uDelState		= rs->GetCollect("DelState");
			char szAimName[1024] = {0};
			strcpy(szAimName, (_bstr_t) rs->GetCollect("AimName"));
	
			linkMan->GetDataEntityManager().SetGuidAttr(string("OwnerGUID"), string("FriendsMember"), 0, OwnerGUID);
			linkMan->GetDataEntityManager().SetGuidAttr(string("AimGUID"), string("FriendsMember"), 0, AimGUID);

			linkMan->GetDataEntityManager().SetLongAttr(string("GroupID"), string("FriendsMember"), 0, uGroupID);
			linkMan->GetDataEntityManager().SetLongAttr(string("DelState"), string("FriendsMember"), 0, uDelState);
			linkMan->GetDataEntityManager().SetStringAttr(string("AimName"), string("FriendsMember"), 0, szAimName);

			// 下条记录
			rs->MoveNext();
		}

		ReleaseRs(rs);
		return true;
	}	
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADLINKMANFAIL), e);
		ReleaseRs(rs);
		return false;
	}
	return false;
}
//保存联系人信息
bool CRsPlayer::SaveLinkmanData(CDBLinkmanGroup* linkmanGroup,_ConnectionPtr cn)
{
	if(linkmanGroup == NULL)	return false;

	bool bRe = true;

	try
	{
		char szOwnerGuid[64]	= {0};
		linkmanGroup->GetOwnerGUID().tostring(szOwnerGuid);

		//! 删除该玩家所有数据
		char szDelSql[512] = {0};
		sprintf(szDelSql, "DELETE FriendsMember WHERE OwnerGUID='%s'", szOwnerGuid);
		if(!ExecuteCn(szDelSql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER, STR_DBS_PLAYER_SAVELINKMANERR));
			return false;
		}

		_RecordsetPtr rs;

		TESTHR(CreateRs(rs));

		// 更新equipment表
		char sql[1024];
		sprintf(sql, "select * from FriendsMember where OwnerGUID = '%s'", szOwnerGuid);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		if(!rs->GetadoEOF())
		{
			AddLogText("Save LinkMan Rs=NULL! OwnerID[%s]!", szOwnerGuid);
			ReleaseRs(rs);
			return false;
		}

		BOOL bUpdate = FALSE;
		//! 重新写入数据
		set<CDBLinkMan*>::iterator itr = linkmanGroup->GetLinkmanGroupSet().begin();
		for(; itr != linkmanGroup->GetLinkmanGroupSet().end(); itr++)
		{
			CGUID OwnerGUID;
			(*itr)->GetGuidAttr(string("OwnerGUID"), 0, OwnerGUID);
			if(linkmanGroup->GetOwnerGUID() != OwnerGUID)
				continue;
			CGUID AimGUID;
			(*itr)->GetGuidAttr(string("AimGUID"), 0, AimGUID);
			if(AimGUID == OwnerGUID)
			{
				assert(false);
				return FALSE;
			}

			if(linkmanGroup->GetOwnerGUID() == OwnerGUID)
			{
				// 添加新行
				rs->AddNew();
				bUpdate = TRUE;
				CDataEntityManager::EntityPropertyMapItr epItr = (*itr)->GetDataEntityManager().GetEntityPropertyMap().begin();
				for(; epItr != (*itr)->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
				{
					CEntityProperty* ep = epItr->second;
					if(ep)
					{
						if(ep->GetDBTableName() == "FriendsMember")
						{
							switch(ep->GetDataType())
							{
							case DATA_OBJECT_TIME:
								{
									char szTimeValue[2048];
									DWORD timeValue[6] = {0};
									ep->GetBufAttr(0, (void*)&timeValue[0], sizeof(DWORD)*6);
									sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
										timeValue[3], timeValue[4], timeValue[5]);
									rs->PutCollect(ep->GetEPName().c_str(), szTimeValue);
								}
								break;
							case  DATA_OBJECT_STRING:
								{
									rs->PutCollect(ep->GetEPName().c_str(), ep->GetStringAttr(0));
								}
								break;
							case DATA_OBJECT_BUFFER:
								{
									//SaveBufferField((*goodsItr), ep->GetEPName().c_str(), rs);
								}
								break;
							case DATA_OBJECT_GUID:
								{
									CGUID tGUID;
									ep->GetGuidAttr(0, tGUID);
									char szGuid[128];
									tGUID.tostring(szGuid);
									if(ep->GetDbFlag() == 0) // 0:非主键,1:主键
										rs->PutCollect(ep->GetEPName().c_str(), szGuid);
								}
								break;
							case DATA_OBJECT_BOOL:
							case DATA_OBJECT_CHAR:
							case DATA_OBJECT_BYTE:
								{
									rs->PutCollect(ep->GetEPName().c_str(), (BYTE)ep->GetLongAttr(0));
								}
								break;
							case DATA_OBJECT_SHORT:
							case DATA_OBJECT_WORD:
							case DATA_OBJECT_USHORT:
								{
									rs->PutCollect(ep->GetEPName().c_str(), (WORD)ep->GetLongAttr(0));
								}
								break;
							case DATA_OBJECT_FLOAT:
							case DATA_OBJECT_LONG:
							case DATA_OBJECT_ULONG:
							case DATA_OBJECT_DWORD:
								{
									rs->PutCollect(ep->GetEPName().c_str(), (DWORD)ep->GetLongAttr(0));
								}
								break;
							}
						}
					}
				}
			}
		}

		if(bUpdate)
			rs->Update();
		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_SAVELINKMANERR), e);
		return false;
	}

	return bRe;
}


// 预读玩家数据,在WS初次连接DBS时
void CRsPlayer::PreLoadPlayerBase(_ConnectionPtr& cn)
{
	if(NULL == cn)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return;
	}
	char sql[512];
	time_t tDelDate = 0;

	CDBLoginPlayer *pPlayer = NULL;

	CGUID lPlayerID;
	char nRemainDays = -1;
	string strName("");

	// 再装载数据库中的角色
	_RecordsetPtr rs;
	try
	{
		// 在获取数据库中的
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT * FROM CSL_PLAYER_BASE");
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly));

		if(rs->GetadoEOF()) // 无该记录
		{
			AddLogText("PreLoad CSL_PLAYER_BASE RS is NULL!");
			ReleaseRs(rs);
			return;
		}

		SYSTEMTIME lastTime;
		GetLocalTime(&lastTime);
		tm tt;
		memset(&tt,0, sizeof(tm));
		tt.tm_year= lastTime.wYear-1900; tt.tm_mon = lastTime.wMonth-1; tt.tm_mday = lastTime.wDay;
		tt.tm_hour = lastTime.wHour; tt.tm_min = lastTime.wMinute; tt.tm_sec = lastTime.wSecond;
		time_t curTime = mktime(&tt);

		rs->MoveFirst();
		char szGuidStr[128];
		DWORD dwPreLoadNum = 0;
		while( !rs->GetadoEOF() && dwPreLoadNum < GetGame()->GetSetup()->dwPreLoadNum)
		{
			CGUID tGuid((_bstr_t)rs->GetCollect("guid"));
			lPlayerID = tGuid;
			tGuid.tostring(szGuidStr);
		
			DWORD dwLogoutTime = rs->GetCollect("LogoutTime");

			if(dwLogoutTime <= 0 || abs(curTime - dwLogoutTime) > (GetGame()->GetSetup()->dwPreLoadTime*24*3600) )
			{
				rs->MoveNext();
				continue;
			}

			//再从数据库中读删除时间
			if(!tDelDate) tDelDate = GetPlayerDeletionDateByRs( lPlayerID , rs );

			if(tDelDate)
			{
				time_t tCurTime = time(0);
				tm *pCurTime = localtime(&tCurTime);
				double dDiffSecond = difftime(tCurTime, tDelDate);
				short days = -short(dDiffSecond/86400.0);
				nRemainDays = days+1;
				if(nRemainDays < 0)
				{
					rs->MoveNext();
					continue;
				}
			}
			else
			{
				nRemainDays = -1;
			}

			pPlayer = new CDBLoginPlayer(lPlayerID);

			CDataEntityManager::EntityPropertyMapItr epItr = pPlayer->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pPlayer->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_PLAYER_BASE")
				{
					switch(epItr->second->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							_variant_t var = rs->GetCollect(epItr->first.c_str());
							long timeBuf[6] = {0};
							if( var.vt != VT_NULL ) 
							{
								SYSTEMTIME st;
								VariantTimeToSystemTime(var.date, &st);

								timeBuf[0] = st.wYear;
								timeBuf[1] = st.wMonth;
								timeBuf[2] = st.wDay;
								timeBuf[3] = st.wHour;
								timeBuf[4] = st.wMinute;
								timeBuf[5] = st.wSecond;
								epItr->second->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
							}
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							string strValue = (_bstr_t)rs->GetCollect(epItr->first.c_str());
							epItr->second->SetStringAttr(0, strValue.c_str());
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							//LoadBufferField(pPlayer, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID(_bstr_t(rs->GetCollect(epItr->first.c_str())));
							epItr->second->SetGuidAttr(0, tGUID);
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_DWORD:
						{
							long value = rs->GetCollect(epItr->first.c_str());
							epItr->second->SetLongAttr(0, value);
						}
						break;
					}
				}
			}

			GetGame()->GetDBEntityManager()->AddOneDBLoginPlayer(pPlayer);
			pPlayer->SetAccount(pPlayer->GetStringAttr(string("Account"), 0), 64);
			GetGame()->GetDBEntityManager()->AddLoginPlayerByCdkey(pPlayer);
			rs->MoveNext();
			dwPreLoadNum++;
		}

		ReleaseRs(rs);
	}// end try
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_OPENBASEERR), e);
		ReleaseRs(rs);
	}
}
// 预读策略
void CRsPlayer::PreLoadPlayerAttrs(_ConnectionPtr& cn)
{
	if(cn == NULL) return;

	char sql[1024];

	_RecordsetPtr rs;
	if(NULL == cn)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return;
	}

	char szPlayerID[128];

	TESTHR(CreateRs(rs));
	sprintf(sql, "SELECT * FROM baseproperty");

	if(!OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly))
	{
		PrintErr(sql);
		TESTHR(FALSE);
		return;
	}
	if(rs->GetadoEOF()) // 无该记录
	{
		AddLogText("PreLoad baseproperty Player RS is NULL!");
		ReleaseRs(rs);
		return;
	}

	CDBPlayer* pPlayer = NULL;
	rs->MoveFirst();
	CDBEntityManager::DBLoginPlayerMapItr loginPlayerItr = GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().begin();
	for(; loginPlayerItr != GetGame()->GetDBEntityManager()->GetDBLoginPlayerMap().end(); loginPlayerItr++)
	{
		if(rs->GetadoEOF()) break;

		loginPlayerItr->first.tostring(szPlayerID);

		pPlayer = new CDBPlayer(NULL_GUID);
		try
		{
			CDataEntityManager::EntityPropertyMapItr epItr = pPlayer->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pPlayer->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "baseproperty")
				{
					switch(epItr->second->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							_variant_t var = rs->GetCollect(epItr->first.c_str());
							long timeBuf[6] = {0};
							if( var.vt != VT_NULL ) 
							{
								SYSTEMTIME st;
								VariantTimeToSystemTime(var.date, &st);

								timeBuf[0] = st.wYear;
								timeBuf[1] = st.wMonth;
								timeBuf[2] = st.wDay;
								timeBuf[3] = st.wHour;
								timeBuf[4] = st.wMinute;
								timeBuf[5] = st.wSecond;
								epItr->second->SetBufAttr(0, (void*)&timeBuf[0], sizeof(DWORD)*6);
							}
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							string strValue = (_bstr_t)rs->GetCollect(epItr->first.c_str());
							epItr->second->SetStringAttr(0, strValue.c_str());
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							LoadBufferField(pPlayer, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID(_bstr_t(rs->GetCollect(epItr->first.c_str())));
							epItr->second->SetGuidAttr(0, tGUID);
						}
						break;
					case DATA_OBJECT_FLOAT:
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
					case DATA_OBJECT_WORD:
					case DATA_OBJECT_DWORD:
						{
							long value = rs->GetCollect(epItr->first.c_str());
							epItr->second->SetLongAttr(0, value);
						}
						break;
					}
				}
			}

			// 重设GUID
			CGUID pGuid;
			pPlayer->GetGuidAttr(string("guid"), 0, pGuid);
			pPlayer->SetExID(pGuid);

			// 载入物品记录
			// 读取数据
			pPlayer->GetGoodsGroupPtr()->SetOwnerGUID(pPlayer->GetExID());
			if(!GetGame()->GetRSGoods()->LoadGoods(pPlayer, pPlayer->GetGoodsGroupPtr(), cn))
			{
				AddLogText("PreLoad Player[%s] Goods Err!", szPlayerID);
				return;
			}

			// 载入联系人记录
			// 读取数据
			pPlayer->GetLinkmanGroupPtr()->SetOwnerGUID(pPlayer->GetExID());
			if(!GetGame()->GetRsPlayer()->LoadLinkmanData(pPlayer, pPlayer->GetLinkmanGroupPtr(), cn))
			{
				AddLogText("PreLoad Player[%s] LinkMan Err!", szPlayerID);
				return;
			}

			LoadSetupOnServer(pPlayer, cn);
			LoadSpriteSystem(pPlayer, cn);

			GetGame()->GetDBEntityManager()->AddOneDBPlayer(pPlayer);

			rs->MoveNext();
		}
		catch(_com_error &e)
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_LOADPROPERTYERR), e);	
			ReleaseRs(rs);
			return;
		}
	}
}


//读取小精灵数据
bool CRsPlayer::LoadSpriteSystem(CDBPlayer* pPlayer, _ConnectionPtr &cn)
{
	if( NULL == pPlayer || NULL == cn)	return false;
	bool bRe = TRUE;

	_RecordsetPtr rs;

	char szPlayerGuid[64] = {0};
	pPlayer->GetExID().tostring(szPlayerGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Player_Sprite_System WHERE PlayerGuid='%s'", szPlayerGuid);

		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(szSql, rs, cn));

		if(!rs->GetadoEOF())
		{
			//! 读取自动补给数据
			{
				//! 得到数据的长度
				long lDataSize = rs->GetFields()->GetItem("AutoSupplyData")->ActualSize;
				if(Max_DB_Item_Size < lDataSize)
				{
					MessageBox(NULL, "DBS中取得的自动补给数据长度超过了程序定义的最大值，详细情况查看错误日志。", "Error", MB_OK);
					AddErrorLogText("DBS中取得的自动补给数据长度[%d]超过了程序定义的最大值[%d]，读取失败！", lDataSize, Max_DB_Item_Size);
					return false;
				}
				if(lDataSize > 0)
				{
					_variant_t varBLOB;
					varBLOB = rs->GetFields()->GetItem("AutoSupplyData")->GetChunk(lDataSize);
					if(varBLOB.vt == (VT_ARRAY | VT_UI1)) //! 判断数据类型
					{
						//! 取得数据指针
						void *pBuf = NULL;
						SafeArrayAccessData(varBLOB.parray, &pBuf);

						//! 获取数据
						pPlayer->SetBufAttr(string("AutoSupplyData"), string("CSL_Player_Sprite_System"), 0, pBuf, lDataSize);

						//! 释放
						pBuf = NULL;
						SafeArrayUnaccessData (varBLOB.parray);
					}
				}
			}
		}

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("LoadSpriteSystem Err!", e);	
		ReleaseRs(rs);

		bRe = false;
	}

	return bRe;
}

//保存小精灵数据
bool CRsPlayer::SaveSpriteSystem(CDBPlayer* pPlayer, _ConnectionPtr &cn)
{
	if( NULL == pPlayer || NULL == cn)	return false;
	bool bRe = TRUE;

	_RecordsetPtr rs;

	char szPlayerGuid[64] = {0};
	pPlayer->GetExID().tostring(szPlayerGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Player_Sprite_System WHERE PlayerGuid='%s'", szPlayerGuid);

		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(szSql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		//! 没有玩家数据，则创建并对ID赋值
		if(rs->GetadoEOF())
		{
			rs->AddNew();
			rs->PutCollect("PlayerGuid", szPlayerGuid);
		}

		//! 更新自动补给数据
		{
			CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(string("AutoSupplyData"));
			if(ep)
			{
				long bufSize = ep->GetBufSize(0);
				if(bufSize)
				{
					BYTE* buf = new BYTE[bufSize];
					ep->GetBufAttr(0, buf, bufSize);

					if(Max_DB_Item_Size < bufSize)
					{
						MessageBox(NULL, "需要保存的自动补给数据长度超过了定义的最大值，详细情况查看错误日志。", "Error", MB_OK);
						AddErrorLogText("需要保存的自动补给数据长度[%d]超过了定义的最大值[%d]，保存失败！", bufSize, Max_DB_Item_Size);
						SAFE_DELETE_ARRAY(buf);
						return false;
					}

					BYTE			*pBuf = &buf[0];
					VARIANT			varBLOB;
					SAFEARRAY		*psa;
					SAFEARRAYBOUND	rgsabound[1];

					rgsabound[0].lLbound = 0;
					rgsabound[0].cElements = bufSize;
					psa = SafeArrayCreate(VT_UI1, 1, rgsabound);

					//! 将pBuf指向的二进制数据保存到SAFEARRAY对象psa中
					for (long i = 0; i < (long)rgsabound[0].cElements; i++)
					{
						SafeArrayPutElement (psa, &i, pBuf++); 
					}
					varBLOB.vt = VT_ARRAY | VT_UI1;
					varBLOB.parray = psa;
					rs->GetFields()->GetItem("AutoSupplyData")->AppendChunk(varBLOB);
					SAFE_DELETE_ARRAY(buf);
					SafeArrayDestroy(psa);
				}
				
			}
			rs->Update();
		}

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("SaveSpriteSystem Err!", e);	
		ReleaseRs(rs);

		bRe = false;
	}

	return bRe;
}


//读取玩家需要保存在服务器的设置
bool CRsPlayer::LoadSetupOnServer(CDBPlayer* pPlayer, _ConnectionPtr &cn)
{
	if( NULL == pPlayer || NULL == cn)	return false;
	bool bRe = TRUE;

	_RecordsetPtr rs;

	char szPlayerGuid[64] = {0};
	pPlayer->GetExID().tostring(szPlayerGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Player_Sprite_System WHERE PlayerGuid='%s'", szPlayerGuid);

		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(szSql, rs, cn));

		if(!rs->GetadoEOF())
		{
			{
				//! 得到数据的长度
				long lDataSize = rs->GetFields()->GetItem("PlayerSetup")->ActualSize;
				if(Max_DB_Item_Size < lDataSize)
				{
					MessageBox(NULL, "DBS中取得的玩家系统设置数据长度超过了程序定义的最大值，详细情况查看错误日志。", "Error", MB_OK);
					AddErrorLogText("DBS中取得的玩家系统设置数据长度[%d]超过了程序定义的最大值[%d]，读取失败！", lDataSize, Max_DB_Item_Size);
					return false;
				}
				if(lDataSize > 0)
				{
					_variant_t varBLOB;
					varBLOB = rs->GetFields()->GetItem("PlayerSetup")->GetChunk(lDataSize);
					if(varBLOB.vt == (VT_ARRAY | VT_UI1)) //! 判断数据类型
					{
						//! 取得数据指针
						void *pBuf = NULL;
						SafeArrayAccessData(varBLOB.parray, &pBuf);

						//! 获取数据
						LONG lPos = 0;
						pPlayer->SetBufAttr(string("AutoSupplyData"), string("CSL_Player_Sprite_System"), 0, pBuf, lDataSize);

						//! 释放
						pBuf = NULL;
						SafeArrayUnaccessData (varBLOB.parray);
					}
				}
			}
		}

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("LoadSetupOnServer Err!", e);	
		ReleaseRs(rs);

		bRe = false;
	}

	return bRe;
}

//保存玩家需要保存在服务器的设置
bool CRsPlayer::SaveSetupOnServer(CDBPlayer* pPlayer, _ConnectionPtr &cn)
{
	if( NULL == pPlayer || NULL == cn)	return false;
	bool bRe = TRUE;

	_RecordsetPtr rs;

	char szPlayerGuid[64] = {0};
	pPlayer->GetExID().tostring(szPlayerGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Player_Sprite_System WHERE PlayerGuid='%s'", szPlayerGuid);

		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(szSql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		//! 没有玩家数据，则创建并对ID赋值
		if(rs->GetadoEOF())
		{
			rs->AddNew();
			rs->PutCollect("PlayerGuid", szPlayerGuid);
			AddErrorLogText("[在新建角色时，此条信息输出不代表错误]没有找到角色[%s]的玩家系统设置数据，数据被重新建立（内存中）。", szPlayerGuid);
		}

		//! 更新自动补给数据
		CEntityProperty* ep = pPlayer->GetDataEntityManager().GetEntityProperty(string("PlayerSetup"));
		if(ep)
		{
			long bufSize = ep->GetBufSize(0);
			if(bufSize)
			{
				BYTE* buf = new BYTE[bufSize];
				ep->GetBufAttr(0, buf, bufSize);

				if(Max_DB_Item_Size < bufSize)
				{
					MessageBox(NULL, "需要保存的玩家系统设置数据长度超过了定义的最大值，详细情况查看错误日志。", "Error", MB_OK);
					AddErrorLogText("需要保存的玩家系统设置数据长度[%d]超过了定义的最大值[%d]，保存失败！", bufSize, Max_DB_Item_Size);
					SAFE_DELETE_ARRAY(buf);
					return false;
				}

				BYTE			*pBuf = &buf[0];
				VARIANT			varBLOB;
				SAFEARRAY		*psa;
				SAFEARRAYBOUND	rgsabound[1];

				rgsabound[0].lLbound = 0;
				rgsabound[0].cElements = bufSize;
				psa = SafeArrayCreate(VT_UI1, 1, rgsabound);

				//! 将pBuf指向的二进制数据保存到SAFEARRAY对象psa中
				for (long i = 0; i < (long)rgsabound[0].cElements; i++)
				{
					SafeArrayPutElement (psa, &i, pBuf++); 
				}
				varBLOB.vt = VT_ARRAY | VT_UI1;
				varBLOB.parray = psa;
				rs->GetFields()->GetItem("PlayerSetup")->AppendChunk(varBLOB);
				SAFE_DELETE_ARRAY(buf);
				SafeArrayDestroy(psa);
			}
		}
		rs->Update();
		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("SaveSetupOnServer Err!", e);	
		ReleaseRs(rs);

		bRe = false;
	}

	return bRe;
}

// 根据玩家名字查找玩家对象
bool CRsPlayer::CheckPlayerIsExist(const char* szName, _ConnectionPtr& cn)
{
	if(!szName) return false;

	_RecordsetPtr rs;

	char szPlayerGuid[64] = {0};
	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_PLAYER_BASE WHERE Name='%s'", szName);

		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(szSql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		//! 有玩家数据
		if(!rs->GetadoEOF())
		{
			ReleaseRs(rs);
			return true;
		}
	}
	catch (_com_error& e)
	{
		ReleaseRs(rs);
		PrintErr("CheckPlayerIsExist Err!", e);	
	}

	return false;
}