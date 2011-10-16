#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndParticleModify: public ui::Wnd
{
	char				m_pszParticleFilePathName[128];	//particle路径文件名
	ui::TreeView::Node	*m_pNode;

	ui::CheckBox	*m_Check[2];
	ui::Button		*m_pBtnConfirm;		//确定保存
	ui::Button		*m_pBtnCancel;		//取消
	
	ui::EditBox		*m_pEditBox;		//编辑框
public:
	WndParticleModify(void);
	~WndParticleModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();

	void SetCurPartNode(ui::TreeView::Node *pNode);
	void CaculateWhilchToDisplay();
};