#include "stdafx.h"

#include "../Engine/utility.h"
#include "../Engine/console.h"

#include "gamemodelmanager.h"
#include "DisplayModel.h"

#include "gameselectray.h"
#include "gamebuildlist.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


GameBuildList* GameBuildList::intance=NULL;

void GameBuildList::InitInstance()
{
	intance = new GameBuildList();
}

GameBuildList* GameBuildList::GetInstance()
{
	return intance;
}

void GameBuildList::ReleaseInstance()
{
	delete intance;
}

GameBuildList::Build::Build()	:
m_wID(0xffff)
{
	m_pModel       = NULL;
	m_pLightSource = NULL;
	m_pSoundSource = NULL;
	m_pBlockData   = NULL;
	

}

GameBuildList::Build::~Build()
{
	ASSERT(!m_pModel);
	
}

BOOL GameBuildList::Build::Create(const char *pszFileName,DWORD dwTextureProcision)
{
	//utility::File file;
	//if (!file.Open(pszFileName))
	//{
	//	return FALSE;
	//}

	//utility::Text::Variable *pTempVar(NULL);
	//utility::Text script;
	//script.Create(&file);  
	//file.Close();

	//static char szPath[_MAX_PATH];
	//utility::File::GetFilePath(pszFileName,szPath);
	////模型
	//utility::Text::Node *pModelNode = script.GetRootNode()->GetChildNode("model");
	//if (pModelNode)
	//{
	//	m_pModel = new tagModel;   
	//	m_pModel->pGameModel = new GameModel;
	//	strcat(szPath,"\\");
	//	strcat(szPath,pModelNode->GetVar("file")->GetStringValue());
 //		if (!m_pModel->pGameModel->Create(szPath,dwTextureProcision))
	//	{
	//		OutputConsole("error:GameBuildList::Build::Create(%s) Create game model failed!\n",pszFileName);
	//		return FALSE;
	//	}
	//	//打开所有显示,统计
	//	m_pModel->pGameModel->SetGroupVisible(TRUE);
	//	m_pModel->pGameModel->SelectDefaultPart();
	//	m_pModel->pGameModel->EnableParticleEmitter(TRUE);
	//	m_pModel->pGameModel->EnableTrailEmitter(TRUE);
	//	m_pModel->dwTrianglCount = m_pModel->pGameModel->GetVisibleTriangleCount();

	//	m_pModel->bAutoHide = pModelNode->GetVar("autohide")->GetBOOLValue();
	//	m_pModel->bLighted  = pModelNode->GetVar("lighted")->GetBOOLValue();

	//	pTempVar = pModelNode->GetVar("global_timer");
	//	m_pModel->dwGlobalTimer = (DWORD)((pTempVar)? (DWORD)pTempVar->GetIntValue() : 0);
	//	m_pModel->dwGlobalTimer = (m_pModel->dwGlobalTimer <= 8) ? m_pModel->dwGlobalTimer : 0 ;
	//	if (m_pModel->pGameModel->CheckStatic())
	//	{
	//		m_pModel->dwGlobalTimer = 8;//静态物件时钟用7
	//	}
	//}

	////光源
	//utility::Text::Node *pLightSourceNode = script.GetRootNode()->GetChildNode("lightsource");
	//if (pLightSourceNode)
	//{
	//	m_pLightSource = new tagLightSource;
	//	m_pLightSource->bDynamic = pLightSourceNode->GetVar("dynmatic")->GetBOOLValue();
	//	m_pLightSource->dwColor  = pLightSourceNode->GetVar("color")->GetHexValue();
	//	m_pLightSource->fHeight  = pLightSourceNode->GetVar("height")->GetFloatValue();
	//	m_pLightSource->fRadius  = pLightSourceNode->GetVar("radius")->GetFloatValue();
	//	m_pLightSource->fMinIntensity = pLightSourceNode->GetVar("minintensity")->GetFloatValue();
	//	m_pLightSource->fMaxIntensity = pLightSourceNode->GetVar("maxintensity")->GetFloatValue();
	//	m_pLightSource->dwCycleTime   = (DWORD)pLightSourceNode->GetVar("cycletime")->GetIntValue();

	//}
	////声源
	//utility::Text::Node *pSoundSourceNode = script.GetRootNode()->GetChildNode("soundsource");
	//if (pSoundSourceNode)
	//{
	//	m_pSoundSource = new tagSoundSource;
	//	m_pSoundSource->wID = (WORD)pSoundSourceNode->GetVar("id")->GetIntValue();
	//	m_pSoundSource->fHeight  = pSoundSourceNode->GetVar("height")->GetFloatValue();
	//	m_pSoundSource->fRolloff = pSoundSourceNode->GetVar("rolloff")->GetFloatValue();
	//	m_pSoundSource->fReferenceDistance = pSoundSourceNode->GetVar("ReferenceDistance")->GetFloatValue();
	//	m_pSoundSource->fMaxGain = pSoundSourceNode->GetVar("MaxGain")->GetFloatValue();
	//	m_pSoundSource->fMinGain = pSoundSourceNode->GetVar("MinGain")->GetFloatValue();
	//}
	////阻挡
	//utility::Text::Node *pBlockNode = script.GetRootNode()->GetChildNode("block");
	//if (pBlockNode)
	//{
	//	m_pBlockData = new tagBlockData;
	//	static const char * szBlockType[] = {"normal","high"};
	//	m_pBlockData->byType = (BYTE)pBlockNode->GetVar("type")->GetEnumValue(szBlockType,2);
	//	m_pBlockData->iXMin  = pBlockNode->GetVar("xmin")->GetIntValue();
	//	m_pBlockData->iXMax  = pBlockNode->GetVar("xmax")->GetIntValue();
	//	m_pBlockData->iZMin  = pBlockNode->GetVar("zmin")->GetIntValue();
	//	m_pBlockData->iZMax  = pBlockNode->GetVar("zmax")->GetIntValue();
	//}
	//script.Destroy();

	return TRUE;
}

BOOL GameBuildList::Build::Create(CRFile* pFile,bool bAsyn)
{
	if(NULL == pFile)	return FALSE;

	CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	utility::Text::Variable *pTempVar(NULL);
	utility::Text script;
	script.Create(pFile);  

	//模型
	utility::Text::Node *pModelNode = script.GetRootNode()->GetChildNode("model");
	if (pModelNode)
	{
		char szPath[MAX_PATH]="";
		m_pModel = new tagModel;
		_snprintf(szPath,MAX_PATH,"%s\\%s",m_pszFilePath,pModelNode->GetVar("file")->GetStringValue());
		m_pModel->pGameModel = pModelManager->CreateDisplayModel(szPath,bAsyn);
 		/*if (!m_pModel->pGameModel->Create(szPath,dwTextureProcision))
		{
			OutputConsole("error:GameBuildList::Build::Create(%s) Create game model failed!\n",pszFileName);
			return FALSE;
		}*/
		//打开所有显示,统计
		//m_pModel->pGameModel->SetGroupVisible(TRUE);
		//m_pModel->pGameModel->SelectDefaultPart();
		if (m_pModel->pGameModel)
		{
			m_pModel->pGameModel->ShowAllGroup(bAsyn);
			m_pModel->pGameModel->EnableParticleEmitter(TRUE);
			m_pModel->pGameModel->EnableTrailEmitter(TRUE);
		}
		//m_pModel->dwTrianglCount = m_pModel->pGameModel->GetVisibleTriangleCount();

		m_pModel->bAutoHide = pModelNode->GetVar("autohide")?pModelNode->GetVar("autohide")->GetBOOLValue():FALSE;
		m_pModel->bLighted  = pModelNode->GetVar("lighted")->GetBOOLValue();
		m_pModel->bprojected  = (pModelNode->GetChildNode("canbeprojected") == NULL )? FALSE:TRUE;
		pTempVar = pModelNode->GetVar("projectedsize");
		if (pTempVar)
		{
			m_pModel->bprojected = true;
			m_pModel->dwprojectedsize = pTempVar->GetIntValue();
		}
		pTempVar = pModelNode->GetVar("global_timer");
		m_pModel->dwGlobalTimer = (DWORD)((pTempVar)? (DWORD)pTempVar->GetIntValue() : 0);
		m_pModel->dwGlobalTimer = (m_pModel->dwGlobalTimer <= 8) ? m_pModel->dwGlobalTimer : 0 ;
		//if (m_pModel->pGameModel->CheckStatic())
		//{
		//	m_pModel->dwGlobalTimer = 8;//静态物件时钟用7
		//}

		//有自主时钟标志
		//if (m_pModel->dwGlobalTimer == 0)
		//{
			//pGrid->pBuild->pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
			//pGrid->pBuild->pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
			//m_pModel->pGameModel->SetCurAction(MAKEFOURCC('S','T','N','D'));
		//}
	}

	//光源
	utility::Text::Node *pLightSourceNode = script.GetRootNode()->GetChildNode("lightsource");
	if (pLightSourceNode)
	{
		m_pLightSource = new tagLightSource;
		m_pLightSource->bDynamic = pLightSourceNode->GetVar("dynmatic")->GetBOOLValue();
		m_pLightSource->dwColor  = pLightSourceNode->GetVar("color")->GetHexValue();
		m_pLightSource->fHeight  = pLightSourceNode->GetVar("height")->GetFloatValue();
		m_pLightSource->fRadius  = pLightSourceNode->GetVar("radius")->GetFloatValue();
		m_pLightSource->fMinIntensity = pLightSourceNode->GetVar("minintensity")->GetFloatValue();
		m_pLightSource->fMaxIntensity = pLightSourceNode->GetVar("maxintensity")->GetFloatValue();
		m_pLightSource->dwCycleTime   = (DWORD)pLightSourceNode->GetVar("cycletime")->GetIntValue();

	}
	//声源
	utility::Text::Node *pSoundSourceNode = script.GetRootNode()->GetChildNode("soundsource");
	if (pSoundSourceNode)
	{
		m_pSoundSource = new tagSoundSource;
		m_pSoundSource->wID = (WORD)pSoundSourceNode->GetVar("id")->GetIntValue();
		m_pSoundSource->fHeight  = pSoundSourceNode->GetVar("height")->GetFloatValue();
		m_pSoundSource->fRolloff = pSoundSourceNode->GetVar("rolloff")->GetFloatValue();
		m_pSoundSource->fReferenceDistance = pSoundSourceNode->GetVar("ReferenceDistance")->GetFloatValue();
		m_pSoundSource->fMaxGain = pSoundSourceNode->GetVar("MaxGain")->GetFloatValue();
		m_pSoundSource->fMinGain = pSoundSourceNode->GetVar("MinGain")->GetFloatValue();
	}
	//阻挡
	utility::Text::Node *pBlockNode = script.GetRootNode()->GetChildNode("block");
	if (pBlockNode)
	{
		m_pBlockData = new tagBlockData;
		static const char * szBlockType[] = {"normal","high"};
		m_pBlockData->byType = (BYTE)pBlockNode->GetVar("type")->GetEnumValue(szBlockType,2);
		m_pBlockData->iXMin  = pBlockNode->GetVar("xmin")->GetIntValue();
		m_pBlockData->iXMax  = pBlockNode->GetVar("xmax")->GetIntValue();
		m_pBlockData->iZMin  = pBlockNode->GetVar("zmin")->GetIntValue();
		m_pBlockData->iZMax  = pBlockNode->GetVar("zmax")->GetIntValue();
	}
	script.Destroy();
	return TRUE;
}

void GameBuildList::Build::Destroy()
{
	static CGameModelManager* pModelManager = CGameModelManager::GetInstance();
	if (m_pModel)
	{
		pModelManager->ReleaseDisplayModel(m_pModel->pGameModel);
		m_pModel->pGameModel = NULL;
	}
	SAFEDELETE(m_pModel);
	SAFEDELETE(m_pLightSource);
	SAFEDELETE(m_pSoundSource);
	SAFEDELETE(m_pBlockData);
}
GameBuildList::Build::tagModel::tagModel()
{
	
}
GameBuildList::Build::tagModel::~tagModel()
{
	
}
GameBuildList::GameBuildList(void)
{                   	
}

GameBuildList::~GameBuildList(void)
{
}

void GameBuildList::Initial()
{
}
void GameBuildList::Release()
{
	itBuild it = m_mapBuild.begin();
	for(; it != m_mapBuild.end(); it++ )
	{
		Build* pBuild = it->second;
		SAFEDESTROY(pBuild);
	}
	m_mapBuild.clear();
}


//请求一个Build
GameBuildList::Build *GameBuildList::AcquireBuild(WORD wID,bool bAsyn)
{
	itBuild it = m_mapBuild.find(wID);
	if(it != m_mapBuild.end() )
	{
		GameBuildList::Build *pBuild = (*it).second;
		pBuild->AddRef();
		return pBuild;
	}
	
	Build *pBuild = LoadBuild(wID,bAsyn);
	if(pBuild)
	{
		pBuild->AddRef();
		m_mapBuild[wID] = pBuild;
	}
	return pBuild;
}
//释放一个Build
void GameBuildList::ReleaseBuild(WORD wID,Build* pbuild)
{
	Build* pBuild = NULL;
	itBuild it = m_mapBuild.find(wID);
	if(it != m_mapBuild.end() )
	{
		pBuild = it->second;
		if (pBuild->IsShared())
		{
			if (pBuild->RemoveRef() == 0)
			{
				SAFEDESTROY(pBuild);
				m_mapBuild.erase(it);
			}
		}
		else
		{
			SAFEDESTROY(pBuild);
			m_mapBuild.erase(it);
		}
	}
}

//同步创建默认的Model资源
GameBuildList::Build *GameBuildList::LoadBuild(WORD wID,bool bAsyn)
{
	Build *pBuild = NULL;
	char pszPath[MAX_PATH],szName[_MAX_PATH];
	_snprintf(pszPath,_MAX_PATH,"model\\build\\%1.1d\\%3.3d",(wID & 0xf000) >> 12 , wID & 0x0fff);	
	_snprintf(szName,_MAX_PATH,"%s\\build",pszPath);
	if (! CClientResource::GetInstance()->IsFileExist(szName) )
	{
		OutputError("error:建筑文件不存在 \"%s\"\n",szName);
		MessageBox(NULL,szName,"错误:建筑文件不存在",MB_OK);
		return pBuild;
	}
	pBuild = new Build();
	pBuild->SetLoadState(Load_Not);
	pBuild->SetFilePath(pszPath);
	if(bAsyn)
	{
		rfAsynOpen(szName,ReadBuilCallBack,(void*)wID);
		pBuild->SetLoadState(Load_Doing);
	}
	else
	{
		CRFile *pFile = rfOpen(szName);
		if(pFile)
		{
			pBuild->Create(pFile,false);
			rfClose(pFile);
		}
		pBuild->SetLoadState(Load_Did);
	}
	return pBuild;
}

void GameBuildList::OnReadBuildCompleted(CRFile* prfFile,void* pData)
{
#pragma warning (push)
#pragma  warning(disable:4311)
	itBuild it = m_mapBuild.find((int)pData);
#pragma warning (pop)
	if(it != m_mapBuild.end() )
	{
		GameBuildList::Build *pBuild = (*it).second;
		pBuild->Create(prfFile);
		pBuild->SetLoadState(Load_Did);
	}
	rfClose(prfFile);
}

//异步读取的回调函数
void GameBuildList::ReadBuilCallBack(CRFile* prfFile,void* pData)
{
	static GameBuildList *pBuildList = GameBuildList::GetInstance();
	pBuildList->OnReadBuildCompleted(prfFile,pData);
	
}

