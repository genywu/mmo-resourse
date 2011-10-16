//---------------------------------------------------------------------------------
//重写by 贾涛
//---------------------------------------------------------------------------------
#pragma once
#include "CmdPaintColor.h"

// 编辑状态

enum PAINTCOLORSTATUETYPE
{
	PCST_IDLE = 0,					// 空闲
	PCST_PAINT,						// 涂色
	PCST_PICKFORECOLOR,				// 选前景色
	PCST_PICKBACKCOLOR				// 选后景色
};

class ToolPaintColor
{
private:
	RECT	m_rcColorDialog;													// 记录选色对话框的rc
	RECT	m_rcForeColorQuad, m_rcBackColorQuad;								// 记录颜色面板的rc
	DWORD	m_dwForeColor, m_dwBackColor;										// 选中的颜色，包括前景色和后景色。
	DWORD	m_dwTextureIndex;													// 选色面板贴图的index

	float   m_fBrushRadius;															// 画刷的大小

	DWORD   m_dwHoverVertex;

	float m_fColorChroma;														// 颜色浓度


	PAINTCOLORSTATUETYPE	m_eEditStatus;										// 编辑状态
	DWORD					m_dwBrushType;										// 画笔类型(0 铅笔，1毛笔)

public:
	ToolPaintColor(void);
	~ToolPaintColor(void);

	void Create(TCHAR *colorfile, RECT &rcColorDialog, RECT &rcForeColorQuad, RECT &rcBackColorQuad);
	void Destroy();
	void MessageProc(BOOL bActive);
	void Render();
	void ShowColorDialog();
	DWORD PickColor(DWORD x, DWORD y);
	DWORD  GetHoverVertex();

	void _ShowStatus();
	void DrawBrush(void);
};
