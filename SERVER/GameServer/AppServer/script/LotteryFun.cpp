//=============================================================================
/**
 *  file: OtherFun.cpp
 *
 *  Brief:定义了国家系统、战斗系统、跑商、邮件等其他的脚本函数
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
#include "variablelist.h"
#include "../player.h"
#include "../serverregion.h"
#include "../PKSys.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "../../../setup/GlobeSetup.h"
#include "../../../setup/LogSystem.h"
#include "../../../setup/MailList.h"
#include "../Country/Country.h"
#include "../Country/CountryHandler.h"
#include "../business/CBusinessManager.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../public/kl_tp_utility.h"
#include "appserver/FBusiness/ShopManager.h"
#include "../../../setup/counttimerlist.h"
#include "..\goods\CGoodsFactory.h"


//彩票系统脚本相关函数

double CScript::OpenLotteryPage(const char* CmdStr, char* retStr)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->OpenLotteryPage();
		lRet = 1;
	}
	return lRet;
}
double CScript::DoLottery(const char* CmdStr, char* retStr)
{
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if (player)
	{
		player->DoLottery();
		lRet = 1;
	}
	return lRet;
}
double CScript::SetWorldCredit(const char* CmdStr, char* retStr)
{
	DWORD dwWorldCredit = GetIntParam(CmdStr,0);
	long lRet = 0;
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
		//player->SetWorldCredit((long)dwWorldCredit);
		CMessage msg(MSG_S2W_LOTTERY_CHANGEWORLDCREDIT);
		msg.Add((long)dwWorldCredit);
		msg.Send();

	}
	return lRet;
}

double CScript::GetPersonalCredit(const char* cmd, char* retStr)
{

	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
		return player->GetPersonalCredit();
	}
	return 0;
}

double CScript::SetCurHighestCredit(const char* CmdStr, char* retStr)
{
	char* strName = GetStringParam(CmdStr,0);
	DWORD lCredit = GetIntParam(CmdStr,1);
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
		//player->SetCreditHighest((long)lCredit);
		//player->SetHighestName(strName);
		CMessage msgToWS(MSG_S2W_LOTTERY_CHANGEPRECEDE);
		msgToWS.Add((long)lCredit);
		msgToWS.Add((char*)strName);
		msgToWS.Send();
	}
	return 0;
}

double CScript::GetPersonalCreditTotal(const char* CmdStr, char* retStr)
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
		return player->GetPersonalCreditTotal();
	}
	return 0;
}

double CScript::SetWorldCreditVelue(const char* CmdStr, char* retStr)
{
	DWORD PersonalCreditVelue = GetIntParam(CmdStr,0);
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
	    player->SetWorldCreditVelue((int)PersonalCreditVelue);
		player->SetLotteryLevel((int)PersonalCreditVelue);
		//if(PersonalCreditVelue==2)
		//{
		//	player->SetSendWorldInfo(true);
		//}
	}
	return 0;
}

double CScript::GetWorldCreditMax(const char* CmdStr, char* retStr)
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	if(player)
	{
		return GetInst(CLotteryXml).GetWorldCreditMax();
	}
	return 0;
}

double CScript::SendWorldCredit(const char* CmdStr, char* retStr)
{
	char* strName = GetStringParam(CmdStr,0);  
	DWORD lCredit = GetIntParam(CmdStr,1);
	DWORD lWorldCredit = GetIntParam(CmdStr,2);

	CMessage msg(MSG_S2C_LOTTERY_MaxTodayPoint);
	msg.Add( (long)lCredit );	
	msg.Add( (char*)strName );	
	msg.SendToPlayer(((CPlayer*)p_SrcShape)->GetExID());	// 发送到客户端

	CMessage msgEx(MSG_S2C_LOTTERY_WorldPoint);
	msgEx.Add( (long)lWorldCredit );	
	msgEx.SendToPlayer(((CPlayer*)p_SrcShape)->GetExID());

	CMessage msgToWSCredit(MSG_S2W_LOTTERY_CHANGEWORLDCREDIT);
	msgToWSCredit.Add((long)lWorldCredit);
	msgToWSCredit.Send();

	CMessage msgToWSName(MSG_S2W_LOTTERY_CHANGEPRECEDE);
	msgToWSName.Add((long)lCredit);
	msgToWSName.Add((char*)strName);
	msgToWSName.Send();
	return 0;
}

double CScript::SendLotteryChat(const char* CmdStr, char* retStr)
{
	char* strText = GetStringParam(CmdStr,0);  
	CMessage msgToWSName(MSG_S2W_LOTTERY_WORLDINFO);
	msgToWSName.Add((char*)strText);
	msgToWSName.Send();
	return 0;
}

double CScript::GetLotteryRe(const char* CmdStr, char* retStr)
{
	CPlayer* player= dynamic_cast<CPlayer*>(p_SrcShape);
	DWORD LotteryRe = 0;

	char *pGuidName = GetStringParam(CmdStr, 0);
	if(!pGuidName) return 0;

	CGUID guid;
	char* szVarName = GetStringParam(CmdStr, 1);
	if(!szVarName)
	{
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if (pGuidName[0] == '\0')
	{
		LotteryRe = player->GetLotteryRe();
		int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, CGoodsFactory::QueryGoodsName(LotteryRe));
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}
	return LotteryRe;
}