// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\BaseObject.h"
#include "..\Player.h"
#include "..\Goods\CGoods.h"
#include "../../../../public/StrUnifyInput.h"
#include "CEquipmentEnchaseShadowContainer.h"
#include "../session/CSession.h"
#include "../session/CSessionFactory.h"
#include "../session/CPlug.h"





CEquipmentEnchaseShadowContainer::CEquipmentEnchaseShadowContainer()
{		
	SetGoodsAmountLimit( 9999);
}


CEquipmentEnchaseShadowContainer::~CEquipmentEnchaseShadowContainer()
{	
	Release();
}

BOOL CEquipmentEnchaseShadowContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{	
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		//该物品可以镶嵌		
		if(pGoods->CanEnchase())
		{
			bResult = Add( CEquipmentEnchaseShadowContainer::EEPC_EQUIPMENT, pGoods, pPreviousContainer, pVoid );
		}
		else
		{
			//##察看这个物品是否是宝石
			LONG lCardType=pGoods->GetAddonPropertyValues(GAP_CARD_CATEGORY,1);
			if(lCardType==CGoodsBaseProperties::CARD_CAN_ENCHASE)
			{

			}
		}
	}
	return bResult;
}

BOOL CEquipmentEnchaseShadowContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	BOOL bResult = FALSE;
	if( pObject )
	{
		ENCHASE_EQUIPMENT_PLACE_CELL uepc = static_cast<ENCHASE_EQUIPMENT_PLACE_CELL>( dwPosition );
		itPos it = m_mEquipmentPosition.find( uepc );
		//if( it == m_mEquipmentPosition.end() )
		//{
		switch( uepc )
		{
		case CEquipmentEnchaseShadowContainer::EEPC_EQUIPMENT:
			{					
				if( pObject -> CanEnchase() )
				{						
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
		case CEquipmentEnchaseShadowContainer::EEPC_CARD1: //卡片栏位:
		case CEquipmentEnchaseShadowContainer::EEPC_CARD2: //卡片栏位:
		case CEquipmentEnchaseShadowContainer::EEPC_CARD3: //卡片栏位:
		case CEquipmentEnchaseShadowContainer::EEPC_CARD4: //卡片栏位:
		case CEquipmentEnchaseShadowContainer::EEPC_CARD5: //卡片栏位:
			{
				CGoods* pEquip=GetGoods(EEPC_EQUIPMENT);
				if(!pEquip)
					return FALSE;
				DWORD dwEquipLevel=pEquip->GetAddonPropertyValues(GAP_ROLE_MINIMUM_LEVEL_LIMIT,1);
				DWORD dwCardLevel=pObject->GetAddonPropertyValues(GAP_MIN_REINFORCE_LEVEL,1);
				if(dwCardLevel>dwEquipLevel)
				{
					char* strRes=CStringReading::LoadString(24,2);

					CSession* pSession=const_cast<CSession*>(CSessionFactory::QuerySession(GetOwnerID()));
					if(pSession)
					{
						list<long>& plugList=pSession->GetPlugList();

						if(plugList.size()>0)
						{
							list<long>::iterator plugIt=plugList.begin();
							long lPlugId=*plugIt;
							CPlug* pPlug=pSession->QueryPlugByID(lPlugId);
							if(pPlug)
							{
								CPlayer* pPlayer=dynamic_cast<CPlayer*>(pPlug->GetOwner());
								if(pPlayer && strRes)
								{
									pPlayer->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
								}
							}
						}
					}
					return FALSE;
				}
				
				//! 先进行老卡片镶嵌判断
				CGoodsBaseProperties::EQUIP_TYPE etype=pEquip->QueryEquipType();
				DWORD addonVal1=pObject->GetAddonPropertyValues(GAP_ENCHASE_TARGET,1);
				DWORD addonVal2=pObject->GetAddonPropertyValues(GAP_ENCHASE_TARGET,2);
				if(!((CGoodsBaseProperties::ATTACK == etype && addonVal1 == 1) || 
					(CGoodsBaseProperties::DEFENSE == etype && addonVal2 == 1)
					)) 
				{
					//! 老卡片判断失败，进行新卡片判断
					DWORD ExType = pEquip->GetAddonPropertyValues(GAP_WEAPON_CATEGORY, 1);
					if(0 == ExType)
						ExType = pEquip->GetAddonPropertyValues(GAP_GUARD_EQUIP_CATEGORY, 1);

					if(!CheckCardValue(pObject->GetAddonPropertyValues(GAP_CARD_CATEGORY, 1), pEquip->GetGoodsBaseType(), ExType))
						return FALSE;
				}

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
			break;
		case CEquipmentEnchaseShadowContainer::EEPC_YUANBAO_ITEM1:
		//case CEquipmentEnchaseShadowContainer::EEPC_YUANBAO_ITEM2:
		//case CEquipmentEnchaseShadowContainer::EEPC_YUANBAO_ITEM3:
			{
				//这3个栏位的放入条件？
				//if( pObject -> GetAddonPropertyValues(GAP_GEM_TYPE, 1) == 2 )
				//{						
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
				//}
			}
			break;				
		}
		//}
	}
	return bResult;
}

CGoodsShadowContainer::tagGoodsShadow* CEquipmentEnchaseShadowContainer::GetGoodsShadowTag(CGoods* pGoods)
{
	/*
	struct tagGoodsShadow
	{		
		tagGoodsShadow();		
		~tagGoodsShadow();		
		LONG				lOriginalContainerType;		
		CGUID				OriginalContainerID;		
		LONG				lOriginalContainerExtendID;		
		DWORD				dwOriginalGoodsPosition;				
		CGUID				guGoodsID;		
		DWORD				dwGoodsBasePropertiesIndex;		
		DWORD				dwGoodsAmount;				
	};*/
	map<CGUID,tagGoodsShadow,hash_guid_compare>::iterator tagIter=m_mGoodsShadow.begin();
	for(;tagIter!=m_mGoodsShadow.end();tagIter++)
	{
		if(tagIter->first==pGoods->GetExID())
		{
			CGoodsShadowContainer::tagGoodsShadow* pShadow= MP_NEW CGoodsShadowContainer::tagGoodsShadow;
			pShadow->dwGoodsAmount=tagIter->second.dwGoodsAmount;
			pShadow->dwGoodsBasePropertiesIndex=tagIter->second.dwGoodsBasePropertiesIndex;
			pShadow->dwOriginalGoodsPosition=tagIter->second.dwOriginalGoodsPosition;
			pShadow->guGoodsID=tagIter->second.guGoodsID;
			pShadow->lOriginalContainerExtendID=tagIter->second.lOriginalContainerExtendID;
			pShadow->lOriginalContainerType=tagIter->second.lOriginalContainerType;
			pShadow->OriginalContainerID=tagIter->second.OriginalContainerID;
			return pShadow;
		}
			
	}	
	return NULL;
}

//容器清除
VOID CEquipmentEnchaseShadowContainer::Clear(LPVOID pVoid)
{	
	CAmountLimitGoodsShadowContainer::Clear( pVoid );
	m_mEquipmentPosition.clear();
	SetGoodsAmountLimit( EEPC_NUM );
}

//容器释放
VOID CEquipmentEnchaseShadowContainer::Release()
{	
	CAmountLimitGoodsShadowContainer::Release();
	m_mEquipmentPosition.clear();
}

//容器是否已满
BOOL CEquipmentEnchaseShadowContainer::IsFull()
{	
	return CAmountLimitGoodsShadowContainer::IsFull();
}

//查询物品所在位置
BOOL CEquipmentEnchaseShadowContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{	
	BOOL bResult = FALSE;
	if( pGoods )
	{		
		bResult = QueryGoodsPosition( pGoods->GetExID(), dwPosition );
	}
	return bResult;
}

//通过GUID查询物品位置
BOOL CEquipmentEnchaseShadowContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{	
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

//是否存在指定ID的物品
BOOL CEquipmentEnchaseShadowContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{	
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

//通过GUID查找物品
CBaseObject* CEquipmentEnchaseShadowContainer::Find(const CGUID& guid)
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

//移除制定GUID的物品
CBaseObject* CEquipmentEnchaseShadowContainer::Remove(const CGUID& guid, LPVOID pVoid)
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

BOOL CEquipmentEnchaseShadowContainer::RemoveShadowEx(CGUID& guid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		DWORD dwPosition = 0;
		if(!QueryGoodsPosition(guid, dwPosition) )
		{
			return FALSE;
		}

		itPos itpos=m_mEquipmentPosition.begin();
		for(;itpos!=m_mEquipmentPosition.end();itpos++)
		{
			if(guid==itpos->second)
			{
				break;
			}
		}
		if(itpos!=m_mEquipmentPosition.end())
		{
			m_mEquipmentPosition.erase(itpos);
			bResult=TRUE;
		}
		m_mGoodsShadow.erase( it );
	}
	return bResult;
}

//##ModelId=423E6B890119
VOID CEquipmentEnchaseShadowContainer::TraversingContainer(CContainerListener* pListener)
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

BOOL CEquipmentEnchaseShadowContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	return CAmountLimitGoodsShadowContainer::OnObjectAdded( pContainer, pObj, dwAmount, pVoid );
}

BOOL CEquipmentEnchaseShadowContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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

//指定ID，返回第一个找到的物品
CGoods* CEquipmentEnchaseShadowContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
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

//通过位置取得物品
CGoods* CEquipmentEnchaseShadowContainer::GetGoods(DWORD dwPosition)
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

//批量取得同一ID的物品
VOID CEquipmentEnchaseShadowContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
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

//取得物品个数
DWORD CEquipmentEnchaseShadowContainer::GetGoodsAmount()
{		
	// 这个容器不涉及到服务期间的传送，不需要作严格的判定。
	return CAmountLimitGoodsShadowContainer::GetGoodsAmount();
}


BOOL CEquipmentEnchaseShadowContainer::Swap(ENCHASE_EQUIPMENT_PLACE_CELL uepcCell, CGoods* pIn, CGoods* * ppOut, LPVOID pVoid)
{	
	return FALSE;
}

CBaseObject* CEquipmentEnchaseShadowContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CEquipmentEnchaseShadowContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CEquipmentEnchaseShadowContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	return CAmountLimitGoodsShadowContainer::Remove( dwPosition, dwAmount, pVoid );
}

CBaseObject* CEquipmentEnchaseShadowContainer::Find( CBaseObject* pObj )
{
	return CAmountLimitGoodsShadowContainer::Find( pObj );
}

CBaseObject* CEquipmentEnchaseShadowContainer::Find( LONG oType,const CGUID& guid )
{
	return CAmountLimitGoodsShadowContainer::Find( oType, guid );
}