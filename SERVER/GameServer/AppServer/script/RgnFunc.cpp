#include "StdAfx.h"
#include "mmsystem.h"
#include "script.h"
#include "variablelist.h"

#include "../player.h"
#include "../goods/cgoods.h"
#include "../serverregion.h"
#include "../monster.h"
#include "../../../setup/GlobeSetup.h"
#include "../../../setup/LogSystem.h"
#include "../Country/Country.h"
#include "../Country/CountryHandler.h"
#include "..\OrganizingSystem\GameOrganizingCtrl.h"
#include "..\OrganizingSystem\GSTeam.h"
#include "../RgnManager.h"
#include "../public/StrType.h"
#include "../public/StrUnifyInput.h"


double CScript::GetRgnAttr(const char* cmd, char* retStr)
{
	long lRet = 0;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pRegion);
	if(!pRegion)
		return lRet;
	char *strAttrName=GetStringParam(cmd,0);
	if (strAttrName)
	{
		lRet = pRegion->GetAttribute(strAttrName);
		M_FREE( strAttrName, sizeof(char)*MAX_VAR_LEN );
	}
	return lRet;
}
double CScript::SetRgnAttr(const char* cmd, char* retStr)
{
	long lRet = 0;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pRegion);
	if(!pRegion)
		return lRet;
	char *strAttrName=GetStringParam(cmd,0);
	if (strAttrName)
	{
		double lNum=GetIntParam(cmd,1);
		if(lNum != ERROR_CODE)
		{
			 pRegion->SetAttribute(strAttrName,lNum);
		}
		M_FREE( strAttrName, sizeof(char)*MAX_VAR_LEN );
	}
	return lRet;
}
double CScript::GetCellType(const char* cmd, char* retStr)
{
	long lRet = 0;
	if(!m_pRegion)
		return lRet;
	long lTileX = GetIntParam(cmd,0);
	long lTileY = GetIntParam(cmd,1);
	if(lTileX == ERROR_CODE || lTileY== ERROR_CODE)
		return lRet;
	lRet = m_pRegion->GetSecurity(lTileX,lTileY);
	return lRet;
}

double CScript::RegionIsExit(const char* CmdStr, char* retStr)
{
	long lRet = 0;
	char* szGuid=GetStringParam(CmdStr,0);
	if (szGuid==NULL)
	{
		return 0;
	}

	const CGUID& RegionID = GetScriptGuidValue(dynamic_cast<CMoveShape*>(p_SrcShape),szGuid);
	if(RegionID == NULL_GUID)
	{
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CRegion* pRegion=NULL;
	pRegion = (CRegion*)GetGame()->FindRegion(RegionID);			
	if (pRegion)
		lRet = 1;
	else
	{
		CMessage msg(MSG_S2W_GM_QUERY_REGIONISEXIT);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(RegionID);
		msg.Add(GetID());
		msg.Send();
		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_REGIONISEXIT);
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return lRet;
}

double CScript::GetRegionID(const char* CmdStr, char* retStr)
{
	char* pRegionName=GetStringParam(CmdStr,0);
	if (pRegionName==NULL)
		return 0;

	char* szGuid=GetStringParam(CmdStr,1);
	if (szGuid==NULL)
	{
		M_FREE( pRegionName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CRegion* pRegion=NULL;
	pRegion = (CRegion*)GetGame()->FindRegion(pRegionName);			
	if (pRegion)
		SetScriptGuidValue((CMoveShape*)GetSrcShape(), szGuid, pRegion->GetExID());
	else
	{
		CMessage msg(MSG_S2W_GM_QUERY_FINDREGION);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(pRegionName);
		msg.Add(GetID());
		msg.Send();
		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETREGIONID);
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( pRegionName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetMapInfo(const char* CmdStr, char* retStr)
{
	CRegion* pRegion = GetRegion();
	if( pRegion )
	{
		double lX	= GetIntParam( CmdStr, 0 );
		double lY	= GetIntParam( CmdStr, 1 );
		if( lX == ERROR_CODE || lY == ERROR_CODE ) return -1;
		CRegion::tagCell* pCell = pRegion -> GetCell( lX, lY );
		if( pCell )
		{
			/**## 如果是国战区，就返回2，它的优先级最高 */
			if( pCell -> lWar == 1 ) return 2;
			/**## 如果是安全区，返回3 */
			else if( pRegion -> GetSecurity(lX,lY)== CRegion::SECURTIY_SAFE  ) return 3;
			/**## 如果是竞技区，返回1 */
			else if( pRegion -> GetSecurity(lX,lY) == CRegion::SECURTIY_FIGHT ) return 1;
			/**## 如果不是以上的，就默认为普通PK区，返回0 */
			else return 0;
		} 
	} 
	/**## 如果出错就返回-1 */
	return -1;
}

double CScript::GetCountryOwningRegion(const char* CmdStr, char* retStr)
{
	double lRet = 0;
	return lRet;
}


double CScript::SetBlock(const char* CmdStr, char* retStr)
{
	char* szID = GetStringParam(CmdStr,0);
	if(!szID) return 0;

	CGUID lRegionID(szID);

	double x = GetIntParam(CmdStr,1);
	double y = GetIntParam(CmdStr,2);
	double block = GetIntParam(CmdStr,3);

	if (lRegionID==NULL_GUID)
	{
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	if (block<CRegion::BLOCK_NO || block > CRegion::BLOCK_SHAPE)
	{
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return -1;
	}

	CServerRegion* pServerRegion = GetGame()->FindRegion(lRegionID);
	if (pServerRegion)
	{
		pServerRegion->SetBlock(x,y, block);
	}

	M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
	return 1;
}
double CScript::GetRegionRandomPos(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* szID = GetStringParam(CmdStr,0);

	CGUID lRegionID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szID);

	if (lRegionID == NULL_GUID)
	{
		lRegionID = p_SrcShape->GetRegionGUID();
	}

	CServerRegion* pRegion = GetGame()->FindRegion(lRegionID);
	if (pRegion)
	{
		long x = 0, y = 0;
		pRegion->GetRandomPos(x, y);

		((CPlayer*)p_SrcShape)->GetVariableList()->SetVarValue("$m_Temp", 0, x);
		((CPlayer*)p_SrcShape)->GetVariableList()->SetVarValue("$m_Temp", 1, y);
		M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}

	M_FREE( szID, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetRandomPosInRange(const char* CmdStr, char* retStr)
{
	double rgnID = GetIntParam( CmdStr, 0 );
	
	CServerRegion* pRegion = NULL;
	pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnID);
	if(pRegion == NULL)
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, rgnID);
	if(pRegion == NULL)
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, rgnID);
	if(pRegion == NULL)
		pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, rgnID);
	
	if(!pRegion)
	{
		pRegion = static_cast<CServerRegion*>( this->p_SrcShape->GetFather() );
	}

	if( pRegion == NULL )
	{
		return -1;
	}

	int x, y, w, h;
	long lPosX = 0, lPosY = 0;

	x = GetIntParam( CmdStr, 1 );
	y = GetIntParam( CmdStr, 2 );
	w = GetIntParam( CmdStr, 3 );
	h = GetIntParam( CmdStr, 4 );

	if( x == ERROR_CODE || y == ERROR_CODE || w == ERROR_CODE || h == ERROR_CODE )
	{
		return -1;
	}

	pRegion->GetRandomPosInRange( lPosX, lPosY, x, y, w, h );

	char *strPosX = GetStringParam( CmdStr, 5 );
	char *strPosY = GetStringParam( CmdStr, 6 );
	int ret = CVariableList::VAR_NOT_FOUND;
	ret = SetScriptVarValue( static_cast<CMoveShape*>( p_SrcShape ), strPosX, 0, lPosX );
	ret = SetScriptVarValue( static_cast<CMoveShape*>( p_SrcShape ), strPosY, 0, lPosY ) == CVariableList::VAR_NOT_FOUND ? 
		CVariableList::VAR_NOT_FOUND : ret;

	M_FREE( strPosX, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strPosY, sizeof(char)*MAX_VAR_LEN );

	return ret == CVariableList::VAR_NOT_FOUND;
}

double CScript::GetLineDir( const char *cmd, char *retStr )
{
	double sx = GetIntParam( cmd, 0 );
	double sy = GetIntParam( cmd, 1 );
	double dx = GetIntParam( cmd, 2 );
	double dy = GetIntParam( cmd, 3 );
	
	if( sx == ERROR_CODE || sy == ERROR_CODE || dx == ERROR_CODE || dy == ERROR_CODE )
	{
		return -1;
	}

	long dir = ::GetLineDir( sx, sy, dx, dy );

	return dir;
}

double CScript::GetNextPosByDir( const char *cmd, char *retStr )
{
	double dir = GetIntParam( cmd, 0 );
	double posX = GetIntParam( cmd, 1 );
	double posY = GetIntParam( cmd, 2 );
	double step = GetIntParam( cmd, 3 );

	// pos table
	int post[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	int destX = posX + post[(int)dir][0] * step;
	int destY = posY + post[(int)dir][1] * step;

	char *retPosX = GetStringParam( cmd, 4 );
	char *retPosY = GetStringParam( cmd, 5 );
	SetScriptVarValue( static_cast<CMoveShape*>( p_SrcShape ), retPosX, 0, destX );
	SetScriptVarValue( static_cast<CMoveShape*>( p_SrcShape ), retPosY, 0, destY );

	M_FREE( retPosX, sizeof(char)*MAX_VAR_LEN );
	M_FREE( retPosY, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::GetRegionPlayerNum(const char* CmdStr, char* retStr)
{
	if(p_SrcShape == NULL) return 0;

	char* strRgnID = GetStringParam(CmdStr,0);
	if(!strRgnID) return -1.0f;

	char* szRetValue = GetStringParam(CmdStr, 1);
	if(!szRetValue) 
	{
		M_FREE( strRgnID, sizeof(char)*MAX_VAR_LEN );
		return -1.0f;
	}

	CGUID rgnGuid;
	if(strRgnID[0] == '\0') // 当前场景
	{
		if(GetSrcShape())
			rgnGuid = GetSrcShape()->GetRegionGUID();
	}
	else
		rgnGuid = GetScriptGuidValue((CMoveShape*)GetSrcShape(), strRgnID);
	if(rgnGuid == NULL_GUID)
	{
		M_FREE( strRgnID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRetValue, sizeof(char)*MAX_VAR_LEN );
		return -1.0f;
	}

	CServerRegion* pRegion = NULL;
	pRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGuid);
	if(pRegion)
	{
		SetScriptVarValue((CMoveShape*)GetSrcShape(), szRetValue, 0, pRegion->GetPlayerAmout());
		M_FREE( strRgnID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRetValue, sizeof(char)*MAX_VAR_LEN );
		return 1.0f;
	}
	else // 需要跨服务器查询
	{
		//等待函数, 收到回应消息再运行脚本
		SetIsHang(true);
		SetHangFunc(SR_FUNC_VIEWGSPLAYERNUM);
		// 发送消息到WS
		CMessage msg(MSG_S2W_RGN_VIEWGSPLAYERNUM);
		msg.Add(rgnGuid);
		msg.Add(p_SrcShape->GetExID()); // 查看者GUID
		msg.Add(GetID()); // script id
		msg.Add(szRetValue);
		msg.Send();
	}
	M_FREE( strRgnID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetValue, sizeof(char)*MAX_VAR_LEN );
	return -1.0f;
}

double CScript::RunScriptForAllPlayerInRgn(const char* CmdStr, char* retStr)
{
	if( m_pRegion != 0 )
	{
		char* strPath = GetStringParam(CmdStr,0);
		static_cast<CServerRegion*>( m_pRegion )->RunScriptAllPlayer(strPath);
		M_FREE( strPath, sizeof(char)*MAX_VAR_LEN );
	}
	return 0;
}

//以特定蓝图创建一个副本，并返回该副本的ＩＤ，如果因副本数量已达到上限则创建失败被返回0。
double CScript::CreatePRgn(const char* CmdStr, char* retStr)
{
	//等待函数, 收到回应消息再运行脚本
	SetIsHang(true);
	SetHangFunc(SR_FUNC_CREATEPRGN);

	char* szPGUID = GetStringParam(CmdStr, 0);
	if(szPGUID == NULL) return 0;

	CGUID pGuid = NULL_GUID;
	pGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPGUID);
	CPlayer* player = GetGame()->FindPlayer(pGuid);
	if(player == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// 异步操作返回RgnID
	char* szRetRgnGuid = GetStringParam(CmdStr, 1);

	if(szRetRgnGuid == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}


	double templateID = GetIntParam(CmdStr,2);

	//wq:2008-7-9,需要修改
	player->SetCreateDupRgnType(RGN_PERSONAL);
	player->SetlCreateDupRgnTID(templateID);
	player->SetlCreateDupRgnScriptID(this->GetID());

	// 返回值参数，返回异步操作的结果
	char* szRetVarName = GetStringParam(CmdStr, 3);
	if(szRetVarName == NULL) 
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long lRetFlag = GameManager::GetInstance()->GetRgnManager()->CreateRgnMsg(this, templateID, RGN_PERSONAL, player->GetExID(), szRetVarName, szRetRgnGuid); // 向WS 发送消息
	if(lRetFlag < 0)
		SetIsHang(false);

	M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetVarName, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
double CScript::CreatePHRgn(const char* CmdStr, char* retStr)
{
	char* szPGUID = GetStringParam(CmdStr, 0);
	if(szPGUID == NULL) return 0;

	CGUID pGuid = NULL_GUID;
	pGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPGUID);
	CPlayer* player = GetGame()->FindPlayer(pGuid);
	if(player == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// 玩家已有房屋副本
	if(player->GetPersonalHouseRgnGUID() != NULL_GUID)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	//等待函数, 收到回应消息再运行脚本
	SetIsHang(true);
	SetHangFunc(SR_FUNC_CREATEPHRGN);

	// 异步操作返回RgnID
	char* szRetRgnGuid = GetStringParam(CmdStr, 1);
	if(szRetRgnGuid == NULL) 
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	double templateID = GetIntParam(CmdStr, 2);

	player->SetCreateDupRgnType(RGN_PERSONAL_HOUSE);
	player->SetlCreateDupRgnTID(templateID);
	player->SetlCreateDupRgnScriptID(this->GetID());	

	// 返回值参数，返回异步操作的结果
	char* szRetVarName = GetStringParam(CmdStr, 3);
	if(szRetVarName == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long lRetFlag = GameManager::GetInstance()->GetRgnManager()->CreateRgnMsg(this, templateID, RGN_PERSONAL_HOUSE, player->GetExID(), szRetVarName, szRetRgnGuid); // 向WS 发送消息
	if(lRetFlag < 0)
		SetIsHang(false);

	M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetVarName, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}

double CScript::CreateTRgn(const char* CmdStr, char* retStr)
{
	//等待函数, 收到回应消息再运行脚本
	SetIsHang(true);
	SetHangFunc(SR_FUNC_CREATETRGN);

	char* szPGUID = GetStringParam(CmdStr, 0);
	if(szPGUID == NULL) return 0;

	CGUID pGuid = NULL_GUID;
	pGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPGUID);
	CPlayer* player = GetGame()->FindPlayer(pGuid);
	if(player == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// 异步操作返回RgnID
	char* szRetRgnGuid = GetStringParam(CmdStr, 1);
	if(szRetRgnGuid == NULL) 
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	double templateID = GetIntParam(CmdStr, 2);

	
	//wq:2008-7-9,需要修改
	// 将该玩家放入排队,map自动按long的时间值排序,小的排前面		
	player->SetCreateDupRgnType(RGN_TEAM);
	player->SetlCreateDupRgnTID(templateID);
	player->SetlCreateDupRgnScriptID(this->GetID());	
	
	// 返回值参数，返回异步操作的结果
	char* szRetVarName = GetStringParam(CmdStr, 3);
	if(szRetVarName == NULL)
	{
		M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long lRetFlag = GameManager::GetInstance()->GetRgnManager()->CreateRgnMsg(this, templateID, RGN_TEAM, player->GetExID(), szRetVarName, szRetRgnGuid); // 向WS 发送消息
	if(lRetFlag < 0)
		SetIsHang(false);

	M_FREE( szPGUID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetVarName, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
// 取得当前脚本玩家个人副本GUID	GetCurPlayerPDupRgnID(@Ret);
double CScript::GetCurPlayerPDupRgnID	(const char* CmdStr, char* retStr)
{
	int ret = 0;
	char* szRetRgnGuid = GetStringParam(CmdStr, 0);
	if(!szRetRgnGuid) return 0;

	CPlayer* player = (CPlayer*)p_SrcShape;

	if(player)
	{
		const CGUID& sGuid = player->GetPersonalRgnGUID();
		ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetRgnGuid, sGuid);
	}

	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return (double)ret;
}
//取得当前脚本玩家个人房屋副本GUID	GetCurPlayerPHDupRgnID(@Ret);
double CScript::GetCurPlayerPHDupRgnID	(const char* CmdStr, char* retStr)
{
	int ret = 0;
	char* szRetRgnGuid = GetStringParam(CmdStr, 0);
	if(!szRetRgnGuid) return 0;

	CPlayer* player = (CPlayer*)p_SrcShape;

	if(player)
	{
		const CGUID& sGuid = player->GetPersonalHouseRgnGUID();
		ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetRgnGuid, sGuid);
	}

	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return (double)ret;
}


double CScript::GetPlayerPDupRgnID(const char* CmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(CmdStr, 0);
	if(!szPlayerGuid) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	int ret = 0;
	char* szRetRgnGuid = GetStringParam(CmdStr, 1);
	if(szRetRgnGuid == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	
	if(player)
	{
		const CGUID& sGuid = player->GetPersonalRgnGUID();
		ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetRgnGuid, sGuid);
	}
	else
	{
		CMessage msg(MSG_S2W_RGN_VIEWPLAYERPID); // 查看玩家的个人副本
		msg.Add(sPGuid);
		if(szRetRgnGuid)
			msg.Add(szRetRgnGuid);
		else
			msg.Add("");
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETPLAYERPDUPRGNID);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return (double)ret;
}
double CScript::GetPlayerPHDupRgnID(const char* CmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(CmdStr, 0);
	if(!szPlayerGuid) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	int ret = 0;
	char* szRetRgnGuid = GetStringParam(CmdStr, 1);
	if(szRetRgnGuid == NULL)
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(player)
	{
		const CGUID& sGuid = player->GetPersonalHouseRgnGUID();
		int ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetRgnGuid, sGuid);
	}
	else
	{
		CMessage msg(MSG_S2W_RGN_VIEWPLAYERPHID); // 查看玩家的个人房屋副本
		msg.Add(sPGuid);
		if(szRetRgnGuid)
			msg.Add(szRetRgnGuid);
		else
			msg.Add("");
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETPLAYERPHDUPRGNID);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return (double)ret;
}
double CScript::GetTeamDupRgnID(const char* CmdStr, char* retStr)	
{
	// 查找player
	char* szPlayerGuid = GetStringParam(CmdStr, 0);
	if(!szPlayerGuid) return 0;

	LONG lRgnID = GetIntParam(CmdStr, 1);

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	char* szRetRgnGuid = GetStringParam(CmdStr, 2);
	if(szRetRgnGuid == NULL)
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	int ret = 0;
	if(sPGuid != NULL_GUID)
	{
		const CGUID& sGuid = GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGUID(sPGuid, lRgnID);
		int ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetRgnGuid, sGuid);
	}
	else
	{
		CMessage msg(MSG_S2W_RGN_VIEWPLAYERTID); // 查看玩家的组队副本
		msg.Add(sPGuid);
		msg.Add(lRgnID);
		if(szRetRgnGuid)
			msg.Add(szRetRgnGuid);
		else
			msg.Add("");
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETTEAMDUPRGNID);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return (double)ret;
}
double CScript::GetNormalDupRgnID(const char* CmdStr, char* retStr)
{
	long rgnTID = (long)GetIntParam(CmdStr,0);
	if( rgnTID == ERROR_CODE )
	{
		return 0;
	}

	// 查找Rgn
	char* szGuid = GetStringParam(CmdStr, 1);
	if(szGuid == NULL) return 0;

	CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnTID);
	if(pRgn)
	{
		int ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szGuid, pRgn->GetExID());
	}
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}

double CScript::ChangeDupRgn(const char* cmdStr, char* retStr)
{
	CPlayer* player = (CPlayer*)p_SrcShape;
	if(player == NULL) return 0;

	// 查找RgnID
	char* szRgnGuid = GetStringParam(cmdStr, 0);
	if(szRgnGuid == NULL) return 0;

	// rgn type
	long rType = GetIntParam(cmdStr, 1);
	if(rType == ERROR_CODE) 
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// Rgn GUID
	CGUID RgnGuid;
	RgnGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);
	long ret = 0;
	if(player)
	{
		long x = GetIntParam(cmdStr, 2);
		if(x == ERROR_CODE) x = -1;
		long y = GetIntParam(cmdStr, 3);
		if(y == ERROR_CODE) y = -1;

		ret = player->ChangeRegion((eRgnType)rType, RgnGuid, x, y, 0);
	}

	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
// 切换场景by模板id
double CScript::ChangeRgnByTemplateID (const char* CmdStr, char* retStr)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(p_SrcShape);
	if(!pPlayer) return 0;

	double templateID = GetIntParam(CmdStr,0);

	if( templateID == ERROR_CODE )
	{
		return 0;
	}

	//查找ServerRegion  GUID
	CGUID rgnGUID = NULL_GUID;
	CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, templateID);
	if(pRgn)
		rgnGUID = pRgn->GetExID();
	
	double x = GetIntParam(CmdStr,1);
	double y = GetIntParam(CmdStr,2);
	double dir = GetIntParam(CmdStr,3);
	double lRange = GetIntParam(CmdStr, 4);
	double lCarriageDistance = GetIntParam(CmdStr,5);

	if( x == ERROR_CODE || y == ERROR_CODE )
	{
		x = y = -1;
		dir = pPlayer->GetDir();
	}
	if( dir == ERROR_CODE )
	{
		dir = pPlayer->GetDir();
	}
	if (lRange==ERROR_CODE)
	{
		lRange = 2;
	}
	if (lCarriageDistance==ERROR_CODE)
	{
		lCarriageDistance = 0;
	}
	pPlayer->ChangeRegion(RGN_NORMAL, rgnGUID, x, y, dir, templateID, 0,lRange, lCarriageDistance);
	return 0;
}
// 添加场景变量
double CScript::AddRgnVar(const char* cmdStr, char* retStr)
{
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 查找player
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(!szRgnGuid) return 0;

	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	if(sPGuid == NULL_GUID)
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0.0f;
	}

	CMessage msg(MSG_S2W_RGN_VAR_ADD);
	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Add(GetID());
	msg.Add(rgnType); // leixing

	// 场景指针
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(sPGuid);

	// 需要异步操作
	if(NULL == rgn)
	{
		SetIsHang(true);
		SetHangFunc(SR_FUNC_ADDRGNVAR);
	}

	// RgnID
	msg.Add(sPGuid);

	// 取第一个参数： 变量名字符串
	char* firstVarName = NULL;
	firstVarName = GetStringParam(cmdStr, 2);
	if(firstVarName)
	{
		if( (firstVarName[0] != '#' && firstVarName[0] != '@' && firstVarName[0] != '$')  )
		{
			TestLog(CStringReading::LoadString(IDS_GS_RGNFUNC, STR_GS_RGNFUNC_ADDRGNVARERR));
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		msg.Add(firstVarName);

		// 读取字符串值
		if(firstVarName[0] == '#') // 字符串
		{
			char* strValue = NULL;
			strValue = GetStringParam(cmdStr, 3);

			if(m_pVariableList && strValue)
			{	
				if(rgn != NULL)
				{
					rgn->GetVariableList()->AddVar(firstVarName, strValue);
					long size = (long)strlen(strValue);	
					msg.Add(size);
					msg.AddEx((void*)strValue, size);
				}
			}
			M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
		}
		else if(firstVarName[0] == '$') //数字
		{
			double value = GetIntParam(cmdStr, 3);

			if(m_pVariableList && (value != ERROR_CODE) )
			{
				if(rgn != NULL)
					rgn->GetVariableList()->AddVar(firstVarName, value);
				{
					msg.Add((long)sizeof(double));
					msg.AddEx((void*)&value, sizeof(double));
				}
			}
		}

		else if(firstVarName[0] == '@') // 定义GUID
		{
			char* strValue = NULL;
			strValue = GetStringParam(cmdStr, 3);

			if(strValue)
			{
				CGUID tGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, strValue);
				if(rgn != NULL)
					rgn->GetVariableList()->AddGuid(firstVarName, tGUID);
				msg.Add(tGUID);
				M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
			}
		}

		M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
	}

	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Send();
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
// 设置场景变量
double CScript::SetRgnVar(const char* cmdStr, char* retStr)
{
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 查找 rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	// rgn GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	if(sPGuid == NULL_GUID) 
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0.0f;
	}

	CMessage msg(MSG_S2W_RGN_VAR_SET);
	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Add(GetID());
	msg.Add(rgnType); // leixing

	// 场景指针
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(sPGuid);

	// 需要异步操作
	if(NULL == rgn)
	{
		SetIsHang(true);
		SetHangFunc(SR_FUNC_SETRGNVAR);
	}

	msg.Add(sPGuid);
	
	// 取第一个参数： 变量名字符串
	char* firstVarName = NULL;
	firstVarName = GetStringParam(cmdStr, 2);

	if(firstVarName)
	{
		if( (firstVarName[0] != '#' && firstVarName[0] != '@' && firstVarName[0] != '$')  )
		{
			TestLog(CStringReading::LoadString(IDS_GS_RGNFUNC, STR_GS_RGNFUNC_SETRGNVARERR));
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		msg.Add(firstVarName);

		// 读取字符串值
		if(firstVarName[0] == '#') // 字符串
		{
			char* strValue = NULL;
			strValue = GetStringParam(cmdStr, 3);

			if(m_pVariableList && strValue)
			{
				if(rgn != NULL)
					rgn->GetVariableList()->SetVarValue(firstVarName, strValue);
				long size = (long)strlen(strValue);
				msg.Add(size);
				msg.AddEx((void*)strValue, size);				
			}
			M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
		}
		else if(firstVarName[0] == '$') //数字
		{
			double value = GetIntParam(cmdStr, 3);

			if(m_pVariableList && (value != ERROR_CODE) )
			{
				if(rgn != NULL)
					rgn->GetVariableList()->SetVarValue(firstVarName, 0, value);
				msg.Add((long)sizeof(double));
				msg.AddEx((void*)&value, sizeof(double));
			}
		}
		else if(firstVarName[0] == '@') // 定义GUID
		{
			char* strValue = NULL;
			strValue = GetStringParam(cmdStr, 3);

			if(strValue)
			{
				CGUID tGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, strValue);
				if(rgn != NULL)
					rgn->GetVariableList()->SetGuidValue(firstVarName, &tGUID);
				msg.Add(tGUID);
				M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			}
		}

		M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
	}

	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Send();
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
// 删除场景变量
double CScript::DelRgnVar(const char* cmdStr, char* retStr)
{
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 查找player
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(!szRgnGuid) return 0;

	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);
	
	if(sPGuid == NULL_GUID)
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0.0f;
	}

	CMessage msg(MSG_S2W_RGN_VAR_DEL);
	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Add(GetID());
	msg.Add(rgnType); // leixing

	// 场景指针
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(sPGuid);

	// 需要异步操作
	if(NULL == rgn)
	{
		SetIsHang(true);
		SetHangFunc(SR_FUNC_DELRGNVAR);
	}

	msg.Add(sPGuid);

	// 取第一个参数： 变量名字符串
	char* firstVarName = NULL;
	firstVarName = GetStringParam(cmdStr, 2);

	if(firstVarName)
	{
		if( (firstVarName[0] != '#' && firstVarName[0] != '@' && firstVarName[0] != '$')  )
		{
			TestLog(CStringReading::LoadString(IDS_GS_RGNFUNC, STR_GS_RGNFUNC_DELRGNVARERR));
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		// 读取字符串值
		if(firstVarName[0] == '#') // 字符串
		{
			if(m_pVariableList)
			{
				if(rgn != NULL)
					rgn->GetVariableList()->RemoveVar(firstVarName);
				msg.Add(firstVarName);
			}
		}
		else if(firstVarName[0] == '$') //数字
		{
			if(m_pVariableList)
			{
				if(rgn != NULL)
					rgn->GetVariableList()->RemoveVar(firstVarName);
				msg.Add(firstVarName);	
			}
		}
		else if(firstVarName[0] == '@') // 定义GUID
		{
			if(rgn != NULL)
				rgn->GetVariableList()->RemoveGuid(firstVarName);
			msg.Add(firstVarName);
		}

		M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
	}

	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Send();
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
// 取得指定场景变量
double CScript::GetRgnVar(const char* cmdStr, char* retStr)
{
	double ret = 0;

	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 查找rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(!szRgnGuid) return 0;

	// rgn GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	if(sPGuid == NULL_GUID) 
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0.0f;
	}

	CMessage msg(MSG_S2W_RGN_VAR_GET);
	msg.Add((BYTE)GetGame()->GetIDIndex());
	msg.Add(GetID()); // script id
	msg.Add(rgnType); // leixing

	// 场景指针
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(sPGuid);
	if(NULL == rgn)
	{
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETRGNVAR);
	}
	msg.Add(sPGuid);

	// 取第一个参数： 变量名字符串
	char* firstVarName = NULL;
	firstVarName = GetStringParam(cmdStr, 2);

	if(firstVarName)
	{
		if( (firstVarName[0] != '#' && firstVarName[0] != '@' && firstVarName[0] != '$')  )
		{
			TestLog(CStringReading::LoadString(IDS_GS_RGNFUNC, STR_GS_RGNFUNC_GETRGNVARERR));
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		// 读取字符串值
		if(firstVarName[0] == '#') // 字符串
		{
			// 返回值变量名
			char* strValue = GetStringParam(cmdStr, 3);

			if(m_pVariableList)
			{
				if(rgn != NULL)
				{
					if(strValue)
					{
						char* strRet = (char*)rgn->GetVariableList()->GetVarValue(firstVarName);
						SetScriptVarValue((CMoveShape*)p_SrcShape, strValue, strRet);
					}
				}
				else
				{
					if(strValue)
						msg.Add(firstVarName);
					else
						msg.Add("");
					msg.Add(strValue); // 添加返回值字符串变量名
				}
			}
			M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
		}
		else if(firstVarName[0] == '$') //数字
		{
			char* strValue = GetStringParam(cmdStr, 3);

			if(m_pVariableList && (strValue != NULL) )
			{
				if(rgn != NULL)
				{
					if(strValue)
					{
						ret = rgn->GetVariableList()->GetVarValue(firstVarName, 0);
						SetScriptVarValue((CMoveShape*)p_SrcShape, strValue, 0, ret);
					}
				}
				else
				{
					if(strValue)
						msg.Add(firstVarName);
					else
						msg.Add("");
					msg.Add(strValue); // 添加返回值字符串变量名
				}
			}
			M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
		}

		else if(firstVarName[0] == '@') // 定义GUID
		{
			char* strValue = GetStringParam(cmdStr, 3);

			if(rgn != NULL)
			{
				CGUID oGUID;
				if(strValue)
				{
					oGUID = rgn->GetVariableList()->GetGuidValue(firstVarName);
					SetScriptGuidValue((CMoveShape*)p_SrcShape, strValue, oGUID);
				}
			}
			else
			{
				if(strValue)
					msg.Add(firstVarName);
				else
					msg.Add("");
				msg.Add(strValue); // 添加返回值字符串变量名
			}

			M_FREE( strValue, sizeof(char)*MAX_VAR_LEN );
		}
		M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
	}
	if(!rgn)
	{
		msg.Send();
	}
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
//删除指定副本ＩＤ的副本。
double CScript::DeleteRgn(const char* cmdStr, char* retStr)
{
	// GSID
//	long GSID = (long)GetIntParam(cmdStr, 0);
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);
	

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 1);
	if(guid == NULL || guid[0] != '@') 
	{
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	if(p_SrcShape)
		rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);

	char* szRetVarName = GetStringParam(cmdStr, 2);
	if(!szRetVarName) 
	{
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}
	double dRet = 0.0f;

	//wq:2008-7-9,需要修改
	dRet = GameManager::GetInstance()->GetRgnManager()->DeleteRgnMsg(this->GetID(), (eRgnType)rgnType, NULL_GUID, rgnGUID, szRetVarName, guid);
	// 挂起script，等待WS的回应，然后执行删除操作
	SetIsHang(true);
	SetHangFunc(SC_FUNC_DELETERGN);
	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetVarName, sizeof(char)*MAX_VAR_LEN );
	return dRet;
}

//返回指定副本ＩＤ的副本的蓝图ＩＤ，如该副本ＩＤ的副本不存在则返回0。
double CScript::GetTemplateIDByGUID	(const char* cmdStr, char* retStr)
{
	double ret = -1.0f;

	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);
	if(rgnType == ERROR_CODE) return 0;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 1);
	if(guid == NULL) return 0;

	// 返回值变量名
	char* szRet = GetStringParam(cmdStr, 2);
	if(szRet == NULL) 
	{
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	if(p_SrcShape)
		rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);
	
	CServerRegion* findRgn = NULL;
	switch((eRgnType)rgnType)
	{
	case RGN_PERSONAL:
		{
			CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().find(rgnGUID);
			if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end())
			{
				findRgn = itr->second;
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(rgnGUID);
			if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
			{
				findRgn = itr->second;
			}
		}
		break;
	case RGN_TEAM:
		{
			CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().find(rgnGUID);
			if(itr != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end())
			{
				findRgn = itr->second;
			}
		}
		break;
	case RGN_NORMAL:
		{
			CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().find(rgnGUID);
			if(itr != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end())
			{
				findRgn = itr->second;
			}
		}
		break;
	}

	//wq:2008-7-9,需要修改
	if(findRgn != NULL) // 本地Rgn
	{
		SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, (double)findRgn->GetID());
	}
	else
	{
		// 发送消息到WS
		CMessage msg(MSG_S2W_RGN_GETTEMPLATEID);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add((long)rgnType);
		msg.Add(rgnGUID);
		msg.Add(szRet);
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SC_FUNC_GETMPLATEIDBYGUID);
	}
	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//返回当前所在场景的ＩＤ，如果该场景不存在则返回0。
double CScript::GetCurRgnGUID(const char* cmdStr, char* retStr)
{
	double ret = 0.0f;
	char* guid = NULL;

	if((CPlayer*)p_SrcShape)
	{
		// GUID变量名
		guid = GetStringParam(cmdStr, 0);
		if(guid == NULL) return 0;

		// 查找GUID
		int sRet = SetScriptGuidValue((CMoveShape*)p_SrcShape, guid, m_guRegionID );
		if(sRet != CVariableList::VAR_NOT_FOUND)
			ret = 1.0f;
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//返回指定副本ＩＤ的副本中当前在线人数。（原GetRegionPlayerNum）
//返回指定副本ＩＤ的副本的类型。
double CScript::GetDupTypeByGUID(const char* cmdStr, char* retStr)
{
	double ret = -1.0f;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(NULL == guid) return 0;

	// 返回值变量名
	char* szRet = GetStringParam(cmdStr, 1);
	if(NULL == szRet) 
	{
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	if(p_SrcShape)
		rgnGUID=GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);

	CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().find(rgnGUID);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end())
	{
		SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, (double)RGN_PERSONAL);
		ret = 0.0f;
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
		return ret;
	}

	if(itr == GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end())
	{
		itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(rgnGUID);
		if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
		{
			SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, (double)RGN_PERSONAL_HOUSE);
			ret = 0.0f;
			M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
			return ret;
		}
	}

	if(itr == GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
	{
		itr = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().find(rgnGUID);
		if(itr != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end())
		{
			SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, (double)RGN_TEAM);
			ret = 0.0f;
			M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
			return ret;
		}
	}

	if(itr == GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end())
	{
		// 发送消息到WS
		CMessage msg(MSG_S2W_RGN_GETDUPTYPE);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(rgnGUID);
		msg.Add(szRet);
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(RGN_FUNC_GETDUPTYPEBYGUID);
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//返回指定副本ＩＤ的副本的所有者ＩＤ。
double CScript::GetDupRgnOnwerGUID	(const char* cmdStr, char* retStr)
{
	double ret = 0.0f;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid == NULL) return 0;

	char* ownerGuid = GetStringParam(cmdStr, 1);
	if(ownerGuid == NULL)
	{
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);
	CServerRegion* sRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
	if(sRgn)
		SetScriptGuidValue((CMoveShape*)p_SrcShape, ownerGuid, sRgn->GetOwnerGUID());
	else
	{
		// 发送消息到WS
		CMessage msg(MSG_S2W_RGN_GETDUPRGNONWER);
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(rgnGUID);
		msg.Add(ownerGuid);
		msg.Add(GetID());
		msg.Send();
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETDUPRGNONWERGUID);
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( ownerGuid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//返回指定副本ＩＤ的副本的人数上限。
double CScript::GetDupRgnPlayerMaxNum(const char* cmdStr, char* retStr)
{
	double ret = 0.0f;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid == NULL) return 0;

	if(guid)
	{
		// rgn GUID
		CGUID rgnGUID;
		// 查找GUID
		if(p_SrcShape)
			rgnGUID=GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);
		
		CServerRegion* sRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
		if(sRgn)
		{
			switch((eRgnType)sRgn->GetRgnType())
			{
			case RGN_PERSONAL:
				ret = CGlobeSetup::GetSetup()->lPDupRgnMaxPlayerNum;
				break;
			case RGN_PERSONAL_HOUSE:
				ret = CGlobeSetup::GetSetup()->lPHDupRgnMaxPlayerNum;
				break;
			case RGN_TEAM:
				ret = CGlobeSetup::GetSetup()->lTDupRgnMaxPlayerNum;
				break;
			}
		}
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//取得个人GUID
double CScript::GetPlayerGUID(const char* cmdStr, char* retStr)
{
	double ret = 0;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid)
	{
		int isFind = INVILID_VALUE;
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
		if(pMoveShape)
			isFind = SetScriptGuidValue(pMoveShape, guid, pMoveShape->GetExID());
		if(isFind != INVILID_VALUE)
			ret = 1.0f;
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

//取得目标的GUID
double CScript::GetDestGUID(const char* cmdStr, char* retStr)
{
	double ret = 0;
	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid)
	{
		int isFind = INVILID_VALUE;
		CMoveShape* pSrcMoveShape = dynamic_cast<CMoveShape*>(p_SrcShape);
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(p_DesShape);
		if(pSrcMoveShape && pMoveShape)
			isFind = SetScriptGuidValue(pSrcMoveShape, guid, pMoveShape->GetExID());
		if(isFind != INVILID_VALUE)
			ret = 1.0f;
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}


//返回指定角色ＩＤ玩家角色的个人房屋副本ＩＤ。（需要该玩家不在线时也可查询）
double CScript::GetPHRgnGUIDByOnwerName	(const char* cmdStr, char* retStr)
{
	double ret = 0;

	// 角色Name
	char* szPlayerName = GetStringParam(cmdStr, 0);
	if(szPlayerName == NULL) return 0;

	// 查找GUID
	CPlayer* player = GetGame()->FindPlayer(szPlayerName);

	// 返回GUID
	char* szRetGUID = GetStringParam(cmdStr, 1);
	if(szRetGUID == NULL)
	{
		M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnGUID;

	if(player)
	{	
		rgnGUID = player->GetPersonalHouseRgnGUID();
		int tret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetGUID, rgnGUID);
		ret = 1.0f;
	}
	else // 本地没有该玩家数据
	{
		//wq:2008-7-9,需要修改
		//等待函数, 直到收到数据库返回消息才取消该挂起标志
		SetIsHang(true);
		SetHangFunc(SC_FUNC_GETPHRGNGUIDBYONWERNAME);
		// 发送消息到WS,WS如果没有数据则到数据库检查
		CMessage msg(MSG_S2W_SERVER_PLAYER_PERSONALHOUSEGUID);
		msg.Add(GetID());// Script ID
		msg.Add(szPlayerName); // PlayerGUID
		msg.Add(szRetGUID); // 返回的PH GUID 变量名
		msg.SendToSocket(GetGame()->GetDBSSocket());
	}

	M_FREE( szPlayerName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//随机返回当前副本中的一个不为阻挡的坐标，并将坐标值保存到指定的2个玩家变量中。（原GetRegionRandomPos）
//传送到指定副本的指定坐标。（原ChangeRegion）
//队伍中达成条件的成员全部传送到指定副本的指定坐标。（原SetTeamRegion）
//在指定副本中指定坐标创建指定原始名、数量、脚本的生物。（原CreateMonster）
//删除指定副本中所有为指定原始名的生物。
double CScript::DeleteMonster		(const char* cmdStr, char* retStr)
{
	return DelMonster(cmdStr);
}

//在指定副本中指定坐标创建指定ＩＤ的地图地物。（暂定，可能公测后开放）
double CScript::CreateDupRgnEntity	(const char* cmdStr, char* retStr)
{
	return 0;
}
//以副本为执行者执行一个脚本。（即使该副本中没有玩家脚本也继续执行）
double CScript::RunDupRgnScript(const char* cmdStr, char* retStr)
{
	double ret = 0;
	//return call(cmdStr);
	char* szScript = GetStringParam(cmdStr, 0);
	if(!szScript) return 0;

	stRunScript stRunScr;
	stRunScr.srcShape = NULL;
	stRunScr.desShape = NULL;
	stRunScr.pszFileName = szScript;

	RunScript(&stRunScr, const_cast<CHAR*>( GetGame() -> GetScriptFileData(szScript)));
	M_FREE( szScript, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
//返回目前游戏中存在的副本数量。
double CScript::GetDupRgnNum(const char* cmdStr, char* retStr)
{
	//等待函数, 收到回应消息再运行脚本
	SetIsHang(true);
	SetHangFunc(SR_FUNC_GETDUPRGNNUM);

	double ret = 0;
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 返回的WS上所有副本个数
	char* szRetNumVar = GetStringParam(cmdStr, 1);
	if(szRetNumVar == NULL) return -1.0f;

	CMessage msg(MSG_S2W_RGN_VIEWRGNNUM);
	msg.Add(rgnType);
	msg.Add(p_SrcShape->GetExID()); // 查看者GUID
	if(szRetNumVar)
		msg.Add(szRetNumVar);
	else
		msg.Add("");
	msg.Add(GetID()); // script id
	msg.Send();
	
	M_FREE( szRetNumVar, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//返回目前游戏中存在的副本数量上限。
double CScript::GetDupRgnMaxNum(const char* cmdStr, char* retStr)
{
	double ret = 0;
	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	switch((eRgnType)rgnType)
	{
	case RGN_NORMAL:
		ret = -1.0f;
		break;
	case RGN_PERSONAL:
		ret = CGlobeSetup::GetSetup()->lPDupRgnMaxNum;
		break;
	case RGN_PERSONAL_HOUSE:
		ret = CGlobeSetup::GetSetup()->lPHDupRgnMaxNum;
		break;
	case RGN_TEAM:
		ret = CGlobeSetup::GetSetup()->lTDupRgnMaxNum;
		break;
	}
	return ret;
}
//取得指定副本ＩＤ的副本的管理权开放（1／0）。
double CScript::GetDupRgnOperFlag(const char* cmdStr, char* retStr)
{
	double ret = 0;

	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid == NULL) return 0;

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	if(p_SrcShape)
		rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);
	int sRet = INVILID_VALUE;
	CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().find(rgnGUID);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end())
	{
		sRet = itr->second->GetOperFlag();
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return sRet;
	}

	itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(rgnGUID);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
	{
		sRet = itr->second->GetOperFlag();
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return sRet;
	}

	itr = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().find(rgnGUID);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end())
	{
		sRet = itr->second->GetOperFlag();
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return sRet;
	}


	itr = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().find(rgnGUID);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end())
	{
		sRet = itr->second->GetOperFlag();
		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return sRet;
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

// 取得当前脚本调用者的GUID
double CScript::GetCallerGUID(const char* cmd, char* retStr)
{
	char* szRetGUID;
	CShape* pLeader = p_SrcShape;
	if(pLeader == NULL) return 0;

	szRetGUID = GetStringParam(cmd, 0);
	if(szRetGUID)
	{
		SetScriptGuidValue((CMoveShape*)p_SrcShape, szRetGUID, pLeader->GetExID());
	}
	M_FREE( szRetGUID, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}
//返回执行者当前所在队伍中的指定队伍顺序号的角色ＩＤ。
double CScript::GetTeamMaterIDByPos(const char* cmdStr, char* retStr)
{
	double ret = 0;
	CPlayer* player = (CPlayer*)p_SrcShape;
	if(player == NULL) return 0;

	long pos = GetIntParam(cmdStr, 0);
	if(pos == ERROR_CODE)
		return 0;

	char* szMateGUID = GetStringParam(cmdStr, 1);
	if(szMateGUID == NULL) return 0;

	CGUID teamGUID = player->GetTeamID();
	GSTeam* team = (GSTeam*)GetOrganiCtrl()->GetGameTeamByID(teamGUID);
	if(team) // 组队传送
	{
		list<CGUID> teamMemberList;
		team->GetMemberList(teamMemberList);
		list<CGUID>::iterator teamItr = teamMemberList.begin();
		long num = 0;
		while(teamItr != teamMemberList.end())
		{
			if(pos == num)
				break;
			
			num++;
			teamItr++;
		}

		if(teamItr != teamMemberList.end())
		{
			ret = SetScriptGuidValue(player, szMateGUID, *teamItr);
		}
	}
	
	M_FREE( szMateGUID, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
//修改指定副本ＩＤ的永久副本的显示名，弹出一个输入框来修改。命名规则同玩家角色命名规则。
double CScript::EditDupRgnName(const char* cmdStr, char* retStr)
{
	double ret = 0;
	// GUID变量名
	char* guid = GetStringParam(cmdStr, 0);
	if(guid == NULL) return 0;

	// rgn GUID
	CGUID rgnGUID;
	// 查找GUID
	if(p_SrcShape)
		rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, guid);
	int sRet = INVILID_VALUE;
	
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
	if(rgn)
	{
		char* RgnName = GetStringParam(cmdStr, 1);
		if(RgnName)
		{
			rgn->SetName(RgnName);
			// 发送消息给客户端
			CMessage msg(MSG_S2C_RGN_CHANGENAME);
			msg.Add((long)rgn->GetRgnType());
			msg.Add(rgn->GetExID());
			msg.Add(rgn->GetID());
			msg.Add(rgn->GetResourceID());
			msg.Add((char*)rgn->GetName());
			if(p_SrcShape)
				msg.SendToPlayer(p_SrcShape->GetExID());
			sRet = 1.0f;
		}

		M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
		return sRet;
	}

	M_FREE( guid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

// 创建个人房屋副本物品(Monster)
double CScript::CreatePHGoods(const char* cmdStr, char* retStr)//(const CGUID& ownerID, const char* name, long nCounts, long posX, long posY,
//	long width, long height, long dir, const char* strScript, const char* strTalk);
{
	double ret = 0;
	char* szOwnerID = GetStringParam(cmdStr, 0); // GUID变量名
	if(!szOwnerID) return 0;

	CGUID ownerID = NULL_GUID;
	ownerID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szOwnerID);
	char* name = GetStringParam(cmdStr, 1);
	if(name == NULL) 
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long nCounts = (long)GetIntParam(cmdStr, 2);
	long posX = (long)GetIntParam(cmdStr, 3);
	long posY = (long)GetIntParam(cmdStr, 4);
	long width = (long)GetIntParam(cmdStr, 5);
	long height = (long)GetIntParam(cmdStr, 6);
	long dir = (long)GetIntParam(cmdStr, 7);
	char* strScript = GetStringParam(cmdStr, 8);
	if(strScript == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	char* strTalk = GetStringParam(cmdStr, 9);
	if(strTalk == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CPlayer* player = GetGame()->FindPlayer(ownerID);
	if(player == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		M_FREE( name, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
		M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CGUID phRgnID = player->GetPersonalHouseRgnGUID();
	CRgnManager::RgnByIDMapItr rgnItr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(phRgnID);
	if(rgnItr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
	{
		CServerRegion* pRegion	= rgnItr->second;

		if( pRegion ) // 本地服务器
		{
			GameManager::GetInstance()->GetRgnManager()->CreatePHGoods(phRgnID, name, nCounts, posX, posY,
				width, height, dir, strScript, strTalk);
		}
	}
	else
	{
		// 发送消息到WS，由WS在其它GS上创建Goods
		//##发送消息给World Server，发送后不管
		CMessage msg( MSG_S2W_SERVER_CREATEPHGOODS );
		msg.Add( (BYTE)0 );
		CGUID guid;
		CGUID::CreateGUID(guid);	
		msg.Add(guid);
		msg.Add( ownerID );
		msg.Add( name );
		msg.Add( (long)nCounts );
		msg.Add( (long)width );
		msg.Add( (long)height );			
		msg.Add( (long)posX );
		msg.Add( (long)posY );
		msg.Add ( (long)-1 );
		if( strScript )
		{
			msg.Add( BYTE(1) );
			msg.Add( strScript );
		}
		else 
		{
			msg.Add( BYTE(0) );
		}					
		msg.Send();
	}


	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( name, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strScript, sizeof(char)*MAX_VAR_LEN );
	M_FREE( strTalk, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

// 删除个人房屋副本物品(Monster)
double CScript::DeletePHGoods(const char* cmdStr, char* retStr)//(const CGUID& ownerID, const CGUID& goodsID);
{
	double ret = 0;
	char* szOwnerID = GetStringParam(cmdStr, 0); // GUID变量名
	if(!szOwnerID) return 0;

	CGUID ownerID = NULL_GUID;
	ownerID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szOwnerID);
	char* szgoodsID = GetStringParam(cmdStr, 1);
	if(!szgoodsID) 
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(szgoodsID[0] == '@') // 是GUID
	{
		CGUID goodsID = NULL_GUID;
		if(CScript::GetGeneralVariableList())
			goodsID = CScript::GetGeneralVariableList()->GetGuidValue(szgoodsID);
		if(goodsID == NULL_GUID && p_SrcShape)
			((CPlayer*)p_SrcShape)->GetVariableList()->GetGuidValue(szgoodsID);
		if(goodsID == NULL_GUID)
			GetVariableList()->GetGuidValue(szgoodsID);

		GameManager::GetInstance()->GetRgnManager()->DeletePHGoods(ownerID, goodsID);
	}
	else // 是名字
	{
		GameManager::GetInstance()->GetRgnManager()->DeletePHGoods(ownerID, szgoodsID);
	}

	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szgoodsID, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

// 取得本地GSID
double CScript::GetLocalGSID(const char* cmdStr, char* retStr)
{
	return (double)GetGame()->GetIDIndex();
}

// 通过玩家名字查找GUID
double CScript::GetGUIDByPlayerName  (const char* cmdStr, char* retStr)
{
	double ret = 0;
	char* szgoodsID = NULL;
	char* szOwnerID = GetStringParam(cmdStr, 0); // player名字
	if(!szOwnerID) return 0;

	CPlayer* player = GetGame()->FindPlayer(szOwnerID);
	if(player)//在当前服务器上
	{
		szgoodsID = GetStringParam(cmdStr, 1);
		if(!szgoodsID) 
		{
			M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		const CGUID& goodsID = player->GetExID();
		int err = SetScriptGuidValue((CMoveShape*)p_SrcShape, szgoodsID, goodsID);
		// GUID 地址指针值
		ret = (double)(long)&player->GetExID();
	}
	else
	{
		//不在当前服务器上,向WorldServer提交查询
		CMessage msg(MSG_S2W_GM_QUERY_PLAYER);
		msg.Add(p_SrcShape->GetExID());
		if(szOwnerID)
			msg.Add(szOwnerID);// player名字
		else
			msg.Add("");
		msg.Add(GetID()); // script id
		if(szgoodsID)
			msg.Add(szgoodsID); // GUID 变量名字
		else
			msg.Add("");
		msg.Send();

		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETGUIDBYPLAYERNAME);			
	}

	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szgoodsID, sizeof(char)*MAX_VAR_LEN );

	return ret;
}
// 通过玩家名字查找其P GUID
double CScript::GetPGUIDByPlayerName  (const char* cmdStr, char* retStr)
{
	double ret = 0;

	char* szOwnerID = GetStringParam(cmdStr, 0); // player名字
	if(szOwnerID == NULL) return 0;
	char* szgoodsID = GetStringParam(cmdStr, 1);
	if(szgoodsID == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CPlayer* player = GetGame()->FindPlayer(szOwnerID);
	if(player)//在当前服务器上
	{
		int err = SetScriptGuidValue((CMoveShape*)p_SrcShape, szgoodsID, player->GetPersonalRgnGUID());
		ret = 1.0f;
	}
	else
	{
		//不在当前服务器上,向WorldServer提交查询
		CMessage msg(MSG_S2W_GM_QUERY_PLAYER_P);
		msg.Add(szOwnerID);// player名字
		msg.Add(GetID()); // script id
		msg.Add(szgoodsID); // GUID 变量名字
		msg.Send();

		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETPGUIDBYPLAYERNAME);
		ret = 2.0f;				
	}

	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szgoodsID, sizeof(char)*MAX_VAR_LEN );

	return ret;
}
// 通过玩家名字查找其PH GUID
double CScript::GetPHGUIDByPlayerName  (const char* cmdStr, char* retStr)
{
	double ret = 0;

	char* szOwnerID = GetStringParam(cmdStr, 0); // player名字
	if(szOwnerID == NULL) return 0;
	char* szgoodsID = GetStringParam(cmdStr, 1);
	if(szgoodsID == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CPlayer* player = GetGame()->FindPlayer(szOwnerID);
	if(player)//在当前服务器上
	{
		int err = SetScriptGuidValue((CMoveShape*)p_SrcShape, szgoodsID, player->GetPersonalHouseRgnGUID());
	}
	else
	{
		//不在当前服务器上,向WorldServer提交查询
		CMessage msg(MSG_S2W_GM_QUERY_PLAYER_PH);
		msg.Add(szOwnerID);// player名字
		msg.Add(GetID()); // script id
		msg.Add(szgoodsID); // GUID 变量名字
		msg.Send();

		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETPHGUIDBYPLAYERNAME);
		//            m_nRunningFunction=CommandId;					
	}

	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szgoodsID, sizeof(char)*MAX_VAR_LEN );

	return ret;
}
// 通过玩家名字查找其T GUID
double CScript::GetTGUIDByPlayerName  (const char* cmdStr, char* retStr)
{
	double ret = 0;

	char* szOwnerID = GetStringParam(cmdStr, 0); // player名字
	if(szOwnerID == NULL) return 0;
	char* szgoodsID = GetStringParam(cmdStr, 1);
	LONG lTemplateID = GetIntParam(cmdStr, 2);
	if(szgoodsID == NULL)
	{
		M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CGUID ownerID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szOwnerID);

	if(ownerID != NULL_GUID)//在当前服务器上
	{
		int err = SetScriptGuidValue((CMoveShape*)p_SrcShape, szgoodsID, 
			GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGUID(ownerID, lTemplateID));
		ret = 1.0f;
	}
	else
	{
		//不在当前服务器上,向WorldServer提交查询
		CMessage msg(MSG_S2W_GM_QUERY_PLAYER_T);
		msg.Add(szOwnerID);// player名字
		msg.Add(GetID()); // script id
		msg.Add(szgoodsID); // GUID 变量名字
		msg.Send();

		/*脚本暂停执行,等待返回结果*/
		SetIsHang(true);
		SetHangFunc(SR_FUNC_GETTGUIDBYPLAYERNAME);			
	}

	M_FREE( szOwnerID, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szgoodsID, sizeof(char)*MAX_VAR_LEN );
	return ret;
}

double CScript::ChangeDupRgnByRgnID(const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	// 查找Rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(szRgnGuid == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	long rType = GetIntParam(cmdStr, 2);
	if(rType == ERROR_CODE)
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// player GUID
	CGUID rgnPGuid;
	rgnPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	long ret = 0;
	if(player)
	{
		long x = GetIntParam(cmdStr, 3);
		long y = GetIntParam(cmdStr, 4);
		ret = player->ChangeRegion((eRgnType)rType, rgnPGuid, x, y, 0);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
// 设置当前脚本玩家个人副本GUID       SetPlayerPDupRgnID("@OwnerID","@RgnID");
double CScript::SetPlayerPDupRgnID (const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	// 查找Rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(szRgnGuid == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnPGuid;
	rgnPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);
	
	long ret = 0;
	if(player)
	{
		player->SetPersonalRgnGUID(rgnPGuid);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
// 设置当前脚本玩家个人房屋副本GUID		SetPlayerPHDupRgnID("@OwnerID","@RgnID");	
double CScript::SetPlayerPHDupRgnID (const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	// 查找Rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(szRgnGuid == NULL)
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnPGuid;
	rgnPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	long ret = 0;
	if(player)
	{
		player->SetPersonalHouseRgnGUID(rgnPGuid);
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
// 设置当前脚本队伍副本GUID		AddTeamRgnID("@OwnerID",int,"@RgnID");
double CScript::AddTeamRgnID (const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	LONG templateRgnID = GetIntParam(cmdStr, 1);
	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);
	
	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	// 查找Rgn
	char* szRgnGuid = GetStringParam(cmdStr, 2);
	if(szRgnGuid == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// rgn GUID
	CGUID rgnPGuid;
	rgnPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);
	
	long ret = 0;
	if(NULL != player)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
			
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(sPGuid, templateRgnID, rgnPGuid, FALSE);
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_AddRgnID);
			msg.Add(pTeam->GetLeader());
			msg.Add(templateRgnID);
			msg.Add(rgnPGuid);
			msg.Send(false);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//! 为当前脚本队伍删除副本GUID			DelTeamRgnID("@OwnerID", $templateID);
double CScript::DelTeamRgnID (const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	LONG templateRgnID = GetIntParam(cmdStr, 1);
	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	if(NULL != player)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			GameManager::GetInstance()->GetRgnManager()->DelTeamRgnGUID(sPGuid, templateRgnID);
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_DelRgnID);
			msg.Add(pTeam->GetLeader());
			msg.Add(templateRgnID);
			msg.Send(false);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

//! 为当前脚本队伍清除所有副本GUID		ClearTeamRgnID("@OwnerID");
double CScript::ClearTeamRgnID (const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* player = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	// 查找Player
	player = GetGame()->FindPlayer(sPGuid);

	if(NULL != player)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(sPGuid);
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_ClearRgnID);
			msg.Add(pTeam->GetLeader());
			msg.Send(false);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

double CScript::SwapTeamRgnID(const char* cmdStr, char* retStr)
{
	return 0;
}

//! 得到玩家所在队伍的队伍副本数			GetTeamRgnIDNum("@OwnerID", $Ret);
double CScript::GetTeamRgnIDNum(const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* pPlayer = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	pPlayer = GetGame()->FindPlayer(sPGuid);

	// 返回值变量名
	char* szRet = GetStringParam(cmdStr, 1);
	if(szRet == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(NULL != pPlayer)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			long rgnNum = 0;
			map<LONG, CGUID>* rgnIdMap = GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidMap(pMaster->GetExID());
			if(rgnIdMap)
				rgnNum = rgnIdMap->size();
			SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, (LONG)rgnNum);
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_GetRgnNum);
			msg.Add(pTeam->GetLeader());
			msg.Add(GetID());
			msg.Add(szRet);

			SetIsHang(true);
			SetHangFunc(SR_FUNC_GETTEAMRGNIDNUM);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
//! 得到玩家所在队伍的第N个队伍副本ID		GetTeamRgnIDByIndex("@OwnerID", $N, "@RgnGuid");
double CScript::GetTeamRgnIDByIndex(const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* pPlayer = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	pPlayer = GetGame()->FindPlayer(sPGuid);
	LONG lIndex = GetIntParam(cmdStr, 1);

	// 返回值变量名
	char* szRet = GetStringParam(cmdStr, 2);
	if(szRet == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(NULL != pPlayer)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			char szGuid[128];
			GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidByIndex(pMaster->GetExID(), lIndex).tostring(szGuid);
			SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, szGuid);
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_GetRgnGuid);
			msg.Add(pTeam->GetLeader());
			msg.Add(GetID());
			msg.Add(lIndex);
			msg.Add(szRet);

			SetIsHang(true);
			SetHangFunc(SR_FUNC_GETTEAMRGNIDBYINDEX);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
//! 得到玩家所在队伍的第N个队伍副本的蓝图ID	GetTeamRgnTemplatIDByIndex("@OwnerID", $N, $Ret);
double CScript::GetTeamRgnTemplatIDByIndex(const char* cmdStr, char* retStr)
{
	// 查找player
	char* szPlayerGuid = GetStringParam(cmdStr, 0);
	if(szPlayerGuid == NULL) return 0;

	CPlayer* pPlayer = NULL;
	// player GUID
	CGUID sPGuid;
	sPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szPlayerGuid);

	pPlayer = GetGame()->FindPlayer(sPGuid);
	LONG lIndex = GetIntParam(cmdStr, 1);

	// 返回值变量名
	char* szRet = GetStringParam(cmdStr, 2);
	if(szRet == NULL) 
	{
		M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(NULL != pPlayer)
	{
		GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(pPlayer->GetTeamID());
		if(NULL == pTeam)
		{
			M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}
		CPlayer *pMaster = GetGame()->FindPlayer(pTeam->GetLeader());
		if (NULL != pMaster)
		{
			SetScriptVarValue((CMoveShape*)p_SrcShape, szRet, 0, 
				GameManager::GetInstance()->GetRgnManager()->GetTeamRgnTemplateIdByIndex(pMaster->GetExID(), lIndex));
		}
		else
		{
			CMessage msg(MSG_S2W_TEAM_GetRgnTemplateID);
			msg.Add(pTeam->GetLeader());
			msg.Add(GetID());
			msg.Add(lIndex);
			msg.Add(szRet);

			SetIsHang(true);
			SetHangFunc(SR_FUNC_GETTEAMRGNTEMPLATIDBYINDEX);
		}
	}

	M_FREE( szPlayerGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRet, sizeof(char)*MAX_VAR_LEN );
	return 0;
}

// 查看副本变量数据
double CScript::ViewRgnVar(const char* cmdStr, char* retStr)
{
	if(p_SrcShape == NULL)
		return -1.0f;

	// rgn type
	long rgnType = (long)GetIntParam(cmdStr, 0);

	// 查找Rgn
	char* szRgnGuid = GetStringParam(cmdStr, 1);
	if(szRgnGuid == NULL) return 0;

	// rgn GUID
	CGUID rgnPGuid;
	rgnPGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);
	
	int cmdLen = strlen(cmdStr);

	// 取第一个参数： 变量名字符串
	char* firstVarName = NULL;
	firstVarName = GetStringParam(cmdStr, 2);
	if(szRgnGuid == NULL)
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return -1.0f;
	}

	// 返回值变量名字
	char* retVarName = GetStringParam(cmdStr, 3);
	if(retVarName == NULL)
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
		return -1.0f;
	}

	if(firstVarName)
	{
		if( (firstVarName[0] != '#' && firstVarName[0] != '@' && firstVarName[0] != '$')  )
		{
			TestLog(CStringReading::LoadString(IDS_GS_RGNFUNC, 4));
			M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
			M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
			M_FREE( retVarName, sizeof(char)*MAX_VAR_LEN );
			return 0;
		}

		double firstVarLen = strlen(firstVarName);
		char MainVarName[NAME_STR_NUM];
		char InnVarName[NAME_STR_NUM];

		char* MainVarNamePtr = MainVarName;
		char* InnVarNamePtr  = InnVarName;
		
		//--begin 解析嵌套'['']'使用
		long pos = 0;
		CheckParamStringForArray(firstVarName, firstVarLen, &MainVarNamePtr, NAME_STR_NUM, &InnVarNamePtr, NAME_STR_NUM, pos);
		//--end 解析嵌套'['']'使用

		CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID((eRgnType)rgnType, rgnPGuid);

		// 本地GSID
		if(rgn)
		{
			// 读取字符串值
			if(MainVarName[0] == '#') // 字符串
			{
				char* strValue = NULL;

				if(m_pVariableList)
				{
					if(InnVarName[0] == '\0') // 不是数组变量
					{
						if(rgn)
						{
							strValue = (char*)rgn->GetVariableList()->GetVarValue(MainVarName);
							SetScriptVarValue((CMoveShape*)p_SrcShape, retVarName, strValue);
						}
					}
					else
					{
						int lArray = 0;
						
						if(InnVarName[0] == '$') // 是变量
						{
							if(rgn)
								lArray = rgn->GetVariableList()->GetVarValue(InnVarName, 0);
						}
						else if(InnVarName[0] != '$' && InnVarName[0] != '#') // 是常量
							lArray = atoi(InnVarName);
					
						// 去掉'[' ']'
						char name[128];
						_snprintf(name, 128, "%s%d", MainVarName, lArray);

						if(rgn)
						{
							strValue = (char*)rgn->GetVariableList()->GetVarValue(name);
							SetScriptVarValue((CMoveShape*)p_SrcShape, retVarName, strValue);
						}
					}
				}
			}
			else if(firstVarName[0] == '$') //数字
			{
				double value = 0;

				if(m_pVariableList && (value != ERROR_CODE) )
				{
					if(InnVarName[0] == '\0') // 不是数组变量
					{
						if(rgn)
						{
							value = rgn->GetVariableList()->GetVarValue(MainVarName, 0);
							SetScriptVarValue((CMoveShape*)p_SrcShape, retVarName, 0, value);
						}
					}
					else
					{
						int lArray = 0;
						if(InnVarName[0] == '$') // 是变量
						{
							if(rgn)
								lArray = rgn->GetVariableList()->GetVarValue(InnVarName, 0);
						}
						else if(InnVarName[0] != '$' && InnVarName[0] != '#') // 是常量
							lArray = atoi(InnVarName);

						// 去掉'[' ']'
						if(rgn)
						{
							value = rgn->GetVariableList()->GetVarValue(MainVarName, lArray);
							SetScriptVarValue((CMoveShape*)p_SrcShape, retVarName, lArray, value);
						}
					}
				}

			}
			else if(firstVarName[0] == '@') // 定义GUID
			{
				CGUID tGUID;
				if(InnVarName[0] == '\0') // 不是数组变量
				{
					if(rgn)
						tGUID = rgn->GetVariableList()->GetGuidValue(MainVarName);
				}
				else // 是数组变量
				{
					int lArray = 0;
					if(InnVarName[0] == '$') // 下标是普通变量
					{
						lArray = GetScriptVarValue((CMoveShape*)p_SrcShape, InnVarName, 0);
					}
					else if(InnVarName[0] != '$' && InnVarName[0] != '#') // 下标是常量
					{
						lArray = atoi(InnVarName);
					}

					// 去掉'[' ']'
					for(int i=0; i < lArray; i++)
					{
						char tNum[NAME_STR_NUM];
						char name[NAME_STR_NUM];
						strcpy_s(name, NAME_STR_NUM, MainVarName);
						itoa(i, tNum, 10);
						strcat_s(name, NAME_STR_NUM, tNum);
						if(rgn)
						{
							tGUID = rgn->GetVariableList()->GetGuidValue(name);
							SetScriptGuidValue((CMoveShape*)p_SrcShape, retVarName, tGUID);
						}
					}
				}	
			}
		}
		else // 发送MSG_S2W_RGN_VIEWRGNVAR消息到WS
		{
			//wq:2008-7-9,需要修改
			CMessage msg(MSG_S2W_RGN_VIEWRGNVAR);
			msg.Add((long)rgnType);
			msg.Add(rgnPGuid);
			msg.Add(firstVarName);
			msg.Add(retVarName);
			msg.Add(p_SrcShape->GetExID());
			msg.Add(GetID()); // hunging script ID
			msg.Send();
			SetIsHang(true);
			SetHangFunc(SC_FUNC_VIEWRGNVAR);
		}
	}

	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( firstVarName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( retVarName, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
// 通过GUID调用该player、monster的脚本
double CScript::CallScriptByGUID(const char* cmdStr, char* retStr)
{
	if(NULL == p_SrcShape)	return 0;

	// 查找shape GUID string
	char* szshapeGuid = GetStringParam(cmdStr, 0);
	if(szshapeGuid == NULL) return 0;

	// shape GUID
	CGUID shapePGuid;
	shapePGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szshapeGuid);

	CMoveShape* shape = GetGame()->FindShape(TYPE_PLAYER, shapePGuid);
	
	if(NULL == shape)
	{
		CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pRegion);
		if(pRegion)
			shape = dynamic_cast<CMoveShape*>(pRegion->FindChildObject(TYPE_MONSTER, shapePGuid));
	}		

	char* szScript = GetStringParam(cmdStr, 1);
	if(NULL == szScript)
	{
		M_FREE( szshapeGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	if(shape) // 本地调用脚本
	{
		stRunScript stRunScr;
		stRunScr.pRegion = (CRegion*)(p_SrcShape->GetFather());
		stRunScr.srcShape = shape;
		stRunScr.desShape = NULL;
		stRunScr.pszFileName = szScript;
		RunScript(&stRunScr,(char*)GetGame()->GetScriptFileData(szScript));
	}
	else // 跨服调用脚本
	{
		CMessage msg(MSG_S2W_QUEST_PlayerRunScript);
		msg.Add(shapePGuid); // call guid
		msg.Add(szScript);	 // called script name
		msg.SendToSocket(GetGame()->GetWSSocket());
	}

	M_FREE( szshapeGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szScript, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}

// 查看指定pos的副本详细数据
double CScript::ViewDupRgnData(const char* cmdStr, char* retStr)
{
	long rgnType = (long)GetIntParam(cmdStr, 0);
	long pos = (long)GetIntParam(cmdStr, 1);

	long dataType = (long)GetIntParam(cmdStr, 2);
	char* retVarName = GetStringParam(cmdStr, 3);
	if(retVarName == NULL)
		return -1.0f;

	char* szGuid = GetStringParam(cmdStr, 4);
	if(szGuid == NULL)
	{
		M_FREE( retVarName, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CGUID guid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szGuid);
	CMessage msg(MSG_S2W_RGN_VIEWRGNDATA);
	msg.Add(guid);
	msg.Add(rgnType);
	msg.Add(pos);
	if(p_SrcShape)
		msg.Add(p_SrcShape->GetExID());//viewer id
	else
		msg.Add(NULL_GUID);
	msg.Add(dataType);
	msg.Add(retVarName);
	msg.Add(GetID());
	msg.Send();
	
	SetIsHang(true);
	SetHangFunc(SR_FUNC_VIEWDUPRGNDATA);
	
	M_FREE( retVarName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
	return 1.0f;
}

// @变量赋值接口,Param1:@Src   Param2:@Des
double CScript::EvaluateGUID(const char* cmdStr, char* retStr)
{
	// 查找szSrcGuid
	char* szSrcRgnGuid = GetStringParam(cmdStr, 0);
	if(szSrcRgnGuid == NULL) return 0;

	CGUID rgnSrcGUID;
	// src GUID
	rgnSrcGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szSrcRgnGuid);
	// 查找szDesGuid
	char* szDesGuid = GetStringParam(cmdStr, 1);
	if(szDesGuid == NULL) 
	{
		M_FREE( szSrcRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	// des GUID
	int ret = SetScriptGuidValue((CMoveShape*)p_SrcShape, szDesGuid, rgnSrcGUID);
	M_FREE( szSrcRgnGuid, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szDesGuid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
// 查看Rgn是否存在
double CScript::ViewRgnValidByGUID(const char* cmd, char* retStr)
{
	// 查找szRgnGuid
	char* szRgnGuid = GetStringParam(cmd, 0);
	if(szRgnGuid == NULL) return 0;

	CGUID rgnGUID;
	// src GUID
	rgnGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szRgnGuid);

	// 返回值变量名
	char* szRetName = GetStringParam(cmd, 1);
	if(szRetName == NULL)
	{
		M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
	if(rgn)
	{
		// 为返回值赋值
		int ret = SetScriptVarValue((CMoveShape*)p_SrcShape, szRetName, 0, 1.0f);
	}
	else
	{
		CMessage msg(MSG_S2W_RGN_VIEWRGNAVLID);
		msg.Add(rgnGUID); // rgn guid
		if(p_SrcShape)
			msg.Add(p_SrcShape->GetExID());
		else
			msg.Add(NULL_GUID);
		msg.Add(szRetName); // ret name
		msg.Add(GetID()); //script id
		msg.Send();

		SetIsHang(true);
		SetHangFunc(SR_FUNC_VIEWRGNVALIDBYGUID);
	}

	M_FREE( szRetName, sizeof(char)*MAX_VAR_LEN );
	M_FREE( szRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return 0;
}
//设置指定副本ＩＤ的副本的管理权开放（1／0）。
double CScript::SetDupRgnOperFlag(const char* cmd, char* retStr)
{
	double ret = 0;
	// 查找szSrcGuid
	char* szSrcRgnGuid = GetStringParam(cmd, 0);
	if(szSrcRgnGuid == NULL) return 0;
	
	// flag
	double flag = GetIntParam(cmd, 1);
	if(flag == ERROR_CODE)
	{
		M_FREE( szSrcRgnGuid, sizeof(char)*MAX_VAR_LEN );
		return 0;
	}

	CGUID rgnSrcGUID;
	// src GUID
	rgnSrcGUID = GetScriptGuidValue((CMoveShape*)p_SrcShape, szSrcRgnGuid);
	
	CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnSrcGUID);
	if(rgn)
	{
		bool opFlag = false;
		if(abs(flag-0)<0.00001f) opFlag = false;
		else if(abs(flag-1)<0.00001f) opFlag = true;
		rgn->SetOperFlag(opFlag);
	}

	M_FREE( szSrcRgnGuid, sizeof(char)*MAX_VAR_LEN );
	return ret;
}
// 刷新玩家的副本记录列表
double CScript::ReFreshPlayerDupRgnList(const char* cmd, char* retStr)
{
	char* szGuid = GetStringParam(cmd, 0);
	if(szGuid)
	{
		SetIsHang(true);
		SetHangFunc(SR_FUNC_REFRESHPLAYERDUPRGNLIST);

		CMessage msg(MSG_S2W_RGN_QUESTPLAYERRGNLIST);
		msg.Add(GetID());
		
		CGUID playerGuid;
		if(strcmp(szGuid, "") != 0)
			playerGuid = GetScriptGuidValue((CMoveShape*)p_SrcShape, szGuid);
		else
			playerGuid = p_SrcShape->GetExID();

		msg.Add(playerGuid);

		CPlayer* player = GetGame()->FindPlayer(playerGuid);
		if(player)
		{
			msg.Add((BYTE)1); // 本GS查询
			map<LONG, CGUID>* rgnIdMap = GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidMap(playerGuid);
			if(rgnIdMap)
			{
				msg.Add((long)rgnIdMap->size());
				map<LONG, CGUID>::iterator itr = rgnIdMap->begin();
				for(; itr != rgnIdMap->end(); itr++)
				{
					msg.Add((long)itr->first);
					msg.Add(itr->second);
				}
			}
			else
				msg.Add((long)0);

			
		}
		else
		{
			msg.Add((BYTE)0); // 非本GS查询
		}
		msg.Send();
		M_FREE( szGuid, sizeof(char)*MAX_VAR_LEN );
		return 1;
	}
	return 0;
}
// 清除当前GS的所有该类型副本
double CScript::ClearCurGSDupRgn(const char* cmd, char* retStr)
{
	long rgnType = GetIntParam(cmd, 0);
	if(rgnType == ERROR_CODE)
		return 0;

	CShape* player = p_SrcShape;
	if(player == NULL) return 0;

	CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
	if(rgnManager)
	{
		CServerRegion* rgn = NULL;
		switch(rgnType)
		{
		case RGN_PERSONAL:
			{
				CRgnManager::RgnByIDMap::iterator rgnItr = rgnManager->GetPersonalDupRgnMap().begin();
				for(; rgnItr != rgnManager->GetPersonalDupRgnMap().end(); rgnItr++)
				{
					rgn = rgnItr->second;
					// 如果该副本中有玩家则返回
					if( rgn && (rgn->GetPlayerAmout() != 0) )
						continue;
					
					CMessage msg(MSG_S2W_RGN_DELETE);
					msg.Add(rgn->GetOwnerGUID());   // owner id
					msg.Add(rgn->GetExID());		// Region的GUID
					msg.Add((long)rgnType); // 模板Rgn的Type
					msg.Add((long)-1);/*挂起的脚本ID*/
					// 异步操作返回值变量名
					msg.Add((char*)"");
					// 异步操作返回值变量名
					msg.Add((char*)"");
					msg.Send();
				}
			}
			break;
		case RGN_PERSONAL_HOUSE:
			{
				CRgnManager::RgnByIDMap::iterator rgnItr = rgnManager->GetPersonalHouseDupRgnMap().begin();
				for(; rgnItr != rgnManager->GetPersonalHouseDupRgnMap().end(); rgnItr++)
				{
					rgn = rgnItr->second;
					// 如果该副本中有玩家则返回
					if( rgn && (rgn->GetPlayerAmout() != 0) )
						continue;

					CMessage msg(MSG_S2W_RGN_DELETE);
					msg.Add(rgn->GetOwnerGUID());   // owner id
					msg.Add(rgn->GetExID());		// Region的GUID
					msg.Add((long)rgnType); // 模板Rgn的Type
					msg.Add((long)-1);/*挂起的脚本ID*/
					// 异步操作返回值变量名
					msg.Add((char*)"");
					// 异步操作返回值变量名
					msg.Add((char*)"");
					msg.Send();
				}
			}
			break;
		case RGN_TEAM:
			{
				CRgnManager::RgnByIDMap::iterator rgnItr = rgnManager->GetTeamDupRgnMap().begin();
				for(; rgnItr != rgnManager->GetTeamDupRgnMap().end(); rgnItr++)
				{
					rgn = rgnItr->second;
					// 如果该副本中有玩家则返回
					if( rgn && (rgn->GetPlayerAmout() != 0) )
						continue;

					CMessage msg(MSG_S2W_RGN_DELETE);
					msg.Add(rgn->GetOwnerGUID());   // owner id
					msg.Add(rgn->GetExID());		// Region的GUID
					msg.Add((long)rgnType); // 模板Rgn的Type
					msg.Add((long)-1);/*挂起的脚本ID*/
					// 异步操作返回值变量名
					msg.Add((char*)"");
					// 异步操作返回值变量名
					msg.Add((char*)"");
					msg.Send();
				}
			}
			break;
		}

	}
	return 1.0f;
}
// 改变当前场景的某个据点状态
double CScript::SetRgnStrongPointState (const char* cmd, char* retStr)
{
	if(!m_pRegion) return -1.0f;


	char* szStrongPointName = GetStringParam(cmd, 0);
	if(szStrongPointName)
	{
		double llState = GetIntParam(cmd, 1);
		if(llState != ERROR_CODE)
		{
			CServerRegion* pRgn = dynamic_cast<CServerRegion*>(m_pRegion);
			if(pRgn)
			{
				pRgn->SetStrongPointState(szStrongPointName, llState);
				// 发送消息通知该场景所有玩家
				CMessage msg(MSG_S2C_RGN_STRONGPOINT_STATE_CHANGE);
				msg.Add((long)pRgn->GetID()); // region id
				msg.Add(pRgn->GetExID()); // region guid
				msg.Add(szStrongPointName); // strongpoint name
				msg.Add((long)llState);
				msg.SendToRegion(pRgn);
			}
		}
		M_FREE(szStrongPointName, sizeof(char)*MAX_VAR_LEN);
	}
	return -1.0f;
}
// 取得当前场景的某个据点状态
double CScript::GetRgnStrongPointState(const char* cmd, char* retStr)
{
	if(!m_pRegion) return -1.0f;

	long retFlag = -1;
	char* szStrongPointName = GetStringParam(cmd, 0);
	if(szStrongPointName)
	{
		CServerRegion* pRgn = dynamic_cast<CServerRegion*>(m_pRegion);
		if(pRgn)
		{
			retFlag = pRgn->GetStrongPointState(szStrongPointName);
		}
		M_FREE(szStrongPointName, sizeof(char)*MAX_VAR_LEN);
	}
	return retFlag;
}
// 取得指定地图指定阵营精确人数
double CScript::GetRegionCampPlayerNum(const char* cmd, char* retStr)
{
	if(!m_pRegion) return -1.0f;

	char* szRgnGuidName = GetStringParam(cmd, 0);
	if(szRgnGuidName)
	{
		long lCampType =  (long)GetIntParam(cmd, 1);
		if(lCampType != eCampUnion && lCampType != eCampEmpire) // 阵营值未在定义范围内
		{
			char* szRetName = GetStringParam(cmd, 2);
			if(szRetName)
			{
				SetScriptVarValue((CMoveShape*)GetSrcShape(), szRetName, 0, -1.0f);
				M_FREE(szRetName, sizeof(char)*MAX_VAR_LEN);
			}
			M_FREE(szRgnGuidName, sizeof(char)*MAX_VAR_LEN);
			return -1.0f;
		}

		char* szRetName = GetStringParam(cmd, 2);
		if(!szRetName)
		{
			M_FREE(szRgnGuidName, sizeof(char)*MAX_VAR_LEN);
			return -1.0f;
		}

		CGUID rgnGUID;
		CServerRegion* pRgn = NULL;
		if(szRgnGuidName[0] == '\0') // 是空字符串，表示当前场景
		{
			pRgn = dynamic_cast<CServerRegion*>(m_pRegion);
		}
		else
		{
			rgnGUID = GetScriptGuidValue((CMoveShape*)GetSrcShape(), szRgnGuidName);
			pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
		}

		if(pRgn) // 是本地场景
		{
			long lCampId = pRgn->GetCampPlayerNum((eCampType)lCampType);
			SetScriptVarValue((CMoveShape*)GetSrcShape(), szRetName, 0, lCampId);
		}
		else // 是其它服务器场景
		{
			// 挂起该脚本
			SetIsHang(true);
			SetHangFunc(SR_FUNC_GETRGNCAMPPLAYERNUM);

			// 发送查询消息
			CMessage msg(MSG_S2W_RGN_CAMP_PLAYER_NUM_QUEST);
			msg.Add((long)GetGame()->GetIDIndex()); // gsid
			msg.Add(rgnGUID);// 场景GUID
			msg.Add((long)lCampType);// 指定阵营类型
			msg.Add(GetID()); // 挂起脚本guid
			msg.Add(szRetName); //挂起脚本返回脚本变量名
			msg.Send();
		}


		M_FREE(szRetName, sizeof(char)*MAX_VAR_LEN);
		M_FREE(szRgnGuidName, sizeof(char)*MAX_VAR_LEN);
	}
	return 1.0f;
}