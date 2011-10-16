//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月7日 11:21:01
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#include "StdAfx.h"
#include "..\..\engine\render.h"



#include "terraintilesetwnd.h"
#include "Control.h"

#include "commdb.h"
#include "EditWnd.h"
#include "map.h"
#include "TerrainMesh.h"
#include "SetTileTool.h"

WndTerrainTileSet::WndTerrainTileSet(void)
{
}

WndTerrainTileSet::~WndTerrainTileSet(void)
{
}

BOOL WndTerrainTileSet::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName, pRect, dwStyle, pParent);

	// 地表图素按钮
	INT nWidth = 32;
	INT nHeight = 32;
	RECT TexRc = {0, 0, 64, 64};
	INT nLeft, nTop, nRight, nBottom;
	TCHAR szTitle[128];
	RECT rc;
	m_pRadioButton = new RadioButton[16];
	for(INT i = 0; i < 16; i++)
	{
		

		nTop = 3 ;	nBottom = nTop + nHeight;
		nLeft = nWidth * i + i * 2 + 4;	
		nRight = nLeft + nWidth;
		rc.left = nLeft;
		rc.top = nTop;
		rc.right = nRight;
		rc.bottom = nBottom;
		sprintf(szTitle, "tile%d", i);
		m_pRadioButton[i].Create(szTitle,&rc,FWS_VISIBLE,this);
		m_pRadioButton[i].SetTextureHandle(0);
	}

	m_pRadioButton[0].SetChecked();
	m_nCheckedIndex = 0;
	
	/*rc.left = nLeft + 1;
	rc.top = nTop;
	rc.right = nRight + 1;
	rc.bottom = nBottom;
	m_edboxsetile = new EditBox;
	m_edboxsetile->Create("edixbox",&rc,this);*/

	m_pColor = new D3DCOLOR[16];

	return TRUE;
}

void WndTerrainTileSet::Destroy()
{
	for(INT i = 0; i < 16; i++)
	{
		m_pRadioButton[i].Destroy();
	}
	SAFEDELETEARRAY(m_pRadioButton);

	SAFEDELETEARRAY(m_pColor);
	
	ui::Wnd::Destroy();
}


void WndTerrainTileSet::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		EditWnd* pWndEditor = (EditWnd* )ui::Manager::GetInstance()->GetMainWnd();
		EditMap *pMap = CommDB::GetInstance()->GetMap();
		TerrainMesh *pTerrain = CommDB::GetInstance()->GetTerrainMesh();
		//Terrain *pTerrain = pMap->GetTerrain();
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
		render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(pTerrain->GetShaderHandle());
		DWORD dwTextureCount = pShaderGroup->GetTextureCount(0);


		if(pTerrain)
		{
			for(INT i = 0; i < 16; i++)
			{
				if(m_pRadioButton[i].GetTextureHandle() == 0)
					continue;

				if(m_pRadioButton[i].IsClicked())
				{
					for(int j = 0; j < 16; j++)
					{
						m_pRadioButton[j].SetChecked(FALSE);
					}
					m_pRadioButton[i].SetChecked(TRUE);
					m_nCheckedIndex = i;
					pTerrain->SetSelectedTextureID(i);

					break;
				}
			}
		}

		if (pInputInfo->eType == UIMT_KB_KEYDOWN)
		{
			switch(pInputInfo->dwData)
			{
			case DIK_RETURN:                 // W
				{
					m_nCheckedIndex++;
					if ((DWORD)m_nCheckedIndex >= dwTextureCount)
						m_nCheckedIndex = 0;

					SetBoxChecked(m_nCheckedIndex);
					pTerrain->SetSelectedTextureID(m_nCheckedIndex);
				}
				break;
			}
		}

		ui::Wnd::UpdateChild();
	}
}

void WndTerrainTileSet::Render()
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
// Function name   : TerrainTileSetWnd::SetCheckBoxTextureHandle
// Description     : 设置CheckBox 的纹理句柄
// Return type     : VOID 
// Argument        : DWORD dwIndex
// Argument        : DWORD dwTextureHandle
// Argument        : RECT rc    : 采用的纹理范围（单位：像素）
//---------------------------------------------------------------------------------
VOID WndTerrainTileSet::SetCheckBoxTextureHandle(DWORD dwIndex, DWORD dwTextureHandle, RECT rc)
{
	m_pRadioButton[dwIndex].SetTextureHandle(dwTextureHandle);
	m_pRadioButton[dwIndex].SetTextureRect(rc);
	//设置颜色
	if (dwTextureHandle)
	{
		render::Interface * pInteface = render::Interface::GetInstance();
		render::Texture * pTexture = pInteface->GetTexture(dwTextureHandle);
		m_pColor[dwIndex] = pTexture->GetPixel(0,0);
	}

}



//---------------------------------------------------------------------------------
// Function name   : TerrainTileSetWnd::ResetCheckBox
// Description     : 设置CheckBox 的显示状态
// Return type     : VOID 
//---------------------------------------------------------------------------------
VOID WndTerrainTileSet::ResetCheckBox()
{
	for(INT i = 0; i < 16; i++)
	{
		m_pRadioButton[i].SetChecked(FALSE);
	}
}



//---------------------------------------------------------------------------------
// Function name   : TerrainTileSetWnd::ShowCheckBox
// Description     : 重置所有的CheckBox
// Return type     : VOID 
// Argument        : DWORD dwIndex
// Argument        : BOOL bShow
//---------------------------------------------------------------------------------
VOID WndTerrainTileSet::ShowCheckBox(DWORD dwIndex, BOOL bShow)
{
	m_pRadioButton[dwIndex].ShowWnd(bShow);
}


VOID WndTerrainTileSet::SetBoxChecked(DWORD dwIndex, BOOL bCheckFlag)
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



