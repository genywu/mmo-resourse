#include "stdafx.h"

#include <map>
#include "Largess.h"
//#include "..\..\server\dbserver\appworld\goods\cgoodsfactory.h"
#include "..\..\server\dbserver\dbserver\game.h"
//#include "..\..\server\dbserver\appworld\goods\CGoodsBaseProperties.h"
//#include "../../server/setup/Logsystem.h"
#include "..\..\server\dbserver\AppWorld\DBentityManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

multimap<string,CLargess::tagLargess> CLargess::m_mapLargess;
CRITICAL_SECTION CLargess::CriticalSectionmapLargess;
CRITICAL_SECTION CLargess::csWorkerThread;
//CRITICAL_SECTION CLargess::CriticalSectionTransferLargessThread;
CRITICAL_SECTION CLargess::csConnectSatus;

HANDLE CLargess::m_hWorkThread = NULL;

extern void AddLogText(char* msg, ...);
extern CGame* GetGame();

// 初始化
bool CLargess::Init()
{	
	//if (!GetGame()->GetSetup()->dwLoadLargessTime) return false;

	InitializeCriticalSection(&CriticalSectionmapLargess);
	InitializeCriticalSection(&csWorkerThread);

	InitializeCriticalSection(&csConnectSatus);
	return true;
}

// 释放
bool CLargess::UnInit()
{	
	// 等待工作线程结束
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

	DeleteCriticalSection(&CriticalSectionmapLargess);
	DeleteCriticalSection(&csWorkerThread);
	//DeleteCriticalSection(&CriticalSectionTransferLargessThread);
	DeleteCriticalSection(&csConnectSatus);
	return true;
}

//从数据库载入赠送记录到内存
void CLargess::AppendLargessToMap(long lSendID,string strCDKey,DWORD dwGoodsIndex,long lSendNum,long lObtainedNum,long lGoodsLevel)
{	
	tagLargess stLargess={lSendID,dwGoodsIndex,lSendNum,lObtainedNum,lGoodsLevel};
	if (m_mapLargess.empty())
		m_mapLargess.insert(pair<string,tagLargess> (strCDKey,stLargess));
	else
	{
		multimap<string,tagLargess>::iterator it=m_mapLargess.begin();
		for (;it!=m_mapLargess.end();it++)
		{
			if (it->second.lSendID==lSendID)
				return;
		}
		if (it==m_mapLargess.end())
			m_mapLargess.insert(pair<string,tagLargess> (strCDKey,stLargess));	
	}
	return;
}
/*
//添加金钱到银行
bool CLargess::AddGoldCoin(CDBPlayer* pPlayer,CDBGoods* pGoods)
{
if (pPlayer->m_cBank.AddGoldCoinOfLargess(pGoods))
{
return true;
}
else
{
return false;
}
}

//添加一个赠品到仓库
bool CLargess::AddOneLargess(CDBPlayer* pPlayer,CDBGoods* pGoods)
{
bool res = false;
DWORD dwPosition=0;
DWORD dwTmpAmount=0;	
for (;dwPosition<96;dwPosition++)
{
if(pPlayer->m_cDepot.Add(dwPosition,pGoods))  
{
res = true;
break;
}
}

// 调试信息
if( !res )
{
char str[512];
sprintf(str, "Acc:%s Id:%d Pos:%d 赠送%s(%d)失败!",pPlayer->GetAccount(), pPlayer->GetID(),
dwPosition,	pGoods->GetName(), pGoods->GetAmount());
PutStringToFile("Largess", str);
}

return res;
}
*/
//存某帐号下的赠品装载明细到数据库
bool CLargess::SaveLoadDetails(string strCDKey)
{
	return false;
}

// 把赠品附加到玩家仓库中
void CLargess::LoadLargess(CDBPlayer* pParam)
{
}

//////////////////////////////////////////////////////////////////////////
//	线程相关
//////////////////////////////////////////////////////////////////////////

// 启动工作线程
void CLargess::StartWorkerThread()
{
}

unsigned __stdcall CLargess::WorkerThread(void* pParam)
{
	return 0;
}

bool CLargess::TransferLargessThread()
{
	return false;
}

bool CLargess::CycleLoadLargessThread()
{
	return false;
}


void CLargess::GetTime(char* strTime)
{		
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	sprintf(strTime,"%d-%d-%d %d:%d:%d",Time.wYear,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute,Time.wSecond);
	return;
}
