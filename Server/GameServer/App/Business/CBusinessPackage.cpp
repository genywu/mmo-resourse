///
/// @file CBusinessPackage.cpp
/// @brief 跑商背包描述
///
#include "StdAfx.h"
#include "CBusinessPackage.h"
#include "BusinessCommon.h"
#include "CBusinessManager.h"

#include "../Container/CVolumeLimitGoodsContainer.h"
#include "../Goods/CGoodsFactory.h"
#include "../Goods/CGoods.h"
#include "../Player.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Business
{
	CPackage::CPackage()
	{
		m_pContainer = OBJ_CREATE(CVolumeLimitGoodsContainer);
		m_pContainer->SetContainerVolume( 0 );
		m_pGoods = NULL;
		m_dwGoodsCount = m_dwMaxGoodsCount = 0;
	}

	CPackage::~CPackage()
	{
		if( m_pGoods != NULL )
		{
			CGoodsFactory::GarbageCollect( &m_pGoods, 48 );
		}
		OBJ_RELEASE(CVolumeLimitGoodsContainer, m_pContainer);
	}

	bool CPackage::IsValid() const
	{
		// 商业背包有效：背包存在（容器容积大于0且对应的背包物品存在）且背包当前持久大于0
		return m_pContainer->GetVolume() > 0 && m_pGoods != NULL &&
			m_pGoods->GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 ) > 0;
	}

	LONG CPackage::GetCurDurability()
	{
		if( m_pGoods == NULL )
		{
			return 0;
		}
		return m_pGoods->GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
	}

	DWORD CPackage::UpdateEndure( DWORD dwProfit )
	{
		if( m_pGoods == NULL )
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Busiess package GOODS does not exist!" );
			return 0;
		}

		DWORD dwSubEndure = (DWORD)(dwProfit * GetInst( CBusinessManager ).GetSetup().fEndureQuo);
		DWORD dwCurEndure = GetCurDurability();
		DWORD dwRet = dwCurEndure >= dwSubEndure ? dwCurEndure - dwSubEndure : 1;
		m_pGoods->ChangeAttribute( GAP_GOODS_MAX_DURABILITY, dwRet, 2, true );
		
		return dwRet;
	}

	void CPackage::UpdateEndureToClient( const CGUID &player_id )
	{
		if( m_pGoods == NULL )
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Busiess package GOODS does not exist!" );
			return ;
		}
		// TODO:客户端检查耐久为0让背包消失
		m_pGoods->UpdateAttribute( player_id );
	}

	// 获得当前商业背包内物品总数
	DWORD CPackage::GetCurGoodsAmount()
	{
		if(m_pGoods && m_pContainer)
		{
			DWORD dwNum=0;
			CAmountLimitGoodsContainer::goodsIt it=m_pContainer->GetGoodsMap()->begin();
			for(;it!=m_pContainer->GetGoodsMap()->end();it++)
			{
				if(it->second)
				{
					dwNum+=it->second->GetAmount();
				}
			}
			return dwNum;
		}
		return 0;
	}

	DWORD CPackage::UpdateGoodsPrice( CGoods *pGoods )
	{
		if( pGoods != NULL )
		{
			DWORD ret = UpdateGoodsPrice( pGoods->GetBasePropertiesIndex(), 
				pGoods->GetAmount(), pGoods->GetBuyPrice() );
			pGoods->SetBuyPrice( ret );
			return ret;
		}

		return 0;
	}

	DWORD CPackage::UpdateGoodsPrice( DWORD index, DWORD amount, DWORD price )
	{
		vector<CGoods*> goods_list;
		DWORD amount_sum = 0;
		DWORD base_price = 0;

		CAmountLimitGoodsContainer::goodsIt it = m_pContainer->GetGoodsMap()->begin();
		for( ; it != m_pContainer->GetGoodsMap()->end(); ++it )
		{
			CGoods *pGoods = it->second;
			if( pGoods != NULL && pGoods->GetBasePropertiesIndex() == index )
			{
				amount_sum += pGoods->GetAmount();
				base_price = pGoods->GetBuyPrice();
				goods_list.push_back( pGoods );
			}
		}

		DWORD price_ret = ( amount * price + amount_sum * base_price ) / ( amount + amount_sum );

		for( vector<CGoods*>::iterator it = goods_list.begin(); it != goods_list.end(); ++ it )
		{
			(*it)->SetBuyPrice( price_ret );
		}

		return price_ret;
	}

	// 获得当前商业背包使用了多少格子
	DWORD CPackage::GetUsedVolume()
	{
		if(m_pGoods && m_pContainer)		
			return (DWORD)m_pContainer->GetGoodsMap()->size();
		
		return 0;
	}

	BOOL CPackage::MountBusinessPackGoods(CGoods* pGoods)
	{
		if( pGoods == NULL )
		{
			return FALSE;
		}

		if( !CanSwap( pGoods ) )
		{
			return FALSE;
		}

		LONG lVolumeLimit = pGoods->GetAddonPropertyValues( GAP_PACK_CAPACITY, 1 );
		LONG lAmountLimit = pGoods->GetAddonPropertyValues( GAP_PACK_CAPACITY, 2 );

		// 安装背包物品
		m_pGoods = pGoods;
		m_pContainer->ChangeContainerVolume( lVolumeLimit );
		m_dwMaxGoodsCount = lAmountLimit;
		return TRUE;
	}

	BOOL CPackage::CanSwap( CGoods *pPackGoods )
	{
		DWORD dwPackType = pPackGoods->GetAddonPropertyValues( GAP_PACK_TYPE, 1 );
		if( dwPackType != SPT_BUSINESS )
		{
			return FALSE;
		}

		if( m_pGoods == NULL )
		{
			return TRUE;
		}

		// 新的跑商背包必须能满足容积和物品数量限制	
		LONG lVolumeLimit = pPackGoods->GetAddonPropertyValues( GAP_PACK_CAPACITY, 1 );
		LONG lAmountLimit = pPackGoods->GetAddonPropertyValues( GAP_PACK_CAPACITY, 2 );

		// 如果已经有装备的跑商背包，则不能是同一物品
		if( m_pGoods != NULL )
		{
			if( ( m_pGoods == pPackGoods ) || ( m_pGoods->GetExID() == pPackGoods->GetExID() ) )
			{
				return FALSE;
			}
			
			// 新背包体积不够
			if( lVolumeLimit < (long)GetUsedVolume() )
			{
				return FALSE;
			}

			// 物品容积容纳不下当前物品数
			DWORD dwCurGoodsAmount = GetCurGoodsAmount();
			if( lAmountLimit < (long)GetCurGoodsAmount() )
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	int CPackage::Swap( CGoods *pPackGoods )
	{
		MountBusinessPackGoods( pPackGoods );
		DWORD dwGoodsAmount = m_pContainer->GetGoodsAmount();
		int ret = 0;
		if( dwGoodsAmount > 0 )
		{
			ret = 2;
			m_pContainer->ReFillCell();
		}
		else
		{
			ret = 1;
		}

		return ret;
	}

	bool CPackage::CheckValid()
	{
		if( m_pGoods == NULL )
		{
			return false;
		}

		if( GetCurDurability() <= 1 )
		{
			// TODO:删除数据库中的背包
			m_pContainer->Release();
			CGoodsFactory::GarbageCollect( &m_pGoods, 48 );
			m_pGoods = NULL;
			return false;
		}

		return true;
	}

	void CPackage::EncodeToSave( DBWriteSet &db )
	{
		if( m_pGoods == NULL )
		{
			// invalid
			db.AddToByteArray( (BYTE) 0 );
		}
		else
		{
			db.AddToByteArray( (BYTE) 1 );
			m_pGoods->Serialize( db );
			m_pContainer->Serialize( db );
		}
	}

    void CPackage::CodeForClient(DBWriteSet& db)
    {
        if( IsValid() )
        {
            // 背包本身
            db.AddToByteArray( (BYTE) 1 );
            m_pGoods->SerializeForOldClient( db );

            // 背包内物品
            db.AddToByteArray( (DWORD) m_pContainer->GetGoodsAmount() );
            CPacketListener cPacketListener;
            cPacketListener.m_pDBWriteData = &db;
            m_pContainer->TraversingContainer( &cPacketListener );
        }
        else
        {
            db.AddToByteArray( (BYTE) 0 );
        }
    }

	void CPackage::DecodeFromDB( DBReadSet &db )
	{
		BYTE valid = db.GetByteFromByteArray();
		if( valid == 1 )
		{
			// valid
			CGoods *pGoods = CGoodsFactory::UnserializeGoods( db ,54);
			if( pGoods == NULL )
			{
				Log4c::Warn(ROOT_MODULE, "Decode pakcage goods failed." );
			}
			MountBusinessPackGoods( pGoods );
			m_pContainer->Unserialize( db );
		}
	}
}

