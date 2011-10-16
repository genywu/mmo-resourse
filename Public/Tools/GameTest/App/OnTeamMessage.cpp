#include "stdafx.h"
#include "GameClient.h"
#include "TeamMgr.h"
#include "RgnObjMgr.h"
#include "..\Script\ScriptSys.h"
#include "..\GameTest\GameTestDlg.h"


void CGameClient::OnTeamMessage(CMessage* pMsg)
{
	static bool bTeaming = false;	//是否正在组队
	switch(pMsg->GetType())
	{
	case MSG_S2C_TEAM_MemberPos:
		{
			long bufSize = pMsg->GetLong();
			if(bufSize==0)
				return;
			BYTE* pData = new BYTE[bufSize];
			// 			vector<BYTE> v_buf(bufSize);
			pMsg->GetEx(pData,bufSize);
			// 			HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
			// 			long pos = 0;
			// 			long size = _GetLongFromByteArray(pData,pos);
			// 			for(long i =0;i<size;i++)
			// 			{
			// 				CGUID playerGuid;
			// 				_GetBufferFromByteArray(pData,pos,playerGuid);
			// 				long x = _GetLongFromByteArray(pData,pos);
			// 				long y = _GetLongFromByteArray(pData,pos);
			// 
			// 				if( pInfo->find(playerGuid) != pInfo->end())
			// 				{
			// 					((*pInfo)[playerGuid]).fPosX = x;
			// 					((*pInfo)[playerGuid]).fPosY = y;
			// 				}
			// 			}
			SAFE_DELETE_ARRAY(pData);
		}
		break;
		//招募
	case MSG_S2C_TEAM_RECRUIT:
		{
			CGUID PlayerGuiid;
			pMsg->GetGUID(PlayerGuiid);
			long lFlag = pMsg->GetLong();
			if( lFlag == 0)
				return;

			CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,PlayerGuiid);
			if( pPlayer == NULL )
				return;
			char strName[20];
			pMsg->GetStr(strName,20);
			long lPass = pMsg->GetLong();
			// 			pPlayer->SetRecruitingTeam(true);
			// 			pPlayer->SetTeamname(strName);
			// 			pPlayer->SetTeamHasPassword( lPass ==0? false:true);
		}
		break;
		//更新招募人数
	case MSG_S2C_TEAM_RECRUITED_NUM:
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			long lNum = pMsg->GetLong();
			CPlayer* pPlayer =(CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
			if( pPlayer == NULL )
				return;

			//			pPlayer->SetTeammateAmount(lNum);
		}
		break;
	case MSG_S2C_TEAM_MastterChange:
		{

			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			//			m_pMainPlayer->SetCaptainID(playerGuid);
		}
		break;
		//物品分配 
	case MSG_S2C_TEAM_GoodsSetChange:
		{
			long lAllotMOde = pMsg->GetLong();
		}
		break;
		//玩家是否在线的状态改变
	case MSG_S2C_TEAM_MemberStateChange:
		{
			CGUID guid;
			pMsg->GetGUID(guid);
			if( guid == m_pMainPlayer->GetExID() )
				return;
			long lState = pMsg->GetLong();
			//			HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
			// 			if( pInfo->find(guid) != pInfo->end())
			// 			{
			// 				((*pInfo)[guid]).bOnline = lState ==0?false:true;
			//			}
		}
		break;
	case MSG_S2C_TEAM_TeamCreate:			// 创建队友
		{
			CGUID teamGuid;
			pMsg->GetGUID(teamGuid);
			CGUID captainGuid;
			pMsg->GetGUID(captainGuid);
			long lGoodsAllot = pMsg->GetLong();
			long lMemNum = pMsg->GetLong();

			// 			HSM_OneTM_Info* pList = m_pMainPlayer->GetOneTeammateInfo();
			// 			pList->clear();
			// 
			// 			for(int i=0;i<lMemNum;i++)
			// 			{
			// 				tagWSMemberInfo mi;
			// 				pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
			// 				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,mi.guid);
			// 				if(pPlayer)
			// 				{
			// 					pPlayer->SetTeamID(teamGuid);
			// 					pPlayer->SetCaptainID(captainGuid);
			// 					if( captainGuid == mi.guid )
			// 						pPlayer->SetIsTeamLeader(true);
			// 					else
			// 						pPlayer->SetIsTeamLeader(false);
			// 
			// 					pPlayer->SetTeammateAmount(lMemNum);
			// 				}
			// 				tagOneTeammateInfo oneInfo;
			// 				oneInfo.PlayerGuid = mi.guid;
			// 				// 				oneInfo.lOccu = mi.lOccupation;
			// 				// 				oneInfo.lSex = mi.lSex;
			// 				oneInfo.bOnline = (mi.lState==0?false:true);
			// 				strcpy(oneInfo.szName,mi.szName);
			// 				(*pList)[mi.guid] = oneInfo;
			//			}
		}
		break;
	case MSG_S2C_TEAM_TeamData:				// 所有队伍数据到成员
		{
			CGUID teamGuid;
			pMsg->GetGUID(teamGuid);
			CGUID captainGuid;
			pMsg->GetGUID(captainGuid);
			long lAllot = pMsg->GetLong();
			long lMemNum = pMsg->GetLong();

			// 			HSM_OneTM_Info* pData = m_pMainPlayer->GetOneTeammateInfo();
			// 			for(int i=0;i<lMemNum;i++)
			// 			{
			// 				tagWSMemberInfo mi;
			// 				pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
			// 				CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,mi.guid);
			// 				if(pPlayer)
			// 				{
			// 					pPlayer->SetTeamID(teamGuid);
			// 					pPlayer->SetCaptainID(captainGuid);
			// 					if( captainGuid == mi.guid )
			// 						pPlayer->SetIsTeamLeader(true);
			// 					else
			// 						pPlayer->SetIsTeamLeader(false);
			// 					pPlayer->SetTeammateAmount(lMemNum);
			// 				}
			// 				tagOneTeammateInfo oneInfo;
			// 				oneInfo.PlayerGuid = mi.guid;
			// 				// 				oneInfo.lOccu = mi.lOccupation;
			// 				// 				oneInfo.lSex = mi.lSex;
			// 				oneInfo.bOnline = (mi.lState==0?false:true);
			// 				strcpy(oneInfo.szName,mi.szName);
			// 				(*pData)[mi.guid] = oneInfo;
			// 			}
			// 			long lTeamQuestID = pMsg->GetLong();
			// 			m_pMainPlayer->SetTeamQuestID(lTeamQuestID);
		}
		break;
	case MSG_S2C_TEAM_MemberAllIdioinfo:// 队伍成员的详细信息
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			if( playerGuid == m_pMainPlayer->GetExID())
				return;

			long lFlag = pMsg->GetLong();
			tagMemberIdioinfo mi;
			if(lFlag)
			{
				pMsg->GetEx(&mi,sizeof(tagMemberIdioinfo));
			}
			//--------------------------------------------
			// 			HSM_OneTM_Info* pList = m_pMainPlayer->GetOneTeammateInfo();
			// 			if(pList)
			// 			{
			// 				if(pList->find(playerGuid) != pList->end())
			// 				{
			// 					// 					((*pList)[playerGuid]).dwMaxHp = mi.lMaxHp;
			// 					// 					((*pList)[playerGuid]).dwMaxMp = mi.lMaxMp;
			// 					// 					((*pList)[playerGuid]).dwMaxRp = mi.lMaxEnergy;
			// 					// 					((*pList)[playerGuid]).dwHp = mi.lCurrHp;
			// 					// 					((*pList)[playerGuid]).dwMp = mi.lCurrMp;
			// 					// 					((*pList)[playerGuid]).dwRp = mi.lCurrEnergy;
			// 					// 					((*pList)[playerGuid]).lLevel = mi.lLevel;
			// 					((*pList)[playerGuid]).fPosX = mi.fPosX;
			// 					((*pList)[playerGuid]).fPosY = mi.fPosY;
			// 				}
			// 			}
		}
		break;
	case MSG_S2C_TEAM_UpdateIdioinfo:	// 更新某个队友的某个属性
		{
			CGUID PlayerGuid;
			pMsg->GetGUID(PlayerGuid);
			long lType = pMsg->GetLong();
			if(lType==eMIS_Pos)
			{
				float x = pMsg->GetFloat();
				float y = pMsg->GetFloat();
				//				HSM_OneTM_Info *pInfo = m_pMainPlayer->GetOneTeammateInfo();
				// 				if( pInfo->find(PlayerGuid) != pInfo->end() )
				// 				{
				// 					((*pInfo)[PlayerGuid]).fPosX = x;
				// 					((*pInfo)[PlayerGuid]).fPosY = y;
				// 				}
			}
		}
		break;
		//	有新玩家加入队伍后
	case MSG_S2C_TEAM_MemberData:
		{
			tagWSMemberInfo mi;
			pMsg->GetEx(&mi,sizeof(tagWSMemberInfo));
			if(mi.guid == m_pMainPlayer->GetExID())
				return;

			CGUID playerGuid = mi.guid;
			// 			HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
			// 			{
			// 				tagOneTeammateInfo oneInfo;
			// 				oneInfo.PlayerGuid = mi.guid;
			// 				//				oneInfo.lOccu = mi.lOccupation;
			// 				//				oneInfo.lSex = mi.lSex;
			// 				oneInfo.bOnline = (mi.lState==0?false:true);
			// 				strcpy(oneInfo.szName,mi.szName);
			// 				(*pMap)[mi.guid] = oneInfo;
			// 			}
			// 			m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount()+1);
		}
		break;
	case MSG_S2C_TEAM_MemberLeaveHere:
		{
			CGUID guid;
			pMsg->GetGUID(guid);

			// 			HSM_OneTM_Info * pMap = m_pMainPlayer->GetOneTeammateInfo();
			// 
			// 			if(m_pMainPlayer->GetExID() == guid)
			// 			{
			// 				long lSize = pMap->size();
			// 				HSM_OneTM_Info::iterator it = pMap->begin();
			// 				for(;it != pMap->end();it++)
			// 				{
			// 					//					it->second.dwHp = 0;
			// 					//					it->second.dwMp = 0;
			// 					//					it->second.dwRp = 0;
			// 				}
			// 			}
			// 			else
			// 			{
			// 				if(pMap->find(guid) != pMap->end())
			// 				{
			// 					//					(*pMap)[guid].dwHp = 0;
			// 					//					(*pMap)[guid].dwMp = 0;
			// 					//					(*pMap)[guid].dwRp = 0;
			// 				}
			// 			}
		}
		break;
	case MSG_S2C_TEAM_MemberLeave:
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			//自己退队
			if(playerGuid == m_pMainPlayer->GetExID())
			{
				// 				m_pMainPlayer->SetTeamID(CGUID::GUID_INVALID);
				// 				m_pMainPlayer->SetCaptainID(CGUID::GUID_INVALID);
				// 				m_pMainPlayer->GetOneTeammateInfo()->clear();
				// 				m_pMainPlayer->SetTeammateAmount(1);
			}
			else
			{
				// 				HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
				// 				if( pMap->find(playerGuid) != pMap->end())
				// 				{
				// 					pMap->erase(playerGuid);
				// 					m_pMainPlayer->SetTeammateAmount(m_pMainPlayer->GetTeammateAmount() - 1 );
				// 					CPlayer* pPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,playerGuid);
				// 					if(pPlayer)
				// 					{
				// 						pPlayer->SetTeamID(CGUID::GUID_INVALID);
				// 						pPlayer->SetCaptainID(CGUID::GUID_INVALID);
				// 						pPlayer->SetTeammateAmount(1);
				// 					}
				// 				}
			}
		}
		break;
	case MSG_S2C_TEAM_MembereExStateChange:	// 队友异常状态改变
		{
			CGUID playerGuid;
			pMsg->GetGUID(playerGuid);
			if( playerGuid == m_pMainPlayer->GetExID())
				return;

			DWORD dwFlag = pMsg->GetDWord();
			DWORD dwStateID = pMsg->GetDWord();
			DWORD dwLevel = pMsg->GetDWord();

			// 			HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
			// 			if(dwFlag && pMap)
			// 			{
			// 				if(pMap->find(playerGuid) != pMap->end())
			// 				{
			// 					//					((*pMap)[playerGuid]).lLevel = dwLevel;
			// 				}
			// 			}
		}
		break;
		// 邀请方发出请求
	case MSG_S2C_TEAM_QUERY_JOIN:
		{
			CGUID SelfGuid,RequesterGuid;
			pMsg->GetGUID(SelfGuid);
			pMsg->GetGUID(RequesterGuid);

			CPlayer* pRequestPlayer = (CPlayer*)m_pClientObjMgr->FindObject(TYPE_PLAYER,RequesterGuid);
			char name[256];
			string strName = pMsg->GetStr(name,256);
			m_pMainPlayer->GetTeamMgr()->SetTeamRequesterGuid(RequesterGuid);
			pMsg->Reset();
		}
		break;
		// 	case MSG_S2C_TEAM_ANSWER_JOIN:
		// 		{
		// 			CGUID AimGuid,SorGuid;
		// 			pMsg->GetGUID(AimGuid);
		// 			pMsg->GetGUID(SorGuid);
		// 			CPlayer* pAimPlyaer = (CPlayer*)m_pRegion->FindChildObject(TYPE_PLAYER,AimGuid);
		// 			if( pAimPlyaer != NULL )
		// 				return;
		// 			char name[256];
		// 			strcpy(name,pAimPlyaer->GetName());
		// 			BYTE b = pMsg->GetByte();
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_SYNCHRONIZE:
		// 		{
		// 			for(int i=0;i<4;i++)
		// 			{
		// 				pMsg->GetLong();
		// 			}
		// 			CGUID teamGuid,captionGuid;
		// 			pMsg->GetGUID(teamGuid);
		// 			char teamName[64],pass[64];
		// 			pMsg->GetStr(teamName,23);
		// 			pMsg->GetStr(pass,9);
		// 			pMsg->GetGUID(captionGuid);
		// 			DWORD dwAmount = pMsg->GetDWord();
		// 
		// 			m_pMainPlayer->GetOneTeammateInfo()->clear();
		// 			m_pMainPlayer->SetTeamID(teamGuid);
		// 			m_pMainPlayer->SetCaptainID(captionGuid);
		// 
		// 			CPlayer::HSM_OneTM_Info* pMap = m_pMainPlayer->GetOneTeammateInfo();
		// 
		// 			for(int i=0;i<dwAmount;i++)
		// 			{
		// 				pMsg->GetLong();
		// 				long lType = pMsg->GetLong();
		// 				CGUID guid;
		// 				pMsg->GetGUID(guid);
		// 				pMsg->GetLong();
		// 
		// 				CGUID regionGuid;
		// 				pMsg->GetGUID(regionGuid);
		// 				char strTeammate[32];
		// 				pMsg->GetStr(strTeammate,17);
		// 
		// 				if( pMap->find(guid) != pMap->end() )
		// 				{
		// //					((*pMap)[guid]).RegionGuid = regionGuid;
		// 					strcpy(((*pMap)[guid]).szName,strTeammate);
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_TEAMATE_JOIN:
		// 		{
		// 			CGUID teamGuid;
		// 			pMsg->GetGUID(teamGuid);
		// 			long lType = pMsg->GetLong();
		// 			CGUID playerGuid;
		// 			pMsg->GetGUID(playerGuid);
		// 			CGUID regionGuid;
		// 			pMsg->GetGUID(regionGuid);
		// 			float fHpPercent = pMsg->GetFloat();
		// 			char strName[32];
		// 			pMsg->GetStr(strName,17);
		// 
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			if( pInfo->find(playerGuid) != pInfo->end())
		// 			{
		// 				CPlayer::tagOneTeammateInfo tmInfo;
		// 				tmInfo.PlayerGuid = playerGuid;
		// //				tmInfo.dwHp = (WORD)(fHpPercent*100);
		// //				tmInfo.dwMaxHp = 100;
		// 				strcpy(tmInfo.szName,strName);
		// //				tmInfo.RegionGuid = regionGuid;
		// 
		// 				m_pMainPlayer->SetTeamID(teamGuid);
		// 				(*pInfo)[playerGuid] = tmInfo;
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_STATE:
		// 		{
		// 			CGUID TeamID;
		// 			pMsg->GetGUID(TeamID);
		// 			long lType=pMsg->GetLong();
		// 			CGUID PlayerID;
		// 			pMsg->GetGUID(PlayerID);
		// 			float fHp=pMsg->GetFloat();
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			{
		// 				if(pInfo->find(PlayerID) != pInfo->end())
		// 				{
		// //					(*pInfo)[PlayerID].dwHp=(WORD)(100*fHp);
		// //					(*pInfo)[PlayerID].dwMaxHp=100;
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_TEAMATE_CHANGE_REGION:
		// 		{
		// 			CGUID TeamID,PlayerID,RegionID;
		// 			pMsg->GetGUID(TeamID);
		// 			long lType=pMsg->GetLong();
		// 			pMsg->GetGUID(PlayerID);
		// 			pMsg->GetGUID(RegionID);
		// 			CPlayer::HSM_OneTM_Info* pInfo = m_pMainPlayer->GetOneTeammateInfo();
		// 			{
		// 				if(pInfo->find(PlayerID) != pInfo->end())
		// 				{
		// //					(*pInfo)[PlayerID].RegionGuid = RegionID;
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		break;
		// 	case MSG_S2C_TEAM_CHANGE_LEAER:
		// 		{
		// 			CGUID TeamID,LeaderID;
		// 			pMsg->GetGUID(TeamID);
		// 			m_pMainPlayer->SetCaptainID(LeaderID);
		// 		}
		// 		break;
	}

}