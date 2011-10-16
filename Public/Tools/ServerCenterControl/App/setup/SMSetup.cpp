

#include "stdafx.h"
#include "SMSetup.h"


SMSetup SMSetup::c_SMSetup;

SMSetup::SMSetup(VOID)
{

}
SMSetup::~SMSetup(VOID)
{

}

SMSetup& SMSetup::GetInstance(VOID)
{
	return c_SMSetup;
}


//! 获取配置
BOOL SMSetup::LoadSetup(const char *pXmlFilename)
{
	//! 函数开始
	if(NULL == pXmlFilename)
		return FALSE;

	TiXmlNode *pNode = NULL; 		

	TiXmlDocument* m_Tdoc = new TiXmlDocument(pXmlFilename);
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode = m_Tdoc->FirstChild("SMList");

	if (pNode == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	TiXmlElement* pSMList = pNode->ToElement();//获取node 的指针

	if (pSMList == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	////////////////////////////////////////////

	//! 发送给所有SM的全局配置
	
	m_Scc2SmSetup.dwGetHardWareInfoTime = atoi(pSMList->Attribute("GetHardWareInfoTime"));
	m_Scc2SmSetup.dwSaveHardWareInfoSize = atoi(pSMList->Attribute("SaveHardWareInfoSize"));

	m_Scc2SmSetup.dwStartTimeOut = atoi(pSMList->Attribute("StartTimeOut"));
	m_Scc2SmSetup.dwShutdownTimeOut = atoi(pSMList->Attribute("ShutdownTimeOut"));
	m_Scc2SmSetup.dwPingTimeOut = atoi(pSMList->Attribute("PingTimeOut"));
	m_Scc2SmSetup.dwDelayTestTime = atoi(pSMList->Attribute("DelayTestTime"));

	//! SM分组信息
	m_mapSMSetup.clear();

	TiXmlElement* pRegion = pSMList->FirstChildElement("Region");
	for (; pRegion != NULL; pRegion = pRegion->NextSiblingElement("Region"))
	{
		DWORD dwRegionID = atoi(pRegion->Attribute("ID"));
		LPCSTR pRegionName = pRegion->Attribute("Name");
		if(NULL == pRegionName)
		{
			LogTools::OutLog("load RegionName lost when load %s", pXmlFilename);
			continue;
		}

		TiXmlElement* pGroup = pRegion->FirstChildElement("Group");
		for (; pGroup != NULL; pGroup = pGroup->NextSiblingElement("Group"))
		{
			DWORD dwGroupID = atoi(pGroup->Attribute("ID"));
			LPCSTR pGroupName = pGroup->Attribute("Name");
			if(NULL == pGroupName)
			{
				LogTools::OutLog("load GroupName lost when load %s", pXmlFilename);
				continue;
			}

			TiXmlElement* pSMNode = pGroup->FirstChildElement("SMNode");
			for (; pSMNode != NULL; pSMNode = pSMNode->NextSiblingElement("SMNode"))
			{
				tagSMSetup TmpSetup;

				{
					TmpSetup.dwRegionID = dwRegionID;
					TmpSetup.dwGroupID = dwGroupID;

					LONG lCopyLen = (LONG)strlen(pRegionName);
					lCopyLen = (lCopyLen < MAX_SM_NAME_SZIE) ? lCopyLen : MAX_SM_NAME_SZIE;
					memcpy(TmpSetup.szRegionName, pRegionName, lCopyLen);

					lCopyLen = (LONG)strlen(pGroupName);
					lCopyLen = (lCopyLen < MAX_SM_NAME_SZIE) ? lCopyLen : MAX_SM_NAME_SZIE;
					memcpy(TmpSetup.szGroupName, pGroupName, lCopyLen);
				}

				TmpSetup.dwID = atoi(pSMNode->Attribute("ID"));
				{
					const char *pName = pSMNode->Attribute("Name");
					if(NULL == pName)
					{
						LogTools::OutLog("load Name lost when load %s", pXmlFilename);
						continue;
					}

					LONG lCopyLen = (LONG)strlen(pName);
					lCopyLen = (lCopyLen < MAX_SM_NAME_SZIE) ? lCopyLen : MAX_SM_NAME_SZIE;
					memcpy(TmpSetup.szName, pName, lCopyLen);
				}
				{
					const char *pExplain = pSMNode->Attribute("Explain");
					if(NULL == pExplain)
					{
						LogTools::OutLog("load Explain lost when load %s", pXmlFilename);
						continue;
					}

					LONG lCopyLen = (LONG)strlen(pExplain);
					lCopyLen = (lCopyLen < BASE_TEXT_SIZE) ? lCopyLen : BASE_TEXT_SIZE;
					memcpy(TmpSetup.szExplain, pExplain, lCopyLen);
				}
				{
					const char *pIP = pSMNode->Attribute("IP");
					if(NULL == pIP)
					{
						LogTools::OutLog("load IP lost when load %s", pXmlFilename);
						continue;
					}

					LONG lCopyLen = (LONG)strlen(pIP);
					lCopyLen = (lCopyLen < MAX_HOST_ADDRESS_SZIE) ? lCopyLen : MAX_HOST_ADDRESS_SZIE;
					memcpy(TmpSetup.szIP, pIP, lCopyLen);
					TmpSetup.lIP = inet_addr(pIP);
				}

				{
					const char *pLoginPwd = pSMNode->Attribute("LoginPwd");
					if(NULL == pLoginPwd)
					{
						LogTools::OutLog("load LoginPwd lost when load %s", pXmlFilename);
						continue;
					}

					TMD5 tmd5;
					LONG lCopyLen = (LONG)strlen(pLoginPwd);
					tmd5.MessageDigest((const unsigned char *)pLoginPwd, lCopyLen, (unsigned char *)TmpSetup.szMD5LoginPwd);
				}

				m_mapSMSetup.insert(pair<LONG, tagSMSetup>(TmpSetup.dwID, TmpSetup));
			}
		}
	}

	if(0 == m_mapSMSetup.size())
	{
		LogTools::OutLog("the %s is null!!!", pXmlFilename);
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	return TRUE;
}


//! 得到一个SM的配置
const tagSMSetup* SMSetup::GetSMSetup(DWORD dwSMID) const
{
	map<LONG, tagSMSetup>::const_iterator ite = m_mapSMSetup.find(dwSMID);
	if(m_mapSMSetup.end() != ite)
		return &(ite->second);
	return NULL;
}

//! 得到一个SM的配置
const tagSMSetup* SMSetup::GetSMSetupByIP(LONG lIP) const
{
	for (map<LONG, tagSMSetup>::const_iterator ite = m_mapSMSetup.begin(); m_mapSMSetup.end() != ite; ++ite)
	{
		if (lIP == ite->second.lIP)
		{
			return &(ite->second);
		}
	}
	return NULL;
}

//!
VOID SMSetup::GetSMNetSetupList(vector<tagTempConnSetup> &vSMSetup)
{
	vSMSetup.clear();
	for (map<LONG, tagSMSetup>::iterator ite = m_mapSMSetup.begin(); m_mapSMSetup.end() != ite; ++ite)
	{
		vSMSetup.push_back(tagTempConnSetup(ite->second.szIP, ite->second.dwID));
	}
}