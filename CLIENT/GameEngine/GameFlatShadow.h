//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年4月22日 10:13:41
// 更新时间 : 
// 摘要     : 平面阴影
//           实现：
//               在模型的脚底处画一面片
//---------------------------------------------------------------------------------
//修正于2004.12 贾涛
#pragma once
#include "gamemap.h"


class GameFlatShadow
{
private:
	DWORD m_dwShaderGroupHandle;

	D3DXVECTOR3 m_pPos[36];
	D3DXVECTOR2 m_pUV0[36];
	D3DCOLOR	m_pColor[36];

public:
	GameFlatShadow(void);
	~GameFlatShadow(void);

	void Create();
	void Destroy();

	void Render(float x,float y,float z,float fScaleFactor, GameMap *pGameMap);
};
