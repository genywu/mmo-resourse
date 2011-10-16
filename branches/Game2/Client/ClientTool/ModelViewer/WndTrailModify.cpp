#include "stdafx.h"
#include "WndTrailModify.h"
#include "WndTrailTree.h"
#include "WndViewer.h"

WndTrailModify::WndTrailModify(void)
{
	m_pEditBox = NULL;
	m_pBtnConfirm = NULL;
	m_pBtnCancel = NULL;
	m_pNode = NULL;
}
WndTrailModify::~WndTrailModify(void)
{

}

BOOL WndTrailModify::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rect = {20, 24, 20+145 , 24+25 };
	m_pEditBox = new ui::EditBox;
	m_pEditBox->Create("编辑",&rect,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);

	RECT rcConfirm = {10, 100, 10 + 80, 100+25};
	m_pBtnConfirm = new ui::Button;		//确定保存
	m_pBtnConfirm->Create("确定/保存",&rcConfirm, FWS_VISIBLE, this);

	RECT rcCancel = {100, 100, 100 + 80, 100+25};
	m_pBtnCancel = new ui::Button;		//取消
	m_pBtnCancel->Create("取消",&rcCancel, FWS_VISIBLE, this);
	return TRUE;
}
void WndTrailModify::Destroy()
{
	SAFEDESTROY(m_pEditBox);
	SAFEDESTROY(m_pBtnConfirm);
	SAFEDESTROY(m_pBtnCancel);
	m_pNode = NULL;
	ui::Wnd::Destroy();
}

void WndTrailModify::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();		
		if(pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
		{
			if(!m_pNode)
				return;
			RECT rect;
			GetWndRect(&rect);
			if (::PtInRect(&rect,pInputInfo->ptMouse))
			{			
				m_pBtnConfirm->GetWndRect(&rect);
				if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnConfirm->IsVisible())
				{
					WndViewer  * pWndViewer	= (WndViewer *)GetParent();
					WndTrailTree *pWndTrailTree = pWndViewer->GetWndTrailTree();
					char strValue[128];
					m_pEditBox->GetEditText(strValue,127);
					if (strlen(strValue)>0)
					{
						m_pNode->SetValue(strValue);
						pWndTrailTree->SaveTrailFile();
					}
					ShowWnd(false);
				}

				m_pBtnCancel->GetWndRect(&rect);
				if(::PtInRect(&rect,pInputInfo->ptMouse) && m_pBtnCancel->IsVisible())
				{
					ShowWnd(false);
				}
			}
		}
	}
}
void WndTrailModify::Render()
{
	string sTitle;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
		if(m_pNode)
		{
			string sTitle = " 编辑:";
			sTitle	 += m_pNode->GetName();
			if(strcmp(m_pNode->GetValue(),"")!=0)
			{
				sTitle += " = ";
				sTitle += m_pNode->GetValue();
			}
			render::Interface *pInterface = render::Interface::GetInstance();
			render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

			RECT &rc = m_tWindowInfo.rcRect;

			int iX = rc.left, iY = rc.top;//,iW = rc.right - rc.left + 1,;

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
		}
		ui::Wnd::RenderChild();	
	} 
}

void WndTrailModify::CaculateWhichToDisplay()
{
	if (m_pNode == NULL)
		return;
	ShowWnd(false);
	const char * name = m_pNode->GetName();
	if (strcmp(name,"timelength") == 0 || strcmp(name,"top_color") == 0 || strcmp(name,"bottom_color") == 0)
	{
		m_pEditBox->ShowWnd(true);
		m_pBtnConfirm->ShowWnd(true);
		m_pBtnCancel->ShowWnd(true);
		ShowWnd(true);
	}
}