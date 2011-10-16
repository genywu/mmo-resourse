#pragma once

#include "Monster.h"
#include "NPC.h"
#include "Collection.h"
#include "SignPos.h"
#include "Roads.h"
#include <fstream>
#include <string>
#include <map>
using namespace std;

typedef vector<CMonster *>			VECMONSTER;
typedef VECMONSTER::iterator		VECMONSTERITR;
typedef vector<CMonsterInfo *>		VECMONSTERINFO;
typedef VECMONSTERINFO::iterator	VECMONSTERINFOITR;
typedef vector<CNPC *>				VECNPC;
typedef VECNPC::iterator			VECNPCITR;
typedef vector<CCollection *>		VECCOLLECTION;
typedef VECCOLLECTION::iterator		VECCOLLECTIONITR;
typedef vector<CCollectionInfo *>	VECCOLLINFO;
typedef	VECCOLLINFO::iterator		VECCOLLINFOITR;
typedef vector<CSignPos *>			VECSIGNPOS;
typedef VECSIGNPOS::iterator		VECSIGNPOSITR;
typedef map<int,CRoads *>			MAPROADS;
typedef MAPROADS::iterator			MAPROADSITR;
typedef MAPROADS::reverse_iterator	MAPROADSREVERSEITR;
typedef vector<CRoads::tagRoadsPoint>	VECROADPOINT;
typedef VECROADPOINT::iterator		VECROADPOINTITR;

// 基本格子结构
typedef struct tagCell
{
	friend ifstream &operator>>(ifstream &rstream, tagCell *prCell);

	unsigned short	bBlock		:3;		// 阻挡（0：可行走  1：可飞行  2：不可飞行）
	unsigned short	bSecurity	:3;		// 安全区
	unsigned short	lWar		:2;		// 城战区
	unsigned short	lMaterial	:8;		// 材质
	WORD			lSwitch;			// 切换点(编号从1开始)
}Cell;

// 切换点
typedef struct tagSwitch
{
	friend ifstream &operator>>(ifstream &rstream, tagSwitch *prSwtich);

	long			lState;				// 切换状态
	long			lRegionID;			// 出口点ID号
	long			lCoordX;			// 出口点坐标
	long			lCoordY;
	long			lDir;				// 出口点方向
}Switch;

class CRegion
{
public:
	CRegion(void);
public:
	~CRegion(void);
	BOOL Release();

public:
	// 读地图场景文件 "*.rgn"
	friend ifstream &operator>>(ifstream &rstream, CRegion *pregion);

// 数据读写
public:
	const inline unsigned GetRegionNo()const				{return m_uRegionNo;}
	inline void SetRegionNo(const unsigned uNo)				{m_uRegionNo = uNo;}
	const inline CString GetRegionName()const				{return m_szRegionName;}
	inline void SetRegionName(const CString szName)			{m_szRegionName = szName;}
	const inline string &GetRgn()const						{return m_strCls_Rgn;}
	const inline long Width()const							{return m_lWidth;}
	const inline long Height()const							{return m_lHeight;}
	const inline long CellCount()const						{return m_lWidth * m_lHeight;}
	inline Cell* GetCell()const								{return m_pCell;}
	const inline long SwitchCount()const					{return m_lSwitchCount;}
	inline CPoint* GetSwitch()const							{return m_ptSwitch;}
	const inline long SwitchPoint()const					{return m_lSwitchPoint;}
	VECMONSTER &GetMonster();
	VECNPC &GetNPC();
	VECCOLLECTION &GetCollection();
	VECSIGNPOS &GetSignPos();
	MAPROADS &GetRoads();
	void WriteToVecDisPlay(long lType,long lValue);	
	bool CheckMonsterNo(const unsigned uNo, unsigned &uInd);			// 校验MonsterNo信息
	bool CheckCollectionNo(const unsigned uNo, unsigned &uInd);			// 校验CollectionNo信息
protected:
	unsigned		m_uRegionNo;		// 场景编号
	CString			m_szRegionName;		// 场景名称
	string			m_strCls_Rgn;		// 7 Characters
	long			m_lVersion;			// Version
	long			m_lType;			// Type
	long			m_lWidth;			// Region width
	long			m_lHeight;			// Region Height
	Cell*			m_pCell;			// Region cell pointer
	long			m_lSwitchCount;		// Switch count
	CPoint*			m_ptSwitch;			// Switch coordinate
	long			m_lSwitchPoint;		// Switch point
	Switch*			m_pSwitch;			// Switch pointer

	VECMONSTER		m_vecMonster;		// 当前场景怪物列表
	VECNPC			m_vecNPC;			// 当前场景NPC列表
	VECCOLLECTION	m_vecCollection;	// 当前场景采集物列表
	VECSIGNPOS		m_vecSignPos;		//当前场景路标点
	MAPROADS		m_mapRoads;			//当前场景路径标识

	vector<long>	m_vecMDisplay;		//数量列表框中已先项目的怪物区域信息
	vector<long>	m_vecCDisplay;		//数量列表框中已先项目的采集物区域信息

};

// 读地图场景文件 "*.rgn"
ifstream &operator>>(ifstream &rstream, tagCell *prCell);
ifstream &operator>>(ifstream &rstream, tagSwitch *prSwitch);
ifstream &operator>>(ifstream &rstream, CRegion *pregion); 
