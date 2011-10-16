#include "StdAfx.h"
#include "../ServerRegion.h"
#include "../Player.h"
#include "../Monster.h"
#include "../Goods/CGoods.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应GM消息
void CPlayer::OnMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
		// 触发NPC脚本
	case MSG_C2S_PLAYER_TOUCHNPC:
		{
			CPlayer* pPlayer = pMsg -> GetPlayer();

			if (pPlayer && pPlayer->JudgeNpcScriptslegality())
			{
				return;	
			}

			if( pPlayer && pMsg -> GetRegion())
			{
				if( pPlayer -> IsDied() )
				{
					return;
				}

				if( pPlayer -> GetCurrentProgress() != CPlayer::PROGRESS_NONE )
				{
					return;
				}

				CGUID guid;
				pMsg -> GetGUID(guid);
				CMonster* pNpc	= dynamic_cast<CMonster*>( pMsg -> GetRegion() -> FindAroundObject( pPlayer, TYPE_MONSTER, guid ) );
				if( pNpc )
				{
					if( pNpc -> Distance( pPlayer ) > 8 )
					{
						const char* strRes=AppFrame::GetText("50");
						if(strRes)
						{
							pPlayer -> SendNotifyMessage(strRes);
						}
						return;
					}

					if( pNpc -> GetTalkScriptFile()[0] != '0' )
					{
						//stRunScript st;
						//st.pszFileName = (char*)pNpc->GetTalkScriptFile();
						//st.desShape =pNpc; //pNpc;
						//st.pRegion = pMsg->GetRegion();
						//st.srcShape =this; //this;
						//RunScript(&st, (char*)GetGame()->GetScriptFileData(pNpc->GetTalkScriptFile()));
						Script::RunEnv env( this, pNpc, pMsg->GetRegion() );
						GetInst( ScriptSys ).RunScript( env, pNpc->GetTalkScriptFile() );
					}
				}
				else
				{
					//##找不到指定的NPC。
				}
			}
		}
		break;
	case MSG_C2S_PLAYER_USEITEM: //玩家请求使用一个消耗性道具的处理
		{   
			if( pMsg -> GetPlayer() -> IsDied() == FALSE )
			{

				if( pMsg -> GetPlayer() -> GetCurrentProgress() != CPlayer::PROGRESS_TRADING && 
					/*pMsg -> GetPlayer() -> GetCurrentProgress() != CPlayer::PROGRESS_SHOPPING &&	*/				
					pMsg -> GetPlayer() -> GetCurrentProgress() != CPlayer::PROGRESS_INCREMENT)
				{
					WORD wPlace=pMsg->GetWord();
					BYTE  bytePos=pMsg->GetByte();
					LONG lType=pMsg->GetLong();
					CGUID objGuid;
					pMsg->GetGUID(objGuid);
					DWORD dwGoodsId=0;

					//验证containerId
					CVolumeLimitGoodsContainer* pVolContainer=NULL;
					if(wPlace==CS2CContainerObjectMove::PEI_PACKET)
					{
						pVolContainer=pMsg->GetPlayer()->getOriginPack();
					}
					else if(wPlace>=CS2CContainerObjectMove::PEI_PACK1 && 
						wPlace<=CS2CContainerObjectMove::PEI_PACK5)
					{
						pVolContainer=pMsg->GetPlayer()->getSubpackContainer()->GetSubpack(wPlace-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer;
					}

					if(pVolContainer)
					{
						if(bytePos>=0 && bytePos<pVolContainer->GetVolume())
						{
							CGUID goodsGuid=pVolContainer->GetCellGuid()[bytePos];
							CGoods* pGoods=pVolContainer->GetGoods(bytePos);
							if(pGoods && pGoods->GetGoodsBaseType()==GBT_CONSUMABLE)
							{
#ifdef _GOODSLOG1_
								char pszGoodsLog[1024]="";			
								char srcGoodsGUID[50]="";
								char serGoodsGUID[50]="";
								objGuid.tostring(srcGoodsGUID);
								goodsGuid.tostring(serGoodsGUID);
								_snprintf(pszGoodsLog,1024,"[MSG_C2S_PLAYER_USEITEM][容器:%d][位置:%d][TYPE:%d][GUID:%s][GUID-SERVER:%s]",
									wPlace,bytePos,lType,srcGoodsGUID,serGoodsGUID);
								CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
								dwGoodsId=pGoods->GetBasePropertiesIndex();
								pMsg -> GetPlayer() -> UseItem(wPlace, bytePos ,lType,objGuid);
							}
						}						
					}					
				}
			}
		}
		break;

	case MSG_C2S_PLAYER_PKSWITCH:
		{
			// 据点战功能开启时不能修改PK状态
			CServerRegion* pRgn = dynamic_cast<CServerRegion*>(GetFather());
			if(pRgn && pRgn->GetStrongPointFlag() == true) return;

			BYTE lPos = pMsg->GetByte();
			bool bPk = pMsg->GetByte()==0?false:true;
			switch(lPos)
			{
				case 0:
					SetPk_Normal(bPk);				
					break;
				case 1:
					SetPk_Team(bPk);				
					break;
				case 2:
					SetPk_Union(bPk);				
					break;
				case 3:
					SetPk_Badman(bPk);				
					break;
				case 4:
					SetPk_Country(bPk);				
					break;
				case 5:
					SetPk_Camp(bPk);
					break;
				case 6:
					SetPk_Pirate(bPk);
					break;
			}
		}
		break;

	case MSG_C2S_PLAYER_HELPREQUEST://玩家请求帮助文档
		{
			//stRunScript st;
			//st.pszFileName = "scripts/help/help.script";
			//st.desShape = this;
			//st.pRegion = pMsg->GetRegion();
			//st.srcShape = NULL;
			//RunScript(&st, (char*)GetGame()->GetScriptFileData(st.pszFileName));
			Script::RunEnv env( NULL, this, pMsg->GetRegion() );
			GetInst( ScriptSys ).RunScript( env, "scripts/help/help.lua" );
		}
		break;
	case MSG_C2S_PLAYER_QUEST_RELIVE:
		{

			CPlayer *pPlayer = pMsg->GetPlayer();
			if(!pPlayer->IsDied())
				return;
			BYTE bRliveType = pMsg->GetByte();
			if (bRliveType==0)
			{
				//回城复活
				pPlayer->OnRelive(0);
			} 
			else
			{
				//原地复活
				string strLv = "Level";
				if (pPlayer->GetAttribute(strLv)<=10)
				{
					pPlayer->OnRelive(1);
				}
				else
				{
					
					string str=GlobalSetup::GetSetup()->szReliveGoodsName;
					DWORD dwIndex = GoodsSetup::QueryGoodsIDByOriginalName(str.c_str());

					if (pPlayer->GetGoodsAmountByOriginName(str.c_str())>=1)
					{
						//原地复活
						pPlayer->DelGoods((char*)str.c_str(),1);
						OnRelive(1);
						CMessage msg(MSG_S2C_PLAYER_RELIVE_RESULT);
						msg.Add(BYTE(1));//成功
						msg.Add(dwIndex);
						msg.Add(pPlayer->GetExID());

					} 
					//tag
					//else		
					//{
					//	//星钻预冻																		
					//	CMessage msg(MSG_S2B_ACCOUNT_LOCK);
					//	msg.Add((BYTE)eICT_USE_FUNCTION); //消费类型
					//	msg.Add(pPlayer->GetExID());	//player guid
					//	msg.Add(pPlayer->GetAccount());	//player cdkey
					//	msg.Add(NULL_GUID);				//attach guid
					//	msg.Add((DWORD)1);
					//	msg.Add((DWORD)eIFUNC_RELIVE);			//goods id
					//	msg.Add((DWORD)1);					//goods num
					//	msg.Add(GlobalSetup::GetSetup()->lRelivePrice);			//money sum
					//	msg.SendToBS();
					//}
					/*
					else
					{				
						CMessage msg(MSG_S2C_PLAYER_RELIVE_RESULT);
						msg.Add(BYTE(0));//失败
						msg.Add(dwIndex);
						msg.SendToPlayer(pPlayer->GetExID());// Add(pPlayer->GetExID());
					}*/

				}
				

			}						
		}
		break;
	case MSG_C2S_PLAYER_SKILL_RELIVE:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (pPlayer && pPlayer->GetReliveSkill())
			{
				long lRecoverRatio = pMsg->GetLong();
				pPlayer->SetReliveSkill(false);				
				pPlayer->OnRelive(1,lRecoverRatio);
			}			
		}
		break;
	case MSG_C2S_PLAYER_SKILL_CANCEL_RELIVE:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if (pPlayer && pPlayer->IsDied())
			{
				pPlayer->FreeReliveScript();

				const char* strScript = GlobalSetup::GetSetup()->pszPlayerDiedScript;
				if(strScript)
				{
					//stRunScript sScript;
					//sScript.pszFileName = (char*)strScript;
					//sScript.desShape = NULL;
					//sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					//sScript.srcShape	= this;
					//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
					Script::RunEnv env( this, NULL, static_cast<CRegion*>( pPlayer->GetFather() ) );
					GetInst( ScriptSys ).RunScript( env, strScript );
				}
			}
		}
		break;

	case MSG_C2S_PLAYER_UpdateShowFashion:
		{
			if(pMsg->GetPlayer())
			{
				BYTE bShowFashion = pMsg->GetByte();
				
				pMsg->GetPlayer()->SetShowFashion(bShowFashion);

				CMessage msg(MSG_S2C_PLAYER_UpdateShowFashion);
				msg.Add(pMsg->GetPlayer()->GetExID());
				msg.Add(bShowFashion);
				msg.SendToAround(pMsg->GetPlayer(),NULL);
			}
		}
		break;
	case MSG_C2S_PLAYER_CHANGE_TITLE_TYPE:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			LONG lNewTitleType = pMsg->GetLong();
			if(NULL != pPlayer)
			{
				pPlayer->SetTitleType(lNewTitleType);
			}
		}
		break;
	}
}