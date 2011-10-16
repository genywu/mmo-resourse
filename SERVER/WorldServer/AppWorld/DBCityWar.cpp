#include "StdAfx.h"
#include "DBCityWar.h"
#include "DBentityManager.h"
#include "organizingsystem/AttackCitySys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
//CDBCityWar::CDBCityWar()
//: CBaseDBEntity(),
//	m_lRegionID(0)
//{
//	CGUID::CreateGUID(m_GUID);
//	m_listDecWarFactions.clear();
//	for(int pos=(int)DB_CITY_WAR_ATTR_LONG_CITYREGIONID;  pos <= (int)DB_CITY_WAR_ATTR_BUF_DECWARFACTION; pos++)
//	{
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//
//CDBCityWar::CDBCityWar(const CGUID& id)
//:	CBaseDBEntity(id),
//	m_lRegionID(0)
//{
//	m_listDecWarFactions.clear();
//	for(int pos=(int)DB_CITY_WAR_ATTR_LONG_CITYREGIONID;  pos <= (int)DB_CITY_WAR_ATTR_BUF_DECWARFACTION; pos++)
//	{
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//CDBCityWar::~CDBCityWar()
//{
//
//}
//
//// Other interfaces
//// 打开准备发送的属性在Vector中的相应位置的标志
//void CDBCityWar::OpenAttrFlagInVector(DB_CITY_WAR_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_CITY_WAR_ATTR_LONG_CITYREGIONID && attrType <= DB_CITY_WAR_ATTR_BUF_DECWARFACTION)
//	{
//		long pos = DB_CITY_WAR_ATTR_BUF_DECWARFACTION - attrType;
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//// 关闭准备发送的属性在Vector中的相应位置的标志
//void CDBCityWar::CloseAttrFlagInVector(DB_CITY_WAR_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_CITY_WAR_ATTR_LONG_CITYREGIONID && attrType <= DB_CITY_WAR_ATTR_BUF_DECWARFACTION)
//	{
//		long pos = DB_CITY_WAR_ATTR_BUF_DECWARFACTION - attrType;
//		m_SendAttrEnumVector[pos] = 0;
//	}
//}
//
//// virtual interfaces
//void CDBCityWar::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos)
//{
////	CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
//	switch(op)
//	{
//	case DB_OPER_CREATE:
//		Create(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_DELETE:
//		Delete(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_SAVE:
//		Save(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_LOAD:
//		Load(msgid, msgBuf, msgBufPos);
//		break;
//	case DB_OPER_UPDATE:
//		Update(msgid, msgBuf, msgBufPos);
//		break;
//	}
//}
//
//void CDBCityWar::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//	////AddLogText("DB2W[%d] : DB_OPER_CREATE_CITY_WAR, CardID[%d]...OK!", msgid, szGuid);
//}
//void CDBCityWar::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
//	// flag
//	BYTE flag = _GetByteFromByteArray(msgBuf, msgBufPos);
//	//if(flag == 1)
//		////AddLogText("DB2W[%d] : DB_OPER_DELETE_CITY_WAR, CardID[%d]...OK!", msgid, szGuid);
//	//else
//	//	//AddLogText("DB2W[%d] : DB_OPER_DELETE_CITY_WAR, CardID[%d]...FAILED!", msgid, szGuid);
//}
//void CDBCityWar::Load(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
////	Initiate();
//
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos); // MSGID
//
//	//删除定时器
////	GetGame()->DelMsgTimer(msgid);
//
////	g_RSCityWar.LoadAllCards();
//
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
//	// 解析消息
//	CDBEntityManager::DBCardMapItr itr = GetGame()->GetDBEntityManager()->GetDBCardMap().find(GetExID());
//
//	if(itr != GetGame()->GetDBEntityManager()->DBCardMapEnd())
//	{
//		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos);
//
//		for(int i=0; i<(int)attrNum; i++)
//		{
//			// add one card data
//			WORD num = _GetWordFromByteArray(msgBuf, msgBufPos);
//			DB_CITY_WAR_ATTR_TYPE attrType = (DB_CITY_WAR_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);
//			switch(attrType)
//			{
//			case DB_CITY_WAR_ATTR_LONG_CITYREGIONID:
//				{
//					SetRegionID(_GetLongFromByteArray(msgBuf, msgBufPos));
//				}
//				break;
//			case DB_CITY_WAR_ATTR_BUF_WAR_START_TIME:
//				{
//					tagTime& time = GetWarStartTime();
//					time.tmTime.tm_year = _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_mon	= _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_wday = _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_mday = _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_hour = _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_min	= _GetWordFromByteArray(msgBuf, msgBufPos);
//					time.tmTime.tm_sec	= _GetWordFromByteArray(msgBuf, msgBufPos);
//				}
//				break;
//			case DB_CITY_WAR_ATTR_BUF_DECWARFACTION:
//				{
//					list<CGUID>& df = GetDecWarFactions();
//					CGUID tGUID;
//					for(int j=0; j<num; j++)
//					{
//						_GetBufferFromByteArray(msgBuf, msgBufPos, tGUID);
//						df.push_back(tGUID);
//					}
//				}
//				break;
//			}
//		}
//
//		GetAttackCitySys()->SetDecWarFactionsFromDB(GetRegionID(), GetWarStartTime(), GetDecWarFactions());
//
//		////AddLogText("DB2W[%d] : DB_OPER_LOAD_CITY_WAR, CardID[%s]...OK!", msgid, szGuid);
//
//		return;
//	}
//
//	////AddLogText("DB2W[%d] : DB_OPER_LOAD_CITY_WAR, CardID[%s]...FAILED!", msgid, szGuid);
//}
//
//void CDBCityWar::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//	////AddLogText("DB2W[%d] : DB_OPER_SAVE_CITY_WAR, CardID[%s]...OK!", msgid, szGuid);
//}
//void CDBCityWar::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//	////AddLogText("DB2W[%d] : DB_OPER_UPDATE_CITY_WAR, CardID[%s]...OK!", msgid, szGuid);
//}
///*
//void CDBCityWar::Initiate(void)
//{
//	m_lSellerID			= 0;
//	m_lBuyerID			= 0;
//	m_dwPrice			= 0;
//	m_dwChargePrice		= 0;
//	m_dwCardState		= 0;
//	m_dwCardTime		= 0;
//	m_dwStartTime		= 0;
//	m_dwSerialNumber	= 0;
//	memset(m_CardNumber, 0, sizeof(m_CardNumber));
//	memset(m_CardType,   0, sizeof(m_CardType));
//	memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
//	memset(m_BuyerAccount,  0, sizeof(m_BuyerAccount));
//}
//*/
//// 组装消息
//void CDBCityWar::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
//{
//	// 传递来的消息前面已经加入了
//	// OPEntityID:    4byte, 操作实体的ID值
//	// DBOP Type:     1byte, 数据库操作类型（枚举值）
//	// MSG ID:        4byte, 该消息的MSGID
//	// Entity Num:    2byte, 该实体个数
//	if(DB_OPER_DELETE == opType) // DELETE操作
//	{
//		return;
//	}
//
//	WORD attrNum = 0;
//	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
//	{
//		if(m_SendAttrEnumVector[i] != 0)
//			attrNum++;
//	}
//	_AddToByteArray(&pBA, m_GUID); // 实体ID
//	_AddToByteArray(&pBA, attrNum); // 属性种类个数
//
//	for(int i = 0; i < (int)m_SendAttrEnumVector.size(); i++)
//	{
//		if(m_SendAttrEnumVector[i] != 0 
//			&& ( DB_OPER_SAVE == opType || DB_OPER_UPDATE == opType || DB_OPER_LOAD == opType) )// 标志打开并且为SAVE/UPDATE/CREATE操作
//		{
//			switch(DB_CITY_WAR_ATTR_LONG_CITYREGIONID+i)
//			{
//			case DB_CITY_WAR_ATTR_LONG_CITYREGIONID:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_LONG_CITYREGIONID);
//					if(DB_OPER_LOAD != opType)
//						_AddToByteArray(&pBA, GetRegionID());
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_CITY_WAR_ATTR_BUF_WAR_START_TIME:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_BUF_WAR_START_TIME);
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_year);
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_mon );
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_wday);
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_mday);
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_hour);
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_min );
//						_AddToByteArray(&pBA, (WORD)GetWarStartTime().tmTime.tm_sec );
//					}
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_CITY_WAR_ATTR_BUF_DECWARFACTION:
//				{
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, (WORD)m_listDecWarFactions.size());
//						_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_BUF_DECWARFACTION);
//						list<CGUID>::iterator itr = m_listDecWarFactions.begin();
//						for(; itr != m_listDecWarFactions.end(); itr++)
//						{
//							_AddToByteArray(&pBA, *itr);
//						}
//					}
//					else
//					{
//						_AddToByteArray(&pBA, (WORD)1);
//						_AddToByteArray(&pBA, (long)DB_CITY_WAR_ATTR_BUF_DECWARFACTION);
//						_AddToByteArray(&pBA, (BYTE)0);
//					}
//				}
//				break;
//			}
//		}
//	}
//
//}