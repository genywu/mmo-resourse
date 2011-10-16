#include "stdafx.h"
#include "wndfogadjust.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"

WndFogAdjust::WndFogAdjust(void)
{

	m_pStaticFogEnable = NULL;
	m_pStaticFogStart=NULL;
	m_pStaticFogEnd=NULL;
	m_pEditBoxFogStart=NULL;
	m_pEditBoxFogEnd=NULL;
	m_pSBFogStart=NULL;
	m_pSBFogEnd=NULL;


	m_pBtnOK=NULL;
	m_pChkFogEnable=NULL;
	m_pBtnFog=NULL;

	m_bIsOK = FALSE;
	m_bIsPopupCheckFlag=FALSE;//检查弹出结束
	m_bPickColor = FALSE;
}

WndFogAdjust::~WndFogAdjust(void)
{
}

BOOL WndFogAdjust::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//create control
	RECT rcFogStartStatic = { 10,10,40,30};
	m_pStaticFogStart = new ui::Static;
	m_pStaticFogStart->Create("开始",&rcFogStartStatic,FWS_VISIBLE ,this);

	

	RECT rcFogStart= { 50,10,240,30};
	m_pSBFogStart = new ui::ScrollBar;
	m_pSBFogStart->Create("开始:",&rcFogStart,FWS_VISIBLE | FWS_OUTLINE ,this);
	m_pSBFogStart->SetRange(30);
	m_pSBFogStart->SetPos(15);

	RECT rcEditBoxFogStart = { 250,10,295,30};
	m_pEditBoxFogStart = new ui::EditBox;
	m_pEditBoxFogStart->Create("BoxStart",&rcEditBoxFogStart,FWS_VISIBLE | FWS_OUTLINE | FWS_DISABLE,this);

	RECT rcFogEndStatic = { 10,45,40,65};
	m_pStaticFogEnd = new ui::Static;
	m_pStaticFogEnd->Create("结束",&rcFogEndStatic,FWS_VISIBLE ,this);

	RECT rcFogEnd = { 50,45,240,65};
	m_pSBFogEnd = new ui::ScrollBar;
	m_pSBFogEnd->Create("结束",&rcFogEnd,FWS_VISIBLE | FWS_OUTLINE ,this);
	m_pSBFogEnd->SetRange(200);
	m_pSBFogEnd->SetPos(30);

	RECT rcEditBoxFogEnd = { 250,45,295,65};
	m_pEditBoxFogEnd = new ui::EditBox;
	m_pEditBoxFogEnd->Create("BoxEnd",&rcEditBoxFogEnd,FWS_VISIBLE | FWS_OUTLINE | FWS_DISABLE,this);


	RECT rcOK = { 40,75,90,100};
	m_pBtnOK = new ui::Button;
	m_pBtnOK->Create("确定",&rcOK,FWS_VISIBLE,this);

	RECT rcCancel = { 100,75,210,100};
	m_pChkFogEnable = new ui::CheckBox;
	m_pChkFogEnable->Create("EnableFog",&rcCancel,FWS_VISIBLE| FWS_OUTLINE ,this);

	RECT rcFogEnableStatic = { 160,75,200,100};
	m_pStaticFogEnable = new ui::Static;
	m_pStaticFogEnable->Create("开启雾",&rcFogEnableStatic,FWS_VISIBLE|FWS_DISABLE ,this);

	RECT rcFog = { 220,70,250,100};
	m_pBtnFog = new ColorButton;
	m_pBtnFog->Create("FogColor",&rcFog,FWS_VISIBLE,this);
	m_pBtnFog->SetColor(0xFFFFFFFF);

	render::Interface * pInteface = render::Interface::GetInstance();
	m_dwColorMapHandle = pInteface->CreateTexture("ui\\color.tga");
	render::Texture   * pTexture  = pInteface->GetTexture(m_dwColorMapHandle );
	m_dwColorMapWidth  = pTexture->GetWidth(); 
	m_dwColorMapHeight = pTexture->GetHeight(); 

	return TRUE;
}

void WndFogAdjust::Destroy()
{
	render::Interface * pInteface = render::Interface::GetInstance();
	pInteface->DestroyTexture(m_dwColorMapHandle);
	SAFEDESTROY(m_pStaticFogStart);
	SAFEDESTROY(m_pStaticFogEnable);
	SAFEDESTROY(m_pStaticFogEnd);
	SAFEDESTROY(m_pEditBoxFogStart);
	SAFEDESTROY(m_pEditBoxFogEnd);
	SAFEDESTROY(m_pSBFogStart);
	SAFEDESTROY(m_pSBFogEnd);
	SAFEDESTROY(m_pBtnOK);
	SAFEDESTROY(m_pChkFogEnable);
	SAFEDESTROY(m_pBtnFog);
	ui::Wnd::Destroy();
}


DWORD WndFogAdjust::GetFogStart()
{
	char szTemp[128];
    m_pEditBoxFogStart->GetEditText(szTemp,128);
	return (DWORD)atoi(szTemp);
}
DWORD WndFogAdjust::GetFogEnd()
{
	char szTemp[128];
	m_pEditBoxFogEnd->GetEditText(szTemp,128);
	return (DWORD)atoi(szTemp);
}
void WndFogAdjust::SetFogEnable(BOOL enable)
{
	m_pChkFogEnable->SetChecked(enable);
}
void WndFogAdjust::SetFogColor(DWORD dwColor)
{
	m_pBtnFog->SetColor(dwColor);
}

void WndFogAdjust::SetFogStart(DWORD dwStart)
{
	m_pSBFogStart->SetPos(dwStart);
}
void WndFogAdjust::SetFogEnd(DWORD dwEnd)
{
	m_pSBFogEnd->SetPos(dwEnd);
}

void WndFogAdjust::BeginPopUp()
{
	m_pBtnFog->SetColor(0xFF008AFF);
	m_bIsPopupCheckFlag = TRUE;
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndFogAdjust::EndPopUp(void)
{
	ModifyStyle(0,FWS_VISIBLE);
	
	ui::Manager::GetInstance()->EndPopupWnd();
}

void WndFogAdjust::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();
	
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	render::Interface *pInterface = render::Interface::GetInstance();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		m_pSBFogStart->SetRange(m_pSBFogEnd->GetPos());

		char szTemp[128] = {0};
		m_pEditBoxFogStart->Clear();
		_snprintf(szTemp,127,"%d",m_pSBFogStart->GetPos());
		m_pEditBoxFogStart->InsertText(0,szTemp,lstrlen(szTemp));
		m_pEditBoxFogEnd->Clear();
		_snprintf(szTemp,127,"%d",m_pSBFogEnd->GetPos());
		m_pEditBoxFogEnd->InsertText(0,szTemp,lstrlen(szTemp));

		if (m_bPickColor)
		{
			if (pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData == MK_LBUTTON)
			{
				m_bPickColor = FALSE;
			}
			else
			{
				//检查颜色
				DWORD px = (DWORD)pInputInfo->ptMouse.x;
				DWORD py = (DWORD)pInputInfo->ptMouse.y;
				if (px > m_dwPickX && py > m_dwPickY &&
					px < m_dwPickX + m_dwColorMapWidth && 
					py < m_dwPickX + m_dwColorMapHeight)
				{
					//计算位置
					int iPosX = px - m_dwPickX;
					int iPosY = py - m_dwPickY;
					//取得颜色
					render::Texture   * pTexture  = pInterface->GetTexture(m_dwColorMapHandle);
					DWORD dwColor = pTexture->GetPixel(iPosX,iPosY);					
					m_pBtnFog->SetColor(dwColor);
				}
			}
			
		}
		else
		{
			if (m_pBtnOK->IsClicked())
			{
				EndPopUp();
				m_bIsOK = TRUE;
				m_bIsPopupCheckFlag = FALSE;
			}
			if (m_pBtnFog->IsClicked())
			{
				m_bPickColor = TRUE;
			}
			
			ui::Wnd::UpdateChild();
		}		
	}
}

void WndFogAdjust::Render()
{
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
		//NO CHILD
		ui::Wnd::RenderChild();	

		RECT &rc = m_tWindowInfo.rcRect;
		if (m_bPickColor)
		{
			RECT rcRect;			
			m_pBtnFog->GetWndRect(&rcRect);			

			m_dwPickX = rcRect.right; 
			m_dwPickY = rcRect.bottom;

			pLayer2D->DrawTexture(m_dwPickX,m_dwPickY,m_dwColorMapHandle);
		}
	}				   
}

void WndFogAdjust::ColorButton::Render()
{
	//NO DRAW DEFAULT
	ui::Wnd::RenderDefault();

	render::Interface *pInterface = render::Interface::GetInstance();
	ASSERT(pInterface);

	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	ASSERT(pLayer2D);
	
	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc = m_tWindowInfo.rcRect;
		
		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_dwColor);	
	}
	//NO CHILD
}