

#include "StdAfx.h"
#include "../Linkman/LinkmanSystem.h"


//! 响应世界服务器的联系人消息
extern void OnLinkmanMessage_ForWs(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{	
	case MSG_W2S_LINKMAN_PLAYER_INITDATA://! 初始化玩家联系人 
		{
			LinkmanSystem::GetInstance().OnInitMessage(pMsg);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_NOTIFY: //! WS转发的玩家请求添加联系人消息
		{
			//! 转发消息
			pMsg->GetLong();
			char szSenderName[MAX_PLAYER_NAME_SIZE + 1] = {0};
			pMsg->GetStr(szSenderName, MAX_PLAYER_NAME_SIZE);
			CGUID SenderGuid;
			pMsg->GetGUID(SenderGuid);
			CGUID ReceiverGuid;
			pMsg->GetGUID(ReceiverGuid);
			
			pMsg->SetType(MSG_S2C_LINKMAN_REQUEST_ADD);
			pMsg->SendToPlayer(ReceiverGuid);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE: //! WS对玩家添加联系人请求的回复
		{
			//! 转发消息
			pMsg->GetLong();
			pMsg->GetLong();
			char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
			pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);
			CGUID ReceiverGuid;
			pMsg->GetGUID(ReceiverGuid);

			pMsg->SetType(MSG_S2C_LINKMAN_ADD);
			pMsg->SendToPlayer(ReceiverGuid);
		}
		break;
	case MSG_W2S_LINKMAN_ADD_NOTIFY: //! 添加联系人通知
		{
			CGUID OwnerGuid;
			pMsg->GetGUID(OwnerGuid);
			char szOwnerName[MAX_PLAYER_NAME_SIZE + 1] = {0};
			pMsg->GetStr(szOwnerName, MAX_PLAYER_NAME_SIZE - 1);
			LONG lGroupID = pMsg->GetLong();
			tagLinkman_data Linkman_data;
			pMsg->GetEx(&Linkman_data, sizeof(tagLinkman_data));

			LinkmanSystem::GetInstance().OnAddLinkMan(OwnerGuid, szOwnerName, lGroupID, Linkman_data);
		}
		break;
	case MSG_W2S_LINKMAN_MOVE_NOTIFY: //! 移动联系人通知
		{
			BOOL bOptResult = pMsg->GetLong();
			if(bOptResult)
			{
				CGUID OwnerGuid;
				pMsg->GetGUID(OwnerGuid);
				char szOwnerName[MAX_PLAYER_NAME_SIZE + 1] = {0};
				pMsg->GetStr(szOwnerName, MAX_PLAYER_NAME_SIZE - 1);
				LONG lSrcGroupID = pMsg->GetLong();
				LONG lDesGroupID = pMsg->GetLong();
				tagLinkman_data Linkman_data;
				pMsg->GetEx(&Linkman_data, sizeof(tagLinkman_data));

				LinkmanSystem::GetInstance().OnMoveLinkMan(OwnerGuid, szOwnerName, lSrcGroupID, lDesGroupID, Linkman_data);
			}
			else
			{
				pMsg->GetLong();
				char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
				pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);
				CGUID ReceiverGuid;
				pMsg->GetGUID(ReceiverGuid);

				pMsg->SetType(MSG_S2C_LINKMAN_MOVE);
				pMsg->SendToPlayer(ReceiverGuid);
			}
		}
		break;
	case MSG_W2S_LINKMAN_DEL_NOTIFY: //! 删除联系人通知
		{
			CGUID OwnerGuid, AimGuid;
			pMsg->GetGUID(OwnerGuid);
			char szOwnerName[MAX_PLAYER_NAME_SIZE + 1] = {0};
			pMsg->GetStr(szOwnerName, MAX_PLAYER_NAME_SIZE);
			LONG lGroupID = pMsg->GetLong();
			pMsg->GetGUID(AimGuid);
			char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
			pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

			LinkmanSystem::GetInstance().OnDelLinkMan(OwnerGuid, szOwnerName, lGroupID, AimGuid, szAimName);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_RELEASEDATA: //! 清除玩家联系人数据通知
		{
			LinkmanSystem::GetInstance().OnReleaseMessage(pMsg);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_LOGIN:	//! 响应联系人登陆消息
		{
			LinkmanSystem::GetInstance().OnLinkmanLoginMessage(pMsg);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_LOGOUT:	//! 响应联系人离线消息
		{
			LinkmanSystem::GetInstance().OnLinkmanLogoutMessage(pMsg);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_INFO_CHANGE: //! 响应玩家信息改变
		{
			LinkmanSystem::GetInstance().OnPlayerInfoChange(pMsg);
		}
		break;
	case MSG_W2S_LINKMAN_PLAYER_DELSTATE_CHANGE: //! 响应玩家信息改变
		{
			LinkmanSystem::GetInstance().OnPlayerDelStateChange(pMsg);
		}
		break;
	case MSG_W2S_LEAVEWORD_SAVE_RESULT: //! 转发留言保存结果
		{
			LONG lResult = pMsg->GetLong();
			char szName[MAX_PLAYER_NAME_SIZE] = {0};
			pMsg->GetStr(szName, MAX_PLAYER_NAME_SIZE);
			CGUID guid;
			pMsg->GetGUID(guid);
	
			CMessage msg(MSG_S2C_LEAVEWORD_SAVE_RESULT);
			msg.Add(lResult);
			msg.Add(szName);
			msg.SendToPlayer(guid);
		}
		break;
	case MSG_W2S_LEAVEWORD_POP: //! 转发玩家留言获得
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			
			pMsg->SetType(MSG_S2C_LINKMAN_LEAVE_WORD);
			pMsg->SendToPlayer(PlayerGuid);
		}
		break;
	}
}


//! 响应客户端的联系人消息
extern void OnLinkmanMessage_ForClient(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{

	case MSG_C2S_LINKMAN_ADD: //! A 向 S 申请添加联系人 B
		{
			LinkmanSystem::GetInstance().OnRequestAdd(pMsg);
		}
		break;
	case MSG_C2S_LINKMAN_DEL: //! A 向 S 请求删除联系人 B
		{
			LinkmanSystem::GetInstance().OnRequestDel(pMsg);
		}
		break;
	case MSG_C2S_LINKMAN_MOVE: //! A 向 S 请求移动联系人 B
		{
			LinkmanSystem::GetInstance().OnRequestMove(pMsg);
		}
		break;
	case MSG_C2S_LINKMAN_RESPONSE_ADD: //! B 向 S 回应 A 的添加请求
		{
			LinkmanSystem::GetInstance().OnAnswerAdd(pMsg);
		}
		break;
	}
}