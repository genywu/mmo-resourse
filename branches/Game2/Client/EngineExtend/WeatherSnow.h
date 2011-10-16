//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年2月2日 16:46:04
// 更新时间 : 
// 摘要     : 用point sprite实现的粒子雪。
//---------------------------------------------------------------------------------
#pragma once
#include "Weather.h"

class WeatherSnow : public Weather
{
	//---------------------------------------------------------------------------------
	// 成员变量
private:
	VERTEX *m_ptSnow;
	BOOL *m_bLiving;

	FLOAT m_fSize;      // 基准尺寸

	//---------------------------------------------------------------------------------
	// 成员函数
public:
	WeatherSnow(void);
	virtual ~WeatherSnow(void);

	virtual BOOL CreateParticles();
	virtual VOID Destroy();

	virtual VOID Render();
	virtual BOOL Update(BOOL bEnable, D3DXVECTOR3 &vWindForce = D3DXVECTOR3(0, 0, 0));

	void EnableLiving(BOOL bEnable);

	virtual void ModifyParameters(TCHAR *config);

private:
	virtual VOID ReBirthParticle(VERTEX *pSnowParticle);
};
