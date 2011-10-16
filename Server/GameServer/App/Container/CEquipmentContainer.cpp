// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "../Player.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "CEquipmentContainer.h"
#include "CContainer.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"
#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CEquipmentContainer::s_dwEquipmentLimit = EC_TOTAL;

//##ModelId=41B3CA7F02A3
CEquipmentContainer::CEquipmentContainer()
{
	// TODO: Add your specialized code here.	
	m_pGoodsBackToPack=NULL;
	AddListener( this );
}

//##ModelId=41B3CA960090
CEquipmentContainer::~CEquipmentContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41B3D0AE026C
bool CEquipmentContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods) 
	{
		bResult = Add( pGoods->GetEquipType(), pGoods, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41B3D0C20191
void CEquipmentContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin(); 
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second)
		{			
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				if( m_vListeners[i] )
				{
					m_vListeners[i] -> OnObjectRemoved( this, it -> second, 
						it -> second -> GetAmount(),pVoid );
				}				
			}
			//##用完之后先把这个置为NULL。但是不删除该物品。也许这个容器的克隆容器会使用。
			it -> second = NULL;
		}
	}
	m_mEquipment.clear();
}

//##ModelId=41B3D0C9024D
void CEquipmentContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( GetContainerMode() == CGoodsContainer::GCM_NORMAL )
	{
		map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
		for( ; it != m_mEquipment.end(); it ++ )
		{
#ifdef _GOODSLOG1_
			if(it->second)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				it->second->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CEquipmentContainer析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					it->second->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(it->second->GetBasePropertiesIndex()),srcGoodsGUID,it->second->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif	
			CGoodsFactory::GarbageCollect( &(it -> second) ,2);
		}
	}
	m_mEquipment.clear();
	CGoodsContainer::Release();
	AddListener( this );
}

//##ModelId=41B418F902D3
void CEquipmentContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( pListener -> OnTraversingContainer(this, it -> second) == false )
		{
			//break;
		}
	}
}


bool CEquipmentContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			bResult	= true;
			break;
		}
	}
	return bResult;
}

//##ModelId=41B570CD0068
bool CEquipmentContainer::Swap(eEquipType ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	if( pIn && ppOut )
	{		
		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( pIn -> GetBasePropertiesIndex() );
		if( pProperties && pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
		{
			bool bIsValidEquipment = false;
			if (ecColumn == pIn->GetEquipType())
			{
				bIsValidEquipment = true;
			}

			if( bIsValidEquipment )
			{
				CGoods* pOriginalGoods = GetGoods( ecColumn );
				CGoods* pWeapon2=NULL;
				
				//bool bCanSwap=false;
				CPlayer* pPlayer=GetGame()->FindPlayer(GetOwnerID());
				if( pOriginalGoods && pPlayer)
				{
					Remove(pOriginalGoods);		
					if( Add(ecColumn, pIn, NULL) )
					{
						CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
						if( pMsg )
						{
							pMsg -> SetSourceObject( pIn -> GetType(),pIn->GetExID() );
							pMsg -> SetDestinationObject( pOriginalGoods -> GetType(),pOriginalGoods->GetExID());
						}
						*ppOut = pOriginalGoods;
						bResult = true;
					}
					else
					{
						ClearBackToPackGoods();						
					}					
				}
			}
		}
	}
	return bResult;
}

//##ModelId=41B3D0A1025D
CBaseObject* CEquipmentContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( (*it).second->GetExID() == guid )
			{
				pResult = it -> second;
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41B3D0B80395
CBaseObject* CEquipmentContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( (*it).second->GetExID() == guid )
			{
				pResult = it -> second;
				break;
			}
		}
	}
	if( pResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectRemoved( this, pResult, pResult -> GetAmount(), pVoid );
			}
		}
		m_mEquipment.erase( it );
	}

	return pResult;
}



//##ModelId=41BF9DAB0109
bool CEquipmentContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObject == NULL )
	{
		return false;
	}

	eEquipType ecColumn = static_cast<eEquipType>( dwPosition );
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		if( it -> second != NULL )
		{
			return false;
		}
	}
	CPlayer* pOwner=GetGame() -> FindPlayer( GetOwnerID() );
	if( GetOwnerType() == TYPE_PLAYER )
	{		
		if( pOwner )
		{
			/*
			enum EquipRestrict
			{
			UNKNOW_NOTFIT,
			LEVEL_NOTFIT,		//等级不够
			STR_NOTFIT,			//力量不够
			DEX_NOTFIT,			//敏捷不够
			CON_NOTFIT,			//体质不够
			INT_NOTFIT,			//灵力不够
			OCCUPATION_NOTFIT,	//职业不对
			SEX_NOTFIT,			//性别不对
			SKILL_NOTFIT,		//技能不对
			REGION_FORBID,		//场景禁止
			COOLDOWN_NOTFIT,	//冷却不够
			EQUIP_TYPE_NOTFIT,	//防具类型不对
			EQUIPFIT,			//可以装备
			};
			*/
			long lRet=pOwner->CanMountEquip(pObject);
			if( lRet!= CPlayer::EQUIPFIT )
			{
				const char* pStrMsg=NULL;
				if(lRet==CPlayer::LEVEL_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("11");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//等级不够
				}
				else if(lRet==CPlayer::STR_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("12");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//力量不够
				}
				else if(lRet==CPlayer::DEX_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("13");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//敏捷不够
				}
				else if(lRet==CPlayer::CON_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("14");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//体质不够
				}
				else if(lRet==CPlayer::INT_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("15");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//灵力不够
				}
				else if(lRet==CPlayer::OCCUPATION_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("16");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//职业不对
				}
				else if(lRet==CPlayer::SEX_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("17");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//性别不对
				}
				else if(lRet==CPlayer::SKILL_NOTFIT)
				{
					pStrMsg=AppFrame::GetText("18");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
					//技能不对
				}
				else if(lRet==CPlayer::REGION_FORBID)
				{
					//场景禁止
					pStrMsg=AppFrame::GetText("19");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
				}
				else if(lRet==CPlayer::COOLDOWN_NOTFIT)
				{
					//冷却不对
					pStrMsg=AppFrame::GetText("21");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
				}
				else if(lRet==CPlayer::EQUIP_TYPE_NOTFIT)
				{
					//防具类型不对
					pStrMsg=AppFrame::GetText("22");
					if(pStrMsg)
						//pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
						pOwner->SendNotifyMessage(pStrMsg);
				}
				return false;
			}
		}
		else
		{
			//##暂时屏蔽，会导致反序列化时丢失数据。
			//return false;
		}
	}	

	bool bResult = false;
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( pObject -> GetBasePropertiesIndex() );
	if( pProperties )
	{
		if( pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT )
		{
			if (ecColumn == pObject->GetEquipType())
			{
				bResult = true;
			}

            if(bResult)
            {
                m_mEquipment[ecColumn]=pObject;
            }
		}		
	}
	if( bResult )
	{
		
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pObject -> GetAmount(), pVoid );
			}
		}
	}
	return bResult;
}

//##ModelId=41BFA0F30177
void CEquipmentContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			it -> second -> AI();
		}
	}
}

//##ModelId=41BFA1030271
bool CEquipmentContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult			= false;
	DWORD dwNumEquipment	= 0;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			++ dwNumEquipment;
		}
	}
	if( dwNumEquipment == CEquipmentContainer::s_dwEquipmentLimit )
	{
		bResult = true;
	}
	return bResult;
}

//##ModelId=41BFA16E03B9
bool CEquipmentContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return false;
	}

	bool bResult = false;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second == pGoods )
		{
			dwPosition	= it -> first;
			bResult		= true;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA17A00BB
bool CEquipmentContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	bool bResult = false;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if(it->second && it->second->GetExID() == guid)
		{
			dwPosition	= it -> first;
			bResult		= true;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA197036B
CGoods* CEquipmentContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			pResult	= it -> second;
			break;
		}
	}
	return pResult;
}

//##ModelId=41BFA1AC0242
CGoods* CEquipmentContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	CGoods* pResult				= NULL;
	eEquipType ecColumn	= static_cast<eEquipType>( dwPosition );
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		pResult = it -> second;
	}	
	return pResult;
}

//##ModelId=41BFA1B8007D
void CEquipmentContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( it -> second );
		}
	}
}

void CEquipmentContainer::GetGoodsByID(DWORD dwGoodsBasePropertiesIndex,vector<CGoods*>& vOut)
{
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( it -> second );
		}
	}
}

void CEquipmentContainer::SetBackToPackGoods(CGoods* pGoods)
{
	if(pGoods)
	{
		DWORD dwPos=0;
		if(QueryGoodsPosition(pGoods,dwPos))
			m_pGoodsBackToPack=pGoods;
	}
}



bool CEquipmentContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	
	if( pGoodsContainer && pGoods && GetOwnerType() == TYPE_PLAYER )
	{
		CPlayer* pPlayer = GetGame() -> FindPlayer( GetOwnerID() );
		if( pPlayer )
		{
			DWORD dwPosition = 0;
			if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				pGoods->RunScriptOnEquip(pPlayer);				

				pPlayer->MountEquipEx(pGoods,true);
				pPlayer->UpdateAttribute();

				CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
				msg.Add( GetOwnerID() );
				msg.Add( dwPosition );
				msg.Add( BYTE(1) /*表示穿上装备*/ );
				msg.Add( pGoods -> GetBasePropertiesIndex() );
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) );
				msg.SendToAround( pPlayer, pPlayer );

		
				pPlayer->ProduceEvent(ET_EquipMount,CS2CContainerObjectMove::PEI_EQUIPMENT,dwPosition,
                    pGoods->GetGoodsBaseType(), pGoods->GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),
                    (void*)GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
			}
		}
	}
	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetDestinationObject( pGoods -> GetType(),pGoods->GetExID() );
			pMsg -> SetDestinationObjectAmount( pGoods -> GetAmount() );
		}
	}
	return true;
}

//##ModelId=41BFB92601A5
bool CEquipmentContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );

	if( pGoodsContainer && pGoods && GetOwnerType() == TYPE_PLAYER )
	{
		CPlayer* pPlayer = GetGame() -> FindPlayer( GetOwnerID() );
		if( pPlayer )
		{
			//##为了实现刷新属性,必须先移出该物品.但是刷新后又必须放回去.
			//##才能保证逻辑的正确性.
			DWORD dwPosition = 0;
			if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				map<eEquipType,CGoods*>::iterator it = m_mEquipment.find( 
					static_cast<eEquipType>(dwPosition) );
				if( it != m_mEquipment.end() )
				{
                    pGoods->RunScriptOnRemoveEquip(pPlayer);
					//##保存零时变量
					CGoods* pUnmountedEquipment = it -> second;
					it -> second = NULL;
					
                    LONG lCurrentDurability = pUnmountedEquipment->GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
                    if( !(pGoods->HasAddonProperty(GAP_GOODS_MAX_DURABILITY)&&lCurrentDurability == 0) )
					    pPlayer->MountEquipEx(pGoods,false);
					pPlayer->UpdateAttribute( true );
					
					//##恢复
					it -> second = pUnmountedEquipment;
				}

				//##给周围的玩家发送消息.
				CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
				msg.Add( GetOwnerID() );
				msg.Add( dwPosition );
				msg.Add( BYTE(0) /*表示卸下装备*/ );
				msg.Add( pGoods -> GetBasePropertiesIndex() );
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) );
				msg.SendToAround( pPlayer );
				//pPlayer->Proc(EVENT_OnEquipUnMount,(const void*)dwPosition,(const void *)(pGoods -> GetBasePropertiesIndex()));
				pPlayer->ProduceEvent(ET_EquipUnmount,CS2CContainerObjectMove::PEI_EQUIPMENT,dwPosition,
                    pGoods->GetGoodsBaseType(), pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),  
                    (void*)GoodsSetup::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
			}
		}
	}
	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetSourceObject( pGoods -> GetType(),pGoods->GetExID() );
			pMsg -> SetSourceObjectAmount( dwAmount );
		}
	}
	return true;
}

//##ModelId=41BFEAD70138

DWORD CEquipmentContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ))
			{
				++ dwAmount;
			}
		}
	}
	return dwAmount;
}


DWORD CEquipmentContainer::GetEquipAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) && it->first < 10001)
			{
				++ dwAmount;
			}
		}
	}
	return dwAmount;
}

DWORD CEquipmentContainer::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

DWORD CEquipmentContainer::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}


CBaseObject* CEquipmentContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CEquipmentContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CEquipmentContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

CBaseObject* CEquipmentContainer::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}

bool CEquipmentContainer::Serialize( DBWriteSet& setWriteDB, bool b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );

	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				setWriteDB.AddToByteArray( static_cast<DWORD>(it -> first) );
				it -> second -> Serialize( setWriteDB, b );
			}
		}
	}

	return true;
}
bool CEquipmentContainer::Unserialize( DBReadSet& setReadDB, bool b )
{
	CEquipmentContainer::Clear();
	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		eEquipType ecColumn = static_cast<eEquipType>( 
			setReadDB.GetDwordFromByteArray() );
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,13 );
		if( pGoods )
		{
			CGoodsFactory::UpdateEquipmentAttr(pGoods);
			Add( ecColumn, pGoods, NULL );
		}
	}

	return true;
}

//##ModelId=41C0E6DB0119
CBaseObject* CEquipmentContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.find(static_cast<eEquipType>(dwPosition));
	if(it!=m_mEquipment.end())
	{
		pResult=it->second;		
	}
	
	if( pResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectRemoved( this, pResult, pResult -> GetAmount(), pVoid );
			}
		}
		m_mEquipment.erase( it );
	}

	return pResult;	
}
