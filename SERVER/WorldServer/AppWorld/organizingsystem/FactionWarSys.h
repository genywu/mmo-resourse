//////////////////////////////////////////////////////////////////////////
//FactionWarSys.h
//Function:帮战系统相关功能
//Author:Joe
//Create Time:2005.6.27
//////////////////////////////////////////////////////////////////////////
struct tagTime;
class COrganizing;
#include <set>
#include "../public/GUID.h"
using namespace std;

#pragma once
//struct tagEnemyFaction
//{
//	CGUID	FactionGuid1;		//敌对帮会甲方
//	CGUID	FactionGuid2;		//敌对帮会乙方
//	DWORD	dwDisandTime;		//敌对状态解散倒计时(毫秒)
//
//	tagEnemyFaction()
//		:FactionGuid1(NULL_GUID)
//		,FactionGuid2(NULL_GUID)
//		,dwDisandTime(0)
//	{
//	}
//
//	tagEnemyFaction(const CGUID& guid1,const CGUID& guid2,DWORD dwTime)
//		:FactionGuid1(guid1)
//		,FactionGuid2(guid2)
//		,dwDisandTime(dwTime)
//	{
//	}
//
//};
//
////帮战类别
//struct tagWarType
//{
//	long	lType;		//类别
//	long	lFightTime;	//战斗时间
//	long	lMoney;		//宣战需要的金钱
//};
//
//class CFactionWarSys
//{
//public:
//	CFactionWarSys(void);
//	~CFactionWarSys(void);
//private:
//	static	CFactionWarSys* pInstance;
//public:
//	static	CFactionWarSys* GetInstance();
//	void	Release();
//private:
//	map<long,tagWarType>	m_FactionWars;		//帮战的配置
//	list<tagEnemyFaction>	EnemyFactionList;	//敌对帮会列表
//	DWORD					m_dwStartTime;		//开始统计时间
//
//public:
//	bool   Initialize();																		//初始化
//	bool	LoadIni();
//	void	Clear()		{EnemyFactionList.clear();}
//
//	void	Run();																				//帮战系统处理
//
//	bool DigUpTheHatchet(const CGUID& DigGuid,const CGUID& BeDigedFactinID,long lLvl,tagTime& Time);			//宣战
//	void StopFactionWar(const CGUID& FactionGuid1,const CGUID& FactionGuid2);										//终止帮战
//
//	void OnPlayerDied(const CGUID& PlayerID, const CGUID& Killer);								//玩家死亡时触发
//	void OnTimeOut(const CGUID& FactionGuid1,const CGUID& FactionGuid2);											//帮战时间已到触发
//
//	bool IsEnemyRelation(const CGUID& FactionGuid1,const CGUID& FactionGuid2);					//判断两个帮会是否是敌对关系
//	long GetDecWarMoneyByType(long lType);														//得到宣战某个类型需要的金钱
//
//	void GetEnemyFactionsByFactionID(const CGUID& FactionGuid,set<COrganizing*>& FactionList,long& DisTime);//得到一个帮会的敌对帮会列表
//
//	void AddOneEnmeyFaction(const CGUID& FactionGuid1,const CGUID& FactionGuid2,DWORD dwLeaveTime);
//	void ClearEnemyFaction(const CGUID& FactionGuid1,const CGUID& FactionGuid2);								//清楚两个帮会间的敌对关系
//
//	void GenerateSaveData();
//};
//
//CFactionWarSys* GetFactionWarSys();
