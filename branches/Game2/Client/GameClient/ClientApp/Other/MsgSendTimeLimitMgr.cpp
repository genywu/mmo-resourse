#include "stdafx.h"
#include "MsgSendTimeLimitMgr.h"
#include "../../../../Public/Module/Tools.h"
//时间间隔
ulong MsgSendTimeLimitMgr::m_Interleave[MST_NUM] = {
	0,			//聊天框发送脚本
	2000,		//排行榜查询
	0,			//mainbar上设置技能热键
	0,			//任务
};

//上次发送时间
ulong MsgSendTimeLimitMgr::m_LastSendTime[MST_NUM] = {
	0,			//聊天框发送脚本
	0,			//排行榜查询
	0,			//mainbar上设置技能热键
	0,			//任务
	
};

bool MsgSendTimeLimitMgr::IsTimeOut(eMsgSendType e)
{
	if( GetCurTickCount() - m_LastSendTime[e] < m_Interleave[e])
	{
		MessageBox(g_hWnd,"请间隔一段时间后再发送","ERROR",MB_OK);
		return false;
	}
	else
		return true;
}

void MsgSendTimeLimitMgr::UpdateLastSendTime(eMsgSendType e)
{
	m_LastSendTime[e] = GetCurTickCount();
}

MsgSendTimeLimitMgr::MsgSendTimeLimitMgr()
{

}

MsgSendTimeLimitMgr::~MsgSendTimeLimitMgr()
{

}