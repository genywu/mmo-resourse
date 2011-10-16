
#pragma once
#include "..\..\engine\ui.h"

class WndFogAdjust:
	public ui::Wnd
{
public:
	WndFogAdjust(void);
	~WndFogAdjust(void);
private:
	class ColorButton : public ui::Button
	{
		DWORD m_dwColor;
	public:
		virtual void Render();

		void  SetColor(DWORD dwColor) { m_dwColor = dwColor;}
		DWORD GetColor() { return m_dwColor;}
	};

	ui::Static    *   m_pStaticFogStart;
	ui::Static    *   m_pStaticFogEnable;
	ui::Static    *   m_pStaticFogEnd;
	ui::EditBox   *   m_pEditBoxFogStart;
	ui::EditBox   *   m_pEditBoxFogEnd;
	ui::ScrollBar *   m_pSBFogStart;
	ui::ScrollBar *   m_pSBFogEnd;


	ui::Button  * m_pBtnOK;
	ui::CheckBox  * m_pChkFogEnable;
	ColorButton	* m_pBtnFog;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø

	BOOL m_bPickColor;
	DWORD m_dwColorMapHandle;
	DWORD m_dwColorMapWidth,m_dwColorMapHeight;
	DWORD m_dwPickX,m_dwPickY;
public:
	BOOL CheckPopUp()	{ BOOL bValue = m_bIsPopupCheckFlag;return bValue;}
	BOOL IsOK()	{ return m_bIsOK;}
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	void Update();
	void Render();

	BOOL  GetFogEnable(){return m_pChkFogEnable->IsChecked();}
	DWORD GetFogColor(){return m_pBtnFog->GetColor();}
	DWORD GetFogStart();
	DWORD GetFogEnd();

	void SetFogEnable(BOOL enable);
	void SetFogColor(DWORD dwColor);
	void SetFogStart(DWORD dwStart);
	void SetFogEnd(DWORD dwEnd);

	void BeginPopUp();
	void EndPopUp(void);

};