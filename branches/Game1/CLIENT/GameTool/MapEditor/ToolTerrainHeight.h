//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月8日 11:19:52
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#pragma once
#include "TerrainMesh.h"

// 编辑状态
enum HEIGHTEDITSTATUETYPE 
{
	HEST_IDLE = 0,  // 空闲
	HEST_RAISE,		// 提高
	HEST_LOWER,     // 压低
	HEST_PLATEAU,   // 平台
	HEST_NOISE,     // 随机
	HEST_SMOOTH     // 平滑
};

class ToolTerrainHeight
{
	//---------------------------------------------------------------------------------
	// 成员变量
	//---------------------------------------------------------------------------------
private:
	HEIGHTEDITSTATUETYPE m_eEditStatus;
	DWORD m_dwBrushSize;

	//---------------------------------------------------------------------------------
	// 成员函数
	//---------------------------------------------------------------------------------
public:
	ToolTerrainHeight(void);
	~ToolTerrainHeight(void);

	VOID SetEditStatue(HEIGHTEDITSTATUETYPE flag);

	// 显示编辑状态
	VOID ShowStatus();

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
};
