#include "stdafx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndModelShaderTree.h"
#include "WndModel.h"
#include "WndShaderModify.h"
#include <fstream>
	#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
WndModelTexture::WndModelTexture()
{
	m_pTextureTree = NULL;			//纹理列表
	strcpy(m_pszModelName , "");
}

WndModelTexture::~WndModelTexture()
{

}

void WndModelTexture::Update()
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
			WndShaderModify *pWndShaderModify = pWndViewer->GetWndShaderModify();
			if(pWndShaderModify)
			{
				ui::TreeView::Node *pNode = m_pTextureTree->GetSelNode();
				if(pNode && ::PtInRect(pNode->GetContentRect(), pInputInfo->ptMouse))
				{
					pWndShaderModify->SetCurNode(pNode);
					pWndShaderModify->CaculateWhichToDisplay();
					return;
				}
			}
		}
	}
}
void WndModelTexture::Render()
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
		sTitle = "模型(";
		sTitle += m_pszModelName;
		sTitle += ")Shader列表";
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
BOOL WndModelTexture::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcTreeView = {0, 16, 235 , 16+400 };
	m_pTextureTree = new ui::TreeView;
	m_pTextureTree->Create("Shader列表", &rcTreeView, FWS_VISIBLE , this);

	return TRUE;
}

void WndModelTexture::Destroy()
{
	SAFEDESTROY(m_pTextureTree);
	ui::Wnd::Destroy();
}

void WndModelTexture::CreateShaderTree(const char * ModelFilePathName)
{
	strcpy(m_pszShaderFileName,ModelFilePathName);

	utility::File	file;
	file.Open(m_pszShaderFileName);
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

	m_pTextureTree->SetRootNode(pRootNode);
	m_pTextureTree->AddNode(pGroupNode,pRootNode);

	script.Destroy();
}

void WndModelTexture::SaveToShaderFile()
{
	ui::TreeView::Node *pRootNode = m_pTextureTree->GetRootNode();
	ofstream stream;
	stream.open(m_pszShaderFileName);
	string strBuffer;
	if (stream.is_open())
	{	
		m_pTextureTree->ExportToScript(&strBuffer,pRootNode);
		string fileHeader = "//Frost Shader File 1.0\n//-----------------------------------------------------\n//config comment:\n//twoside:[true]/[false]\n//zwrite:[true]/[false]\n//alphatest: [none] [transparent]\n//alphablend:[additive] [alpha] [color] [reserved]\n//Op:        [Disable] [Selectarg1] [Selectarg2] [Modulate] [Modulate2x]\n//           [Modulate4x] [Add] [Addsigned] [Addsigned2x] [SUbtract][Addsmooth]\n//           [Blenddiffusealpha] [Blendtexturealpha] [BLendfactoralpha] \n//           [Blendtexturealphapm],[Blendcurrentalpha] [PReModulate]\n//           [Modulatealpha_addcolor] [Modulatecolor_addalpha] [Modulateinvalpha_addcolor]\n//           [Modulateinvcolor_addalpha] [BUmpenvmap] [Bumpenvmapluminance] [Dotproduct3]\n//           [Multiplyadd],[Lerp]\n//Arg:       [Diffuse] [Current] [Texture] [Tfactor] [Specular] [Temp]\n//           [Modulate2x] [Modulate4x] [subtract]\n//-----------------------------------------------------";
		stream<<fileHeader.c_str()<<endl;
		stream<<strBuffer.c_str();
		stream.close();
	}
}