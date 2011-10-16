#include "StdAfx.h"
#include ".\rsfaction.h"
#include "..\..\server\DBServer\AppWorld\DBFactionApplyGroup.h"
#include "..\..\server\DBServer\AppWorld\DBFactionMemberGroup.h"
#include "..\..\server\dbserver\appworld\dbentitymanager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../../server/DBServer/AppWorld/DBLoadThread.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRsFaction::CRsFaction(void)
{
}

CRsFaction::~CRsFaction(void)
{
}
//存Buffer
bool CRsFaction::SaveBufferField(CBaseDBEntity *pBaseEntity, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pBaseEntity == NULL)	return false;
	if(szFiledName == NULL) return false;

	VARIANT varBLOB;
	SAFEARRAY* psa;

	CEntityProperty* ep = pBaseEntity->GetDataEntityManager().GetEntityProperty(string(szFiledName));
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
			PrintErr("Save Fatction Buffer Err!", e);
			SAFE_DELETE_ARRAY(buffer);
			return false;
		}

		SafeArrayDestroy(psa);
	}
	SAFE_DELETE_ARRAY(buffer);
	return true;
}
//读Buffer
bool CRsFaction::LoadBufferField(CBaseDBEntity *pBaseEntity, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pBaseEntity == NULL)	return false;

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
			CEntityProperty* ep = pBaseEntity->GetDataEntityManager().GetEntityProperty(string(szFiledName));
			if(ep)
				ep->SetBufAttr(0, pBuf, lSize);
		}
	}
	catch(_com_error e)
	{
		PrintErr("Load Fatction Buffer Err!", e);
		return false;
	}
	return true;
}



//! 读取所有帮会
BOOL CRsFaction::LoadAllFaction(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;

	//! 取出所有帮会基本信息
	if (!LoadAllFaction_BaseInfo(mapFaction, cn)) return FALSE;
	//! 取出所有帮会成员（包括申请者）
	if (!LoadAllFaction_Member(mapFaction, cn)) return FALSE;
	//! 取出所有帮会成员申请者
	if (!LoadAllFaction_Apply(mapFaction, cn)) return FALSE;
	//! 取出所有帮会留言
	if (!LoadAllFaction_LeaveWord(mapFaction, cn)) return FALSE;

	return TRUE;
}

//! 保存一个帮会
BOOL CRsFaction::SaveOneFaction(CDBFaction* pDBFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;
	do 
	{
		//! 更新（包括新建）一个帮会基本信息
		if (!SaveOneFaction_BaseInfo(pDBFaction, cn))
			break;
		////! 更新（包括新建）一个帮会成员
		//if (!SaveOneFaction_Member(pDBFaction, cn))
		//	break;
		////! 更新（包括新建）一个申请者
		//if (!SaveOneFaction_Apply(pDBFaction, cn))
		//	break;
		////! 更新（包括新建）一个帮会留言
		//if (!SaveOneFaction_LeaveWord(pDBFaction, cn))
		//	break;

		return TRUE;
	} while(false);
	PrintErr("帮会更新失败，所有改变已回滚！");
	return FALSE;
}

//! 删除一个帮会
BOOL CRsFaction::DelOneFaction(const CGUID &FactionGuid, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;

	BOOL bRe = TRUE;
	//! 删除一个帮会基本信息
	if (!DelOneFaction_BaseInfo(FactionGuid, cn))
	{
		bRe = FALSE;
		PrintErr("删除工会基本信息失败");
	}
	//! 删除帮会成员
	if (!DelOneFaction_Member(FactionGuid, cn))
	{
		bRe = FALSE;
		PrintErr("删除工会成员失败");
	}
	//! 删除一个申请者
	if (!DelOneFaction_Apply(FactionGuid, cn))
	{
		bRe = FALSE;
		PrintErr("删除工会申请者失败");
	}
	//! 删除一个帮会留言
	if (!DelOneFaction_LeaveWord(FactionGuid, cn))
	{
		bRe = FALSE;
		PrintErr("删除工会留言失败");
	}

	return bRe;
}



//! 取出所有帮会基本信息
BOOL CRsFaction::LoadAllFaction_BaseInfo(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;

	BOOL bRe = TRUE;
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));

		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Faction_BaseProperty ORDER BY FactionGuid");
		TESTHR(	OpenRs(szSql, rs, cn, adOpenForwardOnly) );

		assert(0 == mapFaction.size());

		while (!rs->GetadoEOF())
		{
			//! 
			CDBFaction *pDBFaction = new CDBFaction(NULL_GUID);
			
			CDataEntityManager::EntityPropertyMapItr epItr = pDBFaction->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pDBFaction->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_Faction_BaseProperty")
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
							LoadBufferField(pDBFaction, epItr->first.c_str(), rs);
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

			CGUID facGuid;
			pDBFaction->GetGuidAttr(string("FactionGuid"), 0, facGuid);
			pDBFaction->SetExID(facGuid);
			pDBFaction->GetFactionApplyGroupPtr()->SetOwnerGUID(facGuid);
			pDBFaction->GetFactionMemberGroupPtr()->SetOwnerGUID(facGuid);
			mapFaction[facGuid] = pDBFaction;

			rs->MoveNext();
		}
		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr("读取工会基本信息错误！", e);		
		return FALSE;
	}
	return bRe;
}

//! 取出所有帮会成员
BOOL CRsFaction::LoadAllFaction_Member(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;

	BOOL bRe = TRUE;
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));

		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Faction_Members ORDER BY FactionGuid_members");
		TESTHR(	OpenRs(szSql, rs, cn, adOpenForwardOnly) );

		CGUID CurrFactionGuid, CurrGuid;
		CDBFaction* pCurrFaction = NULL;
		while (!rs->GetadoEOF())
		{
			CurrGuid = CGUID(((char*)(_bstr_t)rs->GetCollect("FactionGuid_members")));
			if (NULL_GUID == CurrGuid)
			{
				rs->MoveNext();
				continue;
			}
			if (CurrGuid != CurrFactionGuid)
			{
				map<CGUID, CDBFaction*>::iterator ite = mapFaction.find(CurrGuid);
				if (ite != mapFaction.end())
				{
					CurrFactionGuid = CurrGuid;
					pCurrFaction = ite->second;
					pCurrFaction->GetFactionMemberGroupPtr()->Clear();
				}
				else
				{
					CurrFactionGuid = NULL_GUID;
					rs->MoveNext();
					continue;
				}
			}
			CGUID memberGuid = CGUID(((char*)(_bstr_t)rs->GetCollect("MemberGuid")));
			CDBFactionMember* tFacMember = new CDBFactionMember(memberGuid);
			CDataEntityManager::EntityPropertyMapItr epItr = tFacMember->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != tFacMember->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_Faction_Members")
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
							LoadBufferField(pCurrFaction, epItr->first.c_str(), rs);
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

			pCurrFaction->GetFactionMemberGroupPtr()->SetOwnerGUID(pCurrFaction->GetExID());
			pCurrFaction->GetFactionMemberGroupPtr()->GetMemberGroupSet().insert(tFacMember);
			rs->MoveNext();
		}
		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr("读取工会成员信息错误！", e);		
		return FALSE;
	}
	return bRe;
}

//! 取出所有帮会成员申请者
BOOL CRsFaction::LoadAllFaction_Apply(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;

	BOOL bRe = TRUE;
	_RecordsetPtr rs;

	try
	{
		TESTHR(CreateRs(rs));

		char szSql[512] = {0};
		sprintf(szSql,"SELECT * FROM CSL_Faction_Apply ORDER BY FactionGuid_Apply");
		TESTHR(	OpenRs(szSql, rs, cn, adOpenForwardOnly) );

		CGUID CurrFactionGuid, CurrGuid;
		CDBFaction* pCurrFaction = NULL;
		while (!rs->GetadoEOF())
		{
			CurrGuid = CGUID(((char*)(_bstr_t)rs->GetCollect("FactionGuid_Apply")));
			if (NULL_GUID == CurrGuid)
			{
				rs->MoveNext();
				continue;
			}
			if (CurrGuid != CurrFactionGuid)
			{
				map<CGUID, CDBFaction*>::iterator ite = mapFaction.find(CurrGuid);
				if (ite != mapFaction.end())
				{
					CurrFactionGuid = CurrGuid;
					pCurrFaction = ite->second;
					pCurrFaction->GetFactionApplyGroupPtr()->Clear();
				}
				else
				{
					CurrFactionGuid = NULL_GUID;
					rs->MoveNext();
					continue;
				}
			}
			CGUID applyGuid = CGUID(((char*)(_bstr_t)rs->GetCollect("PlayerGuid")));
			CDBFactionApply* tFacApply = new CDBFactionApply(applyGuid);
			CDataEntityManager::EntityPropertyMapItr epItr = tFacApply->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != tFacApply->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				if(epItr->second->GetDBTableName() == "CSL_Faction_Apply")
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
							LoadBufferField(pCurrFaction, epItr->first.c_str(), rs);
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

			pCurrFaction->GetFactionApplyGroupPtr()->SetOwnerGUID(pCurrFaction->GetExID());
			pCurrFaction->GetFactionApplyGroupPtr()->GetApplayGroupSet().insert(tFacApply);
			rs->MoveNext();
		}
		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr("读取工会申请者列表错误！", e);		
		return FALSE;
	}
	return bRe;
}

//! 取出所有帮会留言
BOOL CRsFaction::LoadAllFaction_LeaveWord(map<CGUID, CDBFaction*> &mapFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;
	return TRUE;
}



//! 更新（包括新建）一个帮会基本信息
BOOL CRsFaction::SaveOneFaction_BaseInfo(CDBFaction* pDBFaction, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;
	if( NULL == pDBFaction)	return FALSE;
	BOOL bRe = TRUE;

	_RecordsetPtr rs;

	char szFacGuid[64] = {0};
	CGUID facGuid;
	pDBFaction->GetGuidAttr(string("FactionGuid"), 0, facGuid);
	facGuid.tostring(szFacGuid);
	char szMasterGuid[64] = {0};

	try
	{
		//! 先删除相同ID的数据
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_BaseProperty WHERE FactionGuid='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));

		//! 再添加
		TESTHR(CreateRs(rs));

		TESTHR(OpenRs("CSL_Faction_BaseProperty", rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));
		rs->AddNew();

		CDataEntityManager::EntityPropertyMapItr epItr = pDBFaction->GetDataEntityManager().GetEntityPropertyMap().begin();
		for(; epItr != pDBFaction->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
		{
			CEntityProperty* ep = epItr->second;
			if(ep)
			{
				if(ep->GetDBTableName() == "CSL_Faction_BaseProperty")
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
							rs->PutCollect(epItr->first.c_str(), szTimeValue);
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							rs->PutCollect(epItr->first.c_str(), ep->GetStringAttr(0));
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							SaveBufferField(pDBFaction, epItr->first.c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID;
							ep->GetGuidAttr(0, tGUID);
							char szGUID[128];
							tGUID.tostring(szGUID);
							rs->PutCollect(epItr->first.c_str(), szGUID);
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
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		ReleaseRs(rs);

		bRe = FALSE;
	}

	return bRe;
}

//! 更新（包括新建）一个帮会的成员
BOOL CRsFaction::SaveOneFaction_Member(CDBFactionMemberGroup* tMemberGroup, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;
	if( NULL == tMemberGroup)	return FALSE;
	BOOL bRe = TRUE;

	_RecordsetPtr rs;

	char szFacGuid[64] = {0};
	tMemberGroup->GetOwnerGUID().tostring(szFacGuid);

	try
	{
		//! 先删除相同ID的数据
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_Members WHERE FactionGuid_members='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));

		if(!tMemberGroup->GetMemberGroupSet().size()) return TRUE;

		//! 再添加
		TESTHR(CreateRs(rs));

		TESTHR(OpenRs("CSL_Faction_Members", rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));
	
		set<CDBFactionMember*>::iterator memberItr = tMemberGroup->GetMemberGroupSet().begin();
		for(; memberItr != tMemberGroup->GetMemberGroupSet().end(); memberItr++)
		{
			rs->AddNew();

			CDataEntityManager::EntityPropertyMapItr epItr = (*memberItr)->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != (*memberItr)->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				CEntityProperty* ep = epItr->second;
				if(ep)
				{
					if(ep->GetDBTableName() == "CSL_Faction_Members")
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
								rs->PutCollect(epItr->first.c_str(), szTimeValue);
							}
							break;
						case  DATA_OBJECT_STRING:
							{
								rs->PutCollect(epItr->first.c_str(), ep->GetStringAttr(0));
							}
							break;
						case DATA_OBJECT_BUFFER:
							{
								SaveBufferField((CBaseDBEntity*)this, epItr->first.c_str(), rs);
							}
							break;
						case DATA_OBJECT_GUID:
							{
								CGUID tGUID;
								ep->GetGuidAttr(0, tGUID);
								char szGUID[128];
								tGUID.tostring(szGUID);
								if(ep->GetDbFlag() == 0) // 0:非主键,1:主键
									rs->PutCollect(epItr->first.c_str(), szGUID);
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

		}
		
		rs->Update();

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		ReleaseRs(rs);

		bRe = FALSE;
	}

	return bRe;
}

//! 更新（包括新建）一个帮会的申请者
BOOL CRsFaction::SaveOneFaction_Apply(CDBFactionApplyGroup* tApplyGroup, _ConnectionPtr& cn)
{
	if( NULL == cn)	return FALSE;
	if( NULL == tApplyGroup)	return FALSE;


	BOOL bRe = TRUE;

	_RecordsetPtr rs;

	char szFacGuid[64] = {0};
	tApplyGroup->GetOwnerGUID().tostring(szFacGuid);

	try
	{
		//! 先删除相同ID的数据
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_Apply WHERE FactionGuid_Apply='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));
	
		if(!tApplyGroup->GetApplayGroupSet().size()) return TRUE;

		TESTHR(CreateRs(rs));
		//! 再添加
		set<CDBFactionApply*>::iterator applyItr = tApplyGroup->GetApplayGroupSet().begin();
		for(; applyItr != tApplyGroup->GetApplayGroupSet().end(); applyItr++)
		{
			rs->AddNew();

			CDataEntityManager::EntityPropertyMapItr epItr = (*applyItr)->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != (*applyItr)->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				CEntityProperty* ep = epItr->second;
				if(ep)
				{
					if(ep->GetDBTableName() == "CSL_Faction_Apply")
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
								rs->PutCollect(epItr->first.c_str(), szTimeValue);
							}
							break;
						case  DATA_OBJECT_STRING:
							{
								rs->PutCollect(epItr->first.c_str(), ep->GetStringAttr(0));
							}
							break;
						case DATA_OBJECT_BUFFER:
							{
								SaveBufferField((CBaseDBEntity*)this, epItr->first.c_str(), rs);
							}
							break;
						case DATA_OBJECT_GUID:
							{
								CGUID tGUID;
								ep->GetGuidAttr(0, tGUID);
								char szGUID[128];
								tGUID.tostring(szGUID);
								if(ep->GetDbFlag() == 0) // 0:非主键,1:主键
									rs->PutCollect(epItr->first.c_str(), szGUID);
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

		}
		rs->Update();

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		ReleaseRs(rs);

		bRe = FALSE;
	}

	return bRe;
}

//! 更新（包括新建）一个帮会的留言
BOOL CRsFaction::SaveOneFaction_LeaveWord(CDBFaction* pDBFaction, _ConnectionPtr& cn)
{
	/*
	if( NULL == cn)	return FALSE;
	if( NULL == pDBFaction)	return FALSE;
	BOOL bRe = TRUE;

	_RecordsetPtr rs;

	char szFacGuid[64] = {0};
	pDBFaction->GetExID().tostring(szFacGuid);

	try
	{
		//! 先删除相同ID的数据
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_LeaveWord WHERE FactionGuid='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));

		//! 再添加
		TESTHR(CreateRs(rs));

		if (0 < pDBFaction->m_vFacLeaveWord.size())
		{
			TESTHR(OpenRs("CSL_Faction_LeaveWord", rs, cn, adOpenForwardOnly, adLockOptimistic, adCmdTable));

			char szWordGuid[64] = {0};
			char szMemGuid[64] = {0};	
			for (LONG i = 0; i < pDBFaction->m_vFacLeaveWord.size(); ++i)
			{
				pDBFaction->m_vFacLeaveWord[i].Guid.tostring(szWordGuid);
				pDBFaction->m_vFacLeaveWord[i].MemberGuid.tostring(szMemGuid);

				rs->AddNew();

				rs->PutCollect("FactionGuid",	szFacGuid);
				rs->PutCollect("Guid",			szWordGuid);
				rs->PutCollect("MemberGuid",	szMemGuid);
				rs->PutCollect("Body",			pDBFaction->m_vFacLeaveWord[i].szBody);
				rs->PutCollect("Time",			pDBFaction->m_vFacLeaveWord[i].Time);								
			}

			rs->Update();
		}

		ReleaseRs(rs);
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		ReleaseRs(rs);

		bRe = FALSE;
	}

	return bRe;
	*/
	return TRUE;
}


//! 删除一个帮会基本信息
BOOL CRsFaction::DelOneFaction_BaseInfo(const CGUID &FactionGuid, _ConnectionPtr& cn)
{
	if( NULL_GUID == FactionGuid)	return FALSE;
	char szFacGuid[64] = {0};
	FactionGuid.tostring(szFacGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_BaseProperty WHERE FactionGuid='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		return FALSE;
	}

	return TRUE;
}

//! 删除一个帮会所有成员
BOOL CRsFaction::DelOneFaction_Member(const CGUID &FactionGuid, _ConnectionPtr& cn)
{
	if( NULL_GUID == FactionGuid)	return FALSE;
	char szFacGuid[64] = {0};
	FactionGuid.tostring(szFacGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_Members WHERE FactionGuid_members='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		return FALSE;
	}

	return TRUE;
}

//! 删除一个帮会所有申请者
BOOL CRsFaction::DelOneFaction_Apply(const CGUID &FactionGuid, _ConnectionPtr& cn)
{
	if( NULL_GUID == FactionGuid)	return FALSE;

	char szFacGuid[64] = {0};
	FactionGuid.tostring(szFacGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_Apply WHERE FactionGuid_Apply='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		return FALSE;
	}

	return TRUE;
}

//! 删除一个帮会留言
BOOL CRsFaction::DelOneFaction_LeaveWord(const CGUID &FactionGuid, _ConnectionPtr& cn)
{
	/*游戏取消了功能留言功能*/
	/*
	if( NULL_GUID == FactionGuid)	return FALSE;

	char szFacGuid[64] = {0};
	FactionGuid.tostring(szFacGuid);

	try
	{
		char szSql[512] = {0};
		sprintf(szSql,"DELETE CSL_Faction_LeaveWord WHERE FactionGuid='%s'", szFacGuid);
		TESTHR(ExecuteCn(szSql, cn));
	}
	catch(_com_error &e)
	{
		PrintErr("", e);	
		return FALSE;
	}
	*/

	return TRUE;
}


