#include "StdAfx.h"
#include "../GlobalRgnManager.h"
#include "../Player.h"
#include "../WorldRegion.h"
#include "../Script/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应RGN消息
void OnRgnMessage(CMessage* pMsg)
{
	switch(pMsg->GetType())
	{
	case MSG_S2W_RGN_ANSWERGSUPDATEPLAYERRGNLIST:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID playerID;
			pMsg->GetGUID(playerID);
			long gsid = pMsg->GetLong();
			CMessage msg(MSG_W2S_RGN_ANSWERPLAYERRGNLIST);
			msg.Add(scriptID);
			msg.Add(playerID);
			BYTE flag = pMsg->GetByte();
			if(flag == RET_TEAMRGN_LOCAL_OK)
			{
				msg.Add((BYTE)RET_TEAMRGN_REMOTE_OK); // 远程更新成功
			}
			else
			{
				msg.Add((BYTE)RET_TEAMRGN_REMOTE_ERROR); // 远程更新失败
			}
			msg.SendToGS(gsid);
		}
		break;
	case MSG_S2W_RGN_ANSWERWSQUESTPLAYERRGNLIST:
		{		
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID PlayerID;
			pMsg->GetGUID(PlayerID);
			long gsid = pMsg->GetLong();
			long queryRgnNum = pMsg->GetLong();	
			
			CMessage msg(MSG_W2S_RGN_QUESTGSUPDATEPLAYERRGNLIST);
			msg.Add(scriptID);
			msg.Add(PlayerID);
			msg.Add(gsid);

			if(queryRgnNum != 0)
			{
				msg.Add((long)queryRgnNum);
				for(int i=0; i<queryRgnNum; i++)
				{
					long templateID = pMsg->GetLong();
					msg.Add((long)templateID);
					CGUID rgnID;
					pMsg->GetGUID(rgnID);
					msg.Add(rgnID);
					if(GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnID))
						msg.Add((BYTE)1); // 组队副本记录存在
					else
						msg.Add((BYTE)0); // 组队副本记录不存在
				}
			}
			else
			{
				msg.Add((long)0);
			}
			msg.SendToSocket(pMsg->GetSocketID());
		}
		break;
	case MSG_S2W_RGN_QUESTPLAYERRGNLIST:
		{
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID playerID;
			pMsg->GetGUID(playerID);
			BYTE flag = pMsg->GetByte();
			if(flag == 1) // 当前GS查询
			{
				long num = pMsg->GetLong();
				CMessage msg(MSG_W2S_RGN_ANSWERPLAYERRGNLIST);
				msg.Add(scriptID);
				msg.Add(playerID);
				msg.Add((BYTE)RET_TEAMRGN_LOCAL_OK);
				msg.Add((long)num);
				CGUID rgnID;
				for(int i=0; i<num; i++)
				{
					long templateID = pMsg->GetLong();
					msg.Add((long)templateID);
					pMsg->GetGUID(rgnID);
					msg.Add(rgnID);
					CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnID);
					if(rgn)
						msg.Add((BYTE)1); // 组队副本记录存在
					else
						msg.Add((BYTE)0); // 组队副本记录不存在
				}
				msg.SendToSocket(pMsg->GetSocketID());
			}
			else
			{
				CPlayer* player = GetGame().GetMapPlayer(playerID);
				if(player)
				{
					CMessage msg(MSG_W2S_RGN_QUESTGSQUESTPLAYERRGNLIST);
					msg.Add(scriptID);
					msg.Add(playerID);
					msg.Add((long)pMsg->GetGSID());
					msg.SendToSocket(player->GetGsSocket());
				}
				else
				{
					CMessage msg(MSG_W2S_RGN_ANSWERPLAYERRGNLIST);
					msg.Add((long)-1);
					msg.Add(playerID);
					msg.Add((BYTE)RET_TEAMRGN_LOCAL_ERROR);
					msg.SendToSocket(pMsg->GetSocketID());
				}
			}
		}
		break;
	case MSG_S2W_RGN_ANSWERVIEWGSPLAYERNUM:
		{
			CGUID rgnGuid;
			pMsg->GetGUID(rgnGuid);
			long num = pMsg->GetLong();
			CGUID ownerID;
			pMsg->GetGUID(ownerID);// 查看者GUID
			CGUID scriptID;
			pMsg->GetGUID(scriptID); // script id
			char szRet[128];
			pMsg->GetStr(szRet, 128);
			long gsid = pMsg->GetLong();

			CMessage msg(MSG_W2S_RGN_VIEWGSPLAYERNUM);
			msg.Add((long)num); // 人数
			msg.Add(ownerID);
			msg.Add(scriptID);
			msg.Add(szRet);
			msg.SendToGS(gsid); // 需要回应的GSID
		}
		break;
	case MSG_S2W_RGN_VIEWGSPLAYERNUM:
		{
			CGUID RgnID;
			pMsg->GetGUID(RgnID); // 场景TemplateID
			CGUID ownerID;
			pMsg->GetGUID(ownerID);// 查看者GUID
			CGUID scriptID;
			pMsg->GetGUID(scriptID); // script id
			char szRet[128];
			pMsg->GetStr(szRet, 128);
			CWorldRegion* pRgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(RgnID);
			if(pRgn)
			{
				CMessage msg(MSG_W2S_RGN_QUESTVIEWGSPLAYERNUM);
				msg.Add(RgnID);
				msg.Add(ownerID);
				msg.Add(scriptID);
				msg.Add(szRet);
				msg.Add((long)pMsg->GetGSID()); // 需要回应的GSID
				msg.SendToGS(pRgn->GetGsid());
			}
			else
			{
				CMessage msg(MSG_W2S_RGN_VIEWGSPLAYERNUM);
				msg.Add((long)-1); // WS未找到该TemplateID的场景
				msg.Add(ownerID);
				msg.Add(scriptID);
				msg.Add(szRet);
				msg.SendToSocket(pMsg->GetSocketID());
			}
		}
		break;

	case MSG_S2W_RGN_CREATE_QUEST: // 收到GS1的回应GS0的消息，转发给GS0
		{
			// 更新本地Player数据
			// srcGSID
			long gsid = pMsg->GetLong();

			// owner ID
			CGUID ownerID;
			pMsg->GetGUID(ownerID);
			CPlayer* pPlayer = GetGame().GetMapPlayer(ownerID);
			if(pPlayer)
			{
				// Rgn Type
				long rgnType = pMsg->GetLong(); 
				long templateRgnID = pMsg->GetLong(); // Rgn TemplateID
				long lResourceID = pMsg->GetLong(); // Rgn ResourceID
				// Rgn GUID
				CGUID RgnID;
				pMsg->GetGUID(RgnID);
				CGUID scriptID;
				pMsg->GetGUID(scriptID);

				char szVarStr[128] = {0};
				pMsg->GetStr(szVarStr, sizeof(szVarStr));
				char szRgnIDStr[128] = {0};
				pMsg->GetStr(szRgnIDStr, sizeof(szRgnIDStr));
				long Flag = pMsg->GetLong(); // Flag
				if(Flag >= 1)
				{
					GetGame().GetGlobalRgnManager()->AddPersonalRgnGUID(ownerID, RgnID);
				}
				else
				{
					if(Flag == C_RET_THESE_RGNS_FULL_ERROR) // GS服务器副本已经满载
					{
						// 删除该副本从管理器
						CGlobalRgnManager::MapRgnItr rgnItr = GetGame().GetGlobalRgnManager()->GetRgnMap().find(RgnID);
						if(rgnItr != GetGame().GetGlobalRgnManager()->GetRgnMap().end())
						{
							SAFE_DELETE(rgnItr->second);
							GetGame().GetGlobalRgnManager()->GetRgnMap().erase(rgnItr);
						}
					}
				}
			}
			
			if(gsid != pMsg->GetGSID())
			{
				pMsg->SetType(MSG_W2S_RGN_CREATE_ANSWER);
				pMsg->SendToGS(gsid);// src GSID
			}
		}
		break;
	case MSG_S2W_RGN_CREATE:
		{
	//		long GSID = pMsg->GetLong();
	
			long templateRgnID = pMsg->GetLong();// 模板Rgn的ID
			pMsg->Add(templateRgnID); 
			long Type = pMsg->GetLong(); // 模板Rgn的Type
			CGUID ownerID;		// 所有者的GUID
			pMsg->GetGUID(ownerID);

			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			char szRetVarName[128];
			pMsg->GetStr(szRetVarName, 128);

			char szRetRgnIDName[128];
			pMsg->GetStr(szRetRgnIDName, 128);
			//GetGame().GetGlobalRgnManager()->FindGSRgnByGUID(GSID, RGN_NORMAL, NULL_GUID);

			BYTE isDupRgnDataFlag = pMsg->GetByte(); // 该副本是否有缓冲数据
            if( !GetGame().GetGlobalRgnManager()->CreateGSRgn(templateRgnID, (eRgnType)Type, ownerID, scriptID, pMsg->GetGSID(), szRetVarName, szRetRgnIDName, isDupRgnDataFlag > 0 ? true : false ) )
			{
				// 错误处理
			}
		}
		break;


	case MSG_S2W_RGN_DELETE:
		{
	//		long GSID = pMsg->GetLong();

			CGUID ownerID;
			pMsg->GetGUID(ownerID);
			CGUID templateRgnID;
			pMsg->GetGUID(templateRgnID);// 模板Rgn的ID
			//pMsg->Add(templateRgnID); 
			long Type = pMsg->GetLong(); // 模板Rgn的Type
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			//GetGame().GetGlobalRgnManager()->FindGSRgnByGUID(GSID, RGN_NORMAL, NULL_GUID);
			char szRetVarName[128];
			pMsg->GetStr(szRetVarName, 128);

			char szRetRgnIDName[128];
			pMsg->GetStr(szRetRgnIDName, 128);
			if( !GetGame().GetGlobalRgnManager()->DeleteGSRgn((eRgnType)Type, ownerID, templateRgnID, scriptID, pMsg->GetGSID(), szRetVarName, szRetRgnIDName) )
			{
				// 错误处理
			}
		}
		break;
	
	case MSG_S2W_RGN_VIEWRGNVAR:
		{
//			long GSID = pMsg->GetLong();
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
		}
		break;
	case MSG_S2W_RGN_VAR_ADD:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long gsid = 0;
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
		
			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
			if(!rgn)
			{
				char szGuid[128];
				guid.tostring(szGuid);
//				char pszRgnInfo[1024];
// 				_snprintf(pszRgnInfo,1024,"MSG_S2W_RGN_VAR_ADD: Rgn[%s] is NULL!.",szGuid);
// 				PutStringToFile("Login_WS_Info",pszRgnInfo);

                Log4c::Trace(ROOT_MODULE,"MSG_S2W_RGN_VAR_ADD: Rgn[%s] is NULL!.",szGuid);
				CMessage msg(MSG_W2S_RGN_VAR_ADD);
				msg.Add((long)0);
				msg.Add(scriptID);
				msg.SendToSocket(pMsg->GetSocketID());
				return;
			}
						
			gsid = rgn->GetGsid();
			
			char varName[128];
			pMsg->GetStr(varName, 128);
			//if(varName[0] == '#') // 字符串
			//{
			//	long size = pMsg->GetLong();
			//	BYTE* buf = new BYTE[size+1];
			//	pMsg->GetEx(buf, size);
			//	buf[size] = '\0';
			//	rgn->GetVariableList()->AddVar(varName, (char*)buf);
			//	SAFE_DELETE_ARRAY(buf);
			//}
			//else if(varName[0] == '$') // 整数
			//{
			//	long size = pMsg->GetLong();
			//	BYTE* buf = new BYTE[size];
			//	pMsg->GetEx(buf, size);
			//	rgn->GetVariableList()->AddVar(varName, *(double*)buf);
			//	SAFE_DELETE_ARRAY(buf);
			//}
			//else if(varName[0] == '@')
			//{
			//	CGUID guid;
			//	pMsg->GetGUID(guid);
			//	rgn->GetVariableList()->AddGuid(varName, guid);
			//}
			long varType = VarList::VarType( varName );
			if( varType == VariableList::VAR_NUM )
			{
				double val;
				pMsg->GetEx( &val, sizeof( val ) );
				rgn->GetVariableList()->AddVar( varName, val );
			}
			else if( varType == VariableList::VAR_STRING )
			{
				char val[512];
				pMsg->GetStr( val, sizeof( val ) );
				rgn->GetVariableList()->AddVar( varName, tString( val ) );
			}
			else if( varType == VariableList::VAR_GUID )
			{
				CGUID val;
				pMsg->GetGUID( val );
				rgn->GetVariableList()->AddVar( varName, val );
			}

			// 转发到Rgn所在GS
			if(gsid != pMsg->GetGSID())
			{
				pMsg->SetType(MSG_W2S_RGN_VAR_ADD_QUEST);		
				pMsg->SendToGS(gsid);
			}
		}
		break;
	case MSG_S2W_RGN_VAR_ADD_ANSWER:
		{
			long flag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID guid;
			pMsg->GetGUID(guid);
			BYTE gsid = pMsg->GetByte();

			pMsg->SetType(MSG_W2S_RGN_VAR_ADD);
			pMsg->SendToGS(gsid);
		}
		break;
	case MSG_S2W_RGN_VAR_DEL:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long gsid = 0;
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
			if(!rgn)
			{
				char szGuid[128];
				guid.tostring(szGuid);
// 				char pszRgnInfo[1024];
// 				_snprintf(pszRgnInfo,1024,"MSG_S2W_RGN_VAR_DEL: Rgn[%s] is NULL!.",szGuid);
// 				PutStringToFile("Login_WS_Info",pszRgnInfo);

                Log4c::Trace(ROOT_MODULE,"MSG_S2W_RGN_VAR_DEL: Rgn[%s] is NULL!.",szGuid);

				CMessage msg(MSG_W2S_RGN_VAR_DEL);
				msg.Add((long)0);
				msg.Add(scriptID);
				msg.SendToSocket(pMsg->GetSocketID());

				return;
			}
			gsid = rgn->GetGsid();

			char varName[128];
			pMsg->GetStr(varName, 128);
			//if(varName[0] == '#') // 字符串
			//{
			//	rgn->GetVariableList()->RemoveVar(varName);
			//}
			//else if(varName[0] == '$') // 整数
			//{
			//	rgn->GetVariableList()->RemoveVar(varName);
			//}
			//else if(varName[0] == '@')
			//{
			//	rgn->GetVariableList()->RemoveGuid(varName);
			//}
			rgn->GetVariableList()->RemoveVar( varName );

			// 转发到Rgn所在GS
			if(gsid != pMsg->GetGSID())
			{
				pMsg->SetType(MSG_W2S_RGN_VAR_DEL_QUEST);		
				pMsg->SendToGS(gsid);
			}
		}
		break;
	case MSG_S2W_RGN_VAR_DEL_ANSWER:
		{
			long flag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID guid;
			pMsg->GetGUID(guid);
			BYTE gsid = pMsg->GetByte();
			pMsg->SetType(MSG_W2S_RGN_VAR_DEL);
			pMsg->SendToGS(gsid);
		}
		break;
	case MSG_S2W_RGN_VAR_SET:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long gsid = 0;
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
			if(!rgn)
			{
				char szGuid[128];
				guid.tostring(szGuid);
// 				char pszRgnInfo[1024];
// 				_snprintf(pszRgnInfo,1024,"MSG_S2W_RGN_VAR_SET: Rgn[%s] is NULL!.",szGuid);
// 				PutStringToFile("Login_WS_Info",pszRgnInfo);

                Log4c::Trace(ROOT_MODULE,"MSG_S2W_RGN_VAR_SET: Rgn[%s] is NULL!.",szGuid);

				CMessage msg(MSG_W2S_RGN_VAR_SET);
				msg.Add((long)0);
				msg.Add(scriptID);
				msg.SendToSocket(pMsg->GetSocketID());

				return;
			}
				
			gsid = rgn->GetGsid();

			char varName[128];
			pMsg->GetStr(varName, 128);
			//if(varName[0] == '#') // 字符串
			//{
			//	long size = pMsg->GetLong();
			//	BYTE* buf = new BYTE[size+1];
			//	pMsg->GetEx(buf, size);
			//	buf[size] = '\0';
			//	rgn->GetVariableList()->AddVar(varName, (char*)buf);
			//	SAFE_DELETE_ARRAY(buf);
			//}
			//else if(varName[0] == '$') // 整数
			//{
			//	long size = pMsg->GetLong();
			//	BYTE* buf = new BYTE[size];
			//	pMsg->GetEx(buf, size);
			//	rgn->GetVariableList()->AddVar(varName, *(double*)buf);
			//	SAFE_DELETE_ARRAY(buf);
			//}
			//else if(varName[0] == '@')
			//{
			//	CGUID guid;
			//	pMsg->GetGUID(guid);
			//	rgn->GetVariableList()->AddGuid(varName, guid);
			//}
			long varType = VarList::VarType( varName );
			if( varType == VariableList::VAR_NUM )
			{
				double val;
				pMsg->GetEx( &val, sizeof( val ) );
				rgn->GetVariableList()->SetVarValue( varName, val );
			}
			else if( varType == VariableList::VAR_STRING )
			{
				char val[512];
				pMsg->GetStr( val, sizeof( val ) );
				rgn->GetVariableList()->SetVarValue( varName, tString( val ) );
			}
			else if( varType == VariableList::VAR_GUID )
			{
				CGUID val;
				pMsg->GetGUID( val );
				rgn->GetVariableList()->SetVarValue( varName, val );
			}

			// 转发到Rgn所在GS
			if(gsid != pMsg->GetGSID())
			{
				pMsg->SetType(MSG_W2S_RGN_VAR_SET_QUEST);		
				pMsg->SendToGS(gsid);
			}			
		}
		break;
	case MSG_S2W_RGN_VAR_SET_ANSWER:
		{
			long flag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID guid;
			pMsg->GetGUID(guid);
			BYTE gsid = pMsg->GetByte();
			pMsg->SetType(MSG_W2S_RGN_VAR_SET);
			pMsg->SendToGS(gsid);
		}
		break;
	case MSG_S2W_RGN_VAR_GET:
		{
			BYTE srcGSID = pMsg->GetByte();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			long gsid = 0;
			eRgnType rType = (eRgnType)pMsg->GetLong();
			CGUID guid;
			pMsg->GetGUID(guid);
			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
			if(!rgn)
			{
				char szGuid[128];
				guid.tostring(szGuid);
// 				char pszRgnInfo[1024];
// 				_snprintf(pszRgnInfo,1024,"MSG_S2W_RGN_VAR_SET: Rgn[%s] is NULL!.",szGuid);
// 				PutStringToFile("Login_WS_Info",pszRgnInfo);

                Log4c::Warn(ROOT_MODULE,"MSG_S2W_RGN_VAR_SET: Rgn[%s] is NULL!.",szGuid);

				CMessage msg(MSG_W2S_RGN_VAR_GET);
				msg.Add((long)0);
				msg.Add(scriptID);
				msg.SendToSocket(pMsg->GetSocketID());

				return;
			}

			gsid = rgn->GetGsid();

			// 转发到Rgn所在GS
			if(gsid != pMsg->GetGSID())
			{			
				pMsg->SetType(MSG_W2S_RGN_VAR_GET_QUEST);		
				pMsg->SendToGS(gsid);
			}
		}
		break;
	case MSG_S2W_RGN_VAR_GET_ANSWER:
		{
			long flag = pMsg->GetLong();
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			BYTE srcGSID = pMsg->GetByte();
			long gsid = 0;
			CGUID guid;
			pMsg->GetGUID(guid);

			CWorldRegion* rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);
			if(rgn)
			{
				char varName[128];
				pMsg->GetStr(varName, 128);
				//if(varName[0] == '#') // 字符串
				//{
				//	char str[1024];
				//	pMsg->GetStr(str, sizeof(str));
				//	rgn->GetVariableList()->AddVar(varName, str);
				//}
				//else if(varName[0] == '$') // 整数
				//{
				//	long size = pMsg->GetLong();
				//	BYTE* buf = new BYTE[size];
				//	pMsg->GetEx(buf, size);
				//	rgn->GetVariableList()->AddVar(varName, *(double*)buf);
				//	SAFE_DELETE_ARRAY(buf);
				//}
				//else if(varName[0] == '@')
				//{
				//	CGUID guid;
				//	pMsg->GetGUID(guid);
				//	rgn->GetVariableList()->AddGuid(varName, guid);
				//}
				long varType = VarList::VarType( varName );
				if( varType == VariableList::VAR_NUM )
				{
					double val;
					pMsg->GetEx( &val, sizeof( val ) );
					rgn->GetVariableList()->AddVar( varName, val );
				}
				else if( varType == VariableList::VAR_STRING )
				{
					char val[512];
					pMsg->GetStr( val, sizeof( val ) );
					rgn->GetVariableList()->AddVar( varName, tString( val ) );
				}
				else if( varType == VariableList::VAR_GUID )
				{
					CGUID val;
					pMsg->GetGUID( val );
					rgn->GetVariableList()->AddVar( varName, val );
				}
			}

			pMsg->SetType(MSG_W2S_RGN_VAR_GET);		
			pMsg->SendToGS(srcGSID);
		}
		break;
	case MSG_S2W_RGN_VIEWRGNNUM:
		{
			long rgnType = pMsg->GetLong();
			CGUID viewerID;
			pMsg->GetGUID(viewerID);

			// 返回副本个数变量名
			char szRetNumVar[64];
			pMsg->GetStr(szRetNumVar, 64);
			// 挂起的脚本ID
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			CMessage msg(MSG_W2S_RGN_VIEWRGNNUM);
            msg.Add((long)CMessage::GetGSNum()); //GS num

			long totalDupRgnNum = 0;
			CGlobalRgnManager::MapRgnItr itr = GetGame().GetGlobalRgnManager()->GetRgnMap().begin();
			for(; itr != GetGame().GetGlobalRgnManager()->GetRgnMap().end(); itr++)
			{
				if(itr->second->GetGsid() == pMsg->GetGSID())
				{
					totalDupRgnNum++;
				}
			}
			msg.Add(viewerID);
			msg.Add(scriptID);
			if(szRetNumVar)
				msg.Add(szRetNumVar);
			else
				msg.Add("");
			msg.Add(totalDupRgnNum);
			msg.SendToGS(pMsg->GetGSID());
		}
		break;
	case MSG_S2W_RGN_VIEWRGNDATA:
		{
			long gsid = -1;
			CGUID guid;
			pMsg->GetGUID(guid);
			long rgnType = pMsg->GetLong();
			long pos = pMsg->GetLong();
			CGUID viewerID;
			pMsg->GetGUID(viewerID);

			long dataType = pMsg->GetLong();
			char retVarName[128];
			pMsg->GetStr(retVarName, 128);
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			CMessage msg(MSG_W2S_RGN_VIEWRGNDATA);
			msg.Add(gsid);
			msg.Add(rgnType);
			msg.Add(pos);

			CWorldRegion* worldRgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(guid);

			if(worldRgn)
			{
				msg.Add((BYTE)1); // 查看标志
				msg.Add(worldRgn->GetID()); // template id
				msg.Add(worldRgn->GetExID()); // guid
				msg.Add(worldRgn->GetName()); // name
				msg.Add(worldRgn->GetOwnerGUID());   // owner id
			}
			else
			{
				msg.Add((BYTE)0); // 查看标志
			}
			
			msg.Add(viewerID);
			msg.Add(dataType);
			if(retVarName)
				msg.Add(retVarName);
			else
				msg.Add("");
			msg.Add(scriptID);
			msg.SendToGS(pMsg->GetGSID());
		
		}
		break;
	case MSG_S2W_RGN_VIEWRGNAVLID:
		{
			CMessage msg(MSG_W2S_RGN_VIEWRGNAVLID);
			CWorldRegion* retRgn = NULL;
			CGUID rgnID;
			CGUID senderID;
			pMsg->GetGUID(rgnID);
			pMsg->GetGUID(senderID);

			msg.Add(rgnID);
			msg.Add(senderID);

			char retFlagStr[128];
			pMsg->GetStr(retFlagStr, 128);
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			long gsid = -1;
			
			retRgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnID);
			
			if(retRgn)
			{
				msg.Add((BYTE)1); //找到场景
				msg.Add(retFlagStr); // ret falg string
				msg.Add(scriptID); // script id
				msg.Add((BYTE)gsid); //gsid
			}
			else
			{
				msg.Add((BYTE)0); //找到场景
				msg.Add(retFlagStr); // ret falg string
				msg.Add(scriptID); // script id
			}
			msg.SendToGS(pMsg->GetGSID());
		}
		break;
	case MSG_S2W_RGN_VIEWPLAYERPID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID pGUID;
			CPlayer* player = GetGame().GetMapPlayer(playerGUID);
			if(player)
			{
				pGUID = player->GetPersonalRgnGUID();
				CMessage msg(MSG_W2S_RGN_VIEWPLAYERPID);
				msg.Add(playerGUID);
				if(szRet)
					msg.Add(szRet);
				else
					msg.Add("");
				msg.Add(scriptID);
				msg.Add(pGUID);
				msg.SendToGS(pMsg->GetGSID());
			}
		}
		break;
	case MSG_S2W_RGN_VIEWPLAYERPHID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID pGUID;
			CPlayer* player = GetGame().GetMapPlayer(playerGUID);
			if(player)
			{
				pGUID = player->GetPersonalHouseRgnGUID();
				CMessage msg(MSG_W2S_RGN_VIEWPLAYERPHID);
				msg.Add(playerGUID);
				if(szRet)
					msg.Add(szRet);
				else
					msg.Add("");
				msg.Add(scriptID);
				msg.Add(pGUID);
				msg.SendToGS(pMsg->GetGSID());
			}
		}
		break;
	case MSG_S2W_RGN_VIEWPLAYERTID:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			LONG lTemplateID = pMsg->GetLong();
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);
			CGUID pGUID;
			CPlayer* player = GetGame().GetMapPlayer(playerGUID);
			if(player)
			{
				pGUID = GetGame().GetGlobalRgnManager()->GetTeamRgnGUID(playerGUID, lTemplateID);
				CMessage msg(MSG_W2S_RGN_VIEWPLAYERTID);
				msg.Add(playerGUID);
				msg.Add(lTemplateID);
				if(szRet)
					msg.Add(szRet);
				else
					msg.Add("");
				msg.Add(scriptID);
				msg.Add(pGUID);
				msg.SendToGS(pMsg->GetGSID());
			}
		}
		break;
	case MSG_S2W_RGN_GETTEMPLATEID:
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

			long rgnTID = 0;
			CWorldRegion* rgn = NULL;
			rgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnGUID);
			if(rgn)
				rgnTID = rgn->GetID();

			// 发送消息到GS
			CMessage msg(MSG_W2S_RGN_GETTEMPLATEID);
			msg.Add(playerGUID);
			msg.Add((long)rgnType);
			msg.Add(rgnGUID);
			msg.Add(szRet);
			msg.Add(scriptID);
			msg.Add(rgnTID);
			msg.SendToGS(pMsg->GetGSID());
		}
		break;
	case MSG_S2W_RGN_GETDUPTYPE:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			long rgnType = 0;

			CWorldRegion* wRgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnGUID);

			// 发送消息到GS
			CMessage msg(MSG_W2S_RGN_GETDUPTYPE);
			msg.Add(playerGUID);
			msg.Add(rgnGUID);
			msg.Add(szRet);
			msg.Add(scriptID);
			if(wRgn)
				msg.Add((long)wRgn->GetRgnType());
			else
				msg.Add((long)0);
			msg.SendToGS(pMsg->GetGSID());
		}
		break;
	case MSG_S2W_RGN_GETDUPRGNONWER:
		{
			CGUID playerGUID;
			pMsg->GetGUID(playerGUID);
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			char szRet[128];
			pMsg->GetStr(szRet, sizeof(szRet));
			CGUID scriptID;
			pMsg->GetGUID(scriptID);

			CWorldRegion* wRgn = GetGame().GetGlobalRgnManager()->FindRgnByGUID(rgnGUID);

			// 发送消息到WS
			CMessage msg(MSG_W2S_RGN_GETDUPRGNONWER);
			msg.Add(playerGUID);
			msg.Add(rgnGUID);
			msg.Add(szRet);
			msg.Add(scriptID);
			if(wRgn)
				msg.Add(wRgn->GetOwnerGUID());
			else
				msg.Add(NULL_GUID);
			msg.SendToGS(pMsg->GetGSID());
		}
		break;
	case MSG_S2W_RGN_UPDATE_PLAYER_TEAM_RGNGUID:
		{
			DBReadSet setReadDB;
			pMsg->GetDBReadSet(setReadDB);
			CGUID ownerGuid;
			setReadDB.GetBufferFromByteArray(ownerGuid);
			GetGame().GetGlobalRgnManager()->RgnGuidDecode(ownerGuid, setReadDB);

			pMsg->SetType(MSG_W2S_RGN_UPDATE_PLAYER_TEAM_RGNGUID);
			pMsg->SendToAllGS();
		}
		break;
	case MSG_S2W_RGN_CAMP_PLAYER_NUM_QUEST: // GS查询请求
		{
			long srcGSID = pMsg->GetLong(); // 发送端GSID
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			// 获取场景所在GSID
			long gsid = GetGame().GetGlobalRgnManager()->FindGSIDByRgnID(rgnGUID);
			pMsg->SetType(MSG_W2S_RGN_CAMP_PLAYER_NUM_QUEST);
			pMsg->SendToGS(gsid);
		}
		break;
	case MSG_S2W_RGN_CAMP_PLAYER_NUM_ANSWER: // GS查询回应
		{
			long srcGSID = pMsg->GetLong(); // 发送端GSID
			CGUID rgnGUID;
			pMsg->GetGUID(rgnGUID);
			pMsg->SetType(MSG_W2S_RGN_CAMP_PLAYER_NUM_ANSWER);
			pMsg->SendToGS(srcGSID);
		}
		break;
	}
}