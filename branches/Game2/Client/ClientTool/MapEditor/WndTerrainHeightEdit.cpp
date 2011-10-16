//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月8日 14:36:59
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#include "StdAfx.h"
#include "wndterrainheightedit.h"

#include "..\..\engine\render.h"

#include "Control.h"
#include "CommDB.h"
#include "CommUI.h"
#include "EditWnd.h"
#include "TerrainMesh.h"
#include "ToolTerrainHeight.h"

WndTerrainHeightEdit::WndTerrainHeightEdit(void)
{
}

WndTerrainHeightEdit::~WndTerrainHeightEdit(void)
{
}

BOOL WndTerrainHeightEdit::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	ui::Wnd::Create(pszName, pRect, dwStyle, pParent);

	m_pRadioButton = new RadioButton[16];

	//工具图标
	const char *szIconName[] = { "ui\\icon\\Raise.tga",
								 "ui\\icon\\Lower.tga",
								 "ui\\icon\\Plateau.tga",
								 "ui\\icon\\Noise.tga",
								 "ui\\icon\\Smooth.tga"};

	INT nWidth, nHeight;
	nWidth = nHeight = 32;
	RECT rcTex = {0, 0, 32, 32};
	for(INT i = 0; i < 5; i++)
	{
		INT nLeft, nTop, nRight, nBottom;
		TCHAR szTitle[128];

		nTop = nHeight * (i / 5) + (i / 5) * 2;	nBottom = nTop + nHeight;
		nLeft = nWidth * (i % 5) + (i % 5) * 2;	nRight = nLeft + nWidth;
		
		RECT rc = {nLeft, nTop, nRight, nBottom};
		sprintf(szTitle, "tile%d", i);
		m_pRadioButton[i].Create(szTitle,&rc,FWS_VISIBLE,this);

		DWORD dwTextureHandle = pInterface->CreateTexture(szIconName[i],0);
		m_pRadioButton[i].SetTextureHandle(dwTextureHandle);
		m_pRadioButton[i].SetTextureRect(rcTex);
	}

	m_pRadioButton[0].SetChecked();
	m_nCheckedIndex = 0;

	return TRUE;
}

void WndTerrainHeightEdit::Destroy()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	for(INT i = 0; i < 5; i++)
	{	
		pInterface->DestroyTexture(m_pRadioButton[i].GetTextureHandle());
		m_pRadioButton[i].Destroy();

	}
	SAFEDELETEARRAY(m_pRadioButton);
	
	ui::Wnd::Destroy();	
}


void WndTerrainHeightEdit::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
		/*EditWnd* pWndEditor = (EditWnd* )ui::Manager::GetInstance()->GetMainWnd();
		ToolTerrainHeight* pHeightTool = pWndEditor->GetToolHeightEdit();		
		EditMap *pMap = pWndEditor->GetMap();*/

		EditWnd* pEditWnd = CommUI::GetInstance()->GetEditWnd();
		ToolTerrainHeight* pHeightTool = pEditWnd->GetToolTerrainHeight();		
		TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

		if(pTerrainMesh)
		{
			for(DWORD i = 0; i < 5; i++)
			{
				if(m_pRadioButton[i].GetTextureHandle() == 0)
					continue;

				if(m_pRadioButton[i].IsClicked())
				{
					for(int j = 0; j < 5; j++)
					{
						m_pRadioButton[j].SetChecked(FALSE);
					}
					m_pRadioButton[i].SetChecked(TRUE);
					m_nCheckedIndex = i;
					pHeightTool->SetEditStatue((HEIGHTEDITSTATUETYPE)(i+1));
					break;
				}
			}
		}

		if(pInputInfo->eType == UIMT_KB_KEYDOWN)
		{
			switch(pInputInfo->dwData)
			{
			case DIK_1:
				m_nCheckedIndex = 0;
				break;
			case DIK_2:
				m_nCheckedIndex = 1;
				break;
			case DIK_3:
				m_nCheckedIndex = 2;
				break;
			case DIK_4:
				m_nCheckedIndex = 3;
				break;
			case DIK_5:
				m_nCheckedIndex = 4;
				break;
			}

			//pHeightTool->SetEditStatue(HEIGHTEDITSTATUETYPE(m_nCheckedIndex));
			SetBoxChecked(m_nCheckedIndex);
		}

		ui::Wnd::UpdateChild();
	}
}

void WndTerrainHeightEdit::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
	}

	ui::Wnd::RenderChild();
}



//---------------------------------------------------------------------------------
// Function name   : WndTerrainHeightEdit::SetCheckBoxTextureHandle
// Description     : 设置CheckBox 的纹理句柄
// Return type     : VOID 
// Argument        : DWORD dwIndex
// Argument        : DWORD dwTextureHandle
// Argument        : RECT rc    : 采用的纹理范围（单位：像素）
//---------------------------------------------------------------------------------
VOID WndTerrainHeightEdit::SetCheckBoxTextureHandle(DWORD dwIndex, DWORD dwTextureHandle, RECT rc)
{
	m_pRadioButton[dwIndex].SetTextureHandle(dwTextureHandle);
	m_pRadioButton[dwIndex].SetTextureRect(rc);
}



//---------------------------------------------------------------------------------
// Function name   : WndTerrainHeightEdit::ResetCheckBox
// Description     : 设置CheckBox 的显示状态
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID WndTerrainHeightEdit::ResetCheckBox()
{
	for(INT i = 0; i < 5; i++)
	{
		m_pRadioButton[i].SetChecked(FALSE);
	}
}



//---------------------------------------------------------------------------------
// Function name   : WndTerrainHeightEdit::ShowCheckBox
// Description     : 重置所有的CheckBox
// Return type     : VOID 
// Argument        : DWORD dwIndex
// Argument        : BOOL bShow
//---------------------------------------------------------------------------------
VOID WndTerrainHeightEdit::ShowCheckBox(DWORD dwIndex, BOOL bShow)
{
	m_pRadioButton[dwIndex].ShowWnd(bShow);
}


VOID WndTerrainHeightEdit::SetBoxChecked(DWORD dwIndex, BOOL bCheckFlag)
{
	for(DWORD i = 0; i < 16; i++)
	{
		if(i == dwIndex)
		{
			m_pRadioButton[i].SetChecked(bCheckFlag);
			m_nCheckedIndex = dwIndex;
		}
		else
			m_pRadioButton[i].SetChecked(!bCheckFlag);
	}
}
