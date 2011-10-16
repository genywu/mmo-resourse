//=============================================================================
/**
 *  file: GlobalFun.cpp
 *
 *  Brief:定义了全局、系统的相关脚本函数
 *			
 *
 *  Authtor:wangqiao
 *	
 *	Create Datae:2008-11-06
 */
//=============================================================================

#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "GlobalScriptTimer.h"
#include "variablelist.h"
#include "../player.h"
#include "../serverregion.h"
#include "../../../setup/LogSystem.h"
#include "../../GameServer/GameManager.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "../setup/GMList.h"
#include "../RgnManager.h"

double CScript::FormatGUID(const char* CmdStr, char* retStr)
{
	char * pszGuid = GetStringParam(CmdStr, 0);
	char * pszGuidString = GetStringParam(CmdStr, 1);
	if(pszGuid && pszGuidString)
	{
		CGUID temptGUID(pszGuidString);
		SetScriptGuidValue((CMoveShape*)p_SrcShape,pszGuid,temptGUID);
	}

	M_FREE( pszGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pszGuidString, sizeof(char)*MAX_VAR_LEN );
	return 1;
}

double CScript::ListOnlineGM(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	CPlayer* pPlayer;
	tagGMInfo*	pGM;
	int iCount=0;
	char buf[64];
	for (map<string, tagGMInfo>::iterator it=CGMList::GetGMInfoMap()->begin(); it!=CGMList::GetGMInfoMap()->end(); it++)
	{
		pGM = &it->second;				
		pPlayer= GetGame()->FindPlayer(pGM->strName.c_str());	//得到对象指针
		if( pPlayer )
		{
			iCount++;

			switch(pGM->lLevel)
			{
			case GM_ADMIN:
				_snprintf(buf, 64, "%s (系统管理员)",pGM->strName.c_str());
				break;
			case GM_ARCH:
				_snprintf(buf, 64, "%s (游戏管理员)",pGM->strName.c_str());
				break;
			case GM_WIZARD:
				_snprintf(buf, 64, "%s (巫师)",pGM->strName.c_str());
				break;
			}

			((CPlayer*)p_SrcShape) -> SendNotifyMessage( buf );
		}
	}
	//return iCount;			
	CMessage msg(MSG_S2W_GM_QUERYGM);
	msg.Add(p_SrcShape->GetExID());
	msg.Send();

	return 1;
}
double CScript::ListSilencePlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	CMessage msg( MSG_S2W_GM_QUERY_SILENCE_PLAYERS );
	msg.Add( p_SrcShape -> GetExID() );
	msg.Send();
	return 1;
}

double CScript::ListBanedPlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double lPlayers = -1;	
	CMessage msg( MSG_S2W_GM_QUERY_PLAYERCOUNT );
	msg.Add( p_SrcShape -> GetExID() );
	msg.Add(GetID());
	msg.Send();

	/**# 脚本暂停执行,等待返回结果 **/
	SetIsHang(true);
	SetHangFunc(SR_FUNC_LISTBANEDPLAYER);
	return GetGame()->GetPlayerCount();
}
double CScript::GetOnlinePlayers(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double lPlayers = -1;	
	CMessage msg( MSG_S2W_GM_QUERY_PLAYERCOUNT );
	msg.Add( p_SrcShape -> GetExID() );
	msg.Add(GetID());
	msg.Send();

	/**# 脚本暂停执行,等待返回结果 **/
	SetIsHang(true);
	SetHangFunc(SR_FUNC_GETONLINEPLAYER);
	return GetGame()->GetPlayerCount();
}

double CScript::PostRegionInfo(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* pMessage = GetStringParam( CmdStr, 0 );
	if( pMessage )
	{
		DWORD dwColor = static_cast<DWORD>( GetIntParam(CmdStr, 1) );
		DWORD dwBkColor = 0;

		if( dwColor == ERROR_CODE )
			dwColor = 0xFFFFFFFF;
		else
		{
			dwBkColor = static_cast<DWORD>(GetIntParam(CmdStr, 2));
			if (dwBkColor == ERROR_CODE)
				dwBkColor = 0;
		}

		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
		if( pRegion )
		{
			CMessage msg( MSG_S2C_OTHER_ADDINFO );
			msg.Add( dwColor );
			msg.Add( dwBkColor );
			msg.Add((BYTE)eNOTIFYPOS_LEFT_BOTTOM);
			if(pMessage[0] != '@') // 不是GUID变量
				msg.Add( pMessage );
			else
			{
				char szGUID[128];
				CGUID guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, pMessage);
				guid.tostring(szGUID);
				msg.Add(szGUID);
			}
			BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_PLAYER)
			{
				msg.SendToPlayer( pShape -> GetExID() );
			}
			END_LOOP_REGION_SHAPE_BY_TYPE
		}
	}
	M_FREE( pMessage, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::PostRegionInfoByIndex(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	int ePos =  GetIntParam( CmdStr, 0 );
	if (ePos== INVILID_VALUE && ePos==ERROR_CODE)
		return INVILID_VALUE;
	long TextType =GetIntParam(CmdStr ,1);
	if (ePos== INVILID_VALUE && ePos==ERROR_CODE)
		return INVILID_VALUE;
	LONG TextIndex = GetIntParam( CmdStr, 2 );
	if( TextIndex!= INVILID_VALUE && TextIndex!=ERROR_CODE )
	{
		DWORD dwColor = static_cast<DWORD>( GetIntParam(CmdStr, 3) );
		DWORD dwBkColor = 0;

		if( dwColor == ERROR_CODE )
			dwColor = 0xFFFFFFFF;
		else
		{
			dwBkColor = static_cast<DWORD>(GetIntParam(CmdStr, 4));
			if (dwBkColor == ERROR_CODE)
				dwBkColor = 0;
		}

		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
		if( pRegion )
		{
			//这里MSG_S2C_OTHER_ADDINFO消息格式不对！！
			CMessage msg( MSG_S2C_OTHER_ADDINFO_BYINDEX);
			msg.Add( dwColor );
			msg.Add( dwBkColor );
			msg.Add((BYTE)ePos);
			msg.Add( TextType );
			msg.Add( TextIndex );
			BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_PLAYER)
			{
				msg.SendToPlayer( pShape -> GetExID() );
			}
			END_LOOP_REGION_SHAPE_BY_TYPE
		}
	}

	return 0;
}

double CScript::PostWorldInfo(const char* CmdStr, char* retStr)
{
	char* pMessage = GetStringParam( CmdStr, 0 );
	if( pMessage )
	{
		DWORD dwColor = static_cast<DWORD>( GetIntParam(CmdStr, 1) );
		DWORD dwBkColor = 0;
		if( dwColor == ERROR_CODE )
			dwColor = 0xFFFFFFFF;
		else
		{
			dwBkColor = static_cast<DWORD>( GetIntParam(CmdStr, 2));
			if (dwBkColor == ERROR_CODE)
				dwBkColor = 0;
		}

		/**## MSG_S2W_GM_WORLD_MESSAGE, MSG_W2S_GM_WORLD_MESSAGE */
		/**## 格式：double lGMId, char* pMsgBody, DWORD dwColor */
		CMessage msg( MSG_S2W_GM_WORLD_MESSAGE );

		if(p_SrcShape)
			msg.Add( p_SrcShape -> GetExID() );
		else
			msg.Add(NULL_GUID);
		if(pMessage[0] != '@') // 不是GUID变量
			msg.Add( pMessage );
		else
		{
			char szGUID[128];
			CGUID guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, pMessage);
			guid.tostring(szGUID);
			msg.Add(szGUID);
		}
		msg.Add( dwColor );
		msg.Add( dwBkColor );
		msg.Send();
	}
	M_FREE( pMessage, sizeof(char)*MAX_VAR_LEN );

	return 0;
}

double CScript::PostWorldInfoByIndex(const char* CmdStr, char* retStr)
{
	return 0;
}

double CScript::PostCountryInfo(const char* CmdStr, char* retStr)
{
	char *content = GetStringParam(CmdStr, 0);
	double cid = GetIntParam(CmdStr,1);

	if( !content ) return 0;

	if( cid==ERROR_CODE )
	{
		M_FREE( content, sizeof(char)*MAX_VAR_LEN );
		return 0;	
	}

	DWORD dwColor = static_cast<DWORD>( GetIntParam(CmdStr, 2) );
	DWORD dwBkColor = 0;
	if( dwColor == ERROR_CODE )
		dwColor = 0xFFFFFFFF;
	else
	{
		dwBkColor = static_cast<DWORD>( GetIntParam(CmdStr, 3));
		if (dwBkColor == ERROR_CODE)
			dwBkColor = 0;
	}

	/**## MSG_S2W_GM_WORLD_MESSAGE, MSG_W2S_GM_WORLD_MESSAGE */
	/**## 格式：double lGMId, char* pMsgBody, DWORD dwColor */
	CMessage msg( MSG_S2W_GM_COUNTRY_MESSAGE );
	if(p_SrcShape)
		msg.Add( p_SrcShape -> GetExID() );
	else
		msg.Add(NULL_GUID);

	if(content[0] != '@') // 不是GUID变量
		msg.Add( content );
	else
	{
		char szGUID[128];
		CGUID guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, content);
		guid.tostring(szGUID);
		msg.Add(szGUID);
	}

	msg.Add( (long)cid );
	msg.Add( dwColor );
	msg.Add( dwBkColor );
	msg.Send();
	M_FREE( content, sizeof(char)*MAX_VAR_LEN );
	return 1;
}

double CScript::RollRegionInfo(const char* CmdStr, char* retStr)	
{
	double dRet = 0.0;
	if(p_SrcShape == NULL) return 0;

	long lRegionID = GetIntParam(CmdStr,0);
	long lRgnType = GetIntParam(CmdStr,1);
	long lNum = GetIntParam(CmdStr,2);
	char *pMessage = GetStringParam( CmdStr,3);
	long lFileNum = GetIntParam(CmdStr,4);

	if (lNum== ERROR_CODE || lNum==INVILID_VALUE || lNum<0)
	{
		lNum = 0;
	}
	if (lFileNum==ERROR_CODE || lFileNum==INVILID_VALUE)
	{
		lFileNum = 0;
	}

	if( pMessage)
	{
		CServerRegion* pRegion = NULL;
		if (lRegionID==0 || lRegionID==ERROR_CODE || lRegionID==INVILID_VALUE || lRgnType==ERROR_CODE || lRgnType==INVILID_VALUE)
		{
			pRegion = dynamic_cast<CServerRegion*>( p_SrcShape -> GetFather() );
		}
		else 
		{
			pRegion  =  GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID((eRgnType)lRgnType, lRegionID);
		}

		if(pMessage[0]=='#') // 不是GUID变量			
		{
			pMessage = (char*)GetScriptVarValue((CMoveShape*)p_SrcShape, pMessage);
		}

		if( pRegion && lRgnType!=RGN_TEAM)
		{
			//本场景在本服务器
			CMessage msg( MSG_S2C_OTHER_NEWS_TOP_WORLD );
			msg.Add(lNum);
			msg.Add(pMessage);
			msg.Add(lFileNum);

			BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion, TYPE_PLAYER)
			{
				msg.SendToPlayer( pShape -> GetExID() );
			}
			END_LOOP_REGION_SHAPE_BY_TYPE
		}
		else
		{
			CMessage msg(MSG_S2W_OTHER_NEWS_TOP_REGION);
			msg.Add(lRegionID);
			msg.Add(lRgnType);
			msg.Add(lNum);			
			msg.Add(pMessage);
			msg.Add(lFileNum);
			msg.Send();
		}
		dRet = 1.0;	
		M_FREE( pMessage, sizeof(char)*MAX_VAR_LEN );
	}


	return dRet;
}
double CScript::RollWorldInfo(const char* CmdStr, char* retStr)	
{
	double dRet = 0.0;
	long lNum = GetIntParam(CmdStr,0);
	char *strNews = GetStringParam(CmdStr,1);
	long lFileNum = GetIntParam(CmdStr,2);
	if (lNum==ERROR_CODE || lNum==INVILID_VALUE || lNum<0)
		lNum = 0;
	if (lFileNum==ERROR_CODE || lFileNum==INVILID_VALUE)
	{
		lFileNum = 0;
	}

	if (strNews)
	{	
		if(strNews[0]=='#') // 不是GUID变量			
		{
			strNews = (char*)GetScriptVarValue((CMoveShape*)p_SrcShape, strNews);
		}
		
		CMessage msg(MSG_S2W_OTHER_NEWS_TOP_WORLD);
		msg.Add(lNum);
		msg.Add(strNews);
		msg.Add(lFileNum);
		msg.Send();
	}
	M_FREE( strNews, sizeof(char)*MAX_VAR_LEN );
	return dRet;
}


double CScript::KickAllPlayer(const char* CmdStr, char* retStr)
{
	/**## 需不需要做一次GM权限的验证？ */
	CMessage msg( MSG_S2W_GM_KICK_ALLPLAYER );
	if(p_SrcShape)
		msg.Add( p_SrcShape -> GetExID() );
	else
		msg.Add(NULL_GUID);
	msg.Send();
	return 1;
}
double CScript::KickRegionPlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	/**## 需不需要做一次GM权限的验证？ */
	char* rgnID	= GetStringParam( CmdStr, 0 );
	if(!rgnID) return 0;

	CGUID lRegionID(rgnID);

	if (lRegionID == NULL_GUID)
	{
		M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CServerRegion* pRegion = GetGame() -> FindRegion( lRegionID );
	/**## 首先验证场景是否在本服务器 */
	if( pRegion )
	{
		BEGIN_LOOP_REGION_SHAPE_BY_TYPE(pRegion,TYPE_PLAYER)
		{
			if(pShape->GetExID()!=p_SrcShape->GetExID())
			{
				GetGame()->KickPlayer(pShape->GetExID());
			}
		}
		END_LOOP_REGION_SHAPE_BY_TYPE
	}
	/**## 否则发消息给World Server */
	else 
	{
		/**## MSG_S2W_GM_KICK_REGIONPLAYER 和 MSG_W2S_GM_KICK_REGIONPLAYER 格式 */
		/**## double lGMPlayerID, char* strRegionName */
		CMessage msg( MSG_S2W_GM_KICK_REGIONPLAYER );
		msg.Add( p_SrcShape -> GetExID() );
		msg.Add( lRegionID );
		msg.Send();
	}
	M_FREE( rgnID, sizeof(char)*MAX_VAR_LEN );
	return 1;
}

//注册一个全局定时器
double CScript::RegisterGlobalTimer(const char* cmd, char* retStr)
{
	int nRet = 0;
	long lfuturetime = static_cast<long>(GetIntParam(cmd, 0));
	if(lfuturetime != ERROR_CODE && lfuturetime != INVILID_VALUE)
	{
		long lInterTime = static_cast<long>(GetIntParam(cmd, 1));
		if(lInterTime != ERROR_CODE && lInterTime != INVILID_VALUE)
		{
			char* pszScriptName=GetStringParam(cmd,2);
			if( pszScriptName )
			{
				long lUserData = static_cast<long>(GetIntParam(cmd, 3));
				if(lUserData != ERROR_CODE && lUserData != INVILID_VALUE)
				{
					nRet = CGlobalScriptTimer::GetInstance()->RegisterTimer(lfuturetime,
						lInterTime,
						pszScriptName,
						lUserData);
				}
				M_FREE( pszScriptName, sizeof(char)*MAX_VAR_LEN );
			}
		}
	}
	return nRet;
}

double CScript::UnRegisterGlobalTimer(const char* cmd, char* retStr)
{
	int nRet = 0;
	long lUserData = static_cast<long>(GetIntParam(cmd, 0));
	if(lUserData != ERROR_CODE && lUserData != INVILID_VALUE)
	{
		long lTimerID = static_cast<long>(GetIntParam(cmd, 1));
		if(lTimerID != ERROR_CODE && lTimerID != INVILID_VALUE)
		{
			nRet = CGlobalScriptTimer::GetInstance()->UnRegisterTimer(lUserData,lTimerID);
		}
	}
	return nRet;
}


double CScript::KickPlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* pName = GetStringParam( CmdStr, 0 );
	if( pName )
	{
		CPlayer* pPlayer = GetGame() -> FindPlayer( pName );
		if( pPlayer )
			/**## 玩家在当前服务器上 */
		{
			GetGame()->KickPlayer(pPlayer->GetExID());
			char szText[64];
			_snprintf( szText, 64, "玩家%s已经被踢出游戏。", pName );
			((CPlayer*)p_SrcShape) -> SendNotifyMessage( szText );
			M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
			return 1;
		}
		else
			/**## 玩家不在当前服务器上 */
		{
			/**## MSG_S2W_GM_KICKPLAYER 消息格式： */
			/**## double SenderID, char* strPlayer */
			/**## 此消息服务器会通知是否成功 */
			CMessage msg( MSG_S2W_GM_KICKPLAYER );
			msg.Add( p_SrcShape -> GetExID() );
			msg.Add( pName );
			msg.Send();
		}
		M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
	}
	
	return 1;
}

double CScript::KickAround(const char* cmd, char* retStr)
{
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;
	/*
	*找7×7格子的玩家
	*/
	int nStartX=0,nStartY=0;
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
	nStartX=pPlayer->GetTileX() - 3;
	nStartY=pPlayer->GetTileY() - 3;
	if(pPlayer->GetTileX()+3 >= pRegion->GetWidth())
	{
		nStartX=pRegion->GetWidth() - 7;
	}
	else if(pPlayer->GetTileX()-3 <= 0)
	{
		nStartX=0;
	}
	if(pPlayer->GetTileY()+3 >= pRegion->GetHeight())
	{
		nStartY=pRegion->GetHeight() - 7;
	}
	else if(pPlayer->GetTileY()-3 <= 0)
	{
		nStartY=0;
	}
	list<CPlayer*> playersList;
	CShape* pShape=NULL;
	for(int i=nStartX;i<nStartX+7;i++)
	{
		for(int j=nStartY;j<nStartY+7;j++)
		{
			pShape=pRegion->GetShape(i,j);
			if(pShape && pShape->GetType()==TYPE_PLAYER && pShape->GetExID() != pPlayer->GetExID())
			{
				playersList.push_back(dynamic_cast<CPlayer*>(pShape));
			}
		}
	}
	playersList.unique();
	list<CPlayer*>::iterator PlayerIter=playersList.begin();
	for(;PlayerIter!=playersList.end();PlayerIter++)
	{
		GetGame()->KickPlayer((*PlayerIter)->GetExID());
	}

	return 1;
}

double CScript::BanPlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* pName=GetStringParam(CmdStr,0);
	if (pName)
	{
		CPlayer* pPlayer = GetGame()->FindPlayer(pName);
		if(pPlayer)
		{
			GetGame()->KickPlayer(pPlayer->GetExID());
			//GetGame()->OnPlayerExit(pPlayer->GetExID());
		}

		double lTime=GetIntParam(CmdStr,1);
		CMessage msg(MSG_S2W_GM_BANPLAYER);
		msg.Add(p_SrcShape->GetExID()); //gm id
		msg.Add(pName);
		msg.Add((long)lTime);
		msg.Send(); //send to world

		M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
	}

	return 1;
}
double CScript::SilencePlayer(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	long lRet = 0;

	char* pName = GetStringParam( CmdStr, 0 );
	if( pName )
	{
		double lTime = GetIntParam( CmdStr, 1 );
		if( lTime == ERROR_CODE )
			lTime = 1;
		lTime = lTime*60000;
		CPlayer* pPlayer = GetGame() -> FindPlayer( pName );
		if( pPlayer )
		{
			pPlayer -> SetSilence( lTime);
			lRet = 1;
		}
		else
		{
			CMessage msg( MSG_S2W_GM_SILENCE_PLAYER );
			if(p_SrcShape)
				msg.Add( p_SrcShape -> GetExID() );
			else
				msg.Add(NULL_GUID);
			msg.Add(GetID());
			msg.Add( pName );
			msg.Add( (long)lTime );
			msg.Send();
			SetIsHang(true);
			SetHangFunc(SR_FUNC_SILENCEPLAYER);
		}
		M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
	}
	return lRet;
}


double CScript::GetTextRes(const char* CmdStr, char* retStr/* =NULL */)
{
	double dRet = 0;
	DWORD dwTypeID = GetIntParam(CmdStr,0);
	DWORD dwID = GetIntParam(CmdStr,1);
	char* strRes = GetStringParam(CmdStr,2);

	if (dwID!=ERROR_CODE && dwTypeID!=ERROR_CODE && dwID!=INVILID_VALUE && dwTypeID!=INVILID_VALUE  && strRes!=NULL)
	{
		dRet = SetScriptVarValue((CMoveShape*)p_SrcShape, strRes, CStringReading::LoadString(dwTypeID,dwID));
	}	
	M_FREE( strRes, sizeof(char)*MAX_VAR_LEN );
	return dRet;
}

double CScript::SetPlayerRegion(const char* CmdStr, char* retStr)
{
	eRgnType rgnType = (eRgnType)(long)GetIntParam(CmdStr, 0);

	char* strPlayer	= GetStringParam( CmdStr, 1 );
	if (strPlayer==NULL)
		return 0;

	char* tGuid = GetStringParam(CmdStr,2);
	if(!tGuid) 
	{
		M_FREE( strPlayer, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CGUID lRegionID = GetScriptGuidValue((CMoveShape*)p_SrcShape, tGuid);
	double lX			= GetIntParam( CmdStr, 3 );
	double lY			= GetIntParam( CmdStr, 4 );
	double dir		= GetIntParam(CmdStr,5);
	double lRange		= GetIntParam(CmdStr, 6);

	if(lX==0 && lY==0)	//如果不行,则在全地图选随机空坐标
	{
		lX = -1;
		lY = -1;
	}

	CPlayer* pPlayer = NULL;

	if (strPlayer[0]==0)
		pPlayer = ((CPlayer*)this->GetSrcShape());
	else
		pPlayer = GetGame() -> FindPlayer( strPlayer );

	if( dir == ERROR_CODE )
	{
		if (pPlayer)
			dir = pPlayer->GetDir();
		else
			dir = -1;
	}
	if (lRange==ERROR_CODE)
	{
		lRange = 2;
	}

	if( lRegionID != NULL_GUID && pPlayer )
	{
		if( lX == ERROR_CODE || lY == ERROR_CODE )
		{
			lX = lY = -1;
		}
		pPlayer -> ChangeRegion(rgnType, lRegionID, lX, lY, dir, 0, lRange );
	}

	if(!pPlayer && strPlayer)
	{
		CMessage msg(MSG_S2W_GM_CHANGEREGION);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add((long)strPlayer);
		msg.Add(lRegionID);
		msg.Add((long)lX);
		msg.Add((long)lY);
		msg.Send();
	}

	M_FREE( tGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strPlayer, sizeof(char)*MAX_VAR_LEN );
	return 1;
}
double CScript::SetPlayerRegionEx(const char* CmdStr, char* retStr)
{
	// rgn type
	eRgnType type = (eRgnType)(long)GetIntParam(CmdStr, 0);
	char* strPlayer	= GetStringParam( CmdStr, 1 );
	char* tGuid = GetStringParam(CmdStr,2);

	double lX			= GetIntParam( CmdStr, 3 );
	double lY			= GetIntParam( CmdStr, 4 );

	if(strPlayer && tGuid)
	{
		CGUID lRegionID = GetScriptGuidValue((CMoveShape*)p_SrcShape, tGuid);
		CPlayer* pPlayer=GetGame()->FindPlayer(strPlayer);
		if( lRegionID != NULL_GUID && pPlayer )
		{
			int nStartX=0,nStartY=0;
			CServerRegion* pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
			nStartX=pPlayer->GetTileX() - 3;
			nStartY=pPlayer->GetTileY() - 3;
			if(pPlayer->GetTileX()+3 >= pRegion->GetWidth())
			{
				nStartX=pRegion->GetWidth() - 7;
			}
			else if(pPlayer->GetTileX()-3 <= 0)
			{
				nStartX=0;
			}
			if(pPlayer->GetTileY()+3 >= pRegion->GetHeight())
			{
				nStartY=pRegion->GetHeight() - 7;
			}
			else if(pPlayer->GetTileY()-3 <= 0)
			{
				nStartY=0;
			}
			list<CPlayer*> playersList;
			CShape* pShape=NULL;
			for(int i=nStartX;i<nStartX+7;i++)
			{
				for(int j=nStartY;j<nStartY+7;j++)
				{
					pShape=pRegion->GetShape(i,j);
					if(pShape && pShape->GetType()==TYPE_PLAYER)
					{
						playersList.push_back(dynamic_cast<CPlayer*>(pShape));
					}
				}
			}
			playersList.unique();
			list<CPlayer*>::iterator PlayerIter=playersList.begin();
			for(;PlayerIter!=playersList.end();PlayerIter++)
			{
				(*PlayerIter)->ChangeRegion(type,lRegionID,lX,lY,(*PlayerIter)->GetDir());
			}
		}
	}
	M_FREE( tGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strPlayer, sizeof(char)*MAX_VAR_LEN );
	return 1;
}


double CScript::GetPlayerRegion(const char* CmdStr, char* retStr)
{
	long lRet = 0;
	char* pszPlayerName	= GetStringParam( CmdStr, 0 );
	if (pszPlayerName==NULL)
		return 0;
	

	CPlayer* pPlayer = NULL;
	if( strlen(pszPlayerName)==0 )
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame() -> FindPlayer( pszPlayerName );

	if( pPlayer )
	{
		const CGUID& RegionID = pPlayer->GetRegionGUID();
		long lX = pPlayer->GetTileX();
		long lY = pPlayer->GetTileY();

		 SetScriptGuidValue(pPlayer,"@PlayerRegionID",RegionID);
		 SetScriptVarValue(pPlayer,"$PlayerRegionX",0, lX);
		 SetScriptVarValue(pPlayer,"$PlayerRegionY",0, lY);
		 lRet = 1;
	}
	else
	{
		CMessage msg(MSG_S2W_GM_GETPLAYER_REGION);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(GetID());
		msg.Add(pszPlayerName);
		msg.Send();

		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETPLAYERREGION);
	}
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangePlayerRegion(const char* CmdStr, char* retStr)
{
	long lRet = 0;
	CPlayer* pSrcPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pSrcPlayer)	return 0;

	char* pszPlayerName	= GetStringParam( CmdStr, 0 );
	if (pszPlayerName==NULL)
		return 0;
	char* pszRegionGuid = GetStringParam(CmdStr,1);
	if(!pszRegionGuid)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	long lX = GetIntParam( CmdStr, 2 );
	long lY = GetIntParam( CmdStr, 3 );
	if(lX== ERROR_CODE || lY == ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( pszRegionGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	CGUID RegionID = GetScriptGuidValue(pSrcPlayer,pszRegionGuid);

	CPlayer *pPlayer = NULL;
	if(strlen(pszPlayerName) == 0)
	{
		pPlayer = pSrcPlayer;
	}
	else
	{
		pPlayer =  GetGame() -> FindPlayer( pszPlayerName );
	}
	if(pPlayer)
	{
		pPlayer->ChangeRegion(RGN_ALL,RegionID,lX,lY,0);
		lRet = 1;
	}
	else
	{
		CMessage msg(MSG_S2W_GM_CHANGETPLAYER_REGION);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(GetID());
		msg.Add(pszPlayerName);
		msg.Add(RegionID);
		msg.Add(lX);
		msg.Add(lY);
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SR_FUNC_CHANGEPLAYERREGION);
	}
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pszRegionGuid, sizeof(char)*MAX_VAR_LEN );

	return lRet;
}
double CScript::ChangePlayerName(const char* cmd, char* retStr)
{
	if(!GetSrcShape() || GetSrcShape()->GetType() != TYPE_PLAYER)
	{
		return 0.0f;
	}

	CPlayer* pPlayer = static_cast<CPlayer*>(GetSrcShape());
	if(!pPlayer) return 0.0f;

	const char* pStr = strchr(pPlayer->GetName(), CGlobeSetup::GetSetup()->strSpeStr[0]);
	if(!pStr)
	{
		// 回应客户端消息
		CMessage retmsg(MSG_S2C_OTHER_CHANGENAME);
		retmsg.Add(pPlayer->GetExID());
		retmsg.Add(pPlayer->GetName());
		retmsg.Add((BYTE)-2); // 名字不合法
		retmsg.SendToPlayer(pPlayer->GetExID());
		return 0.0f;
	}

	DelPlayerTalkBoxScript(m_guSrcID, SR_FUNC_UPDATENAME);

	//等待函数, 收到回应消息再运行脚本
	SetIsHang(true);
	SetHangFunc(SR_FUNC_UPDATENAME);
	
	CMessage msg(MSG_S2C_OTHER_UPDATENAMEOPEN);
	msg.Add(GetID()); // scriptid
	msg.SendToPlayer(GetSrcShape()->GetExID());
	return 1.0f;
}