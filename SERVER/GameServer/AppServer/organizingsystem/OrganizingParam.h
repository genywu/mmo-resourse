#pragma once
#include "..\public\OrganizingDef.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Create Time:2005.3.15


//! 升级类型
enum eUpgradeType
{
	eUT_FactionLevel,	//! 工会等级
	eUT_ExpLevel,		//! 经验福利等级
	eUT_DepotLevel,		//! 仓库等级

	eUT_SmithingLevel,	//! 锻造等级
	eUT_MetallurgyLevel,//! 炼金等级
	eUT_SewingLevel,	//! 缝纫等级
};

class COrganizingParam
{
public:
	COrganizingParam(void);
	~COrganizingParam(void);

public:
	//!											得到唯一实例句柄
	static		COrganizingParam&				getInstance(void);
	//!											解码
	void										DecodeByWS(BYTE* pData, LONG &lPos);
	//!											添加到CByteArray
	BOOL										AddToByteArray(vector<BYTE> &vByteArray);

	//!											获得等级配置
	const		tagFacLevelSetup*				GetLevelSetup(LONG lLevel);
	//!											得到使用一个权限的工会等级
	LONG										GetFacPurviewNeedLevel(eFactionPurviewIndex PurviewIndex);

	//!											得到创建联盟的条件
	const		VecCondition&					GetUnionCreateCondition(VOID);

	//!											得到一个升级类型的条件
	const		VecCondition*					GetUpgradeVecCondition(DWORD dwUpgradeType, LONG lAimLevel);



private:
	//!											配置获取成功标记
	BOOL										m_LoadSucceed;

	//!											工会全局配置
	tagGlobeSetup								m_tagGlobeSetup;
	//!											帮会相关的等级参数配置
	vector<tagFacLevelSetup>					m_vFacLevelSetup;
	//!											工会功能开放的等级配置
	LONG										m_FunctionWithLevel[MAXNUM_FactionPurview];

	//!											工会辅助功能设置
	map<LONG, tagFacSetWithLevel>				m_mapFacSetWithLevel;

	//!											联盟创建条件
	VecCondition								m_vUnionUpgradeCondition;

	//!											帮会经验奖励等级的配置
	vector<tagFacExpLevelSetup>					m_vFacExpLevelSetup;
	//!											帮会仓库等级的配置
	vector<tagFacDepotLevelSetup>				m_vFacDepotLevelSetup;

	//!											锻造等级配置
	vector<tagFacSubLevelSetup>					m_SmithingLevelSetup;
	//!											炼金等级配置
	vector<tagFacSubLevelSetup>					m_MetallurgyLevelSetup;
	//!											缝纫等级配置
	vector<tagFacSubLevelSetup>					m_SewingLevelSetup;	
};