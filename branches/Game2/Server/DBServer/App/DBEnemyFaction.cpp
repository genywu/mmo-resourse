#include "StdAfx.h"
#include "DBEnemyFaction.h"
#include "DBentityManager.h"
#include "..\..\..\dbaccess\worlddb\RSEnemyFactions.h"
#include "../DBServer/game.h"

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
//	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
//}
//
//CDBEnemyFaction::CDBEnemyFaction(const CGUID& id)
//:	m_lFactionID1(NULL_GUID),
//	m_lFactionID2(NULL_GUID),
//	m_dwDisandTime(0),
//	CBaseDBEntity(id)
//{
//}
//CDBEnemyFaction::~CDBEnemyFaction()
//{
////	GetGame()->GetDBEntityManager()->FreeDBEnemyFactionID(m_ID);
//}
//
//// virtual interfaces
//void CDBEnemyFaction::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos)
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
//			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
//			// DBOP Type:     1byte, 数据库操作类型（枚举值）
//			// MSG ID:        4byte, 该消息的MSGID
//			// Entity Num:	2byte, 该实体个数(X)
//			Save(msgid, msgBuf, msgBufPos);
//			GetGame()->GetRsEnemyFactions()->SaveEnemyFaction(this);
//		}
//		break;
//	case DB_OPER_LOAD:
//		{
//			// 组装消息
//			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
//			// DBOP Type:     1byte, 数据库操作类型（枚举值）
//			// MSG ID:        4byte, 该消息的MSGID
//			// Entity Num:	2byte, 该实体个数(X)
//			GetGame()->GetRsEnemyFactions()->LoadEnemyFaction(this);
//		}
//		break;
//	case DB_OPER_UPDATE:
//		//g_RSCards.SaveLargess(this);
//		Update(msgid, msgBuf, msgBufPos);
//		break;
//	}
//}
//void CDBEnemyFaction::Create(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
//	//	Initiate();
//
//	// MSGID
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	// 解析消息
//	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//	for(WORD i = 0; i < attrNum; i++)
//	{
//		WORD attrDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
//		DB_ENEMY_FACTION_ATTR_TYPE type = (DB_ENEMY_FACTION_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值
//
//		switch(type)
//		{
//		case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetFactionID1(tGuid);
//			}
//			break;
//		case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetFactionID2(tGuid);
//			}
//			break;
//		case DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME:
//			{
//				SetDisandTime(_GetLongFromByteArray(msgBuf, msgBufPos));
//			}
//			break;
//		}
//	}
//}
//void CDBEnemyFaction::Delete(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//
//}
//void CDBEnemyFaction::Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA)
//{
//	// initiate
//	//Initiate();
//	CDBEntityManager::DBEnemyFactionMapItr itr = GetGame()->GetDBEntityManager()->DBEnemyFactionMapBegin();
//	for(; itr != GetGame()->GetDBEntityManager()->DBEnemyFactionMapEnd(); itr++)
//	{
//		_AddToByteArray(&pBA, m_GUID); // 实体ID
//		_AddToByteArray(&pBA, (WORD)3/*CDBEnemyFaction的所有属性种类个数*/); // 发送的CDBEnemyFaction的所有属性种类个数
//
//		// add one card data
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1);
//		_AddToByteArray(&pBA, GetFactionID1());
//
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2);
//		_AddToByteArray(&pBA, GetFactionID2());
//
//		_AddToByteArray(&pBA, (WORD)1);
//		_AddToByteArray(&pBA, (long)DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME);
//		_AddToByteArray(&pBA, GetDisandTime());
//	}
//}
//
//void CDBEnemyFaction::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//	// initiate
//	//	Initiate();
//
//	// MSGID
////	long msgid = _GetLongFromByteArray(msgBuf, msgBufPos);
//
//	// 解析消息
//	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
//	for(WORD i = 0; i < attrNum; i++)
//	{
//		WORD attrDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
//		DB_ENEMY_FACTION_ATTR_TYPE type = (DB_ENEMY_FACTION_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值
//		//DB_ATTR_DATA_TYPE cppType = (DB_ATTR_DATA_TYPE)pMsg->GetByte(); //该属性的C++数据类型
//
//		switch(type)
//		{
//		case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID1:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetFactionID1(tGuid);
//			}
//			break;
//		case DB_ENEMY_FACTION_ATTR_LONG_FACTIONID2:
//			{
//				CGUID tGuid;
//				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
//				SetFactionID2(tGuid);
//			}
//			break;
//		case DB_ENEMY_FACTION_ATTR_DWORD_DISANDTIME:
//			{
//				SetDisandTime(_GetLongFromByteArray(msgBuf, msgBufPos));
//			}
//			break;
//		}
//	}
//}
//void CDBEnemyFaction::Update(long msgid, BYTE* msgBuf, long& msgBufPos)
//{
//}
