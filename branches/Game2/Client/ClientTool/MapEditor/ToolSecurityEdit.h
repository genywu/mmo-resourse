#pragma once

#include <vector>


const DWORD dwSecurityColor[8] =
{
	0x00000000,	    //空
	0x80004000,		//草地
	0xa0128013,		//泥巴
};


//此处保存MtlEdit编辑的

//数据和方法
//分三个状态
//1 选择状态

//2 设置状态

class EditWnd;

class ToolSecurityEdit
{
	int m_iBrushSize;

	DWORD m_dwCurSelGrid;


public:
	ToolSecurityEdit(void);
	~ToolSecurityEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	
	void DrawBrush(void);
	void PaintBrush(DWORD dwSecurity);

	DWORD GetSelectGrid(void);
};
