#pragma once
#include "OrganizingDef.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Create Time:2005.3.15



class COrganizingParam
{
public:
	COrganizingParam(void);
	~COrganizingParam(void);

public:
	//!											得到唯一实例句柄
	static		COrganizingParam&				getInstance(void);
	//!											解码
	void										DecodeFromMsgSet(DBReadSet &DbReadSet);
	//!											添加到CByteArray
	BOOL										AddToByteArray(vector<BYTE> &vByteArray);
	//!											解码
	void										DecodeFromByteArray(BYTE* pData, LONG &lPos);

	//!											获得等级配置
	const		tagFacLevelSetup*				GetLevelSetup(LONG lLevel);
	//!											得到使用一个权限的工会等级
	LONG										GetFacPurviewNeedLevel(eFactionPurviewIndex PurviewIndex);

	//!											得到创建联盟的条件
	const		VecCondition&					GetUnionCreateCondition(VOID);

	//!											得到一个升级类型的条件
	const		VecCondition*					GetUpgradeVecCondition(DWORD dwUpgradeType, LONG lAimLevel);

	//!											得到一个等级的福利经验配置
	const		tagFacExpLevelSetup*			GetFacExpLevelSetup(LONG lExpLevel);

	//!											得到一个 行会功能的最高等级
	LONG										GetFacFunMaxLevel(DWORD dwUpgradeType);

	//!											警告传位的时间
	LONG										GetDemiseWarningTime();

	//!											警告传位的期限
	LONG										GetDemiseTerm();

	//!											人数太少解散的期限
	LONG										GetDisbandTerm();

	//!											人数最少达标率，百分比
	float										GetMemberLeastRate();
	//!											获取消耗资源信息
	const		tagBaseConsume*					GetConsume(DWORD dwLv);
	//!											获取商业等级基数
	float										GetBussBase(DWORD dwLv); 


private:
	//!											配置获取成功标记
	BOOL										m_LoadSucceed;

	//!											工会全局配置
	tagGlobeSetup								m_tagGlobeSetup;
	//!											帮会相关的等级参数配置
	vector<tagFacLevelSetup>					m_vFacLevelSetup;
	//!											工会功能开放的等级配置
	LONG										m_FunctionWithLevel[MAXNUM_FactionPurview];

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
	//!											家族对应消耗基数和临界点
	map<LONG,tagBaseConsume>					m_BaseConsume;
	//!											商业等级对应的资源消耗基数
	map<LONG,LONG>								m_BussinessBase; 
};