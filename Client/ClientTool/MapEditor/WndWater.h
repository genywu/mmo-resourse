#pragma once
#include "..\..\engine\ui.h"

class WndWater : public ui::Wnd
{
	// 法线纹理层组件
	ui::Static	* m_pStaticNormal;
	ui::Static	* m_pStaticNorSpeedX;
	ui::Static	* m_pStaticNorSpeedY;
	ui::Static	* m_pStaticEnvMat00;
	ui::Static	* m_pStaticEnvMat01;
	ui::Static	* m_pStaticEnvMat10;
	ui::Static	* m_pStaticEnvMat11;
	ui::Static	* m_pStaticNorUVScale;
	ui::EditBox * m_pEditNorSpeedX;
	ui::EditBox * m_pEditNorSpeedY;
	ui::EditBox * m_pEditEnvMat00;
	ui::EditBox * m_pEditEnvMat01;
	ui::EditBox * m_pEditEnvMat10;
	ui::EditBox * m_pEditEnvMat11;
	ui::EditBox * m_pEditNorUVScale;
	ui::Button	* m_pBtnModifyNormalTex;
	ui::EditBox * m_pEditNormalTex;

	// 环境纹理层
	ui::Static	* m_pStaticEnv;
	ui::Static	* m_pStaticEnvX;
	ui::Static	* m_pStaticEnvY;
	ui::Static	* m_pStaticEnvUVScale;
	ui::EditBox * m_pEditEnvSpeedX;
	ui::EditBox * m_pEditEnvSpeedY;
	ui::EditBox * m_pEditEnvUVScale;
	ui::Button	* m_pBtnModifyEnvTex;
	ui::EditBox * m_pEditEnvTex;

	// 反射纹理层
	ui::Static	* m_pStaticRef;
	ui::Static	* m_pStaticTran;
	ui::Static	* m_pStaticRefX;
	ui::Static	* m_pStaticRefY;
	ui::Static	* m_pStaticRefMat00;
	ui::Static	* m_pStaticRefMat01;
	ui::Static	* m_pStaticRefMat10;
	ui::Static	* m_pStaticRefMat11;
	ui::Static	* m_pStaticRefUVScale;
	ui::EditBox * m_pEditTran;
	ui::EditBox * m_pEditRefSpeedX;
	ui::EditBox * m_pEditRefSpeedY;
	ui::EditBox * m_pEditRefMat00;
	ui::EditBox * m_pEditRefMat01;
	ui::EditBox * m_pEditRefMat10;
	ui::EditBox * m_pEditRefMat11;
	ui::EditBox * m_pEditRefUVScale;

	Water::CWater * GetCurrentSelectWater();

public:
	void SetParameter(Water::CWater * pWater);

	BOOL Create(const char * pszName,const RECT * pRect, DWORD dwStyle, Wnd * pParent);
	void Destroy();

    void Update();
	void Render();

	WndWater(void);
	~WndWater(void);
};