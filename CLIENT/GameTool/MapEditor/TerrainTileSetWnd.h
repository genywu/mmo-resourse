//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月7日 11:19:27
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#pragma once
#include "..\..\frostengine\ui.h"
class EditBox;
class RadioButton;


class WndTerrainTileSet : public ui::Wnd
{
	D3DCOLOR *m_pColor;
	RadioButton *m_pRadioButton;
	INT       m_nCheckedIndex;

public:
	WndTerrainTileSet(void);
	~WndTerrainTileSet(void);

	virtual BOOL Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent);
	virtual void Destroy();

	virtual void Update();
	virtual void Render();

	// 设置CheckBox 的纹理句柄
	VOID SetCheckBoxTextureHandle(DWORD dwIndex, DWORD dwTextureHandle, RECT rc);

	VOID SetBoxChecked(DWORD dwIndex, BOOL bCheckFlag = TRUE);

	DWORD GetCheckedIndex() { return m_nCheckedIndex; }

	// 设置CheckBox 的显示状态
	VOID ShowCheckBox(DWORD dwIndex, BOOL bShow = TRUE);

	// 重置所有的CheckBox
	VOID ResetCheckBox();

	D3DCOLOR GetBrushColor() { return m_pColor[m_nCheckedIndex]; }
	D3DCOLOR GetBrushColor(DWORD dwIndex) { return m_pColor[dwIndex]; }
};
