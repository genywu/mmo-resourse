#pragma once

#include "..\ServerRegion.h"
#include "..\public\Date.h"

///////////////////////////////////////////////////////////////////////////////////////
//VillageWarSys.h
//帮战系统
//Function:完成帮战系统的相关功能
//Author:Joe
//Create Time:2005.7.13
//////////////////////////////////////////////////////////////////////////


struct tagVilWarSetup
{
	long	lID;				//标示ID
	long	lWarRegionID;		//战斗的地图
	ULONG	lDeclarWarEventID;
	tagTime DeclarWarTime;		//宣战时间
	ULONG	lStartInfoEventID;
	tagTime WarStartInfoTime;	//村战开始信息时间
	ULONG	lStartEventID;
	tagTime WarStartTime;		//村战开始时间
	ULONG	lEndInfoEventID;
	tagTime WarEndInfoTime;		//村战结束信息时间
	ULONG	lEndEventID;
	tagTime WarEndTime;			//村战结束时间
	ULONG	lClearEventID;
	tagTime	ClearPlayerTime;	//清除据点地图的玩家时间
	long	lVilRegionID;		//村落地图
	
	eCityState	RegionState;	//这次战斗中当前场景所处的状态
	list<CGUID>	DecWarFactions;	//报名的帮会
};
class CVillageWarSys
{
public:
	CVillageWarSys(void);
	~CVillageWarSys(void);

private:
	static CVillageWarSys* instance;											//该类唯一实例句柄


	map<long,tagVilWarSetup>	m_VillageWars;									//村战的	
	typedef map<long,tagVilWarSetup>::iterator itVelWar;

	bool	FindInList(list<long>& TemptList,long lID);							//list查找函数
public:
	static CVillageWarSys* getInstance();										//得到唯一实例句柄
	void   Release();	

	bool DecordFromByteArray(BYTE* pByte, long& pos);
	bool UpdateApplyWarFacs(BYTE* pByte, long& pos);

public:
	void OnDelcareWar(long lWarID);									//宣战开始
	void OnAttackVillageStart(long lWarID);							//攻村开始
	void OnAttackVillageOutTime(long lWarID);						//攻村时间到
	void OnAttackVillageEnd(long lWarID);							//攻村结束
	void OnClearPlayer(long lWarID);								//清除据点玩家

	void	InitiVillRegionState();

	long	GetWarRegionIDByTime(long lTime);
	long	GetVilRegionIDByTime(long lTime);
	bool	IsAlreadyDeclarForWar(long lWarID,const CGUID& FactionID);			//判断某个帮会是否已经宣战
	bool	GetWarNameForDeclar(const CGUID& FactionID,string &strName);		//得到报名战斗的名字

	long	GetWarStartTime(long lWarRegionID);
};

CVillageWarSys* GetVilWarSys();
