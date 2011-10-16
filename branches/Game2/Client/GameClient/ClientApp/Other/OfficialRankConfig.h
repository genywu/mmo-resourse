#pragma once

// 状态设置
class COffciailRankConfig
{
public:
	//国家官阶信息
	struct tagOfficialRankConfig
	{
		int CreditMinValue;		//国家威望最小值
		int CreditMaxValue;		//国家威望最大值
		string strFighterOfficialRank;	//战士国家官阶
		string strHunterOfficialRank;	//猎手国家官阶
		string strDaoshiOfficialRank;	//道士国家官阶
		int	textColorA;			//字颜色
		int textColorR;
		int textColorG;
		int	textColorB;
		int	textBkColorA;		//字背景颜色
		int textBkColorR;
		int textBkColorG;
		int textBkColorB;
	};
	
public:
	COffciailRankConfig(void);
	virtual ~COffciailRankConfig(void);

private:
	static vector<COffciailRankConfig::tagOfficialRankConfig>	m_vecOfficialRackConfig;	//保存国家官阶的配置信息
public:
	static vector<COffciailRankConfig::tagOfficialRankConfig>&	GetOfficialRankVec(){return m_vecOfficialRackConfig;}
	static void LoadOfficialRankConfig();
	
};
