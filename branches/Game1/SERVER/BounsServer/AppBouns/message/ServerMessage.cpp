#include "stdafx.h"
#include "..\GetBounsInfoOper.h"
#include "..\FreezeBounsOper.h"
#include "..\GetBounsOper.h"
#include "..\BounsRollBackOper.h"

extern long g_lLoadMsgNum;					// 请求查询的消息数量
extern long g_lFreezeMsgNum;				// 请求冻结的消息数量
extern long g_lObtainMsgNum;				// 获取奖励的消息数量
extern bool g_bDealMsgState;

void OnServerMessage(CMessage* pMsg)
{
	if(pMsg == NULL)
	{
		return;
	}

	switch(pMsg->GetType())
	{
	case MSG_S2BDS_SENDSERVERINFO:
		{
			char strIP[256];
			pMsg->GetStr(strIP, 256);
			DWORD dwGSNum  = pMsg->GetDWord();
			long lSocketID = pMsg->GetSocketID();

			CGame::tagClientInfo* pInfo = GetGame()->GetGameServer(strIP, dwGSNum);
			if(pInfo)
			{
				CMessage::MapGSIDSocket(pInfo->dwIndex, lSocketID);
				long lPos = GetGame()->AddGameServer(pInfo->dwIndex, strIP);
				AddLogText("GameServer [%s][%d] 建立连接!", strIP, dwGSNum);

				// 发送奖励描述资源
				map<DWORD, tagBounsDes> *pMapBounsDes = GetGame()->GetBounsDes();
				if(pMapBounsDes)
				{
					CMessage msg(MSG_BDS2S_BOUNS_DES_UPDATE);
					msg.Add((BYTE)1); //全部发送
					DBWriteSet setWriteDB;
					msg.GetDBWriteSet(setWriteDB);
					setWriteDB.AddToByteArray((long)pMapBounsDes->size());
					for(map<DWORD, tagBounsDes>::iterator itor = pMapBounsDes->begin(); itor != pMapBounsDes->end(); ++itor)
					{
						(*itor).second.EncodeToDataBlock(setWriteDB);
					}
					msg.SendToSocket(lSocketID);
				}
			}
			else
			{
				GetGame()->GetNetServer()->DisConn(lSocketID);
				AddLogText("GameServer [%s] 配置参数不匹配,连接被断开!", strIP);
			}

		}
		break;

		// 获取领奖信息
	case MSG_S2BDS_BOUNS_LOAD:
		{
			// 处理奖励消息状态关闭,停止处理
			if(!g_bDealMsgState)
				return;

			char szAccount[256];
			pMsg->GetStr(szAccount, 256);
			CGUID guid;
			pMsg->GetGUID(guid);
			DWORD dwServerID = pMsg->GetDWord();
			CGetBounsInfoOper *pOper = new CGetBounsInfoOper(OT_BOUNS_INFO);
			if(pOper)
			{
				pOper->SetGSSocketID(pMsg->GetSocketID());
				pOper->SetAccount(szAccount);
				pOper->SetPlayerID(guid);
				pOper->SetAreaServerID(dwServerID);
				GetGame()->GetBounsDBManager()->AddOper(pOper);
				g_lLoadMsgNum++;
				RefeashInfoText();	
			}
		}
		break;

		// 领奖
	case MSG_S2BDS_BOUNS_GET:
		{	
			CGUID guidPlayer;
			pMsg->GetGUID(guidPlayer);
			BYTE bOper = pMsg->GetByte();
			if( bOper == 1 )			// 领取奖励，记录日志
			{
				CGetBounsOper *pOper = new CGetBounsOper(OT_BOUNS_GET);
				if(pOper)
				{
					CGUID guidBouns;
					char szPlayerName[128], szIP[128];
					DWORD dwWSID;
					pMsg->GetGUID(guidBouns);
					pMsg->GetStr(szPlayerName, 128);
					pMsg->GetStr(szIP, 128);
					dwWSID = pMsg->GetDWord();
					pOper->SetPlayerID(guidPlayer);
					pOper->SetBounsGUID(guidBouns);
					pOper->SetPlayerName(szPlayerName);
					pOper->SetPlayerIP(szIP);
					pOper->SetWSID(dwWSID);

					pOper->SetGSSocketID(pMsg->GetSocketID());

					GetGame()->GetBounsDBManager()->AddOper(pOper);
					g_lObtainMsgNum++;
					RefeashInfoText();	

				}
			}
			else if( bOper == 2 )		// 冻结奖励
			{
				// 处理奖励消息状态关闭,停止处理
				if(!g_bDealMsgState)
					return;

				CFreezeBounsOper *pOper = new CFreezeBounsOper(OT_BOUNS_FREEZE);
				if(pOper)
				{	
					char szIP[128];
					pMsg->GetStr(szIP, 128);
					DBReadSet setReadDB;
					pMsg->GetDBReadSet(setReadDB);
					pOper->GetBounsInfo()->DecodeFromDataBlock(setReadDB);
					pOper->SetGSSocketID(pMsg->GetSocketID());
					pOper->SetPlayerID(guidPlayer);
					pOper->SetPlayerIP(szIP);

					GetGame()->GetBounsDBManager()->AddOper(pOper);
					g_lFreezeMsgNum++;
					RefeashInfoText();	
				}
			}
		}
		break;

		// 领奖失败回滚
	case MSG_S2BDS_BOUNS_ROLLBACK:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			CBounsRollBackOper *pOper = new CBounsRollBackOper(OT_BOUNS_GET_ROLLBACK);
			if(pOper)
			{
				pOper->SetGSSocketID(pMsg->GetSocketID());
				GetGame()->GetBounsDBManager()->AddOper(pOper);
			}
		}
        break;

        // 上传物品数据
    case MSG_S2BDS_UPLOAD_GOODS:
        {
            tagUploadGoods stUploads;
            DBReadSet setReadDB;
            pMsg->GetDBReadSet(setReadDB);
            stUploads.DecodeFromDataBlock(setReadDB);
            CUploadGoods* pUpload = new CUploadGoods(stUploads);
            if(pUpload)
            {
                pUpload->BeginUpload();
            }
        }
        break;

	}
}