#pragma once


class EditWnd;


class ToolEnvCreatureFlagEdit
{
	int m_iBrushSize;

	DWORD m_dwCurSelGrid;

public:
	ToolEnvCreatureFlagEdit(void);
	~ToolEnvCreatureFlagEdit(void);

	void MessageProc(BOOL bActive);
	void Render();
	void Create();
	void Destroy(void);
	void _ShowStatus(void);
	
	void DrawBrush(void);
	void PaintBrush(BOOL bEnvCreature);

	DWORD GetSelectGrid(void);
};
