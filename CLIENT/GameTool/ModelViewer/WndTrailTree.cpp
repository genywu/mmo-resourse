#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"
#include "WndModelShaderTree.h"
#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndModelTree.h"
#include "WndModel.h"
#include "WndTrailTree.h"
#include "WndPartModify.h"
#include "WndTrailModify.h"
#include <fstream>
WndTrailTree::WndTrailTree(void)
{
	m_pTrailTreeView = NULL;
}
WndTrailTree::~WndTrailTree(void)
{

}

BOOL WndTrailTree::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTreeView = {5, 20, 275 , 420 };
	m_pTrailTreeView = new ui::TreeView;
	m_pTrailTreeView->Create("TrailÁÐ±í", &rcTreeView, FWS_VISIBLE , this);
	return true;
}
void WndTrailTree::Destroy()
{
	SAFEDESTROY(m_pTrailTreeView);
	ui::Wnd::Destroy();
}

void WndTrailTree::CreateTrailTree(const char * trailFilePathName)
{
	strcpy(m_pszTrailFilePathName,trailFilePathName);

	utility::File	file;
	file.Open(m_pszTrailFilePathName);

	utility::Text script;
	script.Create(&file);
	file.Close();


	utility::Text::Node *pGroupNode = script.GetRootNode();
	ASSERT(pGroupNode);

	const char *Name = pGroupNode->GetName();
	ui::TreeView::Node *pRootNode = new ui::TreeView::Node;
	pRootNode->SetName(Name);
	pRootNode->SetParent(NULL);
	pRootNode->SetValue("");

	m_pTrailTreeView->SetRootNode(pRootNode);
	m_pTrailTreeView->AddNode(pGroupNode,pRootNode);

	script.Destroy();
}
void WndTrailTree::Update()
{
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();		
		if(pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData & MK_RBUTTON)
		{
			RECT rect;
			this->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse))
			{
				WndViewer  * pWndViewer	= (WndViewer *)GetParent();
				ui::TreeView::Node *pCurNode = m_pTrailTreeView->GetSelNode();
				WndTrailModify *pWndTrailModify = pWndViewer->GetWndTrailModify();
				pWndTrailModify->SetNode(pCurNode);
				pWndTrailModify->CaculateWhichToDisplay();
			}
		}
	}
}
void WndTrailTree::Render()
{
	string sTitle;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
		RECT &rc = m_tWindowInfo.rcRect;
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

		int iX = rc.left, iY = rc.top;//,iW = rc.right - rc.left + 1,;
		sTitle = "Trail:";
		char m_pszModelName[128] = "";
		strcpy(m_pszTrailName , utility::File::GetFileName(m_pszTrailFilePathName));
		sTitle += m_pszTrailName;
		int iTextLen = (int)sTitle.size();
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
		ui::Wnd::RenderChild();	
	} 
}

void WndTrailTree::SaveTrailFile()
{
	ui::TreeView::Node *pRootNode = m_pTrailTreeView->GetRootNode();
	ofstream stream;
	stream.open(m_pszTrailFilePathName);
	string strBuffer;
	if (stream.is_open())
	{	
		m_pTrailTreeView->ExportToScript(&strBuffer,pRootNode);
		stream<<strBuffer.c_str();
		stream.close();
	}
}