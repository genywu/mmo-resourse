#include "stdafx.h"
#include "GoodsRender.h"
#include "../../../EngineExtend/DisplayModel.h"
#include "../../../EngineExtend/GameModelManager.h"
#include "../../../EngineExtend/GameGraphics2D.h"
#include "../../../GameClient/Game.h"
#include "GoodsViewerPage.h"

float GoodsRender::m_XRot;
float GoodsRender::m_YRot;
float GoodsRender::m_ZRot;

float GoodsRender::m_Scale;

float GoodsRender::m_XTrans;
float GoodsRender::m_YTrans;
float GoodsRender::m_ZTrans;

//用于模型显示
CDisplayModel* GoodsRender::m_DispModel = NULL;
CGameModelManager* GoodsRender::m_GameModelManager = NULL;
AnimInfo* GoodsRender::m_AniInfo = NULL;//模型的动画

GoodsRender::GoodsRender()
{
	m_GameModelManager = CGameModelManager::GetInstance();
	m_AniInfo = new AnimInfo();

	m_XRot = m_YRot = m_XRot = 0.0f;
	m_Scale = 0.0f;
	m_XTrans = m_YTrans = m_ZTrans = 0;
}

GoodsRender::~GoodsRender()
{
	SAFEDELETE(m_AniInfo);
	m_GameModelManager = NULL ;
}
void GoodsRender::ReSetRotAngle(float xRot /* = 5.0f  */,float yRot /* = 5.0f  */,float zRot /* = 5.0f */)
{
	m_XRot += xRot;
	m_YRot += yRot;
	m_ZRot += zRot;
}

void GoodsRender::ResetScal(float scale /* = 2.0f */)
{
	m_Scale = scale;
}

void GoodsRender::ResetTranslation(float xTran /* = 1  */,float yTran /* = 1  */,float zTran /* = 1  */)
{
	m_XTrans = xTran;
	m_YTrans = yTran;
	m_ZTrans = zTran;
}

void GoodsRender::SetTranAdd(float xTranAdd /* = 1 */,float yTranAdd /* = 1 */, float zTranAdd /* = 1 */)
{
	m_XTrans += xTranAdd;
	m_YTrans += yTranAdd;
	m_ZTrans += zTranAdd;
}
bool GoodsRender::LoadModel(uint ID,const char* PathName /* = "model/goods"*/)
{
	if(m_GameModelManager != NULL && m_DispModel != NULL)
		m_GameModelManager->ReleaseDisplayModel(m_DispModel);
	CEGUI::String strName = PathName;
	strName += "/";
	strName += CEGUI::PropertyHelper::intToString(ID);
	strName += "/model";
	m_DispModel = m_GameModelManager->CreateDisplayModel(strName.c_str(),false);
	if(m_DispModel == NULL)
	{
		throw CEGUI::InvalidRequestException("Create model of " + strName + " is Failed");
		return false;
	}
	m_DispModel->ShowAllGroup(false);
	m_DispModel->EnableParticleEmitter(TRUE);
	m_DispModel->EnableTrailEmitter(TRUE);

	m_XRot = m_YRot = m_XRot = 0.0f;
	m_Scale = DEFAULT_SCALE;
	//m_XTrans = m_YTrans = 0.0f;
	m_XTrans = DEFAULT_XTRANS;
	m_YTrans = DEFAULT_YTRANS;
	m_ZTrans = DEFAULT_ZDISTANCE;

	return true;
}

void GoodsRender::Render()
{
	if(!m_DispModel || GetInst(GoodsViewerPage).GetPageWindow()->isVisible() == false)
		return;
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	DWORD dwwidth = render::Interface::GetInstance()->GetWndWidth();
	DWORD dwheight = render::Interface::GetInstance()->GetWndHeight();
	D3DVIEWPORT9 oldViewPort,CurViewPort;
	//保存视区
	pd3dDevice->GetViewport(&oldViewPort);

	//计算视区(客户端窗口大小)
	CurViewPort.X = 0 ;
	CurViewPort.Y = 0 ; 
	CurViewPort.Width = dwwidth;
	CurViewPort.Height = dwheight;
	CurViewPort.MinZ = 0.0f;
	CurViewPort.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&CurViewPort);
	//计算清空区域 
	pd3dDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,D3DCOLOR_ARGB(255,0,0,0),1.0f,0);

	static D3DXMATRIX mattempview,mattempworld;
	static D3DXMATRIX mattempprojective;
	static D3DXMATRIX matWorld;
	//全身照

	//观察矩阵
	D3DXMatrixLookAtLH(&mattempview,
		&D3DXVECTOR3(0.0f,1.0f, 2000.0f),
		&D3DXVECTOR3(0.0f,1.0f, 0.0f),
		&D3DXVECTOR3(0.0f,1.0f, 0.0f));

	static D3DXMATRIX matbillboard;
	static D3DXMATRIX matIdenty = *D3DXMatrixIdentity(&matIdenty);
	D3DXMatrixInverse(&matbillboard,NULL,&mattempview);
	matbillboard._41 = matbillboard._42 = matbillboard._43 = 0.0f;
	m_AniInfo->m_tActionInfo.bCurActionLooped = true;
	m_AniInfo->SetCurrentTime(timeGetTime());
	m_AniInfo->SetBillboard(&matbillboard);
	m_AniInfo->SetViewMatrix(&mattempview);
	
	//正交投影
	D3DXMatrixOrthoLH(&mattempprojective,(float)dwwidth,(float)dwheight, 0.0f, 2000.0f);

	static D3DXMATRIX matModelWorld,matSCL,matROT,matROTX,matROTY,matROTZ,matTranslate;
	//计算旋转、放缩、平移
	D3DXMatrixIdentity(&matROTX);
	D3DXMatrixIdentity(&matROTY);
	D3DXMatrixIdentity(&matROTZ);

	D3DXMatrixRotationY(&matROTY,m_YRot);
	D3DXMatrixRotationX(&matROTX,m_XRot);
	D3DXMatrixRotationZ(&matROTZ,m_ZRot);
	
	matROT = matROTX * matROTY * matROTZ ;

	D3DXMatrixScaling(&matSCL,m_Scale,m_Scale,m_Scale);
	D3DXMatrixMultiply(&matModelWorld,&matSCL,&matROT);

	D3DXMatrixTranslation(&matTranslate,-m_XTrans,m_YTrans,m_ZTrans);
	//计算最后的世界矩阵 
	D3DXMatrixMultiply(&matModelWorld,&matModelWorld,&matTranslate);
	// 显示坐标
	if (g_bDebug)
	{
		GameGraphics2D::SetUIModelRendState();
		char str[256];
		sprintf(str, "Trans: %f, %f , %f",m_XTrans, m_YTrans,m_ZTrans);
		GetGame()->OutPutText(0, 15, str, lstrlen(str), D3DCOLOR_ARGB(255,255,0,0));
		
		sprintf(str,"Rotate: %f, %f , %f",m_XRot,m_YRot,m_ZRot);
		GetGame()->OutPutText(0,30,str,lstrlen(str),D3DCOLOR_ARGB(255,255,0,0));

		sprintf(str,"Scal: %f",m_Scale);
		GetGame()->OutPutText(0,45,str,lstrlen(str),D3DCOLOR_ARGB(255,255,0,0));
	}

	m_DispModel->SetDirLightEnable(false);
	m_AniInfo->SetWorldMatrix(&matModelWorld);
	m_AniInfo->SetScalMatrix(&matSCL);
	m_AniInfo->SetViewMatrix(&mattempview);
	m_AniInfo->SetProjectedMatrix(&mattempprojective);
	//物品无动画
	m_DispModel->DrawModelByAniminfo(m_AniInfo);
	m_AniInfo->SetScalMatrix(&matIdenty);

	D3DXMatrixIdentity( &matWorld );
	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
	pd3dDevice->SetTransform(D3DTS_VIEW,&mattempview);
	pd3dDevice->SetTransform(D3DTS_PROJECTION,&mattempprojective);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	//粒子系统刷新
	if(particle::Manager::GetInstance())
		particle::Manager::GetInstance()->Update(m_Scale,true,false);
	particle::Manager::GetInstance()->Draw(m_Scale,true,false);
	pd3dDevice->SetViewport(&oldViewPort);
	GameGraphics2D::SetUIModelRendState();
}



