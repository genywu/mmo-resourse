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
#include "wndPartTree.h"
#include "WndModelFileModify.h"
#include "WndPartModify.h"
#include "WndShaderModify.h"
#include "WndTrailTree.h"
#include "WndParticleTree.h"
#include <fstream>

WndModelTree::WndModelTree()
{
	m_pModelTreeView = NULL;			//纹理列表
	strcpy(m_pszModelFilePathName , "");
	strcpy(m_pszModelName, "" );
}

WndModelTree::~WndModelTree()
{

}


void WndModelTree::Update()
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
			WndViewer  * pWndViewer	= (WndViewer *)GetParent();
			WndPartTree *pWndPartTree = pWndViewer->GetWndPartTree();
			WndModelTexture *pModelTexture = pWndViewer->GetWndModelShaderTree();
			WndShaderModify *pWndShaderModify = pWndViewer->GetWndShaderModify();
			WndPartModify *pWndPartModify = pWndViewer->GetWndPartModify();
			WndTrailTree  *pWndTrailTree = pWndViewer->GetWndTrailTree();
			WndParticleTree   *pWndParticleTree = pWndViewer->GetWndParticleTree();
			ui::TreeView::Node *pCurNode = m_pModelTreeView->GetSelNode();
			if(pCurNode)
			{
				const char *name = pCurNode->GetName();
				const char *value = pCurNode->GetValue();
				ui::TreeView::Node *pCurNodeParent = pCurNode->GetParent();
				ui::TreeView::Node *pNodeGrandfather = NULL;
				ui::TreeView::Node *pNodeGreatGrandfather = NULL;
				ui::TreeView::Node *pNodeGGreatGrandfather = NULL;
				if(pCurNodeParent)
				{
					pNodeGrandfather = pCurNodeParent->GetParent();
					if(pNodeGrandfather)
						pNodeGreatGrandfather = pNodeGrandfather->GetParent();
					if (pNodeGreatGrandfather)
						pNodeGGreatGrandfather = pNodeGreatGrandfather->GetParent();
					////////////////////////////////////////
					if(::PtInRect(&m_tWindowInfo.rcRect, pInputInfo->ptMouse))
					{
						WndModelModify *pWndModelModify = pWndViewer->GetWndModelModify();
						const char *pParentName = pCurNodeParent->GetName();
						const char *GrandFatherName = NULL;
						if (pNodeGrandfather)
							GrandFatherName = pNodeGrandfather->GetName();
						const char *GreatGrandFatherName = NULL;
						if (pNodeGreatGrandfather)
							GreatGrandFatherName = pNodeGreatGrandfather->GetName();

						pModelTexture->ShowWnd(false);		//显示shader树形窗口
						pWndPartTree->ShowWnd(false);
						pWndModelModify->ShowWnd(false);
						pWndPartModify->ShowWnd(false);
						pWndShaderModify->ShowWnd(false);
						pWndTrailTree->ShowWnd(false);
						pWndParticleTree->ShowWnd(false);
						if( strcmp( pParentName, "shader" ) == 0 && strcmp( name, "file" ) == 0 )
						{
							char ShaderFilePathName[256];
							utility::File::GetFilePath(m_pszModelFilePathName,ShaderFilePathName);
							strcat(ShaderFilePathName,"\\");
							strcat(ShaderFilePathName,value);
							
							if(strcmp(ShaderFilePathName,pModelTexture->GetShaderFilePathName())!=0)
							{
								pModelTexture->SetCurrentModelName(m_pszModelName);
								pModelTexture->CreateShaderTree(ShaderFilePathName);
							}
							pModelTexture->ShowWnd(true);		//显示shader树形窗口
						}
						else if (GreatGrandFatherName && strcmp( GreatGrandFatherName, "group" ) == 0 && strcmp( name, "file" ) == 0)
						{
							char PartFilePathName[256];
							utility::File::GetFilePath(m_pszModelFilePathName,PartFilePathName);
							strcat(PartFilePathName,"\\");
							strcat(PartFilePathName,value);
							if (strcmp(pWndViewer->GetWndPartTree()->GetPartFilePathName(), PartFilePathName) != 0)
							{				
								pWndPartTree->CreatePartTree(PartFilePathName);
								pWndPartTree->ShowWnd(true);
							}
							if (!pWndPartTree->IsVisible())
								pWndPartTree->ShowWnd(true);
						}
						else if (GrandFatherName && strcmp(GrandFatherName,"trail_emitter") == 0 && GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0 && strcmp(name,"trail") == 0)
						{
							char TrailFilePathName[256];
							utility::File::GetFilePath(m_pszModelFilePathName,TrailFilePathName);
							strcat(TrailFilePathName,"\\");
							strcat(TrailFilePathName,value);

							if (strcmp(TrailFilePathName,pWndTrailTree->GetTrailFilePathName()) != 0)
							{							
								pWndTrailTree->CreateTrailTree(TrailFilePathName);
							}	
							pWndTrailTree->ShowWnd(true);
						}
						else if(strcmp(name,"particle") == 0 && GrandFatherName && strcmp(GrandFatherName,"particle_emitter") == 0 && GreatGrandFatherName && strcmp(GreatGrandFatherName,"main") == 0)
						{
							char PaticleFilePathName[256];
							utility::File::GetFilePath(m_pszModelFilePathName,PaticleFilePathName);
							strcat(PaticleFilePathName,"\\");
							strcat(PaticleFilePathName,value);
							const char * strFilePathName = pWndParticleTree->GetParticleFilePathName();
							if (strcmp(PaticleFilePathName,strFilePathName) != 0)
							{							
								pWndParticleTree->CreateParticleTree(PaticleFilePathName);
							}	
							pWndParticleTree->ShowWnd(true);
						}
						else
						{
							pWndModelModify->SetNode(pCurNode);
							pWndModelModify->CaculateWhichToDisplay();
						}
					}
				}
			}
		}
	}
}
void WndModelTree::Render()
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
		sTitle += m_pszModelName;
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
BOOL WndModelTree::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTreeView = {5, 20, 235 , 420 };
	m_pModelTreeView = new ui::TreeView;
	m_pModelTreeView->Create("Shader列表", &rcTreeView, FWS_VISIBLE , this);

	return TRUE;
}

void WndModelTree::Destroy()
{
	SAFEDESTROY(m_pModelTreeView);
	ui::Wnd::Destroy();
}


void WndModelTree::CreateModelTree(const char * ModelFilePathName)
{
	strcpy(m_pszModelFilePathName,ModelFilePathName);

	utility::File	file;
	file.Open(ModelFilePathName);

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

	m_pModelTreeView->SetRootNode(pRootNode);
	m_pModelTreeView->AddNode(pGroupNode,pRootNode);

	script.Destroy();
}

void WndModelTree::SaveModelFile()
{
	ui::TreeView::Node *pRootNode = m_pModelTreeView->GetRootNode();
	ofstream stream;
	stream.open(m_pszModelFilePathName);
	string strBuffer;
	if (stream.is_open())
	{	
		m_pModelTreeView->ExportToScript(&strBuffer,pRootNode);
		stream<<strBuffer.c_str();
		stream.close();
	}
}