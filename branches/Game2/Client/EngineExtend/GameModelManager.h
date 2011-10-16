//=============================================================================
/**
*  file: GameModelManager.h
*
*  Brief:管理一个模型的所有资源
*
*  Authtor:wangqiao
*	
*	Datae:2007-10-25
*/
//=============================================================================
#ifndef __GAME_MODEL_MANAGER__
#define __GAME_MODEL_MANAGER__
#pragma once

#include "GameModel.h"

class CDisplayModel;
class CRFile;

const int GModelArraySize =	2000;
const int DModelArraySize =	20000;

const DWORD ErrorModelID = 0xFFFFFFFF;

class CGameModelManager
{

private:
	static CGameModelManager* instance;
public:
	static void InitInstance();
	static CGameModelManager* GetInstance();
	static void ReleaseInstance();
public:
	CGameModelManager(void);
	virtual ~CGameModelManager(void);

	void Initial();
	void Release();

	//DECLARE_SINGLETON(CGameModelManager);

	typedef enum eAsynReadFileType
	{
		FileType_Model=0,
		FileType_ModelShaderGroup,
		FileType_Part,
		FileType_AnimColor,
		FileType_AnimUV,
		FileType_AnimVisibility,
		FileType_AnimTexture,
		FileType_RenderMesh,
		FileType_Texture,
		FileType_Action,
		FileType_BindPose,
		FileType_Locator,
		FileType_PickMesh,
		FileType_ParticleMesh,
		FileType_ParticleEmitter,
		FileType_ParticleShaderGroup,
		FileType_TrailMesh,
		FileType_TrailEmitter,
		FileType_TextureProjective,
		FileType_TextureProjectiveShaderGroup,
		FileType_Water,								//水面文件类型
		FileType_WaterShaderGroup					//水面ShaderGroup文件类型
	}eARFT;
	typedef struct tagAsynReadFileParam
	{
		DWORD dwLoadVersion;
		//文件类型
		eARFT FileType;
		char pszFileName[MAX_PATH];
		//模型ID
		DWORD dwModelID;
		DWORD dwGroup;
		DWORD dwPart;
		DWORD dwLevel;

		int nParam;
		tagAsynReadFileParam(DWORD dwVersion,eARFT type,const char* pFileName,
			DWORD ModelID,DWORD Group=0,DWORD Part=0,DWORD Level=0,int Param=0)
			:dwLoadVersion(dwVersion),FileType(type),dwModelID(ModelID)
			,dwGroup(Group),dwPart(Part),dwLevel(Level),nParam(Param)
		{
			strncpy(pszFileName,pFileName,MAX_PATH);
		}
	}tagAsynRFParam;
private:
	
	//模型主数据和显示模型数据
	struct tagGameModelInfo
	{
		//唯一标识符
		DWORD dwID;
		//模型的路径数据
		char strFileName[MAX_PATH];
		//主模型
		GameModel* pGameModel;
		//关联的显示模型列表
		set<DWORD> DisplayModels;
		tagGameModelInfo(DWORD ID,const char* pszName,GameModel* pModel)
			:dwID(ID),pGameModel(pModel)
		{
			strncpy(strFileName,pszName,MAX_PATH);
			DisplayModels.clear();
		}
	};

	struct tagDisplayModelInfo
	{
		//唯一标示
		DWORD dwID;
		//gamemodel id
		DWORD dwGameModelID;
		//显示模型
		CDisplayModel* pDisplayModel;

		tagDisplayModelInfo(DWORD ID,DWORD dwGMID, CDisplayModel* pModel)
			:dwID(ID),dwGameModelID(dwGMID),pDisplayModel(pModel)
		{
		}
	};

	//GameModel模型数据
	tagGameModelInfo* m_GameModels[GModelArraySize];
	//DisplayModel模型数据
	tagDisplayModelInfo* m_DisplayModels[DModelArraySize];

	//typedef hash_map<DWORD,tagGameModelInfo*>	HMGameModels;
	//typedef hash_map<DWORD,tagGameModelInfo*>::iterator itGameModel;
	////正在使用的游戏模型列表
	//HMGameModels	m_GameModels;

	typedef hash_map<string,DWORD>	HMStringDword;
	typedef hash_map<string,DWORD>::iterator iHMsd;
	//从模型的字符串到ID的映射
	HMStringDword m_GameModelSToD;

	DWORD m_dwTextureProcision;
	///纹理共享文件夹
	char m_pszTextureSharFolder[MAX_PATH];
private:
	//同步创建默认的Model资源
	GameModel *LoadModel(DWORD dwModelID,const string& strModelPath,
							const string& strFileName,bool bAsyn=true);
	//装载bindpos
	BOOL LoadBindPos(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);
	//装载locator
	BOOL LoadLoactor(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);
	//装载PickMesh
	BOOL LoadPickMesh(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);
	//装载粒子参数
	BOOL LoadParticleEmitter(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);
	//装载纹理投影参数
	BOOL LoadTextureProjective(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);
	//装载水面参数
	BOOL LoadWater(GameModel * pGameModel, const string & strModelPath, bool bAsyn = true);
	//装载尾迹参数
	BOOL LoadTrailEmitter(GameModel *pGameModel,const string& strModelPath,bool bAsyn=true);

	//装载Part内容
	BOOL LoadPart(GameModel::Group::Part* pPart,
							DWORD dwModelID,DWORD dwGroup,DWORD dwPart,
							const string& strModelPath,bool bAsyn=true);
	//装载Part的某个等级数据
	BOOL LoadPartLevel(GameModel::Group::Part::Level *pLevel,
						DWORD dwModelID,DWORD dwGroup,DWORD dwPart,DWORD dwLevel,
						const string& strModelPath,bool bAsyn=true);

	model::AnimColor* LoadAnimColor(DWORD dwModelID,const string& strModelPath,
									const char* pszFileName,bool bAsyn=true);
	model::AnimUV* LoadAnimUV(DWORD dwModelID,const string& strModelPath,
									const char* pszFileName,bool bAsyn=true);
	model::AnimVisibility* LoadAnimVisibility(DWORD dwModelID,const string& strModelPath,
									const char* pszFileName,bool bAsyn=true);
	model::AnimTexture* LoadAnimTexture(DWORD dwModelID,const string& strModelPath,
									const char* pszFileName,bool bAsyn=true);

	BOOL LoadRenderMesh(DWORD dwModelID,DWORD dwGroup,DWORD dwPart,
		GameModel::Group::Part::tagRenderMesh* pRenderMesh,
						const string& strModelPath,bool bAsyn=true);

	BOOL LoadMesh(DWORD dwModelID,GameModel::Group::Part::tagRenderMesh* pRenderMesh,
						CRFile* pFile,bool bAsyn=true);

	BOOL LoadShaderInfo(DWORD dwShaderGroup,DWORD dwShaderIndex,bool bAsyn=true);

	BOOL LoadTexture(render::Interface::tagTextureInfo* pTextureInfo,
					DWORD dwTextureID,bool bAsyn=true);

	BOOL LoadAction(DWORD dwGameModelID,GameModel::Action* pAction,
					const string& strModelPath,bool bAsyn=true);

	//释放RenderMesh资源
	void ReleaseRenderMesh(DWORD dwGameModelID,
						GameModel::Group::Part::tagRenderMesh* pRenderMesh);
	//释放Shader信息
	void ReleaseShaderInfo(DWORD dwShaderGroup,DWORD dwShaerIndex);
public:
	const char * GetGameModelInfoFileName(DWORD dwID);
	//CDisplayModel* GetDisplayModel(DWORD dwID);
	GameModel* GetGameModel(DWORD dwID);
	//根据文件名得到GameModelID
	DWORD GetGameModelID(const char* pszModeFileName);

	void SetTextureShareFolder(const char* pszFolderName)
	{strncpy(m_pszTextureSharFolder,pszFolderName,MAX_PATH);}
	//根据type和graphicid,获得一个显示模型实例
	CDisplayModel* CreateDisplayModel(const string& strFileName,bool bAsyn=true);
	//释放一个显示模型
	void ReleaseDisplayModel(CDisplayModel* pDisplayModel);
	//释放所有GameModel
	void ReleaseAllDisplayModel();
	//释放单个模型
	void ReleaseGameModel(DWORD dwIndex);
	//释放所有模型
	void ReleaseAllGameModel();
	//显示某个DisplayModel的全部Group,并选择缺省的Part和等级
	void ShowDisplayModelAllGroup(CDisplayModel* pDisplayModel,bool bAsyn=true);
	//显示某个DisplayModel的全部Group,并选择缺省的Part和等级
	void ShowDisplayModelAllGroupbyLevel(CDisplayModel* pDisplayModel,DWORD dwlevel,bool bAsyn=true);
	//请求Group->Part的Lvl信息
	GameModel::Group::Part::Level *
		AcquireLevelInfo(DWORD dwModelID,DWORD dwGroup,
								 DWORD dwPart,DWORD & dwLevel,bool bAsyn=true);	
	//释放Level资源
	void ReleaseLevelInfo(DWORD dwGameModelID,DWORD dwGroup,DWORD dwPart,
							DWORD dwLevel,GameModel::Group::Part::Level *pLevel);
	
	//获得Action信息
	GameModel::Action* GetActionAnim(DWORD dwModelID,DWORD dwActionID,bool bAsyn=false);
		
	//隐藏DisplayModel的Group
	//void HideGroup(DWORD dwDisplayModelID,DWORD dwGroup);
	//设置DisplayModel的动作
	//void SetAction(DWORD dwDisplayModelID,DWORD dwActionID,BOOL bInterpolation=FALSE,bool bAsyn=false);

	void OnModelReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);
	void OnBindPosReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);
	void OnLoacatorReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);
	void OnPickMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);
	void OnShaderReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);
	void OnPartReadCompleted(tagGameModelInfo* pGameModelInfo,
							tagAsynRFParam* pParam,CRFile* prfFile);

	void OnAnimColorReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnAnimUVReadCompleted(tagGameModelInfo* pGameModelInfo,
								tagAsynRFParam* pParam,CRFile* prfFile);
	void OnAnimVisiReadCompleted(tagGameModelInfo* pGameModelInfo,
								tagAsynRFParam* pParam,CRFile* prfFile);
	void OnAnimTextureReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);

	void OnRenderMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnTextureReadCompleted(tagAsynRFParam* pParam,CRFile* prfFile);

	void OnActionReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);

	void OnParticleMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnParticleEmitterReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnParticleShaderGroupReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnTextureProjectiveReadCompleted(tagGameModelInfo* pGameModelInfo,
		tagAsynRFParam* pParam,CRFile* prfFile);
	void OnTextureProjectiveShaderGroupReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnTrailMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnTrailEmitterReadCompleted(tagGameModelInfo* pGameModelInfo,
									tagAsynRFParam* pParam,CRFile* prfFile);
	void OnWaterReadCompleted(tagGameModelInfo * pGameModelInfo, tagAsynRFParam * pParam, CRFile * prfFile);				//水面读取完成
	void OnWaterShaderGroupReadCompleted(tagGameModelInfo * pGameModelInfo, tagAsynRFParam * pParam, CRFile * prfFile);		//水面ShaderGroup读取完成

	void OnDataReadCompleted(tagAsynRFParam* pParam,CRFile* prfFile);
	//异步读取的回调函数
	static void ReadModelCallBack(CRFile* prfFile,void* pData);

//资源释放策略和接
	//主要是对Model的Group和Action信息进行管理
private:
	//释放资源间隔
	DWORD m_dwReleaseIntel;
	//当前操作的索引
	DWORD m_dwModelOperIndex;
	//当前纹理操作索引
	DWORD m_dwTextureOperIndex;
	//上一次释放时间
	DWORD m_dwLastReleaseTime;
	//资源延迟释放时间,引用计数为0时开始计时
	DWORD m_dwDelayReleaseTime;
	//上一次重新装载的时间
	DWORD m_dwLastReloadTime;
	//当前模型的装载版本
	DWORD m_dwLoadVersion;
public:
	//设置释放参数
	void SetReleaseStrategy(DWORD dwReleaseIntel,DWORD dwLastReleaseTime,DWORD dwDelayReleaseTime)
	{
		m_dwReleaseIntel = dwReleaseIntel;m_dwLastReleaseTime = dwLastReleaseTime;
		m_dwDelayReleaseTime = dwLastReleaseTime;
	}
	//释放资源
	void ReleaseModelResource();
	//重新装载所有资源
	void ReloadAllModel();

	void Run(void);
};

#endif
