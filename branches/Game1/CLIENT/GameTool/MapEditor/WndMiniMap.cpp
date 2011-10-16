#include "stdafx.h"

#include "..\..\frostengine\render.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "map.h"
#include "commui.h"
#include "commdb.h"
#include "editwnd.h"
#include "wndminimap.h"


WndMiniMap::WndMiniMap(void)
{
	m_dwReferenceTextureHandle = 0;
	m_dwShowMapStatus = 0;
}

WndMiniMap::~WndMiniMap(void)
{
}

BOOL WndMiniMap::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	//½¨Á¢´°¿Ú
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcBtn = { 205,222,217,234};
	m_pBtnShowMapStatus = new ui::Button;
	m_pBtnShowMapStatus->Create("=",&rcBtn,FWS_VISIBLE,this);

	render::Interface *pInterface = render::Interface::GetInstance();
	m_dwMapTextureHandle = pInterface->CreateTexture(256,256,0xFFFFFFFF);
	ASSERT(m_dwMapTextureHandle);

	return TRUE;
}

void WndMiniMap::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->DestroyTexture(m_dwMapTextureHandle);
	
	SAFEDESTROY(m_pBtnShowMapStatus);

	ui::Wnd::Destroy();
}

void WndMiniMap::Update()
{
	//UPDATE DEFAULT
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();		
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();
		//°áÒÆÉãÏñ»ú
		if (bActive)
		{
			if (pInputInfo->dwMouseButtonState & MK_LBUTTON && !(pInputInfo->byKeyBuffer[DIK_LALT] & 0x80))
			{
				//int   iX = m_tWindowInfo.rcRect.left;
				//int   iY = m_tWindowInfo.rcRect.top;
				//float fW = (float)(m_tWindowInfo.rcRect.right  - m_tWindowInfo.rcRect.left);
				//float fH = (float)(m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top);

				EditMap *pMap = CommDB::GetInstance()->GetMap();
				EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
				render::Camera *pCamera = pEditWnd->GetCamera();
				D3DXVECTOR3 *pPosition = pCamera->GetPosition(); 
				float fMapWidth = (float)pMap->GetWidth();
				float fMapDepth = (float)pMap->GetDepth();

				//¼ì²éÊó±ê·¶Î§
				if (pInputInfo->ptMouse.x < m_iMiniMapX || pInputInfo->ptMouse.x > m_iMiniMapX + m_iMiniMapWidth ||
					pInputInfo->ptMouse.y < m_iMiniMapY || pInputInfo->ptMouse.y > m_iMiniMapY + m_iMiniMapHeight)
				{
				}
				else
				{
					int iCX = pInputInfo->ptMouse.x - m_iMiniMapX;
					int iCY = pInputInfo->ptMouse.y - m_iMiniMapY;

					float fOX = (float)iCX / (float)m_iMiniMapWidth * fMapWidth;		
					float fOZ = (float)(m_iMiniMapHeight - 1 - iCY) / (float)m_iMiniMapHeight * fMapDepth;
					pCamera->GetView()->x += - pCamera->GetPosition()->x + fOX;
					pCamera->GetPosition()->x = fOX;
					pCamera->GetView()->z += - pCamera->GetPosition()->z + fOZ;
					pCamera->GetPosition()->z = fOZ;
				}
			}
		}
		if (m_pBtnShowMapStatus->IsClicked())
		{
			m_dwShowMapStatus ++;
			if (m_dwShowMapStatus > 2)
				m_dwShowMapStatus = 0;
		}
	}
}

void WndMiniMap::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		int   iX = m_tWindowInfo.rcRect.left;
		int   iY = m_tWindowInfo.rcRect.top;
		int   iW = m_tWindowInfo.rcRect.right  - m_tWindowInfo.rcRect.left;
		int   iH = m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;

		//»­ÉãÏñ»ú
		EditMap *pMap = CommDB::GetInstance()->GetMap();
		EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
		render::Camera *pCamera = pEditWnd->GetCamera();
		D3DXVECTOR3 *pPosition = pCamera->GetPosition(); 
		D3DXVECTOR3 *pView     = pCamera->GetView(); 
		float fMapWidth = (float)pMap->GetWidth();
		float fMapDepth = (float)pMap->GetDepth();

		float fCX = pPosition->x / fMapWidth * (float)m_iMiniMapWidth;
		float fCY = (fMapDepth - pPosition->z) / fMapDepth * (float)m_iMiniMapHeight;
		
		D3DXVECTOR2 v;
		v.x = pView->x - pPosition->x;
		v.y = pView->z - pPosition->z;
		D3DXVec2Normalize(&v,&v);

		int iEX =   (int)(v.x * 12.0f);
		int iEY = - (int)(v.y * 12.0f);

		pLayer2D->DrawSolidQuad(iX,iY,iW,iH,0xe0112233);
		switch(m_dwShowMapStatus)
		{
		case 0://È«»­
			if (m_dwReferenceTextureHandle)
			{
				pLayer2D->DrawTexture(m_iMiniMapX,m_iMiniMapY,m_iMiniMapWidth,m_iMiniMapHeight,m_dwReferenceTextureHandle,0XFFFFFFFF);
			}
			pLayer2D->DrawTexture(m_iMiniMapX,m_iMiniMapY,m_iMiniMapWidth,m_iMiniMapHeight,m_dwMapTextureHandle,0X80FFFFFF);
			break;
		case 1://Ö»»­²Î¿¼
			if (m_dwReferenceTextureHandle)
			{
				pLayer2D->DrawTexture(m_iMiniMapX,m_iMiniMapY,m_iMiniMapWidth,m_iMiniMapHeight,m_dwReferenceTextureHandle,0XFFFFFFFF);
			}
			break;
		case 2://Ö»»­µØÍ¼
			pLayer2D->DrawTexture(m_iMiniMapX,m_iMiniMapY,m_iMiniMapWidth,m_iMiniMapHeight,m_dwMapTextureHandle,0X80FFFFFF);
			break;
		}

		DrawArrow(m_iMiniMapX + (int)fCX,m_iMiniMapY + (int)fCY,m_iMiniMapX + (int)fCX+ iEX,m_iMiniMapY + (int)fCY + iEY,0xFFFFFF00);
		pLayer2D->DrawWireQuad(m_iMiniMapX + (int)fCX - 4 ,m_iMiniMapY + (int)fCY - 4 ,8,8,0xFFFFFFFF);		                         

	}

	ui::Wnd::RenderChild();
}

void WndMiniMap::DrawArrow(int iX0,int iY0,int iX1,int iY1,DWORD dwColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();

	float slopy , cosy , siny;
	float Par = 8.0f;  //length of Arrow (>)
	slopy = atan2f( (float)( iY0 - iY1 ),
		(float)( iX0 - iX1 ) );
	cosy = cosf( slopy );
	siny = sinf( slopy ); 

	pLayer2D->DrawLine(iX0,iY0,iX1,iY1,dwColor);

	pLayer2D->DrawLine(iX1,iY1,iX1 + int( Par * cosy - ( Par / 2.0f * siny ) ),iY1 + int( Par * siny + ( Par / 2.0f * cosy ) ),dwColor);
	pLayer2D->DrawLine(iX1 + int( Par * cosy + Par / 2.0f * siny ),iY1 - int( Par / 2.0f * cosy - Par * siny ),iX1,iY1,dwColor);
  
}

void WndMiniMap::SetMapPixel(DWORD dwGridIndex,DWORD dwColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();

	int x = dwGridIndex % dwMapWidth;
	int z = (dwMapDepth -1  -  dwGridIndex / dwMapWidth);

	render::Texture *pTexture = pInterface->GetTexture(m_dwMapTextureHandle);

	int iTexWidth = pTexture->GetWidth();
	int iTexHeight= pTexture->GetHeight();

	int mx = iTexWidth * x / dwMapWidth;
	int my = iTexHeight * z / dwMapDepth;    

	int sizex = iTexWidth / dwMapWidth + 1;
	int sizey = iTexHeight / dwMapDepth + 1;                                 

	for(int i = 0; i < sizex; i++)
	{
		for(int j = 0; j < sizey; j++)
		{
			if (mx + i < iTexWidth && my + j < iTexHeight) 
				pTexture->SetPixel(mx + i,my + j,dwColor);
		}
	}
}

void WndMiniMap::UpdateMiniPixel()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Texture *pTexture = pInterface->GetTexture(m_dwMapTextureHandle);
	pTexture->CopyBitmapToTexture();
}

void WndMiniMap::CalculateMiniMapPosition()
{
	int   iX = m_tWindowInfo.rcRect.left;
	int   iY = m_tWindowInfo.rcRect.top;
	int   iW = m_tWindowInfo.rcRect.right  - m_tWindowInfo.rcRect.left;
	int   iH = m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;

	//»­ÉãÏñ»ú
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	int   iMapWidth = (int)pMap->GetWidth();
	int   iMapDepth = (int)pMap->GetDepth();

	float fRatio(1.0f);
	float fRatioW = (float)iW / (float)iMapWidth;
	float fRatioH = (float)iH / (float)iMapDepth;

	if (fRatioW < fRatioH)
	{
		m_iMiniMapWidth = iW;
		m_iMiniMapHeight = (int)((float)iMapDepth * fRatioW);
	}
	else
	{
		m_iMiniMapWidth = (int)((float)iMapWidth * fRatioH);
		m_iMiniMapHeight = iH;
	}

	m_iMiniMapX = iX + iW / 2 - m_iMiniMapWidth / 2;
	m_iMiniMapY = iY + iH / 2 - m_iMiniMapHeight / 2;
}

void WndMiniMap::LoadReference(const char *pszFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (m_dwReferenceTextureHandle)                                 
	{
	   pInterface->DestroyTexture(m_dwReferenceTextureHandle);
	}
	m_dwReferenceTextureHandle = pInterface->CreateTexture(pszFileName);
}

DWORD WndMiniMap::NewReference()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (m_dwReferenceTextureHandle)                                 
	{
	   pInterface->DestroyTexture(m_dwReferenceTextureHandle);
	}
	m_dwReferenceTextureHandle = pInterface->CreateTexture(256,256,0xFFFFFFF);
	return m_dwReferenceTextureHandle;
}