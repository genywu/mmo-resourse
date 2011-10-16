#include "StdAfx.h"
#include ".\shape.h"
#include ".\monster.h"
#include "ClientRegion.h"
#include "GameClient.h"
#include "..\Script\ScriptSys.h"

//
#include "../GameTest/resource.h"
#include "..\GameTest\GameTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CShape::CShape(void)
:m_lCurRgnType(0),
m_lRegionID(0),
m_fPosX(0.0f),
m_fPosY(0.0f),
m_fDir(DIR_UP),
m_wAction(0xffff),
m_fNowDir(0.0f),
m_bIsAtk(false),
m_fSpeed(0.0f)
{
	m_RegionGuid = CGUID::GUID_INVALID;
}

CShape::~CShape(void)
{
}

void CShape::SetPosXY(float x, float y)
{ 
	m_fPosX = x;
	m_fPosY = y;
}

long CShape::GetTileX()
{
	return (long)GetPosX();
}

long CShape::GetTileY()
{
	return (long)GetPosY();
}

// 设置阻挡
void CShape::SetBlock(int x, int y, unsigned short block)
{

	assert(GetFather());

	if (GetType()==TYPE_GOODS || GetType()==TYPE_EFFECT || GetType()==TYPE_SKILL || GetType()==TYPE_SUMMON)
		return;

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


// 添加到CByteArray
bool CShape::AddShapeToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray,GetExID());
	_AddToByteArray(pByteArray, GetRegionID());
	_AddToByteArray(pByteArray,GetRegionGUID());
	_AddToByteArray(pByteArray, GetPosX());
	_AddToByteArray(pByteArray, GetPosY());
	_AddToByteArray(pByteArray, (long)0);
	_AddToByteArray(pByteArray, (long)0);
	//	_AddToByteArray(pByteArray, GetSpeed());
	//	_AddToByteArray(pByteArray, GetState());
	_AddToByteArray(pByteArray, GetAction());
	return true;
}

// 解码
bool CShape::DecordShapeFromByteArray(BYTE* pByte, long& pos)
{
	_GetBufferFromByteArray(pByte,pos,m_guExID);
	m_lCurRgnType = _GetLongFromByteArray(pByte,pos);
	m_lRegionID = _GetLongFromByteArray(pByte, pos);
	_GetBufferFromByteArray(pByte, pos,m_RegionGuid);
	m_fPosX = _GetFloatFromByteArray(pByte, pos);
	m_fPosY = _GetFloatFromByteArray(pByte, pos);
	(eDIR)_GetLongFromByteArray(pByte, pos);
	int lPos = _GetLongFromByteArray(pByte, pos);
	m_fSpeed = _GetFloatFromByteArray(pByte, pos);
	m_wState = _GetWordFromByteArray(pByte, pos);
	m_wAction = _GetWordFromByteArray(pByte, pos);


	SetPosX( GetPosX() );
	SetPosY( GetPosY() );
	//	SetSpeed(m_fSpeed);
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

// 获取面对的坐标
void CShape::GetFacePos(long& x, long& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = GetTileX() + pos[(long)m_fDir][0];
	y = GetTileY() + pos[(long)m_fDir][1];
}

//获取面对的目标
CShape* CShape::GetFaceShape()
{
	long lTileX = 0;
	long lTileY = 0;
	GetFacePos(lTileX,lTileY);
	return ((CClientRegion*)GetFather())->GetShape(lTileX,lTileY);
}

// 获取dir方向的坐标
void CShape::GetDirPos(long dir, long& x, long& y)
{
	static int pos[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	x = x + pos[dir][0];
	y = y + pos[dir][1];
}

// 获取面对的坐标
long CShape::GetFaceDir()
{
	static int dir[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	return dir[(long)m_fDir];
}


// 求两个Shape间的距离（单位：格子）
long CShape::Distance(CShape* pShape)
{
	//return max( abs(GetTileX() - pShape->GetTileX()), abs(GetTileY() - pShape->GetTileY()) ) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP);
	return max( abs(GetTileX() - pShape->GetTileX()) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(GetTileY() - pShape->GetTileY()) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
}

// 求x,y方向上的最小距离（单位：格子）
long CShape::MinXYDistance(CShape* pShape)
{
	return min( abs(GetTileX() - pShape->GetTileX()) - GetFigure(DIR_RIGHT) - pShape->GetFigure(DIR_RIGHT),
		abs(GetTileY() - pShape->GetTileY()) - GetFigure(DIR_UP) - pShape->GetFigure(DIR_UP) );
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

CShape::eDIR CShape::GetDir()
{
	return RadianDirTo8Dir(m_fDir);
}
void CShape::SetDir(eDIR dir)
{
	if( dir >= DIR_UP && dir <= DIR_LEFTUP)
		m_fDir = D3DX_PI/4*dir - D3DX_PI/2;
}

float CShape::GetLineDirF(float sx, float sy, float dx, float dy, float fdir)
{
	//D3DXVECTOR3 vSour(sy, 0.f, sx);
	//D3DXVECTOR3 vDest(dy, 0.f, dx);

	//D3DXVECTOR3 dir = vSour - vDest;

	//float fDirection = fdir;
	//D3DXVec3Normalize(&dir,&dir);
	//if( dir.x != 0 )
	//{
	//	float fDir =  atanf( dir.z/dir.x );
	//	if( dir.x < 0 )
	//		fDir += D3DX_PI;
	//	fDirection = - fDir-D3DX_PI/2;
	//}
	//else
	//{
	//	if(dir.z != 0 )
	//		fDirection = - dir.z/fabsf(dir.z)*D3DX_PI/2-D3DX_PI/2 ;
	//}

	//return fDirection;
	return 1;
}


CShape::eDIR CShape::RadianDirTo8Dir(float fRadianDir)
{
	float fDir = fRadianDir;
	if( fDir <0 )
		fDir += 2* D3DX_PI;

	eDIR dir = CShape::DIR_UP;
	if( D3DX_PI/4 - fDir > 0.4 )
		dir = CShape::DIR_RIGHT;
	else if(D3DX_PI/2 - fDir > 0.4)
		dir = CShape::DIR_RIGHTDOWN;
	else if(D3DX_PI*3/4 - fDir > 0.4)
		dir = CShape::DIR_DOWN;
	else if(D3DX_PI  - fDir > 0.4 )
		dir = CShape::DIR_LEFTDOWN;
	else if(  D3DX_PI*5/4 -fDir  > 0.4 )
		dir = CShape::DIR_LEFT;
	else if( D3DX_PI*3/2 - fDir  > 0.4 )
		dir = CShape::DIR_LEFTUP;
	else if( D3DX_PI*7/4 - fDir  > 0.4 )
		dir = CShape::DIR_UP;
	else if(2 * D3DX_PI - fDir > 0.4)
		dir = CShape::DIR_RIGHTUP;
	else 
		dir = CShape::DIR_RIGHT;
	return dir;
}

//新加 得到模型拾取盒中心点的屏幕坐标
// by MartySa 2009.2.5
//POINT	&CShape::GetPickBoxCenterOfScreenCoord(POINT &ptScreen, long lTileHeight, D3DXMATRIX &ViewMatrix, D3DXMATRIX &ProjectionMatrix)
//{
//	D3DXVECTOR3 vPos(GetPosY(), 0.5f + lTileHeight, GetPosX());
//	D3DXMATRIX  mFinal;
//
//	RECT rc = {0,0, 1024, 768};	
//	int iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;
//
//	D3DXMATRIX matViewClip;
//
//	D3DXMatrixIdentity(&matViewClip);
//	matViewClip._41  = matViewClip._11  = (float)iW/2;
//	matViewClip._42  = matViewClip._22  = (float)iH/2;
//	matViewClip._22  = -matViewClip._22;	
//
//	D3DXMATRIX matTemp;
//	D3DXMatrixMultiply(&mFinal, &ViewMatrix, &ProjectionMatrix);
//	D3DXMatrixMultiply(&mFinal, &mFinal, &matViewClip);
//
//	D3DXVec3TransformCoord(&vPos, &vPos, &mFinal);
//
//	ptScreen.x = (long)vPos.x;
//	ptScreen.y = (long)vPos.y;
//
//	return ptScreen;
//}

//-----------------------------------------------------
//新加解码接口
//-----------------------------------------------------
bool CShape::DecordFromDataBlock(DBReadSet& db,bool bEx /* = true */)
{
	CBaseObject::DecordFromDataBlock(db,bEx);
	m_lCurRgnType = (eRgnType)db.GetByteFromByteArray();
	m_lRegionID = db.GetLongFromByteArray();
	db.GetBufferFromByteArray(m_RegionGuid);
	m_fPosX = db.GetFloatFromByteArray();
	m_fPosY = db.GetFloatFromByteArray();
	SetDir((eDIR)db.GetLongFromByteArray());
	int iPos = db.GetLongFromByteArray();
	m_fSpeed = db.GetFloatFromByteArray();
	m_wState = db.GetWordFromByteArray();
	m_wAction = db.GetWordFromByteArray();
	
	SetNowDir(GetDirEx());
	SetPosX(GetPosX());
	SetPosY(GetPosY());
    SetSpeed(m_fSpeed);
	return true;
}