#pragma once

#ifndef _CountryParam_h_
#define _CountryParam_h_

class CCountryParam
{
private:
	CCountryParam(void);
	~CCountryParam(void);

private:
	static CCountryParam* instance;											// 该类唯一实例句柄

	static long	m_lMaxCountyrs;													// 国家最大数
	static long	m_lMaxCountryPower;												// 国力最大值
	

	// 复活相关
	static map<BYTE,long>	m_mpMainRegions;// 主城场景
	static map<BYTE,RECT>	m_mpMainRect;// 主城坐标
	static map<BYTE,long>	m_mpMainDir;// 主城方向
	static map<BYTE,long>  m_mpStartCamp;//阵营编号

	static map<BYTE,long>	m_mpJailVillageRegions;// 流放村场景
	static map<BYTE,RECT>	m_mpJailVillageRect;// 流放村坐标
	static map<BYTE,long>	m_mpJailVillageDir;// 流放村方向

	static map<BYTE,long>	m_mpJailIslandRegions;// 流放岛场景
	static map<BYTE,RECT>	m_mpJailIslandRect;// 流放岛坐标
	static map<BYTE,long>	m_mpJailIslandDir;// 流放岛方向

public:
	static bool	Initialize();														//初始化
	static void	Release();															//释放资源

	static void	GetCountryList(long lCampID,list<BYTE> &lCountryID);				//根据同盟ID获取国家列表

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

	long GetMaxCountrys(void) { return m_lMaxCountyrs; }
	long GetMaxCountryPower(void) { return m_lMaxCountryPower; }
	// 主城相关属性接口
	static long GetCountryMainRegionID	(BYTE byCountryID);
	static long GetCountryMainRectLeft	(BYTE byCountryID);
	static long GetCountryMainRectRight	(BYTE byCountryID);
	static long GetCountryMainRectBottom(BYTE byCountryID);
	static long GetCountryMainRectTop	(BYTE byCountryID);
	static long GetCountryMainDir		(BYTE byCountryID);
	static long GetCountryStartCamp		(BYTE byCountryID);

	// 流放村相关属性接口
	static long GetCountryJailVillageRegionID	(BYTE byCountryID);
	static long GetCountryJailVillageRectLeft	(BYTE byCountryID);
	static long GetCountryJailVillageRectRight	(BYTE byCountryID);
	static long GetCountryJailVillageRectBottom	(BYTE byCountryID);
	static long GetCountryJailVillageRectTop	(BYTE byCountryID);
	static long GetCountryJailVillageDir		(BYTE byCountryID);

	// 流放岛相关属性接口
	static long GetCountryJailIsLandRegionID	(BYTE byCountryID);
	static long GetCountryJailIsLandRectLeft	(BYTE byCountryID);
	static long GetCountryJailIsLandRectRight	(BYTE byCountryID);
	static long GetCountryJailIsLandRectBottom	(BYTE byCountryID);
	static long GetCountryJailIsLandRectTop		(BYTE byCountryID);
	static long GetCountryJailIsLandDir			(BYTE byCountryID);

	static inline	CCountryParam* GetInstance()
	{
		if(instance == NULL)
		{
			instance = new CCountryParam;
		}
		return instance;
	}
};

//得到组织控制类句柄
inline CCountryParam* GetCountryParam()
{
	return CCountryParam::GetInstance();
}


#endif