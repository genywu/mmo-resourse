#include "StdAfx.h"
#include ".\moveshape.h"
#include "mmsystem.h"



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
