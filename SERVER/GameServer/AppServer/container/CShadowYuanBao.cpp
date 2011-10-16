// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Player.h"
#include "..\BaseObject.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"
#include "CShadowYuanBao.h"
#include "CContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41F0713100D0
CShadowYuanBao::CShadowYuanBao()
{
	// TODO: Add your specialized code here.
	SetGoodsAmountLimit( 1 );
}

//##ModelId=41F0713A0293
//CShadowYuanBao::CShadowYuanBao(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41F0713F035D
CShadowYuanBao::~CShadowYuanBao()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41F0714A0195
BOOL CShadowYuanBao::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CShadowYuanBao::Add( 0, dynamic_cast<CGoods*>(pObject), pPreviousContainer, pVoid );
}

//##ModelId=41F0715201E2
BOOL CShadowYuanBao::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	//##首先清除自己的影子。
	goodsshadowIt it = m_mGoodsShadow.begin();
	if( it != m_mGoodsShadow.end() )
	{
		RemoveShadow( it -> first );
	}
	Clear();

	//##然后添加新的物品
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pPreviousContainer && pGoods )
	{
		if( pGoods -> GetBasePropertiesIndex() == CGoodsFactory::GetYuanBaoIndex() )
		{
			//##首先,添加到原容器内.
			CGoodsContainer* pContainer = NULL;
			if( pPreviousContainer -> lContainerType == TYPE_PLAYER )
			{
				CPlayer* pPlayer = GetGame() -> FindPlayer( pPreviousContainer -> ContainerID );
				if( pPlayer )
				{
					if( pPreviousContainer -> lContainerExtendID == CS2CContainerObjectMove::PEI_YUANBAO )
					{
						pContainer = &( pPlayer -> m_cYuanBao );
					}
				}
			}

			if( pContainer )
			{
				BOOL	bAdded		= FALSE;
				//LONG	lGoodsID	= pGoods -> GetID();
				CGUID	guGoodsID	= pGoods ->GetExID();
				DWORD	dwAmount	= pGoods -> GetAmount();
				DWORD	dwBaseIndex	= pGoods -> GetBasePropertiesIndex();
				if( pPreviousContainer -> dwGoodsPosition != 0xFFFFFFFF )
				{
					bAdded = pContainer -> Add( pPreviousContainer -> dwGoodsPosition, pGoods, NULL, pVoid );
					if( bAdded )
					{
						CGoods* pOriginalGoods = pContainer -> GetGoods( pPreviousContainer -> dwGoodsPosition );
						if( pOriginalGoods )
						{
							//lGoodsID	= pOriginalGoods -> GetID();
							guGoodsID	= pOriginalGoods ->GetExID();
						}
					}
				}
				else 
				{
					if( bAdded = pContainer -> Add(pGoods, NULL, pVoid) )
					{
						pContainer -> QueryGoodsPosition( pGoods, pPreviousContainer -> dwGoodsPosition );
					}
				}

				if( bAdded )
				{
					//##然后再自身容器内添加影子.
					tagGoodsShadow tShadow;
					tShadow.lOriginalContainerType		= pPreviousContainer -> lContainerType;
					tShadow.OriginalContainerID			= pPreviousContainer -> ContainerID;
					tShadow.lOriginalContainerExtendID	= pPreviousContainer -> lContainerExtendID;
					tShadow.dwOriginalGoodsPosition		= pPreviousContainer -> dwGoodsPosition;
					tShadow.dwGoodsAmount				= dwAmount;
					//tShadow.lGoodsID					= lGoodsID;
					tShadow.guGoodsID					= guGoodsID;
					tShadow.dwGoodsBasePropertiesIndex	= dwBaseIndex;

					//DWORD dwHashValue					= CContainer::CalculateHashValue( TYPE_GOODS, lGoodsID );
					m_mGoodsShadow[guGoodsID]			= tShadow;

					CGoodsShadowContainer::AddShadow( guGoodsID );

					bResult								= TRUE;
				}
			}
		}
	}
	return bResult;
}



//##ModelId=41F071F30047
BOOL CShadowYuanBao::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return FALSE;
}

//##ModelId=41F071FB01EC
BOOL CShadowYuanBao::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObj )
	{
		RemoveShadow( pObj->GetExID() );
	}
	return TRUE;
}


//##ModelId=41F5EBAD0188
DWORD CShadowYuanBao::GetGoldCoinsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( m_mGoodsShadow.size() > 0 )
	{
		goodsshadowIt it = m_mGoodsShadow.begin();
		if( it != m_mGoodsShadow.end() )
		{
			dwResult = it -> second.dwGoodsAmount;
		}
	}
	return dwResult;
}
