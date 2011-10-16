#include "stdafx.h"
#include "SkillLogicSys.h"
#include "../../../Net/Message.h"
#include "../Other/MsgSendTimeLimitMgr.h" //用于限制消息发送的管理类
#include "../../GameClient/Game.h"

void SkillLogicSys::Send_Set_Skill_HotKey(uint hotkeyIdex,uint skillID)
{
	MsgSendTimeLimitMgr& msl = GetInst(MsgSendTimeLimitMgr);
	if(msl.IsTimeOut(MSG_TYPE_SKILL_SET_HOTKEY) == false)
		return;
	CMessage msg(MSG_C2S_EXCHANGEHOTKEY);
	msg.Add((BYTE)hotkeyIdex);
	msg.Add((DWORD)(skillID|0x80000000));
	msg.Send((CClient*)GetGame()->GetNetClient());
	
	msl.UpdateLastSendTime(MSG_TYPE_SKILL_SET_HOTKEY);
	return;
}

