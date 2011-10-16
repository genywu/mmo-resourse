/*LinkmanMessage.cpp
*作者: 王峰
*创建日期:2008-2-14
*/
#include "stdafx.h"
#include "../Net/Message.h"

#include "Player.h"

// 处理 好友系统的消息
void OnLinkmanMessage( CMessage *pMsg )
{
	//CFriendsPageEx*	pFriendsPageEx = GetGame()->GetCGuiEx()->GetFriendsPageEx();

	switch( pMsg->GetType() ) 
	{
		// 初始化的联系人列表
		case MSG_S2C_LINKMAN_INIT:
			{
				//pFriendsPageEx->RecvInitMsg( pMsg );
			}
			break;

		// 添加联系人
		case MSG_S2C_LINKMAN_ADD:
			{
				//pFriendsPageEx->RecvAddMsg( pMsg );
			}
			break;

		// 删除联系人
		case MSG_S2C_LINKMAN_DEL:
			{
				//pFriendsPageEx->RecvDelMsg( pMsg );
			}
			break;

		// 移动联系人
		case MSG_S2C_LINKMAN_MOVE:
			{
				//pFriendsPageEx->RecvMoveMsg( pMsg );
			}
			break;

		// 请求添加协议
		case MSG_S2C_LINKMAN_REQUEST_ADD:
			{
				//pFriendsPageEx->RecvRequestMsg( pMsg );
			}
			break;

		// 上线
		case MSG_S2C_LINKMAN_LOGIN:
			{
				//pFriendsPageEx->RecvLoginMsg( pMsg );
			}
			break;

		// 下线
		case MSG_S2C_LINKMAN_LOGOUT:
			{
				//pFriendsPageEx->RecvLogoutMsg( pMsg );
			}
			break;

		// 联系人删除角色
		case MSG_S2C_LINKMAN_DELSTATE_CHANGE:
			{
				//pFriendsPageEx->RecvLinkmanDelMsg( pMsg );
			}
			break;

		// 联系人信息更新
		case MSG_S2C_LINKMAN_INFO_CHANGE:
			{
				//pFriendsPageEx->RecvChangeInfoMsg( pMsg );
			}
			break;

		// 留言
		case MSG_S2C_LINKMAN_LEAVE_WORD:
			{
				//pFriendsPageEx->RecvLeaveWordMsg( pMsg );
			}
			break;

		// 留言结果
		case MSG_S2C_LEAVEWORD_SAVE_RESULT:
			{
				//pFriendsPageEx->RecvLWResultMsg( pMsg );
			}
			break;

		// 其他联系人对自己的操作信息
		case MSG_S2C_LINKMAN_OTHER_HANDLE:
			{
				//pFriendsPageEx->RecvOtherHandleMsg( pMsg );
			}
			break;
	}
}