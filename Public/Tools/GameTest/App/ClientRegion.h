
#pragma once

#include "region.h"

class CPlayer;
class CShape;
class CMoveShape;
class CFindPath;
class CGoods;
class CMonster;

enum REGION_TYPE
{
	RT_Normal=0,	//普通场景
	RT_Village,		//村战地图
	RT_City,		//城
};

// 客户端专用的场景类
class CClientRegion : public CRegion
{
public:
	CClientRegion(void);
	virtual ~CClientRegion(void);

	// DriveClient, Client to Test Tile Hight
	// by MartySa 2009.2.5
public:
	struct tagTilePoint
	{
		long lTilePosX;
		long lTilePosY;
		tagTilePoint() : lTilePosX(0), lTilePosY(0)
		{}		

		bool operator == (const tagTilePoint &tilepoint) const
		{	
			if ( &tilepoint == this )
			{
				return true; 
			}

			return ( tilepoint.lTilePosX == lTilePosX && tilepoint.lTilePosY == lTilePosY);
		}

		bool operator != (const tagTilePoint &tilepoint) const
		{	
			if ( &tilepoint == this )
			{
				return false; 
			}

			return ( tilepoint.lTilePosX != lTilePosX || tilepoint.lTilePosY != lTilePosY);
		}

		bool operator < (const tagTilePoint &tilepoint) const
		{
			if ( &tilepoint == this )
			{
				return false; 
			}

			if ( tilepoint.lTilePosX != lTilePosX )
			{
				return ( tilepoint.lTilePosX < lTilePosX );	
			}

			return ( tilepoint.lTilePosY < lTilePosY );	
		}
	};
private:
	//游戏场景格子高度管理
	map<tagTilePoint, long>		 m_mapTileHightManager;

public:
	void					  _AddTileHight(tagTilePoint tilepoint, long lHeight);
	map<tagTilePoint, long>	 *_GetTileHightManager() { return m_mapTileHightManager.size()>0 ? &m_mapTileHightManager : NULL; }
	void					  _ClearTileHightManager() { m_mapTileHightManager.clear(); }
	long					  _GetTileHightByTilePoint(tagTilePoint tilepoint);
	//end

private:
	long m_lTaxRate;									//税率
	REGION_TYPE  m_WarRegionType;						//战斗场景类型
	//////////////////////////////////////////////////////////////////////////
	//	接口
	//////////////////////////////////////////////////////////////////////////
public:
	virtual BOOL Load();		// 读取

	void	RemoveObject(CBaseObject* pObject);						//移出Object

	bool	AddGoods(CGoods* pGoods,DWORD dwPos,LONG lExtendID);
	CGoods*	RemoveGoods(CGUID& guid, DWORD dwPos, LONG lExtendID, DWORD dwAmount);
	bool	ClearGoods();

	//得到一个中心点附近没有MoveShape的格子
	void	GetNoShapeCell(long lCenterX,long lCenterY,long lRadius,long lFirstDir,long& lX,long& lY);
	//得到空间某个方向上最少moveshape的格子
	void	GetLeastShapesCellFromSpace(int nObjCenterX,int nObjCenterY, int nObjFigureX,int nObjFigureY,int nMaxDistance,int nDiretion, long& lX,long& lY);
	//得到一个圆形空间里某个方向上最少moveshape的格子
	void	GetLeastShapesCellFromCircularSpace(int nObjCenterX,int nObjCenterY,
		int nObjFigureX,int nObjFigureY,int nMaxDistance,float fDir,
		long& lX,long& lY);

	//---------------------------------------
	//新加shape管理接口
	//  [12/6/2008 Square]
	CShape*	GetShape(long x, long y);							//某点上的Shape
	CGoods* GetGoods(long x, long y);
	CPlayer* GetPlayer(long x,long y);
	CMonster* GetMonster(long x,long y);
	CGoods* GetRegionGoods();				//从场景对象列表中取一个Goods,
	CShape* GetShapeByShowName(const char* strShowName);

	//获取一个点的指定范围内是否有Shape
	CShape* GetShapeFromSpace(long x, long y, long lSize, OBJECT_TYPE eType, CShape* pShape = NULL);		//获取区域范围内的Shape

	//得到一个不等于pPlayer的对象
	CShape* GetAimShape(CPlayer* pPlayer);

	CShape* GetShapeByType(OBJECT_TYPE eType,CPlayer* pSelf);

	//税率
	void SetTaxRate(long l)		{ m_lTaxRate = l; }
	long GetTaxRate()			{return m_lTaxRate;}
	//场景类型
	void SetWarRegionType(REGION_TYPE eType) {m_WarRegionType = eType;}
	REGION_TYPE GetWarRegionType()		{ return m_WarRegionType;}

	//	
	CPlayer * FindTradeAnswer(CPlayer* pSelf);
	//////////////////////////////////////////////////////////////////////////
	//	寻路模块
	//////////////////////////////////////////////////////////////////////////
public:
	CFindPath* GetFindPath();
private:
	CFindPath *	m_pFindPath;	// 寻路模块
	//取对象
public:
	//////////////////////////////////////////////////////////////////////////
	struct tagNpc{
		char strName[256];
		long	lPosX;
		long	lPosY;
		tagNpc(void) 
		{
			memset(this,0,sizeof(tagNpc));
		}
	};
	list<tagNpc*>*	GetNpcList()	{return &m_listNpc;}
	//	map<string,tagNpc*>* GetMapNpcList()	{ return &m_mapNpcList; }
private:
	//npc列表
	list<tagNpc*>	m_listNpc;
	bool	m_bNpcListLoaded;

	//  [8/11/2009 Square]
public:
	void AddObject(CBaseObject* pObject);
	CShape* GetNearestShapeByType(OBJECT_TYPE,CPlayer*);
};
