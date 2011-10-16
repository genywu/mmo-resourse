#pragma once
#include "..\..\frostengine\ui.h"

class WndSelectLevel : public ui::Wnd
{

	ui::ListBox  * m_pListBoxLevel;

	DWORD m_dwGroupName,m_dwPartName;

public:
	WndSelectLevel(void);
	~WndSelectLevel(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	virtual void Update();
	virtual void Render();

	int   GetLevel() { return m_pListBoxLevel->GetSelItem();}
	void  SetLevel(int iLevel) { m_pListBoxLevel->SetSelItem(iLevel);}

	void  AddLevel(DWORD dwGroupName,DWORD dwPartName,int iLevelCount);

	DWORD GetGroupName() { return m_dwGroupName;}
	DWORD GetPartName()  { return m_dwPartName;}

	void  SetGroup(DWORD dwName) ;

	BOOL IsSelLevelChange(void);
};