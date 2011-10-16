#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "AppClient\RoleModel.h"
#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndPart.h"
#include "WndModel.h"
#include "DlgSelectLevel.h"

WndPart::WndPart(void)
{
}

WndPart::~WndPart(void)
{	
}

BOOL WndPart::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	ZeroMemory(m_szCurrentGroupName, 10);
	m_iCurrentSelModelIndex = 0;
	m_eCurrentModelType = MT_ROLE;

	//create control
	RECT rcListBoxLevel= { 4, 16, 100 - 4, 373 };
	m_pListBoxPart = new ui::ListBox;
	m_pListBoxPart->Create("LevelListBox", &rcListBoxLevel, FWS_VISIBLE | FWS_LBS_CHECK, this);

	return TRUE;
}

void WndPart::Destroy()
{
	SAFEDESTROY(m_pListBoxPart);
	ui::Wnd::Destroy();
}

void WndPart::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pListBoxPart->IsSelChanged())
		{
			WndViewer * pWndViewer = (WndViewer *)GetParent();
			WndSelectLevel * pWndSelectLevel = pWndViewer->GetWndSelectLevel();
			WndModel	* pWndModel = pWndViewer->GetWndModel();
			int			iSelItem   = m_pListBoxPart->GetSelItem();
			BOOL		bChecked   = m_pListBoxPart->GetItemChecked(iSelItem);
			DWORD		dwGroup	   = 0;
			DWORD		dwPart	   = 0;
			strncpy((char *)&dwGroup, m_szCurrentGroupName, 4);
			strncpy((char*)&dwPart, m_pListBoxPart->GetItemText(iSelItem), 4);
			CDisplayModel * pModel;

			if (m_eCurrentModelType == MT_ROLE)
			{
				pModel = pWndViewer->GetModel();

				if (bChecked)
				{
					pModel->ShowGroup(dwGroup, dwPart, pModel->GetLvlOfGroupPart(dwGroup, dwPart), false);

					GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(pModel->GetGameModelID());

					GameModel::Group	   * pGroup = pGM->GetGroup(dwGroup);
					GameModel::Group::Part * pPart  = pGroup->GetPart(dwPart);
					pWndSelectLevel->AddLevel(dwGroup, dwPart, pPart->GetLevelCount());
					pWndSelectLevel->ShowWnd(true);

					pWndViewer->SetLevelModelIndex(pWndModel->GetSelRoleModel());
					pWndViewer->SetLevelModelType(MT_ROLE);
				}else
				{
					pModel->HideGroup(dwGroup);
					pWndSelectLevel->ShowWnd(false);
				}
			}else if (m_eCurrentModelType == MT_BIND)
			{
				pModel = pWndViewer->GetCurrentSelBindModel();

				if (bChecked)
				{
					pModel->ShowGroup(dwGroup, dwPart, pModel->GetLvlOfGroupPart(dwGroup, dwPart), false);

					GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(pModel->GetGameModelID());

					GameModel::Group	   * pGroup = pGM->GetGroup(dwGroup);
					GameModel::Group::Part * pPart  = pGroup->GetPart(dwPart);
					pWndSelectLevel->AddLevel(dwGroup, dwPart, pPart->GetLevelCount());
					pWndSelectLevel->ShowWnd(true);

					pWndViewer->SetLevelModelIndex(pWndModel->GetSelBindModel());
					pWndViewer->SetLevelModelType(MT_BIND);
				}else
				{
					pModel->HideGroup(dwGroup);
					pWndSelectLevel->ShowWnd(false);
				}
			}

			for(int iItem = 0; iItem < m_pListBoxPart->GetItemCount(); iItem++)
				m_pListBoxPart->SetItemChecked(iItem, FALSE);

			m_pListBoxPart->SetItemChecked(iSelItem, bChecked);
		}

		ui::Wnd::UpdateChild();		
	}
}

void WndPart::Render()
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
		ui::Wnd::SetUIRendState();
		ui::Wnd::RenderDefault();
	
		render::Interface::Layer2D * pLayer2D = render::Interface::GetInstance()->GetLayer2D();

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left, iY = rc.top;//,iW = rc.right - rc.left + 1,;
		sTitle += m_szCurrentGroupName;
		sTitle += "部件扩展列表";
		int iTextLen = (int)sTitle.size();

		RECT rcLB;
		m_pListBoxPart->GetWndRect(&rcLB);
		pLayer2D->DrawWireQuadRect(rcLB.left ,rcLB.top,rcLB.right,rcLB.bottom,FWC_HIGH);
		ui::Wnd::SetUIRendState();
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

/*DWORD WndPart::GetActionCode(int iActionIndex)
{
	DWORD dwCode = 0;
	strncpy((char*)&dwCode, m_pListBoxAction->GetItemText(iActionIndex), 4);

	return dwCode;
}

DWORD WndPart::GetSelActionCode()
{
	DWORD dwCode   = 0;
	int	  iSelItem = m_pListBoxAction->GetSelItem();
	strncpy((char*)&dwCode, m_pListBoxAction->GetItemText(iSelItem), 4);

	return dwCode;
}

void WndPart::SetActionItemCheck()
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
}*/

void WndPart::AddPart(const char * szName)
{
	m_pListBoxPart->InsertItem(szName);
}

void WndPart::ClearPart()
{
	m_pListBoxPart->DeleteAllItem();
}

/*BOOL WndPart::IsActionChange(void)
{
	return m_pListBoxAction->IsSelChanged();
}

void WndPart::NextAction(void)
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

void WndPart::SelAction(DWORD dwName)
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
	

   
}*/
