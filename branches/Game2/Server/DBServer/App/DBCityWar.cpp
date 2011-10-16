#include "StdAfx.h"
#include "DBCityWar.h"
#include "..\..\..\dbaccess\worlddb\RSCityWar.h"
#include "DBentityManager.h"
#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
//CDBCityWar::CDBCityWar()
//: CBaseDBEntity(),
//m_lRegionID(0)
//{
//	m_listDecWarFactions.clear();
//	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
//}
//
//CDBCityWar::CDBCityWar(const CGUID& id)
//: CBaseDBEntity(id),
//m_lRegionID(0)
//{
//	m_listDecWarFactions.clear();
//}
//CDBCityWar::~CDBCityWar()
//{
//
//};
//
//// virtual interfaces
//void CDBCityWar::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	switch(op)
//	{
//	case DB_OPER_CREATE:
//		Create(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_DELETE:
//		Delete(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_SAVE:
//		{
//			// 组装消息
//			Save(msgid, msgBuf, msgBufPos);
//			GetGame()->GetRsCityWar()->Save(this);
//		}
//		break;
//	case DB_OPER_LOAD:
//		{
//			GetGame()->GetRsCityWar()->Load(this);
//			vector<BYTE> pBA;
//			Load(msgid, msgBuf, msgBufPos, pBA);
//		}
//		break;
//	case DB_OPER_UPDATE:
//		GetGame()->GetRsCityWar()->Save(this);
//		Update(msgid, msgBuf, msgBufPos);
//		break;
//	}
//}
//void CDBCityWar::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
////	Initiate();
//
//	// MSGID
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	// 解析消息
//	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//	for(WORD i = 0; i < attrNum; i++)
//	{
//		WORD attrDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
//		DB_CITY_WAR_ATTR_TYPE type = (DB_CITY_WAR_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值
//	
//		switch(type)
//		{
//		case DB_CITY_WAR_ATTR_LONG_CITYREGIONID:
//			{
//				SetRegionID(_GetLongFromByteArray(msgBuf, msgBufPos));
//			}
//			break;
//		case DB_CITY_WAR_ATTR_BUF_WAR_START_TIME:
//			{
//				tagTime& time = GetWarStartTime();
//				time.tmTime.tm_year = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_mon	= _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_wday = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_mday = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_hour = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_min	= _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_sec	= _GetWordFromByteArray(msgBuf, msgBufPos);
//			}
//			break;
//		case DB_CITY_WAR_ATTR_BUF_DECWARFACTION:
//			{
//				list<CGUID>& df = GetDecWarFactions();
//				for(int j=0; j<attrDataNum; j++)
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					df.insert(df.end(), tGuid);
//				}
//			}
//			break;
//		}
//	}
//}
//void CDBCityWar::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//
//}
//void CDBCityWar::Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA)
//{
//	// initiate
//	//Initiate();
//
//	CDBEntityManager::DBCityWarMapItr itr = GetGame()->GetDBEntityManager()->DBCityWarMapBegin();
//	for(; itr != GetGame()->GetDBEntityManager()->DBCityWarMapEnd(); itr++)
//	{
//		_AddToByteArray(&pBA, m_GUID); // 实体ID
//		_AddToByteArray(&pBA, (WORD)3/*CDBCityWar的所有属性种类个数*/); // 发送的CDBCityWar的所有属性种类个数
//
//		// add one card data
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_LONG_CITYREGIONID);
//		_AddToByteArray(&pBA, GetRegionID());
//
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_BUF_WAR_START_TIME);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_year	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_mon	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_wday	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_mday	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_hour	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_min	);
//		_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_sec	);
//						
//		_AddToByteArray(&pBA, (WORD)GetDecWarFactions().size());
//		_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_BUF_DECWARFACTION);
//		list<CGUID>::iterator itr = GetDecWarFactions().begin();
//		for(; itr != GetDecWarFactions().end(); itr++)
//		{
//			_AddToByteArray(&pBA, *itr);
//		}
//	}
//
//}
//
//void CDBCityWar::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
////	Initiate();
//
//	// MSGID
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	// 解析消息
//	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//	for(WORD i = 0; i < attrNum; i++)
//	{
//		WORD attrDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
//		DB_CITY_WAR_ATTR_TYPE type = (DB_CITY_WAR_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值
//		//DB_ATTR_DATA_TYPE cppType = (DB_ATTR_DATA_TYPE)pMsg->GetByte(); //该属性的C++数据类型
//		switch(type)
//		{
//		case DB_CITY_WAR_ATTR_LONG_CITYREGIONID:
//			{
//				SetRegionID(_GetLongFromByteArray(msgBuf, msgBufPos));
//			}
//			break;
//		case DB_CITY_WAR_ATTR_BUF_WAR_START_TIME:
//			{
//				tagTime& time = GetWarStartTime();
//				time.tmTime.tm_year = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_mon	= _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_wday = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_mday = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_hour = _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_min	= _GetWordFromByteArray(msgBuf, msgBufPos);
//				time.tmTime.tm_sec	= _GetWordFromByteArray(msgBuf, msgBufPos);
//			}
//			break;
//		case DB_CITY_WAR_ATTR_BUF_DECWARFACTION:
//			{
//				list<CGUID>& df = GetDecWarFactions();
//				for(int j=0; j<attrDataNum; j++)
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					df.insert(df.end(), tGuid);
//				}
//			}
//			break;
//		}
//	}
//}
//void CDBCityWar::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//
//}
