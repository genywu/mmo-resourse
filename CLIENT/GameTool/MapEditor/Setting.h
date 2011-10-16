#pragma once

class Setting
{
	DECLARE_SINGLETON(Setting);

	float m_fViewDistance;
	float m_fHieghtMapRatio;
	DWORD m_bFogEnable;
	DWORD m_dwFogColor;
	char  m_pszTextureSharFolder[MAX_PATH];

public:
	Setting(void);
	~Setting(void);

	BOOL  GetFogEnable()  const { return m_bFogEnable;}
	void  SetFogEnable(BOOL bEnable) { m_bFogEnable = bEnable;}

	DWORD GetFogColor() const { return m_dwFogColor;}
	DWORD SetFogColor(DWORD dwColor) { return m_dwFogColor;}

	float GetViewDistance() const { return m_fViewDistance;}
	float GetHeightMapRatio() const { return m_fHieghtMapRatio;}
                                                                       
	void  SetViewDistance(float fViewDistance) { m_fViewDistance = fViewDistance;}
	void  SetHeightMapRatio(float fHeightMapRatio) { m_fHieghtMapRatio = fHeightMapRatio;}

	BOOL  LoadSetting();
	void  SaveSetting();
};
