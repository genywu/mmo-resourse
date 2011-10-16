#include "StdAfx.h"
#include "BaseModule.h"
#include "Procedure.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"
//#include "../Script/Script.h"
//#include "../Script/VariableList.h"
#include "../../Server/GameManager.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CBaseModule::CBaseModule(stModuParam *pParam,stStaticModuParam *pStatiParam)
:m_pParam(pParam)
,m_pStatiParam(pStatiParam)
{
	 m_nID = m_pParam->GetID();
	 m_nLvl = m_pParam->GetLv();
}

CBaseModule::CBaseModule(void *pParam)
:m_pParam(((tagCBaseModuleParam*)pParam)->pParam)
,m_pStatiParam(((tagCBaseModuleParam*)pParam)->pStatiParam)
{
	m_nID = m_pParam->GetID();
	m_nLvl = m_pParam->GetLv();
}

CBaseModule::~CBaseModule(void)
{
		OBJ_RELEASE(stStaticModuParam, m_pStatiParam);

	itPro it=m_Procedures.begin();
	for (;it!=m_Procedures.end();++it)
	{
		OBJ_RELEASE(CProcedure, *it);
	}
	m_Procedures.clear();
}



void CBaseModule::Initial()
{
	// 执行脚本
#ifdef _DEBUG
	DWORD dwStartTime = timeGetTime();
#endif
//  [5/4/2009 chenxianj]
	//CScript *pTempScript =  PopFreeScript(m_pParam->strFileName.c_str());
	//if (pTempScript)
	//{
	//	pTempScript->SetVariableList(m_pParam->pVariableList);		

	//	if( pTempScript->LoadScript(m_pParam->strFileName.c_str(), (char*)GetGame()->GetScriptFileData(m_pParam->strFileName.c_str())) )
	//		pTempScript->RunStep();
	//	PushFreeScript(pTempScript);
	//}

	Script::RunEnv env( NULL, NULL, NULL, m_pParam->pVariableList );
	GetInst( ScriptSys ).RunScript( env, m_pParam->strFileName.c_str() );

#ifdef _DEBUG
	GetGame()->AddInitScript(timeGetTime()-dwStartTime);
#endif

}


//初始化基本数据
void CBaseModule::InitBaseData(vector<CProcedure*> &vProcedure)
{
	m_Procedures = vProcedure;
}




// 添加到CByteArray
bool CBaseModule::AddToByteArray(vector<BYTE>* pByteArray)
{
	DWORD dwFuture =0;
	DWORD dwCount = 0;
	//当前执行的工序号
	_AddToByteArray(pByteArray,m_pParam->nProceNum);
	//添加当前定时器信息
	long lTimerNum =  (long)m_pParam->Timers.size();
	_AddToByteArray(pByteArray,lTimerNum);
	stModuParam::itTimer it = m_pParam->Timers.begin();
	for(;it != m_pParam->Timers.end();it++)
	{
		stTimerParam *pTimerParam = (*it);
		pTimerParam->GetRemainTime(dwFuture,dwCount);

		if (m_pParam->bTime)
		{
			time_t t_CurrentTime = time(NULL);
			dwFuture =  (long)t_CurrentTime + dwFuture/1000; 
		}
		_AddToByteArray(pByteArray,pTimerParam->lFlag);
		_AddToByteArray(pByteArray,dwFuture);
		_AddToByteArray(pByteArray,dwCount);
	}
	return true;
}

// 添加到CByteArray为发送到客户段
bool CBaseModule::AddToByteArrayForClient(vector<BYTE>* pByteArray,ModuleType eType)
{
	//添加id和等级	
	_AddToByteArray(pByteArray,(long)m_pParam->nID);
	_AddToByteArray(pByteArray,(long)m_pParam->nLvl);
	//如果是状态添加第一个定时器剩余时间到客户端
	if(eType == ModuType_Buff || eType == ModuType_DeBuff)
	{
		stModuParam::itTimer it = m_pParam->Timers.begin();
		if( it!=m_pParam->Timers.end() )
		{
			stTimerParam *pTimerParam = (*it);
			DWORD dwRemainTime = 
				pTimerParam->lFuture+pTimerParam->lInter*(pTimerParam->nCount-pTimerParam->nCalledCount);
			_AddToByteArray(pByteArray,dwRemainTime);
		}
		else
		{
			_AddToByteArray(pByteArray,(DWORD)0);
		}
	}
	
	return true;
}

// 解码
bool CBaseModule::DecordFromByteArray(long lProceNum,list<stTimerParam*>& Timers)
{
	m_pParam->nProceNum = lProceNum;
	m_pParam->Timers = Timers;
	return true;
}


// 添加到DataBlock
bool CBaseModule::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	DWORD dwFuture =0;
	DWORD dwCount = 0;
	//当前执行的工序号
	setWriteDB.AddToByteArray(m_pParam->nProceNum);
	//添加当前定时器信息
	long lTimerNum =  (long)m_pParam->Timers.size();
	setWriteDB.AddToByteArray(lTimerNum);
	stModuParam::itTimer it = m_pParam->Timers.begin();
	for(;it != m_pParam->Timers.end();it++)
	{
		stTimerParam *pTimerParam = (*it);
		pTimerParam->GetRemainTime(dwFuture,dwCount);
		setWriteDB.AddToByteArray(pTimerParam->lFlag);
		setWriteDB.AddToByteArray(dwFuture);
		setWriteDB.AddToByteArray(dwCount);
	}
	return true;
}
// 到添加到DataBlock为客户端
bool CBaseModule::CodeToDataBlockForClient(DBWriteSet& setWriteDB,ModuleType eType)
{
	//添加id和等级	
	setWriteDB.AddToByteArray((long)m_pParam->nID);
	setWriteDB.AddToByteArray((long)m_pParam->nLvl);
	//如果是状态添加第一个定时器剩余时间到客户端
	if(eType == ModuType_Buff || eType == ModuType_DeBuff)
	{
		stModuParam::itTimer it = m_pParam->Timers.begin();
		if( it!=m_pParam->Timers.end() )
		{
			stTimerParam *pTimerParam = (*it);
			DWORD dwRemainTime = 
				pTimerParam->lFuture+pTimerParam->lInter*(pTimerParam->nCount-pTimerParam->nCalledCount);
			setWriteDB.AddToByteArray(dwRemainTime);
		}
		else
		{
			setWriteDB.AddToByteArray((DWORD)0);
		}
	}
	return true;
}
// 解码
bool CBaseModule::DecodeFromDataBlock(long lProceNum,list<stTimerParam*>& Timers)
{
	m_pParam->nProceNum = lProceNum;
	m_pParam->Timers = Timers;
	return true;
}


//设置施放者
void  CBaseModule::SetFireObjs()
{
	if(m_pParam->FirerID != NULL_GUID)
	{
		if (m_pParam->FirerID == m_pParam->pUser->GetExID())
		{
			m_pParam->pFirer = m_pParam->pUser;
		}
		else
		{
			CServerRegion* pServerRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
			if(NULL != pServerRegion)
			{
				CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nFirerType,m_pParam->FirerID);
				if (pObj)
				{
					m_pParam->pFirer = dynamic_cast<CMoveShape*>(pObj);
				}
				else
				{
					m_pParam->pFirer = NULL;
				}
				
			}			
		}		
	}
	else
	{
		m_pParam->pFirer = NULL;
	}

	//  [5/4/2009 chenxianj]
	//m_pParam->pVariableList->AddVar("$ParamF",(DWORD)m_pParam->pFirer);
	m_pParam->pVariableList->AddVar("$ParamF",(VariableList::Number)(DWORD)m_pParam->pFirer);
}
//得到下一个工序
eModuleRet CBaseModule::NextProcedure()
{
	eModuleRet eRet = MR_Continue;

#ifdef _DEBUG_SKILL_
	if ((long)m_Procedures.size() <= m_pParam->nProceNum)
	{
		Log4c::Trace(ROOT_MODULE, "CBaseModule::NextProcedure()error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
		return MR_Modu_End;
	}
#endif
	//结束某个工序
	m_Procedures[m_pParam->nProceNum]->End(m_pParam);
	
	if( ++m_pParam->nProceNum <  (long)m_Procedures.size())
		eRet = MR_Proc_Next;
	else
		eRet = MR_Modu_End;

#ifdef _DEBUG_SKILL_
	if ((long)m_Procedures.size() <= m_pParam->nProceNum)
	{
		Log4c::Trace(ROOT_MODULE, "CBaseModule::NextProcedure()2error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
		return MR_Modu_End;
	}
#endif
	return eRet;
}

//连续执行所有工序 
eModuleRet CBaseModule::DoSome(eModuleRet eRet)
{
	while(eRet == MR_Proc_Next)//"下一个处理工序"
	{
		//调用工序的开始
#ifdef _DEBUG_SKILL_
		if ((long)m_Procedures.size() <= m_pParam->nProceNum)
		{
			Log4c::Trace(ROOT_MODULE, "CBaseModule::DoSome() error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
			return MR_Modu_End;
		}
#endif
		CProcedure* pProcedure = m_Procedures[m_pParam->nProceNum];
		if(pProcedure)
		{
			eRet = m_Procedures[m_pParam->nProceNum]->Begin(m_pParam);
		}
		else
		{
			eRet = MR_Modu_End; 
		}
		//下一个工序
		if(eRet == MR_Proc_Next)
			eRet = NextProcedure();
	}
	return eRet;
}
//开始
eModuleRet CBaseModule::Begin(long lMouseX,long lMouseY,long lMouseShapeType,const CGUID& MouseShapeID,long lCreateID)
{
#ifdef _RUNSTACKINFO3_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CBaseModule::Begin(SkillID:%d) Start",m_pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	 m_pParam->pMouseShape = NULL;
	//初始化数据
	m_pParam->SetDesObjs(lMouseX,lMouseY,lMouseShapeType,MouseShapeID);//"设置目标数据"
	SetFireObjs();//"设置施放者"
	m_pParam->dwCreateID = lCreateID;
	eModuleRet eRet = MR_Proc_Next;
	if(m_Procedures.size() ==0)	
	{
		return eRet;
	}
	//连续执行一些工序	
	eRet = DoSome(eRet);

#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CBaseModule::Begin() End()");
#endif
	return eRet;
}

void CBaseModule::ProcNumSub()
{
	--m_pParam->nProceNum;
	if((m_pParam->nProceNum)<=0) 
		m_pParam->nProceNum=0;
}


//事件处理接口
eModuleRet CBaseModule::Proc(DWORD type,const tagProcVar *pVar)
{
#ifdef _RUNSTACKINFO3_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CBaseModule::Proc(SkillID:%d,EventType:%d,ProceNum:%d) Start",m_pParam->nID,type,m_pParam->nProceNum);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

	SetFireObjs();
	
	m_pParam->pMouseShape = NULL;
	//设置当前事件ID
	m_pParam->pEventVar = (tagProcVar*)pVar;
	m_pParam->lCurEventType = type;

	eModuleRet eRet = MR_Continue;
#ifdef _DEBUG_SKILL_
	if ((long)m_Procedures.size() <= m_pParam->nProceNum)
	{
		Log4c::Trace(ROOT_MODULE, "CBaseModule::Proc() error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
		return MR_Modu_End;
	}
#endif


	eRet = m_Procedures[m_pParam->nProceNum]->Proc(type,pVar,m_pParam);
	
	//下一个工序
	if(eRet == MR_Proc_Next)
	{
		eRet = NextProcedure();
		DoSome(eRet);
	}
	else if(eRet == MR_Modu_End)
	{
		m_Procedures[m_pParam->nProceNum]->End(m_pParam);
	}

	m_pParam->pEventVar = NULL;
	m_pParam->lCurEventType = 0;
	
#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CBaseModule::Proc() End()");
#endif
	return eRet;
}
//超时处理接口
eModuleRet CBaseModule::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
#ifdef _RUNSTACKINFO3_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CBaseModule::OnTimeOut(SkillID:%d,ProceNum:%d) Start",m_pParam->nID,m_pParam->nProceNum);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

	m_pParam->pMouseShape = NULL;
	SetFireObjs();
	//设置当前定时器ID
	m_pParam->lCurTimerID = timerid;
	eModuleRet eRet = MR_Continue;

#ifdef _DEBUG_SKILL_
	if ((long)m_Procedures.size() <= m_pParam->nProceNum)
	{
		Log4c::Trace(ROOT_MODULE, "CBaseModule::OnTimeOut() error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
		return MR_Modu_End;
	}
#endif	
	eRet = m_Procedures[m_pParam->nProceNum]->OnTimeOut(timerid,curtime,var,m_pParam);


	//下一个工序
	if(eRet == MR_Proc_Next)
		eRet = NextProcedure();
	else if (eRet == MR_Modu_End)
	{
#ifdef _DEBUG_SKILL_
		if ((long)m_Procedures.size() <= m_pParam->nProceNum)
		{
			Log4c::Trace(ROOT_MODULE, "CBaseModule::OnTimeOut()1 error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
			return MR_Modu_End;
		}
#endif

		m_Procedures[m_pParam->nProceNum]->End(m_pParam);
	}
#ifdef _DEBUG_SKILL_
	if ((long)m_Procedures.size() <= m_pParam->nProceNum)
	{
		Log4c::Trace(ROOT_MODULE,"CBaseModule::OnTimeOut()2 error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
		return MR_Modu_End;
	}
#endif

	eRet = DoSome(eRet);
	m_pParam->lCurTimerID = -1;

	


#ifdef _RUNSTACKINFO3_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CBaseModule::OnTimeOut() End()");
#endif
	return eRet;
}
//结束调用接
eModuleRet CBaseModule::End()
{
	eModuleRet eRet = MR_Modu_End;
	
	if( (long)m_Procedures.size() > m_pParam->nProceNum)
	{
		eRet = m_Procedures[m_pParam->nProceNum]->End(m_pParam);
		if (m_Procedures.size()>1 && m_pParam->nProceNum==0)
		{
			++m_pParam->nProceNum;
#ifdef _DEBUG_SKILL_
			if ((long)m_Procedures.size() <= m_pParam->nProceNum)
			{
				Log4c::Trace(ROOT_MODULE,"CBaseModule::End()2 error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);
				return MR_Modu_End;
			}
#endif
			eRet = m_Procedures[m_pParam->nProceNum]->End(m_pParam);
		}
	}
	else
	{
#ifdef _DEBUG_SKILL_
		Log4c::Trace(ROOT_MODULE,"CBaseModule::End() error: m_Procedures.size() =%d, m_pParam->nProceNum = %d", m_Procedures.size(), m_pParam->nProceNum);	
#endif
	}
	return eRet;
}
