#include "StdAfx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\render.h"

#include "Control.h"
#include "newmapdlg.h"
#include "CommDB.h"
#include "CommUI.h"
#include "EditWnd.h"
#include "TerrainMesh.h"
#include "SetTileTool.h"
#include "TerrainTileSetWnd.h"
#include "WndMiniMap.h"

WndNewMap::WndNewMap(void)
{
	m_dwShaderGroupHandle = 0xFFFFFFFF;
	m_nDefaultBackTextureID = 0;
}

WndNewMap::~WndNewMap(void)
{
}

BOOL WndNewMap::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rcWidth = { 78,34,178,58};
	m_pEditWidth = new ui::EditBox;
	m_pEditWidth->Create("Width",&rcWidth,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	RECT rcDepth = { 78,76,178,100};
	m_pEditDepth= new ui::EditBox;
	m_pEditDepth->Create("Depth",&rcDepth,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	RECT rcOK = { 92,309,153,335};
	m_pBtnOK = new ui::Button;
	m_pBtnOK->Create("确定",&rcOK,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	RECT rcCancel = { 278,310,340,334};
	m_pBtnCancel= new ui::Button;
	m_pBtnCancel->Create("取消",&rcCancel,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	m_bIsOK = FALSE;

	m_bIsPopupCheckFlag = FALSE;

	//读取地形图素集
	RECT rcBoxTileSet = { 227,34,405,116};
	m_pListBoxTileSet = new ui::ListBox;
	m_pListBoxTileSet->Create("图素",&rcBoxTileSet,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE,this);

	utility::File file;
	if (!file.Open("terrain\\set.txt"))
	{
		//error 
		return FALSE;
	}

	utility::Text script;
	script.Create(&file);
	file.Close();

	utility::Text::Node *pRoot =  script.GetRootNode();

	for(DWORD i = 0; i < pRoot->GetChildCount(); i++)
	{
		utility::Text::Variable *pVar = pRoot->GetChildNode(i)->GetVar("name");
		m_pListBoxTileSet->InsertItem(pVar->GetStringValue());
	}

	script.Destroy();

	m_pCheckBox = new RadioButton[16];

	// 地表图素按钮
	INT nWidth, nHeight, nStartX, nStartY;
	nWidth = nHeight = 32;
	nStartX = 227; nStartY = 116;
	for(INT i = 0; i < 16; i++)
	{
		INT nLeft, nTop, nRight, nBottom;
		TCHAR szTitle[128];

		nTop = nStartY + nHeight * (i / 4) + (i / 4) * 8;	nBottom = nTop + nHeight;
		nLeft = nStartX + nWidth * (i % 4) + (i % 4) * 8;	nRight = nLeft + nWidth;
		
		RECT rc = {nLeft, nTop, nRight, nBottom};
		sprintf(szTitle, "tile%d", i);
		m_pCheckBox[i].Create(szTitle,&rc,FWS_VISIBLE,this);
		m_pCheckBox[i].SetTextureHandle(0);
	}

	return TRUE;
}
void WndNewMap::Destroy()
{
	SAFEDESTROY(m_pListBoxTileSet);
	SAFEDESTROY(m_pBtnOK)
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pEditDepth);
    SAFEDESTROY(m_pEditWidth);

	for(INT i = 0; i < 16; i++)
	{
		m_pCheckBox[i].Destroy();
	}

	SAFEDELETEARRAY(m_pCheckBox);

	if(m_dwShaderGroupHandle != -1)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
	}

	ui::Wnd::Destroy();
}

void WndNewMap::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pListBoxTileSet->IsSelChanged())
		{
			INT nTileSetIndex = m_pListBoxTileSet->GetSelItem();
			if(nTileSetIndex != -1)
			{
				ChangeTileSet(nTileSetIndex);
			}
		}

		for(INT i = 0; i < 16; i++)
		{
			if(m_pCheckBox[i].GetTextureHandle() == 0)
				continue;

			if(m_pCheckBox[i].IsClicked())
			{
				for(int j = 0; j < 16; j++)
				{
					m_pCheckBox[j].SetChecked(FALSE);
				}
				m_pCheckBox[i].SetChecked(TRUE);				
				m_nDefaultBackTextureID = i;
				break;
			}
		}

		if (m_pBtnOK->IsClicked())
		{
			EndPopUp();
			m_bIsOK = TRUE;
			m_bIsPopupCheckFlag= TRUE;
		}
		if (m_pBtnCancel->IsClicked())
		{
			EndPopUp();
			m_bIsOK = FALSE;
			m_bIsPopupCheckFlag = TRUE;
		}

		ui::Wnd::UpdateChild();
	}
}

void WndNewMap::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

		RECT &rc       = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		int iLen = lstrlen(m_tWindowInfo.szName);

		int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,iLen)/2;
		int iTextStartY = iY ;
		
		pLayer2D->DrawSolidQuad(iX ,iY ,iW,pLayer2D->GetFontSize() + FWA_FRAMESIZE,m_tWindowInfo.colorMiddle);

		ui::Wnd::SetUIRendState();
		pLayer2D->OutPutText(iTextStartX,iTextStartY,m_tWindowInfo.szName,iLen,0xffffffff);

		pLayer2D->OutPutText(iX + 26, iY + 40,"宽度",4,0xffffffff);
		pLayer2D->OutPutText(iX + 26, iY + 80,"深度",4,0xffffffff);

		ui::Wnd::RenderChild();
	}
}

void WndNewMap::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndNewMap::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}



int WndNewMap::GetWidth(void)
{
	char szTemp[1024];
	m_pEditWidth->GetEditText(szTemp,1024);
	int iWidth = atoi(szTemp);
	if (iWidth < 1)
	{
		iWidth = 1;
	}
	if (iWidth > 4096)
	{
		iWidth = 4096;
	}	
	return iWidth;
}

int WndNewMap::GetDepth(void)
{
	char szTemp[1024];
	m_pEditDepth->GetEditText(szTemp,1024);
	int iDepth = atoi(szTemp);
	if (iDepth < 1)
	{
		iDepth = 1;
	}
	if (iDepth > 4096)
	{
		iDepth = 4096;
	}	
	return iDepth;
}

DWORD WndNewMap::GetSelectTileSet()
{
	int iSel = m_pListBoxTileSet->GetSelItem();
	if (iSel < 0)
	{
		return 0;
	}
    return (DWORD) iSel;
}

VOID WndNewMap::SetCheckBoxTextureHandle(DWORD dwIndex, DWORD dwTextureHandle)
{
	RECT rc = {0, 0, 64, 64};
	m_pCheckBox[dwIndex].SetTextureHandle(dwTextureHandle);
	m_pCheckBox[dwIndex].SetTextureRect(rc);
}

VOID WndNewMap::ResetCheckBox()
{
	for(INT i = 0; i < 16; i++)
	{
		m_pCheckBox[i].SetChecked(FALSE);
	}
}

void WndNewMap::ChangeTileSet(DWORD dwIndex)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	//先销毁ShaderGroupHandle
	if (m_dwShaderGroupHandle !=0xffffffff)
	{
		pLayer3D->DestroyShaderGroup(m_dwShaderGroupHandle);
	}

	// 建立对应的Shader
	char szTempName[128];
	sprintf(szTempName, "terrain\\set%d\\shader", dwIndex);
	utility::File fileShader;	
	if (!fileShader.Open(szTempName))
	{
		OutputConsole("error: open \"%s\" failed.\n",szTempName);
		return ;
	}

	char *pszShaderText = new char [fileShader.GetLength() + 1];
	memcpy(pszShaderText,fileShader.GetData(),fileShader.GetLength());
	pszShaderText[fileShader.GetLength()] = 0;	
	fileShader.Close();

	//get shader group handle
	m_dwShaderGroupHandle = pLayer3D->CreateShaderGroup(pszShaderText,"terrain\\texture");

	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(m_dwShaderGroupHandle);

	DWORD dwTextureCount = pShaderGroup->GetTextureCount(0);

	RECT rc = {0, 0, 64, 64};
	for(DWORD n = 0; n < 16; n++)
	{
		if (n < dwTextureCount)
		{
			m_pCheckBox[n].SetTextureHandle(pShaderGroup->GetTextureHandle(0,n));
			m_pCheckBox[n].SetTextureRect(rc);
			m_pCheckBox[n].ShowWnd(TRUE);
		}
		else
			m_pCheckBox[n].ShowWnd(FALSE);
	}
}
