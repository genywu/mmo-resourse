// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CAmountLimitGoodsShadowContainer.h"
#include "../BaseObject.h"
#include "../Goods/CGoods.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41EF273401C5
CAmountLimitGoodsShadowContainer::CAmountLimitGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
	m_dwMaxGoodsAmount = 0;
}

//##ModelId=41EF275103A9
//CAmountLimitGoodsShadowContainer::CAmountLimitGoodsShadowContainer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41EF27470271
CAmountLimitGoodsShadowContainer::~CAmountLimitGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41EF27430177
bool CAmountLimitGoodsShadowContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( IsFull() == false )
	{
		bResult = CGoodsShadowContainer::Add( pObject, pPreviousContainer, pVoid );
		if( bResult && pObject )
		{
			CGoodsShadowContainer::AddShadow(pObject->GetExID() );
		}
	}
	return bResult;
}

//##ModelId=41EF27670186
bool CAmountLimitGoodsShadowContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( IsFull() == false )
	{
		bResult = CGoodsShadowContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid );
		if( bResult && pObject )
		{
			CGoodsShadowContainer::AddShadow( pObject->GetExID());
		}
	}
	return bResult;
}

//##ModelId=41EF278D02CE
void CAmountLimitGoodsShadowContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsShadowContainer::Clear( pVoid );
}

//##ModelId=41EF2794034B
void CAmountLimitGoodsShadowContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsShadowContainer::Release();
	m_dwMaxGoodsAmount = 0;
}

//##ModelId=41EF27A0002E
bool CAmountLimitGoodsShadowContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( m_mGoodsShadow.size() >= m_dwMaxGoodsAmount )
	{
		bResult = true;
	}
	return bResult;
}


//##ModelId=41EF3A130213
void CAmountLimitGoodsShadowContainer::SetGoodsAmountLimit(DWORD dwMaxNumber)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwMaxGoodsAmount = dwMaxNumber;
}

//##ModelId=41EF3A1A0242
DWORD CAmountLimitGoodsShadowContainer::GetGoodsAmountLimit()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwMaxGoodsAmount;
}

//##ModelId=41EF56D402AF
bool CAmountLimitGoodsShadowContainer::Clone(CBaseObject* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsShadowContainer* pContainer = 
		dynamic_cast<CAmountLimitGoodsShadowContainer*>( pObject );
	if( pContainer == NULL )
	{
		return false;
	}

	pContainer -> m_dwMaxGoodsAmount	= this -> m_dwMaxGoodsAmount;
	pContainer -> m_mGoodsShadow		= this -> m_mGoodsShadow;

	return true;
}
