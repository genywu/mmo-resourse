#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBGoodsGroup.h"
#include "DBentityManager.h"
#include "DBGood.h"
#include "DBplayer.h"
#include "DBLoadThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBGoodsGroup::CDBGoodsGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBGoodsGroup::CDBGoodsGroup(const CGUID& id)
: CBaseDBEntity(id)
{
	m_mapGoodsGroup.clear();
}
CDBGoodsGroup::~CDBGoodsGroup()
{
	map<CGUID, CDBGoods*>::iterator itr = m_mapGoodsGroup.begin();
	for(; itr != m_mapGoodsGroup.end(); itr++)
	{
		SAFE_DELETE(itr->second);
	}
	m_mapGoodsGroup.clear();
}

// virtual interfaces
void CDBGoodsGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			// 组装消息
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_GOODSGROUP);
			long tmpPos = msgBufPos;
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_GOODSGROUP;
			saveData->saveObj->objNum  = 1;
			*(CDBGoodsGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
			if(b_IsFirstCreated) // 是刚创建的对象,到数据库读取
			{
				LoadThreadData* loadData = new LoadThreadData(GetExID(), DB_GOODSGROUP);
				loadData->loadObj->objGUID = GetExID();
				loadData->loadObj->objType = DB_GOODSGROUP;
				loadData->loadObj->objNum  = 1;
				*(CDBGoodsGroup*)(loadData->dbObj) = *this;
				GetGame()->GetDBEntityManager()->PushLoadThreadData(loadData);
			}
			else //不是刚创建的对象,直接返回内存数据
			{
				// 组装消息
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				msg.Add((long)DB_GOODSGROUP);	// opType		  4byte, 操作对象类型
				msg.Add((BYTE)0);
				msg.Add((BYTE)DB_OPER_LOAD);
				msg.Add(msgid);
				msg.Add((WORD)1);

				if(GetOwnerGUID() == NULL_GUID)
				{
					AddLogText("CDBGoodsGroup::ProcessMsg Owner ID is NULL!");
				}
				vector<BYTE> pBA;
				AddToByteArray(pBA);
				msg.Add((long)pBA.size());
				msg.AddEx(&pBA[0], pBA.size());
				msg.SendToSocket(GetGame()->GetWSSocket());
			}
		}
		break;
	}
}

void CDBGoodsGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, m_OwnerGUID); // 拥有者ID

	if(m_OwnerGUID == NULL_GUID)
	{
		AddLogText("CDBGoodsGroup::AddToByteArray() OnerID is NULL!");
	}

	// 添加该包所有物品的place和num信息
	// 是否有数据
	_AddToByteArray(&pBA, (long)GetGoodsGroupMap().size());
	map<CGUID, CDBGoods*>::iterator placeItr = GetGoodsGroupMap().begin();
	for(; placeItr != GetGoodsGroupMap().end(); placeItr++)
	{
		// 容器内物品数据
		placeItr->second->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}

void CDBGoodsGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	Clear();

	long tmpPos = msgBufPos;

	char szOwnerID[128];
	m_OwnerGUID.tostring(szOwnerID);

	// 是否有数据标志位
	long num = _GetLongFromByteArray(msgBuf, msgBufPos, bufSize);
	if(num)
	{
		for(int j=0; j<num; j++)
		{
			// 容器内物品数据
			CGUID goodsID;
			_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
			CDBGoods* existGoods = new CDBGoods(goodsID);
			if(existGoods)
				existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_mapGoodsGroup[goodsID] = existGoods;
		}
	}
}

void CDBGoodsGroup::Clear(void)
{
	map<CGUID, CDBGoods*>::iterator itr = m_mapGoodsGroup.begin();
	for(; itr != m_mapGoodsGroup.end(); itr++)
	{
		SAFE_DELETE(itr->second);
	}
	m_mapGoodsGroup.clear();
}

