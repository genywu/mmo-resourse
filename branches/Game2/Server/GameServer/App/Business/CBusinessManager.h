///
/// @file CBusinessManager.h
/// @brief 商业系统对外模块顶层接口
///
#pragma once

#include "CTradeArea.h"
#include "CTradeSpot.h"
#include "CBusinessGoods.h"
#include "BusinessCommon.h"

class TiXmlElement;

namespace Business
{

	class CBusinessManager
	{
	public:
		typedef std::map<int, CTradeArea*> TradeAreaTable;
		typedef std::map<int, CTradeSpot*> TradeSpotTable;
		typedef std::vector<tagCreditDesc> CreditDescList;
		/// 建立NPC-贸易点之间的映射
		typedef std::map<CGUID, int, guid_compare> NPCSpotTable;
	public:
		CBusinessManager()
		{
			m_Setup.enable = 0;
		}

		~CBusinessManager();

		void Release();
		
		void DecodeFromDataBlock( DBReadSet &db );

		///
		/// 开启刷新限购额定时器，切换场景时调用。限购额的刷新是在切换场景后一段时间之后进行的。
		///
		void StartRefreshBuyLimitTimer( int RegionResID, const CGUID &player_id );

		///
		/// 开启商业系统，主要是开启一些定时器
		///
		void Startup();
		void Shutdown();

		/// Query the credit level by the credit value
		int QueryCreditLevel( int credit );

		///
		/// 编码商业数据库存档数据
		///
		void EncodeToSave( DBWriteSet &db );

		///
		/// 发送存档消息
		///
		void SendSaveMsg();

		///
		/// 添加NPC-贸易点映射
		///
		void AddNPCSpotRecord( const CGUID &npc_id, int spot_id );

		///
		/// 根据NPC ID查找对应的贸易点ID
		///
		int GetSpotByNPC( const CGUID &npc_id );

		///
		/// Get the trade spot by its id
		///
		CTradeSpot *GetTradeSpot( int id );

		///
		/// query the global system setup
		///
		const tagSystemSetup &GetSetup() const
		{
			return m_Setup;
		}

		///
		/// 判断商业系统是否被开启
		///
		bool IsEnable() const
		{
			return m_Setup.enable == 1;
		}
	private:
		TradeAreaTable m_Areas;
		TradeSpotTable m_Spots;
		CreditDescList m_CreditDesc;
		tagSystemSetup m_Setup;
		NPCSpotTable m_NpcSpotTable;
	};
}