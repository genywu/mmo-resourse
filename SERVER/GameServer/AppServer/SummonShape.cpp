#include "stdafx.h"
#include <mmsystem.h>
#include "SummonShape.h"
#include "ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//LONG CSummonShape::g_lID = 1;

CSummonShape::CSummonShape( tagMasterInfo& tMasterInfo, DWORD dwLifeTime )
{
	m_tSkillID		= SKILL_UNKNOW;
	m_tMasterInfo	= tMasterInfo;
	m_bMoveable		= TRUE;
	m_dwLifeTime	= dwLifeTime;
	m_dwTimeStamp	= timeGetTime();
	m_lType			= TYPE_SUMMON;
	//m_lID			= g_lID++;
}

CSummonShape::~CSummonShape()
{
}

VOID CSummonShape::SetAction( WORD act )
{
	m_wAction = act;
}

VOID CSummonShape::ForceMove( LONG lDestX, LONG lDestY, DWORD dwTime )
{
	//##首先验证X,Y是否属于场景的范围内
	LONG lVerifiedDestX = lDestX;
	LONG lVerifiedDestY = lDestY;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		if( lDestX >= pRegion -> GetWidth() )
		{
			lVerifiedDestX = pRegion -> GetWidth() - 1;
		}
		if( lDestX < 0 )
		{
			lVerifiedDestX = 0;
		}
		if( lDestY >= pRegion -> GetHeight() )
		{
			lVerifiedDestY = pRegion -> GetWidth() - 1;
		}
		if( lDestY < 0 )
		{
			lVerifiedDestY = 0;
		}
	}
	else
	{
		return;
	}

	CMessage msg( MSG_S2C_SHAPE_FORCEMOVE );
	msg.Add( GetExID() );
	msg.Add( GetType() );
	msg.Add( GetTileX() );
	msg.Add( GetTileY() );
	msg.Add( lVerifiedDestX );
	msg.Add( lVerifiedDestY );
	msg.Add( dwTime );
	//##移动时的动作,客户端自己定义
	msg.Add( static_cast<DWORD>(0) );
	msg.SendToAround( this );

	//##移到终点
	SetPosXY( lVerifiedDestX+0.5f, lVerifiedDestY+0.5f );
}

VOID CSummonShape::MoveStep( LONG lDir )
{
	if( !m_bMoveable ) return;

	//##获取目标点
	LONG lX = GetTileX(), lY = GetTileY();
	LONG lOldX = lX, lOldY = lY;
	GetDirPos( lDir, lX, lY );

	//##验证距离
	if( Distance( lX, lY ) > 1 ) return;

	//##移动没有阻挡的限制，如果有阻挡，是地形阻挡，那么就停止，否则就移动
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( GetFather() );
	if( pRegion )
	{
		if( pRegion -> GetBlock( lX, lY ) == CRegion::BLOCK_UNFLY )
		{
			return;
		}
		//##移动，发送消息
		LONG lLastX, lLastY;
		lLastX = GetTileX();
		lLastY = GetTileY();

		SetPosXY( lX+0.5f, lY+0.5f );
		SetDir( lDir );
	}
}

VOID CSummonShape::End()
{
	CMessage msg( MSG_S2C_RGN_DELSHAPE );
	msg.Add( GetType() );
	msg.Add( GetExID() );
	msg.Add( static_cast<LONG>(0) );
	msg.SendToAround( this );
}

DWORD CSummonShape::GetRemainedTime()
{
	if( timeGetTime() >= m_dwTimeStamp + m_dwLifeTime )
		return 0;
	else return m_dwTimeStamp + m_dwLifeTime - timeGetTime();
}


//##召唤物作用域构造，析构函数
CScope::CScope( DWORD dwLength, DWORD dwHeight )
{
	m_dwLength	= dwLength;
	m_dwHeight	= dwHeight;
	m_pScope	= new BYTE[dwLength * dwHeight];
	if( m_pScope )
	{
		ZeroMemory( m_pScope, dwLength * dwHeight );
	}
}

CScope::~CScope()
{
	SAFE_DELETE( m_pScope );
	m_dwLength	= 0;
	m_dwHeight	= 0;
}

BOOL CScope::Get( DWORD dwX, DWORD dwY )
{
	if( m_pScope == NULL ) return FALSE;
	if( dwX >= m_dwLength || dwY >= m_dwHeight ) return FALSE;
	if( m_pScope[dwY * m_dwLength + dwX] ) return TRUE;
	else return FALSE;
}

VOID CScope::Set( BYTE* pBfr )
{
	if( m_pScope == NULL || pBfr == NULL ) return;
	CopyMemory( m_pScope, pBfr, m_dwLength * m_dwHeight );
}

VOID CScope::Set( BYTE b, DWORD dwX, DWORD dwY )
{
	if( m_pScope == NULL ) return;
	if( dwX >= m_dwLength || dwY >= m_dwHeight ) return;
	m_pScope[dwY * m_dwLength + dwX] = b;
}

CScope& CScope::operator =( CScope& cIn )
{
	SAFE_DELETE( m_pScope );
	this -> m_dwLength	= cIn.m_dwLength;
	this -> m_dwHeight	= cIn.m_dwHeight;
	this -> Set( cIn.m_pScope );
	return( *this );
}

BOOL CScope::operator ==( CScope& cIn )
{
	if( this -> m_dwLength != cIn.m_dwLength ||
		this -> m_dwHeight != cIn.m_dwHeight ||
		this -> m_pScope == NULL ||	cIn.m_pScope == NULL ||
		memcmp( this -> m_pScope, cIn.m_pScope, 
		this -> m_dwLength * this -> m_dwHeight ) != 0 )
		return FALSE;
	return TRUE;
}

BOOL CScope::GetOverlappingField( tagRetangle& rSrc1, tagRetangle rSrc2, tagRetangle& rOut )
{
	//##先确定左上角X的重叠情况
	//##Src2的左上角X在Src1内
	if( rSrc2.dwX >= rSrc1.dwX && rSrc2.dwX < rSrc1.dwX + rSrc1.dwLength )
	{
		rOut.dwX = rSrc2.dwX;
		//##右上角的X坐标，Src2的右上角在Src1的区域外
		if( rSrc2.dwX + rSrc2.dwLength > rSrc1.dwX + rSrc1.dwLength )
		{
			rOut.dwLength = rSrc1.dwX + rSrc1.dwLength - rOut.dwX;
		}
		//##Src2的右上角在Src1的区域内
		else 
		{
			rOut.dwLength = rSrc2.dwX + rSrc2.dwLength - rOut.dwX;
		}
	}
	//##Src1的左上角X在Src2内
	else if( rSrc1.dwX >= rSrc2.dwX && rSrc1.dwX < rSrc2.dwX + rSrc2.dwLength )
	{
		rOut.dwX = rSrc1.dwX;
		//##右上角的X坐标，Src1的右上角在Src2的区域外
		if( rSrc1.dwX + rSrc1.dwLength > rSrc2.dwX + rSrc2.dwLength )
		{
			rOut.dwLength = rSrc2.dwX + rSrc2.dwLength - rOut.dwX;
		}
		//##Src1的右上角在Src2的区域内
		else 
		{
			rOut.dwLength = rSrc1.dwX + rSrc1.dwLength - rOut.dwX;
		}
	}
	//##不重叠
	else return FALSE;

	//##再确定左上角Y的重叠情况
	//##Src2的左上角Y在Src1内
	if( rSrc2.dwY >= rSrc1.dwY && rSrc2.dwY < rSrc1.dwY + rSrc1.dwHeight )
	{
		rOut.dwY = rSrc2.dwY;
		//##右上角的Y坐标，Src2的右上角在Src1的区域外
		if( rSrc2.dwY + rSrc2.dwHeight > rSrc1.dwY + rSrc1.dwHeight )
		{
			rOut.dwHeight = rSrc1.dwY + rSrc1.dwHeight - rOut.dwY;
		}
		//##Src2的右上角在Src1的区域内
		else 
		{
			rOut.dwHeight = rSrc2.dwY + rSrc2.dwHeight - rOut.dwY;
		}
	}
	//##Src1的左上角Y在Src2内
	else if( rSrc1.dwY >= rSrc2.dwY && rSrc1.dwY < rSrc2.dwY + rSrc2.dwHeight )
	{
		rOut.dwY = rSrc1.dwY;
		//##右上角的X坐标，Src1的右上角在Src2的区域外
		if( rSrc1.dwY + rSrc1.dwLength > rSrc2.dwY + rSrc2.dwLength )
		{
			rOut.dwHeight = rSrc2.dwY + rSrc2.dwHeight - rOut.dwY;
		}
		//##Src1的右上角在Src2的区域内
		else 
		{
			rOut.dwHeight = rSrc1.dwY + rSrc1.dwHeight - rOut.dwY;
		}
	}
	//##不重叠
	else return FALSE;

	return TRUE;
}