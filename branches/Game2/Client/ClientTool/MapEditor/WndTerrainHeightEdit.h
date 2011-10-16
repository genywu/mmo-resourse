//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月8日 14:36:55
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#pragma once
#include "..\..\engine\ui.h"

class RadioButton;

class WndTerrainHeightEdit : public ui::Wnd
{
	//---------------------------------------------------------------------------------
	// 成员变量
	//---------------------------------------------------------------------------------
private:
	RadioButton *m_pRadioButton;
	INT       m_nCheckedIndex;
	//---------------------------------------------------------------------------------
	// 成员函数
	//---------------------------------------------------------------------------------
public:
	WndTerrainHeightEdit(void);
	~WndTerrainHeightEdit(void);

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
};
