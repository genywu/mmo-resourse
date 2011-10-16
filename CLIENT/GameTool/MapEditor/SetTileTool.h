#pragma once
#include "TerrainMesh.h"

// 编辑状态
enum TILEEDITSTATUETYPE 
{
	TEST_IDLE = 0,  // 空闲
	TEST_PLACE		// 放置
};

class ToolSetTile
{
	TILEEDITSTATUETYPE m_eEditStatus;
	TerrainMesh::_tagProcessData* m_pData;
	DWORD m_dwBrushSize;

public:
	ToolSetTile(void);
	~ToolSetTile(void);

	// 设置编辑状态
	VOID SetEditStatue(TILEEDITSTATUETYPE flag);

	// 显示编辑状态
	VOID ShowStatus();

	// 比较操作参数，不同时更新为pData。
	BOOL CompareProcessData(TerrainMesh::_tagProcessData* pData);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
};
