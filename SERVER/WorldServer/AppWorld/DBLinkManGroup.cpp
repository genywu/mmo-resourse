#include "stdafx.h"
#include "../worldServer/game.h"
#include "DBLinkmanGroup.h"
#include "DBentityManager.h"
#include "DBlinkman.h"
#include "Linkman/LinkmanSystem.h"

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
	case DB_OPER_LOAD:
		Load(msgid, msgBuf, msgBufPos, bufSize);
		break;
	}
}

void CDBLinkmanGroup::AddToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, m_GUID); // 实体ID

	_AddToByteArray(&pBA, GetOwnerGUID());

	_AddToByteArray(&pBA, (WORD)m_setLinkmanGroup.size()); // 物品个数

	set<CDBLinkMan*>::iterator itr = m_setLinkmanGroup.begin();
	for(; itr != m_setLinkmanGroup.end(); itr++)
	{
		(*itr)->AddEntityDataToMsg(DB_OPER_LOAD, pBA);
	}
}


void CDBLinkmanGroup::Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize)
{ 
	m_mapDirtyAttrData.clear();
	set<CDBLinkMan*>::iterator itr = m_setLinkmanGroup.begin();
	for(; itr != m_setLinkmanGroup.end(); itr++)
		SAFE_DELETE(*itr);
	m_setLinkmanGroup.clear();

//	_GetBufferFromByteArray(msgBuf, msgBufPos, m_OwnerGUID, bufSize); // 所有者ID

	WORD goodsNum = _GetWordFromByteArray(msgBuf, msgBufPos, bufSize); // 物品个数

	for(int j=0; j<goodsNum; j++)
	{
		// 解析消息
		CGUID goodsID;
		_GetBufferFromByteArray(msgBuf, msgBufPos, goodsID, bufSize);
		CDBLinkMan* existGoods = GetGame()->GetDBEntityManager()->FindDBLinkman(goodsID);
		if(NULL == existGoods)
			existGoods = new CDBLinkMan(goodsID);
		if(existGoods)
		{
			existGoods->Load(0, msgBuf, msgBufPos);
			m_setLinkmanGroup.insert(existGoods);
		}
	}

	LinkmanSystem::GetInstance().GetPlayerLinkmanData_FromDBS(m_OwnerGUID, this);
}

