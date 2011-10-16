///////////////////////////////////////////////////////////////////////////////////////
//AttackCitySys.h
//攻城战系统
//Function:完成攻城战系统的时间控制等等
//Author:Joe
//Create Time:2004.7.29
#pragma once
#include "..\public\Timer.h"
#include "..\ServerCityRegion.h"
#include "..\public\Date.h"


typedef struct tagAttackCityTime
	{
		long	lTime;					//第几次
		long	lCityRegionID;			//城战地图
		ULONG	lDeclarWarEventID;
		tagTime DeclarWarTime;			//宣战时间
		ULONG	lStartInfoEventID;
		tagTime	AttackCityStartInfoTime;//攻城开始提示时间
		ULONG	lStartEventID;
		tagTime	AttackCityStartTime;	//攻城开始时间
		ULONG	lEndInfoEventID;
		tagTime	AttackCityEndInfoTime;	//攻城结束提示时间
		ULONG	lEndEventID;
		tagTime	AttackCityEndTime;		//攻城结束时间
		ULONG	lMassEventID;
		tagTime	MassTime;				//集结时间
		ULONG	lClearEventID;
		tagTime	ClearPlayerTime;		//清除城战区域的闲杂人时间
		ULONG	lRefreshEventID;
		tagTime	RefreshRegionTime;		//刷新场景时间

		eCityState	RegionState;		//这次战斗中当前场景所处的状态
		list<CGUID>	DecWarFactions;		//向这次城战宣战的帮会
	}tagACT;


class CAttackCitySys
{
public:
	CAttackCitySys(void);
	~CAttackCitySys(void);

public:
	
	struct tagGetTodayTaxTime
	{
		ULONG lEventID;
		long lHour;
		long lMinute;
		long lSecond;
	};

private:
	static map<long,tagACT>		m_Attacks;
	typedef map<long,tagACT>::iterator itAttack;

	static  tagGetTodayTaxTime m_GetTodayTaxTime;
	static CAttackCitySys* instance;											//该类唯一实例句柄

public:
	static CAttackCitySys* getInstance();										//得到唯一实例句柄	
	void   Release();															//释放资源

	bool DecordFromByteArray(BYTE* pByte, long& pos);

	bool UpdateApplyWarFacs(BYTE* pByte, long& pos);

public:
	void OnAttackCityStart(long lWarID);							//攻城开始
	void OnAttackCityTimeOut(long lWarID);							//攻城时间到
	void OnAttackCityEnd(long lWarID);								//攻城结束
	void OnDeclarWar(long lWarID);									//攻城宣战
	void OnMass(long lWarID);										//攻城集结
	void OnClearOtherPlayer(long lWarID);							//清除其它人员
	void OnRefreshRegion(long lWarID);								//刷新场景

	void	InitiCityRegionState();

	long GetWarRegionIDByTime(long lWarID);
	bool IsAlreadyDeclarForWar(long lWarID,const CGUID& FactionID);			//判断某个帮会是否已经宣战
	bool GetWarNameForDeclar(const CGUID& FactionID,string &strName);			//得到报名战斗的名字

	long GetCurTimeWarNumByRegionID(long lRegionID);				//得到当前时间的WarID 
	eCityState GetCityStateByWarNum(long lWarID);					//得到WarID当前状态

	long	GetWarStartTime(long lWarRegionID);
};
CAttackCitySys* GetAttackCitySys();
