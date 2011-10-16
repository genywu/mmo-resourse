#include "StdAfx.h"
#include "..\Player.h"
#include "SmartGladiator.h"
#include "..\Monster.h"


CSmartGladiator::CSmartGladiator(void)
{
}

CSmartGladiator::~CSmartGladiator(void)
{
	while( m_qTarget.size() > 0 )
	{
		m_qTarget.pop();
	}
}