#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBFactionMemberGroup.h"
#include "DBentityManager.h"
#include "DBLoadThread.h"
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
void CDBFactionMemberGroup::Clear()
{
	m_mapDirtyAttrData.clear();
	set<CDBFactionMember*>::iterator itr = m_setMemberGroup.begin();
	for(; itr != m_setMemberGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setMemberGroup.clear();
}
// virtual interfaces
void CDBFactionMemberGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_FACTIONMEMBERGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_FACTIONMEMBERGROUP;
			saveData->saveObj->objNum  = 1;
			*(CDBFactionMemberGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
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


void CDBFactionMemberGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();
	set<CDBFactionMember*>::iterator itr = m_setMemberGroup.begin();
	for(; itr != m_setMemberGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setMemberGroup.clear();

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
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			CDBFactionMember* loadGoods = new CDBFactionMember(goodsID);
			*loadGoods = *existGoods;
			m_setMemberGroup.insert(loadGoods);
		}
	}

}

