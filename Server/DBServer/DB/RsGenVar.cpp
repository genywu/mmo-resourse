#include "StdAfx.h"
#include "Rsgenvar.h"
#include "..\..\server\dbserver\AppWorld\DBGenVar.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"
#include "..\..\server\DBServer/dbserver/game.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "..\..\server\DBServer\AppWorld\DBMailGoods.h"
#include "..\..\server\dbserver\AppWorld\DBGenVarGroup.h"
#include "../../server/DBServer/AppWorld/ThreadDataDef.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRsGenVar::CRsGenVar(void)
{
}

CRsGenVar::~CRsGenVar(void)
{
}

DB_OPERATION_RESULT_TYPE CRsGenVar::Add(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn)
{
	if(!pGenVarGroup || cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_CNPTRNULL));
		return DB_OPER_RESULT_ERROR;
	}

	_RecordsetPtr rs;
	// 比物品个数多分配2048字节
	char *sql = new char[1024 * (pGenVarGroup->GetGenVarGroupSet().size()+2)];

	string iniName   = "scriptvar";
	string tableName = "CSL_SCRIPTVAR";
	try
	{
		TESTHR(CreateRs(rs));

		// 清空sql语句
		memset(sql, 0, 1024 * (pGenVarGroup->GetGenVarGroupSet().size()+2));

		// 生成语句头
		strcpy(sql, "INSERT INTO CSL_SCRIPTVAR(");

		CDataEntityManager* pObjAttrDef = NULL;
		std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(iniName);
		if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
			pObjAttrDef = itr->second;

		if(!pObjAttrDef) return DB_OPER_RESULT_ERROR;

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
		strcat(sql, ") select ");

		set<CDBGenVar*>::iterator goodsItr = pGenVarGroup->GetGenVarGroupSet().begin();
		for(; goodsItr != pGenVarGroup->GetGenVarGroupSet().end(); goodsItr++)
		{	
			//! 生成语句身
			attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
			for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
			{
				if(attrItr->second)
				{
					if(attrItr->second->GetDBTableName() == tableName)
					{
						// 添加属性字段值
						CEntityProperty* ep = (*goodsItr)->GetDataEntityManager().GetEntityProperty(attrItr->second->GetEPName());
						if(ep)
						{
							if(ep->GetDBTableName() == tableName)
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
										//rs->PutCollect((*epItr).c_str(), szTimeValue);
										strcat(sql, "'");
										strcat(sql, szTimeValue);
										strcat(sql, "',");
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

			sqlLen = strlen(sql);
			sql[sqlLen-1] = '\0';

			strcat(sql, " union all select ");
		}
		sqlLen = strlen(sql);
		sql[sqlLen-17] = '\0';

		TESTHR(ExecuteCn(sql, cn));
		SAFE_DELETE_ARRAY(sql);
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		SAFE_DELETE_ARRAY(sql);
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_SAVERGNGOODSFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}
	SAFE_DELETE_ARRAY(sql);
	return DB_OPER_RESULT_ERROR;
}

DB_OPERATION_RESULT_TYPE CRsGenVar::Load(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn)
{
	bool IsLocalConnection = false;
	if(!pGenVarGroup || NULL == cn)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_CNPTRNULL));
		return DB_OPER_RESULT_ERROR;
	}
	char sql[1024];

	pGenVarGroup->Clear();

	_RecordsetPtr rs;
	try
	{
		// 在获取数据库中的
		TESTHR(CreateRs(rs));
		sprintf(sql, "SELECT * FROM CSL_SCRIPTVAR");
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockReadOnly));

		CDBGenVar *pGenVar = NULL;	
		while( !rs->GetadoEOF() )
		{
			CGUID tGuid;
			CGUID::CreateGUID(tGuid);
			pGenVar = new CDBGenVar(tGuid);
			CDataEntityManager::EntityPropertyMapItr epItr = pGenVar->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pGenVar->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_SCRIPTVAR")
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

			pGenVarGroup->GetGenVarGroupSet().insert(pGenVar);
			rs->MoveNext();
		}

		ReleaseRs(rs);

		return DB_OPER_RESULT_OK;
	}// end try
	catch(_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSPLAYER,STR_DBS_PLAYER_OPENBASEERR), e);
		ReleaseRs(rs);
		return DB_OPER_RESULT_ERROR;
	}
	return DB_OPER_RESULT_ERROR;
}
DB_OPERATION_RESULT_TYPE CRsGenVar::Save(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn)
{
	if(!pGenVarGroup || cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_CNPTRNULL));
		return DB_OPER_RESULT_ERROR;
	}

	_RecordsetPtr rs;
	// 比物品个数多分配2048字节
	char *sql = new char[1024 * (pGenVarGroup->GetGenVarGroupSet().size()+2)];

	string iniName   = "scriptvar";
	string tableName = "CSL_SCRIPTVAR";
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql, "DELETE FROM CSL_SCRIPTVAR");
		// 先删除该个人房屋的成员
		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL));
			return DB_OPER_RESULT_ERROR;
		}

		// 清空sql语句
		memset(sql, 0, 1024 * (pGenVarGroup->GetGenVarGroupSet().size()+2));

		// 生成语句头
		strcpy(sql, "INSERT INTO CSL_SCRIPTVAR(");

		CDataEntityManager* pObjAttrDef = NULL;
		std::map<string, CDataEntityManager*>::iterator itr = GetGame()->GetDBEntityManager()->GetObjAttrDef().find(iniName);
		if(itr != GetGame()->GetDBEntityManager()->GetObjAttrDef().end())
			pObjAttrDef = itr->second;

		if(!pObjAttrDef) return DB_OPER_RESULT_ERROR;

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
		strcat(sql, ") select ");

		set<CDBGenVar*>::iterator goodsItr = pGenVarGroup->GetGenVarGroupSet().begin();
		for(; goodsItr != pGenVarGroup->GetGenVarGroupSet().end(); goodsItr++)
		{	
			//! 生成语句身
			attrItr = pObjAttrDef->GetEntityPropertyMap().begin();
			for (; attrItr != pObjAttrDef->GetEntityPropertyMap().end(); attrItr++)
			{
				if(attrItr->second)
				{
					if(attrItr->second->GetDBTableName() == tableName)
					{
						// 添加属性字段值
						CEntityProperty* ep = (*goodsItr)->GetDataEntityManager().GetEntityProperty(attrItr->second->GetEPName());
						if(ep)
						{
							if(ep->GetDBTableName() == tableName)
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
										//rs->PutCollect((*epItr).c_str(), szTimeValue);
										strcat(sql, "'");
										strcat(sql, szTimeValue);
										strcat(sql, "',");
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

			sqlLen = strlen(sql);
			sql[sqlLen-1] = '\0';

			strcat(sql, " union all select ");
		}
		sqlLen = strlen(sql);
		sql[sqlLen-17] = '\0';

		TESTHR(ExecuteCn(sql, cn));
		SAFE_DELETE_ARRAY(sql);
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		SAFE_DELETE_ARRAY(sql);
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_SAVERGNGOODSFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}
	SAFE_DELETE_ARRAY(sql);
	return DB_OPER_RESULT_ERROR;
}
DB_OPERATION_RESULT_TYPE CRsGenVar::Del(CDBGenVarGroup* pGenVarGroup, _ConnectionPtr& cn)
{
	char sql[500] = "";
	try
	{
		set<CDBGenVar*>::iterator itr = pGenVarGroup->GetGenVarGroupSet().begin();
		for(; itr != pGenVarGroup->GetGenVarGroupSet().end(); itr++)
		{
			sprintf(sql, "DELETE FROM CSL_SCRIPTVAR WHERE varName='%s'", (*itr)->GetStringAttr(string("varName"), 0));
			TESTHR(ExecuteCn(sql, cn));
		}
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	return DB_OPER_RESULT_ERROR;
}
