#pragma once

#include "..\..\frostengine\ui.h"


class RadioButton;

class WndNewMap :
	public ui::Wnd
{

	class ColorButton : public ui::Button
	{
		DWORD m_dwColor;
	public:
		virtual void Render();

		void  SetColor(DWORD dwColor) { m_dwColor = dwColor;}
		DWORD GetColor() { return m_dwColor;}
	};
protected:
	ui::Static    *   m_pStaticFogStart;
	ui::Static    *   m_pStaticFogEnd;
	ui::EditBox   *   m_pEditBoxFogStart;
	ui::EditBox   *   m_pEditBoxFogEnd;
	ui::ScrollBar *   m_pSBFogStart;
	ui::ScrollBar *   m_pSBFogEnd;

	RadioButton     *m_pRadioButton;

	ui::Static	  *   m_pWidthStatic;
	ui::Static	  *   m_pDepthStatic;


	ui::Static  * m_pStaticAmbient;	
	ui::Static  * m_pStaticDirectional;
	ui::Static  * m_pStaticFog;

	ColorButton		* m_pBtnAmbient;
	ColorButton		* m_pBtnDirectional;
	ColorButton		* m_pBtnFog;

	ui::Combo	* m_pComboWidth;
	ui::Combo	* m_pComboDepth;
	ui::Combo   * m_pComboTileSet;

	ui::Button  * m_pBtnOK;
	ui::Button  * m_pBtnCancel;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø	

	enum PICKTYPE{ PICK_AMBIENT,PICK_DIRECTIONAL,PICK_FOG};
	BOOL m_bPickColor;
	enum PICKTYPE m_ePickType;
	DWORD m_dwColorMapHandle;
	DWORD m_dwColorMapWidth,m_dwColorMapHeight;
	DWORD m_dwPickX,m_dwPickY;

	DWORD m_dwShaderGroupHandle;
	DWORD m_dwDefaultTextureID;

public:
	WndNewMap(void);
	~WndNewMap(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsOK() { return m_bIsOK;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	virtual void Update();
	virtual void Render();

	int   GetWidth(); 
	int   GetDepth();
	int   GetTileSetIndex();
	DWORD GetDefaultTextureID();

	void  ChangeTileSet(DWORD dwIndex);

	DWORD GetAmbientLightColor() { return m_pBtnAmbient->GetColor();}
	DWORD GetSunLightColor() { return m_pBtnDirectional->GetColor(); }
	DWORD GetFogColor() { return m_pBtnFog->GetColor(); }

	void  SetAmbientLightColor(DWORD dwColor) { return m_pBtnAmbient->SetColor(dwColor);}
	void  SetSunLightColor(DWORD dwColor)	  { return m_pBtnDirectional->SetColor(dwColor);}
	void  SetFogColor(DWORD dwColor) { return m_pBtnFog->SetColor(dwColor);}
	float GetFogStart();
	float GetFogEnd();
};
