//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年2月5日 13:44:03
// 更新时间 : 
// 摘要     : 用面片实现体积雾
//---------------------------------------------------------------------------------
#pragma once
#include "Weather.h"

class WeatherFog : public Weather
{
	//---------------------------------------------------------------------------------
	// 成员变量
private:
	struct _tagFog                  // 雾的数据结构
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 speed;
		FLOAT size;

		BOOL living;
	};
	_tagFog *m_ptFog;

	FLOAT m_fSize;                  // 基准尺寸

	//---------------------------------------------------------------------------------
	// 成员函数
public:
	WeatherFog(void);
	virtual ~WeatherFog(void);

	virtual BOOL CreateParticles();
	virtual VOID Destroy();

	virtual VOID Render();
	virtual BOOL Update(BOOL bEnable);

	virtual void ModifyParameters(TCHAR *config);

	void EnableLiving(BOOL bEnable);

private:
	virtual VOID ReBirthParticle(_tagFog *pFog);
};
