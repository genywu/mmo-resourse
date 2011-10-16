#include "stdafx.h"
#include "RgnManager.h"
#include "../NETS/networld/Message.h"
#include "script/VariableList.h"
#include "GlobalRgnManager.h"
#include "DBentityManager.h"
#include "DBDupRgn.h"
#include "../public/strtype.h"
#include "../public/StrUnifyInput.h"

using namespace std;


CRgnManager::CRgnManager()
{
}
CRgnManager::~CRgnManager()
{
	RgnByIDMapItr rgnItr = m_mapNormalRgn.begin();
	for(; rgnItr != m_mapNormalRgn.end(); rgnItr++)
	{
		SAFE_DELETE(rgnItr->second);
	}
	m_mapNormalRgn.clear();

	rgnItr = m_mapPersonalDupRgn.begin();
	for(; rgnItr != m_mapPersonalDupRgn.end(); rgnItr++)
	{
		SAFE_DELETE(rgnItr->second);
	}
	m_mapPersonalDupRgn.clear();

	rgnItr = GetPersonalHouseDupRgn().begin();
	for(; rgnItr != GetPersonalHouseDupRgn().end(); rgnItr++)
	{
		SAFE_DELETE(rgnItr->second);
	}
	GetPersonalHouseDupRgn().clear();

	rgnItr = GetTeamDupRgn().begin();
	for(; rgnItr != GetTeamDupRgn().end(); rgnItr++)
	{
		SAFE_DELETE(rgnItr->second);
	}
	GetTeamDupRgn().clear();
}


//! 创建Rgn,向GS发送RGN_CREATE消息
CWorldRegion* CRgnManager::CreateRgn(long srcGSID/*发送创建请求的GSID*/, long templateRgnID, eRgnType rgnType, long GSID, const CGUID& ownerID, long scriptID, const CGUID& dbRgnGUID, const char* szRetVarName, const char* szRetRgnIDName/*异步返回值变量名*/)
{
	char logStr[2048];

	CWorldRegion* ret = NULL;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			CMessage msg(MSG_W2S_RGN_CREATE_PERSONAL);
			msg.Add(templateRgnID); // 模板Rgn的ID
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);		// 所有者的GUID
			
			vector<BYTE> pBA;
			pBA.clear();

			CGUID rgnID;
			// 取得模板Rgn
			CWorldRegion* worldRgn = NULL;
			map<long, CGlobalRgnManager::tagRegion>::iterator tRgnItr = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalRgnListMap()->find(templateRgnID);
			if(tRgnItr != GetGame()->GetGlobalRgnManager()->GetTemplatePersonalRgnListMap()->end())
			{
				
				if(dbRgnGUID == NULL_GUID)
					CGUID::CreateGUID(rgnID);
				else rgnID = dbRgnGUID;
						
				// 添加该副本到管理器
				RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
				if(rgnItr == m_mapPersonalDupRgn.end())
				{
					worldRgn = new CWorldRegion;

					tRgnItr->second.pRegion->AddToByteArray(&pBA);
					long pos = 0;
					worldRgn->DecordFromByteArray(&pBA[0], pos);

					// 初始化该Rgn
					worldRgn->SetExID(rgnID);
					m_mapPersonalDupRgn[rgnID] = worldRgn;
					ret = worldRgn;
					// 设置类型
					worldRgn->SetRgnType((long)RGN_PERSONAL);
					msg.Add(rgnID); // RgnID
					if(srcGSID == GSID)
					{
						msg.Add((long)1);	    // 已经创建标志

						char szGUID[128];
						rgnID.tostring(szGUID);
						// 日志字符串
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPRGN), szGUID, scriptID, srcGSID, GSID, 1);
						AddLogText(logStr);
					}
					else
					{
						msg.Add((long)2); // 已经创建标志, 需要WSID向srcGSID发送回应消息
						msg.Add(srcGSID); // SRC GSID

						char szGUID[128];
						rgnID.tostring(szGUID);
						// 日志字符串
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPRGN), szGUID, scriptID, srcGSID, GSID, 2);
						AddLogText(logStr);
					}

					CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
					if(owner)
					{
						owner->SetPersonalRgnGUID(rgnID);
						worldRgn->SetOwnerGUID(ownerID);
						owner->SetRegionID(worldRgn->GetID());
						owner->SetCurRgnType((eRgnType)worldRgn->GetType());
					}					
				}
				else
				{
					msg.Add(rgnID); // RgnID
					msg.Add((long)-6);	    // 已经创建标志
					SAFE_DELETE(worldRgn);

					char szGUID[128];
					rgnID.tostring(szGUID);
					sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPRGN),
						szGUID, scriptID, srcGSID, GSID, -6);
					AddLogText(logStr);
				}
			}
			else
			{
				msg.Add(rgnID); // RgnID
				msg.Add((long)-1);	    // 未找到该模板RGN
				char szGUID[128];
				rgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPRGN),
					szGUID, scriptID, srcGSID, GSID, -1);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			msg.SendGSID(GSID);
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			CMessage msg(MSG_W2S_RGN_CREATE_PERSONALHOUSE);
			msg.Add(templateRgnID); // 模板Rgn的ID
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);		// 所有者的GUID
			
			vector<BYTE> pBA;
			pBA.clear();

			CGUID rgnID;
			// 取得模板Rgn
			CWorldRegion* worldRgn = NULL;
			map<long, CGlobalRgnManager::tagRegion>::iterator tRgnItr = GetGame()->GetGlobalRgnManager()->GetTemplatePersonalHouseRgnListMap()->find(templateRgnID);
			if(tRgnItr != GetGame()->GetGlobalRgnManager()->GetTemplatePersonalHouseRgnListMap()->end())
			{
				
				if(dbRgnGUID == NULL_GUID)
					CGUID::CreateGUID(rgnID);
				else rgnID = dbRgnGUID;

				// 添加该副本到管理器
				RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
				if(rgnItr == m_mapPersonalHouseDupRgn.end())
				{
					worldRgn = new CWorldRegion;

					tRgnItr->second.pRegion->AddToByteArray(&pBA);
					long pos = 0;
					worldRgn->DecordFromByteArray(&pBA[0], pos);

					// 初始化该Rgn
					worldRgn->SetExID(rgnID);
					m_mapPersonalHouseDupRgn[rgnID] = worldRgn;
					ret = worldRgn;
					// 设置类型
					worldRgn->SetRgnType((long)RGN_PERSONAL_HOUSE);

					msg.Add(rgnID); // RgnID
					if(srcGSID == GSID)
					{
						msg.Add((long)1);	    // 已经创建标志

						char szGUID[128];
						rgnID.tostring(szGUID);
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPHRGN),
							szGUID, scriptID, srcGSID, GSID, 1);
						AddLogText(logStr);
					}
					else
					{
						msg.Add((long)2); // 已经创建标志, 需要WSID向srcGSID发送回应消息
						msg.Add(srcGSID); // SRC GSID

						char szGUID[128];
						rgnID.tostring(szGUID);
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPHRGN),
							szGUID, scriptID, srcGSID, GSID, 2);
						AddLogText(logStr);
					}

					// 添加DBDupRgn
					CDBEntityManager::DBDupRgnMap& dupRgnMap = GetGame()->GetDBEntityManager()->GetDBDupRgnMap();
					CDBEntityManager::DBDupRgnMapItr itr = dupRgnMap.find(worldRgn->GetExID());
					if(itr == dupRgnMap.end())
					{
						CDBDupRgn* tworldRgn = new CDBDupRgn(worldRgn->GetExID());
						dupRgnMap[worldRgn->GetExID()] = tworldRgn;
						tworldRgn->SetGSID(GSID);
						tworldRgn->SetName(worldRgn->GetName());
						tworldRgn->SetTemplateRgnID(worldRgn->GetID());
						tworldRgn->SetOwnerGUID(ownerID);
					}

					CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
					if(owner)
					{
						owner->SetPersonalHouseRgnGUID(rgnID);
						worldRgn->SetOwnerGUID(ownerID);
						owner->SetRegionID(worldRgn->GetID());
						owner->SetCurRgnType((eRgnType)worldRgn->GetType());
					}
				}
				else
				{
					msg.Add(rgnID); // RgnID
					msg.Add((long)-6);	    // 已经创建标志
					SAFE_DELETE(worldRgn);

					char szGUID[128];
					rgnID.tostring(szGUID);
					sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPHRGN),
						szGUID, scriptID, srcGSID, GSID, -6);
					AddLogText(logStr);
				}
			}
			else
			{
				msg.Add(rgnID); // RgnID
				msg.Add((long)-1);	    // 未找到该模板RGN

				char szGUID[128];
				rgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATEPHRGN),
					szGUID, scriptID, srcGSID, GSID, -1);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			msg.SendGSID(GSID);
		}
		break;
	case RGN_TEAM:
		{
			CMessage msg(MSG_W2S_RGN_CREATE_TEAM);
			msg.Add(templateRgnID); // 模板Rgn的ID
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);		// 所有者的GUID
			
			vector<BYTE> pBA;
			pBA.clear();

			CGUID rgnID;
			// 取得模板Rgn
			CWorldRegion* worldRgn = NULL;
			map<long, CGlobalRgnManager::tagRegion>::iterator tRgnItr = GetGame()->GetGlobalRgnManager()->GetTemplateTeamRgnListMap()->find(templateRgnID);
			if(tRgnItr != GetGame()->GetGlobalRgnManager()->GetTemplateTeamRgnListMap()->end())
			{
				
				if(dbRgnGUID == NULL_GUID)
					CGUID::CreateGUID(rgnID);
				else rgnID = dbRgnGUID;

				// 添加该副本到管理器
				RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
				if(rgnItr == m_mapTeamDupRgn.end())
				{
					worldRgn = new CWorldRegion;

					tRgnItr->second.pRegion->AddToByteArray(&pBA);
					long pos = 0;
					worldRgn->DecordFromByteArray(&pBA[0], pos);

					// 初始化该Rgn
					worldRgn->SetExID(rgnID);
					m_mapTeamDupRgn[rgnID] = worldRgn;
					ret = worldRgn;
					// 设置类型
					worldRgn->SetRgnType((long)RGN_TEAM);

					msg.Add(rgnID); // RgnID
					if(srcGSID == GSID)
					{
						msg.Add((long)1);	    // 已经创建标志

						char szGUID[128];
						rgnID.tostring(szGUID);
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATETRGN),
							szGUID, scriptID, srcGSID, GSID, 1);
						AddLogText(logStr);
					}
					else
					{
						msg.Add((long)2); // 已经创建标志, 需要WSID向srcGSID发送回应消息
						msg.Add(srcGSID); // SRC GSID

						char szGUID[128];
						rgnID.tostring(szGUID);
						sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATETRGN),
							szGUID, scriptID, srcGSID, GSID, 2);
						AddLogText(logStr);
					}
					
					CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
					if(owner)
					{
						owner->SetTeamRgnGUID(rgnID);
						worldRgn->SetOwnerGUID(ownerID);
						owner->SetRegionID(worldRgn->GetID());
						owner->SetCurRgnType((eRgnType)worldRgn->GetType());
					}
				}
				else
				{
					msg.Add(rgnID); // RgnID
					msg.Add((long)-6);	    // 已经创建标志
					SAFE_DELETE(worldRgn);

					char szGUID[128];
					rgnID.tostring(szGUID);
					sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATETRGN),
						szGUID, scriptID, srcGSID, GSID, -6);
					AddLogText(logStr);
				}
			}
			else
			{
				msg.Add((long)-1);	    // 未找到该模板RGN

				char szGUID[128];
				rgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_CREATETRGN),
					szGUID, scriptID, srcGSID, GSID, -1);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			msg.SendGSID(GSID);
		}
		break;
	case RGN_NORMAL:
		break;
	}

	return ret;
}

//! 删除Rgn,向WS发送RGN_DELETE消息
void CRgnManager::DeleteRgn(eRgnType rgnType, const CGUID&ownerID, const CGUID& RgnID, long GSID, long scriptID, const char* szRetVarName, const char* szRetRgnIDName/*异步返回值变量名*/)
{
	char logStr[2048];

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			CMessage msg(MSG_W2S_RGN_DELETE_PERSONAL);
		
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);
			msg.Add(RgnID); // Rgn ID

			// 删除该副本从管理器
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(RgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CGUID ownerID = rgnItr->second->GetOwnerGUID();

				CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
				if(owner)
				{
					// 是删除的当前场景
					if(RgnID == owner->GetPersonalRgnGUID())
						owner->SetPersonalRgnGUID(NULL_GUID);
				}

				SAFE_DELETE(rgnItr->second);
				m_mapPersonalDupRgn.erase(rgnItr);
				msg.Add((BYTE)1); // ok

				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETEPRGN),
					szGUID, scriptID, GSID, 1);
				AddLogText(logStr);
			}
			else
			{
				msg.Add((BYTE)0); // fail
				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETEPRGN),
					szGUID, scriptID, GSID, 0);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			
			msg.SendGSID(GSID);
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			CMessage msg(MSG_W2S_RGN_DELETE_PERSONALHOUSE);
		
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);
			msg.Add(RgnID); // Owner ID
			
			// 删除该副本从管理器
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(RgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CGUID ownerID = rgnItr->second->GetOwnerGUID();

				CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
				if(owner)
				{
					// 是删除的当前场景
					if(RgnID == owner->GetPersonalHouseRgnGUID())
						owner->SetPersonalHouseRgnGUID(NULL_GUID);
				}

				msg.Add((BYTE)1);// del ok

				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETEPHRGN),
					szGUID, scriptID, GSID, 1);
				AddLogText(logStr);

				// 删除数据库对象
				CDBEntityManager::DBDupRgnMapItr dbRgnItr = GetGame()->GetDBEntityManager()->GetDBDupRgnMap().find(rgnItr->first);
				if(dbRgnItr != GetGame()->GetDBEntityManager()->GetDBDupRgnMap().end())
				{
					SAFE_DELETE(dbRgnItr->second);
					GetGame()->GetDBEntityManager()->GetDBDupRgnMap().erase(dbRgnItr);
				}

				// 发送数据库删除DBDupRgn消息
				SendDelPHDupRgnMsgToDB(rgnItr->first);
				SAFE_DELETE(rgnItr->second);
				m_mapPersonalHouseDupRgn.erase(rgnItr);
			}
			else
			{
				msg.Add((BYTE)0); // fail

				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETEPHRGN),
					szGUID, scriptID, GSID, 0);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			msg.SendGSID(GSID);
		}
		break;
	case RGN_TEAM:
		{
			CMessage msg(MSG_W2S_RGN_DELETE_TEAM);
			
			msg.Add((long)rgnType); // 模板Rgn的Type
			msg.Add(ownerID);
			msg.Add(RgnID); // Owner ID

			// 删除该副本从管理器
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(RgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CGUID ownerID = rgnItr->second->GetOwnerGUID();

				CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
				if(owner)
				{
					// 是删除的当前场景
					if(RgnID == owner->GetTeamRgnGUID())
						owner->SetTeamRgnGUID(NULL_GUID);
				}

				SAFE_DELETE(rgnItr->second);
				m_mapTeamDupRgn.erase(rgnItr);
				msg.Add((BYTE)1);// del ok

				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETETRGN),
					szGUID, scriptID, GSID, 1);
				AddLogText(logStr);
			}
			else
			{
				msg.Add((BYTE)0); // fail
				char szGUID[128];
				RgnID.tostring(szGUID);
				sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETETRGN),
					szGUID, scriptID, GSID, 0);
				AddLogText(logStr);
			}

			msg.Add(scriptID);
			if(szRetVarName)
				msg.Add(szRetVarName);
			else
				msg.Add("");
			if(szRetRgnIDName)
				msg.Add(szRetRgnIDName);
			else
				msg.Add("");
			msg.SendGSID(GSID);
		}
		break;
	case RGN_NORMAL:
		break;
	}
}

//! 查找Region
CWorldRegion* CRgnManager::FindRgnByGUID(eRgnType rgnType, const CGUID& rgnID)
{
	CWorldRegion* ret = NULL;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
				ret = rgnItr->second;
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
				ret = rgnItr->second;
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
				ret = rgnItr->second;
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
				ret = rgnItr->second;
		}
		break;
	case RGN_PROXY:
		{
			/*		std::map<CGUID, CProxyServerRegion*>::iterator itr = m_mapProxyServerRgn.find(rgnID);
			if(itr != m_mapProxyServerRgn.end())
			SAFE_DELETE(itr->second);

			m_mapProxyServerRgn.erase(itr);*/
		}
	}

	return ret;
}

//! 添加变量, 向GS发送VAR_ADD消息
bool CRgnManager::AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* strValue)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, strValue);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, strValue);

					CMessage msg(MSG_W2S_RGN_VAR_ADD);
					msg.Add(rgnID); // RgnID
					msg.Add(strVarName); // var name
					msg.Add(strValue);   // var value
					msg.Send();

					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, strValue);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, strValue);
					ret = true;
				}
			}
		}
		break;
	}


	return ret;
}
bool CRgnManager::AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, value);
					CMessage msg(MSG_W2S_RGN_VAR_ADD);
					msg.Add(rgnID); // RgnID
					msg.Add(strVarName); // var name
					msg.Add(value);   // var value
					msg.Send();
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	}


	return ret;
}
bool CRgnManager::AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value, long arraySize/*数组大小*/)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, arraySize, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, arraySize, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, arraySize, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddVar(strVarName, arraySize, value);
					ret = true;
				}
			}
		}
		break;
	}


	return ret;
}
bool CRgnManager::AddVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const CGUID& value)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddGuid(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddGuid(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddGuid(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					vl->AddGuid(strVarName, value);
					ret = true;
				}
			}
		}
		break;
	}


	return ret;
}
//! 删除变量, 向WS发送VAR_DEL消息
bool CRgnManager::DelVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( false == vl->RemoveVar(strVarName) )
						if( false == vl->RemoveGuid(strVarName) )
							ret = false;

					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( false == vl->RemoveVar(strVarName) )
						if( false == vl->RemoveGuid(strVarName) )
							ret = false;

					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( false == vl->RemoveVar(strVarName) )
						if( false == vl->RemoveGuid(strVarName) )
							ret = false;

					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( false == vl->RemoveVar(strVarName) )
						if( false == vl->RemoveGuid(strVarName) )
							ret = false;

					ret = true;
				}
			}
		}
		break;
	}


	return ret;
}

//! 设置变量, 向WS发送VAR_SET消息
bool CRgnManager::SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* strValue)
{	
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, strValue) )
						ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, strValue) )
						ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, strValue) )
						ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, strValue) )
						ret = true;
				}
			}
		}
		break;
	}


	return ret;
}
bool CRgnManager::SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, value) )
						ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
bool CRgnManager::SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long value, long arrayPos/*数组位置*/)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, arrayPos, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, arrayPos, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, arrayPos, value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetVarValue(strVarName, arrayPos, value) )
						ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
bool CRgnManager::SetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const CGUID& value)
{
	bool ret = false;

	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetGuidValue(strVarName, &value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetGuidValue(strVarName, &value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetGuidValue(strVarName, &value) )
						ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					if( CVariableList::VAR_NOT_FOUND != vl->SetGuidValue(strVarName, &value) )
						ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
//! 取得变量, 向WS发送VAR_GET消息
bool CRgnManager::GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, const char* value)
{
	bool ret = false;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetVarValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetVarValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetVarValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetVarValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
bool CRgnManager::GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long& value)
{
	bool ret = false;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, 0);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, 0);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, 0);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, 0);
					ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
bool CRgnManager::GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, long& value, long arrayPos/*数组位置*/)
{
	bool ret = false;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, arrayPos);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, arrayPos);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, arrayPos);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = (long)vl->GetVarValue(strVarName, arrayPos);
					ret = true;
				}
			}
		}
		break;
	}

	return ret;
}
bool CRgnManager::GetVar(eRgnType rgnType, const CGUID& rgnID, const char* strVarName, CGUID& value)
{
	bool ret = false;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetGuidValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			RgnByIDMapItr rgnItr = m_mapPersonalHouseDupRgn.find(rgnID);
			if(rgnItr != m_mapPersonalHouseDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetGuidValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			RgnByIDMapItr rgnItr = m_mapTeamDupRgn.find(rgnID);
			if(rgnItr != m_mapTeamDupRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetGuidValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	case RGN_NORMAL:
		{
			RgnByIDMapItr rgnItr = m_mapNormalRgn.find(rgnID);
			if(rgnItr != m_mapNormalRgn.end())
			{
				CVariableList* vl = rgnItr->second->GetVariableList();
				if(vl)
				{
					value = vl->GetGuidValue(strVarName);
					ret = true;
				}
			}
		}
		break;
	}

	return ret;
}



// 发送数据库删除DBDupRgn消息
void CRgnManager::SendDelPHDupRgnMsgToDB(const CGUID& rgnGUID)
{
	CMessage msg(MSG_W2DB_OPER_DUPRGN);
	// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
	// DBOP Type:     1byte, 数据库操作类型（枚举值）
	// MSG ID:        4byte, 该消息的MSGID
	// Entity Num:	2byte, 该实体个数(X)
	msg.Add((BYTE)0);
	msg.Add((BYTE)DB_OPER_DELETE);
	msg.Add((long)-1);
	msg.Add((WORD)1);
	// GUID
	vector<BYTE> pBA;
	_AddToByteArray(&pBA, rgnGUID);
	msg.AddEx(&pBA[0], pBA.size());
	msg.SendToSocket(GetGame()->GetDBSocketID());
}