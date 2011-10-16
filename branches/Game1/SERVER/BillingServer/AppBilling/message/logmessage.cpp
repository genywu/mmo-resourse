#include "stdafx.h"

#include "..\..\..\..\nets\netbilling\message.h"
#include "..\..\..\..\dbaccess\billingdb\rsaccount.h"
#include "../../../../public/wordsfilter.h"
#include "../../../../public/tools.h"
#include "..\..\..\..\dbaccess\billingdb\rslogInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应LOG消息
void OnLogMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
		case MSG_C2L_LOG_QUESTCDKEY://客户CDKEY验证
			{
			}
			break;

		case MSG_C2L_LOG_QUEST_PLAYERBASELIST://客户请求自己的基本信息
			{
			}
			break;

		case MSG_W2L_LOG_ANSWER_PLAYERBASELIST://worldserver返回一个账号的玩家基本信息列表
			{				
			}
			break;

		case MSG_C2L_LOG_QUEST_PLAYERDATA://客户请求自己的详细信息 -> 进入游戏
			{
			}
			break;

		case MSG_W2L_LOG_ANSWER_PLAYERDATA://worldserver返回客户的详细信息是否成功取出
			{
			}
			break;

		case MSG_W2L_LOG_PLAYERQUIT://worldserver发送退出玩家的cdkey
			{			
			}
			break;

		case MSG_W2L_LOG_DELETEROLE://worldserver返回一个角色删除成功与否
			{
			}
			break;

		case MSG_W2L_LOG_RESTOREROLE:  
			{
			}
			break;
		case MSG_W2L_LOG_CREATEROLE://worldserver返回一个角色创建成功与否
			{
			}
			break;
		case MSG_C2L_LOG_QUESTEXIT:
			{
			}
			break;

		// 发送帐号信息给客户端
		case MSG_W2L_LOG_SENDACCOUNTINFO:
			{
			}
			break;
	}
}