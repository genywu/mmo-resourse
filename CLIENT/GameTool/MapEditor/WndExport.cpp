#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"



#include "cmdmgr.h"
#include "EditWnd.h"
#include "wndExport.h"
#include "control.h"

extern const char *szChangePointType[];

WndExport::WndExport(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOk = FALSE;
}

WndExport::~WndExport(void)
{
}


BOOL WndExport::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	//建立窗口
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcSurface = {15,28,38,51};
	m_pCheckSurface = new CheckBox;
	m_pCheckSurface->Create("SURFACE",&rcSurface,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG  ,this);

	RECT rcHeightmap = {106,28,129,51};
	m_pCheckHeightmap = new CheckBox;
	m_pCheckHeightmap->Create("HEIGHTMAP",&rcHeightmap,FWS_VISIBLE| FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcColormap = {15,60,38,83};
	m_pCheckColormap = new CheckBox;
	m_pCheckColormap->Create("COLORMAP",&rcColormap,FWS_VISIBLE| FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcGrid = {106,60,129,83};
	m_pCheckGrid = new CheckBox;
	m_pCheckGrid->Create("GRID",&rcGrid,FWS_VISIBLE| FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcNode = {15,93,38,116};
	m_pCheckNode = new CheckBox;
	m_pCheckNode->Create("NODE",&rcNode,FWS_VISIBLE| FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcSound = {106,93,130,116};
	m_pCheckSound = new CheckBox;
	m_pCheckSound->Create("Sound",&rcSound,FWS_VISIBLE| FWS_OUTLINE | FWS_FILLBKG,this);

	RECT rcOk = {17,194,86,218};
	m_pBtnOk = new ui::Button;
	m_pBtnOk->Create("OK",&rcOk,FWS_VISIBLE,this);

	RECT rcCancel = {118,194,187,218};
	m_pBtnCancel = new ui::Button;
	m_pBtnCancel->Create("CANCEL",&rcCancel,FWS_VISIBLE,this);


	RECT rcStaticTitle = {15,2,187,22};
	m_pStaticTitle = new ui::Static;
	m_pStaticTitle->Create("导出地图",&rcStaticTitle,FWS_VISIBLE,this);



	return TRUE;
}

void WndExport::Destroy()
{
	SAFEDESTROY(m_pStaticTitle);
	SAFEDESTROY(m_pBtnOk);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pCheckSurface);
	SAFEDESTROY(m_pCheckHeightmap);
	SAFEDESTROY(m_pCheckGrid);
	SAFEDESTROY(m_pCheckColormap);
	SAFEDESTROY(m_pCheckNode);
	SAFEDESTROY(m_pCheckSound);

	//销毁窗体
	ui::Wnd::Destroy();
}

void WndExport::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pBtnOk->IsClicked())
		{
			EndPopUp();
			m_bIsOk = TRUE;
			m_bIsPopupCheckFlag= TRUE;
		}

		if (m_pBtnCancel->IsClicked())
		{
			EndPopUp();
			m_bIsOk = FALSE;
			m_bIsPopupCheckFlag = TRUE;
		}

		ui::Wnd::UpdateChild();
	}

}

void WndExport::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndExport::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}