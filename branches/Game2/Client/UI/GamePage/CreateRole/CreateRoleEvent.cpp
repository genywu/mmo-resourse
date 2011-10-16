#include "stdafx.h"
#include "CreateRoleEvent.h"
#include "CreateRolePage.h"
#include "../../../GameClient/Game.h"
#include "../SelectRole/SelectRolePage.h"
#include "../../../GameClient/ClientApp/Player.h"
#include "../../../EngineExtend/DisplayModel.h"
#include "../../../EngineExtend/GameScene.h"
#include "../../../EngineExtend/GameMap.h"
#include "../../../GameClient/ClientApp/Other/CountryList.h"
#include "../../../GameClient/ClientApp/MsgboxSys/MsgEventManager.h"
#include "../../../UI/UIDef.h"
float CREvent::s_RolePos[4] = 
{
	28.0f,0.0f,36.f,0.f,
};

int	CREvent::m_CurrentAction;		// 当前动作
int  CREvent::m_NextAction;           // 下一个动作

extern void SetCreateRoleInitProperty(CEGUI::Window* pgWnd);

short CREvent::m_wFaceNum[2] = {0};	// 人物的面部特征数量（男0、女1）
short CREvent::m_wHairNum[2] = {0};	// 人物的发型数量（男0、女1）
short CREvent::nSelected = 0;         //选中Player索引
BYTE CREvent::nCountry = 0;          //国家ID
short CREvent::m_chSex = 0;           //性别
BYTE  CREvent::m_byHairStyle = 0;     //发型
BYTE  CREvent::m_byHairColor = 0;     //发色
BYTE  CREvent::m_byFace = 0;           //脸型
BYTE  CREvent::m_nConstellation = 0;  //星座
CPlayer *CREvent::m_pPlayer = 0;      //待创建人物角色
BYTE  CREvent::m_nRandCountry = 0;

map<WORD,short> CREvent::m_mapHairColorNum[2];	//人物的发色数量（男0、女1）
vector<BYTE>    CREvent::m_vecRandCountryRange;//国家随机范围

CREvent::CREvent():m_SelectSence(NULL)
{
}

CREvent::CREvent(EVENTTYPE type)
{
	SetEventType(type);
}
CREvent::~CREvent()
{
}
void CREvent::OnPageLoad(GamePage *pPage)
{
	pPage->LoadPageWindow();
	CEGUI::Window *pPageWin = pPage->GetPageWindow();
	CEGUI::PushButton* pCreateBtn = static_cast<CEGUI::PushButton*>(pPageWin->getChild("CreateRole"));
	pCreateBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&CREvent::OnCreateRoleBtn, this));
	CEGUI::PushButton* pGoBackBtn = static_cast<CEGUI::PushButton*>(pPageWin->getChildRecursive("BackToSelRol"));
	pGoBackBtn->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&CREvent::GoBackBtn,this));
	SetCreateRoleInitProperty(pPageWin);
	m_bRoleLeftRotate = false;    //向左旋转
	m_bRoleRightRotate = false;   //向右旋转
	if (m_SelectSence == NULL)
	{
		m_SelectSence = new GameScene();
		m_SelectSence->CreateSence("model/interface/selectchar/map",
			"model/interface/selectchar/map/camera_end",
			"model/interface/selectchar/map/envcreature",
			"model/interface/selectchar/map/enveffect");
	}
	CRFile* prfile = rfOpen("data/CreateRolePos.ini");
	if (prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		stream >> s_RolePos[0]  >> s_RolePos[1]  >> s_RolePos[2]  >> s_RolePos[3];
	}
	if(m_pPlayer == NULL)
		m_pPlayer = new CPlayer;
	m_pPlayer->SetGraphicsID(CREvent::GetSelectSex()+1);
	LoadFaceHairIni();
	m_pPlayer->SetDisplayModel();
	m_pPlayer->SetDisplayModelGroup();
}

void CREvent::OnPageOpen(GamePage *pPage)
{

}

void CREvent::OnPageClose(GamePage *pPage)
{

}

void CREvent::OnPageUpdate(GamePage *pPage)
{

}

void CREvent::OnPageRelease(GamePage *pPage)
{
	//Player指针需要在模型管理器释放所有模型之前被释放，所以不在析构函数里面调用
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_SelectSence);
}

//获取人物的面部特征数量（男0、女1）
short CREvent::GetFaceNum(WORD wSexIndex)
{
	if (wSexIndex<=1&&wSexIndex>=0)
	{
		return m_wFaceNum[wSexIndex];
	}
	return 0;
}

void CREvent::SetRandCountryRange(BYTE lRange)
{
	m_vecRandCountryRange.clear();
	for(BYTE c=1; c < 7; c++)
	{
		if( lRange & ( 1 << c ) )
			m_vecRandCountryRange.push_back( c );
	}
}

void CREvent::RandomChoseCountry()					//随机选择国家
{
	if( m_vecRandCountryRange.size() )
	{
		BYTE nRandCountry = m_vecRandCountryRange[ rand()%m_vecRandCountryRange.size() ];
		SetRandCountry(nRandCountry);

		TCHAR szPrompt[1024];
        wsprintf( szPrompt, AppFrame::GetText("Player_76"), //您确定接受女神的抉择，选择 %s 吗？：
            CCountryList::GetCountry(nRandCountry)->strName.c_str() );
		//OpenPrompt(1,szPrompt);
	}
}

void CREvent::RandomChoseDetails()
{
	WORD MaxNum[2],Results[2];
	MaxNum[0] = GetFaceNum(m_chSex);
	MaxNum[1] = GetHairNum(m_chSex);
	for (int i = 0;i<2;i++)
	{		
		Results[i] = random((int)MaxNum[i] + 1);
		while (Results[i] == 0)
		{
			Results[i] = random((int)MaxNum[i]);
		}
	}
	SetFace(Results[0]);
	SetHairStyle(Results[1]);
	MaxNum[1] = GetHairColorNum(m_chSex,m_byHairStyle);
	SetHairColor(0);
}

short CREvent::GetHairColorNum(WORD wSexIndex,WORD wHairIndex)
{
	if (wSexIndex<=1&&wSexIndex>=0)
	{
		if (wHairIndex>=1&&wHairIndex<=m_wHairNum[wSexIndex])
		{
			return m_mapHairColorNum[wSexIndex][wHairIndex];
		}
		return 0;
	}
	return 0;
}

short CREvent::GetHairNum(WORD wSexIndex)
{
	if (wSexIndex<=1&&wSexIndex>=0)
	{
		return m_wHairNum[wSexIndex];
	}
	return 0;
}

bool CREvent::OnCreateRoleBtn(const CEGUI::EventArgs &e)
{
	if (GetInst(SelectRolePage).GetPlayerCount() >= 1)
	{
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Base_34"));   //目前不能创建更多的角色了！
		return false;
	}
	CEGUI::Window *pPageWin = GetInst(CreateRolePage).GetPageWindow();
	CEGUI::Editbox* pNameEdit = static_cast<CEGUI::Editbox*>(pPageWin->getChild("EditName"));

	const char * strName = CEGUIStringToAnsiChar(pNameEdit->getText());
	if (strcmp(strName,"") == 0)
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Player_72"));  //"名字不能为空"
		return false;
	}
	if (!CheckName(strName))
	{
		GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("Player_73"));  //"名字中不能有空格"
		return false; 
	}
	int  iSex  = random(2);
	//RandomChoseDetails();
	//RandomChoseCountry();
	BYTE lConstellation = random(12) + 1;
	//const char *strName,char nOccupation, char nSex, BYTE lHead, BYTE lFace, BYTE lCountry,BYTE lConstellation,BYTE bRandCountry
	GetGame()->C2L_AddRole_Send(strName, 0, (char)GetSelectSex(), GetHair(), GetFace(), GetSelectCountry(), lConstellation, 0 );
	return true;
}

void CREvent::SetSelectSex(short chSex)
{
	m_chSex = chSex;
	if(m_pPlayer)
	{
		if (m_chSex == 0)
			m_pPlayer->SetGraphicsID(1);
		else
			m_pPlayer->SetGraphicsID(2);
		m_pPlayer->SetDisplayModel();
		m_pPlayer->SetDisplayModelGroup();
	}
}

bool CREvent::CheckName(const char* strName)
{
	char temp[64];
	strcpy(temp,strName);
	WORD len = (WORD)strlen(strName);
	for (WORD i = 0;i<len;i++)
	{
		if (temp[i]==' ')
		{
			return false;
		}
	}
	return true;
}

void CREvent::SetHairStyle(DWORD dwIndex)			//设定发型
{
	char  str[32];
	DWORD dwName = 0;
	//tagScene * pScene = &m_pSceneArray[0];

	sprintf(str, "%04d", dwIndex+1);
	dwName = MAKEFOURCC(str[0], str[1], str[2], str[3]);
	//pScene->pGameModel[m_chSex]->ShowGroup(MAKEFOURCC('H','A','I','R'), dwName, 0, false);
	if(m_pPlayer->GetDisplayModel())
		m_pPlayer->GetDisplayModel()->ShowGroup(MAKEFOURCC('H','A','I','R'), dwName, 0, false);
	m_byHairStyle = (BYTE)dwIndex+1;
	m_byHairColor = 0;
}

void CREvent::SetHairColor(DWORD dwIndex)			//设定发色
{
	char  str[32];
	DWORD dwName = 0;
	//tagScene * pScene = &m_pSceneArray[0];

	sprintf(str, "%04d", m_byHairStyle);
	dwName = MAKEFOURCC(str[0], str[1], str[2], str[3]);
	if(m_pPlayer->GetDisplayModel())
		m_pPlayer->GetDisplayModel()->ShowGroup(MAKEFOURCC('H','A','I','R'), dwName, dwIndex, false);

	m_byHairColor = (BYTE)(dwIndex);
}

void CREvent::SetFace(DWORD dwIndex)				//设定脸型
{
	char  str[32];
	DWORD dwName = 0;
	//tagScene * pScene = &m_pSceneArray[0];

	sprintf(str, "%04d", dwIndex+1);
	dwName = MAKEFOURCC(str[0], str[1], str[2], str[3]);
	if(m_pPlayer && m_pPlayer->GetDisplayModel())
		m_pPlayer->GetDisplayModel()->ShowGroup(MAKEFOURCC('F','A','C','E'), dwName, 0, false);

	m_byFace = (BYTE)dwIndex+1;
}

bool CREvent::LoadFaceHairIni()			// 读取面部特征、头发及发色的配置文件
{
	stringstream stream;
	const char strFileName[64] = "data/FaceHair.ini";
	CRFile* prfile = rfOpen(strFileName);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", strFileName);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return false;
	}

	prfile->ReadToStream(stream);
	rfClose(prfile);

	//男模
	if (ReadTo(stream,"<Man>"))
	{
		Read(stream,"*",m_wFaceNum[0]);
		Read(stream,"#",m_wHairNum[0]);
		for (int i = 1;i<=m_wHairNum[0];i++)
		{
			Read(stream,"$",m_mapHairColorNum[0][i]);
		}
	}
	//女模
	if (ReadTo(stream,"<Woman>"))
	{
		Read(stream,"*",m_wFaceNum[1]);
		Read(stream,"#",m_wHairNum[1]);
		for (int j = 1;j<=m_wHairNum[1];j++)
		{
			Read(stream,"$",m_mapHairColorNum[1][j]);
		}
	}
	return true;
}

bool CREvent::GoBackBtn(const CEGUI::EventArgs &e)         //回到上页
{
	GetGame()->SetGameState(GS_SELECT);
	return true;
}

void CREvent::Display()
{
	if(m_pPlayer){
		m_SelectSence->RenderLayerBack();
		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer3D *pLayer3D   = pInterface->GetLayer3D();
		DWORD dwCurrentTime = timeGetTime();
		// 显示角色
		if (m_pPlayer && m_pPlayer->GetDisplayModel()
			&& m_pPlayer->GetDisplayModel()->GetLoadState() == Load_Did)
		{
			static WORD dwState[] = 
			{
				CShape::STATE_PEACE,		// 取武器
				CShape::STATE_FIGHT,		// 挥舞
				CShape::STATE_FIGHT,		// 站定
			};
			static WORD dwAct[] = 
			{
				CShape::ACT_SEL_X0,			// 取武器
				CShape::ACT_SEL_X1,			// 挥舞
				CShape::ACT_SEL_X2,			// 站定
			};

			// 判断角色模型旋转
			if (m_bRoleLeftRotate)
				s_RolePos[3] += 0.1f;

			if (m_bRoleRightRotate)
				s_RolePos[3] -= 0.1f;

			//判断播放结束
			CDisplayModel *pDisplayModel = m_pPlayer->GetDisplayModel();
			AnimInfo *pAnimInfo = m_pPlayer->GetAnimInfo();
			pAnimInfo->GetActionInfo()->bStayToLastFrame = false;

			pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
			pAnimInfo->SetCurrentTime(dwCurrentTime);
			if (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 10 && pAnimInfo->GetActionInfo()->dwCurAction == MAKEFOURCC('E','I','0','0'))
			{
				pAnimInfo->GetActionInfo()->bInterpolation = TRUE;
				pAnimInfo->GetActionInfo()->dwOldAction = pAnimInfo->GetActionInfo()->dwCurAction;
				pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;
				pAnimInfo->GetActionInfo()->fOldActionSpeedRate = pAnimInfo->GetActionInfo()->fCurActionSpeedRate;
				pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
				pAnimInfo->GetActionInfo()->dwInterpolationStartTime  = dwCurrentTime;
				pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
				pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
				pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
			}
			/* else if (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0 && pAnimInfo->GetActionInfo()->dwCurAction == MAKEFOURCC('E','I','0','0'))
			{
			pAnimInfo->GetActionInfo()->bInterpolation = TRUE;
			pAnimInfo->GetActionInfo()->dwOldAction = pAnimInfo->GetActionInfo()->dwCurAction;
			pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = pAnimInfo->GetActionInfo()->dwCurActionElapseTime;
			pAnimInfo->GetActionInfo()->fOldActionSpeedRate = pAnimInfo->GetActionInfo()->fCurActionSpeedRate;
			pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
			pAnimInfo->GetActionInfo()->dwInterpolationStartTime  = dwCurrentTime;
			pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
			pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
			pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
			}*/
			else
			{
				if (pAnimInfo->GetActionInfo()->dwCurAction != MAKEFOURCC('E','I','0','0')&&pAnimInfo->GetActionInfo()->dwCurAction != MAKEFOURCC('E','B','0','0'))
				{
					pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('E','I','0','0');
					pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
				}
			}

			//this->RoleLeftRotate();

			//bool bDisplayFinish = (pAnimInfo->GetActionInfo()->dwCurActionLoopCount > 0);
			//if (bDisplayFinish)
			//{
			//	// 出招
			//	if (m_iCurrentAction < 2)
			//	{
			//		m_iCurrentAction++;
			//		g_bForceUpdate = true;
			//		pSelectedPlayer->SetAction(dwAct[m_iCurrentAction]);
			//	}
			//	else
			//	{
			//		pSelectedPlayer->SetAction(dwAct[m_iCurrentAction]);
			//		g_bForceUpdate = true;
			//	}
			//}

			// 显示坐标
			if (g_bDebug)
			{
				char str[256];
				sprintf(str, "%f, %f, %f (%f)", s_RolePos[0], s_RolePos[1], s_RolePos[2], m_pPlayer->GetNowDir());
				GetGame()->OutPutText(0, 0, str, lstrlen(str), D3DCOLOR_ARGB(255,0,255,0));
			}
			// 渲染简单阴影
			render::Camera *pCamera = m_SelectSence->GetCamera();
			pDisplayModel->SetDirLightEnable(false);
			pAnimInfo->SetupAnimInfo(s_RolePos[2], s_RolePos[0], s_RolePos[1],s_RolePos[3],pCamera);
			//pAnimInfo->SetViewMatrix(m_pCamera->GetViewMatrix());
			//pAnimInfo->SetProjectedMatrix(m_pCamera->GetProjectionMatrix());
			pDisplayModel->ProcessAnimJoint(pAnimInfo);
			pDisplayModel->ProcessAnimTexture(pAnimInfo);
			pDisplayModel->ProcessAnimUV(pAnimInfo);
			pDisplayModel->ProcessEnvironmentMaps(pCamera->GetViewMatrix());
			pDisplayModel->ProcessParticleEmitter(pAnimInfo);
			pDisplayModel->ProcessTextureProjective(pAnimInfo);
			pDisplayModel->ProcessTrailEmitter(pAnimInfo);
			pDisplayModel->ProcessLocator(pAnimInfo);
			pDisplayModel->ProcessAnimColor(pAnimInfo);
			pDisplayModel->ProcessVisibility(pAnimInfo);
			pDisplayModel->RenderModel();
			m_pPlayer->GetLeftHandWeapon()->Display(dynamic_cast<CShape *>(m_pPlayer));
			m_pPlayer->GetRightHandWeapon()->Display(dynamic_cast<CShape *>(m_pPlayer));
			m_pPlayer->GetWing()->Display(dynamic_cast<CShape *>(m_pPlayer));
			m_pPlayer->GetHeadWear()->Display(dynamic_cast<CShape *>(m_pPlayer));
			//CTextureProjective::BeginRendShadow(D3DXVECTOR3(s_RolePos[0],s_RolePos[1],s_RolePos[2]), 3);
			//CTextureProjective::AddRendShadow(pDisplayModel,pAnimInfo);
			//CTextureProjective::AddRendShadow(m_pPlayer->GetLeftHandWeapon()->GetDisplayModel(),m_pPlayer->GetLeftHandWeapon()->GetAnimInfo());
			//CTextureProjective::AddRendShadow(m_pPlayer->GetRightHandWeapon()->GetDisplayModel(),m_pPlayer->GetRightHandWeapon()->GetAnimInfo());
			//CTextureProjective::AddRendShadow(m_pPlayer->GetWing()->GetDisplayModel(),m_pPlayer->GetWing()->GetAnimInfo());
			//CTextureProjective::EndRendShadow();
		}
		particle::Manager::GetInstance()->Render();
		//pLayer2D->Update();
		//pLayer3D->SetMaterial(&pScene->pGameMap->m_material);
		pLayer3D->SetFogEnable(m_SelectSence->GetGameMap()->GetFogEnable());
		pLayer3D->SetMipMap(FALSE);
		pLayer3D->RenderTest();
		pLayer3D->RenderBlendLow();
		pLayer3D->EndEffectsState();
		//DisplayTextureProjective();
		pLayer3D->RenderBlend();
		pLayer3D->Clear();
	}
}