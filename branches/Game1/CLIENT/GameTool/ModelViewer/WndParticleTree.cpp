#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"
#include "WndModelShaderTree.h"
#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndParticleTree.h"
#include "WndParticleModify.h"
#include <fstream>
WndParticleTree::WndParticleTree(void)
{
	m_pParticleTreeView = NULL;			//纹理列表
	strcpy(m_pszParticleFilePathName , " ");
}
WndParticleTree::~WndParticleTree(void)
{

}

BOOL WndParticleTree::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTreeView = {5, 20, 235 , 420 };
	m_pParticleTreeView = new ui::TreeView;
	m_pParticleTreeView->Create("ParticleTree", &rcTreeView, FWS_VISIBLE , this);

	return TRUE;
}
void WndParticleTree::Destroy()
{
	SAFEDESTROY(m_pParticleTreeView);
	ui::Wnd::Destroy();
}

void WndParticleTree::CreateParticleTree(const char * strParticleFilePathName)
{
	strcpy(m_pszParticleFilePathName,strParticleFilePathName);

	utility::File	file;
	file.Open(m_pszParticleFilePathName);

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

	m_pParticleTreeView->SetRootNode(pRootNode);
	m_pParticleTreeView->AddNode(pGroupNode,pRootNode);

	script.Destroy();
}

void WndParticleTree::Update()
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
				WndParticleModify *pWndPartModify = pWndViewer->GetWndParticleModify();
				ui::TreeView::Node *pCurNode = m_pParticleTreeView->GetSelNode();
				pWndPartModify->SetCurPartNode(pCurNode);
				pWndPartModify->CaculateWhilchToDisplay();
			}
		}
	}
}

void WndParticleTree::Render()
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
		sTitle = "粒子文件名:";
		char ParticleName[128] = "";
		strcpy(ParticleName , utility::File::GetFileName(m_pszParticleFilePathName));
		sTitle += ParticleName;
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

void WndParticleTree::SaveParticleFile()
{
	ui::TreeView::Node *pRootNode = m_pParticleTreeView->GetRootNode();
	ofstream stream;
	stream.open(m_pszParticleFilePathName);
	string strBuffer;
	if (stream.is_open())
	{	
		m_pParticleTreeView->ExportToScript(&strBuffer,pRootNode);
		stream<<strBuffer.c_str();
		stream.close();
	}
}