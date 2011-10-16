#include "StdAfx.h"
#include "MoveShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMoveShape::CMoveShape(void)
{
	m_bIsGod = false;
}

CMoveShape::~CMoveShape(void)
{
	m_vExStates.clear();
}

VOID CMoveShape::SetExStates( BYTE* pBytes, LONG lSize )
{

	if( !pBytes ) return;
	m_vExStates.clear();
	_AddToByteArray( &m_vExStates, pBytes, lSize );
}
