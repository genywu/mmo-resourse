// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CEquipmentUpgrade.h"
#include "..\Player.h"
#include "..\ServerRegion.h"
#include "..\Listener\CUpgradePriceListener.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\..\..\setup\globesetup.h"
#include "../../../setup/LogSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEquipmentUpgrade::CEquipmentUpgrade()
{	
}

CEquipmentUpgrade::~CEquipmentUpgrade()
{
	// TODO: Add your specialized code here.
	Release();
}

BOOL CEquipmentUpgrade::IsPlugAvailable()
{	
	BOOL bResult = FALSE;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pPlayer -> GetFather() );
		if( pRegion )
		{
			if( pRegion -> GetSecurity( pPlayer -> GetTileX(), pPlayer -> GetTileY() ) == CRegion::SECURTIY_SAFE )
			{
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

BOOL CEquipmentUpgrade::OnSessionEnded()
{
	return OnPlugEnded();
}

BOOL CEquipmentUpgrade::OnSessionAborted()
{
	return OnPlugEnded();
}

BOOL CEquipmentUpgrade::OnPlugInserted()
{
	m_cUpgradeContainer.SetOwner( TYPE_SESSION, m_SessionID );
	m_cUpgradeContainer.SetContainerExtendID( Get32ID() << 8 );

	//##把监听器加入到玩家的容器内。
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer -> getOriginPack()->AddListener( &m_cUpgradeContainer );
		pPlayer -> m_cEquipment.AddListener( &m_cUpgradeContainer );
	}

	return TRUE;
}

BOOL CEquipmentUpgrade::OnPlugAborted()
{	
	return OnPlugEnded();
}

BOOL CEquipmentUpgrade::OnPlugEnded()
{
	//##直接发送关闭界面的消息给客户端。
	//##把监听器加入到玩家的容器内。
	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer ->getOriginPack()->RemoveListener( &m_cUpgradeContainer );
		pPlayer -> m_cEquipment.RemoveListener( &m_cUpgradeContainer );
	}

	CMessage msg( MSG_S2C_CLOSEGOODSUPGRADE );
	msg.SendToPlayer( GetOwnerID() );

	return TRUE;
}

BOOL CEquipmentUpgrade::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return TRUE;
}

VOID CEquipmentUpgrade::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_cUpgradeContainer.Clear();
}

CContainer* CEquipmentUpgrade::GetContainer(LONG lID)
{
	// TODO: Add your specialized code here.
	CContainer* pResult = NULL;
	if( lID == 0 )
	{
		pResult = &m_cUpgradeContainer;
	}
	return pResult;
}


DWORD CEquipmentUpgrade::GetSucceedResult()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 成功后的结果：１表示升１级，２表示升２级。至少是升１级。
	DWORD dwResult = 0;

	//##首先获取基本宝石的信息.基本宝石不会包含升2级的概率问题
	CGoods* pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 1 );
		if( dwOperation > dwResult )
		{
			dwResult = dwOperation;
		}
	}

	//##然后获取宝石1,2,3的信息
	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_ONE );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 1 );
		if( dwOperation > dwResult )
		{
			//##获取升多级概率
			LONG lProbability = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 2 );
			if( random(100) <= lProbability )
			{
				dwResult = dwOperation;
			}
		}
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_TWO );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 1 );
		if( dwOperation > dwResult )
		{
			//##获取升多级概率
			LONG lProbability = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 2 );
			if( random(100) <= lProbability )
			{
				dwResult = dwOperation;
			}
		}
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_THREE );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 1 );
		if( dwOperation > dwResult )
		{
			//##获取升多级概率
			LONG lProbability = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_SUCCEED_RESULT, 2 );
			if( random(100) <= lProbability )
			{
				dwResult = dwOperation;
			}
		}
	}

	return dwResult;
}

DWORD CEquipmentUpgrade::GetFailedResult()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 失败后的结果：1.什么都不作，2.降1级，3.降到0级，4.销毁
	DWORD dwResult = 4;

	//##首先获取基本宝石的信息.
	CGoods* pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_FAILED_RESULT, 1 );
		if( dwOperation > 0 && dwOperation < dwResult )
		{
			dwResult = dwOperation;
		}
	}

	//##然后获取宝石1,2,3的信息
	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_ONE );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_FAILED_RESULT, 1 );
		if( dwOperation > 0 && dwOperation < dwResult )
		{
			dwResult = dwOperation;
		}
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_TWO );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_FAILED_RESULT, 1 );
		if( dwOperation > 0 && dwOperation < dwResult )
		{
			dwResult = dwOperation;
		}
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_THREE );
	if( pGoods )
	{
		DWORD dwOperation = pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_UPGRADE_FAILED_RESULT, 1 );
		if( dwOperation > 0 && dwOperation < dwResult )
		{
			dwResult = dwOperation;
		}
	}

	return dwResult;
}

//##ModelId=423FBACD035B
DWORD CEquipmentUpgrade::GetProbability()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	LONG lProbability = 0;

	//##首先获取基本宝石的信息.
	CGoods* pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM );
	if( pGoods )
	{
		lProbability += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_PROBABILITY, 1 );
	}

	//##然后获取宝石1,2,3的信息
	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_ONE );
	if( pGoods )
	{
		lProbability += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_PROBABILITY, 1 );
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_TWO );
	if( pGoods )
	{
		lProbability += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_PROBABILITY, 1 );
	}

	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_THREE );
	if( pGoods )
	{
		lProbability += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_PROBABILITY, 1 );
	}

	//##再获取物品本身的附加值
	pGoods = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_EQUIPMENT );
	if( pGoods )
	{
		lProbability += pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_PROBABILITY, 1 );
	}

	//##裁减边界
	if( lProbability < 0   ) lProbability = 0;
	if( lProbability > 100 ) lProbability = 100;

	return static_cast<DWORD>( lProbability );
}

//##ModelId=4240C60C00BB
DWORD CEquipmentUpgrade::GetUpgradePrice()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CUpgradePriceListener lListener;
	m_cUpgradeContainer.TraversingContainer( &lListener );
	return lListener.m_dwPrice;
}

//##ModelId=423FBB240000
BOOL CEquipmentUpgrade::DoesUpgradeValid()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 升级有效的条件：1.装备能升级，2.有基本宝石，且对应武器当前等级。3.有足够的钱。
	BOOL bResult = FALSE;
	CPlayer* pOwner = dynamic_cast<CPlayer*>( GetOwner() );
	if( pOwner )
	{
		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pOwner -> GetFather() );
		if( pRegion )
		{
			DWORD dwUpgradePrice = GetUpgradePrice();
			if( pOwner -> GetMoney() >= dwUpgradePrice )
			{
				CGoods* pEquipment = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_EQUIPMENT );
				if( pEquipment && pEquipment -> CanUpgraded() )
				{
					//##获取装备等级
					LONG lEquipmentLevel =1;// pEquipment -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );

					//##获取基本宝石
					CGoods* pBaseGem = m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM );
					if( pBaseGem )
					{
						//##获取基本宝石的级别信息.
						LONG lMinLevel = pBaseGem -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_LEVEL, 1 );
						LONG lMaxLevel = pBaseGem -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GEM_LEVEL, 2 );
						if( lMaxLevel < lMinLevel )
						{
							lMaxLevel = lMinLevel;
						}
						//##判定级别
						if( lMinLevel <= lEquipmentLevel &&
							lMaxLevel >= lEquipmentLevel )
						{
							bResult = TRUE;
						}
						else
						{
							pOwner -> SendNotifyMessage( "基本宝石的级别不符合要求。" );
						}
					}
					else
					{
						pOwner -> SendNotifyMessage( "升级必须有相应的基本宝石。" );
					}
				}
				else 
				{
					pOwner -> SendNotifyMessage( "升级必须有相应的装备。" );
				}
			}
			else
			{
				CHAR szMsg[256];
				sprintf( szMsg, "升级需要%d枚金币，你的金币不够。", dwUpgradePrice  );
				pOwner -> SendNotifyMessage( szMsg );
			}
		}
	}
	return bResult;
}

//##ModelId=423FBC9A01B5
BOOL CEquipmentUpgrade::Upgrade()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( DoesUpgradeValid() )
	{
		CPlayer* pPlayer	= dynamic_cast<CPlayer*>( GetOwner() );
		CGoods* pEquipment	= m_cUpgradeContainer.GetGoods( CEquipmentUpgradeShadowContainer::UEPC_EQUIPMENT );
		if( pPlayer && pEquipment )
		{
			CServerRegion* pRegion = dynamic_cast<CServerRegion*>( pPlayer -> GetFather() );
			if( pRegion )
			{
				DWORD dwUpgradePrice	= GetUpgradePrice();
				DWORD dwCurrentLevel	=1;// pEquipment -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
				if( dwCurrentLevel < 99 )
				{
					DWORD dwProbability	= GetProbability();
					BOOL bGoodsUpdated	= TRUE;
					BOOL bGoodsRemoved	= FALSE;
					// 检查手续费是否足够？
					if(pPlayer -> GetMoney() >= dwUpgradePrice)
					{
						//##更改金钱
						pPlayer -> SetMoney( pPlayer -> GetMoney() - dwUpgradePrice  );
						if( static_cast<DWORD>( random(100) )+1 <= dwProbability )
							//##升级成功
						{
							DWORD dwSucceedResult = GetSucceedResult();

							//##裁减边界
							dwCurrentLevel += dwSucceedResult;
							if( dwCurrentLevel > 99 ) dwCurrentLevel = 99;

							//##升级成功
							CGoodsFactory::UpgradeEquipment( pEquipment, dwCurrentLevel );
							pPlayer -> SendNotifyMessage( "该物品升级成功。" );
						}
						else
							//##升级失败
						{

							DWORD dwFailedResult = GetFailedResult();
							switch( dwFailedResult )
							{
							case 1:
								//##什么都不作
								bGoodsUpdated = FALSE;
								pPlayer -> SendNotifyMessage( "该物品升级失败。" );
								break;
							case 2:
								//##降1级
								pPlayer -> SendNotifyMessage( "该物品升级失败并降低1等级。" );
								if( dwCurrentLevel > 0 )
								{
									CGoodsFactory::UpgradeEquipment( pEquipment, dwCurrentLevel - 1 );
								}
								break;
							case 3:
								//##直接降到0级
								pPlayer -> SendNotifyMessage( "该物品升级失败，等级变为0级。" );
								CGoodsFactory::UpgradeEquipment( pEquipment, 0 );
								break;
							case 4:
								//##删除物品
								pPlayer -> SendNotifyMessage( "该物品升级失败，损耗过渡，彻底报废。" );
								bGoodsUpdated = FALSE;
								bGoodsRemoved = TRUE;
								break;
							}
						}
						//##更新客户端物品属性
						if( bGoodsUpdated )
						{
							vector<BYTE> vGoods;
							pEquipment -> SerializeForOldClient( &vGoods );
							CMessage Msg( MSG_S2C_GOODS_UPDATEPROPERTY );
							Msg.Add( pPlayer->GetExID() );
							//Msg.Add( pEquipment -> GetID() );
							Msg.Add( pEquipment -> GetExID() );
							Msg.Add( static_cast<LONG>( vGoods.size() ) );
							Msg.AddEx( &vGoods[0], static_cast<LONG>( vGoods.size() ) );
							Msg.SendToPlayer( pPlayer->GetExID() );
						}

						if( bGoodsRemoved )
						{
							DeleteGoods( CEquipmentUpgradeShadowContainer::UEPC_EQUIPMENT );
						}

						//##删除除装备外的所有物品
						DeleteGoods( CEquipmentUpgradeShadowContainer::UEPC_BASE_GEM );
						DeleteGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_ONE );
						DeleteGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_TWO );
						DeleteGoods( CEquipmentUpgradeShadowContainer::UEPC_GEM_THREE );
					}
					else
					{
						pPlayer -> SendNotifyMessage( "升级费用不足，无法再升级。" );
					}
				}
				else 
				{
					pPlayer -> SendNotifyMessage( "该武器已经升级到最高等级，无法再升级。" );
				}
			}
		}
	}
	return bResult;
}

//##ModelId=424104490167
BOOL CEquipmentUpgrade::DeleteGoods(CEquipmentUpgradeShadowContainer::UPGRADING_EQUIPMENT_PLACE_CELL uepc)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods	 = m_cUpgradeContainer.GetGoods( uepc );
	if( pGoods )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
		if( pPlayer )
		{
			//##获取装备原始位置属性。
			CGoodsContainer::tagPreviousContainer tPreviousContainer;
			m_cUpgradeContainer.GetGoodsOriginalContainerInformation( pGoods -> GetExID(), tPreviousContainer );

			//##删除该物品
			CS2CContainerObjectMove comRemoveMsg;
			comRemoveMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
			comRemoveMsg.SetSourceContainerExtendID( tPreviousContainer.lContainerExtendID );
			CGoods* pRemovedGoods = dynamic_cast<CGoods*>( m_cUpgradeContainer.Remove(pGoods, &comRemoveMsg) );
			if( pRemovedGoods )
			{
				comRemoveMsg.Send( pPlayer->GetExID() );
				CGoodsFactory::GarbageCollect( &pRemovedGoods ,52);
				bResult = TRUE;
			}
		}
	}
	return bResult;
}
