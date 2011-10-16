

#include "stdafx.h"
#include "ServiceSCC.h"

#include "../App/AppClass/UserManage.h"
#include "../App/AppClass/SmManage.h"
#include "../App/AppClass/DbOperator.h"
#include "../App/AppClass/PackManage.h"
#include "../App/AppClass/ActionLogManage.h"
#include "../App/AppClass/GmSystem.h"


//! ServiceSCC的最大实例数量
const ULONG ServiceSCC_MAX_INSTANCE_NUM = 1;

ULONG		ServiceSCC::c_InstanceNum;
ServiceSCC		ServiceSCC::c_ServiceSCCInstance;


ServiceSCC::ServiceSCC(VOID)
:m_bExotericExit(FALSE)
,m_hMutex(NULL)
,m_hMainThrd(NULL)
#ifdef _DEBUG
,m_dwCommand(0)
#endif
{
	if(ServiceSCC_MAX_INSTANCE_NUM <= c_InstanceNum)
		throw;
	++c_InstanceNum;
}

ServiceSCC::~ServiceSCC(VOID)
{
	--c_InstanceNum;
}

ServiceSCC& ServiceSCC::GetInstance(VOID)
{
	return c_ServiceSCCInstance;
}

void OutMemInfo(const char *pFileName, const char *pTxt)
{
	if(NULL == pTxt)
		return;

	if (NULL != pFileName)
	{
		char strFile[MAX_PATH];
		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		_snprintf(strFile, MAX_PATH, "%s\\MemPoolInfo_%04d-%02d-%02d.txt", pFileName, stTime.wYear, stTime.wMonth, stTime.wDay);

		FILE *fp;
		if( (fp=fopen(strFile, "a+")) == NULL )
		{
			return;
		}

		fseek(fp, 0, SEEK_END);
		char strTime[MAX_PATH];
		_snprintf(strTime, MAX_PATH, "%d:%d:%d\r\n", stTime.wHour, stTime.wMinute, stTime.wSecond);
		fwrite(strTime, strlen(strTime), 1, fp);
		fwrite(pTxt, strlen(pTxt), 1, fp);
		fclose(fp);
	}
}

//! 初始化程序
BOOL ServiceSCC::Init(VOID)
{
	char szCurrDir[MAX_PATH] = {0};
	char szTempDir[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szCurrDir, MAX_PATH);
	GetFilePath(szCurrDir,szTempDir);
	::SetCurrentDirectory(szTempDir);
	::CreateDirectory( "log",NULL);


	//! 创建互拆锁，只允许一个实例运行
	m_hMutex = CreateMutex(NULL, FALSE, "{805C9EE5-7CFD-4182-9685-1BF1FC58D418}");
	if(NULL == m_hMutex)
	{
		LogTools::MsgboxErrInfo("create CreateMutex lost!!");
		return FALSE;
	}

	DWORD dwWaitResult = WaitForSingleObject(m_hMutex, 50);
	if (WAIT_FAILED == dwWaitResult)
	{
		LogTools::MsgboxErrInfo("wait Mutex lost!!");
		CloseHandle(m_hMutex);
		return FALSE;
	}
	else if (WAIT_TIMEOUT == dwWaitResult)
	{
		LogTools::MsgboxErrInfo("can only run one process!!!");
		CloseHandle(m_hMutex);
		return FALSE;
	}

	//! 逻辑模块
	///////////////////////////////////////////////////////////////
	SET_OUT_INFO_SETUP(OutMemInfo, "log\\");

	//! 网络服务
	if(!ServiceNet::GetInstance().Init())
	{
		LogTools::OutLog("ServiceNet initialize lost!!!");
		return FALSE;
	}

	//! 数据库
	if( !DbOperator::Init())
	{
		LogTools::OutLog("DbOperator::Init() lost!!!");
		return FALSE;
	}
	
	//! 用户管理
	if( !UserManage::GetInstance().Init() )
	{
		LogTools::OutLog("UserManage initialize lost!!!");
		return FALSE;
	}

	//! SM管理
	if( !SmManage::GetInstance().Init())
	{
		LogTools::OutLog("SmManage initialize lost!!!");
		return FALSE;
	}

	//! 更新包管理
	if( !PackManage::GetInstance().Init())
	{
		LogTools::OutLog("PackManage initialize lost!!!");
		return FALSE;
	}

	//! 数据库日志管理
	if( !ActionLogManage::GetInstance().Init())
	{
		LogTools::OutLog("ActionLogManage initialize lost!!!");
		return FALSE;
	}

	//! 连接SM
	//ServiceNet::GetInstance().ConnectAllSM();

	LogTools::OutLog("initialize finish!");
	return TRUE;
}

//! 执行程序
VOID ServiceSCC::Begin(VOID)
{
	UINT threadID = 0;
	m_hMainThrd = (HANDLE)_beginthreadex(NULL, 0, ServiceSCC::MainLoopFunc, this, 0, &threadID);
}

//!
unsigned	__stdcall	ServiceSCC::MainLoopFunc( void* pThis )
{
	if(NULL == pThis)
		return -1;

	ServiceSCC *pServiceSCC = (ServiceSCC*)pThis;
	LogTools::OutLog("//////////////////////////////////////////////////////////");
	LogTools::OutLog("MainLoop begin!!!");


#ifndef _DEBUG
	__try
	{
#endif
		while (!pServiceSCC->m_bExotericExit && pServiceSCC->RunOneFrame())
		{
			Sleep(1);
		}
#ifndef _DEBUG
	}
	__except(_Sword3::CrashFilter(GetExceptionInformation(),GetExceptionCode()))
	{
		throw("ServiceControl::ServiceMain exception!!!");
	}
#endif
	
	LogTools::OutLog("MainLoop end!!!");
	LogTools::OutLog("//////////////////////////////////////////////////////////");	

	return 0;
}

//! 外部结束程序（不能跟RUN在同一个线程调用）
VOID ServiceSCC::CloseExec(VOID)
{
	if(NULL != m_hMainThrd)
	{
		m_bExotericExit = TRUE;
		WaitForSingleObject(m_hMainThrd, 2 * 60 * 1000);
	}
}

//! 执行一帧程序
BOOL ServiceSCC::RunOneFrame(VOID)
{
	ServiceNet::GetInstance().ProcessMessage();


#ifndef OUT_SERVICE
	//! 执行输入指令
	RunCommand();
#endif

	static DWORD LastTime = timeGetTime();
	DWORD CurrTime = timeGetTime();
	if(5 * 60 * 1000 < CurrTime - LastTime)
	{
		GmSystem::GetInstance().GetReportDbManage().CheckCache();
		OUT_RUNNING_INFO(false);
		LastTime = CurrTime;
	}
	return TRUE;
}

//! 释放程序
VOID ServiceSCC::Relese(VOID)
{
	LogTools::OutLog("Release begin!!!");

	ActionLogManage::GetInstance().Release();
	PackManage::GetInstance().Release();
	SmManage::GetInstance().Release();
	UserManage::GetInstance().Release();
	DbOperator::Release();
	ServiceNet::GetInstance().Release();

	///////////////////////////////////////////////////////////////
	//! 释放互拆锁
	ReleaseMutex(m_hMutex);
	CloseHandle(m_hMutex);

	OUT_RUNNING_INFO(true);
	LogTools::OutLog("Release end!!!");
	LogTools::OutLog("//////////////////////////////////////////////////////////");	
}
//! 响应SUC连接
VOID ServiceSCC::OnConnSUC(LONG lSocket)
{
	UserManage::GetInstance().OnUserConn(lSocket);
}

//! 响应断开SUC
VOID ServiceSCC::OnLostSUC(LONG lSocket)
{
	UserManage::GetInstance().OnUserClose(lSocket);
}
//! 响应SM连接
DWORD ServiceSCC::OnConnSM(LONG lSocket, LONG lIP)
{
	DWORD SMID = SmManage::GetInstance().OnConnect(lSocket, lIP);
	if (0 == SMID)
	{
		UCHAR arrIp[4] = {0};
		memmove(arrIp, &lIP, sizeof(LONG));
		LogTools::OutLog("illegal SMIP[%u.%u.%u.%u]!!!", (ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);
	}
	return SMID;
}

//! 响应断开SM
VOID ServiceSCC::OnLostSM(DWORD dwSMID)
{
	LogTools::OutLog("SM[ID：%d] active disconnect", dwSMID);
	SmManage::GetInstance().OnClose(dwSMID);
	//ServiceNet::GetInstance().ConnectSM(dwSMID);
}

//AsynchronousExecute::MyOverlapped* AsynchronousExecute::CreateMyOverlapped(VOID)
//{
//	return (MyOverlapped*)M_ALLOC(sizeof(MyOverlapped));
//}
//VOID AsynchronousExecute::RestoreMyOverlapped(MyOverlapped** ppMyOverlapped)
//{
//	if(NULL != ppMyOverlapped)
//		M_FREE(*ppMyOverlapped, sizeof(MyOverlapped));
//}