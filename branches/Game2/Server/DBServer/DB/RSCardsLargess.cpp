#include "stdafx.h"
#include <map>
#include "RSCardsLargess.h"
//#include "..\..\server\dbserver\appworld\goods\cgoodsfactory.h"
#include "..\..\server\dbserver\dbserver\game.h"
//#include "..\..\server\dbserver\appworld\goods\CGoodsBaseProperties.h"
//#include "../../server/setup/Logsystem.h"
#include "..\nets\netdb\message.h"
#include "..\..\server\DBServer\AppWorld\DBentityManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

multimap<string,CDBCardsLargess::tagLargess*> CDBCardsLargess::m_mapLargess;
CRITICAL_SECTION CDBCardsLargess::csMapLargess;
CRITICAL_SECTION CDBCardsLargess::csCycleLoadThread;
//CRITICAL_SECTION CDBCardsLargess::csConnectSatus;

HANDLE CDBCardsLargess::m_hWorkThread = NULL;

extern void AddLogText(char* msg, ...);
extern CGame* GetGame();

// 初始化
bool CDBCardsLargess::Init()
{	
	//if (!GetGame()->GetSetup()->dwLoadLargessTime) return false;

	InitializeCriticalSection(&csMapLargess);
	InitializeCriticalSection(&csCycleLoadThread);

	return true;
}

// 释放
bool CDBCardsLargess::UnInit()
{	
	// 等待工作线程结束
	WaitForSingleObject(m_hWorkThread, INFINITE);
	CloseHandle(m_hWorkThread);

	DeleteCriticalSection(&csMapLargess);
	DeleteCriticalSection(&csCycleLoadThread);
	//DeleteCriticalSection(&csConnectSatus);
	return true;
}

//从数据库载入赠送记录到内存
void CDBCardsLargess::AppendLargessToMap(long SendID, string strCDKey,long lSendNum,long lObtainedNum)
{	
	if (m_mapLargess.empty())
	{
		tagLargess *stLargess = new tagLargess;
		stLargess->lSendID = SendID;
		stLargess->lSendNum = lSendNum;
		stLargess->lObtainedNum = lObtainedNum;

		m_mapLargess.insert(pair<string,tagLargess*> (strCDKey,stLargess));
	}
	else
	{
		multimap<string,tagLargess*>::iterator it=m_mapLargess.begin();
		for (;it!=m_mapLargess.end();it++)
		{
			if (it->second->lSendID==SendID)
				return;
		}
		if (it==m_mapLargess.end())
		{
			tagLargess *stLargess = new tagLargess;
			stLargess->lSendID = SendID;
			stLargess->lSendNum = lSendNum;
			stLargess->lObtainedNum = lObtainedNum;

			m_mapLargess.insert(pair<string,tagLargess*> (strCDKey,stLargess));	
		}
	}
	return;
}

//添加金钱到银行
bool CDBCardsLargess::AddGoldCoin(/*long lSendId,*/CDBPlayer* pPlayer,CDBGoods* pGoods)
{
//	if (pPlayer->m_cBank.AddGoldCoinOfLargess(pGoods)/*Add(pGoods)*/)
//	if(pGoods)
//	{
//		CGUID goodID = pGoods->GetExID();
//		map<CGUID, CDBGoods*>& mapGood = pPlayer->GetmapEquipGoods();
//
//		map<CGUID, CDBGoods*>::iterator itr = mapGood.find(goodID);

//		if(itr == mapGood.end())	
//		{
//			mapGood[goodID] = pGoods;
			//ObtainRecord stObtainRecord={lSendId,pGoods->GetAmount(),""};
//			//AddlistObtainRecord(stObtainRecord);
//			return true;
//		}
//		else
//		{
//			//ObtainRecord stObtainRecord={lSendID,0,"Exceed maximum amount"};
//			//AddlistObtainRecord(stObtainRecord);
//			return false;
//		}
//	}

	return false;
}

//添加一个赠品到仓库
bool CDBCardsLargess::AddOneLargess(/*long lSendID,*/CDBPlayer* pPlayer,CDBGoods* pGoods)
{
	DWORD dwPosition=0;
	DWORD dwTmpAmount=0;	
	for (;dwPosition<8*12;dwPosition++)
	{
	//	if (pPlayer->m_cDepot.Add(dwPosition,pGoods))  
			break;
	}
	if (dwPosition<96/*8*12*/)
	{
		//ObtainRecord stObtainRecord={lSendID,1,""};
		//AddlistObtainRecord(stObtainRecord);
		return true;
	}
	else
	{
		//ObtainRecord stObtainRecord={lSendID,0,"Depot space is not enough"};
		//AddlistObtainRecord(stObtainRecord);
		return false;
	}
}

//存某帐号下的赠品装载明细到数据库
bool CDBCardsLargess::SaveLoadDetails(string strCDKey)
{
	return true;
}

// 把赠品附加到玩家仓库中
void CDBCardsLargess::LoadLargess(CDBPlayer* pParam)
{

}

//////////////////////////////////////////////////////////////////////////
//	线程相关
//////////////////////////////////////////////////////////////////////////

// 启动 LoginDB->CostDB的读取线程
void CDBCardsLargess::StartCycleLoadLargessThread()
{
}

// CostDB->dbserver 的读取线程
unsigned __stdcall CDBCardsLargess::CycleLoadLargessThread(void* pParam)
{
	
	return 0;
}


