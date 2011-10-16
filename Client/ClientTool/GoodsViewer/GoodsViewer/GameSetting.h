/**
* @filename:GameSetting.h
* @date: 2009/12/15
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: 游戏设置类，包括背景色、灯光颜色、纹理文件路径
*/
#pragma  once

class Setting
{
	DECLARE_SINGLETON(Setting);

	DWORD m_BackgroundColor;///< 背景色
	DWORD m_LightColor;///< 灯光颜色
	char  m_szTextureSharFolder[MAX_PATH];///< 纹理文件路径
	char  m_szGoodsModelFolder[MAX_PATH];///<物品模型文件路径
public:
	Setting(void);
	~Setting(void);

	void SetBackGroundColor(DWORD BackColor){m_BackgroundColor = BackColor;};
	DWORD GetBackGroundColor()const { return m_BackgroundColor;};

	void SetLightColor(DWORD LightColor){m_LightColor = LightColor;};
	DWORD GetLightColor()const {return m_LightColor;};

	bool LoadSetting();
	void SaveSetting();
};