#include "StdAfx.h"
#include "organizingparam.h"
#include "../public/tinystr.h"
#include "../public/tinyxml.h"
#include "../goods/CGoodsFactory.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Create Time:2005.3.15

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern HWND g_hWnd;

COrganizingParam	g_OrganizingParam;

COrganizingParam::COrganizingParam(void)
:m_LoadSucceed(FALSE)
{
}

COrganizingParam::~COrganizingParam(void)
{
}

//得到唯一实例句柄
//该函数不支持多线程操作
COrganizingParam& COrganizingParam:: getInstance()
{
	return g_OrganizingParam;
}

//!									解码
void COrganizingParam::DecodeByWS(BYTE* pData, LONG &lPos)
{
	m_LoadSucceed = FALSE;

	//! 工会全局配置
	_GetBufferFromByteArray(pData, lPos, &m_tagGlobeSetup, sizeof(tagGlobeSetup));

	//! 帮会相关的等级参数配置
	m_vFacLevelSetup.clear();
	LONG lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacLevelSetup FacLevelSetup;
		FacLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		FacLevelSetup.lMaxMemNum	= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_vFacLevelSetup.push_back(FacLevelSetup);
	}

	//! 联盟创建条件
	m_vUnionUpgradeCondition.clear();
	LONG lConditionSize = _GetLongFromByteArray(pData, lPos);
	for (LONG j = 0; j < lConditionSize; ++j)
	{
		tagUpgradeCondition UpgradeCondition;
		_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
		m_vUnionUpgradeCondition.push_back(UpgradeCondition);
	}

	//! 工会功能开放的等级配置
	_GetBufferFromByteArray(pData, lPos, &m_FunctionWithLevel, sizeof(LONG) * MAXNUM_FactionPurview);

	m_mapFacSetWithLevel.clear();
	LONG lSetWithLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG j = 0; j < lSetWithLevelSize; ++j)
	{
		tagFacSetWithLevel tmpFacSetWithLevel;
		_GetBufferFromByteArray(pData, lPos, &tmpFacSetWithLevel, sizeof(tagFacSetWithLevel));
		m_mapFacSetWithLevel[tmpFacSetWithLevel.lLevel] = tmpFacSetWithLevel;
	}

	//! 帮会经验奖励等级的配置
	m_vFacExpLevelSetup.clear();
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacExpLevelSetup FacExpLevelSetup;
		FacExpLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		FacExpLevelSetup.lBaseExp	= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacExpLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_vFacExpLevelSetup.push_back(FacExpLevelSetup);
	}
	//! 帮会仓库等级的配置
	m_vFacDepotLevelSetup.clear();
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacDepotLevelSetup FacDepotLevelSetup;
		FacDepotLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		FacDepotLevelSetup.lCellNum		= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize				= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacDepotLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_vFacDepotLevelSetup.push_back(FacDepotLevelSetup);
	}
	//! 锻造等级配置
	m_SmithingLevelSetup.clear();
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacSubLevelSetup FacSubLevelSetup;
		FacSubLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_SmithingLevelSetup.push_back(FacSubLevelSetup);
	}
	//! 炼金等级配置
	m_MetallurgyLevelSetup.clear();
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacSubLevelSetup FacSubLevelSetup;
		FacSubLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_MetallurgyLevelSetup.push_back(FacSubLevelSetup);
	}
	//! 缝纫等级配置
	m_SewingLevelSetup.clear();
	lLevelSize = _GetLongFromByteArray(pData, lPos);
	for (LONG i = 0; i < lLevelSize; ++i)
	{
		tagFacSubLevelSetup FacSubLevelSetup;
		FacSubLevelSetup.lLevel		= _GetLongFromByteArray(pData, lPos);
		LONG lConditionSize			= _GetLongFromByteArray(pData, lPos);
		for (LONG j = 0; j < lConditionSize; ++j)
		{
			tagUpgradeCondition UpgradeCondition;
			_GetBufferFromByteArray(pData, lPos, &UpgradeCondition, sizeof(tagUpgradeCondition));
			FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
		}
		m_SewingLevelSetup.push_back(FacSubLevelSetup);
	}


	m_LoadSucceed = TRUE;
}

// 添加到CByteArray
BOOL COrganizingParam::AddToByteArray(vector<BYTE> &vByteArray)
{
	//! 工会全局配置
	_AddToByteArray(&vByteArray, &m_tagGlobeSetup, sizeof(tagGlobeSetup));

	//! 帮会相关的等级参数配置
	_AddToByteArray(&vByteArray, (LONG)m_vFacLevelSetup.size());
	for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
	{
		_AddToByteArray(&vByteArray, m_vFacLevelSetup[i].lLevel);
		_AddToByteArray(&vByteArray, m_vFacLevelSetup[i].lMaxMemNum);
		_AddToByteArray(&vByteArray, (LONG)m_vFacLevelSetup[i].vCondition.size());
		for (LONG j = 0; j < m_vFacLevelSetup[i].vCondition.size(); ++j)
		{
			_AddToByteArray(&vByteArray, &m_vFacLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
		}
	}
	//! 工会功能开放的等级配置
	_AddToByteArray(&vByteArray, &m_FunctionWithLevel, sizeof(LONG) * MAXNUM_FactionPurview);
	//! 工会辅助功能设置
	_AddToByteArray(&vByteArray, (LONG)m_mapFacSetWithLevel.size());
	map<LONG, tagFacSetWithLevel>::iterator ite = m_mapFacSetWithLevel.begin();
	for (; ite != m_mapFacSetWithLevel.end(); ++ite)
	{
		_AddToByteArray(&vByteArray, &(ite->second), sizeof(tagFacSetWithLevel));
	}
	return TRUE;
}

//! 获得等级配置
const tagFacLevelSetup* COrganizingParam::GetLevelSetup(LONG lLevel)
{
	if (m_LoadSucceed)
	{
		for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
		{
			if (lLevel == m_vFacLevelSetup[i].lLevel)
			{
				return &m_vFacLevelSetup[i];
			}
		}
	}

	return NULL;
}

//! 得到使用一个权限的工会等级
LONG COrganizingParam::GetFacPurviewNeedLevel(eFactionPurviewIndex PurviewIndex)
{
	if (MAXNUM_FactionPurview > PurviewIndex && 0 <= PurviewIndex)
	{
		return m_FunctionWithLevel[PurviewIndex];
	}
	return  0x7FFFFFFF;
}

//!											得到创建联盟的条件
const VecCondition& COrganizingParam::GetUnionCreateCondition(VOID)
{
	assert(m_LoadSucceed);
	return m_vUnionUpgradeCondition;
}


//!											得到一个升级类型的条件
const VecCondition* COrganizingParam::GetUpgradeVecCondition(DWORD dwUpgradeType, LONG lAimLevel)
{
	if (m_LoadSucceed)
	{
		switch(dwUpgradeType)
		{
		case eUT_FactionLevel:		//! 工会等级
			{
				for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacLevelSetup[i].lLevel)
					{
						return &(m_vFacLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_ExpLevel:			//! 经验福利等级
			{
				for (LONG i = 0; i < m_vFacExpLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacExpLevelSetup[i].lLevel)
					{
						return &(m_vFacExpLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_DepotLevel:		//! 仓库等级
			{
				for (LONG i = 0; i < m_vFacDepotLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacDepotLevelSetup[i].lLevel)
					{
						return &(m_vFacDepotLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_SmithingLevel:		//! 锻造等级
			{
				for (LONG i = 0; i < m_SmithingLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_SmithingLevelSetup[i].lLevel)
					{
						return &(m_SmithingLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_MetallurgyLevel:	//! 炼金等级
			{
				for (LONG i = 0; i < m_MetallurgyLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_MetallurgyLevelSetup[i].lLevel)
					{
						return &(m_MetallurgyLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_SewingLevel:		//! 缝纫等级
			{
				for (LONG i = 0; i < m_SewingLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_SewingLevelSetup[i].lLevel)
					{
						return &(m_SewingLevelSetup[i].vCondition);
					}
				}
			}
			break;
		}
	}

	return NULL;
}