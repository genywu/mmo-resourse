#pragma once

#include <vector>
	

// 阻挡类型
const DWORD dwBlockColor[5] = 
{
	0x00000000,		// 无阻挡
	0x80800000,		// 可飞行阻挡
	0x80FF0000,		// 不可飞行阻挡
	0x800000ff,		// 人物
};

class EditWnd;


class ToolBlockEdit
{
	int m_iBrushSize;

	DWORD m_dwCurSelGrid;

	DWORD m_dwCurBLockType;//0 可飞行阻挡 1 不可飞行阻挡 2 人物

public:
	ToolBlockEdit(void);
	~ToolBlockEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	
	void DrawBrush(void);
	void PaintBrush(DWORD dwBlock);

	DWORD GetSelectGrid(void);
};
