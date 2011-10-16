#include "stdafx.h"
#include "../DBServer/game.h"
#include "DBLinkmanGroup.h"
#include "DBentityManager.h"
#include "DBlinkman.h"
#include "DBLoadThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDBLinkmanGroup::CDBLinkmanGroup()
: CBaseDBEntity()
{
	GetGame()->GetDBEntityManager()->GenerateGUID(m_GUID);
}

CDBLinkmanGroup::CDBLinkmanGroup(const CGUID& id)
: CBaseDBEntity(id)
{
}
CDBLinkmanGroup::~CDBLinkmanGroup()
{
	set<CDBLinkMan*>::iterator itr = m_setLinkmanGroup.begin();
	for(; itr != m_setLinkmanGroup.end(); itr++)
	{
		SAFE_DELETE(*itr);
	}
	m_setLinkmanGroup.clear();
}

// virtual interfaces
void CDBLinkmanGroup::ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	switch(op)
	{
	case DB_OPER_SAVE:
		{
			// 组装消息
			SaveThreadData* saveData = new SaveThreadData(GetExID(), DB_LINKMANGROUP);
			// 需要记录一个时间戳,在存盘线程做
			Save(msgid, msgBuf, msgBufPos, bufSize);
			saveData->saveObj->objGUID = GetExID();
			saveData->saveObj->objType = DB_LINKMANGROUP;
			saveData->saveObj->objNum  = 1;
			*(CDBLinkmanGroup*)(saveData->dbObj) = *this;
			GetGame()->GetDBEntityManager()->PushSaveThreadData(saveData);
		}
		break;
	case DB_OPER_LOAD:
		{
			if(b_IsFirstCreated) // 是刚创建的对象,到数据库读取
			{
				LoadThreadData* loadData = new LoadThreadData(GetExID(), DB_LINKMANGROUP);
				loadData->loadObj->objGUID = GetExID();
				loadData->loadObj->objType = DB_LINKMANGROUP;
				loadData->loadObj->objNum  = 1;
				*(CDBLinkmanGroup*)(loadData->dbObj) = *this;
				GetGame()->GetDBEntityManager()->PushLoadThreadData(loadData);
			}
			else //不是刚创建的对象,直接返回内存数据
			{
				// 组装消息
				CMessage msg(MSG_DB2W_OPER_ENTITY);
				// opType		  4byte, 操作对象类型
				// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
				// DBOP Type:     1byte, 数据库操作类型（枚举值）
				// MSG ID:        4byte, 该消息的MSGID
				// Entity Num:	2byte, 该实体个数(X)
				msg.Add((long)DB_LINKMANGROUP);
				msg.Add((BYTE)0);
				msg.Add((BYTE)DB_OPER_LOAD);
				msg.Add(msgid);
				msg.Add((WORD)1);
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

void CDBLinkmanGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID
	_AddToByteArray(&pBA, GetOwnerGUID()); // ownerID
	_AddToByteArray(&pBA, (WORD)m_setLinkmanGroup.size()); // 物品个数

	set<CDBLinkMan*>::iterator itr = m_setLinkmanGroup.begin();
	for(; itr != m_setLinkmanGroup.end(); itr++)
	{
		(*itr)->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}


void CDBLinkmanGroup::Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{
	m_mapDirtyAttrData.clear();
	set<CDBLinkMan*>::iterator itr = m_setLinkmanGroup.begin();
	for(; itr != m_setLinkmanGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setLinkmanGroup.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 拥有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBLinkMan* existGoods = GetGame()->GetDBEntityManager()->FindDBLinkman(goodsID);
		if(NULL == existGoods)
		{
			existGoods = new CDBLinkMan(goodsID);
		}
		if(existGoods)
		{
			existGoods->Save(0, msgBuf, msgBufPos, bufSize);
			m_setLinkmanGroup.insert(existGoods);
		}
	}

	

}

