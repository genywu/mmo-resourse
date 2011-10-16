#include "stdafx.h"
#include "ThreadDataDef.h"
#include "DBSaveThread.h"
#include "DBAdoBase.h"
#include <process.h>
#include "DBentityManager.h"
#include "../dbaccess/WorldDB/RSGoods.h"
#include "../dbaccess/WorldDB/RsPlayer.h"
#include "../dbaccess/WorldDB/RsDupRgn.h"
#include "../dbaccess/WorldDB/RsRegion.h"
#include "../dbaccess/WorldDB/RsFaction.h"
#include "../dbaccess/WorldDB/RsMail.h"
#include "../dbaccess/WorldDB/RsGenVar.h"
#include "DBPlayer.h"
#include "DBLoginPlayer.h"
#include "DBDupRgn.h"
#include "DBRgnGoods.h"
#include "DBEntityDef.h"
#include "ThreadDataDef.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "DBGoodsGroup.h"
#include "DBRgnGoodsGroup.h"
#include "DBLinkManGroup.h"
#include "DBMail.h"
#include "DBMailGoods.h"
#include "DBMailGroup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern long lSavePlayers;
extern long lCurSaveThreadQueueSize;
extern SYSTEMTIME g_stLastSaveTime;			// 上次成功存盘的时间
extern DWORD g_dwThisSaveStartTime;			// 本次存盘开始时间
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DBSaveThread::DBSaveThread(string strProvider, 
						 string strDataSource, 
						 string strInitialCatalog,
						 string strUserID, 
						 string strPassword
						 )
:
m_SaveRunning(false), 
m_hSaveWaitThread(0)
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
}

//-----------------------------------------------------------------------------------------------------------------
DBSaveThread::~DBSaveThread(void)
{
	Exit();
	delete m_pConnPtr;
}

_ConnectionPtr DBSaveThread::GetConnPtr(void)
{
	return m_pConnPtr->GetConnPtr();
}
bool DBSaveThread::OpenCn(void)
{
	return m_pConnPtr->OpenCn();
}

//-----------------------------------------------------------------------------------------------------------------
bool DBSaveThread::Begin(void)
{
	if (0 != m_hSaveWaitThread)
		return false;

	m_SaveRunning	= true;

	if(0 == (m_hSaveWaitThread = (HANDLE)_beginthreadex(NULL, 0, RunThread, (void*)this, 0, &m_lThreadID)))
	{	
		m_SaveRunning	= false;
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------------------------------------------
void DBSaveThread::Exit(void)
{
	if (0 == m_hSaveWaitThread)
		return;

	m_hSaveWaitThread = 0;
}
//-----------------------------------------------------------------------------------------------------------------
bool DBSaveThread::ProcessData(void)
{
	bool ret = true;
	// 拷贝所有元素到临时变量数据区域
	list<SaveThreadData*> tempSaveThreadDataList;
	GetGame()->GetDBEntityManager()->PopSaveThreadData(tempSaveThreadDataList);

	GetSystemTime(&g_stLastSaveTime);
	g_dwThisSaveStartTime = timeGetTime();

	char pszLogingInfo[512]="";
	_snprintf(pszLogingInfo,512,"保存线程正在处理 [%d] 个Save数据...\r\n", tempSaveThreadDataList.size());
	PutStringToFile("Login_DBS_Info",pszLogingInfo);
	
	lCurSaveThreadQueueSize = tempSaveThreadDataList.size();
	SaveThreadData* firstSaveData = *(tempSaveThreadDataList.begin());

	if(firstSaveData && firstSaveData->dbObj != NULL && firstSaveData->saveObj != NULL)
		AddLogText(pszLogingInfo);

	list<SaveThreadData*>::iterator saveDataItr = tempSaveThreadDataList.begin();
	for(; saveDataItr != tempSaveThreadDataList.end(); saveDataItr++)
	{
		if(*saveDataItr && (*saveDataItr)->dbObj && (*saveDataItr)->saveObj)
		{
			// 执行保存操作
			switch((*saveDataItr)->saveObj->objType)
			{
			case DB_GENVARGROUP:
				{
					DB_OPERATION_RESULT_TYPE opResult = DB_OPER_RESULT_ERROR;
					if((*saveDataItr)->saveObj->opType == DB_OPER_DELETE) // 需要删除的对象
						opResult = GetGame()->GetRsGenVar()->Del(((CDBGenVarGroup*)(*saveDataItr)->dbObj), GetConnPtr());
					else if((*saveDataItr)->saveObj->opType == DB_OPER_INSERT) // 需要保存的对象
						opResult = GetGame()->GetRsGenVar()->Add( ((CDBGenVarGroup*)(*saveDataItr)->dbObj), GetConnPtr());
					else if((*saveDataItr)->saveObj->opType == DB_OPER_SAVE)
						opResult = GetGame()->GetRsGenVar()->Save( ((CDBGenVarGroup*)(*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_MAIL:
				{
					if((*saveDataItr)->saveObj->opType == DB_OPER_DELETE) // 需要删除的对象
						GetGame()->GetRsMail()->DeleteMail((*saveDataItr)->saveObj->objGUID, GetConnPtr());
					else if((*saveDataItr)->saveObj->opType == DB_OPER_INSERT) // 需要保存的对象
						GetGame()->GetRsMail()->SaveMail(((CDBMail*)(*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_MAILGROUP:
				{
					DB_OPERATION_RESULT_TYPE opResult = DB_OPER_RESULT_ERROR;
					if((*saveDataItr)->saveObj->opType == DB_OPER_DELETE) // 需要删除的对象
						opResult = GetGame()->GetRsMail()->DeleteMails(((CDBMailGroup*)(*saveDataItr)->dbObj), GetConnPtr());
					else if((*saveDataItr)->saveObj->opType == DB_OPER_INSERT || (*saveDataItr)->saveObj->opType == DB_OPER_SAVE) // 需要保存的对象
						opResult = GetGame()->GetRsMail()->SavePlayerMails( ((CDBMailGroup*)(*saveDataItr)->dbObj), GetConnPtr());
					SaveThreadData* DisposedSaveData = new SaveThreadData(((CDBMailGroup*)(*saveDataItr)->dbObj)->GetOwnerGUID(), DB_MAILGROUP);
					*(DisposedSaveData->saveObj) = *((*saveDataItr)->saveObj);
					*((CDBMailGroup*)(DisposedSaveData->dbObj)) = *((CDBMailGroup*)((*saveDataItr)->dbObj));
					DisposedSaveData->saveObj->opResultType = opResult;
					GetGame()->GetDBEntityManager()->PushDisposedSaveThreadData(DisposedSaveData);
				}
				break;
			case DB_FACTIONAPPLYGROUP:
				{
					GetGame()->GetRsFaction()->SaveOneFaction_Apply(((CDBFactionApplyGroup*)(*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_FACTIONMEMBERGROUP:
				{
					GetGame()->GetRsFaction()->SaveOneFaction_Member(((CDBFactionMemberGroup*)(*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_LINKMANGROUP:
				{
					CRsPlayer::SaveLinkmanData(((CDBLinkmanGroup*)(*saveDataItr)->dbObj), GetConnPtr());
					SaveThreadData* DisposedSaveData = new SaveThreadData(((CDBLinkmanGroup*)(*saveDataItr)->dbObj)->GetOwnerGUID(), DB_PLAYER);
					GetGame()->GetDBEntityManager()->PushDisposedSaveThreadData(DisposedSaveData);
				}
				break;
			case DB_PHRGNGOODSGROUP:
				{
					CRsDupRgn::SaveDupRgnObj(((CDBRgnGoodsGroup*)(*saveDataItr)->dbObj)->GetOwnerGUID(), ((CDBRgnGoodsGroup*)(*saveDataItr)->dbObj)->GetGoodsGroupMap(), GetConnPtr());
				}
				break;
			case DB_GOODSGROUP:
				{
					CRSGoods::SaveGoodsFiled(((CDBGoodsGroup*)(*saveDataItr)->dbObj)->GetOwnerGUID(), ((CDBGoodsGroup*)(*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_LOGINPLAYER:
				{					
					// 是否首次保存玩家属性数据标志
					// 保证只保存一次属性数据
					bool FirstSaveBaseFlag = false;
					bool ret = GetGame()->GetRsPlayer()->SavePlayerBase((CDBLoginPlayer*)(*saveDataItr)->dbObj, GetConnPtr());
				}
				break;
			case DB_PLAYER:
				{										
					GetGame()->GetRsPlayer()->SavePlayerAbilities((CDBPlayer*)((*saveDataItr)->dbObj), GetConnPtr());
					lSavePlayers--;
				}
				break;
			case DB_REGION:
				{
					GetGame()->GetRsRegion()->Save((CDBRegion*)((*saveDataItr)->dbObj),GetConnPtr());
				}
				break;
			case DB_PHDUPRGN:
				{
					GetGame()->GetRsDupRgn()->SaveDupRgn((CDBDupRgn*)((*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			case DB_FACTION:
				{
					GetGame()->GetRsFaction()->SaveOneFaction((CDBFaction*)((*saveDataItr)->dbObj), GetConnPtr());
				}
				break;
			}
		} 
		else if( *saveDataItr 
			&& ( NULL == (*saveDataItr)->dbObj && NULL == (*saveDataItr)->saveObj ) )// 传入一个空对象表示线程退出
		{
			ret = false;
		}
		// 清除消息对象
		SAFE_DELETE((*saveDataItr));
	}

	if(ret)
		AddLogText("Save [%d] 个数据完成!", tempSaveThreadDataList.size());

	_snprintf(pszLogingInfo,512,"Save [%d] 个数据完成!(ret:%d)", tempSaveThreadDataList.size(),ret);
	PutStringToFile("Login_DBS_Info",pszLogingInfo);
	return ret;
}
DBAdoBase* DBSaveThread::GetAdoBasePtr(void)
{
	return m_pConnPtr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned __stdcall DBSaveThread::RunThread(void *pThis)
{
#ifndef _DEBUG
	__try
	{
#endif
		HRESULT hr = ::CoInitializeEx( NULL, COINIT_MULTITHREADED );
		DBSaveThread *pSaveThread = (DBSaveThread*)pThis;
		if (NULL != pSaveThread)
		{
			pSaveThread->GetAdoBasePtr()->OpenCn();
			while(pSaveThread->ProcessData())
			{
				
			}
			pSaveThread->GetAdoBasePtr()->CloseCn();
			
			char outStr[1024];
			sprintf(outStr, "ID:[%d] 线程成功退出!\r\n", pSaveThread->GetThreadID());
			OutputDebugStr(outStr);
			SetEvent(pSaveThread->GetQuitWaitHandle());
		}
		::CoUninitialize();

#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		//	MessageBox(g_hWnd, "程序出现错误，请查看最新的错误报告文件", "Error", MB_OK);

		PutDebugString("程序出现错误，请查看最新的错误报告文件");
		AddErrorLogText("程序出现错误，请查看最新的错误报告文件");

		if(GetGame())
			GetGame()->Release();
		DeleteGame();

		Sleep(5000);

		WinExec("DBServer.exe", SW_SHOW);

//		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
		SetEvent(g_hGameThreadExitEvent);
		_endthreadex( 0 );

		return 0;
	}
#endif

	_endthreadex(0);
	return 0;
}