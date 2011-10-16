// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Player.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "CEquipmentContainer.h"
#include "CContainer.h"
//#include "..\Message Packaging\CS2CContainerObjectMove.h"




DWORD CEquipmentContainer::s_dwEquipmentLimit = 9;

//##ModelId=41B3CA7F02A3
CEquipmentContainer::CEquipmentContainer()
{
	// TODO: Add your specialized code here.
	AddListener( this );
}

////##ModelId=41B3CA9E02D2
//CEquipmentContainer::CEquipmentContainer(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	AddListener( this );
//}

//##ModelId=41B3CA960090
CEquipmentContainer::~CEquipmentContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41B3D0AE026C
BOOL CEquipmentContainer::Add(CBaseObject* pObject, LPVOID pVoid)
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
			case CGoodsBaseProperties::EP_UNKNOW:
				break;
			case CGoodsBaseProperties::EP_HEAD:
				bResult = Add( EC_HEAD, pGoods, pVoid );
				break;
			case CGoodsBaseProperties::EP_BODY:
				bResult = Add( EC_BODY, pGoods, pVoid );
				break;
			//case CGoodsBaseProperties::EP_POSTERIOR:
				//bResult = Add( EC_POSTERIOR, pGoods, pVoid );
				//break;
			case CGoodsBaseProperties::EP_WEAPON:
				bResult = Add( EC_WEAPON, pGoods, pVoid );
				break;
			case CGoodsBaseProperties::EP_GLOVE:
				bResult = Add( EC_GLOVE, pGoods, pVoid );
				break;
			case CGoodsBaseProperties::EP_BOOT:
				bResult = Add( EC_BOOT, pGoods, pVoid );
				break;
			//case CGoodsBaseProperties::EP_JEWELRY:
				//bResult = Add( EC_JEWELRY, pGoods, pVoid );
				//break;
			//case CGoodsBaseProperties::EP_ORNAMENTS:
				//##装饰品有三个装备位
				//if( GetGoods(EC_ORNAMENTS_ONE) == NULL )
				//{
					//bResult = Add( EC_ORNAMENTS_ONE, pGoods, pVoid );
				//}
				//else if( GetGoods(EC_ORNAMENTS_TWO) == NULL )
				//{
					//bResult = Add( EC_ORNAMENTS_TWO, pGoods, pVoid );
				//}
				//break;

			case CGoodsBaseProperties::EP_HEADGEAR:
				bResult = Add( EC_BOOT, pGoods, pVoid );
				break;
			//case CGoodsBaseProperties::EP_TALISMAN:
				//bResult = Add( EC_BOOT, pGoods, pVoid );
				//break;
			case CGoodsBaseProperties::EP_FROCK:
				bResult = Add( EC_BOOT, pGoods, pVoid );
				break;
			//case CGoodsBaseProperties::EP_MEDAL:
				//bResult = Add( EC_BOOT, pGoods, pVoid );
				//break;
			case CGoodsBaseProperties::EP_WING:
				bResult = Add( EC_BOOT, pGoods, pVoid );
				break;
			//case CGoodsBaseProperties::EP_MANTEAU:
				//bResult = Add( EC_BOOT, pGoods, pVoid );
				//break;
			case CGoodsBaseProperties::EP_FAIRY:
				bResult = Add( EC_BOOT, pGoods, pVoid );
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
		if( it -> second == NULL )
		{
			continue;
		}
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectRemoved( this, it -> second, pVoid );
			}
		}
	}
	m_mEquipment.clear();
}

//##ModelId=41B3D0C9024D
VOID CEquipmentContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		CGoodsFactory::GarbageCollect( &(it -> second) );
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

//##ModelId=41B5456901AB
DWORD CEquipmentContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	/*
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			dwResult += it -> second -> GetWeight();
		}
	}*/
	return dwResult;
}

//##ModelId=41B6917E00AB
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
	return FALSE;
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
				m_vListeners[i] -> OnObjectRemoved( this, pResult, pVoid );
			}
		}
		m_mEquipment.erase( it );
	}

	return pResult;
}



//##ModelId=41BF9DAB0109
BOOL CEquipmentContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObject == NULL )
	{
		return FALSE;
	}

	//if( GetOwnerType() == TYPE_PLAYER )
	//{
	//	CPlayer* pOwner = GetGame() -> FindPlayer( GetOwnerID() );
	//	if( pOwner )
	//	{
	//		if( pOwner -> CanMountEquip(pObject) != CPlayer::EQUIPFIT )
	//		{
	//			return FALSE;
	//		}
	//	}
	//	else
	//	{
	//		//##暂时屏蔽，会导致反序列化时丢失数据。
	//		//return FALSE;
	//	}
	//}

	EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( dwPosition );
	map<EQUIPMENT_COLUMN,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		if( it -> second != NULL )
		{
			char str[256];
			sprintf(str, "此(名称:%s)装备装备在%d位置出错,此位置已有装备。",pObject->GetName(),dwPosition);
			PutStringToFile("debug-DB", str);
			return FALSE;
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
				/*
				EC_HEAD,				//	1：头盔
				EC_NECKLACE,			//	2：项链
				EC_WING,				//	3：翅膀
				EC_BODY,				//	4：盔甲
				EC_BACK,				//	5：腰带
				EC_GLOVE,			//6：手套	
				EC_BOOT,			//7：鞋子
				EC_HEADGEAR,			//	8：头饰
				EC_FROCK,			//	9：外套		
				EC_LRING,			//	10：左戒指
				EC_RRING,			//	11：右戒指		
				EC_MEDAL1,			//	12：勋章1
				EC_MEDAL2,			//	13	勋章2
				EC_MEDAL3,			//	14	勋章3
				EC_FAIRY,			//	15：小精灵
				EC_WEAPON,			//	16：武器
				EC_WEAPON2,			//	17：副手武器
				EC_CLOAK,			//17披风
				EC_HORSE,			//18坐骑
				*/
			case CGoodsBaseProperties::EP_HEAD:	//头盔
				if( ecColumn == CEquipmentContainer::EC_HEAD )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;		
			case CGoodsBaseProperties::EP_NECKLACE:	//项链
				if( ecColumn == CEquipmentContainer::EC_NECKLACE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WING:	//翅膀
				if( ecColumn == CEquipmentContainer::EC_WING )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BODY:	//盔甲
				if( ecColumn == CEquipmentContainer::EC_BODY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BACK:	//腰带
				if( ecColumn == CEquipmentContainer::EC_BACK )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GLOVE:	//手套
				if( ecColumn == CEquipmentContainer::EC_GLOVE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BOOT:	//鞋子
				if( ecColumn == CEquipmentContainer::EC_BOOT )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HEADGEAR:	//头饰
				if( ecColumn == CEquipmentContainer::EC_HEADGEAR )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;			
			case CGoodsBaseProperties::EP_FROCK:	//外套
				if( ecColumn == CEquipmentContainer::EC_FROCK )
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
			case CGoodsBaseProperties::EP_FAIRY:	//精灵
				if( ecColumn == CEquipmentContainer::EC_FAIRY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON:	//武器
				if( ecColumn == CEquipmentContainer::EC_WEAPON )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON2: //副手武器
				if( ecColumn == CEquipmentContainer::EC_WEAPON2 )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;					
			case CGoodsBaseProperties::EP_HORSE: //坐骑
				if(ecColumn==CEquipmentContainer::EC_HORSE)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_MEDAL1: //坐骑
				if(ecColumn==EC_MEDAL1 || ecColumn==EC_MEDAL2 || ecColumn==EC_MEDAL3)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
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
		else if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_PACK )
		{
			m_mEquipment[ecColumn]=pObject;
			bResult=TRUE;
		}

	}
	if( bResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
			}
		}
	}
	return bResult;
}

BOOL CEquipmentContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	if( pObject == NULL )
	{
		return FALSE;
	}

	CGoods* pGoods = GetGoods( dwPosition );
	if( pGoods )
	{
		char str[256];
		char szGuid[40];
		char szExGuid[40];
		
		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

		sprintf(str, "装备:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );
		
		PutStringToFile("debug-DB", str);
		return FALSE;
	}

	BOOL bResult = FALSE;
	EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( dwPosition );
	CGoodsBaseProperties* pProperties = 
		CGoodsFactory::QueryGoodsBaseProperties( pObject -> GetBasePropertiesIndex() );
	if( pProperties )
	{
		if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT )
		{
			switch( pProperties -> GetEquipPlace() )
			{
			case CGoodsBaseProperties::EP_HEAD:	//头盔
				if( ecColumn == CEquipmentContainer::EC_HEAD )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;		
			case CGoodsBaseProperties::EP_NECKLACE:	//项链
				if( ecColumn == CEquipmentContainer::EC_NECKLACE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WING:	//翅膀
				if( ecColumn == CEquipmentContainer::EC_WING )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BODY:	//盔甲
				if( ecColumn == CEquipmentContainer::EC_BODY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BACK:	//腰带
				if( ecColumn == CEquipmentContainer::EC_BACK )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GLOVE:	//手套
				if( ecColumn == CEquipmentContainer::EC_GLOVE )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_BOOT:	//鞋子
				if( ecColumn == CEquipmentContainer::EC_BOOT )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_HEADGEAR:	//头饰
				if( ecColumn == CEquipmentContainer::EC_HEADGEAR )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;			
			case CGoodsBaseProperties::EP_FROCK:	//外套
				if( ecColumn == CEquipmentContainer::EC_FROCK )
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
			case CGoodsBaseProperties::EP_FAIRY:	//精灵
				if( ecColumn == CEquipmentContainer::EC_FAIRY )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON:	//武器
				if( ecColumn == CEquipmentContainer::EC_WEAPON )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_WEAPON2: //副手武器
				if( ecColumn == CEquipmentContainer::EC_WEAPON2 )
				{
					m_mEquipment[ecColumn] = pObject;
					bResult = TRUE;
				}
				break;					
			case CGoodsBaseProperties::EP_HORSE: //坐骑
				if(ecColumn==CEquipmentContainer::EC_HORSE)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_MEDAL1: //坐骑
				if(ecColumn==CEquipmentContainer::EC_MEDAL1 || ecColumn==CEquipmentContainer::EC_MEDAL2 || ecColumn==CEquipmentContainer::EC_MEDAL3)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_CLOAK:	
				if(ecColumn==CEquipmentContainer::EC_CLOAK)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_DECORATION1:	
				if(ecColumn==CEquipmentContainer::EC_DECORATION1 || ecColumn==CEquipmentContainer::EC_DECORATION2)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_ADORN:
				if(ecColumn==CEquipmentContainer::EC_ADORN)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;
			case CGoodsBaseProperties::EP_GUARDIAN:
				if(ecColumn==CEquipmentContainer::EC_GUARDIAN)
				{
					m_mEquipment[ecColumn]=pObject;
					bResult=TRUE;
				}
				break;

			default:
				break;
			}
		}
		else if(pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_PACK)
		{
			m_mEquipment[ecColumn]=pObject;

			bResult=TRUE;
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

//##ModelId=41BFB91C01D4
BOOL CEquipmentContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFB92601A5
BOOL CEquipmentContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
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
				it -> second -> GetBasePropertiesIndex() ) )
			{
				++ dwAmount;
			}
		}
	}
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

BOOL CEquipmentContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	_AddToByteArray( pStream, GetGoodsAmount() );
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				_AddToByteArray( pStream, static_cast<DWORD>(it -> first) );
				it -> second -> Serialize( pStream, b );
			}
		}
	}

	return TRUE;
}

BOOL CEquipmentContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Clear();
	DWORD dwGoodsAmount = _GetDwordFromByteArray( pStream, lOffset );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( 
			_GetDwordFromByteArray(pStream, lOffset) );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(74);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,38 );
		if( pGoods )
		{
			Add( ecColumn, pGoods );
		}
	}

	return TRUE;
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
	Clear();
	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		EQUIPMENT_COLUMN ecColumn = static_cast<EQUIPMENT_COLUMN>( 
			setReadDB.GetDwordFromByteArray() );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(74);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,39 );
		if( pGoods )
		{
			Add( ecColumn, pGoods, NULL );
		}
	}

	return TRUE;
}

//##ModelId=41C0E6DB0119
CBaseObject* CEquipmentContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}
