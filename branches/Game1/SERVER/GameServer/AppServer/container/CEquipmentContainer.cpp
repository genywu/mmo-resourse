// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Player.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "CEquipmentContainer.h"
#include "CContainer.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"
#include "../../../../public/StrUnifyInput.h"



DWORD CEquipmentContainer::s_dwEquipmentLimit = CEquipmentContainer::EC_TOTAL;

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
BOOL CEquipmentContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods) 
	{
		CGoodsBaseProperties* pProperties = 
			CGoodsFactory::QueryGoodsBaseProperties( pGoods -> GetBasePropertiesIndex() );
		if( pProperties && pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT )
		{
			switch( pProperties -> GetEquipPlace() )
			{			
			case CGoodsBaseProperties::EP_HEAD:
				bResult = Add( EC_HEAD, pGoods, pPreviousContainer, pVoid );
				break;
			case CGoodsBaseProperties::EP_BODY:
				bResult = Add( EC_BODY, pGoods, pPreviousContainer, pVoid );
				break;			
			case CGoodsBaseProperties::EP_GLOVE:
				bResult = Add( EC_GLOVE, pGoods, pPreviousContainer, pVoid );
				break;
			case CGoodsBaseProperties::EP_BOOT:
				bResult = Add( EC_BOOT, pGoods, pPreviousContainer, pVoid );
				break;			
			case CGoodsBaseProperties::EP_HEADGEAR:
				bResult = Add( EC_HEADGEAR, pGoods, pPreviousContainer, pVoid );
				break;
			case CGoodsBaseProperties::EP_FROCK:
				bResult = Add( EC_FROCK, pGoods, pPreviousContainer, pVoid );
				break;
			case CGoodsBaseProperties::EP_WING:
				bResult = Add( EC_WING, pGoods, pPreviousContainer, pVoid );
				break;
			case CGoodsBaseProperties::EP_FAIRY:
				bResult = Add( EC_FAIRY, pGoods, pPreviousContainer, pVoid );
				break;
			default:
				break;
			}
		}
	}
	return bResult;
}

//##ModelId=41B3D0C20191
VOID CEquipmentContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin(); 
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
VOID CEquipmentContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( GetContainerMode() == CGoodsContainer::GCM_NORMAL )
	{
		map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.begin();
		for( ; it != m_mEquipment.end(); it ++ )
		{
#ifdef _GOODSLOG1_
			if(it->second)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				it->second->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CEquipmentContainer析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					it->second->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(it->second->GetBasePropertiesIndex()),srcGoodsGUID,it->second->GetAmount());
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
VOID CEquipmentContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( pListener -> OnTraversingContainer(this, it -> second) == FALSE )
		{
			//break;
		}
	}
}


BOOL CEquipmentContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			bResult	= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41B570CD0068
BOOL CEquipmentContainer::Swap(EQUIPMENT_COLUMN ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pIn && ppOut )
	{		
		CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( pIn -> GetBasePropertiesIndex() );
		if( pProperties && pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT )
		{
			BOOL bIsValidEquipment = FALSE;
			switch( pProperties -> GetEquipPlace() )
			{			
			case CGoodsBaseProperties::EP_HEAD:
				if( ecColumn == EC_HEAD )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BACK:
				if(ecColumn==EC_BACK)
				{
					bIsValidEquipment=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON:
				if( ecColumn == EC_WEAPON )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON2:
				if( ecColumn == EC_WEAPON2 )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BODY:
				if( ecColumn == EC_BODY )
				{
					bIsValidEquipment = TRUE;
				}
				break;				
			case CGoodsBaseProperties::EP_GLOVE:
				if( ecColumn == EC_GLOVE )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HORSE:
				if( ecColumn == EC_HORSE )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BOOT:
				if( ecColumn == EC_BOOT )
				{
					bIsValidEquipment = TRUE;
				}
				break;				
			case CGoodsBaseProperties::EP_HEADGEAR:
				if( ecColumn == EC_HEADGEAR )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_FROCK:
				if( ecColumn == EC_FROCK )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_MEDAL1:
				if( ecColumn == EC_MEDAL1 || ecColumn == EC_MEDAL2 || ecColumn == EC_MEDAL3 )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_DECORATION1:
				if( ecColumn == EC_DECORATION1 || ecColumn == EC_DECORATION2 )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_LRING:
				if(ecColumn==EC_LRING || ecColumn==EC_RRING)
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_NECKLACE:
				if(ecColumn==EC_NECKLACE)
				{
					bIsValidEquipment=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WING:
				if( ecColumn == EC_WING )
				{
					bIsValidEquipment = TRUE;
				}
				break;				
			case CGoodsBaseProperties::EP_FAIRY:
				if( ecColumn == EC_FAIRY )
				{
					bIsValidEquipment = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_CLOAK:
				if(ecColumn==EC_CLOAK)
				{
					bIsValidEquipment=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_ADORN:
				if(ecColumn==EC_ADORN)
				{
					bIsValidEquipment=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GUARDIAN:
				if(ecColumn==EC_GUARDIAN)
				{
					bIsValidEquipment=TRUE;
				}
				break;
			default:
				break;
			}
			if( bIsValidEquipment )
			{
				CGoods* pOriginalGoods = GetGoods( ecColumn );
				CGoods* pWeapon2=NULL;
				
				//BOOL bCanSwap=FALSE;
				CPlayer* pPlayer=GetGame()->FindPlayer(GetOwnerID());
				if( pOriginalGoods && pPlayer)
				{
					if(ecColumn==EC_WEAPON)
					{
						//判断是否有副手武器
						pWeapon2=GetGoods(EC_WEAPON2);
						if(pWeapon2)
						{						
							//副手武器类型
							eGoodsBaseType eType=pWeapon2->GetGoodsBaseType();
							//主手武器类型
							eWeaponType eWt=pOriginalGoods->GetWeaponType();						
							//要替换武器的类型
							eWeaponType eWtSwap=pIn->GetWeaponType();
							//要替换武器单手OR双手
							eWeaponHandType eWhtSwap=pIn->GetWeaponHandType();
							if(eWhtSwap==WHT_DOUBLE_HAND ||(eType==GT_WEAPON2 && eWhtSwap!=WHT_SINGLE_HAND) ||
								(eType==GT_ARROW && eWhtSwap!=WT_BOW))
							{
								// 要替换武器是双手武器，检查背包空间
								DWORD dwPackSpace=pPlayer->GetTotalEmptySpaceNum();
								if(dwPackSpace>=1)
								{
									SetBackToPackGoods(pWeapon2);
								}
								else
								{
									//背包空间不够不能交换
									char* strRes=CStringReading::LoadString(20,33);
									if(strRes)
									{
										pPlayer->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
									}									
									return FALSE;
								}
							}
							
						}
					}

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
						bResult = TRUE;
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
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			//if( CContainer::CalculateHashValue( it -> second -> GetType(), 
			//	it -> second -> GetID() ) == dwHashCode )
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
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			//if( CContainer::CalculateHashValue( it -> second -> GetType(), 
			//	it -> second -> GetID() ) == dwHashCode )
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
BOOL CEquipmentContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObject == NULL )
	{
		return FALSE;
	}

	EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( dwPosition );
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		if( it -> second != NULL )
		{
			return FALSE;
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
				char* pStrMsg=NULL;
				if(lRet==CPlayer::LEVEL_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,11);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//等级不够
				}
				else if(lRet==CPlayer::STR_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,12);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//力量不够
				}
				else if(lRet==CPlayer::DEX_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,13);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//敏捷不够
				}
				else if(lRet==CPlayer::CON_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,14);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//体质不够
				}
				else if(lRet==CPlayer::INT_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,15);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//灵力不够
				}
				else if(lRet==CPlayer::OCCUPATION_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,16);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//职业不对
				}
				else if(lRet==CPlayer::SEX_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,17);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//性别不对
				}
				else if(lRet==CPlayer::SKILL_NOTFIT)
				{
					pStrMsg=CStringReading::LoadString(20,18);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
					//技能不对
				}
				else if(lRet==CPlayer::REGION_FORBID)
				{
					//场景禁止
					pStrMsg=CStringReading::LoadString(20,19);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
				}
				else if(lRet==CPlayer::COOLDOWN_NOTFIT)
				{
					//冷却不对
					pStrMsg=CStringReading::LoadString(20,21);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
				}
				else if(lRet==CPlayer::EQUIP_TYPE_NOTFIT)
				{
					//防具类型不对
					pStrMsg=CStringReading::LoadString(20,22);
					if(pStrMsg)
						pOwner->SendNotifyMessage(pStrMsg,NOTIFY_COLOR_RED);
				}
				return FALSE;
			}
		}
		else
		{
			//##暂时屏蔽，会导致反序列化时丢失数据。
			//return FALSE;
		}
	}	

	//如果物品是盾牌
	if(pObject->GetGoodsBaseType()==GT_WEAPON2)
	{
		//取得主手武器
		CGoods* pWeapon=GetGoods(EC_WEAPON);
		if(pWeapon)
		{
			eWeaponType eWT=pWeapon->GetWeaponType();			
			if(eWT!=WT_SINGLE_HAMMER && eWT!=WT_SINGLE_KNIFE && eWT!=WT_SINGLE_SWORD)
			{
				char* strRes=CStringReading::LoadString(20,28);
				if(strRes && pOwner)
					pOwner->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
				return FALSE;
			}
		}
	}

	//如果装备是箭矢
	if(pObject->GetGoodsBaseType()==GT_ARROW)
	{
		//取得主手武器
		CGoods* pWeapon=GetGoods(EC_WEAPON);
		if(pWeapon)
		{
			eWeaponType eWT=pWeapon->GetWeaponType();			
			if(eWT!=WT_BOW)
			{
				char* strRes=CStringReading::LoadString(20,29);
				if(strRes && pOwner)
					pOwner->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
				return FALSE;
			}
		}
		else
		{
			char* strRes=CStringReading::LoadString(20,29);
			if(strRes && pOwner)
				pOwner->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
			return FALSE;
		}
	}

	//是否有副手武器
	if(pObject->GetGoodsBaseType()==GT_WEAPON)
	{	
		CGoods* pWeapon2=GetGoods(EC_WEAPON2);
		if(pWeapon2)
		{
			if(pWeapon2->GetGoodsBaseType()==GT_ARROW)
			{
				eWeaponType eWT=pObject->GetWeaponType();
				if(eWT!=WT_BOW)
				{
					//需要把箭矢弹回
					CPlayer* pPlayer=GetGame()->FindPlayer(GetOwnerID());
					if(pPlayer)
					{
						DWORD dwEmptySpace=pPlayer->GetTotalEmptySpaceNum();
						if(dwEmptySpace<1)
						{
							char* strRes=CStringReading::LoadString(20,30);
							if(strRes && pOwner)
								pOwner->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
							return FALSE;
						}

						SetBackToPackGoods(pWeapon2);
					}
				}
			}
			else if(pWeapon2->GetGoodsBaseType()==GT_WEAPON2)
			{
				eWeaponType eWT=pObject->GetWeaponType();
				if(eWT!=WT_SINGLE_SWORD && eWT!=WT_SINGLE_KNIFE && eWT!=WT_SINGLE_HAMMER)
				{
					//需要把盾牌弹回
					CPlayer* pPlayer=GetGame()->FindPlayer(GetOwnerID());
					if(pPlayer)
					{
						DWORD dwEmptySpace=pPlayer->GetTotalEmptySpaceNum();
						if(dwEmptySpace<1)
						{
							char* strRes=CStringReading::LoadString(20,30);
							if(strRes && pOwner)
								pOwner->SendNotifyMessage(strRes,NOTIFY_COLOR_RED);
							return FALSE;
						}

						SetBackToPackGoods(pWeapon2);
					}
				}
			}
		}
	}

	BOOL bResult = FALSE;
	CGoodsBaseProperties* pProperties = 
		CGoodsFactory::QueryGoodsBaseProperties( pObject -> GetBasePropertiesIndex() );
	if( pProperties )
	{
		if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT )
		{
			switch( pProperties -> GetEquipPlace() )
			{
			case CGoodsBaseProperties::EP_BODY:
				if( ecColumn == CEquipmentContainer::EC_BODY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HORSE:
				if(ecColumn==CEquipmentContainer::EC_HORSE)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BOOT:
				if( ecColumn == CEquipmentContainer::EC_BOOT )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GLOVE:
				if( ecColumn == CEquipmentContainer::EC_GLOVE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_NECKLACE:
				if( ecColumn == CEquipmentContainer::EC_NECKLACE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_LRING:
				if( ecColumn == CEquipmentContainer::EC_LRING || ecColumn==CEquipmentContainer::EC_RRING)
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;

			case CGoodsBaseProperties::EP_WEAPON:
				if( ecColumn == CEquipmentContainer::EC_WEAPON )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON2:
				if( ecColumn == CEquipmentContainer::EC_WEAPON2 )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			
			case CGoodsBaseProperties::EP_BACK:
				if( ecColumn == CEquipmentContainer::EC_BACK )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HEAD:
				if( ecColumn == CEquipmentContainer::EC_HEAD )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HEADGEAR:
				if( ecColumn == EC_HEADGEAR )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;				
			case CGoodsBaseProperties::EP_FROCK:
				if( ecColumn == EC_FROCK )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;

			case CGoodsBaseProperties::EP_MEDAL1:
				if( EC_MEDAL1 == ecColumn || EC_MEDAL2 == ecColumn || EC_MEDAL3 == ecColumn )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WING:
				if( ecColumn == EC_WING )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;				
			case CGoodsBaseProperties::EP_FAIRY:
				if( ecColumn == EC_FAIRY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;	
			case CGoodsBaseProperties::EP_CLOAK:
				if(ecColumn==EC_CLOAK)
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_DECORATION1:
				if( EC_DECORATION1 == ecColumn || EC_DECORATION2 == ecColumn )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_ADORN:
				if(EC_ADORN==ecColumn)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GUARDIAN:
				if(EC_GUARDIAN==ecColumn)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;

			default:
				break;
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
VOID CEquipmentContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			it -> second -> AI();
		}
	}
}

//##ModelId=41BFA1030271
BOOL CEquipmentContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult			= FALSE;
	DWORD dwNumEquipment	= 0;
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			++ dwNumEquipment;
		}
	}
	if( dwNumEquipment == CEquipmentContainer::s_dwEquipmentLimit )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41BFA16E03B9
BOOL CEquipmentContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second == pGoods )
		{
			dwPosition	= it -> first;
			bResult		= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA17A00BB
BOOL CEquipmentContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		//if( it -> second && CContainer::CalculateHashValue( it -> second -> GetType(), 
		//	it -> second -> GetID() ) == dwHashValue )
		if(it->second && it->second->GetExID() == guid)
		{
			dwPosition	= it -> first;
			bResult		= TRUE;
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
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
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
	EQUIPMENT_COLUMN ecColumn	= static_cast<EQUIPMENT_COLUMN>( dwPosition );
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		pResult = it -> second;
	}	
	return pResult;
}

//##ModelId=41BFA1B8007D
VOID CEquipmentContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( it -> second );
		}
	}
}

VOID CEquipmentContainer::GetGoodsByID(DWORD dwGoodsBasePropertiesIndex,vector<CGoods*>& vOut)
{
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( it -> second );
		}
	}
}

VOID CEquipmentContainer::SetBackToPackGoods(CGoods* pGoods)
{
	if(pGoods)
	{
		DWORD dwPos=0;
		if(QueryGoodsPosition(pGoods,dwPos))
			m_pGoodsBackToPack=pGoods;
	}
}



BOOL CEquipmentContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	
	if( pGoodsContainer && pGoods && GetOwnerType() == TYPE_PLAYER )
	{
		CPlayer* pPlayer = GetGame() -> FindPlayer( GetOwnerID() );
		if( pPlayer)
		{
			DWORD dwPosition = 0;
			if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				if (pGoods->HasAddonProperty(GAP_GOODS_MAXIMUM_DURABILITY)&&pGoods->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY,2)==0)
				{
				}
				else 
				{
					pGoods->RunScriptOnEquip(pPlayer);				
					pPlayer->MountEquipEx(pGoods,true);
					pPlayer->UpdateAttribute();
					pPlayer->ProduceEvent(ET_EquipMount,
						CS2CContainerObjectMove::PEI_EQUIPMENT,
						dwPosition,
						pGoods->GetGoodsBaseType(), 
						pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),
						(void*)CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
				}
				//武器换装属性
				long lTime = 0;
				if (pGoods->HasAddonProperty(GAP_WEAPON_MODEL_TIME))
				{
					if (pGoods->GetAddonPropertyValues(GAP_WEAPON_MODEL_TIME,1) == 1)
					{
						lTime = pGoods->GetAddonPropertyValues(GAP_WEAPON_MODEL_TIME,2)*60;

						long lEndTime = time(NULL) + lTime;
						if (lEndTime<0)
						{
							lEndTime = 0;
						}
						
						pGoods->ChangeAttribute(GAP_WEAPON_MODEL_TIME,2,1,true);
						pGoods->ChangeAttribute(GAP_WEAPON_MODEL_TIME,lEndTime,2,true);
						pGoods->UpdateAttribute(pPlayer->GetExID());
					}
					else if (pGoods->GetAddonPropertyValues(GAP_WEAPON_MODEL_TIME,1) == 2)
					{
						lTime = pGoods->GetAddonPropertyValues(GAP_WEAPON_MODEL_TIME,2);
						lTime = lTime - time(NULL);
						if (lTime<0)
						{
							lTime = 0;
						}
					}	

					if (lTime>0)
					{
						pPlayer->RegisterChangeGoodsTimerID(pGoods->GetExID(),lTime*1000);
					}
					else
					{
						pGoods->ChangeAttribute(GAP_WEAPON_MODEL_TIME,0,2,true);
						pGoods->UpdateAttribute(pPlayer->GetExID());
					}
				}
				

				CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
				msg.Add( GetOwnerID() );
				msg.Add( dwPosition );
				msg.Add( BYTE(1) /*表示穿上装备*/ );
				msg.Add( pGoods -> GetBasePropertiesIndex() );
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) );
				//剩余时间
				msg.Add( lTime );
				if (lTime>0)
				{
					//主手模型ID
					msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_MODEL_EX,1));
					//副手模型ID
					msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_MODEL_EX,2));
				}
				msg.SendToAround( pPlayer, pPlayer );

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
	return TRUE;
}

//##ModelId=41BFB92601A5
BOOL CEquipmentContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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
				map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.find( 
					static_cast<EQUIPMENT_COLUMN>(dwPosition) );
				if( it != m_mEquipment.end() )
				{
					if (pGoods->HasAddonProperty(GAP_GOODS_MAXIMUM_DURABILITY)&&pGoods->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY,2)==0)
					{	
						CGoods* pUnmountedEquipment = it -> second;
						it -> second = NULL;
						it -> second = pUnmountedEquipment;
					}
					else
					{						//##保存零时变				
						CGoods* pUnmountedEquipment = it -> second;
						it -> second = NULL;
						//##恢复
						pGoods->RunScriptOnRemoveEquip(pPlayer);
						pPlayer->MountEquipEx(pGoods,false);
						pPlayer->UpdateAttribute( TRUE );
						pPlayer->ProduceEvent(	ET_EquipUnmount,
							CS2CContainerObjectMove::PEI_EQUIPMENT,
							dwPosition,
							pGoods->GetGoodsBaseType(), 
							pGoods-> GetAddonPropertyValues( GAP_WEAPON_CATEGORY, 1 ),  
							(void*)CGoodsFactory::QueryGoodsOriginalName(pGoods->GetBasePropertiesIndex()));
						it -> second = pUnmountedEquipment;
					}
				}

			


				//武器换装属性
				long lTime = 0;
				if (pGoods->HasAddonProperty(GAP_WEAPON_MODEL_TIME) )
				{
					lTime = pGoods->GetAddonPropertyValues(GAP_WEAPON_MODEL_TIME,2);
					lTime = lTime - timeGetTime();
					if (lTime<=0)
					{
						lTime = 0;						
					}
					pPlayer->CancelChangeTimer(pGoods->GetExID());
				}

				

				//##给周围的玩家发送消息.
				CMessage msg( MSG_S2C_PLAYER_CHANGE_EQUIPMENT );
				msg.Add( GetOwnerID() );
				msg.Add( dwPosition );
				msg.Add( BYTE(0) /*表示卸下装备*/ );
				msg.Add( pGoods -> GetBasePropertiesIndex() );
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) );
				//时效
				msg.Add( lTime);
				//主手模型ID
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_MODEL_EX,1));
				//副手模型ID
				msg.Add( pGoods -> GetAddonPropertyValues(GAP_WEAPON_MODEL_EX,2));

				msg.SendToAround( pPlayer );
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
	return TRUE;
}

//##ModelId=41BFEAD70138

DWORD CEquipmentContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
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
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
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

BOOL CEquipmentContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );

	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				setWriteDB.AddToByteArray( static_cast<DWORD>(it -> first) );
				it -> second -> Serialize( setWriteDB, b );
			}
		}
	}

	return TRUE;
}
BOOL CEquipmentContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	CEquipmentContainer::Clear();
	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( 
			setReadDB.GetDwordFromByteArray() );
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,13 );
		if( pGoods )
		{
			CGoodsFactory::UpdateEquipmentAttr(pGoods);
			Add( ecColumn, pGoods, NULL );
		}
	}

	return TRUE;
}

//##ModelId=41C0E6DB0119
CBaseObject* CEquipmentContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	//CBaseObject* pObj= CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
	//如果拿起背包装备，要更新容器容积
	/*
	if(dwPosition>=CS2CContainerObjectMove::PEI_PACK1 && dwPosition<=CS2CContainerObjectMove::PEI_PACK5)
	{
		int pos=dwPosition-10000;
		CPlayer* pOwner = GetGame() -> FindPlayer( GetOwnerID() );
		if( pOwner )
		{
			pOwner->getEquipPack(pos)->ChangeContainerVolume(0,0);
		}		
	}	*/
	CGoods* pResult = NULL;
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.find(static_cast<EQUIPMENT_COLUMN>(dwPosition));
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
