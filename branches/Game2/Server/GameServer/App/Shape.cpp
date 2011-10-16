#include "StdAfx.h"
#include "shape.h"
#include "ServerRegion.h"
#include "Area.h"
#include "Monster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long _dir[8][2] =
{
	0,-1,	1,-1,	1,0,	1,1,
	0,1,	-1,1,	-1,0,	-1,-1
};

// 8个方向的滑动顺序
long _slip_order[8][8] =
{
	0, 7, 1, 6, 2, 5, 3, 4,
	1, 0, 2, 7, 3, 6, 4, 5,
	2, 1, 3, 0, 4, 7, 5, 6,
	3, 2, 4, 1, 5, 0, 6, 7,
	4, 3, 5, 2, 6, 1, 7, 0,
	5, 4, 6, 3, 7, 2, 0, 1,
	6, 5, 7, 4, 0, 3, 1, 2,
	7, 6, 0, 5, 1, 4, 2, 3
};

// 在移动时需要检测的前方格子列表[figure][dir]
list<POINT> s_listMoveCheckCell[3][8];

void CShape::InitMoveCheckCellList()
{
	POINT pt = {0,0};

	// 体形0 1*1
	pt.x = 0;	pt.y = -1;	s_listMoveCheckCell[0][0].push_back(pt);
	pt.x = 1;	pt.y = -1;	s_listMoveCheckCell[0][1].push_back(pt);
	pt.x = 1;	pt.y = 0;	s_listMoveCheckCell[0][2].push_back(pt);
	pt.x = 1;	pt.y = 1;	s_listMoveCheckCell[0][3].push_back(pt);
	pt.x = 0;	pt.y = 1;	s_listMoveCheckCell[0][4].push_back(pt);
	pt.x = -1;	pt.y = 1;	s_listMoveCheckCell[0][5].push_back(pt);
	pt.x = -1;	pt.y = 0;	s_listMoveCheckCell[0][6].push_back(pt);
	pt.x = -1;	pt.y = -1;	s_listMoveCheckCell[0][7].push_back(pt);

	// 体形1 3*3
	pt.x = -1;	pt.y = -2;	s_listMoveCheckCell[1][0].push_back(pt);
	pt.x = 0;	pt.y = -2;	s_listMoveCheckCell[1][0].push_back(pt);
	pt.x = 1;	pt.y = -2;	s_listMoveCheckCell[1][0].push_back(pt);

	pt.x = 0;	pt.y = -2;	s_listMoveCheckCell[1][1].push_back(pt);
	pt.x = 1;	pt.y = -2;	s_listMoveCheckCell[1][1].push_back(pt);
	pt.x = 2;	pt.y = -2;	s_listMoveCheckCell[1][1].push_back(pt);
	pt.x = 2;	pt.y = -1;	s_listMoveCheckCell[1][1].push_back(pt);
	pt.x = 2;	pt.y = 0;	s_listMoveCheckCell[1][1].push_back(pt);

	pt.x = 2;	pt.y = -1;	s_listMoveCheckCell[1][2].push_back(pt);
	pt.x = 2;	pt.y = 0;	s_listMoveCheckCell[1][2].push_back(pt);
	pt.x = 2;	pt.y = 1;	s_listMoveCheckCell[1][2].push_back(pt);

	pt.x = 2;	pt.y = 0;	s_listMoveCheckCell[1][3].push_back(pt);
	pt.x = 2;	pt.y = 1;	s_listMoveCheckCell[1][3].push_back(pt);
	pt.x = 2;	pt.y = 2;	s_listMoveCheckCell[1][3].push_back(pt);
	pt.x = 1;	pt.y = 2;	s_listMoveCheckCell[1][3].push_back(pt);
	pt.x = 0;	pt.y = 2;	s_listMoveCheckCell[1][3].push_back(pt);

	pt.x = -1;	pt.y = 2;	s_listMoveCheckCell[1][4].push_back(pt);
	pt.x = 0;	pt.y = 2;	s_listMoveCheckCell[1][4].push_back(pt);
	pt.x = 1;	pt.y = 2;	s_listMoveCheckCell[1][4].push_back(pt);

	pt.x = 0;	pt.y = 2;	s_listMoveCheckCell[1][5].push_back(pt);
	pt.x = -1;	pt.y = 2;	s_listMoveCheckCell[1][5].push_back(pt);
	pt.x = -2;	pt.y = 2;	s_listMoveCheckCell[1][5].push_back(pt);
	pt.x = -2;	pt.y = 1;	s_listMoveCheckCell[1][5].push_back(pt);
	pt.x = -2;	pt.y = 0;	s_listMoveCheckCell[1][5].push_back(pt);

	pt.x = -2;	pt.y = 1;	s_listMoveCheckCell[1][6].push_back(pt);
	pt.x = -2;	pt.y = 0;	s_listMoveCheckCell[1][6].push_back(pt);
	pt.x = -2;	pt.y = -1;	s_listMoveCheckCell[1][6].push_back(pt);

	pt.x = -2;	pt.y = 0;	s_listMoveCheckCell[1][7].push_back(pt);
	pt.x = -2;	pt.y = -1;	s_listMoveCheckCell[1][7].push_back(pt);
	pt.x = -2;	pt.y = -2;	s_listMoveCheckCell[1][7].push_back(pt);
	pt.x = -1;	pt.y = -2;	s_listMoveCheckCell[1][7].push_back(pt);
	pt.x = 0;	pt.y = -2;	s_listMoveCheckCell[1][7].push_back(pt);

	// 体形2 5*5
	pt.x = -2;	pt.y = -3;	s_listMoveCheckCell[2][0].push_back(pt);
	pt.x = -1;	pt.y = -3;	s_listMoveCheckCell[2][0].push_back(pt);
	pt.x = 0;	pt.y = -3;	s_listMoveCheckCell[2][0].push_back(pt);
	pt.x = 1;	pt.y = -3;	s_listMoveCheckCell[2][0].push_back(pt);
	pt.x = 2;	pt.y = -3;	s_listMoveCheckCell[2][0].push_back(pt);

	pt.x = -1;	pt.y = -3;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 0;	pt.y = -3;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 1;	pt.y = -3;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 2;	pt.y = -3;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 3;	pt.y = -3;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 3;	pt.y = -2;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 3;	pt.y = -1;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 3;	pt.y = 0;	s_listMoveCheckCell[2][1].push_back(pt);
	pt.x = 3;	pt.y = 1;	s_listMoveCheckCell[2][1].push_back(pt);

	pt.x = 3;	pt.y = -2;	s_listMoveCheckCell[2][2].push_back(pt);
	pt.x = 3;	pt.y = -1;	s_listMoveCheckCell[2][2].push_back(pt);
	pt.x = 3;	pt.y = 0;	s_listMoveCheckCell[2][2].push_back(pt);
	pt.x = 3;	pt.y = 1;	s_listMoveCheckCell[2][2].push_back(pt);
	pt.x = 3;	pt.y = 2;	s_listMoveCheckCell[2][2].push_back(pt);

	pt.x = 3;	pt.y = -1;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 3;	pt.y = 0;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 3;	pt.y = 1;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 3;	pt.y = 2;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 3;	pt.y = 3;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 2;	pt.y = 3;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 1;	pt.y = 3;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = 0;	pt.y = 3;	s_listMoveCheckCell[2][3].push_back(pt);
	pt.x = -1;	pt.y = 3;	s_listMoveCheckCell[2][3].push_back(pt);

	pt.x = 2;	pt.y = 3;	s_listMoveCheckCell[2][4].push_back(pt);
	pt.x = 1;	pt.y = 3;	s_listMoveCheckCell[2][4].push_back(pt);
	pt.x = 0;	pt.y = 3;	s_listMoveCheckCell[2][4].push_back(pt);
	pt.x = -1;	pt.y = 3;	s_listMoveCheckCell[2][4].push_back(pt);
	pt.x = -2;	pt.y = 3;	s_listMoveCheckCell[2][4].push_back(pt);

	pt.x = 1;	pt.y = 3;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = 0;	pt.y = 3;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -1;	pt.y = 3;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -2;	pt.y = 3;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -3;	pt.y = 3;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -3;	pt.y = 2;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -3;	pt.y = 1;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -3;	pt.y = 0;	s_listMoveCheckCell[2][5].push_back(pt);
	pt.x = -3;	pt.y = -1;	s_listMoveCheckCell[2][5].push_back(pt);

	pt.x = -3;	pt.y = 2;	s_listMoveCheckCell[2][6].push_back(pt);
	pt.x = -3;	pt.y = 1;	s_listMoveCheckCell[2][6].push_back(pt);
	pt.x = -3;	pt.y = 0;	s_listMoveCheckCell[2][6].push_back(pt);
	pt.x = -3;	pt.y = -1;	s_listMoveCheckCell[2][6].push_back(pt);
	pt.x = -3;	pt.y = -2;	s_listMoveCheckCell[2][6].push_back(pt);

	pt.x = -3;	pt.y = 1;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -3;	pt.y = 0;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -3;	pt.y = -1;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -3;	pt.y = -2;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -3;	pt.y = -3;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -2;	pt.y = -3;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = -1;	pt.y = -3;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = 0;	pt.y = -3;	s_listMoveCheckCell[2][7].push_back(pt);
	pt.x = 1;	pt.y = -3;	s_listMoveCheckCell[2][7].push_back(pt);
}

CShape::CShape(void)
{
	SetRegionID(0);
	SetPosXY(0.5f,0.5f);
	SetDir(0);
	SetPos(0);
	SetState(STATE_PEACE);
	SetAction(ACT_STAND);
	m_CurRgnType = RGN_NORMAL;

	m_pArea = NULL;
	m_pNextArea = NULL;

	m_lNextRegionID = 0;	// 将要进入的场景ID
	m_lNextTileX = 0;		// 将要进入的场景X
	m_lNextTileY = 0;		// 将要进入的场景Y
	m_lNextDir = 0;			// 将要进入的场景Dir

	m_fSpeed=2000.f;
	m_fChangeSpeed = 0;
	m_sCurSpeedLvl = SPEED_LEVEL_COUNT/2;
	m_bSpeedLvlChanged = false;
	m_lChangeState = 0;
	
	//初始化速度分级值
	m_SpeedLvlValue[0].fRatio = 0.333333f;//2/6
	m_SpeedLvlValue[1].fRatio = 0.50f;//3/6
	m_SpeedLvlValue[2].fRatio = 0.666667f;//4/6
	m_SpeedLvlValue[3].fRatio = 0.833333f;//5/6
	m_SpeedLvlValue[4].fRatio = 1.0f;//6/6
	m_SpeedLvlValue[5].fRatio = 1.25f;
	m_SpeedLvlValue[6].fRatio = 1.5f;
	m_SpeedLvlValue[7].fRatio = 1.75f;
	m_SpeedLvlValue[8].fRatio = 2.0f;

	for(int i=0;i<SPEED_LEVEL_COUNT;i++)
	{
		m_SpeedLvlValue[i].fActionRaio = 1.0f / m_SpeedLvlValue[i].fRatio;
	}
}

CShape::~CShape(void)
{
}

float CShape::GetSpeed()
{
	return m_SpeedLvlValue[m_sCurSpeedLvl].fSpeed;
}

float CShape::GetSpeedChangedRatio()
{
	return m_SpeedLvlValue[m_sCurSpeedLvl].fActionRaio;
}

//设置基本速度
void CShape::SetSpeed(float l)
{
	m_fSpeed = l;
	//重新设置速度级别值
	InitSpeedLvlValue();
	//重设当前速度级别
	SetSpeedLvl();
}

//对应基本速度，设置各等级对应的速度值
void CShape::InitSpeedLvlValue()
{
	for(int i=0;i<SPEED_LEVEL_COUNT;i++)
	{
		m_SpeedLvlValue[i].fSpeed = m_fSpeed*m_SpeedLvlValue[i].fRatio;
	}
}

//当基本行走速度、变化速度发生改变的时候，重设速度级值
void CShape::SetSpeedLvl()
{
	float fSpeed = m_fSpeed+m_fChangeSpeed;
	
	short n = SPEED_LEVEL_COUNT/2;
	if(fSpeed < m_fSpeed)
	{
		for(n=0;n<=SPEED_LEVEL_COUNT/2;n++)
		{
			if(fSpeed <= m_SpeedLvlValue[n].fSpeed)
				break;
		}
	}
	else if(fSpeed > m_fSpeed)
	{
		for(n=SPEED_LEVEL_COUNT-1;n>=SPEED_LEVEL_COUNT/2;n--)
		{
			if(fSpeed >= m_SpeedLvlValue[n].fSpeed)
				break;
		}
	}

	//速度级别发生变化
	if(n != m_sCurSpeedLvl)
	{
		m_bSpeedLvlChanged = true;
		m_sCurSpeedLvl = n;		
	}

	//当速度变化以后触发接口
	OnSpeedChanged();
}

//改变速度值
void CShape::ChangeSpeed(short nSpeed)
{
	if (nSpeed==0)
	{
		return;
	}
	m_fChangeSpeed += 1/(float)nSpeed;
	//重设当前速度级别
	SetSpeedLvl();
}

void CShape::SetChangeSpeed(short nSpeed)
{
	if(nSpeed !=0 )
		m_fChangeSpeed  = 1/(float)nSpeed;
	else
		m_fChangeSpeed = 0.0f;
	//重设当前速度级别
	SetSpeedLvl();
}


// 设置阻挡
void CShape::SetBlock(int x, int y, unsigned short block)
{
	assert(GetFather());
	switch(block) 
	{
	case CRegion::BLOCK_SHAPE:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && pCell->bBlock == CRegion::BLOCK_NO )
				{
					pCell->bBlock = CRegion::BLOCK_SHAPE;
				}
			}
		}
		break;

	case CRegion::BLOCK_NO:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && pCell->bBlock == CRegion::BLOCK_SHAPE )
				{
					pCell->bBlock = CRegion::BLOCK_NO;
				}
			}
		}
		break;
	}
}

//// 添加到CByteArray
//bool CShape::AddShapeToByteArray(vector<BYTE>* pByteArray)
//{
//	_AddToByteArray(pByteArray, GetExID());
//	_AddToByteArray(pByteArray, (long)GetCurRgnType());
//	_AddToByteArray(pByteArray, GetRegionID());
//	_AddToByteArray(pByteArray, GetRegionGUID());
//	_AddToByteArray(pByteArray, GetPosX());
//	_AddToByteArray(pByteArray, GetPosY());
//	_AddToByteArray(pByteArray, GetDir());
//	_AddToByteArray(pByteArray, GetPos());
//	_AddToByteArray(pByteArray, GetSpeed());
//	_AddToByteArray(pByteArray, GetState());
//	_AddToByteArray(pByteArray, GetAction());
//	return true;
//}
//
//// 解码
//bool CShape::DecordShapeFromByteArray(BYTE* pByte, long& pos)
//{
//	_GetBufferFromByteArray(pByte,pos,m_guid);
//	m_CurRgnType = (eRgnType)_GetLongFromByteArray(pByte, pos);
//	m_lRegionID = _GetLongFromByteArray(pByte, pos);
//	_GetBufferFromByteArray(pByte, pos, m_RegionGuid);
//	m_fPosX = _GetFloatFromByteArray(pByte, pos);
//	m_fPosY = _GetFloatFromByteArray(pByte, pos);
//	m_fDir = _GetLongFromByteArray(pByte, pos);
//	m_lPos = _GetLongFromByteArray(pByte, pos);
//	m_lPos = 0;
//	m_fSpeed = _GetFloatFromByteArray(pByte, pos);
//	m_wState = _GetWordFromByteArray(pByte, pos);
//	m_wAction = _GetWordFromByteArray(pByte, pos);
//	SetSpeed(m_fSpeed);
//	return true; 
//}

// 添加到CByteArray
bool CShape::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CBaseObject::AddToByteArray(pByteArray, bExData);
	_AddToByteArray(pByteArray, GetExID());
	_AddToByteArray(pByteArray, (long)GetCurRgnType());
	_AddToByteArray(pByteArray, GetRegionID());
	_AddToByteArray(pByteArray, GetRegionGUID());
	_AddToByteArray(pByteArray, GetPosX());
	_AddToByteArray(pByteArray, GetPosY());
	_AddToByteArray(pByteArray, GetDir());
	_AddToByteArray(pByteArray, GetPos());
	_AddToByteArray(pByteArray, GetSpeed());
	_AddToByteArray(pByteArray, GetState());
	_AddToByteArray(pByteArray, GetAction());
	return true;
}

// 解码
bool CShape::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CBaseObject::DecordFromByteArray(pByte, pos, bExData);
	_GetBufferFromByteArray(pByte,pos,m_guid);
	m_CurRgnType = (eRgnType)_GetLongFromByteArray(pByte, pos);
	m_lRegionID = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos, m_RegionGuid);
	m_fPosX = _GetFloatFromByteArray(pByte, pos);
	m_fPosY = _GetFloatFromByteArray(pByte, pos);
	m_fDir = (float)_GetLongFromByteArray(pByte, pos);
	m_lPos = _GetLongFromByteArray(pByte, pos);
	m_lPos = 0;
	m_fSpeed = _GetFloatFromByteArray(pByte, pos);
	m_wState = _GetWordFromByteArray(pByte, pos);
	m_wAction = _GetWordFromByteArray(pByte, pos);
	SetSpeed(m_fSpeed);
	return true; 
}

// 添加到CByteArray
bool CShape::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	CBaseObject::CodeToDataBlock(setWriteDB,bExData);
	setWriteDB.AddToByteArray((BYTE)GetCurRgnType());
	setWriteDB.AddToByteArray(GetRegionID());
	setWriteDB.AddToByteArray(GetRegionGUID());
	setWriteDB.AddToByteArray(GetPosX());
	setWriteDB.AddToByteArray(GetPosY());
	setWriteDB.AddToByteArray(GetDir());
	setWriteDB.AddToByteArray(GetPos());
	setWriteDB.AddToByteArray(GetSpeed());
	setWriteDB.AddToByteArray(GetState());
	setWriteDB.AddToByteArray(GetAction());
	return true;
}
bool CShape::CodeToDataBlock_ForClient(DBWriteSet& setWriteDB, bool bExData)		// 添加到CByteArray
{
	CBaseObject::CodeToDataBlock_ForClient(setWriteDB,bExData);
	setWriteDB.AddToByteArray((BYTE)GetCurRgnType());
	setWriteDB.AddToByteArray(GetRegionID());
	setWriteDB.AddToByteArray(GetRegionGUID());
	setWriteDB.AddToByteArray(GetPosX());
	setWriteDB.AddToByteArray(GetPosY());
	setWriteDB.AddToByteArray(GetDir());
	setWriteDB.AddToByteArray(GetPos());
	setWriteDB.AddToByteArray(GetSpeed());
	setWriteDB.AddToByteArray(GetState());
	setWriteDB.AddToByteArray(GetAction());
	return true;
}
// 解码
bool CShape::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CBaseObject::DecodeFromDataBlock(setReadDB,bExData);
	m_CurRgnType = (eRgnType)setReadDB.GetByteFromByteArray();
	m_lRegionID = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_RegionGuid);
	m_fPosX = setReadDB.GetFloatFromByteArray();
	m_fPosY = setReadDB.GetFloatFromByteArray();
	m_fDir = (float)setReadDB.GetLongFromByteArray();
	m_lPos = setReadDB.GetLongFromByteArray();
	m_lPos = 0;
	m_fSpeed = setReadDB.GetFloatFromByteArray();
	m_wState = setReadDB.GetWordFromByteArray();
	m_wAction = setReadDB.GetWordFromByteArray();
	SetSpeed(m_fSpeed);
	return true;
}

// 获取面对的坐标
void CShape::GetFacePos(long& x, long& y)
{
	long lDir = GetDir();
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = GetTileX() + pos[lDir][0];
	y = GetTileY() + pos[lDir][1];
}
//获取背后的坐标
void CShape::GetBackPos(long& x, long& y)
{
	long lDir = GetDir();
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = GetTileX() + pos[lDir+4][0];
	y = GetTileY() + pos[lDir+4][1];
}


//获取面对的目标
CShape* CShape::GetFaceShape()
{
	long lTileX = 0;
	long lTileY = 0;
	GetFacePos(lTileX,lTileY);
	return ((CServerRegion*)GetFather())->GetShape(lTileX,lTileY);
}


void CShape::GetFacePosForBigShape(long& x,long& y)
{
	int nStep=0;
	if(GetType()==TYPE_MONSTER)
	{
		CMonster* pMonster=dynamic_cast<CMonster*>(this);		
		//nStep=(pMonster->GetBaseProperty()->dwFigure*2+1 - 1)/2 + 1;
		nStep = (pMonster->GetFigure(DIR_UP)*2+1 -1)/2 + 1;
	}
	else if(GetType()==TYPE_PLAYER)
	{
		nStep=1;
	}
	switch(GetDir())
	{
	case 0:
		{
			x=GetTileX();
			y=GetTileY() - nStep;
		}
		break;
	case 1:
		{
			x=GetTileX() + nStep;
			y=GetTileY() - nStep;
		}
		break;
	case 2:
		{
			x=GetTileX() + nStep;
			y=GetTileY();
		}
		break;
	case 3:
		{
			x=GetTileX() + nStep;
			y=GetTileY() + nStep;
		}
		break;
	case 4:
		{
			x=GetTileX();
			y=GetTileY() + nStep;
		}
		break;
	case 5:
		{
			x=GetTileX() - nStep;
			y=GetTileY() + nStep;
		}
		break;
	case 6:
		{
			x=GetTileX() - nStep;	
			y=GetTileY();
		}
		break;
	case 7:
		{
			x=GetTileX() - nStep;
			y=GetTileY() - nStep;
		}
		break;
	}
}

// 获取dir方向的坐标
void CShape::GetDirPos(long dir, long& x, long& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = x + pos[dir][0];
	y = y + pos[dir][1];
}

// 获取尾部的坐标
long CShape::GetRearDir()
{
	long lDir = GetDir();
	static int dir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	return dir[lDir];
}

// 获取左边的坐标
long CShape::GetLeftDir()
{
	long lDir = GetDir();
	static int dir[8] = { 6, 7, 0, 1, 2, 3, 4, 5 };
	return dir[lDir];
}

// 获取右边的坐标
long CShape::GetRightDir()
{
	long lDir = GetDir();
	static int dir[8] = { 2, 3, 4, 5, 6, 7, 0, 1 };
	return dir[lDir];
}

// 求两个Shape间的距离（单位：格子）
long CShape::Distance(CShape* pShape)
{
	//return max( abs(GetTileX() - pShape->GetTileX()), abs(GetTileY() - pShape->GetTileY()) ) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP);
	return max( abs(GetTileX() - pShape->GetTileX()) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(GetTileY() - pShape->GetTileY()) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
}

long CShape::Distance(long x,long y)
{
	return max( abs(GetTileX() - x), abs(GetTileY() - y) );
}

long CShape::Distance(long sx,long sy,long dx,long dy)
{
	return max( abs(sx - dx), abs(sy - dy) );
}

FLOAT CShape::RealDistance( FLOAT fX, FLOAT fY )
{
	FLOAT fXOffset = fX - GetTileX();
	FLOAT fYOffset = fY - GetTileY();
	return sqrt( fYOffset * fYOffset + fXOffset * fXOffset );
}

LONG CShape::RealDistance( LONG lX, LONG lY )
{
	LONG lResult = 0x7FFFFFFF;

	FLOAT fDistance = RealDistance( static_cast<FLOAT>(lX), static_cast<FLOAT>(lY) );

	FLOAT fDecimalPart = fDistance - static_cast<LONG>(fDistance);
	if( fDecimalPart > 0.5 )
	{
		lResult = static_cast<LONG>( fDistance ) + 1;
	}
	else 
	{
		lResult = static_cast<LONG>( fDistance );
	}
	return lResult;
}

LONG CShape::RealDistance( CShape* pTarget )
{
	LONG lResult = 0x7FFFFFFF;
	if( pTarget )
	{
		FLOAT fResult = RealDistance( static_cast<FLOAT>( pTarget -> GetTileX() ), static_cast<FLOAT>( pTarget -> GetTileY() ) );

		FLOAT fDeltaX = abs( pTarget -> GetTileX() - static_cast<FLOAT>( GetTileX() ) );
		FLOAT fDeltaY = abs( pTarget -> GetTileY() - static_cast<FLOAT>( GetTileY() ) );

		LONG lDeltaX = static_cast<LONG>( fDeltaX ) - pTarget -> GetFigure( DIR_RIGHT ) - GetFigure( DIR_RIGHT );
		LONG lDeltaY = static_cast<LONG>( fDeltaY ) - pTarget -> GetFigure( DIR_UP ) - GetFigure( DIR_UP );

		if( lDeltaX >= lDeltaY )
		{
			fResult = fResult * ( lDeltaX / fDeltaX );
		}
		else
		{
			fResult = fResult * ( lDeltaY / fDeltaY );
		}

		FLOAT fDecimalPart = fResult - static_cast<LONG>(fResult);
		if( fDecimalPart > 0.5 )
		{
			lResult = static_cast<LONG>( fResult ) + 1;
		}
		else 
		{
			lResult = static_cast<LONG>( fResult );
		}
	}
	return lResult;
}

float CShape::RealDistanceF( CShape *shape )
{
    if( !shape ) return 0.0f;
    float dis = ::Distance( shape->GetPosX(), shape->GetPosY(), GetPosX(), GetPosY() );
    return dis - GetFigure( DIR_RIGHT ) - GetFigure( DIR_RIGHT );
}

//从幅度方向值转换到8个枚举值方向
CShape::eDIR CShape::RadianDirTo8Dir(float fRadianDir)
{
	float fDir = fRadianDir;
	if(fDir<0)
		fDir = fDir + (float)2*PI;
	eDIR dir = CShape::DIR_UP;

	if(PI/4 - fDir > 0.4 )
		dir = CShape::DIR_RIGHT;
	else if(PI/2 - fDir >0.4  )
		dir = CShape::DIR_RIGHTDOWN;
	else if( PI*3/4 - fDir  > 0.4 )
		dir = CShape::DIR_DOWN;
	else if(PI  - fDir > 0.4 )
		dir = CShape::DIR_LEFTDOWN;
	else if(  PI*5/4 -fDir  > 0.4 )
		dir = CShape::DIR_LEFT;
	else if( PI*3/2 - fDir  > 0.4 )
		dir = CShape::DIR_LEFTUP;
	else if( PI*7/4 - fDir  > 0.4 )
		dir = CShape::DIR_UP;
	else if(2 * PI - fDir > 0.4)
		dir = CShape::DIR_RIGHTUP;
	else 
		dir = CShape::DIR_RIGHT;
	return dir;
}
// 求两个SHAPE是否在附近(9个Area之内)
bool CShape::IsInAround(CShape* pShape)
{
	if (pShape == NULL)
		return false;

	if (GetRegionID() != pShape->GetRegionID())
		return false;

	if (GetArea() && pShape->GetArea())
	{
		if (abs(GetArea()->GetX() - pShape->GetArea()->GetX()) < 2 &&
			abs(GetArea()->GetY() - pShape->GetArea()->GetY()) < 2)
		{
			return true;
		}
	}

	return false;
}
