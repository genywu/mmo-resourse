//**************************************************
//		文件名 ：Colony.h
//		作者   ：heliangchen
//		时间   ：08/4/14
//		简介   ：monster集群管理类	被ServerRegion管理。
//				与AI一起 实现怪物群体行为
//***************************************************

#pragma once

#include "BaseObject.h"

const int COLONY_LEADER_DISTANCE_MAX=  3;
const int HURT_KEEP_TIME= 5000;
const int TRACING_DISTANCE= 15;

class CColony:public CBaseObject
{
public:
	CColony();
	~CColony();
public:
	//和平状态
	void OnPeaceState(CMoveShape* pMonster);
	//战斗状态
	void OnFightState(CMoveShape* pMonster);
	//当被杀死的时候
	void WhenBeenKilled(CMoveShape* pMonster);
	//当出生的时候
	void WhenBorn(CMoveShape* pMonster);
	//处理战斗状态转化为和平状态
	BOOL CheckReturnPeace();
	//添加删除成员
	BOOL AddMember(BOOL bIsLeader, CMonster* pMonster);
	BOOL DelMember(CMonster* pMonster);
	//添加目标
	//void AddTarget(CMonster* pMonster);
	//void AddTarget(int iType, const CGUID& guid);
	//设置id
	void SetID(DWORD ID){m_ID= ID;}
	DWORD GetID(){return m_ID;}
	//是否有目标
	BOOL HasTarget(){return m_mapTargetList.size()!=0;}
	//是否有路径
	BOOL HasRoad();
	//成员死亡
	//BOOL MenberDied(CMoveShape* pMoveshape);
	//map<CGUID,OBJECT_TYPE>& GetTargetList(){return m_mapTargetList;}
	//取得实际首领目标
	CMoveShape* GetLeaderTarget();
	//取得实际领袖
	CMonster* GetLeader();
	//取得目标列表
	BOOL GetTargetList(vector<CMoveShape*>& vecTarget);
	//取得成员列表
	BOOL GetMemberList(vector<CMonster*>& vecMember);
	//取得战斗成员列表
	BOOL GetFightMemberList(vector<CMonster*>& vecMember);
	//取得目标坐标
	POINT GetAimPoint(const CGUID& guid);
	//取得领导的坐标
	POINT GetLeaderPoint();
	//是否有实际领导
	BOOL HaveFactLeader(){return m_guidFactLeader!= NULL_GUID;}
protected:
	//丢掉所有目标
	void SearchTarget();
	//void LoseAllTarget();
	void CountLeaderAim();
	void CountMemberAim();
	void CountOneMenberAim(int& AimX,int &AimY, int OppoX,int OppoY,int DirDis);
	enum //顺路径寻路的返回类型
	{
		NEXTSTEP_RESULT_NULL=0,	//下一步
		NEXTSTEP_RESULT_NEXT,	//下一个节点，不是首尾节点
		NEXTSTEP_RESULT_RETURN,	//到首尾节点时候转向
		NEXTSTEP_RESULT_NEXTCYCLE,//当路径首尾相连的时候，当走到尾点时候，转到下个周期的头节点
	};
	int RunByRoad(LONG &lX, LONG &lY,INT & CurrentIndex);
	void SetDirPos(int x,int y, int dir);
	void MoveTo(int x, int y);
protected:
	LONG m_ID;
	//坐标
	LONG m_iX;
	LONG m_iY;
	//方向
	LONG m_iDir;
	//初始方向
	int m_iInitializeDir;

	struct  tgMember
	{
		tgMember()
		{
			MemType= 0;
			MemGuid= NULL_GUID;
			AimX= 0;
			AimY= 0;
			OppoX= 0;
			OppoY= 0;
		}
		void Clean()
		{
			MemType= 0;
			MemGuid= NULL_GUID;
			AimX= 0;
			AimY= 0;
			OppoX= 0;
			OppoY= 0;
		}
		//成员类型
		int		MemType;
		//成员id
		CGUID	MemGuid;
		//成员目标坐标
		int		AimX;
		int		AimY;
		//成员相对（首领）坐标
		int		OppoX;
		int		OppoY;
	};
	//目标信息
	struct STargetInfo
	{
		STargetInfo(){type=(OBJECT_TYPE)0;}
		OBJECT_TYPE type;
	};
	//领导
	tgMember m_tgLeader;
	//成员列表
	map<CGUID,tgMember> m_mapMember;
	//战斗成员列表
	map<CGUID,OBJECT_TYPE> m_mapFightMember;
	//逻辑领导
	CGUID m_guidFactLeader;
	//目标列表
	map<CGUID,STargetInfo> m_mapTargetList;
	//路径
	vector<POINT> m_vectRoad;
	//路径方向
	int m_iRoadDirect;
	//行走点记录
	int m_iRoadRecord;

};