#pragma once



const DWORD dwMtlColor[10] =
{
	 0xa0000000//缺省	
	,0xa000FF00//草地
	,0xa0FF0000//泥巴
	,0xa0FFFF00//沙漠
	,0xa0FF00FF//木板
	,0xa0808080//石板
	,0xa00000FF//水面
	,0xa0FFFFFF//雪地
	,0xa000FFFF//沼泽
};


//此处保存MtlEdit编辑的

//数据和方法
//分三个状态
//1 选择状态

//2 设置状态

class EditWnd;

class ToolMtlEdit
{
	int m_iBrushSize;

	DWORD m_dwCurSelGrid;

public:
	ToolMtlEdit(void);
	~ToolMtlEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	
	void DrawBrush(void);
	void PaintBrush(DWORD dwMtl);

	DWORD GetSelectGrid(void);

};
