/*

○○○○○○
○                                      ○
○○○    ○○ ○     ○○    ○○○  ○○○
○  ○      ○○    ○    ○  ○        ○
○          ○      ○    ○     ○     ○
○○○      ○○○       ○○    ○○○    ○○

"FROST" 3D GRAPHICS RENDERING ENGINE.

WRITTEN BY JIATAO.
--------------------------------------------------------------------
COPYRIGHT 2002-2003. 

描述: 渲染支持模块
作者: 贾涛
建立: 
修改: 2003/10/31
*/

#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include "fsingleton.h"

struct tagD3DFVF_2D
{
	D3DXVECTOR3 pos;
	float		rhw;
	D3DCOLOR    color;
	D3DXVECTOR2 uv;
};

enum LAYER2D_ELEMENT_TYPE
{
	LET_LINES = 0,
	LET_TRISTRIP,
	LET_STRING
};


enum LAYER3D_MULTI_STREAM_TYPE 
{
	LMST_POSCOL = 0,
	LMST_POSDIFTEX0,
	LMST_POSDIFSPCTEX0,
	LMST_ENDOFTYPE
};

#define MAX_SHADER_GROUP_ARRAY_SIZE				3072
#define MAX_POLYGON_ARRAY_SIZE					131072 * 4
#define LAYER3D_VERTEXBUFFER_SIZE				(8192 * 3)


#define MAX_TEXTURE_CACHE_SIZE	16384

#define DEFAULT_MAX_TEXTURE_SIZE    512

#define FONT_TEXTURE_SIZE  512


//__CLASS__(render::Layer2D)__DEFINE__
#define MAX_RENDER_LAYER2D_CACHE_SIZE 2048
#define MAX_RENDER_LAYER2D_VB_SIZE    256

class render
{
public:
	class Interface;

	//__CLASS__(render::Quad)___XZ
	class QuadXZ
	{
		D3DXVECTOR3 m_vMin,m_vMax;

		D3DXVECTOR3 m_vConner[4];

	public:
		void	Set(float fxMin , float fxMax , float fzMin , float fzMax);
		inline 	const D3DXVECTOR3 * GetConner() const { return m_vConner;}

		void	Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);

		inline const D3DXVECTOR3 * GetMin() { return &m_vMin;}
		inline const D3DXVECTOR3 * GetMax() { return &m_vMax;}

		QuadXZ(void);
		~QuadXZ(void);
	};

	//__CLASS__(render::BoundingBox)___
	class BoundingBox
	{
		D3DXVECTOR3 m_vCenter;
		float m_fOutsideRadius;

		D3DXVECTOR3 m_vMin,m_vMax;
		D3DXVECTOR3 m_vConner[8];
		D3DXPLANE	m_plane[6];	

	public:
		void operator = (const BoundingBox &BoundingBox);
		void Set(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax);
		void Set(const D3DXVECTOR3 &vCenter,float fRadius);

		BOOL TestPointInside(D3DXVECTOR3 * pvPoint);
		BOOL TestRadialIntersected(D3DXVECTOR3 &vOrigin,D3DXVECTOR3 &vDirection);
		BOOL TestRadialIntersected(const D3DXVECTOR3 *pOrigin,const D3DXVECTOR3 *pDirection,float &fDistance);
		BOOL TestRadialIntersected(const D3DXMATRIX  *pMatWorld,const D3DXVECTOR3 *pOrigin,const D3DXVECTOR3 *pDirection,float &fDistance);


		BOOL TestLineSegmentIntersected(D3DXVECTOR3 vLineSegment[2],D3DXVECTOR3 &vIntersection);
		BOOL TestTriangleIntersected(D3DXVECTOR3 vTriangle[3]);
		BOOL TestSphereIntersected(D3DXVECTOR3 &vCenter,float fRadius);
		BOOL TestBoundingBoxIntersected(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox);
		BOOL TestBoundingBoxIntersectedFast(const D3DXMATRIX *pMatWorld,const render::BoundingBox *pBoundingBox);

		void _DEBUG_Render(const D3DXMATRIX *pMatWorld,DWORD dwColor);

		void Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);
		const D3DXVECTOR3 * GetCenter() const { return &m_vCenter;}
		inline float GetOutsideRadius() const { return m_fOutsideRadius;}
		inline const D3DXVECTOR3 * GetConnerVertex() const { return m_vConner;}
	};
	//__CLASS__(render::Frustum)___
	class Frustum
	{
		D3DXVECTOR3 m_vFrustum[8];
		D3DXPLANE   m_planeFrustum[6];

	public:
		void CalculateFrustum(const D3DXMATRIX *pView,const D3DXMATRIX *pProjection);
		BOOL BoxIntersectFrustum(const BoundingBox * pBoundingBox);
		BOOL BoxIntersectFrustum(const D3DXMATRIX *pWorldMatrix,const render::BoundingBox * pBoundingBox);
		BOOL BoxInFrustum(const BoundingBox * pBoundingBox);
		BOOL PointInFrustum(const D3DXVECTOR3 *pPoint);
		BOOL QuadXZInFrustum(QuadXZ * pQuadXZ);
		BOOL QuadXZIntersectFrustum(QuadXZ * pQuadXZ);

		inline const D3DXVECTOR3 * GetConnerVertex() const { return m_vFrustum;}

		void _DEBUG_Render();
	};	
	//__CLASS_(render::Camera)___
	class Camera  
	{
		//view attribute
		D3DXVECTOR3 m_vecPosition;//Position
		D3DXVECTOR3 m_vecView    ;//View

		D3DXMATRIX  m_matView;
		D3DXMATRIX  m_matBillboard;
		D3DXMATRIX  m_matBillboardY;

		//proj attrribute
		float m_fFOVY;
		float m_fAspect;
		float m_fNearPlane;
		float m_fFarPlane;
		D3DXMATRIX  m_matProjection;
	public:
		void CalculateAxis();
		void MoveView(D3DXVECTOR3 *pVec);
		void MovePosition(D3DXVECTOR3 *pVec);
		void Rotate(D3DXVECTOR3 *pVec);

		float GetNearPlane() const { return m_fNearPlane;}
		float GetFarPlane()  const { return m_fFarPlane;}
		float GetAspect() const { return m_fAspect;}
		float GetFOVY() const { return m_fFOVY;}

		//轴
		D3DXVECTOR3 m_vecXaxis,m_vecYaxis,m_vecZaxis;

		D3DXVECTOR3 * GetPosition() { return &m_vecPosition;}
		D3DXVECTOR3 * GetView()  { return &m_vecView;}

		void RotatePosition(D3DXVECTOR3 *pVec);
		void SetFOVY(float fFOVY);
		void SetAspect(float fAspect);
		void SetNearClipPlane(float fNear);
		void SetFarClipPlane(float fFar);

		void SetPosition(D3DXVECTOR3 *pvecPosition);
		void SetView(const D3DXVECTOR3 *pvecView);

		const D3DXMATRIX * GetProjectionMatrix() ;
		const D3DXMATRIX * GetViewMatrix() ;

		const D3DXMATRIX * GetBillboard();
		const D3DXMATRIX * GetBillboardY();

		void FlatMoveFB(float c);
		void FlatMoveLR(float c);

		Camera();
		virtual ~Camera();
	};

	//__CLASS__(render::Texture)___[纹理]
	class Texture
	{
		// 位图点阵（32位)
		DWORD * m_pBitmap;
		DWORD   m_dwBitmapSize;

		//纹理部分(D3D纹理 16/32)
		LPDIRECT3DTEXTURE9 m_pD3DTexture;

		DWORD m_dwWidth,m_dwHeight;

		D3DFORMAT m_d3dFormat;

		BOOL BitmapReadFilePCX(const char *pszFileName);
		BOOL BitmapReadFileBMP(const char *pszFilename);
		BOOL BitmapReadFileTGA(const char *pszFileName);
		BOOL BitmapReadFilePSD(const char *pszFileName);
		BOOL BitmapReadFilePPM(const char *pszFileName);
		BOOL BitmapReadFileJPG(const char *pszFileName);
		
		//ADD BY 彭谦 ******新增压缩纹理接口(目前不支持获得像素操作,不支持纹理缩小操作,m_pBitmap变量无效)
		BOOL TextureReadFileDDS(const char *pszFileName,LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel = 1);
		//ADD BY 彭谦 ******新增压缩纹理接口(目前不支持获得像素操作,不支持纹理缩小操作,m_pBitmap变量无效)
		
		BOOL CreateBitmap(DWORD dwWidth , DWORD dwHeight,DWORD dwColor = 0x00000000);

		void BitmapFlipVertical(void);

		DWORD *ReduceBitmap(DWORD *pSrc,DWORD dwWidth,DWORD dwHeight,DWORD dwTimes);//缩小位图times倍

	public:
		Texture(void);
		~Texture(void);

		BOOL Create(LPDIRECT3DDEVICE9 pDevice,const char * pszFileName,BOOL bTextureMode32,DWORD dwMaxTextureWidth = DEFAULT_MAX_TEXTURE_SIZE ,DWORD dwMaxTextureHeight = DEFAULT_MAX_TEXTURE_SIZE,DWORD dwTextureProcision=0);
		BOOL Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD dwColor,BOOL bTextureMode32,DWORD dwMaxTextureWidth = DEFAULT_MAX_TEXTURE_SIZE,DWORD dwMaxTextureHeight = DEFAULT_MAX_TEXTURE_SIZE);
		BOOL Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap,BOOL bTextureMode32,DWORD dwMaxTextureWidth = DEFAULT_MAX_TEXTURE_SIZE,DWORD dwMaxTextureHeight = DEFAULT_MAX_TEXTURE_SIZE);
		
		void Destroy();

		DWORD GetWidth()  const { return m_dwWidth;}
		DWORD GetHeight() const { return m_dwHeight;}
		DWORD * GetBitmap() const { return m_pBitmap;}
		DWORD GetBitmapSize() { return m_dwBitmapSize; }

		const LPDIRECT3DTEXTURE9 GetD3DTexture() const { return m_pD3DTexture;}

		BOOL CreateD3DTexture(LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel = 1);
		void DestroyD3DTexture();
		BOOL CopyBitmapToTexture();
		DWORD GetPixel(int iX, int iY);
		void  SetPixel(int iX, int iY,DWORD dwColor);

		BOOL CreateBitmap(const char * pszFileName);
		void DestroyBitmap();
	};	

	//__CLASS__(render::Interface)____[接口]
	class Interface
	{
		DECLARE_SINGLETON(Interface);

		HWND m_hWnd;

		//__CLASS__(render::Layer2D)__[渲染层] 
		class Layer2D
		{
			static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
		public:
			//__CLASS__(render::Layer2D::Font)___字体
			class Font
			{
				LPDIRECT3DVERTEXBUFFER9 m_pVB;

				struct tagPOSTDIFTEX0
				{
					float x,y,z,rhw;
					DWORD color;
					float u,v;
				};

				std::string m_strFileName;
				IDirect3DTexture9 * m_pTexture; 


				DWORD m_dwFontSize;
				DWORD m_dwFontInTextureSize;

				BOOL CreateFontTexture();
				void DestroyFontTexture();

				BOOL CreateVertexBuffer();
				void DestroyVertexBuffer();

			public:
				Font();
				~Font();

				DWORD GetFontSize() const { return m_dwFontSize;}
				DWORD GetFontInTextureSize() const { return m_dwFontInTextureSize;}

				BOOL Create(const char *szFileName);
				void Destroy();

				IDirect3DTexture9 * GetTexture() const { return  m_pTexture;}

				void _DEBUG_RenderFontTexture(int iX,int iY);

				void BeginRender();
				void RenderText(int iX,int iY,const char *szText,int iLen,DWORD dwColor = 0xFFFFFFFF,float fScale = 1.0f);
				void RenderText(int iX,int iY,const char *szText,DWORD *pColor,int iLen,float fScale = 1.0f);
				void EndRender();

				void Reset();	

			};

		private:
			Font * m_pFont;

			LPDIRECT3DVERTEXBUFFER9 m_pVB;


			//属性标记
			DWORD m_dwFillMode;


			BOOL CreateFont(const char *pszFontFileName);
			void DestroyFont();

		public:

			BOOL CreateVertexBuffer();
			void DestroyVertexBuffer();

			Layer2D(void);
			~Layer2D(void);

			void DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color = 0XFFFFFFFF);

			void DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0XFFFFFFFF);
			void DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0xFFFFFFFF);

			void DrawWireQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color = 0XFFFFFFFF);
			void DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color = 0XFFFFFFFF);

			void DrawTexture(int iX,int iY,DWORD dwTextureHandle,D3DCOLOR dwColor = 0XFFFFFFFF);
			void DrawTexture(int iX,int iY,int iWidth,int iHeight,DWORD dwTextureHandle,D3DCOLOR dwColor = 0XFFFFFFFF);
			void DrawTexture(int iX,int iY,int iWidth,int iHeight,float uOffset,float vOffset,float fScale,DWORD dwTextureHandle,D3DCOLOR dwColor = 0XFFFFFFFF);
			void DrawTextureSubRect(int iX,int iY,const RECT *pRect,DWORD dwTextureHandle,D3DCOLOR color = 0XFFFFFFFF);
			void DrawTextureSubRect(int iX,int iY,int iWidth,int iHeight,const RECT *pRect,DWORD dwTextureHandle,D3DCOLOR dwColor = 0XFFFFFFFF);
			void DrawTextureRotation(int iX,int iY,float fAngle,DWORD dwTextureHandle,D3DCOLOR dwColor);
			void TextOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR dwColor,float fScale = 1.0f);
			void TextOutEx(int iX,int iY,const char *pszString,int iLen,float fScale = 1.0f);
			int	 CalcTextWidth(const char *pszTextBuffer, int iLen);

			int  GetFontSize()   const { return m_pFont->GetFontSize();}
			Font * GetFont() const { return m_pFont;}

			BOOL Create(const char *pszFontFileName);
			void Destroy(void);

			void SetFillMode(DWORD dwFillMode) { m_dwFillMode = dwFillMode;}

			inline DWORD ConverTextColor(const char *pszColorText);
			LPDIRECT3DVERTEXBUFFER9		GetVertexBuffer() { return m_pVB;}
			//调试
			void _DEBUG_TextOutCache(int iX,int iY);
			void Update();
		};

		//__CLASS__(render::Layer3D)____[多流渲染 && 多边形缓冲]
		class Layer3D
		{
		public:
			//STRUCT__(POLYGON)___
			struct _tagTriangle
			{
				D3DXVECTOR3 pos[3];
				D3DCOLOR    color[3];
				D3DCOLOR    specular[3];
				D3DXVECTOR2 uv0[3];
				//D3DXVECTOR2 uv1[3];
				_tagTriangle *pNext;
			};
			//CLASS___(ShaderGroup)____
		public:
			class ShaderGroup
			{
				friend class Layer3D;

				//纹理句柄列表
				DWORD *m_pdwTexHandleArray[2];

				DWORD  m_dwTexCount[2];

				//Shader数据结构
			public:
				struct _tagShader
				{
					//自身渲染链表指针
					_tagShader *pNext;

					//__属性____________
					char szName[256];
					//顶点流格式
					DWORD  dwStreamType;

					//默认渲染属性
					//双面
					BOOL  bTwoSide;
					DWORD dwAlphaTestMode;
					DWORD dwAlphaBlendMode;
					//            _____[TestMode] [0] none [1] transparent
					//		     |
					// alpha op -|
					//		     |____ [BlendMode] [0] additive [1] alpha 
					//							   [2] color [3] reserved
					//
					struct _tagLayer
					{				  
						DWORD dwColorOp;
						DWORD dwColorArg1;
						DWORD dwColorArg2;
						DWORD dwAlphaOp;
						DWORD dwAlphaArg1;
						DWORD dwAlphaArg2;
						DWORD dwTexture;
					}tLayer[2];

					//多边形表头
					_tagTriangle *pTriangleAlphaTestHead;
					_tagTriangle *pTriangleAlphaBlendHead;
					_tagTriangle *pTriangleTransparentHead;
					//_tagTriangle *pTraingleAlphaBlendTopHead;

				};
			private:

				_tagShader *m_pShaderArray;
				DWORD		m_dwShaderCount;

				//_____渲染表结构_______

				struct _tagTex0
				{
					_tagTex0 *pNext;

					DWORD dwTex0Handle;

					//指向Shader的指针
					_tagShader *pShaderHead;
				};


				//第一层TEX1表项
				struct _tagTex1
				{
					DWORD dwTex1Handle;

					_tagTex0 *pTex0Head;
				};

				_tagTex1 *m_pTex1Array;


			public:
				ShaderGroup(void);
				~ShaderGroup(void);

				//建立SHADER
				BOOL Create(const char * pszString,const char *pszTexPath,DWORD dwTextureProcision=0);
				BOOL CreateFromFile(const char * pszFileName,DWORD dwTextureProcision=0);
				void Destroy(void);
				void Dump(void);

				void RenderAlphaTestMode();
				void RenderAlphaBlendMode();
				void RenderTransparentMode();
				//void RenderAlphaBlendTopMode();


				_tagShader * GetShader(DWORD dwShaderIndex) { return &m_pShaderArray[dwShaderIndex];}
				DWORD GetTextureCount(DWORD dwLayer) const { return m_dwTexCount[dwLayer];}
				DWORD GetTextureHandle(DWORD dwLayer,DWORD dwTextureIndex) { return m_pdwTexHandleArray[dwLayer][dwTextureIndex];}

				DWORD GetShaderCount() { return m_dwShaderCount;}

				//多流渲染
				void __RenderPOSCOL(_tagTriangle *pHead);
				void __RenderPOSDIFTEX0(_tagTriangle *pHead);
				void __RenderPOSDIFSPCTEX0(_tagTriangle *pHead);
				void _SetTextureOp(_tagShader *pShader);
			};

			//_____多流渲染部分_______
			//多流MULTI-STREAM
			//DEFAULT 3D STREAM SOURCE
			LPDIRECT3DVERTEXBUFFER9 m_pVB_POSITION;
			//LPDIRECT3DVERTEXBUFFER9 m_pVB_NORMAL;
			LPDIRECT3DVERTEXBUFFER9 m_pVB_COLOR_DIFFUSE;
			LPDIRECT3DVERTEXBUFFER9 m_pVB_TEX0;
			LPDIRECT3DVERTEXBUFFER9 m_pVB_COLOR_SPECULAR;
			//STREAM_DECL HANDLE ARRAY
			IDirect3DVertexDeclaration9 * m_pVertexDeclarationArray[LMST_ENDOFTYPE];

			BOOL CreateVertexBuffer();
			void DestroyVertexBuffer();

			void SetStreamType(DWORD dwStreamType);
			void SetTexture(DWORD dwShaderGroupHandle,DWORD dwShaderIndex);

			void CreateVertexDecl();
			void DestroyVertexDecl();

			//__多边形缓冲___
			_tagTriangle * m_pPolygonArray;
			DWORD m_dwPolygonArrayUsage;

			void CreatePolgyonArray(void);
			void DestroyPolygonArray(void);

			//____SHADERGROUP管理____
			ShaderGroup * m_pShaderGroupArray[MAX_SHADER_GROUP_ARRAY_SIZE];
			DWORD m_dwShaderGroupArrayUsage;

			//渲染层属性
			DWORD m_dwFillMode;
		public:
			Layer3D(void);
			~Layer3D(void);

			//属性接口
			void  SetFillMode(DWORD dwFillMode) { m_dwFillMode = dwFillMode;}
			DWORD GetTriangleArrayUsage() const { return m_dwPolygonArrayUsage;}

			BOOL Create(void);
			void Destroy();

			DWORD CreateShaderGroup(const char * pszString,const char *pszTexPath,DWORD dwTextureProcision=0);
			DWORD CreateShaderGroupFromFile(const char *pszFileName,DWORD dwTextureProcision=0);
			void  DestroyShaderGroup(DWORD dwSGHandle);

			ShaderGroup * GetShaderGroup(DWORD dwShaderGroup) { return m_pShaderGroupArray[dwShaderGroup];}

			void Dump(void);

			void Render(void);

			DWORD GetFillMode() { return m_dwFillMode;}

			//直接渲染，只使用纹理，状态需要自己设置
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor);
			//送入缓冲，使用SHADER属性渲染
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DCOLOR *pColor,DWORD dwMode = 0);
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,DWORD dwMode = 0);
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,const D3DCOLOR *pColorSpecular,DWORD dwMode = 0);


			void SetStreamSource(DWORD dwStreamType);

			//调试和编辑使用
			void _DEBUG_DrawLine(const D3DXVECTOR3 pVtx[],D3DCOLOR dwColor);
			void _DEBUG_DrawLine(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,D3DCOLOR dwColor);
			void _DEBUG_DrawTriangle(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DCOLOR dwColor);		//直接画一个三角形
			void _DEBUG_DrawTriangle(DWORD dwTexturHandle,const D3DXVECTOR3 *pVtx,const D3DXVECTOR2 *pUV0,D3DCOLOR dwColor);
			void _DEBUG_DrawWireBox(const D3DXVECTOR3 *pCenter,float fRadius,D3DCOLOR dwColor);//直接画一个BOX
			void _DEBUG_DrawWireBox(const  D3DXVECTOR3 *pMin,const  D3DXVECTOR3 *pMax,D3DCOLOR dwColor);
			void _DEBUG_DrawWireQuad(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,const D3DXVECTOR3 *pVtx3,D3DCOLOR dwColor);

			void Clear();
			void Reset();
		};

		//硬件检测结果
		struct _tagInfo
		{
			D3DADAPTER_IDENTIFIER9 AdapterIdentifier;//适配器标示
			D3DPRESENT_PARAMETERS  d3dpp;			 //绘图参数
			D3DCAPS9			   d3dCaps;			 //接口描述表			
			D3DDISPLAYMODE		   * pModeArray;	 //显示模式列表
			int					   iModeCount;		 //显示模式计数
		}m_stInfo;

	public:
		//配置部分
		struct _tagSetting
		{
			//当前字库名称
			char szFont[_MAX_PATH];

			//当先渲染属性
			BOOL   bHVProcess;

			//参数
			BOOL bIsWindow;

			//窗口参数		
			int iWndPosX,iWndPosY ;//windows mode only
			int iWndHeight,iWndWidth;
			BOOL bWndAutoStyle;		//自动调整窗口风格
			BOOL bWndAutoCenter;		//自动调整居于中心

			//全屏幕参数
			D3DFORMAT d3dfmtBackBuffer;
			//D3DFORMAT d3dfmtStencil;

			//细节
			BOOL bTextureMode32;
			BOOL bMipMapping;

			//垂直同步
			DWORD dwPresentationInterval;//ONE | DEFAULT | IMMEDIATE;


		}m_tSetting;
	private:

		//初始化后状态表

		//INTERFACE & DEVICE
		LPDIRECT3D9		   m_pD3D;			
		LPDIRECT3DDEVICE9  m_pD3DDevice; 
		//纹理缓冲
		Texture * m_pTextureArray[MAX_TEXTURE_CACHE_SIZE];
		DWORD	  m_dwTextureMemorySize;
		DWORD	  m_dwTextureCnt;
		//渲染层
		Layer2D *m_pLayer2D;
		Layer3D *m_pLayer3D;

		//__私有函数接口__
		void DefaultSetting(const char *pszConfigString);

		BOOL CreateDevice(void);
		void DestroyDevice(void);

		BOOL CreateDirect3D9(void);
		void DestroyDirect3D9(void);

		BOOL CheckHardware(void);
		void SetDefaultRenderState();

		BOOL CreateLayer();
		void DestroyLayer(void);


		Interface(void);
		~Interface(void);
	public:

		BOOL Create(HWND hWnd,const char *pszConfigFile);
		void Destroy();

		LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice;}

		BOOL BeginScene(void);
		void EndScene(void);  

		_tagSetting * GetSetting() { return &m_tSetting;} 
		_tagInfo * GetInfo() { return &m_stInfo;}

		void Clear(D3DCOLOR d3dColor);

		//__TEXTURE__
		DWORD  CreateTexture(const char * pszFileName,DWORD dwTextureProcision=0);
		DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap);
		DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwColor);
		void   DestroyTexture(DWORD dwTextureHandle);
		void   SetTexture(DWORD dwStage,DWORD dwTextureHandle);
		Texture * GetTexture(DWORD dwTextureHandle);
		DWORD  GetTextureCount() const { return m_dwTextureCnt;}
		DWORD  GetTextureMemorySize() const { return m_dwTextureMemorySize;}

		//___ATTRIB__
		int  GetWndWidth()  const { return m_tSetting.iWndWidth;}
		int  GetWndHeight() const { return m_tSetting.iWndHeight;}
		HWND GetWndHandle() const { return m_hWnd;}

		//___STATUS___常用状态设置
		void Cull(BOOL bEnable);

		//渲染层接口
		Layer2D *GetLayer2D() { return m_pLayer2D;}
		Layer3D *GetLayer3D() { return m_pLayer3D;}

		//调试数据
		void Dump(void);

		//调整设置
		void ModifySetting(const char *pszConfigString);		
		BOOL Reset(void);
		D3DFORMAT CheckDepthStencilFormat(D3DFORMAT dwBackBufferFormat);
	};
};