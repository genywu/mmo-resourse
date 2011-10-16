#include "stdafx.h"
#include "../../net/Message.h"
#include "../../ServerMonitor/SM.h"
void OnLoginMessage(CMessage* pMsg)
{
	long lSocketID = pMsg->GetSocketID();
	switch( pMsg->GetType() )
	{
	case MSG_SCC2SM_LOGIN_Re_Login:
		{
			GetSM()->SendGappInfo();
#ifdef SEND_HARDWARE_INFO
			GetSM()->SendHardWareInfo();
#endif
			GetSM()->InitSccPrama(pMsg);
		//	GetSM()->CheckGappVersion();
		}
		break;
		//case MSG_SCC2SM_LOGIN_Request_Login:
		//	{
		//		//IP
		//		char szTmp[128] = {0};
		//		UCHAR arrIp[4] = {0};
		//		DWORD dwIP = pMsg->GetIP();
		//		memmove(arrIp, &dwIP, sizeof(LONG));
		//		sprintf(szTmp, "%u.%u.%u.%u",(ULONG)arrIp[0], (ULONG)arrIp[1], (ULONG)arrIp[2], (ULONG)arrIp[3]);

		//		//MD5密码
		//		char strPass[BASE_PASSWORD_SIZE];
		//		pMsg->GetEx(strPass, BASE_PASSWORD_SIZE);
		//		
		//		CSM::tagSCC* pSCC = GetSM()->GetSCC(szTmp,strPass);
		//		CMessage msg(MSG_SM2SCC_LOGIN_Re_Login);
		//		BYTE bRet = false;
		//		if (pSCC)
		//		{
		//			bRet = true;
		//			pSCC->bConnected = true;
		//		//	CMessage::MapSCCIDSocket(pSCC->dwIndex,lSocketID);
		//			LogTools::OutLogAndTxt("IP：%s,SCC连接成功",szTmp);
		//			msg.Add(bRet);
		//			msg.SendToSocket(lSocketID);
		//			/*GetSM()->SendGappInfo(lSocketID);
		//			GetSM()->SendHardWareInfo(lSocketID);
		//			GetSM()->CheckGappVersion();*/
		//		}
		//		else
		//		{
		//			msg.Add(bRet);
		//			msg.SendToSocket(lSocketID);
		//		}		
		//	}
		//	break;
	default:
		break;
	}
}