//GlobalRgnManager.h/////////////////////////////////////////////////////////////////////
//对象类:该对象管理所有的场景对象实例
//Author:安海川
//Create Time:2008.11.03
#include "stdafx.h"
#include "GlobalRgnManager.h"
#include "WorldRegion.h"
#include "../nets/networld/message.h"
#include "script/VariableList.h"
#include "DBentity/entityManager.h"
#include "../public/strtype.h"
#include "../public/StrUnifyInput.h"
#include "../../setup/RegionPlugSetup.h"

extern HWND g_hWnd;

CGlobalRgnManager::CGlobalRgnManager()
{
	m_mapRgn.clear();
}
CGlobalRgnManager::~CGlobalRgnManager()
{
	MapRgnItr itr = m_mapRgn.begin();
	for(; itr != m_mapRgn.end(); itr++)
	{
		MP_DELETE(itr->second);
	}
	m_mapRgn.clear();

	s_mapRegionList.clear();

	// Personal Rgn
	map<long, tagRegion>::iterator tRgnItr = s_mapPersonalRgnList.begin();
	for(; tRgnItr != s_mapPersonalRgnList.end(); tRgnItr++)
	{
		MP_DELETE(tRgnItr->second.pRegion);
	}
	s_mapPersonalRgnList.clear();

	// Personal House Rgn
	tRgnItr = s_mapPersonalHouseRgnList.begin();
	for(; tRgnItr != s_mapPersonalHouseRgnList.end(); tRgnItr++)
	{
		MP_DELETE(tRgnItr->second.pRegion);
	}
	s_mapPersonalHouseRgnList.clear();

	// Team Rgn
	tRgnItr = s_mapTeamRgnList.begin();
	for(; tRgnItr != s_mapTeamRgnList.end(); tRgnItr++)
	{
		MP_DELETE(tRgnItr->second.pRegion);
	}
	s_mapTeamRgnList.clear();
}

//! 运行接口
void CGlobalRgnManager::Run(void)
{

}
//! 读取Ini配置文件数据
bool CGlobalRgnManager::LoadRgnSetupIni(const char* strFilePath)
{
	char str[256];
	string strTemp;

	CRFile* prfile = rfOpen(strFilePath);
	if( prfile )
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		extern long g_lTotalMonster;
		long lM=0;

		// 读场景信息
		tagRegion stRegion;
		DWORD dwID = 0;
		DWORD dwResourceID = 0;
		long lSvrResourceID = 0;
		DWORD dwAreaId=0;
		float fExpScale = 1.0f;
		float fOccuExpScale = 1.0f;
		float fSpScale = 1.0f;
		float fMeriScale = 1.0f;
		DWORD dwWarType = RWT_Normal;
		long  nRide = 1;
		long nChangeEquip = 1;
		int nNoPk = 0;
		int nNoContribute = 0;
		string strName;
		DWORD dwIndex = 0;
		int btCountry = 0;
		long lNotify= 0;
		int nSpaceType=0;

		// 分线ID,0:表示不是分线场景,>0表示是分线场景的ID
		long lLinedIdFlag = 0;
		long lACRgnType = 0;
		int lRgnHideFlag = 0;
		//  [8/26/2009 chenxianj]
		int nReliveFlag = 0;
		long nDeadReturn = 0;
		long lStrongPointFlag=0;
        int  nCommonHookFlag = 0;
        int  nStableHookFlag = 0;
        //家族成员能否退出、家族解散、成员邀请等家族成员变动的操作标识，1不能，0能
        int  nChangeFactionMemberFlag = 0;

		DWORD dupType = 0;
		while(ReadTo(stream, "#"))
		{
			string szGuid;
			stream >> dupType 
				>> dwID 
				>> dwResourceID 
				>> lSvrResourceID
				>> dwAreaId 
				>> fExpScale 
				>> fOccuExpScale 
				>> fSpScale 
				>> fMeriScale 
				>> dwWarType
				>> nRide
				>> nChangeEquip
				>> nNoPk 
				>> nNoContribute 
				>>strName 
				>> dwIndex 
				>> btCountry 
				>> lNotify
				>> nSpaceType 
				>> szGuid 
				>> lLinedIdFlag
				>> lACRgnType
				>> lRgnHideFlag
			    >> nReliveFlag
				>> nDeadReturn
				>> lStrongPointFlag
                >> nCommonHookFlag
                >> nStableHookFlag
                >> nChangeFactionMemberFlag;
			stRegion.dwGameServerIndex = dwIndex;
			stRegion.RegionType = (eRgnType)dupType;

			CGUID rgnGuid(szGuid.c_str());

			// 读入场景
			CWorldRegion* pRegion = MP_NEW CWorldRegion;

			if(pRegion)
			{
				pRegion->SetRgnType(dupType);
				pRegion->SetAreaId(dwAreaId);
				pRegion->SetExID(rgnGuid);
				pRegion->SetID(dwID);
				pRegion->SetName(strName.c_str());
				pRegion->SetWarType((eRWT)dwWarType);
				pRegion->SetSpaceType((eRST)nSpaceType);
				pRegion->SetResourceID(dwResourceID);
				pRegion->SetSvrResourceID( lSvrResourceID );
				pRegion->SetExpScale(fExpScale);
				pRegion->SetOccuExpScale(fOccuExpScale);
				pRegion->SetSpScale(fSpScale);
				pRegion->SetMeriScale(fMeriScale);
				pRegion->SetCountry(btCountry);
				pRegion->SetNotify(lNotify);
				pRegion->SetLinedIdFlag(lLinedIdFlag);
				pRegion->SetRide(nRide==0?false:true);
				pRegion->SetChangeEquip(nChangeEquip==0?false:true);
				pRegion->SetRelive(nReliveFlag==0?false:true);
				pRegion->SetDeadReturn(nDeadReturn==0?false:true);
				if(lLinedIdFlag != 0) // 是分线场景ID
				{
					m_mapLinedRgnIdByLinedId[lLinedIdFlag].push_back(dwID);
				}
				pRegion->SetACRgnType( lACRgnType );
				pRegion->SetRgnHideFlag(lRgnHideFlag);

				pRegion->SetNoPk( nNoPk);
				pRegion->SetNoContribute( nNoContribute==0?false:true );
				pRegion->SetGsid(dwIndex);
				pRegion->SetStrongPointFlag(lStrongPointFlag);

                //普通挂机和稳定挂机Flag;
                pRegion->SetCommonHook(nCommonHookFlag==0?false:true);
                pRegion->SetStableHook(nStableHookFlag==0?false:true);

                //家族成员能否变动标识 1:不能  0:能
                pRegion->SetNoChangeFactionMemberFlag(nChangeFactionMemberFlag==0?false:true);

				if( pRegion->Load() )
				{
					stRegion.pRegion = pRegion;
			
					//!!!GS的NormalRgn使用模板的NormalRgn对象指针!!!
					switch((eRgnType)dupType) 
					{
					case RGN_NORMAL:// Normal Rgn
						{
							// 加入模板map
							s_mapRegionList[dwID] = stRegion;
	
							pRegion->SetGsid(dwIndex);

							// 加入RgnMap
							MapRgnItr rgnItr = m_mapRgn.find(rgnGuid);
							if(rgnItr == m_mapRgn.end())// 未找到
							{
								m_mapRgn[rgnGuid] = pRegion;
							}
							else
							{
								char szOut[1024];
								sprintf(szOut, "严重错误!读取Rgn数据时发现RegionList.ini配置场景数据的GUID[%s]有重复!", szGuid.c_str());
								AddLogText(szOut);
							}

							// 设置类型
							pRegion->SetRgnType((long)dupType);
							pRegion->SetIsTemplateRgnFlag(true);

							sprintf(str, "普通场景模板 (%d) %s [m=%d]...ok!", dwID, strName.c_str(), g_lTotalMonster-lM);
							AddLogText(str);
						}
						break;
					case RGN_PERSONAL: // Personal Rgn
						{
							map<long, tagRegion>::iterator rgnItr = s_mapPersonalRgnList.find(dwID);
							if(rgnItr != s_mapPersonalRgnList.end())
							{
								char szOut[1024];
								sprintf(szOut, "严重错误!case RGN_PERSONAL，场景数据的GUID[%s]有重复!", szGuid.c_str());
								AddLogText(szOut);
							}
							// 加入模板map
							s_mapPersonalRgnList[dwID] = stRegion;

							// 设置类型
							pRegion->SetRgnType((long)dupType);
							pRegion->SetIsTemplateRgnFlag(true);

							sprintf(str, "个人副本场景模板 (%d) %s [m=%d]...ok!", dwID, strName.c_str(), g_lTotalMonster-lM);
							AddLogText(str);

						}
						break;
					case RGN_PERSONAL_HOUSE: // Personal Rgn
						{
							map<long, tagRegion>::iterator rgnItr = s_mapPersonalHouseRgnList.find(dwID);
							if(rgnItr != s_mapPersonalHouseRgnList.end())
							{
								char szOut[1024];
								sprintf(szOut, "严重错误!case RGN_PERSONAL_HOUSE，场景数据的GUID[%s]有重复!", szGuid.c_str());
								AddLogText(szOut);
							}
							// 加入模板map
							s_mapPersonalHouseRgnList[dwID] = stRegion;

							// 设置类型
							pRegion->SetRgnType((long)dupType);
							pRegion->SetIsTemplateRgnFlag(true);

							sprintf(str, "个人房屋副本场景模板 (%d) %s [m=%d]...ok!", dwID, strName.c_str(), g_lTotalMonster-lM);
							AddLogText(str);

						}
						break;
					case RGN_TEAM: // Personal Rgn
						{
							map<long, tagRegion>::iterator rgnItr = s_mapTeamRgnList.find(dwID);
							if(rgnItr != s_mapTeamRgnList.end())
							{
								char szOut[1024];
								sprintf(szOut, "严重错误!case RGN_TEAM，场景数据的GUID[%s]有重复!", szGuid.c_str());
								AddLogText(szOut);
							}
							// 加入模板map
							s_mapTeamRgnList[dwID] = stRegion;

							// 设置类型
							pRegion->SetRgnType((long)dupType);
							pRegion->SetIsTemplateRgnFlag(true);

							sprintf(str, "队伍副本场景模板 (%d) %s [m=%d]...ok!", dwID, strName.c_str(), g_lTotalMonster-lM);
							AddLogText(str);

						}
						break;
					default:
						{
							AddLogText("严重错误!未知类型场景");
						}
						break;
					}
									
					lM = g_lTotalMonster;
				}
				else
				{
					sprintf(str, "场景模板 (%d) %s...Load failed!", dwID, strName.c_str());
					AddLogText(str);
				}
			}
			else
			{
				sprintf(str, "场景 (%d) %s... Read Setup failed!", dwID, strName.c_str());
				AddLogText(str);
			}
		}

		sprintf(str, "游戏世界共计 Monster=%d!", g_lTotalMonster);
		AddLogText(str);
	}
	else
	{
		char str[256];
		sprintf(str, "Can't find file %s", strFilePath);
		return false;
	}

	return true;
}
//! 计算当前GS该类型副本是否达到上限,返回有效GS的gsid
long CGlobalRgnManager::GetCurGsDupRgnNumIsMax(eRgnType rgnType)
{
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			for(int i=0; i<m_vecFirstPersonalServer.size(); i++)
			{	
				CGame::tagGameServer* gsInfo = GetGame()->GetGameServer(m_vecFirstPersonalServer[i]);
				if(gsInfo != NULL && gsInfo->bConnected == true)
				{
					// 判断负载
					long dupRgnNum = 0;
					MapRgnItr gsRgnItr = m_mapRgn.begin();
					for(; gsRgnItr != m_mapRgn.end(); gsRgnItr++)
					{
						if(gsRgnItr->second->GetGsid() == m_vecFirstPersonalServer[i] && gsRgnItr->second->GetRgnType() == rgnType)
						{
							dupRgnNum++;
						}
					}

					// (负载)玩家数量和monster数量超过限制 重新选择一个合适的GS
					if( dupRgnNum >= CGlobeSetup::GetSetup()->lPDupRgnMaxNum )
					{
						continue;
					}
					else
						return m_vecFirstPersonalServer[i];
				}
			}
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			for(int i=0; i<m_vecFirstPersonalHouseServer.size(); i++)
			{	
				CGame::tagGameServer* gsInfo = GetGame()->GetGameServer(m_vecFirstPersonalHouseServer[i]);
				if(gsInfo != NULL && gsInfo->bConnected == true)
				{
					// 判断负载
					long dupRgnNum = 0;
					MapRgnItr gsRgnItr = m_mapRgn.begin();
					for(; gsRgnItr != m_mapRgn.end(); gsRgnItr++)
					{
						if(gsRgnItr->second->GetGsid() == m_vecFirstPersonalHouseServer[i] && gsRgnItr->second->GetRgnType() == rgnType)
						{
							dupRgnNum++;
						}
					}

					// (负载)玩家数量和monster数量超过限制 重新选择一个合适的GS				
					if( dupRgnNum >= CGlobeSetup::GetSetup()->lTDupRgnMaxNum )
					{
						continue;
					}
					else
						return m_vecFirstPersonalHouseServer[i];
				}
			}
		}
		break;
	case RGN_TEAM:
		{
			for(int i=0; i<m_vecFirstTeamServer.size(); i++)
			{	
				CGame::tagGameServer* gsInfo = GetGame()->GetGameServer(m_vecFirstTeamServer[i]);
				if(gsInfo != NULL && gsInfo->bConnected == true)
				{
					// 判断负载
					long dupRgnNum = 0;
					MapRgnItr gsRgnItr = m_mapRgn.begin();
					for(; gsRgnItr != m_mapRgn.end(); gsRgnItr++)
					{
						if(gsRgnItr->second->GetGsid() == m_vecFirstTeamServer[i] && gsRgnItr->second->GetRgnType() == rgnType)
						{
							dupRgnNum++;
						}
					}

					// (负载)玩家数量和monster数量超过限制 重新选择一个合适的GS				
					if( dupRgnNum >= CGlobeSetup::GetSetup()->lTDupRgnMaxNum )
					{
						continue;
					}
					else
						return m_vecFirstTeamServer[i];
				}
			}
		}
		break;
	}
	
	return -1;
}
//! 创建指定GS的Rgn
bool CGlobalRgnManager::CreateGSRgn(long templateRgnID, eRgnType rgnType, const CGUID& ownerID, const CGUID& scriptID/*挂起的脚本ID*/, long pMsgGSID, const char* szVarName, const char* szRgnIDName, bool isDupRgnDataFlag)
{
	long retFlag = INVILID_VALUE;

	CGUID rgnID = NULL_GUID;

	CWorldRegion* retRgn = NULL;

	// 最终生成副本的GSID
	long GSID = pMsgGSID;

	// 先检查能否在优先服务器创建副本
	GSID = GetCurGsDupRgnNumIsMax(rgnType);

	// 所有服务器已满载
	if(GSID == -1)
	{
		CMessage msg(MSG_W2S_RGN_CREATE);
		//msg.Add(GSID);
		msg.Add(templateRgnID);
		msg.Add((long)rgnType);
		msg.Add(ownerID);
		msg.Add(NULL_GUID);
		msg.Add((long)C_RET_ALL_RGNS_FULL_ERROR); // 所有服务器已满载
		msg.Add((long)GSID); //  服务器ID
		msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
		msg.Add((long)0); // 场景数据长度
		msg.Add(scriptID);
		if(szVarName)
			msg.Add(szVarName);  // 异步操作返回值变量名
		else
			msg.Add("");
		if(szRgnIDName)
			msg.Add(szRgnIDName);  // 异步操作返回值变量名
		else
			msg.Add("");
		msg.Add((BYTE)isDupRgnDataFlag);// 是否有缓冲数据
		msg.SendGSID(pMsgGSID);	

		retFlag = C_RET_ALL_RGNS_FULL_ERROR;
		return false;
	}
				
	CWorldRegion* wRgn = NULL;
	CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
	if(owner == NULL)
	{
		CMessage msg(MSG_W2S_RGN_CREATE);
		//msg.Add(GSID);
		msg.Add(templateRgnID);
		msg.Add((long)rgnType);
		msg.Add(ownerID);
		msg.Add(NULL_GUID);
		msg.Add((long)C_RET_NOT_FIND_OWNER_ERROR); // 未找到玩家
		msg.Add((long)GSID); //  服务器ID
		msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
		msg.Add((long)0); // 场景数据长度
		msg.Add(scriptID);
		if(szVarName)
			msg.Add(szVarName);  // 异步操作返回值变量名
		else
			msg.Add("");
		if(szRgnIDName)
			msg.Add(szRgnIDName);  // 异步操作返回值变量名
		else
			msg.Add("");
		msg.Add((BYTE)isDupRgnDataFlag);// 是否有缓冲数据
		msg.SendGSID(pMsgGSID);	

		retFlag = C_RET_NOT_FIND_OWNER_ERROR;
		return false;
	}
	wRgn = CreateRgn(isDupRgnDataFlag, pMsgGSID, templateRgnID, rgnType, GSID, ownerID, scriptID, NULL_GUID, szVarName, szRgnIDName); // 创建Rgn 并向GS回送消息
	return false;
}

CWorldRegion* CGlobalRgnManager::GetWorldRegionByRgnId(long lId)
{
	map<long, tagRegion>::iterator it=s_mapRegionList.find(lId);	
	if(it!=s_mapRegionList.end())
	{
		return it->second.pRegion;
	}
	return NULL;
}

//! 删除指定GS的Rgn
bool CGlobalRgnManager::DeleteGSRgn(eRgnType rgnType, const CGUID& ownerID, const CGUID& rgnID, const CGUID& scriptID/*脚本ID*/, long pMsgGSID, const char* szRetVarName, const char* szRgnIDName)
{
	DeleteRgn(rgnType, ownerID, rgnID, scriptID, szRetVarName, szRgnIDName); // 删除Rgn 并向GS回送消息
	return true;
}
//! 查找GSID by RgnGUID
long CGlobalRgnManager::FindGSIDByRgnID(const CGUID& ownerID)
{
	long ret = -1;

	MapRgnItr itr = m_mapRgn.find(ownerID);
	if(itr != m_mapRgn.end())
	{
		ret = itr->second->GetGsid();
	}

	return ret;
}


//! 查找Region
CWorldRegion* CGlobalRgnManager::FindRgnByGUID(const CGUID& ownerID)
{
	CWorldRegion* ret = NULL;

	MapRgnItr itr = m_mapRgn.find(ownerID);
	if(itr != m_mapRgn.end())
	{
		ret = itr->second;
	}

	return ret;
}
//根据地图名字得到tagRegion指针
CGlobalRgnManager::tagRegion* CGlobalRgnManager::GetTemplateNormalRegion(const char* strName)
{
	map<long,tagRegion>::iterator RegionIter = s_mapRegionList.begin();
	for(; RegionIter != s_mapRegionList.end(); RegionIter++)
	{
		if(strcmp(RegionIter->second.pRegion->GetName(),strName)==0)
		{
			return &(RegionIter->second);
		}
	}
	return NULL;
}
//! 组装分线场景数据
bool CGlobalRgnManager::AddLinedRgnInfoToByteArray(vector<BYTE>& pBA)
{
	_AddToByteArray(&pBA, (long)m_mapLinedRgnIdByLinedId.size());
	map<long, vector<long>>::iterator itr = m_mapLinedRgnIdByLinedId.begin();
	for(; itr != m_mapLinedRgnIdByLinedId.end(); itr++)
	{
		_AddToByteArray(&pBA, itr->first);
		_AddToByteArray(&pBA, (long)itr->second.size());
		vector<long>::iterator templateIdItr = itr->second.begin();
		for(; templateIdItr != itr->second.end(); templateIdItr++)
		{
			// template id
			_AddToByteArray(&pBA, (*templateIdItr));
			// rgn guid
			tagRegion* pTRgn = GetTemplateNormalRegion((*templateIdItr));
			if( pTRgn && pTRgn->pRegion )
				_AddToByteArray(&pBA, pTRgn->pRegion->GetExID());
			else
				_AddToByteArray(&pBA, NULL_GUID);
		}
	}
	return true;
}
BOOL CGlobalRgnManager::LoadRegionPlugSetupIni(const char* szPath)
{
	return CRegionPlugSetup::LoadRegionPlugSetup(szPath);
}
// 读取副本创建优先服务器配置文件
void CGlobalRgnManager::LoadDupRgnServerIni(const char* filename)
{
	//////////////////////////////////////////////////////////////////////////
	// 从 data\duprgnserver.ini 读取
	//////////////////////////////////////////////////////////////////////////

	CRFile *prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	m_vecFirstPersonalHouseServer.clear();
	m_vecFirstPersonalServer.clear();
	m_vecFirstTeamServer.clear();

	string strTemp;
	string strSetupBegin;
	string strSetupEnd;
	string strServerType;
	long tmpServerID;
	// 先读取到第一个Begin
	ReadTo(stream, "<begin>");
	while(1)
	{
		stream >> strSetupEnd;
		if (strSetupEnd=="<end>" || stream.eof())
			break;

		// 读取到服务器类型定义
		if (strSetupEnd=="*")
		{
			stream >> strServerType;
		}

		// 个人房屋副本
		if(strServerType == "PersonalHouseRgnServerID")
		{
			while(1)
			{
				stream >> strSetupEnd;
				if (strSetupEnd=="<phend>" || stream.eof())
					break;

				if (strSetupEnd=="#")
				{	
					stream >> tmpServerID;
					m_vecFirstPersonalHouseServer.push_back(tmpServerID);
				}
			}
		}
		// 个人副本
		if(strServerType == "PersonalRgnServerID")
		{
			while(1)
			{
				stream >> strSetupEnd;
				if (strSetupEnd=="<pend>" || stream.eof())
					break;
				if (strSetupEnd=="#")
				{
					stream >> tmpServerID;
					m_vecFirstPersonalServer.push_back(tmpServerID);
				}
			}
		}
		// 组队副本
		if(strServerType == "TeamRgnServerID")
		{
			while(1)
			{
				stream >> strSetupEnd;
				if (strSetupEnd=="<tend>" || stream.eof())
					break;
				if (strSetupEnd=="#")
				{
					stream >> tmpServerID;
					m_vecFirstTeamServer.push_back(tmpServerID);
				}
			}
		}
	}

	return;
}

//! 创建Rgn,向GS发送RGN_CREATE消息
CWorldRegion* CGlobalRgnManager::CreateRgn(bool isDupRgnDataFlag, long srcGSID/*发送创建请求的GSID*/, long templateRgnID, eRgnType rgnType, long GSID, const CGUID& ownerID, const CGUID& scriptID, const CGUID& dbRgnGUID,
										   const char* szRetVarName, const char* szRetRgnIDName/*异步返回值变量名*/)
{
	CWorldRegion* ret = NULL;
	CMessage msg(MSG_W2S_RGN_CREATE);
	msg.Add(templateRgnID); // 模板Rgn的ID
	msg.Add((long)rgnType); // 模板Rgn的Type
	msg.Add(ownerID);		// 所有者的GUID

	CGUID rgnID;
	if(dbRgnGUID == NULL_GUID)
		CGUID::CreateGUID(rgnID);
	else rgnID = dbRgnGUID;
	map<long, CGlobalRgnManager::tagRegion>::iterator tRgnItr;

	bool IsFindTemplateRgnFlag = true;
	switch(rgnType)
	{
	case RGN_PERSONAL:
		{
			tRgnItr = GetTemplatePersonalRgnListMap()->find(templateRgnID);
			if(tRgnItr == GetTemplatePersonalRgnListMap()->end())
				IsFindTemplateRgnFlag = false;
		}
		break;
	case RGN_PERSONAL_HOUSE:
		{
			tRgnItr = GetTemplatePersonalHouseRgnListMap()->find(templateRgnID);
			if(tRgnItr == GetTemplatePersonalHouseRgnListMap()->end())
				IsFindTemplateRgnFlag = false;
		}
		break;
	case RGN_TEAM:
		{
			tRgnItr = GetTemplateTeamRgnListMap()->find(templateRgnID);
			if(tRgnItr == GetTemplateTeamRgnListMap()->end())
				IsFindTemplateRgnFlag = false;
		}
		break;
	}
	if(IsFindTemplateRgnFlag == false)
	{
		msg.Add(rgnID); // RgnID
		msg.Add((long)C_RET_RGN_TEMPLAYTEID_ERROR);	    // 未找到该模板RGN

		msg.Add((long)0); // srcgsid
		msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
		msg.Add((long)0); // bufsize

		msg.Add(scriptID);
		if(szRetVarName)
			msg.Add(szRetVarName);
		else
			msg.Add("");
		if(szRetRgnIDName)
			msg.Add(szRetRgnIDName);
		else
			msg.Add("");
		msg.SendGSID(srcGSID);
		return NULL;
	}

	// 取得模板Rgn
	CWorldRegion* worldRgn = NULL;
	
	bool canRgnCreated = true;
	CPlayer* pPlayer = GetGame()->GetMapPlayer(ownerID);
	if(pPlayer)
	{
		switch(rgnType)
		{
		case RGN_PERSONAL_HOUSE:
			{
				if(pPlayer->GetPersonalHouseRgnGUID() != NULL_GUID && rgnType == RGN_PERSONAL_HOUSE)
					canRgnCreated = false;
			}
			break;
		case RGN_PERSONAL:
			{
			}
			break;
		case RGN_TEAM:
			{
			}
			break;
		default:
			break;
		}
		
	}

	// 个人副本未创建
	if(canRgnCreated)
	{
		// 添加该副本到管理器
		MapRgnItr rgnItr = m_mapRgn.find(rgnID);
		if(rgnItr == m_mapRgn.end())
		{
			if(rgnType == RGN_PERSONAL)
			{
				pPlayer->SetPersonalRgnGUID(rgnID);
				AddPersonalRgnGUID(ownerID, rgnID);
			}

			worldRgn = MP_NEW CWorldRegion;	
			worldRgn->CloneData(tRgnItr->second.pRegion);
			if(!isDupRgnDataFlag) // 没有缓冲数据
			{
			worldRgn->New();
			worldRgn->CloneAllData(tRgnItr->second.pRegion);
			}
			// 初始化该Rgn
			worldRgn->SetExID(rgnID);
			// 设置类型
			worldRgn->SetRgnType((long)rgnType);
			m_mapRgn[rgnID] = worldRgn;
			ret = worldRgn;
			if(worldRgn)
				worldRgn->SetGsid(GSID);
			msg.Add(rgnID); // RgnID
			if(srcGSID == GSID)
				msg.Add((long)C_RET_LOCAL_SUCCESS);	    // 已经创建标志
			else
				msg.Add((long)C_RET_REMOTE_SUCCESS); // 已经创建标志, 需要WSID向srcGSID发送回应消息

			msg.Add(srcGSID); // SRC GSID

			vector<BYTE> pRgnData;
			if(!isDupRgnDataFlag) // 没有缓冲数据
			{
			worldRgn->AddToByteArray(&pRgnData);
			}
			msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
			msg.Add((long)pRgnData.size());
			if(pRgnData.size())
				msg.AddEx(&pRgnData[0], (long)pRgnData.size());

			worldRgn->SetOwnerGUID(ownerID);
			if(pPlayer)
				pPlayer->SetPersonalHouseRgnGUID(worldRgn->GetExID());
		}
		else
		{
			msg.Add(rgnID); // RgnID
			msg.Add((long)C_RET_RGN_HAS_CREATED_ERROR);	    // 已经创建标志
			msg.Add(srcGSID); // SRC GSID
			msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
			msg.Add((long)0); // rgn data
		}
	}
	else
	{
		msg.Add(rgnID); // RgnID
		msg.Add((long)C_RET_RGN_HAS_CREATED_ERROR);	    // 已经创建标志
		msg.Add(srcGSID); // SRC GSID
		msg.Add((BYTE)isDupRgnDataFlag); // 缓冲数据标记
		msg.Add((long)0); // rgn data
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

	return ret;
}

//! 删除Rgn,向WS发送RGN_DELETE消息
void CGlobalRgnManager::DeleteRgn(eRgnType rgnType, const CGUID&ownerID, const CGUID& RgnID, const CGUID& scriptID, const char* szRetVarName, const char* szRetRgnIDName/*异步返回值变量名*/)
{
	char logStr[2048];
	CGUID RgnOwnerID;

	long GSID = -1;

	CMessage msg(MSG_W2S_RGN_DELETE);

	msg.Add((long)rgnType); // 模板Rgn的Type
	msg.Add(ownerID);
	msg.Add(RgnID); // Rgn ID

	// 删除该副本从管理器
	MapRgnItr rgnItr = m_mapRgn.find(RgnID);
	if(rgnItr != m_mapRgn.end())
	{
		RgnOwnerID = rgnItr->second->GetOwnerGUID();
		GSID = rgnItr->second->GetGsid();

		MP_DELETE(rgnItr->second);
		m_mapRgn.erase(rgnItr);
		msg.Add((BYTE)1); // ok
	}
	else
	{
		msg.Add((BYTE)0); // fail
		char szGUID[128];
		RgnID.tostring(szGUID);
		char szScriptID[128];
		scriptID.tostring(szScriptID);
		sprintf(logStr, CStringReading::LoadString(IDS_WS_RGNMANAGER, STR_WS_RGNMANAGER_DELETERGN),
			szGUID, szScriptID, GSID, 0, -1);
		PutStringToFile("Login_WS_Info",logStr);
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

	CPlayer* owner = GetGame()->GetMapPlayer(ownerID);
	if(owner)
	{
		switch(rgnType)
		{
		case RGN_PERSONAL:
			{
				msg.SetType(MSG_W2S_RGN_DELETE);
				// 是删除的当前场景
				if(RgnID == owner->GetPersonalRgnGUID())
				{
					owner->SetPersonalRgnGUID(NULL_GUID);
					DelPersonalRgnGUID(ownerID);
				}
			}
			break;
		case RGN_PERSONAL_HOUSE:
			{
				msg.SetType(MSG_W2S_RGN_DELETE);
				// 是删除的当前场景
				if(RgnID == owner->GetPersonalHouseRgnGUID())
					owner->SetPersonalHouseRgnGUID(NULL_GUID);

				// 发送数据库删除DBDupRgn消息
				SendDelPHDupRgnMsgToDB(RgnID);
			}
			break;
		case RGN_TEAM:
			{
				msg.SetType(MSG_W2S_RGN_DELETE);
				// 是删除的当前场景
				GetGame()->GetGlobalRgnManager()->DelTeamRgnGUID(owner->GetExID(), RgnID);
			}
			break;
		}
	}
}


// 发送数据库删除DBDupRgn消息
void CGlobalRgnManager::SendDelPHDupRgnMsgToDB(const CGUID& rgnGUID)
{
	CMessage msg(MSG_W2DB_OPER_ENTITY);
//	msg.Add((long)COM_PERSONALHOUSERGN); // opType			4byte, 操作对象类型
	// OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
	// DBOP Type:     1byte, 数据库操作类型（枚举值）
	// MSG ID:        4byte, 该消息的MSGID
	// Entity Num:	2byte, 该实体个数(X)
	msg.Add((BYTE)0);
	
	msg.Add((long)-1);
	msg.Add((WORD)1);
	// GUID
	vector<BYTE> pBA;
	_AddToByteArray(&pBA, rgnGUID);
	msg.Add((long)pBA.size());
	if(pBA.size())
		msg.AddEx(&pBA[0], pBA.size());
	msg.SendToSocket(GetGame()->GetDBSocketID());
}
void CGlobalRgnManager::AddTeamRgnGUID(const CGUID& ownerID, LONG lTemplateRgnID, const CGUID& rgnGuid, BOOL bCover)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		map<LONG, CGUID>::iterator itr = ite->second.find(lTemplateRgnID);
		if(ite->second.end() != itr && !bCover)
			return;
		ite->second[lTemplateRgnID] = rgnGuid;
	}
	else
	{
		map<LONG, CGUID> teamGuidMap;
		teamGuidMap[lTemplateRgnID] = rgnGuid;
		m_mapTeamRgnGUID[ownerID] = teamGuidMap;
	}
}
void CGlobalRgnManager::DelTeamRgnGUID(const CGUID& ownerID, LONG lTemplateRgnID)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
		ite->second.erase(lTemplateRgnID);
}
void CGlobalRgnManager::DelTeamRgnGUID(const CGUID& ownerID, const CGUID& rgnGuid)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		map<LONG, CGUID>::iterator itr = ite->second.begin();
		for (; itr != ite->second.end(); ++itr)
		{
			if(itr->second == rgnGuid)
			{
				ite->second.erase(itr);
				return;
			}
		}
	}
}
//! 清除一个玩家的队伍副本
void CGlobalRgnManager::ClearTeamRgnGUID(const CGUID& ownerGuid)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerGuid);
	if( ite != m_mapTeamRgnGUID.end() )
		ite->second.clear();
}
const CGUID& CGlobalRgnManager::GetTeamRgnGUID(const CGUID& ownerID, LONG lTemplateRgnID)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		map<LONG, CGUID>::iterator itr = ite->second.find(lTemplateRgnID);
		if(ite->second.end() != itr)
			return itr->second;
	}
	return NULL_GUID;
}
//! 通过顺序得到一个队伍的副本GUID
const CGUID& CGlobalRgnManager::GetTeamRgnGuidByIndex(const CGUID& ownerID, LONG lIndex)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		if(ite->second.size() < lIndex || 0 == lIndex)
			return NULL_GUID;
		LONG lCurrIndex = 1;
		map<LONG, CGUID>::iterator itr = ite->second.begin();
		for (; itr != ite->second.end(); ++itr)
		{
			if(lIndex == lCurrIndex)
			{
				return itr->second;
			}
			++lCurrIndex;
		}
	}
	return NULL_GUID;
}
//! 通过顺序得到一个队伍的副本模板ID
LONG CGlobalRgnManager::GetTeamRgnTemplateIdByIndex(const CGUID& ownerID, LONG lIndex)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		if(ite->second.size() < lIndex || 0 == lIndex)
			return -1;
		LONG lCurrIndex = 1;
		map<LONG, CGUID>::iterator itr = ite->second.begin();
		for (; itr != ite->second.end(); ++itr)
		{
			if(lIndex == lCurrIndex)
			{
				return itr->first;
			}
			++lCurrIndex;
		}
	}
	return -1;
}
map<LONG, CGUID>*	CGlobalRgnManager::GetTeamRgnGuidMap(const CGUID& ownerGuid)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerGuid);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		return &ite->second;
	}
	return NULL;
}
VOID CGlobalRgnManager::RgnGuidCode(const CGUID& ownerID, vector<BYTE> &vData)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		_AddToByteArray(&vData, (LONG)ite->second.size());
		map<LONG, CGUID>::iterator itr = ite->second.begin();
		for (; itr != ite->second.end(); ++itr)
		{
			_AddToByteArray(&vData, itr->first);
			_AddToByteArray(&vData, itr->second);
		}
	}
}
VOID CGlobalRgnManager::RgnGuidDecode(const CGUID& ownerID, BYTE *pData, LONG &lPos)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		ite->second.clear();
		LONG lNum = _GetLongFromByteArray(pData, lPos);
		for (LONG i = 0; i < lNum; ++i)
		{
			LONG lTemplateRgnID = _GetLongFromByteArray(pData, lPos);
			CGUID guid;
			_GetBufferFromByteArray(pData, lPos, guid);
			if(ite->second.end() == ite->second.find(lTemplateRgnID))
			{
				ite->second.insert(pair<LONG, CGUID>(lTemplateRgnID, guid));
				CWorldRegion* pRgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(guid);
				if(pRgn)
					pRgn->SetOwnerGUID(ownerID);
			}
		}
	}
}
//! 解码交换队伍副本（不覆盖相同模板的副本）
VOID CGlobalRgnManager::RgnGuidSwapFromArray(const CGUID& ownerID, BYTE *pData, LONG &lPos)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		LONG lNum = _GetLongFromByteArray(pData, lPos);
		for (LONG i = 0; i < lNum; ++i)
		{
			LONG lTemplateRgnID = _GetLongFromByteArray(pData, lPos);
			CGUID guid;
			_GetBufferFromByteArray(pData, lPos, guid);
			if(ite->second.end() == ite->second.find(lTemplateRgnID))
			{
				ite->second.insert(pair<LONG, CGUID>(lTemplateRgnID, guid));
				CWorldRegion* pRgn = GetGame()->GetGlobalRgnManager()->FindRgnByGUID(guid);
				if(pRgn)
					pRgn->SetOwnerGUID(ownerID);
			}
		}
	}
}


//!					队伍副本GUID编码
VOID CGlobalRgnManager::RgnGuidCode(const CGUID& ownerID, DBWriteSet& setWriteDB)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite != m_mapTeamRgnGUID.end() )
	{
		setWriteDB.AddToByteArray((LONG)ite->second.size());
		map<LONG, CGUID>::iterator itr = ite->second.begin();
		for (; itr != ite->second.end(); ++itr)
		{
			setWriteDB.AddToByteArray(itr->first);
			setWriteDB.AddToByteArray(itr->second);
		}
	}
}
//!					队伍副本GUID解码
VOID CGlobalRgnManager::RgnGuidDecode(const CGUID& ownerID, DBReadSet& setReadDB)
{
	map<LONG, CGUID> mapRegionID;
	LONG lNum = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lNum; ++i)
	{
		LONG lTemplateRgnID = setReadDB.GetLongFromByteArray();
		CGUID guid;
		setReadDB.GetBufferFromByteArray(guid);
		mapRegionID.insert(pair<LONG, CGUID>(lTemplateRgnID, guid));
	}

	m_mapTeamRgnGUID[ownerID] = mapRegionID;
}
//!					解码交换队伍副本（不覆盖相同模板的副本）
VOID CGlobalRgnManager::RgnGuidSwapFromArray(const CGUID& ownerID, DBReadSet& setReadDB)
{
	map<CGUID, map<LONG, CGUID>>::iterator ite = m_mapTeamRgnGUID.find(ownerID);
	if( ite == m_mapTeamRgnGUID.end() )
	{
		m_mapTeamRgnGUID.insert(pair<CGUID, map<LONG, CGUID>>(ownerID, map<LONG, CGUID>()));
		ite = m_mapTeamRgnGUID.find(ownerID);
	}

	LONG lNum = setReadDB.GetLongFromByteArray();
	for (LONG i = 0; i < lNum; ++i)
	{
		LONG lTemplateRgnID = setReadDB.GetLongFromByteArray();
		CGUID guid;
		setReadDB.GetBufferFromByteArray(guid);
		if(ite->second.end() == ite->second.find(lTemplateRgnID))
			ite->second.insert(pair<LONG, CGUID>(lTemplateRgnID, guid));
	}
}
//!					得到玩家的个人副本GUID列表
map<CGUID, CGUID>&	CGlobalRgnManager::GetPersonalRgnGuidMap(const CGUID& ownerGuid)
{
	return m_mapPersonalRgnGUID;
}
void CGlobalRgnManager::AddPersonalRgnGUID(const CGUID& ownerGuid, const CGUID& rgnGuid)
{
	m_mapPersonalRgnGUID[ownerGuid] = rgnGuid;
}
//! 删除一个玩家个人副本GUID
void CGlobalRgnManager::DelPersonalRgnGUID(const CGUID& ownerGuid)
{
	m_mapPersonalRgnGUID.erase(ownerGuid);
}
//!					得到一个个人副本的GUID
const CGUID& CGlobalRgnManager::GetPersonalRgnGUID(const CGUID& ownerGuid)
{
	map<CGUID, CGUID>::iterator itr = m_mapPersonalRgnGUID.find(ownerGuid);
	if(itr != m_mapPersonalRgnGUID.end())
		return itr->second;
	return NULL_GUID;
}