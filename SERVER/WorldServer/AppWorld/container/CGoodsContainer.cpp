// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Listener\CContainerListener.h"
#include "CGoodsContainer.h"
//#include "..\Message Packaging\CS2CContainerObjectMove.h"




//##ModelId=41BF91DC003E
CGoodsContainer::CGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_lOwnerType	= 0;
	m_lOwnerID		= 0;
}

//##ModelId=41BF91EE001F
//CGoodsContainer::CGoodsContainer(LONG lID) : CContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_lOwnerType	= 0;
//	m_lOwnerID		= 0;
//}

//##ModelId=41BF91E001E4
CGoodsContainer::~CGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_lOwnerType	= 0;
	m_lOwnerID		= 0;
}

LONG CGoodsContainer::GetOwnerType()
{
	return m_lOwnerType;
}

LONG CGoodsContainer::GetOwnerID()
{
	return m_lOwnerID;
}

//##ModelId=41BF92970167
BOOL CGoodsContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CContainer::Add( pObject, pVoid );
}

//##ModelId=41BF96FB01C5
BOOL CGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##因为同位置相关,所以这个Add函数只实现有物品处于该位置的时候，且2个
	//##物品类型相同的状况下，添加物品的问题，派生类的函数必须自行判断是否
	//##需要重叠。然后调用该函数。
	BOOL bResult = FALSE;
	if( pObject )
	{
		CGoods* pGoods = GetGoods( dwPosition );
		if( pGoods )
		{
			//##如果要加入的物品，类型相同且物品是可堆叠的。
			if( pGoods -> GetBasePropertiesIndex() ==
				pObject -> GetBasePropertiesIndex() &&
				pGoods -> GetMaxStackNumber() > 1 )
			{
				DWORD dwRemainedSlot = pGoods -> GetMaxStackNumber() -
					pGoods -> GetAmount();
				//##如果加入的数量少于等于这个，则直接加入。
				if( pObject -> GetAmount() <= dwRemainedSlot )
				{
					pGoods -> SetAmount( pGoods -> GetAmount() + pObject -> GetAmount() );
					CGoodsFactory::GarbageCollect( &pObject );

					//##重置目标物，避免错误。
					//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
					//if( pMsg )
					//{
					//	pMsg -> SetObject( 0, 0 );
					//	pMsg -> SetObjectAmount( 0 );
					//	pMsg -> GetObjectStream().clear();
					//}

					for( size_t i = 0; i < m_vListeners.size(); i ++ )
					{
						if( m_vListeners[i] )
						{
							//##该合并消息必须设置目标物品
							m_vListeners[i] -> OnObjectAdded( this, pGoods, pVoid );
						}
					}

					bResult = TRUE;
				}
				//##否则什么都不作
				else
				{
				}
			}
		}
	}
	return bResult;
}
BOOL CGoodsContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	CGoods* pGoods = GetGoods( dwPosition );

	if( pGoods ) //m_pGoldCoins存在了
	{
		char str[256];
		char szGuid[40];
		char szExGuid[40];
		
		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

		sprintf(str, "基本容器:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );

		PutStringToFile("debug-DB", str);
		return FALSE;
	}

	BOOL bResult = FALSE;
	if( pObject )
	{
		bResult = TRUE;
	}
	return bResult;
}
//##ModelId=41BF92C1002E
VOID CGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CContainer::Clear( pVoid );
}

//##ModelId=41BF92C700EA
VOID CGoodsContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lOwnerType	= 0;
	m_lOwnerID		= 0;
	CContainer::Release();
}

//##ModelId=41BF956500FA
VOID CGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//return CContainer::AI();
}

//##ModelId=41BF992A001F
BOOL CGoodsContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return TRUE;
}

//##ModelId=41BF928C00FA
CBaseObject* CGoodsContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	return NULL;
}

//##ModelId=41BF92B5030D
CBaseObject* CGoodsContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return NULL;
}

//##ModelId=41C0E6B9032C
CBaseObject* CGoodsContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoods* pResult	= NULL;
	CGoods* pGoods	= GetGoods( dwPosition );
	if( pGoods && dwAmount > 0 )
	{
		//##如果没有全部拿出去，则直接减少原有的计数,然后新生成一个物品.
		//##但是条件必须是消耗性物品.
		if( pGoods -> GetAmount() > dwAmount )
		{
			if( pGoods -> GetMaxStackNumber() >  1 )
			{
#ifdef __MEMORY_LEAK_CHECK__
				CBaseObject::SetConstructFlag(52);
#endif
				pResult = CGoodsFactory::CreateGoods( pGoods -> GetBasePropertiesIndex(),21 );
				pResult -> SetAmount( dwAmount );

				//##通知监听器
				for( size_t i = 0; i < m_vListeners.size(); i ++ )
				{
					if( m_vListeners[i] )
					{
						//##该消息必须设置目标物品，在移出之前通知监听器，监听器应该先检验这个物品是否存在。
						m_vListeners[i] -> OnObjectRemoved( this , pResult, pVoid );
					}
				}

				pGoods -> SetAmount( pGoods -> GetAmount() - dwAmount );
			}
		}
		else if( pGoods -> GetAmount() == dwAmount )
		{
			pResult = dynamic_cast<CGoods*>( Remove(pGoods, pVoid) );
		}
	}
	return pResult;
}

//##ModelId=41BF9944009C
VOID CGoodsContainer::SetOwner(LONG lType, LONG lID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lOwnerType	= lType;
	m_lOwnerID		= lID;
}

CBaseObject* CGoodsContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CContainer::Remove( pObject, pVoid );
}

CBaseObject* CGoodsContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CGoodsContainer::Find( CBaseObject* pObj )
{
	return CContainer::Find( pObj );
}

CBaseObject* CGoodsContainer::Find( LONG oType,const CGUID& guid )
{
	return CContainer::Find( oType, guid );
}

