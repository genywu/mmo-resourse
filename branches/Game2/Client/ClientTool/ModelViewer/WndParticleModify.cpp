#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndParticleTree.h"
#include "WndParticleModify.h"
#include "WndModelShaderTree.h"
#include "WndModelTree.h"

WndParticleModify::WndParticleModify(void)
{
	m_pNode = NULL;

	m_Check[0] = NULL;
	m_Check[1] = NULL;
	m_pBtnConfirm = NULL;		//确定保存
	m_pBtnCancel = NULL;		//取消
	m_pEditBox = NULL;		//编辑框
}
WndParticleModify::~WndParticleModify(void)
{

}

BOOL WndParticleModify::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rect = {20, 24, 20+145 , 24+25 };
	m_Check[0] = new ui::CheckBox;
	m_Check[0]->Create("true",&rect,FWS_VISIBLE | FWS_FILLBKG,this);
	
	RECT rect2 = {20,24+35,20+145,24+35+25};
	m_Check[1] = new ui::CheckBox;
	m_Check[1]->Create("false",&rect2,FWS_VISIBLE | FWS_FILLBKG,this);

	RECT rcComfirm = {10, 110, 10 + 80, 110+25};
	m_pBtnConfirm = new ui::Button;		//确定保存
	m_pBtnConfirm->Create("确定/保存",&rcComfirm, FWS_VISIBLE,this);

	RECT rcCancel = {100, 110, 100 + 80, 110+25};
	m_pBtnCancel = new ui::Button;		//取消
	m_pBtnCancel->Create("取消",&rcCancel, FWS_VISIBLE,this);

	m_pEditBox = new ui::EditBox;		//编辑框
	m_pEditBox->Create("编辑框",&rect,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE,this);
	return TRUE;
}
void WndParticleModify::Destroy()
{
	SAFEDESTROY(m_Check[0]);
	SAFEDESTROY(m_Check[1]);
	SAFEDESTROY(m_pBtnConfirm);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pEditBox);
	ui::Wnd::Destroy();
}

void WndParticleModify::Update()
{
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();		
		if(pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData & MK_LBUTTON)
		{
			RECT rect;
			this->GetWndRect(&rect);
			if(::PtInRect(&rect,pInputInfo->ptMouse))
			{
				WndViewer  * pWndViewer	= (WndViewer *)GetParent();
				WndParticleTree *pWndParticleTree = pWndViewer->GetWndParticleTree();

				const char *name = m_pNode->GetName();
				const char *value = m_pNode->GetValue();
				RECT rect;
				m_Check[0]->GetWndRect(&rect);
				if (::PtInRect(&rect,pInputInfo->ptMouse) && m_Check[0]->GetStyle())
				{
					int flag = m_Check[0]->IsChecked();
					m_Check[1]->SetChecked(!flag);
				}
				m_Check[1]->GetWndRect(&rect);
				if (::PtInRect(&rect,pInputInfo->ptMouse) && m_Check[1]->GetStyle())
				{
					int flag = m_Check[1]->IsChecked();
					m_Check[0]->SetChecked(!flag);
				}
				m_pBtnCancel->GetWndRect(&rect);
				if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnCancel->IsVisible())
				{
					ShowWnd(false);
				}
				m_pBtnConfirm->GetWndRect(&rect);
				if (::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnConfirm->IsVisible())
				{
					if (strcmp(name,"billboard") == 0 || strcmp(name,"billboard_y") == 0)
					{
						if (m_Check[0]->IsChecked())
							m_pNode->SetValue("true");
						else
							m_pNode->SetValue("false");
						pWndParticleTree->SaveParticleFile();
						ShowWnd(false);
					}
					else if (strcmp(name,"shader") != 0 && strcmp(name,"parameter") != 0 && 
						strcmp(name,"acceleration") != 0 && strcmp(name,"acceleration") != 0 && 
						strcmp(name,"pitch") != 0 && strcmp(name,"visibility") != 0 &&
						strcmp(name,"color") != 0 && strcmp(name,"emit") != 0 &&
						strcmp(name,"uvanim") != 0 && strcmp(name,"size") != 0 )
					{
						char strValue[128] = "";
						m_pEditBox->GetEditText(strValue,127);
						if (strlen(strValue) == 0)
						{
							MessageBox(NULL,"你没有输入任何东西","Error",MB_OK);
							return;
						}
						m_pNode->SetValue(strValue);
						m_pEditBox->Clear();
						pWndParticleTree->SaveParticleFile();
					}
				}
			}
		}
	}
}
void WndParticleModify::Render()
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
		sTitle = "编辑:";
		if(m_pNode)
		{
			sTitle += m_pNode->GetName();
			sTitle += " = ";
			sTitle += m_pNode->GetValue();
		}
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

void WndParticleModify::SetCurPartNode(ui::TreeView::Node *pNode)
{
	if (pNode == m_pNode)
		return;
	m_pNode = pNode;
}
void WndParticleModify::CaculateWhilchToDisplay()
{
	if (m_pNode == NULL)
		return;

	m_Check[0]->ShowWnd(false);
	m_Check[1]->ShowWnd(false);
	m_pBtnConfirm->ShowWnd(false);
	m_pBtnCancel->ShowWnd(false);
	m_pEditBox->ShowWnd(false);
	ShowWnd(false);
	const char *name = m_pNode->GetName();
	const char *value = m_pNode->GetValue();

	if (strcmp(name,"file") == 0)
	{
		
		WndViewer *pWndViewer = (WndViewer*)GetParent();
		WndModelTexture *pWndTextureTree = pWndViewer->GetWndModelShaderTree();
		WndParticleTree *pWndParticleTree = pWndViewer->GetWndParticleTree();
		WndModelTree	*pWndModelTree = pWndViewer->GetWndModelTree();
		char ShaderFilePathName[256];
		utility::File::GetFilePath(pWndParticleTree->GetParticleFilePathName(),ShaderFilePathName);
		strcat(ShaderFilePathName,"\\");
		strcat(ShaderFilePathName,value);
		if (strcmp(ShaderFilePathName,pWndTextureTree->GetShaderFilePathName()) != 0)
		{
			pWndTextureTree->SetCurrentModelName(pWndModelTree->GetCurModelName());
			pWndTextureTree->CreateShaderTree(ShaderFilePathName);
		}
		pWndTextureTree->ShowWnd(true);
		pWndParticleTree->ShowWnd(false);
		this->ShowWnd(false);
	}
	if (strcmp(name,"billboard") == 0 || strcmp(name,"billboard_y") == 0)
	{
		if (strcmp(value,"true") == 0)
		{
			m_Check[0]->SetChecked(true);
			m_Check[1]->SetChecked(false);
		}
		else
		{
			m_Check[0]->SetChecked(false);
			m_Check[1]->SetChecked(true);
		}
		m_Check[0]->ShowWnd(true);
		m_Check[1]->ShowWnd(true);
		m_pBtnConfirm->ShowWnd(true);
		m_pBtnCancel->ShowWnd(true);
		ShowWnd(true);
	}
	else if (strcmp(name,"shader") != 0 && strcmp(name,"parameter") != 0 && 
			 strcmp(name,"acceleration") != 0 && strcmp(name,"acceleration") != 0 && 
			 strcmp(name,"pitch") != 0 && strcmp(name,"visibility") != 0 &&
			 strcmp(name,"color") != 0 && strcmp(name,"emit") != 0 &&
			 strcmp(name,"uvanim") != 0 && strcmp(name,"size") != 0 && 
			 strcmp(name,"main") != 0 && strcmp(name,"file") != 0)
	{
		m_pEditBox->ShowWnd(true);
		m_pBtnConfirm->ShowWnd(true);
		m_pBtnCancel->ShowWnd(true);
		ShowWnd(true);
	}
}