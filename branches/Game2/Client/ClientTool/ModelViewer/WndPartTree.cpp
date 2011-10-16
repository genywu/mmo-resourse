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
#include "WndPartTree.h"
#include "WndPartModify.h"
#include <fstream>

WndPartTree::WndPartTree()
{
	m_pPartTreeView = NULL;			//纹理列表
	m_pNode = NULL;
	strcpy(m_pszPartFilePathName , "");
}

WndPartTree::~WndPartTree()
{

}

void WndPartTree::SetCurPartNode(ui::TreeView::Node *pNode)
{
	if (pNode == m_pNode)
		return;
	m_pNode = pNode;
}
void WndPartTree::Update()
{
	//UPDATE DEFAULT
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
				WndPartModify *pWndPartModify = pWndViewer->GetWndPartModify();
				ui::TreeView::Node *pCurNode = m_pPartTreeView->GetSelNode();
				pWndPartModify->SetCurPartNode(pCurNode);
				pWndPartModify->CaculateToDisplay();
			}
		}
	}
}
void WndPartTree::Render()
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
		sTitle = "模型:";
		char m_pszModelName[128] = "";
		strcpy(m_pszModelName , utility::File::GetFileName(m_pszPartFilePathName));
		sTitle += m_pszModelName;
		int iTextLen = (int)sTitle.size();

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
BOOL WndPartTree::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTreeView = {5, 20, 275 , 420 };
	m_pPartTreeView = new ui::TreeView;
	m_pPartTreeView->Create("PartTree", &rcTreeView, FWS_VISIBLE , this);

	return TRUE;
}

void WndPartTree::Destroy()
{
	SAFEDESTROY(m_pPartTreeView);
	ui::Wnd::Destroy();
}


void WndPartTree::CreatePartTree(const char * strPartFilePathName)
{
	strcpy(m_pszPartFilePathName,strPartFilePathName);

	utility::File	file;
	file.Open(m_pszPartFilePathName);

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

	m_pPartTreeView->SetRootNode(pRootNode);
	m_pPartTreeView->AddNode(pGroupNode,pRootNode);

	script.Destroy();
}

void WndPartTree::SavePartFile()
{
	ui::TreeView::Node *pRootNode = m_pPartTreeView->GetRootNode();
	ofstream stream;
	stream.open(m_pszPartFilePathName);
	string strBuffer;
	if (stream.is_open())
	{	
		m_pPartTreeView->ExportToScript(&strBuffer,pRootNode);
		stream<<strBuffer.c_str();
		stream.close();
	}
}