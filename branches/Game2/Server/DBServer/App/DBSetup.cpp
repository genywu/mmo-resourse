#include "StdAfx.h"
#include "DBSetup.h"
#include "..\..\..\dbaccess\worlddb\RSSetup.h"
#include "DBEntityManager.h"
#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBSetup::CDBSetup()
: CBaseDBEntity(),
m_lLeaveWordID(0),m_lPlayerID(0)
{	
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBSetup::CDBSetup(const CGUID& id)
: m_lLeaveWordID(0),m_lPlayerID(0)
{
	m_GUID = id;
}
CDBSetup::~CDBSetup()
{
}

// virtual interfaces
void CDBSetup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			// 组装消息
			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
			// DBOP Type:     1byte, 数据库操作类型（枚举值）
			// MSG ID:        4byte, 该消息的MSGID
			// Entity Num:	2byte, 该实体个数(X)
			Save(msgid, msgBuf, msgBufPos);
//			g_RsSetup.SavePlayerID();
//			g_RsSetup.SaveLeaveWorldID();
		}
		break;
	case DB_OPER_LOAD:
		{
			// 组装消息
			// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
			// DBOP Type:     1byte, 数据库操作类型（枚举值）
			// MSG ID:        4byte, 该消息的MSGID
			// Entity Num:	2byte, 该实体个数(X)
			GetGame()->GetRsSetup()->LoadLeaveWorldID();
			GetGame()->GetRsSetup()->LoadPlayerID();
			vector<BYTE> pBA;
			Load(msgid, msgBuf, msgBufPos, pBA);
		}
		break;
	}
}
void CDBSetup::Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA)
{
	// initiate
	//Initiate();

	CDBEntityManager::DBSetupMapItr itr = GetGame()->GetDBEntityManager()->GetDBSetupMap().begin();
	if(itr->second)
	{
		_AddToByteArray(&pBA, GetID()); // ENTITY ID
		_AddToByteArray(&pBA, (WORD)2/*CDRegion的所有属性种类个数*/); // 发送的CDRegion的所有属性种类个数

		// add one card data
		_AddToByteArray(&pBA, (WORD)1);
		_AddToByteArray(&pBA, (long)DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID);
		_AddToByteArray(&pBA, GetLeaveWordID());

		_AddToByteArray(&pBA, (WORD)1);
		_AddToByteArray(&pBA, (long)DB_GEN_SETUP_ATTR_LONG_PLAYER_ID);
		_AddToByteArray(&pBA, GetPlayerID());
	}
}

void CDBSetup::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
{
	// initiate
	//	Initiate();

	// 解析消息
	WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数
	for(WORD i = 0; i < attrNum; i++)
	{
		WORD attrDataNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性数据个数
		DB_SETUP_ATTR_TYPE type = (DB_SETUP_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);//属性类型枚举值

		switch(type)
		{
		case DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID:
			{
				SetLeaveWordID(_GetLongFromByteArray(msgBuf, msgBufPos));
			}
			break;
		case DB_GEN_SETUP_ATTR_LONG_PLAYER_ID:
			{
				CGUID tGuid;
				_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
				SetPlayerID(tGuid);
			}
			break;
		}
	}
}
