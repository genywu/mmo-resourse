#include "StdAfx.h"
#include "DBSetup.h"
#include "DBentityManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBSetup::CDBSetup()
: CBaseDBEntity(),
m_lLeaveWordID(0),m_lPlayerID(NULL_GUID)
{	
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
	m_ID = 0;
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
}

CDBSetup::CDBSetup(const CGUID& id)
: CBaseDBEntity(id),
m_lLeaveWordID(0),m_lPlayerID(NULL_GUID)
{
	m_SendAttrEnumVector.push_back(1);
	m_SendAttrEnumVector.push_back(1);
}
CDBSetup::~CDBSetup()
{
}

void CDBSetup::OpenAttrFlagInVector(DB_SETUP_ATTR_TYPE attrType)
{
	if(attrType == DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID)
	{
		m_SendAttrEnumVector[0] = 1;
	}
	else if(attrType == DB_GEN_SETUP_ATTR_LONG_PLAYER_ID)
	{
		m_SendAttrEnumVector[1] = 1;
	}
}
// 关闭准备发送的属性在Vector中的相应位置的标志
void CDBSetup::CloseAttrFlagInVector(DB_SETUP_ATTR_TYPE attrType)
{
	if(attrType == DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID)
	{
		m_SendAttrEnumVector[0] = 0;
	}
	else if(attrType == DB_GEN_SETUP_ATTR_LONG_PLAYER_ID)
	{
		m_SendAttrEnumVector[1] = 0;
	}
}

// virtual interfaces
void CDBSetup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	//CBaseDBEntity::ProcessMsg(op, msgid, pMsg);
	switch(op)
	{
	case DB_OPER_SAVE:
		Save(msgid, msgBuf, msgBufPos);
		break;
	case DB_OPER_LOAD:
		Load(msgid, msgBuf, msgBufPos);
		break;
	}
}

void CDBSetup::Load(long msgid, BYTE* msgBuf, long& msgBufPos)
{
	long pos = 0; // 数据块读取指针

	// 解析消息
	CDBEntityManager::DBSetupMapItr itr = GetGame()->GetDBEntityManager()->GetDBSetupMap().find(GetExID());

	if(itr != GetGame()->GetDBEntityManager()->DBSetupMapEnd())
	{
		WORD attrNum = _GetWordFromByteArray(msgBuf, msgBufPos); // 属性种类个数

		for(int i=0; i<(int)attrNum; i++)
		{
			// add one card data
			WORD num = _GetWordFromByteArray(msgBuf, msgBufPos);
			DB_SETUP_ATTR_TYPE attrType = (DB_SETUP_ATTR_TYPE)_GetLongFromByteArray(msgBuf, msgBufPos);

			switch(attrType)
			{
			case DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID:
				{
					SetLeaveWordID(_GetLongFromByteArray(msgBuf, msgBufPos));
					
					GetGame()->SetDBLeaveWordID(GetLeaveWordID());
				}
				break;
			case DB_GEN_SETUP_ATTR_LONG_PLAYER_ID:
				{
					CGUID tGuid;
					_GetBufferFromByteArray(msgBuf, msgBufPos, tGuid);
					SetPlayerID(tGuid);
					GetGame()->SetDBPlayerID(GetPlayerID());
				}
				break;
			}
		}

		//AddLogText("DB2W[%d] : DB_OPER_LOAD_SETUP, SetupID[%d]...OK!", msgid, GetID());

		return;
	}

	//AddLogText("DB2W[%d] : DB_OPER_LOAD_SETUP, SetupID[%d]...FAILED!", msgid, GetID());
}

void CDBSetup::Save(long msgid, BYTE* msgBuf, long& msgBufPos)
{
	//AddLogText("DB2W[%d] : DB_OPER_SAVE_SETUP, SetupID[%d]...OK!", msgid, GetID());
}

void CDBSetup::AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA)
{
	// 传递来的消息前面已经加入了
	// OPEntityID:    4byte, 操作实体的ID值
	// DBOP Type:     1byte, 数据库操作类型（枚举值）
	// MSG ID:        4byte, 该消息的MSGID
	// Entity Num:    2byte, 该实体个数
	if(DB_OPER_DELETE == opType) // DELETE操作
	{
		return;
	}

	_AddToByteArray(&pBA, NULL_GUID);	// 实体ID
	_AddToByteArray(&pBA, (WORD)2);			// 属性种类个数

	if(( DB_OPER_CREATE == opType || DB_OPER_SAVE == opType || DB_OPER_UPDATE == opType || DB_OPER_LOAD == opType) )// 标志打开并且为SAVE/UPDATE/CREATE操作
	{
		_AddToByteArray(&pBA, (WORD)1);
		_AddToByteArray(&pBA, (long)DB_GEN_SETUP_ATTR_LONG_LEAVE_WORD_ID);
		_AddToByteArray(&pBA, GetGame()->GetDBLeaveWordID());
		
		_AddToByteArray(&pBA, (WORD)4);
		_AddToByteArray(&pBA, (long)DB_GEN_SETUP_ATTR_LONG_PLAYER_ID);
		_AddToByteArray(&pBA, GetGame()->GetDBPlayerID());
	}

}


