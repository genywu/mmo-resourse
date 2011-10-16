
#include "StdAfx.h"
#include "DbLinkmanManage.h"
#include "DBentityManager.h"
#include "DBAccessThread.h"

#include "../DBServer/game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DbLinkmanManage g_DbLinkmanManage;

DbLinkmanManage::DbLinkmanManage(void)
{
	m_vData.reserve(512);
}
DbLinkmanManage::~DbLinkmanManage(void)
{
}

DbLinkmanManage& DbLinkmanManage::GetDbLinkmanManage(void)
{
	return g_DbLinkmanManage;
}

//! 发送一个玩家的联系人数据到WS
void DbLinkmanManage::InitLinkmanDataToWs(const CGUID& PlayerGuid)
{
	_ConnectionPtr& cn = GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr();
	RsLinkmanManage::GetRsLinkmanManage().LoadPlayerInitData_FromDB(PlayerGuid, cn, m_vData);

	CMessage msg(MSG_DB2W_LINKMAN_RE_PLAYER_DATA);
	msg.Add(PlayerGuid);
	msg.Add((LONG)m_vData.size());
	for (long i = 0; i < m_vData.size(); ++i)
	{
		msg.AddEx(&(m_vData[i]), sizeof(tagLinkman_DB));
	}

	msg.SendToSocket(GetGame()->GetWSSocket());
}

//! 响应WS的数据写入消息
void DbLinkmanManage::OnLinkmanWriteData (CMessage *pMsg)
{
	_ConnectionPtr& cn = GetGame()->GetDBEntityManager()->GetAccessThread()->GetConnPtr();

	LONG lNum = pMsg->GetLong();
	for (LONG i = 0; i < lNum; ++i)
	{
		tagLinkman_DB tmpLinkman_DB;

		LONG lAddNum = pMsg->GetLong();
		for (LONG j = 0; j < lAddNum; ++j)
		{
			pMsg->GetEx(&tmpLinkman_DB, sizeof(tagLinkman_DB));
			RsLinkmanManage::GetRsLinkmanManage().SaveLinkman(tmpLinkman_DB, cn);
		}

		LONG lDelNum = pMsg->GetLong();
		for (LONG j = 0; j < lDelNum; ++j)
		{
			pMsg->GetEx(&tmpLinkman_DB, sizeof(tagLinkman_DB));
			RsLinkmanManage::GetRsLinkmanManage().DeleteLinkman(tmpLinkman_DB, cn);
		}
	}
}