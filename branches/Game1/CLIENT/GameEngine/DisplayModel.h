//=============================================================================
/**
*  file: DisplayModel.h
*
*  Brief:包含一个具体需要显示模型的资源
*
*  Authtor:wangqiao
*	
*	Datae:2007-10-25
*/
//=============================================================================
#ifndef __DISPLAY_MODEL__
#define __DISPLAY_MODEL__

#pragma once

#include "GameModel.h"

class CGameModelManager;

class AnimInfo
{ 
public:
	struct tagActionInfo
	{
		friend class CDisplayModel;

	public:
		DWORD dwCurActionStartTime;			//开始时间
		DWORD dwCurActionLoopCount;			//动作循环计数
		DWORD dwCurAction;					//当前动作编号
		DWORD dwCurActionElapseTime;		//当前动作播放时间
		float fCurActionSpeedRate;			//自定义当前动作速率
		BOOL  bCurActionLooped;				//是否循环标志(不循环将停止在最后一帧)

		BOOL  bInterpolation;		    	//内插标志
		bool  bStayToLastFrame;				//停留在最后一帧

		DWORD dwInterpolationElapseTime;	//流失时间
		DWORD dwInterpolationStartTime;		//开始时间
		DWORD dwInterpolationTimeLength;	

		DWORD dwOldAction;					//上一个动作
		DWORD dwOldActionTimeElapse;		//上一个动作停止的时间
		float fOldActionSpeedRate;			//上一个动作的速率
	}m_tActionInfo;

private:
	friend CDisplayModel;

	//当前的渲染数据
	D3DXMATRIX m_matWorld;
	float m_fDirY;			
	float m_fAlpha;
	D3DXMATRIX m_matview;
	D3DXMATRIX m_matproj;
	D3DXMATRIX m_matscal;
	D3DXMATRIX m_matfortp;

	D3DXMATRIX m_matBillboard;
	D3DXMATRIX m_matBillboardY;	   

	//处理数据,动作动画时间
	DWORD m_dwCurrentTime;		//当前时间

	//粒子发射器时间
	DWORD m_dwParticleEmitterLastTimeArray[GAMEMODEL_MAXPARTICLEEMITTER];

	//拖尾发射器坐标列表
	GameModel::TrailContainer * m_pTrailContainerArray;

	//骨骼坐标列表
	D3DXVECTOR3  m_vec3JointArrayPos[3];

	//是否处理骨骼位置变换
	bool						m_bIsProcessSelfAnim;

	//全局动画时间表
	DWORD m_dwtextureprojectivetime,m_dwAnimColorStartTime,m_dwAnimUVStartTime,m_dwAnimVisibilityStartTime,m_dwAnimTextureStartTime;
	DWORD m_dwAnimWaterStartTime;

public:
	tagActionInfo * GetActionInfo() { return &m_tActionInfo;}
	const tagActionInfo * GetActionInfo() const { return &m_tActionInfo;}

	inline const D3DXMATRIX * GetWorldMatrix() const { return &m_matWorld;}
	inline const D3DXMATRIX * GetViewMatrix() const { return &m_matview;}
	inline const D3DXMATRIX * GetProjectedMatrix() const { return &m_matproj;}
	inline const D3DXMATRIX * GetMatrixForTP() const { return &m_matfortp;}
	inline const D3DXMATRIX * GetScalMatrix() const { return &m_matscal;}
	inline const D3DXMATRIX * GetBillboard()   const { return &m_matBillboard;}
	inline const D3DXMATRIX * GetBillboardY()    const { return &m_matBillboardY;}
	inline const float GetDirY() const {return m_fDirY;}
	inline const float GetTextureProjectiveAlpha() const {return m_fAlpha;}

	inline void SetWorldMatrix(const D3DXMATRIX * pWorldMatrix)
	{ 
		m_matWorld = *pWorldMatrix;
		//memcpy(&m_matWorld,pWorldMatrix,sizeof(D3DXMATRIX));
	}
	inline void SetViewMatrix(const D3DXMATRIX * pViewMatrix)
	{ 
		m_matview = *pViewMatrix;
		//memcpy(&m_matview,pViewMatrix,sizeof(D3DXMATRIX));
	}
	inline void SetScalMatrix(const D3DXMATRIX * pScalMatrix)
	{ 
		m_matscal = *pScalMatrix;
		/*if (&m_matscal != pScalMatrix)
		{
		memcpy(&m_matscal,pScalMatrix,sizeof(D3DXMATRIX));
		}*/

	}
	inline void SetMatrixForTP(const D3DXMATRIX * matfortp)
	{ 
		m_matfortp = *matfortp;
		/*if (&m_matscal != pScalMatrix)
		{
		memcpy(&m_matscal,pScalMatrix,sizeof(D3DXMATRIX));
		}*/

	}
	inline void SetProjectedMatrix(const D3DXMATRIX * pProjMatrix)
	{ 
		m_matproj = *pProjMatrix;
		/*if (&m_matscal != pScalMatrix)
		{
		memcpy(&m_matscal,pScalMatrix,sizeof(D3DXMATRIX));
		}*/

	}
	inline void SetBillboard(const D3DXMATRIX * pBillboardMatrix)
	{ 
		m_matBillboard = *pBillboardMatrix;
		//memcpy(&m_matBillboard,pBillboardMatrix,sizeof(D3DXMATRIX));
	}
	inline void SetBillboardY(const D3DXMATRIX * pBillboardMatrixY)
	{ 
		m_matBillboardY = *pBillboardMatrixY;
		//memcpy(&m_matBillboardY,pBillboardMatrixY,sizeof(D3DXMATRIX));
	}
	inline void SetDirY(float fy){m_fDirY = fy;}
	inline void SetTextureProjectiveAlpha(float fa){m_fAlpha = fa;}

	void SetCurrentTime(DWORD dwCurrentTime) { m_dwCurrentTime = dwCurrentTime;}
	DWORD GetCurrentTime() const { return m_dwCurrentTime;}
	DWORD GetTextureProjectiveTime()const {return m_dwtextureprojectivetime;}
	void  SetTextureProjectiveTime(DWORD time){m_dwtextureprojectivetime = time;}
	void SetAnimTime(DWORD dwTime)
	{
		m_dwtextureprojectivetime = 
			m_dwAnimColorStartTime = 
			m_dwAnimTextureStartTime = 
			m_dwAnimUVStartTime = 
			m_dwAnimVisibilityStartTime =
			m_dwAnimWaterStartTime = dwTime;
	}
	AnimInfo();
	~AnimInfo();
	void ClearTrailContainer(void);
	void DeleteTrailContainer(void);
	void SetTrailContainerArray(GameModel::TrailContainer * p)	{ m_pTrailContainerArray = p; }
	DWORD* GetParticleEmitterLastTimeArray()		{ return m_dwParticleEmitterLastTimeArray; }
	void SetupAnimInfo(float fX, float fY, float fH, float fDir, render::Camera * pCamera);
	void SetupAnimInfoRotMatrix(float fX, float fY, float fH, D3DXMATRIX mat3DDir, render::Camera * pCamera);
	void SetCurActionSpeedRate(float fSpeedRate = 1.0f)		{ m_tActionInfo.fCurActionSpeedRate = fSpeedRate; }			//设定当前动作速率
	void CaculateJointPos(D3DXVECTOR3 *vecSrcPos, D3DXVECTOR3 *vecTargetPos, DWORD dwjointCount);
	bool IsSelfAnimJoint() const	{return	m_bIsProcessSelfAnim;}
	void EnableProcessSelfAnimJoint(bool b)	{m_bIsProcessSelfAnim = b;}
	const D3DXVECTOR3 *GetSelfJointArrayPos()	const			{return m_vec3JointArrayPos;};
};


class CDisplayModel
{
public:
	CDisplayModel(CGameModelManager* pGameModelManager);
	virtual ~CDisplayModel(void);
private:
	//gamemodel管理器
	CGameModelManager* m_pGameModelManager;
	//ID
	DWORD m_dwModelID;
	//GameModel ID
	DWORD m_dwGameModelID;
	//是否显示所有Group,并选择缺省的Part和Lvl
	BOOL  m_bAllGroup;
	DWORD m_dwAnimColorCircle;
	DWORD m_dwAnimUVCircle;
	DWORD m_dwAnimVisibilityCircle;
	DWORD m_dwAnimTextureCircle;
public:
	class CDisplayGroup
	{
	private:
		DWORD m_dwPartName;//Part名称
		DWORD m_dwPartLevel;//Part等级		

		//等级信息
		GameModel::Group::Part::Level *m_pLevel;
	public:
		CDisplayGroup(void);
		~CDisplayGroup(void);

		void SetPartNameLevel(DWORD dwPart,DWORD dwLevel){m_dwPartName=dwPart;m_dwPartLevel=dwLevel;}
		DWORD GetPartName(void)	{return m_dwPartName;}
		DWORD GetPartLevel(void)	{return m_dwPartLevel;}

		void ReleaseRenderMesh();
		void ReleaseLevelInfo();

		GameModel::Group::Part::Level *GetLevelInfo(){return m_pLevel;}
		void SetLevelInfo(GameModel::Group::Part::Level *pLevel);

		//得到三角形个数
		DWORD GetTriangleCount(void);

		/*void SetRegionAmbient(DWORD dwShaderGroupHandle,DWORD dwcolor);
		void SetAmbient(DWORD dwShaderGroupHandle,DWORD dwcolor);*/
		//void SetLightEnable(DWORD dwShaderGroupHandle,bool byselflightenable = true,bool lightenable = true);
		void ForceToEnableLight(void);
		void SetDirLightEnable(bool enable);
		void SetUseNormal(bool busenormal);
		void SetColor(DWORD dwColor);
		void SetColorCompel(DWORD dwColor);			//避开模型显示组是否受灯光效果影响的配置,强制为模型显示组设定颜色
		void MultiplyColor(DWORD dwColor);
		void MultiplyAlpha(float fRatio);

		//处理动画信息
		void  ProcessAnimJoint(const AnimInfo *pAnimInfo,model::AnimJoint * pOldAnimJoint,
			model::AnimJoint *pCurAnimJoint,
			DWORD dwOldActionTimeElapse,DWORD dwCurActionTimeElapse,
			model::BindPose *pBindPose,float fRatio);
		void ProcessLevelAnimVisibility(DWORD dwShaderHandle,DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessLevelAnimTexture(DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessLevelAnimColor(DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessLevelAnimUV(DWORD dwTimeElapse,DWORD dwTimeCircle);
		void ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix,bool bnormalize = false);
		//处理点光源
		void  ProcessPointLight(const D3DXVECTOR3 * pLightPosition,
			float fIntensity, DWORD dwColor);
		//处理方向光
		void  ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,
			const D3DXVECTOR3 *pLightDirection,DWORD dwColor);
		//根据Buffer创建VECBUFFER中的颜色并且将BUFFER的值拷贝过去
		void CopyBufferToColor(std::vector<D3DCOLOR*>& vecbuffer);
		//拷贝VECBUFFER中的颜色到BUFFER中去
		void CopyColorToBuffer(std::vector<D3DCOLOR*>& vecbuffer);

		void  CalculateBillboard(const model::Mesh *pMesh,model::Buffer * pBuffer,
			const D3DXMATRIX *pBillboarMatrix,const D3DXMATRIX* pmatscal = NULL);



		void GetEffectTypeAndMacoDecla(model::Buffer *pBuffer,render::Interface::Layer3D::RendMesh* prendmesh,render::Interface::Layer3D::RendMesh::TAGMESH* ptagmesh,DWORD& dwEffectType,DWORD& dwMacoDecla);
		//渲染模型
		void Render(DWORD dwShaderGroupHandle);
		void RenderTransparent(DWORD dwShaderGroupHandle);																			//渲染透明组
		void RenderModelFlash(DWORD dwShaderGroupHandle);

		void RenderByShaderState(DWORD dwShaderGroupHandle, const render::Interface::Layer3D::_tagShaderState * pShaderState);		//使用动态着色变量进行渲染



		void DrawAlphaTestOnly(DWORD dwShaderGroupHandle);
		void DrawAlphaTestByColorOp(DWORD dwShaderGroupHandle, DWORD dwColorOp, DWORD dwColorArg1, DWORD dwColorArg2);			//alpha测试使用颜色操作直接绘制模型
		void DrawAlphaBlendOnly(DWORD dwShaderGroupHandle);
		void DrawAlphaBlendByColorOp(DWORD dwShaderGroupHandle, DWORD dwColorOp, DWORD dwColorArg1, DWORD dwColorArg2);			//alpha混合使用颜色操作直接绘制模型

		void DrawTransparent(DWORD dwShaderGroupHandle);
		void DrawWithoutState(DWORD dwShaderGroupHandle);
		void DrawWithoutStateAndTexture(DWORD dwShaderGroupHandle);


		//调试
		void  _DEBUG_RenderNormal(void);
	};	

	typedef map<DWORD,CDisplayGroup*> mapDisplayGroups;
	typedef map<DWORD,CDisplayGroup*>::iterator itDisplayGroup;

	typedef map<DWORD,GameModel::Locator*> mapLocators;
	typedef map<DWORD,GameModel::Locator*>::iterator itLocator;

	typedef map<DWORD,GameModel::TrailEmitter*> TrailEmitters;
	typedef map<DWORD,GameModel::TrailEmitter*>::iterator itTrailEmitter;

	typedef map<DWORD,BOOL>	TrailEnables;
	typedef map<DWORD,BOOL>::iterator itTrailEnable;

	typedef map<DWORD,GameModel::ParticleEmitter*> ParticleEmitters;
	typedef map<DWORD,GameModel::ParticleEmitter*>::iterator itParticleEmitter;

	typedef map<DWORD,GameModel::TextureProjective*> TextureProjectives;
	typedef map<DWORD,GameModel::TextureProjective*>::iterator itTextureProjectives;

	typedef map<DWORD,BOOL>	ParticleEnables;
	typedef map<DWORD,BOOL>::iterator itParticleEnable;

private:
	//着色变量
	DWORD m_dwShaderGroupHandle;

	//需要渲染得所哟Group
	mapDisplayGroups m_DisplayGroups;
	//定位器列表
	mapLocators m_Locators;
	//绑定位置
	model::BindPose * m_pBindPose;
	//需要的尾迹
	TrailEmitters m_TrailEmitters;
	TrailEnables m_TrailEnables;
	//对所有尾迹的操作标示(1:所有显示,0:所有不显示,-1:依照m_TrailEnables设置
	short	m_nAllTrailEnable;

	//需要显示的粒子
	ParticleEmitters m_ParticleEmitters;
	ParticleEnables m_ParticleEnables;

	BOOL m_bRendShadow;
	//投影纹理
	TextureProjectives m_TextureProjectives;

	// 水面
	Water::CWater * m_pWater;

	//对所有尾迹的操作标示(1:所有显示,0:所有不显示,-1:依照m_ParticleEnables设置
	short	m_nAllParticleEnable;
	//该模型的范围盒
	render::BoundingBox *m_pPickBox;
	//该模型的选择盒
	render::BoundingBox *m_pSizeBox;
	//该模型的范围Mesh
	GameModel::PickMesh *m_pPickMesh;
	bool m_brendsimplemesh;

private:
	void SetAllParticleEmitter(BOOL bValue);
	void SetAllTextureProjective(BOOL bValue);

	void SetParticleEmitter(DWORD dwName,BOOL bValue);
	void SetTrailEmitter(DWORD dwName,BOOL bValue);

	BOOL IsParticleEnable(DWORD dwName);
	BOOL IsTrailEnable(DWORD dwName);
	//释放资源
	void ReleaseAll();

public:
	DWORD GetID()	{return m_dwModelID;}
	void SetID(DWORD dwID) {m_dwModelID = dwID;}

	DWORD GetGameModelID()	{return m_dwGameModelID;}
	void SetGameModelID(DWORD dwID) {m_dwGameModelID = dwID;}

	BOOL IsShowAllGroup() {return m_bAllGroup;}

	void SetShaderGroupHandle(DWORD dwID){m_dwShaderGroupHandle=dwID;}

	void SetAnimCircleTime(DWORD dwColorCircle,DWORD dwUVCircle,
		DWORD dwVisiCircle,DWORD TextureCircle)
	{
		m_dwAnimColorCircle=dwColorCircle;m_dwAnimUVCircle=dwUVCircle;
		m_dwAnimVisibilityCircle=dwVisiCircle;m_dwAnimTextureCircle=TextureCircle;
	}

	//CDisplayModel::AnimInfo* GetAnimInfo()	{return m_pAnimInfo;}
	//DWORD GetCurActionLoopCount()	{return m_pAnimInfo->m_tActionInfo.dwCurActionLoopCount;}

	//得到Group->Part的等级参数
	BOOL IsGroupPartShow(DWORD dwGroup,DWORD dwPart);
	DWORD GetLvlOfGroupPart(DWORD dwGroup,DWORD dwPart);
	BOOL IsGroupPartLevelShow(DWORD dwGroup,DWORD dwPart,DWORD dwLevel);
	//设置group信息
	void ShowGroup(DWORD dwGroup,DWORD dwPart,DWORD dwLevel,bool bAsyn=true);
	//设置group信息
	void ShowAllGroupbyLevel(DWORD dwLevel,bool bAsyn=true);
	//显示全部Group,并选择缺省的Part和等级
	void ShowAllGroup(bool bAsyn=true);
	//隐藏某个Group
	void HideGroup(DWORD dwGroup);
	// 隐藏所有Group
	void HideAllGroup();

	mapDisplayGroups& GetGroups()	{return m_DisplayGroups;}

	//设置Group->Part->相应等级信息
	BOOL SetLeveInfo(DWORD dwGroup,DWORD dwPart,DWORD dwLevel,
		GameModel::Group::Part::Level *pLevel);


	void SetLocatorArray(mapLocators& Locators);
	void SetBindPos(model::BindPose *pBindPose){m_pBindPose=pBindPose;}

	//得到group、part选中的等级
	DWORD GetSelectedLvl(DWORD dwGroup,DWORD dwPart);
	//得到三角形个数
	DWORD GetTriangleCount(void);

	//根据ID得到locator
	GameModel::Locator* GetLocator(DWORD dwLocator);

	void SetParticleEmitters(ParticleEmitters& partiEmitters);
	void SetTrailEmitters(TrailEmitters& trailEmitters);

	void SetTextureProjective(TextureProjectives& ptextureprojective);

	void EnableTextureProjective(BOOL bValue);

	void SetWater(Water::CWater * pWater)		{ m_pWater = pWater; }			//设定水面
	Water::CWater * GetWater()					{ return m_pWater; }			//得到水面

	void EnableParticleEmitter(DWORD dwName,BOOL bValue);
	void EnableTrailEmitter(DWORD dwName,BOOL bValue);
	void SetAllTrailEmitter(BOOL bValue);

	void EnableParticleEmitter(BOOL bValue);
	void EnableTrailEmitter(BOOL bValue);

	void SetRendShadow(BOOL bRendShadow){m_bRendShadow = bRendShadow;}
	BOOL GetRendShadow(){return m_bRendShadow;}

	//模型颜色处理
	/*void SetRegionAmbient(DWORD dwcolor);
	void SetAmbient(DWORD dwcolor);*/
	void ForceToEnableLight(void);						//强制模型受光照运算
	void SetDirLightEnable(bool lightenable = true);
	void SetUseNormal(bool busenormal);
	void SetModelColor(DWORD dwColor);
	void SetModelColorCompel(DWORD dwColor);			//避开模型是否受灯光效果影响的配置,强制为模型设定颜色
	void MultiplyModelColor(DWORD dwColor);
	void MultiplyModelAlpha(float fRatio);

	//正常模型显示,计算时间 -> 顶点处理 -〉效果处理 -> 渲染
	void ProcessAnimLoop(AnimInfo *pAnimInfo);			//单独处理模型动画时间循环
	void ProcessAnimJoint(AnimInfo *pAnimInfo);			//处理模型的骨骼动画信息
	void ProcessVisibility(AnimInfo *pAnimInfo);
	void ProcessAnimUV(AnimInfo *pAnimInfo);
	void ProcessAnimTexture(AnimInfo *pAnimInfo);
	void ProcessAnimColor(AnimInfo *pAnimInfo);
	void ProcessLocator(AnimInfo *pAnimInfo);
	void ProcessPickMesh(AnimInfo *pAnimInfo);
	void ProcessParticleEmitter(AnimInfo *pAnimInfo,bool bdrawtoscreen = false,bool brendtotarget = false);
	void ProcessTrailEmitter(AnimInfo *pAnimInfo);
	void ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix,bool bnormalize = false);
	void ProcessTextureProjective(AnimInfo *pAnimInfo);
	void ProcessWater(AnimInfo * pAnimInfo);												//处理水面特效
	void SetRendSimpleMesh(bool brendsimesh){m_brendsimplemesh = brendsimesh;}

	//光照计算
	void ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,
		const D3DXVECTOR3 *pLightDirectiion,DWORD dwColor);
	void CopyBufferToColor(std::vector<D3DCOLOR*>& vecbuffer);
	void CopyColorToBuffer(std::vector<D3DCOLOR*>& vecbuffer);
	void ProcessPointLight(const D3DXVECTOR3 *pLightPosition,
		float fIntensity,DWORD dwColor);
	//渲染模型
	void RenderModel();
	void RenderModelTransparent();					//渲染透明模型
	void RenderModelFlash(DWORD dwFlashColor);		//渲染闪光模型



	void DrawModel(void);							//直接画模型
	void DrawModel(DWORD Group);					//直接画模型某一部分
	void DrawModelWithoutState();					//直接画模型,自己设置状态
	void DrawModelWithoutStateAndTexture();			//直接画模型,状态纹理都自己设定
	void DrawModelByColorOp(DWORD dwColorOp, DWORD dwColorArg1, DWORD dwColorArg2);			//使用颜色操作直接绘制模型

	//渲染到Icon
	void DisplayIcon(long lX,long lY,float fScale,float fRotation,AnimInfo *pAnimInfo);
	void DrawModelByAniminfo(AnimInfo *pAnimInfo);

	//调试
	void Dump(void);
	void _DEBUG_RenderNormal();
	void _DEBUG_RenderLocator();
	void _DEBUG_RenderPickBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor = 0xA000FF00);
	void _DEBUG_RenderSizeBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor = 0xA0FF0000);
	void _DEBUG_RenderAction(AnimInfo *pAnimInfo);

	//子模型处理
private:
	struct tagChildModelInfo
	{
		//定位点
		GameModel::Locator* pLocator;
		set<CDisplayModel*> setChildModels;
	};

	//对应相应骨骼需要显示的子模型
	typedef map<DWORD,tagChildModelInfo>	ChildDisplayModels;
	typedef map<DWORD,tagChildModelInfo>::iterator itChildModel;
	ChildDisplayModels m_ChildDisplayModels;

public:
	void AddChildModel(DWORD dwLocatorID,CDisplayModel* pDisplayModel);
	void RemoveChildeModel(DWORD dwLocatorID,CDisplayModel* pDisplayModel);


	/////////////////////////////////////////////////////////////////
	//逻辑层应用接口
private:
	void ProcessLocator(AnimInfo *pAnimInfo,GameModel::Locator* pLocator);
	void ProcessLocatorByFrame(AnimInfo *pAnimInfo,GameModel::Locator* pLocator,DWORD dwFrame);

	//Group的装载状态
	eLoadState m_eGroupLoadState;
	//粒子的装载状态
	eLoadState m_eParticleLoadState;

	//检查模型文件和Shader是否读取完毕
	eLoadState CheckBaseModelLoadState();
public:
	//得到装载情况(主要是Group)
	eLoadState GetLoadState();
	//检查所有Group的装载情况
	void CheckGroupLoadState();
	//检查粒子的装载情况
	void CheckParticleLoadState();
	//销毁
	void Destroy(void);
	//得到定位点位置
	BOOL GetLocatorPos(AnimInfo *pAnimInfo,DWORD dwLocator,
		float &fX, float &fY, float &fH,DWORD dwFrame = -1);
	//得到拾取盒
	render::BoundingBox *GetPickBox() { return m_pPickBox;}
	void SetPickBox(render::BoundingBox *pBox) {m_pPickBox=pBox;}
	render::BoundingBox *GetSizeBox() { return m_pSizeBox;}
	void SetSizeBox(render::BoundingBox *pBox){m_pSizeBox=pBox;}
	//得到拾取MESH
	GameModel::PickMesh *GetPickMesh() { return m_pPickMesh;}
	void SetPickMesh(GameModel::PickMesh *pmesh) {m_pPickMesh=pmesh;}
	//显示模型数据
	void Display(AnimInfo *pAnimInfo);
	//显示模型绑定到其他模型的绑定点
	void GetAnimInfobyLocator(AnimInfo *pOwnAnimInfo,CDisplayModel *pParentModel,AnimInfo *pParentAnimInfo,DWORD parentlocatorname);
	bool CheckStatic();
};

#endif
