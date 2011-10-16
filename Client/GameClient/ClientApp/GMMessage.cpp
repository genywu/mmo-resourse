#include "stdafx.h"
#include "..\nets\netclient\myNetClient.h"
#include "..\nets\netclient\message.h"
#include "gameclient\game.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应其他消息
void OnGMMessage( CMessage* pMsg )
{
	if( pMsg == NULL ) return;

	switch( pMsg -> GetType() )
	{
	case CMessage::MSG_S2C_GM_ANSWER_GMMODE:
		/**## 服务器返回客户端请求GM模式是否成功 */
		/**## 消息格式：{ MSG_S2C_GM_ANSWER_GMMODE, LONG GMLevel } */
		/**## GMLevel的定义参见GMList.h */
		{
			long lLevel = pMsg -> GetLong();
			if ( lLevel > 0 )
			{
				GetGame() -> SetGMMode();
			}
		}
		break;
	}
}