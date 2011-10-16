#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"

#include "gamesetting.h"
#include "DlgSelectLevel.h"
#include "WndViewer.h"
#include "WndAction.h"
#include "AppClient\RoleModel.h"

WndSelectLevel::WndSelectLevel(void)
{
	m_dwGroupName = 0;
	m_dwPartName = 0;
}

WndSelectLevel::~WndSelectLevel(void)
{
	
}

BOOL WndSelectLevel::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//create control
	RECT rcListBoxLevel= { 4, 28, 100 - 4, 28 + 126 };
	m_pListBoxLevel = new ui::ListBox;
	m_pListBoxLevel->Create("LevelListBox",&rcListBoxLevel,FWS_VISIBLE  ,this);

	return TRUE;
}

void WndSelectLevel::Destroy()
{
	SAFEDESTROY(m_pListBoxLevel);
	ui::Wnd::Destroy();
}

void WndSelectLevel::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();
	
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	render::Interface *pInterface = render::Interface::GetInstance();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pListBoxLevel->IsSelChanged())
		{
			//设置不同的级别
		}
		ui::Wnd::UpdateChild();
	}
}

void WndSelectLevel::Render()
{
	//NO DRAW DEFAULT
	render::Interface *pInterface = render::Interface::GetInstance();
	ASSERT(pInterface);

	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	ASSERT(pLayer2D);

	char szGroup[5],szPart[5];

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		DWORD &dwStyle = m_tWindowInfo.dwStyle;
	
		render::Interface::Layer2D *pLayer2D = render::Interface::GetInstance()->GetLayer2D();

		int iFontSize = pLayer2D->GetFontSize();

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		memcpy(szGroup,(char*)&m_dwGroupName,4);
		szGroup[4] = 0;
		memcpy(szPart,(char*)&m_dwPartName,4);
		szPart[4] = 0;

		WndViewer  * pWndViewer  = (WndViewer *)GetParent();
		int			 iModelIndex = pWndViewer->m_iLevelModelIndex;
		string		 sModelName;
		switch (pWndViewer->m_eLevelModelType)
		{
		case MT_ROLE:
			pWndViewer->GetModelName(sModelName, pWndViewer->m_vRoleModels[iModelIndex]->GetFilePathName());
			break;
		case MT_BIND:
			pWndViewer->GetModelName(sModelName, pWndViewer->m_vBindModels[iModelIndex]->GetFilePathName());
			break;
		}

		string sTmp = "模型" + sModelName + "级别列表";
		sTmp += "\n";
		sTmp += szGroup;
		sTmp += "::";
		sTmp += szPart;
		int iTextLen = (int)sTmp.size();

		if (iTextLen > 2)
		{
			int iStartX = iX + 3;
			int iStartY = iY + 3;
			pLayer2D->TextOut(iStartX + 1,iStartY + 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY - 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX + 1,iStartY - 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY + 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX + 1,iStartY    ,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY    ,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX    ,iStartY - 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX    ,iStartY + 1,sTmp.c_str(),iTextLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX,iStartY,sTmp.c_str(),iTextLen,FWC_WHITE);		
		}

		RECT rcLB;
		m_pListBoxLevel->GetWndRect(&rcLB);

		pLayer2D->DrawWireQuadRect(rcLB.left,rcLB.top,rcLB.right,rcLB.bottom,FWC_HIGH);

		//NO CHILD
		ui::Wnd::RenderChild();	
	}
}

void WndSelectLevel::AddLevel(DWORD dwGroupName,DWORD dwPartName,int iLevelCount)
{
	char str[256];
	m_pListBoxLevel->DeleteAllItem();
	for(int i = 0; i < iLevelCount; i++)
	{
        sprintf(str,"级别%d",i);
		m_pListBoxLevel->InsertItem(str);
	}
	m_pListBoxLevel->SetSelItem(0);
	m_dwGroupName = dwGroupName;
	m_dwPartName  = dwPartName;
}

BOOL WndSelectLevel::IsSelLevelChange(void)
{
	return m_pListBoxLevel->IsSelChanged();
}
