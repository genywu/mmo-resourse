#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\ui.h"
#include "..\..\frostengine\render.h"

#include "commui.h"
#include "NewMapDlg.h"
#include "EditWnd.h"


IMPLEMENT_SINGLETON(CommUI);

CommUI::CommUI(void)
{
}

CommUI::~CommUI(void)
{
}

void CommUI::Create(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
    
	int iWidth = pInterface->GetWndWidth();
	int iHeight= pInterface->GetWndHeight();

	RECT rc= {0,0,iWidth,iHeight};
	
	m_pEditWnd = new EditWnd;
	//FWS_DEBUG | FWS_VISIBLE | FWS_OUTLINE | FWS_SIZE | FWS_DRAG,NULL);
	m_pEditWnd->Create("编辑窗口",&rc,FWS_VISIBLE,NULL);
	
	//生成界面
	ui::Manager::GetInstance()->SetMainWnd(m_pEditWnd);



}


void CommUI::Destroy(void)
{
	ui::Manager::GetInstance()->SetMainWnd(NULL);

	SAFEDESTROY(m_pEditWnd);
}
