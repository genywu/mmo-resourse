#include "stdafx.h"
#include <mdebug.h>
#include <mrender.h>
#include <mscript.h>
#include <mfile.h>
#include <mskinmesh.h>
#include <mskinanim.h>
#include <msphere.h>
#include <mtimer.h>
#include <mframebuffer.h>
#include "model.h"

static const char szComponentName[COMP_MAX][16] = {"head","body","hand","middle","leg","weapon"};

Model::Component::Component()
{

}

Model::Component::~Component()
{

}

BOOL Model::Component::Create(const char *szName,const char *pszFileName)
{
	MRender *pRender = MRender::GetInstance();
	strcpy(m_szName,szName);

	m_pMesh = new MSkinMesh;
	BOOL bResult = m_pMesh->Create(pszFileName);
	
	m_pFrameBuffer = new MFrameBuffer;
	m_pFrameBuffer->Create(m_pMesh);

    return bResult;
}

void Model::Component::Destroy()
{
	SAFEDESTROY(m_pFrameBuffer);
	m_pMesh->Destroy();
	SAFEDELETE(m_pMesh);
}
//====================================================================================

//===================================================================================
Model::Animation::Animation():m_pAnim(NULL)
{
}

Model::Animation::~Animation()
{
}

BOOL Model::Animation::Create(const char *szName,const char *pszFileName)
{
	strcpy(m_szName,szName);
	m_pAnim = new MSkinAnim;

	BOOL bResult = m_pAnim->Create(pszFileName);

	return bResult;
}

void Model::Animation::Destroy()
{
	SAFEDESTROY(m_pAnim);
}

Model::Model(void):m_iCurAnimation(0),m_pBodySphere(NULL),m_pFeetSphere(NULL)
{
	ZeroMemory(m_iDisplayComponent,COMP_MAX * sizeof(int));
}

Model::~Model(void)
{
}

BOOL Model::Create(const char * pszFileName)
{
	MFile file;
	if (!file.Open(pszFileName))
	{
		CONSOLE("Model: Load file failed!\n");
		return FALSE;
	}

	//得到路径
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
   _splitpath( pszFileName, drive, dir, NULL,NULL);
    _makepath( m_szModelPath,drive,dir,NULL,NULL);

	MScript script;
	script.Create((const char*)file.GetData(),file.GetSize());

	MScript *pSubScript(NULL); 
	//处理组件
	pSubScript = script.Subset("component");
	VERIFY(pSubScript);
	CreateComponent(pSubScript);
	SAFEDESTROY(pSubScript);

	//处理动画
	pSubScript = script.Subset("animation");
	VERIFY(pSubScript);
	CreateAnimation(pSubScript);
	SAFEDESTROY(pSubScript);

	//处理碰撞球
	pSubScript = script.Subset("collision");
	VERIFY(pSubScript);
	CreateCollision(pSubScript);
	SAFEDESTROY(pSubScript);

	//建立控制器
	pSubScript = script.Subset("controller");
	VERIFY(pSubScript);
	CreateController(pSubScript);
	SAFEDESTROY(pSubScript);

	script.Destroy();
	file.Close();

	//初始组件


	//初始动作
	SetAnimation("stand");


	return TRUE;
}

void Model::Destroy()
{
	DestroyController();
	DestroyCollision();
	DestroyAnimation();
	DestroyComponent();
}

void Model::CreateComponent(MScript *pScript)
{
	int i(0),iCount(0);
	char szName[DEFAULT_SCRIPT_WORD_SIZE];
	char szFileName[DEFAULT_SCRIPT_WORD_SIZE];
	char szFullPathName[_MAX_PATH];

	MScript *pSubScript(NULL); 

	for(int iType = COMP_HEAD; iType < COMP_MAX;iType++)
	{
		pSubScript = pScript->Subset(szComponentName[iType]);
		
		iCount = pSubScript->Count(".msm");
		m_iCompontCnt[iType] = iCount;
		m_pCompontList[iType] = new Component[iCount];

		for(i = 0; i < iCount; i++)
		{
			pSubScript->ReadWord(szName);
			pSubScript->ReadWord(szFileName);
			Component &comp = m_pCompontList[iType][i];
			strcpy(szFullPathName,m_szModelPath);
			strcat(szFullPathName,szFileName);
			comp.Create(szName,szFullPathName);
		}
		pSubScript->Destroy();
		SAFEDELETE(pSubScript);
	}	
}

void Model::DestroyComponent()
{
	int i(0),j(0);
	//destroy component
	for(i = COMP_HEAD; i < COMP_MAX;i++)
	{
		for(j = 0; j < m_iCompontCnt[i];j++)
		{
			m_pCompontList[i][j].Destroy();
		}
		SAFEDELETE_ARRAY(m_pCompontList[i]);
	}

}

void Model::CreateAnimation(MScript * pScript)
{
	char szName[DEFAULT_SCRIPT_WORD_SIZE];
	char szFileName[DEFAULT_SCRIPT_WORD_SIZE];
	char szFullPathName[_MAX_PATH];

	m_iAnimationCnt = pScript->Count(".msa");
	m_pAnimationList = new Animation[m_iAnimationCnt];

	for(int i = 0; i < m_iAnimationCnt; i++)
	{
		pScript->ReadWord(szName);
		pScript->ReadWord(szFileName);
		Animation &anim = m_pAnimationList[i];
		strcpy(szFullPathName,m_szModelPath);
		strcat(szFullPathName,szFileName);
		VERIFY(anim.Create(szName,szFullPathName));
	}
}

void Model::DestroyAnimation()
{
	for(int i = 0; i < m_iAnimationCnt; i++)
	{
		Animation &anim = m_pAnimationList[i];
		anim.Destroy();
	}
	SAFEDELETE_ARRAY(m_pAnimationList);
}


void Model::RenderModel(MRender * pRender)
{
	//根据动画控制器
	int iFrame = m_pController->GetFrame();
	//动画选择表
	MSkinAnim *pAnim = m_pAnimationList[m_iCurAnimation].GetAnim();
	//设置当前动画数据的取出帧数
	pAnim->SetCurFrame(iFrame);
	
	D3DXMATRIX matInt;
	D3DXMatrixIdentity(&matInt);

	//根据显示组件表取得组件
	for(int i = COMP_HEAD; i < COMP_MAX; i++)
	{
		int iDisplay = m_iDisplayComponent[i];
		if (iDisplay < m_iCompontCnt[i])
		{
			//进行组合变形渲染
			MSkinMesh *pMesh = m_pCompontList[i][iDisplay].GetMesh();

			MFrameBuffer *pFrameBuffer = m_pCompontList[i][iDisplay].GetFrameBuffer();


			pMesh->Deformation(&matInt,pFrameBuffer,pAnim);
			
			pFrameBuffer->Render();
		}
	}
}

void Model::_DEBUG_RenderCollision(MRender *pRender)
{
	
	MSkinAnim *pAnim = m_pAnimationList[m_iCurAnimation].GetAnim();

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	
	D3DXMATRIX *pMat(NULL); 

	int iJointIndex(0);

	//--身体 -- 骨盆---------------------------------------
	iJointIndex = pAnim->JointNameToIndex(m_szBodyLocation);
	if (iJointIndex != -1)
		pMat = pAnim->GetFrame(iJointIndex);//pelvis
	else
		pMat = &matIdentity;

	m_pBodySphere->Transform(pMat);
	m_pBodySphere->Render(pRender);

	//--脚-----------------------------------------
	iJointIndex = pAnim->JointNameToIndex(m_szFeetLocation);
	if (iJointIndex != -1)
		pMat = pAnim->GetFrame(iJointIndex);//pelvis
	else
		pMat = &matIdentity;

//	m_pFeetSphere->Transform(pMat);
//	m_pFeetSphere->Render(pRender);
	

	//--手-----------------------------------------
	iJointIndex = pAnim->JointNameToIndex(m_szHandLocation);
	if (iJointIndex != -1)
		pMat = pAnim->GetFrame(iJointIndex);//pelvis
	else
		pMat = &matIdentity;

//	m_pHandSphere->Transform(pMat);
//	m_pHandSphere->Render(pRender);

}

void Model::SetAnimation(const char *pszName)
{
	for(int i = 0; i < m_iAnimationCnt; i++)
	{
		if (stricmp(m_pAnimationList[i].GetName(),pszName) == 0)
		{
			//found
			m_iCurAnimation = i;
			
			ResetController();
			
			break;
		}
	}
}

void Model::SetAnimation(int iIndex)
{
	ASSERT(iIndex >=0);
	ASSERT(iIndex < m_iAnimationCnt);

	m_iCurAnimation = iIndex;
	ResetController();
}

void Model::SetNextAnimation()
{
	m_iCurAnimation ++;
	m_iCurAnimation = m_iCurAnimation % m_iAnimationCnt;
}

void Model::SetDisplayComponent(const char * pszType,int iIndex)
{
	for(int i = COMP_HEAD; i < COMP_MAX;i++)
	{
        if (stricmp(szComponentName[i],pszType) == 0)
		{
            m_iDisplayComponent[i] = iIndex;
			ASSERT(iIndex < m_iCompontCnt[i]);
			return;
		}
	}
	CONSOLE("model:setdisplaycomponent: type not found \"%s\"",pszType);
}


int Model::GetComponentCnt(const char * pszType)
{
	for(int i = COMP_HEAD; i < COMP_MAX;i++)
	{
        if (stricmp(szComponentName[i],pszType) == 0)
		{
            return m_iCompontCnt[i];
		}
	}
	CONSOLE("model:GetComponentCnt: type not found \"%s\"",pszType);
	return -1;
}

int Model::GetDisplayComonent(const char * pszType)
{
	for(int i = COMP_HEAD; i < COMP_MAX;i++)
	{
        if (stricmp(szComponentName[i],pszType) == 0)
		{
			return m_iDisplayComponent[i];
		}
	}
	CONSOLE("model:GetDisplayComonent: type not found \"%s\"",pszType);
	return -1;
}

const char * Model::TypeIndexToTypeString(int iIndex)
{	
	return szComponentName[iIndex];	
}

int Model::GetCurAnimCnt(void)
{
	return m_pAnimationList[m_iCurAnimation].GetAnim()->GetFrameCnt();
}

void Model::CreateCollision(MScript * pScript)
{
	MScript *pSubScript(NULL);
	char szWordBuffer[DEFAULT_SCRIPT_WORD_SIZE];

	float fRadius;
	D3DXVECTOR3 vPos(0,0,0);
	
	//身体
	pSubScript = pScript->Subset("body");
	VERIFY(pSubScript);
	pSubScript->ReadWord(m_szBodyLocation,64);
	fRadius = (float)atof(pSubScript->ReadWord(szWordBuffer));
	SAFEDESTROY(pSubScript);
	m_pBodySphere = new MSphere;
	m_pBodySphere->Create(vPos,fRadius,0x80f5b57f);
	
	//脚
	pSubScript = pScript->Subset("feet");
	VERIFY(pSubScript);
	pSubScript->ReadWord(m_szFeetLocation,64);
	fRadius = (float)atof(pSubScript->ReadWord(szWordBuffer));
	SAFEDESTROY(pSubScript);
	m_pFeetSphere = new MSphere;
	m_pFeetSphere->Create(vPos,fRadius,0x807fb5f5);

	//手
	pSubScript = pScript->Subset("hand");
	VERIFY(pSubScript);
	pSubScript->ReadWord(m_szHandLocation,64);
	fRadius = (float)atof(pSubScript->ReadWord(szWordBuffer));
	SAFEDESTROY(pSubScript);
	m_pHandSphere = new MSphere;
	m_pHandSphere->Create(vPos,fRadius,0x807ff5b5);
}


void Model::DestroyCollision()
{
	SAFEDESTROY(m_pBodySphere);
	SAFEDESTROY(m_pFeetSphere);
	SAFEDESTROY(m_pHandSphere);
}

void Model::CreateController(MScript *pScript)
{
	m_pController = new Controller;
	m_pController->Create(pScript);
}

void Model::DestroyController(void)
{
	SAFEDELETE(m_pController);
}

//================================================
void Model::Controller::Update()
{
	MTimer *pTimer = MTimer::GetInstance();
	ASSERT(pTimer);
	DWORD dwCurTime = pTimer->GetTime();

	if (dwCurTime - m_dwLastAnimatedTime > m_dwDelay)
	{
		m_dwLastAnimatedTime = dwCurTime;
        m_iCurFrame++;
		m_iCurFrame %= m_iMaxFrame;
	}
}

Model::Controller::Controller(void):
m_dwLastAnimatedTime(NULL),m_iCurFrame(0)
{
	
}

void Model::Controller::Create(MScript *pScript)
{
	char szWordBuffer[DEFAULT_SCRIPT_WORD_SIZE];
	pScript->ReadWord(szWordBuffer);
	DWORD dwFPS = atoi(szWordBuffer);
	if (dwFPS > 60)	dwFPS = 60;//上限60fps
	m_dwDelay = 1000 / dwFPS;
}

void Model::Controller::Destroy()
{
}

Model::Controller::~Controller()
{

}

void Model::ResetController()
{
	MSkinAnim *pAnim = m_pAnimationList[m_iCurAnimation].GetAnim();
	m_pController->Reset(pAnim->GetFrameCnt());
}

void Model::_DEBUG_Render(MRender * pRender)
{
	//根据显示组件表取得组件
	for(int i = COMP_HEAD; i < COMP_MAX; i++)
	{
		int iDisplay = m_iDisplayComponent[i];
		if (iDisplay < m_iCompontCnt[i])
		{
			//进行组合变形渲染
			MSkinMesh *pMesh = m_pCompontList[i][iDisplay].GetMesh();

			pMesh->_DEBUG_RenderMesh();
		}
	}
}

const char * Model::GetComponent(const char * pszType, int iIndex)
{
	for(int i = COMP_HEAD; i < COMP_MAX;i++)
	{
        if (stricmp(szComponentName[i],pszType) == 0)
		{
			ASSERT(iIndex >= 0);
			ASSERT(iIndex < m_iCompontCnt[i]); 
			return m_pCompontList[i][iIndex].GetName();            
		}
	}
	return NULL;
}

int Model::GetAnimationCnt(void)
{
	return m_iAnimationCnt;
}

const char * Model::GetAnimationName(int iIndex)
{
	ASSERT(iIndex >= 0);
	ASSERT(iIndex < m_iAnimationCnt);
	return m_pAnimationList[iIndex].GetName();
}
