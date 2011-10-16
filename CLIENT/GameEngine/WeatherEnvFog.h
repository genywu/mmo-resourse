//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年2月9日 11:05:46
// 更新时间 : 
// 摘要     : 控置硬件支持的环境雾的参数，可以打开和关闭雾效，并设定颜色渐变。
//---------------------------------------------------------------------------------
#pragma once
#include <d3dx9.h>
#include "weather.h"

class WeatherEnvFog : public Weather
{
	//---------------------------------------------------------------------------------
	// 成员变量
private:
	BOOL m_bEnableFog, m_bUpdateFog;
	D3DXCOLOR m_StartColor, m_EndColor, m_CurColor;    // 起始颜色，结束颜色，当前颜色
	DWORD m_dwTime;                                    // 变色时间
	FLOAT m_fStart, m_fEnd, m_fDensity;                // 起始位置，结束位置，浓度
	DWORD m_dwMode;                                    // 插值模式
	BOOL m_bModify;										// 是否更改参数

	//---------------------------------------------------------------------------------
	// 成员函数
public:
	WeatherEnvFog(void);
	~WeatherEnvFog(void);

	VOID ModifyEnvFogParameter(TCHAR *config);

	VOID EnableFog(BOOL bFlag = TRUE);
	VOID SetupPixelFog(LPDIRECT3DDEVICE9 pd3dDevice);
	//VOID ChangeColor(D3DXCOLOR color, DWORD dwTime);
	void SetTime(DWORD dwTime);
	void SetColor(D3DXCOLOR color);
	void SetEnvFogDistance(FLOAT start, FLOAT end);
	D3DCOLOR GetCurrentColor();
	VOID Update();
};
