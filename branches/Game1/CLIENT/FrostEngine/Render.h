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
#include "singleton.h"
#include "math.h"

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
enum eRENDTYPE
{
	RENDTYPE_TESTNONE = 0,
	RENDTYPE_TESTTRANSPARENT,
	RENDTYPE_BLENDADDITIVE,
	RENDTYPE_BLENDALPHA,
	RENDTYPE_BLENDALPHALOW,
	RENDTYPE_BLENDALPHALOW1,
	RENDTYPE_BLENDALPHALOW2,
	RENDTYPE_BLENDALPHALOW3,
	RENDTYPE_BLENDALPHALOW4,
	RENDTYPE_TRANSPARENT
};

enum LAYER3D_MULTI_STREAM_TYPE
{
	LMST_POSNML = 0,
	LMST_POSCOL,
	LMST_POSNMLTEX0,
	LMST_POSCOLTEX0,
	LMST_POSNMLCOL,
	LMST_POSNMLCOLTEX0,
	LMST_POS,
	LMST_POSTEX0,
	LMST_POSTEX0TEX1,
	LMST_POSNORDIFTEX0TEX1,		//pos, nor, dif, 两重uv
	LMST_ENDOFTYPE
};
enum LAYER3D_MULTI_STREAM_TYPE_FOR_GPU
{
	LMSTFG_POS = 0,
	LMSTFG_POSNML,
	LMSTFG_POSUV,
	LMSTFG_POSNMLUV,
	LMSTFG_POSMATRIX,
	LMSTFG_POSNMLMATRIX,
	LMSTFG_POSUVMATRIX,
	LMSTFG_POSNMLUVMATRIX,
	LMSTFG_POSCOLOR,
	LMSTFG_POSUVCOLOR,
	LMSTFG_POSNMLCOLOR,
	LMSTFG_POSNMLUVCOLOR,
	LMSTFG_ENDOFTYPE
};


#define MAX_SHADER_GROUP_ARRAY_SIZE				3072
#define MAX_POLYGON_ARRAY_SIZE					(1)

#define MAX_TEXTURE_CACHE_SIZE	16384

#define DEFAULT_MAX_TEXTURE_SIZE    512

#define FONT_TEXTURE_SIZE  512


//__CLASS__(render::Layer2D)__DEFINE__
#define MAX_RENDER_LAYER2D_CACHE_SIZE 2048
#define MAX_RENDER_LAYER2D_VB_SIZE    256
#define	MAX_RENDER_LAYER2D_FONT_SIZE  2


#define LAYER3D_RENDVERTEXBUFFER_SIZE			(640 * 3)
#define LAYER3D_RENDINDEXBUFFER_SIZE			(640 * 9)
#define LAYER3D_TEMPBUFFER_SIZE					(50000)
#define LAYER3D_VERTEXBUFFER_SIZE				(8192)
#define LAYER3D_INDEXBUFFER_SIZE				(50000)
#define LAYER3D_DRAWVERTEXBUFFER_SIZE			(8192 * 3)
#define LAYER3D_DRAWINDEXBUFFER_SIZE			(8192 * 3)




#define EFFECTTYPE_USEPOS		 (1 << 0 )
#define EFFECTTYPE_USENORMAL	 (1 << 1 )
#define EFFECTTYPE_USEUV		 (1 << 2 )
#define EFFECTTYPE_USEMATINDEX	 (1 << 3 )
#define EFFECTTYPE_USECOLOR		 (1 << 4 )
#define EFFECTTYPE_UVANIMI		 (1 << 5 )
#define EFFECTTYPE_TEXTUREANIMI  (1 << 6 )
#define EFFECTTYPE_ENVIROMENT	 (1 << 7 )
#define EFFECTTYPE_USELIGHT	     (1 << 8 )
#define EFFECTTYPE_ANIMUSESCAL	 (1 << 9 )
#define EFFECTTYPE_BILLBOARD	 (1 << 10)
#define EFFECTTYPE_BILLBOARDY	 (1 << 11)
#define EFFECTTYPE_SCAL			 (1 << 12)
#define EFFECTTYPE_TEXPROJECTED	 (1 << 13)
#define EFFECTTYPE_SWMAXMAT		 (1 << 14)
#define EFFECTTYPE_HWMAXMAT		 (1 << 15)




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
		inline const D3DXVECTOR3 * GetCornerVertex() const { return m_vConner;}
		float GetLength(){return FM_Distance(m_vConner[0],m_vConner[3]);}
		float GetWidth(){return FM_Distance(m_vConner[0],m_vConner[1]);}
		float GetHeight(){return FM_Distance(m_vConner[0],m_vConner[4]);}
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

		inline const D3DXVECTOR3 * GetCornerVertex() const { return m_vFrustum;}

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

		BOOL BitmapReadFilePCX(CRFile* pFile);
		BOOL BitmapReadFileBMP(CRFile* pFile);
		BOOL BitmapReadFileTGA(CRFile* pFile);
		BOOL BitmapReadFilePSD(CRFile* pFile);
		BOOL BitmapReadFilePPM(CRFile* pFile);
		BOOL BitmapReadFileJPG(CRFile* pFile);
		//ADD BY 彭谦 ******新增压缩纹理接口(目前不支持获得像素操作,不支持纹理位深压缩操作,m_pBitmap变量无效)
		BOOL TextureReadFileDDS(CRFile* pFile,LPDIRECT3DDEVICE9 pDevice,DWORD dwLevel = 1);
		//ADD BY 彭谦 ******新增压缩纹理接口(目前不支持获得像素操作,不支持纹理位深压缩操作,m_pBitmap变量无效)

		BOOL CreateBitmap(DWORD dwWidth , DWORD dwHeight,DWORD dwColor = 0x00000000);

		void BitmapFlipVertical(void);

		DWORD *ReduceBitmap(DWORD *pSrc,DWORD dwWidth,DWORD dwHeight,DWORD dwTimes);//缩小位图times倍

	public:
		Texture(void);
		~Texture(void);

		BOOL Create(LPDIRECT3DDEVICE9 pDevice,const char * pszFileName,BOOL bTextureMode32,DWORD dwMaxTextureWidth = DEFAULT_MAX_TEXTURE_SIZE ,DWORD dwMaxTextureHeight = DEFAULT_MAX_TEXTURE_SIZE,DWORD dwTextureProcision=0);
		BOOL Create(LPDIRECT3DDEVICE9 pDevice,CRFile* pFile,BOOL bTextureMode32,DWORD dwMaxTextureWidth = DEFAULT_MAX_TEXTURE_SIZE ,DWORD dwMaxTextureHeight = DEFAULT_MAX_TEXTURE_SIZE,DWORD dwTextureProcision=0);

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
		BOOL CreateJpgTextureFromMemory(unsigned char * buf,unsigned long lSize,LPDIRECT3DDEVICE9 pDevice);
		void DestroyBitmap();
		D3DFORMAT GetFormat(){return m_d3dFormat;}
	};	

	//__CLASS__(render::Interface)____[接口]
	class Interface
	{
		DECLARE_SINGLETON(Interface);

		HWND m_hWnd;

		// add by lpf 2007-11-27
		struct _tagSetting;

		//__CLASS__(render::Layer2D)__[渲染层] 
		class Layer2D
		{
			static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
		public:
			// add by lpf 2007-11-27
			// 字体信息
			struct _tagFontInfo
			{
				int		 iStyleIndex;		//字体样式索引
				bool	 bBorder;			//字体是否描边
				bool	 bShadow;			//字体是否有阴影
				DWORD	 dwShadowOffset;	//字体阴影的偏移量
				float	 fScale;			//字体缩放
				D3DCOLOR dwColorFont;		//字体颜色
				D3DCOLOR dwColorBorder;		//字体描边颜色
				D3DCOLOR dwColorShadow;		//字体阴影颜色
			};

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
				void ResetRelease();
				void ResetReBuild();

			};

		private:
			// modify by lpf 2007-11-27
			int			   m_iCurrentFontsSize;							//当前系统字体样式数量
			Font		 * m_pFonts[MAX_RENDER_LAYER2D_FONT_SIZE];		//当前系统字体样式指针
			_tagFontInfo   m_tFontInfo;									//当前2D渲染层的渲染字体的信息

			LPDIRECT3DVERTEXBUFFER9 m_pVB;

			//属性标记
			DWORD				 m_dwFillMode;				//填充模式
			D3DTEXTUREFILTERTYPE m_eFilterType;				//纹理过滤类型

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

			void DrawText(int iX, int iY, const char * pszString, _tagFontInfo * pFontInfo = NULL);				//根据字体信息来输出文本
			void DrawText(int iX, int iY, const char * pszString, int iLen, _tagFontInfo * pFontInfo = NULL);	//根据字体信息来输出指定长度的文本

			// modify by lpf 2007-11-27 - 增加了一个参数iStyleIndex
			void TextOut(int iX, int iY, const char *pszString, int iLen, D3DCOLOR dwColor, int iStyleIndex = 0, float fScale = 1.0f);
			void TextOutEx(int iX,int iY,const char *pszString,int iLen, int iStyleIndex = 0, float fScale = 1.0f);
			int	 CalcTextWidth(const char *pszTextBuffer, int iLen);

			// modify by lpf 2007-11-27
			//int	   GetFontSize() const								{ return m_pFonts[m_tFontInfo.iStyleIndex]->GetFontSize();}
			int	   GetFontSize(int iStyleIndex = -1) const				{ return (iStyleIndex == -1) ? m_pFonts[m_tFontInfo.iStyleIndex]->GetFontSize() : m_pFonts[iStyleIndex]->GetFontSize(); }
			Font * GetFont() const									{ return m_pFonts[m_tFontInfo.iStyleIndex]; }
			void   GetCurrentFontInfo(_tagFontInfo * pFontInfo)		{ *pFontInfo = m_tFontInfo; }						//得到当前2D层渲染的字体信息

			// modify by lpf 2007-11-27 - 修改了该函数的传入参数
			BOOL Create(const render::Interface::_tagSetting & tSetting);
			void Destroy(void);

			void				 SetFillMode(DWORD dwFillMode)		{ m_dwFillMode = dwFillMode; }
			D3DTEXTUREFILTERTYPE GetFilterType()					{ return m_eFilterType; }			//得到当前2D层使用的纹理过滤类型

			// add by lpf 2007-11-27
			void SetCurrentFontInfo(const _tagFontInfo * pFontInfo)		{ m_tFontInfo = *pFontInfo; }

			inline DWORD ConverTextColor(const char *pszColorText);
			LPDIRECT3DVERTEXBUFFER9		GetVertexBuffer() { return m_pVB;}
			//调试
			// modify by lpf 2007-11-27 - 增加了一个参数
			void _DEBUG_TextOutCache(int iX,int iY, int iStyleIndex);
			void Update();
			void UpdateRelease();
			void UpdateRebuild();
		};

		//__CLASS__(render::Layer3D)____[多流渲染 && 多边形缓冲]
		class Layer3D
		{
		public:
			//STRUCT__(POLYGON)___
			struct _tagTriangle
			{
				D3DXVECTOR3 pos[3];
				D3DXVECTOR3 normal[3];
				D3DCOLOR    color[3];
				D3DCOLOR    specular[3];
				D3DXVECTOR2 uv0[3];
				//D3DXVECTOR2 uv1[3];
				_tagTriangle *pNext;
			};

			// 动态渲染状态结构体(数据成员为0xffffffff时,表示使用默认的渲染状态)
			struct _tagShaderState
			{
				char szShaderName[256];		//需要创建的动态Shader名字

				// 渲染属性
				DWORD dwStreamType;			//顶点流格式
				DWORD dwTwoSide;			//双面性
				DWORD dwAlphaTestMode;
				DWORD dwAlphaBlendMode;

				// 纹理操作
				DWORD dwColorOp;
				DWORD dwColorArg1;
				DWORD dwColorArg2;
				DWORD dwAlphaOp;
				DWORD dwAlphaArg1;
				DWORD dwAlphaArg2;
			};
			//*****利用GPU性能做的优化==ADD BY 彭谦===
		public:
			class RendMesh;
			class RendMeshManager;
			class EffectManager;
			RendMeshManager* GetRendMeshManager(){return &m_RendMeshManager;}
			EffectManager* GetEffectManager(){return &m_EffectManager;}
			class RendMesh 
			{
			public:
				struct TAGMESH
				{
					TAGMESH();
					~TAGMESH();
					void Release();
					void SetMatrix(D3DXMATRIX* pmatrix,DWORD count);
					void SetMatrixForNormal(D3DXMATRIX* pmatrix,DWORD count);
					void SetFlash(bool bflash = false);
					void SetEffectID(DWORD dwEID){dwEffectID = dwEID;}
					void SetTechID(DWORD dwTeD){dwTechID = dwTeD;}
					void SetDeclID(DWORD dwDec){dwDeclID = dwDec;}
					void SetBaseColor(DWORD dwcolor);
					void SetMultiColor(DWORD dwcolor);
					void SetMatrixForUse(D3DXMATRIX* mat){matrixforuse = *mat;}
					void SetMatrixForVP(D3DXMATRIX* mat){matrixforVP = *mat;}
					void SetMatrixForV(D3DXMATRIX* mat){matrixforView = *mat;}
					void SetMatrixForP(D3DXMATRIX* mat){matrixforProjective = *mat;}
					std::vector<DWORD>& GetVecPassID(){return vecdwPassID;}
					D3DXMATRIX* ptagmesharray;
					D3DXMATRIX* ptagmesharrayfornormal;
					D3DXMATRIX  matrixforuse;
					D3DXMATRIX  matrixforVP;
					D3DXMATRIX  matrixforView;
					D3DXMATRIX  matrixforProjective;
					bool bflash;
					DWORD dwEffectID;
					DWORD dwTechID;
					DWORD dwDeclID;
					D3DXVECTOR4 v4basecolor;
					D3DXVECTOR4 v4multicolor;
					std::vector<DWORD> vecdwPassID;
				};
			public:
				RendMesh();
				~RendMesh();
				void Init();
				bool GetIsUsed(){return m_bused;}
				void SetIsUsed(bool b){m_bused = b;}
				void CreateMatrixArray(DWORD dwMatrixMode = 0);
				void DestroyMatrixArray();
				void SetStreamType(DWORD dwStreamType);
				TAGMESH* GetTagMesh(DWORD mode = 0);
				bool Create(D3DXVECTOR3* ppos,
					DWORD* pmatrixindex,
					D3DXVECTOR3* pnormal,
					D3DXVECTOR2* puv,WORD* pindex,
					DWORD dwvertexcount,
					DWORD dwindexcount,
					DWORD dwmatcount,
					DWORD dwtexhandle,
					DWORD dwalphatorbmodel,
					DWORD dwcolorop,
					DWORD dwcolorarg1,
					DWORD dwcolorarg2,
					DWORD dwalphaop,
					DWORD dwalphaarg1,
					DWORD dwalphaarg2,
					bool  bstatic,
					BOOL  btwoside,
					bool  busenormal,
					bool  bswprocess = false
					);
				bool ReCreate(D3DXVECTOR3* ppos,DWORD* pmatrixindex, D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,bool bswprocess = true);
				void Release();
				void ReleaseRsSWP();
				void ReleaseRsHWP();
				void Render(DWORD mode = 0);
				void Draw(D3DXVECTOR3* ppos,DWORD* pmatrixindex, D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,D3DXMATRIX* pmatarray,D3DXMATRIX* pmatarrayfornormal,D3DXMATRIX* pmatforuse,D3DXMATRIX* pmatview,D3DXMATRIX* pmatproj,D3DXMATRIX* pmatfortp,D3DXVECTOR4 v4basecolor,D3DXVECTOR4 v4multicolor,bool bnoanim,bool buseenv,bool buseuv,bool blight,bool btransusescal,bool busetexture,bool btextureprojected,DWORD dwMode);
				void SendToRendList(DWORD Mode);
				DWORD GetTexHandle(){return m_dwtexhandle;}
				bool  GetStatic(){return m_bstatic;}
				DWORD GetJointCount(){return m_dwtagmesharraycount;}
				DWORD GetAlphaOrBlendMode(){return m_dwAlphaTestOrBlendMode;}
				bool  GetIsSoftWareProcessing(){return m_bswprocess;}
				bool  GetIsRsHvpCreated(){return m_bhwpcreated;}
				bool  GetIsRsSvpCreated(){return m_bswpcreated;}
				void  SetToReCreate(bool b){m_btorecreate = b;}
				bool  GetToReCreate(){return m_btorecreate;}
				void  SetSoftWareVertexprocess(bool b);
			protected:
			private:
				DWORD m_dwStreamType;
				DWORD m_dwtexhandle;
				DWORD m_dwColorOp;
				DWORD m_dwColorArg1;
				DWORD m_dwColorArg2;
				DWORD m_dwAlphaOp;
				DWORD m_dwAlphaArg1;
				DWORD m_dwAlphaArg2;
				DWORD m_dwAlphaTestOrBlendMode;
				DWORD m_dwtagmesharraycount;
				DWORD m_dwvertexcount;
				DWORD m_dwindexcount;
				DWORD m_dwtrianglecount;
				DWORD m_dwusedmatrixstaticrendcount;
				DWORD m_dwusedmatrixTransparentcount;
				bool  m_bused;
				bool  m_bswprocess;
				bool  m_bhwpcreated;
				bool  m_bswpcreated;
				bool  m_bstatic;
				BOOL  m_bTwoside;
				bool  m_busenormal;
				bool  m_bpushedstaicrend;
				bool  m_bpushedtransparent;
				bool  m_btorecreate;


				std::vector<TAGMESH*> m_vectagmesharrayforstaticrend;
				std::vector<TAGMESH*> m_vectagmesharrayforTransparent;

				LPDIRECT3DVERTEXBUFFER9 m_pd3dposbuffer;
				LPDIRECT3DVERTEXBUFFER9 m_pd3duvbuffer;
				LPDIRECT3DVERTEXBUFFER9 m_pd3dnormalbuffer;
				LPDIRECT3DVERTEXBUFFER9 m_pd3dmatrixindexbuffer;
				//LPDIRECT3DVERTEXBUFFER9 m_pd3ddiffusebuffer;
				LPDIRECT3DINDEXBUFFER9  m_pd3dindexbuffer;

				LPDIRECT3DVERTEXBUFFER9 m_pd3dposbufferforswp;
				LPDIRECT3DVERTEXBUFFER9 m_pd3duvbufferforswp;
				LPDIRECT3DVERTEXBUFFER9 m_pd3dnormalbufferforswp;
				LPDIRECT3DVERTEXBUFFER9 m_pd3dmatrixindexbufferforswp;
				//LPDIRECT3DVERTEXBUFFER9 m_pd3ddiffusebufferforswp;
				LPDIRECT3DINDEXBUFFER9  m_pd3dindexbufferforswp;
			};
			class RendMeshManager
			{
			public:
				RendMeshManager();
				~RendMeshManager();
				DWORD CreateRendMesh(DWORD dwShaderGroupHandle,DWORD dwShaderHandle,DWORD dwAlphaModel,D3DXVECTOR3* ppos,DWORD* pmatrixindex,D3DXVECTOR3* pnormal,D3DXVECTOR2* puv,WORD* pindex,DWORD dwvertexcount,DWORD dwindexcount,DWORD dwmatcount,bool bstatic,bool busenormal,bool bswprocess = false);
				void  ReleaseRendMesh(DWORD index);
				RendMesh* GetRendMesh(DWORD index);
				void ResetRelease();
				void ResetRebuild();
				void Destroy();
			protected:
			private:
				std::vector<RendMesh*> m_vecrendmesh;
			};
			class Effect
			{
			public:
				Effect();
				~Effect();
				DWORD m_dwPasNum;
				std::vector<string> m_vecTechName;
				LPD3DXEFFECT m_pd3deffect;
			};
			class EffectManager
			{
			public:
				enum MACROTYPR
				{  
					ET_USEPOS = 0,
					ET_USENORMAL,	
					ET_USEUV,		
					ET_USEMATINDEX,	
					ET_USECOLOR,	
					ET_UVANIMI,		
					ET_TEXTUREANIMI,	
					ET_ENVIROMENT,	
					ET_USELIGHT,
					ET_ANIMUSESCAL,
					ET_BILLBOARD,
					ET_BILLBOARDY,
					ET_SCAL,
					ET_TEXPROJECTED,
					ET_SWMAXMAT, 
					ET_HWMAXMAT,
					ET_MACROMAX
				};
				EffectManager();
				~EffectManager();
				DWORD CreateEffectFile(DWORD dwmac);
				void SetEffectandPass(DWORD dwEffectID,DWORD dwTechID,DWORD dwPass);
				void UpdateChanges();
				LPD3DXEFFECT GetNowEffects();
				void EndEffectandPass();
				void ResetRelease();
				void ResetRebuild();
				void Destroy();
			protected:
			private:
				hash_map<DWORD,Effect*> m_mapd3deffect;

				DWORD		m_dwNowEffectID;
				DWORD		m_dwNowTechID;
				DWORD		m_dwNowPassID;
				DWORD		m_dwMacro[ET_MACROMAX];
				D3DXMACRO   m_dwd3dxMacro[ET_MACROMAX];

			};
		private:
			EffectManager m_EffectManager;
			RendMeshManager m_RendMeshManager;
			//利用GPU性能做的优化==ADD BY 彭谦===*****//
		public:
			//CLASS___(ShaderGroup)____
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
					_tagShader();
					~_tagShader();
					//自身渲染链表指针
					_tagShader *pNext;

					//__属性____________
					char szName[256];

					// 顶点流格式
					DWORD  dwStreamType;
					DWORD  dwTempStreamType;



					// 默认渲染属性
					BOOL  bTwoSide;				//双面性					
					DWORD dwAlphaTestMode;
					DWORD dwAlphaBlendMode;

					//            _____[TestMode] [0] none [1] transparent
					//		     |
					// alpha op -|
					//		     |____ [BlendMode] [0] additive [1] alpha 
					//							   [2] color [3] reserved
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


					//增加索引顶点结构---add by 彭谦----//

					struct RendBuffer
					{  
						RendBuffer();
						~RendBuffer();
						void Clear(DWORD rendtype);
						bool AddTagBuffer(DWORD count,DWORD streamtype,DWORD rendtype);
						bool AddDynamicTagBuffer(DWORD count,DWORD streamtype,DWORD rendtype);
						struct _tagBuffer
						{
							enum eBUFFERTYPE
							{
								BUFFERTYPE_POS = 0,
								BUFFERTYPE_UV ,
								BUFFERTYPE_DIFFUSE,
								BUFFERTYPE_INDEX,
								BUFFERTYPE_NORMAL
							};
							_tagBuffer();
							~_tagBuffer();
							void Clear();
							bool CreateBuffer();
							//void SetMaterial(D3DMATERIAL9* pmtrl);
							bool ExtendVertexBuffer(DWORD num,DWORD streamtype);
							bool ExtendIndexBuffer(DWORD num);
							void LockBuffer(DWORD dwbuffertype);
							void UnLockBuffer(DWORD dwbuffertype);
							void Release();
							DWORD dwTempColorOp;
							DWORD dwTempColorArg1;
							DWORD dwTempColorArg2;
							DWORD dwTempAlphaOp;
							DWORD dwTempAlphaArg1;
							DWORD dwTempAlphaArg2;

							DWORD trianglecount;
							DWORD indexcount;
							DWORD vertexcount;
							DWORD vertexsize;
							DWORD indexsize;
							LPDIRECT3DVERTEXBUFFER9 pd3dposbuffer;
							LPDIRECT3DVERTEXBUFFER9 pd3duvbuffer;
							LPDIRECT3DVERTEXBUFFER9 pd3dnormalbuffer;
							LPDIRECT3DVERTEXBUFFER9 pd3ddiffusebuffer;
							LPDIRECT3DINDEXBUFFER9  pd3dindexbuffer;
							bool bposlocked;
							bool buvlocked;
							bool bnormallocked;
							bool bdiffuselocked;
							bool bindexlocked;
							WORD* pindex;
							D3DXVECTOR3* ppos;
							D3DXVECTOR3* pnormal;
							D3DCOLOR*    pdiffuse;
							D3DXVECTOR2* puv;
							D3DXMATRIX   matworld; 

						};
						struct BufferList
						{
							int bufferusenum;
							int dynamicbufferusenum;
							int lightbufferusenum;
							int lightdynamicbufferusenum;
							bool bpushed;
							vector<_tagBuffer> rendbuffer;
							vector<_tagBuffer> dynamicrendbuffer;
							vector<_tagBuffer> lightrendbuffer;
							vector<_tagBuffer> lightdynamicrendbuffer;
							BufferList();
							~BufferList();
							void Init();
							void Release();
							void Clear();

						};
						BufferList testnonebuffer;
						BufferList testtransparentbuffer;
						BufferList blendadditivebuffer;
						BufferList blendalphabuffer;
						BufferList blendalphalowbuffer;
						BufferList blendalphalow1buffer;
						BufferList blendalphalow2buffer;
						BufferList blendalphalow3buffer;
						BufferList blendalphalow4buffer;
						BufferList transparentbuffer;
						DWORD dwtexturehandle;

					}rendbuffer;
					void Rend(DWORD dwrendtype);
				};

			private:
				_tagShader * m_pShaderArray;
				DWORD		 m_dwShaderCount;


				//_____渲染表结构_______
				struct _tagTex0
				{
					_tagTex0 *pNext;

					DWORD dwTex0Handle;

					//指向Shader的指针
					_tagShader * pShaderHead;
					_tagShader * pDynamicShaderHead;
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
				BOOL Create(CRFile* pFile,DWORD dwTextureProcision=0);

				BOOL CreateFromFile(const char * pszFileName,DWORD dwTextureProcision=0);
				void Destroy(void);
				void Dump(void);
				void ReSet();
				void RenderAlphaTestMode();			//渲染alpha测试模式
				void RenderAlphaBlendMode();		//渲染alpha混合模式
				void RenderTransparentMode();
				//void RenderAlphaBlendTopMode();

				_tagShader * GetShader(DWORD dwShaderIndex)
				{
					if(dwShaderIndex < m_dwShaderCount)
						return &m_pShaderArray[dwShaderIndex];
					else
						return NULL;
				}
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
			LPDIRECT3DVERTEXBUFFER9 m_pVB_NORMAL;
			LPDIRECT3DVERTEXBUFFER9 m_pVB_COLOR_DIFFUSE;
			LPDIRECT3DVERTEXBUFFER9 m_pVB_TEX0;
			LPDIRECT3DINDEXBUFFER9  m_pIB;
			D3DLIGHT9*              m_pD3DLight;
			D3DMATERIAL9*           m_pMaterial;
			D3DCOLOR                m_Ambinet;
			BOOL                    m_bFog;
			BOOL                    m_bMipMap;
			//STREAM_DECL HANDLE ARRAY
			IDirect3DVertexDeclaration9 * m_pVertexDeclarationArray[LMST_ENDOFTYPE];
			IDirect3DVertexDeclaration9 * m_pVertexDeclarationArrayForGpu[LMSTFG_ENDOFTYPE];
			hash_map<DWORD,IDirect3DVertexDeclaration9*> m_mapVertexDeclarationArrayForGpu;
			BOOL CreateVertexBuffer();
			void DestroyVertexBuffer();

			void SetStreamType(DWORD dwStreamType);
			void SetTexture(DWORD dwShaderGroupHandle,DWORD dwShaderIndex);


			void CreateVertexDecl();
			void DestroyVertexDecl();
			void SetLight(D3DLIGHT9* d3dlight);
			void SetMaterial(D3DMATERIAL9* pmaterial);
			void SetAmbient(D3DCOLOR ambient);
			void SetFogEnable(BOOL bfog){m_bFog = bfog;}
			void SetMipMap(BOOL bmipmap){m_bMipMap = bmipmap;}	//开启MIPMAP

			//__多边形缓冲___
			_tagTriangle * m_pPolygonArray;
			DWORD m_dwPolygonArrayUsage;

			void CreatePolgyonArray(void);
			void DestroyPolygonArray(void);

			//____SHADERGROUP管理____
			ShaderGroup * m_pShaderGroupArray[MAX_SHADER_GROUP_ARRAY_SIZE];
			DWORD m_dwShaderGroupArrayUsage;

			//----Add By 彭谦----//
			//优化渲染
			list<ShaderGroup::_tagShader* > m_shaderalphatestnonelist;
			list<ShaderGroup::_tagShader* > m_shaderalphatesttransparentlist;
			list<ShaderGroup::_tagShader* > m_shaderalphablendaddtivelist;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalist;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalowlist;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalow1list;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalow2list;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalow3list;
			list<ShaderGroup::_tagShader* > m_shaderalphablendalphalow4list;
			list<ShaderGroup::_tagShader* > m_shadertransparentlist;

			//----Add By 彭谦----//
			//GPU优化渲染
			list<RendMesh* > m_RendMeshalphatestnonelist;
			list<RendMesh* > m_RendMeshalphatesttransparentlist;
			list<RendMesh* > m_RendMeshalphablendaddtivelist;
			list<RendMesh* > m_RendMeshalphablendalphalist;
			list<RendMesh* > m_RendMeshalphablendalphalowlist;
			list<RendMesh* > m_RendMeshalphablendalphalow1list;
			list<RendMesh* > m_RendMeshalphablendalphalow2list;
			list<RendMesh* > m_RendMeshalphablendalphalow3list;
			list<RendMesh* > m_RendMeshalphablendalphalow4list;
			list<RendMesh* > m_RendMeshtransparentlist;
			void RendAlphaTestlist();
			void RendAlphaBlendlist();
			void RendAlphaBlendlowlist();
			//----Add By 彭谦----//
			//优化渲染

			//渲染层属性
			DWORD				 m_dwFillMode;				//填充模式
			D3DTEXTUREFILTERTYPE m_eFilterType;				//纹理过滤类型
		public:
			Layer3D(void);
			~Layer3D(void);

			//属性接口
			void  SetFillMode(DWORD dwFillMode) { m_dwFillMode = dwFillMode;}
			DWORD GetTriangleArrayUsage() const { return m_dwPolygonArrayUsage;}

			BOOL Create(const render::Interface::_tagSetting & tSetting);				//创建3D渲染层
			void Destroy();

			DWORD CreateShaderGroup(const char * pszString,const char *pszTexPath,DWORD dwTextureProcision=0);
			DWORD CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision=0);
			DWORD CreateShaderGroupFromFile(const char *pszFileName,DWORD dwTextureProcision=0);
			void  DestroyShaderGroup(DWORD dwSGHandle);
			render::Interface::Layer3D::ShaderGroup::_tagShader * CreateDynamicShader(DWORD dwShaderGroupHandle, DWORD dwSrcShaderIndex, const render::Interface::Layer3D::_tagShaderState * pNewShaderState);			//创建动态着色变量,并将其添加到对应的纹理映射表中

			ShaderGroup * GetShaderGroup(DWORD dwShaderGroup)
			{
				if(dwShaderGroup < MAX_SHADER_GROUP_ARRAY_SIZE)
					return m_pShaderGroupArray[dwShaderGroup];
				return NULL;
			}

			void Dump(void);
			void Render(void);			//3D层统一渲染函数
			void RenderTest(void);		//3D层TEST统一渲染函数
			void RenderBlendLow(void);	//3D层BLENDLOW统一渲染函数
			void RenderBlend(void);		//3D层BLEND统一渲染函数
			void EndEffectsState();

			DWORD GetFillMode() { return m_dwFillMode;}
			//渲染三角形组,状态,纹理都需自己设置.tex1表示用2层纹理渲染
			void DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,bool btex01 = false);
			void DrawTriangleStrips(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNor, const D3DCOLOR * pCol, const D3DXVECTOR2 * pUV2);			//绘制三角形Strips
			void DrawTriangleFan(DWORD dwNumVertex,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNor, const D3DCOLOR * pCol, const D3DXVECTOR2 * pUV2);		//绘制三角形扇

			//渲染三角形组,状态,纹理都需自己设置
			void DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos);
			//专门为GPU渲染投影纹理的地形用的接口
			void DrawTriangleArray(DWORD dwCount,const D3DXVECTOR3 *pPos,D3DXMATRIX matpos,D3DXMATRIX matposproj);
			//直接渲染，只画顶点和颜色，状态需要自己设置
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos);
			//直接渲染，只画顶点和颜色，状态需要自己设置
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,D3DXMATRIX matpos,D3DXMATRIX matposproj);
			//直接渲染，只使用纹理，状态需要自己设置
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor);
			//专门为GPU渲染投影纹理的地形用的接口
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,D3DXMATRIX matpos,D3DXMATRIX matposproj);
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,WORD* pindex,D3DXMATRIX matpos,D3DXMATRIX matposproj);
			//直接渲染，只使用纹理，状态需要自己设置
			void DrawTriangleArray(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR3 *pNormal,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor);
			//根据Shader的配置来设置状态渲染
			void DrawTriangleArrayByShader(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,DWORD dwIndexcount,const D3DXVECTOR3 *pPos,const D3DXVECTOR3 *pNormal,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColor,const WORD* pindexbuffer,BOOL blight,DWORD dwMode = 0);
			//送入缓冲，使用SHADER属性渲染
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DCOLOR *pColor,DWORD dwMode = 0);
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,DWORD dwMode = 0);
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const D3DXVECTOR3 *pPos,const D3DXVECTOR2 *pUV0,const D3DCOLOR *pColorDiffuse,const D3DCOLOR *pColorSpecular,DWORD dwMode = 0);
			void SendTriangleToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwCount,const _tagTriangle* ptriangle,DWORD dwMode = 0);
			void SendBufferToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwPNCCount,DWORD dwIndexcount,const D3DXVECTOR3* pPos,const D3DXVECTOR3* pNormal,const D3DCOLOR* pDiffuse, const D3DXVECTOR2* pUV0,const WORD* pindexbuffer,BOOL blight,DWORD dwMode = 0);
			render::Interface::Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer* SendDynamicBufferToCache(DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwPNCCount,DWORD dwIndexcount,const D3DXVECTOR3* pPos,const D3DXVECTOR3* pNormal,const D3DCOLOR* pDiffuse,const D3DXVECTOR2* pUV0,const WORD* pindexbuffer,BOOL blight,DWORD dwMode = 0);
			//void SetLightEnable(DWORD dwShaderGroup,DWORD dwShaderIndex,bool enable);
			void SendTriangleToCacheEx(render::Interface::Layer3D::ShaderGroup::_tagShader * pDynamicShader, DWORD dwCount, const _tagTriangle* ptriangle, DWORD dwMode = 0);			//将需要渲染的三角形送入缓冲,并记录在指定的动态着色变量中
			void GetTagBuffer(Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer*& pbuffer,DWORD& dwrendtype,DWORD& dwstreamtype,DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwPNCCount,BOOL blight,DWORD dwMode = 0);
			void GetDynamicTagBuffer(Layer3D::ShaderGroup::_tagShader::RendBuffer::_tagBuffer*& pbuffer,DWORD& dwrendtype,DWORD& dwstreamtype,DWORD dwShaderGroup,DWORD dwShaderIndex,DWORD dwPNCCount,BOOL blight,DWORD dwMode = 0);
			void SetStreamSource(DWORD dwStreamType);
			void SetShaderTempStreamType(DWORD dwShaderGroupHandle,DWORD dwShaderIndex, DWORD type);
			void SetColorAndAlphaOp(DWORD dwcolorop,DWORD dwcolorarg1,DWORD dwcolorarg2,DWORD dwalphaop,DWORD dwalphaarg1,DWORD dwalphaarg2,bool init = false);
			void SetVertexDecla(DWORD dwdecl,bool init = false);

			//调试和编辑使用
			void _DEBUG_DrawLine(const D3DXVECTOR3 pVtx[],D3DCOLOR dwColor);
			void _DEBUG_DrawLine(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,D3DCOLOR dwColor);
			void _DEBUG_DrawTriangle(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,D3DCOLOR dwColor);		//直接画一个三角形
			void _DEBUG_DrawTriangle(DWORD dwTexturHandle,const D3DXVECTOR3 *pVtx,const D3DXVECTOR2 *pUV0,D3DCOLOR dwColor);
			void _DEBUG_DrawWireBox(const D3DXVECTOR3 *pCenter,float fRadius,D3DCOLOR dwColor);//直接画一个BOX
			void _DEBUG_DrawWireBox(const  D3DXVECTOR3 *pMin,const  D3DXVECTOR3 *pMax,D3DCOLOR dwColor);
			void _DEBUG_DrawWireQuad(const D3DXVECTOR3 *pVtx0,const D3DXVECTOR3 *pVtx1,const D3DXVECTOR3 *pVtx2,const D3DXVECTOR3 *pVtx3,D3DCOLOR dwColor);
			void _DEBUG_DrawWireQuad(const D3DXVECTOR3 *pVtx0,DWORD dwLineCount,D3DCOLOR dwColor);

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
			// modify by lpf 2007-11-27
			char szFonts[MAX_RENDER_LAYER2D_FONT_SIZE][_MAX_PATH];

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
			BOOL b2DLinear;			//2D纹理线性过滤
			BOOL b3DLinear;			//3D纹理线性过滤
			BOOL bMipMapping;

			//垂直同步
			BOOL bPresentationInterval;//ONE | DEFAULT | IMMEDIATE;

			bool bborderaddress;
			DWORD dwMaxTexStage;
			bool bgpuprocess;
			DWORD dwshaderverison;
			int   ihvpmatrixcount;
			int   isvpmatrixcount;
		}m_tSetting;

		//纹理缓冲
		struct tagTextureInfo:public RCObject,public LoadState
		{
			//文件名
			char pszFileName[MAX_PATH];
			//标示是否被分配
			BOOL bUse;
			//纹理句柄
			Texture *pTexture;
		};
	private:

		//初始化后状态表

		//INTERFACE & DEVICE
		LPDIRECT3D9		   m_pD3D;			
		LPDIRECT3DDEVICE9  m_pD3DDevice; 
		BOOL               m_bDeviceLost;
		BOOL			   m_bAlreadyBeginScene;
		tagTextureInfo m_Textures[MAX_TEXTURE_CACHE_SIZE];

		DWORD	  m_dwTextureMemorySize;
		DWORD	  m_dwTextureCnt;

		typedef hash_map<string,DWORD> HMStringDWORD;
		typedef hash_map<string,DWORD>::iterator itHMsw;
		HMStringDWORD m_TextureNameToID;

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
		BOOL              GetDeviceLost(void) { return m_bDeviceLost;}
		void              SetDeviceLost(BOOL blost) { m_bDeviceLost = blost;}

		BOOL BeginScene(void);
		void EndScene(void);  

		_tagSetting * GetSetting() { return &m_tSetting;} 
		_tagInfo * GetInfo() { return &m_stInfo;}

		void Clear(D3DCOLOR d3dColor);

		//__TEXTURE__
		//根据文件名产生一个纹理ID
		DWORD  AcquireTexture(const char* pszFileName);
		void ReleaseTexture(DWORD dwTextureID);

		tagTextureInfo* GetTextureInfo(DWORD dwID);
		Texture *CreateTexture(CRFile* pFile,DWORD dwTextureProcision=0);

		DWORD  CreateTexture(const char * pszFileName,DWORD dwTextureProcision=0);
		DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap);
		DWORD  CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwColor);
		void   DestroyTexture(DWORD dwTextureHandle);
		void   DestroyAllTexture(void);
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
		BOOL ResetRelease();
		BOOL ResetRebuild();
		D3DFORMAT CheckDepthStencilFormat(D3DFORMAT dwBackBufferFormat);
	};
};

typedef render::Interface::Layer2D::_tagFontInfo FONTINFO, * PFONTINFO;			//add by lpf 2009-01-07