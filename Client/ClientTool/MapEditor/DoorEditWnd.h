#pragma once
#include "..\..\engine\ui.h"
#include "changewnd.h"

class WndPopupDoorEdit :
	public ui::Wnd
{

	ui::ListBox * m_pListType;
	ui::EditBox * m_pEditRegionID;
	ui::EditBox * m_pEditX;
	ui::EditBox * m_pEditY;
	ui::ListBox * m_pListDirect;
	ui::Button  * m_pBtnOK;
	ui::Button  * m_pBtnCancel;
	ui::EditBox * m_pEditName;

	ui::Static  * m_pStaticType;
	ui::Static  * m_pStaticRegionID;
	ui::Static  * m_pStaticX;
	ui::Static  * m_pStaticY;
	ui::Static  * m_pStaticDirect;
	ui::Static  * m_pStaticName;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø
	

public:
	WndPopupDoorEdit(void);
	~WndPopupDoorEdit(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsOK() { BOOL bValue = m_bIsOK; m_bIsOK = FALSE;return bValue;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	virtual void Update();

	void GetDoor(tagDoor *pDoor); 
	void SetDoor(const tagDoor *pDoor);
};
