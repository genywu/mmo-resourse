#pragma once                    //┃    //┃
#ifndef __MS2DTEXTUREMANAGER_H__//┃    //┃
#define __MS2DTEXTUREMANAGER_H__//┃    //┃
//━━━━━━━━━━━━━━━━┛    //┃
//文件名称：Ms2DTextureManager.h        //┃
//功能描述：2D引擎纹理管理器管理        //┃
//平台相关：Windows                     //┃
//━━━━━━━━━━━━━━━━━━━━┛
//纹理管理器

// 自定义顶点类型
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// 描述矩形顶点个数
#define RECTANGLE_VERTEX_COUNT  4

// 自定义定点类型
struct CUSTOMVERTEX
{
    FLOAT x, y, z;  // 坐标
    FLOAT rhw;
    DWORD color;    // 颜色
    FLOAT tu, tv;   // 贴图UV
};

class CMs2DTexture
{
public:
    CMs2DTexture(TCHAR* szFileName);
    CMs2DTexture(HMODULE hSrcModule, INT nResId);
    ~CMs2DTexture(VOID);

    // 创建缓冲区
    VOID CreateBuff(VOID);

    // 渲染纹理贴图
    VOID RenderTexture(LPDIRECT3DDEVICE9 lpDirectDevice9);

    // 设置图片位置
    VOID SetRect(RECT& DesRc, RECT& SrcRc);

    // 更新渲染状态
    VOID UpdataRenderState(LPDIRECT3DDEVICE9 lpDirectDevice9);

    // 更新顶点缓冲区
    VOID UpdataVB(VOID);
protected:
    LPDIRECT3DBASETEXTURE9  m_lpTexture;
    D3DXIMAGE_INFO          m_Imageinfo;
    DWORD                   m_dwCubeMapFlags;

    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    LPVOID                  m_lpVBuff;
    CUSTOMVERTEX            m_Vertex[RECTANGLE_VERTEX_COUNT];

    LPDIRECT3DINDEXBUFFER9  m_pIB;
    LPVOID                  m_lpIBuff;
    DWORD                   m_dwIndexed[RECTANGLE_VERTEX_COUNT];
};
typedef CMs2DTexture* LPCMs2DTexture;

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
