#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBMailGroup.h"
#include "DBentityManager.h"
#include "DBLoadThread.h"
#include "DBMail.h"
#include "DBPlayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBMailGroup::CDBMailGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBMailGroup::CDBMailGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBMailGroup::~CDBMailGroup()
{
	set<CDBMail*>::iterator itr = m_setMailGroup.begin();
	for(; itr != m_setMailGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setMailGroup.clear();
}

// virtual interfaces
void CDBMailGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_INSERT:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_MAILGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Insert(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_MAILGROUP;
			saveData->saveObj->objNum  = 1;
			saveData->saveObj->opType  = DB_OPER_INSERT;
			*(CDBMailGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_SAVE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_MAILGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_MAILGROUP;
			saveData->saveObj->objNum  = 1;
			saveData->saveObj->opType  = DB_OPER_SAVE;
			*(CDBMailGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
		}
		break;
	case DB_OPER_DELETE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_MAILGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Delete(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_MAILGROUP;
			saveData->saveObj->objNum  = -(long)GetMailGroupSet().size();
			saveData->saveObj->opType  = DB_OPER_DELETE;
			*(CDBMailGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	}
}

void CDBMailGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID
	_AddToByteArray(&pBA, (WORD)m_setMailGroup.size()); // 物品个数

	set<CDBMail*>::iterator itr = m_setMailGroup.begin();
	for(; itr != m_setMailGroup.end(); itr++)
	{
		(*itr)->AddToByteArray(pBA);
	}
}

void CDBMailGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	Clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBMail* existGoods = new CDBMail(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_setMailGroup.insert(existGoods);
		}
	}
	CDBPlayer* owner =  GetGame()->GetDBEntityManager()->FindPlayer(m_OwnerGUID);
	if(owner)
	{
		*(owner->GetMailGroupPtr()) = *this;
	}
}

void CDBMailGroup::Insert(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize, const CGUID& ownerID)
{ 
	Clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	if(m_OwnerGUID == NULL_GUID && ownerID != NULL_GUID)
		m_OwnerGUID = ownerID;

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	CDBPlayer* owner =  GetGame()->GetDBEntityManager()->FindPlayer(m_OwnerGUID);
	if(owner)
	{
		for(int j=0; j<goodsNum; j++)
		{
			// 解析消息
			CGUID goodsID;
			_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
			CDBMail* existGoods = new CDBMail(goodsID);
			if(existGoods)
			{
				existGoods->Save(0, msgBuf, msgBufPos, bufSize);
				owner->GetMailGroupPtr()->GetMailGroupSet().insert(existGoods);
				existGoods->SetGuidAttr(string("PlayerID"), string("CSL_MAIL"), 0, m_OwnerGUID);
				m_setMailGroup.insert(existGoods);
			}
		}
	}
}
void CDBMailGroup::Delete(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize, const CGUID& ownerID)
{
	Clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	if(m_OwnerGUID == NULL_GUID && ownerID != NULL_GUID)
		m_OwnerGUID = ownerID;

	CDBPlayer* owner =  GetGame()->GetDBEntityManager()->FindPlayer(m_OwnerGUID);
	if(owner)
	{
		for(int j=0; j<goodsNum; j++)
		{
			// 解析消息
			CGUID goodsID;
			_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
			CDBMail* existGoods = new CDBMail(goodsID);
			if(existGoods)
			{
				existGoods->Save(0, msgBuf, msgBufPos, bufSize);
				existGoods->SetGuidAttr(string("PlayerID"), string("CSL_MAIL"), 0, m_OwnerGUID);
				m_setMailGroup.insert(existGoods);
			}
		}
	}
}
void CDBMailGroup::Clear(void)
{
	set<CDBMail*>::iterator itr = GetMailGroupSet().begin();
	for(; itr != GetMailGroupSet().end(); itr++)
	{
		SAFE_DELETE((*itr));
	}
	GetMailGroupSet().clear();
}

