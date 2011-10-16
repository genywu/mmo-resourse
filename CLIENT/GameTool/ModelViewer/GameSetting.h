#pragma once

#include "..\..\frostengine\singleton.h"

class Setting
{
	DECLARE_SINGLETON(Setting);

	DWORD m_dwBackgroundColor;
	DWORD m_dwLightColor;
	char  m_pszTextureSharFolder[MAX_PATH];

public:
	Setting(void);
	~Setting(void);

	void  SetBackGroundColor(DWORD dwBackColor) { m_dwBackgroundColor = dwBackColor;}
	DWORD GetBackGroundColor() const { return m_dwBackgroundColor;}

	void  SetLightColor(DWORD dwLightColor) { m_dwLightColor = dwLightColor;}
	DWORD GetLightColor() const { return m_dwLightColor;}

	bool LoadSetting();
	void SaveSetting();
};

