#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndModel : public ui::Wnd
{
	ui::ListBox * m_pRoleModels;			//角色模型列表
	ui::ListBox * m_pBindModels;			//绑定模型列表

public:
	WndModel(void);
	~WndModel(void);

	BOOL Create(const char * pszName, const RECT * pRect, DWORD dwStyle, ui::Wnd * pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	virtual void Update();
	virtual void Render();

	DWORD GetSelRoleModel()								{ return m_pRoleModels->GetSelItem(); }
	DWORD GetSelBindModel()								{ return m_pBindModels->GetSelItem(); }
	DWORD GetItemCount()								{ return m_pBindModels->GetItemCount(); }
	void  SetItemChecked(int iItem, bool bValue)		{ m_pBindModels->SetItemChecked(iItem, bValue); }			//设定列表选项点击状态

	void AddRoleModel(const char * szModelName);
	void AddBindModel(const char * szModelName);
	void ClearRoleModels()								{ m_pRoleModels->DeleteAllItem(); }
	void ClearBindModels()								{ m_pBindModels->DeleteAllItem(); }
};