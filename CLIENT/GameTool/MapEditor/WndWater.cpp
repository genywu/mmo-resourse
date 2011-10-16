#include "stdafx.h"

/*
肖杨 16:22:17
  肖杨 16:19:26
  

		WORD wAction = GetAction();
		if (wAction < ACT_FACEACTION || wAction > ACT_FACEACTIONMAX)
		{
			extern bool g_bForceUpdate;
			g_bForceUpdate = true;
			SetAction(GetAction());
			g_bForceUpdate = false;
		}
		//*******
		//取消武器显示ADD BY 彭谦
		//*******
	}

}
肖杨 16:19:26
  

		WORD wAction = GetAction();
		if (wAction < ACT_FACEACTION || wAction > ACT_FACEACTIONMAX)
		{
			extern bool g_bForceUpdate;
			g_bForceUpdate = true;
			SetAction(GetAction());
			g_bForceUpdate = false;
		}
		//*******
		//取消武器显示ADD BY 彭谦
		//*******
	}

}

*/





//#include "..\..\frostengine\utility.h"
//#include "..\..\frostengine\console.h"

//#include "commui.h"
//#include "commdb.h"
//#include "map.h"
//#include "cmdmgr.h"
//#include "editwnd.h"
#include "WndWater.h"
#include "ToolBuildEdit.h"
#include "CommUI.h"
#include "CommDB.h"
#include "EditWnd.h"
#include "Map.h"

#define VALUE_LEN		128
static char s_szValue[VALUE_LEN];

WndWater::WndWater(void)
{
}

WndWater::~WndWater(void)
{
}


Water::CWater * WndWater::GetCurrentSelectWater()
{
	EditMap * pMap = CommDB::GetInstance()->GetMap();
	ToolBuildEdit * pWndTool = CommUI::GetInstance()->GetEditWnd()->GetToolBuildEdit();

	int i = pWndTool->GetSelGridIndex(0);
	if (i == -1)
		return NULL;

	DWORD dwGridIndex = (DWORD)i;
	Grid * pGrid = pMap->GetGrid(dwGridIndex);
	Grid::tagBuild * pGridBuild = pGrid->GetBuild();
	WORD wBuildID = pGrid->GetBuild()->wID;

	GameBuildList::Build * pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID, false);
	GameBuildList::Build::tagModel * pModel = pBuild->GetModel();
	CDisplayModel * pGameModel(NULL);
	if (pModel && pModel->pGameModel)
	{
		pGameModel = pModel->pGameModel;
		return pGameModel->GetWater();
	}

	return NULL;
}



void WndWater::SetParameter(Water::CWater * pWater)
{
	Water::CWater * pWaterTmp = pWater;
	if (pWater == NULL)
	{
		pWaterTmp = new Water::CWater();
		ZeroMemory(pWaterTmp, sizeof(Water::CWater));
	}

	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNorSpeedX);
	m_pEditNorSpeedX->SetEditText(s_szValue);
	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNorSpeedY);
	m_pEditNorSpeedY->SetEditText(s_szValue);

	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNormat00);
	m_pEditEnvMat00->SetEditText(s_szValue);
	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNormat01);
	m_pEditEnvMat01->SetEditText(s_szValue);
	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNormat10);
	m_pEditEnvMat10->SetEditText(s_szValue);
	sprintf(s_szValue, "%.6f", pWaterTmp->m_fNormat11);
	m_pEditEnvMat11->SetEditText(s_szValue);


	sprintf(s_szValue, "%.6f", pWaterTmp->m_fEnvSpeedX);
	m_pEditEnvSpeedX->SetEditText(s_szValue);
	sprintf(s_szValue, "%.6f", pWaterTmp->m_fEnvSpeedY);
	m_pEditEnvSpeedY->SetEditText(s_szValue);

	if (pWater == NULL)
		SAFE_DELETE(pWaterTmp);
}


BOOL WndWater::Create(const char * pszName, const RECT * pRect, DWORD dwStyle, Wnd * pParent)
{
	//建立窗口
	ui::Wnd::Create(pszName, pRect, dwStyle, pParent);
	POINT pt = { 5, 5 };
	long lX = 80;

	// 创建法线纹理层操作组件
	RECT rcStaticNormal= { pt.x + 80, pt.y + 20, pt.x, pt.y };
	m_pStaticNormal = new ui::Static;
	m_pStaticNormal->Create("法线纹理层", &rcStaticNormal, FWS_VISIBLE , this);

	pt.y += 25;
	RECT rcStaticNorSpeedX = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticNorSpeedX = new ui::Static;
	m_pStaticNorSpeedX->Create("SpeedX:", &rcStaticNorSpeedX, FWS_VISIBLE , this);

	RECT rcNorX = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditNorSpeedX = new ui::EditBox;
	m_pEditNorSpeedX->Create("SpeedX", &rcNorX, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticNorSpeedY = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticNorSpeedY = new ui::Static;
	m_pStaticNorSpeedY->Create("SpeedY:", &rcStaticNorSpeedY, FWS_VISIBLE , this);

	RECT rcNorY = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditNorSpeedY = new ui::EditBox;
	m_pEditNorSpeedY->Create("SpeedY", &rcNorY, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvMat00 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticEnvMat00 = new ui::Static;
	m_pStaticEnvMat00->Create("EnvMat00:", &rcStaticEnvMat00, FWS_VISIBLE , this);

	RECT rc00 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvMat00 = new ui::EditBox;
	m_pEditEnvMat00->Create("EnvMat00", &rc00, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvMat01 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticEnvMat01 = new ui::Static;
	m_pStaticEnvMat01->Create("EnvMat01:", &rcStaticEnvMat01, FWS_VISIBLE , this);

	RECT rc01 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvMat01 = new ui::EditBox;
	m_pEditEnvMat01->Create("EnvMat01", &rc01, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvMat10 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticEnvMat10 = new ui::Static;
	m_pStaticEnvMat10->Create("EnvMat10:", &rcStaticEnvMat10, FWS_VISIBLE , this);

	RECT rc10 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvMat10 = new ui::EditBox;
	m_pEditEnvMat10->Create("EnvMat10", &rc10, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvMat11 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticEnvMat11 = new ui::Static;
	m_pStaticEnvMat11->Create("EnvMat11:", &rcStaticEnvMat11, FWS_VISIBLE , this);

	RECT rc11 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvMat11 = new ui::EditBox;
	m_pEditEnvMat11->Create("EnvMat11", &rc11, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticNorUVScale = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticNorUVScale = new ui::Static;
	m_pStaticNorUVScale->Create("UVScale:", &rcStaticNorUVScale, FWS_VISIBLE , this);

	RECT rcNorUVScale = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditNorUVScale = new ui::EditBox;
	m_pEditNorUVScale->Create("UVScale", &rcNorUVScale, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcModifyNormalTex = { pt.x, pt.y, pt.x + 70, pt.y + 20 };
	m_pBtnModifyNormalTex = new ui::Button;
	m_pBtnModifyNormalTex->Create("Texture", &rcModifyNormalTex, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	RECT rcNormalTex = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditNormalTex = new ui::EditBox;
	m_pEditNormalTex->Create("NormalTex", &rcNormalTex, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	// 环境纹理组件
	pt.y += 30;
	RECT rcStaticEnv = { pt.x + 80, pt.y + 20, pt.x, pt.y };
	m_pStaticEnv = new ui::Static;
	m_pStaticEnv->Create("环境纹理层", &rcStaticEnv, FWS_VISIBLE , this);

	pt.y += 25;
	RECT rcStaticEnvSpeedX = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticEnvX = new ui::Static;
	m_pStaticEnvX->Create("SpeedX:", &rcStaticEnvSpeedX, FWS_VISIBLE , this);

	RECT rcEnvX = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvSpeedX = new ui::EditBox;
	m_pEditEnvSpeedX->Create("SpeedX", &rcEnvX, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvSpeedY = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticEnvY = new ui::Static;
	m_pStaticEnvY->Create("SpeedY:", &rcStaticEnvSpeedY, FWS_VISIBLE , this);

	RECT rcEnvY = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvSpeedY = new ui::EditBox;
	m_pEditEnvSpeedY->Create("SpeedY", &rcEnvY, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticEnvUVScale = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticEnvUVScale = new ui::Static;
	m_pStaticEnvUVScale->Create("UVScale:", &rcStaticEnvUVScale, FWS_VISIBLE , this);

	RECT rcEnvUVScale = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvUVScale = new ui::EditBox;
	m_pEditEnvUVScale->Create("UVScale", &rcEnvUVScale, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcModifyEnvTex = { pt.x, pt.y, pt.x + 70, pt.y + 20 };
	m_pBtnModifyEnvTex = new ui::Button;
	m_pBtnModifyEnvTex->Create("Texture", &rcModifyEnvTex, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG ,this);

	RECT rcEnvTex = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditEnvTex = new ui::EditBox;
	m_pEditEnvTex->Create("NormalTex", &rcEnvTex, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	// 反射纹理组件
	pt.y += 30;
	RECT rcStaticRef = { pt.x + 80, pt.y + 20, pt.x, pt.y };
	m_pStaticRef = new ui::Static;
	m_pStaticRef->Create("反射纹理层", &rcStaticRef, FWS_VISIBLE , this);

	pt.y += 25;
	RECT rcStaticTran = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticTran = new ui::Static;
	m_pStaticTran->Create("Tran:", &rcStaticTran, FWS_VISIBLE , this);

	RECT rcTran = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditTran = new ui::EditBox;
	m_pEditTran->Create("Transparence", &rcTran, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefSpeedX = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticRefX = new ui::Static;
	m_pStaticRefX->Create("SpeedX:", &rcStaticRefSpeedX, FWS_VISIBLE , this);

	RECT rcRefX = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefSpeedX = new ui::EditBox;
	m_pEditRefSpeedX->Create("SpeedX", &rcRefX, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefSpeedY = { pt.x, pt.y, pt.x + 60, pt.y + 20 };
	m_pStaticRefY = new ui::Static;
	m_pStaticRefY->Create("SpeedY:", &rcStaticRefSpeedY, FWS_VISIBLE , this);

	RECT rcRefY = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefSpeedY = new ui::EditBox;
	m_pEditRefSpeedY->Create("SpeedY", &rcRefY, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefMat00 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticRefMat00 = new ui::Static;
	m_pStaticRefMat00->Create("RefMat00:", &rcStaticRefMat00, FWS_VISIBLE , this);

	RECT rcRef00 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefMat00 = new ui::EditBox;
	m_pEditRefMat00->Create("RefMat00", &rcRef00, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefMat01 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticRefMat01 = new ui::Static;
	m_pStaticRefMat01->Create("RefMat01:", &rcStaticRefMat01, FWS_VISIBLE , this);

	RECT rcRef01 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefMat01 = new ui::EditBox;
	m_pEditRefMat01->Create("RefMat01", &rcRef01, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefMat10 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticRefMat10 = new ui::Static;
	m_pStaticRefMat10->Create("RefMat10:", &rcStaticRefMat10, FWS_VISIBLE , this);

	RECT rcRef10 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefMat10 = new ui::EditBox;
	m_pEditRefMat10->Create("RefMat10", &rcRef10, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefMat11 = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticRefMat11 = new ui::Static;
	m_pStaticRefMat11->Create("RefMat11:", &rcStaticRefMat11, FWS_VISIBLE , this);

	RECT rcRef11 = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefMat11 = new ui::EditBox;
	m_pEditRefMat11->Create("RefMat11", &rcRef11, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	pt.y += 25;
	RECT rcStaticRefUVScale = { pt.x, pt.y, pt.x + 80, pt.y + 20 };
	m_pStaticRefUVScale = new ui::Static;
	m_pStaticRefUVScale->Create("UVScale:", &rcStaticRefUVScale, FWS_VISIBLE , this);

	RECT rcRefUVScale = { pt.x + lX, pt.y, pt.x + 190, pt.y + 20 };
	m_pEditRefUVScale = new ui::EditBox;
	m_pEditRefUVScale->Create("UVScale", &rcRefUVScale, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	return TRUE;
}

void WndWater::Destroy()
{
	//销毁控件
	// 法线纹理组件
	SAFEDESTROY(m_pStaticNormal);
	SAFEDESTROY(m_pStaticNorSpeedX);
	SAFEDESTROY(m_pStaticNorSpeedY);
	SAFEDESTROY(m_pStaticEnvMat00);
	SAFEDESTROY(m_pStaticEnvMat01);
	SAFEDESTROY(m_pStaticEnvMat10);
	SAFEDESTROY(m_pStaticEnvMat11);
	SAFEDESTROY(m_pEditNorSpeedX);
	SAFEDESTROY(m_pEditNorSpeedY);
	SAFEDESTROY(m_pEditEnvMat00);
	SAFEDESTROY(m_pEditEnvMat01);
	SAFEDESTROY(m_pEditEnvMat10);
	SAFEDESTROY(m_pEditEnvMat11);
	SAFEDESTROY(m_pStaticNorUVScale);
	SAFEDESTROY(m_pEditNorUVScale);
	SAFEDESTROY(m_pBtnModifyNormalTex);
	SAFEDESTROY(m_pEditNormalTex);

	// 环境纹理组件
	SAFEDESTROY(m_pStaticEnv);
	SAFEDESTROY(m_pStaticEnvX);
	SAFEDESTROY(m_pStaticEnvY);
	SAFEDESTROY(m_pStaticEnvUVScale);
	SAFEDESTROY(m_pEditEnvSpeedX);
	SAFEDESTROY(m_pEditEnvSpeedY);
	SAFEDESTROY(m_pEditEnvUVScale);
	SAFEDESTROY(m_pBtnModifyEnvTex);
	SAFEDESTROY(m_pEditEnvTex);

	// 反射纹理组件
	SAFEDESTROY(m_pStaticRef);
	SAFEDESTROY(m_pStaticTran);
	SAFEDESTROY(m_pStaticRefX);
	SAFEDESTROY(m_pStaticRefY);
	SAFEDESTROY(m_pStaticRefMat00);
	SAFEDESTROY(m_pStaticRefMat01);
	SAFEDESTROY(m_pStaticRefMat10);
	SAFEDESTROY(m_pStaticRefMat11);
	SAFEDESTROY(m_pStaticRefUVScale);
	SAFEDESTROY(m_pEditTran);
	SAFEDESTROY(m_pEditRefSpeedX);
	SAFEDESTROY(m_pEditRefSpeedY);
	SAFEDESTROY(m_pEditRefMat00);
	SAFEDESTROY(m_pEditRefMat01);
	SAFEDESTROY(m_pEditRefMat10);
	SAFEDESTROY(m_pEditRefMat11);
	SAFEDESTROY(m_pEditRefUVScale);

	//销毁窗体
	ui::Wnd::Destroy();
}

void WndWater::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	Water::CWater * pWater = NULL;

	if (m_pEditNorSpeedX->CheckEnter())
	{
		m_pEditNorSpeedX->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNorSpeedX = (float)atof(s_szValue);
	}else if (m_pEditNorSpeedY->CheckEnter())
	{
		m_pEditNorSpeedY->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNorSpeedY = (float)atof(s_szValue);
	}

	else if (m_pEditEnvMat00->CheckEnter())
	{
		m_pEditEnvMat00->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNormat00 = (float)atof(s_szValue);
	}else if (m_pEditEnvMat01->CheckEnter())
	{
		m_pEditEnvMat01->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNormat01 = (float)atof(s_szValue);
	}else if (m_pEditEnvMat10->CheckEnter())
	{
		m_pEditEnvMat10->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNormat10 = (float)atof(s_szValue);
	}else if (m_pEditEnvMat11->CheckEnter())
	{
		m_pEditEnvMat11->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fNormat11 = (float)atof(s_szValue);
	}

	else if (m_pEditEnvSpeedX->CheckEnter())
	{
		m_pEditEnvSpeedX->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fEnvSpeedX = (float)atof(s_szValue);
	}else if (m_pEditEnvSpeedY->CheckEnter())
	{
		m_pEditEnvSpeedY->GetEditText(s_szValue, VALUE_LEN);
		pWater = GetCurrentSelectWater();
		if (pWater != NULL)
			pWater->m_fEnvSpeedY = (float)atof(s_szValue);
	}

	ui::Wnd::UpdateChild();
}

void WndWater::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		render::Interface::Layer2D * pLayer2D = render::Interface::GetInstance()->GetLayer2D();

		//画背景
		RenderDefault();

		//画自己
		pLayer2D->DrawLine(5, 260, 205, 260);
		pLayer2D->DrawLine(5, 390, 205, 390);

		//画Child
		RenderChild();
	}
}