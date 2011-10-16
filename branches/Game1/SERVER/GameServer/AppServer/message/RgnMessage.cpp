#include "stdafx.h"
#include "..\nets\netserver\message.h"
#include "..\nets\Servers.h"
#include "..\player.h"
#include "..\serverRegion.h"
#include "..\..\gameserver\game.h"
#include "..\..\gameserver\GameManager.h"
#include "../setup/GlobeSetup.h"
#include "../setup/QuestSystem.h"
#include "..\Session\CSessionFactory.h"
#include <MMSystem.h>
#include "SequenceString.h"
#include "../session/CPlug.h"
#include "..\public\dupliregionsetup.h"
#include "../setup/ContributeSetup.h"

#include "../RgnManager.h"
#include "../script/VariableList.h"
#include "../script/Script.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"


// 响应RGN消息
void OnRgnMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_W2S_RGN_QUESTGSUPDATEPLAYERRGNLIST:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID playerID;
			pMsg->GetGUID(playerID);
			long gsid = pMsg->GetLong();
			CMessage msg(MSG_S2W_RGN_ANSWERGSUPDATEPLAYERRGNLIST);
			msg.Add(scriptID);
			msg.Add(playerID);
			msg.Add((long)gsid);
			CPlayer* player = GetGame()->FindPlayer(playerID);
			if(player)
			{
				msg.Add((BYTE)RET_TEAMRGN_LOCAL_OK);
				long num = pMsg->GetLong();
				GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(playerID);
				for(int i=0; i<num; i++)
				{
					long templateID = pMsg->GetLong();
					CGUID rgnID;
					pMsg->GetGUID(rgnID);
					BYTE idFlag = pMsg->GetByte();
					if(idFlag == 1) // 该副本存在
					{
						GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(playerID, templateID, rgnID, TRUE);
					}
					else
					{
						GameManager::GetInstance()->GetRgnManager()->DelTeamRgnGUID(playerID, templateID);
					}
				}
			}
			else
			{
				msg.Add((BYTE)RET_TEAMRGN_LOCAL_ERROR);
			}
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_QUESTGSQUESTPLAYERRGNLIST:// 查询GS玩家副本list
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID playerID;
			pMsg->GetGUID(playerID);

			long gsid = pMsg->GetLong();

			CMessage msg(MSG_S2W_RGN_ANSWERWSQUESTPLAYERRGNLIST);
			msg.Add(scriptID);
			msg.Add(playerID);
			msg.Add((long)gsid);
			CPlayer* player = GetGame()->FindPlayer(playerID);
			if(player)
			{
				map<LONG, CGUID>* rgnIdMap = GameManager::GetInstance()->GetRgnManager()->GetTeamRgnGuidMap(playerID);
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
				msg.Add((long)0);
			}
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_ANSWERPLAYERRGNLIST:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID playerID;
			pMsg->GetGUID(playerID);
			CPlayer* player = GetGame()->FindPlayer(playerID);

			// 挂起的脚本迭代器
			listScript::iterator itrx;
			//取消挂起的脚本
			itrx = g_Scripts.find(scriptID);
			int synRet = -1;
			if(itrx != g_Scripts.end())
			{
				BYTE flag = pMsg->GetByte();
				if(flag == RET_TEAMRGN_LOCAL_OK)
				{
					if(player)
						GameManager::GetInstance()->GetRgnManager()->ClearTeamRgnGUID(playerID);

					// 清除玩家的RgnList
					long num = pMsg->GetLong();
					for(int i=0; i<num; i++)
					{
						long templateID = pMsg->GetLong();

						CGUID rgnID;
						pMsg->GetGUID(rgnID);
						BYTE flag = pMsg->GetByte();
						if(flag == 1) // 该副本存在
						{
							if(player)
							{
								GSTeam *pTeam = GetOrganiCtrl()->GetGameTeamByID(player->GetTeamID());
								if(NULL == pTeam)
									GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(playerID, templateID, rgnID, TRUE);
								else
									GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(pTeam->GetMasterID(), templateID, rgnID, TRUE);
							}
						}
						else
						{
							if(player)
								GameManager::GetInstance()->GetRgnManager()->DelTeamRgnGUID(playerID, templateID);
						}
					}
				}
				ContinueRunScript(scriptID, NULL);
			}
		}
		break;
	case MSG_W2S_RGN_QUESTVIEWGSPLAYERNUM:
		{
			CGUID lRgnID;
			pMsg->GetGUID(lRgnID);
			CGUID ownerID;
			pMsg->GetGUID(ownerID);
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			char szRet[128];
			pMsg->GetStr(szRet, 128);
			long gsid = pMsg->GetLong();
			
			CMessage msg(MSG_S2W_RGN_ANSWERVIEWGSPLAYERNUM);
			msg.Add(lRgnID);
			CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(lRgnID);
			if(pRgn)
			{
				msg.Add((long)pRgn->GetPlayerAmout()); // 查找成功
			}
			else
			{
				msg.Add((long)-1); // 查找失败
			}
			msg.Add(ownerID);
			msg.Add(scriptID);
			msg.Add(szRet);
			msg.Add((long)gsid); // 需要回应的GSID
			msg.SendToSocket(GetGame()->GetWSSocket());
		}
		break;
	case MSG_W2S_RGN_VIEWGSPLAYERNUM:
		{
			long playerNum = pMsg->GetLong();
			CGUID ownerID;
			pMsg->GetGUID(ownerID);
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			char szRet[128];
			pMsg->GetStr(szRet, 128);
		
			// 挂起的脚本迭代器
			listScript::iterator itrx;
			//取消挂起的脚本
			itrx = g_Scripts.find(scriptID);
			int synRet = -1;
			if(itrx != g_Scripts.end())
			{
				itrx->second->SetScriptVarValue((CMoveShape*)itrx->second->GetSrcShape(), szRet, 0, playerNum);
				ContinueRunScript(scriptID, NULL);
			}
		}
		break;
	case MSG_W2S_RGN_CREATE:
		{
			GameManager::GetInstance()->GetRgnManager()->OnCreateRgnMsg(pMsg);
		}
		break;
	case MSG_W2S_RGN_DELETE:
		{
			GameManager::GetInstance()->GetRgnManager()->OnDeleteRgnMsg(pMsg);
		}
		break;
	case MSG_W2S_RGN_CREATE_ANSWER: // 收到GS1的回应GS0的消息
		{
			long srcGSID = pMsg->GetLong(); // srcGSID
			CGUID ownerID;
			pMsg->GetGUID(ownerID);
			eRgnType rgnType = (eRgnType)pMsg->GetLong(); // Rgn Type
			long templateRgnID = pMsg->GetLong(); // Rgn TemplateID
			long resourceID = pMsg->GetLong(); // Rgn ResourceID
			CGUID rgnID;// Rgn GUID
			pMsg->GetGUID(rgnID);

			// 取消挂起的脚本
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			
			char szRetVarName[128];// 异步操作返回值变量名
			pMsg->GetStr(szRetVarName, 128);
			char szRetRgnIDName[128];// 异步操作返回值变量名
			pMsg->GetStr(szRetRgnIDName, 128);
			long flag = pMsg->GetLong(); // flag

			BYTE isDupRgnDataFlag = pMsg->GetByte();
			if(isDupRgnDataFlag == 0)
			{
				long lBufSize = pMsg->GetLong();
				if(lBufSize > 0)
				{
					BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*lBufSize);
					pMsg->GetEx(buf, lBufSize);
					// 将该内存转移到map管理
					if(!GameManager::GetInstance()->GetRgnManager()->AddDupRgnData(templateRgnID, buf, lBufSize))
						M_FREE(buf, sizeof(BYTE)*lBufSize);
					buf = NULL;
					lBufSize = 0;
				}
			}
			CPlayer* pPlayer = GetGame()->FindPlayer(ownerID);
			if(pPlayer)
			{	
				pPlayer->SetRegionGUID(rgnID);
				switch(rgnType)
				{
				case RGN_NORMAL:
					break;
				case RGN_PERSONAL:
					pPlayer->SetPersonalRgnGUID(rgnID);
					pPlayer->SetCurPRgnNum(pPlayer->GetCurPRgnNum()+1);
					break;
				case RGN_PERSONAL_HOUSE:
					pPlayer->SetPersonalHouseRgnGUID(rgnID);
					break;
				case RGN_TEAM:
					GameManager::GetInstance()->GetRgnManager()->AddTeamRgnGUID(ownerID, templateRgnID, rgnID, TRUE);
					pPlayer->SetCurTRgnNum(pPlayer->GetCurTRgnNum()+1);
					break;
				}
			}

			// 挂起的脚本迭代器
			listScript::iterator itrx;

			//取消挂起的脚本
			itrx = g_Scripts.find(scriptID);
			
			int synRet = -1;
			if(itrx != g_Scripts.end())
			{
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
				{
					stackVarList->Release();
					stackVarList->AddGuid(szRetRgnIDName, rgnID);
					stackVarList->AddVar(szRetVarName, (double)flag);
				}
				ContinueRunScript(scriptID, stackVarList);
			}
		}
		break;

	case MSG_W2S_RGN_VIEWRGNVAR:
		{
			CGUID playerID;
			pMsg->GetGUID(playerID);

			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
	
			char varName[128];
			pMsg->GetStr(varName, 128);

			// 返回脚本变量名字
			char retVarName[128];
		
			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
				stackVarList->Release();

			// 脚本ID
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			if(varName[0] == '#') // 字符串
			{
				char value[1024];
				pMsg->GetStr(value, 1024);
				pMsg->GetStr(retVarName, 128);
				if(stackVarList)
					stackVarList->AddVar(retVarName, value);
			}
			else if(varName[0] == '$') // 整数
			{
				double value = 0;
				value = pMsg->GetLONG64();		
				if(stackVarList)
					stackVarList->AddVar(retVarName, value);
			}
			else if(varName[0] == '@')
			{
				CGUID tGUID;
				pMsg->GetGUID(tGUID);
				if(stackVarList)
					stackVarList->AddGuid(retVarName, tGUID);
			}
			// 取消脚本挂起
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_VAR_ADD:
		{
			//wq:2008-7-9,需要修改
			long asnyFlag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			ContinueRunScript(scriptID, NULL);
		}
		break;
	case MSG_W2S_RGN_VAR_ADD_QUEST:
		{
			BYTE scrGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			
			CServerRegion* rgn = NULL;
			CMessage msg(MSG_S2W_RGN_VAR_ADD_ANSWER);
			CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
			if(rgnManager)
			{
				rgn = rgnManager->FindRgnByRgnGUID(guid);
				if(rgn)
				{
					char varName[128];
					pMsg->GetStr(varName, 128);
					if(varName[0] == '#') // 字符串
					{
						long size = pMsg->GetLong();
						BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*(size+1));
						pMsg->GetEx(buf, size);
						buf[size] = '\0';
						rgn->GetVariableList()->AddVar(varName, (char*)buf);
						M_FREE(buf, sizeof(BYTE)*(size+1));
					}
					else if(varName[0] == '$') // 整数
					{
						long size = pMsg->GetLong();
						BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*(size+1));
						pMsg->GetEx(buf, size);
						buf[size] = '\0';
						rgn->GetVariableList()->AddVar(varName, *(double*)buf);
						M_FREE(buf, sizeof(BYTE)*(size+1));
					}
					else if(varName[0] == '@')
					{
						CGUID guid;
						pMsg->GetGUID(guid);
						rgn->GetVariableList()->AddGuid(varName, guid);
					}

					msg.Add((long)1);
					msg.Add(scriptID);
					msg.Add(guid);
					msg.Add((BYTE)scrGSID);
					msg.Send();
					return;
				}
			}

			msg.Add((long)-1); // 失败标志
			msg.Add(scriptID);
			msg.Add(guid);
			msg.Add((BYTE)scrGSID);
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_VAR_DEL:
		{
			//wq:2008-7-9,需要修改
			long asnyFlag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			ContinueRunScript(scriptID, NULL);
		}
		break;
	case MSG_W2S_RGN_VAR_DEL_QUEST:
		{
			BYTE scrGSID= pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CServerRegion* rgn = NULL;
			CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
			CMessage msg(MSG_S2W_RGN_VAR_DEL_ANSWER);
			if(rgnManager)
			{
				rgn = rgnManager->FindRgnByRgnGUID(guid);
				if(rgn)
				{
					char varName[128];
					pMsg->GetStr(varName, 128);
					if(varName[0] == '#') // 字符串
					{
						rgn->GetVariableList()->RemoveVar(varName);
					}
					else if(varName[0] == '$') // 整数
					{
						rgn->GetVariableList()->RemoveVar(varName);
					}
					else if(varName[0] == '@')
					{
						rgn->GetVariableList()->RemoveGuid(varName);
					}

					msg.Add((long)1);
					msg.Add(scriptID);
					msg.Add(guid);
					msg.Add((BYTE)scrGSID);
					msg.Send();
					return;
				}
			}
			msg.Add((long)-1);
			msg.Add(scriptID);
			msg.Add(guid);
			msg.Add((BYTE)scrGSID);
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_VAR_SET:
		{
			//wq:2008-7-9,需要修改
			long asnyFlag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			ContinueRunScript(scriptID, NULL);
		}
		break;
	case MSG_W2S_RGN_VAR_SET_QUEST:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CServerRegion* rgn = NULL;
			CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
			CMessage msg(MSG_S2W_RGN_VAR_SET_ANSWER);
			if(rgnManager)
			{
				rgn = rgnManager->FindRgnByRgnGUID(guid);
				if(rgn)
				{
					char varName[128];
					pMsg->GetStr(varName, 128);
					if(varName[0] == '#') // 字符串
					{
						long size = pMsg->GetLong();
						BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*(size+1));
						pMsg->GetEx(buf, size);
						buf[size] = '\0';
						rgn->GetVariableList()->AddVar(varName, (char*)buf);
						M_FREE(buf, sizeof(BYTE)*(size+1));
					}
					else if(varName[0] == '$') // 整数
					{
						long size = pMsg->GetLong();
						BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*(size+1));
						pMsg->GetEx(buf, size);
						buf[size] = '\0';
						rgn->GetVariableList()->AddVar(varName, *(double*)buf);
						M_FREE(buf, sizeof(BYTE)*(size+1));
					}
					else if(varName[0] == '@')
					{
						CGUID tGUID;
						pMsg->GetGUID(tGUID);
						rgn->GetVariableList()->AddGuid(varName, tGUID);
					}

					msg.Add((long)1); // 成功标志
					msg.Add(scriptID);
					msg.Add(guid);
					msg.Add((BYTE)srcGSID);
					msg.Send();
					return;
				}
			}

			msg.Add((long)-1); // 成功标志
			msg.Add(scriptID);
			msg.Add(guid);
			msg.Add((BYTE)srcGSID);
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_VAR_GET:
		{
			// 是否异步操作
			long flag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			BYTE srcGSID = pMsg->GetByte();
			long gsid = 0;
			CGUID guid;
			pMsg->GetGUID(guid);

			CVariableList* pStackVarList = CScript::GetStackVarListInst();
			if(pStackVarList)
			{
				pStackVarList->Release();
			}
		
			char varName[128];
			pMsg->GetStr(varName, 128);
			if(varName[0] == '#') // 字符串
			{
				char str[1024];
				pMsg->GetStr(str, sizeof(str));
				char retVarName[1024];
				pMsg->GetStr(retVarName, sizeof(retVarName));
				pStackVarList->AddVar(retVarName, str);
			}
			else if(varName[0] == '$') // 整数
			{
				long size = pMsg->GetLong();
				BYTE* buf = (BYTE*)M_ALLOC(sizeof(BYTE)*size);
				pMsg->GetEx(buf, size);
				char retVarName[1024];
				pMsg->GetStr(retVarName, sizeof(retVarName));
				pStackVarList->AddVar(retVarName, *(double*)buf);
				M_FREE(buf, sizeof(BYTE)*size);
			}
			else if(varName[0] == '@')
			{
				CGUID guid;
				pMsg->GetGUID(guid);
				char retVarName[1024];
				pMsg->GetStr(retVarName, sizeof(retVarName));
				pStackVarList->AddGuid(retVarName, guid);
			}

			ContinueRunScript(scriptID, pStackVarList);
		}
		break;
	case MSG_W2S_RGN_VAR_GET_QUEST:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			eRgnType rType = (eRgnType)pMsg->GetLong();

			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char rgnVarName[128];
			pMsg->GetStr(rgnVarName, sizeof(rgnVarName));
			char retVarName[128];
			pMsg->GetStr(retVarName, sizeof(retVarName));
			
			CServerRegion* rgn = NULL;
			CRgnManager* rgnManager = GameManager::GetInstance()->GetRgnManager();
			CMessage msg(MSG_S2W_RGN_VAR_GET_ANSWER);
			if(rgnManager)
			{
				rgn = rgnManager->FindRgnByRgnGUID(rgnGUID);
				if(rgn)
				{
					msg.Add((long)1); // 成功标志
					msg.Add(scriptID);
					msg.Add((BYTE)srcGSID);
					msg.Add(rgnGUID);

					if(rgnVarName)
						msg.Add(rgnVarName);
					else
						msg.Add("");			
			
					if(rgnVarName[0] == '#') // 字符串
					{
						const char* rgnVarValue = rgn->GetVariableList()->GetVarValue(rgnVarName);
						if(rgnVarValue)
							msg.Add(rgnVarValue);
						else
							msg.Add("");
						if(retVarName)
							msg.Add(retVarName);
						else
							msg.Add("");
					}
					else if(rgnVarName[0] == '$') // 整数
					{
						double ret = rgn->GetVariableList()->GetVarValue(rgnVarName, 0);
						msg.Add((long)sizeof(double));
						msg.AddEx((void*)&ret, sizeof(double));
						if(retVarName)
							msg.Add(retVarName);
						else
							msg.Add("");
					}
					else if(rgnVarName[0] == '@')
					{
						CGUID ret = rgn->GetVariableList()->GetGuidValue(rgnVarName);
						msg.Add(ret);
						if(retVarName)
							msg.Add(retVarName);
						else
							msg.Add("");
					}
					msg.Send();
					return;
				}
				msg.Add((long)-1); // 成功标志
				msg.Add(scriptID);
				msg.Add((BYTE)srcGSID);
				msg.Add(rgnGUID);
				msg.Send();
			}
		}
		break;
	case MSG_W2S_RGN_VIEWRGNNUM:
		{
			long gsNum = pMsg->GetLong(); //GS num

			for(int i=0; i<gsNum; i++)
			{
				long gsid = pMsg->GetLong(); //GSID
				long rgnType = pMsg->GetLong(); //GS rgn type
				long num = pMsg->GetLong(); //GS rgn num
			}
			CGUID viewerID;
			pMsg->GetGUID(viewerID);

			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			char szRetNumVar[64];
			pMsg->GetStr(szRetNumVar, 64);
			long dupRgnNum = pMsg->GetLong();
	
			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				// 设置返回值 rgn个数
				stackVarList->AddVar(szRetNumVar, dupRgnNum);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_VIEWRGNDATA:
		{
			CMessage msg(MSG_W2S_RGN_VIEWRGNDATA);
			long gsid = pMsg->GetLong(); // gsid
			long rgnType = pMsg->GetLong();
			long pos = pMsg->GetLong();
			BYTE flag = pMsg->GetByte(); // 查看标志;

			if(flag == 1)
			{
				long gsid = pMsg->GetLong(); // gsid
				long rgnID = pMsg->GetLong();// template id
				CGUID rgnGUID;
				pMsg->GetGUID(rgnGUID); // guid
				char name[128];
				pMsg->GetStr(name, 128);// name
				CGUID ownerID;
				pMsg->GetGUID(ownerID); // owner id
								
				char szRgnGUID[128];
				rgnGUID.tostring(szRgnGUID);
				char szRgnOnwerID[128];
				ownerID.tostring(szRgnOnwerID);

				CGUID viewerID;
				pMsg->GetGUID(viewerID);

				long dataType = pMsg->GetLong();
				char retVarName[128];
				pMsg->GetStr(retVarName, 128);
				CGUID scriptID;
				pMsg->GetGUID(scriptID);

				//wq:2008-7-9,需要修改
				CVariableList* stackVarList = CScript::GetStackVarListInst();
				if(stackVarList)
					stackVarList->Release();

				CPlayer* owner = GetGame()->FindPlayer(viewerID);
				//0:gsid 1:rgnType 2:pos 3:templateID 4:rgnGUID 5:name 6:OnwerID
				switch(dataType)
				{
				case 0:
					stackVarList->AddVar(retVarName, gsid);
					
					break;
				case 1:
					stackVarList->AddVar(retVarName, rgnType);
					break;
				case 2:
					stackVarList->AddVar(retVarName, pos);
					break;
				case 3:
					stackVarList->AddVar(retVarName, rgnID);
					break;
				case 4:
					stackVarList->AddGuid(retVarName, rgnGUID);
					break;
				case 5:
					stackVarList->AddVar(retVarName, name);
					break;
				case 6:
					stackVarList->AddGuid(retVarName, ownerID);
					break;
				}
				
				ContinueRunScript(scriptID, stackVarList);
			}
		}
		break;
	case MSG_W2S_RGN_VIEWRGNAVLID:
		{
			CGUID rgnID;
			CGUID senderID;
			pMsg->GetGUID(rgnID);
			pMsg->GetGUID(senderID);
			CPlayer* player = GetGame()->FindPlayer(senderID);
		
			long flag = -1;
			long gsid = -1;
			char retFlagStr[128];
			
			flag = pMsg->GetByte(); //找到场景标志
			pMsg->GetStr(retFlagStr, 128);
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			//wq:2008-7-9,需要修改
			
			if(flag == 1) // 找到
			{
				gsid = pMsg->GetByte();				
			}
			else
			{
				gsid = -1;
			}

			// 取消挂起
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddVar(retFlagStr, flag);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_VIEWPLAYERPID:
	case MSG_W2S_RGN_VIEWPLAYERPHID:
	case MSG_W2S_RGN_VIEWPLAYERTID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			LONG lTemplateID = pMsg->GetLong();
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID pGUID;
			pMsg->GetGUID(pGUID);
			CPlayer* player = GetGame()->FindPlayer(playerGUID);
			
			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddGuid(szRet, pGUID);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_GETTEMPLATEID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			long rgnType = pMsg->GetLong();
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long rgnTID = pMsg->GetLong();

			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddVar(szRet, rgnTID);
			}
			ContinueRunScript(scriptID, stackVarList);

		}
		break;
	case MSG_W2S_RGN_GETDUPTYPE:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long rgnType = pMsg->GetLong();

			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddVar(szRet, rgnType);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_GETDUPRGNONWER:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID ownerGUID;
			pMsg->GetGUID(ownerGUID);

			//wq:2008-7-9,需要修改
			CVariableList* stackVarList = CScript::GetStackVarListInst();
			if(stackVarList)
			{
				stackVarList->Release();
				stackVarList->AddGuid(szRet, ownerGUID);
			}
			ContinueRunScript(scriptID, stackVarList);
		}
		break;
	case MSG_W2S_RGN_UPDATE_PLAYER_TEAM_RGNGUID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			GameManager::GetInstance()->GetRgnManager()->RgnGuidDecode(playerGUID, setReadDB);

		}
		break;
	case MSG_W2S_RGN_CAMP_PLAYER_NUM_QUEST:
		{
			long srcGSID = pMsg->GetLong(); // 发送端GSID
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			long campType = pMsg->GetLong();
			CGUID scriptGUID;
			pMsg->GetGUID(scriptGUID);
			char retVarName[128] = {0};
			pMsg->GetStr(retVarName, sizeof(retVarName));

			CMessage msg(MSG_S2W_RGN_CAMP_PLAYER_NUM_ANSWER);
			msg.Add((long)srcGSID); // 发送端GSID
			msg.Add(rgnGUID);// 场景GUID
			msg.Add((long)campType);// 指定阵营类型
			msg.Add(scriptGUID); // 挂起脚本guid
			msg.Add(retVarName); //挂起脚本返回脚本变量名
			CServerRegion* pRgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByRgnGUID(rgnGUID);
			if(pRgn)
			{
				long campNum = pRgn->GetCampPlayerNum((eCampType)campType);
				msg.Add((long)campNum); // 查询数量
			}
			else
			{
				msg.Add((long)-1); //查询失败
			}
			msg.Send();
		}
		break;
	case MSG_W2S_RGN_CAMP_PLAYER_NUM_ANSWER:
		{
			long srcGSID = pMsg->GetLong(); // 发送端GSID
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			long campType = pMsg->GetLong();
			CGUID scriptGUID;
			pMsg->GetGUID(scriptGUID);
			char retVarName[128] = {0};
			pMsg->GetStr(retVarName, sizeof(retVarName));
			long retValue = pMsg->GetLong();
			CVariableList* pStackVarList = CScript::GetStackVarListInst();
			if(pStackVarList)
			{
				pStackVarList->Release();
				pStackVarList->AddVar(retVarName, retValue);
			}

			 // 整数
			ContinueRunScript(scriptGUID, pStackVarList);
		}
		break;
	}
}