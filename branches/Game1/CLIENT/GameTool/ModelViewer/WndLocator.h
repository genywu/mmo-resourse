#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndLocator : public ui::Wnd
{
	char		   m_szCurrentSelBindModelName[50];		//当前选择的绑定模型名字
	int			   m_iCurrentSelBindModelIndex;			//当前选择的绑定模型索引
	ui::ListBox  * m_pListBoxLocator;

public:
	WndLocator(void);
	~WndLocator(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	virtual void Update();
	virtual void Render();

	const char * GetCurrentSelBindModelName()					{ return m_szCurrentSelBindModelName; }
	void  SetCurrentSelBindModelName(const char * pszName)		{ strcpy_s(m_szCurrentSelBindModelName, 50, pszName); }
	int	  GetCurrentSelBindModelIndex()							{ return m_iCurrentSelBindModelIndex; }
	void  SetCurrentSelBindModelIndex(int iSelIndex)			{ m_iCurrentSelBindModelIndex = iSelIndex; }
	DWORD GetSelLocator()										{ return m_pListBoxLocator->GetSelItem(); }
	DWORD GetItemCount()										{ return m_pListBoxLocator->GetItemCount(); }
	void  SetItemChecked(int iItem, bool bValue)				{ m_pListBoxLocator->SetItemChecked(iItem, bValue); }			//设定列表选项点击状态

	void AddLocator(const char * szName);
	void ClearLocator();
	void ClearAllItemChecked();
};