#pragma once
#include "../public/OrganizingDef.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Editer:	Fred
//Create Time:2005.3.15
//Edit Time:2008.04.08





class WSOrganizingParam
{
public:
	WSOrganizingParam(void);
	~WSOrganizingParam(void);

public:
	//!									得到唯一实例句柄
	static		WSOrganizingParam&		getInstance(void);
	//!									初始化
	BOOL								Initialize(void);
	//!									释放对象
	void								Release(void);
	//!									添加到消息缓存
	BOOL								CodeToMsgSet(DBWriteSet &DbWriteSet);

	//!									获得等级配置
	const		tagFacLevelSetup*		GetLevelSetup(LONG lLevel);
	//!									得到一个工会等级开放的所有会员权限
	LONG								GetCurrLvlAllPurview(LONG lCurrLvl);
	//!									得到一个升级类型的条件
	const		VecCondition*			GetUpgradeVecCondition(DWORD dwUpgradeType, LONG lAimLevel);
	//!									添加每个家族等级段的科技等级最高级，用于降低家族等级时降低科技等级
	void								AddLevel(DWORD dwFacLv,eUpgradeType eType,DWORD dwLv);
	//!									获取消耗资源信息
	const		tagBaseConsume*			GetConsume(DWORD dwLv);
	//!									获取商业等级基数
	float								GetBussBase(DWORD dwLv); 
	//!									全局配置
	const		tagGlobeSetup&			GetGlobe(){return m_tagGlobeSetup;}
	//!									家族等级对应的科技等级
	const		tagTechInfo*			GetTechInfo(LONG lv){return m_TechInfo.find(lv)!=m_TechInfo.end()?&m_TechInfo[lv]:NULL;}
private:						
	//!									重读配置
	BOOL								ReLoad(void);


private:
	//!									配置读取成功标记
	BOOL								m_LoadSucceed;

	//!									工会全局配置
	tagGlobeSetup						m_tagGlobeSetup;
	//!									帮会相关的等级参数配置
	vector<tagFacLevelSetup>			m_vFacLevelSetup;
	//!									工会功能开放的等级配置
	LONG								m_FunctionWithLevel[MAXNUM_FactionPurview];

	//!									联盟创建条件
	VecCondition						m_vUnionUpgradeCondition;

	//!									帮会经验奖励等级的配置
	vector<tagFacExpLevelSetup>			m_vFacExpLevelSetup;
	//!									专业等级的配置
	vector<tagFacDepotLevelSetup>		m_vFacDepotLevelSetup;

	//!									战争等级配置
	vector<tagFacSubLevelSetup>			m_SpecialLevelSetup;
	//!									商业等级配置
	vector<tagFacSubLevelSetup>			m_BussinessLevelSetup;
	//!									贵族等级配置
	vector<tagFacSubLevelSetup>			m_NobleLevelSetup;		
	//!									家族等级对应科技等级最高级配置
	map<LONG,tagTechInfo>				m_TechInfo;
	//!									家族对应消耗基数和临界点
	map<LONG,tagBaseConsume>			m_BaseConsume;
	//!									商业等级对应的资源消耗基数
	map<LONG,LONG>						m_BussinessBase;
	
};

