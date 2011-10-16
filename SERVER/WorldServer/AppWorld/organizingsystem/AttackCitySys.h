///////////////////////////////////////////////////////////////////////////////////////
//AttackCitySys.h
//攻城战系统
//Function:完成攻城战系统的时间控制等等
//Author:Joe
//Create Time:2004.7.29
#pragma once
#include "organizingctrl.h"
#include "..\public\Timer.h"
#include "..\WorldCityRegion.h"
#include "..\public\Date.h"
//
//typedef struct tagAttackCityTime
//	{
//		long	lTime;					//第几次
//		long	lCityRegionID;			//城战地图
//		ULONG	lDeclarWarEventID;
//		tagTime DeclarWarTime;			//宣战时间
//		ULONG	lStartInfoEventID;
//		tagTime	AttackCityStartInfoTime;//攻城开始提示时间
//		ULONG	lStartEventID;
//		tagTime	AttackCityStartTime;	//攻城开始时间
//		ULONG	lEndInfoEventID;
//		tagTime	AttackCityEndInfoTime;	//攻城结束提示时间
//		ULONG	lEndEventID;
//		tagTime	AttackCityEndTime;		//攻城结束时间
//		ULONG	lMassEventID;
//		tagTime	MassTime;				//集结时间
//		ULONG	lClearEventID;
//		tagTime	ClearPlayerTime;		//清除城战区域的闲杂人时间
//		ULONG	lRefreshEventID;
//		tagTime	RefreshRegionTime;		//刷新场景时间
//
//		eCityState	RegionState;		//这次战斗中当前场景所处的状态
//		list<CGUID>	DecWarFactions;		//向这次城战宣战的帮会
//	}tagACT;
//
//
//class CAttackCitySys
//{
//private:
//	CAttackCitySys(void);
//	~CAttackCitySys(void);
//
//public:
//	
//	struct tagGetTodayTaxTime
//	{
//		ULONG lEventID;
//		long lHour;
//		long lMinute;
//		long lSecond;
//	};
//
//private:
//	static map<long,tagACT>		m_Attacks;
//	typedef map<long,tagACT>::iterator itAttack;
//
//	static  tagGetTodayTaxTime m_GetTodayTaxTime;
//	static CAttackCitySys* instance;											//该类唯一实例句柄
//
//public:
//	static CAttackCitySys* getInstance();										//得到唯一实例句柄
//	static bool   Initialize();													//初始化
//	static bool	  Reload();
//	void   Release();															//释放资源
//
//	bool AddToByteArray(vector<BYTE>& ByteArray);								//添加到CByteArray
//	bool UpdateApplyWarFacsToGameServer(long lWarID,vector<BYTE>& ByteArray);	//添加宣战帮会到CByteArray
//	void GenerateSaveData();
//	void	SetDecWarFactionsFromDB(long lRegionID,tagTime& WarStartTime,list<CGUID>& DecWarFactions);
//public:
//	static	void CALLBACK OnAttackCityStart(LPARAM lparam);						//攻城开始
//	static	void CALLBACK OnAttackCityEnd(LPARAM lparam);						//攻城结束
//	static	void CALLBACK OnDeclarWar(LPARAM lparam);							//攻城宣战
//	static	void CALLBACK OnMass(LPARAM lparam);								//攻城集结
//	static	void CALLBACK OnClearOtherPlayer(LPARAM lparam);					//清除其它人员
//	static	void CALLBACK OnRefreshRegion(LPARAM lparam);						//刷新场景
//	static	void CALLBACK OnAttackCityStartInfo(LPARAM lparam);					//攻城开始倒计时提示信息
//	static	void CALLBACK OnAttackCityEndnfo(LPARAM lparam);					//攻城结束倒计时提示信息
//	
//	eCityState GetCityState(long lRegionID);									//得到一个场景的当前状态
//	eCityState GetCityStateByWarNum(long lWarTime);								//得到战斗标示当前状态
//	long GetWarNumByRegionIDAndCurTime(long lRegionID);							//得到当前时间的一个场景的攻城标示		
//	void GetDecWarFactions(long lRegionID,list<CGUID>& Factions);				//得到一个场景的宣战帮会
//	void GetDecWarFactionsByWarNum(long lWarNum,list<CGUID>& Factions);			//得到一次战斗的宣战帮会
//	long	GetWarRegionIDByTime(long lTime);
//	bool	IsAlreadyDeclarForWar(long lWarNum,const CGUID& FactionGuid);		//判断一个帮会是否对某次战斗宣战
//	bool	IsAlreadyDeclarForWar(const CGUID& FactionGuid);								//判断一个帮会是否宣战
//	bool	OnPlayerDeclareWar(const CGUID& PlayerGuid,long lWarID,long lMoney);			//当一个玩家向一个城宣战时
//	bool	IsEnemyRelation(const CGUID& FactionGuid1,const CGUID& FactionGuid2);					//是否处于敌对关系
//	bool	FindInList(list<long>& TemptList,long lID);							//list查找函数
//
//	void UpdateAllDecFacToGameServer();											//更新所有的宣战帮会到GameServer
//	
//	void	OnFacWinCity(long lWarNum,long lWarRegionID,const CGUID& FactionGuid,const CGUID& UnionGuid);	//当个一个帮会赢得了城战]
//	void	OnCityWarEnd(long lWarNum);											//据点争霸战结束
//	void    OnCityWarEndToGameServer(long lRegionID,long lTime);				//发送结束消息到Gameserver
//
//	void	SetCityWarEnemyFactions(long lRegionID,list<CGUID>& DecWarFactions);	//根据装载的宣战帮会设置敌对关系
//
//	void	InitialCityAllFactionEnemyRelation();								//根据宣战情况更新帮会的敌对关系
//	void	UpdateCityAllFactionEnemyRelation();								//根据宣战情况更新帮会的敌对关系
//};
//CAttackCitySys* GetAttackCitySys();
