


#include "stdafx.h"
#include "ServiceSCC.h"

#include "../App/AppClass/SmManage.h"
#include "../App/AppClass/PackManage.h"
#include "../../../SCMPublic/def/MsgGMT_SCC2SM.h"


#ifndef OUT_SERVICE
//! 执行输入指令
VOID ServiceSCC::RunCommand(VOID)
{
	switch(m_dwCommand)
	{
	case 1:
		{
			//! 关闭
			SM *pSM = SmManage::GetInstance().FindSM(1);
			if(NULL != pSM)
			{
				for (LONG i = 1; i < 7; ++i)
				{
					CMessage msgRequest(MSG_SCC2SM_CTRL_Request_SHUTDOWN);
					msgRequest.Add(i);
					msgRequest.SendToSm(pSM->GetSocket());
				}
			}
		}
		break;
	case 2:
		{
			//! 开启
			SM *pSM = SmManage::GetInstance().FindSM(1);
			if(NULL != pSM)
			{
				for (LONG i = 1; i < 7; ++i)
				{
					CMessage msgRequest(MSG_SCC2SM_CTRL_Request_START);
					msgRequest.Add(i);
					msgRequest.SendToSm(pSM->GetSocket());
				}
			}
		}
		break;
	case 3:
		{
			PackManage::GetInstance().RefreshFileCache();
			LogTools::OutTextToInterface("file update succeed!");
		}
		break;
	case 4: //! 更新所有服务器
		{
			LogTools::OutTextToInterface("update pack sending...");
			vector<tagUpdateInfo> vUpdateInfo;
			SmManage::GetInstance().GetNeedToHighestGapp(vUpdateInfo);

			map<DWORD, set<LONG>> mapGappTypeToSocket;
			for (LONG i = 0; i < vUpdateInfo.size(); ++i)
			{
				set<DWORD>::iterator ite = vUpdateInfo[i].setType.begin();
				for (; vUpdateInfo[i].setType.end() != ite; ++ite)
				{
					mapGappTypeToSocket[*ite].insert(vUpdateInfo[i].lSocket);
				}
			}

			for (map<DWORD, set<LONG>>::iterator ite = mapGappTypeToSocket.begin(); mapGappTypeToSocket.end() != ite; ++ite)
			{
				CMessage msg(MSG_SCC2SM_CTRL_Request_UpdateTypeGapp);
				DBWriteSet dbWriteSet;
				msg.GetDBWriteSet(dbWriteSet);
				dbWriteSet.AddToByteArray(ite->first);
				if(PackManage::GetInstance().FindAndGetHighestPack(ite->first, dbWriteSet))
				{
					for (set<LONG>::iterator iteSocket = ite->second.begin(); ite->second.end() != iteSocket; ++iteSocket)
					{
						msg.SendToSm(*iteSocket);
					}
				}
			}
			LogTools::OutTextToInterface("send succeed!");
		}
		break;
	case 5:
		{
			//PackManage::GetInstance().GetFile(2, "00.00.0002.zip");
			LogTools::OutTextToInterface("-----------------------------------SM info begin-----------------------------------");
			SmManage::GetInstance().ShowAllInfo();
			LogTools::OutTextToInterface("-----------------------------------SM info end-----------------------------------");
			
		}
		break;
	default:
		break;
	}

	m_dwCommand = 0;
}
//! 外部传入的指令编号
VOID ServiceSCC::InputCommand(DWORD dwCommand)
{
	m_dwCommand = dwCommand;
}
#endif