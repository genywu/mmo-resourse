


#include "stdafx.h"
#include "OrganiseSys.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


COrganiseSys::COrganiseSys()
{

}

COrganiseSys::~COrganiseSys()
{
	Release();
}

void COrganiseSys::Release()
{
}

void COrganiseSys::DecodeFromDataBlock(DBReadSet& dbRead,bool bSelf)
{
	// 行会和联盟 GUID
	dbRead.GetBufferFromByteArray(m_FamilyGuid);
	dbRead.GetBufferFromByteArray(m_UnionGuid);

	if((BOOL)dbRead.GetByteFromByteArray())
	{
		LONG lFacDemiseWarning = 0;
		LONG lFacDisbandWarning = 0;

		if( bSelf )
		{
			lFacDemiseWarning = dbRead.GetLongFromByteArray(); //! 帮会传位提醒倒记时
			lFacDisbandWarning = dbRead.GetLongFromByteArray(); //! 帮会解散提醒倒记时
		}

		// 称号
		TCHAR szFactionTitle[ MAX_MEMBER_NAME_SIZE ];
		dbRead.GetStringFromByteArray( szFactionTitle, MAX_MEMBER_NAME_SIZE );

		// 行会图标CRC
		m_dwFacIconCRC = dbRead.GetDwordFromByteArray();

		// PUBLIC信息、权限、官阶
		tagFacPublicData stFacPublicData;
		ZeroMemory( &stFacPublicData, sizeof(stFacPublicData) );

		TCHAR	szPronounce[ MAX_PronounceCharNum ];
		ZeroMemory( szPronounce, sizeof(szPronounce) );

		long lMyJobPurview = 0;

		if( bSelf )
		{
			// 获取PUBLIC信息
			dbRead.GetBufferFromByteArray( &stFacPublicData, sizeof(tagFacPublicData) );
			// 获取官阶
			m_lFamilyJobLevel = dbRead.GetLongFromByteArray();

			// 获取权限数据
			lMyJobPurview = dbRead.GetLongFromByteArray();

			// 公告
			dbRead.GetStringFromByteArray( szPronounce, MAX_PronounceCharNum);
		}
		else
		{
			dbRead.GetStringFromByteArray( stFacPublicData.szName, MAX_MEMBER_NAME_SIZE );
		}

		// 帮会名称和称号
		m_strFamilyName = stFacPublicData.szName;
		m_strFamilyTitle = szFactionTitle;

		// 创建一个行会
		//CFactionManager*	pFactionManager = GetGame()->GetFactionManager();

		//FamilyInfo	stFacInfo;
		//ZeroMemory( &stFacInfo, sizeof(stFacInfo) );
		//stFacInfo.FamilyGuid				 = m_FamilyGuid;
		//strcpy(stFacInfo.strName,stFacPublicData.szName);
		//strcpy(stFacInfo.strPlacard,szPronounce);
		//stFacInfo.arrFunLevel[ FUN_FACTION ] = stFacPublicData.lLevel;
		//stFacInfo.tMasterLastOnlineTime		 = (time_t)lFacDemiseWarning;
		//stFacInfo.tLessMemNumTime			 = (time_t)lFacDisbandWarning;
	}
}