#include "StdAfx.h"
#include "Script.h"
#include "../player.h"
#include "../AntiCheat/CPlayerManager.h"

#define CHECK_SHAPE( ret ) \
	if( this->p_SrcShape == NULL || \
	dynamic_cast<CPlayer*>( this->p_SrcShape ) == NULL ) \
	{ \
		return ret; \
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/// 函数功能:     添加改变玩家活跃度的脚本函数
/// 函数脚本原型：$retActivity = ChangeActivity( "@PlayerGUID", $changeActivity );
/// 函数参数:     @PlayerGUID : 玩家GUID
///               $changeActivity : 改变的活跃度值
/// 函数返回值:   改变后的活跃度值
///////////////////////////////////////////////////////////////////////////////////////////////////////
double CScript::ChangeActivity(const char *cmd, char *retStr /* = NULL */)
{
	CHECK_SHAPE( 0 );

	long lRet = 0;
	char *pszGUID = GetStringParam(cmd, 0);
	if(!pszGUID) 
		return lRet;


	long nAnctivity = GetIntParam(cmd, 1);
	if(ERROR_CODE == nAnctivity)
	{
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		return lRet;
	}

	if(strlen(pszGUID) > 0)
	{
		const CGUID& rID = GetScriptGuidValue(dynamic_cast<CMoveShape*>(p_SrcShape), pszGUID);
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		if(NULL_GUID == rID)
		{
			return lRet;
		}
		return GetInst(AntiCheat::CPlayerManager).PlayerChangeActivity(rID, nAnctivity);
	}
	/// 传入GUID为空的情况
	else if( p_SrcShape != NULL )
	{
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		return GetInst(AntiCheat::CPlayerManager).PlayerChangeActivity(dynamic_cast<CPlayer*>(p_SrcShape)->GetExID(), nAnctivity);
	}

	return lRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/// 函数功能:     踢玩家下线并限制其第二天才能登录的脚本函数
/// 函数脚本原型：ACKickPlayer( ["@playerGUID"] );
/// 函数参数:     @PlayerGUID : 玩家GUID, 可选
/// 函数返回值:   无
///////////////////////////////////////////////////////////////////////////////////////////////////////
double CScript::ACKickPlayer(const char *cmd, char *retStr /* = NULL */)
{
	CHECK_SHAPE( 0 );

	char *pszGUID = GetStringParam(cmd, 0);
	if(!pszGUID)
		return 0;
	if(strlen(pszGUID) > 0)
	{
		const CGUID& rID = GetScriptGuidValue(dynamic_cast<CMoveShape*>(p_SrcShape), pszGUID);
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		if(NULL_GUID == rID)
		{
			return 0;
		}
		GetInst(AntiCheat::CPlayerManager).KickOnlinePlayer(rID);
	}
	/// 传入GUID为空的情况
	else if( p_SrcShape != NULL )
	{
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		GetInst(AntiCheat::CPlayerManager).KickOnlinePlayer(dynamic_cast<CPlayer*>(p_SrcShape)->GetExID());
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/// 函数功能:     停止外挂检查的脚本函数
/// 函数脚本原型：StopCheckAC( ["@playerGUID"] );
/// 函数参数:     @PlayerGUID : 玩家GUID, 可选
/// 函数返回值:   无
///////////////////////////////////////////////////////////////////////////////////////////////////////
double CScript::StopCheckAC(const char *cmd, char *retStr /* = NULL */)
{
	CHECK_SHAPE( 0 );

	char *pszGUID = GetStringParam(cmd, 0);
	if(!pszGUID)
		return 0;
	if(strlen(pszGUID) > 0)
	{
		const CGUID& rID = GetScriptGuidValue(dynamic_cast<CMoveShape*>(p_SrcShape), pszGUID);
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		if(NULL_GUID == rID)
		{
			return 0;
		}
		GetInst(AntiCheat::CPlayerManager).StopCheck(rID);
	}
	/// 传入GUID为空的情况
	else if( p_SrcShape != NULL )
	{
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		GetInst(AntiCheat::CPlayerManager).StopCheck(dynamic_cast<CPlayer*>(p_SrcShape)->GetExID());
	}

	M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
	return 0;
}

double CScript::GetACAttr( const char *cmd, char *retStr )
{
	CHECK_SHAPE( 0 );

	char *strName = GetStringParam( cmd, 0 );
	char *strAttrName = GetStringParam( cmd, 1 );
	if( strName != NULL && strAttrName != NULL )
	{
		CPlayer *player = GetGame()->FindPlayer( strName );
		if( player == NULL )
		{
			player = static_cast<CPlayer*>( this->p_SrcShape );
		}

		if( player == NULL )
		{
			M_FREE(strName, sizeof(char)*MAX_VAR_LEN);
			M_FREE(strAttrName, sizeof(char)*MAX_VAR_LEN);
			return 0;
		}

		long ret = GetInst( AntiCheat::CPlayerManager ).ScriptQueryProperty( player->GetExID(), strAttrName);
		M_FREE(strName, sizeof(char)*MAX_VAR_LEN);
		M_FREE(strAttrName, sizeof(char)*MAX_VAR_LEN);

		return ret;
	}
	M_FREE(strName, sizeof(char)*MAX_VAR_LEN);
	M_FREE(strAttrName, sizeof(char)*MAX_VAR_LEN);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// 函数功能 :开启玩家身上定时器
/// 函数脚本原型: StarCheackAC( ["@playerGUID"] );
/// 函数参数:     @PlayerGUID : 玩家GUID, 可选
/// 函数返回值:   无
//////////////////////////////////////////////////////////////////////////
double CScript::StartCheackAC(const char *cmd , char *retStr /* = NULL */)
{
	CHECK_SHAPE( 0 );

	char *pszGUID = GetStringParam(cmd, 0);
	if(!pszGUID)
		return 0;
	if ( strlen(pszGUID) >0 )
	{
		const CGUID& rID = GetScriptGuidValue( dynamic_cast<CMoveShape*>(p_SrcShape), pszGUID );
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		if ( NULL_GUID == rID)
		{
			return 0;
		}
		GetInst( AntiCheat::CPlayerManager ).StartCheck(rID);
	}
	else if ( p_SrcShape != NULL)
	{
		M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
		GetInst( AntiCheat::CPlayerManager ).StartCheck( dynamic_cast<CPlayer*>(p_SrcShape)->GetExID());
	}

	M_FREE(pszGUID, sizeof(char)*MAX_VAR_LEN);
	return 0;
}