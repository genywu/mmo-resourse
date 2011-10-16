#include "stdafx.h"
#include "../worldServer/game.h"
#include "DBFactionMemberGroup.h"
#include "DBentityManager.h"
#include "DBFaction.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBFactionMemberGroup::CDBFactionMemberGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBFactionMemberGroup::CDBFactionMemberGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBFactionMemberGroup::~CDBFactionMemberGroup()
{
	set<CDBFactionMember*>::iterator itr = m_setMemberGroup.begin();
	for(; itr != m_setMemberGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setMemberGroup.clear();
}

// virtual interfaces
void CDBFactionMemberGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{

		}
		break;
	case DB_OPER_LOAD:
		{
			Load(msgid, msgBuf, msgBufPos, bufSize);
		}
		break;
	}
}

void CDBFactionMemberGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID
	_AddToByteArray(&pBA, (WORD)m_setMemberGroup.size()); // 物品个数

	set<CDBFactionMember*>::iterator itr = m_setMemberGroup.begin();
	for(; itr != m_setMemberGroup.end(); itr++)
	{
		(*itr)->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}

void CDBFactionMemberGroup::Clear()
{
	m_mapDirtyAttrData.clear();
	set<CDBFactionMember*>::iterator itr = m_setMemberGroup.begin();
	for(; itr != m_setMemberGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setMemberGroup.clear();
}
void CDBFactionMemberGroup::Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	Clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBFactionMember* existGoods = new CDBFactionMember(goodsID);
		if(existGoods)
		{
			existGoods->Load(0, msgBuf, msgBufPos, bufSize);
			m_setMemberGroup.insert(existGoods);
		}
	}
}

