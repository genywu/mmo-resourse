/*
处理SCC控制消息
*/
#include "stdafx.h"
#include "../SMDef.h"
#include "../../net/Message.h"

void OnCtrlMessage(CMessage* pMsg)
{
	DWORD lSocketID = pMsg->GetSocketID();
	switch(pMsg->GetType())
	{
	case MSG_SCC2SM_CTRL_Request_START:
		{
			long lGappID = pMsg->GetLong();
			CHECKGAPP(lGappID);
			pGapp->Start();
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP41"),lGappID);
		}
		break;
	case MSG_SCC2SM_CTRL_Request_SHUTDOWN:
		{
			long lGappID = pMsg->GetLong();
			CHECKGAPP(lGappID);				
			pGapp->ShutDown();
			LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP42"),lGappID);
		}
		break;
	case MSG_SCC2SM_CTRL_Request_UpdateGapp:
		{
			long lGappID = pMsg->GetLong();
			CHECKGAPP(lGappID);
			BYTE b=pMsg->GetByte();
			//更新到最新版本
			pGapp->SetIsLatest(b);
			pGapp->CheckVersion();
		}
		break;
	case MSG_SCC2SM_CTRL_Request_UpdateTypeGapp:
		{
			DBReadSet dbReadSet;
			pMsg->GetDBReadSet(dbReadSet);
			long lType = dbReadSet.GetLongFromByteArray();
			GetSM()->UpdateGappByType(lType,dbReadSet);
	
		}
		break;
	case MSG_SCC2SM_CTRL_Request_RESTARTCOM:
		{
			ExitWindowsEx(EWX_REBOOT,0);
		}
		break;
		//更新相关
	case MSG_SCC2SM_CTRL_Re_File:
		{
			BYTE bFile = pMsg->GetByte();	
			long lGappID = pMsg->GetLong();
			long lType = pMsg->GetLong();
			CHECKGAPP(lGappID);
			if (!bFile)
			{
				//更新过程中获取文件失败，则结束更新流程
				pGapp->SetIsLatest(0);
				char filename[BASE_NAME_SIZE]="";
				pMsg->GetStr(filename,BASE_NAME_SIZE);
				GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP43"),lGappID,pGapp->GetDisplayName(),filename);
				break;
			}
			DBReadSet dbreadset;
			pMsg->GetDBReadSet(dbreadset);
			//解析文件
			pGapp->GetFile(dbreadset); 
			
		}
		break;
	case MSG_SCC2SM_CTRL_Re_ServerPack:
		{		
			DBReadSet dbreadset;
			pMsg->GetDBReadSet(dbreadset);
			
			long lGappID = dbreadset.GetLongFromByteArray();
			long lType = dbreadset.GetLongFromByteArray();
			BYTE bFile = dbreadset.GetByteFromByteArray();	
			CHECKGAPP(lGappID);
			if (!bFile)
			{
				LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP44"),lGappID);
				break;
			}
			pGapp->WritePackRes(dbreadset);
		}
		break;
	case MSG_SCC2SM_CTRL_Request_GappAction:
		{
			long SMID=pMsg->GetLong();
			long lGappID = pMsg->GetLong();
			CHECKGAPP(lGappID);
			long actionID=pMsg->GetLong();	
			/*LogTools::OutLogAndTxt(AppFrame::GetText("IDS_SM_TIP45"),
				SMID,lGappID,pGapp->GetName(),actionID);*/
			if (pGapp->GetState()==eSS_Busy)
			{
				GetSM()->SendError(AppFrame::GetText("IDS_SM_TIP88"),pGapp->GetDisplayName());
				break;
			}
			pMsg->SetType(MSG_SM2GAPP_ACTION);
			pMsg->SendToGapp(lGappID);
			
		}
		break;
	case MSG_SCC2SM_CTRL_Request_SMAction:
		{
			long lSMID=pMsg->GetLong();
			long lGappID=pMsg->GetLong();
			switch(pMsg->GetLong())
			{
			case SETPORT:
				{
					long lport=pMsg->GetLong();
					CHECKGAPP(lGappID);
					if (pGapp->GetType()==eGT_LS)
					{
						bool ret= pGapp->SetLSPort(lport);
						CMessage msg(MSG_SM2SCC_CTRL_Notify_SMAction);
						msg.Add(lSMID);
						msg.Add(lGappID);
						msg.Add((long)SETPORT);
						if(ret) 
						{	
							msg.Add((BYTE)1);
						}
						else
							msg.Add((BYTE)0);
						msg.SendToAllSCC();
					}
				}
				break;
			default:
				break;
			}
		}
	default:
		break;
	}
}