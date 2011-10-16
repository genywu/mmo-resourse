#pragma once


#include "..\frostengine\utility.h"
#include "..\frostengine\particle.h"
#include "..\frostengine\model.h"
#include "..\frostengine\Water.h"

class CTextureProjective;
#include <map>

#define GAMEMODEL_MAXNAME			 64
#define GAMEMODEL_MAXPARTICLEEMITTER 32
#define GAMEMODEL_MAXTEXTUREPROJECTIVE 20
#define GAMEMODEL_MAXTRAILEMITTER    16
#define GAMEMODEL_MAXGROUP		     96

//定义了模型里无效的ID
#define INVALID_ID	0x0FFFFFFF

//角色模型数据容器
class GameModel:public RCObject,public LoadState
{
private:
	DWORD m_dwID;

	DWORD m_dwAnimColorCircle;
	DWORD m_dwAnimUVCircle;
	DWORD m_dwAnimVisibilityCircle;
	DWORD m_dwAnimTextureCircle;

	char m_pszFileName[MAX_PATH];

public:
	class Action;
	class AnimInfo;

	class TrailContainer	//容器,接受发射器的拖尾
	{
		//点组
	public:
		struct tagTrail
		{
			D3DXVECTOR3 vPos[2];
			DWORD dwStartTime;
			DWORD dwTimeLength;
			DWORD dwTopColor;
			DWORD dwBottomColor;
			float fFade;
			tagTrail *pNext;

			tagTrail *pNextRender;
		};
	private:
		tagTrail * m_pTrailUsed;
		tagTrail * m_pTrailFree;
		tagTrail * m_pTrailRenderHead;
	public:
		void Render(DWORD dwShaderGroupHandle,DWORD dwShaderIndex);
		void Update(DWORD dwCurrentTime);
		void Clear();

		TrailContainer();
		~TrailContainer();
		tagTrail * GetFreeTrail();
	};
	class TrailEmitter:public RCObject,public LoadState
	{
		DWORD m_dwName;

		char m_pszMeshFileName[MAX_PATH];
		model::Mesh    *m_pMesh;  //尾迹几何体
		model::Buffer  *m_pBuffer;//当前数据

		char m_pszColorFileName[MAX_PATH];
		std::map<DWORD,unsigned __int64> m_mapColor;

		DWORD   m_dwCurColorIndex;

		BOOL	m_bEnable;//打开

		DWORD m_dwLifeTimeLength;

		DWORD m_dwLastElapseTime,m_dwCurElapseTime;

	public:
		model::Mesh   * GetMesh()   const { return m_pMesh;}
		model::Buffer * GetBuffer() const { return m_pBuffer;}

		DWORD   GetLifeTimeLength() const { return m_dwLifeTimeLength;}
		unsigned __int64  GetColor();

		DWORD  GetCurElapseTime() const { return m_dwCurElapseTime;}
		DWORD  GetLastElapseTime() const { return m_dwLastElapseTime;}

		DWORD  GetName() const { return m_dwName;}
		const char* GetMeshFileName() {return m_pszMeshFileName;}
		const char* GetColorFileName() {return m_pszColorFileName;}

		BOOL   Create(utility::Text::Node *pNode,const char *szResourcePath);
		BOOL   Create(utility::Text::Node *pNode);
		void   CreateMesh(CRFile *pFile);
		BOOL   CreateColor(CRFile* pFile);
		void   Destroy();

		void   Enable(BOOL bValue) { m_bEnable = bValue;}
		BOOL   IsEnable() { return m_bEnable;}

		void SelectColor(DWORD dwIndex);
	};
private:
	typedef std::pair<DWORD,TrailEmitter*> TRAILEMITTER_PAIR;
	std::map<DWORD,TrailEmitter*> m_mapTrailEmitterArray;
public:
	class AnimInfo
	{ 
	public:
		struct tagActionInfo
		{
			DWORD dwCurActionStartTime;		//开始时间
			DWORD dwCurActionLoopCount;		//动作循环计数
			DWORD dwCurAction;				//当前动作编号
			DWORD dwCurActionElapseTime;	//当前动作播放时间
			float fCurActionSpeedRate;		//自定义当前动作速率
			BOOL  bCurActionLooped;			//是否循环标志(不循环将停止在最后一帧)

			BOOL  bInterpolation;		    //内插标志

			DWORD dwInterpolationElapseTime;//流失时间
			DWORD dwInterpolationStartTime;	//开始时间
			DWORD dwInterpolationTimeLength;

			DWORD dwOldAction;				//上一个动作
			DWORD dwOldActionTimeElapse;	//上一个动作停止的时间
			float fOldActionSpeedRate;		//上一个动作的速率
		}m_tActionInfo;
	private:
		friend GameModel;

		//当前的渲染数据
		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matproj;
		D3DXMATRIX m_matview;
		D3DXMATRIX m_matscal;

		D3DXMATRIX m_matBillboard;
		D3DXMATRIX m_matBillboardY;	   

		//处理数据,动作动画时间
		DWORD m_dwCurrentTime;		//当前时间

		//粒子发射器时间
		//粒子发射器时间
		DWORD m_dwParticleEmitterLastTimeArray[GAMEMODEL_MAXPARTICLEEMITTER];
		//拖尾发射器坐标列表
		TrailContainer *m_pTrailContainerArray;

		//全局动画时间表
		DWORD m_dwtextureprojectivetime,m_dwAnimColorStartTime,m_dwAnimUVStartTime,m_dwAnimVisibilityStartTime,m_dwAnimTextureStartTime;



	public:

		tagActionInfo * GetActionInfo() { return &m_tActionInfo;}
		const tagActionInfo * GetActionInfo() const { return &m_tActionInfo;}

		inline const D3DXMATRIX * GetWorldMatrix() const { return &m_matWorld;}
		inline const D3DXMATRIX * GetViewMatrix() const { return &m_matview;}
		inline const D3DXMATRIX * GetProjectedMatrix() const { return &m_matproj;}
		inline const D3DXMATRIX * GetBillboard()   const { return &m_matBillboard;}
		inline const D3DXMATRIX * GetBillboardY()    const { return &m_matBillboardY;}

		inline void SetWorldMatrix(const D3DXMATRIX * pWorldMatrix)         { memcpy(&m_matWorld,pWorldMatrix,sizeof(D3DXMATRIX));}
		inline void SetBillboard(const D3DXMATRIX * pBillboardMatrix)       { memcpy(&m_matBillboard,pBillboardMatrix,sizeof(D3DXMATRIX));}
		inline void SetBillboardY(const D3DXMATRIX * pBillboardMatrixY)     { memcpy(&m_matBillboardY,pBillboardMatrixY,sizeof(D3DXMATRIX));}

		//void ChangeActionTo(DWORD dwSrcAction,DWORD dwSrcTime,DWORD dwDstAction,DWORD dwTimeLength);
		//void ChangeAction(DWORD dwAction);

		void SetCurrentTime(DWORD dwCurrentTime) { m_dwCurrentTime = dwCurrentTime;}
		DWORD GetCurrentTime() const { return m_dwCurrentTime;}
		DWORD GetTextureProjectiveTime()const {return m_dwtextureprojectivetime;}
		void SetAnimTime(DWORD dwTime)
		{
			m_dwtextureprojectivetime = 				
				m_dwAnimColorStartTime = 
				m_dwAnimTextureStartTime = 
				m_dwAnimUVStartTime = 
				m_dwAnimVisibilityStartTime = dwTime;
		}


		AnimInfo();
		~AnimInfo();
		void ClearTrailContainer(void);
	};



	class ParticleEmitter:public RCObject,public LoadState
	{
		DWORD m_dwName;
		DWORD m_dwType;//
		DWORD m_dwEmittNumber;

		char m_pszMeshFileName[MAX_PATH];
		model::Mesh   * m_pMesh;
		model::Buffer * m_pBuffer;

		//粒子
		char m_pszEmitterFileName[MAX_PATH];
		particle::Emitter * m_pParticleEmitter;

		BOOL m_bEnable;//打开

		DWORD m_dwLastTimeTemp,m_dwCurTimeTemp;

	public:
		void CloneEmitter(ParticleEmitter* pParticleEmitter);
		DWORD GetName()  const { return m_dwName;}
		DWORD GetType()  const { return m_dwType;}
		BOOL  IsEnable() const { return m_bEnable;}

		const char* GetMeshFileName(){return m_pszMeshFileName;}
		model::Mesh   * GetMesh()   { return m_pMesh;}
		model::Buffer * GetBuffer() { return m_pBuffer;}

		const char* GetEmitterFileName(){return m_pszEmitterFileName;}
		particle::Emitter * GetParticleEmitter() { return m_pParticleEmitter;}
		particle::Emitter *& GetParticleEmitterRef() { return m_pParticleEmitter;}

		void Create(utility::Text::Node *pNode,const char *szResourcePath);
		void Create(utility::Text::Node *pNode);
		void CreateMesh(CRFile *pFile);

		void Destroy();
		DWORD GetEmitteNumber() { return m_dwEmittNumber;}

		void Enable(BOOL bValue) { m_bEnable = bValue;}
	};

private:
	typedef std::pair<DWORD,ParticleEmitter*> PARTICLEEMITTER_PAIR;
	std::map<DWORD,ParticleEmitter*> m_mapParticleEmitterArray;
public:
	class TextureProjective:public RCObject,public LoadState
	{
		//纹理投影
		CTextureProjective * m_ptexproj;
		//DWORD m_texprjnum;
		DWORD m_dwName;
		DWORD m_dwLastTimeTemp,m_dwCurTimeTemp;
		char m_pszTextureProjFileName[MAX_PATH];

	public:
		DWORD GetName()  const { return m_dwName;}
		BOOL  IsEnable();

		CTextureProjective * GetTextureProjective() { return m_ptexproj;}
		const char* GetTextureProjFileName(){return m_pszTextureProjFileName;}
		//BOOL  IsEnable() const { return m_ptexproj->IsVisable();}
		void Create(utility::Text::Node *pNode);
		void Destroy();
		//DWORD GetTexprjNumber() { return m_texprjnum;}

		void Enable(BOOL bValue);
	};
private:
	typedef std::pair<DWORD,TextureProjective*> TEXTUREPROJECTIVE_PAIR;
	std::map<DWORD,TextureProjective*> m_maptextureprojectivearray;

	Water::CWater * m_pWater;          //水面指针
public:
	class Locator:public LoadState
	{
		DWORD m_dwName;
		char m_pszFileName[MAX_PATH];
		D3DXMATRIX     m_matNowWorldMatrix;//目前的世界变换矩阵
		D3DXMATRIX     m_matNowModelMatrix;//目前的模型坐标变换矩阵
		model::Mesh    *m_pMesh;//定位几何体
		model::Buffer  *m_pBuffer;//当前数据
	public:
		const char* GetFileName()	{return m_pszFileName;}
		model::Buffer * GetBuffer() { return m_pBuffer;}
		model::Mesh   * GetMesh()   { return m_pMesh;}
		D3DXMATRIX&     GetNowWorldMatrix(){ return m_matNowWorldMatrix;}
		void			SetNowWorldMatrix(D3DXMATRIX& nowmat) { m_matNowWorldMatrix = nowmat;}
		D3DXMATRIX&     GetNowModelMatrix(){ return m_matNowModelMatrix;}
		void			SetNowModelMatrix(D3DXMATRIX& nowmat) { m_matNowModelMatrix = nowmat;}
		DWORD			GetName()				{ return m_dwName;}

		void Create(utility::Text::Node * pNode,const char *szResourcePath);
		void Create(utility::Text::Node * pNode);

		BOOL Create(CRFile *pFile);
		void Destroy();				
	};
	class PickMesh:public LoadState
	{
	public:
		PickMesh();
		~PickMesh();
		model::Buffer * GetBuffer() { return m_pBuffer;}
		model::Mesh   * GetMesh()   { return m_pMesh;}
		BOOL Create(CRFile *pFile);
		void Destroy();	
		void SetFileName(const char *pszFileName){strncpy(m_strFileName,pszFileName,MAX_PATH);}
		const char* GetFileName()	{return m_strFileName;}
	protected:
		char m_strFileName[MAX_PATH];
		model::Mesh    *m_pMesh;//定位几何体
		model::Buffer  *m_pBuffer;//当前数据
	private:
	};
private:
	typedef std::pair<DWORD,Locator*> LOCATOR_PAIR;
	std::map<DWORD,Locator*> m_mapLocatorArray;								//组件列表
public:
	class Group
	{
	public:
		class Part:public RCObject,public LoadState
		{
		private:
			DWORD m_dwName;//部件名称
			char m_strFileName[MAX_PATH];
		public:
			struct tagRenderMesh:public RCObject,public LoadState
			{				
				//部件组件，包含一组mesh + frame buffer   + 定位点索引
				//名称				
				WORD wIndex;
				//Mesh文件名
				char pszMeshFileName[MAX_PATH];
				//图形数据
				model::Mesh    *pMesh;

				//数据缓冲
				model::Buffer  *pBuffer;

				DWORD dwRendMeshIndex;


				//配置文件属性
				DWORD dwAlphaMode;

				BOOL  bBillboard;
				BOOL  bAroundYBillboard;		//绕Y轴?
				BOOL  bAnimUV;//是否流动纹理
				BOOL  bAnimColor;//是否变化颜色
				BOOL  bAnimVisibility;//是否受可见变化影响
				BOOL  bAnimTexture;//是否受动态纹理影响
				BOOL  bEffectByLight;//是否被光照影响
				BOOL  bEffectByGridColor;//是否被光照影响
				BOOL  bSpecular;//是否高光
				BOOL  bEnvironmentMaps;//环境贴图
				BOOL  bProjected;

			};

			//定义的RenderMeshes的vector容器
			typedef vector<tagRenderMesh*>	LevelMeshes;
			typedef vector<tagRenderMesh*>::iterator itLevelMesh;

			class Level:public RCObject,public LoadState
			{
				LevelMeshes		m_vRMArray;//RenderMesh
				char m_pszColorFileName[64];
				model::AnimColor      * m_pAnimColor;
				char m_pszUVFileName[64];
				model::AnimUV		  * m_pAnimUV;
				char m_pszVisiFileName[64];
				model::AnimVisibility * m_pAnimVisibility;
				char m_pszTexFileName[64];
				model::AnimTexture    * m_pAnimTexture;
				std::vector<DWORD> m_vParticleEnable;

			public:
				Level();
				~Level();
				BOOL Create(utility::Text::Node *pLevelNode,const char *pszResourcePath);
				BOOL Create(utility::Text::Node *pLevelNode);
				void Destroy();

				int  GetRMCount() { return (int)m_vRMArray.size();}
				LevelMeshes&  GetRMArray() { return m_vRMArray;}

				const char* GetColorFileName() {return m_pszColorFileName;}
				model::AnimColor      * GetAnimColor() { return m_pAnimColor;}
				void SetAnimColor(model::AnimColor* pAnimColor){m_pAnimColor=pAnimColor;}

				const char* GetUVFileName(){return m_pszUVFileName;}
				model::AnimUV         * GetAnimUV()    { return m_pAnimUV;}
				void SetAnimUV(model::AnimUV *pAnimUV) {m_pAnimUV=pAnimUV;}

				const char* GetVisiFileName() {return m_pszVisiFileName;}
				model::AnimVisibility * GetAnimVisibility() { return m_pAnimVisibility;}
				void SetAnimVisibility(model::AnimVisibility *pAnimVisi){m_pAnimVisibility=pAnimVisi;}

				const char* GetTexFileName() {return m_pszTexFileName;}
				model::AnimTexture    * GetAnimTexture() { return m_pAnimTexture;}
				void SetAnimTexture(model::AnimTexture* pAnimTextue) {m_pAnimTexture=pAnimTextue;}

				std::vector<DWORD>& GetParticleEnable(){return m_vParticleEnable;}


			};
		private:
			tagRenderMesh * m_pRenderMeshArray;
			DWORD m_dwRenderMeshCount;//模型列表

			Level * m_pLevelArray;
			DWORD m_dwLevelCount;//级别数据列表

			DWORD m_dwSelectLevel;//选择的等级

		public:
			DWORD  GetName()  { return m_dwName;}
			const char* GetFileName()	{return m_strFileName;}
			//函数接口
			//BOOL Create(utility::Text::Node *pPartNode,char *szResourcePath);
			BOOL Create(utility::Text::Node *pPartNode,const char *szModelPath);
			BOOL Create(CRFile* pFile);
			void Destroy();

			void Dump();

			tagRenderMesh * GetRenderMeshArray() { return m_pRenderMeshArray;}
			tagRenderMesh * GetRenderMesh(DWORD dwID);

			DWORD			GetRenderMeshArrayCount() { return m_dwRenderMeshCount;}
			Level         * GetLevelArray() { return m_pLevelArray;}
			DWORD			GetLevelCount() { return m_dwLevelCount;}

			DWORD			GetSelectLevelIndex() { return m_dwSelectLevel;}
			Level		  * GetSelectLevel() { return &m_pLevelArray[m_dwSelectLevel];}
			Level         * GetLevel(DWORD & dwLevel);

			Part();
			~Part();

			void  _DEBUG_RenderNormal();

			void  DrawAlphaTestOnly(DWORD dwShaderGroupHandle);
			void  DrawAlphaBlendOnly(DWORD dwShaderGroupHandle);
			void  DrawTransparent(DWORD dwShaderGroupHandle);
			void  Render(DWORD dwShaderGroupHandle);
			void  RenderTransparent(DWORD dwShaderGroupHandle, DWORD dwMode); // 新加渲染透明模型
			void  RenderBlend(DWORD dwShaderGroupHandle);			
			void  ProcessAnimJoint(const GameModel::AnimInfo *pAnimInfo,model::AnimJoint * pOldAnimJoint,model::AnimJoint *pCurAnimJoint,DWORD dwOldActionTimeElapse,DWORD dwCurActionTimeElapse,model::BindPose *pBindPose,float fRatio);			
			void  CalculateBillboard(const model::Mesh *pMesh,model::Buffer * pBuffer,const D3DXMATRIX *pBillboarMatrix);
			void  ProcessPointLight(const D3DXVECTOR3 * pLightPosition, float fIntensity, DWORD dwColor);
			void  ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,const D3DXVECTOR3 *pLightDirection,DWORD dwColor);
			void  SetColor(DWORD dwColor);
			void  MultiplyColor(DWORD dwColor);
			void  MultiplyAlpha(float fRatio);

			void  SetLevel(DWORD dwLevel);

			DWORD GetLevel() const { return m_dwSelectLevel;}

			void GetRenderMeshes(DWORD dwLevel,Level *&pLevel,LevelMeshes& RenderMeshes);

			void ProcessLevelAnimVisibility(DWORD dwTimeElapse,DWORD dwTimeCircle);
			void ProcessLevelAnimTexture(DWORD dwTimeElapse,DWORD dwTimeCircle);

			void ProcessLevelAnimColor(DWORD dwTimeElapse,DWORD dwTimeCircle);
			void ProcessLevelAnimUV(DWORD dwTimeElapse,DWORD dwTimeCircle);
			void ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix);
		};
	private:
		////////////////////////////////////////////////////////////////////
		DWORD  m_dwName;												    //名称
		DWORD  m_bVisible;

		typedef std::pair<DWORD,Part*> PART_PAIR;
		std::map<DWORD,Part*> m_mapPartArray;								//组件列表
		Part * m_pCurSelected;												//当前激活的组件
	public:

		Group();
		~Group();
		//建立模型
		//BOOL Create(utility::Text::Node *pGroupNode,char *szResourcePath); 
		BOOL Create(utility::Text::Node *pGroupNode,const char *szModelPath); 
		//销毁模型
		void Destroy();

		//设置当前部件
		void SelectPart(DWORD dwPart);
		//可见设置，跟显示相关
		void SetVisible(BOOL bVisible) { m_bVisible = bVisible;}
		BOOL IsVisible() const { return m_bVisible;}

		void Dump(void);

		DWORD GetName()	 const { return m_dwName;}
		DWORD GetPartCount() const { return (DWORD)m_mapPartArray.size();}//取得部件数

		Group::Part * GetSelectPart(void);
		Group::Part * GetPart(DWORD dwName);	

		std::map<DWORD,Part*> * GetPartArray() { return &m_mapPartArray;}
		void SelectDefault(void);
		DWORD GetDefaultPartName(void);
		void SelectNull();
	};

private:
	typedef std::pair<DWORD,Group*> GROUP_PAIR;
	//部件类型
	std::map<DWORD,Group *> m_mapGroupArray;

public:
	//动作
	class Action:public RCObject,public LoadState
	{
		DWORD m_dwName;
		char m_strFileName[MAX_PATH];
		model::AnimJoint * m_pAnimJoint;
		BOOL m_bvisibility;

	public:
		//动作关联的粒子可见标志
		struct tagParticleParameter
		{
			BOOL  bVisibile;
			DWORD dwStartTime;//开始发射时间
			DWORD dwEndTime;  //结束发射时间
		};
	private:
		typedef std::pair<DWORD,tagParticleParameter> PARTICLEVISIBLE_PAIR;
		std::map<DWORD,tagParticleParameter> m_mapParticleVisible;
		//动作关联的拖尾可见标志
		typedef std::pair<DWORD,BOOL> TRAILVISIBLE_PAIR;
		std::map<DWORD,BOOL> m_mapTrailVisible;


		//数据
	public:
		Action();
		~Action();

		tagParticleParameter * GetParticleParameter(DWORD dwIndex);

		BOOL GetTrailVisible(DWORD dwIndex);
		BOOL GetVisibility(void){return m_bvisibility;}

		BOOL Create(utility::Text::Node * pActionNode,char *szResourcePath);
		BOOL Create(utility::Text::Node * pActionNode);
		void Destroy(void);

		DWORD GetName(void) const { return m_dwName;}

		model::AnimJoint * GetAnimJoint()	{ return m_pAnimJoint;}
		void SetAnimJoint(model::AnimJoint *pAnimJoint){m_pAnimJoint=pAnimJoint;}
		const char* GetAnimJoinFileName()	{return m_strFileName;}

		void Dump(void);
	};

private:
	//整个模型统一管理的Mesh动画
	typedef map<string,model::AnimColor*> MapAnimColor;
	typedef map<string,model::AnimColor*>::iterator itAnimColor;
	MapAnimColor m_mapAnimColor;

	typedef map<string,model::AnimUV*> MapAnimUV;
	typedef map<string,model::AnimUV*>::iterator itAnimUV;
	MapAnimUV m_mapAnimUV;

	typedef map<string,model::AnimVisibility*> MapAnimVisi;
	typedef map<string,model::AnimVisibility*>::iterator itAnimVisi;
	MapAnimVisi m_mapAnimVisi;

	typedef map<string,model::AnimTexture*> MapAnimTexture;
	typedef map<string,model::AnimTexture*>::iterator itAnimTex;
	MapAnimTexture m_mapAnimTexture;

	typedef std::pair<DWORD,Action*> ACTION_PAIR;
	std::map<DWORD,Action *> m_mapActionArray;

	//该模型的范围盒
	render::BoundingBox *m_pPickBox;
	//该模型的范围MESH
	PickMesh *m_pPickMesh;
	//PickBox是否自动生成
	BOOL m_bPickBoxAutoMake;
	//该模型的选择盒
	render::BoundingBox *m_pSizeBox;
	//PickBox是否自动生成
	BOOL m_bSizeBoxAutoMake;

	//着色变量
	//shader的文件名
	char		m_pszShaderFileName[MAX_PATH];
	DWORD		m_dwShaderGroupHandle;
	//是否渲染阴影
	BOOL m_bRendShadow;

	//绑定位置
	model::BindPose * m_pBindPose;

	void CalculateBillboard(const model::Mesh *pMesh,model::Buffer * pBuffer,const D3DXMATRIX *pBillboardMatrix);

public:
	GameModel();
	~GameModel();

	void AutoMakeBoundingBox();

	DWORD GetID()	{return m_dwID;}
	void SetID(DWORD dwID){m_dwID=dwID;}
	BOOL  GetRendShadow(){return m_bRendShadow;}
	DWORD GetAnimColorCircle(){return m_dwAnimColorCircle;}
	DWORD GetAnimUVCircle(){return m_dwAnimUVCircle;}
	DWORD GetAnimVisibilityCircle(){return m_dwAnimVisibilityCircle;}
	DWORD GetAnimTextureCircle(){return m_dwAnimTextureCircle;}

	model::BindPose * GetBindPos()	{return m_pBindPose;}
	std::map<DWORD,Locator*>& GetLocatorArray()	{return m_mapLocatorArray;}

	map<DWORD,TrailEmitter*>& GetTrailEmitterArray(){return m_mapTrailEmitterArray;}
	TrailEmitter* GetTrailEmitter(DWORD dwName);

	map<DWORD,ParticleEmitter*>& GetParticleEmitterArray() {return m_mapParticleEmitterArray;}
	ParticleEmitter* GetParticleEmitter(DWORD dwName);

	map<DWORD,TextureProjective*>& GetTextureProjectiveArray() {return m_maptextureprojectivearray;}

	Water::CWater * GetWater()	{ return m_pWater; }           //得到水面指针

	//显示设置
	void SetGroupVisible(DWORD dwGroup,BOOL bVisible);
	void SelectPart(DWORD dwGroup,DWORD dwPart);

	//初始化 && 销毁	[通过配置文件建立角色模型]
	BOOL Create(const char *pszFileName,DWORD dwTextureProcision=0);
	BOOL Create(CRFile* pFile);
	BOOL CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision=0);
	void Destroy();

	const char* GetModelFileName()	{return m_pszFileName;}
	//调试
	void Dump(void);
	void _DEBUG_RenderPickBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor = 0xA000FF00);
	void _DEBUG_RenderSizeBox(const D3DXMATRIX *pWroldMatrix,D3DCOLOR dwColor = 0xA0FF0000);

	void _DEBUG_RenderAction(const GameModel::AnimInfo *pAnimInfo);

	//设置动作
	DWORD	 GetActionCount(void) const { return (DWORD)m_mapActionArray.size();}
	//数据存取
	std::map<DWORD,Group*> * GetGroupArray() { return &m_mapGroupArray;}

	std::map<DWORD,Action*>    * GetActionArray() { return &m_mapActionArray;}

	//得到动作
	Action * GetAction(DWORD dwName); 

	//模型颜色处理
	void SetModelColor(DWORD dwColor);
	void MultiplyModelColor(DWORD dwColor);
	void MultiplyModelAlpha(float fRatio);

	//正常模型显示,计算时间 -> 顶点处理 -〉效果处理 -> 渲染
	void ProcessAnimJoint(AnimInfo *pAnimInfo);
	void ProcessVisibility(AnimInfo *pAnimInfo);
	void ProcessAnimUV(AnimInfo *pAnimInfo);
	void ProcessAnimTexture(AnimInfo *pAnimInfo);
	void ProcessAnimColor(GameModel::AnimInfo *pAnimInfo);
	void ProcessLocator(const GameModel::AnimInfo *pAnimInfo);
	void ProcessParticleEmitter(AnimInfo *pAnimInfo);
	void ProcessTextureProjective(AnimInfo *pAnimInfo);
	void ProcessTrailEmitter(AnimInfo *pAnimInfo);
	void ProcessEnvironmentMaps(const D3DXMATRIX *pViewMatrix);
	void RenderModel(); 
	void RenderModelTransparent();		// 渲染透明模型
	void RenderModelBlend();

	//直接画模型
	void DrawModel(void);
	void DrawModelShadow();
	void DrawModelTransparent();

	//光照计算
	void ProcessDirectionalLight(const D3DXVECTOR3 *pCameraPosition,const D3DXVECTOR3 *pLightDirectiion,DWORD dwColor);
	void ProcessPointLight(const D3DXVECTOR3 *pLightPosition,float fIntensity,DWORD dwColor);

	//取得部件类型数
	DWORD	 GetGroupCount(void) const { return (DWORD)m_mapGroupArray.size();}
	Group  * GetGroup(DWORD dwName);

	//数据接口
	const char*			GetShaderFileName()	const {return m_pszShaderFileName;}
	DWORD		         GetShaderGroupHandle() const { return m_dwShaderGroupHandle;}
	render::BoundingBox *GetSizeBox() { return m_pSizeBox;}
	render::BoundingBox *GetPickBox() { return m_pPickBox;}	
	PickMesh *GetPickMesh() { return m_pPickMesh;}	
	Locator *		     GetLocator(DWORD dwLocatorName);
	DWORD				 GetVisibleTriangleCount(void);	

	//模型组件处理
	void SetGroupVisible(bool bVisible);
	void SetGroupLevel(int iLevel);
	void SelectDefaultPart(void);
	void SelectDefaultPart(DWORD dwGroup);
	void SelectNullPart(DWORD dwGroup);

	model::AnimColor* GetAnimColor(const char* pszFileName);
	void SetAnimColor(const char* pszFileName,model::AnimColor* pAnimColor);

	model::AnimUV* GetAnimUV(const char* pszFileName);
	void SetAnimUV(const char* pszFileName,model::AnimUV* pAnimUV);

	model::AnimVisibility* GetAnimVisi(const char* pszFileName);
	void SetAnimVisi(const char* pszFileName,model::AnimVisibility* pAnimVisi);

	model::AnimTexture* GetAnimTexture(const char* pszFileName);
	void SetAnimTexture(const char* pszFileName,model::AnimTexture* pANim);


	//调试
	void _DEBUG_RenderNormal();
	void _DEBUG_RenderLocator(); 	

	//粒子发射器	
	void EnableParticleEmitter(BOOL bValue);
	void EnableParticleEmitter(DWORD dwName,BOOL bValue);
	//Trail发射器	
	void EnableTrailEmitter(BOOL bValue);
	void EnableTrailEmitter(DWORD dwName,BOOL bValue);
	//投影纹理
	void EnableTextureProjective(BOOL bValue);

	bool CheckStatic(void);//检查是否是静态物件

};
