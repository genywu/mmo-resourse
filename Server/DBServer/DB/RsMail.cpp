#include "StdAfx.h"
#include "RsMail.h"
#include "..\..\server\dbserver\AppWorld\DBMail.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"
#include "../../server/DBServer/AppWorld/DBAccessThread.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"
#include "..\..\server\DBServer/dbserver/game.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "..\..\server\DBServer\AppWorld\DBMailGoods.h"
#include "..\..\server\DBServer\AppWorld\DBMailGoodsGroup.h"
#include "..\.\server\DBServer\AppWorld\DBMailGroup.h"
#include "../../server/DBServer/AppWorld/ThreadDataDef.h"
#include "../../server/DBServer/AppWorld/DBSaveThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CRsMail::CRsMail(void)
{
}

CRsMail::~CRsMail(void)
{
}
//存Buffer
bool CRsMail::SaveBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pGoods == NULL)	return false;
	if(szFiledName == NULL) return false;

	VARIANT varBLOB;
	SAFEARRAY* psa;

	CEntityProperty* ep = pGoods->GetDataEntityManager().GetEntityProperty(string(szFiledName));
	if(NULL == ep) return false;

	BYTE* buffer = NULL;
	long dataSize = ep->GetBufSize(0);
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
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN, STR_DBS_DUPRGN_SAVERGNOBJFAIL), e);
			SAFE_DELETE_ARRAY(buffer);
			return false;
		}
		SafeArrayDestroy(psa);
	}
	SAFE_DELETE_ARRAY(buffer);
	return true;
}
//读Buffer
bool CRsMail::LoadBufferField(CBaseDBEntity *pGoods, const char* szFiledName, _RecordsetPtr &rs)
{
	if(pGoods == NULL)	return false;

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
			CEntityProperty* ep = pGoods->GetDataEntityManager().GetEntityProperty(string(szFiledName));
			if(ep)
				ep->SetBufAttr(0, pBuf, lSize);
		}
	}
	catch(_com_error e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_LOADRGNOBJERR), e);
		return false;
	}
	return true;
}

//! 读取个人房屋信息
void CRsMail::LoadMail(CDBMail* mail, _RecordsetPtr &rs, _ConnectionPtr& cn)
{
	if(!mail) return;
	if(rs->GetadoEOF()) return;

	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_CNPTRNULL));
		return;
	}

	try
	{
		//!
		CDataEntityManager::EntityPropertyMapItr epItr = mail->GetDataEntityManager().GetEntityPropertyMap().begin();
		for(; epItr != mail->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
		{
			if(rs->GetadoEOF()) break;

			if(epItr->second)
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
						LoadBufferField((CBaseDBEntity*)mail, epItr->first.c_str(), rs);
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
		// 读取房间内物品
		LoadMailGoods(mail->GetExID(), mail->GetGoodsGroupPtr(), cn);
		return;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_LOADONERGNFAIL), e);
		return;
	}

	return;
}
DB_OPERATION_RESULT_TYPE CRsMail::SavePlayerMails(CDBMailGroup* mailGroup, _ConnectionPtr& cn)
{
	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_FINDDUPRGNPROPERTYERR));
		return DB_OPER_RESULT_ERROR;
	}
	if(!mailGroup) return DB_OPER_RESULT_OK;

	_RecordsetPtr rs;
	char sql[1024] = "";
	try
	{
		TESTHR(CreateRs(rs));

		char szGUID[128];
		mailGroup->GetOwnerGUID().tostring(szGUID);
		sprintf(sql, "SELECT * FROM CSL_MAIL WHERE PlayerID='%s'", szGUID);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));

		set<CDBMail*>::iterator itr = mailGroup->GetMailGroupSet().begin();
		for(; itr != mailGroup->GetMailGroupSet().end(); itr++)
		{
			CDBMail* mail = *itr;
			rs->AddNew();
			set<string>::iterator epItr = mail->GetDirtyAttrDataMap().begin();
			for(; epItr != mail->GetDirtyAttrDataMap().end(); epItr++)
			{
				CEntityProperty* ep = mail->GetDataEntityManager().GetEntityProperty(*epItr);
				if(ep)
				{
					if(ep->GetDBTableName() == "CSL_MAIL")
					{
						switch(ep->GetDataType())
						{
						case DATA_OBJECT_TIME:
							{
								char szTimeValue[2048];
								DWORD timeValue[6] = {0};
								ep->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
								if(szTimeValue[0] != 0 && szTimeValue[1] != 0 && szTimeValue[2] != 0
									&& szTimeValue[3] != 0 && szTimeValue[4] != 0 && szTimeValue[5] != 0)
								{
									sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
										timeValue[3], timeValue[4], timeValue[5]);
									rs->PutCollect((*epItr).c_str(), szTimeValue);
								}
							}
							break;
						case  DATA_OBJECT_STRING:
							{
								rs->PutCollect((*epItr).c_str(), ep->GetStringAttr(0));
							}
							break;
						case DATA_OBJECT_BUFFER:
							{
								SaveBufferField((CBaseDBEntity*)mail, (*epItr).c_str(), rs);
							}
							break;
						case DATA_OBJECT_GUID:
							{
								CGUID tGUID;
								ep->GetGuidAttr(0, tGUID);
								char szGUID[128];
								tGUID.tostring(szGUID);
								rs->PutCollect((*epItr).c_str(), szGUID);
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
			SaveMailGoods(mail->GetExID(), mail->GetGoodsGroupPtr()->GetGoodsGroupSet(), cn);
		}

		rs->Update();
		ReleaseRs(rs);
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_SAVERGNOBJFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	return DB_OPER_RESULT_ERROR;
}
//! 保存个人房屋信息
void CRsMail::SaveMail(CDBMail* mail, _ConnectionPtr& cn)
{	
	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_FINDDUPRGNPROPERTYERR));
		return;
	}

	if(!mail) return;

	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
		TESTHR(CreateRs(rs));

		char szGUID[128];
		mail->GetExID().tostring(szGUID);
		
		TESTHR(OpenRs("CSL_MAIL", rs, cn, adOpenForwardOnly, adLockOptimistic));
	
		if(!rs->GetadoEOF()) // 没有记录,创建一个
		{
			ReleaseRs(rs);
			AddLogText("Create mail[%s] Err,RS is NULL!", szGUID);
		}

		bool isNewCreated = false;
		BYTE phNum = rs->GetRecordCount();
		if(phNum == 0xff) // 没有记录,创建一个
		{
			rs->AddNew();
			isNewCreated = true;
		}

		set<string>::iterator epItr = mail->GetDirtyAttrDataMap().begin();
		for(; epItr != mail->GetDirtyAttrDataMap().end(); epItr++)
		{
			CEntityProperty* ep = mail->GetDataEntityManager().GetEntityProperty(*epItr);
			if(ep)
			{
				if(ep->GetDBTableName() == "CSL_MAIL")
				{
					switch(ep->GetDataType())
					{
					case DATA_OBJECT_TIME:
						{
							char szTimeValue[2048];
							DWORD timeValue[6] = {0};
							ep->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
							if(szTimeValue[0] != 0 && szTimeValue[1] != 0 && szTimeValue[2] != 0
								&& szTimeValue[3] != 0 && szTimeValue[4] != 0 && szTimeValue[5] != 0)
							{
								sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
									timeValue[3], timeValue[4], timeValue[5]);
								rs->PutCollect((*epItr).c_str(), szTimeValue);
							}
						}
						break;
					case  DATA_OBJECT_STRING:
						{
							rs->PutCollect((*epItr).c_str(), ep->GetStringAttr(0));
						}
						break;
					case DATA_OBJECT_BUFFER:
						{
							SaveBufferField((CBaseDBEntity*)mail, (*epItr).c_str(), rs);
						}
						break;
					case DATA_OBJECT_GUID:
						{
							CGUID tGUID;
							ep->GetGuidAttr(0, tGUID);
							char szGUID[128];
							tGUID.tostring(szGUID);
							if( (ep->GetDbFlag() == 0) ) // 0:非主键,1:主键
								rs->PutCollect((*epItr).c_str(), szGUID);
							else if(isNewCreated) // 是主键但是是刚创建对象
								rs->PutCollect((*epItr).c_str(), szGUID);
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
		return;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_SAVERGNOBJFAIL), e);
		return;
	}

	return;
}
DB_OPERATION_RESULT_TYPE CRsMail::DeletePlayerMails(const CGUID& playerID, _ConnectionPtr& cn)
{
	_RecordsetPtr rs;
	char sql[1024] = "";
	try
	{
		set<CGUID> guidSet;
		char szGUID[128];
		playerID.tostring(szGUID);

		sprintf(sql, "SELECT * FROM CSL_MAIL WHERE PlayerID='%s'", szGUID);
		TESTHR(CreateRs(rs));
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));
		rs->MoveFirst();
		while(!rs->GetadoEOF())
		{
			CGUID tGUID((_bstr_t)rs->GetCollect("guid"));
			guidSet.insert(tGUID);
			rs->MoveNext();
		}
		ReleaseRs(rs);
		// 删除房间内物品
		DeletePlayerMailsGoods(guidSet, cn);

		sprintf(sql, "DELETE FROM CSL_MAIL WHERE PlayerID='%s'", szGUID);
		// 先删除该个人房屋的成员
		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELDUPRGNPROPERTYERR));
			return DB_OPER_RESULT_ERROR;
		}

		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	return DB_OPER_RESULT_ERROR;
}
//! 删除邮件信息
DB_OPERATION_RESULT_TYPE CRsMail::DeleteMails(CDBMailGroup* goodsGroup, _ConnectionPtr& cn)
{
	_RecordsetPtr rs;
	char* sql = new char[1024* (long)goodsGroup->GetMailGroupSet().size()];
	try
	{
		char szGUID[128];

		sprintf(sql, "DELETE FROM CSL_MAIL WHERE guid='");
		set<CDBMail*>::iterator itr = goodsGroup->GetMailGroupSet().begin();
		for(; itr != goodsGroup->GetMailGroupSet().end(); itr++)
		{
			(*itr)->GetExID().tostring(szGUID);
			strcat(sql, szGUID);
			strcat(sql, "'");
			strcat(sql, " OR guid='");
			// 删除房间内物品
			DeleteMailGoods((*itr)->GetExID());
		}

		long sqlLen = strlen(sql);
		sql[sqlLen-10] = '\0';

		// 先删除该个人房屋的成员
		if(!ExecuteCn(sql, cn))
		{
			SAFE_DELETE_ARRAY(sql);
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELDUPRGNPROPERTYERR));
			return DB_OPER_RESULT_ERROR;
		}
		
		SAFE_DELETE_ARRAY(sql);
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		SAFE_DELETE_ARRAY(sql);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	SAFE_DELETE_ARRAY(sql);
	return DB_OPER_RESULT_ERROR;
}
//! 删除个人房屋信息
void CRsMail::DeleteMail(const CGUID& guid, _ConnectionPtr& cn)
{
	char sql[500] = "";
	try
	{
		char szGUID[128];
		guid.tostring(szGUID);

		sprintf(sql, "DELETE FROM CSL_MAIL WHERE PlayerID='%s'", szGUID);
		// 先删除该个人房屋的成员
		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELDUPRGNPROPERTYERR));
			return;
		}

		// 删除房间内物品
		DeleteMailGoods(guid);
		return;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return;
	}

	return;
}
//! 读取个人房屋内的物品信息
DB_OPERATION_RESULT_TYPE CRsMail::LoadMailGoods(const CGUID& rgnGUID, CDBMailGoodsGroup* goodsGroup, _ConnectionPtr& cn)
{
	if(!goodsGroup) return DB_OPER_RESULT_OK;

	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_CNPTRNULL));
		return DB_OPER_RESULT_ERROR;
	}

	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
		TESTHR(CreateRs(rs));

		char szGUID[128];
		rgnGUID.tostring(szGUID);

		// 先查找该个人房屋的成员
		sprintf(sql, "SELECT * FROM CSL_MAIL_GOODS WHERE MailID='%s'", szGUID);
		TESTHR(OpenRs(sql, rs, cn));

		while(!rs->GetadoEOF())
		{
			// 基本属性部分
			_variant_t vGoodsID = rs->GetCollect("goodsID");
			CGUID guid = CGUID(((char*)(_bstr_t)vGoodsID));

			CDBMailGoods* pGoods = new CDBMailGoods(guid);
			CDataEntityManager::EntityPropertyMapItr epItr = pGoods->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pGoods->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
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
						LoadBufferField((CBaseDBEntity*)pGoods, epItr->first.c_str(), rs);
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


			goodsGroup->GetGoodsGroupSet().insert(pGoods);
			// 下条记录
			rs->MoveNext();
		}

		ReleaseRs(rs);
		
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_LOADRGNGOODSFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	return DB_OPER_RESULT_OK;
}
//! 保存个人房屋内的物品信息
DB_OPERATION_RESULT_TYPE    CRsMail::SaveMailGoods(const CGUID& ownerID, set<CDBMailGoods*>& goodsGroup, _ConnectionPtr& cn)
{

	if(cn == NULL)
	{
		AddLogText(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_CNPTRNULL));
		return DB_OPER_RESULT_ERROR;
	}

	_RecordsetPtr rs;
	char sql[1024] = "";
	string iniName   = "mailgoods";
	string tableName = "CSL_MAIL_GOODS";
	try
	{
		TESTHR(CreateRs(rs));

		char szGUID[128];
		ownerID.tostring(szGUID);

		sprintf(sql, "DELETE FROM CSL_MAIL_GOODS WHERE MailID='%s'", szGUID);
		// 先删除该个人房屋的成员
		if(!ExecuteCn(sql, cn))
		{
			PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL));
			return DB_OPER_RESULT_ERROR;
		}

		TESTHR(CreateRs(rs));
		// 先查找该个人房屋的成员
		sprintf(sql, "SELECT * FROM CSL_MAIL_GOODS WHERE MailID='%s'", szGUID);
		TESTHR(OpenRs(sql, rs, cn, adOpenForwardOnly, adLockOptimistic));		

		set<CDBMailGoods*>::iterator goodsItr = goodsGroup.begin();
		for(; goodsItr != goodsGroup.end(); goodsItr++)
		{	
			rs->AddNew();
			//! 生成语句身
			CDataEntityManager::EntityPropertyMapItr attrItr = (*goodsItr)->GetDataEntityManager().GetEntityPropertyMap().begin();
			for (; attrItr != (*goodsItr)->GetDataEntityManager().GetEntityPropertyMap().end(); attrItr++)
			{
				if(attrItr->second)
				{
					// 添加属性字段值
					CEntityProperty* ep = attrItr->second; 
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
									rs->PutCollect(attrItr->first.c_str(), szTimeValue);
								}
								break;
							case  DATA_OBJECT_STRING:
								{
									rs->PutCollect(attrItr->first.c_str(), ep->GetStringAttr(0));
								}
								break;
							case DATA_OBJECT_BUFFER:
								{
									SaveBufferField((*goodsItr), attrItr->first.c_str(), rs);
								}
								break;
							case DATA_OBJECT_GUID:
								{
									CGUID tGUID;
									ep->GetGuidAttr(0, tGUID);
									char szGuid[128];
									tGUID.tostring(szGuid);
									rs->PutCollect(attrItr->first.c_str(), szGuid);
								}
								break;
							case DATA_OBJECT_BOOL:
							case DATA_OBJECT_CHAR:
							case DATA_OBJECT_BYTE:
								{
									rs->PutCollect(attrItr->first.c_str(), (BYTE)ep->GetLongAttr(0));
								}
								break;
							case DATA_OBJECT_SHORT:
							case DATA_OBJECT_WORD:
							case DATA_OBJECT_USHORT:
								{
									rs->PutCollect(attrItr->first.c_str(), (WORD)ep->GetLongAttr(0));
								}
								break;
							case DATA_OBJECT_FLOAT:
							case DATA_OBJECT_LONG:
							case DATA_OBJECT_ULONG:
							case DATA_OBJECT_DWORD:
								{
									rs->PutCollect(attrItr->first.c_str(), (DWORD)ep->GetLongAttr(0));
								}
								break;
							}
						}
					}
					
				}
			}
		}
		
		if(goodsGroup.size())
			rs->Update();
		return DB_OPER_RESULT_OK;
	}
	catch (_com_error &e)
	{
		ReleaseRs(rs);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_SAVERGNGOODSFAIL), e);
		return DB_OPER_RESULT_ERROR;
	}

	return DB_OPER_RESULT_ERROR;
}
bool CRsMail::DeletePlayerMailsGoods(set<CGUID>& mailSet, _ConnectionPtr& cn)
{
	char* sql = new char[256*mailSet.size()];
	sprintf(sql, "DELETE FROM CSL_MAIL_GOODS WHERE guid='");
	try
	{
		set<CGUID>::iterator itr = mailSet.begin();
		for(; itr != mailSet.end(); itr++)
		{
			char szGUID[128];
			(*itr).tostring(szGUID);
			strcat(sql, szGUID);
			strcat(sql, "' OR '");
		}

		long sqlLen = strlen(sql);
		sql[sqlLen-4] = '\0';
		// 先删除该个人房屋的成员
		TESTHR(ExecuteCn(sql, cn));
		SAFE_DELETE_ARRAY(sql);
		return true;
	}
	catch (_com_error &e)
	{
		SAFE_DELETE_ARRAY(sql);
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return false;
	}

	return false;
}
//! 删除个人房屋内的物品信息
void    CRsMail::DeleteMailGoods(const CGUID& guid)
{
	char sql[500] = "";
	try
	{
		char szGUID[128];
		guid.tostring(szGUID);
		sprintf(sql, "DELETE FROM CSL_MAIL_GOODS WHERE MailID='%s'", szGUID);
		// 先删除该个人房屋的成员
		TESTHR(ExecuteCn(sql, GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr()));
		return;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		return;
	}

	return;
}
//! 读取玩家的所有邮件信息
bool CRsMail::LoadPlayerMails(const CGUID& playerID, CDBMailGroup* mailGroup, _ConnectionPtr& cn)
{
	if(!mailGroup) return true;

	mailGroup->Clear();

	mailGroup->SetOwnerGUID(playerID);

	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
		TESTHR(CreateRs(rs));

		char szGUID[128];
		playerID.tostring(szGUID);

		sprintf(sql, "select * FROM CSL_MAIL WHERE PlayerID='%s'", szGUID);
		// 先删除该个人房屋的成员
		TESTHR(OpenRs(sql, rs, cn));

		while(!rs->GetadoEOF())
		{
			CGUID tGuid((_bstr_t)rs->GetCollect("guid"));
			CDBMail* mail = new CDBMail(tGuid);
			LoadMail(mail, rs, cn);
			mailGroup->GetMailGroupSet().insert(mail);
			rs->MoveNext();
		}
		ReleaseRs(rs);
		return true;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		ReleaseRs(rs);
		return false;
	}
	ReleaseRs(rs);
	return false;
}
//! 查询邮件
bool CRsMail::QueryPlayerMails(long curTime, CDBMailGroup* mailGroup, _ConnectionPtr& cn)
{
	if(!mailGroup) return true;

	mailGroup->Clear();

	_RecordsetPtr rs;
	char sql[500] = "";
	try
	{
		TESTHR(CreateRs(rs));

		sprintf(sql, "select * FROM CSL_MAIL WHERE Date<=%d", curTime);
		// 先删除该个人房屋的成员
		TESTHR(OpenRs(sql, rs, cn));

		while(!rs->GetadoEOF())
		{
			CGUID tGuid((_bstr_t)rs->GetCollect("guid"));
			CDBMail* mail = new CDBMail(tGuid);
			LoadMail(mail, rs, cn);
			mailGroup->GetMailGroupSet().insert(mail);
			rs->MoveNext();
		}
		ReleaseRs(rs);
		return true;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		ReleaseRs(rs);
		return false;
	}
	ReleaseRs(rs);
	return false;
}
//! 数据库里添加系统信件
void CRsMail::InsertSysMailToGroup(CDBMail* pMail, MAIL_LEVEL_OPER_FLAG levelOpFlag, long level, long sex, long* countryIDArray, long countryNum, _ConnectionPtr& cn)
{
	if(!pMail || cn == NULL || !countryIDArray) return;

	// 产生sql语句
	char sql[1024];
	_RecordsetPtr rs;
	_RecordsetPtr mailRs;
	try
	{
		// 记录收信人ID
		vector<CGUID> vecRecieverID;

		TESTHR(CreateRs(rs));
		sprintf(sql, "select * FROM baseproperty WHERE ");
		if(level != -1)
		{
			if(levelOpFlag == MAIL_LEVEL_GREATANDEQUAL)
				strcat(sql, "Levels >= ");
			else if(levelOpFlag == MAIL_LEVEL_LESSANDEQUAL)
				strcat(sql, "Levels <= ");

			char szNum[128];
			itoa(level, szNum, 10);
			strcat(sql, szNum);
		}
		else if(sex != -1)
		{
			if( (sex != -1))
				strcat(sql, ", AND ");

			strcat(sql, "Sex = ");
			char szNum[128];
			itoa(sex, szNum, 10);
			strcat(sql, szNum);
		}
		else if(countryIDArray != NULL && countryNum != 0)
		{
			if( (sex != -1) || (level != -1))
				strcat(sql, ", AND ");

			for(int i=0; i<countryNum; i++)
			{
				strcat(sql, "(Country = ");
				char szNum[128];
				itoa(countryIDArray[i], szNum, 10);
				strcat(sql, szNum);
				strcat(sql, ", OR ");
			}
			long sqlLen = strlen(sql);
			sql[sqlLen-5] = '\0';
			strcat(sql, ")");
		}

		TESTHR(OpenRs(sql, rs, cn));
		while(!rs->GetadoEOF())
		{
			CGUID tGuid((_bstr_t)rs->GetCollect("guid"));
			vecRecieverID.push_back(tGuid);
			rs->MoveNext();
		}
		ReleaseRs(rs);

		TESTHR(CreateRs(mailRs));
		sprintf(sql, "select * FROM CSL_MAIL WHERE PlayerID='{00000000-0000-0000-0000-000000000000}'");
		TESTHR(OpenRs(sql, mailRs, cn));
		for(int i=0; i<vecRecieverID.size(); i++)
		{
			rs->AddNew();
			CDataEntityManager::EntityPropertyMapItr epItr = pMail->GetDataEntityManager().GetEntityPropertyMap().begin();
			for(; epItr != pMail->GetDataEntityManager().GetEntityPropertyMap().end(); epItr++)
			{
				CEntityProperty* ep = epItr->second;
				if(ep)
				{
					if(ep->GetDBTableName() == "CSL_MAIL")
					{
						switch(ep->GetDataType())
						{
						case DATA_OBJECT_TIME:
							{
								char szTimeValue[2048];
								DWORD timeValue[6] = {0};
								ep->GetBufAttr(0, (void*)timeValue, 6*sizeof(DWORD));
								if(szTimeValue[0] != 0 && szTimeValue[1] != 0 && szTimeValue[2] != 0
									&& szTimeValue[3] != 0 && szTimeValue[4] != 0 && szTimeValue[5] != 0)
								{
									sprintf(szTimeValue, "%d-%d-%d %d:%d:%d", timeValue[0], timeValue[1], timeValue[2],
										timeValue[3], timeValue[4], timeValue[5]);
									rs->PutCollect(epItr->first.c_str(), szTimeValue);
								}
							}
							break;
						case  DATA_OBJECT_STRING:
							{
								rs->PutCollect(epItr->first.c_str(), ep->GetStringAttr(0));
							}
							break;
						case DATA_OBJECT_BUFFER:
							{
								SaveBufferField((CBaseDBEntity*)pMail, epItr->first.c_str(), rs);
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
		}
		if(vecRecieverID.size())
			mailRs->Update();
		ReleaseRs(mailRs);
		return;
	}
	catch (_com_error &e)
	{
		PrintErr(CStringReading::LoadString(IDS_DBS_RSDUPRGN,STR_DBS_DUPRGN_DELRGNOBJFAIL), e);
		ReleaseRs(rs);
		ReleaseRs(mailRs);
		return;
	}
	ReleaseRs(rs);
	ReleaseRs(mailRs);
	return;
}