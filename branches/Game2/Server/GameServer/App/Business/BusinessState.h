///
/// @file BusinessState.h
/// @brief 维护处于跑商操作界面的玩家列表
///
#pragma once

namespace Business
{
	class CTradeSpot;

	///
	/// 玩家处于哪种操作界面
	///
	enum 
	{
		OP_NONE,
		OP_BUY,
		OP_SELL,
	};

	///
	/// 玩家处于打开买/卖等界面时需要记录的信息
	///
	struct tagState
	{
		int iState;
		DWORD dwHiggleCount;	///< 使用‘讨价还价’技能连续成功次数
		/// 记录‘讨价还价’技能信息
		struct 
		{
			int iPriceQuo;
		};
	};


	///
	/// CStateGroup
	/// 同session相似，该类管理处于跑商操作界面的玩家列表及保存需要为某些操作提供的附加数据。
	/// 例如，处于买、卖界面的玩家需要实时地更新当前商品价格，还需要为处于买、卖界面的玩家
	/// 保存某些商业技能的连续成功次数。
	///
	class CStateGroup
	{
	public:
		typedef stdext::hash_map<CGUID, tagState, hash_guid_compare> PlayerStateTable;
	public:
		CStateGroup() : m_pSpot( NULL )
		{
		}

		~CStateGroup()
		{
		}

		void SetTradeSpot( CTradeSpot *pSpot )
		{
			m_pSpot = pSpot;
		}

		/// 
		/// 物价变化时主动更新价格，由物价波动定时器触发
		///
		void SendUpdatePriceMsg();
		void SendUpdateBuyPriceMsg( CPlayer *player );
		void SendUpdateSellPriceMsg( CPlayer *player );

		///
		/// 添加玩家
		///
		void AddPlayer( const CGUID &player_id, int iOperType );

		///
		/// 移除玩家，由客户端发送消息调用
		///
		void RemovePlayer( const CGUID &player_id );

		/// 
		/// 获取某玩家操作状态
		///
		const tagState &GetPlayerState( const CGUID &player_id );
		
		///
		/// 获取变化价格
		///
		/// 该类价格通常是由于其他因素，如技能在当前价格上导致的变化
		///
		DWORD AdjustPrice( CPlayer *pPlayer, DWORD dwBasePrice );

		///
		/// 记录‘讨价还价‘技能使用结果
		///
		void SetHiggleRecord( const CGUID &player_id, int iPriceQuo, bool bSuccess );

		///
		/// 使用’讨价还价‘技能
		///
		bool UseHiggleSkill( const CGUID &player_id, int iBuyQuo, int SellQuo, int iSuccessOdds );

	private:
		tagState &QueryPlayerState( const CGUID &player_id );
	private:
		PlayerStateTable m_PlayerTable;
		CTradeSpot *m_pSpot;
	};
}