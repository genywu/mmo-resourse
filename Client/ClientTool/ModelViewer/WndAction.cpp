#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "AppClient\RoleModel.h"
#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndAction.h"

WndAction::WndAction(void)
{
}

WndAction::~WndAction(void)
{	
}

BOOL WndAction::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	ZeroMemory(m_szCurrentModelName, 10);
	m_iCurrentSelModelIndex = 0;
	m_eCurrentActionType = AT_ROLE;

	//create control
	RECT rcListBoxLevel= { 4, 16, 100 - 4, 373 };
	m_pListBoxAction = new ui::ListBox;
	m_pListBoxAction->Create("LevelListBox",&rcListBoxLevel,FWS_VISIBLE | FWS_LBS_CHECK,this);

	return TRUE;
}

void WndAction::Destroy()
{
	SAFEDESTROY(m_pListBoxAction);
	ui::Wnd::Destroy();
}

void WndAction::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pListBoxAction->IsSelChanged())
		{
			WndViewer * pWndViewer = (WndViewer *)GetParent();
			int			iSelItem   = m_pListBoxAction->GetSelItem();
			BOOL		bChecked   = m_pListBoxAction->GetItemChecked(iSelItem);
			DWORD		dwCode	   = 0;
			strncpy((char*)&dwCode, m_pListBoxAction->GetItemText(iSelItem), 4);

			if (m_eCurrentActionType == AT_ROLE)
			{
				if (bChecked)
				{
					AnimInfo::tagActionInfo	* pActionInfo = pWndViewer->m_cAnimInfo.GetActionInfo();
					pActionInfo->dwCurAction	= dwCode;
					pActionInfo->bInterpolation = FALSE;

					CRoleModel		  * pRoleModel = pWndViewer->m_vRoleModels[m_iCurrentSelModelIndex];
					GameModel::Action * pAction	   = CGameModelManager::GetInstance()->GetActionAnim(pWndViewer->m_pModel->GetGameModelID(), pActionInfo->dwCurAction);
					if (pAction && pAction->GetAnimJoint())
						pWndViewer->m_pWndAnimModify->SetTimeLength(pAction->GetAnimJoint()->GetTimeLength());

					pRoleModel->SetCurrentSelActionIndex(iSelItem);
					pWndViewer->m_pWndAnimModify->ShowWnd(TRUE);
				}else
				{
					pWndViewer->m_cAnimInfo.GetActionInfo()->dwCurAction = 0;
					pWndViewer->m_pWndAnimModify->ShowWnd(FALSE);
					pWndViewer->m_vRoleModels[m_iCurrentSelModelIndex]->SetCurrentSelActionIndex(-1);
				}
			}else if (m_eCurrentActionType == AT_BIND)
			{
				CBindModel * pBindModel = pWndViewer->m_vBindModels[m_iCurrentSelModelIndex];

				if (bChecked)
				{
					pBindModel->ChangeStatus(dwCode);
					pBindModel->SetCurrentSelActionIndex(iSelItem);
				}else
				{
					pBindModel->ChangeStatus(0);
					pBindModel->SetCurrentSelActionIndex(-1);
				}

				pWndViewer->m_pWndAnimModify->ShowWnd(false);
			}

			for(int iItem = 0; iItem < m_pListBoxAction->GetItemCount(); iItem++)
				m_pListBoxAction->SetItemChecked(iItem, FALSE);

			m_pListBoxAction->SetItemChecked(iSelItem, bChecked);
		}
		ui::Wnd::UpdateChild();		
	}
}

void WndAction::Render()
{
	string sTitle;

	//NO DRAW DEFAULT
	render::Interface *pInterface = render::Interface::GetInstance();
	ASSERT(pInterface);

	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	ASSERT(pLayer2D);

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
	
		render::Interface::Layer2D * pLayer2D = render::Interface::GetInstance()->GetLayer2D();

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left, iY = rc.top;//,iW = rc.right - rc.left + 1,;
		sTitle = "模型";
		sTitle += m_szCurrentModelName;
		sTitle += "动作列表";
		int iTextLen = (int)sTitle.size();

		RECT rcLB;
		m_pListBoxAction->GetWndRect(&rcLB);
		pLayer2D->DrawWireQuadRect(rcLB.left ,rcLB.top,rcLB.right,rcLB.bottom,FWC_HIGH);

		pLayer2D->TextOut(iX + 1 + 4,iY + 1 + 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4,iY + 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX+ 4,iY + 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX+ 4,iY - 1+ 2,sTitle.c_str(),iTextLen,FWC_BLACK);
		pLayer2D->TextOut(iX+ 4,iY+ 2,sTitle.c_str(),iTextLen,FWC_WHITE);

		//NO CHILD
		ui::Wnd::RenderChild();	
	}
}

DWORD WndAction::GetActionCode(int iActionIndex)
{
	DWORD dwCode = 0;
	strncpy((char*)&dwCode, m_pListBoxAction->GetItemText(iActionIndex), 4);

	return dwCode;
}

DWORD WndAction::GetSelActionCode()
{
	DWORD dwCode   = 0;
	int	  iSelItem = m_pListBoxAction->GetSelItem();
	strncpy((char*)&dwCode, m_pListBoxAction->GetItemText(iSelItem), 4);

	return dwCode;
}

void WndAction::SetActionItemCheck()
{
	int			iSelAction = 0;
	WndViewer * pWndViewer = (WndViewer *)GetParent();

	switch (m_eCurrentActionType)
	{
	case AT_ROLE:
		iSelAction = pWndViewer->m_vRoleModels[m_iCurrentSelModelIndex]->GetCurrentSelActionIndex();
		break;
	case AT_BIND:
		iSelAction = pWndViewer->m_vBindModels[m_iCurrentSelModelIndex]->GetCurrentSelActionIndex();
		break;
	}

	if (iSelAction != -1)
		m_pListBoxAction->SetItemChecked(iSelAction, true);
}

void WndAction::AddAction(const char *szName)
{
	m_pListBoxAction->InsertItem(szName);
}

void WndAction::ClearAction()
{
	m_pListBoxAction->DeleteAllItem();
}

BOOL WndAction::IsActionChange(void)
{
	return m_pListBoxAction->IsSelChanged();
}

void WndAction::NextAction(void)
{
	if (!m_pListBoxAction->GetItemCount())
		return ;

	int iSelItem = m_pListBoxAction->GetSelItem();
	int iNextItem = iSelItem+1;
	if (iNextItem >= m_pListBoxAction->GetItemCount())
	{
		iNextItem = 0;
	}
	m_pListBoxAction->SetSelItem(iNextItem);

    DWORD code = 0;
	strncpy((char*)&code,m_pListBoxAction->GetItemText(iNextItem),4);

	WndViewer *pWndViewer = (WndViewer *)GetParent();
	AnimInfo::tagActionInfo *pActionInfo = pWndViewer->m_cAnimInfo.GetActionInfo();
	pActionInfo->dwCurAction = code;		
	pActionInfo->bInterpolation = FALSE;

	pWndViewer->m_pWndAnimModify->ShowWnd(TRUE);
	//GameModel::Action *pAction = pWndViewer->m_pModel->GetAction(code);
	GameModel::Action * pAction = CGameModelManager::GetInstance()->GetActionAnim(pWndViewer->m_pModel->GetGameModelID(), pActionInfo->dwCurAction);
	if (pAction && pAction->GetAnimJoint())
		pWndViewer->m_pWndAnimModify->SetTimeLength(pAction->GetAnimJoint()->GetTimeLength());

	for(int  iItem = 0; iItem < m_pListBoxAction->GetItemCount(); iItem++)
	{
		m_pListBoxAction->SetItemChecked(iItem,FALSE);
	}

	m_pListBoxAction->SetItemChecked(iNextItem,TRUE);
}

void WndAction::SelAction(DWORD dwName)
{
	if (!m_pListBoxAction->GetItemCount())
		return ;

	
	for(int  iItem = 0; iItem < m_pListBoxAction->GetItemCount(); iItem++)
	{
		if (memcmp(&dwName,m_pListBoxAction->GetItemText(iItem),4) == 0)
		{
			//found
			m_pListBoxAction->SetSelItem(iItem);
			WndViewer *pWndViewer = (WndViewer *)GetParent();
			AnimInfo::tagActionInfo *pActionInfo = pWndViewer->m_cAnimInfo.GetActionInfo();
			pActionInfo->dwCurAction = dwName;		
			pActionInfo->bInterpolation = FALSE;

			pWndViewer->m_pWndAnimModify->ShowWnd(TRUE);
			//GameModel::Action *pAction = pWndViewer->m_pModel->GetAction(dwName);
			GameModel::Action * pAction = CGameModelManager::GetInstance()->GetActionAnim(pWndViewer->m_pModel->GetGameModelID(), pActionInfo->dwCurAction);
			if (pAction && pAction->GetAnimJoint())
				pWndViewer->m_pWndAnimModify->SetTimeLength(pAction->GetAnimJoint()->GetTimeLength());

			for(int  i = 0; i < m_pListBoxAction->GetItemCount(); i++)
			{
				m_pListBoxAction->SetItemChecked(i,FALSE);
			}

			m_pListBoxAction->SetItemChecked(iItem,TRUE);
		}
	}
	

   
}
