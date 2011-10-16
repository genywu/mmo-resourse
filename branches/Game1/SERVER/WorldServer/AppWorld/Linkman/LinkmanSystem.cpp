
#include "StdAfx.h"
#include "LinkmanSystem.h"
#include "..\nets\networld\message.h"
#include "../dbentity/entityManager.h"
#include "../DBEntity/EntityGroup.h"
#include "../session/CSessionFactory.h"
#include "../session/WorldServerSession.h"

#ifdef _DEBUG
//!				好友系统实例计数
LONG			g_LinkmanSystemInstanceNum = 0;
#endif
//!				好友系统全局唯一实例
LinkmanSystem	g_LinkmanSystem;


LinkmanSystem::LinkmanSystem(void)
{
#ifdef _DEBUG
	if (0 != g_LinkmanSystemInstanceNum)
		throw;
	++g_LinkmanSystemInstanceNum;
#endif
}

LinkmanSystem::~LinkmanSystem(void)
{	
}


//! 得到全局唯一实例
LinkmanSystem& LinkmanSystem::GetInstance(void)
{
	return g_LinkmanSystem;
}


//! 得到配置并且初始化对象
bool LinkmanSystem::LoadSetupAndInit(const char* pFilename)
{
	if(m_LGSManage.LoadSetup(pFilename))
	{
		return true;
	}

	return false;
}

//! 将配置信息编码到数组
void LinkmanSystem::SetupAddToByteArray(vector<BYTE>& veData)
{
	m_LGSManage.AddToByteArray(&veData);
}

//! 把一个玩家的数据编码到数组，用于DBS保存
void LinkmanSystem::AddToByteArray_ForDBS(const CGUID &PlayerGuid, CEntityGroup* linkmanGroupOwner)
{
	assert(NULL != linkmanGroupOwner);
	linkmanGroupOwner->SetCurDbOperFlag(1);
	linkmanGroupOwner->SetCurDbOperType(DB_OPER_DELETE_INSERT);

	if(m_mapPlayerLinkMan.find(PlayerGuid) == m_mapPlayerLinkMan.end())
	{		
		linkmanGroupOwner->ReleaseChilds();
		linkmanGroupOwner->SetCurDbOperFlag(0);
		return;
	}

	VecPlayerLinkMan &vPlayerLinkman = m_mapPlayerLinkMan[PlayerGuid];
	vector<tagLinkman_DB> vLinkman;
	for (LONG i = 0; i < vPlayerLinkman.size(); ++i)
	{
		if (eKW_ServerKeep == m_LGSManage.GetLinkmanGroupSetup_ByOrder(i)->m_uKeepWay)
			vPlayerLinkman[i].Serialize_ForDBS(vLinkman);
	}
	
	linkmanGroupOwner->ReleaseChilds();

	for (LONG i = 0; i < vLinkman.size(); ++i)
	{
		CGUID tguid;
		CGUID::CreateGUID(tguid);
		CBaseEntity* linkMan = GetGame()->GetEntityManager()->NewBaseEntity(string("[player_friend]"), tguid);
		vLinkman[i].OwnerGUID = PlayerGuid;
		linkMan->SetGuidAttr(string("OwnerGUID"), PlayerGuid);
		linkMan->SetGuidAttr(string("AimGUID"), vLinkman[i].AimGUID);
		assert(PlayerGuid != vLinkman[i].AimGUID);
		linkMan->SetLongAttr(string("GroupID"), vLinkman[i].uGroupID);
		linkMan->SetLongAttr(string("DelState"), vLinkman[i].uDelState);
		linkMan->SetStringAttr(string("AimName"), vLinkman[i].szAimName);
		linkmanGroupOwner->AddChild(linkMan);

		linkMan->SetCurDbOperFlag(1);
		linkMan->SetCurDbOperType(DB_OPER_DELETE_INSERT);
	}
}

//! 解码DB数据
void LinkmanSystem::DecodeDataFromDBS(const CGUID &PlayerGuid, CEntityGroup* linkmanGroupOwner, CEntityGroup* linkmanGroupAim)
{
	DelPlayerLinkman(PlayerGuid);

	//! 创建数据结构
	VecPlayerLinkMan tmpPlayerLinkMan;

	LONG lGroupNum = m_LGSManage.GetGroupNum();
	for (LONG i = 0; i < lGroupNum; ++i)
	{
		LinkmanGroup tmpLinkmanGroup(m_LGSManage.GetLinkmanGroupSetup_ByOrder(i));

		tmpPlayerLinkMan.push_back(tmpLinkmanGroup);
	}

	map<CGUID, CBaseEntity*>::iterator itr = linkmanGroupOwner->GetEntityGroupMap().begin();
	for(; itr != linkmanGroupOwner->GetEntityGroupMap().end(); itr++)
	{
		tagLinkman_DB Linkman_DB;
		itr->second->GetGuidAttr(string("OwnerGUID"), Linkman_DB.OwnerGUID);
		Linkman_DB.uGroupID = itr->second->GetLongAttr(string("GroupID"));
		itr->second->GetGuidAttr(string("AimGUID"), Linkman_DB.AimGUID);
		strcpy(Linkman_DB.szAimName, itr->second->GetStringAttr(string("AimName")));
		Linkman_DB.uDelState = itr->second->GetLongAttr(string("DelState"));

		if(PlayerGuid == Linkman_DB.OwnerGUID)
		{
			LONG lIdx = m_LGSManage.GetIndexByID(Linkman_DB.uGroupID);
			if(-1 == lIdx)
			{
				assert(false);
				continue;
			}

			assert(Linkman_DB.AimGUID != PlayerGuid);
			if(!tmpPlayerLinkMan[lIdx].AddMember(Linkman_DB))
			{
				assert(false);
				continue;
			}
		}
		else
		{
			assert(false);
			AddErrorLogText("玩家联系人数据出错：拥有者GUID与当前玩家GUID不对应");
		}
	}

	itr = linkmanGroupAim->GetEntityGroupMap().begin();
	for(; itr != linkmanGroupAim->GetEntityGroupMap().end(); itr++)
	{
		tagLinkman_DB Linkman_DB;
		itr->second->GetGuidAttr(string("OwnerGUID"), Linkman_DB.OwnerGUID);
		Linkman_DB.uGroupID = itr->second->GetLongAttr(string("GroupID"));
		itr->second->GetGuidAttr(string("AimGUID"), Linkman_DB.AimGUID);
		strcpy(Linkman_DB.szAimName, itr->second->GetStringAttr(string("AimName")));
		Linkman_DB.uDelState = itr->second->GetLongAttr(string("DelState"));

		if(PlayerGuid == Linkman_DB.OwnerGUID)
		{
			AddErrorLogText("玩家联系人数据出错：目标GUID与当前玩家GUID不对应");
		}
		else
		{
			assert(Linkman_DB.AimGUID == PlayerGuid);
			long groupID = itr->second->GetLongAttr(string("GroupID"));
			LONG lIdx		= m_LGSManage.GetIndexByID(groupID);
			if (-1 != lIdx)
			{
				if(!tmpPlayerLinkMan[lIdx].AddCompriseMePlayer(Linkman_DB))
				{
					assert(false);
					continue;
				}
			}
		}
	}

	m_mapPlayerLinkMan.insert(pair<CGUID, VecPlayerLinkMan>(PlayerGuid, tmpPlayerLinkMan));

	OnLoadPlayerDataFinish(PlayerGuid);
}
//! 响应一个玩家数据读取完成
void LinkmanSystem::OnLoadPlayerDataFinish(const CGUID &PlayerGuid)
{
	//! 发送好友初始化消息，发送好友上线提醒
	/////////////////////////////////////////////////////////////
	CPlayer *pPlayer = GetGame()->GetMapPlayer(PlayerGuid);
	if(NULL == pPlayer) return;
	if(m_mapPlayerLinkMan.end() == m_mapPlayerLinkMan.find(PlayerGuid))
	{
		assert(false); //! 代码进入这里，说明读取玩家的好友信息时失败了，为了防止程序崩溃，重新建立好友数据结构
		VecPlayerLinkMan tmpPlayerLinkMan;

		LONG lGroupNum = m_LGSManage.GetGroupNum();
		for (LONG i = 0; i < lGroupNum; ++i)
		{
			LinkmanGroup tmpLinkmanGroup(m_LGSManage.GetLinkmanGroupSetup_ByOrder(i));

			tmpPlayerLinkMan.push_back(tmpLinkmanGroup);
		}

		m_mapPlayerLinkMan[PlayerGuid] = tmpPlayerLinkMan;
	}

	VecPlayerLinkMan &vPlayerLinkMan = m_mapPlayerLinkMan[PlayerGuid];

	CMessage msgInit(MSG_W2S_LINKMAN_PLAYER_INITDATA);
	msgInit.Add((BYTE)TRUE);
	msgInit.Add(PlayerGuid);
	map<LONG, vector<CGUID>> mapLoginInfo;

	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{
		const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);

		if (NULL != pLinkmanGroupSetup && eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			//! 添加婚姻关系的特殊处理
			if (eLGT_Spouse == pLinkmanGroupSetup->m_uGroupType)
			{
				do 
				{
					if (0 < vPlayerLinkMan[i].GetMeAddMap().size())
					{
						//! 判断是否有线下离婚行为，若有，则完成离婚
						if (1 == vPlayerLinkMan[i].GetMeAddMap().size() && 0 < vPlayerLinkMan[i].GetAddMeMap().size())
						{
							const CGUID &SpouseGuid = vPlayerLinkMan[i].GetMeAddMap().begin()->first;
							//! 从添加自己的人中找到自己添加的人
							if (vPlayerLinkMan[i].GetAddMeMap().end() != vPlayerLinkMan[i].GetAddMeMap().find(SpouseGuid))
							{
								//! 找的到对方的联系人对象
								map<CGUID, VecPlayerLinkMan>::iterator iteSpouseLinkMan = m_mapPlayerLinkMan.find(SpouseGuid);
								if(m_mapPlayerLinkMan.end() != iteSpouseLinkMan)
								{
									if (iteSpouseLinkMan->second.size() > i)
									{
										if(iteSpouseLinkMan->second[i].GetMeAddMap().end() != iteSpouseLinkMan->second[i].GetMeAddMap().find(PlayerGuid))
											break;
									}
								}
								//! 找不到则以自己的数据为准
								else
									break;
							}
						}

						//! 前面的系列判断中，无法确认配偶的合法性，直接清除该组信息
						vPlayerLinkMan[i].Release();
					}
				} while(false);		
			}

			//! 统计需要登陆信息的玩家和GS服务器
			const map<CGUID, tagLinkman_DB>& AddMeMap = vPlayerLinkMan[i].GetAddMeMap();
			for (map<CGUID, tagLinkman_DB>::const_iterator ite = AddMeMap.begin(); ite != AddMeMap.end(); ++ite)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(ite->first);
				if (NULL != pPlayer)
				{
					//! 统计信息
					if (mapLoginInfo.find(pPlayer->GetGsSocket()) == mapLoginInfo.end())
					{
						mapLoginInfo.insert(pair<LONG, vector<CGUID>>(pPlayer->GetGsSocket(), vector<CGUID>()));
					}
					mapLoginInfo[pPlayer->GetGsSocket()].push_back(ite->first);	
				}
			}

			//! 更新添加我的玩家的成员状态
			const map<CGUID, tagMeAddPlayer>& MeAddMap = vPlayerLinkMan[i].GetMeAddMap();
			for ( map<CGUID, tagMeAddPlayer>::const_iterator ite = MeAddMap.begin(); ite != MeAddMap.end(); ++ite)
			{
				map<CGUID, VecPlayerLinkMan>::iterator iteMeAddLinkman = m_mapPlayerLinkMan.find(ite->first);
				if (iteMeAddLinkman != m_mapPlayerLinkMan.end())
				{
					iteMeAddLinkman->second[i].LinkmanLogin(ite->first);
				}
			}
		}

		//! 获取发送到玩家所在GS服务器的初始化数据
		vPlayerLinkMan[i].UpdateOnlineState();
		vPlayerLinkMan[i].Serialize_ForGs(&msgInit);
	}

	//! 发送初始化数据
	msgInit.SendToSocket(pPlayer->GetGsSocket());

	//! 获取登陆信息
	tagLinkman_data Linkman_data;
	Linkman_data.guid = PlayerGuid;
	memmove(Linkman_data.szName, pPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	LinkmanGroup::UpdateLinkman_data(&Linkman_data);

	//! 发送玩家登陆通知
	for (map<LONG, vector<CGUID>>::iterator ite = mapLoginInfo.begin(); ite != mapLoginInfo.end(); ++ite)
	{
		CMessage msgLogin(MSG_W2S_LINKMAN_PLAYER_LOGIN);
		msgLogin.AddEx(&Linkman_data, sizeof(tagLinkman_data));
		msgLogin.Add((LONG)ite->second.size());
		for(size_t i = 0; i < ite->second.size(); ++i)
			msgLogin.Add(ite->second[i]);

		msgLogin.SendToSocket(ite->first);
	}

}

//! 响应玩家上线
void LinkmanSystem::OnPlayerOnline(const CGUID &PlayerGuid)
{
	if(m_TimeoutQueue.find_and_erase(PlayerGuid))
	{
		OnLoadPlayerDataFinish(PlayerGuid);
		return;
	}
	//! 发送数据读取请求
	//##创建会话
	CWorldServerSession* pSession = (CWorldServerSession*)(CWorldServerSession*)GetGame()->GetSessionFactoryInst()->CreateSession(1800000, ST_WS_SESSION);
	if( pSession && pSession->Start() )
	{
		pSession->AddOneSessionStep(CWorldServerSession::WORLD_SESSION_LOAD_LINKMAN);
		pSession->SendLoadLinkmanMsg(PlayerGuid);
	}
}


//! 响应玩家离线
void LinkmanSystem::OnPlayerOffline(CPlayer *pPlayer)
{
	map<CGUID, VecPlayerLinkMan>::iterator iteLinkman = m_mapPlayerLinkMan.find(pPlayer->GetExID());
	if (m_mapPlayerLinkMan.end() == iteLinkman) return;

	VecPlayerLinkMan &vPlayerLinkMan = iteLinkman->second;

	map<LONG, vector<CGUID>> mapLoginInfo;

	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{
		const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			//! 统计需要登陆信息的玩家和GS服务器
			const map<CGUID, tagLinkman_DB>& AddMeMap = vPlayerLinkMan[i].GetAddMeMap();
			for (map<CGUID, tagLinkman_DB>::const_iterator ite = AddMeMap.begin(); ite != AddMeMap.end(); ++ite)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(ite->first);
				if (NULL != pPlayer)
				{
					Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());
						
					if(!tLoginInfo)
					{
						char outStr[1024];
						sprintf(outStr, "Err OnPlayerOffline: LoginInfo[%s] Is Null!", pPlayer->GetAccount());
						AddLogText(outStr);
						return;
					}

					if(tLoginInfo && PLAYER_STATE_BACKTOLOGIN != tLoginInfo->GetPlayerState())
					{
						//! 统计信息
						if (mapLoginInfo.find(pPlayer->GetGsSocket()) == mapLoginInfo.end())
						{
							mapLoginInfo.insert(pair<LONG, vector<CGUID>>(pPlayer->GetGsSocket(), vector<CGUID>()));
						}
						mapLoginInfo[pPlayer->GetGsSocket()].push_back(ite->first);	
					}
				}
			}

			//! 更新添加我的玩家的成员状态
			const map<CGUID, tagMeAddPlayer>& MeAddMap = vPlayerLinkMan[i].GetMeAddMap();
			for ( map<CGUID, tagMeAddPlayer>::const_iterator ite = MeAddMap.begin(); ite != MeAddMap.end(); ++ite)
			{
				map<CGUID, VecPlayerLinkMan>::iterator iteMeAddLinkman = m_mapPlayerLinkMan.find(ite->first);
				if (iteMeAddLinkman != m_mapPlayerLinkMan.end())
				{
					iteMeAddLinkman->second[i].LinkmanLogout(ite->first);
				}
			}
		}
	}

	//! 发送玩家下线通知
	for (map<LONG, vector<CGUID>>::iterator ite = mapLoginInfo.begin(); ite != mapLoginInfo.end(); ++ite)
	{
		CMessage msgLogout(MSG_W2S_LINKMAN_PLAYER_LOGOUT);
		msgLogout.Add(pPlayer->GetExID());
		msgLogout.Add((LONG)ite->second.size());
		for(size_t i = 0; i < ite->second.size(); ++i)
			msgLogout.Add(ite->second[i]);

		msgLogout.SendToSocket(ite->first);
	}

	SavePlayerData(pPlayer->GetExID());

	PushCache(pPlayer->GetExID());
	ClearTimeoutData();
}

//!
void LinkmanSystem::SavePlayerData(const CGUID &PlayerGuid)
{
	//! 发送数据读取请求
	//##创建会话
	CWorldServerSession* pSession = (CWorldServerSession*)(CWorldServerSession*)GetGame()->GetSessionFactoryInst()->CreateSession(1800000, ST_WS_SESSION);
	if( pSession && pSession->Start() )
	{
		pSession->AddOneSessionStep(CWorldServerSession::WORLD_SESSION_SAVE_LINKMAN);
		pSession->SendSaveLinkmanMsg(PlayerGuid);
	}
}

//! 响应玩家删除状态改变
void LinkmanSystem::OnPlayerDelStateChange(const CGUID &PlayerGuid, BOOL bDeleted)
{
}

//!								响应玩家等级改变
void LinkmanSystem::OnPlayerLevelChange(CPlayer *pPlayer)
{
	const CGUID &PlayerGuid = pPlayer->GetExID();
	LONG lLevel = pPlayer->GetLevel();

	map<CGUID, VecPlayerLinkMan>::iterator itePlayerLinkMan = m_mapPlayerLinkMan.find(PlayerGuid);
	if(itePlayerLinkMan == m_mapPlayerLinkMan.end()) return;

	VecPlayerLinkMan &vPlayerLinkMan = itePlayerLinkMan->second;

	//! 发送列表
	map<LONG, vector<CGUID>> mapSendTable;

	//! 更新WS资料，统计发送列表
	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{	
		const LinkmanGroupSetup *pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay && eTIT_Show_All == pLinkmanGroupSetup->m_uTimelyInfo)
		{
			//! 统计需要登陆信息的玩家和GS服务器
			const map<CGUID, tagLinkman_DB>& AddMeMap = vPlayerLinkMan[i].GetAddMeMap();
			for (map<CGUID, tagLinkman_DB>::const_iterator ite = AddMeMap.begin(); ite != AddMeMap.end(); ++ite)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(ite->first);
				if (NULL != pPlayer)
				{
					Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());

					if(!tLoginInfo)
					{
						char outStr[1024];
						sprintf(outStr, "Err OnPlayerLevelChange: LoginInfo[%s] Is Null!", pPlayer->GetAccount());
						AddLogText(outStr);
						return;
					}
					if(tLoginInfo && PLAYER_STATE_BACKTOLOGIN != tLoginInfo->GetPlayerState())
					{
						//! 统计信息
						if (mapSendTable.find(pPlayer->GetGsSocket()) == mapSendTable.end())
						{
							mapSendTable.insert(pair<LONG, vector<CGUID>>(pPlayer->GetGsSocket(), vector<CGUID>()));
						}
						mapSendTable[pPlayer->GetGsSocket()].push_back(ite->first);
					}
				}
			}
		}
	}

	//! 发送通知
	for (map<LONG, vector<CGUID>>::iterator ite = mapSendTable.begin(); ite != mapSendTable.end(); ++ite)
	{
		CMessage msgToGs(MSG_W2S_LINKMAN_PLAYER_INFO_CHANGE);
		msgToGs.Add(PlayerGuid);
		msgToGs.Add((LONG)eLIT_Level);
		msgToGs.Add(lLevel);
		msgToGs.Add((LONG)ite->second.size());
		for(size_t i = 0; i < ite->second.size(); ++i)
			msgToGs.Add(ite->second[i]);
		msgToGs.SendToSocket(ite->first);
	}
}

//!								响应玩家职业改变
void LinkmanSystem::OnPlayerOccuChange(CPlayer *pPlayer)
{
	const CGUID &PlayerGuid = pPlayer->GetExID();
	LONG lOccupation = pPlayer->GetOccupation();

	map<CGUID, VecPlayerLinkMan>::iterator itePlayerLinkMan = m_mapPlayerLinkMan.find(PlayerGuid);
	if(itePlayerLinkMan == m_mapPlayerLinkMan.end()) return;

	VecPlayerLinkMan &vPlayerLinkMan = itePlayerLinkMan->second;

	//! 发送列表
	map<LONG, vector<BYTE>> mapSendTable;

	//! 更新WS资料，统计发送列表
	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{	
		const LinkmanGroupSetup *pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay && eTIT_Show_All == pLinkmanGroupSetup->m_uTimelyInfo)
		{
			//! 统计需要登陆信息的玩家和GS服务器
			const map<CGUID, tagLinkman_DB>& AddMeMap = vPlayerLinkMan[i].GetAddMeMap();
			for (map<CGUID, tagLinkman_DB>::const_iterator ite = AddMeMap.begin(); ite != AddMeMap.end(); ++ite)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(ite->first);
				if (NULL != pPlayer)
				{
					Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());

					if(!tLoginInfo)
					{
						char outStr[1024];
						sprintf(outStr, "Err OnPlayerLeveUp: LoginInfo[%s] Is Null!", pPlayer->GetAccount());
						AddLogText(outStr);
						return;
					}
					if(tLoginInfo && PLAYER_STATE_BACKTOLOGIN != tLoginInfo->GetPlayerState())
					{
						//! 统计信息
						if (mapSendTable.find(pPlayer->GetGsSocket()) == mapSendTable.end())
						{
							mapSendTable.insert(pair<LONG, vector<BYTE>>(pPlayer->GetGsSocket(), vector<BYTE>()));
						}
						_AddToByteArray(&mapSendTable[pPlayer->GetGsSocket()], ite->first);	
					}
				}
			}
		}
	}

	//! 发送通知
	for (map<LONG, vector<BYTE>>::iterator ite = mapSendTable.begin(); ite != mapSendTable.end(); ++ite)
	{
		CMessage msgToGs(MSG_W2S_LINKMAN_PLAYER_INFO_CHANGE);
		msgToGs.Add(PlayerGuid);
		msgToGs.Add((LONG)eLIT_Occupation);
		msgToGs.Add(lOccupation);
		msgToGs.Add((LONG)ite->second.size());
		for(size_t i = 0; i < ite->second.size(); ++i)
			msgToGs.Add(ite->second[i]);
		msgToGs.SendToSocket(ite->first);
	}
}

//! 能否留言
ULONG LinkmanSystem::CanLeaveWord(const CGUID &SenderGuid, const char *pReceiverName, CGUID &reReceiverGuid)
{
	assert(m_mapPlayerLinkMan.end() != m_mapPlayerLinkMan.find(SenderGuid));
	VecPlayerLinkMan &vPlayerLinkManmap = m_mapPlayerLinkMan[SenderGuid];

	ULONG uRe = eLWPRI_Cant;
	for (LONG i = 0; i < vPlayerLinkManmap.size(); ++i)
	{
		const LinkmanGroupSetup *pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			//! 如果这个组有屏蔽功能
			if (eFFT_Filtrate_All == pLinkmanGroupSetup->m_uFunctionFiltrate)
			{
				if (NULL_GUID != vPlayerLinkManmap[i].FindMember(pReceiverName))
				{
					return eLWPRI_Cant;
				}
			}
			//! 否则
			else 
			{
				const CGUID &FindGuid = vPlayerLinkManmap[i].FindMember(pReceiverName);
				if (NULL_GUID != FindGuid)
				{
					reReceiverGuid = FindGuid;
				}

				//! 找出最高的留言优先级
				if (eLWPRI_Cant != pLinkmanGroupSetup->m_uLeaveWordPRI && NULL_GUID != reReceiverGuid)
				{
					uRe = (pLinkmanGroupSetup->m_uLeaveWordPRI > uRe) ? pLinkmanGroupSetup->m_uLeaveWordPRI : uRe;
				}
			}
		}
	}
	return uRe;
}

//! 能否发信（接收者必须在线）
BOOL LinkmanSystem::CanMail(CPlayer *pReceiver, const char *pSenderName)
{
	if(NULL == pSenderName || NULL == pReceiver)
		return FALSE;

	const CGUID &ReceiverGuid = pReceiver->GetExID();

	assert(m_mapPlayerLinkMan.end() != m_mapPlayerLinkMan.find(ReceiverGuid));
	VecPlayerLinkMan &vPlayerLinkManmap = m_mapPlayerLinkMan[ReceiverGuid];

	for (LONG i = 0; i < vPlayerLinkManmap.size(); ++i)
	{
		const LinkmanGroupSetup *pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			//! 如果这个组有屏蔽功能
			if (eFFT_Filtrate_All == pLinkmanGroupSetup->m_uFunctionFiltrate)
			{
				if (NULL_GUID != vPlayerLinkManmap[i].FindMember(pSenderName))
				{
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

//!-----------------------------------------------------------------------------------------------

//! 响应系统请求添加消息
void LinkmanSystem::OnSystemRequestAddMsg(CMessage *pMsg)
{
	CGUID OwnerGuid;
	pMsg->GetGUID(OwnerGuid);
	ULONG lGroupID	= pMsg->GetLong();
	char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

	CMessage msgReAdd(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE);
	//! 确认操作双方存在
	CPlayer *pRequsetPlayer = GetGame()->GetMapPlayer(OwnerGuid);
	if(NULL == pRequsetPlayer) return;
	//! 判断是否发送名字为“隐匿者”
	if(0 == strcmp(szAimName, CGlobeSetup::GetSetup()->szHideName))
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimHideName);
		msgReAdd.Add(CGlobeSetup::GetSetup()->szHideName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	CPlayer *pAimPlayer = GetGame()->GetMapPlayer(szAimName);
	if(NULL == pAimPlayer)
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimNotBeing);
		msgReAdd.Add(szAimName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	if( pAimPlayer->GetHideFlag() == 1 && !GetGame()->PlayerIsGM(pRequsetPlayer)
		&& (pAimPlayer->GetTeamID() != pRequsetPlayer->GetTeamID() || pAimPlayer->GetTeamID()==NULL_GUID || pRequsetPlayer->GetTeamID()==NULL_GUID) )
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimHideName);
		msgReAdd.Add(CGlobeSetup::GetSetup()->szHideName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	//! ------------确认操作合法性------------
	LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	if (-1 == lIdx) return;
	const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
	assert(pLinkmanGroupSetup);
	if(NULL == pLinkmanGroupSetup) return;
	if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
	const CGUID &AimGuid = pAimPlayer->GetExID();
	if (AimGuid == OwnerGuid)return;

	//! 找到请求者的联系人对象
	map<CGUID, VecPlayerLinkMan>::iterator ite = m_mapPlayerLinkMan.find(OwnerGuid);
	if (m_mapPlayerLinkMan.end() == ite) return;
	VecPlayerLinkMan &PlayerLinkMan = ite->second;
	if(ite->second.size() <= lIdx) return;
	//! 确认玩家没有被这个组和其他组添加
	if(pLinkmanGroupSetup->m_bMemberIsOnly)
	{
		for (LONG i = 0; i < PlayerLinkMan.size(); ++i)
		{
			const LinkmanGroupSetup *pTmpLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
			if(!pTmpLinkmanGroupSetup->m_bMemberIsOnly)
				continue;
			if(NULL_GUID != PlayerLinkMan[i].FindMember(szAimName)) return;
		}
	}
	
	//! 判断人数
	if (PlayerLinkMan[lIdx].GetMemberNum() >= pLinkmanGroupSetup->m_uMaxMemberNum) return;

	if(eLGT_Spouse == pLinkmanGroupSetup->m_uGroupType)
	{
		map<CGUID, VecPlayerLinkMan>::iterator iteAim = m_mapPlayerLinkMan.find(AimGuid);
		if (m_mapPlayerLinkMan.end() == iteAim) return;
		if (0 < iteAim->second[lIdx].GetMeAddMap().size())
			if(iteAim->second[lIdx].GetMeAddMap().end() == iteAim->second[lIdx].GetMeAddMap().find(OwnerGuid)) return;
	}

	if(pLinkmanGroupSetup->m_bAddAimAffirm)
	{
		assert(false);
		AddErrorLogText("Linkman ini error: GroupID = %d, GroupIndex = %d", lGroupID, lIdx);
		return;
	}

	//! 开始添加
	tagLinkman_DB Linkman_DB;
	Linkman_DB.AimGUID = AimGuid;
	Linkman_DB.OwnerGUID = OwnerGuid;
	Linkman_DB.uGroupID = lGroupID;
	memmove(Linkman_DB.szAimName, szAimName, MAX_PLAYER_NAME_SIZE);
	Linkman_DB.uDelState = eLDS_NotDel;

	//! 更新添加者的[我添加的联系人]
	PlayerLinkMan[lIdx].AddMember(Linkman_DB);

	//! 更新被添加人的[添加我的联系人]
	if(m_mapPlayerLinkMan.end() != m_mapPlayerLinkMan.find(AimGuid))
		if(m_mapPlayerLinkMan[AimGuid].size() > lIdx)
			m_mapPlayerLinkMan[AimGuid][lIdx].AddCompriseMePlayer(Linkman_DB);

	tagLinkman_data Linkman_data;
	Linkman_data.guid = AimGuid;
	memmove(Linkman_data.szName, szAimName, MAX_PLAYER_NAME_SIZE);
	LinkmanGroup::UpdateLinkman_data(&Linkman_data);

	CMessage msgAddNotify(MSG_W2S_LINKMAN_ADD_NOTIFY);
	msgAddNotify.Add(OwnerGuid);
	msgAddNotify.Add(pRequsetPlayer->GetName());
	msgAddNotify.Add(lGroupID);
	msgAddNotify.AddEx(&Linkman_data, sizeof(tagLinkman_data));

	msgAddNotify.SendToSocket(pRequsetPlayer->GetGsSocket());
	if(pAimPlayer->GetGsSocket() != pRequsetPlayer->GetGsSocket())
		msgAddNotify.SendToSocket(pAimPlayer->GetGsSocket());

	if(eLGT_Spouse == pLinkmanGroupSetup->m_uGroupType)
		SavePlayerData(OwnerGuid);

}
//! 响应玩家请求添加消息
void LinkmanSystem::OnPlayerRequestAddMsg(CMessage *pMsg)
{
	CGUID OwnerGuid;
	pMsg->GetGUID(OwnerGuid);
	ULONG lGroupID	= pMsg->GetLong();
	char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

	CMessage msgReAdd(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE);
	//! 确认操作双方存在
	CPlayer *pRequsetPlayer = GetGame()->GetMapPlayer(OwnerGuid);
	if(NULL == pRequsetPlayer) return;

	//! 判断是否发送名字为“隐匿者”
	if(0 == strcmp(szAimName, CGlobeSetup::GetSetup()->szHideName))
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimHideName);
		msgReAdd.Add(CGlobeSetup::GetSetup()->szHideName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	CPlayer *pAimPlayer = GetGame()->GetMapPlayer(szAimName);
	if(NULL == pAimPlayer)
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimNotBeing);
		msgReAdd.Add(szAimName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	if( pAimPlayer->GetHideFlag() == 1 && !GetGame()->PlayerIsGM(pRequsetPlayer)
		&& (pAimPlayer->GetTeamID() != pRequsetPlayer->GetTeamID() || pAimPlayer->GetTeamID()==NULL_GUID || pRequsetPlayer->GetTeamID()==NULL_GUID) ) // 隐匿者
	{
		msgReAdd.Add((LONG)FALSE);
		msgReAdd.Add((LONG)eOET_AimHideName);
		msgReAdd.Add(CGlobeSetup::GetSetup()->szHideName);
		msgReAdd.Add(OwnerGuid);
		msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		return;
	}

	//! ------------确认操作合法性------------
	LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	if (-1 == lIdx) return;
	const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
	assert(pLinkmanGroupSetup);
	if(NULL == pLinkmanGroupSetup) return;
	if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
	if (!pLinkmanGroupSetup->m_bUserAdd) return;
	const CGUID &AimGuid = pAimPlayer->GetExID();
	if (AimGuid == OwnerGuid)return;

	//! 找到请求者的联系人对象
	map<CGUID, VecPlayerLinkMan>::iterator ite = m_mapPlayerLinkMan.find(OwnerGuid);
	if (m_mapPlayerLinkMan.end() == ite) return;
	VecPlayerLinkMan &PlayerLinkMan = ite->second;
	if(PlayerLinkMan.size() <= lIdx) return;
	//! 确认玩家没有被这个组和其他组添加
	if(pLinkmanGroupSetup->m_bMemberIsOnly)
	{
		for (LONG i = 0; i < PlayerLinkMan.size(); ++i)
		{
			const LinkmanGroupSetup *pTmpLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
			if(!pTmpLinkmanGroupSetup->m_bMemberIsOnly)
				continue;
			if(NULL_GUID != PlayerLinkMan[i].FindMember(szAimName)) return;
		}
	}

	//! 判断人数
	if (PlayerLinkMan[lIdx].GetMemberNum() >= pLinkmanGroupSetup->m_uMaxMemberNum) return;

	//! 是否需要目标确认
	if (pLinkmanGroupSetup->m_bAddAimAffirm)
	{
		//! 创建会话数据
		tagOptSession OptSession;
		OptSession._uEndTime	= time(NULL) + MAX_LINKMAN_OPT_TIME_SEC;
		OptSession._uOptType	= eLOT_Add;
		OptSession._uSrcGroupID	= INVALID_GROUP_ID;
		OptSession._uDesGroupID	= lGroupID;

		if(CreateAddSession(AimGuid, OwnerGuid, OptSession))
		{
			CMessage msgrEquestAdd(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_NOTIFY);
			msgrEquestAdd.Add(lGroupID);
			msgrEquestAdd.Add(pRequsetPlayer->GetName());
			msgrEquestAdd.Add(OwnerGuid);
			msgrEquestAdd.Add(AimGuid);
			msgrEquestAdd.SendToSocket(pAimPlayer->GetGsSocket());
		}
		else
		{
			msgReAdd.Add((LONG)FALSE);
			msgReAdd.Add((LONG)eOET_OverfullToOne);
			msgReAdd.Add(szAimName);
			msgReAdd.Add(OwnerGuid);
			msgReAdd.SendToSocket(pRequsetPlayer->GetGsSocket());
		}
	}
	else
	{
		tagLinkman_DB Linkman_DB;
		Linkman_DB.AimGUID = AimGuid;
		Linkman_DB.OwnerGUID = OwnerGuid;
		memmove(Linkman_DB.szAimName, szAimName, MAX_PLAYER_NAME_SIZE);
		Linkman_DB.uDelState = eLDS_NotDel;
		Linkman_DB.uGroupID = lGroupID;

		//! 更新添加者的[我添加的联系人]
		PlayerLinkMan[lIdx].AddMember(Linkman_DB);

		//! 更新被添加人的[添加我的联系人]
		if(m_mapPlayerLinkMan.end() != m_mapPlayerLinkMan.find(AimGuid))
			if(m_mapPlayerLinkMan[AimGuid].size() > lIdx)
				m_mapPlayerLinkMan[AimGuid][lIdx].AddCompriseMePlayer(Linkman_DB);

		tagLinkman_data Linkman_data;
		Linkman_data.guid = AimGuid;
		memmove(Linkman_data.szName, szAimName, MAX_PLAYER_NAME_SIZE);
		LinkmanGroup::UpdateLinkman_data(&Linkman_data);

		CMessage msgAddNotify(MSG_W2S_LINKMAN_ADD_NOTIFY);
		msgAddNotify.Add(OwnerGuid);
		msgAddNotify.Add(pRequsetPlayer->GetName());
		msgAddNotify.Add(lGroupID);
		msgAddNotify.AddEx(&Linkman_data, sizeof(tagLinkman_data));
		
		msgAddNotify.SendToSocket(pRequsetPlayer->GetGsSocket());
		if(pAimPlayer->GetGsSocket() != pRequsetPlayer->GetGsSocket())
			msgAddNotify.SendToSocket(pAimPlayer->GetGsSocket());;
		
	}
}

//! 响应玩家回复添加消息
void LinkmanSystem::OnPlayerReAddMsg(CMessage *pMsg)
{
	ULONG lGroupID	= pMsg->GetLong();
	CGUID RequestPlayerGuid;
	pMsg->GetGUID(RequestPlayerGuid);
	BOOL lAgree		= pMsg->GetLong();
	CGUID RePlayerGuid;
	pMsg->GetGUID(RePlayerGuid);

	//! 创建验证会话的对象
	tagOptSession OptSession;
	OptSession._uSrcGroupID = INVALID_GROUP_ID;
	OptSession._uOptType	= eLOT_Add;
	OptSession._uDesGroupID = lGroupID;

	BOOL bLegitimate = FALSE;
	if (EndAddSession(RePlayerGuid, RequestPlayerGuid, OptSession))
	{
		bLegitimate = TRUE;
	}
	else if (eLot_Move == OptSession._uOptType)
	{
		bLegitimate = TRUE;
	}

	if (bLegitimate)
	{
		CPlayer *pRePlayer = GetGame()->GetMapPlayer(RePlayerGuid);
		CPlayer *pRequestPlayer = GetGame()->GetMapPlayer(RequestPlayerGuid);
		if(NULL == pRePlayer || NULL == pRequestPlayer) return;

		LONG lSrcIdx = m_LGSManage.GetIndexByID(OptSession._uSrcGroupID);
		LONG lDesIdx = m_LGSManage.GetIndexByID(OptSession._uDesGroupID);
		if(-1 == lDesIdx) 
		{
			assert(false);
			return;
		}
		if (lAgree)
		{
			map<CGUID, VecPlayerLinkMan>::iterator iteRequest = m_mapPlayerLinkMan.find(RequestPlayerGuid);
			if(m_mapPlayerLinkMan.end() == iteRequest) return;
			if(iteRequest->second.size() <= lDesIdx && iteRequest->second.size() <= lSrcIdx) return;
			LinkmanGroup &RequestLinkmanGroup = (iteRequest->second)[lDesIdx];

			map<CGUID, VecPlayerLinkMan>::iterator iteRe = m_mapPlayerLinkMan.find(RePlayerGuid);
			if (m_mapPlayerLinkMan.end() == iteRe) return;
			if(iteRe->second.size() <= lDesIdx && iteRe->second.size() <= lSrcIdx) return;
			LinkmanGroup &ReLinkmanGroup = (iteRe->second)[lDesIdx];

			//! 如果人数满了，则返回错误提示
			const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
			assert(NULL != pLinkmanGroupSetup);
			if(NULL == pLinkmanGroupSetup) return;
			if(RequestLinkmanGroup.GetMemberNum() > pLinkmanGroupSetup->m_uMaxMemberNum)
			{
				if(NULL != pRequestPlayer)
				{
					CMessage msg(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE);
					msg.Add((LONG)FALSE);
					msg.Add((LONG)eOET_ListFull);
					msg.Add(pRePlayer->GetName());
					msg.Add(RequestPlayerGuid);
					msg.SendToSocket(pRequestPlayer->GetGsSocket());
				}
				return;
			}

			//! 如果不是移动请求，联系人在其他组则返回失败
			if (eLot_Move != OptSession._uOptType)
			{
				pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
				if (NULL == pLinkmanGroupSetup)
					return;
				if (pLinkmanGroupSetup->m_bMemberIsOnly)
				{
					for (LONG i = 0; i < iteRequest->second.size(); ++i)
					{
						pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
						assert(NULL != pLinkmanGroupSetup);
						if(NULL == pLinkmanGroupSetup) continue;

						if(!pLinkmanGroupSetup->m_bMemberIsOnly) continue;
						if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
						{
							if (NULL_GUID != iteRequest->second[i].FindMember(pRePlayer->GetName()))
							{
								if(NULL != pRequestPlayer)
								{
									CMessage msg(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE);
									msg.Add((LONG)FALSE);
									msg.Add((LONG)eOET_AimInOther);
									msg.Add(pRePlayer->GetName());
									msg.Add(RequestPlayerGuid);
									msg.SendToSocket(pRequestPlayer->GetGsSocket());
								}
								return;
							}
						}
					}
				}	
			}

			//! 创建联系人数据对象
			tagLinkman_DB Linkman_DB;
			Linkman_DB.AimGUID = RePlayerGuid;
			Linkman_DB.OwnerGUID = RequestPlayerGuid;
			memmove(Linkman_DB.szAimName, pRePlayer->GetName(), MAX_PLAYER_NAME_SIZE);
			Linkman_DB.uDelState = eLDS_NotDel;
			Linkman_DB.uGroupID = OptSession._uDesGroupID;

			if (eLot_Move == OptSession._uOptType)
			{
				if(-1 == lSrcIdx) 
				{
					assert(false);
					return;
				}
				pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx);
				assert(NULL != pLinkmanGroupSetup);
				if(NULL == pLinkmanGroupSetup) return;
				if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
				{
					(iteRequest->second)[lSrcIdx].DelMember(RePlayerGuid);
					GetGameLogInterface()->logT450_linkman_opt_log(pRequestPlayer, RePlayerGuid, pRePlayer->GetName(), 2, OptSession._uSrcGroupID);

					(iteRe->second)[lSrcIdx].DelCompriseMePlayer(RequestPlayerGuid);
				}

				//! 添加联系人
				RequestLinkmanGroup.AddMember(Linkman_DB);
				GetGameLogInterface()->logT450_linkman_opt_log(pRequestPlayer, RePlayerGuid, pRePlayer->GetName(), 1, OptSession._uDesGroupID);

				if(NULL != pRequestPlayer)
				{
					tagLinkman_data Linkman_data;
					Linkman_data.guid = RePlayerGuid;
					memmove(Linkman_data.szName, pRePlayer->GetName(), MAX_PLAYER_NAME_SIZE);
					LinkmanGroup::UpdateLinkman_data(&Linkman_data);

					CMessage msgMoveNotify(MSG_W2S_LINKMAN_MOVE_NOTIFY);
					msgMoveNotify.Add((LONG)TRUE);
					msgMoveNotify.Add(RequestPlayerGuid);
					msgMoveNotify.Add(pRequestPlayer->GetName());
					msgMoveNotify.Add(OptSession._uSrcGroupID);
					msgMoveNotify.Add(OptSession._uDesGroupID);
					msgMoveNotify.AddEx(&Linkman_data, sizeof(tagLinkman_data));

					msgMoveNotify.SendToSocket(pRequestPlayer->GetGsSocket());
					if(pRePlayer->GetGsSocket() != pRequestPlayer->GetGsSocket())
						msgMoveNotify.SendToSocket(pRePlayer->GetGsSocket());;
				}
			}
			else
			{	
				//! 添加联系人
				RequestLinkmanGroup.AddMember(Linkman_DB);
		
				GetGameLogInterface()->logT450_linkman_opt_log(pRequestPlayer, RePlayerGuid, pRePlayer->GetName(), 1, OptSession._uDesGroupID);
				
				tagLinkman_data Linkman_data;
				Linkman_data.guid = RePlayerGuid;
				memmove(Linkman_data.szName, pRePlayer->GetName(), MAX_PLAYER_NAME_SIZE);
				LinkmanGroup::UpdateLinkman_data(&Linkman_data);

				CMessage msgAddNotify(MSG_W2S_LINKMAN_ADD_NOTIFY);
				msgAddNotify.Add(RequestPlayerGuid);
				msgAddNotify.Add(pRequestPlayer->GetName());
				msgAddNotify.Add(OptSession._uDesGroupID);
				msgAddNotify.AddEx(&Linkman_data, sizeof(tagLinkman_data));

				msgAddNotify.SendToSocket(pRequestPlayer->GetGsSocket());
				if(pRePlayer->GetGsSocket() != pRequestPlayer->GetGsSocket())
					msgAddNotify.SendToSocket(pRePlayer->GetGsSocket());;
			}

			//! 添加[添加我的人]
			ReLinkmanGroup.AddCompriseMePlayer(Linkman_DB);
		}
		else
		{
			if(NULL != pRequestPlayer)
			{
				CMessage msg(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_RE);
				msg.Add((LONG)FALSE);
				msg.Add((LONG)eOET_AimReject);
				msg.Add(pRePlayer->GetName());
				msg.Add(RequestPlayerGuid);
				msg.SendToSocket(pRequestPlayer->GetGsSocket());
			}
		}
	}
}

//! 响应玩家请求删除消息
void LinkmanSystem::OnPlayerRequestDelMsg(CMessage *pMsg)
{
	CGUID OwnerGuid;
	pMsg->GetGUID(OwnerGuid);
	ULONG lGroupID	= pMsg->GetLong();
	char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);
	bool bIsSysRequest = (0 != pMsg->GetByte());

	LONG lIdx = m_LGSManage.GetIndexByID(lGroupID);
	const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lIdx);
	assert(NULL != pLinkmanGroupSetup);
	if(NULL == pLinkmanGroupSetup) return;

	//! 确认合法性
	if (eKW_ServerKeep != pLinkmanGroupSetup->m_uKeepWay) return;
	if (!bIsSysRequest && !pLinkmanGroupSetup->m_bUserDel) return;

	//! 找到联系人组
	map<CGUID, VecPlayerLinkMan>::iterator iteOpter = m_mapPlayerLinkMan.find(OwnerGuid);
	if (m_mapPlayerLinkMan.end() == iteOpter) return;
	LinkmanGroup &OpterLinkmanGroup = (iteOpter->second)[lIdx];

	//! 得到被删除者的ID
	CGUID AimGuid = OpterLinkmanGroup.FindMember(szAimName);
	if (NULL_GUID == AimGuid) return;

	//! 删除联系人
	OpterLinkmanGroup.DelMember(AimGuid);
	tagLinkman_DB Linkman_DB;
	Linkman_DB.AimGUID = AimGuid;
	Linkman_DB.OwnerGUID = OwnerGuid;
	Linkman_DB.uGroupID = lGroupID;
	Linkman_DB.uDelState = eLDS_NotDel;
	strcpy(Linkman_DB.szAimName, szAimName);
	
	GetGameLogInterface()->logT450_linkman_opt_log(GetGame()->GetMapPlayer(OwnerGuid), AimGuid, szAimName, 2, lGroupID);

	//! 更新被删除者的[添加我的联系人]列表
	map<CGUID, VecPlayerLinkMan>::iterator iteAim = m_mapPlayerLinkMan.find(AimGuid);
	if (m_mapPlayerLinkMan.end() != iteAim) 
	{
		if(iteAim->second.size() > lIdx)
			(iteAim->second)[lIdx].DelCompriseMePlayer(OwnerGuid);
	
	}

	CPlayer *pOptPlayer = GetGame()->GetMapPlayer(OwnerGuid);
	if(NULL == pOptPlayer){assert(false); return;}
	CMessage msgDelNotify(MSG_W2S_LINKMAN_DEL_NOTIFY);
	msgDelNotify.Add(OwnerGuid);
	msgDelNotify.Add(pOptPlayer->GetName());
	msgDelNotify.Add(lGroupID);
	msgDelNotify.Add(AimGuid);
	msgDelNotify.Add(szAimName);

	msgDelNotify.SendToSocket(pOptPlayer->GetGsSocket());

	CPlayer *pAimPlayer = GetGame()->GetMapPlayer(AimGuid);
	if(NULL != pAimPlayer)
	{
		if(pAimPlayer->GetGsSocket() != pOptPlayer->GetGsSocket())
			msgDelNotify.SendToSocket(pAimPlayer->GetGsSocket());
	}

	if(eLGT_Spouse == pLinkmanGroupSetup->m_uGroupType)
		SavePlayerData(OwnerGuid);
}

//! 响应玩家请求移动消息
void LinkmanSystem::OnPlayerRequestMoveMsg(CMessage *pMsg)
{
	CGUID OwnerGuid;
	pMsg->GetGUID(OwnerGuid);
	ULONG lSrcGroupID	= pMsg->GetLong();
	ULONG lDesGroupID	= pMsg->GetLong();
	char szAimName[MAX_PLAYER_NAME_SIZE + 1] = {0};
	pMsg->GetStr(szAimName, MAX_PLAYER_NAME_SIZE);

	CMessage msgMoveNotify(MSG_W2S_LINKMAN_MOVE_NOTIFY);

	CPlayer *pOptPlayer = GetGame()->GetMapPlayer(OwnerGuid);
	if(NULL == pOptPlayer) return;

	//! 判断是否发送名字为“隐匿者”
	if(0 == strcmp(szAimName, CGlobeSetup::GetSetup()->szHideName))
	{
		msgMoveNotify.Add((LONG)FALSE);
		msgMoveNotify.Add((LONG)eOET_AimHideName);
		msgMoveNotify.Add(CGlobeSetup::GetSetup()->szHideName);
		msgMoveNotify.Add(OwnerGuid);
		msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
		return;
	}

	CPlayer *pAimPlayer = GetGame()->GetMapPlayer(szAimName);
	if(NULL == pAimPlayer)
	{
		msgMoveNotify.Add((LONG)FALSE);
		msgMoveNotify.Add((LONG)eOET_AimNotBeing);
		msgMoveNotify.Add(szAimName);
		msgMoveNotify.Add(OwnerGuid);
		msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
		return;
	}

	if( pAimPlayer->GetHideFlag() == 1 && !GetGame()->PlayerIsGM(pOptPlayer) 
		&& (pAimPlayer->GetTeamID() != pOptPlayer->GetTeamID() || pAimPlayer->GetTeamID()==NULL_GUID || pOptPlayer->GetTeamID()==NULL_GUID) ) // 隐匿者
	{
		msgMoveNotify.Add((LONG)FALSE);
		msgMoveNotify.Add((LONG)eOET_AimHideName);
		msgMoveNotify.Add(CGlobeSetup::GetSetup()->szHideName);
		msgMoveNotify.Add(OwnerGuid);
		msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
		return;
	}

	//! 操作的组
	LONG lSrcIdx = m_LGSManage.GetIndexByID(lSrcGroupID);
	LONG lDesIdx = m_LGSManage.GetIndexByID(lDesGroupID);
	const LinkmanGroupSetup* pSrcGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lSrcIdx);
	const LinkmanGroupSetup* pDesGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx);
	if(NULL == pSrcGroupSetup || NULL == pDesGroupSetup) {assert(false); return;}

	//! 操作者的联系人对象
	map<CGUID, VecPlayerLinkMan>::iterator iteOpter = m_mapPlayerLinkMan.find(OwnerGuid);
	if (m_mapPlayerLinkMan.end() == iteOpter) return;
	if(iteOpter->second.size() <= max(lSrcIdx, lDesIdx)) return;
	LinkmanGroup &OptSrcGroup = (iteOpter->second)[lSrcIdx];
	LinkmanGroup &OptDesGroup = (iteOpter->second)[lDesIdx];


	//! 先验证删除
	CGUID AimGuid;
	BOOL bRunDel = TRUE;
	if (eKW_ServerKeep != pSrcGroupSetup->m_uKeepWay)
	{
		bRunDel = FALSE;
	}
	else
	{
		if(!pSrcGroupSetup->m_bUserDel) return;//! 不允许客户端删除操作
		AimGuid = OptSrcGroup.FindMember(szAimName);
		if (NULL_GUID == AimGuid) return;//! 删除对象不在分组中
	}

	//! 再验证添加
	BOOL bRunAdd = TRUE;
	if (eKW_ServerKeep != pDesGroupSetup->m_uKeepWay)
	{
		bRunAdd = FALSE;
	}
	else
	{
		if(!pDesGroupSetup->m_bUserAdd) return; //! 不允许客户端添加操作
		for (LONG i = 0; i < (iteOpter->second).size(); ++i)
		{
			if (i != lSrcIdx)
				if (NULL_GUID != (iteOpter->second)[i].FindMember(szAimName)) return;
		}
		if (NULL_GUID != (iteOpter->second)[lDesIdx].FindMember(szAimName)) return;//! 添加对象已存在
		if ( NULL == pAimPlayer) //! 玩家不在线，找不到玩家的ID，无法进行
		{
			msgMoveNotify.Add((LONG)FALSE);
			msgMoveNotify.Add((LONG)eOET_AimNotBeing);
			msgMoveNotify.Add(szAimName);
			msgMoveNotify.Add(OwnerGuid);
			msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
			return;
		}
		if (pAimPlayer->GetExID() == OwnerGuid)//! 被添加者是操作者自己
		{
			return;
		}
		//! 人数已满
		if((iteOpter->second)[lDesIdx].GetMemberNum() >= m_LGSManage.GetLinkmanGroupSetup_ByOrder(lDesIdx)->m_uMaxMemberNum)
		{
			msgMoveNotify.Add((LONG)FALSE);
			msgMoveNotify.Add((LONG)eOET_ListFull);
			msgMoveNotify.Add(szAimName);
			msgMoveNotify.Add(OwnerGuid);
			msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
			return;
		}
		AimGuid = pAimPlayer->GetExID();
	}

	if ((!bRunDel) && (!bRunAdd)) return;//! 两个组都管理在客户端，不需要服务器执行


	//! 开始执行
	if (bRunAdd)
	{
		if (pDesGroupSetup->m_bAddAimAffirm) //! 如果要求目标确认
		{
			//! 创建会话数据
			tagOptSession OptSession;
			OptSession._uEndTime	= time(NULL) + MAX_LINKMAN_OPT_TIME_SEC;
			OptSession._uOptType	= eLot_Move;
			OptSession._uSrcGroupID	= lSrcGroupID;
			OptSession._uDesGroupID	= lDesGroupID;

			if(CreateAddSession(AimGuid, OwnerGuid, OptSession))
			{
				CMessage msgrEquestAdd(MSG_W2S_LINKMAN_PLAYER_REQUEST_ADD_NOTIFY);
				msgrEquestAdd.Add(lDesGroupID);
				msgrEquestAdd.Add(pOptPlayer->GetName());
				msgrEquestAdd.Add(OwnerGuid);
				msgrEquestAdd.Add(AimGuid);
				msgrEquestAdd.SendToSocket(pAimPlayer->GetGsSocket());
			}
			else
			{
				msgMoveNotify.Add((LONG)FALSE);
				msgMoveNotify.Add((LONG)eOET_OverfullToOne);
				msgMoveNotify.Add(szAimName);
				msgMoveNotify.Add(OwnerGuid);
				msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
			}

			return;
		}
	}

	map<CGUID, VecPlayerLinkMan>::iterator iteAim = m_mapPlayerLinkMan.find(AimGuid);
	if (m_mapPlayerLinkMan.end() == iteAim) return;
	if(iteAim->second.size() <= max(lSrcIdx, lDesIdx)) return;
	LinkmanGroup &AimSrcGroup = (iteAim->second)[lSrcIdx];
	LinkmanGroup &AimDesGroup = (iteAim->second)[lDesIdx];

	if (bRunDel)
	{
		tagLinkman_DB Linkman_DB;
		Linkman_DB.AimGUID = AimGuid;
		Linkman_DB.OwnerGUID = OwnerGuid;
		memmove(Linkman_DB.szAimName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
		Linkman_DB.uDelState = eLDS_NotDel;

		OptSrcGroup.DelMember(AimGuid);
		GetGameLogInterface()->logT450_linkman_opt_log(pOptPlayer, AimGuid, szAimName, 2, lSrcGroupID);
		AimSrcGroup.DelCompriseMePlayer(OwnerGuid);
	}

	//! 创建联系人数据对象
	tagLinkman_DB Linkman_DB;
	Linkman_DB.AimGUID = AimGuid;
	Linkman_DB.OwnerGUID = OwnerGuid;
	memmove(Linkman_DB.szAimName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	Linkman_DB.uDelState = eLDS_NotDel;
	Linkman_DB.uGroupID = lDesGroupID;

	if (bRunAdd)
	{
		OptDesGroup.AddMember(Linkman_DB);
		GetGameLogInterface()->logT450_linkman_opt_log(pOptPlayer, AimGuid, szAimName, 1, lDesGroupID);
		AimDesGroup.AddCompriseMePlayer(Linkman_DB);
	}

	//! 发送消息
	tagLinkman_data Linkman_data;
	Linkman_data.guid = AimGuid;
	memmove(Linkman_data.szName, pAimPlayer->GetName(), MAX_PLAYER_NAME_SIZE);
	LinkmanGroup::UpdateLinkman_data(&Linkman_data);

	msgMoveNotify.Add((LONG)TRUE);
	msgMoveNotify.Add(OwnerGuid);
	msgMoveNotify.Add(pOptPlayer->GetName());
	msgMoveNotify.Add(lSrcGroupID);
	msgMoveNotify.Add(lDesGroupID);
	msgMoveNotify.AddEx(&Linkman_data, sizeof(tagLinkman_data));

	msgMoveNotify.SendToSocket(pOptPlayer->GetGsSocket());
	if(pAimPlayer->GetGsSocket() != pOptPlayer->GetGsSocket())
		msgMoveNotify.SendToSocket(pAimPlayer->GetGsSocket());
}


//! 响应GS地图改变消息
void LinkmanSystem::OnGsMapChange(const CGUID &PlayerGuid, const char* pMapName)
{
	map<CGUID, VecPlayerLinkMan>::iterator itePlayerLinkMan = m_mapPlayerLinkMan.find(PlayerGuid);
	if(itePlayerLinkMan == m_mapPlayerLinkMan.end()) return;

	VecPlayerLinkMan &vPlayerLinkMan = itePlayerLinkMan->second;

	//! 发送列表
	map<LONG, vector<BYTE>> mapSendTable;

	//! 更新WS资料，统计发送列表
	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{	
		const LinkmanGroupSetup *pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay && eTIT_Show_All == pLinkmanGroupSetup->m_uTimelyInfo)
		{
			//! 统计需要登陆信息的玩家和GS服务器
			const map<CGUID, tagLinkman_DB>& AddMeMap = vPlayerLinkMan[i].GetAddMeMap();
			for (map<CGUID, tagLinkman_DB>::const_iterator ite = AddMeMap.begin(); ite != AddMeMap.end(); ++ite)
			{
				CPlayer *pPlayer = GetGame()->GetMapPlayer(ite->first);
				if (NULL != pPlayer)
				{
					Account* tLoginInfo = GetGame()->GetEntityManager()->FindAccount(pPlayer->GetAccount());

					if(!tLoginInfo)
					{
						char outStr[1024];
						sprintf(outStr, "Err OnGsMapChange: LoginInfo[%s] Is Null!", pPlayer->GetAccount());
						AddLogText(outStr);
						return;
					}
					if(tLoginInfo && PLAYER_STATE_BACKTOLOGIN != tLoginInfo->GetPlayerState())
					{
						//! 统计信息
						if (mapSendTable.find(pPlayer->GetGsSocket()) == mapSendTable.end())
						{
							mapSendTable.insert(pair<LONG, vector<BYTE>>(pPlayer->GetGsSocket(), vector<BYTE>()));
						}
						_AddToByteArray(&mapSendTable[pPlayer->GetGsSocket()], ite->first);
					}
				}
			}
		}
	}

	//! 发送通知
	for (map<LONG, vector<BYTE>>::iterator ite = mapSendTable.begin(); ite != mapSendTable.end(); ++ite)
	{
		CMessage msgToGs(MSG_W2S_LINKMAN_PLAYER_INFO_CHANGE);
		msgToGs.Add(PlayerGuid);
		msgToGs.Add((LONG)eLIT_Map);
		msgToGs.Add(pMapName);
		msgToGs.Add((LONG)ite->second.size());
		for(size_t i = 0; i < ite->second.size(); ++i)
			msgToGs.Add(ite->second[i]);
		msgToGs.SendToSocket(ite->first);
	}
}


//! 响应GS数据发送消息
void LinkmanSystem::OnGsDataSend(BYTE *pData, LONG &lPos)
{
	//! 得到玩家数量
	LONG lPlayerNum = _GetLongFromByteArray(pData, lPos);

	for (LONG lPlayerIdx = 0; lPlayerIdx < lPlayerNum; ++lPlayerIdx)
	{
		//! 得到一个玩家ID
		CGUID PlayerGuid;
		_GetBufferFromByteArray(pData, lPos, PlayerGuid);
		//! 得到联系人组数
		LONG lGroupNum = _GetLongFromByteArray(pData, lPos);
		
		//! 如果配置不一致
		if(lGroupNum != m_LGSManage.GetGroupNum())
		{
			//! 清除GS数据
			return;
		}

		//! 检测是否已有该玩家的联系人数据
		if(m_mapPlayerLinkMan.end() == m_mapPlayerLinkMan.find(PlayerGuid))
		{
			VecPlayerLinkMan tmpPlayerLinkMan;
			LONG lGroupNum = m_LGSManage.GetGroupNum();
			for (LONG i = 0; i < lGroupNum; ++i)
			{
				LinkmanGroup tmpLinkmanGroup(m_LGSManage.GetLinkmanGroupSetup_ByOrder(i));
				tmpPlayerLinkMan.push_back(tmpLinkmanGroup);
			}
			m_mapPlayerLinkMan[PlayerGuid] = tmpPlayerLinkMan;
		}

		VecPlayerLinkMan &vPlayerLinkMan = m_mapPlayerLinkMan[PlayerGuid];

		for (LONG lGorupIdx = 0; lGorupIdx < lGroupNum; ++lGorupIdx)
		{
			vPlayerLinkMan[lGorupIdx].Decode_FromGs(pData, lPos);
		}
	}
}


//! 删除一个玩家的信息
void LinkmanSystem::DelPlayerLinkman(const CGUID &PlayerGuid)
{
	map<CGUID, VecPlayerLinkMan>::iterator ite = m_mapPlayerLinkMan.find(PlayerGuid);
	if(m_mapPlayerLinkMan.end() != ite)
	{
		for (LONG i = 0; i < ite->second.size(); ++i)
		{
			ite->second[i].Release();
		}
		m_mapPlayerLinkMan.erase(PlayerGuid);
	}
}

//! 发送玩家转GS通知
void LinkmanSystem::OnPlayerChangeGs(CPlayer *pPlayer)
{
	map<CGUID, VecPlayerLinkMan>::iterator iteLinkman = m_mapPlayerLinkMan.find(pPlayer->GetExID());
	if (m_mapPlayerLinkMan.end() == iteLinkman) return;

	CMessage msgInit(MSG_W2S_LINKMAN_PLAYER_INITDATA);
	msgInit.Add((BYTE)FALSE);
	msgInit.Add(pPlayer->GetExID());
	VecPlayerLinkMan &vPlayerLinkMan = iteLinkman->second;

	for (LONG i = 0; i < vPlayerLinkMan.size(); ++i)
	{
		const LinkmanGroupSetup* pLinkmanGroupSetup = m_LGSManage.GetLinkmanGroupSetup_ByOrder(i);
		if (eKW_ServerKeep == pLinkmanGroupSetup->m_uKeepWay)
		{
			//! 获取发送到玩家所在GS服务器的初始化数据
			vPlayerLinkMan[i].Serialize_ForGs(&msgInit);
		}
	}
	//! 发送初始化数据
	msgInit.SendToSocket(pPlayer->GetGsSocket());
}

//!-----------------------------------------------------------------------------------------------

//! 创建
BOOL LinkmanSystem::CreateAddSession(const CGUID& OwnerGuid, const CGUID& FlagGuid, tagOptSession &OptSession)
{
	if (m_Session.find(OwnerGuid) == m_Session.end())
	{
		map<CGUID, tagOptSession> FlagMap;
		m_Session.insert(pair<CGUID, map<CGUID, tagOptSession>>(OwnerGuid, FlagMap));
	}

	BOOL bRe = FALSE;
	BOOL bRunClear = FALSE;

	map<CGUID, tagOptSession> &FlagMap = m_Session[OwnerGuid];
	LONG lNow = time(NULL);

	map<CGUID, tagOptSession>::iterator iteOptSession = FlagMap.find(FlagGuid);
	if (iteOptSession != FlagMap.end())
	{
		if(iteOptSession->second._uEndTime < lNow )
		{
			bRunClear = TRUE;
			bRe = TRUE;
		}
	}
	else
	{
		bRe = TRUE;
	}

	if (bRunClear || FlagMap.size() > 20)
	{
		ClearOutTimeSession(FlagMap);
	}

	if (bRe)
	{
		FlagMap[FlagGuid] = OptSession;
	}


	return bRe;
}

//! 移除一个玩家的所有会话
void LinkmanSystem::EndAddSession(const CGUID& OwnerGuid)
{
	m_Session.erase(OwnerGuid);
}

//! 移除两个玩家之间的会话
BOOL LinkmanSystem::EndAddSession(const CGUID& OwnerGuid, const CGUID& FlagGuid, tagOptSession &OptSession)
{
	map<CGUID, map<CGUID, tagOptSession>>::iterator ite = m_Session.find(OwnerGuid);
	if (ite != m_Session.end())
	{
		map<CGUID, tagOptSession>::iterator iteChild = (*ite).second.find(FlagGuid);
		if (iteChild != (*ite).second.end())
		{
			BOOL bRe = FALSE;
			if(iteChild->second._uEndTime > time(NULL))
			{
				if (iteChild->second._uSrcGroupID == OptSession._uSrcGroupID &&
					iteChild->second._uDesGroupID == OptSession._uDesGroupID &&
					iteChild->second._uOptType == OptSession._uOptType)
				{
					bRe = TRUE;
				}
			}

			OptSession._uSrcGroupID = iteChild->second._uSrcGroupID;
			OptSession._uDesGroupID = iteChild->second._uDesGroupID;
			if (!bRe)
			{
				OptSession._uOptType = iteChild->second._uOptType;
			}

			(*ite).second.erase(iteChild);
			return bRe;
		}	
	}
	return FALSE;
}

//! 清除一个玩家的超时会话
void LinkmanSystem::ClearOutTimeSession(map<CGUID, tagOptSession> &PlayerSession)
{
	LONG lNow = time(NULL);
	map<CGUID, tagOptSession>::iterator ite = PlayerSession.begin();
	while(ite != PlayerSession.end())
	{
		if(lNow > ite->second._uEndTime)
		{
			ite = PlayerSession.erase(ite);
			continue;
		}
		++ite;
	}
}

void LinkmanSystem::OutLinkmanInfo(VOID)
{
	char szTmp[1204] = {0};
	
	LONG lPlayerNum = (LONG)m_mapPlayerLinkMan.size();
	LONG lMeAddNum = 0;
	LONG lAddMeNum = 0;

	if(6000 > lPlayerNum)
	{
		map<CGUID, VecPlayerLinkMan>::iterator ite = m_mapPlayerLinkMan.begin();
		for (; m_mapPlayerLinkMan.end() != ite; ++ite)
		{
			for (size_t i = 0; i < ite->second.size(); ++i)
			{
				lMeAddNum += (LONG)ite->second[i].GetMeAddMap().size();
				lAddMeNum += (LONG)ite->second[i].GetAddMeMap().size();
			}
		}
	}
	else
	{
		lMeAddNum = -1;
		lAddMeNum = -1;
	}

	sprintf(szTmp, "LinkmanPlayerNum = %d\r\nLinkmanMeAddNum = %d\r\nLinkmanAddMeNum = %d\r\n\r\n", lPlayerNum, lMeAddNum, lAddMeNum);
	PutStringToFile("MemAllocInfo", szTmp);
}

//! 加入缓存
void LinkmanSystem::PushCache(const CGUID &PlayerGuid)
{
	m_TimeoutQueue.push_back(PlayerGuid, tagQueueValue(PlayerGuid, (DWORD)time(NULL) + 60 * 15));
}

//! 清除超时数据
void LinkmanSystem::ClearTimeoutData(void)
{
	DWORD CurrTime = (DWORD)time(NULL);
	tagQueueValue *pQueueValue = &m_TimeoutQueue.get_front();
	DWORD OutTime = pQueueValue->Timeout;

	while (CurrTime > OutTime && 0 != OutTime)
	{
		DelPlayerLinkman(pQueueValue->PlayerGuid);
		m_TimeoutQueue.pop_front();

		pQueueValue = &m_TimeoutQueue.get_front();
		OutTime = pQueueValue->Timeout;
	}
}

/*
<!--联系人操作日志-->
<Table name="t450_linkman_opt_log" log_no="50" log_pos="1">
<item type="client_head"     head_name=""></item>
<item type="player"   head_name="opter"></item><!--操作者-->
<item type="player"   head_name="aim"></item><!--操作目标-->
<item type="alone"    Item_name="opt_type"      data_type="int32"></item><!--操作类型：1=添加、2=删除-->
<item type="alone"    Item_name="group_id"      data_type="int32"></item><!--所操作的联系人组的ID-->
</Table>
*/
bool LogicLogInterface::logT450_linkman_opt_log(CPlayer *pOpter, const CGUID &AimGuid, const char *pAimName, long lOptType, long lGroupID)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)return false;
	if (NULL == pOpter || NULL == pAimName) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		450, strTime.c_str(),
		&(pOpter->GetExID()),	pOpter->GetName(),
		&AimGuid,				pAimName,
		lOptType,				lGroupID
		);
}