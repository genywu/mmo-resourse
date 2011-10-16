#pragma once

#include "../MyAdobase.h"
#include "RSGoods.h"
#include "RsPlayer.h"
#include <process.h>
using namespace std;

/*
 *	设计思路

 赠品来源于LoginDB的Largess表，
 dbserver定时（30秒）把LoginDB的Largess中的新的赠品记录同步到CostDB的Largess表中，
 dbserver定时（30秒）把CostDB的Largess表中的记录读取到内存中 m_mapLargess，
 每当有帐号登陆读取player数据时，在m_mapLargess中寻找有没有相对于该帐号的赠品，
 如果有就直接放到帐号的仓库中（仓库满了就不放），
 以上步骤都是在dbserver内存中进行，

 在每个系统存盘点的时候，把所有已经赠送出去的赠品在CostDB的Largess表中标记出来，
 并在CostDB的LoadDetail表中写入对应的已经读取记录，表明该赠品已经发放到了玩家手中

 *	soft 2005-11-25 


 *
 * 用于购买成功后，把所得金额放到数据库中，玩家下次登陆时获取，这里区别与原来的是：
 *  只有金钱 没其他物品
 *  从dbserver直接写入数据库，不用从logindb转入数据
 * copy by bugs 2006-1-21
 */


class CDBCardsLargess : public CMyAdoBase
{
//////////////////////////////////////////////////////////////////////////
//	赠送相关
//////////////////////////////////////////////////////////////////////////
public:
	static bool Init();											// 初始化
	static bool UnInit();										// 释放

	static bool SaveLoadDetails(string strCDKey);				// 存某帐号下的赠品装载明细到数据库 存盘线程SaveThread中调用
	static void LoadLargess(CDBPlayer* pParam);					// 把赠品附加到玩家仓库中
	static long GetLargessNum()		{return (long)m_mapLargess.size();}	// 获取还没有分配的赠品数量

private:
	static bool AddOneLargess(/*long lSend,*/CDBPlayer* pPlayer,CDBGoods* pGoods);		// 添加一个赠品到仓库
	static bool AddGoldCoin(/*long lSendID,*/CDBPlayer* pPlayer,CDBGoods* pGoods);		// 添加金钱到银行
	static void AppendLargessToMap(long SendID,string strCDKey,long lSendNum,long lObtainedNum);     // 从数据库载入赠送记录到内存

	// 赠品结构
	struct tagLargess
	{
		long lSendID;											// 赠送ID
		long lSendNum;											// 应赠送数量
		long lObtainedNum;										// 已获取数量
	};

	static multimap<string,tagLargess*> m_mapLargess;						// 赠送记录,KEY为Cdkey

//////////////////////////////////////////////////////////////////////////
//	线程相关
//////////////////////////////////////////////////////////////////////////
private:
	static CRITICAL_SECTION csMapLargess;
	static CRITICAL_SECTION csCycleLoadThread;

	//static CRITICAL_SECTION csConnectSatus;

	static unsigned __stdcall TransferLargessThread(void* pParam);			// LoginDB上的赠品记录传输到CostDB上的线程
	static unsigned __stdcall CycleLoadLargessThread(void* pParam);			// CostDB->dbserver 的读取线程

public:
	static void StartTransferLargessThread();								// 启动将存在于LoginDB上的赠品记录传输到CostDB上的线程
	static void StartCycleLoadLargessThread();								// 启动 CostDB->dbserver 的读取线程

	static HANDLE m_hWorkThread;	// 工作线程句柄
};
