///
/// @file ScriptRgnFunc.cpp
/// @brief 参见 ScriptRgnFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../ScriptSys.h"
#include "../../Player.h"
#include "../../RgnManager.h"
#include "../../OrganizingSystem/GSTeam.h"
#include "../../../../../Public/Setup/GlobalSetup.h"
#include "../../OrganizingSystem/GameOrganizingCtrl.h"


int Script::GetRgnAttr(const char* attrName)
{
	if(attrName==NULL)
		return 0;

	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return 0;

	return region->GetAttribute((char*)attrName);
}

void Script::SetRgnAttr(const char* attrName, int attrValue)
{
	if(attrName==NULL)
		return;

	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return;

	region->SetAttribute((char*)attrName, attrValue);
}

int Script::GetCellType(int x, int y)
{
	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return -1;

	return region->GetSecurity(x, y);
}

int Script::GetMapInfo(int x, int y)
{
	CRegion* region = m_RunEnv.region;
	if(region==NULL)
		return -1;

	CRegion::tagCell* cell = region->GetCell(x, y);
	if(cell==NULL)
		return -1;

	/**## 如果是国战区，就返回2，它的优先级最高 */
	if(cell->lWar == 1)
		return 2;
	/**## 如果是安全区，返回3 */
	else if(region->GetSecurity(x, y) == CRegion::SECURTIY_SAFE )
		return 3;
	/**## 如果是竞技区，返回1 */
	else if(region->GetSecurity(x, y) == CRegion::SECURTIY_FIGHT)
		return 1;
	/**## 如果不是以上的，就默认为普通PK区，返回0 */
	else
		return 0;
}

void Script::SetBlock(const CGUID& regionGuid, int x, int y, int block)
{
	if(block < CRegion::BLOCK_NO || block > CRegion::BLOCK_SHAPE)
		return;

	CServerRegion* region = GetGame()->FindRegion(regionGuid);
	if(region==NULL)
		return;

	region->SetBlock(x, y, block);
}

bool Script::GetRegionRandomPos(const CGUID& regionGuid, long* x, long* y)
{
	CServerRegion* region=NULL;
	if(regionGuid==NULL_GUID)
		region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	else
		region = GetGame()->FindRegion(regionGuid);
	if(region==NULL)
		return false;

	region->GetRandomPos(*x, *y);
	return true;
}

bool Script::GetRandomPosInRange(long regionId, int x, int y, int width, int height, long* randX, long* randY)
{	
	CServerRegion* region = NULL;
	region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, regionId);

	if(region == NULL)
		region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL, regionId);

	if(region == NULL)
		region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_PERSONAL_HOUSE, regionId);

	if(region == NULL)
		region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_TEAM, regionId);
	
	if(region == NULL)
		region = dynamic_cast<CServerRegion*>(m_RunEnv.region);

	if(region == NULL)
		return false;

	return region->GetRandomPosInRange(*randX, *randY, x, y, width, height);
}

long Script::GetLineDir(int srcX, int srcY, int destX, int destY)
{
	return ::GetLineDir((long)srcX,(long) srcY,(long) destX,(long) destY);
}

void Script::GetNextPosByDir(int dir, int x, int y, int step, int* destX, int* destY)
{
	// pos table
	int post[8][2] = {0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1};
	*destX = x + post[(int)dir][0] * step;
	*destY = y + post[(int)dir][1] * step;
}

void Script::RunScriptForAllPlayerInRgn(const char* scriptFilePath)
{
	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region!=NULL)
		region->RunScriptAllPlayer((char*)scriptFilePath);
}

CGUID Script::GetNormalDupRgnID(long regionId)
{
	CServerRegion* region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, regionId);
	if(region==NULL)
		return NULL_GUID;
	else
		return region->GetExID();
}

bool Script::ChangeRgn(long regionId, long x, long y, long dir, long range, long carriageDistance)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	CServerRegion *region = GameManager::GetInstance()->GetRgnManager()->
		FindRgnByTemplateID(RGN_NORMAL,  regionId);
	if(region)
		return player->ChangeRegion(RGN_NORMAL, region->GetExID(), x, y, dir, regionId, 0, range, carriageDistance);

	return player->ChangeRegion(RGN_NORMAL, NULL_GUID, x, y, dir, regionId, 0, range, carriageDistance);
}

bool Script::ChangeDupRgn(const CGUID &regionId, long regionType, long x, long y)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return player->ChangeRegion((eRgnType)regionType, regionId, x, y, 0);
}

CGUID Script::GetCurRgnGUID()
{
	if(m_RunEnv.region==NULL)
		return NULL_GUID;
	else
		return m_RunEnv.region->GetExID();
}

int Script::GetDupRgnPlayerMaxNum(const CGUID& regionGuid)
{
	CServerRegion* region = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(regionGuid);
	if(region==NULL)
		return EXEC_ERROR;

	switch((eRgnType)region->GetRgnType())
	{
	case RGN_PERSONAL:
		return GlobalSetup::GetSetup()->lPDupRgnMaxPlayerNum;

	case RGN_PERSONAL_HOUSE:
		return GlobalSetup::GetSetup()->lPHDupRgnMaxPlayerNum;

	case RGN_TEAM:
		return GlobalSetup::GetSetup()->lTDupRgnMaxPlayerNum;

	default:
		return EXEC_ERROR;
	}
}

bool Script::RunDupRgnScript(const char* scriptFilename)
{
	RunEnv env;
	return GetInst(ScriptSys).RunScript(env, scriptFilename);
}

int Script::GetDupRgnMaxNum(int regionType)
{
	switch((eRgnType)regionType)
	{
	case RGN_PERSONAL:
		return GlobalSetup::GetSetup()->lPDupRgnMaxNum;

	case RGN_PERSONAL_HOUSE:
		return GlobalSetup::GetSetup()->lPHDupRgnMaxNum;

	case RGN_TEAM:
		return GlobalSetup::GetSetup()->lTDupRgnMaxNum;

	case RGN_NORMAL:
		return EXEC_ERROR;

	default:
		return EXEC_ERROR;
	}
}

void Script::SetDupRgnOperFlag(const CGUID& regionGuid, long operationFlag)
{
	CServerRegion* region = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(regionGuid);
	if(region==NULL)
		return;
	region->SetOperFlag(operationFlag>0 ? true:false);
}

bool Script::GetDupRgnOperFlag(const CGUID& regionGuid)
{
	CRgnManager::RgnByIDMapItr itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().find(regionGuid);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalDupRgnMap().end())
		return itr->second->GetOperFlag();

	itr = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(regionGuid);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
		return itr->second->GetOperFlag();

	itr = GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().find(regionGuid);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetTeamDupRgnMap().end())
		return itr->second->GetOperFlag();

	itr = GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().find(regionGuid);
	if(itr != GameManager::GetInstance()->GetRgnManager()->GetNormalRgnMap().end())
		return itr->second->GetOperFlag();

	return false;
}

CGUID Script::GetTeamMaterIDByPos(int pos)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL_GUID;

	GSTeam* team = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
	if(team==NULL)
		return NULL_GUID;

	list<CGUID> members;
	team->GetMemberList(members);
	int i=0;
	for(list<CGUID>::iterator it = members.begin(); it != members.end(); ++it)
		if(i++ == pos)
			return *it;
	return NULL_GUID;
}

void Script::EditDupRgnName(const CGUID& regionGuid, const char* regionName)
{
	if(regionName==NULL)
		return;

	CServerRegion* region = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(regionGuid);
	if(region==NULL)
		return;

	region->SetName(regionName);
	if(m_RunEnv.srcShape==NULL)
		return;

	// 发送消息给客户端
	CMessage msg(MSG_S2C_RGN_CHANGENAME);
	msg.Add((long)region->GetRgnType());
	msg.Add(region->GetExID());
	msg.Add(region->GetID());
	msg.Add(region->GetResourceID());
	msg.Add((char*)region->GetName());
	msg.SendToPlayer(m_RunEnv.srcShape->GetExID());
}

void Script::CreatePHGoods(const CGUID& ownerGuid, const char* name, int counts, 
						   int x, int y, int width, int height, int dir, 
						   const char* script, const char* talk)
{
	if(name==NULL || script==NULL || talk==NULL) 
		return;

	CPlayer* player = GetGame()->FindPlayer(ownerGuid);
	if(player==NULL)
		return;

	CGUID regionGuid = player->GetPersonalHouseRgnGUID();
	CRgnManager::RgnByIDMapItr it = GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().find(regionGuid);
	if(it != GameManager::GetInstance()->GetRgnManager()->GetPersonalHouseDupRgnMap().end())
	{
		CServerRegion* region = it->second;
		if(region) // 本地服务器
			GameManager::GetInstance()->GetRgnManager()->CreatePHGoods(regionGuid, name, counts, 
			x, y, width, height, dir, 
			script, talk);
	}
	else
	{
		// 发送消息到WS，由WS在其它GS上创建Goods
		//##发送消息给World Server，发送后不管
		CMessage msg(MSG_S2W_SERVER_CREATEPHGOODS);
		msg.Add((BYTE)0);
		CGUID guid;
		CGUID::CreateGUID(guid);	
		msg.Add(guid);
		msg.Add(ownerGuid);
		msg.Add(name);
		msg.Add((long)counts);
		msg.Add((long)width);
		msg.Add((long)height);			
		msg.Add((long)x);
		msg.Add((long)y);
		msg.Add ((long)-1);
		if(script)
		{
			msg.Add(BYTE(1));
			msg.Add(script);
		}
		else 
		{
			msg.Add(BYTE(0));
		}					
		msg.Send();
	}
}

void Script::DeletePHGoods(const CGUID& ownerGuid, const CGUID& goodGuid)
{
	GameManager::GetInstance()->GetRgnManager()->DeletePHGoods(ownerGuid, goodGuid);
}

CGUID Script::GetCurPlayerPDupRgnID()
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL_GUID;
	return player->GetPersonalRgnGUID();
}

CGUID Script::GetCurPlayerPHDupRgnID()
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL_GUID;
	return player->GetPersonalHouseRgnGUID();
}

void Script::SetPlayerPDupRgnID(const CGUID& ownerGuid, const CGUID& regionGuid)
{
	CPlayer* player = GetGame()->FindPlayer(regionGuid);
	if(player==NULL)
		return;
	player->SetPersonalRgnGUID(regionGuid);
}

void Script::SetPlayerPHDupRgnID (const CGUID& ownerGuid, const CGUID& regionGuid)
{
	CPlayer* player = GetGame()->FindPlayer(regionGuid);
	if(player==NULL)
		return;
	player->SetPersonalHouseRgnGUID(regionGuid);
}

int Script::GetLocalGSID()
{
	return GetGame()->GetIDIndex();
}

bool Script::ChangeDupRgnByRgnID(const CGUID& playerGuid, int regionType, const CGUID& regionGuid, int x, int y)
{
	CPlayer* player = GetGame()->FindPlayer(playerGuid);
	if(player==NULL)
		return false;
	return player->ChangeRegion((eRgnType)regionType, regionGuid, x, y, 0);
}

CGUID Script::GetCallerGUID()
{
	if(m_RunEnv.srcShape)
		return NULL_GUID;
	return m_RunEnv.srcShape->GetExID();
}

void Script::CallScriptByGUID(const CGUID& shapeGuid, const char* scriptFilename)
{
	CMoveShape* shape = GetGame()->FindShape(TYPE_PLAYER, shapeGuid);
	if(shape==NULL)
	{
		CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
		if(region != NULL)
			shape = dynamic_cast<CMoveShape*>(region->FindChildObject(TYPE_MONSTER, shapeGuid));
	}

	if(shape != NULL) // 本地调用脚本
	{
		RunEnv env;
		env.region = dynamic_cast<CRegion*>(shape->GetFather());
		env.srcShape = shape;
		GetInst(ScriptSys).RunScript(env, scriptFilename);
	}
	else // 跨服调用脚本
	{
		CMessage msg(MSG_S2W_QUEST_PlayerRunScript);
		msg.Add(shapeGuid); // call guid
		msg.Add(scriptFilename);	 // called script name
		msg.SendToSocket(GetGame()->GetWSSocket());
	}
}

void Script::ClearCurGSDupRgn(int regionType)
{
	CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
	if(rgnManager==NULL)
		return;

	CServerRegion* region = NULL;
	switch(regionType)
	{
	case RGN_PERSONAL:
		for(CRgnManager::RgnByIDMap::iterator it = rgnManager->GetPersonalDupRgnMap().begin(); 
			it != rgnManager->GetPersonalDupRgnMap().end(); ++it)
		{
			region = it->second;
			// 如果该副本中有玩家则返回
			if(region && (region->GetPlayerAmout() != 0))
				continue;

			CMessage msg(MSG_S2W_RGN_DELETE);
			msg.Add(region->GetOwnerGUID());   // owner id
			msg.Add(region->GetExID());		// Region的GUID
			msg.Add((long)regionType); // 模板Rgn的Type
			msg.Add((long)-1);/*挂起的脚本ID*/
			// 异步操作返回值变量名
			msg.Add((char*)"");
			// 异步操作返回值变量名
			msg.Add((char*)"");
			msg.Send();
		}
		break;

	case RGN_PERSONAL_HOUSE:
		for(CRgnManager::RgnByIDMap::iterator it = rgnManager->GetPersonalHouseDupRgnMap().begin(); 
			it != rgnManager->GetPersonalHouseDupRgnMap().end(); ++it)
		{
			region = it->second;
			// 如果该副本中有玩家则返回
			if(region && (region->GetPlayerAmout() != 0))
				continue;

			CMessage msg(MSG_S2W_RGN_DELETE);
			msg.Add(region->GetOwnerGUID());   // owner id
			msg.Add(region->GetExID());		// Region的GUID
			msg.Add((long)regionType); // 模板Rgn的Type
			msg.Add((long)-1);/*挂起的脚本ID*/
			// 异步操作返回值变量名
			msg.Add((char*)"");
			// 异步操作返回值变量名
			msg.Add((char*)"");
			msg.Send();
		}
		break;

	case RGN_TEAM:
		for(CRgnManager::RgnByIDMap::iterator it = rgnManager->GetTeamDupRgnMap().begin(); 
			it != rgnManager->GetTeamDupRgnMap().end(); ++it)
		{
			region = it->second;
			// 如果该副本中有玩家则返回
			if(region && (region->GetPlayerAmout() != 0))
				continue;

			CMessage msg(MSG_S2W_RGN_DELETE);
			msg.Add(region->GetOwnerGUID());   // owner id
			msg.Add(region->GetExID());		// Region的GUID
			msg.Add((long)regionType); // 模板Rgn的Type
			msg.Add((long)-1);/*挂起的脚本ID*/
			// 异步操作返回值变量名
			msg.Add((char*)"");
			// 异步操作返回值变量名
			msg.Add((char*)"");
			msg.Send();
		}
		break;

	default:
		break;
	}
}

void Script::AddTeamRgnID(const CGUID& playerGuid, int tempRegionId, const CGUID& regionGuid)
{
	CPlayer* player = GetGame()->FindPlayer(playerGuid);
	if(player==NULL)
		return;

	GSTeam* team = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
	if(team==NULL)
		return;

	CPlayer* master = GetGame()->FindPlayer(team->GetLeader());
	if(master != NULL)
	{
		GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(regionGuid, tempRegionId, regionGuid, FALSE);
	}
	else
	{
		CMessage msg(MSG_S2W_TEAM_AddRgnID);
		msg.Add(team->GetLeader());
		msg.Add((long)tempRegionId);
		msg.Add(regionGuid);
		msg.Send(false);
	}
}

void Script::DelTeamRgnID(const CGUID& playerGuid, int tempRegionId)
{
	CPlayer* player = GetGame()->FindPlayer(playerGuid);
	if(player==NULL)
		return;

	GSTeam* team = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
	if(team==NULL)
		return;

	CPlayer* master = GetGame()->FindPlayer(team->GetLeader());
	if(NULL != master)
	{
		GameManager::GetInstance()->GetRgnManager()->DelTeamRgnGUID(playerGuid, tempRegionId);
	}
	else
	{
		CMessage msg(MSG_S2W_TEAM_DelRgnID);
		msg.Add(team->GetLeader());
		msg.Add((long)tempRegionId);
		msg.Send(false);
	}
}

void Script::ClearTeamRgnID(const CGUID& playerGuid)
{
	CPlayer* player = GetGame()->FindPlayer(playerGuid);
	if(player==NULL)
		return;


	GSTeam* team = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
	if(team==NULL)
		return;

	CPlayer* master = GetGame()->FindPlayer(team->GetLeader());
	if(NULL != master)
	{
		GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(playerGuid);
	}
	else
	{
		CMessage msg(MSG_S2W_TEAM_ClearRgnID);
		msg.Add(team->GetLeader());
		msg.Send(false);
	}
}

void Script::SetRgnStrongPointState(const char* footholdName, int footholdState)
{
	if(footholdName==NULL)
		return;

	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return;

	region->SetStrongPointState(footholdName, footholdState);
	// 发送消息通知该场景所有玩家
	CMessage msg(MSG_S2C_RGN_STRONGPOINT_STATE_CHANGE);
	msg.Add((long)region->GetID()); // region id
	msg.Add(region->GetExID()); // region guid
	msg.Add(footholdName); // strongpoint name
	msg.Add((long)footholdState);
	msg.SendToRegion(region);
}

int Script::GetRgnStrongPointState(const char* footholdName)
{
	if(footholdName==NULL)
		return EXEC_ERROR;

	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return EXEC_ERROR;

	return region->GetStrongPointState(footholdName);
}
