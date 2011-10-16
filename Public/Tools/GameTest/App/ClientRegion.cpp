#include "StdAfx.h"


#include "clientregion.h"
#include "player.h"
#include "monster.h"
#include "goods.h"

#include "..\GameTest\resource.h"
#include "..\GameTest\GameTestDlg.h"
#include "..\Script\ScriptSys.h"
#include "../Utility/FindPath.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long AREA_WIDTH = 15;
long AREA_HEIGHT = 15;


CClientRegion::CClientRegion(void)
{
	m_WarRegionType = RT_Normal;
	m_bNpcListLoaded = false;
	m_pFindPath = NULL;

	m_bNpcListLoaded = false;
}

CClientRegion::~CClientRegion(void)
{
	SAFE_DELETE(m_pFindPath);
	if( !m_listNpc.empty())
	{
		list<tagNpc*>::iterator it = m_listNpc.begin();
		for(;it!=m_listNpc.end();)
		{
			tagNpc* pNpc = (*it);
			SAFE_DELETE(pNpc);
			m_listNpc.erase(it++);
		}
		m_listNpc.clear();
	}





}

// 移出OBJECT
void CClientRegion::RemoveObject(CBaseObject* pObject)
{
	if( pObject == NULL )	return;

	CMoveShape* pShape = dynamic_cast<CMoveShape*>(pObject);

	if( pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER )
		DecCellObjNum(pShape->GetTileX(),pShape->GetTileY());

	CRegion::RemoveObject(pObject);
}

//一个点上是否是Goods
CGoods* CClientRegion::GetGoods(long x, long y)
{
	for(list<CBaseObject*>::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CGoods * pGoods = (CGoods*)(*it);
		if ( pGoods && pGoods->GetType() == TYPE_GOODS 
			&& pGoods->GetTileX() == x && pGoods->GetTileY() == y )
			return pGoods;
	}

	return NULL;
}

CPlayer* CClientRegion::GetPlayer(long x, long y)
{
	for(list<CBaseObject*>::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CPlayer * pPlayer = (CPlayer*)(*it);
		if ( pPlayer && pPlayer->GetType() == TYPE_PLAYER 
			&& pPlayer->GetTileX() == x && pPlayer->GetTileY() == y )
			return pPlayer;
	}

	return NULL;
}

CMonster* CClientRegion::GetMonster(long x, long y)
{
	for(list<CBaseObject*>::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CMonster * pMonster = (CMonster*)(*it);
		if ( pMonster && pMonster->GetType() == TYPE_MONSTER 
			&& pMonster->GetTileX() == x && pMonster->GetTileY() == y )
			return pMonster;
	}
	return NULL;
}

// 获取一个点上是否有Shape
CShape* CClientRegion::GetShape(long x, long y)
{
	for(list<CBaseObject*>::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it)
	{
		CShape *pShape = (CShape*)(*it);
		if (pShape &&
			(pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER || pShape->GetType() == TYPE_GOODS || pShape->GetType() == TYPE_COLLECTION) &&
			pShape->GetTileX() == x && pShape->GetTileY() == y )
		{
			return pShape;
		}
	}
	return NULL;
}
//从场景对象列表中取一个Goods出来
CGoods* CClientRegion::GetRegionGoods()
{
	for( list<CBaseObject*>::iterator it = m_listObject.begin();it != m_listObject.end();it++)
	{
		CGoods* pGoods = (CGoods*)(*it);
		if( pGoods && pGoods->GetType() == TYPE_GOODS)
			return pGoods;
	}
	return NULL;
}
CShape* CClientRegion::GetShapeByShowName(const char* strShowName)
{
	if( strShowName != NULL)
	{
		for( list<CBaseObject*>::iterator it = m_listObject.begin();it != m_listObject.end();it++)
		{
			CShape* pShape = (CShape*)(*it);
			if( pShape && 
				strcmp(pShape->GetName(),strShowName) == 0 )
				return pShape;
		}
	}
	return NULL;
}

CPlayer* CClientRegion::FindTradeAnswer(CPlayer* pSelf)
{
	list<CBaseObject*>::iterator it = m_listObject.begin();
	for(;it != m_listObject.end();++it)
	{
		CPlayer* pPlayer = (CPlayer*)(*it);
		if(pPlayer&&pPlayer!=pSelf&&pPlayer->GetType() == TYPE_PLAYER)
		{
	//		if(!pPlayer->IsRequestTrade())
	//			return pPlayer;
		}
	}
	return NULL;
}

CShape* CClientRegion::GetShapeByType(OBJECT_TYPE eType,CPlayer* pSelf)
{
	list<CBaseObject*>::iterator it = m_listObject.begin();
	for(;it != m_listObject.end();++it)
	{
		CShape* pShape = (CShape*)(*it);
		if(pShape && pShape != pSelf && pShape->GetType() == eType )
		{
			if( eType == TYPE_MONSTER )
			{
				bool b = ((CMonster*)pShape)->IsCanTalk() == 0 ? false:true;
				if( b == false )
					return pShape;
				else
					continue;
			}
			return pShape;
		}
	}
	return NULL;
}

CShape* CClientRegion::GetAimShape(CPlayer* pPlayer)
{
	list<CBaseObject*>::iterator it = m_listObject.begin();
	for(;it != m_listObject.end();++it)
	{
		CShape* pShape = (CShape*)(*it);
		if( pShape && pShape->GetExID() != pPlayer->GetExID() )
		{
			long lType = pShape->GetType();
			if( lType == TYPE_MONSTER )
			{
				bool b = ((CMonster*)pShape)->IsCanTalk() == 0 ? false:true;
				if( b == false )
					return pShape;
				else
					continue;
			}
			return pShape;
		}
	}
	return NULL;
}

// 获取一个点的指定范围内是否有Shape
CShape* CClientRegion::GetShapeFromSpace(long x, long y, long lSize,OBJECT_TYPE eType, CShape* pShape)
{
	for (int i = x-lSize; i < x+lSize; ++i)
	{
		for (int j = y-lSize; j <y+lSize; ++j)
		{
			CShape *pOtherShape = GetShape(i,j);
			if(pOtherShape && pShape != pOtherShape && pOtherShape->GetType() == eType )
				return pOtherShape;
		}
	}
	return NULL;
}

// 读取
BOOL CClientRegion::Load()
{
	CRegion::Load();
	return true;
}

CFindPath* CClientRegion::GetFindPath()
{
	if (m_pFindPath == NULL)
		m_pFindPath = new CFindPath(this);
	return m_pFindPath;
}

//增加一个物品
bool CClientRegion::AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID)
{
	{
		char strGuid[128],strOut[128];
		pGoods->GetExID().tostring(strGuid);
		sprintf(strOut,"region add goods guid:%s",strGuid);
		//PutDebugString(strOut);
	}

	AddObject(pGoods);
	long x = dwPos%GetWidth();
	long y = dwPos%GetWidth();
	pGoods->SetPosXY(x+0.5f,y+0.5f);

	return true;
}

CGoods * CClientRegion::RemoveGoods(CGUID& guID,DWORD dwPos,LONG lExtendID,DWORD dwAmount)
{
	CGoods *pGoods=dynamic_cast<CGoods *>(FindChildGoods(guID));
	if(pGoods==NULL)
	{
		long x=dwPos%GetWidth(),y=dwPos/GetWidth();
		for(list<CBaseObject*>::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it)
		{
			pGoods=dynamic_cast<CGoods *>((*it));
			if ( pGoods && pGoods->GetTileX() == x && pGoods->GetTileY()==y )
			{
				break;
			}
			pGoods=NULL;
		}
	}
	if( pGoods==NULL )
	{
		return NULL;
	}

	RemoveObject(pGoods);
	return pGoods;
}

//得到一个中心点附近没有MoveShape的格子
void CClientRegion::GetNoShapeCell(long lCenterX,long lCenterY,long lRadius,long lFirstDir,long& lX,long& lY)
{
	static long pos[4][2] = {-1,-1, 1,-1, 1,1, -1,1};
	//各个方向的面向坐标
	static int facedir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };

	lX = lCenterX;
	lY = lCenterY;
	//判断是否有阻挡，并且该点上没有对象
	if( GetBlock(lCenterX,lCenterY) == CRegion::BLOCK_NO &&
		GetShape(lCenterX,lCenterY) == NULL)
		return;
	//当半径每增加1的时候,向外扩展一层
	long lTemptX = 0;
	long lTemptY = 0;
	long lStartDir = facedir[lFirstDir]/2;
	for(int i=1;i<=lRadius;i++)
	{	
		long lDir = lStartDir;
		for(int j=0;j<4;j++)
		{
			lTemptX = lCenterX+pos[lDir][0];
			lTemptY = lCenterY+pos[lDir][1];
			int count = pow((float)2,i);
			for(int n=0;n<count;n++)
			{
				if( GetBlock(lTemptX,lTemptY) == CRegion::BLOCK_NO &&
					GetShape(lTemptX,lTemptY) == NULL)
				{
					lX = lTemptX;
					lY = lTemptY;
					return;
				}
				if(lDir==0)
					lTemptX++;
				else if(lDir==1)
					lTemptY++;
				else if(lDir==2)
					lTemptX--;
				else if(lDir==3)
					lTemptY--;
			}

			lDir = (lDir++)%4;
		}
	}
}

//得到一个空间里某方向上的一个格子，该格子上的MoveShape对象最少
void CClientRegion::GetLeastShapesCellFromSpace(int nObjCenterX,int nObjCenterY,
												int nObjFigureX,int nObjFigureY,int nMaxDistance,int nDiretion,
												long& lX,long& lY)
{
	//各个方向的面向坐标
	static int facedir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	//方向坐标的偏移正负值
	static int DirOff[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	//不同方向前进求值的两个方向
	static int forward[8][2] = {6,2, 2,0, 0,4, 2,4, 2,6, 4,6, 4,0, 6,0};

	//最大步数
	int nStep = 0;
	if(nDiretion==0 || nDiretion==4)
		nStep = nMaxDistance*2+1+nObjFigureX;
	else if(nDiretion==2 || nDiretion==6)
		nStep = nMaxDistance*2+1+nObjFigureY;
	else
		nStep = min(nObjFigureX,nObjFigureY)+nMaxDistance*2+1;

	int nCount0 = nMaxDistance;
	int nCount1 = nMaxDistance;

	int nLenX = nMaxDistance+nObjFigureX;
	int nLenY = nMaxDistance+nObjFigureY;
	//循环求值
	int nFirstX = nObjCenterX+DirOff[facedir[nDiretion]][0]*nLenX;
	int nFirstY = nObjCenterY+DirOff[facedir[nDiretion]][1]*nLenY;

	int nLastX = nObjCenterX+DirOff[nDiretion][0]*nLenX;
	int nLastY = nObjCenterY+DirOff[nDiretion][1]*nLenY;

	long lLeastShapeNum = 0xFFFFFF;
	long lLestX = 0;
	long lLeasY = 0;
	for(int i=0;i<nStep;i++)
	{
		int nBeginX = nFirstX+DirOff[nDiretion][0]*i;
		int nBeginY = nFirstY+DirOff[nDiretion][1]*i;
		//察看是否有阻挡
		if( GetBlock(nBeginX,nBeginY)== BLOCK_NO )
		{
			//察看是否有MoveShape
			long lShapeNum = GetCellObjNum(nBeginX,nBeginY);
			if( lShapeNum<= 0)
			{
				lX = nBeginX;lY=nBeginY;
				return;
			}
			else if(lShapeNum < lLeastShapeNum)
			{
				lLeastShapeNum = lShapeNum;
				lLestX = nBeginX;lLeasY = nBeginY;
			}
		}

		//循环个数
		if(nDiretion%2 != 0)
		{
			nCount0 = abs(nLastX-nBeginX);
			nCount1 = abs(nLastY-nBeginY);
		}
		for(int i0=1,i1=1;i0<=nCount0 || i1<=nCount1;i0++,i1++)
		{
			if(i0<=nCount0)
			{
				int nTemptX0 = nBeginX+DirOff[forward[nDiretion][1]][0]*i0;
				int nTemptY0 = nBeginY+DirOff[forward[nDiretion][1]][1]*i0;
				//察看是否有阻挡
				if( GetBlock(nTemptX0,nTemptY0)== BLOCK_NO )
				{
					//察看是否有MoveShape
					long lShapeNum = GetCellObjNum(nTemptX0,nTemptY0);
					if( lShapeNum<= 0)
					{
						lX = nTemptX0;lY=nTemptY0;
						return;
					}
					else if(lShapeNum < lLeastShapeNum)
					{
						lLeastShapeNum = lShapeNum;
						lLestX = nTemptX0;lLeasY = nTemptY0;
					}
				}
			}
			if(i1<=nCount1)
			{
				int nTemptX1 = nBeginX+DirOff[forward[nDiretion][0]][0]*i1;
				int nTemptY1 = nBeginY+DirOff[forward[nDiretion][0]][1]*i1;
				//察看是否有阻挡
				if( GetBlock(nTemptX1,nTemptY1)== BLOCK_NO )
				{
					//察看是否有MoveShape
					long lShapeNum = GetCellObjNum(nTemptX1,nTemptY1);
					if( lShapeNum<= 0)
					{
						lX = nTemptX1;lY=nTemptY1;
						return;
					}
					else if(lShapeNum < lLeastShapeNum)
					{
						lLeastShapeNum = lShapeNum;
						lLestX = nTemptX1;lLeasY = nTemptY1;
					}
				}
			}
		}
	}

	lX = lLestX;lY = lLeasY;
}

void CClientRegion::GetLeastShapesCellFromCircularSpace(int nObjCenterX,int nObjCenterY, int nObjFigureX,int nObjFigureY,int nMaxDistance,float fDir, long& lX,long& lY)
{
	//方向坐标的偏移正负值
	static float fDirOffSet[9][2] = {
		0.0,0.0, 
		(D3DX_PI/8),-(D3DX_PI/8), 
		(D3DX_PI/4),-(D3DX_PI/4), 
		(D3DX_PI*3/8),-(D3DX_PI*3/8), 
		(D3DX_PI/2),-(D3DX_PI/2),
		(D3DX_PI*5/8),-(D3DX_PI*5/8),
		(D3DX_PI*3/4),-(D3DX_PI*3/4),
		(D3DX_PI*7/8),-(D3DX_PI*7/8),
		D3DX_PI,D3DX_PI
	};
	float fMaxDist = (float)(nMaxDistance+min(nObjFigureX,nObjFigureY));
	long lLeastShapeNum = 0xFFFFFF;
	// 正负偏移坐标临时变量
	int lShapeX = nObjCenterX,lShapeY = nObjCenterY;
	while (fMaxDist > 0.0)
	{
		// 当前的弧度
		float fDiretion0 = 0.0;
		float fDiretion1 = 0.0;
		for (int i=0; i<9; ++i)
		{
			fDiretion0 = fDirOffSet[i][0] + fDir;
			fDiretion1 = fDirOffSet[i][1] + fDir;
			// 计算出坐标正偏移
			int nTemptX0 = (int)(nObjCenterX - fMaxDist * cosf(fDiretion0));
			int nTemptY0 = (int)(nObjCenterY - fMaxDist * sinf(fDiretion0));
			// 计算出坐标负偏移
			int nTemptX1 = (int)(nObjCenterX - fMaxDist * cosf(fDiretion1));
			int nTemptY1 = (int)(nObjCenterY - fMaxDist * sinf(fDiretion1));
			//察看是否有阻挡
			if( GetBlock(nTemptX0,nTemptY0)== BLOCK_NO ||
				GetBlock(nTemptX0,nTemptY0)== BLOCK_AIM )
			{
				//察看是否有MoveShape
				long lShapeNum0 = GetCellObjNum(nTemptX0,nTemptY0);
				if( lShapeNum0<= 0)
				{
					lX = nTemptX0;lY=nTemptY0;
					return;
				}
				else if(lShapeNum0 < lLeastShapeNum)
				{
					lLeastShapeNum = lShapeNum0;
					lShapeX = nTemptX0;lShapeY = nTemptY0;
				}
			}
			//察看是否有阻挡
			if( GetBlock(nTemptX1,nTemptY1)== BLOCK_NO ||
				GetBlock(nTemptX1,nTemptY1)== BLOCK_AIM )
			{
				//察看是否有MoveShape
				long lShapeNum1 = GetCellObjNum(nTemptX1,nTemptY1);
				if( lShapeNum1<= 0)
				{
					lX = nTemptX1;lY=nTemptY1;
					return;
				}
				else if(lShapeNum1 < lLeastShapeNum)
				{
					lLeastShapeNum = lShapeNum1;
					lShapeX = nTemptX1;lShapeY = nTemptY1;
				}
			}
		}
		fMaxDist -= 0.5;
	}
	lX = lShapeX;lY = lShapeY;
}

//添加一个格子数据到管理器
void CClientRegion::_AddTileHight(tagTilePoint tilepoint, long lHeight)
{
	map<tagTilePoint, long>::iterator it = m_mapTileHightManager.find(tilepoint);
	if ( it!= m_mapTileHightManager.end() )
	{	
		m_mapTileHightManager.insert(make_pair(tilepoint, lHeight));
	}
}

//根据传入的格子坐标得到当前格子的高度值
long CClientRegion::_GetTileHightByTilePoint(tagTilePoint tilepoint)
{	
	map<tagTilePoint, long>::iterator it = m_mapTileHightManager.begin();
	for ( ; it!=m_mapTileHightManager.end(); ++it)
	{
		if ( (*it).first == tilepoint )
		{
			return (*it).second;
		}
	}
	return -1;
}

//  [8/11/2009 Square]
void CClientRegion::AddObject(CBaseObject* pObject)
{
	if( !FindChildObject(pObject) )
	{
		CBaseObject::AddObject(pObject);
	}
}


/*
=========================================================================
函数功能:重载初始化函数    BY:LGR
=========================================================================
*/
CShape* CClientRegion::GetNearestShapeByType(OBJECT_TYPE eType,CPlayer* pSelf)
{
	int distance = 100,localdis = 0;
	CShape* reCShape = NULL;	
	list<CBaseObject*>::iterator it = m_listObject.begin();
	for(;it != m_listObject.end();++it)
	{
		CShape* pShape = (CShape*)(*it);
		if(pShape && pShape != pSelf && pShape->GetType() == eType )
		{
			if( eType == TYPE_MONSTER )
			{
				bool b = ((CMonster*)pShape)->IsCanTalk() == 0 ? false:true;
				if( b == false )
				{
					int d = pSelf->Distance(pShape);
					if (distance>d )
					{
						reCShape = pShape;
						distance = d ;
					}
				}
			}
		}
	}
	if (reCShape)
		return reCShape;
	else
		return NULL;
}