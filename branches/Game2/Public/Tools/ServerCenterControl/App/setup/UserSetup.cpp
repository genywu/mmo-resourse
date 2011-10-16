

#include "stdafx.h"
#include "UserSetup.h"

#include "../AppClass/DbOperator.h"


UserSetup UserSetup::c_UserSetup;

UserSetup::UserSetup(VOID)
{

}
UserSetup::~UserSetup(VOID)
{

}

UserSetup& UserSetup::GetInstance(VOID)
{
	return c_UserSetup;
}

//! 获取配置
BOOL UserSetup::LoadSetup(const char *pXmlFilename)
{
	//! 函数开始
	if(NULL == pXmlFilename)
		return FALSE;

	TiXmlNode *pNode = NULL; 		

	TiXmlDocument* m_Tdoc = new TiXmlDocument(pXmlFilename);
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode = m_Tdoc->FirstChild("UserSetup");

	if (pNode == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	TiXmlElement* pUserSetup = pNode->ToElement();//获取node 的指针

	if (pUserSetup == NULL)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	////////////////////////////////////////////
	m_mapUserSetup.clear();

	m_TimeOutSec = atoi(pUserSetup->Attribute("TimeOutSec"));

	TiXmlElement* pUserGroup = pUserSetup->FirstChildElement("UserGroup");
	for (; pUserGroup != NULL; pUserGroup = pUserGroup->NextSiblingElement("UserGroup"))
	{
		DWORD dwUserType = atol(pUserGroup->Attribute("UserType"));
		TiXmlElement* pUser = pUserGroup->FirstChildElement("User");
		for (; pUser != NULL; pUser = pUser->NextSiblingElement("User"))
		{
			tagUserSetup UserSetup;
			UserSetup.dwUserType = dwUserType;

			{
				const char *pName = pUser->Attribute("Name");
				if(NULL == pName)
				{
					LogTools::OutLog("load Name lost when load %s", pXmlFilename);
					continue;
				}

				LONG lCopyLen = (LONG)strlen(pName);
				lCopyLen = (lCopyLen < MAX_USER_NAME_SZIE) ? lCopyLen : MAX_USER_NAME_SZIE;
				memcpy(UserSetup.szName, pName, lCopyLen);
			}
			{
				const char *pPreliminaryPwd = pUser->Attribute("PreliminaryPwd");
				if(NULL == pPreliminaryPwd)
				{
					LogTools::OutLog("load PreliminaryPwd lost when load %s", pXmlFilename);
					continue;
				}

				TMD5 tmd5;
				LONG lCopyLen = (LONG)strlen(pPreliminaryPwd);
				unsigned char szTmpMD5[MD5_16_PASSWORD_SIZE] = {0};
				tmd5.MessageDigest((const unsigned char *)pPreliminaryPwd, lCopyLen, szTmpMD5);

				TransitionMD5ToStr(UserSetup.szPwd, szTmpMD5);
			}

			m_mapUserSetup.insert(pair<string, tagUserSetup>(UserSetup.szName, UserSetup));
		}
	}

	if (0 == m_mapUserSetup.size())
	{
		LogTools::OutLog("the %s is null!!!", pXmlFilename);
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	SAFE_DELETE(m_Tdoc);
	return TRUE;
}

//!									找到一个用户
const tagUserSetup* UserSetup::FindUser(const char *pName) const
{
	map<string, tagUserSetup>::const_iterator ite = m_mapUserSetup.find(string(pName));
	if (m_mapUserSetup.end() != ite)
	{
		return &(ite->second);
	}
	return NULL;
}

//! 开始验证用户的密码和初始密码是否一致
VOID UserSetup::BeginUserPwdCmp(VOID)
{
	for (map<string, tagUserSetup>::iterator ite = m_mapUserSetup.begin(); m_mapUserSetup.end() != ite; ++ite)
	{
		DbOperator *pDbOperator = DbOperator::CreatePwdCmpOperator(ite->second.szName, ite->second.szPwd);
		if(NULL != pDbOperator)
		{
			if(!pDbOperator->Begin())
			{
				DbOperator::ReleaseOperator(&pDbOperator);
			}
		}
	}
}
//! 响应在数据库中比较用户密码的结果
VOID UserSetup::OnUserPwdCmp(LONG lResult, const char *pName, const char *pPwd)
{
	if(NULL == pName || NULL == pPwd)
		return;

	map<string, tagUserSetup>::iterator ite = m_mapUserSetup.find(string(pName));
	if (m_mapUserSetup.end() != ite)
	{
		ite->second.bIsCmd = TRUE;
		if (2 == lResult)
		{
			strcpy(ite->second.szPwd, pPwd);
		}
	}
}

//! 查看验证是否完成
BOOL UserSetup::IsFinishCmp(VOID)
{
	for (map<string, tagUserSetup>::iterator ite = m_mapUserSetup.begin(); m_mapUserSetup.end() != ite; ++ite)
	{
		if(!ite->second.bIsCmd)
			return FALSE;
	}
	return TRUE;
}

//! 修改密码（不影响配置文件）
VOID UserSetup::ChangePwd(const char *pName, const char *pPwd)
{
	if(NULL == pName || NULL == pPwd)
		return;

	map<string, tagUserSetup>::iterator ite = m_mapUserSetup.find(string(pName));
	if (m_mapUserSetup.end() != ite)
	{
		strcpy(ite->second.szPwd, pPwd);
	}
}

VOID UserSetup::TransitionMD5ToStr(char *pOut, const byte *pIn)
{
	//把二进制密码转换成字符串
	static char hexDigits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	const unsigned char* pByte = &pIn[0];
	for(int i=0; i<16; i++)
	{
		int bValue = pIn[i];
		int bValue1 = bValue>>4;
		int bValue2 = bValue&0x0F;
		if(bValue1 >= 0 && bValue1 <= 15 && bValue2 >= 0 && bValue2 <= 15)
		{
			pOut[i*2] = hexDigits[bValue1];
			pOut[i*2+1] = hexDigits[bValue2];
		}
	}
}