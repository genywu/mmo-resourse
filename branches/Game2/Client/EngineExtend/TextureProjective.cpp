
#include "stdafx.h"
#include "../Engine/utility.h"
#include "../Engine/console.h"
#include "../Engine/math.h"
#include "gamemap.h"
#include "textureprojective.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


std::list<CTextureProjective * > CTextureProjective::m_listalltp;
DWORD CTextureProjective::m_dwbuffersize = 0;
D3DXVECTOR3* CTextureProjective::m_pposbuffer = NULL;
//  光源的方向
D3DXVECTOR3 CTextureProjective::m_LightPos;
// 用于挖洞的Alpha贴图
LPDIRECT3DTEXTURE9	CTextureProjective::m_pRttAlphaMainPlayer = NULL;	
LPDIRECT3DTEXTURE9  CTextureProjective::m_pRttAlphaCircle = NULL;
// 用于生成阴影贴图的表面
LPDIRECT3DSURFACE9	CTextureProjective::m_pTextureSurf;	
// 用于生成阴影贴图深度对比的深度缓冲区
LPDIRECT3DSURFACE9		CTextureProjective::m_pShadowZ = NULL;	
// 用于RTT对比的深度缓冲区
LPDIRECT3DSURFACE9		CTextureProjective::m_pRTTZ = NULL;	
// 用于保存原来的后备缓冲区和深度缓冲区
LPDIRECT3DSURFACE9		CTextureProjective::m_pOldBackBuffer, CTextureProjective::m_pOldZBuffer;	
// 用于保存原来的视口
D3DVIEWPORT9				CTextureProjective::m_oldViewPort;		
// 用于保存原来的世界转换、视图、透视矩阵
D3DXMATRIX				CTextureProjective::m_oldMatWorld,CTextureProjective::m_oldMatView, CTextureProjective::m_oldMatProj;
// 用于阴影的视口
D3DVIEWPORT9				CTextureProjective::m_shadowViewPort;							
// 渲染即时阴影纹理列表
std::list<CTextureProjective::SHADOWTEXTURE> CTextureProjective::m_listshadow;
CTextureProjective::SHADOWTEXTURE CTextureProjective::m_simpleshadow;
bool CTextureProjective::m_bsimpleshadow = false;
// 当前帧阴影纹理的使用数量
UINT CTextureProjective::m_ushadowusednum = 0;
// RTT纹理列表
std::vector<CTextureProjective::RTTRESOURCE> CTextureProjective::m_vecrtt;

D3DXMATRIX CTextureProjective::m_matshadowscanf = D3DXMATRIX( 0.5f, 0.0f, 0.0f, 0.0f,				
															 0.0f, 0.5f, 0.0f, 0.0f,
															 0.0f, 0.0f, 0.0f, 0.0f,
															 0.5f, 0.5f, 1.0f, 1.0f);

D3DXMATRIX CTextureProjective::m_matshadowview;
D3DXMATRIX CTextureProjective::m_matshadowproj;
D3DXMATRIX CTextureProjective::m_matshadowtexture;

D3DXMATRIX CTextureProjective::m_mattempview;
D3DXMATRIX CTextureProjective::m_mattempprojective;


#define SHADOWTEXTUREWIDTH 256
#define SHADOWTEXTUREHEIGHT 256
#define RTTTEXTUREWIDTH 512
#define RTTTEXTUREHEIGHT 512
CTextureProjective::CTextureProjective()
{
	m_dwtexturegroup = -1;
	m_pvlookatpoint = NULL;
	m_dwnowtime = NULL;
	m_mixcolor = D3DCOLOR_ARGB(255,255,255,255);
	m_mixedmodel = 0;
	m_bvisable = true;
	m_balive = true;
	m_dwstarttime = 0;
	//m_dwuvtimeperframe = 0;
	//m_pcolor = NULL;
	//m_puv = NULL;
	m_usenum = 0;

	m_mattranscal = D3DXMATRIX( 0.5f, 0.0f, 0.0f, 0.0f,				
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f);
	m_vheaddir.x = 0.0f;
	m_vheaddir.y = 0.0f;
	m_vheaddir.z = 1.0f;


}
CTextureProjective* CTextureProjective::CreateTextureProjctive()
{
	CTextureProjective* ptp = new CTextureProjective;
	m_listalltp.push_back(ptp);
	return ptp;
}
void CTextureProjective::DestroyTextureProjctive(CTextureProjective* ptp)
{
	m_listalltp.remove(ptp);
	if (!ptp->m_listmytp.empty())
	{
		for(ptp->m_itmytp = ptp->m_listmytp.begin();ptp->m_itmytp != ptp->m_listmytp.end();ptp->m_itmytp++)
		{
			(*(ptp->m_itmytp))->SetAlive(false);
		}
	}
	ptp->m_listmytp.clear();
	SAFEDELETE(ptp) ;
}
void CTextureProjective::AddNewTextureProj(CTextureProjective* t)
{
	m_listmytp.push_back(t);
}
CTextureProjective::~CTextureProjective()
{
	//SAFEDELETEARRAY(m_ppos);
	render::Interface::Layer3D   *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	pLayer3D->DestroyShaderGroup(m_dwtexturegroup);
}
void CTextureProjective::Destroy()
{
	SAFEDELETEARRAY(m_pposbuffer);
	std::list<CTextureProjective * >::iterator ittp;
	if (!m_listalltp.empty())
	{
		for(ittp = m_listalltp.begin();ittp != m_listalltp.end();ittp++)
		{
			SAFEDELETE(*ittp);
		}
	}
	if (!m_vecrtt.empty())
	{
		for (std::vector<RTTRESOURCE>::iterator it = m_vecrtt.begin();it != m_vecrtt.end();it++)
		{
			(*it).Release();
		}
	}
	if (!m_listshadow.empty())
	{
		for (std::list<SHADOWTEXTURE>::iterator it = m_listshadow.begin();it != m_listshadow.end();it++)
		{
			(*it).Release();
		}
	}
	m_listalltp.clear();
	m_vecrtt.clear();
	m_listshadow.clear();
	m_simpleshadow.Release();
	SAFERELEASE(m_pTextureSurf);
	SAFERELEASE(m_pShadowZ);
	SAFERELEASE(m_pRTTZ);
	SAFERELEASE(m_pOldBackBuffer);
	SAFERELEASE(m_pOldZBuffer);
	SAFERELEASE(m_pRttAlphaMainPlayer);
	SAFERELEASE(m_pRttAlphaCircle);

}
void CTextureProjective::SetPosDirAlphaNowtime(float x,float y,float z,float diry,float alpha,DWORD time)
{
	CTextureProjective::POSDIRALPHATIME temp;
	temp.pos.x = x;
	temp.pos.y = y;
	temp.pos.z = z;
	temp.time = time;
	temp.fdiry = diry;
	temp.falpha = alpha;
	for(std::list<CTextureProjective*>::iterator it = m_listmytp.begin();it != m_listmytp.end();it++)
	{
		if ((time < (*it)->m_ilifetime&&(*it)->m_ilifetime != 0) ||(*it)->m_ilifetime == 0) 
		{
			(*it)->m_listpostime.push_back(temp);
		}

	}
}
void CTextureProjective::SetLookatPos(D3DXVECTOR3 *pos)
{
	this->m_pvlookatpoint = pos;
	this->m_vseepoint    =  *pos;
	this->m_vseepoint.y  = this->m_vseepoint.y + 1.0f;
	this->UpdateMatView();
}
void CTextureProjective::SetNowTime(DWORD* time)
{
	this->m_dwnowtime = time;
}
void CTextureProjective::SetVisiable(BOOL visable)
{
	for(std::list<CTextureProjective*>::iterator it = m_listmytp.begin();it != m_listmytp.end();it++)
	{
		(*it)->m_bvisable = visable;
	}
}
void CTextureProjective::UpdateMatView()
{
	D3DXMatrixLookAtLH(&m_matview,&m_vseepoint,m_pvlookatpoint,&m_vheaddir);
}
void CTextureProjective::SetLifeTime(long time)
{
	m_ilifetime = time;
}
void CTextureProjective::SetSize(DWORD dwsize)
{
	m_dwsize = dwsize;
	m_vectornum = (m_dwsize * 2 - 1) * (m_dwsize * 2 - 1) * 6;
}
void CTextureProjective::SetBufferSize(DWORD size)
{
	if (size > m_dwbuffersize)
	{
		DWORD temnum = (size * 2 - 1) * (size * 2 - 1) * 6;
		if (m_pposbuffer!=NULL)
		{
			delete[] m_pposbuffer;
			m_pposbuffer = new D3DXVECTOR3[temnum];
		}
		else
		{
			m_pposbuffer = new D3DXVECTOR3[temnum];
		}
		m_dwbuffersize = size;
	}

}
void CTextureProjective::AddRendMesh(D3DXVECTOR3* pos,DWORD num)
{
	CTextureProjective::BUILDMESH p;
	p.pos = pos;
	p.num = num;
	m_listmesh.push_back(p);
}
void CTextureProjective::SetTextureState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	//pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE,TRUE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

}
void CTextureProjective::SetShadowState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	if (!m_bsimpleshadow)
	{
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(0,0,0,0));
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
		/*pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );*/
		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
		/*pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
		pd3dDevice->SetRenderState(D3DRS_ALPHAREF,D3DCOLOR_ARGB(0,0,0,0));
		pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);*/
	}
	else
	{

		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);

	}


}
void CTextureProjective::SetState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();

	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER );

	//	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pd3dDevice->SetRenderState( D3DRS_ZENABLE,TRUE);
	pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4|D3DTTFF_PROJECTED);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );


}
DWORD CTextureProjective::InterpolationColor(DWORD dwTimeElapse)
{
	//插值
	//A -+- B
	DWORD dwFrameA = ((DWORD)m_vcolor.size() -1) * dwTimeElapse / m_dwcolortime;
	if (dwFrameA >= m_vcolor.size() - 1)
	{
		//不需要插值
		dwFrameA = (DWORD)m_vcolor.size() - 1; 			
		return m_vcolor[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_vcolor.size(); 
	//get A time
	DWORD dwTimeA =  dwFrameA * m_dwcolortime /  (m_vcolor.size()-1);
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fFrameInterval = (float)m_dwcolortime / (float)(m_vcolor.size()-1);
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 
	/*if (fRatio > 1.0f)
	{
	dwFrameB = (dwFrameA + 2) % m_vcolor.size();
	float fFrameInterval = (float)m_dwcolortime / (float)m_vcolor.size()/2.0f;
	}*/
	//interpolation	color a --- b
	DWORD dwFrameAColor = m_vcolor[dwFrameA];   	
	DWORD dwFrameBColor = m_vcolor[dwFrameB];  

	/*float fAA = (float)((dwFrameAColor & 0xFF000000) >> 24);*/
	float fRA = (float)((dwFrameAColor & 0x00FF0000) >> 16);
	float fGA = (float)((dwFrameAColor & 0x0000FF00) >>  8);
	float fBA = (float)( dwFrameAColor & 0x000000FF);

	/*float fAB = (float)((dwFrameBColor & 0xFF000000) >> 24);*/
	float fRB = (float)((dwFrameBColor & 0x00FF0000) >> 16);
	float fGB = (float)((dwFrameBColor & 0x0000FF00) >>  8);
	float fBB = (float)( dwFrameBColor & 0x000000FF);

	/*DWORD dwA = (DWORD)(fAA + (fAB - fAA) * fRatio);
	if (dwA > 255) dwA = 255;*/
	DWORD dwR = (DWORD)(fRA + (fRB - fRA) * fRatio);
	if (dwR > 255)
	{
		dwR = 255;
	};
	DWORD dwG = (DWORD)(fGA + (fGB - fGA) * fRatio);
	if (dwG > 255) 
	{
		dwG = 255;
	}
	DWORD dwB = (DWORD)(fBA + (fBB - fBA) * fRatio);
	if (dwB > 255) 
	{
		dwB = 255;
	}
	return D3DCOLOR_ARGB(255,dwR,dwG,dwB);

}
float CTextureProjective::InterpolationVisibility(DWORD dwTimeElapse)
{
	//插值
	//A -+- B
	DWORD dwFrameA = (DWORD)m_vvisiblity.size() * dwTimeElapse / m_dwvisibilitytime;
	if (dwFrameA >= m_vvisiblity.size() - 1)
	{
		//不需要插值
		dwFrameA = (DWORD)m_vvisiblity.size() - 1; 			
		return m_vvisiblity[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_vvisiblity.size(); 
	//get A time
	DWORD dwTimeA =  dwFrameA * m_dwvisibilitytime /  m_vvisiblity.size();
	//计算A B 之间 ratio
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fFrameInterval = (float)m_dwvisibilitytime / (float)(m_vvisiblity.size() - 1);
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

	//interpolation	a --- b
	float fFrameAData = m_vvisiblity[dwFrameA];   	
	float fFrameBData = m_vvisiblity[dwFrameB];   	
	return fFrameAData + (fFrameBData - fFrameAData) * fRatio;
}
float CTextureProjective::InterpolationSize(DWORD dwTimeElapse)
{
	DWORD dwFrameA = (DWORD)m_vsize.size() * dwTimeElapse / m_dwsizetime;
	if (dwFrameA >= m_vsize.size() - 1)
	{
		//不需要插值
		dwFrameA = (DWORD)m_vsize.size() - 1; 			
		return m_vsize[dwFrameA];   	
	}

	//else
	DWORD dwFrameB = (dwFrameA + 1) % m_vsize.size(); 
	//get A time
	DWORD dwTimeA =  dwFrameA * m_dwsizetime /  m_vsize.size();
	//计算A B 之间 ratio
	DWORD dwTimeRelateA = dwTimeElapse - dwTimeA;
	float fFrameInterval = (float)m_dwsizetime / (float)(m_vsize.size() - 1);
	float fRatio  =  (float)dwTimeRelateA / fFrameInterval ; 

	//interpolation	a --- b
	float fFrameAData = m_vsize[dwFrameA];   	
	float fFrameBData = m_vsize[dwFrameB];   	
	return fFrameAData + (fFrameBData - fFrameAData) * fRatio;
}
void CTextureProjective::SetRotation(DWORD dwTimeElapse)
{
	float rota = 2 *  D3DX_PI* ((float)dwTimeElapse/(float)m_dwrotationtime);
	D3DXMATRIX matroty;
	D3DXVECTOR3 y(0.0f,0.1f,0.0f);
	D3DXVECTOR3 dir(0.0f,0.0f,1.0f);
	if (m_dwrotadir == 1)
	{
		D3DXMatrixRotationAxis(&matroty,&y,rota);
	}
	else
	{
		D3DXMatrixRotationAxis(&matroty,&y,-rota);
	}

	D3DXVec3TransformCoord(&m_vheaddir,&dir,&matroty);
	UpdateMatView();

}
void CTextureProjective::SetDir(float fdir)
{
	D3DXMATRIX matroty;
	D3DXVECTOR3 y(0.0f,0.1f,0.0f);
	D3DXVECTOR3 dir(0.0f,0.0f,1.0f);
	D3DXMatrixRotationAxis(&matroty,&y,fdir);
	D3DXVec3TransformCoord(&m_vheaddir,&dir,&matroty);
	UpdateMatView();

}
void CTextureProjective::SetAlpha(float alpha)
{
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
	}

	/*ffloat fA = (float)( (m_mixcolor & 0xFF000000) >> 24);*/
	float fR = (float)( (m_mixcolor & 0x00FF0000) >> 16);
	float fG = (float)( (m_mixcolor & 0x0000FF00) >>  8);
	float fB = (float)(  m_mixcolor & 0x000000FF);


	DWORD c = (DWORD)( 255.0f * alpha);
	m_mixcolor &= D3DCOLOR_ARGB(0,255,255,255);
	m_mixcolor |= D3DCOLOR_ARGB(c,0,0,0);
	if (m_mixedmodel != 2)
	{
		c = (DWORD)( fR * alpha);
		m_mixcolor &= D3DCOLOR_ARGB(255,0,255,255);
		m_mixcolor |= D3DCOLOR_ARGB(0,c,0,0);
		c = (DWORD)( fG * alpha);
		m_mixcolor &= D3DCOLOR_ARGB(255,255,0,255);
		m_mixcolor |= D3DCOLOR_ARGB(0,0,c,0);
		c = (DWORD)( fB * alpha);
		m_mixcolor &= D3DCOLOR_ARGB(255,255,255,0);
		m_mixcolor |= D3DCOLOR_ARGB(0,0,0,c);
	}


}
/*D3DXMATRIX CTextureProjective::UpdateUVAnim(DWORD dwTimeElapse)
{
float frame  = ((float)(dwTimeElapse%m_dwuvtime)/((float)m_dwuvtime)) * (float)(m_dwrow * m_dwrow);
int dy = (int)(frame/m_dwrow);
int dx = frame - (dy * m_dwrow);
float scalstart = (float)dy/(float)m_dwrow;
float scalw = 1.0f/(float)m_dwrow;
float scalend = scalstart + scalw;
D3DXMATRIX matscal;
D3DXMATRIX mattran;
D3DXMatrixTranslation( &mattran,scalstart,scalend - 1.0f, 0.0f );
matscal = m_mattranscal * mattran;
return matscal;

}*/
void CTextureProjective::SetDiffuseColor(DWORD color)
{
	m_mixcolor = color;
}
void CTextureProjective::CreateFromFile(CRFile* pfile)
{
	if(NULL == pfile)	return;

	/*static char pszResourcePath[_MAX_PATH];

	utility::File::GetFilePath(pfile->GetFileName(),pszResourcePath);

	utility::File file;
	if (!file.Open(pfile->GetFileName()))
	{
	OutputConsole("error: CreateFromFile() \"%s\" failed!\n",pfile->GetFileName());
	ASSERT(0);
	}*/

	char *szParameter = new char[pfile->GetDatalen() + 1];
	memcpy(szParameter,pfile->GetData(),pfile->GetDatalen());
	szParameter[pfile->GetDatalen()] = 0;
	//file.Close();

	utility::Text cScript;
	cScript.Create(szParameter);

	//读取SHADER
	utility::Text::Node *pShaderNode = cScript.GetRootNode()->GetChildNode("shader");
	//读取参数
	utility::Text::Node *pParameterNode = cScript.GetRootNode()->GetChildNode("parameter");
	//strncpy(m_tParameter.szName,pParameterNode->GetVar("name")->GetStringValue(),MAX_EMITTER_NAME - 1);
	//m_tParameter.szName[MAX_EMITTER_NAME - 1] = 0;
	utility::Text::Variable* pTempVar(NULL);
	utility::Text::Node* ptemptpnode;
	utility::Text::Node* ptempnode;
	DWORD shadercount = pParameterNode->GetChildCount();
	for(DWORD i = 0;i < shadercount;i++)
	{
		ptemptpnode = pParameterNode->GetChildNode(i);
		CTextureProjective* ptp;
		if (i == 0)
		{
			ptp = this;
		}
		else
		{
			ptp = CTextureProjective::CreateTextureProjctive();
		}
		ASSERT(ptp);
		//ptp->m_dwtexturegroup = this->m_dwtexturegroup;
		//ptp->m_dwtexturegroupindex = i;

		_snprintf(ptp->m_pszShaderFileName,_MAX_PATH,"%s",pShaderNode->GetVar("file")->GetStringValue());
		//大小
		pTempVar = ptemptpnode->GetVar("maxsize");
		if ( pTempVar == NULL || pTempVar->GetIntValue() == 0)
		{
			MessageBox(NULL,"没有设定投影纹理BUFFER大小!","配置错误",MB_OK);
			return;
		}
		SetBufferSize(pTempVar->GetIntValue());
		ptp->SetSize(pTempVar->GetIntValue());
		//混合模式
		pTempVar = ptemptpnode->GetVar("mixedmodel");
		if (pTempVar)
		{
			if(strcmp(pTempVar->GetStringValue(),"blendadd") == 0)
			{
				ptp->m_mixedmodel = 0;
			}
			else 	if (strcmp(pTempVar->GetStringValue(),"blendsub") == 0)
			{
				ptp->m_mixedmodel = 1;
			}
			else if(strcmp(pTempVar->GetStringValue(),"alpha") == 0)
			{
				ptp->m_mixedmodel = 2;
			}
		}

		//BILLBOARD
		pTempVar		   = ptemptpnode->GetVar("billboard_y");
		ptp->m_billboardy	   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;
		//生命时间
		pTempVar		   = ptemptpnode->GetVar("lifetime");
		ptp->m_ilifetime	   = (pTempVar)? pTempVar->GetIntValue() : 0;
		//可见度
		ptempnode           = ptemptpnode->GetChildNode("visibility");
		if (ptempnode)
		{
			ptp->m_dwvisibilitytime = ptempnode->GetChildNode("time")->GetVar("time")->GetIntValue();
			ptp->m_vvisiblity.resize(ptempnode->GetVarCount());
			for (unsigned int i = 0;i < ptp->m_vvisiblity.size();i++)
			{
				ptp->m_vvisiblity[i] = ptempnode->GetVar(i)->GetFloatValue();
			}

		}
		//颜色变化
		ptempnode           = ptemptpnode->GetChildNode("color");
		if (ptempnode)
		{
			ptp->m_dwcolortime = ptempnode->GetChildNode("time")->GetVar("time")->GetIntValue();
			ptp->m_vcolor.resize(ptempnode->GetVarCount());
			for (int i = 0;i < (int)ptp->m_vcolor.size();i++)
			{
				ptp->m_vcolor[i] = ptempnode->GetVar(i)->GetHexValue();
			}
		}

		//缩放大小变化
		ptempnode           = ptemptpnode->GetChildNode("size");
		if (ptempnode)
		{
			ptp->m_dwsizetime = ptempnode->GetChildNode("time")->GetVar("time")->GetIntValue();
			ptp->m_vsize.resize(ptempnode->GetVarCount());
			for (int i = 0;i < (int)ptp->m_vsize.size();i++)
			{
				ptp->m_vsize[i] = ptempnode->GetVar(i)->GetFloatValue();
			}

		}
		//缩放大小变化
		ptempnode           = ptemptpnode->GetChildNode("size");
		if (ptempnode)
		{
			ptp->m_dwsizetime = ptempnode->GetChildNode("time")->GetVar("time")->GetIntValue();
			ptp->m_vsize.resize(ptempnode->GetVarCount());
			for (int i = 0;i < (int)ptp->m_vsize.size();i++)
			{
				ptp->m_vsize[i] = ptempnode->GetVar(i)->GetFloatValue();
			}

		}
		//序列帧动画属性
		/*ptempnode           = ptemptpnode->GetChildNode("uvanim");
		if (ptempnode)
		{
		pTempVar                  = ptempnode->GetVar("time");
		ptp->m_dwuvtimeperframe   = (pTempVar)? pTempVar->GetIntValue() : 0;
		ptp->m_dwrow              = ptempnode->GetVar("row")->GetIntValue();
		ptp->m_dwuvtime           = ptp->m_dwuvtimeperframe * ptp->m_dwrow * ptp->m_dwrow;
		}*/

		//旋转
		pTempVar		        = ptemptpnode->GetVar("rotation");
		ptp->m_dwrotationtime   = (pTempVar)? pTempVar->GetIntValue() : 0;
		pTempVar		        = ptemptpnode->GetVar("rotadir");
		ptp->m_dwrotadir		= (pTempVar)? pTempVar->GetIntValue() : 1;
		pTempVar		        = ptemptpnode->GetVar("followedowner");
		ptp->m_bfollowedowner   = (pTempVar)? pTempVar->GetBOOLValue() : FALSE;


		AddNewTextureProj(ptp);

	}

	SAFEDELETEARRAY(szParameter);
	cScript.Destroy();

}
BOOL CTextureProjective::CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision/* =0 */)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	m_dwtexturegroup = pLayer3D->CreateShaderGroup(pFile,dwTextureProcision);
	//get shader group handle
	int i = 0;
	for (std::list<CTextureProjective*>::iterator it = m_listmytp.begin();it != m_listmytp.end();it++,i++)
	{
		(*it)->m_dwtexturegroup = m_dwtexturegroup;
		(*it)->m_dwtexturegroupindex = i;
	}

	return TRUE;
}
void CTextureProjective::SetMixedModel()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	if (m_mixedmodel == 0)
	{
		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD	);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	}
	else if (m_mixedmodel == 1)
	{
		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_REVSUBTRACT);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	}
	else if (m_mixedmodel == 2)
	{
		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD	);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	}
}
void CTextureProjective::Rend(D3DXVECTOR3* pcameraposition,D3DXMATRIX matworld,D3DXMATRIX matview,D3DXMATRIX matproj)
{
	if (!m_dwnowtime||!m_pvlookatpoint||m_dwtexturegroup == -1)
	{
		return;
	}
	//更新时间
	/*if (m_dwstarttime == 0)
	{

	m_dwstarttime = *m_dwnowtime;


	}*/
	//更新时间
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	render::Interface::Layer3D* play3d = pInterface->GetLayer3D();
	//pd3dDevice->GetTransform(D3DTS_VIEW,&m_mattexture);
	//D3DXMatrixInverse(&m_mattexture,NULL,&m_mattexture);

	//D3DXMatrixOrthoLH(&matproj, (float)(m_fsize * 2 - 1),(float)(m_fsize * 2 - 1),1.0f,100.0f);

	//BILLBOARD效果
	if (m_billboardy)
	{


		D3DXVECTOR3 vseedir = *pcameraposition - *m_pvlookatpoint;
		//	D3DXVec3Normalize(&vseedir,&vseedir);
		vseedir.y = 0.0f;
		m_vheaddir = vseedir;
		UpdateMatView();


	}
	//BILLBOARD效果
	pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,255,255,255));
	m_mixcolor = D3DCOLOR_ARGB(255,255,255,255);
	//变色效果
	if (m_vcolor.size() != 0)
	{
		SetDiffuseColor(InterpolationColor((*m_dwnowtime - m_dwstarttime)%m_dwcolortime));	
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,m_mixcolor);
	}
	//变色效果

	if (m_falpha != -1.0f)
	{
		if (m_vvisiblity.size() != 0)
		{
			m_falpha = m_falpha < InterpolationVisibility((*m_dwnowtime - m_dwstarttime)%m_dwvisibilitytime)?m_falpha:InterpolationVisibility((*m_dwnowtime - m_dwstarttime)%m_dwvisibilitytime);
		}
		SetAlpha(m_falpha);
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,m_mixcolor);
	}
	//可见性效果
	else if (m_vvisiblity.size() != 0)
	{
		SetAlpha(InterpolationVisibility((*m_dwnowtime - m_dwstarttime)%m_dwvisibilitytime));
		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,m_mixcolor);
	}
	//可见性效果
	SetMixedModel();
	//缩放效果
	if (m_vsize.size() != 0)
	{
		float size = InterpolationSize((*m_dwnowtime - m_dwstarttime)%m_dwsizetime)*2.0f;
		D3DXMatrixOrthoLH(&m_matproj, size,size,1.0f,100.0f);
	}
	else
	{
#pragma warning(push)
#pragma warning(disable:4244)
		D3DXMatrixOrthoLH(&m_matproj, m_dwsize,m_dwsize,1.0f,100.0f);
#pragma warning(pop)
	}
	//缩放效果
	if (!m_billboardy&&m_bfollowedowner)
	{
		SetDir(m_fdir);
	}
	//旋转
	else if (!m_billboardy&&m_dwrotationtime)
	{
		SetRotation((*m_dwnowtime - m_dwstarttime)%m_dwrotationtime);

	}


	//m_mattexture = m_mattexture * m_matview * m_matproj * m_mattranscal;
	D3DXMATRIX matgird = matworld * matview * matproj;
	D3DXMATRIX matgridprj = matworld * m_matview * m_matproj * m_mattranscal;

	//pd3dDevice->SetTransform(D3DTS_TEXTURE0,&m_mattexture);

	play3d->DrawTriangleArray(m_dwtexturegroup,m_dwtexturegroupindex,m_vectornum/3,m_pposbuffer,matgird,matgridprj);
}
void CTextureProjective::CheckReSetReBuild()
{
	ReSetShadowReBuild();
	ReSetRTTReBuild();
}
void CTextureProjective::CheckReSetRelease()
{
	ReSetShadowRelease();
	ReSetRTTRelease();
}
void CTextureProjective::ReSetRTTRelease()
{
	for (std::vector<RTTRESOURCE>::iterator it = m_vecrtt.begin();it != m_vecrtt.end();it++)
	{
		(*it).Release();
		(*it).used = false;
		(*it).bupdated = false;
	}
	SAFERELEASE(m_pRttAlphaMainPlayer);
	SAFERELEASE(m_pRttAlphaCircle);
	SAFERELEASE(m_pRTTZ);
}
void CTextureProjective::ReSetRTTReBuild()
{
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	for (std::vector<RTTRESOURCE>::iterator it = m_vecrtt.begin();it != m_vecrtt.end();it++)
	{
		pd3dDevice->CreateTexture( RTTTEXTUREWIDTH,RTTTEXTUREHEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,&((*it).pRttTexture), NULL);
	}
	for (int i = 0;i < (int)m_vecrtt.size();i++)
	{
		CreateRTTTexture();
	}
	pd3dDevice->CreateDepthStencilSurface( RTTTEXTUREWIDTH, RTTTEXTUREHEIGHT, render::Interface::GetInstance()->GetInfo()->d3dpp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE,&m_pRTTZ, NULL );
}
void CTextureProjective::ReSetShadowReBuild()
{
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	pd3dDevice->CreateDepthStencilSurface( SHADOWTEXTUREWIDTH, SHADOWTEXTUREHEIGHT, render::Interface::GetInstance()->GetInfo()->d3dpp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE,&m_pShadowZ, NULL );
}
void CTextureProjective::ReSetShadowRelease()
{
	for (std::list<SHADOWTEXTURE>::iterator it = m_listshadow.begin();it != m_listshadow.end();it++)
	{
		it->Release();
	}
	m_listshadow.clear();
	m_simpleshadow.Release();
	ClearNum();
	SAFERELEASE(m_pTextureSurf);
	SAFERELEASE(m_pOldBackBuffer);
	SAFERELEASE(m_pOldZBuffer);
	SAFERELEASE(m_pShadowZ);
}
void CTextureProjective::ClearNum()
{
	/*LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);*/
	m_ushadowusednum = 0;
}
HRESULT CTextureProjective::CreateShadowTexture()
{
	SHADOWTEXTURE temp;
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	// 创建阴影纹理
	/*if(FAILED(pd3dDevice->CreateTexture( SHADOWTEXTUREWIDTH,SHADOWTEXTUREHEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
	&temp.pShadowTexture, NULL) ) )
	return D3DERR_INVALIDCALL;*/

	CRFile* rfile;
	if (!m_simpleshadow.pShadowTexture)
	{
		rfile = rfOpen("Texture/simpleshadow.dds");
		D3DXCreateTextureFromFileInMemory(pd3dDevice,rfile->GetData(),rfile->GetDatalen(),&m_simpleshadow.pShadowTexture);
		if (!m_simpleshadow.pShadowTexture)
		{
			MessageBox(NULL,"简单阴影纹理读取错误!","错误",MB_OK);
		}
		rfClose(rfile);
	}
	if (m_bsimpleshadow)
	{
		temp.pShadowTexture = m_simpleshadow.pShadowTexture;
	}
	else
	{
		pd3dDevice->CreateTexture( SHADOWTEXTUREWIDTH,SHADOWTEXTUREHEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,&temp.pShadowTexture, NULL);
	}
	m_listshadow.push_back(temp);
	return S_OK;
}
void CTextureProjective::SetSimpleShadow(bool b /* = false */)
{
	if (m_bsimpleshadow == !b)
	{
		ReSetShadowRelease();
		ReSetShadowReBuild();
	}
	m_bsimpleshadow = b;
}
void CTextureProjective::BeginRendShadow(D3DXVECTOR3 pos,DWORD size)
{
	if (render::Interface::GetInstance()->GetDeviceLost())
	{
		return;
	}
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	m_ushadowusednum++;
	if ((int)m_listshadow.size() < m_ushadowusednum)
	{
		CreateShadowTexture();
	}

	SHADOWTEXTURE* pst = GetCanUseShadowTexture();
	pst->pShadowTexture->GetSurfaceLevel(0, &m_pTextureSurf);
	pst->pos = pos;
	pst->size = size;
	if (!m_bsimpleshadow)
	{
		if (m_pShadowZ == NULL)
		{
			pd3dDevice->CreateDepthStencilSurface( SHADOWTEXTUREWIDTH, SHADOWTEXTUREHEIGHT, render::Interface::GetInstance()->GetInfo()->d3dpp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE,&m_pShadowZ, NULL );
		}
		pd3dDevice->GetRenderTarget(0, &m_pOldBackBuffer);					// 保存原来的后备缓冲、深度缓冲区、视口、
		pd3dDevice->GetDepthStencilSurface(&m_pOldZBuffer);					// 相机视图矩阵、透视矩阵
		pd3dDevice->GetViewport(&m_oldViewPort);
		pd3dDevice->GetTransform( D3DTS_VIEW, &m_oldMatView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &m_oldMatProj);
		//	pd3dDevice->GetTransform( D3DTS_WORLD,&m_oldMatWorld);
		m_shadowViewPort.X = 0;
		m_shadowViewPort.Y = 0;
		m_shadowViewPort.Width = SHADOWTEXTUREWIDTH;
		m_shadowViewPort.Height = SHADOWTEXTUREHEIGHT;
		m_shadowViewPort.MinZ = 0.0f;
		m_shadowViewPort.MaxZ = 1.0f;
		pd3dDevice->SetViewport(&m_shadowViewPort);	
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);

		pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,40,40,40));
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		D3DXMatrixOrthoLH(&m_mattempprojective,(float)size,(float)size,0,1000.0f);
		//	pd3dDevice->EndScene();
		//pd3dDevice->BeginScene();
		D3DXMatrixLookAtLH(&m_mattempview,&D3DXVECTOR3(pst->pos.x - 4.0f,pst->pos.y - 10.0f,pst->pos.z - 4.0f),
			&pst->pos,
			&D3DXVECTOR3(0.0f,1.0f,0.0f));
		pd3dDevice->SetRenderTarget(0, m_pTextureSurf);			// 设置要渲染的阴影贴图面和深度缓冲区表面
		pd3dDevice->SetDepthStencilSurface(m_pShadowZ);
		pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );
		//pd3dDevice->SetTransform(D3DTS_VIEW,&mattempview);
		//pd3dDevice->SetTransform(D3DTS_PROJECTION,&mattempprojective);
	}




}
void CTextureProjective::AddRendShadow(CDisplayModel* pdmodel,AnimInfo* pAnimInfo)
{
	if (pdmodel&&!m_bsimpleshadow)
	{
		LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
		pdmodel->SetDirLightEnable(false);
		pAnimInfo->SetViewMatrix(&m_mattempview);
		pAnimInfo->SetProjectedMatrix(&m_mattempprojective);
		pdmodel->ProcessAnimJoint(pAnimInfo);
		pdmodel->DrawModelWithoutState();
		//D3DXSaveSurfaceToFile("1.tga",D3DXIFF_TGA,m_pTextureSurf,NULL,NULL);
	}
}
void CTextureProjective::EndRendShadow()
{
	if (render::Interface::GetInstance()->GetDeviceLost())
	{
		return;
	}

	if (!m_bsimpleshadow)
	{
		//D3DXSaveSurfaceToFile("1.tga",D3DXIFF_TGA,m_pTextureSurf,NULL,NULL);
		LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
		//pd3dDevice->EndScene();
		//	pd3dDevice->BeginScene();
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD );
		pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);
		pd3dDevice->SetTransform( D3DTS_VIEW, &m_oldMatView );				// 取回原来保存的设置
		pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_oldMatProj);
		//pd3dDevice->SetTransform( D3DTS_WORLD, &m_oldMatWorld);
		pd3dDevice->SetRenderTarget( 0, m_pOldBackBuffer);
		pd3dDevice->SetDepthStencilSurface(m_pOldZBuffer);
		pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );
		pd3dDevice->SetViewport(&m_oldViewPort);
		SAFERELEASE(m_pTextureSurf);
		SAFERELEASE(m_pOldBackBuffer);
		SAFERELEASE(m_pOldZBuffer);
	}
}
CTextureProjective::SHADOWTEXTURE* CTextureProjective::GetCanUseShadowTexture()
{
	std::list<SHADOWTEXTURE>::iterator it = m_listshadow.begin();
	for (UINT i = 1;i < m_ushadowusednum;i++)
	{
		it++;
	}
	return &(*it);
}
void CTextureProjective::RendShadow(SHADOWTEXTURE* p,D3DXVECTOR3 ldir,D3DXMATRIX matworld,D3DXMATRIX matview,D3DXMATRIX matproj)
{
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	render::Interface::Layer3D* play3d = render::Interface::GetInstance()->GetLayer3D();
	pd3dDevice->SetTexture(0,p->pShadowTexture);
	//pd3dDevice->GetTransform(D3DTS_VIEW,&m_matshadowtexture);
	//D3DXMatrixInverse(&m_matshadowtexture,NULL,&m_matshadowtexture);
	D3DXMATRIX matroty;
	D3DXVECTOR3 y(0.0f,0.1f,0.0f);
	D3DXVECTOR3 dir(0.0f,0.0f,1.0f);
	/*D3DXVECTOR3  lightdir;
	lightdir.x = ldir.x;
	lightdir.y = 0.0f;
	lightdir.z = ldir.z;
	D3DXVec3Normalize(&lightdir,&lightdir);
	float angle = FM_AngleBetweenVectors(lightdir,dir);
	D3DXMatrixRotationAxis(&matroty,&y,angle);
	D3DXVec3TransformCoord(&dir,&dir,&matroty);*/
	D3DXMatrixRotationAxis(&matroty,&y,D3DX_PI/4);
	D3DXVec3TransformCoord(&dir,&dir,&matroty);

	D3DXMatrixLookAtLH(&m_matshadowview,&D3DXVECTOR3(p->pos.x,p->pos.y + 1.0f,p->pos.z),&D3DXVECTOR3(p->pos.x,p->pos.y,p->pos.z),&dir);
	if (!m_bsimpleshadow)
	{
		D3DXMatrixOrthoLH(&m_matshadowproj,(float)p->size,(float)p->size,1.0f,200.0f);
	}
	else
	{
		D3DXMatrixOrthoLH(&m_matshadowproj,2.0,2.0,1.0f,200.0f);
	}
	D3DXMATRIX matgird = matworld * matview * matproj;
	D3DXMATRIX matgridprj = matworld * m_matshadowview * m_matshadowproj * m_matshadowscanf;
	//m_matshadowtexture = m_matshadowtexture * m_matshadowview * m_matshadowproj * m_matshadowscanf;
	//pd3dDevice->SetTransform(D3DTS_TEXTURE0,&m_matshadowtexture);
	play3d->DrawTriangleArray((p->size * 2 - 1) * (p->size *2 - 1) * 2,m_pposbuffer,matgird,matgridprj);
}
/*void CTextureProjective::RendShadowToTexture()
{

LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
pd3dDevice->GetRenderTarget(0, &m_pOldBackBuffer);					// 保存原来的后备缓冲、深度缓冲区、视口、
pd3dDevice->GetDepthStencilSurface(&m_pOldZBuffer);					// 相机视图矩阵、透视矩阵
pd3dDevice->GetViewport(&m_oldViewPort);
pd3dDevice->GetTransform( D3DTS_VIEW, &m_oldMatView );
pd3dDevice->GetTransform( D3DTS_PROJECTION, &m_oldMatProj);
//pd3dDevice->GetTransform( D3DTS_WORLD,&m_oldMatWorld);
m_shadowViewPort.X = 0;
m_shadowViewPort.Y = 0;
m_shadowViewPort.Width = SHADOWTEXTUREWIDTH;
m_shadowViewPort.Height = SHADOWTEXTUREHEIGHT;
m_shadowViewPort.MinZ = 0.0f;
m_shadowViewPort.MaxZ = 1.0f;
pd3dDevice->SetViewport(&m_shadowViewPort);	
pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
D3DXMATRIX mattempview;
D3DXMATRIX mattempprojective;
D3DXMatrixOrthoLH(&mattempprojective,SHADOWTEXTUREWIDTH,SHADOWTEXTUREHEIGHT,0,300.0f);
//D3DXMATRIX matworld;

// 设置阴影视图视口
for (std::list<GAMEMODELINFO>::iterator it = m_listmodel.begin();it != m_listmodel.end();it++)
{
pd3dDevice->BeginScene();
SAFERELEASE(m_pTextureSurf);
GAMEMODELINFO* pmi = (GAMEMODELINFO*)&it;
SHADOWTEXTURE* pst = GetCanUseShadowTexture();
pst->pShadowTexture->GetSurfaceLevel(0, &m_pTextureSurf);
pst->pos.x = pmi->pos.x;
pst->pos.y = pmi->pos.y;
pst->pos.z = pmi->pos.z;
pd3dDevice->SetRenderTarget(0, m_pTextureSurf);					// 设置要渲染的阴影贴图面和深度缓冲区表面
pd3dDevice->SetDepthStencilSurface(m_pTextureZ);
pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L );
D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(pst->pos.x - 300.0f,pst->pos.y + 300.0f,pst->pos.z - 300.0f),&pst->pos,&D3DXVECTOR3(0.0f,1.0f,0.0f));
pd3dDevice->SetTransform(D3DTS_VIEW,&mattempview);
pd3dDevice->SetTransform(D3DTS_PROJECTION,&mattempprojective);
pmi->pmodel->DrawModel();
pd3dDevice->EndScene();
}
pd3dDevice->SetTransform( D3DTS_VIEW, &m_oldMatView );				// 取回原来保存的设置
pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_oldMatProj);

pd3dDevice->SetRenderTarget( 0, m_pOldBackBuffer);
pd3dDevice->SetDepthStencilSurface(m_pOldZBuffer);
pd3dDevice->SetViewport(&m_oldViewPort);

}*/

void CTextureProjective::SetRttUsedFalse(int i)
{
	if (i >= 0 && i < (int)m_vecrtt.size())
	{
		m_vecrtt[i].used = false;
	}

}
int CTextureProjective::CreateRTTTexture()
{
	RTTRESOURCE temp;
	LPDIRECT3DTEXTURE9 temptexture;
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	CRFile* rfile;
	if (!m_pRttAlphaMainPlayer)
	{
		rfile = rfOpen("Texture/upbodyalpha.dds");
		D3DXCreateTextureFromFileInMemory(pd3dDevice,rfile->GetData(),rfile->GetDatalen(),&m_pRttAlphaMainPlayer);
		if (!m_pRttAlphaMainPlayer)
		{
			MessageBox(NULL,"主角头像Alpha贴图读取错误!","错误",MB_OK);
		}
		rfClose(rfile);
	}
	if (!m_pRttAlphaCircle)
	{
		rfile = rfOpen("Texture/headalpha.dds");
		D3DXCreateTextureFromFileInMemory(pd3dDevice,rfile->GetData(),rfile->GetDatalen(),&m_pRttAlphaCircle);
		if (!m_pRttAlphaCircle)
		{
			MessageBox(NULL,"队友头像Alpha贴图读取错误!","错误",MB_OK);
		}
		rfClose(rfile);
	}
	// 创建RTT纹理
	int i = 0;
	for (; i < (int)m_vecrtt.size();i++)
	{
		if (!m_vecrtt[i].used)
		{
			m_vecrtt[i].used = true;
			return i;
		}
	}
	pd3dDevice->CreateTexture( RTTTEXTUREWIDTH,RTTTEXTUREHEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,&temptexture, NULL);
	temp.pRttTexture = temptexture;
	temp.used = true;
	temp.bupdated = false;
	m_vecrtt.push_back(temp);
	return i;
}
bool CTextureProjective::BeginRendToTexture(int& texhandle)
{
	if (render::Interface::GetInstance()->GetDeviceLost())
	{
		return false;
	}
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	if (texhandle > (int)m_vecrtt.size() - 1)
	{
		return false;
	}
	if (texhandle == -1)
	{
		texhandle = CreateRTTTexture();
	}
	if (m_pRTTZ == NULL)
	{
		pd3dDevice->CreateDepthStencilSurface( RTTTEXTUREWIDTH, RTTTEXTUREHEIGHT, render::Interface::GetInstance()->GetInfo()->d3dpp.AutoDepthStencilFormat, D3DMULTISAMPLE_NONE, 0, TRUE,&m_pRTTZ, NULL );
	}

	pd3dDevice->GetRenderTarget(0, &m_pOldBackBuffer);					// 保存原来的后备缓冲、深度缓冲区、视口、
	pd3dDevice->GetDepthStencilSurface(&m_pOldZBuffer);					// 相机视图矩阵、透视矩阵
	pd3dDevice->GetViewport(&m_oldViewPort);
	pd3dDevice->GetTransform( D3DTS_VIEW, &m_oldMatView );
	pd3dDevice->GetTransform( D3DTS_PROJECTION, &m_oldMatProj);
	//pd3dDevice->GetTransform( D3DTS_WORLD,&m_oldMatWorld);
	m_shadowViewPort.X = 0;
	m_shadowViewPort.Y = 0;
	m_shadowViewPort.Width = RTTTEXTUREWIDTH;
	m_shadowViewPort.Height = RTTTEXTUREHEIGHT;
	m_shadowViewPort.MinZ = 0.0f;
	m_shadowViewPort.MaxZ = 1.0f;
	pd3dDevice->SetViewport(&m_shadowViewPort);	
	//pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(0,0,0,0));
	/*pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );*/
	pd3dDevice->SetSamplerState(0,D3DSAMP_MIPFILTER,D3DTEXF_NONE);	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE,true);
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE,true);
	D3DXMATRIX matworld;
	D3DXMatrixIdentity(&matworld);
	pd3dDevice->SetTransform(D3DTS_WORLD,&matworld);
	//pd3dDevice->EndScene();
	//pd3dDevice->BeginScene();
	LPDIRECT3DTEXTURE9 pt = m_vecrtt[texhandle].pRttTexture;
	m_vecrtt[texhandle].bupdated = true;
	pt->GetSurfaceLevel(0, &m_pTextureSurf);
	pd3dDevice->SetRenderTarget(0, m_pTextureSurf);					
	pd3dDevice->SetDepthStencilSurface(m_pRTTZ);
	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );
	return true;



}
bool CTextureProjective::RendToTexture(CDisplayModel* pmodel,AnimInfo* pani,int type)
{

	if (pmodel/*&&pmodel->GetLoadState() == Load_Did*/)
	{
		LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
		D3DXMATRIX matWorld,mattempview;
		D3DXMATRIX mattempprojective;
		//float fwidth = 0.0f;
		//全身照
		if (type == TYPE_BODY)
		{
			D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(0.0f,1.0f, 4.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f));
			//fwidth = 3.0f;
		}
		//胸以上部位照片(左侧面)
		else if (type == TYPE_UPBODYLEFT||type == TYPE_UPBODYHEADWEARL)
		{
			D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(1.3f,1.9f, 1.4f),
				&D3DXVECTOR3(-0.035f,1.09f, 0.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f));
			//fwidth = 3.0f;
		}
		//胸以上部位照片(右侧面)
		else if (type == TYPE_UPBODYRIGHT||type == TYPE_UPBODYHEADWEARR)
		{
			D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(-1.3f,2.0f, 1.8f),
				&D3DXVECTOR3(0.06f,0.9f, 0.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f));
			//fwidth = 3.0f;
		}
		//巨大模型全身照
		else if (type == TYPE_HUGEBODY)
		{
			D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(0.0f,1.5f, 5.0f),
				&D3DXVECTOR3(0.0f,1.5f, 0.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f));
			//fwidth = 3.0f;
		}
		//大头照
		else if (type == TYPE_HEAD||TYPE_HEADHEADWEARL)
		{
			D3DXMatrixLookAtLH(&mattempview,&D3DXVECTOR3(1.5f,1.85f, 1.8f),
				&D3DXVECTOR3(0.05f,1.02f, 0.0f),
				&D3DXVECTOR3(0.0f,1.0f, 0.0f));
			//fwidth = 3.0f;
		}
        
		D3DXMatrixPerspectiveFovLH(&mattempprojective,D3DX_PI/4.0f,1.0f,1.0f,-3.0f);
		static D3DXMATRIX matbillboard;
		D3DXMatrixInverse(&matbillboard,NULL,&mattempview);
		matbillboard._41 = matbillboard._42 = matbillboard._43 = 0.0f;
		pani->SetBillboard(&matbillboard);
		DWORD nowtime = timeGetTime();
		pani->SetCurrentTime(nowtime);
		pmodel->SetDirLightEnable(false);
		pani->SetViewMatrix(&mattempview);
		pani->SetProjectedMatrix(&mattempprojective);
		pmodel->ProcessAnimJoint(pani);
		pmodel->ProcessEnvironmentMaps(&mattempview);
		pmodel->ProcessAnimColor(pani);
		pmodel->ProcessVisibility(pani);
		pmodel->ProcessAnimTexture(pani);
		pmodel->ProcessAnimUV(pani);
		pmodel->ProcessLocator(pani);
		//pmodel->ProcessParticleEmitter(pani,false,true);
		//D3DXMatrixOrthoLH(&mattempprojective,fwidth,fwidth,2.5f,-1000.0f);
		//pmodel->DrawModelWithoutState();
		if (type == TYPE_UPBODYLEFT||type == TYPE_UPBODYRIGHT||type == TYPE_HEAD)
		{
			pmodel->DrawModel(MAKEFOURCC('F','A','C','E'));
			pmodel->DrawModel(MAKEFOURCC('A','R','M','O'));
			pmodel->DrawModel(MAKEFOURCC('H','A','I','R'));
			pmodel->DrawModel(MAKEFOURCC('B','O','D','Y'));
		}
		/*else if (type == TYPE_HEAD)
		{
		pmodel->DrawModel(MAKEFOURCC('F','A','C','E'));
		pmodel->DrawModel(MAKEFOURCC('A','R','M','O'));
		pmodel->DrawModel(MAKEFOURCC('H','A','I','R'));
		pmodel->DrawModel(MAKEFOURCC('H','E','A','D'));
		pmodel->DrawModel(MAKEFOURCC('B','O','D','Y'));
		}*/
		else
		{
			pmodel->DrawModel();
		}
		static D3DXMATRIX mattempworld;
		D3DXMatrixIdentity(&mattempworld);
		pd3dDevice->SetTransform( D3DTS_WORLD, &mattempworld);
		pd3dDevice->SetTransform(D3DTS_VIEW,&mattempview);
		pd3dDevice->SetTransform(D3DTS_PROJECTION,&mattempprojective);
		//D3DXSaveSurfaceToFile("1.bmp",D3DXIFF_BMP,CTextureProjective::m_pTextureSurf,NULL,NULL);
		return true;


	}
	return false;

}
void CTextureProjective::EndRendToTexture()
{
	if (!m_pOldZBuffer)
	{
		SAFERELEASE(m_pTextureSurf);
		SAFERELEASE(m_pOldBackBuffer);
		SAFERELEASE(m_pOldZBuffer);
		return;
	}
	//D3DXSaveSurfaceToFile("1_b.bmp",D3DXIFF_BMP,m_pTextureSurf,NULL,NULL);
	LPDIRECT3DDEVICE9 pd3dDevice = render::Interface::GetInstance()->GetDevice();
	//pd3dDevice->EndScene();
	//pd3dDevice->BeginScene();
	pd3dDevice->SetTransform( D3DTS_VIEW, &m_oldMatView );				// 取回原来保存的设置
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_oldMatProj);
	pd3dDevice->SetRenderTarget( 0, m_pOldBackBuffer);
	pd3dDevice->SetDepthStencilSurface(m_pOldZBuffer);
	pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0,255,0,0), 1.0f, 0L );
	pd3dDevice->SetViewport(&m_oldViewPort);
	SAFERELEASE(m_pTextureSurf);
	SAFERELEASE(m_pOldBackBuffer);
	SAFERELEASE(m_pOldZBuffer);

}
void CTextureProjective::DrawTextureToScreen(int x,int y,int s,int handle,int type)
{
	if (handle == -1)
	{
		return;
	}
	if (!m_vecrtt[handle].bupdated)
	{
		return;
	}
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	render::Interface::Layer3D* play3d = render::Interface::GetInstance()->GetLayer3D();

	D3DXVECTOR3 pos[] = 
	{
		/*D3DXVECTOR3(-1.0f,  1.0f,-10000.0f),
		D3DXVECTOR3( 1.0f, -1.0f,-10000.0f),
		D3DXVECTOR3(-1.0f, -1.0f,-10000.0f),
		D3DXVECTOR3(-1.0f,  1.0f,-10000.0f),
		D3DXVECTOR3( 1.0f,  1.0f,-10000.0f),
		D3DXVECTOR3( 1.0f, -1.0f,-10000.0f),*/
		D3DXVECTOR3(-1.0f,  1.0f,1.0f),
		D3DXVECTOR3( 1.0f, -1.0f,1.0f),
		D3DXVECTOR3(-1.0f, -1.0f,1.0f),
		D3DXVECTOR3(-1.0f,  1.0f,1.0f),
		D3DXVECTOR3( 1.0f,  1.0f,1.0f),
		D3DXVECTOR3( 1.0f, -1.0f,1.0f),


	};
	static D3DXVECTOR2 uv[] = 
	{
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(1.0f,1.0f),
		D3DXVECTOR2(0.0f,1.0f),
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(1.0f,0.0f),
		D3DXVECTOR2(1.0f,1.0f),
	};
	static D3DXVECTOR2 uv1[] = 
	{
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(1.0f,1.0f),
		D3DXVECTOR2(1.0f,1.0f),
		D3DXVECTOR2(0.0f,1.0f),
		D3DXVECTOR2(0.0f,1.0f),
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(0.0f,0.0f),
		D3DXVECTOR2(1.0f,0.0f),
		D3DXVECTOR2(1.0f,0.0f),
		D3DXVECTOR2(1.0f,1.0f),
		D3DXVECTOR2(1.0f,1.0f),
	};


	float fWidth = (float)pInterface->GetWndWidth();
	float fHeight= (float)pInterface->GetWndHeight();	

	float fDrawX = x - fWidth / 2;
	float fDrawY = fHeight / 2 - y;
	static D3DXMATRIX matWorld,matView;
	D3DXMatrixIdentity( &matWorld );

	static D3DXMATRIX matProj;
	D3DXMatrixOrthoLH( &matProj,fWidth, fHeight, -10000.0f,10000.0f );

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld);
	//D3DXMatrixTranslation(&matWorld,0,0,10000.0f);
	/*D3DXMatrixLookAtLH(&matView,&D3DXVECTOR3(0.0f,0.0f, -10000.0f)
	,&D3DXVECTOR3(0.0f,0.0f, 0.0f)
	,&D3DXVECTOR3(0.0f,1.0f, 0.0f));*/
	D3DXMatrixTranslation(&matView,0.0f,0.0f,-10000.0f);
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView);
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj);
	static D3DXMATRIX matModelWorld,matSCL;
#pragma warning(push)
#pragma warning(disable:4244)
	D3DXMatrixScaling(&matSCL,s,s,1.0f);
#pragma  warning(pop)
	D3DXMatrixTranslation(&matModelWorld,(float)fDrawX,(float)fDrawY,0.0f);
	D3DXMatrixMultiply(&matModelWorld,&matSCL,&matModelWorld);
	for (int i = 0;i < 6;i++)
	{
		D3DXVec3TransformCoord(&pos[i],&pos[i],&matModelWorld);
	}

	if (type != ALPHATYPE_NONE)
	{
		if (/*render::Interface::GetInstance()->GetSetting()->dwMaxTexStage < 2*/1)
		{
			if (type == ALPHATYPE_MAINPLAYER)
			{
				pd3dDevice->SetTexture(0,m_pRttAlphaMainPlayer);
			}
			else if (type == ALPHATYPE_CIRCLE)
			{
				pd3dDevice->SetTexture(0,m_pRttAlphaCircle);
			}
			pd3dDevice->Clear( 0L, NULL, D3DCLEAR_STENCIL, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);
			pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,			TRUE);
			pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,			D3DCMP_EQUAL);
			pd3dDevice->SetRenderState(D3DRS_STENCILREF,			0x0);
			pd3dDevice->SetRenderState(D3DRS_STENCILMASK,			0xffffffff);
			pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK,		0xffffffff);
			pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL,			D3DSTENCILOP_KEEP);
			pd3dDevice->SetRenderState(D3DRS_STENCILFAIL,			D3DSTENCILOP_KEEP);
			pd3dDevice->SetRenderState(D3DRS_STENCILPASS,			D3DSTENCILOP_INCR);	// increment to 1

			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
			pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			play3d->DrawTriangleArray(2,pos,uv1,true);
			pd3dDevice->SetTexture(0,m_vecrtt[handle].pRttTexture);
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			pd3dDevice->SetRenderState(D3DRS_STENCILREF,			0x1);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			play3d->DrawTriangleArray(2,pos,uv1,true);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,			FALSE);


		}
		else
		{
			pd3dDevice->SetTexture(0,m_vecrtt[handle].pRttTexture);
			if (type == ALPHATYPE_MAINPLAYER)
			{
				pd3dDevice->SetTexture(1,m_pRttAlphaMainPlayer);
			}
			else if (type == ALPHATYPE_CIRCLE)
			{
				pd3dDevice->SetTexture(1,m_pRttAlphaCircle);
			}
			pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
			pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
			pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

			pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG2);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			play3d->DrawTriangleArray(2,pos,uv1,true);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
			pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

		}
	}
	else
	{
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
		play3d->DrawTriangleArray(2,pos,uv);
	}

}
void CTextureProjective::EndState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pd3dDevice = pInterface->GetDevice();
	//pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	//pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
	//pd3dDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
	//pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,TRUE);
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0);
	pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	pd3dDevice->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD );
	pd3dDevice->SetRenderState(D3DRS_FOGENABLE, false);
	//pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,255,255,255));

}

LPDIRECT3DTEXTURE9 CTextureProjective::GetRTTtexture(int texHandle)
{
    if (texHandle>=0 && (size_t)texHandle<m_vecrtt.size())
    {
        return m_vecrtt[texHandle].pRttTexture;
    }
    return NULL;
}