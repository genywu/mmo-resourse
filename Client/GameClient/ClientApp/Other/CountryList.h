#pragma once
#ifndef _CountryList_h_
#define _CountryList_h_
// 状态设置
class CCountryList
{
public:
	CCountryList(void);
	virtual ~CCountryList(void);
public:

	struct tagCountryList
	{
		BYTE		nID;				// 编号
		long		lIconID;			// 图标编号
		string		strName;			// 名字
		string		strDesc;			// 描述
		string		strAllianceDesc;	// 联盟描述
		D3DCOLOR	color;				// 颜色
		D3DCOLOR	colorBack;			// 背景色
		BYTE		nLeague;			// 阵营
	};
private:
	static map<BYTE, tagCountryList>		m_mpCountryList;

	static D3DCOLOR	m_dwMyCountryColor;			// 自己国家领土颜色
	static D3DCOLOR	m_dwOtherCountryColor;		// 其他国家领土颜色
	static D3DCOLOR	m_dwNoCountryColor;			// 没有国家的领土颜色
public:
	static D3DCOLOR	GetMyCountryColor()			{return m_dwMyCountryColor;}
	static D3DCOLOR	GetOtherCountryColor()		{return m_dwOtherCountryColor;}
	static D3DCOLOR	GetNoCountryColor()			{return m_dwNoCountryColor;}
	static BOOL Load(const char* filename);
	typedef map<BYTE,tagCountryList>MapCountryList;
	static MapCountryList* GetCountryList(){return &m_mpCountryList;}
	static tagCountryList* GetCountry(BYTE nID);
	static const char* GetCountryName(BYTE nID);
};

#endif