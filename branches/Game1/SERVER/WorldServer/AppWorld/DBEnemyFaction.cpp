#include "StdAfx.h"
#include "DBEnemyFaction.h"
#include "DBentityManager.h"
#include "organizingsystem\FactionWarSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
//CDBEnemyFaction::CDBEnemyFaction()
//: CBaseDBEntity(),
//m_lFactionID1(NULL_GUID),
//m_lFactionID2(NULL_GUID),
//m_dwDisandTime(0)
//{
//	CGUID::CreateGUID(m_GUID);
//	for(int pos=(int)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1;  pos <= (int)DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME; pos++)
//	{
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//
//CDBEnemyFaction::CDBEnemyFaction(const CGUID& id)
//: CBaseDBEntity(id),
//m_lFactionID1(NULL_GUID),
//m_lFactionID2(NULL_GUID),
//m_dwDisandTime(0)
//{
//	for(int pos=(int)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1;  pos <= (int)DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME; pos++)
//	{
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//CDBEnemyFaction::~CDBEnemyFaction()
//{
//}
//
//// Other interfaces
//// 打开准备发送的属性在Vector中的相应位置的标志
//void CDBEnemyFaction::OpenAttrFlagInVector(DB_ENEMY_FACTION_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1 && attrType <= DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME)
//	{
//		long pos = DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME - attrType;
//		m_SendAttrEnumVector.push_back(1);
//	}
//}
//// 关闭准备发送的属性在Vector中的相应位置的标志
//void CDBEnemyFaction::CloseAttrFlagInVector(DB_ENEMY_FACTION_ATTR_TYPE attrType)
//{
//	if(attrType >= DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1 && attrType <= DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME)
//	{
//		long pos = DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME - attrType;
//		m_SendAttrEnumVector[pos] = 0;
//	}
//}
//
//// virtual interfaces
//void CDBEnemyFaction::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	//CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
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
//void CDBEnemyFaction::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//	////AddLogText("DB2W[%d] : DB_OPER_CREATE_ENEMY_FACTION, EnemyFactionID[%ds]...OK!", msgid, szGuid);
//}
//void CDBEnemyFaction::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
//	// flag
//	BYTE flag = _GetByteFromByteArray(msgBuf, msgBufPos);
//	//if(flag == 1)
//		////AddLogText("DB2W[%d] : DB_OPER_DELETE_ENEMY_FACTION, EnemyFactionID[%s]...OK!", msgid, szGuid);
//	//else
//	//	//AddLogText("DB2W[%d] : DB_OPER_DELETEENEMY_FACTION, EnemyFactionID[%s]...FAILED!", msgid, szGuid);
//}
//void CDBEnemyFaction::Load(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
//	//	Initiate();
//
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos); // MSGID
//
//	//删除定时器
////	GetGame()->DelMsgTimer(msgid);
//
//	//	g_RSCityWar.LoadAllCards();
//	// 解析消息
//	//CDBEntityManager::DBCountryMapItr itr = GetGame()->GetDBEntityManager()->GetDBEnemyFactionMap().find(GetID());
//
////	int efNum = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
////	for(int j=0; j<efNum; j++)
//	{
//		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//
//		for(int i=0; i<(int)attrNum; i++)
//		{
//			// add one card data
//			WORD num = _GetWordFromByteArray(msgBuf, msgBufPos);
//			DB_ENEMY_FACTION_ATTR_TYPE attrType = (DB_ENEMY_FACTION_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);
//
//			switch(attrType)
//			{
//			case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1:
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					SetFactionID1(tGuid);
//				}
//				break;
//			case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2:
//				{
//					CGUID tGuid;
//					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//					SetFactionID2(tGuid);
//				}
//				break;
//			case DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME:
//				{
//					SetDisandTime(_GetLongFromByteArray(msgBuf, msgBufPos));
//				}
//				break;
//			}			
//		}
//		
//		GetFactionWarSys()->AddOneEnmeyFaction(GetFactionID1(),GetFactionID2(),GetDisandTime());
//
//		GetFactionWarSys()->Initialize();
//
//		////AddLogText("DB2W[%d] : DB_OPER_LOAD_ENEMY_FACTION, EnemyFactionID[%s]...OK!", msgid, szGuid);
//
//		return;
//	}
//
//	////AddLogText("DB2W[%d] : DB_OPER_LOAD_ENEMY_FACTION, EnemyFactionID[%s]...FAILED!", msgid, szGuid);
//}
//
//void CDBEnemyFaction::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
//	//删除定时器
//	GetGame()->DelMsgTimer(msgid);
//	////AddLogText("DB2W[%d] : DB_OPER_LOAD_ENEMY_FACTION, EnemyFactionID[%s]...OK!", msgid, szGuid);
//}
//void CDBEnemyFaction::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	char szGuid[128];
//	GetExID().tostring(szGuid);
//
//	//删除定时器
//	GetGame()->DelMsgTimer(msgid);
//	////AddLogText("DB2W[%d] : DB_OPER_LOAD_ENEMY_FACTION, EnemyFactionID[%s]...OK!", msgid, szGuid);
//}
///*
//void CDBCityWar::Initiate(void)
//{
//m_lSellerID			= 0;
//m_lBuyerID			= 0;
//m_dwPrice			= 0;
//m_dwChargePrice		= 0;
//m_dwCardState		= 0;
//m_dwCardTime		= 0;
//m_dwStartTime		= 0;
//m_dwSerialNumber	= 0;
//memset(m_CardNumber, 0, sizeof(m_CardNumber));
//memset(m_CardType,   0, sizeof(m_CardType));
//memset(m_SellerAccount, 0, sizeof(m_SellerAccount));
//memset(m_BuyerAccount,  0, sizeof(m_BuyerAccount));
//}
//*/
//// 组装消息
//void CDBEnemyFaction::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
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
//			switch(DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1+i)
//			{
//			case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1);
//					if(DB_OPER_LOAD != opType)
//						_AddToByteArray(&pBA, GetFactionID1());
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2);
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, GetFactionID2());
//					}
//					else
//						_AddToByteArray(&pBA, (BYTE)0);
//				}
//				break;
//			case DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME:
//				{
//					_AddToByteArray(&pBA, (WORD)1);
//					_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME);
//					if(DB_OPER_LOAD != opType)
//					{
//						_AddToByteArray(&pBA, GetDisandTime());
//					}
//					else
//					{
//						_AddToByteArray(&pBA, (BYTE)0);
//					}
//				}
//				break;
//			}
//		}
//	}
//
//}