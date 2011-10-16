///
/// @file CTradeSpot.h
/// @brief 贸易点描述及相关逻辑
///
#pragma once

#include "../../../../public/GameEvent.h"
#include "BusinessCommon.h"
#include "BusinessState.h"
#include <map>
#include <string>
#include <vector>

namespace Business
{
	class CBusinessGoods;

	class CTradeSpot : public BaseMemObj, public CGameEvent
	{
	public:
		/// [物编ID-BusinessGoods*]
		typedef std::map<DWORD, CBusinessGoods*> GoodsTemplateTable;
		/// 物编ID
		typedef std::list<DWORD> GoodsIndexList;
		/// 商品分类[类别-该类别列表]
		typedef std::map<int, GoodsIndexList> GoodsCategoryTable;
		/// 国家投资额列表[国家ID-投资额]
		typedef std::map<BYTE, DWORD> CountryInvestTable;
	public:
		CTradeSpot() 
		{
			memset( &m_Property, 0, sizeof( m_Property ) );
			m_lPriceTimer = 0;
			m_StateGroup.SetTradeSpot( this );
		}

		CTradeSpot( const tagTradeSpotProperty &p ) : m_Property( p )
		{
			m_lPriceTimer = 0;
			m_StateGroup.SetTradeSpot( this );
		}

		virtual ~CTradeSpot();

		///
		/// 释放商品模板
		///
		void ReleaseGoodsTable();

		///
		/// 开启价格波动定时器
		///
		bool StartPriceTimer();
		void StopPriceTimer();

		///
		/// Decode goods template list and the base trade spot property.
		///
		void DecodeFromDataBlock( DBReadSet &db );

		///
		/// 购买商品，获取商品列表
		///
		void GetGoodsList( CPlayer *pPlayer, DBWriteSet &db );

		///
		/// 出售商品，获取玩家背包商品价格
		///
		void GetGoodsSellPrice( CPlayer *pPlayer, DBWriteSet &db );

		///
		/// 获取物价指数查询界面数据
		///
		void GetGoodsChecking( CPlayer *pPlayer, DBWriteSet &db );

		///
		/// 根据物编中的ID查找商品
		///
		CBusinessGoods *GetGoods( int id );

		///
		/// 单个商品库存量的减少将同样减少同类及其他类商品的库存
		/// 
		/// @param dwAmount 库存变化量
		///
		void DecreaseGoodsVolume( CBusinessGoods *pGoods, DWORD dwAmount );
		void IncreaseGoodsVolume( CBusinessGoods *pGoods, DWORD dwAmount );

		///
		/// 价格波动定时器回调，更新该贸易点商品价格
		///
		void UpdateGoodsPrice();

		///
		/// 开启刷新限购额定时器，实现一段时间后刷新
		///
		/// 如果该玩家在此贸易点没有任何限购额记录，则不开启定时器
		///
		void StartRefreshBuyLimitTimer( const CGUID &player_id );

		///
		/// 更新对于某个商人而言的商品限购额
		///
		void RefreshGoodsBuyLimit( const CGUID &player_id );

		/// build the goods category by the goods list.
		void BuildGoodsCategory();

		///
		/// 增加某个国家在该贸易点的投资额
		///
		/// @param dwAdd 增加的投资额
		///
		void AddCountryInvest( BYTE country_id, DWORD dwAdd );

		///
		/// 获取某个国家的投资总额
		///
		DWORD GetCountryInvest( BYTE country_id ) const;
		
		///
		/// 编码数据库存档数据
		///
		void EncodeToSave( DBWriteSet &db );

		const GoodsTemplateTable &GetGoodsTemplateTable() const
		{
			return m_GoodsTable;
		}

		void SetProperty( const tagTradeSpotProperty &p )
		{
			m_Property = p;
		}

		const tagTradeSpotProperty &GetProperty() const 
		{
			return m_Property;
		}

		CStateGroup &QueryStateGroup()
		{
			return m_StateGroup;
		}

		///
		void OnMessage( CMessage* ) {}
		long OnTimeOut( DWORD timerid, DWORD curtime, const void *var );
		void OnTimerCancel( DWORD timerid, const void *var );
		long Proc( eEventType type, const tagProcVar *pVar ) { return 0; }

	private:
		///
		/// 判断指定商品是否开放给此玩家
		///
		bool IsGoodsVisible( CBusinessGoods *pGoods, CPlayer *pPlayer );

		///
		/// 解码数据库存档数据到商品
		///
		void DecodeGoodsDBProperty( DBReadSet &db );

		///
		/// 解码数据库存档数据到国家投资总额
		///
		void DecodeCountryInvestDBProperty( DBReadSet &db );

	private:
		/// 贸易点基本属性
		tagTradeSpotProperty m_Property;
		/// 商品模板列表
		GoodsTemplateTable m_GoodsTable;
		/// 商品分类
		GoodsCategoryTable m_GoodsCategory;
		/// 物价波动定时器ID
		long m_lPriceTimer;
		/// 国家投资额列表
		CountryInvestTable m_CountryInvest;
		/// 处于跑商界面操作的玩家状态
		CStateGroup m_StateGroup;
	};

}