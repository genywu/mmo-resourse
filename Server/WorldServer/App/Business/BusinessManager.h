///
/// @file CBusinessManager.h
/// @brief 跑商系统对外接口类
///
#pragma once

#include "TradeArea.h"
#include "TradeSpot.h"
#include "BusinessCommon.h"


class TiXmlElement;
class CEntityGroup;
namespace Business
{

	class CBusinessManager
	{
	public:
		typedef std::map<int, CTradeArea*> TradeAreaTable;
		typedef std::map<int, CTradeSpot*> TradeSpotTable;
		typedef std::vector<tagCreditDesc> CreditDescList;
	public:
		CBusinessManager()
		{
			m_bDBLoaded = false;
			memset( &m_Setup, 0, sizeof( m_Setup ) );
		}

		~CBusinessManager();

		/// called by the dtor automatically.
		void Release();

		bool Load();

		///
		/// 编码配置数据
		/// @param GSId 为指定GS编码数据
		///
		void EncodeToDataBlock( DBWriteSet &db, int GSId );

		///
		/// 编码进entity group，用于数据库存储
		///
		void EncodeToEntityGroup( CEntityGroup *entity );

		///
		/// 从数据库解码相关数据
		///
		void DecodeFromEntityGroup( long ret, CEntityGroup *entity );

		///
		/// 从GS解码存档数据
		///
		void DecodeFromGS( DBReadSet &db );

		///
		/// get the specified trade spot
		///
		CTradeSpot *GetTradeSpot( int id )
		{
			TradeSpotTable::iterator it = m_Spots.find( id );
			return it != m_Spots.end() ? it->second : NULL;
		}

		///
		/// 判断商业系统是否被开启
		///
		bool IsEnable() const
		{
			return m_Setup.enable == 1;
		}

		/// 
		/// 获取数据库装载标志
		///
		/// @see m_bDBLoaded
		///
		bool IsDBLoaded() const
		{
			return m_bDBLoaded;
		}
	private:
		bool LoadCreditDesc( TiXmlElement *elm );
		bool LoadTradeArea( TiXmlElement *elm );
		bool LoadTradeSpot( TiXmlElement *elm, int AreaId );

	private:
		TradeAreaTable m_Areas;
		TradeSpotTable m_Spots;
		CreditDescList m_CreditDesc;
		tagSystemSetup m_Setup;
		/// 标志是否已经从DB读取了存档数据，只有该标志为true时，才会将商业数据编码给GS；
		/// 同时，只有为true时，才可以让玩家登陆游戏(logmesssage.cpp)；
		/// 在第一次变为true时，该标志将永远保持true，这也意味着WS在运行期间不会第二次
		/// 从DBS请求数据；这里可以保证，WS上的商业数据将新于DBS；此标志还可以决定DBS
		/// 重启时是否从DBS装载数据
		bool m_bDBLoaded;
	};
}