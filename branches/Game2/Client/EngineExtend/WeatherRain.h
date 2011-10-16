//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年2月3日 13:40:29
// 更新时间 : 
// 摘要     : 用面片模拟雨
//---------------------------------------------------------------------------------
#pragma once
#include "Weather.h"

class WeatherRain : public Weather
{
	//---------------------------------------------------------------------------------
	// 成员变量
private:
	struct _tagQuadRain                       // 雨的数据结构
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 speed;
		FLOAT length;
		FLOAT width;

		BOOL living;							// 标记是否死亡
	};
	_tagQuadRain *m_ptQuadRain;

	FLOAT m_fMinLength, m_fMaxLength;         // 基准长度范围
	FLOAT m_fWidth;                           // 基准宽度

	//---------------------------------------------------------------------------------
	// 成员函数
public:
	WeatherRain(void);
	virtual ~WeatherRain(void);

	virtual BOOL CreateParticles();
	virtual VOID Destroy();

	virtual VOID Render();
	virtual BOOL Update(BOOL bEnable);

	void EnableLiving(BOOL bEnable);

	virtual void ModifyParameters(TCHAR *config);

private:
	virtual VOID ReBirthParticle(_tagQuadRain *pQuadRain);
};
