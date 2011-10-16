#pragma once

#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\utility.h"

class WndSelectBuild : public ui::Wnd
{
	WORD m_wCurBuildID;

	DWORD m_dwCurTabLabel;

	ui::TabCtl  * m_pTabCtl;
	ui::ListBox * m_pListBoxBuild;

	utility::Text* m_pScriptBuild;	

public:
	WndSelectBuild(void);
	~WndSelectBuild(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);	
	void Destroy();
	
	virtual void Update();
	
	//从硬盘装载数据
	void RefreshBuildType();
	void RefreshFileListBox();

	WORD GetCurBuildID() { return m_wCurBuildID;}
	void Refresh(void);
	void Select(WORD wBuildID);
};
