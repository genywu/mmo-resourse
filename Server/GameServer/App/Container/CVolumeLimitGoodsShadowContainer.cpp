// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "../BaseObject.h"
#include "../Goods/CGoods.h"
#include "CVolumeLimitGoodsShadowContainer.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"

#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41EF4251036B
CVolumeLimitGoodsShadowContainer::CVolumeLimitGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
	m_dwWidth	= 0;
	m_dwHeight	= 0;
}

//##ModelId=41EF425503C8
//CVolumeLimitGoodsShadowContainer::CVolumeLimitGoodsShadowContainer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41EF4257029F
CVolumeLimitGoodsShadowContainer::~CVolumeLimitGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41EF44C70177
bool CVolumeLimitGoodsShadowContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods == NULL )
	{
		return false;
	}

	DWORD dwFirstCell = 0;
	if( FindPositionForGoods(pGoods, dwFirstCell) == false )
	{
		return false;
	}

	return Add( dwFirstCell, pGoods, pPreviousContainer, pVoid );
}

//##ModelId=41EF44CF0148
bool CVolumeLimitGoodsShadowContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( IsFull() == false )
	{
		if( pObject )
		{
			//##该位置是否有物品？
			//CGoods* pGoods = GetGoods( dwPosition );
			//if( !pGoods )
			//{
				//bResult = CGoodsShadowContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid );
			//}
			//else 
			//{
				if( pPreviousContainer )
				{
					//##该位置没有物品，但是，要判断他的原物品是否已经存在于影子容器
					if( GetShadowPosition(pPreviousContainer, dwPosition) )
					{
						//##如果存在于容器中，则以原来的位置作为添加点。
						bResult = CGoodsShadowContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid);
					}
					else 
					{
						if( IsSpaceEnough(pObject, dwPosition) )
						{
							CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
								pObject -> GetBasePropertiesIndex() );
							if( pProperties )
							{
								if( CGoodsShadowContainer::Add(pObject, pPreviousContainer, pVoid) )
								{
									//##添加之后，pObject完全有可能失效。之后，不能再次使用pObject，只能根据
									//##原容器的信息找出该物品
									CGUID guID = NULL_GUID;
									goodsshadowIt it = m_mGoodsShadow.begin();
									for( ; it != m_mGoodsShadow.end(); it ++ )
									{
										if( pPreviousContainer -> lContainerType == it -> second.lOriginalContainerType &&
											pPreviousContainer -> ContainerID == it -> second.OriginalContainerID &&
											pPreviousContainer -> lContainerExtendID == it -> second.lOriginalContainerExtendID &&
											pPreviousContainer -> dwGoodsPosition == it -> second.dwOriginalGoodsPosition )
										{
											guID = it -> first;
										}
									}
									DWORD dwY			= dwPosition / GetContainerWidth();
									DWORD dwX			= dwPosition % GetContainerWidth();
									m_vCells[dwY*GetContainerWidth() +dwX] = guID;
									CGoodsShadowContainer::AddShadow( guID );
									bResult = true;
								}
							}
						}
					}
				}
			//}
		}
	}
	return bResult;
}

//##ModelId=41F0C14A01FA
bool CVolumeLimitGoodsShadowContainer::RemoveShadow(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = true;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		DWORD dwPosition = 0;
		if( QueryGoodsPosition(guid, dwPosition) )
		{
			CS2CContainerObjectMove comMsg;
			comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comMsg.SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			comMsg.SetSourceContainerExtendID( GetContainerExtendID() );
			comMsg.SetSourceObject( TYPE_GOODS,it->second.guGoodsID );
			comMsg.SendToSession( GetOwnerID() );
		}

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
			it -> second.dwGoodsBasePropertiesIndex );
		if( pProperties )
		{
			DWORD dwFirstCell = 0;
			if( QueryGoodsPosition(guid, dwFirstCell) )
			{
				DWORD dwY			= dwFirstCell / GetContainerWidth();
				DWORD dwX			= dwFirstCell % GetContainerWidth();
				
				
				m_vCells[dwY*GetContainerWidth() + dwX] = CGUID::GUID_INVALID;
				
				bResult = true;
			}
		}

		//##删除之前需要通知所有的监听器?
		//##如果不是移出实际的物品,是否有必要通知?
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectAdded( this, NULL, 0, NULL );
			}
		}

		m_mGoodsShadow.erase( it );
	}
	return bResult;
}

bool CVolumeLimitGoodsShadowContainer::RemoveShadowEx(CGUID& guid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		DWORD dwPosition = 0;
		if(!QueryGoodsPosition(guid, dwPosition) )
		{
			return false;
		}

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
			it -> second.dwGoodsBasePropertiesIndex );
		if( pProperties )
		{
			DWORD dwFirstCell = 0;
			if( QueryGoodsPosition(guid, dwFirstCell) )
			{
				DWORD dwY			= dwFirstCell / GetContainerWidth();
				DWORD dwX			= dwFirstCell % GetContainerWidth();


				m_vCells[dwY*GetContainerWidth() + dwX] = CGUID::GUID_INVALID;

				bResult = true;
			}
		}		
		m_mGoodsShadow.erase( it );
	}
	return bResult;
}

//##ModelId=41EF44F20196
CBaseObject* CVolumeLimitGoodsShadowContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CAmountLimitGoodsShadowContainer::Remove( guid, pVoid );
}

//##ModelId=41EF44F802AF
CBaseObject* CVolumeLimitGoodsShadowContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CAmountLimitGoodsShadowContainer::Remove( dwPosition, dwAmount, pVoid );
}

//##ModelId=41EF44D60196
void CVolumeLimitGoodsShadowContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsShadowContainer::Clear();
	
	m_vCells.clear();
	m_vCells.resize( m_dwWidth * m_dwHeight, NULL_GUID );
}

//##ModelId=41EF44DC00AB
void CVolumeLimitGoodsShadowContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsShadowContainer::Release();
	m_dwWidth	= 0;
	m_dwHeight	= 0;
	m_vCells.clear();
}

//##ModelId=41EF4682034B
bool CVolumeLimitGoodsShadowContainer::Clone(CBaseObject* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( CAmountLimitGoodsShadowContainer::Clone(pObject) )
	{
		CVolumeLimitGoodsShadowContainer* pContainer = 
			dynamic_cast<CVolumeLimitGoodsShadowContainer*>( pObject );
		if( pContainer == NULL )
		{
			return false;
		}

		pContainer -> m_dwWidth		= this -> m_dwWidth;
		pContainer -> m_dwHeight	= this -> m_dwHeight;
		pContainer -> m_vCells		= this -> m_vCells;
		return true;
	}
	return false;
}

//##ModelId=41EF44E202BF
bool CVolumeLimitGoodsShadowContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = true;
	if( CAmountLimitGoodsShadowContainer::IsFull() == false )
	{
		itCell it = m_vCells.begin();
		for( ; it < m_vCells.end(); it ++ )
		{
			if( *it == CGUID::GUID_INVALID )
			{
				bResult = false;
				break;
			}
		}
	}
	return false;
}

//##ModelId=41EF450303D8
bool CVolumeLimitGoodsShadowContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return false;
	}

	//DWORD dwHashValue = CContainer::CalculateHashValue( pGoods -> GetType(), 
	//	pGoods -> GetID() );

	return QueryGoodsPosition( pGoods->GetExID(), dwPosition );
}

//##ModelId=41EF450B0399
bool CVolumeLimitGoodsShadowContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == guid )
		{
			dwPosition = static_cast<DWORD>( i );
			return true;
		}
	}
	return false;
}

//##ModelId=41EF451602DE
CGoods* CVolumeLimitGoodsShadowContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition >= m_vCells.size() )
	{
		return NULL;
	}

	if( m_vCells[dwPosition] == CGUID::GUID_INVALID )
	{
		return NULL;
	}

	return dynamic_cast<CGoods*>( Find(m_vCells[dwPosition]) );
}

//##ModelId=41EF4645005D
void CVolumeLimitGoodsShadowContainer::SetContainerVolume(DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();
	m_dwWidth	= dwWidth;
	m_dwHeight	= dwHeight;
	m_vCells.resize( dwWidth * dwHeight, NULL_GUID );
	SetGoodsAmountLimit( dwWidth * dwHeight );
}

//##ModelId=41EF46520157
DWORD CVolumeLimitGoodsShadowContainer::GetContainerWidth()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwWidth;
}

//##ModelId=41EF465A007D
DWORD CVolumeLimitGoodsShadowContainer::GetContainerHeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwHeight;
}

//##ModelId=41EF46600290
bool CVolumeLimitGoodsShadowContainer::FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL ) return false;
	if( pGoods -> GetMaxStackNumber() > 1 )
	{
		goodsshadowIt it = m_mGoodsShadow.begin();
		for( ; it != m_mGoodsShadow.end(); it ++ )
		{
			if( it -> second.dwGoodsBasePropertiesIndex == 
				pGoods -> GetBasePropertiesIndex() )
			{
				if( it -> second.dwGoodsAmount + pGoods -> GetAmount() <=
					pGoods -> GetMaxStackNumber() )
				{
					if( QueryGoodsPosition(it -> first, dwPosition) )
					{
						return true;
					}
				}
			}
		}
	}

	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == CGUID::GUID_INVALID )
		{
			if( IsSpaceEnough( pGoods, static_cast<DWORD>(i) ) )
			{
				dwPosition = static_cast<DWORD>( i );
				return true;
			}
		}
	}
	return false;
}

//##ModelId=41EF466C0148
bool CVolumeLimitGoodsShadowContainer::IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL || dwFirstCell >= m_vCells.size() || IsFull() )
	{
		return false;
	}

	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
		pGoods -> GetBasePropertiesIndex() );
	if( pProperties == NULL )
	{
		return false;
	}

	DWORD dwY = dwFirstCell / GetContainerWidth();
	DWORD dwX = dwFirstCell % GetContainerWidth();
	/*
	for( DWORD dwHeight = 0; dwHeight < pProperties -> GetHeight(); dwHeight ++ )
	{
		if( dwHeight + dwY >= GetContainerHeight() )
		{
			return false;
		}
		for( DWORD dwWidth = 0; dwWidth < pProperties -> GetWidth(); dwWidth ++ )
		{
			if( dwWidth + dwX >= GetContainerWidth() || 
				m_vCells[(dwHeight + dwY) * GetContainerWidth() + (dwWidth + dwX)] != CGUID::GUID_INVALID )
			{
				return false;
			}
		}
	}*/
	return true;
}

//##ModelId=41EF4674006D
bool CVolumeLimitGoodsShadowContainer::IsSpaceEnough(vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 暂时不支持这个方法。
	return false;
}

//##ModelId=41EF468E0128
void CVolumeLimitGoodsShadowContainer::GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetContainerWidth();
		DWORD dwX = dwFirstCell % GetContainerWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetContainerHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetContainerWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					CGoods* pGoods = dynamic_cast<CGoods*>( Find(guid) );
					if( pGoods )
					{
						bool bAlreadyAdded	= false;

						vector<CGoods*>::iterator it = vGoods.begin();
						for( ; it != vGoods.end(); it ++ )
						{
							if( *it == pGoods )
							{
								bAlreadyAdded = true;
								break;
							}
						}
						if( bAlreadyAdded == false )
						{
							vGoods.push_back( pGoods );
						}
					}
				}
			}
		}
	}
}

//##ModelId=41EF4697031C
CGoods* CVolumeLimitGoodsShadowContainer::GetFirstGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY		= dwFirstCell / GetContainerWidth();
		DWORD dwX		= dwFirstCell % GetContainerWidth();
		bool  bFounded	= false;
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetContainerHeight() )
			{
				continue;
			}
			if( bFounded )
			{
				break;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetContainerWidth() )
				{
					continue;
				}
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					pResult = dynamic_cast<CGoods*>( Find(guid) );
					if( pResult )
					{
						bFounded = true;
					}
				}
			}
		}
	}
	return pResult;
}

//##ModelId=41EF46A00196
DWORD CVolumeLimitGoodsShadowContainer::GetGoodsAmountInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	vector<CGUID> vGoods;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetContainerWidth();
		DWORD dwX = dwFirstCell % GetContainerWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetContainerHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetContainerWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					bool bAlreadyAdded	= false;

					vector<CGUID>::iterator it = vGoods.begin();
					for( ; it != vGoods.end(); it ++ )
					{
						if( *it == guid )
						{
							bAlreadyAdded = true;
							break;
						}
					}
					if( bAlreadyAdded == false )
					{
						vGoods.push_back( guid );
					}
				}
			}
		}
	}
	return static_cast<DWORD>( vGoods.size() );
}

CBaseObject* CVolumeLimitGoodsShadowContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CVolumeLimitGoodsShadowContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}