/********************************************************************
created:	2009/05/26
created:	26:5:2009   10:57
filename: 	e:\GM\guangmang\CLIENT\GameClient\AppClient\message\CheckCheatMessage.cpp
file path:	e:\GM\guangmang\CLIENT\GameClient\AppClient\message
file base:	CheckCheatMessage
file ext:	cpp
author:		zhaohang(kaba)

purpose:	防外挂客户端消息处理
*********************************************************************/
#include "stdafx.h"
#include "../../../Net/Message.h"
#include "../../../GameClient/Game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void OnCheckCheatMessage(CMessage* pMsg)
{

	//闪屏结构体定义
	FLASHWINFO fl;
	switch(pMsg->GetType())
	{
	case MSG_S2C_AC_QUESTION://答题图片消息
		//用闪屏来提示玩家答题直到窗口有焦
		if(GetForegroundWindow()!=g_hWnd)
		{
			fl.dwFlags=FLASHW_ALL|FLASHW_TIMERNOFG;
			fl.hwnd   = g_hWnd;
			fl.dwTimeout = 0;
			fl.uCount = INT_MAX;
			fl.cbSize = sizeof(fl);
			FlashWindowEx(&fl);
		}
		//答题界面没打开的情况下，显示精灵提示消息
		
		break;
	case MSG_S2C_AC_ANSWER_RET://答题信息反馈
		switch(pMsg->GetByte())
		{
		case 1://答题正确
			//当前频道显示提示
			
			break;
		case 2://答题错误
			//当前频道显示提示
			
			break;
		case 3://答题超时
			//当前频道显示提示
			
			break;
		}//switch(pMsg->GetDWORD)
		break;
	case MSG_S2C_AC_LOGINFAILED:		//疑似外挂值 过高。踢下线。。
		{	
			GetGame()->SetIsBeCheatForbid(true);
            string strInfo = AppFrame::GetText("Base_115");

		}
		break;
		
	}
}