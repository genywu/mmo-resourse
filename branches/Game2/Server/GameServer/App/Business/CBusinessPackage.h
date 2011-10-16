///
/// @file CBusinessPackage.h
/// @brief 跑商背包描述
///
#pragma once

class CVolumeLimitGoodsContainer;
class CGoods;

namespace Business
{
	class CPackage
	{
	public:
		CPackage();

		~CPackage();

		///
		/// 更新持久
		/// @param dwProfit 某笔交易获得的利润
		/// @return 当前耐久
		///
		DWORD UpdateEndure( DWORD dwProfit );
		
		///
		/// 更新持久到客户端
		///
		void UpdateEndureToClient( const CGUID &player_id );

		/// 检查是否有效（可作为商人是否装备了商业背包标志）
		bool IsValid() const;

		/// 得到当前背包可容纳的商品数量
		DWORD GetLeftGoodsCount() const
		{
			return m_dwMaxGoodsCount - m_dwGoodsCount;
		}

		void AddGoodsCount( DWORD dwAdd )
		{
			m_dwGoodsCount += dwAdd;
		}

		///
		/// 更新指定物品的价格，当装入新的物品时需要更新
		///
		/// @return 结果价格
		DWORD UpdateGoodsPrice( DWORD index, DWORD amount, DWORD price );
		DWORD UpdateGoodsPrice( CGoods *pGoods );

		///
		/// 编码商业背包数据存档
		///
		void EncodeToSave( DBWriteSet &db );

        void CodeForClient( DBWriteSet& db);

		///
		/// 由数据库读档
		///
		void DecodeFromDB( DBReadSet &db );

		/// 获得当前商业背包内物品总数
		DWORD GetCurGoodsAmount();
		
		/// 获得当前商业背包使用了多少格子
		DWORD GetUsedVolume();
		
		/// 加载一个商业背包
		BOOL MountBusinessPackGoods( CGoods* pGoods );

		///
		/// 检查是否可以替换商业背包
		///
		/// @param pPackGoods 商业背包物品
		///
		BOOL CanSwap( CGoods *pPackGoods );

		///
		/// 交换背包
		///
		/// @return 1:SWITCH_OBJECT 2:SWITCH_FILE
		int Swap( CGoods *pPackGoods );

		///
		/// 检查背包是否无效，如果无效则消失
		///
		bool CheckValid();

		///
		/// 获取背包当前耐久
		///
		LONG GetCurDurability();

	public:
		CVolumeLimitGoodsContainer *m_pContainer;
		CGoods *m_pGoods;
	private:
		// 当前商品数量
		DWORD m_dwGoodsCount;
		// 最大容纳的商品数量
		DWORD m_dwMaxGoodsCount;
	};
}