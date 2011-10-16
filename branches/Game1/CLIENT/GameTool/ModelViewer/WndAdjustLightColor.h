#pragma once
#include "..\..\frostengine\ui.h"

class WndEditLightColor :  public ui::Wnd
{

	class ColrBtn : public ui::Button
	{
		DWORD m_dwColor;
	public:
		virtual void Render();

		void  SetColor(DWORD dwColor) { m_dwColor = dwColor;}
		DWORD GetColor() { return m_dwColor;}
	};

	ui::Static  * m_pStaticLightColor;	
    ColrBtn		* m_pBtnLightColor;

	ui::Static  * m_pStaticAmbientColor;	
    ColrBtn		* m_pBtnAmbientColor;

	ui::Button  * m_pBtnOK;
	ui::Button  * m_pBtnCancel;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø	

	BOOL m_bPickColor;

	DWORD m_dwColorMapHandle;
	DWORD m_dwColorMapWidth,m_dwColorMapHeight;
	DWORD m_dwPickX,m_dwPickY;

	DWORD m_dwOldColor;

public:
	WndEditLightColor(void);
	~WndEditLightColor(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsOK() { return m_bIsOK;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	virtual void Update();
	virtual void Render();

	DWORD GetLightColor() { return m_pBtnLightColor->GetColor();}

	void  SetLightColor(DWORD dwColor) { return m_pBtnLightColor->SetColor(dwColor);}
};