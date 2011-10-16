#include "StdAfx.h"


#include "../Engine/console.h"
#include "../Engine/utility.h"
#include "../Engine/render.h"

#include "gamemodel.h"
#include "gamegraphics2d.h"


IMPLEMENT_SINGLETON(GameGraphics2D);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD GameGraphics2D::m_dwFogEnable = 0;
GameGraphics2D::GameGraphics2D(void)
{
}

GameGraphics2D::~GameGraphics2D(void)
{
}



/*
 * 功能: 创建2D图象辅助库
 * 摘要: 注意该函数应该在引擎底层Layer2D实例对象创建完毕后才能调用
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了对纹理过滤类型信息的得到
 */
void GameGraphics2D::Create()
{
	render::Interface		   * pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D	= pInterface->GetLayer2D();
	m_dwFontTextureHandle					= pInterface->CreateTexture("font/font.tga");
//	m_dwFontTextureHandle[1] = pInterface->CreateTexture("font/font1.tga");
	
	render::Texture *pTexture = pInterface->GetTexture(m_dwFontTextureHandle);
	m_dwFontTextureWidth  = pTexture->GetWidth();
	m_dwFontTextureHeight = pTexture->GetHeight();
	m_dwFontWidth = m_dwFontTextureWidth / 4;
	m_dwFontHeight= m_dwFontTextureHeight / 4;

	/*pTexture = pInterface->GetTexture(m_dwFontTextureHandle[1]);
	m_dwFontTextureWidth[1]  = pTexture->GetWidth();
	m_dwFontTextureHeight[1] = pTexture->GetHeight();.................................
	m_dwFontWidth[1] = m_dwFontTextureWidth[1] / 4;
	m_dwFontHeight[1]= m_dwFontTextureHeight[1] / 4;*/

	m_eTextureFilterType = pLayer2D->GetFilterType();
}



void GameGraphics2D::Destroy(void)
{
    render::Interface *pInterface = render::Interface::GetInstance();
    pInterface->DestroyTexture(m_dwFontTextureHandle);
    //pInterface->DestroyTexture(m_dwFontTextureHandle[1]);
}



/*
 * 功能: 绘制图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImage(int iX,int iY,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
    render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+fWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+fHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

}



void GameGraphics2D::DrawImageBlend(int iX,int iY,DWORD dwImageHandle,D3DCOLOR d3dColor,float fScale)
{
    render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+fWidth * fScale);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+fHeight * fScale);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}



/*
 * 功能: 绘制图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iWidth		   - 绘制宽度
 *		 iHeight	   - 绘制高度
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImage(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



void GameGraphics2D::DrawImageBlend(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}



/*
 * 功能: 绘制反转图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iWidth		   - 绘制宽度
 *		 iHeight	   - 绘制高度
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageFlip(int iX,int iY,int iWidth,int iHeight,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,-1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,-1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



/*
 * 功能: 绘制图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iWidth		   - 绘制宽度
 *		 iHeight	   - 绘制高度
 *		 fUOffset	   - 纹理U坐标偏移
 *		 fVOffset	   - 纹理V坐标偏移
 *		 fScale		   - 图象缩放
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImage(int iX,int iY,int iWidth,int iHeight,float fUOffset,float fVOffset,float fScale,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f + fUOffset,0.0f + fVOffset) * fScale;	
	pVertices[1].uv = D3DXVECTOR2(1.0f + fUOffset,0.0f + fVOffset) * fScale;	
	pVertices[2].uv = D3DXVECTOR2(0.0f + fUOffset,1.0f + fVOffset) * fScale;	
	pVertices[3].uv = D3DXVECTOR2(1.0f + fUOffset,1.0f + fVOffset) * fScale;

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



/*
 * 功能: 绘制图象子区域
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 pRect		   - 绘制子区域矩形
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageSubRect(int iX,int iY,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();	
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();

	float fDrawWidth =(float)(pRect->right - pRect->left) / fWidth  * fWidth;
	float fDrawHeight=(float)(pRect->bottom- pRect->top)  / fHeight * fHeight;

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+fDrawWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+fDrawHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	//计算UV
	
	float uLeft   = pRect->left / fWidth;
	float uRight  = pRect->right / fWidth;
	float vTop    = pRect->top / fHeight;
	float vBottom = pRect->bottom / fHeight;

	pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
	pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
	pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
	pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



/*
 * 功能: 绘制图象子区域
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 pRect		   - 绘制子区域矩形
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.8.18
 * 修改日志:
 */
void GameGraphics2D::DrawImageSubRectEx(int iX, int iY, const RECT * pRect, DWORD dwImageHandle, D3DCOLOR color)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D * pLayer2D	= pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9			 pDevice	= pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9		 pVB		= pLayer2D->GetVertexBuffer();	
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	D3DVIEWPORT9 old;
	D3DVIEWPORT9 New;
	if (iX > 0)
	{
		New.X	  = iX;
		New.Width = pRect->right - pRect->left;
	}else
	{
		New.X	  = 0;
		New.Width = pRect->right - pRect->left + iX;

		// 如果宽度为负数,则直接返回
		if ((int)New.Width <= 0)
			return;
	}
	if (iY > 0)
	{
		New.Y	   = iY;
		New.Height = pRect->bottom - pRect->top;
	}else
	{
		New.Y	   = 0;
		New.Height = pRect->bottom - pRect->top + iY;

		// 如果高度为负数,则直接返回
		if ((int)New.Height <= 0)
			return;
	}

	New.X = (iX < 0) ? 0 : iX;
	New.Y = (iY < 0) ? 0 : iY;
	New.MinZ = 0.0f;
	New.MaxZ = 0.1f;
	pDevice->GetViewport(&old);
	pDevice->SetViewport(&New);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return;
	}

	pInterface->SetTexture(0, dwImageHandle);

	float fX = (float)(iX - pRect->left);
	float fY = (float)(iY - pRect->top);

	pVertices[0].pos.x = pVertices[2].pos.x = fX;
	pVertices[1].pos.x = pVertices[3].pos.x = fX + (float)pTexture->GetWidth();

	pVertices[0].pos.y = pVertices[1].pos.y = fY;
	pVertices[2].pos.y = pVertices[3].pos.y = fY + (float)pTexture->GetHeight();

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	// 计算UV
	pVertices[0].uv = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].uv = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[2].uv = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].uv = D3DXVECTOR2(1.0f, 1.0f);

	pVB->Unlock();

	// Bind stream source
	pDevice->SetStreamSource(0, pVB, 0, sizeof(tagD3DFVF_2D));
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	pDevice->SetViewport(&old);
}



void GameGraphics2D::DrawImageSubRectBlend(int iX,int iY,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();	
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();

	float fDrawWidth =(float)(pRect->right - pRect->left) / fWidth  * fWidth;
	float fDrawHeight=(float)(pRect->bottom- pRect->top)  / fHeight * fHeight;

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+fDrawWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+fDrawHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	//计算UV
	
	float uLeft   = pRect->left / fWidth;
	float uRight  = pRect->right / fWidth;
	float vTop    = pRect->top / fHeight;
	float vBottom = pRect->bottom / fHeight;

	pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
	pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
	pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
	pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}



/*
 * 功能: 绘制图象子区域
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iWidth		   - 绘制宽度
 *		 iHeight	   - 绘制高度
 *		 pRect		   - 纹理子区域矩形
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageSubRect(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();


	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	//计算UV
	
	float uLeft   = pRect->left / fWidth;
	float uRight  = pRect->right / fWidth;
	float vTop    = pRect->top / fHeight;
	float vBottom = pRect->bottom / fHeight;

	pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
	pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
	pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
	pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



void GameGraphics2D::DrawImageSubRectBlend(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices,D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();


	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	//计算UV
	
	float uLeft   = pRect->left / fWidth;
	float uRight  = pRect->right / fWidth;
	float vTop    = pRect->top / fHeight;
	float vBottom = pRect->bottom / fHeight;

	pVertices[0].uv = D3DXVECTOR2(uLeft,vTop);		
	pVertices[2].uv = D3DXVECTOR2(uLeft,vBottom);		
	pVertices[1].uv = D3DXVECTOR2(uRight,vTop);		
	pVertices[3].uv = D3DXVECTOR2(uRight,vBottom);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}

void GameGraphics2D::DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

	

	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	pInterface->SetTexture(0,NULL);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pVertices[0].pos.x = (float)iStartX;
	pVertices[0].pos.y = (float)iStartY;
	pVertices[0].pos.z = 
	pVertices[0].rhw = 1.0f;
	pVertices[0].color = color;

	pVertices[1].pos.z = 
	pVertices[1].rhw = 1.0f;
	pVertices[1].pos.x = (float)iEndX;
	pVertices[1].pos.y = (float)iEndY;
	pVertices[1].color = color;

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_LINELIST, 0, 1);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
}

void GameGraphics2D::DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();



	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	pInterface->SetTexture(0,NULL);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pVertices[0].pos.x = pVertices[2].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(iX+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(iY+iHeight);

	pVertices[0].color = 
	pVertices[1].color = 
	pVertices[2].color = 
	pVertices[3].color = color;

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
}

DWORD GameGraphics2D::CreateImage(const char *szFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	return pInterface->CreateTexture(szFileName);
}

DWORD GameGraphics2D::CreateImage(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	return pInterface->CreateTexture(dwWidth,dwHeight,pBitmap);
}

void  GameGraphics2D::DestroyImage(DWORD dwHandle)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->DestroyTexture(dwHandle);
}



/*
 * 功能: 绘制旋转图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iRX		   -
 *		 iRY		   -
 *		 fAngle		   - 旋转弧度
 *		 fScale		   - 缩放比例
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageRotation2(int iX,int iY,int iRX,int iRY,float fAngle,float fScale,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();


	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	int iWidth = pTexture->GetWidth();
	int iHeight= pTexture->GetHeight();

	static D3DXMATRIX matWorld,matTrans,matRot,matScale;
	D3DXMatrixScaling(&matScale,fScale,fScale,fScale);
	D3DXMatrixTranslation(&matTrans,(float)iRX,(float)iRY,0);
	D3DXMatrixRotationZ(&matRot,fAngle);
	D3DXMatrixMultiply(&matWorld,&matTrans,&matRot);
	D3DXMatrixMultiply(&matWorld,&matWorld,&matScale);
	D3DXMatrixTranslation(&matTrans,(float)iX,(float)iY,0);
	D3DXMatrixMultiply(&matWorld,&matWorld,&matTrans);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = (float)0;
	pVertices[1].pos.x = pVertices[3].pos.x = (float)(0+iWidth);

	pVertices[0].pos.y = pVertices[1].pos.y = (float)0;
	pVertices[2].pos.y = pVertices[3].pos.y = (float)(0+iHeight);

	for(int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&pVertices[i].pos,&pVertices[i].pos,&matWorld);
	}

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}



/*
 * 功能: 绘制图象(纹理偏移)
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 iWidth		   - 绘制宽度
 *		 iHeight	   - 绘制高度
 *		 fUOffset	   - 纹理U坐标偏移
 *		 fVOffset	   - 纹理V坐标偏移
 *		 fScale		   - 缩放比例
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageSkew(int iX,int iY,int iWidth,int iHeight,float fUOffset,float fVOffset,float fScale,DWORD dwImageHandle,D3DCOLOR d3dColor)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}
	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = (float)iX;
	pVertices[1].pos.x = pVertices[2].pos.x = (float)(iX+iWidth);
	
	pVertices[0].pos.y = pVertices[1].pos.y = (float)iY;
	pVertices[2].pos.y = (float)(iY+iHeight);

	pVertices[3].pos.x = pVertices[4].pos.x = (float)iX;
	pVertices[5].pos.x = (float)(iX+iWidth);

	pVertices[3].pos.y = (float)iY;
	pVertices[4].pos.y = pVertices[5].pos.y = (float)(iY+iHeight);

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 
	pVertices[4].pos.z = 
	pVertices[4].rhw = 
	pVertices[5].pos.z = 
	pVertices[5].rhw = 1.0f;


	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color = 
	pVertices[3].color =
	pVertices[4].color =
	pVertices[5].color = d3dColor;

	pVertices[0].uv = D3DXVECTOR2(0.0f + fUOffset,0.0f + fVOffset) * fScale;	
	pVertices[1].uv = D3DXVECTOR2(1.0f + fUOffset,0.0f + fVOffset) * fScale;	
	pVertices[2].uv = D3DXVECTOR2(1.0f + fUOffset,1.0f + fVOffset) * fScale;	

	pVertices[3].uv = D3DXVECTOR2(0.0f + fUOffset,0.0f + fVOffset) * fScale;	
	pVertices[4].uv = D3DXVECTOR2(0.0f + fUOffset,0.5f + fVOffset) * fScale;	
	pVertices[5].uv = D3DXVECTOR2(1.0f + fUOffset,1.0f + fVOffset) * fScale;	

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);
}



/*
 * 功能: 绘制旋转图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 fAngle		   - 旋转弧度
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.05.26 - lpf
 *		增加了根据当前辅助库纹理过滤状态来绘制图象的功能
 */
void GameGraphics2D::DrawImageRotation(int iX,int iY,float fAngle,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	float fHalfWidth = (float)pTexture->GetWidth()  * 0.5f;
	float fHalfHeight= (float)pTexture->GetHeight() * 0.5f;

	static D3DXMATRIX matWorld,matTrans,matRot;
	D3DXMatrixTranslation(&matTrans,(float)iX,(float)iY,0);
	D3DXMatrixRotationZ(&matRot,fAngle);
	D3DXMatrixMultiply(&matWorld,&matRot,&matTrans);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = - fHalfWidth;
	pVertices[1].pos.x = pVertices[3].pos.x =   fHalfWidth;

	pVertices[0].pos.y = pVertices[1].pos.y = - fHalfHeight;
	pVertices[2].pos.y = pVertices[3].pos.y =   fHalfHeight;

	for(int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&pVertices[i].pos,&pVertices[i].pos,&matWorld);
	}

	pVertices[0].pos.z = 
	pVertices[0].rhw = 
	pVertices[1].pos.z = 
	pVertices[1].rhw = 
	pVertices[2].pos.z = 
	pVertices[2].rhw = 
	pVertices[3].pos.z = 
	pVertices[3].rhw = 1.0f;

	pVertices[0].color =
	pVertices[1].color =
	pVertices[2].color =
	pVertices[3].color = color;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}
/*
 * 功能: 绘制旋转图象
 * 摘要: -
 * 参数: iX			   - 绘制X坐标
 *		 iY			   - 绘制Y坐标
 *		 fAngle		   - 旋转弧度
 *		 dwImageHandle - 图象句柄
 *		 dwColor	   - 绘制图象时的颜色叠加
 * 返回值: -
 * 作者: kaba kuailekaba@gmail.com
 * 创建日期: 2010-11-15
 */
void GameGraphics2D::DrawImageRotation(float fX,float fY,float fAngle,DWORD dwImageHandle,D3DCOLOR color)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

	render::Texture * pTexture = pInterface->GetTexture(dwImageHandle);
	float fHalfWidth = pTexture->GetWidth()  * 0.5f;
	float fHalfHeight= pTexture->GetHeight() * 0.5f;

	static D3DXMATRIX matWorld,matTrans,matRot;
	D3DXMatrixTranslation(&matTrans,fX,fY,0);
	D3DXMatrixRotationZ(&matRot,fAngle);
	D3DXMatrixMultiply(&matWorld,&matRot,&matTrans);

	tagD3DFVF_2D * pVertices;	
	HRESULT hResult = 0;
	if(FAILED( hResult = pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD)))
	{
		OutputConsole(" GameGraphics2D:: DrawTexture Lock VB2D failed.\n");
		pVB->Unlock();
		return ;
	}

	pInterface->SetTexture(0,dwImageHandle);

	pVertices[0].pos.x = pVertices[2].pos.x = - fHalfWidth;
	pVertices[1].pos.x = pVertices[3].pos.x =   fHalfWidth;

	pVertices[0].pos.y = pVertices[1].pos.y = - fHalfHeight;
	pVertices[2].pos.y = pVertices[3].pos.y =   fHalfHeight;

	for(int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&pVertices[i].pos,&pVertices[i].pos,&matWorld);
	}

	pVertices[0].pos.z = 
		pVertices[0].rhw = 
		pVertices[1].pos.z = 
		pVertices[1].rhw = 
		pVertices[2].pos.z = 
		pVertices[2].rhw = 
		pVertices[3].pos.z = 
		pVertices[3].rhw = 1.0f;

	pVertices[0].color =
		pVertices[1].color =
		pVertices[2].color =
		pVertices[3].color = color;

	pVertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pVertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pVertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pVertices[3].uv = D3DXVECTOR2(1.0f,1.0f);

	pVB->Unlock();

	//bind stream source
	pDevice->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);    
	pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
}

/*
 * 功能: 根据字体信息来输出文本
 * 摘要: -
 * 参数: iX		   - 绘制文本左上角X坐标
 *		 iY		   - 绘制文本左上角Y坐标
 *		 pszString - 绘制文本内容
 *		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.11.28
 * 修改日志:
 *	2008.04.18 - lpf
 *		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
 */
void GameGraphics2D::DrawText(int iX, int iY, const char * pszString, PFONTINFO pFontInfo)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();


	


	pLayer2D->DrawText(iX, iY, pszString, pFontInfo);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);

}



/*
 * 功能: 根据字体信息来输出指定长度的文本
 * 摘要: -
 * 参数: iX		   - 绘制文本左上角X坐标
 *		 iY		   - 绘制文本左上角Y坐标
 *		 pszString - 绘制文本内容
 *		 iLen	   - 绘制字符长度
 *		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.03
 * 修改日志:
 *	2008.04.18 - lpf
 *		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
 */
void GameGraphics2D::DrawText(int iX, int iY, const char * pszString, int iLen, PFONTINFO pFontInfo)
{

	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();




	pLayer2D->DrawText(iX, iY, pszString, iLen, pFontInfo);
	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);


}



void GameGraphics2D::OutPutText(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color, int iStyleIndex,float fScale)
{
	if (!pszString)
		return;
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();



	// modify by lpf 2007-11-27
	pLayer2D->OutPutText(iX, iY, pszString, iLen, color, iStyleIndex, fScale);
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);


}

DWORD GameGraphics2D::GetFontWidth(int iStyleIndex)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	return pLayer2D->GetFontSize(iStyleIndex) / 2;
}
void GameGraphics2D::SetUIModelRendState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

	pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

	// FILL Solid Mode
	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
	pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, GetInstance()->m_eTextureFilterType);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, GetInstance()->m_eTextureFilterType);

	// Setup Alpha 
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//Setup Color OP
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//Setup Color OP
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}
void GameGraphics2D::RecoverFogState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
}
void GameGraphics2D::DrawGameModel(int iX,int iY,GameModel * pGameModel,float fScale,float fRotation,GameModel::AnimInfo *pAnimInfo)
{
	
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	float fWidth = (float)pInterface->GetWndWidth();
	float fHeight= (float)pInterface->GetWndHeight();	

	float fDrawX = iX - fWidth / 2;
	float fDrawY = fHeight / 2 - iY;
	static D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );

	static D3DXMATRIX matProj;
	D3DXMatrixOrthoLH( &matProj,fWidth, fHeight, -10000.0f, 10000.0f );

	pInterface->GetDevice()->SetTransform( D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matWorld,0,0,10000.0f); 
    pInterface->GetDevice()->SetTransform( D3DTS_VIEW, &matWorld );
    pInterface->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );

	static D3DXMATRIX matBillboard;
	D3DXMatrixInverse( &matBillboard, NULL, &matWorld );
	matBillboard._41 = matBillboard._42 = matBillboard._43 = 0.0f;

	static D3DXMATRIX matModelWorld,matSCL,matROT;
	
	D3DXMatrixRotationY( &matROT,fRotation);
	D3DXMatrixScaling(&matSCL,fScale,fScale,fScale);

	D3DXMatrixTranslation(&matModelWorld,(float)fDrawX,(float)fDrawY,-50.0f);
	D3DXMatrixMultiply(&matModelWorld,&matROT,&matModelWorld);
	D3DXMatrixMultiply(&matModelWorld,&matSCL,&matModelWorld);
	
	pAnimInfo->SetWorldMatrix(&matModelWorld);
	pAnimInfo->SetCurrentTime(timeGetTime());
	pAnimInfo->SetBillboard(&matBillboard);
	pAnimInfo->SetBillboardY(&matBillboard);
	pGameModel->ProcessAnimJoint(pAnimInfo);
	pGameModel->ProcessAnimTexture(pAnimInfo);
	pGameModel->ProcessEnvironmentMaps(&matWorld);
	pGameModel->ProcessAnimUV(pAnimInfo);
	//pGameModel->SetModelColor(0xFF808080);
	pGameModel->ProcessAnimColor(pAnimInfo);
	//D3DXVECTOR3 vDir(1.0f,1.0f,1.0f);
	//D3DXVec3Normalize(&vDir,&vDir);
	//pGameModel->ProcessDirectionalLight(&D3DXVECTOR3(0,0,-1000.0f),&vDir,0xFFFFFFFF);
	//pGameModel->MultiplyModelColor(0xFF808080);
	
	pGameModel->DrawModel();


	
}

void GameGraphics2D::DrawGameModel(int iX,int iY,GameModel * pGameModel,float fScale,float fRotation,GameModel::AnimInfo *pAnimInfo,D3DXMATRIX &matmodelworld,D3DXMATRIX &matworld)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return;
	}
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

	DWORD dwFogEnable;
	pDevice->GetRenderState(D3DRS_FOGENABLE,&dwFogEnable);
	pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

	// FILL Solid Mode
	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
	pDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0);
	pDevice->SetRenderState( D3DRS_ZENABLE,TRUE);
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE,TRUE);

	// Setup sampler Texture Filter
	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	float fWidth = (float)pInterface->GetWndWidth();
	float fHeight= (float)pInterface->GetWndHeight();	

	float fDrawX = iX - fWidth / 2;
	float fDrawY = fHeight / 2 - iY;
	static D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	static D3DXMATRIX matProj,matBillboard;
	D3DXMatrixOrthoLH( &matProj,fWidth, fHeight, -10000.0f, 10500.0f );

	pInterface->GetDevice()->SetTransform( D3DTS_WORLD, &matWorld);
	D3DXMatrixTranslation(&matWorld,0,0,10000.0f); 
	pInterface->GetDevice()->SetTransform( D3DTS_VIEW, &matWorld );
	pInterface->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );

	//static D3DXMATRIX matBillboard;
	//D3DXMatrixIdentity(&matBillboard);
	
	
	D3DXMatrixInverse( &matBillboard, NULL, &matWorld );
	matBillboard._41 = matBillboard._42 = matBillboard._43 = 0.0f;

	static D3DXMATRIX matModelWorld,matSCL,matROT;

	D3DXMatrixRotationY( &matROT,fRotation);
	D3DXMatrixScaling(&matSCL,fScale,fScale,fScale);

	D3DXMatrixTranslation(&matModelWorld,(float)fDrawX,(float)fDrawY,-50.0f);
	D3DXMatrixMultiply(&matModelWorld,&matROT,&matModelWorld);
	D3DXMatrixMultiply(&matModelWorld,&matSCL,&matModelWorld);
	
	matmodelworld = matModelWorld;
	matworld = matWorld;

	pAnimInfo->SetWorldMatrix(&matModelWorld);
	pAnimInfo->SetCurrentTime(timeGetTime());
	pAnimInfo->SetBillboard(&matBillboard);

	pAnimInfo->SetBillboardY(&matBillboard);
	pGameModel->ProcessAnimJoint(pAnimInfo);
	pGameModel->ProcessAnimTexture(pAnimInfo);
	pGameModel->ProcessEnvironmentMaps(&matWorld);
	pGameModel->ProcessAnimUV(pAnimInfo);
	//pGameModel->SetModelColor(0xFF808080);
	pGameModel->ProcessAnimColor(pAnimInfo);
	//D3DXVECTOR3 vDir(1.0f,1.0f,1.0f);
	//D3DXVec3Normalize(&vDir,&vDir);
	//pGameModel->ProcessDirectionalLight(&D3DXVECTOR3(0,0,-1000.0f),&vDir,0xFFFFFFFF);
	//pGameModel->MultiplyModelColor(0xFF808080);
	
	
	pGameModel->DrawModel();
	


	
}

void GameGraphics2D::NumberOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	if (!pszString||pInterface->GetDeviceLost())
	{
        return ;
	}
	static const DWORD szPositionTable[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 
		0, 0, 0, 0, 12, 10,  0, 11, 15, 13, 0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 0 };
	

	pInterface->GetDevice()->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   

	float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

	float w,h;

	tagD3DFVF_2D * pVertices = NULL;
	UINT wNumTriangles = 0;
	pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	int iTexLen = lstrlen( pszString ) ;

	if (iLen > iTexLen) iLen = iTexLen;

	if (iLen > MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE;

	WORD  wChar(0);
	DWORD dwColor(0);
	int n = 0;
	while(n<iLen)
	{
		unsigned char c =pszString[n++];		

		if ( c=='\n' )
		{
			sy+= m_dwFontHeight;
			sx=(float)iX;
			continue;
		}

		{
			h = (float)m_dwFontHeight * fScale;
			w = (float)m_dwFontWidth * fScale;
			offset = (float)m_dwFontWidth * fScale;
		    int iPos = szPositionTable[c];
			int a = ( iPos % 4 ) * m_dwFontWidth;
			int b = iPos / 4 * m_dwFontHeight;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / m_dwFontTextureWidth;
			ty0 = (float)(b) / m_dwFontTextureHeight;

			//// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)(m_dwFontWidth) / (float)m_dwFontTextureWidth;
			ty1 = ty0 + (float)(m_dwFontHeight) / (float)m_dwFontTextureHeight;
		}
		
		// 填充顶点缓冲区

		pVertices[0].color = 
		pVertices[1].color = 
		pVertices[2].color = 
		pVertices[3].color = 
		pVertices[4].color = 
		pVertices[5].color = color;
		
		pVertices[0].pos.x = sx;
		pVertices[1].pos.x = sx;
		pVertices[2].pos.x = sx + w;

		pVertices[0].pos.y = sy;
		pVertices[1].pos.y = sy + h;
		pVertices[2].pos.y = sy + h;

		pVertices[3].pos.x = sx + w;
		pVertices[4].pos.x = sx + w; 
		pVertices[5].pos.x = sx;

		pVertices[3].pos.y = sy + h;
		pVertices[4].pos.y = sy;
		pVertices[5].pos.y = sy;

		pVertices[0].uv.x = tx0;
		pVertices[1].uv.x = tx0;
		pVertices[2].uv.x = tx1;

		pVertices[0].uv.y = ty0;
		pVertices[1].uv.y = ty1;
		pVertices[2].uv.y = ty1;

		pVertices[3].uv.x = tx1;
		pVertices[4].uv.x = tx1;
		pVertices[5].uv.x = tx0;

		pVertices[3].uv.y = ty1;
		pVertices[4].uv.y = ty0;
		pVertices[5].uv.y = ty0;

		pVertices[0].pos.z = 
		pVertices[1].pos.z = 
		pVertices[2].pos.z = 
		pVertices[3].pos.z = 
		pVertices[4].pos.z = 
		pVertices[5].pos.z = 0.5f;

		pVertices[0].rhw = 
		pVertices[1].rhw = 
		pVertices[2].rhw = 
		pVertices[3].rhw = 
		pVertices[4].rhw = 
		pVertices[5].rhw = 1.0f;	

		pVertices += 6;

		wNumTriangles+=2;

		sx+=offset;	// 坐标x增量
	}
	pVB->Unlock();
	if (wNumTriangles == 0)
	{
		return;
	}

	pInterface->SetTexture(0,m_dwFontTextureHandle);
	pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles );

}

void GameGraphics2D::NumberOutBlend(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (!pszString||pInterface->GetDeviceLost())
	{
		return ;
	}
	static const DWORD szPositionTable[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 
		0, 0, 0, 0, 12, 10,  0, 11, 15, 13, 0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 0 };
	
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();
	

	pInterface->GetDevice()->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

	float w,h;

	tagD3DFVF_2D * pVertices = NULL;
	UINT wNumTriangles = 0;
	pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	int iTexLen = lstrlen( pszString ) ;

	if (iLen > iTexLen) iLen = iTexLen;

	if (iLen > MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE;

	WORD  wChar(0);
	DWORD dwColor(0);
	int n = 0;
	while(n<iLen)
	{
		unsigned char c =pszString[n++];		

		if ( c=='\n' )
		{
			sy+= m_dwFontHeight;
			sx=(float)iX;
			continue;
		}

		{
			h = (float)m_dwFontHeight * fScale;
			w = (float)m_dwFontWidth * fScale;
			offset = (float)m_dwFontWidth * fScale;
		    int iPos = szPositionTable[c];
			int a = ( iPos % 4 ) * m_dwFontWidth;
			int b = iPos / 4 * m_dwFontHeight;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / m_dwFontTextureWidth;
			ty0 = (float)(b) / m_dwFontTextureHeight;

			//// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)(m_dwFontWidth) / (float)m_dwFontTextureWidth;
			ty1 = ty0 + (float)(m_dwFontHeight) / (float)m_dwFontTextureHeight;
		}
		
		// 填充顶点缓冲区

		pVertices[0].color = 
		pVertices[1].color = 
		pVertices[2].color = 
		pVertices[3].color = 
		pVertices[4].color = 
		pVertices[5].color = color;
		
		pVertices[0].pos.x = sx;
		pVertices[1].pos.x = sx;
		pVertices[2].pos.x = sx + w;

		pVertices[0].pos.y = sy;
		pVertices[1].pos.y = sy + h;
		pVertices[2].pos.y = sy + h;

		pVertices[3].pos.x = sx + w;
		pVertices[4].pos.x = sx + w; 
		pVertices[5].pos.x = sx;

		pVertices[3].pos.y = sy + h;
		pVertices[4].pos.y = sy;
		pVertices[5].pos.y = sy;

		pVertices[0].uv.x = tx0;
		pVertices[1].uv.x = tx0;
		pVertices[2].uv.x = tx1;

		pVertices[0].uv.y = ty0;
		pVertices[1].uv.y = ty1;
		pVertices[2].uv.y = ty1;

		pVertices[3].uv.x = tx1;
		pVertices[4].uv.x = tx1;
		pVertices[5].uv.x = tx0;

		pVertices[3].uv.y = ty1;
		pVertices[4].uv.y = ty0;
		pVertices[5].uv.y = ty0;

		pVertices[0].pos.z = 
		pVertices[1].pos.z = 
		pVertices[2].pos.z = 
		pVertices[3].pos.z = 
		pVertices[4].pos.z = 
		pVertices[5].pos.z = 0.5f;

		pVertices[0].rhw = 
		pVertices[1].rhw = 
		pVertices[2].rhw = 
		pVertices[3].rhw = 
		pVertices[4].rhw = 
		pVertices[5].rhw = 1.0f;	

		pVertices += 6;

		wNumTriangles+=2;

		sx+=offset;	// 坐标x增量
	}
	pVB->Unlock();
	if (wNumTriangles == 0)
	{
		return;
	}

	pInterface->SetTexture(0,m_dwFontTextureHandle);
	pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles );
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

}

void GameGraphics2D::NumberOut1(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale)
{
	static const DWORD szPositionTable[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 
		0, 0, 0, 0, 12, 10,  0, 11, 15, 13, 0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 0 };
	render::Interface *pInterface = render::Interface::GetInstance();
	if (pInterface->GetDeviceLost())
	{
		return ;
	}
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();


	pInterface->GetDevice()->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   

	float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

	float w,h;

	tagD3DFVF_2D * pVertices = NULL;
	UINT wNumTriangles = 0;
	pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	int iTexLen = lstrlen( pszString ) ;

	if (iLen > iTexLen) iLen = iTexLen;

	if (iLen > MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE;

	WORD  wChar(0);
	DWORD dwColor(0);
	int n = 0;
	while(n<iLen)
	{
		unsigned char c =pszString[n++];		

		if ( c=='\n' )
		{
			sy+= m_dwFontHeight;
			sx=(float)iX;
			continue;
		}

		{
			h = (float)m_dwFontHeight * fScale;
			w = (float)m_dwFontWidth * fScale;
			offset = (float)m_dwFontWidth * fScale;
		    int iPos = szPositionTable[c];
			int a = ( iPos % 4 ) * m_dwFontWidth;
			int b = iPos / 4 * m_dwFontHeight;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / m_dwFontTextureWidth;
			ty0 = (float)(b) / m_dwFontTextureHeight;

			//// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)(m_dwFontWidth) / (float)m_dwFontTextureWidth;
			ty1 = ty0 + (float)(m_dwFontHeight) / (float)m_dwFontTextureHeight;
		}
		
		// 填充顶点缓冲区

		pVertices[0].color = 
		pVertices[1].color = 
		pVertices[2].color = 
		pVertices[3].color = 
		pVertices[4].color = 
		pVertices[5].color = color;
		
		pVertices[0].pos.x = sx;
		pVertices[1].pos.x = sx;
		pVertices[2].pos.x = sx + w;

		pVertices[0].pos.y = sy;
		pVertices[1].pos.y = sy + h;
		pVertices[2].pos.y = sy + h;

		pVertices[3].pos.x = sx + w;
		pVertices[4].pos.x = sx + w; 
		pVertices[5].pos.x = sx;

		pVertices[3].pos.y = sy + h;
		pVertices[4].pos.y = sy;
		pVertices[5].pos.y = sy;

		pVertices[0].uv.x = tx0;
		pVertices[1].uv.x = tx0;
		pVertices[2].uv.x = tx1;

		pVertices[0].uv.y = ty0;
		pVertices[1].uv.y = ty1;
		pVertices[2].uv.y = ty1;

		pVertices[3].uv.x = tx1;
		pVertices[4].uv.x = tx1;
		pVertices[5].uv.x = tx0;

		pVertices[3].uv.y = ty1;
		pVertices[4].uv.y = ty0;
		pVertices[5].uv.y = ty0;

		pVertices[0].pos.z = 
		pVertices[1].pos.z = 
		pVertices[2].pos.z = 
		pVertices[3].pos.z = 
		pVertices[4].pos.z = 
		pVertices[5].pos.z = 0.5f;

		pVertices[0].rhw = 
		pVertices[1].rhw = 
		pVertices[2].rhw = 
		pVertices[3].rhw = 
		pVertices[4].rhw = 
		pVertices[5].rhw = 1.0f;	

		pVertices += 6;

		wNumTriangles+=2;

		sx+=offset;	// 坐标x增量
	}
	pVB->Unlock();
	if (wNumTriangles == 0)
	{
		return;
	}
	
	pInterface->SetTexture(0,m_dwFontTextureHandle);
	pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles );
}

void GameGraphics2D::NumberOutFilter(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (!pszString||pInterface->GetDeviceLost())
	{
		return ;
	}
	static const DWORD szPositionTable[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 
		0, 0, 0, 0, 12, 10,  0, 11, 15, 13, 0, 1, 2, 3, 4, 5, 6, 7, 8,
		9, 0 };
	
	render::Interface::Layer2D *pLayer2D   = pInterface->GetLayer2D();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();
	LPDIRECT3DVERTEXBUFFER9 pVB   = pLayer2D->GetVertexBuffer();

	pInterface->GetDevice()->SetStreamSource( 0, pVB,0, sizeof(tagD3DFVF_2D) );
	pInterface->GetDevice()->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);   

	float sx = (float)iX, sy = (float)iY,offset=0, tx0=0, ty0=0, tx1=0, ty1=0;

	float w,h;

	tagD3DFVF_2D * pVertices = NULL;
	UINT wNumTriangles = 0;
	pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD);

	int iTexLen = lstrlen( pszString ) ;

	if (iLen > iTexLen) iLen = iTexLen;

	if (iLen > MAX_RENDER_LAYER2D_VB_SIZE) iLen = MAX_RENDER_LAYER2D_VB_SIZE;

	WORD  wChar(0);
	DWORD dwColor(0);
	int n = 0;
	while(n<iLen)
	{
		unsigned char c =pszString[n++];		

		if ( c=='\n' )
		{
			sy+= m_dwFontHeight;
			sx=(float)iX;
			continue;
		}

		{
			h = (float)m_dwFontHeight * fScale;
			w = (float)m_dwFontWidth * fScale;
			offset = (float)m_dwFontWidth * fScale;
		    int iPos = szPositionTable[c];
			int a = ( iPos % 4 ) * m_dwFontWidth;
			int b = iPos / 4 * m_dwFontHeight;
			// 计算纹理左上角 0.0-1.0
			tx0 = (float)(a) / m_dwFontTextureWidth;
			ty0 = (float)(b) / m_dwFontTextureHeight;

			//// 计算纹理右下角 0.0-1.0
			tx1 = tx0 + (float)(m_dwFontWidth) / (float)m_dwFontTextureWidth;
			ty1 = ty0 + (float)(m_dwFontHeight) / (float)m_dwFontTextureHeight;
		}
		
		// 填充顶点缓冲区

		pVertices[0].color = 
		pVertices[1].color = 
		pVertices[2].color = 
		pVertices[3].color = 
		pVertices[4].color = 
		pVertices[5].color = color;
		
		pVertices[0].pos.x = sx;
		pVertices[1].pos.x = sx;
		pVertices[2].pos.x = sx + w;

		pVertices[0].pos.y = sy;
		pVertices[1].pos.y = sy + h;
		pVertices[2].pos.y = sy + h;

		pVertices[3].pos.x = sx + w;
		pVertices[4].pos.x = sx + w; 
		pVertices[5].pos.x = sx;

		pVertices[3].pos.y = sy + h;
		pVertices[4].pos.y = sy;
		pVertices[5].pos.y = sy;

		pVertices[0].uv.x = tx0;
		pVertices[1].uv.x = tx0;
		pVertices[2].uv.x = tx1;

		pVertices[0].uv.y = ty0;
		pVertices[1].uv.y = ty1;
		pVertices[2].uv.y = ty1;

		pVertices[3].uv.x = tx1;
		pVertices[4].uv.x = tx1;
		pVertices[5].uv.x = tx0;

		pVertices[3].uv.y = ty1;
		pVertices[4].uv.y = ty0;
		pVertices[5].uv.y = ty0;

		pVertices[0].pos.z = 
		pVertices[1].pos.z = 
		pVertices[2].pos.z = 
		pVertices[3].pos.z = 
		pVertices[4].pos.z = 
		pVertices[5].pos.z = 0.5f;

		pVertices[0].rhw = 
		pVertices[1].rhw = 
		pVertices[2].rhw = 
		pVertices[3].rhw = 
		pVertices[4].rhw = 
		pVertices[5].rhw = 1.0f;	

		pVertices += 6;

		wNumTriangles+=2;

		sx+=offset;	// 坐标x增量
	}
	pVB->Unlock();
	if (wNumTriangles == 0)
	{
		return;
	}

	pInterface->SetTexture(0,m_dwFontTextureHandle);
	pInterface->GetDevice()->DrawPrimitive( D3DPT_TRIANGLELIST, 0, wNumTriangles );

}