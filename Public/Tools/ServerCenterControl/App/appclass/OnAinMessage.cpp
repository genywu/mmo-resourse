
#include "stdafx.h"

#include "UserManage.h"
#include "../Setup/UserSetup.h"
#include "HardInfoList.h"
#include "SmManage.h"
#include "GmSystem.h"

#pragma warning(disable:4312)

void OnAinMessage(AinMessage* pAimMsg)
{
	switch(pAimMsg->GetType())
	{
	case USER_PWD_CMP_RESULT:
		{
			LONG lRunResult = pAimMsg->GetDword();

			char szUserName[MAX_USER_NAME_SZIE] = {0};
			pAimMsg->GetStr(szUserName, MAX_USER_NAME_SZIE);

			char szPwd[BASE_PASSWORD_SIZE] = {0};
			pAimMsg->GetStr(szPwd, BASE_PASSWORD_SIZE);

			UserSetup::GetInstance().OnUserPwdCmp(lRunResult, szUserName, szPwd);

			//! 全部验证完了，打开用户访问开关
			if(UserSetup::GetInstance().IsFinishCmp())
				CMessage::AllowSUC(TRUE);
		}
		break;
	case USER_PWD_CHANGE_RESULT:
		{
			LONG lChangeResult = pAimMsg->GetDword();

			char szUserName[MAX_USER_NAME_SZIE] = {0};
			pAimMsg->GetStr(szUserName, MAX_USER_NAME_SZIE);

			char szMD5Pwd[BASE_PASSWORD_SIZE] = {0};
			pAimMsg->GetStr(szMD5Pwd, BASE_PASSWORD_SIZE);

			CMessage msg(MSG_SCC2SUC_LOGIN_Re_ChangePwd);
            Json::Value var;

			if(1 == lChangeResult)
			{	
				UserSetup::GetInstance().ChangePwd(szUserName, szMD5Pwd);
				/*msg.Add((DWORD)TRUE);
				msg.Add((DWORD)eULE_NoError);*/
                var["result"] = false;
                var["errCode"] = eULE_NoError;

				ActionLogManage::GetInstance().ActionLog(-1, -1, szUserName, "[%s] change password for self.", szUserName);
			}
			else
			{
				/*msg.Add((DWORD)FALSE);
				msg.Add((DWORD)eULE_SystemErr);*/
                var["result"] = false;
                var["errCode"] = eULE_SystemErr;
			}
            msg.AddJson(var);
			
			UserManage::GetInstance().SendMsgToUser(szUserName, msg);
			
		}
		break;
	case WAITE_HARDWARE_FINISH:
		{
			BOOL			bExecuteOK		= (BOOL)pAimMsg->GetDword();
			tagHardwareInfo *pHardwareInfo	= (tagHardwareInfo*)pAimMsg->GetDword();
			DWORD			dwNum			= pAimMsg->GetDword();
			if(bExecuteOK)
			{
				HardInfoList::OnDBWriteFinish(pHardwareInfo, dwNum);
			}
			else
			{
				
				DWORD dwSMID	= pAimMsg->GetDword();
				HardInfoList::OnDBWriteFinish(pHardwareInfo, dwNum, dwSMID);
			}
		}
		break;
	case WAITE_DBLOG_FINISH:
		{
			BOOL		bExecuteOK	= (BOOL)pAimMsg->GetDword();
			tagDbLog	*pDbLog		= (tagDbLog*)pAimMsg->GetDword();
			DWORD		dwNum		= pAimMsg->GetDword();
			ActionLogManage::OnDBWriteFinish(pDbLog, dwNum, bExecuteOK);
		}
		break;
	case WAITE_GAME_INFO_FINISH:
		{
			BOOL bExecuteOK					= (BOOL)pAimMsg->GetDword();
			tagDbGameInfo	*pDbGameInfo	= (tagDbGameInfo*)pAimMsg->GetDword();
			DWORD dwNum						= pAimMsg->GetDword();
			Gapp::OnDBWriteFinish(pDbGameInfo, dwNum, bExecuteOK);
		}
		break;
	case LOAD_GAME_INFO_FINISH:
		{
			BOOL bExecuteOK					= (BOOL)pAimMsg->GetDword();
			LONG lUserID					= pAimMsg->GetDword();
			tagDbGameInfo *pDbGameInfo		= (tagDbGameInfo*)pAimMsg->GetDword();
			DWORD dwSMID					= pAimMsg->GetDword();
			DWORD dwGappID					= pAimMsg->GetDword();
			DWORD dwInfoType				= pAimMsg->GetDword();

			if(NULL != pDbGameInfo)
				M_FREE(pDbGameInfo, sizeof(tagDbGameInfo) * MAX_GAME_INFO_NUM);

			CMessage msg(MSG_SCC2SUC_INFO_Re_Show);
			/*DBWriteSet dbWriteSet;
			msg.GetDBWriteSet(dbWriteSet);

			dbWriteSet.AddToByteArray((DWORD)bExecuteOK);
			dbWriteSet.AddToByteArray(dwSMID);
			dbWriteSet.AddToByteArray(dwGappID);
			dbWriteSet.AddToByteArray(dwInfoType);
			if(bExecuteOK)
			{		
				DWORD dwDataSize = pAimMsg->GetDword();
				if(0 != dwDataSize)
				{
					vector<BYTE> vData(dwDataSize);
					pAimMsg->GetEx(&vData[0], dwDataSize);

					dbWriteSet.AddToByteArray(dwDataSize);
					dbWriteSet.AddToByteArray(&vData[0], dwDataSize);
				}
			}*/
            Json::Value var;
            var["result"] = bExecuteOK;
            var["smID"] = (int)dwSMID;
            var["gappID"] = (int)dwGappID;
            var["infoType"] = (int)dwInfoType;
            if(bExecuteOK)
            {  
                DWORD dwDataSize = pAimMsg->GetDword();
                if(dwDataSize)
                {
                    vector<BYTE> vData(dwDataSize);
                    var["info"] = pAimMsg->GetEx(&vData[0], dwDataSize);
                }
            }

            msg.AddJson(var);
			msg.SendToSuc(lUserID);
		}
		break;
	case RETURN_REPORT_DATA:
		{
			const DataCache<tagReportInfo>::tagDataBlock *pDataBlock = (const DataCache<tagReportInfo>::tagDataBlock*)pAimMsg->GetDword();
			GmSystem::GetInstance().GetReportDbManage().OnSaveReportFinish(pDataBlock);
		}
		break;
	case SQL_FINISH:
		{
			const char *pSql = (const char*)pAimMsg->GetDword();
			DWORD SqlSize = pAimMsg->GetDword();

			M_FREE(pSql, SqlSize);
		}
		break;
	case DEFER_:
		{
			LogTools::OutErrLog("error aim msg type：%d", pAimMsg->GetType());
		}
		break;
	}
}