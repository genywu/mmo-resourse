#include "StdAfx.h"
#include "DBGenVar.h"
#include "DBGenVarGroup.h"
#include "DBentityManager.h"
#include "../DBServer/game.h"
#include "DBLoadThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDBGenVarGroup::CDBGenVarGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBGenVarGroup::CDBGenVarGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBGenVarGroup::~CDBGenVarGroup()
{
	set<CDBGenVar*>::iterator itr = m_setGenVarGroup.begin();
	for(; itr != m_setGenVarGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setGenVarGroup.clear();
}
CDBGenVarGroup& CDBGenVarGroup::operator=(CDBGenVarGroup& right)
{
	CBaseDBEntity::operator=(right);
	if(this != &right)
	{
		m_GUID = right.m_GUID;
		m_OwnerGUID = right.m_OwnerGUID;
		m_setGenVarGroup.clear();
		set<CDBGenVar*>::iterator itr = right.m_setGenVarGroup.begin();
		for(; itr != right.m_setGenVarGroup.end(); itr++)
		{
			CDBGenVar* tGoods = new CDBGenVar((*itr)->GetExID());
			*tGoods = *(*itr);
			m_setGenVarGroup.insert(tGoods);
		}
	}
	return *this;
}
// virtual interfaces
void CDBGenVarGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_INSERT:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_GENVARGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Insert(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_GENVARGROUP;
			saveData->saveObj->objNum  = 1;
			saveData->saveObj->opType  = DB_OPER_INSERT;
			*(CDBGenVarGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_SAVE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_GENVARGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_GENVARGROUP;
			saveData->saveObj->objNum  = 1;
			saveData->saveObj->opType  = DB_OPER_SAVE;
			*(CDBGenVarGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
		}
		break;
	case DB_OPER_DELETE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_GENVARGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Delete(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_GENVARGROUP;
			saveData->saveObj->objNum  = -(long)GetGenVarGroupSet().size();
			saveData->saveObj->opType  = DB_OPER_DELETE;
			*(CDBGenVarGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	}
}

void CDBGenVarGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID
	_AddToByteArray(&pBA, (WORD)m_setGenVarGroup.size()); // 物品个数

	set<CDBGenVar*>::iterator itr = m_setGenVarGroup.begin();
	for(; itr != m_setGenVarGroup.end(); itr++)
	{
		(*itr)->AddToByteArray(pBA);
	}
}

void CDBGenVarGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	Clear();

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBGenVar* existGoods = new CDBGenVar(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_setGenVarGroup.insert(existGoods);
		}
	}
}

void CDBGenVarGroup::Insert(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize, const CGUID& ownerID)
{ 
	Clear();

	
	if(m_OwnerGUID == NULL_GUID && ownerID != NULL_GUID)
		m_OwnerGUID = ownerID;

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBGenVar* existGoods = new CDBGenVar(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_setGenVarGroup.insert(existGoods);
		}
	}
	
}
void CDBGenVarGroup::Delete(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize, const CGUID& ownerID)
{
	Clear();

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	if(m_OwnerGUID == NULL_GUID && ownerID != NULL_GUID)
		m_OwnerGUID = ownerID;

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBGenVar* existGoods = new CDBGenVar(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_setGenVarGroup.insert(existGoods);
		}
	}
}
void CDBGenVarGroup::Clear(void)
{
	set<CDBGenVar*>::iterator itr = GetGenVarGroupSet().begin();
	for(; itr != GetGenVarGroupSet().end(); itr++)
	{
		SAFE_DELETE((*itr));
	}
	GetGenVarGroupSet().clear();
}

