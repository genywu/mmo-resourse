#pragma once
#include "..\..\frostengine\ui.h"

class WndLightAdjust :
	public ui::Wnd
{

	class ColrBtn : public ui::Button
	{
		DWORD m_dwColor;
	public:
		virtual void Render();

		void  SetColor(DWORD dwColor) { m_dwColor = dwColor;}
		DWORD GetColor() { return m_dwColor;}
	};

	ui::Static  * m_pStaticAmbient;	
	ui::Static  * m_pStaticDirectional;
	ColrBtn		* m_pBtnAmbient;
	ColrBtn		* m_pBtnDirectional;
	ui::Button  * m_pBtnOK;
	ui::Button  * m_pBtnCancel;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø	

	enum PICKTYPE{ PICK_AMBIENT,PICK_DIRECTIONAL};
	BOOL m_bPickColor;
	enum PICKTYPE m_ePickType;
	DWORD m_dwColorMapHandle;
	DWORD m_dwColorMapWidth,m_dwColorMapHeight;
	DWORD m_dwPickX,m_dwPickY;

	DWORD m_dwOldAmbientColor;
	DWORD m_dwOldDirectionalColor;

public:
	WndLightAdjust(void);
	~WndLightAdjust(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsOK() { return m_bIsOK;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	virtual void Update();
	virtual void Render();

	DWORD GetAmbientColor() { return m_pBtnAmbient->GetColor();}
	DWORD GetDirectionalColor() { return m_pBtnDirectional->GetColor(); }

	void  SetAmbientColor(DWORD dwColor) { return m_pBtnAmbient->SetColor(dwColor);}
	void  SetDirectionalColor(DWORD dwColor) { return m_pBtnDirectional->SetColor(dwColor);}
};
