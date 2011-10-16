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

描述: 角色动画支持模块
作者: 贾涛
建立: 2003/10/28
修改: 2003/10/31
修改: 2005/4/14
*/

#pragma once

#include "utility.h"
#include "render.h"

class model
{
public:
	struct _tagShader;

	class AnimVisibility;
	class AnimTexture;
	class AnimJoint;
	class AnimUV;
	class AnimColor;
	class BindPose;
	class Mesh;

	class AnimJoint
	{
	public:
		struct tagFrame
		{
			D3DXVECTOR3 vScale;
			D3DXQUATERNION qRot;
			D3DXVECTOR3 vTranslation;
		};

		struct tagJoint
		{
			char szName[128];
			tagFrame * pFrameArray;
		}; 
	private:
		// 固定数据
		DWORD	  m_dwFrameCount;
		DWORD	  m_dwTimeLength;	//动画总时间
		DWORD	  m_dwJointCount;
		tagJoint *m_pJointArray;

	public:
		AnimJoint();
		~AnimJoint();

		DWORD GetTimeLength(float fSpeedRate = 1.0f) const
		{
			if(fSpeedRate == 0)
				return m_dwTimeLength;
			return static_cast<DWORD>(m_dwTimeLength*fSpeedRate);
		}	//得到动画总时间
		void  SetTimeLength(DWORD dwTimeLength)		{ m_dwTimeLength = dwTimeLength; }
		//得到总帧数
		DWORD GetFrameCount(void)	const {	return m_dwFrameCount;}
		//得到骨骼数
		const DWORD GetJointCount(void)   {return m_dwJointCount;}
		const tagJoint * GetJointArray()	const { return m_pJointArray;}

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile *pFile);

		void Destroy();

		void _DEBUG_Render(const D3DXMATRIX *pWorldMatrix,DWORD dwTimeElapse);
	};


	//渲染缓冲
	class Buffer
	{
		const Mesh  *m_pAttachMesh;

		//变形数据缓冲
		D3DXVECTOR3 *m_pPosBuffer;
		D3DXVECTOR3 *m_pNmlBuffer;
		D3DXVECTOR2 *m_pUV0Buffer;
		D3DCOLOR	*m_pColBufferDiffuse;//颜色缓冲
		//D3DCOLOR	*m_pColBufferSpecular;//颜色缓冲
		WORD* m_pIndexBuffer;
		D3DXMATRIX  *m_pJointMatrixBufferForVertex;
		D3DXMATRIX  *m_pJointMatrixBufferForNormals;

		//D3DMATERIAL9 m_mtrl;
		//D3DMATERIAL9* m_pregionmtrl;
		bool  m_blight;
		bool  m_busenormal;
		bool  m_bsrendstatic;
		bool  m_bswprocess;
		bool  m_bbuffcaculated;
		bool  m_bGpuCaculate;
		bool  m_bnoanim;
		bool  m_buseenvtex;
		bool  m_buseuvanim;
		bool  m_bbillboard;
		bool  m_bbillboardy;
		bool  m_bNormalTransScal;
		bool  m_brendtextureprojcted;

		D3DXMATRIX m_matworld;
		D3DXMATRIX m_matViewProjective;
		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProjective;
		D3DXMATRIX m_matrixforuse;
		D3DXMATRIX m_matrixforbb;
		D3DXMATRIX m_matfortp;
		//DWORD m_dwAmbient;
		//DWORD m_dwRegionAmbient;
		DWORD m_dwPNCCount;
		DWORD m_dwUV0Count;
		DWORD m_dwBaseColor;
		DWORD m_dwMultiColor;
		//DWORD m_dwUV1Count;

	public:
		//渲染
		void Render(DWORD dwShadeGroupHandle,DWORD dwMode);
		void Render(DWORD dwShadeGroupHandle,DWORD dwShaderIndex,DWORD dwMode);
		void RenderTransparent(DWORD dwShadeGroupHandle,DWORD dwMode);				//渲染透明模型
		void RenderModelFlash(DWORD dwShadeGroupHandle,DWORD dwMode);
		void RenderByShaderState(DWORD dwShaderGroupHandle, DWORD dwMode, const render::Interface::Layer3D::_tagShaderState * pShaderState);		//使用动态着色变量进行渲染
		void RenderStatic(DWORD dwShaderGroupHandle, DWORD dwMode);


		void Draw(DWORD dwShadeGroupHandle,DWORD dwMode = 0);
		void DrawWithoutState(DWORD dwShadeGroupHandle);
		void DrawWithoutStateAndTexture(DWORD dwShadeGroupHandle);
		void DrawTransparent(DWORD dwShaderGroupHandle);
		void DrawByColorOp(DWORD dwShaderGroupHandle, DWORD dwMode, DWORD dwColorOp, DWORD dwColorArg1, DWORD dwColorArg2);			//使用颜色操作直接绘制模型
		void _DEBUG_RenderNormal();



		//访问缓冲指针
		//		void SetLightEffect(DWORD dwShaderGroupHandle,bool blight);
		D3DXVECTOR3 * GetPosBuffer() const { return m_pPosBuffer;}
		D3DXMATRIX  * GetJointMatrixBufferForVertex() const { return m_pJointMatrixBufferForVertex;}
		D3DXMATRIX  * GetJointMatrixBufferForNormals() const { return m_pJointMatrixBufferForNormals;}
		D3DXVECTOR3 * GetNmlBuffer() const { return m_pNmlBuffer;}
		D3DXVECTOR2 * GetUV0Buffer() const { return m_pUV0Buffer;}
		WORD        * GetIndexBuffer() const { return m_pIndexBuffer;}
		/*D3DMATERIAL9& GetMaterial()  { return  m_mtrl;}*/
		void          SetUseNormal(bool usenormal){m_busenormal = usenormal;}
		bool          GetUseNormal(){return m_busenormal;}
		void          SetMatWorld(const D3DXMATRIX* matworld){m_matworld = *matworld;}
		D3DXMATRIX*   GetMatWorld(){return &m_matworld;}
		void          SetMatViewProjective(const D3DXMATRIX* mat){m_matViewProjective = *mat;}
		D3DXMATRIX*   GetMatViewProjective(){return &m_matViewProjective;}
		void          SetMatView(const D3DXMATRIX* mat){m_matView = *mat;}
		D3DXMATRIX*   GetMatView(){return &m_matView;}
		void          SetMatProjective(const D3DXMATRIX* mat){m_matProjective = *mat;}
		D3DXMATRIX*   GetMatProjective(){return &m_matProjective;}
		void          SetMatForUse(const D3DXMATRIX* mat){m_matrixforuse = *mat;}
		D3DXMATRIX*   GetMatForUse(){return &m_matrixforuse;}
		void          SetMatForBB(const D3DXMATRIX* mat){m_matrixforbb = *mat;}
		D3DXMATRIX*   GetMatForBB(){return &m_matrixforbb;}
		void          SetMatFotTP(const D3DXMATRIX* mat){m_matfortp = *mat;}
		D3DXMATRIX*   GetMatFotTP(){return &m_matfortp;}
		void          SetRendStatic(bool brendstatic){m_bsrendstatic = brendstatic;}
		bool          GetRendStatic(){return m_bsrendstatic;}
		void          SetLightEnable(bool blight){m_blight = blight;}
		bool          GetLightEnable(){return m_blight;}
		void          SetGpuCaculate(bool b){m_bGpuCaculate = b;}
		bool          GetGpuCaculate(){return m_bGpuCaculate;}
		void          SetNoAnim(bool b){m_bnoanim = b;}
		bool          GetNoAnim(){return m_bnoanim;}
		void          SetUseEnvTexture(bool b){m_buseenvtex = b;}
		bool          GetUseEnvTexture(){return m_buseenvtex;}
		void          SetUseUVAnim(bool b){m_buseuvanim = b;}
		bool          GetUseUVAnim(){return m_buseuvanim;}
		void          SetIsBillBoard(bool b){m_bbillboard = b;}
		bool          GetIsBillBoard(){return m_bbillboard;}
		void          SetIsBillBoardY(bool b){m_bbillboardy = b;}
		bool          GetIsBillBoardY(){return m_bbillboardy;}
		void          SetNormalTransScal(bool b){m_bNormalTransScal = b;}
		bool          GetNormalTransScal(){return m_bNormalTransScal;}
		void          SetTextureProjected(bool b){m_brendtextureprojcted = b;}
		bool          GetTextureProjected(){return m_brendtextureprojcted;}
		void          SetBufferCaculate(bool bcaculate){m_bbuffcaculated = bcaculate;}
		bool          GetBufferCaculate(){return m_bbuffcaculated;}
		bool          GetIsSWProcess(){return m_bswprocess;}
		/*void          SetRegionAmbient(DWORD dwcolor)  { m_dwRegionAmbient = dwcolor;}
		DWORD         GetRegionAmbient(){return m_dwRegionAmbient;}*/
		void         SetBaseColor(DWORD dwcolor){ m_dwBaseColor = dwcolor;}
		DWORD&        GetBaseColor(){return m_dwBaseColor;}
		void         SetMultiColor(DWORD dwcolor){ m_dwMultiColor = dwcolor;}
		DWORD&        GetMultiColor(){return m_dwMultiColor;}
		void         SetTempStreamType(DWORD dwShaderGroupHandle,DWORD type = -1);
		D3DCOLOR    * GetColBufferDiffuse() const { return m_pColBufferDiffuse;}
		//D3DCOLOR    * GetColBufferSpecular() const { return m_pColBufferSpecular;}
		void MultiAlphaToColor();

		inline DWORD GetPNCCount() const { return m_dwPNCCount;}
		inline DWORD GetUV0Count() const { return m_dwUV0Count;}
		//inline DWORD GetUV1Count() const { return m_dwUV1Count;}

		const Mesh  *GetAttachMesh() const { return m_pAttachMesh;}

		Buffer();
		~Buffer();
		//建立和销毁
		void Create(const Mesh *pMesh);
		void Destroy();
	};

	//基本模型
	class Mesh
	{
		friend Buffer;
		render::BoundingBox *m_pBoundingBox;

		//基本数据---
		//顶点表,法向表,颜色表
		D3DXVECTOR3 * m_pPosBuffer;
		D3DXVECTOR3 * m_pNmlBuffer;
		D3DCOLOR    * m_pColBufferDiffuse;
		D3DXVECTOR2 * m_pUV0Buffer;
		DWORD       * m_pdwJointIndexBuffer;
		WORD        * m_pwIndexBuffer;
		DWORD m_dwPNCCount;
		DWORD m_dwIndexCount;
		//纹理0表
		DWORD m_dwUV0Count;
		DWORD m_dwShaderIndex;
		DWORD m_dwGroupCount;
		D3DXVECTOR3 m_vAveragepoint;

	public:		
		struct tagGroup
		{
			~tagGroup()
			{
				mapsourceeffectindex.clear();
				listeffectindex.clear();
			}
			DWORD dwJointIndex;
			DWORD dwPNCIndexStart;
			DWORD dwPNCIndexCount;
			list<WORD> listeffectindex;
			hash_map<WORD,std::list<WORD>> mapsourceeffectindex;

		};



	public:
		struct tagTriangle
		{                       		
			DWORD dwPNCIndex[3];
			DWORD dwUV0Index[3];
			//DWORD dwUV1Index[3];
		};
	private:
		tagGroup * m_pGroupArray;
		//三角形
		tagTriangle * m_pTriangleBuffer;
		DWORD m_dwTriangleCount;

		void GetAnimFrameByTime(const model::AnimJoint *pAnimJoint,DWORD dwJointIndex,D3DXVECTOR3 *pTrs,D3DXVECTOR3 *pScl,D3DXQUATERNION *pRot,DWORD dwTimeElapse,float fAnimCustomSpeedRate);			//根据时间得到动画帧

	public:
		//接口
		D3DXVECTOR3 * GetPosBuffer() const { return m_pPosBuffer;}
		D3DXVECTOR3 * GetNmlBuffer() const { return m_pNmlBuffer;}
		D3DCOLOR    * GetColBufferDiffuse() const { return m_pColBufferDiffuse;}
		D3DXVECTOR2 * GetUV0Buffer() const { return m_pUV0Buffer;}
		DWORD       * GetJointIndexBuffer() const { return m_pdwJointIndexBuffer;}
		WORD        * GetIndexBuffer() const { return m_pwIndexBuffer;}
		tagTriangle * GetTriangleBuffer() const { return m_pTriangleBuffer;}
		DWORD		  GetShaderIndex()    const { return m_dwShaderIndex;}
		const tagGroup * GetGroupArray() const  { return m_pGroupArray;}
		render::BoundingBox * GetBoundingBox() const { return m_pBoundingBox;}

		DWORD GetPNCCount() const { return m_dwPNCCount;}
		D3DXVECTOR3 GetAveragePoint() const { return m_vAveragepoint;}
		DWORD GetIndexCount() const { return m_dwIndexCount;}
		DWORD GetUV0Count() const { return m_dwUV0Count;}
		//DWORD GetUV1Count() const { return m_dwUV1Count;}
		DWORD GetTriangleCount() const { return m_dwTriangleCount;}
		DWORD GetGroupCount() const {return m_dwGroupCount;}

		void CopyPosition(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix);
		void CopyColor(model::Buffer *pBuffer);
		void CopyUV(model::Buffer *pBuffer);
		void CalculateVertexNormal();

		void ProcessAnimJointByFrame(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,DWORD dwFrame);
		
		void ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                  const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
							  DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate);													//处理骨骼动画信息
		void ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                  const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
							  DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,D3DXMATRIX& matrixscal);								//处理骨骼动画信息
		void ProcessAnimJoint(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                  const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
							  DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel);	//处理骨骼动画信息
		//nikai 2009 11 30 自定义骨骼
		void ProcessAnimJointSelf(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
			DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,const D3DXVECTOR3 *pVec3Joint);
		void ProcessAnimJointSelf(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJoint,
			DWORD dwAnimTimeElapse,float fAnimCustomSpeedRate,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel,const D3DXVECTOR3 *pVec3Joint);
		
		void ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                               const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,
										   DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,
										   float fAnimCustomSpeedRateB,float fRatio);													//处理骨骼动画插值
		void ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                               const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,
										   DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,
										   float fAnimCustomSpeedRateB,float fRatio,D3DXMATRIX& matrixscal);								//处理骨骼动画插值
		void ProcessAnimJointInterpolation(model::Buffer *pBuffer,const D3DXMATRIX *pWorldMatrix,const D3DXMATRIX * pViewMatrix,
			                               const D3DXMATRIX * pProjectedMatrix,model::BindPose *pBindPose,AnimJoint *pAnimJointA,
										   DWORD dwTimeElapseA,float fAnimCustomSpeedRateA,AnimJoint *pAnimJointB,DWORD dwTimeElapseB,
										   float fAnimCustomSpeedRateB,float fRatio,D3DXMATRIX& matrixworld,D3DXMATRIX& matrixmodel);	//处理骨骼动画插值

		void ProcessAnimTexture(model::Buffer *pBuffer,const AnimTexture *pAnimTexture,DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessAnimVisibility(model::Buffer *pBuffer,const AnimVisibility *pAnimVisibility,DWORD dwTimeElapse,DWORD dwTimeCircle,bool blendalpha = false);
		void ProcessAnimColor(model::Buffer *pBuffer,const AnimColor *pAnimColor,DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessAnimUV(model::Buffer *pBuffer,const AnimUV *pAnimUV,DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessEnvironmentMaps(model::Buffer *pBuffer,const D3DXMATRIX *pViewMatrix,bool bnormalize = false);

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);
		void Destroy();        		

		void _DEBUG_Render();

		Mesh();
		~Mesh();
	};

	class BindPose
	{
		char m_strFileName[MAX_PATH];
		DWORD	    m_dwJointCount;
		D3DXMATRIX *m_pBindPoseArray;
	public:
		BindPose();
		~BindPose();

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);

		void SetFileName(const char *pszFileName);
		void Destroy();

		const D3DXMATRIX *GetBindPoseArray() { return m_pBindPoseArray;}
		const char* GetFileName()	{return m_strFileName;}
	};
	class AnimVisibility:public RCObject,public LoadState
	{
	public:

	private:
		float    * m_pFrameArray;
		//固定数据
		DWORD	  m_dwFrameCount;

	public:
		AnimVisibility();
		~AnimVisibility();

		DWORD GetFrameCount(void)	const {	return m_dwFrameCount;}
		float * GetFrameArray() const { return m_pFrameArray;}

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);

		void Destroy();
	};

	class AnimColor:public RCObject,public LoadState
	{
	public:

	private:
		DWORD     * m_pColorArray;
		DWORD	  m_dwFrameCount;

	public:
		AnimColor();
		~AnimColor();

		DWORD   GetFrameCount(void)	const {	return m_dwFrameCount;}
		DWORD * GetColorArray() const { return m_pColorArray;}
		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);
		void Destroy();

	};

	class AnimUV:public RCObject,public LoadState
	{
		float m_fUOffset,m_fVOffset;
		int m_iroatdir,m_iroatspeed;
		//当前数据存取

	public:
		AnimUV();
		~AnimUV();

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);
		void Destroy();

		float GetU() const { return m_fUOffset;}
		float GetV() const { return m_fVOffset;}
		int Getroatdir() const { return m_iroatdir;}
		int Getroatspeed() const { return m_iroatspeed;}
	};
	class AnimTexture:public RCObject,public LoadState
	{
		DWORD m_dwRows,m_dwColumns;

	public:
		AnimTexture();
		~AnimTexture();

		BOOL Create(const char *pszFileName);
		BOOL Create(CRFile* pFile);
		void Destroy();

		DWORD GetRows()		   const { return m_dwRows;}
		DWORD GetColumns()     const { return m_dwColumns;}
	};
};
struct POSUV
{
	DWORD posindex;
	DWORD uvindex;
	bool operator == (const POSUV& posuv) const
	{
		return (posindex == posuv.posindex && uvindex != posuv.uvindex);
	}
};
struct EXPOSUV
{
	DWORD posindex;
	DWORD uvindex;
	DWORD bufferindex;
	bool operator == (const EXPOSUV& posuv) const
	{
		return (posindex == posuv.posindex && uvindex == posuv.uvindex);
	}
};
struct INDEX 
{
	bool operator == (const INDEX& index) const
	{
		return posindex == index.posindex;
	}
	DWORD posindex;
	DWORD sourceindex;
};