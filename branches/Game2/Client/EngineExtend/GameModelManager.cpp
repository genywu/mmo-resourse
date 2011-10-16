//=============================================================================
/**
 *  file: GameModelManager.cpp
 *
 *  Brief:管理一个模型的所有资源
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-10-25
 */
//=============================================================================

#include "stdafx.h"

#include "gamemodelmanager.h"

#include "../Engine/render.h"
#include "../Engine/particle.h"
#include "textureprojective.h"
#include "DisplayModel.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static char szTempt[MAX_PATH]="";
static char szError[1024]="";

//IMPLEMENT_SINGLETON(CGameModelManager)
CGameModelManager* CGameModelManager::instance=NULL;

const char* Dw2Str(DWORD dwNum)
{
	static char strDw[5]="";
	_snprintf(strDw,5,"%c%c%c%c",dwNum&0xFF,(dwNum&0xFF00)>>8,(dwNum&0xFF0000)>>16,
		(dwNum&0xFF000000)>>24);
	strDw[4]='\0';
	return strDw;
}

void CGameModelManager::InitInstance()
{
	instance= new CGameModelManager();
}
CGameModelManager* CGameModelManager::GetInstance()
{		
	return instance;
}

void CGameModelManager::ReleaseInstance()
{
	delete instance;
}

CGameModelManager::CGameModelManager()
{
	ZeroMemory(m_GameModels,sizeof(tagGameModelInfo*)*GModelArraySize);
	ZeroMemory(m_DisplayModels,sizeof(tagDisplayModelInfo*)*DModelArraySize);

	//释放资源间隔
	m_dwReleaseIntel = 0;//2秒钟收集一次
	//上一次释放时间
	m_dwLastReloadTime = m_dwLastReleaseTime = timeGetTime();
	//资源延迟释放时间,引用计数为0时开始计时
	m_dwDelayReleaseTime =5 * 60 * 1000;//5分钟

	m_dwModelOperIndex = 1;
	m_dwTextureOperIndex= 1;

	//当前模型的装载版本
	m_dwLoadVersion = 0;
}

CGameModelManager::~CGameModelManager()
{	
}

void CGameModelManager::Initial()
{
}

void CGameModelManager::Release()
{
	//释放所有显示模型
	ReleaseAllDisplayModel();
	//释放游戏模型
	ReleaseAllGameModel();
}



/*
 * 功能: 同步创建默认的Model资源
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2009.10.20 - lpf
 *		在同步创建中增加了"装载水面效果"
 */
GameModel *CGameModelManager::LoadModel(DWORD dwModelID,const string& strModelPath,const string& strFileName,bool bAsyn)
{
	//创建
	//异步创建
	GameModel *pGameModel = NULL;
	Log4c::Trace(ROOT_MODULE,strFileName.c_str());

	if(bAsyn)
	{
		tagAsynReadFileParam* pParam = new tagAsynReadFileParam(m_dwLoadVersion,FileType_Model,"",dwModelID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		if(bRet)
		{
			pGameModel = new GameModel();
			pGameModel->SetID(dwModelID);
			pGameModel->SetLoadState(Load_Doing);
		}
	}
	//同步创建
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pGameModel = new GameModel();
			pGameModel->SetID(dwModelID);
			pGameModel->SetLoadState(Load_Did);
			pGameModel->Create(pFile);
			rfClose(pFile);
			//同步创建该Model的ShaderGroup
			_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pGameModel->GetShaderFileName());
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				pGameModel->CreateShaderGroup(pFile);
				rfClose(pFile);
				//装载BindPos
				LoadBindPos(pGameModel,strModelPath,bAsyn);
				//装载Locator
				LoadLoactor(pGameModel,strModelPath,bAsyn);
				//装载ParticleEmitter
				LoadParticleEmitter(pGameModel,strModelPath,bAsyn);
				//装载TraiEmitter
				LoadTrailEmitter(pGameModel,strModelPath,bAsyn);
				//装载投影纹理
				LoadTextureProjective(pGameModel,strModelPath,bAsyn);
				//装载水面效果
				LoadWater(pGameModel, strModelPath, bAsyn);
				//装载PickMesh
				LoadPickMesh(pGameModel,strModelPath,bAsyn);
			}else
			{
				rfClose(pFile);
				SAFEDESTROY(pGameModel);
				_snprintf(szTempt,_MAX_PATH,"%s\\%s找不到",strModelPath.c_str(),pGameModel->GetShaderFileName());
				OutputDebugStr(szTempt);
			}
		}
	}
	return pGameModel;
}

//装载bindpos
BOOL CGameModelManager::LoadBindPos(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{
	model::BindPose *pBindPos = pGameModel->GetBindPos();
	if (!pBindPos)
	{
		return FALSE;
	}
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pBindPos->GetFileName());
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam = new tagAsynReadFileParam(m_dwLoadVersion,FileType_BindPose,"",pGameModel->GetID());
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
	}
	//同步创建
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pBindPos->Create(pFile);
			rfClose(pFile);
		}
	}
	return TRUE;
}
//装载locator
BOOL CGameModelManager::LoadLoactor(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{
	map<DWORD,GameModel::Locator*>& LocatorArray = pGameModel->GetLocatorArray();
	map<DWORD,GameModel::Locator*>::iterator it = LocatorArray.begin();
	for(;it != LocatorArray.end();it++)
	{
		GameModel::Locator* pLocator = (*it).second;
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pLocator->GetFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_Locator,"",pGameModel->GetID(),0,0,0,pLocator->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
			pLocator->SetLoadState(Load_Doing);
		}
		//同步创建
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				pLocator->Create(pFile);
				rfClose(pFile);
			}
			pLocator->SetLoadState(Load_Did);
		}
	}
	return TRUE;
}
//装载PickMesh
BOOL CGameModelManager::LoadPickMesh(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{

	GameModel::PickMesh* pickmesh = pGameModel->GetPickMesh();
	if(!pickmesh)
	{
		return false;
	}
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pickmesh->GetFileName());
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_PickMesh,"",pGameModel->GetID(),0,0,0,0);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pickmesh->SetLoadState(Load_Doing);
	}
	//同步创建
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pickmesh->Create(pFile);
			rfClose(pFile);
		}
		pickmesh->SetLoadState(Load_Did);
	}

	return TRUE;
}

//装载粒子参数
BOOL CGameModelManager::LoadParticleEmitter(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{
	map<DWORD,GameModel::ParticleEmitter*>& ParticleEmitters =
										pGameModel->GetParticleEmitterArray();
	map<DWORD,GameModel::ParticleEmitter*>::iterator it = ParticleEmitters.begin();
	for(;it != ParticleEmitters.end();it++)
	{
		GameModel::ParticleEmitter* pParticleEmitter = (*it).second;
		//创建Mesh
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pParticleEmitter->GetMeshFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_ParticleMesh,"",
										pGameModel->GetID(),0,0,0,pParticleEmitter->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		}
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				pParticleEmitter->CreateMesh(pFile);
				rfClose(pFile);
			}
		}

		//创建粒子
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pParticleEmitter->GetEmitterFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_ParticleEmitter,"",
										pGameModel->GetID(),0,0,0,pParticleEmitter->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		}
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				particle::Emitter *pEmitter = pParticleEmitter->GetParticleEmitter();
				pEmitter->Create(pFile);
				rfClose(pFile);
				//同步创建该Model的ShaderGroup
				_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pEmitter->GetShaderFileName());
				pFile = rfOpen(szTempt);
				if(pFile)
				{
					pEmitter->CreateShaderGroup(pFile);
					rfClose(pFile);
					//装载Shader信息
					if(!LoadShaderInfo(pEmitter->GetShaderGroupHandle(),0,false))
					{
						Log4c::Warn(ROOT_MODULE,"CGameModelManager::LoadParticleEmitter():Load ShaderInfo (%s) failed.\n",szTempt);
					}
				}
				pParticleEmitter->SetLoadState(Load_Did);
				pEmitter->SetLoadState(Load_Did);
			}
		}
	}
	return TRUE;
}
BOOL CGameModelManager::LoadTextureProjective(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{
	map<DWORD,GameModel::TextureProjective*>& tprojs = pGameModel->GetTextureProjectiveArray();
	map<DWORD,GameModel::TextureProjective*>::iterator it = tprojs.begin();

	for(;it != tprojs.end();it++)
	{
		GameModel::TextureProjective* ptproj = (*it).second;
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),ptproj->GetTextureProjFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_TextureProjective,"",
				pGameModel->GetID(),0,0,0,ptproj->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		}
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			CTextureProjective *pcteproj = ptproj->GetTextureProjective();
			pcteproj->CreateFromFile(pFile);
			rfClose(pFile);
			//同步创建该纹理投影的ShaderGroup
			_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pcteproj->GetShaderFileName());
			pFile = rfOpen(szTempt);
			if(pFile)
			{
				pcteproj->CreateShaderGroup(pFile);
				rfClose(pFile);
				//装载Shader信息
				for (DWORD i = 0;i < pcteproj->GetMyTpSize();i++)
				{
					if( !LoadShaderInfo(pcteproj->GetShaderGroupHandle(),i),bAsyn)
					{
						Log4c::Warn(ROOT_MODULE,"CGameModelManager::OnTextureProjectiveShaderGroupReadCompleted():Load ShaderInfo (%s) failed.\n",
							pFile->GetFileName());
					}
				}
			}
			ptproj->SetLoadState(Load_Did);
			pcteproj->SetLoadState(Load_Did);

		}
	}
	return TRUE;
}



/*
* 功能: 载入水面
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.10.20
* 修改日志:
*/
BOOL CGameModelManager::LoadWater(GameModel * pGameModel, const string & strModelPath, bool bAsyn)
{
	Water::CWater * pWater = pGameModel->GetWater();
	if (pWater == NULL)
		return FALSE;

	_snprintf(szTempt, _MAX_PATH, "%s\\%s", strModelPath.c_str(), pWater->GetWaterFileName());

	if (bAsyn)
	{
		tagAsynReadFileParam * pParam = new tagAsynReadFileParam(m_dwLoadVersion, FileType_Water, "", pGameModel->GetID(), 0, 0, 0, 0);
		BOOL bRet = rfAsynOpen(szTempt, ReadModelCallBack, (void*)pParam);
		if(bRet)
			pWater->SetLoadState(Load_Doing);
		return bRet;
	}else
	{
		// 创建水面文件
		CRFile * pFile = rfOpen(szTempt);
		if (!pWater->Create(pFile))
			return FALSE;
		rfClose(pFile);

		// 创建Shader
		_snprintf(szTempt, _MAX_PATH, "%s\\%s", strModelPath.c_str(), pWater->GetShaderFileName());
		pFile = rfOpen(szTempt);
		pWater->CreateShaderGroup(pFile);
		rfClose(pFile);

		if (!LoadShaderInfo(pWater->GetShaderGroupHandle(), 0, false))
		{
			Log4c::Warn(ROOT_MODULE,"CGameModelManager::LoadWater():Load ShaderInfo (%s) failed.\n", pFile->GetFileName());
		}

		pWater->SetLoadState(Load_Did);
	}

	return TRUE;
}



//装载尾迹参数
BOOL CGameModelManager::LoadTrailEmitter(GameModel *pGameModel,const string& strModelPath,bool bAsyn)
{
	map<DWORD,GameModel::TrailEmitter*>& TrailEmitter =
						pGameModel->GetTrailEmitterArray();
	map<DWORD,GameModel::TrailEmitter*>::iterator it = TrailEmitter.begin();
	for(;it != TrailEmitter.end();it++)
	{
		GameModel::TrailEmitter* pTrailEmitter = (*it).second;
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pTrailEmitter->GetMeshFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_TrailMesh,"",
										pGameModel->GetID(),0,0,0,pTrailEmitter->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		}
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				pTrailEmitter->CreateMesh(pFile);
				LoadShaderInfo(pGameModel->GetShaderGroupHandle(),pTrailEmitter->GetMesh()->GetShaderIndex(),false);
				rfClose(pFile);
			}
		}


		_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pTrailEmitter->GetColorFileName());
		if(bAsyn)
		{
			tagAsynReadFileParam* pParam =
				new tagAsynReadFileParam(m_dwLoadVersion,FileType_TrailEmitter,"",
										pGameModel->GetID(),0,0,0,pTrailEmitter->GetName());
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		}
		else
		{
			CRFile *pFile = rfOpen(szTempt);
			if(pFile)
			{
				pTrailEmitter->CreateColor(pFile);
				rfClose(pFile);
			}
			pTrailEmitter->SetLoadState(Load_Did);
		}
	}
	return TRUE;
}

const char * CGameModelManager::GetGameModelInfoFileName(DWORD dwID)
{
	if(dwID == INVALID_ID || dwID >= GModelArraySize)	return NULL;
	return m_GameModels[dwID]->strFileName;
}

//CDisplayModel* CGameModelManager::GetDisplayModel(DWORD dwID)
//{
//	if(dwID == INVALID_ID || dwID >= DModelArraySize)	return NULL;
//	return m_DisplayModels[dwID]->pDisplayModel;
//}

GameModel* CGameModelManager::GetGameModel(DWORD dwID)
{
	if(dwID == INVALID_ID || dwID >= GModelArraySize)	return NULL;
	return m_GameModels[dwID]->pGameModel;
}

//根据文件名得到GameModelID
DWORD CGameModelManager::GetGameModelID(const char* pszModeFileName)
{
	iHMsd itSD = m_GameModelSToD.find(pszModeFileName);
	if(itSD != m_GameModelSToD.end())
	{
		return (*itSD).second;
	}
	return 0;
}



/*
* 功能: 根据type和graphicid,获得一个显示模型实例
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		在"创建DisplayModel"中,增加了"设置水面"
*/
CDisplayModel* CGameModelManager::CreateDisplayModel(const string& strFileName,bool bAsyn)
{	
	CDisplayModel* pDisplayModel = NULL;
	char szResourcePath[_MAX_PATH];
	CRFile::GetFilePath(strFileName.c_str(),szResourcePath);

	DWORD dwID=1;
	//得到或创建GameModel信息
	tagGameModelInfo* pGameModelInfo = NULL;
	iHMsd itSD = m_GameModelSToD.find(szResourcePath);
	if(itSD != m_GameModelSToD.end())
	{
		pGameModelInfo = m_GameModels[(*itSD).second];
		if(pGameModelInfo == NULL)
		{
			Log4c::Error(ROOT_MODULE,"%-15s 错误，从GameModels列表中得到%s模型为空.",__FUNCTION__,strFileName.c_str());
		}
	}
	else
	{		
		for(;dwID<GModelArraySize;dwID++)
		{
			if(m_GameModels[dwID]==NULL)
				break;
		}
		//GameModel缓冲器已满
		if(dwID==GModelArraySize)
		{
			Log4c::Warn(ROOT_MODULE,"GameModels array overflow.\n");
			return pDisplayModel;
		}
		GameModel *pGameModel = LoadModel(dwID,string(szResourcePath),strFileName,bAsyn);
		if(pGameModel)
		{
			pGameModelInfo = new tagGameModelInfo(dwID,szResourcePath,pGameModel);
			m_GameModelSToD[szResourcePath] = dwID;
			m_GameModels[dwID] = pGameModelInfo;
		}
	}

	//创建DisplayModel
	if(pGameModelInfo)
	{
		dwID = 1;
		for(;dwID<DModelArraySize;dwID++)
		{
			if(m_DisplayModels[dwID]==NULL)
				break;
		}
		//DisplayModel缓冲器已满
		if(dwID==DModelArraySize)
		{
			Log4c::Warn(ROOT_MODULE,"DisplayModels array overflow.\n");
			return pDisplayModel;
		}
		
		pDisplayModel=new CDisplayModel(this);
		pDisplayModel->SetID(dwID);
		pDisplayModel->SetGameModelID(pGameModelInfo->dwID);
		tagDisplayModelInfo* pDisplayModelInfo = new tagDisplayModelInfo(dwID,pGameModelInfo->dwID,pDisplayModel);
		m_DisplayModels[dwID] = pDisplayModelInfo;

		pGameModelInfo->DisplayModels.insert(dwID);
		pGameModelInfo->pGameModel->AddRef();

		if( pGameModelInfo->pGameModel->GetLoadState() == Load_Did )
		{
			//设置动画时间
			pDisplayModel->SetAnimCircleTime(pGameModelInfo->pGameModel->GetAnimColorCircle(),
											pGameModelInfo->pGameModel->GetAnimUVCircle(),
											pGameModelInfo->pGameModel->GetAnimVisibilityCircle(),
											pGameModelInfo->pGameModel->GetAnimTextureCircle());
			//设置ShaderGroupHandle
			pDisplayModel->SetShaderGroupHandle(pGameModelInfo->pGameModel->GetShaderGroupHandle());
			//设置BindPos和Loacator;
			pDisplayModel->SetLocatorArray(pGameModelInfo->pGameModel->GetLocatorArray());
			pDisplayModel->SetBindPos(pGameModelInfo->pGameModel->GetBindPos());
			//设置particle和trail
			pDisplayModel->SetParticleEmitters(pGameModelInfo->pGameModel->GetParticleEmitterArray());
			pDisplayModel->SetTrailEmitters(pGameModelInfo->pGameModel->GetTrailEmitterArray());
			//设置textureprojective
			pDisplayModel->SetTextureProjective(pGameModelInfo->pGameModel->GetTextureProjectiveArray());
			//设置水面
			pDisplayModel->SetWater(pGameModelInfo->pGameModel->GetWater());
			//设置SizeBox和PickBox
			pDisplayModelInfo->pDisplayModel->SetPickBox(pGameModelInfo->pGameModel->GetPickBox());
			pDisplayModelInfo->pDisplayModel->SetSizeBox(pGameModelInfo->pGameModel->GetSizeBox());
			pDisplayModelInfo->pDisplayModel->SetPickMesh(pGameModelInfo->pGameModel->GetPickMesh());
			pDisplayModelInfo->pDisplayModel->SetRendShadow(pGameModelInfo->pGameModel->GetRendShadow());
		}
		return pDisplayModel;
	}
	return pDisplayModel;
}

//释放单个模型
void CGameModelManager::ReleaseGameModel(DWORD dwIndex)
{
	if(dwIndex > 0 && dwIndex < GModelArraySize)
	{
		if(m_GameModels[dwIndex])
		{
			SAFEDESTROY(m_GameModels[dwIndex]->pGameModel);
		}
		SAFE_DELETE(m_GameModels[dwIndex]);
	}
}
//释放所有模型
void CGameModelManager::ReleaseAllGameModel()
{
	for(DWORD i=1;i<GModelArraySize;i++)
	{
		ReleaseGameModel(i);
	}
}

//释放一个显示模型
void CGameModelManager:: ReleaseDisplayModel(CDisplayModel* pDisplayModel)
{
	if(NULL == pDisplayModel)	return;

	DWORD dwID = pDisplayModel->GetID();
	tagDisplayModelInfo *pDisplayModelInfo = m_DisplayModels[dwID];
	//移除在GameModel中的引用
	tagGameModelInfo* pGameModelInfo = m_GameModels[pDisplayModelInfo->dwGameModelID];
	pGameModelInfo->DisplayModels.erase(dwID);
	pGameModelInfo->pGameModel->RemoveRef();

	//释放等级信息
	CDisplayModel::mapDisplayGroups& DisplayGroups =
		pDisplayModelInfo->pDisplayModel->GetGroups();
	CDisplayModel::itDisplayGroup it = DisplayGroups.begin();
	for(;it != DisplayGroups.end();it++)
	{
		GameModel::Group::Part::Level *pLevel = (*it).second->GetLevelInfo();
		ReleaseLevelInfo(pDisplayModelInfo->dwGameModelID,(*it).first,(*it).second->GetPartName(),
							(*it).second->GetPartLevel(),pLevel);
	}

	//删除
	m_DisplayModels[dwID]=NULL;
	SAFE_DELETE(pDisplayModelInfo->pDisplayModel) ;
	SAFE_DELETE(pDisplayModelInfo);
}

//释放所有GameModel
void CGameModelManager::ReleaseAllDisplayModel()
{
	for(DWORD i=1;i<DModelArraySize;i++)
	{
		if(m_DisplayModels[i])
			ReleaseDisplayModel(m_DisplayModels[i]->pDisplayModel);
		m_DisplayModels[i] = NULL;
	}
}

//装载Part内容
BOOL CGameModelManager::LoadPart(GameModel::Group::Part* pPart,
						DWORD dwModelID,DWORD dwGroup,DWORD dwPart,
						const string& strModelPath,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pPart->GetFileName());
	//异步创建
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_Part,"",dwModelID,dwGroup,dwPart);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pPart->SetLoadState(Load_Doing);
	}
	//同步创建
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pPart->Create(pFile);
			rfClose(pFile);
		}
		pPart->SetLoadState(Load_Did);
	}
	return TRUE;
}

//装载Part的某个等级数据
BOOL CGameModelManager::LoadPartLevel(GameModel::Group::Part::Level *pLevel,
						DWORD dwModelID,DWORD dwGroup,DWORD dwPart,DWORD dwLevel,
						const string& strModelPath,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	//_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pPart->GetFileName());
	GameModel* pGameModel = m_GameModels[dwModelID]->pGameModel;
	//装载animcolor
	if( strcmp(pLevel->GetColorFileName() ,"") != 0)
	{
		model::AnimColor *pAnimColor = pGameModel->GetAnimColor(pLevel->GetColorFileName());
		if(!pAnimColor)
		{
			pAnimColor = LoadAnimColor(dwModelID,strModelPath,pLevel->GetColorFileName(),bAsyn);
		}
		pLevel->SetAnimColor(pAnimColor);
	}
	//装载ainmuv
	if( strcmp(pLevel->GetUVFileName() ,"") != 0 )
	{
		model::AnimUV *pAnimUV = pGameModel->GetAnimUV(pLevel->GetUVFileName());
		if(!pAnimUV)
		{
			pAnimUV = LoadAnimUV(dwModelID,strModelPath,pLevel->GetUVFileName(),bAsyn);
		}
		pLevel->SetAnimUV(pAnimUV);
	}
	//装载ainmvisibiltiy
	if( strcmp(pLevel->GetVisiFileName() ,"") != 0 )
	{
		model::AnimVisibility *pAnimVisibility = pGameModel->GetAnimVisi(pLevel->GetVisiFileName());
		if(!pAnimVisibility)
		{
			pAnimVisibility = LoadAnimVisibility(dwModelID,strModelPath,pLevel->GetVisiFileName(),bAsyn);
		}
		pLevel->SetAnimVisibility(pAnimVisibility);
	}
	//装载ainmtexture
	if(strcmp(pLevel->GetTexFileName() ,"") != 0)
	{
		model::AnimTexture *pAnimTexture = pGameModel->GetAnimTexture(pLevel->GetTexFileName());
		if(!pAnimTexture)
		{
			pAnimTexture = LoadAnimTexture(dwModelID,strModelPath,pLevel->GetTexFileName(),bAsyn);
		}
		pLevel->SetAnimTexture(pAnimTexture);
	}
	//装载该等级需要的Mesh
	GameModel::Group::Part::LevelMeshes& RenderMeshes = pLevel->GetRMArray();
	GameModel::Group::Part::itLevelMesh it = RenderMeshes.begin();
	for(;it != RenderMeshes.end();it++)
	{
		GameModel::Group::Part::tagRenderMesh* pRenderMesh = (*it);
		if(pRenderMesh->GetLoadState() == Load_Not)
		{
			LoadRenderMesh(dwModelID,dwGroup,dwPart,pRenderMesh,strModelPath,bAsyn);
		}
	}	
	return TRUE;
}

model::AnimColor* CGameModelManager::LoadAnimColor(DWORD dwModelID,const string& strModelPath,
												   const char* pszFileName,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pszFileName);

	GameModel* pGameModel = m_GameModels[dwModelID]->pGameModel;
	model::AnimColor* pAnimColor = new model::AnimColor();
	pAnimColor->SetLoadState(Load_Not);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_AnimColor,pszFileName,dwModelID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pAnimColor->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pAnimColor->Create(pFile);
			rfClose(pFile);
		}
		pAnimColor->SetLoadState(Load_Did);
	}
	pGameModel->SetAnimColor(pszFileName,pAnimColor);
	return pAnimColor;
}

model::AnimUV* CGameModelManager::LoadAnimUV(DWORD dwModelID,const string& strModelPath,
											 const char* pszFileName,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pszFileName);

	GameModel* pGameModel = m_GameModels[dwModelID]->pGameModel;
	model::AnimUV* pAnimUV = new model::AnimUV();
	pAnimUV->SetLoadState(Load_Not);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_AnimUV,pszFileName,dwModelID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pAnimUV->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pAnimUV->Create(pFile);
			rfClose(pFile);
		}
		pAnimUV->SetLoadState(Load_Did);
	}
	pGameModel->SetAnimUV(pszFileName,pAnimUV);
	return pAnimUV;
}
model::AnimVisibility* CGameModelManager::LoadAnimVisibility(DWORD dwModelID,const string& strModelPath,
															 const char* pszFileName,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pszFileName);

	GameModel* pGameModel = m_GameModels[dwModelID]->pGameModel;
	model::AnimVisibility* pAnimVisibility = new model::AnimVisibility();
	pAnimVisibility->SetLoadState(Load_Not);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_AnimVisibility,pszFileName,dwModelID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pAnimVisibility->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pAnimVisibility->Create(pFile);
			rfClose(pFile);
		}
		pAnimVisibility->SetLoadState(Load_Did);
	}
	pGameModel->SetAnimVisi(pszFileName,pAnimVisibility);
	return pAnimVisibility;
}
model::AnimTexture* CGameModelManager::LoadAnimTexture(DWORD dwModelID,const string& strModelPath,
													   const char* pszFileName,bool bAsyn)
{
	static char szTempt[MAX_PATH]="";
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pszFileName);

	GameModel* pGameModel = m_GameModels[dwModelID]->pGameModel;
	model::AnimTexture* pAnimTexture = new model::AnimTexture();
	pAnimTexture->SetLoadState(Load_Not);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_AnimTexture,pszFileName,dwModelID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pAnimTexture->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			pAnimTexture->Create(pFile);
			rfClose(pFile);
		}
		pAnimTexture->SetLoadState(Load_Did);
	}
	pGameModel->SetAnimTexture(pszFileName,pAnimTexture);
	return pAnimTexture;

}

BOOL CGameModelManager::LoadRenderMesh(DWORD dwModelID,DWORD dwGroup,DWORD dwPart,
									   GameModel::Group::Part::tagRenderMesh* pRenderMesh,
									   const string& strModelPath,bool bAsyn)
{
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pRenderMesh->pszMeshFileName);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_RenderMesh,"",
			dwModelID,dwGroup,dwPart,0,(int)pRenderMesh->wIndex);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pRenderMesh->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{			
			LoadMesh(dwModelID,pRenderMesh,pFile,bAsyn);
			rfClose(pFile);
		}
	}
	return TRUE;
}

BOOL CGameModelManager::LoadMesh(DWORD dwModelID,GameModel::Group::Part::tagRenderMesh* pRenderMesh,
								 CRFile* pFile,bool bAsyn)
{
	//读Mesh文件出错
	if(NULL == pFile)
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::LoadMesh():Load Mesh File (%s\\%s) failed",
            __FUNCTION__,m_GameModels[dwModelID]->strFileName,pRenderMesh->pszMeshFileName);
		return FALSE;
	}
	pRenderMesh->pMesh = new model::Mesh;
	if (!pRenderMesh->pMesh->Create(pFile))		
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::LoadMesh():Create Mesh File (%s\\%s) failed",
            __FUNCTION__,m_GameModels[dwModelID]->strFileName,pRenderMesh->pszMeshFileName);
		MessageBox(NULL,szError,"Error",MB_OK);
	}
	//建立缓冲
	pRenderMesh->pBuffer = new model::Buffer;
	pRenderMesh->pBuffer->Create(pRenderMesh->pMesh);
	//判断该Mesh使用shader的纹理是否已经装载
	DWORD dwShaderGroup = m_GameModels[dwModelID]->pGameModel->GetShaderGroupHandle();
	DWORD dwShaderIndex = pRenderMesh->pMesh->GetShaderIndex();
	pRenderMesh->SetLoadState(Load_Did);

	if( !LoadShaderInfo(dwShaderGroup,dwShaderIndex,bAsyn) )
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::LoadMesh():Load ShaderInfo (%s) faile",
            __FUNCTION__,pFile->GetFileName());
	}

	//重新计算GameModel的BoundingBox
	m_GameModels[dwModelID]->pGameModel->AutoMakeBoundingBox();
	return TRUE;
}



/*
* 功能: 载入Shader信息
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		增加了"创建1层纹理",水面渲染需要两层纹理
*/
BOOL CGameModelManager::LoadShaderInfo(DWORD dwShaderGroup,DWORD dwShaderIndex,bool bAsyn)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();

	render::Interface::Layer3D::ShaderGroup *pShaderGroup =
		pLayer3D->GetShaderGroup(dwShaderGroup);
	if(NULL == pShaderGroup) return FALSE;

	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader =
		pShaderGroup->GetShader(dwShaderIndex);
	if(NULL == pShader)	return FALSE;

	DWORD dwTexture0 = pShaderGroup->GetTextureHandle(0,pShader->tLayer[0].dwTexture);
	DWORD dwTexture1 = pShaderGroup->GetTextureHandle(1,pShader->tLayer[1].dwTexture);

	render::Interface::tagTextureInfo* pTextureInfo0 = pInterface->GetTextureInfo(dwTexture0);
	render::Interface::tagTextureInfo* pTextureInfo1 = pInterface->GetTextureInfo(dwTexture1);

	//创建0层纹理
	if(pTextureInfo0 && pTextureInfo0->GetLoadState() == Load_Not)
	{
		LoadTexture(pTextureInfo0,dwTexture0,bAsyn);
	}
	//创建1层纹理
	if(pTextureInfo1 && pTextureInfo1->GetLoadState() == Load_Not)
	{
		LoadTexture(pTextureInfo1,dwTexture1,bAsyn);
	}
	return TRUE;
}



BOOL CGameModelManager::LoadTexture(render::Interface::tagTextureInfo* pTextureInfo,DWORD dwTextureID,bool bAsyn)
{
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",m_pszTextureSharFolder,pTextureInfo->pszFileName);
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_Texture,"",0,0,0,0,(int)dwTextureID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);

		pTextureInfo->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			render::Interface *pInterface = render::Interface::GetInstance();
			pTextureInfo->pTexture = pInterface->CreateTexture(pFile,m_dwTextureProcision);
			rfClose(pFile);
		}
		else
		{
			Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::LoadTexture():Load texture (%s) failed",__FUNCTION__,szTempt);
			wsprintf(szError,"客户端缺少纹理文件：%s",szTempt);
			MessageBox(NULL,szError,"warning",MB_OK);
		}
		pTextureInfo->SetLoadState(Load_Did);
	}
	return TRUE;
}

BOOL CGameModelManager::LoadAction(DWORD dwGameModelID,GameModel::Action* pAction,
								   const string& strModelPath,bool bAsyn)
{
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",strModelPath.c_str(),pAction->GetAnimJoinFileName());
	if(bAsyn)
	{
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_Action,"",dwGameModelID,0,0,0,(int)pAction->GetName());
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		pAction->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szTempt);
		if(pFile)
		{
			model::AnimJoint *pAnimJoint = new model::AnimJoint();
			pAnimJoint->Create(pFile);
			pAction->SetAnimJoint(pAnimJoint);
		}
		rfClose(pFile);
		pAction->SetLoadState(Load_Did);
	}
	return TRUE;
}

//释放Level资源
void CGameModelManager::ReleaseLevelInfo(DWORD dwGameModelID,DWORD dwGroup,DWORD dwPart,
										 DWORD dwLevel,GameModel::Group::Part::Level *pLevel)
{
	if(NULL == pLevel)	return;

	tagGameModelInfo* pGameModelInfo = m_GameModels[dwGameModelID];
	if(NULL==pGameModelInfo) return;

	//得到要设置的Part
	GameModel::Group* pGroup = pGameModelInfo->pGameModel->GetGroup(dwGroup);
	if(NULL == pGroup)	return;
	//如果Part是0,选择默认Part
	GameModel::Group::Part* pPart = pGroup->GetPart(dwPart);
	if(NULL == pPart)	return;

	int nRef = pPart->RemoveRef();

}

//释放RenderMesh资源
void CGameModelManager::ReleaseRenderMesh(DWORD dwGameModelID,
										  GameModel::Group::Part::tagRenderMesh* pRenderMesh)
{
	if(NULL == pRenderMesh)	return;
	//释放引用计数
}

//释放Shader信息
void CGameModelManager::ReleaseShaderInfo(DWORD dwShaderGroup,DWORD dwShaerIndex)
{
	//释放引用纹理计数
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer3D::ShaderGroup *pShaderGroup =
									pLayer3D->GetShaderGroup(dwShaderGroup);
	render::Interface::Layer3D::ShaderGroup::_tagShader *pShader =
									pShaderGroup->GetShader(dwShaerIndex);
	/*DWORD dwTexture0 = pShader->tLayer[0].dwTexture;
	DWORD dwTexture1 = pShader->tLayer[1].dwTexture;
	render::Interface::tagTextureInfo* pTextureInfo0 = pInterface->GetTextureInfo(dwTexture0);
	render::Interface::tagTextureInfo* pTextureInfo1 = pInterface->GetTextureInfo(dwTexture1);
	*/
}

//显示某个DisplayModel的全部Group,并选择缺省的Part和等级
void CGameModelManager::ShowDisplayModelAllGroup(CDisplayModel* pDisplayModel,bool bAsyn)
{
	if(NULL == pDisplayModel)	return;

	tagGameModelInfo *pGameModelInfo = m_GameModels[pDisplayModel->GetGameModelID()];
	if( pGameModelInfo->pGameModel->GetLoadState() != Load_Did)	return;
	//显示所有Group
	map<DWORD,GameModel::Group*>* pModelGroupArray = pGameModelInfo->pGameModel->GetGroupArray();
	map<DWORD,GameModel::Group*>::iterator itGroup = pModelGroupArray->begin();
	for(;itGroup != pModelGroupArray->end();itGroup++)
	{
		GameModel::Group* pGroup = (*itGroup).second;
		DWORD dwPartName = pGroup->GetDefaultPartName();
		pDisplayModel->ShowGroup(pGroup->GetName(),dwPartName,0,bAsyn);
	}
}

void CGameModelManager::ShowDisplayModelAllGroupbyLevel(CDisplayModel* pDisplayModel,DWORD dwlevel,bool bAsyn)
{
	if(NULL == pDisplayModel)	return;

	tagGameModelInfo *pGameModelInfo = m_GameModels[pDisplayModel->GetGameModelID()];
	if( pGameModelInfo->pGameModel->GetLoadState() != Load_Did)	return;
	//显示所有Group
	map<DWORD,GameModel::Group*>* pModelGroupArray = pGameModelInfo->pGameModel->GetGroupArray();
	map<DWORD,GameModel::Group*>::iterator itGroup = pModelGroupArray->begin();
	for(;itGroup != pModelGroupArray->end();itGroup++)
	{
		GameModel::Group* pGroup = (*itGroup).second;
		DWORD dwPartName = pGroup->GetDefaultPartName();
		pDisplayModel->ShowGroup(pGroup->GetName(),dwPartName,dwlevel,bAsyn);
	}
}



/*
 * 功能: 获得Group->part->levl->Rendermesh
 * 摘要: -
 * 参数: dwModelID - 模型ID
 *		 dwGroup   - 显示组名
 *		 dwPart	   - 显示部件名
 *		 dwLevel   - 需要显示的部件等级索引(从0开始,如果等于0xffffffff则为随机索引的等级信息,注意最终得到的等级信息也从这里返回)
 *		 bAsyn	   - 是否异步载入
 * 返回值: 查询获得的等级信息
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.25 - lpf
 *		修改了参数dwLevel类型，如果传入的是0xffffffff，则该参数在函数执行后会返回随机等级的结果
 */
GameModel::Group::Part::Level *
CGameModelManager::AcquireLevelInfo(DWORD dwModelID,DWORD dwGroup,
									DWORD dwPart,DWORD & dwLevel,bool bAsyn)
{
	GameModel::Group::Part::Level *pLevel = NULL;
	//得到GameModel
	tagGameModelInfo* pGameModelInfo = m_GameModels[dwModelID];
	if(NULL==pGameModelInfo) return pLevel;
	if(pGameModelInfo->pGameModel->GetLoadState() != Load_Did)	return pLevel;
	//得到要设置的Part
	GameModel::Group* pGroup = pGameModelInfo->pGameModel->GetGroup(dwGroup);
	if(NULL == pGroup)
	{
		Log4c::Error(ROOT_MODULE,"%-15s Load Group info Error!(Model:%s,Group:%s)",
            __FUNCTION__,pGameModelInfo->strFileName,Dw2Str(dwGroup));
		return pLevel;
	}
	//如果Part是0,选择默认Part
	if(dwPart == 0)
		dwPart = pGroup->GetDefaultPartName();
	GameModel::Group::Part* pPart = pGroup->GetPart(dwPart);
	if(NULL == pPart)
	{
		Log4c::Error(ROOT_MODULE,"%-15s Load Part info Error!(Model:%s,Group:%s,Part:%s)",
            __FUNCTION__,pGameModelInfo->strFileName,Dw2Str(dwGroup),Dw2Str(dwPart));
		return pLevel;
	}

	int nRef = pPart->AddRef();
	if(pPart->GetLoadState() == Load_Not)
	{
		//没有则装载
		LoadPart(pPart,pGameModelInfo->dwID,dwGroup,dwPart,pGameModelInfo->strFileName,bAsyn);
	}
	//装载Part相应的等级
	if(pPart->GetLoadState() == Load_Did)
	{
		pLevel = pPart->GetLevel(dwLevel);
		if(pLevel)
		{
			if(pLevel->GetLoadState() == Load_Not)
			{
				//装载某个等级的信息
				LoadPartLevel(pLevel,pGameModelInfo->dwID,dwGroup,dwPart,dwLevel,
					pGameModelInfo->strFileName,bAsyn);
				pLevel->SetLoadState(Load_Did);
			}	
		}
		else
		{
			Log4c::Error(ROOT_MODULE,"%-15s Load level info error!(Model:%s,Group:%s,Part:%d,Lvl:%d)",
                __FUNCTION__,pGameModelInfo->strFileName,Dw2Str(dwGroup),dwPart,dwLevel);
		}
	}
	return pLevel;
}



GameModel::Action* CGameModelManager::GetActionAnim(DWORD dwModelID,DWORD dwActionID,bool bAsyn)
{
	GameModel::Action* pAction = NULL;
	//得到GameModel
	tagGameModelInfo* pGameModelInfo = m_GameModels[dwModelID];
	if(NULL==pGameModelInfo || pGameModelInfo->pGameModel->GetLoadState() != Load_Did) return pAction;

	pAction= pGameModelInfo->pGameModel->GetAction(dwActionID);
	if(pAction)
	{
		if(pAction->GetLoadState() == Load_Not)
			LoadAction(pGameModelInfo->dwID,pAction,pGameModelInfo->strFileName,bAsyn);
		pAction->SetNotRefTime(timeGetTime());
	}
	return pAction;
}

void CGameModelManager::ReadModelCallBack(CRFile* prfFile,void* pData)
{
	if(NULL == pData)	return;
	tagAsynRFParam* pParam = (tagAsynRFParam*)pData;

	static CGameModelManager *pModelManager = CGameModelManager::GetInstance();

	pModelManager->OnDataReadCompleted(pParam,prfFile);

	delete pParam;
	if(prfFile)
		rfClose(prfFile);
}



/*
* 功能: 异步读完成接口
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		增加了"FileType_Water"和"FileType_WaterShaderGroup"的读取
*/
void CGameModelManager::OnDataReadCompleted(tagAsynRFParam* pParam,CRFile* prfFile)
{
	if( pParam->dwLoadVersion != m_dwLoadVersion)	return;

	tagGameModelInfo* pGameModelInfo = m_GameModels[pParam->dwModelID];	
	switch(pParam->FileType)
	{
	case FileType_Model:
		OnModelReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_ModelShaderGroup:
		OnShaderReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_Part:
		OnPartReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_AnimColor:
		OnAnimColorReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_AnimUV:
		OnAnimUVReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_AnimVisibility:
		OnAnimVisiReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_AnimTexture:
		OnAnimTextureReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_RenderMesh:
		OnRenderMeshReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_Texture:
		OnTextureReadCompleted(pParam,prfFile);
		break;
	case FileType_Action:
		OnActionReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_BindPose:
		OnBindPosReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_Locator:
		OnLoacatorReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_PickMesh:
		OnPickMeshReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_ParticleMesh:
		OnParticleMeshReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_ParticleEmitter:
		OnParticleEmitterReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_ParticleShaderGroup:
		OnParticleShaderGroupReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_TextureProjective:
		OnTextureProjectiveReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_TextureProjectiveShaderGroup:
		OnTextureProjectiveShaderGroupReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_TrailMesh:
		OnTrailMeshReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_TrailEmitter:
		OnTrailEmitterReadCompleted(pGameModelInfo,pParam,prfFile);
		break;
	case FileType_Water:
		OnWaterReadCompleted(pGameModelInfo, pParam, prfFile);
		break;
	case FileType_WaterShaderGroup:
		OnWaterShaderGroupReadCompleted(pGameModelInfo, pParam, prfFile);
		break;
	default:
		break;
	}
}



/*
* 功能: 模型文件异步装载完成
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		增加了"装载水面效果"和"设定水面效果"
*/
void CGameModelManager::OnModelReadCompleted(tagGameModelInfo* pGameModelInfo,
											 tagAsynRFParam* pParam,
											 CRFile* prfFile)
{
	if(prfFile)
	{
		pGameModelInfo->pGameModel->Create(prfFile);
		pGameModelInfo->pGameModel->SetLoadState(Load_Did);
		//发送异步读取shader消息
		static char szTempt[MAX_PATH]="";
		//异步创建
		_snprintf(szTempt,_MAX_PATH,"%s\\%s",pGameModelInfo->strFileName,
			pGameModelInfo->pGameModel->GetShaderFileName());

		//异步打开Shader文件
		tagAsynReadFileParam* pParam =
			new tagAsynReadFileParam(m_dwLoadVersion,FileType_ModelShaderGroup,"",pGameModelInfo->dwID);
		BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
		if(!bRet)
		{
			OutputError("CGameModelManager::OnModelReadCompleted():fAsynOpen(%s) failed.\n",szTempt);
		}
		//装载BindPos
		LoadBindPos(pGameModelInfo->pGameModel,pGameModelInfo->strFileName);
		//装载Locator
		LoadLoactor(pGameModelInfo->pGameModel,pGameModelInfo->strFileName);
		//装载ParticleEmitter
		LoadParticleEmitter(pGameModelInfo->pGameModel,pGameModelInfo->strFileName,false);
		//装载TraiEmitter
		LoadTrailEmitter(pGameModelInfo->pGameModel,pGameModelInfo->strFileName);
		//装载投影纹理
		LoadTextureProjective(pGameModelInfo->pGameModel,pGameModelInfo->strFileName);
		//装载PickMesh
		LoadPickMesh(pGameModelInfo->pGameModel,pGameModelInfo->strFileName);
		//装载水面效果
		LoadWater(pGameModelInfo->pGameModel, pGameModelInfo->strFileName);

		set<DWORD> &DisplayModels = pGameModelInfo->DisplayModels;
		set<DWORD>::iterator it = DisplayModels.begin();
		for(;it!=DisplayModels.end();it++)
		{
			tagDisplayModelInfo* pDisplayModelInfo = m_DisplayModels[(*it)];
			if(pDisplayModelInfo)
			{
				//判断是否是显示所有Group
				if( pDisplayModelInfo->pDisplayModel->IsShowAllGroup() )
				{
					ShowDisplayModelAllGroup(pDisplayModelInfo->pDisplayModel);
				}

				//设置动画时间
				pDisplayModelInfo->pDisplayModel->SetAnimCircleTime(pGameModelInfo->pGameModel->GetAnimColorCircle(),
											pGameModelInfo->pGameModel->GetAnimUVCircle(),
											pGameModelInfo->pGameModel->GetAnimVisibilityCircle(),
											pGameModelInfo->pGameModel->GetAnimTextureCircle());
				//设置BindPos和Loacator;
				pDisplayModelInfo->pDisplayModel->SetLocatorArray(pGameModelInfo->pGameModel->GetLocatorArray());
				pDisplayModelInfo->pDisplayModel->SetBindPos(pGameModelInfo->pGameModel->GetBindPos());

				//设置particle和trail
				pDisplayModelInfo->pDisplayModel->SetParticleEmitters(pGameModelInfo->pGameModel->GetParticleEmitterArray());
				pDisplayModelInfo->pDisplayModel->SetTrailEmitters(pGameModelInfo->pGameModel->GetTrailEmitterArray());
				//设置TextureProjective
				pDisplayModelInfo->pDisplayModel->SetTextureProjective(pGameModelInfo->pGameModel->GetTextureProjectiveArray());
				//设定水面效果
				pDisplayModelInfo->pDisplayModel->SetWater(pGameModelInfo->pGameModel->GetWater());
				//设置SizeBox和PickBox和PickMesh
				pDisplayModelInfo->pDisplayModel->SetPickBox(pGameModelInfo->pGameModel->GetPickBox());
				pDisplayModelInfo->pDisplayModel->SetSizeBox(pGameModelInfo->pGameModel->GetSizeBox());
				pDisplayModelInfo->pDisplayModel->SetPickMesh(pGameModelInfo->pGameModel->GetPickMesh());
				pDisplayModelInfo->pDisplayModel->SetRendShadow(pGameModelInfo->pGameModel->GetRendShadow());

				CDisplayModel::mapDisplayGroups& DisplayGroups =
										pDisplayModelInfo->pDisplayModel->GetGroups();
				CDisplayModel::itDisplayGroup itDG = DisplayGroups.begin();
				for(;itDG != DisplayGroups.end();itDG++)
				{
					CDisplayModel::CDisplayGroup* pDisplayGroup = (*itDG).second;
					//装载需要显示的Group->Part
					DWORD dwGroup = (*itDG).first;
					DWORD dwPart = pDisplayGroup->GetPartName();
					//得到要设置的Part
					GameModel::Group* pGroup = pGameModelInfo->pGameModel->GetGroup(dwGroup);
					if(pGroup)
					{
						//如果Part是0,那么选择默认Part
						if(0 == dwPart)
							dwPart = pGroup->GetDefaultPartName();
						GameModel::Group::Part* pPart = pGroup->GetPart(dwPart);
						if(pPart)
						{
							int nRef = pPart->AddRef();

							if(pPart->GetLoadState() == Load_Not)
							{
								LoadPart(pPart,pGameModelInfo->dwID,dwGroup,dwPart,
									pGameModelInfo->strFileName);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		OutputError("CGameModelManager::OnModelReadCompleted()failed.(NULL == prfFile)\n");
	}
}

void CGameModelManager::OnBindPosReadCompleted(tagGameModelInfo* pGameModelInfo,
											   tagAsynRFParam* pParam,CRFile* prfFile)
{
	model::BindPose *pBindPos = pGameModelInfo->pGameModel->GetBindPos();
	pBindPos->Create(prfFile);
}

void CGameModelManager::OnLoacatorReadCompleted(tagGameModelInfo* pGameModelInfo,
												tagAsynRFParam* pParam,CRFile* prfFile)
{
	GameModel::Locator* pLocator = pGameModelInfo->pGameModel->GetLocator(pParam->nParam);
	if(pLocator && prfFile)
	{
		if( !pLocator->Create(prfFile) )
		{
			//错误
			Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnLoacatorReadCompleted():Create Loacator (%s) failed",
                __FUNCTION__,prfFile->GetFileName());
		}
		pLocator->SetLoadState(Load_Did);
	}
}
void CGameModelManager::OnPickMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
												tagAsynRFParam* pParam,CRFile* prfFile)
{
	GameModel::PickMesh* ppickmesh = pGameModelInfo->pGameModel->GetPickMesh();
	if(ppickmesh && prfFile)
	{
		if( !ppickmesh->Create(prfFile) )
		{
			//错误
			Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnPickMeshReadCompleted():Create PickMesh (%s) failed",
                __FUNCTION__,prfFile->GetFileName());
		}
		ppickmesh->SetLoadState(Load_Did);
	}
}

//Shader文件装载完成
void CGameModelManager::OnShaderReadCompleted(tagGameModelInfo* pGameModelInfo,
											  tagAsynRFParam* pParam,
											  CRFile* prfFile)
{
	if(prfFile)
	{
		pGameModelInfo->pGameModel->CreateShaderGroup(prfFile);
		DWORD dwShaderGroupHandel = pGameModelInfo->pGameModel->GetShaderGroupHandle();
		//设置已经分配的DisplayModel的ShanderGroupHandel
		set<DWORD>::iterator it = pGameModelInfo->DisplayModels.begin();
		for(;it !=  pGameModelInfo->DisplayModels.end();it++)
		{
			m_DisplayModels[(*it)]->pDisplayModel->SetShaderGroupHandle(dwShaderGroupHandel);
		}
	}
	else
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnLoacatorReadCompleted():Create Loacator (Model:%s) failed.",
			__FUNCTION__,pGameModelInfo->strFileName);
	}
}

//某个Part文件装载完成
void CGameModelManager::OnPartReadCompleted(tagGameModelInfo* pModelInfo,
											tagAsynRFParam* pParam,
											CRFile* prfFile)
{
	//得到要设置的Part
	GameModel::Group* pGroup = pModelInfo->pGameModel->GetGroup(pParam->dwGroup);
	if(NULL == pGroup)	return;
	GameModel::Group::Part* pPart = pGroup->GetPart(pParam->dwPart);
	if(NULL == pPart)	return;
	pPart->SetLoadState(Load_Did);
	if(NULL == prfFile)
	{
		Log4c::Error(ROOT_MODULE,"%-15s Load resource of part error!(Model:%s,Group:%s,Part:%s)",
            __FUNCTION__,pModelInfo->strFileName,Dw2Str(pParam->dwGroup),Dw2Str(pParam->dwPart));
		return;
	}
	//创建Part的模型数据
	pPart->Create(prfFile);

	//搜索各个Display,察看是否有使用该Part的等级信息
	set<DWORD> &DisplayModels = pModelInfo->DisplayModels;
	set<DWORD>::iterator it = DisplayModels.begin();
	for(;it!=DisplayModels.end();it++)
	{
		tagDisplayModelInfo* pDisplayModelInfo = m_DisplayModels[(*it)];
		if(pDisplayModelInfo &&
			pDisplayModelInfo->pDisplayModel->IsGroupPartShow(pParam->dwGroup,pParam->dwPart))
		{
			DWORD dwLevel = pDisplayModelInfo->pDisplayModel->GetLvlOfGroupPart(pParam->dwGroup,pParam->dwPart);
			GameModel::Group::Part::Level *pLevel = pPart->GetLevel(dwLevel);
			if(pLevel)
			{
				if(pLevel->GetLoadState() == Load_Not)
				{
					//装载某个等级的信息
					LoadPartLevel(pLevel,pModelInfo->dwID,pParam->dwGroup,pParam->dwPart,dwLevel,
						pModelInfo->strFileName);
					pLevel->SetLoadState(Load_Did);
				}
			}
			else
			{
				Log4c::Error(ROOT_MODULE,"%-15s 取模型等级信息出错!(Model:%s,Group:%s,Part:%d,Lvl:%d)",
                    __FUNCTION__,pModelInfo->strFileName,Dw2Str(pParam->dwGroup),pParam->dwPart,dwLevel);
			}
			pDisplayModelInfo->pDisplayModel->SetLeveInfo(pParam->dwGroup,pParam->dwPart,dwLevel,pLevel);
		}
	}

}

//颜色动画读取完毕
void CGameModelManager::OnAnimColorReadCompleted(tagGameModelInfo* pGameModelInfo,
												 tagAsynRFParam* pParam, CRFile* prfFile)
{
	model::AnimColor *pAnimColor =
		pGameModelInfo->pGameModel->GetAnimColor(pParam->pszFileName);
	if(NULL == pAnimColor)	return;
	pAnimColor->Create(prfFile);
	pAnimColor->SetLoadState(Load_Did);
}

//UV动画读取完毕
void CGameModelManager::OnAnimUVReadCompleted(tagGameModelInfo* pGameModelInfo,
											  tagAsynRFParam* pParam,CRFile* prfFile)
{
	model::AnimUV *pAnimUV =
		pGameModelInfo->pGameModel->GetAnimUV(pParam->pszFileName);
	if(NULL == pAnimUV)	return;
	pAnimUV->Create(prfFile);
	pAnimUV->SetLoadState(Load_Did);
}

//模型可见动画读取完毕
void CGameModelManager::OnAnimVisiReadCompleted(tagGameModelInfo* pGameModelInfo,
												tagAsynRFParam* pParam,CRFile* prfFile)
{
	model::AnimVisibility *pAnimVisibility =
		pGameModelInfo->pGameModel->GetAnimVisi(pParam->pszFileName);
	if(NULL == pAnimVisibility)	return;
	pAnimVisibility->Create(prfFile);
	pAnimVisibility->SetLoadState(Load_Did);
}

//纹理动画读取完毕
void CGameModelManager::OnAnimTextureReadCompleted(tagGameModelInfo* pGameModelInfo,
												   tagAsynRFParam* pParam,CRFile* prfFile)
{
	model::AnimTexture *pAnimTexture =
		pGameModelInfo->pGameModel->GetAnimTexture(pParam->pszFileName);
	if(NULL == pAnimTexture)	return;
	pAnimTexture->Create(prfFile);
	pAnimTexture->SetLoadState(Load_Did);
}

//RenderMesh读取完毕
void CGameModelManager::OnRenderMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
												  tagAsynRFParam* pParam,CRFile* prfFile)
{
	//得到GameModel
	//得到要设置的Part
	GameModel::Group* pGroup = pGameModelInfo->pGameModel->GetGroup(pParam->dwGroup);
	if(NULL == pGroup)	return;
	GameModel::Group::Part* pPart = pGroup->GetPart(pParam->dwPart);
	if(NULL == pPart)	return;
	GameModel::Group::Part::tagRenderMesh* pRenderMesh = pPart->GetRenderMesh(pParam->nParam);
	if(NULL == pRenderMesh) return;

	LoadMesh(pParam->dwModelID,pRenderMesh,prfFile);
}

//纹理读取完毕
void CGameModelManager::OnTextureReadCompleted(tagAsynRFParam* pParam,CRFile* prfFile)
{

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::tagTextureInfo* pTextureInfo = pInterface->GetTextureInfo(pParam->nParam);
	if(NULL == pTextureInfo)	return;

	if(prfFile)
		pTextureInfo->pTexture = pInterface->CreateTexture(prfFile,m_dwTextureProcision);
	else
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnTextureReadCompleted():Load texture (%s) failed.",__FUNCTION__,pTextureInfo->pszFileName);
	}
	pTextureInfo->SetLoadState(Load_Did);

}

//动作读取完毕
void CGameModelManager::OnActionReadCompleted(tagGameModelInfo* pGameModelInfo,
											  tagAsynRFParam* pParam,CRFile* prfFile)
{
	GameModel::Action *pAction = pGameModelInfo->pGameModel->GetAction(pParam->nParam);
	if(NULL == pAction)	return;

	model::AnimJoint *pAnimJoint = new model::AnimJoint();
	pAnimJoint->Create(prfFile);
	pAction->SetAnimJoint(pAnimJoint);
	pAction->SetLoadState(Load_Did);

	////设置动画信息
	////搜索各个Display,察看是否有使用该Part的等级信息
	//set<DWORD> &DisplayModels = pGameModelInfo->DisplayModels;
	//set<DWORD>::iterator it = DisplayModels.begin();
	//for(;it!=DisplayModels.end();it++)
	//{
	//	tagDisplayModelInfo* pDisplayModelInfo = m_DisplayModels[(*it)];
	//	if(pDisplayModelInfo &&
	//		pDisplayModelInfo->pDisplayModel->GetCurNumAction() == pAction->GetName() )
	//	{
	//		pAction->AddRef();
	//		pDisplayModelInfo->pDisplayModel->SetCurAction(pAction);
	//	}
	//}
}

//粒子Mesh读取完毕
void CGameModelManager::OnParticleMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
													tagAsynRFParam* pParam,CRFile* prfFile)
{
	if(prfFile)
	{
		GameModel::ParticleEmitter* pParticleEmitter =
			pGameModelInfo->pGameModel->GetParticleEmitter(pParam->nParam);
		if(pParticleEmitter)
			pParticleEmitter->CreateMesh(prfFile);
	}
	else
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnParticleMeshReadCompleted():Load particle mesh(modelname:%s) failed",__FUNCTION__,pGameModelInfo->strFileName);
	}
}

//粒子参数读取完毕
void CGameModelManager::OnParticleEmitterReadCompleted(tagGameModelInfo* pGameModelInfo,
													   tagAsynRFParam* pParam,CRFile* prfFile)
{
	if(NULL == prfFile)	return;

	GameModel::ParticleEmitter* pParticleEmitter =
		pGameModelInfo->pGameModel->GetParticleEmitter(pParam->nParam);
	if(NULL == pParticleEmitter) return;

	particle::Emitter *pEmitter = pParticleEmitter->GetParticleEmitter();
	pEmitter->Create(prfFile);

	pParticleEmitter->SetLoadState(Load_Did);

	//异步创建该粒子的ShaderGroup
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",pGameModelInfo->strFileName,
		pEmitter->GetShaderFileName());

	tagAsynReadFileParam* pReadParam =
		new tagAsynReadFileParam(m_dwLoadVersion,FileType_ParticleShaderGroup,"",pGameModelInfo->dwID,0,0,0,pParam->nParam);
	BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pReadParam);
	if(!bRet)
	{
		OutputError("CGameModelManager::OnModelReadCompleted():fAsynOpen(%s) failed.\n",szTempt);
	}
}
//纹理投影参数读取完毕
void CGameModelManager::OnTextureProjectiveReadCompleted(tagGameModelInfo* pGameModelInfo,
														 tagAsynRFParam* pParam,CRFile* prfFile)
{
	GameModel::TextureProjective* ptexproj =
		pGameModelInfo->pGameModel->GetTextureProjectiveArray().begin()->second;
	if(NULL == ptexproj) return;

	CTextureProjective *pctexproj = ptexproj->GetTextureProjective();
	pctexproj->CreateFromFile(prfFile);

	ptexproj->SetLoadState(Load_Did);

	//异步创建该投影纹理的ShaderGroup
	_snprintf(szTempt,_MAX_PATH,"%s\\%s",pGameModelInfo->strFileName,
		pctexproj->GetShaderFileName());

	tagAsynReadFileParam* pReadParam = 
		new tagAsynReadFileParam(m_dwLoadVersion,FileType_TextureProjectiveShaderGroup,"",
		pGameModelInfo->dwID,0,0,0,pParam->nParam);
	BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pReadParam);
	if(!bRet)
	{
		OutputError("CGameModelManager::OnModelReadCompleted():fAsynOpen(%s) failed.\n",szTempt);
	}
}

//粒子系统的shadergroup装载完毕
void CGameModelManager::OnParticleShaderGroupReadCompleted(tagGameModelInfo* pGameModelInfo,
														   tagAsynRFParam* pParam,CRFile* prfFile)
{
	if(NULL == prfFile)	return;
	GameModel::ParticleEmitter* pParticleEmitter =
		pGameModelInfo->pGameModel->GetParticleEmitter(pParam->nParam);
	if(NULL == pParticleEmitter) return;

	particle::Emitter *pEmitter = pParticleEmitter->GetParticleEmitter();
	pEmitter->CreateShaderGroup(prfFile);
	pEmitter->SetLoadState(Load_Did);

	//装载Shader信息
	if(!LoadShaderInfo(pEmitter->GetShaderGroupHandle(),0))
	{
		Log4c::Error(ROOT_MODULE,"%-15s CGameModelManager::OnParticleShaderGroupReadCompleted():Load ShaderInfo (%s) failed.",
            __FUNCTION__,prfFile->GetFileName());
	}
}

//投影纹理系统的shadergroup装载完毕
void CGameModelManager::OnTextureProjectiveShaderGroupReadCompleted(tagGameModelInfo* pGameModelInfo,
																	tagAsynRFParam* pParam,CRFile* prfFile)
{
	if(NULL == prfFile)	return;
	GameModel::TextureProjective* ptexproj =
		pGameModelInfo->pGameModel->GetTextureProjectiveArray().begin()->second;
	if(NULL == ptexproj) return;

	CTextureProjective *pctexproj = ptexproj->GetTextureProjective();
	pctexproj->CreateShaderGroup(prfFile);
	pctexproj->SetLoadState(Load_Did);

	//装载Shader信息
	for (DWORD i = 0;i < pctexproj->GetMyTpSize();i++)
	{
		if( !LoadShaderInfo(pctexproj->GetShaderGroupHandle(),i))
		{
			Log4c::Error(ROOT_MODULE,"%-15s Load ShaderInfo (%s) failed",__FUNCTION__,prfFile->GetFileName());
		}
	}

}

void CGameModelManager::OnTrailMeshReadCompleted(tagGameModelInfo* pGameModelInfo,
												 tagAsynRFParam* pParam,CRFile* prfFile)
{
	GameModel::TrailEmitter* pTrailEmitter = 
		pGameModelInfo->pGameModel->GetTrailEmitter(pParam->nParam);
	if(pTrailEmitter)
	{
		pTrailEmitter->CreateMesh(prfFile);
		LoadShaderInfo(pGameModelInfo->pGameModel->GetShaderGroupHandle(),pTrailEmitter->GetMesh()->GetShaderIndex());
	}
}

//尾迹读取完毕
void CGameModelManager::OnTrailEmitterReadCompleted(tagGameModelInfo* pGameModelInfo,
													tagAsynRFParam* pParam,CRFile* prfFile)
{
	if(NULL == prfFile)	return;
	GameModel::TrailEmitter* pTrailEmitter = 
		pGameModelInfo->pGameModel->GetTrailEmitter(pParam->nParam);
	if(pTrailEmitter)
	{
		pTrailEmitter->CreateColor(prfFile);
		pTrailEmitter->SetLoadState(Load_Did);
	}
}



/*
* 功能: 水面读取完成
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.10.20
* 修改日志:
*/
void CGameModelManager::OnWaterReadCompleted(tagGameModelInfo * pGameModelInfo, tagAsynRFParam * pParam, CRFile * prfFile)
{
	if (prfFile == NULL)
		return;

	Water::CWater * pWater = pGameModelInfo->pGameModel->GetWater();
	if (pWater == NULL)
		return;

	// 创建水面文件
	if (!pWater->Create(prfFile))
	{
		OutputError("CGameModelManager::OnWaterReadCompleted(): CWater::Create(%s) failed.\n",szTempt);
		return;
	}

	_snprintf(szTempt, _MAX_PATH, "%s\\%s", pGameModelInfo->strFileName, pWater->GetShaderFileName());
	tagAsynReadFileParam * pReadParam = new tagAsynReadFileParam(m_dwLoadVersion, FileType_WaterShaderGroup, "", pGameModelInfo->dwID, 0, 0, 0, pParam->nParam);
	BOOL bRet = rfAsynOpen(szTempt, ReadModelCallBack, (void*)pReadParam);
	if(!bRet)
		OutputError("CGameModelManager::OnWaterShaderGroupReadCompleted():fAsynOpen(%s) failed.\n",szTempt);

	pWater->SetLoadState(Load_Doing);
}



/*
* 功能: 水面ShaderGroup读取完成
* 摘要: -
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.10.20
* 修改日志:
*/
void CGameModelManager::OnWaterShaderGroupReadCompleted(tagGameModelInfo * pGameModelInfo, tagAsynRFParam * pParam, CRFile * prfFile)
{
	if (prfFile == NULL)
		return;

	Water::CWater * pWater = pGameModelInfo->pGameModel->GetWater();
	if (pWater == NULL)
		return;

	pWater->CreateShaderGroup(prfFile);
	if (!LoadShaderInfo(pWater->GetShaderGroupHandle(), 0))
		OutputError("CGameModelManager::OnWaterShaderGroupReadCompleted():Load ShaderInfo (%s) failed.\n", prfFile->GetFileName());

	pWater->SetLoadState(Load_Did);
}



void CGameModelManager::Run(void)
{
	ReleaseModelResource();
}


//重新装载所有资源
void CGameModelManager::ReloadAllModel()
{
	DWORD dwCurTime = timeGetTime();
	if(dwCurTime - m_dwLastReloadTime < 3000)	return;
	m_dwLastReloadTime = dwCurTime;

	m_dwLoadVersion++;

	DWORD GameModelID = 1;
	//先释放GameModel资源
	for(;GameModelID<GModelArraySize;GameModelID++)
	{
		if(m_GameModels[GameModelID])
		{
			tagGameModelInfo* pGameModelInfo = m_GameModels[GameModelID];
			GameModel* pGameModel = pGameModelInfo->pGameModel;

			//释放所有资源
			set<DWORD> &DisplayModels = pGameModelInfo->DisplayModels;
			set<DWORD>::iterator it = DisplayModels.begin();
			for(;it!=DisplayModels.end();it++)
			{
				tagDisplayModelInfo* pDisplayModelInfo = m_DisplayModels[(*it)];
				if(pDisplayModelInfo)
				{
					CDisplayModel *pDisplayModel = pDisplayModelInfo->pDisplayModel;
					pDisplayModel->Destroy();
				}
			}
			//清除资源内容
			pGameModel->Destroy();
			_snprintf(szTempt,1024,"%s\\model",pGameModelInfo->strFileName);
			//重新异步创建
			tagAsynReadFileParam* pParam = new tagAsynReadFileParam(m_dwLoadVersion,FileType_Model,"",GameModelID);
			BOOL bRet = rfAsynOpen(szTempt,ReadModelCallBack,(void*)pParam);
			pGameModel->SetLoadState(Load_Doing);
		}
	}
	//释放纹理
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->DestroyAllTexture();
}

//释放资源
//每次判断一个GameModel
void CGameModelManager::ReleaseModelResource()
{
	DWORD dwCurTime = timeGetTime();
	if(dwCurTime-m_dwLastReleaseTime < m_dwReleaseIntel )
		return;

	m_dwLastReleaseTime = dwCurTime;
	if(m_GameModels[m_dwModelOperIndex])
	{
		tagGameModelInfo* pModelInfo = m_GameModels[m_dwModelOperIndex];
		GameModel* pGameModel = pModelInfo->pGameModel;
		if(pGameModel && pGameModel->GetLoadState()  == Load_Did )
		{
			//判断GameModel
			if( pGameModel->IsShared() )
			{
				//循环Group
				map<DWORD,GameModel::Group*>* pModelGroupArray = pGameModel->GetGroupArray();
				map<DWORD,GameModel::Group*>::iterator itGroup = pModelGroupArray->begin();
				for(;itGroup != pModelGroupArray->end();itGroup++)
				{
					GameModel::Group* pGroup = (*itGroup).second;
					//循环Part
					map<DWORD,GameModel::Group::Part*> *pPartArray = pGroup->GetPartArray();
					map<DWORD,GameModel::Group::Part*>::iterator itPart = pPartArray->begin();
					for(;itPart != pPartArray->end();itPart++)
					{
						GameModel::Group::Part* pPart = (*itPart).second;
						//释放Part资源
						if( pPart->GetLoadState()  == Load_Did && 
							pPart->IsShared() == false &&
							dwCurTime - pPart->GetNotRefTime() > m_dwDelayReleaseTime)
						{
							pPart->SetLoadState(Load_Not);
							pPart->Destroy();
						}

					}
				}

				//释放动作资源
				map<DWORD,GameModel::Action*>* pActionArray = pGameModel->GetActionArray();
				map<DWORD,GameModel::Action*>::iterator itAction = pActionArray->begin();
				for(;itAction != pActionArray->end();itAction++)
				{
					GameModel::Action* pAction = (*itAction).second;
					if(pAction &&
						pAction->GetLoadState()  == Load_Did && 
						pAction->IsShared() == false &&
						dwCurTime - pAction->GetNotRefTime() > m_dwDelayReleaseTime)
					{
						pAction->SetLoadState(Load_Not);
						pAction->Destroy();
					}
				}
			}
			else if(dwCurTime - pGameModel->GetNotRefTime() > m_dwDelayReleaseTime)
			{
				//释放GameModel
				m_GameModelSToD.erase(pModelInfo->strFileName);
				SAFEDESTROY(pGameModel);
				SAFE_DELETE(pModelInfo);
				m_GameModels[m_dwModelOperIndex] = NULL;
			}
		}
	}
	//递增索引
	if(++m_dwModelOperIndex >= GModelArraySize)
		m_dwModelOperIndex = 1;

	//释放纹理资源
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = pInterface->GetLayer3D();
	render::Interface::tagTextureInfo* pTextureInfo = pInterface->GetTextureInfo(m_dwTextureOperIndex);
	if(pTextureInfo && 
		pTextureInfo->IsShared() == false &&
		dwCurTime - pTextureInfo->GetNotRefTime() > m_dwDelayReleaseTime)
	{
		pInterface->DestroyTexture(m_dwTextureOperIndex);
	}
	if(++m_dwTextureOperIndex >= MAX_TEXTURE_CACHE_SIZE)
		m_dwTextureOperIndex = 1;
}



