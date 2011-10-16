#include "stdafx.h"
#include "../GameClient/PlayerRanks.h"
#include "Player.h"
#include "Message.h"
#include "Organizingsystem/Union.h"
#include "Organizingsystem/Faction.h"
#include "Organizingsystem/FactionPageOper.h"
#include "Other/DeputyOccuSystemClient.h"
#include "CollectionGoods.h"
#include "Other/AudioList.h"
#include "QuestSystem/QuestSystem.h"
#include "../GameClient/Game.h"  
#include "../MsgBoxSys/MsgEventDef.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../../Public/Common/QuestDef.h"

namespace  ShareQuest
{
    struct ShareData
    {
        BYTE  QuestResult;
        long  QuestId;
        CGUID SendID;
        char  Name[20];
        char  DestInfo[256];
    };
}
void  AnswerShareQuest(MsgEvent& event,eMsgResult UserId)
{                                       
    ShareQuest::ShareData* pShare = (ShareQuest::ShareData*)event.pData;
    if ( !pShare ) return ;             

    /// 提示对方游戏设置同意与不同意组队请求
    BYTE  ret = ( UserId == S_Ok ) ? 1 : 0;
    CMessage msg( MSG_C2S_QUEST_OPER );
    msg.Add( (BYTE)QS::QS_SHARE );
    msg.Add((BYTE)ret); 
    msg.Add(pShare->QuestId);
    msg.Add(pShare->SendID);
    msg.Send();

    SAFE_DELETE( pShare );
}

/////////////////////////////////////////////////
// zhaohang  2010/3/23 
// 加入排行榜系统头文件
#include "../RankSystem/RankSystem.h"
#include "../../../ui/GamePage/RankPage/RankPage.h"
/////////////////////////////////////////////////
void OnOrgasysMessage(CMessage* pMsg)
{
	BYTE *ByteData = new BYTE[102400];

	// 家族逻辑管理对象
	CFactionManager*	pFactionManager = GetGame()->GetFactionManager();

	switch(pMsg->GetType())
	{

		/* 王峰：-----------------------------    家族相关消息 ----------------------------*/

		// 初始化家族协议
	case MSG_S2C_ORGSYS_InitOrgSys_Notify:
		{
			pFactionManager->RecvInitMsg( pMsg );
		}
		break;

		// 创建家族的脚本事件协议
	case MSG_S2C_ORGSYS_CreateFaction_Notify:
		{
			pFactionManager->RecvCreateNotifyMsg( pMsg );
		}
		break;

    case MSG_S2C_ORGSYS_CreateORGSysUI:
        {
           GetInst(FactionOper).CreateFactionUi( );
        }
        break;

		// 创建家族协议
	case MSG_S2C_ORGSYS_CreateFaction_Re:
		{
			GetInst(FactionOper).RecvCreateMsg( pMsg );
		}
		break;

		// 打开家族协议
	case MSG_S2C_Faction_OpenGUI:
		{
			GetInst(FactionOper).RecvOpenMsg( pMsg );
		}
		break;

		// 解散家族协议
	case MSG_S2C_ORGSYS_DisbandFaction_Re:
		{
			GetInst(FactionOper).RecvDisbandMsg( pMsg );
		}
		break;

		// 解散家族通知协议
	case MSG_S2C_ORGSYS_DisbandFaction_Notify:
		{
			GetInst(FactionOper).RecvDisbandNotifyMsg( pMsg );
		}
		break;

		// 退出家族协议
	case MSG_S2C_ORGSYS_Quit_Re:
		{
			GetInst(FactionOper).RecvQuitMsg( pMsg );
		}
		break;

		// 退出家族通知
	case MSG_S2C_ORGSYS_Quit_Notify:
		{
			GetInst(FactionOper).RecvQuitNotifyMsg( pMsg );
		}
		break;

		// 踢出家族协议
	case MSG_S2C_ORGSYS_Kick_Re:
		{
			GetInst(FactionOper).RecvKickMsg( pMsg );
		}
		break;

		// 踢出家族通知协议
	case MSG_S2C_ORGSYS_Kick_Notify:
		{
			GetInst(FactionOper).RecvKickNotifyMsg( pMsg );
		}
		break;

		// 升级家族功能回复协议
	case MSG_S2C_ORGSYS_UpgradeFaction_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvUpgradeMsg( pMsg );
		}
		break;

		// 升级家族功能通知协议
	case MSG_S2C_ORGSYS_UpgradeFaction_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvUpgradeNotifyMsg( pMsg );
		}
		break;

		// 一个家族的公共数据通知
	case MSG_S2C_Faction_PublicData_Notify:
		{
			pFactionManager->RecvFacPublicDataMsg( pMsg );
		}
		break;

		// 更新一个玩家的家族信息
	case MSG_S2C_ORGSYS_UpdatePlayerFacInfo:
		{
			GetInst(FactionOper).RecvUpdatePlayerFacInfoMsg( pMsg );
		}
		break;

		// 家族基本资料回应
	case MSG_S2C_Faction_BaseData_Notify:
		{
			pFactionManager->RecvBaseInfoMsg( pMsg );
		}
		break;

		// 会员资料回应
	case MSG_S2C_Faction_Member_Notify:
		{
			pFactionManager->RecvMemberInfoMsg( pMsg );
		}
		break;

		// 申请者资料回应
	case MSG_S2C_Faction_Apply_Notify:
		{
			pFactionManager->RecvRequisitionInfoMsg( pMsg );
		}
		break;

		// 公告资料回应
	case MSG_S2C_Faction_Placard_Notify:
		{
			pFactionManager->RecvPlacardInfoMsg( pMsg );
		}
		break;

		// 备忘录资料回应
	case MSG_S2C_Faction_Leaveword_Notify:
		{
			pFactionManager->RecvMemoInfoMsg( pMsg );
		}
		break;

		// 修改公告回应
	case MSG_S2C_ORGSYS_SetPlacard_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetPlacardMsg( pMsg );
		}
		break;

		// 修改备忘录回应
	case MSG_S2C_ORGSYS_SetLeaveword_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetMemoMsg( pMsg );
		}
		break;

		// 官阶权限资料回应
	case MSG_S2C_ORGSYS_SetJobPurview_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvJobPurviewMsg( pMsg );
		}
		break;

		// 修改官阶名字回应
	case MSG_S2C_ORGSYS_SetJobName_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetJobNameMsg( pMsg );
		}
		break;

		// 修改官阶权限回应
	case MSG_S2C_ORGSYS_SetJobPurview_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetJobPurviewMsg( pMsg );
		}
		break;

		// 设置成员官阶回应
	case MSG_S2C_ORGSYS_SetMemberJob_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetMemberJobMsg( pMsg );
		}
		break;

		// 审批入会对其他会员的通知
	case MSG_S2C_ORGSYS_CheckRequisition_Notify:
		{
            GetInst(FactionOper).RecvCheckRequisitionMsgNotify( pMsg );
		}
		break;

		// 审批入会
	case MSG_S2C_ORGSYS_CheckRequisition_Re:
		{
            GetInst(FactionOper).RecvCheckRequisitionMsg( pMsg );
		}
		break;

		// 邀请入会
	case MSG_S2C_Faction_InvitePlayer_Re:
		{
			GetInst(FactionOper).RecvInviteRequestMsg( pMsg );
		}
		break;

		// 邀请入会通知
	case MSG_S2C_Faction_InvitePlayer_Notify:
		{
			GetInst(FactionOper).RecvInviteAnswerMsg( pMsg );
		}
		break;

		// 加入家族
	case MSG_S2C_Faction_AddMember_Notify:
		{
			GetInst(FactionOper).RecvJoindNotifyMsg( pMsg );
		}
		break;

		// 设置会员称号
	case MSG_S2C_Faction_SetMemberTitle_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetTitleMsg( pMsg );
		}
		break;

		// 转让族长
	case MSG_S2C_Faction_SetChairman_Re:
		{
			GetInst(FactionOper).RecvSetChairmanMsg( pMsg );
		}
		break;

		// 被转让成族长的通知
	case MSG_S2C_Faction_SetChairman_Notify:
		{
			GetInst(FactionOper).RecvSetChairmanNotifyMsg( pMsg );
		}
		break;


		// 设置成员官阶通知
	case MSG_S2C_ORGSYS_SetMemberJob_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetMemberJobNotifyMsg( pMsg );
		}
		break;

		// 设置成员称号通知
	case MSG_S2C_Faction_SetMemberTitle_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvSetMemberTitleNotifyMsg( pMsg );
		}
		break;

		// 更新成员地图
	case MSG_S2C_Faction_MemberMap_Notifye:
		{
			GetInst(FactionOper).RecvMemberMapNotifyeMsg( pMsg );
		}
		break;

		// 更新成员等级
	case MSG_S2C_Faction_MemberLevel_Notifye:
		{
			GetInst(FactionOper).RecvMemberLevelNotifyMsg( pMsg );
		}
		break;

		// 更新成员职业
	case MSG_S2C_Faction_MemberOccu_Notifye:
		{
			GetInst(FactionOper).RecvMemberOccuNotifyMsg( pMsg );
		}
		break;

		// 更新成员在线状态
	case MSG_S2C_Faction_MemberOnline_Notifye:
		{
			GetInst(FactionOper).RecvMemberOnlineNotifyMsg( pMsg );
		}
		break;

		// 更新招募状态
	case MSG_S2C_Faction_ChangeRecruitState_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvChangeRecruitStateMsg( pMsg );
		}
		break;

		// 更新成员贡献
	case MSG_S2C_Faction_ChangeMemberContribute_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvMemberContributeNotifyMsg( pMsg );
		}
		break;

		// 更新资源
	case MSG_S2C_Faction_ChangeRes_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvResNotifyMsg( pMsg );
		}
		break;

		// 更新经验
	case MSG_S2C_Faction_ChangeExp_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvExpNotifyMsg( pMsg );
		}
		break;

		// 打开家族列表界面
	case MSG_S2C_Faction_ApplyBoardOpen_Notify:
		{
            pFactionManager->RecvFactionList( pMsg );
		}
		break;

		// 申请加入回复
	case MSG_S2C_ORGSYS_JoinFac_Re:
		{
             GetInst(FactionOper).RecvApplyRestMsg( pMsg );
		}
		break;

		// 打开徽标上传的 文件名输入框
	case MSG_S2C_Faction_IconBoardOpen_Notify:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvInputFacIconNotifyMsg( pMsg );
		}
		break;

		// 上传一个家族图标数据回复
	case MSG_S2C_Faction_IconData_Re:
		{
			//GetGame()->GetCGuiEx()->GetFactionPageEx()->RecvUploadFacIconMsg( pMsg );
		}
		break;

		// 一个家族的图标数据更新通知
	case MSG_S2C_Faction_IconData_Notify:
		{
			pFactionManager->RecvUpdateFacIconNotifyMsg( pMsg );
		}
		break;

		// 更新所有家族图标信息
	case MSG_S2C_Faction_AllIconData_Re:
		{
			pFactionManager->RecvUpdateAllFacIconMsg( pMsg );
		}
		break;

		// 更新战争敌对家族信息
	case MSG_S2C_Faction_ChangeEnemy_Notify:
		{
			pFactionManager->RecvWarInfoMsg( pMsg );
		}
		break;

		// 更新参战信息
	case MSG_S2C_Faction_ChangeJoinBattle_Notify:
		{
			pFactionManager->RecvMemberWarMsg( pMsg );
		}
		break;

		// 更新族员功勋
	case MSG_S2C_Faction_ChangeBattle_Notify:
		{
			pFactionManager->RecvMemberAchievementMsg( pMsg );
		}
		break;

		// 更新族员微尘
	case MSG_S2C_Faction_ChangeArmyInvest_Notify:
		{
			pFactionManager->RecvMemberWarFundMsg( pMsg );
		}
		break;

		// 更新据点信息
	case MSG_S2C_Faction_Beachheads_Notify:
		{
			pFactionManager->RecvBeachheadInfoMsg( pMsg );
		}
		break;

		/* End：王峰 --------------------------    家族相关消息 --------------------------*/
		/* 王峰：-----------------------------    联盟相关消息 ----------------------------*/

		// 创建联盟脚本请求通知，打开联盟创建界面
	case MSG_S2C_UNION_Script_Creation:
		{
			GetGame()->GetUnionManager()->RecvScriptCreationMsg( pMsg );
		}
		break;

		// 创建联盟的 结果回复
	case MSG_S2C_UNION_Re_Creation:
		{
			GetGame()->GetUnionManager()->RecvCreationMsg( pMsg );
		}
		break;

		// 创建联盟的 通知
	case MSG_S2C_UNION_Notify_Creation:
		{
			GetGame()->GetUnionManager()->RecvCreationNotifyMsg( pMsg );
		}
		break;

		// 解散联盟通知所有盟友
	case MSG_S2C_UNION_Notify_Disband:
		{
			GetGame()->GetUnionManager()->RecvDisbandMsg( pMsg );
		}
		break;

		// 家族退出联盟通知所有盟友
	case MSG_S2C_UNION_Notify_Quit:
		{
			GetGame()->GetUnionManager()->RecvQuitMsg( pMsg );
		}
		break;

		// 盟主将家族踢出联盟通知
	case MSG_S2C_UNION_Notify_Kick:
		{
			GetGame()->GetUnionManager()->RecvKickMsg( pMsg );
		}
		break;

		// 盟主转让通知所有盟友
	case MSG_S2C_UNION_Notify_SetChairman:
		{
			GetGame()->GetUnionManager()->RecvSetChairmanMsg( pMsg );
		}
		break;

		// 盟主赋予族长权限回复
	case MSG_S2C_UNION_Re_SetRights:
		{
			GetGame()->GetUnionManager()->RecvSetRightsMsg( pMsg );
		}
		break;

		// 族长一个人获得盟主赋予权限的通知
	case MSG_S2C_UNION_Notify_GetRights:
		{
			GetGame()->GetUnionManager()->RecvGetRightsMsg( pMsg );
		}
		break;

		// 盟主 收到 家族族长 回复的邀请结果
	case MSG_S2C_UNION_Response_Invite:
		{
			GetGame()->GetUnionManager()->RecvResponseMsg( pMsg );
		}
		break;

		// 家族族长 收到 盟主邀请加入联盟的请求
	case MSG_S2C_UNION_Notify_Anwser_Invite:
		{
			GetGame()->GetUnionManager()->RecvInviteMsg( pMsg );
		}
		break;

		// 家族加入联盟通知所有盟友
	case MSG_S2C_UNION_Notify_Join:
		{
			GetGame()->GetUnionManager()->RecvJoinMsg( pMsg );
		}
		break;

		// 一个联盟的基本数据通知
	case MSG_S2C_UNION_Notify_BaseData:
		{
			GetGame()->GetUnionManager()->RecvUnionInfoMsg( pMsg );
		}
		break;

		// 一个联盟的成员数据通知
	case MSG_S2C_UNION_Notify_MemberData:
		{
			GetGame()->GetUnionManager()->RecvUnionMemberMsg( pMsg );
		}
		break;

		/* End：王峰 --------------------------    联盟相关消息 --------------------------*/


		/* 王峰：-----------------------------    战争相关消息 ----------------------------*/
		/* End：王峰 --------------------------    战争相关消息 --------------------------*/



		//任务相关消息--------------------------------------------
	case MSG_S2C_Quest_Add:
		{
			//------07.9.11添加liuke-------
			DWORD dwQuestID = pMsg->GetDWord();
			long lStep = pMsg->GetLong();

			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			if(pMainPlayer)
			{
				pMainPlayer->AddQuest((long)dwQuestID,lStep);
				// 在Mainbar上的任务按钮添加闪动效果
				/*CMainBarPageEx * pMainbar = GetGame()->GetCGuiEx()->GetMainBarPageEx();
				CQuestPageEx * pQuest = GetGame()->GetCGuiEx()->GetQuestPageEx();
				if(pQuest && !pQuest->IsOpen() && 
				pMainbar && pMainbar->GetInterfaceComp())
				{
				CButton *pQuestBtn = (CButton *)pMainbar->GetInterfaceComp()->GetComponent("Quest");
				if(pQuestBtn && pQuestBtn->GetSpecialIcon())
				((CImageIcon *)pQuestBtn->GetSpecialIcon())->SetIsChangeAlpha(true);
				}*/
			} 
			if(!GetGame()->GetAudioList()->IsPlaying("SOUNDS\\quest\\q-03.wav",-1))  //当完成音效未完成时，不播放接受音效
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-01.wav");
			}
			CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(dwQuestID,true);
		}
		break;
	case MSG_S2C_Quest_Complete:
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
					//GetGame()->GetCGuiEx()->GetQuestPageEx()->UpdateMainQuestList();
				}	

				CDeputyOccuSystemClient::UpdateCollectionEffectByQuest(dwQuestID,false);

				pMainPlayer->RemoveQuest((long)dwQuestID);
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-03.wav");
			}
		}
		break;
	case MSG_S2C_Quest_Remove:
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
	case MSG_S2C_Quest_UpdateStep:
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
	case MSG_S2C_Quest_UpdateParamData:
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
	case MSG_S2C_Quest_UpdateData:
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
	case MSG_S2C_Quest_UpdateRecordData:
		{
			long lQuestID = pMsg->GetLong();
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			// 为玩家添加一条已经完成的任务
			if(pMainPlayer)
				pMainPlayer->GetRoleQueue()->AddFinishedQuestRecord(lQuestID);
		}
		break;
	case MSG_S2C_Quest_Talk:
		{
			long lQuestID = pMsg->GetLong();
			long lTextID = pMsg->GetLong();

		}
		break;
	case MSG_S2C_MercQuest_UpdateData:
		{
			CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			// 为玩家添加一条已经完成的任务
			if(NULL == pMainPlayer)	break;

			long lCurMercQuestID = pMsg->GetLong();  //获取当前任务ID
			//获得当前的放弃任务列表
			//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->ClearDelQuestList();
			long lNum =  pMsg->GetLong();  //获得放弃任务数

			for(int i=0;i<lNum;i++)
			{
				long lDelQuestID =  pMsg->GetLong();  //获得放弃的任务
				//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->AddMerQuestDel(lDelQuestID);
			}
			long lType = pMsg->GetLong();
			pMainPlayer->GetMercQuestSys()->SetCurQuestID(lCurMercQuestID);
			if(lType==0)
			{
				pMainPlayer->GetMercQuestSys()->SetCurQuestID(lCurMercQuestID);

				//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->UpdateCurQuest();
			}
			else if(lType==1)
			{
				//long lCurMercQuestID = pMsg->GetLong();  //获取当前任务ID
				long lListID = pMsg->GetLong();           //获取任务列表ID 
				long dwLastUpdateTime = pMsg->GetDWord(); //获取任务更新剩余时间
				if(dwLastUpdateTime<0)    dwLastUpdateTime = 0;
				//pMainPlayer->GetMercQuestSys()->SetCurQuestID(lCurMercQuestID);
				//pMainPlayer->GetMercQuestSys()->SetLastUpdateTime(dwLastUpdateTime);
				//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->SetUpdateTime(dwLastUpdateTime);

				if(lListID != 0)
				{
					long lSize = pMsg->GetWord();
					CMercenaryQuestSys::tagMerQuest MerQuests;
					MerQuests.dwLastUpdateTime=dwLastUpdateTime;
					MerQuests.Quests.clear();
					//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->ClearMerQuestLast();
					MerQuests.dwLastUpdateTime = dwLastUpdateTime;
					for(int i=0;i<lSize;i++)
					{
						long lID = pMsg->GetLong();
						long lIsAccept = pMsg->GetLong();  //该任务是否曾做过
						if(lID!=0)
							MerQuests.Quests.push_back(lID);
						//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->AddMerQuestLast(lIsAccept);
					}

					//pMainPlayer->GetMercQuestSys()->SetMercQuests(lListID,MerQuests);
					//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->SetMerQuestList(MerQuests);
					//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->SetListID(lListID);

				}

			}
			//if(GetGame()->GetCGuiEx()->GetMercenaryPageEx()->IsOpen())
			//{
			//	GetGame()->GetCGuiEx()->GetMercenaryPageEx()->UpdatePage();
			//	if(lType==0)
			//	{
			//		GetGame()->GetCGuiEx()->GetMercenaryPageEx()->UpdateQuestNote(-1);
			//	}
			//}
		}
		break;
	case MSG_S2C_MercQuest_OpenInterface:
		{
			long lListID = pMsg->GetLong();
			long lOpenType = pMsg->GetLong(); 
			//GetGame()->GetCGuiEx()->GetMercenaryPageEx()->Open();
		}
		break;
	case MSG_S2C_SHARE_QUESTLIST_UPDATE:
		{

            ShareQuest::ShareData * pShare = new  ShareQuest::ShareData;
            pShare->QuestId = pMsg->GetLong();
            char strName[MAX_PATH] = "";
            pMsg->GetStr(pShare->Name,MAX_PATH);
            CGUID  SendID;
            pMsg->GetGUID( pShare->SendID );
            pShare->QuestResult = pMsg->GetByte();
            char DQuest[16];
            sprintf_s( DQuest ,"Q%d", pShare->QuestId );
            sprintf_s( pShare->DestInfo, AppFrame::GetText("TM_41"), pShare->Name,AppFrame::GetText( DQuest ));
            GetInst(MsgEventManager).PushEvent(Msg_YesNo,pShare->DestInfo,AnswerShareQuest,pShare);

		}
		break;
	case  MSG_S2C_SHARE_QUEST_OPEN:
		{
			//long lQuestId = pMsg->GetLong();
			//CPlayer *pMainPlayer = GetGame()->GetMainPlayer();
			//// 为玩家添加一条已经完成的任务
			//if(NULL == pMainPlayer)	break;

			//long lCurQuest = pMainPlayer->GetMercQuestSys()->GetCurQuestID();
			//long lCurTQuest = GetGame()->GetCGuiEx()->GetMercenaryPageEx()->GetTeamQuestId();
			//if(lCurQuest!=lCurTQuest)
			//{
			//GetGame()->GetCGuiEx()->GetMercenaryNotePageEx()->Open();
			//GetGame()->GetCGuiEx()->GetMercenaryNotePageEx()->UpdateQuestNote();
			//GetGame()->GetAudioList()->Play2DSound("SOUNDS\\quest\\q-01.wav");
			//if(GetGame()->GetCGuiEx()->GetMercenaryPageEx()->IsOpen())
			//{
			//	GetGame()->GetCGuiEx()->GetMercenaryPageEx()->Close();
			//}
			//}
		}
		break;
	case MSG_S2C_SHARE_CURRQUEST:
		{
			long lCurTeamQuest = pMsg->GetLong();
			/*GetGame()->GetCGuiEx()->GetMercenaryPageEx()->SetTeamQuestId(lCurTeamQuest);
			GetGame()->GetCGuiEx()->GetMercenaryNotePageEx()->SetShowQuestId(lCurTeamQuest);
			if(GetGame()->GetCGuiEx()->GetMercenaryPageEx()->IsOpen())
			{
			GetGame()->GetCGuiEx()->GetMercenaryPageEx()->UpdateTeamCurQuest();
			}
			if(GetGame()->GetCGuiEx()->GetMercenaryNotePageEx()->IsOpen())
			{
			GetGame()->GetCGuiEx()->GetMercenaryNotePageEx()->UpdateQuestNote(lCurTeamQuest);
			}*/
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
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,chText,0xff00ffff);
			}else{
				char chText[256]="";
				sprintf(chText,AppFrame::GetText("TM_21"),pQuest->GetAttribute().GetQuestName());
				// GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_CENTER,chText,0xff00ffff);
			}
		}
		break;
		///排行榜系统
	case MSG_S2C_ANSWER_PLAYERRANKS:
		{
			//pMsg->GetEx(ByteData,102400);
			//long lPos = 0;
			//GetPlayerRanks()->DecordFromByteArray(ByteData, lPos);
			/////////////////////////////////////////////////
			// zhaohang  2010/3/22 
			// 新功能排行榜系统,服务器沿用了以前的消息头，但是编解码改变了，因此注释掉以前代码，重新进行这部分的编码 
			long lPos = 0;
			CGUID guid;
			long lBufSize;
			pMsg->GetGUID(guid);
			char eRt = pMsg->GetChar();
			RankPage& page = GetInst(RankPage);
			RankMsgProc &RankMsg = GetInst(RankMsgProc);
			switch(eRt)
			{
			case RT_NOMATCH:	// 无匹配的数据
				RankMsg.SetRankState(RT_NOMATCH);
				break;	
			case RT_UPDATE:		// 服务器正在更新
				RankMsg.SetRankState(RT_UPDATE);
				break;
			case RT_OWNER:
				RankMsg.SetRankState(RT_OWNER);
				lBufSize = pMsg->GetLong();
				pMsg->GetEx(ByteData, lBufSize);
				RankMsg.ReceiveMsg(ByteData,lPos);
				page.UpdatePage();
				break;
			case RT_OTHER:
			case RT_RANK:
				lBufSize = pMsg->GetLong();
				pMsg->GetEx(ByteData, lBufSize);
				RankMsg.ReceiveMsg(ByteData,lPos);
				page.UpdatePage();
				break;
			}
			/////////////////////////////////////////////////
		}
		break;
	}
	SAFE_DELETE_ARRAY(ByteData);
}