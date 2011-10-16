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

public:
	static inline	CCountryParam* GetInstance()
	{
		if(instance == NULL)
		{
			instance = new CCountryParam;
		}
		return instance;
	}
	static void ReleaseInstance()
	{
		if(instance)
			SAFE_DELETE(instance);
	};

	long	m_lMaxCountyrs;													// 国家最大数
	long	m_lMaxCountryPower;												// 国力最大值

	map<BYTE,long>	m_mpStartRegions;// 出生场景
	map<BYTE,RECT>	m_mpStartRect;// 出生坐标
	map<BYTE,long>	m_mpStartDir;// 出生方向
	map<BYTE,long>  m_mpStartCamp;//阵营编号

	// 复活相关
	map<BYTE,long>	m_mpMainRegions;// 主城场景
	map<BYTE,RECT>	m_mpMainRect;// 主城坐标
	map<BYTE,long>	m_mpMainDir;// 主城方向

	map<BYTE,long>	m_mpJailVillageRegions;// 流放村场景
	map<BYTE,RECT>	m_mpJailVillageRect;// 流放村坐标
	map<BYTE,long>	m_mpJailVillageDir;// 流放村方向

	map<BYTE,long>	m_mpJailIslandRegions;// 流放岛场景
	map<BYTE,RECT>	m_mpJailIslandRect;// 流放岛坐标
	map<BYTE,long>	m_mpJailIslandDir;// 流放岛方向

	bool	Initialize();														//初始化
	void	Release();															//释放资源
	bool	Load();

	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};

//得到组织控制类句柄
inline CCountryParam* GetCountryParam()
{
	return CCountryParam::GetInstance();
}


#endif