#pragma once
#include "organizingctrl.h"
#include "..\WorldRegion.h"
#include "..\public\Date.h"

///////////////////////////////////////////////////////////////////////////////////////
//VillageWarSys.h
//帮战系统
//Function:完成帮战系统的相关功能
//Author:Joe
//Create Time:2005.7.13
//////////////////////////////////////////////////////////////////////////

//
//struct tagVilWarSetup
//{
//	long	lID;				//标示ID
//	long	lWarRegionID;		//战斗的地图
//	ULONG	lDeclarWarEventID;
//	tagTime DeclarWarTime;		//宣战时间
//	ULONG	lStartInfoEventID;
//	tagTime WarStartInfoTime;	//村战开始信息时间
//	ULONG	lStartEventID;
//	tagTime WarStartTime;		//村战开始时间
//	ULONG	lEndInfoEventID;
//	tagTime WarEndInfoTime;		//村战结束信息时间
//	ULONG	lEndEventID;
//	tagTime WarEndTime;			//村战结束时间
//	ULONG	lClearEventID;
//	tagTime	ClearPlayerTime;	//清除据点地图的玩家时间
//	long	lVilRegionID;		//村落地图
//		
//	eCityState	RegionState;	//这次战斗中当前场景所处的状态
//	list<CGUID>	DecWarFactions;	//报名的帮会
//};
//class CVillageWarSys
//{
//public:
//	CVillageWarSys(void);
//	~CVillageWarSys(void);
//
//private:
//	static CVillageWarSys* instance;											//该类唯一实例句柄
//
//
//	map<long,tagVilWarSetup>	m_VillageWars;									//村战的	
//	typedef map<long,tagVilWarSetup>::iterator itVelWar;
//
//	bool	FindInList(list<long>& TemptList,long lID);							//list查找函数
//public:
//	static CVillageWarSys* getInstance();										//得到唯一实例句柄
//	bool   Initialize();														//初始化
//	bool   ReLoad();															//重载
//	void   Release();	
//	bool   CheckSetup();														//检测村战和村子地图是否在一个服务器上
//
//	bool	AddToByteArray(vector<BYTE>& ByteArray);							//添加到CByteArray
//	bool	UpdateApplyWarFacsToGameServer(long lWarID,vector<BYTE>& ByteArray);//添加宣战帮会到CByteArray
//	void	GenerateSaveData();
//	void	SetDecWarFactionsFromDB(long lRegionID,tagTime& WarStartTime,list<CGUID>& DecWarFactions);
//public:
//
//	bool	GetVilWarSetupByTime(tagVilWarSetup& VilWarSetup,long lTime);		//得到某次的战斗的参数
//	long	GetWarRegionIDByTime(long lTime);
//	long	GetWarTimeByRegionID(long lRegionID);
//	static	void CALLBACK OnDelcareWar(LPARAM lparam);							//宣战开始
//	static	void CALLBACK OnAttackVillageStart(LPARAM lparam);					//攻村开始
//	static	void CALLBACK OnAttackVillageEnd(LPARAM lparam);					//攻村结束
//	static	void CALLBACK OnClearPlayer(LPARAM lparam);							//清除其它人员
//	static	void CALLBACK OnAttackVillageStartInfo(LPARAM lparam);				//攻村开始信息
//	static	void CALLBACK OnAttackVillageEndInfo(LPARAM lparam);				//攻村结束信息
//
//	bool	ApplyForVillageWar(const CGUID& PlayerGuid,long lWarID,long lMoney);			//帮会申请加入据点争霸战
//	long 	IsApplyForWarByRegionID(long lRegionID);							//得到该场景是否可以申请报名
//
//	bool	IsExitWarID(long lWarID);											//判断是否存在这个WarID
//	eCityState GetRegionState(long lRegionID);									//得到一个场景的当前状态
//	eCityState GetCityStateByWarNum(long lWarTime);								//得到战斗标示当前状态
//	long GetWarNumByRegionIDAndCurTime(long lRegionID);							//得到当前时间的一个场景的攻城标示
//	void GetDecWarFactions(long lRegionID,list<CGUID>& Factions);				//得到一个场景的宣战帮会
//	void GetDecWarFactionsByWarNum(long lWarNum,list<CGUID>& Factions);			//得到一次战斗的宣战帮会
//	void SetVilWarState(long lTime,eCityState CityState);						//设置某次战斗的状态
//	bool IsAlreadyDeclarForWar(long lWarNum,const CGUID& FactionGuid);					//判断一个帮会是否对某次战斗宣战
//	bool IsAlreadyDeclarForWar(const CGUID& FactionGuid);								//判断一个帮会是否宣战
//	bool IsVilRegionLeftClearTime(long lRegionID);								//判断是一个村，并且过了清人时间
//
//	void OnFacWinVillage(long lWarNum,long lWarRegionID,const CGUID& FactionGuid,const CGUID& UnionGuid);	//当一个帮会赢得了村战
//	void OnVillageWarEnd(long lWarNum,long lWarRegionID);						//据点争霸战结束
//};
//
//CVillageWarSys* GetVilWarSys();
