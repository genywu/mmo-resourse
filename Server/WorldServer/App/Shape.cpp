#include "StdAfx.h"
#include "shape.h"
#include "Region.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShape::CShape(void)
{
	m_lRegionID = 0;
	SetPosX(0.0f);
	SetPosY(0.0f);
	SetDir(0);
	SetPos(0);
	SetSpeed(2000.f);
	SetState(STATE_PEACE);
	SetAction(ACT_STAND);
	m_CurRgnType = RGN_NORMAL;
}

CShape::~CShape(void)
{
}

void CShape::SetRegionID(long l)
{
	if (l < 1E6 || l >= 1E7)
	{
		Log4c::Trace(ROOT_MODULE, "CShape::SetRegionID RegionID[%d] is Error!", l);
	}
	m_lRegionID = l; 
}

float CShape::GetPosX()
{
	return m_fPosX;
}

float CShape::GetPosY()
{
	return m_fPosY;
}

void CShape::SetPosXY(float x, float y)
{
	m_fPosX = x;
	m_fPosY = y;
}

// 设置阻挡
void CShape::SetBlock(int x, int y, unsigned short block)
{
	assert(GetFather());
	switch(block) 
	{
	case CRegion::BLOCK_SHAPE:
		for (int i=x-GetFigure(); i<=x+GetFigure(); i++)
		{
			for (int j=y-GetFigure(); j<=y+GetFigure(); j++)
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
		for (int i=x-GetFigure(); i<=x+GetFigure(); i++)
		{
			for (int j=y-GetFigure(); j<=y+GetFigure(); j++)
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

// 添加到CByteArray
bool CShape::AddShapeToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,(CGUID&)GetExID());
	_AddToByteArray(pByteArray, (long)GetCurRgnType());
	_AddToByteArray(pByteArray, GetRegionID());
	_AddToByteArray(pByteArray, GetRegionExID());
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
bool CShape::DecordShapeFromByteArray(BYTE* pByte, long& pos)
{
	_GetBufferFromByteArray(pByte,pos,m_guExID);
	m_CurRgnType = (eRgnType)_GetLongFromByteArray(pByte, pos);
	m_lRegionID = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos, m_RegionID);
	m_fPosX = _GetFloatFromByteArray(pByte, pos);
	m_fPosY = _GetFloatFromByteArray(pByte, pos);
	m_lDir = _GetLongFromByteArray(pByte, pos);
	m_lPos = _GetLongFromByteArray(pByte, pos);
	m_lPos = 0;
	m_fSpeed = _GetFloatFromByteArray(pByte, pos);
	m_wState = _GetWordFromByteArray(pByte, pos);
	m_wAction = _GetWordFromByteArray(pByte, pos);

	return true; 
}

// 添加到CByteArray
bool CShape::AddToByteArray(vector<BYTE>* pByteArray, bool bExData)
{
	CBaseObject::AddToByteArray(pByteArray, bExData);
	AddShapeToByteArray(pByteArray);
	return true;
}

// 解码
bool CShape::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CBaseObject::DecordFromByteArray(pByte, pos, bExData);
	DecordShapeFromByteArray(pByte, pos);
	return true; 
}


// 添加到CByteArray
bool CShape::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	CBaseObject::CodeToDataBlock(setWriteDB,bExData);
	setWriteDB.AddToByteArray((BYTE)GetCurRgnType());
	setWriteDB.AddToByteArray(GetRegionID());
	setWriteDB.AddToByteArray(GetRegionExID());
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
	setReadDB.GetBufferFromByteArray(m_RegionID);
	m_fPosX = setReadDB.GetFloatFromByteArray();
	m_fPosY = setReadDB.GetFloatFromByteArray();
	m_lDir = setReadDB.GetLongFromByteArray();
	m_lPos = setReadDB.GetLongFromByteArray();
	m_lPos = 0;
	m_fSpeed = setReadDB.GetFloatFromByteArray();
	m_wState = setReadDB.GetWordFromByteArray();
	m_wAction = setReadDB.GetWordFromByteArray();
	SetSpeed(m_fSpeed);
	return true;
}

void CShape::AI()
{
}

// 获取dir方向的坐标
void CShape::GetDirPos(long dir, float& x, float& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = x + pos[dir][0];
	y = y + pos[dir][1];
}

// 获取面对的坐标
long CShape::GetFaceDir()
{
	static int dir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	return dir[m_lDir];
}
