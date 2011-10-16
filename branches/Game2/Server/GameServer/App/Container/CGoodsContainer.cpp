// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../Listener/CContainerListener.h"
#include "CGoodsContainer.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41BF91DC003E
CGoodsContainer::CGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_lOwnerType					= 0;
	m_OwnerGuid						= NULL_GUID;
	m_gcmCurrentGoodsContainerMode	= GCM_NORMAL;
}

//##ModelId=41BF91EE001F
//CGoodsContainer::CGoodsContainer(LONG lID) : CContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_lOwnerType	= 0;
//	m_OwnerGuid		= 0;
//}

//##ModelId=41BF91E001E4
CGoodsContainer::~CGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_lOwnerType					= 0;
	m_OwnerGuid						= NULL_GUID;
	m_gcmCurrentGoodsContainerMode	= GCM_NORMAL;
}

LONG CGoodsContainer::GetOwnerType()
{
	return m_lOwnerType;
}

const CGUID& CGoodsContainer::GetOwnerID()
{
	return m_OwnerGuid;
}

//##ModelId=41BF92970167
bool CGoodsContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CContainer::Add( pObject, pPreviousContainer, pVoid );
}

//##ModelId=41BF96FB01C5
bool CGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//##因为同位置相关,所以这个Add函数只实现有物品处于该位置的时候，且2个
	//##物品类型相同的状况下，添加物品的问题，派生类的函数必须自行判断是否
	//##需要重叠。然后调用该函数。
	bool bResult = false;
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
				long nRemainedSlot = pGoods -> GetMaxStackNumber() -
					pGoods -> GetAmount();
				long lAddedAmount = pObject -> GetAmount();
				//##如果加入的数量少于等于这个，则直接加入。
				if(lAddedAmount>=0 && lAddedAmount <= nRemainedSlot )
				{
					//##如果是正常模式，就删除该物品。如果是测试模式，就不删除源物品
					if( GetContainerMode() == GCM_NORMAL )
					{						
						pGoods -> SetAmount( pGoods -> GetAmount() + lAddedAmount );
						//CGoodsFactory::AddGoodsToGCSet(pObject);
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pObject->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:物品叠加][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pObject->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pObject->GetBasePropertiesIndex()),srcGoodsGUID,pObject->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif	
						CGoodsFactory::GarbageCollect( &pObject,1 );
					}

					for( size_t i = 0; i < m_vListeners.size(); i ++ )
					{
						if( m_vListeners[i] )
						{
							//##该合并消息必须设置目标物品
							m_vListeners[i] -> OnObjectAdded( this, pGoods, lAddedAmount, pVoid );
						}
					}

					bResult = true;
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

//##ModelId=41BF92C1002E
void CGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CContainer::Clear( pVoid );
}

//##ModelId=41BF92C700EA
void CGoodsContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lOwnerType					= 0;
	m_OwnerGuid						= NULL_GUID;
	m_gcmCurrentGoodsContainerMode	= GCM_NORMAL;
	CContainer::Release();
}

//##ModelId=41BF956500FA
void CGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//return CContainer::AI();
}

//##ModelId=41BF992A001F
bool CGoodsContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return true;
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
				//##通知监听器
				for( size_t i = 0; i < m_vListeners.size(); i ++ )
				{
					if( m_vListeners[i] )
					{
						//##该消息必须设置目标物品，在移出之前通知监听器，监听器应该先检验这个物品是否存在。
						m_vListeners[i] -> OnObjectRemoved( this, pGoods, dwAmount, pVoid );
					}
				}
				pResult = CGoodsFactory::CreateGoods( pGoods -> GetBasePropertiesIndex() ,14);
				pResult -> SetAmount( dwAmount );
				pGoods  -> SetAmount( pGoods -> GetAmount() - dwAmount );
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
void CGoodsContainer::SetOwner(LONG lType, const CGUID& OwnerGuid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_lOwnerType	= lType;
	m_OwnerGuid		= OwnerGuid;
}


//##ModelId=41FEF83B00B0
void CGoodsContainer::SetContainerMode(GOODS_CONTAINER_MODE gcm)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_gcmCurrentGoodsContainerMode = gcm;
}

//##ModelId=41FEF8A30033
CGoodsContainer::GOODS_CONTAINER_MODE CGoodsContainer::GetContainerMode()
{
	return m_gcmCurrentGoodsContainerMode;
}

bool CGoodsContainer::CanPutIn(CGoods* pGoods,CPlayer* pPlayer)
{
	if(pGoods)
		return true;
	return false;
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
