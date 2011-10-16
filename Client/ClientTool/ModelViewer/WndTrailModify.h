#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;
class WndTrailModify: public ui::Wnd
{
	ui::EditBox			*m_pEditBox;
	ui::Button			*m_pBtnConfirm;
	ui::Button			*m_pBtnCancel;
	ui::TreeView::Node  *m_pNode;
public:
	WndTrailModify(void);
	~WndTrailModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();
	void SetNode(ui::TreeView::Node *pNode) {if (pNode == m_pNode) return; m_pNode = pNode;}
	void CaculateWhichToDisplay();

};