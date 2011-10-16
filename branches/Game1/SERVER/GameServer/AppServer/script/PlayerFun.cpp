//=============================================================================
/**
 *  file: PlayerFun.cpp
 *
 *  Brief:定义跟玩家相关的脚本函数
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
#include "../Goods/CGoodsFactory.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"
#include "../../../setup/LogSystem.h"
#include "../setup/GMList.h"

#include "../RgnManager.h"
#include "..\OrganizingSystem\GameOrganizingCtrl.h"
#include "..\OrganizingSystem\GSTeam.h"



double CScript::ChangeMe(const char* CmdStr, char* retStr)	        //改变属性
{
	char *strTemp=GetStringParam(CmdStr,0);
	if (strTemp)
	{
		double lChangeNum=GetIntParam(CmdStr,1);
		if(p_SrcShape && (OBJECT_TYPE)p_SrcShape->GetType() == TYPE_PLAYER)
		{
			if ( strcmp(strTemp, "wPkCount")==0 )	// PK数
			{
				double lNum = ((CPlayer*)p_SrcShape)->GetPkValue() + lChangeNum;
				if (lNum < 0)
					lNum = 0;
				if (lNum > 65535)
					lNum = 65535;

				((CPlayer*)p_SrcShape)->SetPkValue(lNum);
				((CPlayer*)p_SrcShape)->OnUpdateMurdererSign();
			}
			else
			{ 
				((CPlayer*)p_SrcShape)->ChangeAttribute((string)strTemp, lChangeNum);	//改变服务器数据				
			}
			((CPlayer*)p_SrcShape)->UpdateAttribute();
		}

		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}
	return 0;
}
double CScript::GetMe(const char* CmdStr, char* retStr)			    //获取主角属性
{
	char* strTemp = GetStringParam(CmdStr, 0);
	if (strTemp)
	{
		double ret = 0;
		if(p_SrcShape && (OBJECT_TYPE)p_SrcShape->GetType() == TYPE_PLAYER)
		{
			ret = ((CPlayer*)p_SrcShape)->GetAttribute(string(strTemp));
		}
		// ahc 修改内存泄漏
		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	return 0;
}
double CScript::SetMe(const char* CmdStr, char* retStr)
{
	char *strTemp=GetStringParam(CmdStr,0);
	if (strTemp)
	{
		if(p_SrcShape && (OBJECT_TYPE)p_SrcShape->GetType() == TYPE_PLAYER)
		{
			double lNum=GetIntParam(CmdStr,1);

			if ( strcmp(strTemp, "E_PkCount")==0 )
			{
				if (lNum < 0)
					lNum = 0;
				if (lNum > 65535)
					lNum = 65535;

				((CPlayer*)p_SrcShape)->SetPkValue((WORD)lNum);
				((CPlayer*)p_SrcShape)->OnUpdateMurdererSign();
			}
			else
			{
				if ( strcmp(strTemp, "Exp")==0 )	// 经验值
				{
					LONG64 llNum = lNum;
					long lExp = llNum%dwExpMutipleValue;
					long lExpMul = llNum/dwExpMutipleValue;

					CMessage tmsg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					tmsg.Add( ((CPlayer*)p_SrcShape)->GetType() );
					tmsg.Add( ((CPlayer*)p_SrcShape)->GetExID() );
					tmsg.Add( "ExpMultiple" );
					tmsg.Add( (long)lExpMul );
					tmsg.SendToPlayer(((CPlayer*)p_SrcShape)->GetExID());	// 发送到客户端

					CMessage msg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					msg.Add( ((CPlayer*)p_SrcShape)->GetType() );
					msg.Add( ((CPlayer*)p_SrcShape)->GetExID() );
					msg.Add( strTemp );
					msg.Add( (long)lExp );
					msg.SendToPlayer(((CPlayer*)p_SrcShape)->GetExID());	// 发送到客户端

					((CPlayer*)p_SrcShape)->SetAttribute(string("ExpMultiple"), lExpMul);	//改变服务器数据
					((CPlayer*)p_SrcShape)->SetAttribute(strTemp, lExp);	//改变服务器数据
					((CPlayer*)p_SrcShape)->CheckLevel();
				}
				else
				{
					CMessage msg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					msg.Add( ((CPlayer*)p_SrcShape)->GetType() );
					msg.Add( ((CPlayer*)p_SrcShape)->GetExID() );
					msg.Add( strTemp );
					msg.Add( (long)lNum );
					msg.SendToPlayer(((CPlayer*)p_SrcShape)->GetExID());	// 发送到客户端

					((CPlayer*)p_SrcShape)->SetAttribute(strTemp, lNum);	//改变服务器数据
				}
			}
			((CPlayer*)p_SrcShape)->UpdateAttribute();
		}
		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}
	return 0;
}

double CScript::ChangeDest(const char* CmdStr, char* retStr)	        //改变属性
{
	char *strTemp=GetStringParam(CmdStr,0);
	if (strTemp)
	{
		double lChangeNum=GetIntParam(CmdStr,1);
		if(p_DesShape && (OBJECT_TYPE)p_DesShape->GetType() == TYPE_PLAYER)
		{
			if ( strcmp(strTemp, "wPkCount")==0 )	// PK数
			{
				double lNum = ((CPlayer*)p_DesShape)->GetPkValue() + lChangeNum;
				if (lNum < 0)
					lNum = 0;
				if (lNum > 65535)
					lNum = 65535;

				((CPlayer*)p_DesShape)->SetPkValue(lNum);
				((CPlayer*)p_DesShape)->OnUpdateMurdererSign();
			}
			else
			{ 
				((CPlayer*)p_DesShape)->ChangeAttribute((string)strTemp, lChangeNum);	//改变服务器数据				
			}
			((CPlayer*)p_DesShape)->UpdateAttribute();
		}

		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}
	return 0;
}
double CScript::GetDest(const char* CmdStr, char* retStr)			    //获取主角属性
{
	char* strTemp = GetStringParam(CmdStr, 0);
	if (strTemp)
	{
		double ret = 0;
		if(p_DesShape && (OBJECT_TYPE)p_DesShape->GetType() == TYPE_PLAYER)
		{
			ret = ((CPlayer*)p_DesShape)->GetAttribute(string(strTemp));
		}
		// ahc 修改内存泄漏
		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	return 0;
}
double CScript::SetDest(const char* CmdStr, char* retStr)
{
	char *strTemp=GetStringParam(CmdStr,0);
	if (strTemp)
	{
		if(p_DesShape && (OBJECT_TYPE)p_DesShape->GetType() == TYPE_PLAYER)
		{
			double lNum=GetIntParam(CmdStr,1);

			if ( strcmp(strTemp, "E_PkCount")==0 )
			{
				if (lNum < 0)
					lNum = 0;
				if (lNum > 65535)
					lNum = 65535;

				((CPlayer*)p_DesShape)->SetPkValue((WORD)lNum);
				((CPlayer*)p_DesShape)->OnUpdateMurdererSign();
			}
			else
			{
				CMessage msg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
				msg.Add( ((CPlayer*)p_DesShape)->GetType() );
				msg.Add( ((CPlayer*)p_DesShape)->GetExID() );
				msg.Add( strTemp );
				msg.Add( (long)lNum );
				msg.SendToPlayer(((CPlayer*)p_DesShape)->GetExID());	// 发送到客户端

				LONG lOldValue = ((CPlayer*)p_DesShape)->GetAttribute(string(strTemp));
				((CPlayer*)p_DesShape)->SetAttribute(strTemp, lNum);	//改变服务器数据
				if ( strcmp(strTemp, "E_Exp")==0 )	// 经验值
					((CPlayer*)p_DesShape)->CheckLevel();
			}
			((CPlayer*)p_DesShape)->UpdateAttribute();
		}
		M_FREE( strTemp, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}
	return 0;
}

double CScript::SetPlayerPos(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL)
		return 0;

	double x,y,w,h;
	x=GetIntParam(CmdStr, 0);
	y=GetIntParam(CmdStr, 1);
	w=((CRegion*)((CPlayer*)p_SrcShape)->GetFather())->GetWidth();
	h=((CRegion*)((CPlayer*)p_SrcShape)->GetFather())->GetHeight();

	if (x>=w)	x=w - 1;
	else if (x<0)	x=0;

	if (y>=h)	y=h - 1;
	else if (y<0)	y=0;

	if(p_SrcShape->GetType() == TYPE_PLAYER)
	{
		((CPlayer*)p_SrcShape)->ProduceEvent(ET_Move);
		((CPlayer*)p_SrcShape)->ProduceEvent(ET_EventBreak);
		((CPlayer*)p_SrcShape)->SetPosition(x, y );
	}

	return 1;
}
double CScript::SetPlayerDir(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	long lDir = GetIntParam(CmdStr, 0);
	if(lDir == ERROR_CODE)
		return 0;
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
	if(!pMoveShape)	return 0;

	pMoveShape->SetDir(lDir);

	CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR);	
	Msg.Add( static_cast<BYTE>(pMoveShape->GetDir()) );
	Msg.Add( pMoveShape->GetDirEx());
	Msg.Add( pMoveShape->GetType() );
	Msg.Add( pMoveShape->GetExID() );
	Msg.SendToAround(pMoveShape);

	return 1;
}

double CScript::AddAreaCreditByAreaId(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer) return 0;
	DWORD dwAreaId=GetIntParam(CmdStr,0);
	long lVal=GetIntParam(CmdStr,1);
	pPlayer->AddCredit(dwAreaId,lVal);
	CMessage msg(MSG_S2C_PLAYER_CREDIT_UPDATE);
	msg.Add(dwAreaId);
	msg.Add(pPlayer->GetCredit(dwAreaId));
	msg.SendToPlayer(pPlayer->GetExID());
	return 1;
}

double CScript::SetAreaCreditByAreaId(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer) return 0;
	DWORD dwAreaId=GetIntParam(CmdStr,0);
	long lVal=GetIntParam(CmdStr,1);
	pPlayer->ChangeCredit(dwAreaId,lVal);
	CMessage msg(MSG_S2C_PLAYER_CREDIT_UPDATE);
	msg.Add(dwAreaId);
	msg.Add(pPlayer->GetCredit(dwAreaId));
	msg.SendToPlayer(pPlayer->GetExID());
	return 1;
}

double CScript::AddAreaCredit(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer) return 0;
	DWORD dwAreaId=0;
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
	if(!pRegion)  return 0;
	dwAreaId=pRegion->GetAreaId();
	//改变值
	long lVal=GetIntParam(CmdStr,0);
	pPlayer->AddCredit(dwAreaId,lVal);
	CMessage msg(MSG_S2C_PLAYER_CREDIT_UPDATE);
	msg.Add(dwAreaId);
	msg.Add(pPlayer->GetCredit(dwAreaId));
	msg.SendToPlayer(pPlayer->GetExID());
	return 1;
}

double CScript::SetAreaCredit(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer) return 0;
	DWORD dwAreaId=0;
	CServerRegion* pRegion=dynamic_cast<CServerRegion*>(pPlayer->GetFather());
	if(!pRegion)  return 0;
	dwAreaId=pRegion->GetAreaId();
	//改变值
	long lVal=GetIntParam(CmdStr,0);
	pPlayer->ChangeCredit(dwAreaId,lVal);
	CMessage msg(MSG_S2C_PLAYER_CREDIT_UPDATE);
	msg.Add(dwAreaId);
	msg.Add(pPlayer->GetCredit(dwAreaId));
	msg.SendToPlayer(pPlayer->GetExID());
	return 1;
}

double CScript::GetOrganCreditLevel(const char* cmd,char* retstr/* =NULL */)
{
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer)
		return 0;
	char* strName=GetStringParam(cmd,0);
	if (strName ==NULL)
		return 0;	
	
	long lv= pPlayer->GetOrganLevel(strName);
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	return lv;

}

double CScript::AddOrganCredit(const char* cmd,char* retstr/* =NULL */)
{
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer)
		return 0;
	char* strName=GetStringParam(cmd,0);
	if (strName ==NULL)
		return 0;
	long value=GetIntParam(cmd,1);
	pPlayer->AddOrganCredit(strName,value);
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	((CPlayer*)p_SrcShape)->UpdateAttribute();
	return 0;
}

double CScript::SetOrganCredit(const char* cmd,char * retsre/* =NULL */)
{
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer)
		return 0;
	char* strName=GetStringParam(cmd,0);
	if (strName ==NULL)
		return 0;
	long value=GetIntParam(cmd,1);
	pPlayer->ChangeOrganCredit(strName,value);
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	((CPlayer*)p_SrcShape)->UpdateAttribute();
	return 0;
}

double CScript::GetAreaCredit(const char* CmdStr, char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	DWORD  dwAreaId  = (DWORD)GetIntParam( CmdStr, 1 );


	if(szPlayerName==NULL)
		return 0;	

	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);
	if(pPlayer)
	{		
		DWORD dwCredit=pPlayer->GetCredit(dwAreaId);
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return dwCredit;
	}

	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetAreaCreditLevel(const char* cmd,char* retStr/* =NULL */)
{
	DWORD  dwAreaId  = (DWORD)GetIntParam( cmd, 0 );
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer==NULL)
		return 0;
	return pPlayer->GetCreditLevel(dwAreaId);
}

double CScript::GetCountry(const char* CmdStr, char* retStr)
{
	CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
	if(pMoveShape == NULL) return 0;

	return pMoveShape->GetCountry();
}
double CScript::ChangeCountry(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double n=GetIntParam(CmdStr,0);

	if(n == ERROR_CODE)
	{
		return -1;
	}
	CMessage Msg(MSG_S2W_COUNTRY_CHANGE);
	Msg.Add( p_SrcShape->GetExID() );
	Msg.Add( (BYTE)n);
	Msg.Send();

	return 1;
}
double CScript::GetContribute(const char* CmdStr, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer == NULL) return 0;

	return pPlayer->GetCountryContribute();
}
double CScript::SetContribute(const char* CmdStr, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer == NULL) return 0;

	double n=GetIntParam(CmdStr,0);
	if(n == ERROR_CODE)
	{
		return -1;
	}
	pPlayer->SetCountryContribute(n);

	CMessage msg(MSG_S2C_PLAYER_CHANGE_CONTRIBUTE);
	msg.Add((long)n);
	msg.SendToPlayer(pPlayer->GetExID());

	return pPlayer->GetCountryContribute();
}


double CScript::IsCharged(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	return ((CPlayer*)p_SrcShape)->IsCharged();
}
double CScript::SetCharged(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	DWORD  lRet  = GetIntParam( CmdStr, 0 );

	if(lRet==ERROR_CODE)
	{
		lRet=0;
	}
	((CPlayer*)p_SrcShape)->SetCharged(lRet==0?false:true);
	return ((CPlayer*)p_SrcShape)->IsCharged();
}

double CScript::GetName(const char* CmdStr, char* retStr)
{
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
		int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, ((CPlayer*)GetSrcShape())->GetName() );
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	CPlayer* pPlayer = NULL;
	guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, pGuidName);
	if(p_SrcShape && p_SrcShape->GetExID() == guid)
	{
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	}
	else if(p_DesShape && p_DesShape->GetExID() == guid)
	{
		pPlayer = dynamic_cast<CPlayer*>(p_DesShape);
	}
	else
	{
		pPlayer = GetGame()->FindPlayer(guid);
	}
	if (pPlayer)
	{
		//return (long)pPlayer->GetName();
		int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szVarName, pPlayer->GetName() );
		M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	M_FREE( pGuidName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szVarName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::ChangePlayer(const char* CmdStr, char* retStr)
{
	double nRet = -1;
	char *strName=GetStringParam(CmdStr,0);
	char* strPropName=GetStringParam(CmdStr,1);

	if (strName && strPropName)
	{
		double lChangeNum = GetIntParam(CmdStr, 2);

		CPlayer* pPlayer= GetGame()->FindPlayer(strName);	//得到对象指针
		if( pPlayer )
		{
			nRet=pPlayer->ChangeAttribute(string(strName), lChangeNum);	//改变服务器数据
			pPlayer->UpdateAttribute();
		}
	}
	M_FREE( strPropName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	return nRet;
}
double CScript::SetPlayer(const char* CmdStr, char* retStr)
{
	double nRet = -1;
	char *strName=GetStringParam(CmdStr,0);
	char* strPropName=GetStringParam(CmdStr,1);

	if (strName && strPropName)
	{
		double lNum = GetIntParam(CmdStr, 2);

		CPlayer* pPlayer= GetGame()->FindPlayer(strName);	//得到对象指针
		if( pPlayer )
		{
			if ( strcmp(strPropName, "E_PkCount")==0 )
			{
				if (lNum < 0)
					lNum = 0;
				if (lNum > 65535)
					lNum = 65535;

				pPlayer->SetPkValue((WORD)lNum);
				pPlayer->OnUpdateMurdererSign();
			}
			else
			{
				if ( strcmp(strPropName, "Exp")==0 )	// 经验值
				{
					LONG64 llNum = lNum;
					long lExp = llNum%dwExpMutipleValue;
					long lExpMul = llNum/dwExpMutipleValue;

					CMessage tmsg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					tmsg.Add( pPlayer->GetType() );
					tmsg.Add( pPlayer->GetExID() );
					tmsg.Add( "ExpMultiple" );
					tmsg.Add( (long)lExpMul );
					tmsg.SendToPlayer(pPlayer->GetExID());	// 发送到客户端

					CMessage msg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					msg.Add( pPlayer->GetType() );
					msg.Add( pPlayer->GetExID() );
					msg.Add( strPropName );
					msg.Add( (long)lExp );
					msg.SendToPlayer(pPlayer->GetExID());	// 发送到客户端

					pPlayer->SetAttribute(string("ExpMultiple"), lExpMul);	//改变服务器数据
					pPlayer->SetAttribute(strPropName, lExp);	//改变服务器数据
					pPlayer->CheckLevel();
				}
				else
				{
					CMessage msg(MSG_S2C_OTHER_SETSHAPEPROPERTIY);
					msg.Add( pPlayer->GetType() );
					msg.Add( pPlayer->GetExID() );
					msg.Add( strPropName );
					msg.Add( (long)lNum );
					msg.SendToPlayer(pPlayer->GetExID());	// 发送到客户端

					pPlayer->SetAttribute(strPropName, lNum);	//改变服务器数据
				}
			}
			pPlayer->UpdateAttribute();
		}
	}
	M_FREE( strPropName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	return nRet;
}
double CScript::GetPlayer(const char* CmdStr, char* retStr)
{
	double lRetNum=0;
	char* strName=GetStringParam(CmdStr,0);
	char* strPropName=GetStringParam(CmdStr,1);

	if (strName && strPropName) 
	{
		CPlayer* pPlayer= GetGame()->FindPlayer(strName);	//得到对象指针
		if( pPlayer )	//在当前服务器上
		{
			lRetNum=pPlayer->GetValue(strPropName);	//获取服务器数据
		}
		else
		{
			//不在当前服务器上,向WorldServer提交查询
			CMessage msg(MSG_S2W_GM_QUERY_PLAYERDATA);
			msg.Add(p_SrcShape->GetExID());
			msg.Add(strName);
			msg.Add(strPropName);
			msg.Add(GetID());
			msg.Send();

			/*脚本暂停执行,等待返回结果*/
			SetIsHang(true);
			SetHangFunc(SR_FUNC_GETPLAYER);					
		}
	}
	M_FREE( strPropName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	return lRetNum;
}
double CScript::SetPlayerLevel(const char* CmdStr, char* retStr)
{
	char *strName=GetStringParam(CmdStr,0);
	if (strName)
	{
		double lLevel=GetIntParam(CmdStr,1);

		CPlayer* pPlayer = NULL;
		if (strlen(strName)==0)
			pPlayer = ((CPlayer*)this->GetSrcShape());
		else
			pPlayer=GetGame()->FindPlayer(strName);

		if(pPlayer)
		{
			pPlayer->SetAttribute("Level", lLevel);				
			DWORD dwExp=CPlayerList::GetPlayerExp(pPlayer->GetLevel()-2);
			DWORD dwExpMul=CPlayerList::GetPlayerExpMultiple(pPlayer->GetLevel()-2);
			DWORD dwOldExp = pPlayer->GetExp();
			DWORD dwOldExpMul = pPlayer->GetExpMultiple();
			LONG64 llTmp = (LONG64)dwExpMutipleValue*dwExpMul;
			LONG64 llRet = llTmp + dwExp;
			pPlayer->IncExp(llRet, eAET_NPC);
			pPlayer->CheckLevel();
			pPlayer -> AddIncExpLog(dwOldExp, dwOldExpMul, dwExp, dwExpMul, eAET_NPC, "CScript::SetPlayerLevel()");
			
			CMessage msg(MSG_S2C_PLAYER_SETLEVEL);
			msg.Add((BYTE)lLevel);
			msg.Add(dwExp);
			msg.Add(pPlayer->GetExpMultiple());
			msg.Add(pPlayer->GetNextExp());
			msg.Add(pPlayer->GetNextExpMultiple());
			msg.SendToAround(pPlayer);
		}
		else
		{
			CMessage msg(MSG_S2W_PLAYER_SETLEVEL);
			msg.Add(strName);
			msg.Add((BYTE)lLevel);
			msg.Send();
		}
		M_FREE( strName, sizeof(char)*MAX_VAR_LEN );
	}

	return 1;
}

double CScript::SetPlayerExp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	
	double dbExp = GetIntParam(cmd,1);
	if(dbExp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	long lOldExp = pPlayer->GetExp();
	long lOldExpMultiple = pPlayer->GetExpMultiple();
	LONG64 llExp = dbExp;
	pPlayer->SetExp(llExp);
	pPlayer->CheckLevel();
	pPlayer->AddIncExpLog(lOldExp, lOldExpMultiple, llExp%dwExpMutipleValue, llExp/dwExpMutipleValue, eAET_Script, "");
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangePlayerExp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	
	double dbChangeExp = GetIntParam(cmd,1);
	if(dbChangeExp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	long lOldExp = pPlayer->GetExp();
	long lOldExpMul = pPlayer->GetExpMultiple();
	pPlayer->IncExp(dbChangeExp,eAET_Script);
	pPlayer->CheckLevel();
	LONG64 llChangeExp = dbChangeExp;
	pPlayer->AddIncExpLog(lOldExp, lOldExpMul, llChangeExp%dwExpMutipleValue, llChangeExp/dwExpMutipleValue, eAET_Script, "");
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::SetPlayerOccuExp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbExp = GetIntParam(cmd,2);
	if(dbExp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	long lOldOccuExp = pPlayer->GetOccuExp((eOccupation)lOccu);
	pPlayer->IncOccuExp((eOccupation)lOccu,dbExp-lOldOccuExp,eAET_Script);
	pPlayer->AddIncOccuExpLog(lOccu,lOldOccuExp, dbExp-lOldOccuExp,eAET_Script,"");
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangePlayerOccuExp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbChangeExp = GetIntParam(cmd,2);
	if(dbChangeExp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	long lOldOccuExp = pPlayer->GetOccuExp((eOccupation)lOccu);
	pPlayer->IncOccuExp((eOccupation)lOccu,dbChangeExp,eAET_Script);
	pPlayer->AddIncOccuExpLog(lOccu,lOldOccuExp,dbChangeExp,eAET_Script,"");
	//检测是否职业等级升级
	pPlayer->CheckOccuLevel((eOccupation)lOccu);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::GetPlayerOccuExp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();

	lRet = pPlayer->GetOccuExp((eOccupation)lOccu);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}
double CScript::SetPlayerOccuLvl(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbLvl = GetIntParam(cmd,2);
	if(dbLvl==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	pPlayer->SetOccuLvl((eOccupation)lOccu,dbLvl,TRUE);
	pPlayer->OnOccuLevelChanged();
	char pszTempt[64]="";
	_snprintf(pszTempt, 64,"OccuLvl%d",lOccu);
	//更新属性
	pPlayer->AddAttributeToDirty(string(pszTempt));
	pPlayer->UpdateAttribute();

	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}
double CScript::GetPlayerOccuLvl(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	lRet = pPlayer->GetOccuLvl((eOccupation)lOccu);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}
double CScript::SetPlayerOccuSp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbSp = GetIntParam(cmd,2);
	if(dbSp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	long lOldOccuSp = pPlayer->GetOccuSp((eOccupation)lOccu);
	pPlayer->IncOccuSp((eOccupation)lOccu,dbSp-lOldOccuSp,1);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangePlayerOccuSp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbChangeSp = GetIntParam(cmd,2);
	if(dbChangeSp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	long lOldOccuSp = pPlayer->GetOccuSp((eOccupation)lOccu);
	pPlayer->IncOccuSp((eOccupation)lOccu,dbChangeSp,2);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::GetPlayerOccuSp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	lRet = pPlayer->GetOccuSp((eOccupation)lOccu);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangePlayerOccuExpSp(const char* cmd, char* retStr)
{
	long lRet = 0;
	//玩家名
	char *pszPlayerName=GetStringParam(cmd,0);
	if(!pszPlayerName)	return lRet;
	CPlayer* pPlayer = NULL;
	if(0 == strcmp(pszPlayerName,""))
		pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(pszPlayerName);
	if(!pPlayer)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//职业
	long lOccu = GetIntParam(cmd,1);
	if(lOccu==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	double dbChangeSp = GetIntParam(cmd,2);
	if(dbChangeSp==ERROR_CODE)
	{
		M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	//改变SP值
	long lOldOccuSp = pPlayer->GetOccuSp((eOccupation)lOccu);
	pPlayer->IncOccuSp((eOccupation)lOccu,dbChangeSp,3);

	//改变职业经验
	double dbChangeExp = dbChangeSp*CGlobeSetup::GetSetup()->vigour_fOccExpSpRatio;
	long lOldOccuExp = pPlayer->GetOccuExp((eOccupation)lOccu);
	pPlayer->IncOccuExp((eOccupation)lOccu,dbChangeExp,eAET_Script);
	pPlayer->AddIncOccuExpLog(lOccu,lOldOccuExp,dbChangeExp,eAET_Script,"");
	//检测是否职业等级升级
	pPlayer->CheckOccuLevel((eOccupation)lOccu);
	M_FREE( pszPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::CheckReliveByOrigin(const char* cmd, char* retStr)
{
	CPlayer *pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(pPlayer && pPlayer->CheckReliveByOrigin())
	{
		//可以原地复活
		return 1.0;
	}
	//不能原地复活
	return 0.0;
}

double CScript::CheckReliveGoods(const char* cmd, char* retStr)
{
	double dRet = 0.0;
	CPlayer *pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	char *strIndex = GetStringParam(cmd, 0);	
	char *strRatio = GetStringParam(cmd, 1);
	char *strCostNum = GetStringParam(cmd, 2);
	char *strOrginName = GetStringParam(cmd,3);
	long lIndex = 0;	
	long lRatio = 0;
	long lCostNum = 0;
	char strName[256];	
	if (pPlayer &&  strIndex!=NULL && strRatio!=NULL && strCostNum!=NULL)
	{	
		if(pPlayer->CheckReliveGoods(lIndex,lRatio,lCostNum,strName))
		{
			//复活物品满足条件
			dRet = 1.0;
		}
		GetVariableList()->SetVarValue(strIndex,0,lIndex);
		GetVariableList()->SetVarValue(strRatio,0,lRatio);
		GetVariableList()->SetVarValue(strCostNum,0,lCostNum);
		GetVariableList()->SetVarValue(strOrginName,strName);
	}
	M_FREE( strIndex, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strRatio, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strCostNum, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strOrginName, sizeof(char)*MAX_VAR_LEN );
	
	return dRet;
}

double CScript::ShowPlayerInfo(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;
	char* pAimName=GetStringParam(cmd, 0);
	if (NULL == pAimName)
	{
		return 0;
	}

	char szInfo[2048];
	CPlayer *pAim = GetGame()->FindPlayer(pAimName);
	M_FREE( pAimName, sizeof(char)*MAX_VAR_LEN );
	if(NULL == pAim)
	{
		pPlayer->SendNotifyMessage( "该玩家不在当前服务器!!!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		return 0;
	}
	else
	{
		//! 国家的字符串
		char szCounty[32];
		switch(pAim->GetCountry())
		{
		case 1:
			_snprintf(szCounty, 32, "星之国普瑞恩(%d)", pAim->GetCountry());
			break;
		case 2:
			_snprintf(szCounty, 32, "海之国波尔菲特(%d)", pAim->GetCountry());
			break;
		case 3:
			_snprintf(szCounty, 32, "云之国赛诺特(%d)", pAim->GetCountry());
		    break;
		case 4:
			_snprintf(szCounty, 32, "月之国罗德莱斯(%d)", pAim->GetCountry());
		    break;
		case 5:
			_snprintf(szCounty, 32, "天之国摩兰德尔(%d)", pAim->GetCountry());
			break;
		case 6:
			_snprintf(szCounty, 32, "风之国曼特菲(%d)", pAim->GetCountry());
			break;
		default:
			_snprintf(szCounty, 32, "错误的国家ID(%d)", pAim->GetCountry());
		    break;
		}

		//! 职业的字符串
		char szOccu[32];
		BYTE cOccu = pAim->GetOccupation();
		switch(cOccu)
		{
		case 0:
			_snprintf(szOccu, 32, "初心者(%d)", cOccu);
			break;
		case 1:
			_snprintf(szOccu, 32, "战士(%d)", cOccu);
			break;
		case 2:
			_snprintf(szOccu, 32, "法师(%d)", cOccu);
			break;
		case 3:
			_snprintf(szOccu, 32, "行者(%d)", cOccu);
			break;
		case 4:
			_snprintf(szOccu, 32, "骑士(%d)", cOccu);
			break;
		case 5:
			_snprintf(szOccu, 32, "斗士(%d)", cOccu);
			break;
		case 6:
			_snprintf(szOccu, 32, "斥候(%d)", cOccu);
			break;
		case 7:
			_snprintf(szOccu, 32, "艺人(%d)", cOccu);
			break;
		case 8:
			_snprintf(szOccu, 32, "术士(%d)", cOccu);
			break;
		case 9:
			_snprintf(szOccu, 32, "牧师(%d)", cOccu);
			break;
		case 10:
			_snprintf(szOccu, 32, "圣骑士(%d)", cOccu);
			break;
		case 11:
			_snprintf(szOccu, 32, "暗骑士(%d)", cOccu);
			break;
		case 12:
			_snprintf(szOccu, 32, "角斗士(%d)", cOccu);
			break;
		case 13:
			_snprintf(szOccu, 32, "狂战士(%d)", cOccu);
			break;
		case 14:
			_snprintf(szOccu, 32, "巡守(%d)", cOccu);
			break;
		case 15:
			_snprintf(szOccu, 32, "刺客(%d)", cOccu);
			break;
		case 16:
			_snprintf(szOccu, 32, "剑舞者(%d)", cOccu);
			break;
		case 17:
			_snprintf(szOccu, 32, "吟游诗人(%d)", cOccu);
			break;
		case 18:
			_snprintf(szOccu, 32, "巫师(%d)", cOccu);
			break;
		case 19:
			_snprintf(szOccu, 32, "亡灵巫师(%d)", cOccu);
			break;
		case 20:
			_snprintf(szOccu, 32, "主教(%d)", cOccu);
			break;
		case 21:
			_snprintf(szOccu, 32, "贤者(%d)", cOccu);
			break;
		default:
			_snprintf(szOccu, 32, "错误的职业ID(%d)", cOccu);
			break;
		}

		CRegion *pRegion = (CRegion*)(pAim->GetFather());
		const char *pRegionName = "没找到所在地图";
		LONG lRegionID = 0;
		if(NULL != pRegion)
		{
			pRegionName = pRegion->GetName();
			lRegionID = pRegion->GetID();
		}

		//! 账号、国家、职业、职业等级、等级、经验、HP、MP、金币、银币、地图、点卷基本信息
		pPlayer->SendNotifyMessage( "----------------------------", 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "玩家:【%s】的信息：", pAim->GetName());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "账号【%s】", pAim->GetAccount());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "国家【%s】", szCounty);
		pPlayer->SendNotifyMessage(szInfo, 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "职业【%s】职业等级【%d】职业经验【%d \\ %d】", szOccu, pAim->GetOccuLvl((eOccupation)cOccu), pAim->GetOccuExp((eOccupation)cOccu), pAim->GetNextOccuExp());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "等级【%d】经验【%d \\ %d】", pAim->GetLevel(), pAim->GetExp(), pAim->GetNextExp());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "HP【%d \\ %d】MP【%d \\ %d】", (long)pAim->GetHP(), (long)pAim->GetMaxHP(), (long)pAim->GetMP(), (long)pAim->GetMaxMP());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "钱【G:%d \\ S:%d】", pAim->GetMoney(), pAim->GetSilver());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "地图【%s(%d)】", pRegionName, lRegionID);
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		_snprintf(szInfo, 2048, "点卷【%d】", pAim->GetTicket());
		pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
		pPlayer->SendNotifyMessage( "----------------------------", 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
	}
	
	return 0;
}

double CScript::ShowPlayerGoods(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer || NULL == m_GeneralVariableList)
		return 0;

	char* pAimName=GetStringParam(cmd, 0);
	if (NULL == pAimName)
	{
		return 0;
	}
	
	CPlayer *pAim = GetGame()->FindPlayer(pAimName);
	M_FREE( pAimName, sizeof(char)*MAX_VAR_LEN );
	if(NULL == pAim)
	{
		pPlayer->SendNotifyMessage( "该玩家不在当前服务器!!!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		return 0 ;
	}

	//! 遍历背包
	static DWORD arrContainerType[] = 
	{
		CS2CContainerObjectMove::PEI_PACKET,
		CS2CContainerObjectMove::PEI_EQUIPMENT,
		CS2CContainerObjectMove::PEI_PACK,
		CS2CContainerObjectMove::PEI_PACK1,
		CS2CContainerObjectMove::PEI_PACK2,
		CS2CContainerObjectMove::PEI_PACK3,
		CS2CContainerObjectMove::PEI_PACK4,
		CS2CContainerObjectMove::PEI_PACK5,
	};
	
	pPlayer->SendNotifyMessage( "----------------------------", 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
	char szInfo[256];
	for (LONG i = 0; i < sizeof(arrContainerType) / sizeof(DWORD); ++i)
	{
		CGoodsContainer *pContainer = pAim->FindContainer(arrContainerType[i]);
		if(NULL != pContainer)
		{
			if(CS2CContainerObjectMove::PEI_PACK == arrContainerType[i])
			{
				CSubpackContainer *pCSubpackContainer = (CSubpackContainer*)pContainer;
				for (LONG i = 0; i < 5; ++i)
				{
					CGoods *pGoods = pCSubpackContainer->GetSubpack(i)->pGoods;
					if(NULL != pGoods)
					{
						_snprintf(szInfo, 256, "背包：【%s】，数量【%d】，等级【%d】", pGoods->GetName(), pGoods->GetAmount(), pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1));
						pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
					}
				}	
			}
			else
			{
				
				if(CS2CContainerObjectMove::PEI_EQUIPMENT == arrContainerType[i])
				{
					CEquipmentContainer *pCEquipmentContainer = (CEquipmentContainer*)pContainer;
					map<CEquipmentContainer::EQUIPMENT_COLUMN,CGoods*> *pMapGoods = pCEquipmentContainer->GetGoodsMap();
					map<CEquipmentContainer::EQUIPMENT_COLUMN,CGoods*>::iterator ite = pMapGoods->begin();
					for (; ite != pMapGoods->end(); ++ite)
					{
						CGoods *pGoods = ite->second;
						if(NULL != pGoods)
						{
							_snprintf(szInfo, 256, "背包：【%s】，数量【%d】，等级【%d】", pGoods->GetName(), pGoods->GetAmount(), pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1));
							pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
						}
					}
				}
				else
				{
					CAmountLimitGoodsContainer *pCVolumeLimitGoodsContainer = (CAmountLimitGoodsContainer*)pContainer;
					hash_map<CGUID,CGoods*,hash_guid_compare> *pMapGoods = pCVolumeLimitGoodsContainer->GetGoodsMap();
					hash_map<CGUID,CGoods*,hash_guid_compare>::iterator ite = pMapGoods->begin();
					for (; ite != pMapGoods->end(); ++ite)
					{
						CGoods *pGoods = ite->second;
						if(NULL != pGoods)
						{
							_snprintf(szInfo, 256, "背包：【%s】，数量【%d】，等级【%d】", pGoods->GetName(), pGoods->GetAmount(), pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1));
							pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
						}
					}
				}		
			}
		}
	}

	//! 遍历仓库
	static DWORD arrDepotContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};

	for (LONG i = 0; i < sizeof(arrDepotContainerType) / sizeof(DWORD); ++i)
	{
		CGoodsContainer *pContainer = pAim->GetPlayerDepot()->FindContainer(arrDepotContainerType[i], true);
		if(NULL != pContainer)
		{
			if(eDCT_Secondary == arrDepotContainerType[i])
			{
				CSubpackContainer *pCSubpackContainer = (CSubpackContainer*)pContainer;
				for (LONG i = 0; i < 5; ++i)
				{
					CGoods *pGoods = pCSubpackContainer->GetSubpack(i)->pGoods;
					if(NULL != pGoods)
					{
						_snprintf(szInfo, 256, "仓库：【%s】，数量【%d】，等级【%d】", pGoods->GetName(), pGoods->GetAmount(), pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1));
						pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
					}
				}	
			}
			else
			{
				CAmountLimitGoodsContainer *pCVolumeLimitGoodsContainer = (CAmountLimitGoodsContainer*)pContainer;
				hash_map<CGUID,CGoods*,hash_guid_compare> *pMapGoods = pCVolumeLimitGoodsContainer->GetGoodsMap();
				hash_map<CGUID,CGoods*,hash_guid_compare>::iterator ite = pMapGoods->begin();
				for (; ite != pMapGoods->end(); ++ite)
				{
					CGoods *pGoods = ite->second;
					if(NULL != pGoods)
					{
						_snprintf(szInfo, 256, "仓库：【%s】，数量【%d】，等级【%d】", pGoods->GetName(), pGoods->GetAmount(), pGoods->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1));
						pPlayer->SendNotifyMessage(szInfo, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
					}
				}		
			}
		}
	}
	pPlayer->SendNotifyMessage( "----------------------------", 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM);
	return 0;
}

double CScript::ShowGeneralVariable(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer || NULL == m_GeneralVariableList)
		return 0;

	char szText[256];
	CVariableList::varlistitr itr = m_GeneralVariableList->VarListBegin();
	for (; m_GeneralVariableList->VarListEnd() != itr; ++itr)
	{
		// 简单变量
		if( itr->second->Array == 0 )
		{
			_snprintf(szText, 256, "%s = %d", itr->first.c_str(), (int)(itr->second->Value));
			pPlayer-> SendNotifyMessage( szText, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM );
		}
		else if( itr->second->Array > 0)	// 数组变量
		{
			for(int j = 0; j < itr->second->Array; j++)
			{
				_snprintf(szText, 256, "%s[%d] = %d", itr->first.c_str(), j, itr->second->Value[j]);
				pPlayer-> SendNotifyMessage( szText, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM );
			}
		}
		else
		{	//字符串变量
			_snprintf(szText, 256, "%s = \"%s\"", itr->first.c_str(), itr->second->strValue);
			pPlayer-> SendNotifyMessage( szText, 0xff00ff00, 0, eNOTIFYPOS_LEFT_BOTTOM );
		}
	}
	return 1.0f;
}


double CScript::DurancePlayer(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer || NULL == m_GeneralVariableList)
		return 0;

	char* pAimName=GetStringParam(cmd, 0);
	if (NULL == pAimName)
	{
		return 0;
	}

	BOOL bDurance = (0 == GetIntParam(cmd, 1));

	CPlayer *pAim = GetGame()->FindPlayer(pAimName);
	M_FREE( pAimName, sizeof(char)*MAX_VAR_LEN );
	if(NULL == pAim)
	{
		pPlayer->SendNotifyMessage( "该玩家不在当前服务器!!!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		return 0;
	}

	pAim->SetBoolAttribute("bUseItem", bDurance);
	pAim->SetBoolAttribute("bMoveable", bDurance);

	if(bDurance)
	{
		pPlayer->SendNotifyMessage( "玩家被解禁!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		pAim->SendNotifyMessage( "你可以活动了!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
	}
	else
	{
		pPlayer->SendNotifyMessage( "玩家被禁锢!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
		pAim->SendNotifyMessage( "你已经被禁锢!", 0xfff0000, 0, eNOTIFYPOS_LEFT_BOTTOM);
	}
	
	return 1;
}

double CScript::Onstate(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer)
		return 0;
	if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
		return 1;
	if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL)
		return 2;
	if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_INCREMENT)
		return 3;
	if(pPlayer->GetCurrentProgress()==CPlayer::PROGRESS_FBUSINESS)
		return 4;

	return 0;
}

//! 得到玩家活力点
double CScript::GetActive(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;

	return pPlayer->GetActive();
}
double CScript::AddActive(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;

	long lAddNum = GetIntParam(cmd, 0);
	if(ERROR_CODE == lAddNum)
	{
		return 0;
	}
	return ((pPlayer->AddExActive(lAddNum)) ? 1 : 0);
}

double CScript::TakeActive(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;

	long lTakeNum = GetIntParam(cmd, 0);
	if(ERROR_CODE == lTakeNum)
	{
		return 0;
	}
	return ((pPlayer->TakeActive(lTakeNum)) ? 1 : 0);
}
double CScript::ResetActive(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pPlayer)
		return 0;

	pPlayer->ResetActive(time(NULL));
	return 0;
}

double CScript::GetServer(const char* cmd, char* retStr)
{
	return GetGame()->GetIDIndex();
}

//! 得到结婚二人队伍中对方的GUID，函数会检测当前队伍是否符合一个结婚的队伍
//! 参数1：#GUID
//! 参数2：#OtherSizeGUID
//! 返回值：0=得不到 1=得到了
double CScript::GetOtherOneInMarriageTeam(const char* cmd, char* retStr)
{
	CPlayer *pSrcPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pSrcPlayer)
		return 0;
	//! 被设置者
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = pSrcPlayer;
		else
		{
			CGUID guid = GetScriptGuidValue(pSrcPlayer, szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	char* szRetName = GetStringParam(cmd, 1);
	if(szRetName)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(pAimPlayer->GetTeamID());
		if(NULL != pTeam)
		{
			const CGUID &guid = pTeam->GetMarriageOtherOne(pAimPlayer);
			if(NULL_GUID != guid)
			{
				SetScriptGuidValue(pSrcPlayer, szRetName, guid);
				M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
				return 1;
			}
		}
		
		M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
	}

	return 0;
}

//! 得到玩家结婚步骤
//! 参数1：#GUID
//! 返回值：步骤值
double CScript::GetPlayerMarriageStep(const char* cmd, char* retStr)
{
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	return pAimPlayer->GetMarriageStep();
}
//! 设置玩家结婚步骤
//! 参数1：#GUID
//! 参数2：$Step
//! 返回值：成功则返回设置的值，失败则返回-1
double CScript::SetPlayerMarriageStep(const char* cmd, char* retStr)
{
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return -1;

	long lStep = GetIntParam(cmd, 1);
	if(ERROR_CODE == lStep)
		return -1;

	return pAimPlayer->SetMarriageStep(lStep);
}

//! 设置结婚对象
//! 参数1：#GUID
//! 参数2：#OtherSizeGUID
//! 返回值：无意义
//! 这个脚本函数的效果不能在当前脚本及子脚本中体现，发布时需要注意
double CScript::SetPlayerSpouse(const char* cmd, char* retStr)
{	
	//! 被设置者
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	//! 设置目标
	CPlayer* pOtherSide = NULL;
	szGuid = GetStringParam(cmd, 1);
	if (NULL != szGuid)
	{
		if(0 != strlen(szGuid))
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pOtherSide = GetGame()->FindPlayer(guid);
		}
		
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pOtherSide)
		return 0;

	pAimPlayer->SetSpouse(pOtherSide);
	return 0;
}

//! 删除结婚对象
//! 参数1：#InGUID
//! 返回值：无意义
double CScript::DelPlayerSpouse(const char* cmd, char* retStr)
{
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
			if(NULL == pAimPlayer && NULL_GUID != guid)
			{
				CMessage msg(MSG_S2W_OTHER_DEL_SPOUSE);
				msg.Add(guid);
				msg.Send(false);
			}
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	//! 传入空值表示删除
	pAimPlayer->SetSpouse(NULL);
	return 0;
}

//! 得到对象ID
//! 参数1：#InGUID
//! 参数2：#ReGUID
//! 返回值：0=失败 1=成功
double CScript::GetPlayerSpouse(const char* cmd, char* retStr)
{
	CPlayer *pSrcPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pSrcPlayer)
		return 0;
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = pSrcPlayer;
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	char* szRetName = GetStringParam(cmd, 1);
	if(szRetName)
	{
		const CGUID &guid = pAimPlayer->GetSpouseGuid();
		if(NULL_GUID != guid)
		{
			SetScriptGuidValue(pSrcPlayer, szRetName, guid);
			M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
			return 1;
		}
		M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
	}

	return 0;
}

//! 得到对象名字
//! 参数1：@InGUID
//! 参数2：#ReName
//! 返回值：0=失败 1=成功
double CScript::GetPlayerSpouseName(const char* cmd, char* retStr)
{
	CPlayer *pSrcPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(NULL == pSrcPlayer)
		return 0;
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = pSrcPlayer;
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	char* szRetName = GetStringParam(cmd, 1);
	if(szRetName)
	{
		const char *pName = pAimPlayer->GetSpouseName();
		if(NULL != pName)
		{
			SetScriptVarValue(pSrcPlayer, szRetName, pName);
			M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
			return 1;
		}

		M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
	}

	return 0;
}

//! 获取元素属性
//! 参数1：玩家GUID，可以为""
//! 参数2：元素类型——0=地系、1=水系、2=火系、3=风系、4=光系、5=暗系
//! 参数3：属性类型——0=属性攻击、1=属性防御、2=相性攻击、3=相性防御
//! 返回值：元素值
double CScript::GetElementProperty(const char* cmd, char* retStr)
{
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	DWORD dwElementType		= GetIntParam(cmd,1);
	DWORD dwPropertyType	= GetIntParam(cmd,2);

	return pAimPlayer->GetElementProperty(dwElementType, dwPropertyType);
}

//! 设置元素属性
//! 参数1：玩家GUID，可以为""
//! 参数2：元素类型——0=地系、1=水系、2=火系、3=风系、4=光系、5=暗系
//! 参数3：属性类型——0=属性攻击、1=属性防御、2=相性攻击、3=相性防御
//! 参数4：元素值
//! 返回值：元素值
double CScript::SetElementProperty(const char* cmd, char* retStr)
{
	CPlayer* pAimPlayer = NULL;
	const char* szGuid = GetStringParam(cmd, 0);
	if (NULL != szGuid)
	{
		if (0 == strlen(szGuid))
			pAimPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		else
		{
			CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
			pAimPlayer = GetGame()->FindPlayer(guid);
		}
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	}

	if(NULL == pAimPlayer)
		return 0;

	DWORD dwElementType		= GetIntParam(cmd,1);
	DWORD dwPropertyType	= GetIntParam(cmd,2);
	DWORD dwNewValue		= GetIntParam(cmd,3);

	return pAimPlayer->SetElementProperty(dwElementType, dwPropertyType, dwNewValue);
}

double CScript::IsPlayerOnline(const char* CmdStr, char* retStr)
{
	char* pPlayerName=GetStringParam(CmdStr, 0);
	double returnVal = -1;

	if (pPlayerName)
	{
		CPlayer* p = GetGame()->FindPlayer(pPlayerName);

		if (p)
		{
			//在当前服务器
			returnVal = 1;
		}
		else
		{
			CMessage msg(MSG_S2W_GM_QUERY_PLAYERONLINE);
			if(p_SrcShape)
				msg.Add(p_SrcShape->GetExID());
			else
				msg.Add(NULL_GUID);
			msg.Add(pPlayerName);
			msg.Add(GetID());
			msg.Send();

			//脚本暂停执行,等待返回结果
			SetIsHang(true);
			SetHangFunc(SR_FUNC_ISPLAYERONLINE);
		}

		M_FREE( pPlayerName, sizeof(char)*MAX_VAR_LEN );
	}
	return returnVal;
}

double CScript::ChangeMoneyByName(const char* CmdStr, char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	double  lChangeMoney  = GetIntParam( CmdStr, 1 );

	double lRet = 0;
	if(szPlayerName==NULL)
		return lRet;	

	if(lChangeMoney==ERROR_CODE)
	{
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);

	if(pPlayer)
	{
		//输入脚本名字
		if(lChangeMoney > 1000000)
		{
			char pszInfo[1024]="";
			_snprintf(pszInfo,1024,"在脚本(%s)命令行(%s)，给玩家(%s)设置金钱(%d)错误.",m_FileName,CmdStr,pPlayer->GetName(),(long)lChangeMoney);
			PutStringToFile("ScriptError",pszInfo);
		}
		double lMoney = pPlayer->GetMoney()+lChangeMoney;
		if(lMoney < 0)
			lMoney = 0;
		pPlayer->SetMoney(lMoney,33);
		lRet=1;
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::ChangeSilverByName(const char* CmdStr,char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	double  lChangeMoney  = GetIntParam( CmdStr, 1 );

	double lRet = 0;
	if(szPlayerName==NULL)
		return lRet;	

	if(lChangeMoney==ERROR_CODE)
	{
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);

	if(pPlayer)
	{
		double lMoney = pPlayer->GetSilver()+lChangeMoney;
		if(lMoney < 0)
			lMoney = 0;
		pPlayer->SetSilver(lMoney);
		lRet=1;
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::GetSilverByName(const char* CmdStr,char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	DWORD dwRet = 0;
	if(szPlayerName==NULL)
		return dwRet;
	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);

	if(pPlayer)
	{
		dwRet = pPlayer->GetSilver();				
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return dwRet;
}

double CScript::SetSilverByName(const char* CmdStr,char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	double  lMoney  = GetIntParam( CmdStr, 1 );

	double lRet = 0;
	if(szPlayerName==NULL)
		return lRet;
	if(lMoney==ERROR_CODE)
	{
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);
	if(pPlayer)
	{
		if(lMoney < 0)
			lMoney = 0;
		pPlayer->SetSilver(lMoney);
		lRet=1;
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}


double CScript::GetMoneyByName(const char* CmdStr, char* retStr)
{
	//GetMoenyByName
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	DWORD dwRet = 0;
	if(szPlayerName==NULL)
		return dwRet;
	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);

	if(pPlayer)
	{
		dwRet = pPlayer->GetMoney();				
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return dwRet;
}
double CScript::SetMoneyByName(const char* CmdStr, char* retStr)
{
	CHAR* szPlayerName	= GetStringParam( CmdStr, 0 );
	double  lMoney  = GetIntParam( CmdStr, 1 );

	double lRet = 0;
	if(szPlayerName==NULL)
		return lRet;
	if(lMoney==ERROR_CODE)
	{
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}
	CPlayer* pPlayer = NULL;

	//找到Player
	if( strlen(szPlayerName)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(szPlayerName);
	if(pPlayer)
	{
		if(lMoney < 0)
			lMoney = 0;
		//输入脚本名字
		if(lMoney > 20000000)
		{
			char pszInfo[1024]="";
			_snprintf(pszInfo,1024,"在脚本(%s)命令行(%s)，给玩家(%s)设置金钱(%d)错误.",m_FileName,CmdStr,pPlayer->GetName(),(long)lMoney);
			PutStringToFile("ScriptError",pszInfo);
		}
		pPlayer->SetMoney(lMoney,34);
		lRet=1;
	}
	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::GetOccuValueRatio(const char* cmd, char* retStr)
{
	double dbRet = 0;
	char *pName = GetStringParam(cmd, 0);
	if(pName && *(long*)pName != ERROR_CODE && *(long*)pName != INVILID_VALUE)
	{
		int nOccu = GetIntParam(cmd, 1);
		if(nOccu != ERROR_CODE && nOccu != INVILID_VALUE)
		{
			dbRet = CPlayerList::GetOccuValueRatio(pName,nOccu);
		}		
	}
	M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
	return dbRet;
}

double CScript::GetOccuLvlCoe(const char* cmd, char* retStr)
{
	double dbRet = 1;
	int nOccu = GetIntParam(cmd, 0);
	if(nOccu != ERROR_CODE && nOccu != INVILID_VALUE)
	{
		double eOccuLvl = GetIntParam(cmd, 1);
		if(eOccuLvl != ERROR_CODE && eOccuLvl != INVILID_VALUE)
		{
			dbRet = CPlayerList::GetOccuLvlCoe((eOccupation)nOccu,(DWORD)eOccuLvl);
		}
		else // 参数是字符串
		{
			//AddErrorLogText("ERR: GetOccuLvlCoe(): 2st Arg is int!");
		}
	}
	else // 参数是字符串
	{
		AddErrorLogText("ERR: GetOccuLvlCoe(): 1st Arg is int!");
	}
	return dbRet;
}

//改变角色职业
double CScript::ChangeOccu(const char* cmd, char* retStr)
{
	double dbRet = 0;
	long lNewOccu = static_cast<long>(GetIntParam(cmd, 0));
	if(lNewOccu != ERROR_CODE && lNewOccu != INVILID_VALUE)
	{
		if(lNewOccu >= OCC_Novice && lNewOccu < OCC_Max)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
			if(pPlayer)
			{

				pPlayer->ChangeOccu(static_cast<eOccupation>(lNewOccu));
			}
		}
		else
		{
			AddErrorLogText("ERR: ChangeOccu(): 1st Arg  >= 0 && Arg <= 21");
		}
	}
	else
	{
		AddErrorLogText("ERR: ChangeOccu(): 1st Arg is int!");
	}
	return dbRet;
}

double CScript::CheckLevel(const char* CmdStr, char* retStr)
{
	if(p_SrcShape && (OBJECT_TYPE)p_SrcShape->GetType() == TYPE_PLAYER)
	{
		((CPlayer*)p_SrcShape)->CheckLevel();
	}
	return 1;
}

double CScript::CheckOccuLvl(const char* cmd, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer)	return 0;		
	//职业
	long lOccu = GetIntParam(cmd,0);
	if(lOccu==ERROR_CODE)
		return 0;
	if(lOccu == -1)	lOccu = pPlayer->GetOccupation();
	pPlayer->CheckOccuLevel((eOccupation)lOccu);
	return 1;
}

double CScript::ReLive(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;


	double type = GetIntParam(CmdStr,0);	
	DWORD dwRatio = GetIntParam(CmdStr,1);
	if (dwRatio== ERROR_CODE || dwRatio==INVILID_VALUE)
	{
		dwRatio = 50;
	}
	CMoveShape* pDest = (CMoveShape*)(p_SrcShape);		
	if( type == ERROR_CODE || type == INVILID_VALUE)
	{
		return 0;
	}

	pDest->OnRelive(type,dwRatio);
	return 1;
}

double CScript::ScriptIsRun(const char* CmdStr, char* retStr)
{
	double lRet = 0;
	char *pPlayerGuidName = GetStringParam(CmdStr, 0);
	if(!pPlayerGuidName) return 0;

	CGUID PlayerGuid;

	PlayerGuid = GetVariableList()->GetGuidValue(pPlayerGuidName);

	if(PlayerGuid == NULL_GUID)
		return lRet;
	char* strScriptName=GetStringParam(CmdStr, 1);
	if(strScriptName==NULL)
	{
		M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	CPlayer* pPlayer= NULL;
	if(PlayerGuid != NULL_GUID)
		pPlayer = GetGame()->FindPlayer(PlayerGuid);
	else
		pPlayer = ((CPlayer*)p_SrcShape);
	if(pPlayer)
	{
		if( ScriptIfExit(pPlayer,strScriptName) )
			lRet = 1;
	}

	M_FREE( strScriptName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}
double CScript::RemoveScript(const char* CmdStr, char* retStr)
{
	double lRet = 0;
	char *pPlayerGuidName = GetStringParam(CmdStr, 0);
	if(!pPlayerGuidName) return 0;

	CGUID PlayerGuid;

	PlayerGuid = GetVariableList()->GetGuidValue(pPlayerGuidName);
	if(PlayerGuid == NULL_GUID)
	{
		M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	char* strScriptName=GetStringParam(CmdStr, 1);
	if(strScriptName==NULL)
	{
		M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
		return lRet;
	}

	CPlayer* pPlayer= NULL;
	if(PlayerGuid != NULL_GUID)
		pPlayer = GetGame()->FindPlayer(PlayerGuid);
	else
		pPlayer = ((CPlayer*)p_SrcShape);
	if(pPlayer)
	{
		DelectPlayerScript(pPlayer,strScriptName);
		lRet = 1;
	}
	M_FREE( strScriptName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pPlayerGuidName, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::AutoMove(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	double lX = GetIntParam(CmdStr, 0);
	double lY = GetIntParam(CmdStr, 1);

	if (lX == ERROR_CODE || lY == ERROR_CODE)
		return 0;

	CMessage msg(MSG_S2C_PLAYER_AUTOMOVE);
	msg.Add((long)lX);
	msg.Add((long)lY);
	msg.SendToPlayer(p_SrcShape->GetExID());
	return 1;
}


double CScript::MovePlayer(const char* CmdStr, char* retStr)
{
	// Rgn Type
	eRgnType type = (eRgnType)(long)GetIntParam(CmdStr, 0);

	//##获取Region的ID
	char* szRID = GetStringParam( CmdStr, 1 );
	CGUID lRgnID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRID);
	//##获取四边形的顶点
	LONG lTopLeftX			= GetIntParam( CmdStr, 2 );
	LONG lTopLeftY			= GetIntParam( CmdStr, 3 );
	LONG lBottomRightX		= GetIntParam( CmdStr, 4 );
	LONG lBottomRightY		= GetIntParam( CmdStr, 5 );

	//##获取新的RegionID
	char* szID = GetStringParam( CmdStr, 6 );
	CGUID lNewRgnID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szID);
	//##获取新区域的四边形的顶点
	LONG lNewTopLeftX		= GetIntParam( CmdStr, 7 );
	LONG lNewTopLeftY		= GetIntParam( CmdStr, 8 );
	LONG lNewBottomRightX	= GetIntParam( CmdStr, 9 );
	LONG lNewBottomRightY	= GetIntParam( CmdStr, 10 );

	//##参数个数的检测
	if( lRgnID != NULL_GUID &&
		lTopLeftX != ERROR_CODE && lTopLeftY == ERROR_CODE &&
		lBottomRightX == ERROR_CODE && lBottomRightY == ERROR_CODE &&
		lNewRgnID == NULL_GUID &&
		lNewTopLeftX == ERROR_CODE && lNewTopLeftY == ERROR_CODE &&
		lNewBottomRightX == ERROR_CODE && lNewBottomRightY == ERROR_CODE )
	{
		//##如果只输入了2个参数。第二个参数应该是新的Rgn的ID
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		char* szID = GetStringParam( CmdStr, 1 );
		//lNewRgnID(szID);
		lTopLeftX			= ERROR_CODE;
	}


	//##验证要转移的新区域的有效性
	CServerRegion* pRegion = GetGame() -> FindRegion( lRgnID );
	if( pRegion )
	{
		if( lTopLeftX == ERROR_CODE &&
			lTopLeftY == ERROR_CODE &&
			lBottomRightX == ERROR_CODE &&
			lBottomRightY == ERROR_CODE )
		{
			lTopLeftX		= 0;
			lTopLeftY		= 0;
			lBottomRightX	= pRegion -> GetWidth() - 1;
			lBottomRightY	= pRegion -> GetHeight() - 1;
		}

		//##验证顶点的有效性
		if( lTopLeftX < 0 )
		{
			lTopLeftX = 0;
		}
		if( lTopLeftY < 0 )
		{
			lTopLeftY = 0;
		}
		if( lBottomRightX < 0 )
		{
			lBottomRightX = 0;
		}
		if( lBottomRightY < 0 )
		{
			lBottomRightY = 0;
		}

		if( lTopLeftX >= pRegion -> GetWidth() )
		{
			lTopLeftX = pRegion -> GetWidth() - 1;
		}
		if( lTopLeftY >= pRegion -> GetHeight() )
		{
			lTopLeftY = pRegion -> GetHeight() - 1;
		}
		if( lBottomRightX >= pRegion -> GetWidth() )
		{
			lBottomRightX = pRegion -> GetWidth() - 1;
		}
		if( lBottomRightY >= pRegion -> GetHeight() )
		{
			lBottomRightY = pRegion -> GetHeight() - 1;
		}

		//##验证区域的有效性
		if( lBottomRightY <= lTopLeftY ||
			lBottomRightX <= lTopLeftX )
		{
			M_FREE( szRID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
	}
	else
	{
		//##找不到Region
		M_FREE( szRID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CServerRegion* pNewRegion = GetGame() -> FindRegion( lNewRgnID );
	if( pNewRegion )
	{
		if( lNewTopLeftX == ERROR_CODE &&
			lNewTopLeftY == ERROR_CODE &&
			lNewBottomRightX == ERROR_CODE &&
			lNewBottomRightY == ERROR_CODE )
		{
			lNewTopLeftX		= 0;
			lNewTopLeftY		= 0;
			lNewBottomRightX	= pNewRegion -> GetWidth() - 1;
			lNewBottomRightY	= pNewRegion -> GetHeight() - 1;
		}

		//##验证顶点的有效性
		if( lNewTopLeftX < 0 )
		{
			lNewTopLeftX = 0;
		}
		if( lNewTopLeftY < 0 )
		{
			lNewTopLeftY = 0;
		}
		if( lNewBottomRightX < 0 )
		{
			lNewBottomRightX = 0;
		}
		if( lNewBottomRightY < 0 )
		{
			lNewBottomRightY = 0;
		}

		if( lNewTopLeftX >= pNewRegion -> GetWidth() )
		{
			lNewTopLeftX = pNewRegion -> GetWidth() - 1;
		}
		if( lNewTopLeftY >= pNewRegion -> GetHeight() )
		{
			lNewTopLeftY = pNewRegion -> GetHeight() - 1;
		}
		if( lNewBottomRightX >= pNewRegion -> GetWidth() )
		{
			lNewBottomRightX = pNewRegion -> GetWidth() - 1;
		}
		if( lNewBottomRightY >= pNewRegion -> GetHeight() )
		{
			lNewBottomRightY = pNewRegion -> GetHeight() - 1;
		}

		//##验证区域的有效性
		if( lNewBottomRightY <= lNewTopLeftY ||
			lNewBottomRightX <= lNewTopLeftX )
		{
			M_FREE( szRID, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
	}
	else
	{
		//##找不到新的Region
		M_FREE( szRID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	//##准备开始转移
	LONG lAreaWidth		= lNewBottomRightX - lNewTopLeftX;
	LONG lAreaHeight	= lNewBottomRightY - lNewTopLeftY;

	BEGIN_LOOP_REGION_SHAPE_BY_TYPE( pRegion, TYPE_PLAYER )
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( pShape );
	if( pPlayer )
	{
		if( pPlayer -> GetTileX() <= lBottomRightX &&
			pPlayer -> GetTileX() >= lTopLeftX &&
			pPlayer -> GetTileY() <= lBottomRightY &&
			pPlayer -> GetTileY() >= lTopLeftY )
		{
			LONG lNewX = 0;
			LONG lNewY = 0;
			pNewRegion -> GetRandomPosInRange( lNewX, lNewY, lNewTopLeftX, lNewTopLeftY, 
				lAreaWidth, lAreaHeight );
			pPlayer -> ChangeRegion(type, lNewRgnID, lNewX, lNewY, pPlayer -> GetDir() );
		}
	}
	END_LOOP_REGION_SHAPE_BY_TYPE

	M_FREE( szRID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
	return 1;
}




double CScript::GodMode(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;
	int flag = GetIntParam( CmdStr, 0 );
	if( flag == ERROR_CODE )
	{
		flag = 1;
	}

	((CPlayer*)p_SrcShape) -> God( flag != 0 );
	//((CPlayer*)p_SrcShape) ->AllEquipmentRefineUpdate(100000);
	return 1;
}

double CScript::ResidentMode(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	//##无敌模式切换成一般模式
	((CPlayer*)p_SrcShape) -> God( false );
	return 1;
}

double CScript::GetTicketByName(const char* CmdStr, char* retStr)
{
	double retVal=0;
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=NULL;

	char* name=GetStringParam(CmdStr, 0);	
	if( strlen(name)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(name);
	if(pPlayer)
	{		
		retVal=pPlayer->GetTicket();
	}
	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	return retVal;
}

double CScript::AddTicketByName(const char* CmdStr, char* retStr)
{
	double retVal=0;
	if(p_SrcShape == NULL) return 0;
	CPlayer* pPlayer=NULL;

	char* name=GetStringParam(CmdStr, 0);
	double n = GetIntParam(CmdStr, 1);
	if(!name)
		return retVal;
	if( strlen(name)==0 )
		pPlayer = ((CPlayer*)p_SrcShape);
	else
		pPlayer = GetGame()->FindPlayer(name);
	if(pPlayer)
	{
		DWORD dwVal=pPlayer->GetTicket()+n;
		if(dwVal>CGlobeSetup::GetSetup()->dwTicketLimit)
			dwVal=CGlobeSetup::GetSetup()->dwTicketLimit;
		pPlayer->SetTicket(dwVal);
		GetGameLogInterface()->logT502_Ticket_gain_log(pPlayer,n);
#ifdef _GOODSLOG1_
		char pszGoodsLog[1024]="";
		char playerGUID[50]="";
		pPlayer->GetExID().tostring(playerGUID);		
		_snprintf(pszGoodsLog,1024,"[操作:玩家获得点券][玩家名:%s][玩家GUID:%s][数量:%d]",
			pPlayer->GetName(),playerGUID,n);
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
		retVal=pPlayer->GetTicket();
	}
	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	return retVal;
}

double CScript::IsGM(const char* CmdStr, char* retStr)
{
	double retVal=0;
	if(p_SrcShape == NULL)
		return 0;	
	char* name=GetStringParam(CmdStr, 0);
	if(!name)	return 0;

	if( strlen(name)==0 )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
		if(pPlayer)
		{
			if(CGMList::GetInfoByName(pPlayer->GetName()) == NULL)
				retVal = 0;
			else 
				retVal = 1;
		}
	}
	else
	{
		if(CGMList::GetInfoByName(name) == NULL)
				retVal = 0;
			else 
				retVal = 1;
	}	
	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	return retVal;
}
// 隐匿功能相关接口
double CScript::GetRegionHideFlagById(const char *cmd, char *retStr) // 
{
	int lRgnId = GetIntParam(cmd, 0);
	if(lRgnId == INVILID_VALUE) return -1.0f;
	CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, lRgnId);
	if(pRgn)
	{
		return pRgn->GetRgnHideFlag();
	}
	return -1.0f;
}
double CScript::GetPlayerCanHideByGuid(const char *cmd, char *retStr)
{
	const char* szGuid = GetStringParam(cmd, 0);
	if(!szGuid) return -1.0f;
	CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
	if(guid != NULL_GUID)
	{
		CPlayer* player = GetGame()->FindPlayer(guid);
		if(player)
		{
			M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
			return player->GetCanHide();
		}
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return -1.0f;
}
double CScript::SetPlayerCanHideByGuid(const char *cmd, char *retStr)
{
	const char* szGuid = GetStringParam(cmd, 0);
	if(!szGuid) return 0;
	int flag = GetIntParam(cmd, 1);
	if(flag == INVILID_VALUE)
	{
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
	if(guid != NULL_GUID)
	{
		CPlayer* player = GetGame()->FindPlayer(guid);
		if(player)
		{
			player->SetCanHide((BYTE)flag);
		}
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
double CScript::GetPlayerHideFlagByGuid(const char *cmd, char *retStr)
{
	const char* szGuid = GetStringParam(cmd, 0);
	if(!szGuid) return 0;
	CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
	if(guid != NULL_GUID)
	{
		CPlayer* player = GetGame()->FindPlayer(guid);
		if(player)
		{
			M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
			return player->GetHideFlag();
		}
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return -1.0f;
}
double CScript::SetPlayerHideFlagByGuid(const char *cmd, char *retStr)
{
	const char* szGuid = GetStringParam(cmd, 0);
	if(!szGuid) return 0;
	int flag = GetIntParam(cmd, 1);
	if(flag == INVILID_VALUE)
	{
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	CGUID guid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid);
	if(guid != NULL_GUID)
	{
		CPlayer* player = GetGame()->FindPlayer(guid);
		if(player)
		{
			player->SetHideFlag((BYTE)flag);
		}
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
double CScript::GetHideName(const char *cmd, char *retStr)
{
	char *pName = GetStringParam(cmd, 0);
	if(!pName) return 0;
	SetScriptVarValue((CMoveShape*)GetSrcShape(), pName, CGlobeSetup::GetSetup()->szHideName);
	M_FREE( pName, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
double CScript::GMViewHideNameAround(const char *cmd, char *retStr)
{
	CMoveShape* pGM = static_cast<CMoveShape*>(GetSrcShape());
	CServerRegion* pRgn = static_cast<CServerRegion*>(GetRegion());
	if(pGM && pRgn)
	{
		vector<CGUID> vTemptPlayer;
		BEGIN_LOOP_AROUND_PLAYER( pRgn, pGM )
		{
			if( pPlayer != pGM && pPlayer->GetHideFlag() == 1) // 不是GM，并且是隐匿者
				vTemptPlayer.push_back( pPlayer ->GetExID());
		}
		END_LOOP_AROUND_PLAYER
			
		if(vTemptPlayer.size() == 0)
			return 0;

		CMessage msg(MSG_S2C_GM_VIEW_HIDENAME_AROUND);
		msg.Add((long)vTemptPlayer.size());
		vector<CGUID>::iterator itr = vTemptPlayer.begin();
		for(; itr != vTemptPlayer.end(); itr++)
		{
			CPlayer* p = GetGame()->FindPlayer(*itr);
			if(p)
			{
				msg.Add(*itr);
				msg.Add(p->GetName());
			}
			else
			{
				msg.Add(NULL_GUID);
				msg.Add("");
			}
		}
		msg.SendToPlayer(pGM->GetExID());
		return 1.0f;
	}
	return 0;
}

double CScript::SetActiveRatio(const char *cmd, char *retStr)
{
	float fRatio = GetIntParam(cmd,0);

	CPlayer *pPlayer = (CPlayer*)p_SrcShape;
	if (pPlayer &&fRatio!=INVILID_VALUE && fRatio!=ERROR_CODE)
	{
		pPlayer->SetActiveRatio(fRatio);
	}
	return 1.0;
}

double CScript::GetActiveRatio(const char *cmd, char *retStr)
{
	float fActiveRatio = 1.0;
	CPlayer *pPlayer = (CPlayer*)p_SrcShape;
	if (pPlayer)
	{
		fActiveRatio = pPlayer->GetActiveRatio();
	}
	return fActiveRatio;
}

double CScript::SetStableHookTime(const char* cmd, char* retStr)
{
    LONG64 lTime = GetIntParam(cmd,0);
    if (lTime <= 0)
    {
        return 0;
    }
    CPlayer *pPlayer = (CPlayer*)p_SrcShape;
    if (pPlayer)
    {
        CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeStableTime);
        msg.Add( pPlayer->GetType() );
        msg.Add( pPlayer->GetExID() );
        msg.Add( "StableHookTime" );
        msg.Add( (long)lTime );
        msg.SendToPlayer(pPlayer->GetExID());	// 发送到客户端*/

        //更新到WS
        CMessage msgToWSHookTime(MSG_S2W_OTHER_CHANGEHOOKTIME);
       // msgToWSHookTime.Add(pPlayer->GetType());
        msgToWSHookTime.Add(pPlayer->GetExID());
        msgToWSHookTime.Add((long)lTime);
        msgToWSHookTime.Add("StableHookTime");
        msgToWSHookTime.Send();

        pPlayer->m_SpriteSystem.SetStableTime(lTime);
        //设置保存标识
        pPlayer->SetSavePartFlag(pPlayer->GetSavePartFlag()|SAVE_DETAIL_INNERHOOK);
    }
    return lTime;
}

double CScript::AddStableHookTime(const char* cmd, char* retStr)
{
    double lTime = GetIntParam(cmd,0);
    if (lTime)
    {
        CPlayer *pPlayer = (CPlayer*)p_SrcShape;
        LONG64 lTotalTime = pPlayer->m_SpriteSystem.GetStableTime();
        lTotalTime += lTime;

        CMessage msg(MSG_S2C_SPRITE_AutoFight_ChangeStableTime);
        msg.Add( pPlayer->GetType() );
        msg.Add( pPlayer->GetExID() );
        msg.Add( "StableHookTime" );
        msg.Add( (long)lTotalTime );
        msg.SendToPlayer(pPlayer->GetExID());	// 发送到客户端*/

        //更新到WS
        CMessage msgToWSHookTime(MSG_S2W_OTHER_CHANGEHOOKTIME);
        // msgToWSHookTime.Add(pPlayer->GetType());
        msgToWSHookTime.Add(pPlayer->GetExID());
        msgToWSHookTime.Add((long)lTotalTime);
        msgToWSHookTime.Add("StableHookTime");
        msgToWSHookTime.Send();

        pPlayer->m_SpriteSystem.SetStableTime(lTotalTime);
        //设置保存标识
        pPlayer->SetSavePartFlag(pPlayer->GetSavePartFlag()|SAVE_DETAIL_INNERHOOK);
    }
    return lTime;
}
double CScript::GetStableHookTime(const char* cmd, char* retStr)
{
    long lTime = 0;
    CPlayer *pPlayer = (CPlayer*)p_SrcShape;
    if (pPlayer)
    {
        lTime = pPlayer->m_SpriteSystem.GetStableTime();
    }
    return lTime;
}
double CScript::GetCostYuanBao(const char *cmd, char *retStr)
{
	double dbCostNum = 0.0;
	CPlayer *pPlayer = (CPlayer*)p_SrcShape;
	if (pPlayer)
	{
		dbCostNum = pPlayer->GetCostYuanBao();
	}
	return dbCostNum;
}

double CScript::GetNextTotalExp(const char *cmd, char *retStr)
{
	CPlayer* pPlayer=dynamic_cast<CPlayer*>(p_SrcShape);
	if (pPlayer==NULL)
		return 0;
	LONG64 UpgradeExp = pPlayer->GetNextTotalExp();
	return UpgradeExp;
}


// 结婚日志
double CScript::MarriageLog(const char* cmd, char* retStr)
{
    CPlayer* pPlayer = (CPlayer*)p_SrcShape;
    if (pPlayer)
    {
        const char* szAimGuid = GetStringParam(cmd, 0);
        const CGUID& aimGuid = GetScriptGuidValue(pPlayer, szAimGuid);
        long nState = (long)GetIntParam(cmd, 1);
        GetGame()->GetLogicLogInterface()->logT910_player_marriage_log(pPlayer, &aimGuid, nState);
    }
    return 0;
}