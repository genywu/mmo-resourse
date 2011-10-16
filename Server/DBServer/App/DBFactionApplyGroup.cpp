#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBFactionApplyGroup.h"
#include "DBentityManager.h"
#include "DBLoadThread.h"
#include "DBFaction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBFactionApplyGroup::CDBFactionApplyGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBFactionApplyGroup::CDBFactionApplyGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBFactionApplyGroup::~CDBFactionApplyGroup()
{
	set<CDBFactionApply*>::iterator itr = m_setApplyGroup.begin();
	for(; itr != m_setApplyGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setApplyGroup.clear();
}
void CDBFactionApplyGroup::Clear()
{
	m_mapDirtyAttrData.clear();
	set<CDBFactionApply*>::iterator itr = m_setApplyGroup.begin();
	for(; itr != m_setApplyGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setApplyGroup.clear();
}

// virtual interfaces
void CDBFactionApplyGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_FACTIONAPPLYGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_FACTIONAPPLYGROUP;
			saveData->saveObj->objNum  = 1;
			*(CDBFactionApplyGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
		}
		break;
	}
}

void CDBFactionApplyGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID
	_AddToByteArray(&pBA, (WORD)m_setApplyGroup.size()); // 物品个数

	set<CDBFactionApply*>::iterator itr = m_setApplyGroup.begin();
	for(; itr != m_setApplyGroup.end(); itr++)
	{
		(*itr)->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}


void CDBFactionApplyGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();
	set<CDBFactionApply*>::iterator itr = m_setApplyGroup.begin();
	for(; itr != m_setApplyGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setApplyGroup.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBFactionApply* existGoods = new CDBFactionApply(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			CDBFactionApply* loadGoods = new CDBFactionApply(goodsID);
			*loadGoods = *existGoods;
			m_setApplyGroup.insert(loadGoods);
		}
	}

}

