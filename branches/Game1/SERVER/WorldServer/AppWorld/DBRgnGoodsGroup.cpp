#include "stdafx.h"
#include "../worldServer/game.h"
#include "DBRgnGoodsGroup.h"
#include "DBentityManager.h"
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
	set<CDBRgnGoods*>::iterator itr = m_setGoodsGroup.begin();
	for(; itr != m_setGoodsGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setGoodsGroup.clear();
}

// virtual interfaces
void CDBRgnGoodsGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
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

void CDBRgnGoodsGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 所有者ID
	_AddToByteArray(&pBA, (WORD)m_setGoodsGroup.size()); // 物品个数

	set<CDBRgnGoods*>::iterator itr = m_setGoodsGroup.begin();
	for(; itr != m_setGoodsGroup.end(); itr++)
	{
		(*itr)->AddEntityDataToMsg(DB_OPER_SAVE, pBA);
	}
}

// 该接口由CDBEntityManager调用
void CDBRgnGoodsGroup::Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();
	set<CDBRgnGoods*>::iterator itr = m_setGoodsGroup.begin();
	for(; itr != m_setGoodsGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setGoodsGroup.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	CDBDupRgn* ownerRgn = NULL;
	CDBEntityManager::DBDupRgnMapItr rgnItr = GetGame()->GetDBEntityManager()->GetDBDupRgnMap().find(m_OwnerGUID);
	if(rgnItr != GetGame()->GetDBEntityManager()->GetDBDupRgnMap().end())
	{
		ownerRgn = rgnItr->second;
	}
	else
		return;

	assert( ownerRgn != NULL );

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		
		CDBRgnGoods* existGoods = new CDBRgnGoods(goodsID);
		if(existGoods)
		{
			existGoods->Load(0, msgBuf, msgBufPos, bufSize);
			AddOneRgnGoods(existGoods);
		}
	}
}

void CDBRgnGoodsGroup::AddOneRgnGoods(CDBRgnGoods* goods)
{
	set<CDBRgnGoods*>::iterator goodsItr = m_setGoodsGroup.begin();
	for(; goodsItr != m_setGoodsGroup.end(); goodsItr++ )
	{
		if((*goodsItr)->GetExID() == goods->GetExID())
		{
			SAFE_DELETE(*goodsItr);
			goodsItr = m_setGoodsGroup.erase(goodsItr);
			m_setGoodsGroup.insert(goods);
			return;
		}
	}
	
	m_setGoodsGroup.insert(goods);
}
void CDBRgnGoodsGroup::DelOneRgnGoods(const CGUID& guid)
{
	set<CDBRgnGoods*>::iterator goodsItr = m_setGoodsGroup.begin();
	for(; goodsItr != m_setGoodsGroup.end(); goodsItr++ )
	{
		if((*goodsItr)->GetExID() == guid)
		{
			SAFE_DELETE(*goodsItr);
			m_setGoodsGroup.erase(goodsItr);
			break;
		}
	}
}
