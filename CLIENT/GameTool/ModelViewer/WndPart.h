#pragma once
#include "..\..\frostengine\ui.h"

class WndViewer;

class WndPart : public ui::Wnd
{
	char		   m_szCurrentGroupName[50];	//当前显示的模型组名字
	int			   m_iCurrentSelModelIndex;		//当前选择的模型索引
	eModelType	   m_eCurrentModelType;			//当前显示模型的类型
	ui::ListBox  * m_pListBoxPart;

public:
	WndPart(void);
	~WndPart(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	void Destroy();
	
	void BeginPopUp(void);
	void EndPopUp(void);

	virtual void Update();
	virtual void Render();

	const char  * GetCurrentGroupName()							{ return m_szCurrentGroupName; }
	eModelType	  GetCurrentModelType()							{ return m_eCurrentModelType; }
/*	DWORD		  GetActionCode(int iActionIndex);
	DWORD		  GetSelAction()								{ return m_pListBoxAction->GetSelItem();}
	DWORD		  GetSelActionCode();*/
	void		  SetCurrentGroupName(const char * pszName)		{ strcpy_s(m_szCurrentGroupName, 50, pszName); }
/*	void		  SetCurrentModelIndex(int iIndex)				{ m_iCurrentSelModelIndex = iIndex; }*/
	void		  SetCurrentModelType(eModelType eType)			{ m_eCurrentModelType = eType; }
/*	void		  SetActionItemCheck();*/

	void  AddPart(const char *szName);
	void  ClearPart();

/*	BOOL IsActionChange(void);
	void NextAction(void);
	void SelAction(DWORD dwName);*/
};