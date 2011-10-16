#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBRgnGoodsGroup.h"
#include "DBentityManager.h"
#include "DBLoadThread.h"
#include "DBDupRgn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBRgnGoodsGroup::CDBRgnGoodsGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBRgnGoodsGroup::CDBRgnGoodsGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBRgnGoodsGroup::~CDBRgnGoodsGroup()
{
	map<CGUID,CDBRgnGoods*>::iterator itr = m_mapGoodsGroup.begin();
	for(; itr != m_mapGoodsGroup.end(); itr++)
	{
		SAFE_DELETE(itr->second);
	}
	m_mapGoodsGroup.clear();
}

// virtual interfaces
void CDBRgnGoodsGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_PHRGNGOODSGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_PHRGNGOODSGROUP;
			saveData->saveObj->objNum  = 1;
			*(CDBRgnGoodsGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
		}
		break;
	}
}

void CDBRgnGoodsGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID
	_AddToByteArray(&pBA, (WORD)m_mapGoodsGroup.size()); // 物品个数

	map<CGUID,CDBRgnGoods*>::iterator itr = m_mapGoodsGroup.begin();
	for(; itr != m_mapGoodsGroup.end(); itr++)
	{
		itr->second->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}
CDBRgnGoods* CDBRgnGoodsGroup::FindRgnGoods(const CGUID& guid)
{
	map<CGUID,CDBRgnGoods*>::iterator itr = m_mapGoodsGroup.find(guid);
	if(itr != m_mapGoodsGroup.end())
		return itr->second;
	return NULL;
}
void CDBRgnGoodsGroup::AddDBRgnGoods(CDBRgnGoods* goods)
{
	if(goods)
	{
		map<CGUID,CDBRgnGoods*>::iterator itr = m_mapGoodsGroup.find(goods->GetExID());
		if(itr != m_mapGoodsGroup.end())
		{
			delete itr->second;
			m_mapGoodsGroup.erase(itr);
		}
		m_mapGoodsGroup[goods->GetExID()] = goods;
	}
}
void CDBRgnGoodsGroup::Insert(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBRgnGoods* existGoods = FindRgnGoods(goodsID);
		if(NULL == existGoods)
		{
			existGoods = new CDBRgnGoods(goodsID);
			AddDBRgnGoods(existGoods);
		}
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
		}
	}
}


void CDBRgnGoodsGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();
	map<CGUID,CDBRgnGoods*>::iterator itr = m_mapGoodsGroup.begin();
	for(; itr != m_mapGoodsGroup.end(); itr++)
		SAFE_DELETE(itr->second);
	m_mapGoodsGroup.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBRgnGoods* existGoods = new CDBRgnGoods(goodsID);
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			AddDBRgnGoods(existGoods);
		}
	}
}

