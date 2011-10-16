#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\gameengine\gamemodelmanager.h"

#include "gamesetting.h"
#include "wndviewer.h"
#include "animmodifydlg.h"
#include "WndLocator.h"

WndLocator::WndLocator(void)
{
}

WndLocator::~WndLocator(void)
{	
}

BOOL WndLocator::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	ZeroMemory(m_szCurrentSelBindModelName, 10);
	m_iCurrentSelBindModelIndex = 0;

	//create control
	RECT rcListBoxLevel= { 4, 16, 100 - 4, 16 + 282 };
	m_pListBoxLocator = new ui::ListBox;
	m_pListBoxLocator->Create("LocatorListBox", &rcListBoxLevel, FWS_VISIBLE | FWS_LBS_CHECK, this);

	return TRUE;
}

void WndLocator::Destroy()
{
	SAFEDESTROY(m_pListBoxLocator);
	ui::Wnd::Destroy();
}

void WndLocator::Update()
{
	// UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD & dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pListBoxLocator->IsSelChanged())
		{
			int			iSelItem   = m_pListBoxLocator->GetSelItem();
			BOOL		bChecked   = m_pListBoxLocator->GetItemChecked(iSelItem);
			WndViewer * pWndViewer = (WndViewer *)GetParent();

			// ÅÐ¶Ï°ó¶¨µãÇÐ»»
			switch (iSelItem)
			{
			case 0:			//ÐØ¿Ú
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(0);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','0'));
				break;
			case 1:			//Í·¶¥
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(1);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','1'));
				break;
			case 2:			//½Åµ×ÖÐÐÄ
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(2);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','2'));
				break;
			case 3:			//×óÊÖ
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(3);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','3'));
				break;
			case 4:			//ÓÒÊÖ
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(4);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','4'));
				break;
			case 5:			//×ó½Å
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(5);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','5'));
				break;
			case 6:			//ÓÒ½Å
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(6);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','6'));
				break;
			case 7:			//±³
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(7);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','7'));
				break;
			case 8:			//×óÑü
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(8);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','8'));
				break;
			case 9:			//ÓÒÑü
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(9);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','0','9'));
				break;
			case 10:		//×ó±Û
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(10);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','0'));
				break;
			case 11:		//ÓÒ±Û
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(11);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','1'));
				break;
			case 12:		//×óºó±³
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(12);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','2'));
				break;
			case 13:		//ÓÒºó±³
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(13);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','3'));
				break;
			case 14:		//ºó±³³¤½£
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(14);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','4'));
				break;
			case 15:		//ºó±³¹­
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(15);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','5'));
				break;
			case 16:		//ºó±³¼ý´ü
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(16);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','6'));
				break;
			case 17:		//ºó±³ÕÈ,¸«
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(17);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','7'));
				break;
			case 18:		//×óÈ­
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(18);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','8'));
				break;
			case 19:		//ÓÒÈ­
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(19);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','1','9'));
				break;
			case 20:		//×óÊÜÉËµã
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(20);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','2','1'));
				break;
			case 21:		//ÓÒÊÜÉËµã
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(21);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','2','2'));
				break;
			case 22:		//±§ÈËµã
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(22);
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetLocator(MAKEFOURCC('0','0','2','3'));
				break;
			case 23:		//×ø±êÔ­µã
				pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetCurrentSelLocatorIndex(23);
				break;
			}

			for (int iItem = 0; iItem < m_pListBoxLocator->GetItemCount(); ++iItem)
				m_pListBoxLocator->SetItemChecked(iItem, false);

			pWndViewer->m_vBindModels[m_iCurrentSelBindModelIndex]->SetVisible(bChecked);
			m_pListBoxLocator->SetItemChecked(iSelItem, bChecked);
		}

		ui::Wnd::UpdateChild();
	}
}

void WndLocator::Render()
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

		render::Interface::Layer2D *pLayer2D = render::Interface::GetInstance()->GetLayer2D();

		RECT & rc = m_tWindowInfo.rcRect;

		int iX = rc.left, iY = rc.top;
		sTitle = "Ä£ÐÍ";
		sTitle += m_szCurrentSelBindModelName;
		sTitle += "°ó¶¨µã";
		int iTextLen = (int)sTitle.size();

		RECT rcLB;
		m_pListBoxLocator->GetWndRect(&rcLB);
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

void WndLocator::AddLocator(const char *szName)
{
	m_pListBoxLocator->InsertItem(szName);
}

void WndLocator::ClearLocator()
{
	m_pListBoxLocator->DeleteAllItem();
}

void WndLocator::ClearAllItemChecked()
{
	for (int i = 0; i < m_pListBoxLocator->GetItemCount(); ++i)
		SetItemChecked(i, false);
}