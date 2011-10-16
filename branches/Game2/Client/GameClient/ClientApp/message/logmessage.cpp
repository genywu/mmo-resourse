#include "stdafx.h"
#include "Region.h"
#include "Player.h"
#include "../Goods/Goods.h"
#include "Message.h"
#include "../GameClient/game.h"
#include "ClientRegion.h"
#include "SequenceString.h"
#include "Other/AudioList.h"
#include "Other/GlobeSetup.h"
#include "Depot.h"
#include "../../ui/UIManager.h"
#include "../../ui/GamePage/Login/LoginPage.h"
#include "../../ui/GamePage/SelectWorldServer/WorldServerPage.h"
#include "../../ui/GamePage/SelectRole/SelectRolePage.h"
#include "../../ui/GamePage/CreateRole/CreateRolePage.h"
#include "../MsgBoxSys/MsgEventManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern string g_strLastWisper;

bool g_bHandleSocketClose = true;
char strCostMsg[256];

void OnCostMessage(CMessage *pMsg)
{
	if(pMsg->GetType()==MSG_S2C_COST_NOTENOUGHCOST)
	{
		pMsg->GetStr(strCostMsg,sizeof(strCostMsg));
		g_bHandleSocketClose=false;
		if(g_bDebug)
			MessageBox(g_hWnd,strCostMsg,"cost",0);
	}
}



/*
 * 功能: 响应LOG消息
 * 摘要: -
 * 参数: pMsg - 服务器发送的消息
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.03.20 - lpf
 *		在MSG_L2C_LOG_ANSWER_PLAYERBASELIST消息中增加了"玩家和平状态"和"同步创建玩家模型"的设定;
 *	2008.04.01 - lpf
 *		在MSG_L2C_LOG_ANSWER_PLAYERBASELIST消息中增加了关于Hair和Face属性的设定;
 *		在MSG_L2C_LOG_ANSWERCREATE消息中增加了关于Hair和Face属性的设定;
 *	2008.10.16 - lpf
 *		在MSG_L2C_LOG_ANSWER_PLAYERBASELIST消息中增加了不显示AlphaFade效果;
 *	2008.10.18 - lpf
 *		在MSG_S2C_LOG_ANSWER_PLAYERDATA消息中增加了对进入场景的主玩家AlphaFade的设定;
 *	2009.03.01 - lpf
 *		在MSG_L2C_LOG_ANSWER_PLAYERBASELIST消息中增加了披风装备在登陆页面的显示设定;
 *	2009.03.13 - lpf
 *		在MSG_L2C_LOG_ANSWER_PLAYERBASELIST消息中增加了玩家默认显示披风设定;
 *		在MSG_S2C_LOG_ANSWER_PLAYERDATA消息中增加了场景SetChangeEquip()显示设定;
 *	2009.11.13 - lpf
 *		在MSG_S2C_LOG_ANSWER_PLAYERDATA消息中增加了对当前场景是否为PK场景标志的读取,并对场景对象做了对应的设定;
 */
void OnLogMessage(CMessage* pMsg)
{
	
	char * pszStr = NULL;
	
	g_LoginTime = 0;
	switch(pMsg->GetType())
	{
	case MSG_L2C_LOG_ANSWERCDKEY:	// 服务器返回玩家数据
		{
			GetGame()->SetQueuePos(-1);

			// 重置
			GetGame()->ReNew();
			
			//long Result = pMsg->GetChar(); 
			long Result = pMsg->GetLong();
			switch( Result )
			{
			case LOGIN_LOGINSERVEROK:
				{
					string strCDKey= GetInst(LoginPage).GetCDkey();
                    GetInst(LoginPage).Close();
					GetInst(WorldServerPage).ClearWorldInfo();
					int wsNum=pMsg->GetShort();					
					char wsName[20];
					for(int i=0;i<wsNum;i++)
					{
						long lStateLvl = pMsg->GetLong();
						pMsg->GetStr(wsName,20);
                        GetInst(WorldServerPage).AddWorldInfo(i,wsName,lStateLvl);
					}
                    GetInst(WorldServerPage).UpdateWSList();
					GetInst(WorldServerPage).SetCDKey(strCDKey.c_str());
					GetGame()->SetGameState(GS_SELECTWS);
					return;
				}
				break;

			case LOGIN_VERSIONERROR:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav"); 
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_3"));
					GetGame()->SetAutoLogin(false);
				}
				break;

			case LOGIN_NOTCDKEY:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_4"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_BEBANED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");

					char outStr[256];
					char szBanTime[32] = "";
					pMsg->GetStr(szBanTime, 32);
					sprintf(outStr, AppFrame::GetText("Base_5"), szBanTime);
					pszStr = outStr;
					GetInst(MsgEventManager).PushEvent(Msg_Ok,pszStr);
					GetGame()->SetAutoLogin(false);
				}
				break;

			case LOGIN_CDKEYDISABLE:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_6"));
					GetGame()->SetAutoLogin(false);
				}
				break;

			case LOGIN_IP_FORBID:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_7"));
					GetGame()->SetAutoLogin(false);
				}
				break;

			case LOGIN_PASSWORDERROR:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");

					// 提示密码出错
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_8"));
					GetGame()->SetAutoLogin(false);
				}
				break;

			case LOGIN_CHECKOUT:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_9"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGINDB_ERROR:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_10"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_ACCOUNT_NOT_ACTIVE:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_39"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_ACCOUNT_IS_LOCKED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_40"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_REJECTED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_41"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_WAIT_INIT_FINISH:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_42"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_DB_COM_ERROR:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_43"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_CANT_CONN_DB:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_44"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_DB_OPT_FAILED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_45"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_LOCAL_DB_FAILED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_46"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_LOGINSERVERPLAYERFULL:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_38"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_PLAYER_ALREAD_IN_GAME:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_9"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_WORLD_SERVER_BE_CLOSED:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_25"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case  LOGIN_NOTENOUGH_HEALTHY_TIME:
				{
					char outStr[256]="";
					DWORD dwTime = pMsg->GetDWord();
					//将剩余不健康时间转化为显示时间
					//int iSec = dwTime%60;
			  //      int m1 = dwTime/60;
			  //      int iMin = m1%60;
		   //      	int ihour = m1/60%60; 
					sprintf(outStr, AppFrame::GetText("Base_49"));
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					pszStr = outStr; 
					GetInst(MsgEventManager).PushEvent(Msg_Ok,pszStr);
					GetGame()->SetAutoLogin(false);
				}
				break;
			case SECURITY_OPERATE_LOST:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_407"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_IP_LOGIN_MAX_SAME_CDKEY:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_413"));
					GetGame()->SetAutoLogin(false); 
				}
				break;
			case LOGIN_IP_LOGIN_NIMIETY_CDKEY:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_414"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_IP_CREATE_MAX:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_415"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			case LOGIN_MODE_ERROR:
				{
					GetGame()->GetNetClient()->Close();
					GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
					GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_420"));
					GetGame()->SetAutoLogin(false);
				}
				break;
			default:
				{
					GetGame()->GetNetClient()->Close();
					char outStr[256]="";
					sprintf(outStr,"%s%d",AppFrame::GetText("Base_408"),Result);
					pszStr = outStr; 
					GetInst(MsgEventManager).PushEvent(Msg_Ok,pszStr);
					GetGame()->SetAutoLogin(false);
				}
				break;
			}
			DWORD dwCurTime = timeGetTime();
            //GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLastTime(dwCurTime);
			g_LoginTime = 0;
			GetGame()->SetGameState(GS_LOGIN);
			GetGame()->SDOLogOut();
		}
		break;

	case MSG_L2C_LOG_ANSWER_PLAYERBASELIST:	// 服务器返回玩家数据
		{
			GetInst(WorldServerPage).Close();
			GetGame()->SetQueuePos(-1);

			char strCDKey[20],strName[50];
			BYTE nRoleCnt=0;

			//======================================
					WORD nLevel=0;
					DWORD nOccupation=0,
					nSex=0,
					nCountry=0,
					nHead=0,
					nHair=0,
					nFace=0,
					nNecklace=0,
					nWing=0,
					nBody=0,
					nBack=0,
					nGlove=0,
                    nShoulder=0,
					nWrist=0,
					nLeg=0,
					nBoot=0,
					nLring=0,
					nRring=0,
					nWeapon=0,
					nCard=0,
					nGodGoods=0,
					nEffigy=0,
					nLifeCowry=0,
					nSoulGuard=0,
					nRider=0,

                    nHeadLevel=0,
                    nNecklaceLevel=0,
                    nWingLevel=0,
                    nBodyLevel=0,
                    nBackLevel=0,
                    nGloveLevel=0,
                    nShoulderLevel=0,
                    nWristLevel=0,
                    nLegLevel=0,
                    nBootLevel=0,
                    nLringLevel=0,
                    nRringLevel=0,
                    nWeaponLevel=0,
                    nCardLevel=0,
                    nGodGoodsLevel=0,
                    nEffigyLevel=0,
                    nLifeCowryLevel=0,
                    nSoulGuardLevel=0,
                    nRiderLevel=0,

					nRgnID = 0,
					nField = 0;

			//获取服务器验证码
			long lSignCode = pMsg->GetLong();
			GetGame()->SetSignCode(lSignCode);
			
			long nRemainDays;
			CGUID playerid;
			BYTE bRet=0;
			bRet=pMsg->GetByte();
			int i;

			if(bRet == 2)
			{
				GetGame()->GetNetClient()->Close();
				GetGame()->SetGameState(GS_LOGIN);
				GetGame()->SetAutoLogin(false);
				GetGame()->SDOLogOut();
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_11"));
				return;
			}
			if(bRet != 1)
			{
				GetGame()->GetNetClient()->Close();
				GetGame()->SetGameState(GS_LOGIN);
				GetGame()->SetAutoLogin(false);
				GetGame()->SDOLogOut();
				GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_12"));
				return;
			}
            
			pMsg->GetStr(strCDKey,20);

			// 创建角色时的随机国家范围
			BYTE nRandCountryRange = pMsg->GetByte();

            CreateRolePage::SetRandCountryRange( nRandCountryRange );

			nRoleCnt=BYTE(pMsg->GetShort());
			if(nRoleCnt==0)
			{
				GetGame()->SetGameState(GS_CREATE);
				return;
			}

			// 这里是接受3个角色 bugs

			// 清除旧的角色
            GetInst(SelectRolePage).ClearPlayer();
			for(i=0;i<nRoleCnt;i++)
			{
				// 填充
				nField=BYTE(pMsg->GetShort());//栏位值
				pMsg->GetGUID(playerid);
				pMsg->GetStr(strName,50);//得到名字

					nLevel			=pMsg->GetShort();
					nOccupation		=pMsg->GetShort();
					nSex			=pMsg->GetShort();
					nCountry		=pMsg->GetShort();
					nHead			=pMsg->GetShort();
					nHair			=pMsg->GetShort();
					nFace			=pMsg->GetLong();
					nNecklace		=pMsg->GetShort();
					nWing			=pMsg->GetShort();
					nBody			=pMsg->GetShort();
					nBack			=pMsg->GetShort();
					nGlove			=pMsg->GetShort();
					nShoulder		=pMsg->GetShort();
					nWrist		    =pMsg->GetShort();
					nLeg			=pMsg->GetShort();
                    nBoot           =pMsg->GetShort();
					nLring			=pMsg->GetShort();
					nRring			=pMsg->GetShort();
					nWeapon			=pMsg->GetShort();
					nCard			=pMsg->GetShort();
					nGodGoods		=pMsg->GetShort();
					nEffigy			=pMsg->GetShort();
					nLifeCowry		=pMsg->GetShort();
					nSoulGuard		=pMsg->GetShort();
					nRider			=pMsg->GetShort();

					nHeadLevel		=pMsg->GetShort();
					nNecklaceLevel	=pMsg->GetShort();
					nWingLevel		=pMsg->GetShort();
					nBodyLevel		=pMsg->GetShort();
					nBackLevel		=pMsg->GetShort();
					nGloveLevel		=pMsg->GetShort();
                    nShoulderLevel	=pMsg->GetShort();
                    nWristLevel	    =pMsg->GetShort();
                    nLegLevel		=pMsg->GetShort();
                    nBootLevel      =pMsg->GetShort();
                    nLringLevel		=pMsg->GetShort();
                    nRringLevel		=pMsg->GetShort();
                    nWeaponLevel	=pMsg->GetShort();
                    nCardLevel		=pMsg->GetShort();
                    nGodGoodsLevel	=pMsg->GetShort();
                    nEffigyLevel	=pMsg->GetShort();
                    nLifeCowryLevel	=pMsg->GetShort();
                    nSoulGuardLevel	=pMsg->GetShort();
                    nRiderLevel		=pMsg->GetShort();

					nRemainDays		=(char)pMsg->GetLong();
					nRgnID			=pMsg->GetLong(); 

				CPlayer *pPlayer = new CPlayer;
				pPlayer->SetGraphicsID(nSex+1);
				pPlayer->SetExID(playerid);
				pPlayer->SetOccupation((eOccupation)nOccupation);

				pPlayer->SetName(strName);
				pPlayer->SetLevel(nLevel);
				pPlayer->SetSex((BYTE)nSex);
				pPlayer->SetCountry((BYTE)nCountry);
				pPlayer->SetHairPic((BYTE)nHair);
				pPlayer->SetFacePic((BYTE)nFace);
				pPlayer->SetShowFashion(FASHION_TYPE_MANTEAU);

				//===========================
				// -- 左边装备 
				//pPlayer->SetEquipIndex(CPlayer::EQUIP_HEAD, nHead);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_NECKLACE, nNecklace);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WING,  nWing);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_BODY, nBody);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_BACK, nBack);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_HAND, nGlove);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_SHOULDER, nShoulder);

				// -- 右边装备
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WRIST, nWrist);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LEG, nLeg);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_BOOT,  nBoot);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LRING, nLring);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_RRING, nRring);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WEAPON, nWeapon);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_CARD, nCard);

				// -- 下边装备
				pPlayer->SetEquipIndex(CPlayer::EQUIP_GODGOODS, nGodGoods);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_EFFIGY, nEffigy);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LIFECOWRY, nLifeCowry);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_SOULGUARD, nSoulGuard);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_RIDER, nRider);

				// --装备等级
				// -- 左边装备
				pPlayer->SetEquipLevel(CPlayer::EQUIP_HEAD, (BYTE)nHeadLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_NECKLACE, nNecklaceLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_WING,  nWingLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_BODY, nBodyLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_BACK, nBackLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_HAND, nGloveLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_SHOULDER, nShoulderLevel);

                // -- 右边装备
                pPlayer->SetEquipIndex(CPlayer::EQUIP_WRIST, nWristLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_LEG, nLegLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_BOOT,  nBootLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_LRING, nLringLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_RRING, nRringLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_WEAPON, nWeaponLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_CARD, nCardLevel);

                // -- 下边装备
                pPlayer->SetEquipIndex(CPlayer::EQUIP_GODGOODS, nGodGoodsLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_EFFIGY, nEffigyLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_LIFECOWRY, nLifeCowryLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_SOULGUARD, nSoulGuardLevel);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_RIDER, nRiderLevel);

				pPlayer->SetDisplayModel(false);
				pPlayer->SetDisplayModelGroup();
				pPlayer->SetRegionID(nRgnID);
				pPlayer->SetPlayerRemainDays((char)nRemainDays);
				pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON);
				pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WING);

                GetInst(SelectRolePage).AddPlayer(pPlayer);

				pPlayer->SetState(CShape::STATE_PEACE);
				pPlayer->SetIsNeedAlphaFade(false);
			}

			/*if(pPage->GetPlayerVector()->size())
			{
				pPage->SetSelectedNum( 0 );
			}

			pPage->SetCurPage(0);*/
			GetGame()->SetGameState(GS_SELECT);
			FireUIEvent("SelectRolePage","UpdateDelRemainTime");
		}
		break;		

	case MSG_L2C_LOG_ANSWERDELETE://loginserver返回删除角色的结果
		{
			char strCDKey[20]="";	
			int result=pMsg->GetChar();
			if(result==LOGIN_DELETEROLE_SUCCEED)
			{
				CGUID PlayerID;
				pMsg->GetGUID(PlayerID);
				pMsg->GetStr(strCDKey,20);
				char nRemainDays = pMsg->GetChar();
				SelectRolePage& srp = GetInst(SelectRolePage);
				
				ulong cnt = srp.GetPlayerCount();
				for(ulong c = 0 ; c < cnt ; ++c)
				{
					CPlayer* player = srp.GetPlayer(c);
					if(player && player->GetExID() == PlayerID)
					{
						player->SetPlayerRemainDays(nRemainDays);
						char delInfo[256];
						sprintf(delInfo,"还有%d天被删除",nRemainDays);
						GetInst(MsgEventManager).PushEvent(Msg_Ok,delInfo);
						FireUIEvent("SelectRolePage","UpdateDelRemainTime");
						return;
					}
				}
			}
			else
			{
				int nID=pMsg->GetLong();
				pMsg->GetStr(strCDKey,20);
				long lFlag = pMsg->GetLong();
				char str[256]="";
				if(lFlag == 0)
				{
					sprintf(str,AppFrame::GetText("Base_14"));
					GetInst(MsgEventManager).PushEvent(Msg_Ok,str);
				}
				else if(lFlag == 1)
				{
					sprintf(str,AppFrame::GetText("Base_15"));
					GetInst(MsgEventManager).PushEvent(Msg_Ok,str);
				}
				else if(lFlag == 2)
				{
					sprintf(str,AppFrame::GetText("Base_16"));
					GetInst(MsgEventManager).PushEvent(Msg_Ok,str);
				}
				else if(lFlag == 3)
				{
					sprintf(str,AppFrame::GetText("Base_17"));
					GetInst(MsgEventManager).PushEvent(Msg_Ok,str);
				}
			}
		}
		break;

	case MSG_L2C_LOG_ANSWERRESTORE:
		{
			char res = pMsg->GetChar();

			if(res == LOGIN_RESTOREROLE_SUCCEED)
			{
				CGUID PlayerID;
				pMsg->GetGUID(PlayerID);
				SelectRolePage& srp = GetInst(SelectRolePage);
				ulong cnt = srp.GetPlayerCount();
				for(ulong c = 0 ; c < cnt ; ++c)
				{
					CPlayer* player = srp.GetPlayer(c);
					if(player && player->GetExID() == PlayerID)
					{
						player->SetPlayerRemainDays(-1);
						GetInst(MsgEventManager).PushEvent(Msg_Ok,
							AppFrame::GetText("Base_18"));
						FireUIEvent("SelectRolePage","UpdateDelRemainTime");
						return;
					}
				}
			}
			else
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_19"));
			}
		}
		break;

	case MSG_L2C_LOG_ANSWERCREATE://loginserver返回创建角色的结果
		{
			int result=pMsg->GetChar();
			if(result==LOGIN_CREATEROLE_SUCCEED)
			{
				char strCDKey[20],strName[50];
				short nLevel;
				char nOccupation;
				char nSex;
				BYTE nHead;
				BYTE nHair;
				long lFace;
				CGUID Playerid;
				long lRegionID;
				BYTE byCountry;
				
                DWORD dwHead=0,
                    dwNecklace=0,
                    dwWing=0,
                    dwBody=0,
                    dwBack=0,
                    dwGlove=0,
                    dwShoulder=0,
                    dwWrist=0,
                    dwLeg=0,
                    dwBoot=0,
                    dwLring=0,
                    dwRring=0,
                    dwWeapon=0,
                    dwCard=0,
                    dwGodGoods=0,
                    dwEffigy=0,
                    dwLifeCowry=0,
                    dwSoulGuard=0;

               BYTE nHeadLevel=0,
                    nNecklaceLevel=0,
                    nWingLevel=0,
                    nBodyLevel=0,
                    nBackLevel=0,
                    nGloveLevel=0,
                    nShoulderLevel=0,
                    nWristLevel=0,
                    nLegLevel=0,
                    nBootLevel=0,
                    nLringLevel=0,
                    nRringLevel=0,
                    nWeaponLevel=0,
                    nCardLevel=0,
                    nGodGoodsLevel=0,
                    nEffigyLevel=0,
                    nLifeCowryLevel=0,
                    nSoulGuardLevel=0;
	
				pMsg->GetStr(strCDKey,20);
				pMsg->GetGUID(Playerid);
				pMsg->GetStr(strName,50);
				nLevel=pMsg->GetShort();
				nOccupation=pMsg->GetChar();
				nSex=pMsg->GetChar();
				byCountry = pMsg->GetByte();
				nHead=pMsg->GetByte();
				nHair=pMsg->GetByte();
				lFace=pMsg->GetLong();

				// 左边装备栏
                dwHead=pMsg->GetDWord();
                dwNecklace=pMsg->GetDWord();
                dwWing=pMsg->GetDWord();
                dwBody=pMsg->GetDWord();
                dwBack=pMsg->GetDWord();
                dwGlove=pMsg->GetDWord();
                dwShoulder=pMsg->GetDWord();
                dwWrist=pMsg->GetDWord();
                dwLeg=pMsg->GetDWord();
                dwBoot=pMsg->GetDWord();
                dwLring=pMsg->GetDWord();
                dwRring=pMsg->GetDWord();
                dwWeapon=pMsg->GetDWord();
                dwCard=pMsg->GetDWord();
                dwGodGoods=pMsg->GetDWord();
                dwEffigy=pMsg->GetDWord();
                dwLifeCowry=pMsg->GetDWord();
                dwSoulGuard=pMsg->GetDWord();
				// -- 装备等级
				// 左边装备栏
                nHeadLevel=pMsg->GetByte();
                nNecklaceLevel=pMsg->GetByte();
                nWingLevel=pMsg->GetByte();
                nBodyLevel=pMsg->GetByte();
                nBackLevel=pMsg->GetByte();
                nGloveLevel=pMsg->GetByte();
                nShoulderLevel=pMsg->GetByte();
                nWristLevel=pMsg->GetByte();
                nLegLevel=pMsg->GetByte();
                nBootLevel=pMsg->GetByte();
                nLringLevel=pMsg->GetByte();
                nRringLevel=pMsg->GetByte();
                nWeaponLevel=pMsg->GetByte();
                nCardLevel=pMsg->GetByte();
                nGodGoodsLevel=pMsg->GetByte();
                nEffigyLevel=pMsg->GetByte();
                nLifeCowryLevel=pMsg->GetByte();
                nSoulGuardLevel=pMsg->GetByte();

				lRegionID = pMsg->GetLong();

				CPlayer *pPlayer = new CPlayer;
				pPlayer->SetGraphicsID(nSex+1);
				pPlayer->SetExID(Playerid);
				pPlayer->SetOccupation((eOccupation)nOccupation);
				pPlayer->SetName(strName);
				pPlayer->SetLevel(nLevel);
				pPlayer->SetSex(nSex);
				pPlayer->SetCountry(byCountry);
				pPlayer->SetHairPic(nHair);
				pPlayer->SetFacePic((BYTE)lFace);

				// -- 左边装备 
				pPlayer->SetEquipIndex(CPlayer::EQUIP_HEAD, dwHead);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_NECKLACE, dwNecklace);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WING,  dwWing);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_BODY, dwBody);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_BACK, dwBack);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_HAND, dwGlove);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_SHOULDER, dwShoulder);

				// -- 右边装备
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WRIST, dwWrist);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LEG, dwLeg);
                pPlayer->SetEquipIndex(CPlayer::EQUIP_BOOT, dwBoot);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LRING,  dwLring);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_RRING, dwRring);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_WEAPON, dwWeapon);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_CARD, dwCard);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_GODGOODS, dwGodGoods);

				// -- 下边装备
				pPlayer->SetEquipIndex(CPlayer::EQUIP_EFFIGY, dwEffigy);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_LIFECOWRY, dwLifeCowry);
				pPlayer->SetEquipIndex(CPlayer::EQUIP_SOULGUARD, dwSoulGuard);

				// --装备等级
				// -- 左边装备
				pPlayer->SetEquipLevel(CPlayer::EQUIP_HEAD, nHeadLevel);
				pPlayer->SetEquipLevel(CPlayer::EQUIP_NECKLACE, nNecklaceLevel);
				pPlayer->SetEquipLevel(CPlayer::EQUIP_WING,  nWingLevel);
				pPlayer->SetEquipLevel(CPlayer::EQUIP_BODY, nBodyLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_BACK, nBackLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_HAND, nGloveLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_SHOULDER, nShoulderLevel);

                // -- 右边装备
                pPlayer->SetEquipLevel(CPlayer::EQUIP_WRIST, nWristLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_LEG, nLegLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_BOOT, nBootLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_LRING,  nLringLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_RRING, nRringLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_WEAPON, nWeaponLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_CARD, nCardLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_GODGOODS, nGodGoodsLevel);

                // -- 下边装备
                pPlayer->SetEquipLevel(CPlayer::EQUIP_EFFIGY, nEffigyLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_LIFECOWRY, nLifeCowryLevel);
                pPlayer->SetEquipLevel(CPlayer::EQUIP_SOULGUARD, nSoulGuardLevel);

				pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON);
		//		pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WEAPON2);
				pPlayer->UpdateWeaponShow(CPlayer::EQUIP_WING);
				pPlayer->UpdateWeaponShow(CPlayer::EQUIP_P_HEADGEAR);
				

				// --
				lRegionID = pMsg->GetLong();
				pPlayer->SetPlayerRemainDays(-1);

				// add by lpf - 2007.12.13
				pPlayer->SetDisplayModel();
				pPlayer->SetDisplayModelGroup();

                GetInst(SelectRolePage).AddPlayer(pPlayer);
                GetGame()->SetGameState(GS_SELECT);
				FireUIEvent("SelectRolePage","UpdateDelRemainTime");
				
			}
			else if(result==LOGIN_CREATEROLE_DBERROR)
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
				GetGame()->SetGameState(GS_LOGIN);
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_20"));
			}
			else if(result==LOGIN_CREATEROLE_NAMEEXIST)
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
                GetGame()->SetGameState(GS_LOGIN);
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_21"));
			}
			else if(result==LOGIN_CREATEROLE_FULL)
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");	
                GetGame()->SetGameState(GS_LOGIN);
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_22"));
			}
			else if (result == LOGIN_CREATEROLE_NAMEINVALID)
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
                GetGame()->SetGameState(GS_LOGIN);
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_23"));
			}
			else
			{
				GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
                GetGame()->SetGameState(GS_LOGIN);
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_24"));
			}
		}
		break;

	case MSG_L2C_LOG_ANSWER_PLAYERDATA://loginserver返回获取详细信息的结果,并包含gs的IP和端口
		{
#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MSG_L2C_LOG_ANSWER_PLAYERDATA Start.");
#endif
			Log4c::Trace(ROOT_MODULE,"MSG_L2C_LOG_ANSWER_PLAYERDATA");
			GetGame()->SetQueuePos(-1);

			long lChangeServer = pMsg->GetLong();
			GetGame()->SetChangeServerCode(lChangeServer);
			int result=pMsg->GetChar();
			if(result==LOGIN_QUEST_PLAYERDATA_SUCCEED)
			{
			#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"result==LOGIN_QUEST_PLAYERDATA_SUCCEED Start.");
			#endif
				char strGsIp[20],strCDKey[20];
				long lGsPort;
				pMsg->GetStr(strCDKey,20);
				pMsg->GetStr(strGsIp,20);
				lGsPort=pMsg->GetLong();

                CGUID PlayerID = GetInst(SelectRolePage).GetSelPlayer()->GetExID();

				CClient* pTemptClient = GetGame()->GetNetClient();
				GetGame()->SetGameCDKEY(strCDKey);
				CClient* pNetClient = GetGame()->ConnectGameServer(strGsIp,lGsPort,PlayerID);				

				//连接GameServer成功
				if(pNetClient != NULL)
				{
					GetGame()->SetNetClient(pNetClient);
					CMessage::SetClient(pNetClient);

					pTemptClient->Exit();
					pTemptClient->Close();
					SAFE_DELETE(pTemptClient);
					
					// 释放仓库物品数据
					CGCMana::Inst()->Clear( true );

					// 复位聊天数据
					/*CMainBarPageEx* pMainBarPageEx = GetGame()->GetCGuiEx()->GetMainBarPageEx();
					if( pMainBarPageEx )
					{
						pMainBarPageEx->SetChannel( CHAT_NORMAL );
						pMainBarPageEx->SetChatType( CHAT_NORMAL );
						g_strLastWisper = "";
					}*/
				}
				else
				{
					GetGame()->SetNetClient(NULL);
					CMessage::SetClient(NULL);

					//连接GameServer失败
					CMessage msg(MSG_C2L_LOG_QUESTEXIT);
					msg.Add((long)0);
					msg.Send(pTemptClient);

					//在模拟测试网络延时，不删除。避免多线程操作非法		
					pTemptClient->Exit();
					pTemptClient->Close();
					SAFE_DELETE(pTemptClient);					

					GetGame()->SetGameState(GS_LOGIN);
					GetInst(MsgEventManager).PushEvent(Msg_Ok,
						AppFrame::GetText("Base_25"));
				}

			#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"result==LOGIN_QUEST_PLAYERDATA_SUCCEED End.");
			#endif
			}
			else if(result == LOGIN_LOGINSERVERPLAYERFULL)    //服务器人数达到上限
			{
				GetGame()->ReNew();
				GetGame()->SetGameState(GS_LOGIN);
				g_bHandleSocketClose=true;
                GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav");
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_38"));
			}
			else
			{
				GetInst(MsgEventManager).PushEvent(Msg_Ok,
					AppFrame::GetText("Base_26"));
			}
#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"MSG_L2C_LOG_ANSWER_PLAYERDATA End.");
#endif
		}
		break;

	case MSG_S2C_LOG_ANSWER_PLAYERDATA://gameserver对玩家详细数据的返回
		{
			Log4c::Trace(ROOT_MODULE,"MSG_S2C_LOG_ANSWER_PLAYERDATA");
			//get the detail data from gameserver
			long lCode = pMsg->GetLong();
			long lFlag = pMsg->GetLong();
			// 成功进入游戏场景
			if( lFlag )
			{
				// 释放选择界面的资源
                UIManager::CloseAllPage();
                //UIManager::ReleaseAllPage();
                UIManager::ReleasePage("Login");
                UIManager::ReleasePage("WorldServer");
                UIManager::ReleasePage("SelectRolePage");
                UIManager::ReleasePage("CreateRolePage");
                UIManager::ReleasePage("SelectLS");

                //释放前一个玩家的界面__gongran
                CloseAllWindow();
                DestroyAllWindow();

				CGUID PlayerID;
				pMsg->GetGUID(PlayerID);
				//GetGame()->GetCGuiEx()->GetSelectRolePageEx()->StopSound();
				CPlayer* pPlayer = GetGame()->GetMainPlayer();

				if (pPlayer==NULL)
				{
					GetGame()->NewMainPlayer();
					pPlayer = GetGame()->GetMainPlayer();
				}

				if( pPlayer )
				{
					// 属性转换设置
					pMsg->GetEx(&GetGame()->GetSetup()->fStr2MinAtk[0], sizeof(float)*33);
					GetGame()->GetSetup()->lHitTime = pMsg->GetLong();
					GetGame()->GetSetup()->wBaseRp_Lv1=pMsg->GetWord();
					GetGame()->GetSetup()->wBaseRp_Lv2=pMsg->GetWord();
					GetGame()->GetSetup()->wPkCountPerKill=pMsg->GetWord();
//////////////////////////////////////////世界范围内的属性///////////////////////////////////
					GetGame()->GetSetup()->fBasePriceRate=pMsg->GetFloat();
					GetGame()->GetSetup()->fTradeInRate=pMsg->GetFloat();
					GetGame()->GetSetup()->fRepairFactor=pMsg->GetFloat();
					pMsg->GetStr(GetGame()->GetSetup()->szTalkCountryGoodsName, 32);
					GetGame()->GetSetup()->nTalkCountryGoodsNum = pMsg->GetLong();
					GetGame()->GetSetup()->dwTalkCountryMoney = pMsg->GetDWord();
					pMsg->GetStr(GetGame()->GetSetup()->szTalkWorldGoodsName, 32);
					GetGame()->GetSetup()->nTalkWorldGoodsNum = pMsg->GetLong();
					GetGame()->GetSetup()->dwTalkWorldMoney = pMsg->GetDWord();
					// 盟国
					//pMsg->GetStr(GetGame()->GetSetup()->szTalkCountryUnionGoodsName, 32);
					GetGame()->GetSetup()->nTalkCountryUnionGoodsNum = 0;//pMsg->GetLong();
					GetGame()->GetSetup()->dwTalkCountryUnionMoney = 0;//pMsg->GetDWord();
/////////////////////////////////////////////////////////////////////////////////////////////
					pPlayer->SetEnemyPlayerLevelFirstQuotiety(pMsg->GetByte());
					pPlayer->SetEnemyPlayerLevelSecondQuotiety(pMsg->GetByte());

					if (CGlobeSetup::GetSetup()->dwPlayerAlphaFadeTimeLength != 0)
					{
						pPlayer->SetIsNeedAlphaFade(true);
						pPlayer->SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwPlayerAlphaFadeTimeLength);
						pPlayer->SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);
					}

					extern long AREA_WIDTH;
					extern long AREA_HEIGHT;
					AREA_WIDTH = pMsg->GetLong();
					AREA_HEIGHT = pMsg->GetLong();

					if (pPlayer->GetFather())
					{
						((CClientRegion*)pPlayer->GetFather())->RemoveObject(pPlayer);
					}
					
					DBReadSet setReadDB;
					pMsg->GetDBReadSet(setReadDB);
					pPlayer->DecodeFromDataBlock(setReadDB);

					// 地图名字
					char strRegionName[256];
					pMsg->GetStr(strRegionName,256);
					long lTaxRate = pMsg->GetLong();							//得到税率
					eRWT eWarRegionType = (eRWT)pMsg->GetByte();//得到战斗场景类型
					eRST eSpaceType=(eRST)pMsg->GetByte();//是否是水下场景
					bool bChangeEquip = pMsg->GetByte()==0?false:true;
					CGUID RegionID;
					pMsg->GetGUID(RegionID);
					long lRegionID = pMsg->GetLong();
					long lResourceID = pMsg->GetLong();
					float fExpScale = pMsg->GetFloat();
					eRPKT ePKRegionType = (eRPKT)pMsg->GetByte();

					pPlayer->SetDisplayModel();
					GetGame()->QuestEnterRegion(lRegionID,RegionID,lResourceID,strRegionName, pPlayer->GetTileX(), pPlayer->GetTileY(), pPlayer->GetDir());
					//设置税率
					CClientRegion * pRegion = GetGame()->GetRegion();
					if(pRegion)
					{
						pRegion->SetTaxRate(lTaxRate);
						pRegion->SetWarType(eWarRegionType);
						pRegion->SetExpScale(fExpScale);
						pRegion->SetSpaceType(eSpaceType);
						pRegion->SetChangeEquip(bChangeEquip);
						pRegion->SetPKRegionType(ePKRegionType);

						//当主角进入场景
						pRegion->OnMainPlayerEnter();						
					}
					//打开宠物血条 并加入场景shape链表
					//@todo 王晓轩
					//long lPetCount = pPlayer->GetFightPetNum();
					//if ( lPetCount > 0)
					//{
					//	for (int i = 0; i < lPetCount; i++)
					//	{
					//		CPet *pPet = pPlayer->GetActivePet(i+1);
					//		if (pPet)
					//			GetGame()->GetRegion()->AddObject((CBaseObject*)pPet);
					//	}
					//	//GetGame()->GetCGuiEx()->GetPetNamHpPage()->Open();
					//	//GetGame()->GetCGuiEx()->GetPetHotKeyBarPage()->Update();
					//}
					//////////////////////////获得分线信息////////////////////////////////////
					//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->SetCurRegionID(lRegionID);
					//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->DecodeFromDataBlock(setReadDB);

					// 服务器的时间
					DWORD dwCurTime = pMsg->GetDWord();

					// **************************场景的据点相关信息（09.9.29刘科）*********************
					BYTE	bOpenFlag = pMsg->GetByte();
					long	lPointNum = pMsg->GetLong();
					char	szPointName[256] = "";
					if(pRegion)
					{
						//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetIsOpenSPoint(bOpenFlag>0);
						CClientRegion::tagSPointInfo stSPInfo;
						for ( long num = 0; num < lPointNum; ++num)
						{
							stSPInfo.strPointName = pMsg->GetStr(szPointName,256);
							stSPInfo.lPosX		= pMsg->GetLong();
							stSPInfo.lPosY		= pMsg->GetLong();
							stSPInfo.bState		= (BYTE)pMsg->GetLong();
							pRegion->AddSPointInfo(stSPInfo);
						}
					}
					// ********************************************************************************

					//初始化部分功能数据
					//根据分线设置界面上的分线按钮
					//GetGame()->GetCGuiEx()->GetSelectLinePageEx()->SetSelectBotton();

					//关闭怪物出现音效
					GetGame()->GetAudioList()->SetOppearSoundEnble(false);

					//初始化人物的勋章
					//GetGame()->GetCGuiEx()->GetCharacterPageEx()->InitMedalGoods(); 

					//设置客户端显示的时间
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetServerTime(dwCurTime);

					//如果登陆盛大圈圈，设置圈圈的角色信息
					//GetGame()->SetPlayerInfo(pPlayer->GetName(),pPlayer->GetSex());

				}
			}
			else
			{
				GetGame()->CloseNetClient();
				GetGame()->SetGameState(GS_LOGIN);				
			}
		}
		break;

		// 服务器断开了连接
	case MSG_C2C_LOG_SERVERLOST:
		{			
			GetGame()->ReNew();	// 重新开始

			GetGame()->SetGameState( GS_LOGIN );
			GetGame()->	SDOLogOut();
			g_bHandleSocketClose=true;
		}
		break;
	case MSG_L2C_LOG_UPDATEQUEUENUM:
		{
			long lNumber = pMsg->GetLong();
			GetGame()->SetQueuePos(lNumber);
		}
		break;

		// 收到Server时间
	case MSG_S2C_LOG_GAMESERVERTIME:
		{
			long lPlayerID = pMsg->GetLong();
			unsigned long ulServerTime = pMsg->GetDWord();
			//CGuiExPage::m_lC2SDeltaTime = (long)pMsg->GetDWord() - (long)time( NULL );

			// 启动
			GetGame()->m_ulServerTime = ulServerTime;
			GetGame()->m_ulClientTime = /*(ULONG)time(NULL)*/ timeGetTime();
			GetGame()->m_bMsgTimeStart = true;


#ifdef _DEBUG
			char debug[128];
			sprintf(debug, "Recieve Init Server time %d\r\n", ulServerTime);
			OutputDebugString(debug);
#endif // _DEBUG
		}
		break;

		// 接受用于验证消息的序列字符串
	case MSG_S2C_LOG_SEQUENCESTRING:
		{
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			CSequenceString::Uninitialize();

			// 初始 并启动
			BYTE ByteData[10240];
			pMsg->GetEx(ByteData,10240);
			long lPos = 0;
			CSequenceString::Initialize(ByteData,lPos);
		}
		break;

	// 接受帐号的信息并显示在选择角色界面
	case MSG_L2C_LOG_SENDACCOUNTINFO:
		{
			char strCdkey[256];
			char strInfo[256];
			pMsg->GetStr(strCdkey, 256);
			pMsg->GetStr(strInfo, 256);
		}
		break;
	case MSG_L2C_LOG_UPDATEWORLDSERVERINFO:
		{
			if( GetGame()->GetGameState() == GS_SELECTWS)
			{
				int wsNum=pMsg->GetShort();					
				char wsName[20];
				for(int i=0;i<wsNum;i++)
				{
					long lStateLvl = pMsg->GetLong();
					pMsg->GetStr(wsName,20);
				}
			}
		}
		break;
	case MSG_L2C_LOG_WAITDATALOAD:
		{
			char str[100]="";
			sprintf(str,AppFrame::GetText("Base_47"));
			//GetInst(MsgEventManager).PushEvent(GE_Box_Default_Window,str);
			GetGame()->SetQueuePos(-1);
		}
		break;
	case MSG_L2C_LOG_FROZEN_CDKEY:
		{
			DWORD dwTime = pMsg->GetDWord();
			GetGame()->GetNetClient()->Close();
			GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-12.wav"); 
			int h = 0;                   //小时
			int m = 0;                   //分钟
			int s = 0;                   //秒
			DWORD dwLastTimeTemp = dwTime;
			s = dwLastTimeTemp%60;
			DWORD m1 = dwLastTimeTemp/60;
			m = m1%60;
			DWORD h1 = m1/60;
			h = h1%60; 
			char pszStr[256] = "";
			sprintf(pszStr,AppFrame::GetText("Base_48"),h,m,s);
			GetGame()->SetAutoLogin(false);
			//if (GetGame()->GetGameState() == GS_LOGIN)
			//pMsgBox->Open(pLoginPage,pszStr,CMessageBoxPageEx::MSG_OK,true);
			//else if (GetGame()->GetGameState() == GS_PWD_CARD)
			//	pMsgBox->Open(GetGame()->GetCGuiEx()->GetMatrixCardPageEx(),pszStr,CMessageBoxPageEx::MSG_OK,true);
		}
		break;
	case MSG_L2C_LOG_FCM_TIME:   //服务器通知防沉迷剩余时间
		{
			DWORD dwTime = pMsg->GetDWord();    //秒
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetIsIndulgence(true);
			//GetGame()->GetCGuiEx()->GetMainBarPageEx()->SetIndulgenceTime(dwTime*1000);
			if(GetGame()->GetGameState()==GS_RUN)
			{
				//GetGame()->GetCGuiEx()->GetMainBarPageEx()->OpenIndulgenceComp(1); 
                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->FcmTimeNote(true);
			}
		}
		break;
	case MSG_L2C_LOG_SECURITY_VALIDATE:		//通知客户端进行安全信息输入（密保卡）
		{
			//CMatrixCardPageEx *pMatrixPage = GetGame()->GetCGuiEx()->GetMatrixCardPageEx();
			//CMessageBoxPageEx *pMsgPage    = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
			//if (pMatrixPage && pMsgPage)
			//{
			//	WORD wVertifyCount = pMatrixPage->GetVerticyCount();
			//	if (wVertifyCount>=1)
			//	{
			//		pMsgPage->Open(pMatrixPage,CStringReading::LoadText(1,404),CMessageBoxPageEx::MSG_OK,true);
			//	}
			//	
			//	//获取矩阵卡3个位置
			//	unsigned char *cPos = new unsigned char[3];
			//	pMsg->GetEx(cPos,3);
			//	pMatrixPage->ReSetPage();
			//	pMatrixPage->SetMatrixPositon(cPos,3);
			//	SAFE__DELETE_ARRAY(cPos);
			//	pMatrixPage->TIME_OUT = pMsg->GetLong();
			//}
			
			//设置当前游戏状态为 密保卡输入状态
			GetGame()->SetGameState(GS_PWD_CARD);
		}
		break;
	case MSG_L2C_LOG_SECURITY_VALIDATINT:
		{
			//CMessageBoxPageEx *pMsgPage    = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
			//CLoginPageEx	  *pLoginPage  = GetGame()->GetCGuiEx()->GetLoginPageEx();
			DWORD dwCurTime = timeGetTime();
           // GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLastTime(dwCurTime);
			g_LoginTime = 0;
			GetGame()->SetGameState(GS_LOGIN);
			GetInst(MsgEventManager).PushEvent(Msg_Ok,
				AppFrame::GetText("Base_406"));
			//if (pMsgPage)
			//{
			//	pMsgPage->Open(pLoginPage,CStringReading::LoadText(1,406),CMessageBoxPageEx::MSG_OK,true);
			//}
		}
	}
}

