// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "../BaseObject.h"
#include "../Player.h"
#include "../Goods/CGoods.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"
#include "../MessagePackage/CS2CContainerObjectAmountChange.h"
#include "CContainer.h"
#include "CGoodsShadowContainer.h"
#include "../Listener/CGoodsListListener.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41E5EFB4001F
CGoodsShadowContainer::CGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
}

//##ModelId=41E5EFB702AF
//CGoodsShadowContainer::CGoodsShadowContainer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41E5EFBD008C
CGoodsShadowContainer::~CGoodsShadowContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41E745790186
bool CGoodsShadowContainer::AddShadow(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		DWORD dwPosition = 0;
		if( QueryGoodsPosition(guid, dwPosition) )
		{
			CGoods* pGoods = GetGoods( dwPosition );
			if( pGoods )
			{
				DWORD dwOriginalAmount = pGoods -> GetAmount();

				CS2CContainerObjectMove comMsg;
				comMsg.SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				comMsg.SetSourceContainer(it->second.lOriginalContainerType,
					it->second.OriginalContainerID,it->second.dwOriginalGoodsPosition);
				comMsg.SetSourceContainerExtendID(it->second.lOriginalContainerExtendID);
				
				comMsg.SetDestinationContainer( GetOwnerType(), GetOwnerID(), dwPosition );
				comMsg.SetDestinationContainerExtendID( m_lContainerExtendID );
				comMsg.SetDestinationObject( TYPE_GOODS, it -> second.guGoodsID );
				comMsg.SetDestinationObjectAmount( it -> second.dwGoodsAmount );

				pGoods -> SetAmount( it -> second.dwGoodsAmount );
				pGoods -> SerializeForOldClient( &comMsg.GetObjectStream() );
				pGoods -> SetAmount( dwOriginalAmount );

				//##这儿需要扩展这个消息发送的部分.OwnerID是一个会话的ID，需要能对会话内
				//##的所有玩家发送。
				if( GetOwnerType() == TYPE_PLAYER )
				{
					comMsg.Send( GetOwnerID() );
				}
				else if( GetOwnerType() == TYPE_SESSION )
				{
					comMsg.SendToSession( GetOwnerID() );
				}
			}
		}
		//##退出之前需要通知所有的监听器?
		//##如果不是添加实际的物品,是否有必要通知?
		//##这只是通知监听器有物品加入，但是不告知具体的细节。
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectAdded( this, NULL, 0, NULL );
			}
		}
		bResult = true;
	}
	return bResult;
}

bool CGoodsShadowContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{	
	bool bResult = false;
	if( pPreviousContainer && IsFull() == false )
	{
		CGoods* pGoods	= dynamic_cast<CGoods*>( pObject );
		if( pGoods )
		{			
			CGUID	guGoodsID	= pGoods ->GetExID();
			DWORD	dwAmount	= pGoods -> GetAmount();
			DWORD	dwBaseIndex	= pGoods -> GetBasePropertiesIndex();							

			//##然后再自身容器内添加影子.
			tagGoodsShadow tShadow;
			tShadow.lOriginalContainerType		= pPreviousContainer -> lContainerType;
			tShadow.OriginalContainerID		= pPreviousContainer -> ContainerID;
			tShadow.lOriginalContainerExtendID	= pPreviousContainer -> lContainerExtendID;
			tShadow.dwOriginalGoodsPosition		= pPreviousContainer -> dwGoodsPosition;
			tShadow.dwGoodsAmount				= dwAmount;					
			tShadow.guGoodsID					= guGoodsID;
			tShadow.dwGoodsBasePropertiesIndex	= dwBaseIndex;
			m_mGoodsShadow[guGoodsID]			= tShadow;				
			bResult								= true;		
		}
	}
	return bResult;
}

bool CGoodsShadowContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CGoodsShadowContainer::Add( pObject, pPreviousContainer, pVoid );
}

//##ModelId=41E5F0080177
CBaseObject* CGoodsShadowContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	//CBaseObject* pResult = NULL;
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
	//CGoods* pGoods = GetGoods( dwPosition );
	//if( pGoods )
	//{
	//	DWORD dwHashValue = CContainer::CalculateHashValue(	pGoods -> GetType(), pGoods -> GetID() );
	//	CGoodsContainer* pContainer = GetGoodsSourceContainer( dwHashValue );
	//	if( pContainer )
	//	{
	//		DWORD dwOriginalPosition = 0;
	//		if( pContainer -> QueryGoodsPosition(dwHashValue, dwOriginalPosition) )
	//		{
	//			pResult = dynamic_cast<CGoods*>( pContainer -> Remove( dwOriginalPosition, 
	//				dwAmount, pVoid) );
	//		}
	//	}
	//	//##如果有东西，则发送消息给客户端，减少或删除该物品镜像。
	//	RemoveShadow( dwHashValue );
	//}
	//return pResult;
}

//##ModelId=41E5F00F0251
void CGoodsShadowContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// m_lContainerExtendID = 0;
	CGoodsContainer::Clear();
	m_mGoodsShadow.clear();
}

//##ModelId=41E5F01503C8
void CGoodsShadowContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lContainerExtendID = 0;
	CGoodsContainer::Release();
	m_mGoodsShadow.clear();
}


//##ModelId=41E7635D0000
void CGoodsShadowContainer::SetContainerExtendID(LONG lID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lContainerExtendID = lID;
}

//##ModelId=41F0C6990048
LONG CGoodsShadowContainer::GetContainerExtendID()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_lContainerExtendID;
}

//##ModelId=41E5F01B038A
void CGoodsShadowContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
}

//##ModelId=41E5F02203A9
bool CGoodsShadowContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return false;
}

//##ModelId=41E5F76003D8
void CGoodsShadowContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener )
	{
		goodsshadowIt it = m_mGoodsShadow.begin();
		for( ; it != m_mGoodsShadow.end(); it ++ )
		{
			CBaseObject* pObject = Find( it -> first );
			if( pObject )
			{
				CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
				if( pGoods && pGoods -> GetAmount() == it -> second.dwGoodsAmount )
				{
					pListener -> OnTraversingContainer( this, pGoods );
				}
			}
		}

		//##这儿是一个特例
		CGoodsListListener* pGoodsList = dynamic_cast<CGoodsListListener*>( pListener );
		if( pGoodsList )
		{
			if( m_mGoodsShadow.size() != pGoodsList -> m_vGoodsID.size() )
			{
				pGoodsList -> m_bIsAllGoodsExist = false;
			}
		}
	}
}

/*
DWORD CGoodsShadowContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		CGoods* pGoods = dynamic_cast<CGoods*>( Find(it -> first) );
		if( pGoods )
		{
			dwResult += pGoods -> GetWeight();
		}
	}
	return dwResult;
}*/

//##ModelId=41E5F7760290
bool CGoodsShadowContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( pGoods )
	{
		bResult = QueryGoodsPosition( pGoods->GetExID(), dwPosition );
	}
	return bResult;
}

//##ModelId=41E5F77D0222
bool CGoodsShadowContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult	= false;
	DWORD dwValue	= 0;
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		if( it -> first == guid )
		{
			bResult = true;
			break;
		}
		++ dwValue;
	}
	if( bResult )
	{
		dwPosition = dwValue;
	}
	return bResult;
}

//##ModelId=41E5F78900FA
bool CGoodsShadowContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		if( it -> second.dwGoodsBasePropertiesIndex == 
			dwGoodsBasePropertiesIndex )
		{
			if( Find(it -> first) )
			{
				bResult = true;
				break;
			}
		}
	}
	return bResult;
}

//##ModelId=41E5F791002E
CGoods* CGoodsShadowContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		if( it -> second.dwGoodsBasePropertiesIndex == 
			dwGoodsBasePropertiesIndex )
		{
			pResult = dynamic_cast<CGoods*>( Find(it -> first) );
			if( pResult )
			{
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41E5F797031C
CGoods* CGoodsShadowContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	CGoods* pResult	= NULL;
	DWORD dwValue	= 0;
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		if( dwValue == dwPosition )
		{
			pResult = dynamic_cast<CGoods*>( Find(it -> first) );
			break;
		}
		++ dwValue;
	}
	return pResult;
}

//##ModelId=41E5F79F0119
void CGoodsShadowContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	goodsshadowIt it = m_mGoodsShadow.begin();
	for( ; it != m_mGoodsShadow.end(); it ++ )
	{
		if( it -> second.dwGoodsBasePropertiesIndex == 
			dwGoodsBasePropertiesIndex )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( Find(it -> first) );
			if( pGoods )
			{
				vOut.push_back( pGoods );
			}
		}
	}
}

//##ModelId=41E5F7A70196
DWORD CGoodsShadowContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return static_cast<DWORD>( m_mGoodsShadow.size() );
}


//##ModelId=41E735D901B5
bool CGoodsShadowContainer::RemoveShadow(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		DWORD dwPosition = 0;
		if( QueryGoodsPosition(guid, dwPosition) )
		{
			CS2CContainerObjectMove comMsg;
			comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comMsg.SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			comMsg.SetSourceContainerExtendID( m_lContainerExtendID );
			comMsg.SetSourceObject( TYPE_GOODS,it->second.guGoodsID );
			//##这儿需要扩展这个消息发送的部分.OwnerID是一个会话的ID，需要能对会话内
			//##的所有玩家发送。
			if( GetOwnerType() == TYPE_PLAYER )
			{
				comMsg.Send( GetOwnerID() );
			}
			else if( GetOwnerType() == TYPE_SESSION )
			{
				comMsg.SendToSession( GetOwnerID() );
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
		bResult = true;
	}
	return bResult;
}

//##ModelId=41E61B590109
//##该接口是为了同源容器保持同步的时候使用的。该接口在源容器内物品被拆分，被堆叠，被移动，被删除，
//##的时候保持和源容器的一致性。
bool CGoodsShadowContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##因为无法获取最终的数量，目前暂时把物品的镜像删除掉。
	if( pObj )
	{
		//DWORD dwHashValue = CContainer::CalculateHashValue( pObj -> GetType(), pObj -> GetID() );
		CGUID	guid = pObj->GetExID();
		goodsshadowIt it = m_mGoodsShadow.find( guid );
		if( it != m_mGoodsShadow.end() )
		{
			//##找到了,更新该物品的镜像信息.
			if( it -> second.dwGoodsAmount != dwAmount )
			{
				//##如果原物品数量和改变的数量不同
				it -> second.dwGoodsAmount += dwAmount;

				//##发送数量改变消息。
				DWORD dwPosition = 0;
				if( QueryGoodsPosition(it -> first, dwPosition) )
				{
					CS2CContainerObjectAmountChange coacMsg;
					coacMsg.SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
					coacMsg.SetSourceContainerExtendID( GetContainerExtendID() );
					coacMsg.SetObject( pObj -> GetType(),guid);
					coacMsg.SetObjectAmount( it -> second.dwGoodsAmount );
					coacMsg.SendToSession( GetOwnerID() );
				}
			}
		}
	}
	return true;
}

//##ModelId=41E61B6701F4
//##该接口是为了同源容器保持同步的时候使用的。该接口在源容器内物品被拆分，被堆叠，被移动，被删除，
//##的时候保持和源容器的一致性。
bool CGoodsShadowContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObj )
	{
		//DWORD dwHashValue = CContainer::CalculateHashValue( pObj -> GetType(), pObj -> GetID() );
		CGUID	guid = pObj->GetExID();
		goodsshadowIt it = m_mGoodsShadow.find( guid );
		if( it != m_mGoodsShadow.end() )
		{
			//##找到了,更新该物品的镜像信息.
			//##如果原物品数量和改变的数量不同
			if( it -> second.dwGoodsAmount >= dwAmount )
			{
				it -> second.dwGoodsAmount -= dwAmount;
				if( it -> second.dwGoodsAmount == 0 )
				{
					RemoveShadow( guid );
				}
				else
				{
					//##发送数量改变消息。
					DWORD dwPosition = 0;
					if( QueryGoodsPosition(it -> first, dwPosition) )
					{
						CS2CContainerObjectAmountChange coacMsg;
						coacMsg.SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
						coacMsg.SetSourceContainerExtendID( GetContainerExtendID() );
						coacMsg.SetObject( pObj -> GetType(),guid );
						coacMsg.SetObjectAmount( it -> second.dwGoodsAmount );
						coacMsg.SendToSession( GetOwnerID() );
					}
				}
			}
			else
			{
				RemoveShadow( guid );
			}
		}
	}
	return true;
}

//##ModelId=41E61B73036B
bool CGoodsShadowContainer::OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return true;
}

//##ModelId=41ECAAB8035B
bool CGoodsShadowContainer::GetGoodsOriginalContainerInformation(const CGUID& guid, tagPreviousContainer& tOriginalContainer)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		tOriginalContainer.lContainerType		= it -> second.lOriginalContainerType;
		tOriginalContainer.ContainerID			= it -> second.OriginalContainerID;
		tOriginalContainer.lContainerExtendID	= it -> second.lOriginalContainerExtendID;
		tOriginalContainer.dwGoodsPosition		= it -> second.dwOriginalGoodsPosition;
		bResult = true;
	}
	return bResult;
}

LONG CGoodsShadowContainer::GetGoodsSourceContainerExtendID(const CGUID& guid)
{
	LONG lResult = 0;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		lResult = it -> second.lOriginalContainerExtendID;
	}
	return lResult;
}

//##ModelId=41E656140271
CGoodsContainer* CGoodsShadowContainer::GetGoodsSourceContainer(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoodsContainer* pResult = NULL;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		if( it -> second.lOriginalContainerType == TYPE_PLAYER )
		{
			CPlayer* pPlayer = GetGame() -> FindPlayer( it -> second.OriginalContainerID );
			if( pPlayer )
			{
				switch( it -> second.lOriginalContainerExtendID )
				{
				case CS2CContainerObjectMove::PEI_PACKET:
					pResult = pPlayer -> getOriginPack();
					break;
				case CS2CContainerObjectMove::PEI_EQUIPMENT:
					pResult = &( pPlayer -> m_cEquipment );
					break;
				case CS2CContainerObjectMove::PEI_WALLET:
					pResult = &( pPlayer->GetPackage()->GetWallet() );
					break;
				case CS2CContainerObjectMove::PEI_GEM:
					pResult = &( pPlayer->GetPackage()->GetGem() );
					break;
                case CS2CContainerObjectMove::PEI_EQUIP_GOD:
                    pResult = &(pPlayer->m_cEquipmentGod);
                    break;
                case CS2CContainerObjectMove::PEI_EQUIP_LAND:
                    pResult = &(pPlayer->m_cEquipmentLand);
                    break;
                case CS2CContainerObjectMove::PEI_EQUIP_HUMAN:
                    pResult = &(pPlayer->m_cEquipmentHuman);
                    break;
				case CS2CContainerObjectMove::PEI_PACK:
					pResult=pPlayer->getSubpackContainer();
					break;
				case CS2CContainerObjectMove::PEI_PACK1:
					pResult=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
					break;
				case CS2CContainerObjectMove::PEI_PACK2:
					pResult=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
					break;
				case CS2CContainerObjectMove::PEI_PACK3:
					pResult=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
					break;
				case CS2CContainerObjectMove::PEI_PACK4:
					pResult=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
					break;
				case CS2CContainerObjectMove::PEI_PACK5:
					pResult=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
					break;
				}
			}
		}
	}
	return pResult;
}


CGoods* CGoodsShadowContainer::GetGoodsByPos(LONG ContainerID,LONG lPos)
{

	CGoods* pGoods=NULL;
	CPlayer* pPlayer = GetGame() -> FindPlayer( this->GetOwnerID() );
	CVolumeLimitGoodsContainer* pContainer=NULL;
	//if(pPlayer)
		//pGoods=pPlayer->FindGoods(ContainerID,lPos);
	//return pGoods;
	
	
	if( pPlayer )
	{
		switch(ContainerID)
		{
		case CS2CContainerObjectMove::PEI_PACKET:
			pContainer = pPlayer -> getOriginPack();
			break;		
		case CS2CContainerObjectMove::PEI_PACK1:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK2:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK3:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK4:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK5:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
			break;
		}
		if(pContainer)
		{
			pGoods=pContainer->GetGoods(lPos);
		}
	}
	return pGoods;
}

DWORD CGoodsShadowContainer::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

DWORD CGoodsShadowContainer::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

/*
CGoods* CGoodsShadowContainer::GetGoodsById(LONG ContainerID,LONG lId)
{
	CGoods* pGoods=NULL;
	CPlayer* pPlayer = GetGame() -> FindPlayer( this->GetOwnerID() );
	CVolumeLimitGoodsContainer* pContainer=NULL;
	//if(pPlayer)
	//pGoods=pPlayer->FindGoods(ContainerID,lPos);
	//return pGoods;
	if( pPlayer )
	{
		switch(ContainerID)
		{
		case CS2CContainerObjectMove::PEI_PACKET:
			pContainer = pPlayer -> getOriginPack();
			break;		
		case CS2CContainerObjectMove::PEI_PACK1:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK2:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK3:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK4:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK5:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
			break;
		}
		if(pContainer)
		{
			pGoods=pContainer->GetGoods()
		}
	}
	return pGoods;
}*/

//##ModelId=41E5EFDF01A5
CBaseObject* CGoodsShadowContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		CGoodsContainer* pContainer = GetGoodsSourceContainer( it -> first );
		if( pContainer )
		{
			pResult = pContainer -> Find( TYPE_GOODS, it -> second.guGoodsID);
		}
	}
	return pResult;
}

//##ModelId=41E5F001029F
CBaseObject* CGoodsShadowContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	//## 该函数依然是一个接口,其实主要的移出工作,还是调用了原容器的移出操作.
	//## 移出消息依然是原容器实现的，这儿无论成功与否，只要在容器内找到了这个
	//## 哈西值，就必须把该物品的节点移出掉。并且发送删除消息给客户端。
	CGoods* pResult = NULL;
	goodsshadowIt it = m_mGoodsShadow.find( guid );
	if( it != m_mGoodsShadow.end() )
	{
		CGUID guGoodId = it -> second.guGoodsID;
		CGoodsContainer* pContainer = GetGoodsSourceContainer( it -> first );
		if( pContainer )
		{
			DWORD dwPosition = 0;
			if( pContainer -> QueryGoodsPosition(guid, dwPosition) )
			{
				CGoods* pGoods = pContainer -> GetGoods( dwPosition );
				if( pGoods && pGoods -> GetAmount() == it -> second.dwGoodsAmount )
				{
					pResult = dynamic_cast<CGoods*>( pContainer -> Remove(TYPE_GOODS, 
						guGoodId, pVoid) );
				}
			}
		}
		//##发送消息给客户端以删除容器内该物品镜像
		RemoveShadow( guid );
	}
	return pResult;
}

CGoods* CGoodsShadowContainer::RemoveEx(const CGUID& PlayerId,LONG ContainerID,LONG lPos)
{
	CVolumeLimitGoodsContainer* pContainer = NULL;
	
	CGoods* pGoods=NULL;
	CPlayer* pPlayer = GetGame() -> FindPlayer( PlayerId );
	if( pPlayer )
	{
		switch( ContainerID )
		{
		case CS2CContainerObjectMove::PEI_PACKET:
			pContainer = pPlayer -> getOriginPack();
			break;		
		case CS2CContainerObjectMove::PEI_PACK1:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK2:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK3:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK4:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK5:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
			break;
		}
	}
	if( pContainer )
	{
		
		pGoods=pContainer->GetGoods(lPos);
		if(pContainer->Remove(pGoods->GetExID(),NULL))
		{
			return pGoods;
		}
		else
			return NULL;
	}
	return pGoods;
}

LONG CGoodsShadowContainer::RemoveEx(const CGUID& PlayerId,LONG ContainerID,LONG lPos,LONG lNum)
{	
	LONG lRet=0;
	if(lNum<=0)
		return lRet;
	CGoodsContainer* pContainer = NULL;
	
	CGoods* pGoods=NULL;
	CPlayer* pPlayer = GetGame() -> FindPlayer( PlayerId );
	if( pPlayer )
	{
		switch( ContainerID )
		{
		case CS2CContainerObjectMove::PEI_PACKET:
			pContainer = pPlayer -> getOriginPack();
			break;
		case CS2CContainerObjectMove::PEI_EQUIPMENT:
			pContainer = &( pPlayer -> m_cEquipment );
			break;
		case CS2CContainerObjectMove::PEI_WALLET:
			pContainer = &( pPlayer->GetPackage()->GetWallet() );
			break;
		case CS2CContainerObjectMove::PEI_GEM:
			pContainer = &( pPlayer->GetPackage()->GetGem() );
			break;
        case CS2CContainerObjectMove::PEI_EQUIP_GOD:
            pContainer = &(pPlayer->m_cEquipmentGod);
            break;
        case CS2CContainerObjectMove::PEI_EQUIP_LAND:
            pContainer = &(pPlayer->m_cEquipmentLand);
            break;
        case CS2CContainerObjectMove::PEI_EQUIP_HUMAN:
            pContainer = &(pPlayer->m_cEquipmentHuman);
            break;
		case CS2CContainerObjectMove::PEI_PACK:
			pContainer=pPlayer->getSubpackContainer();
			break;
		case CS2CContainerObjectMove::PEI_PACK1:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK2:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK3:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK4:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
			break;
		case CS2CContainerObjectMove::PEI_PACK5:
			pContainer=pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
			break;
		}
	}
	if( pContainer )
	{
		pGoods=pContainer->GetGoods(lPos);
		if((long)pGoods->GetAmount()>lNum)
		{
			pGoods->SetAmount(pGoods->GetAmount()-lNum);
			lRet=pGoods->GetAmount();			
		}		
	}
	return lRet;
}


CGoodsShadowContainer::tagGoodsShadow* CGoodsShadowContainer::RemoveByGoodsId(DWORD dwId)
{
	map<CGUID,tagGoodsShadow,hash_guid_compare>::iterator tagIter=m_mGoodsShadow.begin();
	for(;tagIter!=m_mGoodsShadow.end();tagIter++)
	{
		if(tagIter->second.dwGoodsBasePropertiesIndex==dwId)
			break;
	}
	if(tagIter!=m_mGoodsShadow.end())
	{
		tagGoodsShadow* pTag=OBJ_CREATE(tagGoodsShadow);
		pTag->lOriginalContainerType=tagIter->second.lOriginalContainerType;
		pTag->OriginalContainerID=tagIter->second.OriginalContainerID;
		pTag->lOriginalContainerExtendID=tagIter->second.lOriginalContainerExtendID;
		pTag->guGoodsID=tagIter->second.guGoodsID;
		pTag->dwOriginalGoodsPosition=tagIter->second.dwOriginalGoodsPosition;
		pTag->dwGoodsBasePropertiesIndex=tagIter->second.dwGoodsBasePropertiesIndex;
		pTag->dwGoodsAmount=tagIter->second.dwGoodsAmount;		
		m_mGoodsShadow.erase(tagIter);
		return pTag;
	}
	return NULL;	
}

void CGoodsShadowContainer::RemoveShadowByGoodsIndex(LONG lGoodsId)
{
	vector<goodsshadowIt> gcVec;
	goodsshadowIt it=m_mGoodsShadow.begin();
	for(;it!=m_mGoodsShadow.end();it++)
	{
		if(it->second.dwGoodsBasePropertiesIndex==lGoodsId)
		{
			gcVec.push_back(it);
		}
	}

	vector<goodsshadowIt>::iterator shadowIter=gcVec.begin();
	for(;shadowIter!=gcVec.end();shadowIter++)
	{
		m_mGoodsShadow.erase(*shadowIter);
	}
}

void CGoodsShadowContainer::RemoveShadowByGoodsGuid(CGUID& guid)
{	
	m_mGoodsShadow.erase(guid);
}

void CGoodsShadowContainer::AddShadowTag(CGoodsShadowContainer::tagGoodsShadow* pTag)
{
	tagGoodsShadow tShadow;
	tShadow.lOriginalContainerType		= pTag->lOriginalContainerType;
	tShadow.OriginalContainerID		= pTag->OriginalContainerID;
	tShadow.lOriginalContainerExtendID	= pTag ->lOriginalContainerExtendID;
	tShadow.dwOriginalGoodsPosition		= pTag->dwOriginalGoodsPosition;
	tShadow.dwGoodsAmount				= pTag->dwGoodsAmount;	
	tShadow.guGoodsID					= pTag->guGoodsID;
	tShadow.dwGoodsBasePropertiesIndex	= pTag->dwGoodsBasePropertiesIndex;
	//DWORD dwHashValue					= CContainer::CalculateHashValue( TYPE_GOODS, lGoodsID );
	m_mGoodsShadow[pTag->guGoodsID]		= tShadow;
}

DWORD CGoodsShadowContainer::GetGoodsNumByIndex(LONG lId)
{
	DWORD dwRet=0;
	map<CGUID,tagGoodsShadow,hash_guid_compare>::iterator it=m_mGoodsShadow.begin();
	for(;it!=m_mGoodsShadow.end();it++)
	{
		if(it->second.dwGoodsBasePropertiesIndex==lId)
		{
			dwRet+=it->second.dwGoodsAmount;
		}
	}
	return dwRet;
}

//##ModelId=41F46E6100A9
bool CGoodsShadowContainer::GetShadowPosition(tagPreviousContainer* ptPreviousContainer, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( ptPreviousContainer )
	{
		goodsshadowIt it = m_mGoodsShadow.begin();
		for( ; it != m_mGoodsShadow.end(); it ++ )
		{
			if( ptPreviousContainer -> lContainerType == it -> second.lOriginalContainerType &&
				ptPreviousContainer -> ContainerID == it -> second.OriginalContainerID &&
				ptPreviousContainer -> lContainerExtendID == it -> second.lOriginalContainerExtendID &&
				ptPreviousContainer -> dwGoodsPosition == it -> second.dwOriginalGoodsPosition )
			{
				bResult = QueryGoodsPosition( it -> first, dwPosition );
				break;
			}
		}
	}
	return bResult;
}

CBaseObject* CGoodsShadowContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

CBaseObject* CGoodsShadowContainer::Find( LONG oType,const CGUID& guid)
{
	return CGoodsContainer::Find( oType, guid );
}

CGoods* CGoodsShadowContainer::FindByGuid(LONG oType,const CGUID& guid,DWORD& dwNum)
{
	goodsshadowIt it=m_mGoodsShadow.find(guid);
	if(it!=m_mGoodsShadow.end())
	{
		dwNum=it->second.dwGoodsAmount;
		CGoods* pGoods=dynamic_cast<CGoods*>(Find(TYPE_GOODS, it -> first) );
		return pGoods;
	}
	return NULL;
}

//##ModelId=41E6485E0148
CGoodsShadowContainer::tagGoodsShadow::tagGoodsShadow()
{
	// TODO: Add your specialized code here.
		lOriginalContainerType		= 0;
		OriginalContainerID		= NULL_GUID;
		lOriginalContainerExtendID	= 0;
		dwOriginalGoodsPosition		= 0;
		//lGoodsID					= 0;
		dwGoodsAmount				= 0;
		dwGoodsBasePropertiesIndex	= 0;
}

//##ModelId=41E64861006D
CGoodsShadowContainer::tagGoodsShadow::~tagGoodsShadow()
{
	// TODO: Add your specialized code here.
	//vGoodsShadowStream.clear();
}
