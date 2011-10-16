#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commui.h"
#include "commdb.h"
#include "map.h"
#include "cmdmgr.h"
#include "editwnd.h"
#include "dooreditwnd.h"
#include "wndrandomreplace.h"

extern const char *szChangePointType[];

WndRandomReplace::WndRandomReplace(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
}

WndRandomReplace::~WndRandomReplace(void)
{
}


BOOL WndRandomReplace::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	//建立窗口
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//建立列表框
	RECT rcListBox = {10,44,138,206};
	m_pListBox = new ui::ListBox;
	m_pListBox->Create("替换候选",&rcListBox,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	//添加按钮
	RECT rcAddRECT = {151,79,216,101};
	m_pBtnAdd = new ui::Button;
	m_pBtnAdd->Create("添加",&rcAddRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	//删除按钮
	RECT rcDelRECT = {150,116,216,138};
	m_pBtnDel= new ui::Button;
	m_pBtnDel->Create("删除",&rcDelRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG  ,this);

	//确定按钮
	RECT rcOkRECT = {45,225,111,246};
	m_pBtnOk= new ui::Button;
	m_pBtnOk->Create("替换",&rcOkRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	//建立按钮
	RECT rcCancelRECT = {129,225,195,246};
	m_pBtnCancel= new ui::Button;
	m_pBtnCancel->Create("取消",&rcCancelRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	//建立按钮
	RECT rcTargetRECT = {47,11,218,32};
	m_pEditTarget= new ui::EditBox;
	m_pEditTarget->Create("目标",&rcTargetRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	RECT rcSourceRECT = {151,44,217,65};
	m_pEditSource = new ui::EditBox;
	m_pEditSource->Create("源",&rcSourceRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	//建立按钮
	RECT rcMinDistanceRECT = {153,153,220,176};
	m_pEditMinDistance= new ui::EditBox;
	m_pEditMinDistance->Create("Min",&rcMinDistanceRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	RECT rcMaxDistanceRECT = {153,184,220,206};
	m_pEditMaxDistance = new ui::EditBox;
	m_pEditMaxDistance->Create("Max",&rcMaxDistanceRECT,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	return TRUE;
}

void WndRandomReplace::Destroy()
{
	//销毁控件
	SAFEDESTROY(m_pEditMinDistance);
	SAFEDESTROY(m_pEditMaxDistance);
	SAFEDESTROY(m_pEditSource);
	SAFEDESTROY(m_pEditTarget);
	SAFEDESTROY(m_pBtnOk);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pBtnDel);
	SAFEDESTROY(m_pBtnAdd);
	SAFEDESTROY(m_pListBox);

	//销毁窗体
	ui::Wnd::Destroy();
}

void WndRandomReplace::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pBtnAdd->IsClicked())
		{
			char szTemp[256];
			m_pEditSource->GetEditText(szTemp,256);
			if (lstrlen(szTemp))
			{
				m_pListBox->InsertItem(szTemp);
			}
			m_pEditSource->Clear();
		}

		if (m_pBtnDel->IsClicked())
		{
			//删除
			int iSelItem = m_pListBox->GetSelItem();
			if (iSelItem >= 0 )
			{
				m_pListBox->DeleteItem(iSelItem);
			}
		}

		if (m_pBtnOk->IsClicked())
		{
			EndPopUp();
			m_bIsOK = TRUE;
			m_bIsPopupCheckFlag= TRUE;
		}
		if (m_pBtnCancel->IsClicked())
		{
			EndPopUp();
			m_bIsOK = FALSE;
			m_bIsPopupCheckFlag = TRUE;
		}

		if (m_pListBox->IsSelChanged())
		{
			int iSelItem = m_pListBox->GetSelItem();
			if (iSelItem >= 0 )
			{
				//画模型
			}
		}

		ui::Wnd::UpdateChild();
	}

}

void WndRandomReplace::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndRandomReplace::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}