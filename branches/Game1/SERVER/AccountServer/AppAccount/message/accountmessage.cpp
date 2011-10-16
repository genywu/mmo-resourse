#include "stdafx.h"
#include "..\ValidateOperator.h"
#include "..\OtherOperator.h"


//! ÏìÓ¦ÕËºÅ²Ù×÷ÏûÏ¢
void OnAccountMessage(CMessage* pMsg)		
{
	switch(pMsg->GetType())
	{
	case MSG_O2A_ACCOUNT_Validate_Request://! ÑéÖ¤ÕËºÅÃÜÂë
		{
			tagAccountInfo_ToAS AccountInfo_ToAS;
			pMsg->GetEx(&AccountInfo_ToAS, sizeof(tagAccountInfo_ToAS));

			DWORD dwLSID = GetGame()->GetClientManager().ClientAbleWork(pMsg->GetSocketID());
			if(0 == dwLSID)
			{
				tagAccountInfo_FromAS AccountInfo_FromAS(AccountInfo_ToAS);
				AccountInfo_FromAS._LoginRerult._dwLoginResult = ePLR_Lost;
				AccountInfo_FromAS._LoginRerult._InfoCode = LOGIN_WAIT_INIT_FINISH;

				CMessage msg(MSG_A2O_ACCOUNT_Validate_Re);
				msg.AddEx(&AccountInfo_FromAS, sizeof(tagAccountInfo_FromAS));
				msg.SendToClient(pMsg->GetSocketID());
				return;
			}
			
			ValidateOperator *pValidateOperator = ValidateOperator::CreateValidateOperator(AccountInfo_ToAS, pMsg->GetSocketID(), dwLSID);
			
			assert(NULL != pValidateOperator);
			if(NULL == pValidateOperator)
			{

				return;
			}
			if(!pValidateOperator->BeginValidate())
			{
				pValidateOperator->SendResult();
				ValidateOperator::ReleasValidateOperator(&pValidateOperator);
			}
		}
		break;
#ifdef ENABLE_FCM
	case MSG_O2A_ACCOUNT_Logout:
		{
			char szCdkey[CDKEY_SIZE] = {0};
			pMsg->GetStr(szCdkey,CDKEY_SIZE);

			DWORD dwLSID = GetGame()->GetClientManager().ClientAbleWork(pMsg->GetSocketID());
			if (0 == dwLSID)
				return;

			OtherOperator *pOtherOperator =	OtherOperator::CreateLoginAndLogout(szCdkey, dwLSID, eLALT_Logout);
			if(NULL != pOtherOperator)
				pOtherOperator->LoginAndLogout();
		}
		break;
	case MSG_O2A_ACCOUNT_ListLogout:
		{
			char szCdkey[CDKEY_SIZE] = {0};
			LONG lNum = pMsg->GetLong();

			DWORD dwLSID = GetGame()->GetClientManager().ClientAbleWork(pMsg->GetSocketID());
			if (0 == dwLSID)
				return;

			for (LONG i = 0; i < lNum; ++i)
			{
				pMsg->GetStr(szCdkey,CDKEY_SIZE);
				OtherOperator *pOtherOperator =	OtherOperator::CreateLoginAndLogout(szCdkey, dwLSID, eLALT_Logout);
				if(NULL != pOtherOperator)
					pOtherOperator->LoginAndLogout();
			}
		}
		break;
#endif
	case MSG_O2A_ACCOUNT_Reject_Request://! ·âºÅ
		{
			char szCdkey[256] = {0};
			pMsg->GetStr(szCdkey,256);
			long lEndTime = pMsg->GetLong();

			OtherOperator *pOtherOperator = OtherOperator::CreateRejectOperator(szCdkey, lEndTime);
			if(NULL != pOtherOperator)
			{
				pOtherOperator->RejectAccount();
			}
		}
		break;
	case MSG_O2A_SERVER_RUN_ERROR:
		{
			DWORD dwClientNo = GetGame()->GetClientManager().ClientAbleWork(pMsg->GetSocketID());
			if(0 != dwClientNo)
			{
				AddErrorLogText("Game world[%d] runtime error, please check each members!!", dwClientNo);
			}
		}
		break;
	}
}
