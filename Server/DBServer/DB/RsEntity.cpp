//RsEntity.cpp/////////////////////////////////////////////////////////////////////
//对象类:数据库操作接口类
//Author:安海川
//Create Time:2008.11.03
#include "stdafx.h"
#include "RsEntity.h"

#include "../App/Entity/EntityGroup.h"
#include "../App/Entity/Entity.h"
#include "../App/Entity/EntityManager.h"
#include "../App/Thread/DBThread.h"
#include "../App/Thread/ThreadDataDef.h"

#include "../Server/Game.h"

#include "../../../Public/Common/RankDef.h"

#pragma warning(push)
#pragma warning(disable : 4244 4267)
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

CRsEntity::CRsEntity(void)
{
}

CRsEntity::~CRsEntity(void)
{
}
HRESULT CRsEntity::GenerateSqlString(char* szRetSqlStr, long lStrSize, DB_OPERATION_TYPE opType, CEntityGroup* pBaseEntity)
{
	if(!pBaseEntity || !szRetSqlStr)
	{
		return E_FAIL;
	}

	string strTbl = "";
	tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pBaseEntity->GetCompositeFlag());
	if(pInfo)
		strTbl = pInfo->strTblName;

	switch(opType)
	{
	case DB_OPER_LOAD:
	case DB_OPER_SAVE:
	case DB_OPER_INSERT:
		// 生成SQL语句
		sprintf(szRetSqlStr, "select * from %s ", 
			strTbl.c_str());
		break;
	case DB_OPER_DELETE_INSERT:
	case DB_OPER_DELETE:
		// 生成SQL语句
		sprintf(szRetSqlStr, "delete from %s ", 
			strTbl.c_str());
		break;

		// 生成SQL语句
		sprintf(szRetSqlStr, "insert INTO %s ", 
			strTbl.c_str());
		break;
	case DB_OPER_ONLY_MEMSAVE:
		return E_FAIL;
	}

	if(pBaseEntity->GetQueryDataVec().size())
	{
		strcat(szRetSqlStr, "where ");
		vector<CTempEntityProperty*>::iterator itr = pBaseEntity->GetQueryDataVec().begin();
		while (itr != pBaseEntity->GetQueryDataVec().end())
		{
			DATA_OBJECT_TYPE enumDataType = (*itr)->GetDataType();
			string strName = (*itr)->GetVarName();
			switch(enumDataType)
			{
			case DATA_OBJECT_GUID:
				{
					CGUID tGuid;
					(*itr)->GetGuidAttr( tGuid);
					char szGuid[128];
					tGuid.tostring(szGuid);
					strcat(szRetSqlStr, strName.c_str());
					strcat(szRetSqlStr, "= '");
					strcat(szRetSqlStr, szGuid);
					strcat(szRetSqlStr, "'");
				}
				break;
			case DATA_OBJECT_STRING:
				{
					strcat(szRetSqlStr, strName.c_str());
					strcat(szRetSqlStr, "= '");
					strcat(szRetSqlStr, (*itr)->GetStringAttr());
					strcat(szRetSqlStr, "'");
				}
				break;
			case DATA_OBJECT_TIME:
				{
					//
				}
				break;
			default:
				{
					char tString[1024];
					long lRet = (*itr)->GetLongAttr();
					if(lRet == -1) // 此位置无值,跳过
						break;
					strcat(szRetSqlStr, strName.c_str());
					strcat(szRetSqlStr, "=");
					itoa(lRet, tString, 10);
					strcat(szRetSqlStr, tString);
				}
				break;
			}

			switch((*itr)->GetDbRelateType())
			{
			case DB_RELATE_AND:
				strcat(szRetSqlStr, " AND ");
				break;
			case DB_RELATE_OR:
				strcat(szRetSqlStr, " OR ");
				break;
			case DB_RELATE_LBLANKAND: // 左括号+AND : AND (
				strcat(szRetSqlStr, " AND ( ");
				break;
			case DB_RELATE_RBLANKAND: // 右括号+AND : ) AND
				strcat(szRetSqlStr, " ) AND ");
				break;
			case DB_RELATE_LBLANKOR: // 左括号+OR : OR (
				strcat(szRetSqlStr, " OR ( ");
				break;
			case DB_RELATE_RBLANKOR: // 右括号+OR : ) OR
				strcat(szRetSqlStr, " ) OR ");
				break;
			case DB_RELATE_LBLANK: // 左括号 : ( 
				strcat(szRetSqlStr, " ( ");
				break;
			case DB_RELATE_RBLANK: // 右括号 :)
				strcat(szRetSqlStr, " ) ");
				break;
			}
			itr++;
		}
	}

	return S_OK;
}
// ---Delete Interfaces.. [11/18/2008 ahc]---
HRESULT CRsEntity::DeleteEntityGroupAttrs(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
	if(!pBaseEntity)
	{
		return E_FAIL;
	}

	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"DeleteEntityGroupAttrs() cn is NULL!");
		return E_FAIL;
	}

	char sql[5120] = {0};

	pBaseEntity->SetCurDbOperType(DB_OPER_DELETE);

	GenerateSqlString(sql, 5120, DB_OPER_DELETE, pBaseEntity);
#ifdef _DEBUG_SM_INFO_
	char szOutPutStr[5120];
	sprintf(szOutPutStr, "In DeleteEntityAttrs() sql[%s]", sql);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
	try
	{
		ExecuteCn(sql, cn);
		return S_OK;
	}
	catch(_com_error &e)
	{
		PrintErr("Delete EntityGroup Attr Err!", e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("Delete EntityGroup Attr ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return E_FAIL;
}

HRESULT CRsEntity::DeleteEntitys(const char* szSql, const char* szTblName, const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"DeleteEntitys() cn is NULL!");
		return E_FAIL;
	}

	map<CGUID, CBaseEntity*>::iterator itr = entityMap.begin();
	for(; itr != entityMap.end(); itr++)
		itr->second->SetCurDbOperType(DB_OPER_DELETE);
	try
	{
		ExecuteCn(szSql, cn);
		return S_OK;
	}
	catch(_com_error &e)
	{
		PrintErr("DeleteEntitys() Err!", e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("DeleteEntitys() ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return E_FAIL;
}

// ---Load Interfaces.. [10/18/2008 ahc]---
HRESULT CRsEntity::LoadEntityAttrs(CBaseEntity *pBaseEntity, _ConnectionPtr& cn)
{
	if(!pBaseEntity)
	{
		return E_FAIL;
	}

	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"LoadEntityAttrs() cn is NULL!");
		return E_FAIL;
	}

	pBaseEntity->SetCurDbOperType(DB_OPER_LOAD);

	char sql[5120] = {0};
	_RecordsetPtr rs;

	string strTbl = "";
	string strPrimaryKey = "";
	tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pBaseEntity->GetCompositeFlag());
	if(pInfo)
	{
		strTbl = pInfo->strTblName;
		strPrimaryKey = pInfo->strPrimarykey;
	}

	// 生成SQL语句
	char szGuid[128];
	//GenerateSqlString(sql, 5120, DB_OPER_LOAD, pBaseEntity);
	pBaseEntity->GetGUID().tostring(szGuid);
	sprintf(sql, "select * from %s where %s='%s", strTbl.c_str(),
		strPrimaryKey.c_str(), szGuid);
	strcat(sql, "'");

#ifdef _DEBUG_SM_INFO_
	char szOutPutStr[5120];
	sprintf(szOutPutStr, "In LoadEntityAttrs() sql[%s]", sql);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

	pBaseEntity->SetCurDbOperFlag(1);
	pBaseEntity->SetCurDbOperType(DB_OPER_LOAD);

	string strName = "";
	try
	{
		TESTHR(CreateRs(rs));

		// 更新表
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		if(rs->GetadoEOF()) // 无该记录
		{
			ReleaseRs(rs);
			return E_FAIL;
		}

		vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
		for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
		{
			DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pBaseEntity->GetCompositeFlag(), *epItr);
			strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);

			long lUseageFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pBaseEntity->GetCompositeFlag(), *epItr);
			if(lUseageFlag == 1)//  1|0: 1(数据库自动操作) 0程序操作(默认)
			{
				continue;
			}

			CEntityProperty* pEP = pBaseEntity->GetDataEntityManager().GetEntityProperty(*epItr);
			if(!pEP)
			{
				continue;
			}

			switch(enumDataType)
			{
			case DATA_OBJECT_TIME:
				{
					_variant_t var = rs->GetCollect(strName.c_str());
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
						if(timeBuf[0] == 1900 && timeBuf[1] == 1 && timeBuf[2] == 1)
							memset(timeBuf, 0, sizeof(timeBuf));
					}
					pEP->SetBufAttr((void*)&timeBuf[0], sizeof(DWORD)*6);
				}
				break;
			case  DATA_OBJECT_STRING:
				{
					_variant_t var = rs->GetCollect(strName.c_str());
					if(var.vt != VT_NULL)
					{
						_bstr_t BstrVar = var;
						char* strPtr = (char*)(BstrVar);
						if(strPtr)
							pEP->SetStringAttr(string(strPtr));
					}
				}
				break;
			case DATA_OBJECT_BUFFER:
				{
					LoadBufferField(pBaseEntity, strName.c_str(), rs);
				}
				break;
			case DATA_OBJECT_GUID:
				{
					CGUID tGUID(_bstr_t(rs->GetCollect(strName.c_str())));
					pEP->SetGuidAttr(tGUID);
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
					long value = rs->GetCollect(strName.c_str());
					pEP->SetLongAttr(value);
				}
				break;
			}
		}

		ReleaseRs(rs);
		return S_OK;
	}
	catch(_com_error &e)
	{
		char szOutputInfo[1024];
		sprintf(szOutputInfo, "Load Entity attr %s Err!", strName.c_str());
		PrintErr(szOutputInfo, e);

		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
			ReleaseRs(rs);
		}
		catch (_com_error &e)
		{
			PrintErr("Load Entity ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return S_OK;
}
HRESULT CRsEntity::LoadEntitys(const char* szSql, const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn)
{
	if(!szSql)
	{
		return E_FAIL;
	}

	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"LoadEntitys() cn is NULL!");
		return E_FAIL;
	}


	_RecordsetPtr rs;

	// 测试代码
	string strName = "";
	try
	{
		TESTHR(CreateRs(rs));

		// 更新表
		TESTHR(OpenRs(szSql, rs, cn, adOpenForwardOnly, adLockOptimistic));

#ifdef _DEBUG_SM_INFO_
		char szOutPutStr[5120];
		sprintf(szOutPutStr, "In LoadEntitys() sql[%s]", szSql);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

		while(!rs->GetadoEOF()) // 无该记录
		{
			CBaseEntity* pTmpEntity = GetGame()->GetEntityManager()->NewBaseEntity(strComFlag, NULL_GUID);

			tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pTmpEntity->GetCompositeFlag());
			if(!pInfo)
			{
				ReleaseRs(rs);
				Log4c::Warn(ROOT_MODULE,"LoadEntitys entity %s info is NULL!", pTmpEntity->GetCompositeFlag().c_str());
				return E_FAIL;
			}

			pTmpEntity->SetCurDbOperType(DB_OPER_LOAD);
			vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
			for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
			{
				long lUseageFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pTmpEntity->GetCompositeFlag(), *epItr);
				DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pTmpEntity->GetCompositeFlag(), *epItr);
				strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);

				if(lUseageFlag == 1)//  1|0: 1(数据库自动操作) 0程序操作(默认)
				{
					continue;
				}
				CEntityProperty* pEP = pTmpEntity->GetDataEntityManager().GetEntityProperty(*epItr);
				if(!pEP)
				{
					continue;
				}

				switch(enumDataType)
				{
				case DATA_OBJECT_TIME:
					{
						_variant_t var = rs->GetCollect(strName.c_str());
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
							if(timeBuf[0] == 1900 && timeBuf[1] == 1 && timeBuf[2] == 1)
								memset(timeBuf, 0, sizeof(timeBuf));
						}
						pEP->SetBufAttr((void*)&timeBuf[0], sizeof(DWORD)*6);
					}
					break;
				case  DATA_OBJECT_STRING:
					{
						_variant_t var = rs->GetCollect(strName.c_str());
						if(var.vt != VT_NULL)
						{
							_bstr_t BstrVar = var;
							char* strPtr = (char*)(BstrVar);
							if(strPtr)
								pEP->SetStringAttr(string(strPtr));
						}
					}
					break;
				case DATA_OBJECT_BUFFER:
					{
						LoadBufferField(pTmpEntity, strName.c_str(), rs);
					}
					break;
				case DATA_OBJECT_GUID:
					{
						CGUID tGUID(_bstr_t(rs->GetCollect(strName.c_str())));
						pEP->SetGuidAttr(tGUID);
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
						long value = rs->GetCollect(strName.c_str());
						pEP->SetLongAttr(value);
					}
					break;
				}
			}

			string strPK = pInfo->strPrimarykey;

			if(strPK == "NULL") // 无主键
			{
				CGUID tmpGuid;
				CGUID::CreateGUID(tmpGuid);
				pTmpEntity->SetGUID(tmpGuid);
			}
			else
			{
				CGUID tmpGuid;
				pTmpEntity->GetGuidAttr(strPK, tmpGuid);
				pTmpEntity->SetGUID(tmpGuid);
			}
			map<CGUID, CBaseEntity*>::iterator oldItr = entityMap.find(pTmpEntity->GetGUID());
			if(oldItr != entityMap.end())
			{
				if(pTmpEntity->GetCompositeType() == COM_COMPOSITE)
					*((CEntityGroup*)(oldItr->second)) = *((CEntityGroup*)pTmpEntity);
				else
					*((CEntity*)(oldItr->second)) = *((CEntity*)pTmpEntity);

				delete pTmpEntity;
				oldItr->second->SetCurDbOperFlag(1);
				oldItr->second->SetCurDbOperType(DB_OPER_LOAD);
			}
			else
			{
				pTmpEntity->SetCurDbOperFlag(1);
				pTmpEntity->SetCurDbOperType(DB_OPER_LOAD);
				entityMap[pTmpEntity->GetGUID()] = pTmpEntity;
			}
			rs->MoveNext();
		}

		ReleaseRs(rs);
		return S_OK;
	}
	catch(_com_error &e)
	{
		char strOutputError[2048];
		sprintf(strOutputError, "LoadEntitys Err! %s %s %s",strComFlag.c_str(), szSql, strName.c_str());
		PrintErr(strOutputError, e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
			ReleaseRs(rs);
		}
		catch (_com_error &e)
		{
			PrintErr("LoadEntitys ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return E_FAIL;
}
HRESULT CRsEntity::LoadAllEntityGroup(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"LoadAllEntityGroup() cn is NULL!");
		return E_FAIL;
	}

	char szSql[5120];
	GenerateSqlString(szSql, sizeof(szSql), DB_OPER_LOAD, pBaseEntity);

	// 测试代码
	string strTestLeafFlag = pBaseEntity->GetLeafComFlag();
	string strTestRootFlag = pBaseEntity->GetCompositeFlag();
	string strName = "";

	_RecordsetPtr rs;
	try
	{
		TESTHR(CreateRs(rs));

		// 更新表
		TESTHR(OpenRs(szSql, rs, cn, adOpenForwardOnly, adLockOptimistic));

#ifdef _DEBUG_SM_INFO_
		char szOutPutStr[5120];
		sprintf(szOutPutStr, "In LoadAllEntityGroup() sql[%s]", szSql);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

		CEntityGroup* pEntityGroup = pBaseEntity;
		pBaseEntity->SetCurDbOperType(DB_OPER_LOAD);

		while(!rs->GetadoEOF()) // 无该记录
		{
			CBaseEntity* pTmpEntity = NULL;
			pTmpEntity = GetGame()->GetEntityManager()->NewBaseEntity(pEntityGroup->GetLeafComFlag(), NULL_GUID);
			pTmpEntity->SetCurDbOperType(DB_OPER_LOAD);
			pTmpEntity->SetCurDbOperFlag(1);

			tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pTmpEntity->GetCompositeFlag());
			if(!pInfo)
			{
				ReleaseRs(rs);
				Log4c::Warn(ROOT_MODULE,"LoadAllEntityGroup entity %s info is NULL!", pTmpEntity->GetCompositeFlag().c_str());
				return E_FAIL;
			}

			vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
			for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
			{
				long lUseageFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pTmpEntity->GetCompositeFlag(), *epItr);
				DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pTmpEntity->GetCompositeFlag(), *epItr);
				strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);

				if(lUseageFlag == 1)//  1|0: 1(数据库自动操作) 0程序操作(默认)
				{
					continue;
				}

				CEntityProperty* pEP = pTmpEntity->GetDataEntityManager().GetEntityProperty(*epItr);
				if(!pEP)
				{
					continue;
				}

				switch(enumDataType)
				{
				case DATA_OBJECT_TIME:
					{
						_variant_t var = rs->GetCollect(strName.c_str());
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
							if(timeBuf[0] == 1900 && timeBuf[1] == 1 && timeBuf[2] == 1)
								memset(timeBuf, 0, sizeof(timeBuf));
						}
						pEP->SetBufAttr((void*)&timeBuf[0], sizeof(DWORD)*6);
					}
					break;
				case  DATA_OBJECT_STRING:
					{
						_variant_t var = rs->GetCollect(strName.c_str());
						if(var.vt != VT_NULL)
						{
							_bstr_t BstrVar = var;
							char* strPtr = (char*)(BstrVar);
							if(strPtr)
								pEP->SetStringAttr(string(strPtr));
						}
					}
					break;
				case DATA_OBJECT_BUFFER:
					{
						LoadBufferField(pTmpEntity, strName.c_str(), rs);
					}
					break;
				case DATA_OBJECT_GUID:
					{
						CGUID tGUID(_bstr_t(rs->GetCollect(strName.c_str())));
						pEP->SetGuidAttr(tGUID);
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
						long value = rs->GetCollect(strName.c_str());
						pEP->SetLongAttr(value);
					}
					break;
				}
			}


			string strPK = pInfo->strPrimarykey;

			if(strPK == "NULL") // 无主键
			{
				CGUID tmpGuid;
				CGUID::CreateGUID(tmpGuid);
				pTmpEntity->SetGUID(tmpGuid);
			}
			else
			{
				CGUID tmpGuid;
				pTmpEntity->GetGuidAttr(strPK, tmpGuid);
				pTmpEntity->SetGUID(tmpGuid);
			}

			// 查找指定容器
			if(pEntityGroup->GetHasDetailLeavesFlag() == 1) // 有分散节点
			{
				map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().begin();
				for(; itr != pEntityGroup->GetEntityGroupMap().end(); itr++)
				{
					CEntityGroup* pLeafGroup = (CEntityGroup*)(itr->second);
					if(itr->second->GetCompositeType() == COM_COMPOSITE)
					{
						switch((DATA_OBJECT_TYPE)pLeafGroup->GetDetailComPositeFlagType())
						{
						case  DATA_OBJECT_STRING:
							{	
								if(strcmp(pLeafGroup->GetStringAttr(pLeafGroup->GetDetailComPositeFlagName()), pTmpEntity->GetStringAttr(pLeafGroup->GetDetailComPositeFlagName())) == 0)
								{
									pLeafGroup->AddChild(pTmpEntity);
									break;
								}
							}
							break;
						case DATA_OBJECT_GUID:
							{
								CGUID tGuid1;
								CGUID tGuid2;
								pLeafGroup->GetGuidAttr(pLeafGroup->GetDetailComPositeFlagName(), tGuid1);
								pTmpEntity->GetGuidAttr(pLeafGroup->GetDetailComPositeFlagName(), tGuid2);
								if(tGuid1 == tGuid2)
								{
									pLeafGroup->AddChild(pTmpEntity);
									break;
								}
							}
							break;
						case DATA_OBJECT_BOOL:
						case DATA_OBJECT_CHAR:
						case DATA_OBJECT_BYTE:
						case DATA_OBJECT_SHORT:
						case DATA_OBJECT_WORD:
						case DATA_OBJECT_USHORT:
						case DATA_OBJECT_FLOAT:
						case DATA_OBJECT_LONG:
						case DATA_OBJECT_ULONG:
						case DATA_OBJECT_DWORD:
							{
								if(pLeafGroup->GetLongAttr(pLeafGroup->GetDetailComPositeFlagName()) 
									== pTmpEntity->GetLongAttr(pLeafGroup->GetDetailComPositeFlagName()))
								{
#ifdef _DEBUG_SM_INFO_
									char szOldGuid[128];
									pTmpEntity->GetGUID().tostring(szOldGuid);
									sprintf(szOutPutStr, "In LoadAllEntityGroup() 对象%s %s读取！", 
										pTmpEntity->GetCompositeFlag().c_str(), szOldGuid);
									CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
									pLeafGroup->AddChild(pTmpEntity);
									break;
								}
							}
							break;
						default:
							{
								Log4c::Warn(ROOT_MODULE,"LoadAllEntityGroup() detailleaf err!Root:%s, type:%d",
									pEntityGroup->GetCompositeFlag().c_str(),
									pLeafGroup->GetDetailComPositeFlagType());
								delete pTmpEntity;
								ReleaseRs(rs);
								return E_FAIL;
							}
							break;
						}
					}
				}
			}
			else
			{
				pEntityGroup->AddChild(pTmpEntity);
				GetGame()->GetEntityManager()->SetEntityAllDbOperType(pEntityGroup, DB_OPER_LOAD);
				GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag(pEntityGroup, true);

				// 如果是Group对象为其子节点添加查询条件
				if(pTmpEntity->GetCompositeType() == COM_COMPOSITE)
				{
					map<CGUID, CBaseEntity*>::iterator tmpLeafItr = ((CEntityGroup*)pTmpEntity)->GetEntityGroupMap().begin();
					for(; tmpLeafItr != ((CEntityGroup*)pTmpEntity)->GetEntityGroupMap().end(); tmpLeafItr++)
					{
						CBaseEntity* pTmpLeaf = tmpLeafItr->second;
						if(pTmpLeaf)
						{
							string strLeafPk = "";
							tagEntityBuildInfo* pLeafInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pTmpLeaf->GetCompositeFlag());
							if(pLeafInfo)
								strLeafPk = pLeafInfo->strPrimarykey;

							// 子结点是Group对象 所有子节点的查询都依据GUID
							if(pTmpLeaf->GetCompositeType() == COM_COMPOSITE)
							{
								CTempEntityProperty* pEp = new CTempEntityProperty(strLeafPk, DATA_OBJECT_GUID);
								if(pEp)
								{
									pEp->SetGuidAttr(pTmpEntity->GetGUID());
									pEp->SetDbRelateType(DB_RELATE_NULL);
									((CEntityGroup*)pTmpLeaf)->GetQueryDataVec().push_back(pEp);
								}
							}
						}
					}
				}
			}

			rs->MoveNext();
		}

		ReleaseRs(rs);
		return S_OK;
	}
	catch(_com_error &e)
	{
		char szOutErrStr[2048];
		sprintf(szOutErrStr, "LoadAllEntityGroup Err! %s %s %s", strTestLeafFlag.c_str(), strTestRootFlag.c_str(), strName.c_str());
		PrintErr(szOutErrStr, e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
			ReleaseRs(rs);
		}
		catch (_com_error &e)
		{
			PrintErr("LoadAllEntityGroup ReleaseRs Err!", e);
			return e.Error();
		}

		return e.Error();
	}
	return E_FAIL;
}

// ---DeleteInsert Interfaces.. [10/18/2008 ahc]---
HRESULT CRsEntity::DeleteInsertEntityGroupAttrs(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
	if(!pBaseEntity)
	{
		return E_FAIL;
	}

	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"CreateEntityAttrs() cn is NULL!");
		return E_FAIL;
	}
	pBaseEntity->SetCurDbOperType(DB_OPER_DELETE_INSERT);

	char sql[5120] = {0};
	string strName = "";
	string strTbl = "";
	tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pBaseEntity->GetCompositeFlag());
	if(pInfo)
		strTbl = pInfo->strTblName;
	try
	{
		// 执行删除操作
		GenerateSqlString(sql, 5120, DB_OPER_DELETE, pBaseEntity);
		TESTHR(ExecuteCn(sql, cn));

#ifdef _DEBUG_SM_INFO_
		char szOutPutStr[5120];
		sprintf(szOutPutStr, "In DeleteInsertEntityAttrs() sql[%s]", sql);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		_CommandPtr		m_pCommand;
		m_pCommand.CreateInstance(__uuidof(Command));

		// 取出存储过程参数
		vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
		for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
		{
			_ParameterPtr tParam;
			tParam.CreateInstance(__uuidof(Parameter));

			DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pBaseEntity->GetCompositeFlag(), *epItr);
			strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
			long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pBaseEntity->GetCompositeFlag(), strName);
			if(lUseFlag == 1)
			{
				char szAutoGuid[128];
				CGUID autoGuid;
				CGUID::CreateGUID(autoGuid);
				autoGuid.tostring(szAutoGuid);
				tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, strlen(szAutoGuid), szAutoGuid);
				m_pCommand->Parameters->Append(tParam);
				continue;

			}

			CEntityProperty* pEP = pBaseEntity->GetDataEntityManager().GetEntityProperty(*epItr);
			if(!pEP)
			{
				m_pCommand->Parameters->Append(tParam);
				continue;
			}
			//! 设置参数
			switch(enumDataType)
			{
			case DATA_OBJECT_BUFFER:
				{
					BYTE* buf = NULL;
					long bufSize = pEP->GetBufSize();
					VARIANT varBLOB;
					SAFEARRAY *psa = 0;
					SAFEARRAYBOUND rgsabound[1];
					rgsabound[0].lLbound = 0;

					if(bufSize > 0)
					{
						rgsabound->cElements = bufSize;
						psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
						buf = new BYTE[bufSize];
		
						pEP->GetBufAttr(buf, bufSize);
						for(long i=0; i<bufSize; i++)
							SafeArrayPutElement(psa, &i, &buf[i]);

						varBLOB.parray = psa;
						varBLOB.vt = VT_ARRAY | VT_UI1;
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
						SAFE_DELETE_ARRAY(buf);
						SafeArrayDestroy(psa);
					}
					else
					{
						_variant_t tv;
						tv.ChangeType(VT_NULL);
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
					}
				}
				break;
			case DATA_OBJECT_STRING:
				{

					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
						pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
				}
				break;
			case DATA_OBJECT_GUID:
				{
					char szAutoGuid[128];
					CGUID autoGuid;
					pEP->GetGuidAttr(autoGuid);
					autoGuid.tostring(szAutoGuid);
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), _bstr_t(szAutoGuid));
				}
				break;
			case DATA_OBJECT_TIME:
				{
					long timeBuf[6] = {0};
					pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

					SYSTEMTIME systemTime;
					systemTime.wYear = timeBuf[0];
					systemTime.wMonth = timeBuf[1];
					systemTime.wDay = timeBuf[2];
					systemTime.wHour = timeBuf[3];
					systemTime.wMinute = timeBuf[4];
					systemTime.wSecond = timeBuf[5];

					DOUBLE tDate;
					if( systemTime.wYear == 0
						|| systemTime.wMonth == 0
						|| systemTime.wDay == 0 )
					{
						systemTime.wYear = 1900;
						systemTime.wMonth = 1;
						systemTime.wDay = 1;
					}
					SystemTimeToVariantTime(&systemTime, &tDate);
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
				}
				break;
			case DATA_OBJECT_FLOAT:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
				}
				break;
			case DATA_OBJECT_SHORT:
			case DATA_OBJECT_USHORT:
			case DATA_OBJECT_WORD:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
				}
				break;
			case DATA_OBJECT_LONG:
			case DATA_OBJECT_ULONG:
			case DATA_OBJECT_DWORD:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
				}
				break;

			case DATA_OBJECT_BOOL:
			case DATA_OBJECT_CHAR:
			case DATA_OBJECT_BYTE:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
				}
				break;
			}

			m_pCommand->Parameters->Append(tParam);
		}

		m_pCommand->ActiveConnection = cn;
		string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(pBaseEntity);
		m_pCommand->CommandText = _bstr_t(strProcName.c_str());
		m_pCommand->CommandType = adCmdStoredProc;
		//! 执行
		m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
		return S_OK;
	}
	catch(_com_error &e)
	{
		char szOutputInfo[1024];
		sprintf(szOutputInfo, "Delete Insert Entity Attr %s Err!", strName.c_str());
		PrintErr(szOutputInfo, e);	
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("Delete Insert Entity ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return E_FAIL;
}

HRESULT CRsEntity::DeleteInsertLeaves(const char* szSql, const string& comFlag, 
									  map<CGUID, CBaseEntity*>& entityMap, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"CreateLeaves() cn is NULL!");
		return E_FAIL;
	}

	if(szSql == NULL) return E_FAIL;

	long ltimeStart = timeGetTime();

	string strName = "";
	try
	{
#ifdef _DEBUG_SM_INFO_
		char szOutPutStr[5120];
		sprintf(szOutPutStr, "In DeleteInsertLeaves() sql[%s]", szSql);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

		// 执行删除操作
		TESTHR(ExecuteCn(szSql, cn));

		// 更新表
		map<CGUID, CBaseEntity*>::iterator itr = entityMap.begin();
		while(itr != entityMap.end()) // 无该记录
		{
			CBaseEntity* pTmpEntity = itr->second;

			if(pTmpEntity->GetCurDbOperFlag() == 0)
			{
				++itr;
				continue;
			}

			tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pTmpEntity->GetCompositeFlag());
			if(!pInfo)
			{
				Log4c::Warn(ROOT_MODULE,"DeleteInsertLeaves Entity %s Info is NULL!", pTmpEntity->GetCompositeFlag().c_str());
				return E_FAIL;
			}

			pTmpEntity->SetCurDbOperType(DB_OPER_DELETE_INSERT);

			_CommandPtr		m_pCommand;
			m_pCommand.CreateInstance(__uuidof(Command));

			// 取出存储过程参数
			vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
			for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
			{
				_ParameterPtr tParam;
				tParam.CreateInstance(__uuidof(Parameter));

				DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pTmpEntity->GetCompositeFlag(), *epItr);
				strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
				long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pTmpEntity->GetCompositeFlag(), strName);
				if(lUseFlag == 1)
				{
					char szAutoGuid[128];
					CGUID autoGuid;
					CGUID::CreateGUID(autoGuid);
					autoGuid.tostring(szAutoGuid);
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), szAutoGuid);
					m_pCommand->Parameters->Append(tParam);
					continue;

				}

				CEntityProperty* pEP = pTmpEntity->GetDataEntityManager().GetEntityProperty(*epItr);
				if(!pEP)
				{
					m_pCommand->Parameters->Append(tParam);
					continue;
				}

				//! 设置参数
				switch(enumDataType)
				{
				case DATA_OBJECT_BUFFER:
					{
						BYTE* buf = NULL;
						long bufSize = pEP->GetBufSize();
						VARIANT varBLOB;
						SAFEARRAY *psa = 0;
						SAFEARRAYBOUND rgsabound[1];
						rgsabound[0].lLbound = 0;

						if(bufSize > 0)
						{
							rgsabound->cElements = bufSize;
							psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
							buf = new BYTE[bufSize];

							pEP->GetBufAttr(buf, bufSize);
							for(long i=0; i<bufSize; i++)
								SafeArrayPutElement(psa, &i, &buf[i]);

							varBLOB.parray = psa;
							varBLOB.vt = VT_ARRAY | VT_UI1;
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
							SAFE_DELETE_ARRAY(buf);
							SafeArrayDestroy(psa);
						}
						else
						{
							_variant_t tv;
							tv.ChangeType(VT_NULL);
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
						}
					}
					break;
				case DATA_OBJECT_STRING:
					{

						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
							pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
					}
					break;
				case DATA_OBJECT_GUID:
					{
						char szAutoGuid[128];
						CGUID autoGuid;
						pEP->GetGuidAttr(autoGuid);
						autoGuid.tostring(szAutoGuid);
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, strlen(szAutoGuid), _bstr_t(szAutoGuid));
					}
					break;
				case DATA_OBJECT_TIME:
					{
						long timeBuf[6] = {0};
						pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

						SYSTEMTIME systemTime;
						systemTime.wYear = timeBuf[0];
						systemTime.wMonth = timeBuf[1];
						systemTime.wDay = timeBuf[2];
						systemTime.wHour = timeBuf[3];
						systemTime.wMinute = timeBuf[4];
						systemTime.wSecond = timeBuf[5];

						DOUBLE tDate;
						if( systemTime.wYear == 0
							|| systemTime.wMonth == 0
							|| systemTime.wDay == 0 )
						{
							systemTime.wYear = 1900;
							systemTime.wMonth = 1;
							systemTime.wDay = 1;
						}
						SystemTimeToVariantTime(&systemTime, &tDate);
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
					}
					break;
				case DATA_OBJECT_FLOAT:
					{
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
					}
					break;
				case DATA_OBJECT_SHORT:
				case DATA_OBJECT_USHORT:
				case DATA_OBJECT_WORD:
					{
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
					}
					break;
				case DATA_OBJECT_LONG:
				case DATA_OBJECT_ULONG:
				case DATA_OBJECT_DWORD:
					{
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
					}
					break;

				case DATA_OBJECT_BOOL:
				case DATA_OBJECT_CHAR:
				case DATA_OBJECT_BYTE:
					{
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
					}
					break;
				}

				m_pCommand->Parameters->Append(tParam);
			}

			m_pCommand->ActiveConnection = cn;
			string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(pTmpEntity);
			m_pCommand->CommandText = _bstr_t(strProcName.c_str());
			m_pCommand->CommandType = adCmdStoredProc;
			//! 执行
			m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
			itr++;
		}

		return S_OK;
	}
	catch(_com_error &e)
	{
		char szOutputInfo[1024];
		sprintf(szOutputInfo, "DeleteInsertLeaves() attr %s Err!", strName.c_str());
		PrintErr(szOutputInfo, e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("DeleteInsertLeaves() ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}

	return E_FAIL;
}
HRESULT CRsEntity::DeleteInsertAllEntityGroup(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn == NULL)
	{
		Log4c::Warn(ROOT_MODULE,"DeleteInsertAllEntityGroup() cn is NULL!");
		return E_FAIL;
	}
	long ltimeStart = timeGetTime();

	if(((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().size() == 0) return S_OK;

#ifdef _DEBUG_SM_INFO_
	char szOutPutStr[5120];
	char szGuid[128];
	pBaseEntity->GetGUID().tostring(szGuid);
	sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

	char sql[5120];
	CGUID pLeafGuid;
	string strName = "";
	CEntityGroup* pEntityGroup = pBaseEntity;
	CEntityGroup* pLeafGroup = NULL;
	try
	{
		// 执行删除操作	
		GenerateSqlString(sql, 5120, DB_OPER_DELETE, pBaseEntity);
		TESTHR(ExecuteCn(sql, cn));
#ifdef _DEBUG_SM_INFO_
		sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() Del sql[%s].",sql);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		
		pBaseEntity->SetCurDbOperType(DB_OPER_DELETE_INSERT);
		map<CGUID, CBaseEntity*>::iterator groupitr = pEntityGroup->GetEntityGroupMap().begin();
		while(groupitr != pEntityGroup->GetEntityGroupMap().end())
		{
			if(groupitr->second->GetCompositeType() != COM_COMPOSITE) continue;

			pLeafGroup = (CEntityGroup*)groupitr->second;
			if(!pLeafGroup->GetCurDbOperFlag()) 
			{
#ifdef _DEBUG_SM_INFO_
				sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() pLeafGroup[%s] jump over.", pLeafGroup->GetCompositeFlag().c_str());
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				groupitr++;
				continue;
			}

#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() pLeafGroup[%s].size()=%d.", pLeafGroup->GetCompositeFlag().c_str(), pLeafGroup->GetEntityGroupMap().size());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif

			map<CGUID, CBaseEntity*>::iterator leafItr = pLeafGroup->GetEntityGroupMap().begin();
			while(leafItr != pLeafGroup->GetEntityGroupMap().end())
			{
				if(leafItr->second->GetCurDbOperFlag() == 0)
				{
					++leafItr;
					continue;
				}

				tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(leafItr->second->GetCompositeFlag());
				if(!pInfo)
				{
					Log4c::Warn(ROOT_MODULE,"DeleteInsertEntityGroup Entity %s Info is NULL!", leafItr->second->GetCompositeFlag().c_str());
					return E_FAIL;
				}

				pLeafGuid = leafItr->first;
				leafItr->second->SetCurDbOperType(DB_OPER_DELETE_INSERT);

				_CommandPtr		m_pCommand;
				m_pCommand.CreateInstance(__uuidof(Command));

				// 取出存储过程参数
				vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
				for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
				{
					_ParameterPtr tParam;
					tParam.CreateInstance(__uuidof(Parameter));

					DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(leafItr->second->GetCompositeFlag(), *epItr);
					strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
					long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(leafItr->second->GetCompositeFlag(), strName);
					if(lUseFlag == 1)
					{
						char szAutoGuid[128];
						CGUID autoGuid;
						CGUID::CreateGUID(autoGuid);
						autoGuid.tostring(szAutoGuid);
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), szAutoGuid);
						m_pCommand->Parameters->Append(tParam);
						continue;

					}

					CEntityProperty* pEP = leafItr->second->GetDataEntityManager().GetEntityProperty(*epItr);
					if(!pEP)
					{
						m_pCommand->Parameters->Append(tParam);
						continue;
					}

					//! 设置参数
					switch(enumDataType)
					{
					case DATA_OBJECT_BUFFER:
						{
							BYTE* buf = NULL;
							long bufSize = pEP->GetBufSize();
							VARIANT varBLOB;
							SAFEARRAY *psa = 0;
							SAFEARRAYBOUND rgsabound[1];
							rgsabound[0].lLbound = 0;

							if(bufSize > 0)
							{
								rgsabound->cElements = bufSize;
								psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
								buf = new BYTE[bufSize];
					
								pEP->GetBufAttr(buf, bufSize);
								for(long i=0; i<bufSize; i++)
									SafeArrayPutElement(psa, &i, &buf[i]);

								varBLOB.parray = psa;
								varBLOB.vt = VT_ARRAY | VT_UI1;
								tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
								SAFE_DELETE_ARRAY(buf);
								SafeArrayDestroy(psa);
							}
							else
							{
								_variant_t tv;
								tv.ChangeType(VT_NULL);
								tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
							}
						}
						break;
					case DATA_OBJECT_STRING:
						{

							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
								pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
						}
						break;
					case DATA_OBJECT_GUID:
						{
							char szAutoGuid[128];
							CGUID autoGuid;
							pEP->GetGuidAttr(autoGuid);
							autoGuid.tostring(szAutoGuid);
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, strlen(szAutoGuid), _bstr_t(szAutoGuid));
						}
						break;
					case DATA_OBJECT_TIME:
						{
							long timeBuf[6] = {0};
							pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

							SYSTEMTIME systemTime;
							systemTime.wYear = timeBuf[0];
							systemTime.wMonth = timeBuf[1];
							systemTime.wDay = timeBuf[2];
							systemTime.wHour = timeBuf[3];
							systemTime.wMinute = timeBuf[4];
							systemTime.wSecond = timeBuf[5];

							DOUBLE tDate;
							if( systemTime.wYear == 0
								|| systemTime.wMonth == 0
								|| systemTime.wDay == 0 )
							{
								systemTime.wYear = 1900;
								systemTime.wMonth = 1;
								systemTime.wDay = 1;
							}
							SystemTimeToVariantTime(&systemTime, &tDate);
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
						}
						break;
					case DATA_OBJECT_FLOAT:
						{
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
						}
						break;
					case DATA_OBJECT_SHORT:
					case DATA_OBJECT_USHORT:
					case DATA_OBJECT_WORD:
						{
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
						}
						break;
					case DATA_OBJECT_LONG:
					case DATA_OBJECT_ULONG:
					case DATA_OBJECT_DWORD:
						{
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
						}
						break;

					case DATA_OBJECT_BOOL:
					case DATA_OBJECT_CHAR:
					case DATA_OBJECT_BYTE:
						{
							tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
						}
						break;
					}

					m_pCommand->Parameters->Append(tParam);
				}

				m_pCommand->ActiveConnection = cn;
				string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(leafItr->second);
				m_pCommand->CommandText = _bstr_t(strProcName.c_str());
				m_pCommand->CommandType = adCmdStoredProc;
				//! 执行
				m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
				leafItr++;
			}
			groupitr++;
		}

#ifdef _DEBUG_SM_INFO_
		sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		return S_OK;
	}
	catch(_com_error &e)
	{
		char szleafid[128];
		pLeafGuid.tostring(szleafid);
		char szOutputInfo[2048];
		sprintf(szOutputInfo, "DeleteInsertAllEntityGroup Attr %s id[%s] sql[%s] Err!", 
			strName.c_str(), szleafid, sql);
		PrintErr(szOutputInfo, e);

		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("DeleteInsertAllEntityGroup ReleaseRs Err!", e);
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			return e.Error();
		}
#ifdef _DEBUG_SM_INFO_
		sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		return e.Error();
	}
#ifdef _DEBUG_SM_INFO_
	sprintf(szOutPutStr, "In DeleteInsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
	
	return E_FAIL;
}

//读Buffer
HRESULT CRsEntity::LoadBufferField(CBaseEntity *pBaseEntity, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pBaseEntity == NULL)	return E_FAIL;

	try
	{
		if(rs->GetadoEOF())
		{
			ReleaseRs(rs);
			return E_FAIL;
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
			long lAttrEnum = GetGame()->GetEntityManager()->GetAttrEnumByStr(pBaseEntity->GetCompositeFlag(), string(szFiledName));
			CEntityProperty* ep = pBaseEntity->GetDataEntityManager().GetEntityProperty(lAttrEnum);
			if(ep)
				ep->SetBufAttr(pBuf, lSize);
			return S_OK;
		}
	}
	catch(_com_error e)
	{
		char outStr[1024];
		sprintf(outStr, "LoadBufferField() %s is null!", szFiledName);
		PrintErr(outStr, e);
		return e.Error();
	}
	return E_FAIL;
}



// 存储过程
HRESULT CRsEntity::ProcedureEntity(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
    return ExecuteProcedure(pBaseEntity->GetStringAttr(string("ProcName")),
        pBaseEntity->GetQueryDataVec(),
        pBaseEntity->GetRetQueryDataVec(),
        cn);
}

HRESULT CRsEntity::ExecuteProcedure(string strProcName, 
                                     vector<CTempEntityProperty*>& vInputParam, 
                                     vector<CTempEntityProperty*>& vOutputParam,
                                     _ConnectionPtr& cn)
{
    if(cn==NULL) 
    {
        PrintErr("存盘线程传过来的连接不存在");
        return E_FAIL;
    }

    _CommandPtr cmdPtr;
    _RecordsetPtr rsPtr;
    try
    {
        cmdPtr.CreateInstance(__uuidof(Command));
        cmdPtr->ActiveConnection = cn;
        cmdPtr->CommandText = strProcName.c_str();
        cmdPtr->CommandType = adCmdStoredProc;

        /// 存储过程输入参数
        vector<CTempEntityProperty*>::iterator itor = vInputParam.begin();
        for(; itor != vInputParam.end(); ++itor)
        {
            DATA_OBJECT_TYPE eDataType = (*itor)->GetDataType();
            string strName = (*itor)->GetVarName();

            _ParameterPtr tParam;
            switch(eDataType)
            {
            case DATA_OBJECT_STRING:
                {
                    tParam = cmdPtr->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput, 128, _bstr_t((*itor)->GetStringAttr()));
                }
                break;

            case DATA_OBJECT_DWORD:
            case DATA_OBJECT_LONG:
                {
                    tParam = cmdPtr->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), _bstr_t((*itor)->GetLongAttr()));
                }
                break;

            case DATA_OBJECT_GUID:
                {
                    char szGuid[64];
                    CGUID tGuid;
                    memset(szGuid, 0, sizeof(szGuid));                    
                    (*itor)->GetGuidAttr(tGuid);
                    tGuid.tostring(szGuid);
                    long lGuidLen = static_cast<long>(strlen(szGuid));
                    tParam = cmdPtr->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, lGuidLen, szGuid);
                }
                break;

            case DATA_OBJECT_BUFFER:
                {
                    BYTE* buf = NULL;
                    long bufSize = (*itor)->GetBufSize();
                    VARIANT varBLOB;
                    SAFEARRAY *psa = 0;
                    SAFEARRAYBOUND rgsabound[1];
                    rgsabound[0].lLbound = 0;

                    if(bufSize > 0)
                    {
                        rgsabound->cElements = bufSize;
                        psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
                        buf = new BYTE[bufSize];

                        (*itor)->GetBufAttr(buf, bufSize);
                        for(long i=0; i<bufSize; i++)
                            SafeArrayPutElement(psa, &i, &buf[i]);

                        varBLOB.parray = psa;
                        varBLOB.vt = VT_ARRAY | VT_UI1;
                        tParam = cmdPtr->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
                        SAFE_DELETE_ARRAY(buf);
                        SafeArrayDestroy(psa);
                    }
                    else
                    {
                        _variant_t tv;
                        tv.ChangeType(VT_NULL);
                        tParam = cmdPtr->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
                    }
                }
                break;
            }
            cmdPtr->Parameters->Append(tParam);
        }

        rsPtr = cmdPtr->Execute(NULL, NULL, adCmdStoredProc);

        /// 输出变量
        long lRet = 0;
        vector<CTempEntityProperty*>::iterator itOut = vOutputParam.begin();
        for(; itOut != vOutputParam.end(); ++itOut)
        {
            DATA_OBJECT_TYPE eDataType = (*itOut)->GetDataType();
            switch(eDataType)
            {
            case DATA_OBJECT_LONG:
                {
                    long lVal = rsPtr->GetCollect(lRet);
                    (*itOut)->SetLongAttr(lVal);
                }
                break;

            case DATA_OBJECT_STRING:
                {
                    _variant_t vtVal = rsPtr->GetCollect(lRet);
                    if(vtVal.vt != VT_NULL)
                    {
                        (*itOut)->SetStringAttr(string((_bstr_t)vtVal));
                    }
                }
                break;

            case DATA_OBJECT_GUID:
                {
                    _variant_t vtVal = rsPtr->GetCollect(lRet);
                    if(vtVal.vt != NULL)
                    {
                        CGUID vtGuid((_bstr_t)vtVal);
                        (*itOut)->SetGuidAttr(vtGuid);
                    }
                }
                break;
            }

            lRet++;
        }
        if(vOutputParam.size() > 0)
        {
            ReleaseRs(rsPtr);
        }
        return S_OK;
    }
    catch(_com_error &e)
    {
        PrintErr("ExecuteProcedure error！", e);
        try
        {

            ReleaseCn(cn);
            Sleep(100);
            ReleaseRs(rsPtr);
        }
        catch (_com_error &e)
        {
            PrintErr("ExecuteProcedure ReleaseRs Err!", e);
            return e.Error();
        }
        return e.Error();
    }
    return E_FAIL;
}



// 创建角色特定存储过程
HRESULT CRsEntity::CreateRoleProcedureEntity(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
    HRESULT hResult = ExecuteProcedure(pBaseEntity->GetStringAttr(string("ProcName")),
        pBaseEntity->GetQueryDataVec(),
        pBaseEntity->GetRetQueryDataVec(),
        cn);

    if(E_FAIL == hResult)
        return E_FAIL;

    long nameExit = 0;
    long maxNumFlag = 0;

    if(pBaseEntity->GetEntityGroupMap().size() < 2)
    {
        GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag(pBaseEntity, false);
        return E_FAIL;
    }

    if(pBaseEntity->GetRetQueryDataVec().size() == 2)
    {
        nameExit = pBaseEntity->GetRetQueryDataVec()[0]->GetLongAttr();
        maxNumFlag = pBaseEntity->GetRetQueryDataVec()[1]->GetLongAttr();
    }

    map<CGUID, CBaseEntity*>::iterator itr = pBaseEntity->GetEntityGroupMap().begin();

    CGUID tguid;
    itr->second->GetGuidAttr(string("guid"), tguid);
    itr->second->SetGUID(tguid);
    if(nameExit == 0 && maxNumFlag == 1 ) // 可以创建
        GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag((CEntityGroup*)itr->second, true);
    else
        GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag((CEntityGroup*)itr->second, false);

    itr++;
    itr->second->GetGuidAttr(string("guid"), tguid);
    itr->second->SetGUID(tguid);
    if(nameExit == 0 && maxNumFlag == 1 ) // 可以创建
        GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag((CEntityGroup*)itr->second, true);
    else
        GetGame()->GetEntityManager()->ResetEntityAllDbOperFlag((CEntityGroup*)itr->second, false);

    return S_OK;
	
}
// 添加邮件特定存储过程
HRESULT CRsEntity::InserMailProcedureEntity(CEntityGroup *pBaseEntity, _ConnectionPtr& cn)
{
	// 存盘线程传过来的连接不存在
	if(cn==NULL) 
	{
		PrintErr("存盘线程传过来的连接不存在");
		return E_FAIL;
	}

	// 先判断返回变量是否存在
	CEntityProperty* pRetValue = NULL;
	vector<CTempEntityProperty*>::iterator epItr = pBaseEntity->GetRetQueryDataVec().begin();
	if( epItr != pBaseEntity->GetRetQueryDataVec().end() )
	{
		pRetValue = (*epItr);
	}
	if(!pRetValue) 
	{
		return E_FAIL;
	}

	_CommandPtr		m_pCommand;
	_variant_t		result1;   
	_bstr_t			strProc=pBaseEntity->GetStringAttr(string("ProcName"));

	try
	{
		m_pCommand.CreateInstance(__uuidof(Command));
		m_pCommand->ActiveConnection = cn;
		m_pCommand->CommandText = strProc;
		m_pCommand->CommandType = adCmdStoredProc;

		_ParameterPtr paraReturn = m_pCommand->CreateParameter(_bstr_t("Return"), adInteger, adParamReturnValue, sizeof(LONG), 0);
		m_pCommand->Parameters->Append(paraReturn);


		// 取出存储过程参数
		vector<CTempEntityProperty*>::iterator epItr = pBaseEntity->GetQueryDataVec().begin();
		for(; epItr != pBaseEntity->GetQueryDataVec().end(); epItr++)
		{
			DATA_OBJECT_TYPE enumDataType = (*epItr)->GetDataType();
			string strName = (*epItr)->GetVarName();

			_ParameterPtr tParam;
			//! 设置参数
			switch(enumDataType)
			{
			case DATA_OBJECT_STRING:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarWChar, adParamInput, 2048, _bstr_t((*epItr)->GetStringAttr()));
				}
				break;
			case DATA_OBJECT_DWORD:
			case DATA_OBJECT_LONG:
				{
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (*epItr)->GetLongAttr());
				}
				break;
			case DATA_OBJECT_GUID:
				{
					char szSenderGuid[64] = {0};
					CGUID tGuid;
					(*epItr)->GetGuidAttr( tGuid);
					tGuid.tostring(szSenderGuid);
					tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szSenderGuid), szSenderGuid);
				}
				break;
			case DATA_OBJECT_BUFFER:
				{
					BYTE* buf = NULL;
					long bufSize = (*epItr)->GetBufSize();
					VARIANT varBLOB;
					SAFEARRAY *psa = 0;
					SAFEARRAYBOUND rgsabound[1];
					rgsabound[0].lLbound = 0;

					if(bufSize > 0)
					{
						rgsabound->cElements = bufSize;
						psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
						buf = new BYTE[bufSize];
		
						(*epItr)->GetBufAttr(buf, bufSize);
						for(long i=0; i<bufSize; i++)
							SafeArrayPutElement(psa, &i, &buf[i]);

						varBLOB.parray = psa;
						varBLOB.vt = VT_ARRAY | VT_UI1;
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
						SAFE_DELETE_ARRAY(buf);
						SafeArrayDestroy(psa);
					}
					else
					{
						_variant_t tv;
						tv.ChangeType(VT_NULL);
						tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
					}
				}
				break;
			}	
			m_pCommand->Parameters->Append(tParam);
		}
		// 先插入邮件
		map<CGUID, CBaseEntity*>::iterator itr = pBaseEntity->GetEntityGroupMap().begin();
		if(itr != pBaseEntity->GetEntityGroupMap().end())
		{
			if(!itr->second)
			{
				// 修改返回值数据
				pRetValue->SetLongAttr(-1);
				return E_FAIL;
			}
			CEntityGroup* pMail = (CEntityGroup*)itr->second;

			HRESULT hRet = S_OK;
			// 添加邮件本身
			if(OperEntityGroups(pMail, DB_OPER_INSERT, cn) != S_OK)
			{
				// 修改返回值数据
				pRetValue->SetLongAttr(-1);
				return E_FAIL;
			}

			// 添加邮件物品
			string strTbl = "";
			map<CGUID, CBaseEntity*>::iterator leafitr = pMail->GetEntityGroupMap().begin();
			if(leafitr != pMail->GetEntityGroupMap().end())
			{
				tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(leafitr->second->GetCompositeFlag());
				if(pInfo)
					strTbl = pInfo->strTblName;

				char szSql[5120];
				char szGuid[128];
				pMail->GetGUID().tostring(szGuid);
				sprintf(szSql, "select * from %s where MailID='%s'", strTbl.c_str(), szGuid);
				if(S_OK != OperLeaves(szSql,
					((CEntityGroup*)leafitr->second)->GetLeafComFlag().c_str(), 
					((CEntityGroup*)leafitr->second)->GetEntityGroupMap(), 
                    DB_OPER_INSERT, cn))
				{
					// 修改返回值数据
					pRetValue->SetLongAttr(-1);
					return E_FAIL;
				}
			}
		}

		//! 执行
		m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 

		// 取得返回值
		LONG lRunResult = (LONG)m_pCommand->Parameters->GetItem("Return")->Value;
		pRetValue->SetLongAttr(lRunResult);

		return S_OK;
	}
	catch(_com_error &e)
	{
		PrintErr("CreateRoleProcedureEntity error！", e);
		try
		{
			//if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
			{
				ReleaseCn(cn);
				Sleep(100);
			}
		}
		catch (_com_error &e)
		{
			PrintErr("CreateRoleProcedureEntity ReleaseRs Err!", e);
			return e.Error();
		}
		return e.Error();
	}
	return E_FAIL;
}

HRESULT CRsEntity::OperEntityGroups(CEntityGroup *pBaseEntity, DB_OPERATION_TYPE opType, _ConnectionPtr& cn)

{
    if(!pBaseEntity)
    {
        return E_FAIL;
    }

    // 存盘线程传过来的连接不存在
    if(cn == NULL)
    {
       Log4c::Warn(ROOT_MODULE,"OperEntityGroups() cn is NULL!");
        return E_FAIL;
    }

    string strName = "";
    try
    {
        pBaseEntity->SetCurDbOperType(opType);

        _CommandPtr		m_pCommand;
        m_pCommand.CreateInstance(__uuidof(Command));

        tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pBaseEntity->GetCompositeFlag());
        if(!pInfo) return E_FAIL;

        // 取出存储过程参数
        vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
        for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
        {
            _ParameterPtr tParam;
            tParam.CreateInstance(__uuidof(Parameter));

            DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pBaseEntity->GetCompositeFlag(), *epItr);
            strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
            long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pBaseEntity->GetCompositeFlag(), strName);
            if(lUseFlag == 1)
            {
                char szAutoGuid[128];
                CGUID autoGuid;
                CGUID::CreateGUID(autoGuid);
                autoGuid.tostring(szAutoGuid);
                tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), szAutoGuid);
                m_pCommand->Parameters->Append(tParam);
                continue;

            }

            CEntityProperty* pEP = pBaseEntity->GetDataEntityManager().GetEntityProperty(*epItr);
            if(!pEP)
            {
                m_pCommand->Parameters->Append(tParam);
                continue;
            }

            //! 设置参数
            switch(enumDataType)
            {
            case DATA_OBJECT_BUFFER:
                {
                    BYTE* buf = NULL;
                    long bufSize = pEP->GetBufSize();
                    VARIANT varBLOB;
                    SAFEARRAY *psa = 0;
                    SAFEARRAYBOUND rgsabound[1];
                    rgsabound[0].lLbound = 0;

                    if(bufSize > 0)
                    {
                        rgsabound->cElements = bufSize;
                        psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
                        buf = new BYTE[bufSize];

                        pEP->GetBufAttr(buf, bufSize);
                        for(long i=0; i<bufSize; i++)
                            SafeArrayPutElement(psa, &i, &buf[i]);

                        varBLOB.parray = psa;
                        varBLOB.vt = VT_ARRAY | VT_UI1;
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
                        SAFE_DELETE_ARRAY(buf);
                        SafeArrayDestroy(psa);
                    }
                    else
                    {
                        _variant_t tv;
                        tv.ChangeType(VT_NULL);
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
                    }
                }
                break;
            case DATA_OBJECT_STRING:
                {

                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
                        pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
                }
                break;
            case DATA_OBJECT_GUID:
                {
                    char szAutoGuid[128];
                    CGUID autoGuid;
                    pEP->GetGuidAttr(autoGuid);
                    autoGuid.tostring(szAutoGuid);
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), _bstr_t(szAutoGuid));
                }
                break;
            case DATA_OBJECT_TIME:
                {
                    long timeBuf[6] = {0};
                    pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

                    SYSTEMTIME systemTime;
                    systemTime.wYear = timeBuf[0];
                    systemTime.wMonth = timeBuf[1];
                    systemTime.wDay = timeBuf[2];
                    systemTime.wHour = timeBuf[3];
                    systemTime.wMinute = timeBuf[4];
                    systemTime.wSecond = timeBuf[5];

                    DOUBLE tDate;
                    if( systemTime.wYear == 0
                        || systemTime.wMonth == 0
                        || systemTime.wDay == 0 )
                    {
                        systemTime.wYear = 1900;
                        systemTime.wMonth = 1;
                        systemTime.wDay = 1;
                    }
                    SystemTimeToVariantTime(&systemTime, &tDate);
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
                }
                break;
            case DATA_OBJECT_FLOAT:
                {
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
                }
                break;
            case DATA_OBJECT_SHORT:
            case DATA_OBJECT_USHORT:
            case DATA_OBJECT_WORD:
                {
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
                }
                break;
            case DATA_OBJECT_LONG:
            case DATA_OBJECT_ULONG:
            case DATA_OBJECT_DWORD:
                {
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
                }
                break;

            case DATA_OBJECT_BOOL:
            case DATA_OBJECT_CHAR:
            case DATA_OBJECT_BYTE:
                {
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
                }
                break;
            }

            m_pCommand->Parameters->Append(tParam);
        }

        m_pCommand->ActiveConnection = cn;
        string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(pBaseEntity);
        m_pCommand->CommandText = _bstr_t(strProcName.c_str());
        m_pCommand->CommandType = adCmdStoredProc;
        //! 执行
        m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
        return S_OK;
    }
    catch(_com_error &e)
    {
        char szOutputInfo[1024];
        sprintf(szOutputInfo, "OperEntityGroups %s Err!", strName.c_str());
        PrintErr(szOutputInfo, e);
        try
        {
            //if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
            {
                ReleaseCn(cn);
                Sleep(100);
            }
        }
        catch (_com_error &e)
        {
            PrintErr("OperEntityGroups ReleaseRs Err!", e);
            return e.Error();
        }
        return e.Error();
    }
    return E_FAIL;
}

HRESULT CRsEntity::OperLeaves(const char* szTblName, const char* szComFlag, map<CGUID,CBaseEntity*>& entityMap, DB_OPERATION_TYPE opType, _ConnectionPtr& cn)

{
    // 存盘线程传过来的连接不存在
    if(cn == NULL)
    {
        Log4c::Warn(ROOT_MODULE,"OperLeaves() cn is NULL!");
        return E_FAIL;
    }

    if(entityMap.size() == 0) return S_OK;

    string strTbl = "";
    tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(szComFlag);
    if(pInfo)
        strTbl = pInfo->strTblName;

    try
    {
#ifdef _DEBUG_SM_INFO_
        char szOutPutStr[5120];
        sprintf(szOutPutStr, "In OperLeaves() sql[%s]", szTblName);
        CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
        map<CGUID, CBaseEntity*>::iterator itr = entityMap.begin();
        while(itr != entityMap.end()) // 无该记录
        {
            CBaseEntity* pTmpEntity = itr->second;

            if(pTmpEntity->GetCurDbOperFlag() == 0)
            {
                ++itr;
                continue;
            }

            pTmpEntity->SetCurDbOperType(opType);

            string strName = "";
            _CommandPtr		m_pCommand;
            m_pCommand.CreateInstance(__uuidof(Command));
            // 取出存储过程参数
            vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
            for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
            {
                _ParameterPtr tParam;
                tParam.CreateInstance(__uuidof(Parameter));

                DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(pTmpEntity->GetCompositeFlag(), *epItr);
                strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
                long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(pTmpEntity->GetCompositeFlag(), strName);
                if(lUseFlag == 1)
                {
                    char szAutoGuid[128];
                    CGUID autoGuid;
                    CGUID::CreateGUID(autoGuid);
                    autoGuid.tostring(szAutoGuid);
                    tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), szAutoGuid);
                    m_pCommand->Parameters->Append(tParam);
                    continue;

                }

                CEntityProperty* pEP = pTmpEntity->GetDataEntityManager().GetEntityProperty(*epItr);
                if(!pEP)
                {
                    m_pCommand->Parameters->Append(tParam);
                    continue;
                }

                //! 设置参数
                switch(enumDataType)
                {
                case DATA_OBJECT_BUFFER:
                    {
                        BYTE* buf = NULL;
                        long bufSize = pEP->GetBufSize();
                        VARIANT varBLOB;
                        SAFEARRAY *psa = 0;
                        SAFEARRAYBOUND rgsabound[1];
                        rgsabound[0].lLbound = 0;

                        if(bufSize > 0)
                        {
                            rgsabound->cElements = bufSize;
                            psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
                            buf = new BYTE[bufSize];

                            pEP->GetBufAttr(buf, bufSize);
                            for(long i=0; i<bufSize; i++)
                                SafeArrayPutElement(psa, &i, &buf[i]);

                            varBLOB.parray = psa;
                            varBLOB.vt = VT_ARRAY | VT_UI1;
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
                            SAFE_DELETE_ARRAY(buf);
                            SafeArrayDestroy(psa);
                        }
                        else
                        {
                            _variant_t tv;
                            tv.ChangeType(VT_NULL);
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
                        }
                    }
                    break;
                case DATA_OBJECT_STRING:
                    {

                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
                            pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
                    }
                    break;
                case DATA_OBJECT_GUID:
                    {
                        char szAutoGuid[128];
                        CGUID autoGuid;
                        pEP->GetGuidAttr(autoGuid);
                        autoGuid.tostring(szAutoGuid);
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), _bstr_t(szAutoGuid));
                    }
                    break;
                case DATA_OBJECT_TIME:
                    {
                        long timeBuf[6] = {0};
                        pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

                        SYSTEMTIME systemTime;
                        systemTime.wYear = timeBuf[0];
                        systemTime.wMonth = timeBuf[1];
                        systemTime.wDay = timeBuf[2];
                        systemTime.wHour = timeBuf[3];
                        systemTime.wMinute = timeBuf[4];
                        systemTime.wSecond = timeBuf[5];

                        DOUBLE tDate;
                        if( systemTime.wYear == 0
                            || systemTime.wMonth == 0
                            || systemTime.wDay == 0 )
                        {
                            systemTime.wYear = 1900;
                            systemTime.wMonth = 1;
                            systemTime.wDay = 1;
                        }
                        SystemTimeToVariantTime(&systemTime, &tDate);
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
                    }
                    break;
                case DATA_OBJECT_FLOAT:
                    {
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
                    }
                    break;
                case DATA_OBJECT_SHORT:
                case DATA_OBJECT_USHORT:
                case DATA_OBJECT_WORD:
                    {
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
                    }
                    break;
                case DATA_OBJECT_LONG:
                case DATA_OBJECT_ULONG:
                case DATA_OBJECT_DWORD:
                    {
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
                    }
                    break;

                case DATA_OBJECT_BOOL:
                case DATA_OBJECT_CHAR:
                case DATA_OBJECT_BYTE:
                    {
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
                    }
                    break;
                }

                m_pCommand->Parameters->Append(tParam);
            }

            m_pCommand->ActiveConnection = cn;
            string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(pTmpEntity);
            m_pCommand->CommandText = _bstr_t(strProcName.c_str());
            m_pCommand->CommandType = adCmdStoredProc;
            //! 执行
            m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
            itr++;
        }

        return S_OK;
    }
    catch(_com_error &e)
    {
        PrintErr("OperLeaves() Err!", e);
        try
        {
            //if(e.Error() == 0x800a0e7d || e.Error() == 0x800a0e78 || e.Error() == 0x80004005) // 连接无法用于执行此操作。在此上下文中它可能已被关闭或无效。
            {
                ReleaseCn(cn);
                Sleep(100);
            }
        }
        catch (_com_error &e)
        {
            PrintErr("OperLeaves() ReleaseRs Err!", e);
            return e.Error();
        }
        return e.Error();
    }

    return E_FAIL;
}

HRESULT CRsEntity::OperAllEntityGroups(CEntityGroup *pBaseEntity, DB_OPERATION_TYPE opType, _ConnectionPtr& cn)
{
    // 存盘线程传过来的连接不存在
    if(cn == NULL)
    {
        Log4c::Warn(ROOT_MODULE,"OperAllEntityGroups() cn is NULL!");
        return E_FAIL;
    }

    if(((CEntityGroup*)pBaseEntity)->GetEntityGroupMap().size() == 0) return S_OK;

    pBaseEntity->SetCurDbOperType(opType);

    string strTbl = "";
    tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pBaseEntity->GetCompositeFlag());
    if(pInfo)
        strTbl = pInfo->strTblName;

    try
    {
        string strName = "";
        bool isCreated = false;
        CEntityGroup* pEntityGroup = (CEntityGroup*)pBaseEntity;
        map<CGUID, CBaseEntity*>::iterator groupitr = pEntityGroup->GetEntityGroupMap().begin();
        while(groupitr != pEntityGroup->GetEntityGroupMap().end())
        {
            CEntityGroup* pLeafGroup = (CEntityGroup*)groupitr->second;
            map<CGUID, CBaseEntity*>::iterator leafItr = pLeafGroup->GetEntityGroupMap().begin();
            while(leafItr != pLeafGroup->GetEntityGroupMap().end())
            {

                if(leafItr->second->GetCurDbOperFlag() == 0)
                {
                    ++leafItr;
                    continue;
                }

                _CommandPtr		m_pCommand;
                m_pCommand.CreateInstance(__uuidof(Command));
                // 取出存储过程参数
                vector<long>::iterator epItr = pInfo->vecAttrEnum.begin();
                for(; epItr != pInfo->vecAttrEnum.end(); epItr++)
                {
                    _ParameterPtr tParam;
                    tParam.CreateInstance(__uuidof(Parameter));

                    DATA_OBJECT_TYPE enumDataType = (DATA_OBJECT_TYPE)GetGame()->GetEntityManager()->GetDataObjectType(leafItr->second->GetCompositeFlag(), *epItr);
                    strName = GetGame()->GetEntityManager()->GetAttrStrByEnum(*epItr);
                    long lUseFlag = GetGame()->GetEntityManager()->GetDbUseageFlag(leafItr->second->GetCompositeFlag(), strName);
                    if(lUseFlag == 1)
                    {
                        char szAutoGuid[128];
                        CGUID autoGuid;
                        CGUID::CreateGUID(autoGuid);
                        autoGuid.tostring(szAutoGuid);
                        tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), szAutoGuid);
                        m_pCommand->Parameters->Append(tParam);
                        continue;

                    }

                    CEntityProperty* pEP = leafItr->second->GetDataEntityManager().GetEntityProperty(*epItr);
                    if(!pEP)
                    {
                        m_pCommand->Parameters->Append(tParam);
                        continue;
                    }

                    //! 设置参数
                    switch(enumDataType)
                    {
                    case DATA_OBJECT_BUFFER:
                        {
                            BYTE* buf = NULL;
                            long bufSize = pEP->GetBufSize();
                            VARIANT varBLOB;
                            SAFEARRAY *psa = 0;
                            SAFEARRAYBOUND rgsabound[1];
                            rgsabound[0].lLbound = 0;

                            if(bufSize > 0)
                            {
                                rgsabound->cElements = bufSize;
                                psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
                                buf = new BYTE[bufSize];

                                pEP->GetBufAttr(buf, bufSize);
                                for(long i=0; i<bufSize; i++)
                                    SafeArrayPutElement(psa, &i, &buf[i]);

                                varBLOB.parray = psa;
                                varBLOB.vt = VT_ARRAY | VT_UI1;
                                tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, bufSize, varBLOB);
                                SAFE_DELETE_ARRAY(buf);
                                SafeArrayDestroy(psa);
                            }
                            else
                            {
                                _variant_t tv;
                                tv.ChangeType(VT_NULL);
                                tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarBinary, adParamInput, 2048, tv);
                            }
                        }
                        break;
                    case DATA_OBJECT_STRING:
                        {

                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adVarChar, adParamInput,
                                pEP->GetBufSize(), _bstr_t(pEP->GetStringAttr()));
                        }
                        break;
                    case DATA_OBJECT_GUID:
                        {
                            char szAutoGuid[128];
                            CGUID autoGuid;
                            pEP->GetGuidAttr(autoGuid);
                            autoGuid.tostring(szAutoGuid);
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adGUID, adParamInput, (ADO_LONGPTR)strlen(szAutoGuid), _bstr_t(szAutoGuid));
                        }
                        break;
                    case DATA_OBJECT_TIME:
                        {
                            long timeBuf[6] = {0};
                            pEP->GetBufAttr(timeBuf, sizeof(DWORD)*6);

                            SYSTEMTIME systemTime;
                            systemTime.wYear = timeBuf[0];
                            systemTime.wMonth = timeBuf[1];
                            systemTime.wDay = timeBuf[2];
                            systemTime.wHour = timeBuf[3];
                            systemTime.wMinute = timeBuf[4];
                            systemTime.wSecond = timeBuf[5];

                            DOUBLE tDate;
                            if( systemTime.wYear == 0
                                || systemTime.wMonth == 0
                                || systemTime.wDay == 0 )
                            {
                                systemTime.wYear = 1900;
                                systemTime.wMonth = 1;
                                systemTime.wDay = 1;
                            }
                            SystemTimeToVariantTime(&systemTime, &tDate);
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDate, adParamInput, sizeof(DATE), tDate);
                        }
                        break;
                    case DATA_OBJECT_FLOAT:
                        {
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adDouble, adParamInput, sizeof(DOUBLE), (DOUBLE)pEP->GetLongAttr());
                        }
                        break;
                    case DATA_OBJECT_SHORT:
                    case DATA_OBJECT_USHORT:
                    case DATA_OBJECT_WORD:
                        {
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adSmallInt, adParamInput, sizeof(short), (short)pEP->GetLongAttr());
                        }
                        break;
                    case DATA_OBJECT_LONG:
                    case DATA_OBJECT_ULONG:
                    case DATA_OBJECT_DWORD:
                        {
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adInteger, adParamInput, sizeof(LONG), (LONG)pEP->GetLongAttr());
                        }
                        break;

                    case DATA_OBJECT_BOOL:
                    case DATA_OBJECT_CHAR:
                    case DATA_OBJECT_BYTE:
                        {
                            tParam = m_pCommand->CreateParameter(_bstr_t(strName.c_str()), adTinyInt, adParamInput, sizeof(BYTE), (BYTE)pEP->GetLongAttr());
                        }
                        break;
                    }

                    m_pCommand->Parameters->Append(tParam);
                }

                m_pCommand->ActiveConnection = cn;
                string strProcName = GetGame()->GetEntityManager()->FindTblProcedure(leafItr->second);
                m_pCommand->CommandText = _bstr_t(strProcName.c_str());
                m_pCommand->CommandType = adCmdStoredProc;
                //! 执行
                m_pCommand->Execute(NULL,NULL,adCmdStoredProc); 
                leafItr++;
            }
            groupitr++;
        }

        return S_OK;
    }
    catch(_com_error &e)
    {
        PrintErr("OperAllEntityGroups() Err!", e);
        try
        {
            ReleaseCn(cn);
            Sleep(100);
        }
        catch (_com_error &e)
        {
            PrintErr("OperAllEntityGroups ReleaseRs Err!", e);
            return e.Error();
        }
        return e.Error();
    }
    return E_FAIL;
}

#pragma warning(pop)
