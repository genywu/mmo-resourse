#include "Precompiled.h"

CMs2DTexture::CMs2DTexture(TCHAR* szFileName)
: m_lpTexture   (NULL)
, m_pVB         (NULL)
, m_lpVBuff     (NULL)
, m_pIB         (NULL)
, m_lpIBuff     (NULL)
{
    ZeroMemoryStruct(m_Imageinfo);

    HRESULT hRet = S_OK;
    LPDIRECT3DDEVICE9 lpDirectDevice9 = CMs2DEngine::s_GetDirectDevice9();

    // 获取纹理信息
    hRet = ::D3DXGetImageInfoFromFile(szFileName, &m_Imageinfo);
    AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));

    // 判断资源类型
    switch(m_Imageinfo.ResourceType)
    {
        // 纹理
    case D3DRTYPE_TEXTURE:
        {
            hRet = ::D3DXCreateTextureFromFileEx(
                lpDirectDevice9,
                szFileName, 
                m_Imageinfo.Width,
                m_Imageinfo.Height,
                m_Imageinfo.MipLevels,
                0,
                m_Imageinfo.Format,
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE,
                D3DX_FILTER_NONE,
                0, 
                &m_Imageinfo,
                NULL,
                (LPDIRECT3DTEXTURE9*)&m_lpTexture
                );
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
            m_Imageinfo.Depth = 0;
        }break;

    case D3DRTYPE_VOLUMETEXTURE:
        {
            hRet = ::D3DXCreateVolumeTextureFromFileEx(
                lpDirectDevice9,
                szFileName,
                m_Imageinfo.Width,
                m_Imageinfo.Height,
                m_Imageinfo.Depth,
                m_Imageinfo.MipLevels,
                0,
                m_Imageinfo.Format,
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE,
                D3DX_FILTER_NONE,
                0,
                &m_Imageinfo,
                NULL,
                (LPDIRECT3DVOLUMETEXTURE9*)m_lpTexture
                );
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
        }break;

    case D3DRTYPE_CUBETEXTURE:
        {
            hRet = ::D3DXCreateCubeTextureFromFileEx(
                lpDirectDevice9,
                szFileName,
                m_Imageinfo.Width,
                m_Imageinfo.MipLevels,
                0,
                m_Imageinfo.Format,
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE,
                D3DX_FILTER_NONE,
                0,
                &m_Imageinfo,
                NULL,
                (LPDIRECT3DCUBETEXTURE9*)m_lpTexture);
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
            m_Imageinfo.Depth = 0;
            m_dwCubeMapFlags = DDS_CUBEMAP_ALLFACES;
        }break;
    default:
        {
            AssertEx(AL_NORMAL, FALSE, _T("加载纹理失败"));
        }break;
    }
    this->CreateBuff();
}

CMs2DTexture::CMs2DTexture(HMODULE hSrcModule, INT nResId)
: m_lpTexture (NULL)
{
    ZeroMemoryStruct(m_Imageinfo);

    HRESULT hRet = S_OK;
    LPDIRECT3DDEVICE9 lpDirectDevice9 = CMs2DEngine::s_GetDirectDevice9();

    // 获取纹理信息
    hRet = ::D3DXGetImageInfoFromResource(hSrcModule, MAKEINTRESOURCE(nResId), &m_Imageinfo);
    AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));

    // 判断资源类型
    switch(m_Imageinfo.ResourceType)
    {
        // 纹理
    case D3DRTYPE_TEXTURE:
        {
            hRet = ::D3DXCreateTextureFromResourceEx(
                lpDirectDevice9,
                hSrcModule,
                MAKEINTRESOURCE(nResId), 
                m_Imageinfo.Width,
                m_Imageinfo.Height,
                m_Imageinfo.MipLevels,
                0,
                m_Imageinfo.Format,
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE,
                D3DX_FILTER_NONE,
                0, 
                &m_Imageinfo,
                NULL,
                (LPDIRECT3DTEXTURE9*)&m_lpTexture
                );
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
            m_Imageinfo.Depth = 0;
        }break;

    case D3DRTYPE_VOLUMETEXTURE:
        {
            hRet = ::D3DXCreateVolumeTextureFromResourceEx(
                lpDirectDevice9,
                hSrcModule,
                MAKEINTRESOURCE(nResId), 
                m_Imageinfo.Width,
                m_Imageinfo.Height,
                m_Imageinfo.Depth,
                m_Imageinfo.MipLevels,
                0,
                m_Imageinfo.Format,
                D3DPOOL_MANAGED,
                D3DX_FILTER_NONE,
                D3DX_FILTER_NONE,
                0,
                &m_Imageinfo,
                NULL,
                (LPDIRECT3DVOLUMETEXTURE9*)m_lpTexture
                );
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
        }break;

    case D3DRTYPE_CUBETEXTURE:
        {
            hRet = ::D3DXCreateCubeTextureFromResource(
                lpDirectDevice9,
                hSrcModule,
                MAKEINTRESOURCE(nResId), 
                (LPDIRECT3DCUBETEXTURE9*)m_lpTexture);
            AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("加载纹理失败"));
            m_Imageinfo.Depth = 0;
            m_dwCubeMapFlags = DDS_CUBEMAP_ALLFACES;
        }break;
    default:
        {
            AssertEx(AL_NORMAL, FALSE, _T("加载纹理失败"));
        }break;
    }
    this->CreateBuff();
}

CMs2DTexture::~CMs2DTexture(VOID)
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pIB);
    SAFE_RELEASE(m_lpTexture);
}

// 创建缓冲区
VOID CMs2DTexture::CreateBuff(VOID)
{
    HRESULT hRet = S_OK;
    LPDIRECT3DDEVICE9 lpDirectDevice9 = CMs2DEngine::s_GetDirectDevice9();

    hRet = lpDirectDevice9->CreateVertexBuffer(
        sizeof(m_Vertex),
        0,
        D3DFVF_CUSTOMVERTEX, 
        D3DPOOL_MANAGED,
        &m_pVB,
        NULL
        );
    AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("创建纹理顶点失败"));

    hRet = lpDirectDevice9->CreateIndexBuffer(
        sizeof(m_dwIndexed),
        0,
        D3DFMT_INDEX32,
        D3DPOOL_DEFAULT,
        &m_pIB,
        NULL
        );
    AssertEx(AL_NORMAL, SUCCEEDED(hRet), _T("创建顶点索引失败"));
}

// 渲染纹理贴图
VOID CMs2DTexture::RenderTexture(LPDIRECT3DDEVICE9 lpDirectDevice9)
{
    lpDirectDevice9->SetTexture(0, m_lpTexture);
    lpDirectDevice9->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 4, 0, 2);
    lpDirectDevice9->SetTexture(0, NULL);
}

// 设置图片位置
VOID CMs2DTexture::SetRect(RECT& DesRc, RECT& SrcRc)
{
    FLOAT fInceptU  = ((FLOAT)(SrcRc.left) * 1.0f) / (FLOAT)m_Imageinfo.Width;
    FLOAT fInceptV  = ((FLOAT)(SrcRc.top) * 1.0f) / (FLOAT)m_Imageinfo.Height;
    FLOAT fConcludeU= ((FLOAT)(SrcRc.right) * 1.0f) / (FLOAT)m_Imageinfo.Width;
    FLOAT fConcludeV= ((FLOAT)(SrcRc.bottom) * 1.0f) / (FLOAT)m_Imageinfo.Height;

    m_Vertex[0].x = (FLOAT)DesRc.left;
    m_Vertex[0].y = (FLOAT)DesRc.top;
    m_Vertex[0].z = 0.0f;
    m_Vertex[0].rhw = 1.0f;
    m_Vertex[0].color = ARGB(255,255,255,255);
    m_Vertex[0].tu = fInceptU;
    m_Vertex[0].tv = fInceptV;

    m_Vertex[1].x = (FLOAT)DesRc.right;
    m_Vertex[1].y = (FLOAT)DesRc.top;
    m_Vertex[1].z = 0.0f;
    m_Vertex[1].rhw = 1.0f;
    m_Vertex[1].color = ARGB(255,255,255,255);
    m_Vertex[1].tu = fConcludeU;
    m_Vertex[1].tv = fInceptV;

    m_Vertex[2].x = (FLOAT)DesRc.right;
    m_Vertex[2].y = (FLOAT)DesRc.bottom;
    m_Vertex[2].z = 0.0f;
    m_Vertex[2].rhw = 1.0f;
    m_Vertex[2].color = ARGB(255,255,255,255);
    m_Vertex[2].tu = fConcludeU;
    m_Vertex[2].tv = fConcludeV;

    m_Vertex[3].x = 0.0f;
    m_Vertex[3].y = (FLOAT)DesRc.bottom;
    m_Vertex[3].z = 0.0f;
    m_Vertex[3].rhw = 1.0f;
    m_Vertex[3].color = ARGB(255,255,255,255);
    m_Vertex[3].tu = fInceptU;
    m_Vertex[3].tv = fConcludeV;

    m_dwIndexed[0] = 1;
    m_dwIndexed[1] = 2;
    m_dwIndexed[2] = 0;
    m_dwIndexed[3] = 3;
}

// 更新渲染状态
VOID CMs2DTexture::UpdataRenderState(LPDIRECT3DDEVICE9 lpDirectDevice9)
{
    lpDirectDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    lpDirectDevice9->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    lpDirectDevice9->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    lpDirectDevice9->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
    lpDirectDevice9->SetFVF(D3DFVF_CUSTOMVERTEX);
    lpDirectDevice9->SetIndices(m_pIB);
}

// 更新顶点缓冲区
VOID CMs2DTexture::UpdataVB(VOID)
{
    m_pVB->Lock(0, sizeof(m_Vertex), &m_lpVBuff, 0);
    memcpy(m_lpVBuff, &m_Vertex, sizeof(m_Vertex));
    m_pVB->Unlock();

    m_pIB->Lock(0, sizeof(m_dwIndexed), &m_lpIBuff, 0);
    memcpy(m_lpIBuff, &m_dwIndexed, sizeof(m_dwIndexed));
    m_pIB->Unlock();
}
