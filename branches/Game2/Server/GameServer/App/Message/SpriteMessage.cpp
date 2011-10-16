#include "StdAfx.h"

#include "../Player.h"
#include "../Region.h"
#include "../SpriteSystem.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../../../../Public/Setup/GlobalSetup.h"


//! 响应客户端的小精灵消息
void OnSpriteMessage_ForClient(CMessage *pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_C2S_SPRITE_AutoSupply_OnOff_Request://! （暂不使用）自动补给系统：功能开关请求
		{

		}
		break;
	case MSG_C2S_SPRITE_AutoSupply_ChangeSetup_Request://! 自动补给系统：修改一个补给设置请求
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				pPlayer->m_SpriteSystem.OnChangeAutoSupply(pMsg);
				
			}	
		}
		break;

	//!-------------------------------------------------------------------------------------------
	//! 非小精灵系统消息
	case MSG_C2S_SaveNewSetupData_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				tagSetupOnServer SetupOnServer;
				pMsg->GetEx(&SetupOnServer, sizeof(tagSetupOnServer));
				BOOL bRevert = pPlayer->m_SetupOnServer.SetSetup(SetupOnServer);
				CMessage msg(MSG_S2C_SaveNewSetupData_Re);
				msg.Add((LONG)bRevert);
				msg.SendToPlayer(pPlayer->GetExID());
			}
		}
		break;
		  //! 请求打开日程表
	case	MSG_C2S_Calendar_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = GlobalSetup::GetSetup()->pszCalendar;
				if(strScript)
				{
					//stRunScript sScript;
					//sScript.pszFileName = (char*)strScript;
					//sScript.desShape = pPlayer;
					//sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					//sScript.srcShape	= pPlayer;
					//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
					Script::RunEnv env( pPlayer, pPlayer, static_cast<CRegion*>( pPlayer->GetFather() ) );
					GetInst( ScriptSys ).RunScript( env, strScript );
				}
			}
		}
		break;
			//! 请求打开迦纳维魔盒
	case	MSG_C2S_JanaweBox_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = GlobalSetup::GetSetup()->pszMagicBox;
				if(strScript)
				{
					//stRunScript sScript;
					//sScript.pszFileName = (char*)strScript;
					//sScript.desShape = pPlayer;
					//sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					//sScript.srcShape	= pPlayer;
					//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
					Script::RunEnv env( pPlayer, pPlayer, static_cast<CRegion*>( pPlayer->GetFather() ) );
					GetInst( ScriptSys ).RunScript( env, strScript );
				}
			}
		}
		break;
			//! 问答与奖励
	case	MSG_C2S_Answer_Request:
		{
			CPlayer *pPlayer = pMsg->GetPlayer();
			if(NULL != pPlayer)
			{
				const char* strScript = GlobalSetup::GetSetup()->pszAnswerRequest;
				if(strScript)
				{
					//stRunScript sScript;
					//sScript.pszFileName = (char*)strScript;
					//sScript.desShape = pPlayer;
					//sScript.pRegion =  dynamic_cast<CRegion*>(pPlayer->GetFather());
					//sScript.srcShape	= pPlayer;
					//RunScript( &sScript, const_cast<CHAR*>( GetGame() -> GetScriptFileData(strScript) ) );
					Script::RunEnv env( pPlayer, pPlayer, static_cast<CRegion*>( pPlayer->GetFather() ) );
					GetInst( ScriptSys ).RunScript( env, strScript );

				}
			}
		}
	break;

	}
}