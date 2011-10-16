#pragma once

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
	bool										AddToByteArray(vector<uchar> &vByteArray);
	//!											解码
	void										DecodeFromByteArray(uchar* pData, long &lPos);

	//!											获得等级配置
	const		tagFacLevelSetup*				GetLevelSetup(long lLevel);
	//!											得到使用一个权限的工会等级
	long										GetFacPurviewNeedLevel(eFactionPurviewIndex PurviewIndex);

	//!											得到创建联盟的条件
	const		VecCondition&					GetUnionCreateCondition(void);

	//!											得到一个升级类型的条件
	const		VecCondition*					GetUpgradeVecCondition(ulong dwUpgradeType, long lAimLevel);

	//!											得到一个等级的福利经验配置
	const		tagFacExpLevelSetup*			GetFacExpLevelSetup(long lExpLevel);

	//!											得到一个 行会功能的最高等级
	long										GetFacFunMaxLevel(ulong dwUpgradeType);

	//!											警告传位的时间
	long										GetDemiseWarningTime();

	//!											警告传位的期限
	long										GetDemiseTerm();

	//!											人数太少解散的期限
	long										GetDisbandTerm();

	//!											人数最少达标率，百分比
	float										GetMemberLeastRate();


private:
	//!											配置获取成功标记
	bool										m_LoadSucceed;

	//!											工会全局配置
	tagGlobalSetup								m_tagGlobalSetup;
	//!											帮会相关的等级参数配置
	vector<tagFacLevelSetup>					m_vFacLevelSetup;
	//!											工会功能开放的等级配置
	long										m_FunctionWithLevel[MAXNUM_FactionPurview];

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