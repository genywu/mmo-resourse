#pragma once
#include "..\..\frostengine\ui.h"
class RadioButton;


class WndNewMap :
	public ui::Wnd
{

	ui::EditBox *m_pEditWidth;
	ui::EditBox *m_pEditDepth;
	RadioButton *m_pCheckBox;

	ui::Button		*m_pBtnOK;
	ui::Button		*m_pBtnCancel;

	ui::ListBox     *m_pListBoxTileSet;


	DWORD           m_dwShaderGroupHandle;
	INT             m_nDefaultBackTextureID;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//检查弹出结束

public:
	WndNewMap(void);
	virtual ~WndNewMap(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	//// 取得ShaderHandle
	//DWORD GetShaderHandle()	{ return m_dwShaderGroupHandle; }


	virtual void Update();
	virtual void Render();

	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsOK() { return m_bIsOK;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}
	
	int GetWidth(void);
	int GetDepth(void);

	DWORD GetSelectTileSet();
	INT GetDefaultBackTextureID() { return m_nDefaultBackTextureID; }

	VOID SetCheckBoxTextureHandle(DWORD dwIndex, DWORD dwTextureHandle);
	VOID ResetCheckBox();
	void ChangeTileSet(DWORD dwIndex);

	//DWORD GetTileSetIndex(void){return m_dwShaderGroupHandle;}
	//DWORD GetDefaultTextureID(void){return m_nDefaultBackTextureID;}

};
