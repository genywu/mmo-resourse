/**
* @filename:QuestMessage.cpp
* @date: 2010/6/30
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 将任务消息从原来的OrganizingsystemMessage.cpp中独立出来
*/
#include "stdafx.h"
#include "Message.h"
#include "../Player.h"
#include "QuestSystem/QuestSystem.h"
#include "Other/AudioList.h"
#include "Other/DeputyOccuSystemClient.h"
#include "../GameClient/Game.h"     
#include "../MsgBoxSys/MsgEventDef.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../../Public/Common/QuestDef.h"

using namespace QS;
void OnQuestMessage(CMessage* pMsg)
{
	int OpType = (int)pMsg->GetByte();
	switch(OpType)
	{
	case QS_ADD:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			long lStep = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				pMainPlayer->AddQuest((long)dwQuestID,lStep);
			} 
			if(!GetGame()->GetAudioList()->IsPlaying("SOUNDS\\quest\\q-03.wav",-1))  //当完成音效未完成时，不播放接受音效
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-01.wav");
			}
			CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(dwQuestID,true);
		}
		break;
	case QS_REMOVE:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(dwQuestID,false);
				pMainPlayer->RemoveQuest((long)dwQuestID);
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-02.wav");
			}
		}
		break;
	case QS_COMPLETE:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				const CQuest *pQuest = CQuestSystem::GetQuest((long)dwQuestID);
				// 属于主线任务，更新任务界面主线任务日志
				if(pQuest && pQuest->GetAttribute().GetQuestType()==QUEST_MAIN)
				{
					pMainPlayer->GetRoleQueue()->AddFinishedQuestRecord((long)dwQuestID);
					long lChapter = pQuest->GetAttribute().GetChapterIndex();
					if(lChapter > 0)
						pMainPlayer->GetRoleQueue()->SetChapter(lChapter);
				}	

				CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(dwQuestID,false);

				pMainPlayer->RemoveQuest((long)dwQuestID);
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-03.wav");
			}
		}
		break;
	case QS_UPDATE_STEP:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			long lStep = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				pMainPlayer->UpdataQuestStep((long)dwQuestID,lStep);
			}
		}
		break;
		//更新一个任务的所有变量
	case QS_UPDATE_PARAM:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			long lIndex = pMsg->GetLong();
			long lValue = pMsg->GetLong();
			long lRegionID = pMsg->GetLong();
			long lRegionX = pMsg->GetLong();
			long lRegionY = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				pMainPlayer->UpdataQuestTarget((long)dwQuestID,lIndex,lValue,lRegionID,lRegionX,lRegionY);
			}	
		}
		break;
	case QS_UPDATE_DATA:
		{
			DWORD dwQuestID = pMsg->GetDWord();
			long lIndex = pMsg->GetLong();
			long lValue = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				pMainPlayer->UpdataQuestTarget((long)dwQuestID,lIndex,lValue);
			}
		}
		break;
		//更新添加一个任务记录(一般是主线任务)
	case QS_UPDATE_RECORD:
		{
			long lQuestID = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			// 为玩家添加一条已经完成的任务
			if(pMainPlayer)
				pMainPlayer->GetRoleQueue()->AddFinishedQuestRecord(lQuestID);
		}
		break;
	case QS_UPDATE_MER:
		{
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			// 为玩家添加一条已经完成的任务
			if(NULL == pMainPlayer)	
				break;
			long lCurMercQuestID = pMsg->GetLong();  //获取当前任务ID
			long lNum =  pMsg->GetLong();  //获得放弃任务数

			for(int i=0;i<lNum;i++)
			{
				long lDelQuestID =  pMsg->GetLong();  //获得放弃的任务
			}
			long lType = pMsg->GetLong();
			pMainPlayer->GetMercQuestSys()->SetCurQuestID(lCurMercQuestID);
			if(lType==0)
			{
				pMainPlayer->GetMercQuestSys()->SetCurQuestID(lCurMercQuestID);
			}
			else if(lType==1)
			{
				long lListID = pMsg->GetLong();           //获取任务列表ID 
				long dwLastUpdateTime = pMsg->GetDWord(); //获取任务更新剩余时间
				if(dwLastUpdateTime<0)    dwLastUpdateTime = 0;
				if(lListID != 0)
				{
					long lSize = pMsg->GetWord();
					CMercenaryQuestSys::tagMerQuest MerQuests;
					MerQuests.dwLastUpdateTime=dwLastUpdateTime;
					MerQuests.Quests.clear();
					MerQuests.dwLastUpdateTime = dwLastUpdateTime;
					for(int i=0;i<lSize;i++)
					{
						long lID = pMsg->GetLong();
						long lIsAccept = pMsg->GetLong();  //该任务是否曾做过
						if(lID!=0)
							MerQuests.Quests.push_back(lID);
					}
				}
			}
		}
		break;
	case QS_TALK:
		{
			long lQuestID = pMsg->GetLong();
			long lTextID = pMsg->GetLong();
		}
		break;

	case QS_OPEN_UI:
		{
			long lListID = pMsg->GetLong();
			long lOpenType = pMsg->GetLong(); 
		}
		break;
	case QS_UPDATE_SHARELIST:
		{
			TeamQuestData& td = GetInst(TeamQuestData);
			td.Clear();
			//获得队伍推荐任务列表
			long lSize=pMsg->GetLong();
			for(int i=0;i<lSize;i++)
			{
				long lQuestId = pMsg->GetLong();
				char strName[MAX_PATH] = "";
				pMsg->GetStr(strName,MAX_PATH);
				td.Add(lQuestId,strName);
			}
			td.NotifyUpdateUI();
		}
		break;
	case QS_SHARE_OPEN:
		{

		}
		break;
	case QS_SHARE_CUR:
		{
			long lCurTeamQuest = pMsg->GetLong();
			//显示提示
			const CQuest *pQuest = CQuestSystem::GetQuest(lCurTeamQuest);
			if(pQuest == NULL)
				break;
			//检查该玩家是否是队长
			CPlayer *pPlayer = GetGame()->GetMainPlayer();
			if(!pPlayer)	return;
			if(pPlayer->GetCaptainID() == pPlayer->GetExID())
			{
				char chText[256]="";
				sprintf(chText,AppFrame::GetText("TM_20"),pQuest->GetAttribute().GetQuestName());
			}else{
				char chText[256]="";
				sprintf(chText,AppFrame::GetText("TM_21"),pQuest->GetAttribute().GetQuestName());
			}
		}
		break;

    case QS_SHARE_Ret:
        {
            BYTE   ret = pMsg->GetByte();
            char   str[64],Msg[256];
            pMsg->GetStr( str,64);
            sprintf_s( Msg,AppFrame::GetText("TM_42"),str);
            if ( ret == 0)
            {
                GetInst(MsgEventManager).PushEvent(Msg_Ok,Msg);
            }
        }
        break;
	}
}