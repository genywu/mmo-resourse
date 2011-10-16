///
/// @file CBusinessPackage.h
/// @author Kevin lynx
/// @brief 描述商业背包
///
#pragma once

class CVolumeLimitGoodsContainer;
class CGoods;
class CPlayer;

namespace Business
{
	class CPackage
	{
	public:
		CPackage();

		~CPackage();

		///
		/// 从GS解码数据，缓存在本地以后存入DB
		///
		void DecodeFromGS( CPlayer *pPlayer, DBReadSet &db );

		///
		/// 将缓存数据（从DB读取）编码给GS
		///
		void EncodeForGS( DBWriteSet &db );

		///
		/// 从数据库解码数据
		///
		/// 事实上，该函数并不完全从数据库解码数据，即从entity group解码，真正从entity group解码
		/// 商业背包数据是在CEntityManager::DetailBusinessPackCDBPlayerToCPlayer中，而此函数仅仅
		/// 是处理解码商业背包本身（是个物品）时，有效地设置自身容器属性。
		///
		void DecodeFromDB( CGoods *pContainerGoods );

	public:
		CVolumeLimitGoodsContainer *m_pContainer;
		CGoods *m_pGoods;
	};
}