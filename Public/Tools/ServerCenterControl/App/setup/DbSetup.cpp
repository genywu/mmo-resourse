

#include "stdafx.h"
#include "DbSetup.h"


DbSetup DbSetup::c_DbSetup;

//! 预分配4个配置对象，足够用了
DbSetup::DbSetup(VOID)
{
}
DbSetup::~DbSetup(VOID)
{

}

DbSetup& DbSetup::GetInstance(VOID)
{
	return c_DbSetup;
}



struct tagDbSetup
{
	DWORD	dwID;
	char	szSqlConType[ABOUT_DATABASE_SETUP_SIZE];
	char	szSqlServerIP[ABOUT_DATABASE_SETUP_SIZE];
	char	szSqlUserName[ABOUT_DATABASE_SETUP_SIZE];
	char	szSqlPassWord[ABOUT_DATABASE_SETUP_SIZE];
	char	szDBName[ABOUT_DATABASE_SETUP_SIZE];

	tagDbSetup(VOID){memset(this, 0, sizeof(tagDbSetup));}
};

//! 获取配置
BOOL DbSetup::LoadSetup(const char *pXmlFilename)
{
	//! 函数开始
	if(NULL == pXmlFilename)
		return FALSE;

	TiXmlNode *pNode = NULL; 		

	TiXmlDocument* m_Tdoc = new TiXmlDocument(pXmlFilename);
	if(!m_Tdoc->LoadFile())
		return FALSE;

	pNode = m_Tdoc->FirstChild("DatabaseSetup");

	if (NULL == pNode)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	TiXmlElement* pDatabaseSetup = pNode->ToElement();//获取node 的指针

	if (NULL == pDatabaseSetup)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}

	////////////////////////////////////////////
	m_mapConnectString.clear();
	memset(&m_ThreadSetup, 0, sizeof(tagThreadSetup));

	TiXmlElement* pDatabase = pDatabaseSetup->FirstChildElement("Database");
	if (NULL == pDatabaseSetup)
	{
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}
	m_ThreadSetup.dwMinDbOptThreadNum		= atol(pDatabaseSetup->Attribute("MinDbOptThreadNum"));
	m_ThreadSetup.dwMaxDbOptThreadNum		= atol(pDatabaseSetup->Attribute("MaxDbOptThreadNum"));
	m_ThreadSetup.dwThreadNumAdjustSpace	= atol(pDatabaseSetup->Attribute("ThreadNumAdjustSpace"));
	
	for (; pDatabase != NULL; pDatabase = pDatabase->NextSiblingElement("Database"))
	{
		tagDbSetup DbSetup;
		DbSetup.dwID = atol(pDatabase->Attribute("ID"));

		if(m_mapConnectString.end() != m_mapConnectString.find(DbSetup.dwID))
		{
			LogTools::OutLog("ignore repeated ID[%d] when load [%s]", DbSetup.dwID, pXmlFilename);
			continue;
		}

		{
			const char *pSqlConType = pDatabase->Attribute("SqlConType");
			if(NULL == pSqlConType)
			{
				LogTools::OutLog("load SqlConType lost when load[%s] [ID:%d]", pXmlFilename, DbSetup.dwID);
				continue;
			}

			LONG lCopyLen = (LONG)strlen(pSqlConType);
			lCopyLen = (lCopyLen < ABOUT_DATABASE_SETUP_SIZE) ? lCopyLen : ABOUT_DATABASE_SETUP_SIZE;
			memcpy(DbSetup.szSqlConType, pSqlConType, lCopyLen);
		}
		{
			const char *pSqlServerIP = pDatabase->Attribute("SqlServerIP");
			if(NULL == pSqlServerIP)
			{
				LogTools::OutLog("load SqlServerIP lost when load[%s] [ID:%d]", pXmlFilename, DbSetup.dwID);
				continue;
			}

			LONG lCopyLen = (LONG)strlen(pSqlServerIP);
			lCopyLen = (lCopyLen < ABOUT_DATABASE_SETUP_SIZE) ? lCopyLen : ABOUT_DATABASE_SETUP_SIZE;
			memcpy(DbSetup.szSqlServerIP, pSqlServerIP, lCopyLen);
		}
		{
			const char *pSqlUserName = pDatabase->Attribute("SqlUserName");
			if(NULL == pSqlUserName)
			{
				LogTools::OutLog("load SqlUserName lost when load[%s] [ID:%d]", pXmlFilename, DbSetup.dwID);
				continue;
			}

			LONG lCopyLen = (LONG)strlen(pSqlUserName);
			lCopyLen = (lCopyLen < ABOUT_DATABASE_SETUP_SIZE) ? lCopyLen : ABOUT_DATABASE_SETUP_SIZE;
			memcpy(DbSetup.szSqlUserName, pSqlUserName, lCopyLen);
		}
		{
			const char *pSqlPassWord = pDatabase->Attribute("SqlPassWord");
			if(NULL == pSqlPassWord)
			{
				LogTools::OutLog("load SqlPassWord lost when load[%s] [ID:%d]", pXmlFilename, DbSetup.dwID);
				continue;
			}

			LONG lCopyLen = (LONG)strlen(pSqlPassWord);
			lCopyLen = (lCopyLen < ABOUT_DATABASE_SETUP_SIZE) ? lCopyLen : ABOUT_DATABASE_SETUP_SIZE;
			memcpy(DbSetup.szSqlPassWord, pSqlPassWord, lCopyLen);
		}
		{
			const char *pDBName = pDatabase->Attribute("DBName");
			if(NULL == pDBName)
			{
				LogTools::OutLog("load DBName lost when load[%s] [ID:%d]", pXmlFilename, DbSetup.dwID);
				continue;
			}

			LONG lCopyLen = (LONG)strlen(pDBName);
			lCopyLen = (lCopyLen < ABOUT_DATABASE_SETUP_SIZE) ? lCopyLen : ABOUT_DATABASE_SETUP_SIZE;
			memcpy(DbSetup.szDBName, pDBName, lCopyLen);
		}

		m_mapConnectString[DbSetup.dwID] = 
			string("Provider=") + DbSetup.szSqlConType +
			"; Server=" + DbSetup.szSqlServerIP + 
			"; Database=" + DbSetup.szDBName + 
			"; UID=" + DbSetup.szSqlUserName + 
			"; PWD=" + DbSetup.szSqlPassWord + 
			"; OLE DB Services=-1; Driver={SQL Server}";
	}

	if(0 == m_mapConnectString.size())
	{
		LogTools::OutLog("did not receive a correct config when load[%s]!!!", pXmlFilename);
		SAFE_DELETE(m_Tdoc);
		return FALSE;
	}
	SAFE_DELETE(m_Tdoc);
	return TRUE;
}


//!
const tagThreadSetup* DbSetup::GetThreadSetup(VOID)
{
	if(0 == m_ThreadSetup.dwMinDbOptThreadNum)
		return NULL;
	return &m_ThreadSetup;
}

//!
const string& DbSetup::GetConnectString(DWORD dwDBID)
{
	map<DWORD, string>::iterator ite = m_mapConnectString.find(dwDBID);
	if (m_mapConnectString.end() != ite)
	{
		return ite->second;
	}

	return INVALID_STRING;
}