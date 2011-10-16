/**
*File:  DBThread.cpp
*brief: 该对象维护一个数据库操作使用的线程,所有操作都在线程内完成
*Author:安海川
*Date:  2009.02.17
*/
#include "stdafx.h"
#include "DBThread.h"

#include "DBAdoBase.h"
#include "ThreadDataDef.h"

#include "../Entity/EntityGroup.h"
#include "../Entity/EntityManager.h"
#include "../../DB/RsEntity.h"

#include "../../../../Public/AppFrame/TextRes.h"
using namespace AppFrame;

//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

extern DWORD g_dwThisSaveStartTime;			// 本次存盘开始时间
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DBThread::DBThread(void)
:m_pConnPtr(NULL),m_lThreadID(0),m_hQuitWait(0)
{
	
}
DBThread::DBThread(string strProvider, 
						 string strDataSource, 
						 string strInitialCatalog,
						 string strUserID, 
						 string strPassword,
						 eThreadType type)
						 :m_pConnPtr(NULL),m_lThreadID(0),m_hQuitWait(0),m_ThreadType(eThreadInit)
{
	m_pConnPtr = new DBAdoBase(string(	"Provider=" + strProvider +
									"; Server=" + strDataSource + 
									"; Database=" + strInitialCatalog + 
									"; UID=" + strUserID + 
									"; PWD=" + strPassword + 
									"; OLE DB Services=-1; Driver={SQL Server}"
									));
	m_strInitialCatalog = strInitialCatalog;
	m_hQuitWait = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_hQuitWait);
	GetGame()->GetEntityManager()->PushThreadQuitEventHandle(m_hQuitWait);
	m_ThreadType = type;
}

//-----------------------------------------------------------------------------------------------------------------
DBThread::~DBThread(void)
{
	Exit();
	delete m_pConnPtr;
}

bool DBThread::OpenCn(void)
{
	return m_pConnPtr->OpenCn();
}

//-----------------------------------------------------------------------------------------------------------------
bool DBThread::Begin(void)
{
	_beginthreadex(NULL, 0, RunThread, (void*)this, 0, &m_lThreadID);
	return true;
}


//-----------------------------------------------------------------------------------------------------------------
void DBThread::Exit(void)
{
	GetAdoBasePtr()->CloseCn();
}
//-----------------------------------------------------------------------------------------------------------------
bool DBThread::ProcessData(void)
{
	bool ret = true;
	// 拷贝所有元素到临时变量数据区域
	ThreadData* tempSaveThreadData = NULL;
	GetGame()->GetEntityManager()->GetThreadData(&tempSaveThreadData, GetThreadID(), m_ThreadType);

	if(tempSaveThreadData)
	{
		if( tempSaveThreadData->retFlag == -99999999 )// 传入一个值表示线程退出
		{
			SAFE_DELETE(tempSaveThreadData);
			return false;
		}

		// 记录操作类型
		tempSaveThreadData->lThreadOpType = (long)m_ThreadType;

		CEntityGroup* pBaseEntity = (CEntityGroup*)tempSaveThreadData->pEntity;
		if(pBaseEntity)
		{
			long threadTime = timeGetTime();
#ifdef _DEBUG_SM_INFO_
			char szGuid[128];
			char szOutPutStr[1024];
			pBaseEntity->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "==TID:%d %s %s", GetThreadID(), szGuid, pBaseEntity->GetCompositeFlag().c_str());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szOutPutStr);
			
			pBaseEntity->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "--ThreadID:%d TravelRootDbOper() Start:dbObj->GetExID[%s]--",GetThreadID(), szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			if(pBaseEntity->GetCurDbOperType() == DB_OPER_SAVE)
			{
				if(pBaseEntity->GetCompositeFlag() == "[player]")
					GetGame()->GetEntityManager()->GenerateQuerySavePlayerInfo(pBaseEntity);
			}
			HRESULT hRet = TravelRootDbOper(pBaseEntity, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "--ThreadID:%d TravelRootDbOper() End:dbObj->GetExID[%s]--",GetThreadID(), szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			tempSaveThreadData->retFlag = (BYTE)hRet;
			ulong lviewTime = timeGetTime() - threadTime;
			if(GetGame()->GetSetup()->dwViewDbOperDeltaTime <= lviewTime)
			{
				char szGuid[128];
				pBaseEntity->GetGUID().tostring(szGuid);
				Log4c::Trace(ROOT_MODULE,"%s",FormatText("DB_DBTHREAD_1", //线程ID[%d]:耗时:%d(ms),操作类型[%d],对象类型[%s],对象ID[%s].
					GetThreadID(), lviewTime, (long)pBaseEntity->GetCurDbOperType(), pBaseEntity->GetCompositeFlag().c_str(), szGuid));
			}
			g_dwThisSaveStartTime = lviewTime;

			// 捕获数据库操作异常，到主线程处理
			if(hRet != S_OK)
			{
				GetGame()->GetEntityManager()->PushRetThreadData(tempSaveThreadData);
				return true;
			}
		}
		else
		{
			Log4c::Warn(ROOT_MODULE,GetText("DB_DBTHREAD_3"));//ProcData entity is Null!
			SAFE_DELETE(tempSaveThreadData);
			return true;
		}

		GetGame()->GetEntityManager()->PopBeginEntityGuid(pBaseEntity->GetGUID(), m_ThreadType);

		if( (pBaseEntity->GetCurDbOperType() == DB_OPER_SAVE && pBaseEntity->GetDelLeafGuid() == NULL_GUID)// 删除叶子节点需要返回
			|| pBaseEntity->GetCurDbOperType() == DB_OPER_INSERT
			|| pBaseEntity->GetCurDbOperType() == DB_OPER_DELETE_INSERT
			|| (pBaseEntity->GetIsTempEntityFlag() && pBaseEntity->GetCurDbOperType() != DB_OPER_LOAD 
				&& pBaseEntity->GetCurDbOperType() != DB_OPER_CREATEROLE_PROCEDURE
				&& pBaseEntity->GetCurDbOperType() != DB_OPER_CHANGENAME_PROCEDURE
				&& pBaseEntity->GetCurDbOperType() != DB_OPER_PROCEDURE) ) // 如果时临时对象则在此释放
		{
			SAFE_DELETE(tempSaveThreadData);
		}
		else
			GetGame()->GetEntityManager()->PushRetThreadData(tempSaveThreadData);
	}
	return ret;
}
//!								执行一次数据库操作
HRESULT DBThread::DbOperEntityAttrs(CEntityGroup* pRoot, _ConnectionPtr& cn)
{
	if(pRoot == NULL) return E_FAIL;
	if(cn == NULL)
	{
		GetAdoBasePtr()->OpenCn();
		return E_FAIL;
	}
	HRESULT hRet = S_OK;
	switch((DATA_OBJECT_TYPE)pRoot->GetCurDbOperType())
	{
	case DB_OPER_SAVE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "SaveEntityAttrsByRs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::SaveEntityAttrsByRs(pRoot, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperEntityGroups(pRoot, DB_OPER_SAVE, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "SaveEntityAttrsByRs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "InsertEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::InsertEntityGroupAttrs(pRoot, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperEntityGroups(pRoot, DB_OPER_INSERT, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "InsertEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_LOAD:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "LoadEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::LoadEntityAttrs(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "LoadEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DeleteEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::DeleteEntityGroupAttrs(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DeleteInsertEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::DeleteInsertEntityGroupAttrs(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteInsertEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_ONLY_MEMSAVE:
		return S_OK;
	}
	return hRet;
}
HRESULT DBThread::DbOperAllEntityGroup(CEntityGroup* pRoot, _ConnectionPtr& cn)
{
	if(pRoot == NULL) return E_FAIL;
	if(cn == NULL)
	{
		GetAdoBasePtr()->OpenCn();
		return E_FAIL;
	}
	
	HRESULT hRet = S_OK;	

	switch((DB_OPERATION_TYPE)pRoot->GetCurDbOperType())
	{
	case DB_OPER_SAVE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "SaveAllEntityGroupByRs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::SaveAllEntityGroupByRs(pRoot, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperAllEntityGroups(pRoot, DB_OPER_SAVE, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "SaveAllEntityGroupByRs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "InsertAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::InsertAllEntityGroup(pRoot, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperAllEntityGroups(pRoot, DB_OPER_INSERT, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "InsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_LOAD:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "LoadAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::LoadAllEntityGroup(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "LoadAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DeleteEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			//hRet = CRsEntity::DeleteEntityAttrs(pRoot, GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DeleteInsertAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::DeleteInsertAllEntityGroup(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteInsertAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_ONLY_MEMSAVE:
		return S_OK;
	}
	return hRet;
}
HRESULT DBThread::DbOperLeaves(DB_OPERATION_TYPE dbOperType, const char* szSql, const char* szTblName, 
							   const string& strComFlag, map<CGUID, CBaseEntity*>& entityMap, 
							   CEntityGroup* pRoot, _ConnectionPtr& cn)
{
	if(cn == NULL)
	{
		GetAdoBasePtr()->OpenCn();
		return E_FAIL;
	}

	HRESULT hRet = S_OK;

	switch(dbOperType)
	{
	case DB_OPER_SAVE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "SaveLeavesByRs() Start:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::SaveLeavesByRs(szSql, strComFlag, entityMap, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperLeaves(szSql, strComFlag.c_str(), entityMap, DB_OPER_SAVE, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "SaveLeavesByRs() End:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "InsertLeaves() Start:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		//	hRet = CRsEntity::InsertLeaves(szSql, strComFlag, entityMap, GetAdoBasePtr()->GetConnPtr());
            hRet = CRsEntity::OperLeaves(szSql, strComFlag.c_str(), entityMap, DB_OPER_INSERT, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "InsertLeaves() End:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_LOAD:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "LoadEntitys() Start:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::LoadEntitys(szSql, strComFlag, entityMap, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "LoadEntitys() End:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "DeleteEntitys() Start:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::DeleteEntitys(szSql, szTblName, strComFlag, entityMap, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteEntitys() End:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_DELETE_INSERT:
		{
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "DeleteInsertLeaves() Start:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			hRet = CRsEntity::DeleteInsertLeaves(szSql, strComFlag, entityMap, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DeleteInsertLeaves() End:dbObj->GetExID[%s].",szSql);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		}
		break;
	case DB_OPER_ONLY_MEMSAVE:
		return S_OK;
	}
	return hRet;
}
//!								递归执行根对象的数据库操作
HRESULT	DBThread::TravelRootDbOper(CEntityGroup* pRoot, _ConnectionPtr& cn)
{
	if(pRoot == NULL) return E_FAIL;
	if(cn == NULL)
	{
		GetAdoBasePtr()->OpenCn();
		cn = GetAdoBasePtr()->GetConnPtr();
//		return E_FAIL;
	}

	HRESULT opResult = S_OK;	

	if(pRoot->GetCompositeType() == COM_LEAF)
	{
		if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
		char szOutPutStr[1024];
		char szGuid[128];
		pRoot->GetGUID().tostring(szGuid);
		sprintf(szOutPutStr, "DbOperEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		opResult = DbOperEntityAttrs(pRoot, cn);
#ifdef _DEBUG_SM_INFO_
		sprintf(szOutPutStr, "DbOperEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
		return opResult;
	}
	else// 递归执行叶子节点操作
	{
		// 执行根结点操作
		switch((DB_OPERATION_TYPE)pRoot->GetCurDbOperType())
		{
		case DB_OPER_INSERTMAIL_PROCEDURE:
			{
				if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
				char szOutPutStr[1024];
				char szGuid[128];
				pRoot->GetGUID().tostring(szGuid);
				sprintf(szOutPutStr, "InserMailProcedureEntity() Start:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				opResult = CRsEntity::InserMailProcedureEntity(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
				sprintf(szOutPutStr, "InserMailProcedureEntity() End:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				return opResult;
			}
			break;
		case DB_OPER_CREATEROLE_PROCEDURE:
			{
				if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
				char szOutPutStr[1024];
				char szGuid[128];
				pRoot->GetGUID().tostring(szGuid);
				sprintf(szOutPutStr, "CreateRoleProcedureEntity() Start:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				opResult = CRsEntity::CreateRoleProcedureEntity(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
				sprintf(szOutPutStr, "CreateRoleProcedureEntity() End:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				if(opResult != S_OK)
					return opResult;
			}
			break;
		case DB_OPER_CHANGENAME_PROCEDURE:
		case DB_OPER_PROCEDURE:
			{
				if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
				char szOutPutStr[1024];
				char szGuid[128];
				pRoot->GetGUID().tostring(szGuid);
				sprintf(szOutPutStr, "ProcedureEntity() Start:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				opResult = CRsEntity::ProcedureEntity(pRoot, GetAdoBasePtr()->GetConnPtr());
#ifdef _DEBUG_SM_INFO_
				sprintf(szOutPutStr, "ProcedureEntity() End:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				if(opResult != S_OK)
					return opResult;
			}
			break;
		}

		if(pRoot->GetHasAttrFlag())
		{
			if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DbOperEntityAttrs() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			opResult = DbOperEntityAttrs(pRoot, cn);
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DbOperEntityAttrs() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			if(opResult != S_OK)
				return opResult;
		}

		if(((CEntityGroup*)pRoot)->GetHasDetailLeavesFlag())// 其CEntityGroup对象全部为该对象的子类型(GoodsGroup,EquipmentGroup...)
		{
			if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DbOperAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			opResult = DbOperAllEntityGroup(pRoot, cn);
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DbOperAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			if(opResult != S_OK)
				return opResult;
		}
		else if(((CEntityGroup*)pRoot)->GetLeafComType() == COM_LEAF) // 其叶子全为Entity
		{
			// 创建叶子节点
			if(!pRoot->GetCurDbOperFlag()) return opResult;
			char szSql[5120];
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			char szGuid[128];
			pRoot->GetGUID().tostring(szGuid);
			sprintf(szOutPutStr, "DbOperLeaves() Start:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			string strTbl = "";
			tagEntityBuildInfo* pInfo = GetGame()->GetEntityManager()->GetEntityBuildInfo(pRoot->GetCompositeFlag());
			if(pInfo)
				strTbl = pInfo->strTblName;

			CRsEntity::GenerateSqlString(szSql, 5120, (DB_OPERATION_TYPE)pRoot->GetCurDbOperType(), pRoot);
			opResult = DbOperLeaves((DB_OPERATION_TYPE)pRoot->GetCurDbOperType(), szSql, strTbl.c_str(), ((CEntityGroup*)pRoot)->GetLeafComFlag(), 
				((CEntityGroup*)pRoot)->GetEntityGroupMap(), pRoot, cn);
#ifdef _DEBUG_SM_INFO_
			sprintf(szOutPutStr, "DbOperLeaves() End:dbObj->GetExID[%s].",szGuid);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
			if(opResult != S_OK)
				return opResult;
		}	
		else // COM_COMPOSITE,叶子节点有多种CEntityGroup对象
		{
			if(((CEntityGroup*)pRoot)->GetEntityGroupMap().size() == 0)
			{
				if(!pRoot->GetCurDbOperFlag()) return opResult;
#ifdef _DEBUG_SM_INFO_
				char szOutPutStr[1024];
				char szGuid[128];
				pRoot->GetGUID().tostring(szGuid);
				sprintf(szOutPutStr, "DbOperAllEntityGroup() Start:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				opResult = DbOperAllEntityGroup(pRoot, cn);
#ifdef _DEBUG_SM_INFO_
				sprintf(szOutPutStr, "DbOperAllEntityGroup() End:dbObj->GetExID[%s].",szGuid);
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),szOutPutStr);
#endif
				if(opResult == S_OK)
				{
					map<CGUID, CBaseEntity*>::iterator itr = ((CEntityGroup*)pRoot)->GetEntityGroupMap().begin();
					for(; itr != ((CEntityGroup*)pRoot)->GetEntityGroupMap().end(); itr++)
					{
						opResult = TravelRootDbOper((CEntityGroup*)itr->second, cn);
					}
				}
				else
					return opResult;
			}
			else
			{
				map<CGUID, CBaseEntity*>::iterator itr = ((CEntityGroup*)pRoot)->GetEntityGroupMap().begin();
				for(; itr != ((CEntityGroup*)pRoot)->GetEntityGroupMap().end(); itr++)
				{
					opResult = TravelRootDbOper((CEntityGroup*)itr->second, cn);
					if(opResult != S_OK)
						return opResult;
				}
			}
		}
	}


	return S_OK;
}
DBAdoBase* DBThread::GetAdoBasePtr(void)
{
	return m_pConnPtr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned __stdcall DBThread::RunThread(void *pThis)
{
#ifndef _DEBUG
	__try
	{
#endif
	//	HRESULT hr = ::CoInitializeEx( NULL, COINIT_MULTITHREADED );
		DBThread *pSaveThread = (DBThread*)pThis;
		if (NULL != pSaveThread)
		{
			pSaveThread->GetAdoBasePtr()->OpenCn();
			while(pSaveThread->ProcessData())
			{

			}
#ifdef _DEBUG_SM_INFO_
			char szOutPutStr[1024];
			sprintf(szOutPutStr, "==Thread ID:%d has Exited！==", pSaveThread->GetThreadID());
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szOutPutStr);
#endif
			pSaveThread->GetAdoBasePtr()->CloseCn();
			SetEvent(pSaveThread->GetQuitWaitHandle());
		}
	//	::CoUninitialize();

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
       Log4c::Warn(ROOT_MODULE,GetText("DB_DBTHREAD_2"));//程序出现错误，请查看最新的错误报告文件
		_endthreadex( 0 );
		return 0;
	}
#endif

	_endthreadex(0);
	return 0;
}
