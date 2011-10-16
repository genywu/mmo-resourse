#pragma once

#include <vector>
	

const DWORD dwWarColor[8] =
{
	0x00000000,	    //非城战
	0x800d2242		//城战
};


//此处保存MtlEdit编辑的

//数据和方法
//分三个状态
//1 选择状态

//2 设置状态

class EditWnd;

class ToolWarEdit
{
	int m_iBrushSize;

	DWORD m_dwCurSelGrid;

public:
	ToolWarEdit(void);
	~ToolWarEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	
	void DrawBrush(void);
	void PaintBrush(DWORD dwMtl);

	DWORD GetSelectGrid(void);

};
