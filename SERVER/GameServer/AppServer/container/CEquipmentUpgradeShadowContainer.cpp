// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\BaseObject.h"
#include "..\Goods\CGoods.h"
#include "CEquipmentUpgradeShadowContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=423E857102EE
DWORD CEquipmentUpgradeShadowContainer::s_dwGoodsLimit = 5;

//##ModelId=423E2DB400BB
CEquipmentUpgradeShadowContainer::CEquipmentUpgradeShadowContainer()
{
	// TODO: Add your specialized code here.
	SetGoodsAmountLimit( s_dwGoodsLimit );
}

////##ModelId=423E2DB701B5
//CEquipmentUpgradeShadowContainer::CEquipmentUpgradeShadowContainer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=423E2DC0034B
CEquipmentUpgradeShadowContainer::~CEquipmentUpgradeShadowContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=423E2E5102EE
BOOL CEquipmentUpgradeShadowContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		//##如果该物品是装备,并且可以升级
		if( pGoods -> CanUpgraded() )
		{
			bResult = Add( CEquipmentUpgradeShadowContainer::UEPC_EQUIPMENT, pGoods, pPreviousContainer, pVoid );
		}
		else
		{
			//##察看这个物品是否是宝石
			LONG lGemType = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_TYPE, 1 );
			if( lGemType != 0 )
			{
				//##如果是基本宝石
				if( lGemType == 1 )
				{
					bResult = Add( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM, pGoods, pPreviousContainer, pVoid );
				}
				//##如果是附加宝石
				else if( lGemType == 2 )
				{
					//##检查第一个附加宝石放置格的情况
					if( GetGoods(UEPC_GEM_ONE) == NULL )
					{
						bResult = Add( CEquipmentUpgradeShadowContainer::UEPC_GEM_ONE, pGoods, pPreviousContainer, pVoid );
					}
					//##检查第二个附加宝石放置格的情况
					else if( GetGoods(UEPC_GEM_TWO) == NULL )
					{
						bResult = Add( CEquipmentUpgradeShadowContainer::UEPC_GEM_TWO, pGoods, pPreviousContainer, pVoid );
					}
					//##检查第三个附加宝石放置格的情况
					else if( GetGoods(UEPC_GEM_THREE) == NULL )
					{
						bResult = Add( CEquipmentUpgradeShadowContainer::UEPC_GEM_THREE, pGoods, pPreviousContainer, pVoid );
					}
				}
			}
		}
	}
	return bResult;
}

//##ModelId=423E2E690119
BOOL CEquipmentUpgradeShadowContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 该函数在添加影子后就不用再通知监听器了,CGoodsShadowContainer内有通知监听器的代码。

	BOOL bResult = FALSE;
	if( pObject )
	{
		UPGRADING_EQUIPMENT_PLACE_CELL uepc = static_cast<UPGRADING_EQUIPMENT_PLACE_CELL>( dwPosition );
		itPos it = m_mEquipmentPosition.find( uepc );
		if( it == m_mEquipmentPosition.end() )
		{
			switch( uepc )
			{
			case UEPC_EQUIPMENT:
				{
					//##首先判断是否是装备,是否可以升级
					if( pObject -> CanUpgraded() )
					{
						//m_mEquipmentPosition[uepc] = CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						m_mEquipmentPosition[uepc] = pObject->GetExID();

						if( CAmountLimitGoodsShadowContainer::Add(dwPosition, pObject, pPreviousContainer, pVoid) )
						{
							bResult = TRUE;
						}
						else
						{
							it = m_mEquipmentPosition.find( uepc );
							if( it != m_mEquipmentPosition.end() )
							{
								m_mEquipmentPosition.erase( it );
							}
						}
					}
				}
				break;
			case UEPC_BASE_GEM:
				{
					if( pObject -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_GEM_TYPE, 1) == 1 )
					{
						//m_mEquipmentPosition[uepc] = CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						m_mEquipmentPosition[uepc] = pObject->GetExID();
						if( CAmountLimitGoodsShadowContainer::Add(dwPosition, pObject, pPreviousContainer, pVoid) )
						{
							bResult = TRUE;
						}
						else
						{
							it = m_mEquipmentPosition.find( uepc );
							if( it != m_mEquipmentPosition.end() )
							{
								m_mEquipmentPosition.erase( it );
							}
						}
					}
				}
				break;
			case UEPC_GEM_ONE:
			case UEPC_GEM_TWO:
			case UEPC_GEM_THREE:
				{
					if( pObject -> GetAddonPropertyValues(CGoodsBaseProperties::GAP_GEM_TYPE, 1) == 2 )
					{
						//m_mEquipmentPosition[uepc] = CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						m_mEquipmentPosition[uepc] = pObject->GetExID();
						if( CAmountLimitGoodsShadowContainer::Add(dwPosition, pObject, pPreviousContainer, pVoid) )
						{
							bResult = TRUE;
						}
						else
						{
							it = m_mEquipmentPosition.find( uepc );
							if( it != m_mEquipmentPosition.end() )
							{
								m_mEquipmentPosition.erase( it );
							}
						}
					}
				}
				break;
			}
		}
	}
	return bResult;
}

//##ModelId=423E2FCD0280
//CBaseObject* CEquipmentUpgradeShadowContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
//{
//	// TODO: Add your specialized code here.
//	CBaseObject* pResult = CAmountLimitGoodsShadowContainer::Remove( dwPosition, dwAmount, pVoid );
//	//if( pResult )
//	//{
//	//	map<UPGRADING_EQUIPMENT_PLACE_CELL, DWORD>::iterator it = m_mEquipmentPosition.find( 
//	//		static_cast<UPGRADING_EQUIPMENT_PLACE_CELL>(dwPosition) );
//	//	if( it != m_mEquipmentPosition.end() )
//	//	{
//	//		m_mEquipmentPosition.erase( it );
//	//	}
//	//}
//	return pResult;
//}

//##ModelId=423E2FDE0242
VOID CEquipmentUpgradeShadowContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsShadowContainer::Clear( pVoid );
	m_mEquipmentPosition.clear();
	SetGoodsAmountLimit( s_dwGoodsLimit );
}

//##ModelId=423E385B033C
VOID CEquipmentUpgradeShadowContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsShadowContainer::Release();
	m_mEquipmentPosition.clear();
}

//##ModelId=423E371001C5
BOOL CEquipmentUpgradeShadowContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//BOOL bResult = FALSE;
	//if( m_mEquipmentPosition.size() >= s_dwGoodsLimit ||
	//	m_mGoodsShadow.size() >= s_dwGoodsLimit )
	//{
	//	bResult = TRUE;
	//}
	//return bResult;
	return CAmountLimitGoodsShadowContainer::IsFull();
}

//##ModelId=423E374803C8
BOOL CEquipmentUpgradeShadowContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods )
	{
		//DWORD dwHashValue = CContainer::CalculateHashValue( pGoods -> GetType(), pGoods -> GetID() );
		bResult = QueryGoodsPosition( pGoods->GetExID(), dwPosition );
	}
	return bResult;
}

//##ModelId=423E37540242
BOOL CEquipmentUpgradeShadowContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		if( it -> second == guid )
		{
			dwPosition = it -> first;
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=423E377200EA
BOOL CEquipmentUpgradeShadowContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		CGoods* pGoods = GetGoods( it -> first );
		if( pGoods && pGoods -> GetBasePropertiesIndex() == dwGoodsBasePropertiesIndex )
		{
			bResult = TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=423E2E26030D
CBaseObject* CEquipmentUpgradeShadowContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		if( it -> second == guid )
		{
			pResult = CAmountLimitGoodsShadowContainer::Find( guid );
			break;
		}
	}
	return pResult;
}

//##ModelId=423E2FAE02DE
CBaseObject* CEquipmentUpgradeShadowContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		if( it -> second == guid )
		{
			pResult = CAmountLimitGoodsShadowContainer::Remove( guid, pVoid );
			break;
		}
	}
	return pResult;
}

//##ModelId=423E6B890119
VOID CEquipmentUpgradeShadowContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		CBaseObject* pObj = Find( it -> second );
		if( pListener -> OnTraversingContainer(this, pObj) == FALSE )
		{
			//break;
		}
	}
}

BOOL CEquipmentUpgradeShadowContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	return CAmountLimitGoodsShadowContainer::OnObjectAdded( pContainer, pObj, dwAmount, pVoid );
}

BOOL CEquipmentUpgradeShadowContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	BOOL bResult = FALSE;

	CGUID guid;
	if( pObj )
	{
		guid = pObj->GetExID();
	}
	if( CAmountLimitGoodsShadowContainer::OnObjectRemoved(pContainer, pObj, dwAmount, pVoid) )
	{
		itPos it = m_mEquipmentPosition.begin();
		for( ; it != m_mEquipmentPosition.end(); it ++ )
		{
			if( it -> second == guid )
			{
				m_mEquipmentPosition.erase( it );
				break;
			}
		}
		bResult = TRUE;
	}

	return bResult;
}

//##ModelId=423E6BB002AF
CGoods* CEquipmentUpgradeShadowContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		CGoods* pGoods = GetGoods( it -> first );
		if( pGoods && pGoods -> GetBasePropertiesIndex() == dwGoodsBasePropertiesIndex )
		{
			pResult = pGoods;
			break;
		}
	}
	return pResult;
}

//##ModelId=423E6BBA0399
CGoods* CEquipmentUpgradeShadowContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		if( it -> first == dwPosition )
		{
			pResult = dynamic_cast<CGoods*>( Find(it -> second) );
			break;
		}
	}
	return pResult;
}

//##ModelId=423E6BC4002E
VOID CEquipmentUpgradeShadowContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	itPos it = m_mEquipmentPosition.begin();
	for( ; it != m_mEquipmentPosition.end(); it ++ )
	{
		CGoods* pGoods = GetGoods( it -> first );
		if( pGoods && pGoods -> GetBasePropertiesIndex() == dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( pGoods );
			break;
		}
	}
}

//##ModelId=423E6BCB032C
DWORD CEquipmentUpgradeShadowContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 这个容器不涉及到服务期间的传送，不需要作严格的判定。
	return CAmountLimitGoodsShadowContainer::GetGoodsAmount();
}

//##ModelId=423E6BD7029F
//BOOL CEquipmentUpgradeShadowContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//}
//
////##ModelId=423E6BDF035B
//BOOL CEquipmentUpgradeShadowContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//}

//##ModelId=423E6BE9001F
BOOL CEquipmentUpgradeShadowContainer::Swap(UPGRADING_EQUIPMENT_PLACE_CELL uepcCell, CGoods* pIn, CGoods* * ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return FALSE;
}

CBaseObject* CEquipmentUpgradeShadowContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CEquipmentUpgradeShadowContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CEquipmentUpgradeShadowContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	return CAmountLimitGoodsShadowContainer::Remove( dwPosition, dwAmount, pVoid );
}

CBaseObject* CEquipmentUpgradeShadowContainer::Find( CBaseObject* pObj )
{
	return CAmountLimitGoodsShadowContainer::Find( pObj );
}

CBaseObject* CEquipmentUpgradeShadowContainer::Find( LONG oType,const CGUID& guid )
{
	return CAmountLimitGoodsShadowContainer::Find( oType, guid );
}