#pragma once
#include "..\..\frostengine\ui.h"

enum eActionType
{
	AT_ROLE,	//角色模型的动作
	AT_BIND		//绑定模型的动作
};

class WndViewer;

class WndAction : public ui::Wnd
{
	char		   m_szCurrentModelName[50];	//当前显示的模型名字
	int			   m_iCurrentSelModelIndex;		//当前选择的模型索引
	eActionType	   m_eCurrentActionType;		//当前显示动作的类型
	ui::ListBox  * m_pListBoxAction;

public:
	WndAction(void);
	~WndAction(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	virtual void Update();
	virtual void Render();

	const char  * GetCurrentModelName()							{ return m_szCurrentModelName; }
	eActionType	  GetCurrentActionType()						{ return m_eCurrentActionType; }
	DWORD		  GetActionCode(int iActionIndex);
	DWORD		  GetSelAction()								{ return m_pListBoxAction->GetSelItem();}
	DWORD		  GetSelActionCode();
	void		  SetCurrentModelName(const char * pszName)		{ strcpy_s(m_szCurrentModelName, 50, pszName); }
	void		  SetCurrentModelIndex(int iIndex)				{ m_iCurrentSelModelIndex = iIndex; }
	void		  SetCurrentActionType(eActionType eType)		{ m_eCurrentActionType = eType; }
	void		  SetActionItemCheck();

	void  AddAction(const char *szName);
	void  ClearAction();

	BOOL IsActionChange(void);
	void NextAction(void);
	void SelAction(DWORD dwName);
};