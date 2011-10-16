#include "StdAfx.h"
#include ".\game.h"
#include ".\playerranks.h"
#include "..\appclient\clientregion.h"
#include "..\appclient\minimap.h"
#include "..\appclient\player.h"
#include "..\appclient\effect.h"
#include "..\nets\netclient\message.h"
#include "..\nets\Clients.h"
#include "..\input\mouse.h"
#include "..\input\keyboard.h"
#include "..\appclient\other\piclist.h"
#include "..\appclient\other\chat.h"
#include "..\appclient\other\goodslist.h"
#include "..\AppClient\Other\BusinessSystem.h"
#include "..\Appclient\Other\FaceActionList.h"
#include "..\appclient\CollectionGoods.h"
#include "../AppClient/Other/SkillListXml.h"
#include "../AppClient/Other/StateListXml.h"
#include "../AppClient/Other/WeaponEffectXml.h"
#include "..\appclient\other\effectsetup.h"
#include "../AppClient/other/GlobeSetup.h"
#include "..\appclient\other\OfficialRankConfig.h"
#include "..\appclient\other\audiolist.h"

#include "../AppClient/ModelResource.h"

#include "..\..\GameEngine\WeatherManager.h"
#include "..\..\GameEngine\gamemap.h"
#include "..\..\GameEngine\GameGraphics2D.h"
#include "..\..\GameEngine\GameModelManager.h"
#include "..\..\GameEngine\TextureProjective.h"
#include "..\GUI\CBitmapX.h"
//==============================================================================
//界面
#include "..\guiextend\CGuiEx.h"
#include "..\guiextend\TopMsgPageEx.h"
#include "..\GUI\gcf\gui\IGraphics2D.h"
#include "..\GUI\gcf\gui\IGUIApp.h"
#include "..\GUI\gcf\gui\IGUIRoot.h"
#include "..\GUI\gcf\gui\CImageFactory.h"

#include "..\GUI\gcf\impl\GuangMGui\CGuangMGraphics2D.h"
#include "..\GUI\gcf\impl\GuangMGui\CGuangMImage.h"
#include "..\GUI\gcf\impl\GuangMGui\CGuangMImageLoader.h"
#include "..\GUI\gcf\impl\GuangMGui\CGuangMGUIRoot.h"
#include "..\AppClient\Other\QuestSystem.h"
#include "OccupSkillList.h"
#include "StudySkillCondition.h"

//==============================================================================
#include "..\appclient\Organizingsystem\organizingctrl.h"
#include "..\public\ClientResource.h"
#include "..\public\dupliregionsetup.h"
#include "..\public\DataBlockAllocator.h"
#include "..\NETS\MemClient.h"
#include "..\AppClient\Other\DeputyOccuSystemClient.h"
#include "../appclient/clientwordsfilter.h"
#include "..\..\GameEngine\jpegencode.h"
#include "..\public\RFile.h"
#include "MsgBoxEvent.h"
#include "GameControl.h"
#include "../AppClient/Skills/SkillXml.h"
#include "../nets/MsgDescribe.h"
#include "../appclient/message/sequencestring.h"
#include "..\appclient\other\CountryList.h"

#include "..\appclient\other\RegionList.h"
#include "..\..\FrostEngine\utility.h"
#include "..\..\FrostEngine\console.h"
#include "..\..\FrostEngine\Water.h"

#include "../AppClient/other/VideoManager.h"

#include "..\appclient\other\testmsgtype.h"

#include "Winreg.h"

#include <sstream>
#include <string>
#include <direct.h>
#include <strsafe.h>

#include "../appclient/petsystem/CPet.h"
#include "..\GUIExtend\MatrixCardPageEx.h"
#include "..\GUIExtend\MainBarPageEx.h"
#include "..\GUIExtend\MessageBoxPageEx.h"
#include "..\GUIExtend\CheckCheatPageEx.h" //防外挂界面
#include "..\GUIExtend\LoginPageEx.h"
#include "..\GUIExtend\SelectWsPageEx.h"
#include "..\GUIExtend\WorldMapPageEx.h"
#include "..\GUIExtend\MercenaryPageEx.h"
#include "..\GUIExtend\WorldMapPageEx.h"
#include "..\GUIExtend\AutoPageEx.h"
#include "..\GUIExtend\RelivePageEx.h"
#include "..\GUIExtend\SystemSetupPageEx.h"
#include "..\GUIExtend\EquipWearPageEx.h"
#include "..\GUIExtend\TimeTextPageEx.h"
#include "..\GUIExtend\PixyTalkBoxPageEx.h"
#include "..\GUIExtend\MedalPageEx.h"
#include "..\GUIExtend\ClewInfo.h"
#include "..\GUIExtend\StatePageEx.h"
#include "..\GUIExtend\TextDescPageEx.h"
#include "..\GUIExtend\SelectRolePageEx.h"
#include "..\GUIExtend\CreateRolePageEx.h"
#include "..\GUIExtend\WorldMapPageEx.h"
#include "..\GUIExtend\ShapeDescPage.h"
#include "..\GUIExtend\TopPage.h"
#include "..\GUIExtend\CardMutifunctionalPageEx.h"
#include "..\GUIExtend\SystemPageEx.h"
#include "..\GUIExtend\FacturePageEx.h"
#include "..\GUIExtend\DepotPageEx.h"
#include "..\GUIExtend\SelectSectionPageEx.h"
#include "..\GUIExtend\TreasureShopPageEx.h"
#include "..\GUIExtend\PresentPageEx.h"
#include "..\GUIExtend\ParamBoxPageEx.h"
#include "..\GUIExtend\HotKeySetupPageEx.h"
#include "..\GUIExtend\TreatyPageEx.h"
#include "SDOA4Client.h"
#include "SDOADx9.h"
#include "Oleauto.h"
#include "comutil.h"
#include <time.h>
#include <atlconv.h>
#include "..\..\..\Public\AccountServerDef.h"

#include "Cosmetology.h"
#include "FunctionKeyMapSystem.h"
#include "shellapi.h"
//是否启用编辑器
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===============界面所需要的命名空间以及界面相关变量================
CGuangMGraphics2D * g_pGuangMGraphics2D = NULL;
CGuangMGuiRoot    * g_pGuangMGUIRoot	= NULL;
CGuangMImageLoader* g_pGuangMImageLoader= NULL;
CImageFactory     * g_pGuangMFactory	= NULL;
CGuangMGUIApp	  * g_pGuangMGUIApp		= NULL;
CGuangMGUIApp	  * GetGuangMGUIApp()
{
	return g_pGuangMGUIApp;
}
//===================================================================

////////////////////////////////////////////////////////IGW接入全局变量//////////////////////////////////////////////////////////////

HINSTANCE          g_hInstIGWDll = 0;
LPigwInitialize    igwInitialize = NULL;
LPigwGetModule     igwGetModule = NULL;
LPigwTerminal      igwTerminal = NULL;

ISDOADx9*			g_SDOADx9 = NULL;
ISDOAApp*			g_SDOAApp = NULL;
ISDOAAppUtils*		g_SDOAAppUtils = NULL;
ISDOAClientService* g_SDOAClientService = NULL;

BOOL g_isSDOLogin = false; // 注意一定要是BOOL类型，不能为bool否则可能会导致问题
LoginResult g_loginResult;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long g_lVersion = 10000;		// 程序版本号（同LoginServer统一）第一位是测试版本号，后三位是版本
bool g_bDebug = true;		// 调试功能开关 

CGame* g_pGame= NULL;		// 游戏客户端数据
int ORGIN_GAME_WIDTH = 1024;
int ORGIN_GAME_HEIGHT = 768;
int SCREEN_WIDTH = 800;		// 游戏区域大小
int SCREEN_HEIGHT = 600;
int WINDOW_WIDTH = 800;		// 窗口大小
int WINDOW_HEIGHT = 600;
POINT g_pointMousePos;
DWORD g_LoginTime = 0;
bool g_bLogin = false; 

bool g_bInIME = false;		// 是否打开输入法状态

HCURSOR hMouseCursors[MC_COUNT];
HCURSOR hCurrentCursor=0;

// 加载鼠标光标
bool LoadMouseCursors(const char *filename)
{
	CRFile* prfile = rfOpen(filename);
	if( prfile )
	{	
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);
		string str;
		for(int i=0;i<MC_COUNT;i++)
		{
			stream>>str;
			hMouseCursors[i]=LoadCursorFromFile(str.c_str());
		}
		hCurrentCursor=hMouseCursors[0];
	}
	return false;
}

// 创建游戏客户端数据
bool CreateGame()
{
	g_pGame = new CGame;
	return true;
}

// 释放游戏客户端数据
bool DeleteGame()
{
	if (g_pGame)
	{
		delete g_pGame;
		g_pGame = NULL;
	}
	return true;
}


// 获取游戏客户端数据run
CGame* GetGame()
{
	return g_pGame;
}

CMouse* GetMouse()
{
	return GetGame()->GetMouse();
}

CKeyBoard* GetKeyboard()
{
	return GetGame()->GetKeyboard();
}

bool CGame::s_bExit = false;
bool CGame::s_bReGame = false;

// 王峰：添加 CGoodsDescPageEx 链表
vector<class CGoodsDescPageEx*>	CGame::m_vGoodsDescPageEx;

void WINAPI TestSetAudioSoundVolume(DWORD NewVolume)
{ // 测试设置音乐音量，仅模拟个数值变化
	DWORD tmpValue;
	if (NewVolume >= 100)
	{
		tmpValue = 100;
	}
	else
	{
		tmpValue = NewVolume;
	}
	GetGame()->GetSetup()->fMusicVolume = (float)tmpValue/100;
	GetGame()->GetCGuiEx()->GetSystemSetupPageEx()->SetDrugPos(1,(float)GetGame()->GetSetup()->fMusicVolume,true);
	GetGame()->GetAudioList()->SetMusicVolume(GetGame()->GetSetup()->fMusicVolume);
}

DWORD WINAPI TestGetAudioSoundVolume()
{
	return  (DWORD)(GetGame()->GetSetup()->fMusicVolume*100);
}

void WINAPI TestSetAudioEffectVolume(DWORD NewVolume)
{ // 测试设置音效音量，仅模拟个数值变化
	DWORD tmpValue;
	if (NewVolume >= 100)
	{
		tmpValue = 100;
	}
	else
	{
		tmpValue = NewVolume;
	}

	GetGame()->GetSetup()->fSoundVolume = (float)tmpValue/100;
	GetGame()->GetCGuiEx()->GetSystemSetupPageEx()->SetDrugPos(2,(float)GetGame()->GetSetup()->fSoundVolume,true);
	GetGame()->GetAudioList()->SetSoundVolume(GetGame()->GetSetup()->fSoundVolume);
}

DWORD WINAPI TestGetAudioEffectVolume()
{
	return (DWORD)(GetGame()->GetSetup()->fSoundVolume*100);
}

CGame::CGame(void)
{
	SetBackColor(255, 0,0,0);
	m_pMouse = NULL;
	m_pKeyboard = NULL;

	m_uGameServerPort = 2347;
	m_strGameServerIP = "127.0.0.1";
	m_uLoginServerPort = 2346;
	m_strLoginServerIP = "127.0.0.1";

	m_pNetClient = NULL;
	//=====================界面相关属性===============================
	m_pGuiEx = NULL;
	m_pOccupSkillList = NULL;
	//================================================================

	m_pMainPlayer = NULL;
	m_pRegion = NULL;

	m_lGameState = -1/*GS_LOGIN*/;
	m_lMouseState = MC_NORMAL;
	m_strCDKEY = "";
	m_strPASSWORD.clear();

	s_lAITick = 0;
	s_lLastPingTime = 0;

	m_pPicList=NULL;
	m_pAudioList = NULL;
	m_pMsgBoxEvent=NULL;

	m_bAutoLogin = false;
	m_pFaceActionList = NULL;

	m_pbitmapShapeTalk = NULL;
	m_pbitmapTeamTalk = NULL;

	m_pbitmapIntanateTimePot = NULL;
	m_pbitmapIntanateBarBack = NULL;
	m_pbitmapIntanateBar = NULL;
	m_pbitmapPlayerHpBar = NULL;
	m_pbitmapHpBarBack = NULL;
	m_pbitmapPlayerHpBar = NULL;
	m_pbitmapMonsterHpBar = NULL;
	m_pbitmapHpBar2 = NULL;
	m_pbitmapHpBarPet = NULL;
	m_pWeatherManager = NULL;
	m_pbitmapSuck = NULL;
	m_pbitmapBlastSuck = NULL;
	m_pbitmapBlastAttack = NULL;
	m_pbitmapBlock = NULL;
	m_pbitmapBlastBlock = NULL;
	m_pbitmapRebound = NULL;
	m_pbitmapBlastRebound = NULL;
	m_pbitmapFullMiss = NULL;
	m_pbitmapBlastFullMiss = NULL;
	m_pbitmapParry = NULL;
	m_pbitmapBlastParry = NULL;
	m_pbitmapResist = NULL;
	m_pbitmapBlastResist = NULL;
	m_pbitmapLost = NULL;
	m_pbitmapExpTip = NULL;
	m_pbitmapConTip = NULL;

	m_pHandsBackIcon = NULL;

	memset(&m_Setup, 0, sizeof(m_Setup));
	memset(&m_RestoreSetup, 0, sizeof(m_RestoreSetup));
	m_Setup.bTeamDisable = false;
	m_Setup.bFriendDisable = false;
	m_Setup.bTradeDisable = false;
	m_Setup.bOthersFootstep=false;
	m_Setup.l3DSoundsRange=8;
	m_Setup.bRoleFlash = true;
	m_Setup.bRoleShake = true;
	m_Setup.bShock = true;
	m_bIsGM = false;
	m_Setup.bUseVSync = false;
	m_Setup.lFrameControl = 10;
	m_Setup.bupdateonlostfoucs  = false;
	m_Setup.bfreecamera  = false;
	m_Setup.b360viewrangecamera = false;
	m_Setup.bAotuRestorecamera = false;

	// Tab设置
	m_TabSet.bEnemyPlayer = false;
	m_TabSet.bPiratePlayer = false;
	m_TabSet.bMonster = false;

	m_wHandSkillIcon = 0;

	m_dwNumOfShape = 0;
	m_dwNumOfVol = 0;

	m_bPlayerInWeatherEffectArea  = FALSE;
	m_iCurrentRenderTriangleCount = 0;

	m_fScrollCenterX = 0.f;
	m_fScrollCenterY = 0.f;

	m_LastTouchNpcID = CGUID::GUID_INVALID;
	m_lScriptTimer=0;
	m_lQueuePos = 0;
	m_dwLastUpdateQueueTime = timeGetTime();
	m_dwCountDownTime = 0;

	m_lWarContendTime = 0;
	m_lDiedStateTime = 0;

	m_bMsgTimeStart = false;
	m_ulServerTime = 0;
	m_ulClientTime = 0;

	m_bDisplayInfo = false;
	m_TopInfos.clear();
	m_stCurTopInfo.listCurInfo.clear();
	m_stCurTopInfo.lTotalLength = 0;
	m_dwDisplayTime = timeGetTime();
	m_lDisplayIntel = 4000;
	m_lDisplaySpeed = 1;

	m_pSelectShape = NULL;
	m_pPKIcon = NULL;
	m_pMouseShapeFoot = NULL;
	m_pMouseTeammerFoot = NULL;
	m_pLockAttackShapeFoot = NULL;
	m_pDupliRegionSetup = NULL;

	m_lSignCode = 0;
	m_lChangeServerCode = 0;
	m_strGameCDKEY = "";
	for(int i=0;i<4;i++)
	{
		m_lReGameCode[i] = 0;
	}


	m_pMemClient = NULL;
	m_bReg = false;

	m_lLoadingDelayTime = 0;
	m_lLoadingTipsIndex = 0;
	m_pLoadingPic = NULL;
	g_bOutPut = g_bDebug;
	m_strSndaid = "";

	m_bIsBeCheateForbid = false;
	m_bIsSadoLog = false;
	m_bSadoLogPageOpen = false;
	m_iAreaID = 1;
	m_iGroupID = 1; 
	m_bIsShowSdo = true;

	m_bServerlistDat = false;
	m_bQuestTrace = false;
	m_bShowUserTreaty = false;
	m_LastVersion = "";
}

void CGame::SetMouseCursor(int mcType)
{
	hCurrentCursor=hMouseCursors[mcType];
	PostMessage(g_hWnd,WM_SETCURSOR,0,0);
}

MOUSE_CURSOR_TYPE CGame::GetMouseType()
{
	for(int i=0;i<MC_COUNT;i++)
	{
		if(hCurrentCursor==hMouseCursors[i])
		{
			return (MOUSE_CURSOR_TYPE)i;
		}
	}
	return MC_NORMAL;
}

HCURSOR CGame::GetMouseCursor()
{
	return hCurrentCursor;
}

CGame::~CGame(void)
{
	// 王峰：释放 m_vGoodsDescPageEx
	m_vGoodsDescPageEx.clear();
}

// 获取输入状态
void CGame::GetInputInfo()
{
	bool m_bIsWindowed = true;
	if(m_bIsWindowed)
	{
		GetCursorPos(&g_pointMousePos);
		ScreenToClient(g_hWnd, &g_pointMousePos);

		if(WINDOW_WIDTH&&WINDOW_HEIGHT)
		{
			g_pointMousePos.x = (long)((float)SCREEN_WIDTH * (float)g_pointMousePos.x / (float)WINDOW_WIDTH);
			g_pointMousePos.y = (long)((float)SCREEN_HEIGHT * (float)g_pointMousePos.y / (float)WINDOW_HEIGHT);
		}
	}
	else
	{
		GetCursorPos(&g_pointMousePos);
	}
	if(GetMouse())
	{
		GetMouse()->SetPosition(g_pointMousePos.x, g_pointMousePos.y);
	}

	if(GetKeyboard() && !g_bInIME)
	{
		GetKeyboard()->RecieveKeyboardInput();	// 获得键盘数据
		//GetGame()->GetCGuiEx()->GetHotKeySetupPageEx()->GetInputKey();
	}
	g_bInIME = false;
}



/*
* 功能: 游戏执行部分
* 摘要: -
* 参数: bControl - 控制标志
* 返回值: 总是返回 - 1
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.03.14 - lpf
*		在GS_RUN中处理了当玩家处于隐蔽状态时,鼠标无法指向它
*/
int CGame::Run(bool bControl)
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"Run() Start.");
#endif
	//检查资源装载情况
	static CClientResource *pClientResource = CClientResource::GetInstance();
	pClientResource->Run();
	CVideoManager::GetInstance()->GarbageColloction();
	UpdateSetup();
	GetInputInfo();

	GetAudioList()->Run();

	if(g_LoginTime > 0)
	{
		DWORD dwTime = timeGetTime();
		if((long)dwTime - (long)g_LoginTime > 8000)
		{
			CMessageBoxPageEx * pMsgBox = GetCGuiEx()->GetMessageBoxPageEx();
			CLoginPageEx * pLoginPage = GetCGuiEx()->GetLoginPageEx();
			if(pMsgBox && pLoginPage)
			{
				//是否是用户登陆阶段
				if(g_bLogin)
				{
					pMsgBox->Open(pLoginPage,CStringReading::LoadString(eIDS_PUBLIC,eIDS_ServerError),CMessageBoxPageEx::MSG_OK_NO,true);
					g_LoginTime = 0;
				}
				else{
					CloseNetClient();
					SetGameState( GS_LOGIN );
					pMsgBox->Open(pLoginPage,CStringReading::LoadString(eIDS_PUBLIC,eIDS_TimeOver),CMessageBoxPageEx::MSG_OK,true);
					if (g_isSDOLogin && g_SDOAApp)   //如果登陆盛大通行证，则推出
					{
						// 登录成功，退出时要登出
						g_SDOAApp->Logout();
						if(!g_isSDOLogin)
						{
							GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
							GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(true);
							GetGame()->GetCGuiEx()->GetLoginPageEx()->SetPageFocus(); 
						}else
						{
							GetGame()->ShowLoginDialog();
							GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
							GetGame()->SetSDOFocus();
						}

					}
					g_LoginTime = 0;
				}
			}
		}
	}

	// 登陆延迟时间控制
	if(m_lLoadingDelayTime > 0)
	{
		DWORD dwDelayTime = 0;
		CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
		if(pSetup)
			dwDelayTime = pSetup->dwLoadingDelayTime;
		long time = (long)timeGetTime();
		if(time - m_lLoadingDelayTime > dwDelayTime)
		{
			m_lLoadingDelayTime = 0;
			SAFE_DECREF(m_pLoadingPic);
			if(!m_quMessage.empty())
			{
				tagWinMessageParam tagMessage=m_quMessage.front();
				m_quMessage.clear();
				m_quMessage.push_back(tagMessage);
			}
		}
		else
			return 0;
	}

	switch( GetGameState() )
	{
	case GS_LOGIN:
		{
			if (bControl == false)	break;

			// 自动登陆//////////////////////////////////////////////////////////////////////////
			if (GetKeyboard()->GetCurrentKey() == DIK_F1)
			{
				string strTemp;
				ifstream stream;
				stream.open("setup/autologin.ini");
				if (stream.is_open())
				{
					stream  >> strTemp >> m_stAutoLogin.strCdkey
						>> strTemp >> m_stAutoLogin.strPassword
						>> strTemp >> m_stAutoLogin.strWorldName
						>> strTemp >> m_stAutoLogin.lPlayerPos;
					stream.close();
					m_bAutoLogin = true;
				}
			}

			PopWinMessage();
		}
		break;
	case GS_PWD_CARD:
		{
			CMatrixCardPageEx	*pMatrixCardPage = GetGame()->GetCGuiEx()->GetMatrixCardPageEx();
			if (pMatrixCardPage)
			{
				pMatrixCardPage->UpdatePage();
				PopWinMessage();
			}
		}
		break;
	case GS_SELECTWS:
		{
			if (bControl == false)	break;

			//显示等候队列
			if(m_lQueuePos >= 0)
			{
				char str[100];
				sprintf(str,CStringReading::LoadString(eIDS_PUBLIC,eIDS_QueeInfo),m_lQueuePos,m_dwCountDownTime/1000); 
				GetGame()->GetCGuiEx()->GetSelectWsPageEx()->SetMessageBox(str);
			}
			PopWinMessage();
		}
		break;

	case GS_SELECT:
		{
			if (bControl == false)	break;

			PopWinMessage();
		}
		break;

	case GS_CREATE:
		{
			if (bControl == false)	break;
			PopWinMessage();
		}
		break;

	case GS_RUN:
		{
			if (IsReGame())
			{
				SetReGame(false);
				ReLoadGame();
				break;
			}

			if (bControl)
			{
				// 检测鼠标在其上的SHAPE
				GetGame()->GetGameControl()->SetMouseShapeGoods(NULL);

				static bool	  bHaveSpy = false;
				CShape		* pShape   = GetRegion()->GetHoverShape();
				if (pShape != NULL)
				{
					CPlayer * pMPlayer = GetGame()->GetMainPlayer();

					// 检测当前玩家是否有侦查隐形技能状态
					if (!pMPlayer->GetIsSpy())
						bHaveSpy = true;
					else
						bHaveSpy = false;

					if ((pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_MONSTER) &&!bHaveSpy &&
						((CMoveShape*)pShape)->GetIsConceal() == false)
						pShape = NULL;
					else if (((CMoveShape*)pShape)->GetType() == TYPE_COLLECTION)
					{
						CCollectionGoods* pCollectGoods = (CCollectionGoods*)pShape;
						if (pCollectGoods!=NULL)
						{
							if (!pCollectGoods->IsBeingCollected()&&pCollectGoods->IsCanBeCollected())
							{
								bool bShowResults = true;
								// 任务条件检测
								if (pCollectGoods->GetOccuID()==DOccu_Other)
								{
									const vector<tagQuestGoods>* pQustList = pCollectGoods->GetRoleInfo();
									if (pQustList!=NULL)
									{
										vector<tagQuestGoods>::const_iterator QustIter = pQustList->begin();
										for (;QustIter!=pQustList->end();QustIter++)
										{
											if (QustIter->nQuestId!=-1)
											{
												CRoleQueue* pQueueList = pMPlayer->GetRoleQueue();
												if (pQueueList!=NULL)
												{
													if (pQueueList->GetRecordByID(QustIter->nQuestId)==NULL||
														pQueueList->GetRoleStepNum(QustIter->nQuestId)!=QustIter->dwRoleStep)
													{
														continue;
													}
													else if (pQueueList->GetRecordByID(QustIter->nQuestId)!=NULL&&pQueueList->GetRoleStepNum(QustIter->nQuestId)==QustIter->dwRoleStep)
													{
														break;
													}
												}
												else
												{
													continue;
												}
											}
											else
												break;
										}
										if (QustIter==pQustList->end())
										{
											bShowResults = false;
										}
										else if (QustIter!=pQustList->end())
										{
											bShowResults = true;
										}
									}
									// 物品条件检测
									const list<tgConditionGoods>* pConditionList = pCollectGoods->GetConditionList();
									if (pConditionList!=NULL&&pConditionList->size()!=0)
									{
										list<tgConditionGoods>::const_iterator conIter = pConditionList->begin();
										for (;conIter!=pConditionList->end();conIter++)
										{
											long lGoodNum = pMPlayer->GetGoodsAmount(conIter->cGoodsName);
											if (conIter->iNumType==0&&lGoodNum<conIter->iNum||
												conIter->iNumType==1&&lGoodNum!=conIter->iNum||
												conIter->iNumType==2&&lGoodNum>=conIter->iNum)
											{
												bShowResults = false;
												PutDebugString(CStringReading::LoadString(eIDS_CONTROL,eIDS_CONTROL_NOGOODS));
												break;
											}
										}
									}
								}
								if (!bShowResults)
								{
									pShape = NULL;
								}
							}
						}
					}
				}

				// 设定鼠标指向的Shape
				GetGame()->GetGameControl()->SetMouseShape(pShape);

				// ---- 如果操作了界面，那么就不允许玩家移动 ----
				static bool bClick = false;
				if (GetMouse()->GetCurrentButton() == LB_DOWN || GetMouse()->GetCurrentButton() == RB_DOWN)
				{
					bClick = true;
					GetGameControl()->SetMoveAble(true);
				}

				//=====================================================================
				// 界面处理
#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN UI Start.");
#endif
				m_pGuiEx->GetWorldMapPageEx()->Run();
				GetGame()->GetCGuiEx()->GetMercenaryPageEx()->UpdateQuestTime();   //佣兵界面时间更新
				GetGame()->GetCGuiEx()->GetAutoPageEx()->AutoRun();                //小精灵功能相关
				GetGame()->GetCGuiEx()->GetRelivePageEx()->UpdateLastTime();       //接受复活术记时
				GetGame()->GetCGuiEx()->GetSystemSetupPageEx()->UpdateButtonDrug();
				GetGame()->GetCGuiEx()->GetEquipEwearPageEx()->Run();              //装备耐久度的闪烁显示
				GetGame()->GetCGuiEx()->GetCharacterPageEx()->FactionTitleRun();
				GetGame()->GetCGuiEx()->GetTimeTextPageEx()->Run();
				GetGame()->GetCGuiEx()->GetPixyTalkBoxPageEx()->Run();
				GetGame()->GetCGuiEx()->GetMadalPageEx()->Run();
				GetGame()->GetCGuiEx()->GetParamBoxPageEx()->Run();
				GetGame()->GetCGuiEx()->GetTreasureShopPageEx()->Run();
				/* *****************************
				功能:防外挂答题界面时间更新
				作者:zhaohang(kaba)
				创建日期: 2009/5/31
				修改日志:
				修改日期:
				修改内容:
				修改人  :
				* *****************************/
				GetGame()->GetCGuiEx()->GetCheckCheatPageEx()->Run();				//防外挂答题界面时间更新
				GetGame()->GetCGuiEx()->GetPinxyTalkBoxPageInstEx()->Run();			//防外挂专用精灵

				m_pGuiEx->GetPresentPageEx()->Run();								// 游戏外赠送系统

#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN UIPage Updata.");
#endif

				int iAct = GetMainPlayer()->GetAction();
				if(iAct == CShape::ACT_WALK || iAct == CShape::ACT_RUN)
				{
					m_pGuiEx->GetMainBarPageEx()->UpdataCoordinate();
				}
				// 不处于隐藏所有界面显示状态，可操作界面
				if(g_pGuangMGUIRoot->GetGUIComp()->IsDrawAllPane())
				{
					// 判断鼠标是否指向界面,聊天框列表和队伍界面除外
					if(g_pGuangMGUIRoot && g_pGuangMGUIRoot->GetGUIComp())
					{
						CComponent * pComp = g_pGuangMGUIRoot->GetGUIComp()->GetComponentAt(g_pointMousePos.x,g_pointMousePos.y);
						if(pComp && pComp->IsEnabled() && 
							strcmp(pComp->GetName(),"ChatInfo")!=0 && 
							strcmp(pComp->GetName(),"TeamPage")!=0 &&
							strcmp(pComp->GetName(),"MedalTipPage")!=0&&
							strcmp(pComp->GetName(),"TimeNotePage")!=0)
						{
							GetGame()->GetGameControl()->SetMouseShapeGoods(NULL);
							GetGame()->GetGameControl()->SetMouseShape(NULL);
						}
					}
				}
#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN PopWinMessage Start.");
#endif
				PopWinMessage();
#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN PopWinMessage End.");
#endif
				//=====================================================================
				m_pGuiEx->GetClewInfo()->UpdataClewInfoList();
				m_pGuiEx->GetStatePageEx()->UpdataStateList();
				if (bClick)
				{
					// ---- 如果操作了界面，那么就不允许玩家移动 ----
					if ( GetMouse()->GetCurrentButton() != LB_DOWN && GetMouse()->GetCurrentButton() != RB_DOWN )
						GetGameControl()->SetMoveAble(false);
					else
					{
						CComponent * pComp = g_pGuangMGUIRoot->GetGUIComp()->GetComponentAt(GetMouse()->GetMouseX(),GetMouse()->GetMouseY());
						if(pComp && pComp->IsEnabled() && pComp->IsVisible() && strcmp(pComp->GetName(),"ChatInfo")!=0)
						{
							GetGameControl()->SetMoveAble(true);
						}
						if(GetMouse()->GetCurrentButton() == LB_DOWN)
						{
							GetMouse()->SetCurrentButton(NONE);
							GetGameControl()->SetMoveAble(false);
						}
					}
				}
				bClick = false;
#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN UI End.");
#endif
			}

			// 场景AI     
			{

#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CClientRegion::Run() Start.");
#endif
				GetRegion()->AI();

#ifdef _RUNSTACKINFO_
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CClientRegion::Run() End.");
#endif

				// 其他控制
				if( bControl )
				{
#ifdef _RUNSTACKINFO_
					CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGetGameControl::Run() Start.");
#endif
					GetGameControl()->Run();
#ifdef _RUNSTACKINFO_
					CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGetGameControl::Run() End.");
#endif
				}

				//运新顶部提示信息
				RunTopInfo();
			}

#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"face animation update Start.");
#endif
			//表情动画更新
			UpdateFace();
#ifdef _RUNSTACKINFO_
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"face animation update End.");
#endif

			s_lAITick++;
		}
		break;
	}


	// 清除鼠标缓冲区
	if( GetMouse() )
	{
		GetMouse()->RecieveMouseInput();			// 获得鼠标数据
	}
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetNetSessionManager()->Run() Start.");
#endif

	GetNetSessionManager()->Run();

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetNetSessionManager()->Run() End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGameModelManager::GetInstance()->Run() Start.");
#endif
	CGameModelManager::GetInstance()->Run();
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGameModelManager::GetInstance()->Run() End.");
#endif
	//运行MsgBox事件 
	//GetMsgBoxEvent()->Run();

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"snatch Start.");
#endif
	// 抓屏
	if ( GetKeyboard()->GetCurrentKey() == DIK_SYSRQ )
	{
		GetKeyboard()->SetCurrentKey(NONE);

		static SYSTEMTIME st;
		GetLocalTime(&st);
		static char strFileName[_MAX_PATH];
		_snprintf(strFileName,256,"PrintScreen/Luvinia %d-%d-%d[%2.2d.%2.2d.%2.2d].jpg",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

		//创建一个拷屏的保存目录
		CreateDirectory("PrintScreen",NULL);

		PrintScreen(strFileName);
	}
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"snatch End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"map BUG report Start.");
#endif
	// 生成地图BUG报告
	if( g_bDebug )
	{
		if(DIK_SCROLL==GetKeyboard()->GetCurrentKey())
		{
			GetKeyboard()->SetCurrentKey(NONE);

			if(GetRegion() && GetMainPlayer())
			{
				FILE* hFile=fopen("MapBugReport.txt","a+t");
				char buf[64];
				sprintf(buf,"%s\t%d\t%d\r\n",GetRegion()->GetName(),GetMainPlayer()->GetTileX(),GetMainPlayer()->GetTileY());
				fwrite(buf,strlen(buf),1,hFile);
				fclose(hFile);
				sprintf(buf,"%s:%d,%d\r\n",GetRegion()->GetName(),GetMainPlayer()->GetTileX(),GetMainPlayer()->GetTileY());
			}
		}
	}
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"map BUG report End.");
#endif

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::Run() End.");
#endif

	//客户端UI编辑zhaohang
#ifdef _GUIEDIT
	if ( GetKeyboard()->GetCurrentKey() == DIK_HOME)
	{
		GetGame()->GetCGuiEx()->GetGuiEditorPageEx()->ChangePageState();
	}
#endif
	return 1;
}

// 游戏消息处理
void CGame::ProcessMessage()
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::ProcessMessage() Start.");
#endif
	// 处理测试程序的消息
#ifdef _CLENTTOTEST_
	if(GetGameState() == GS_RUN && GetActiveWindow() == g_hWnd)
		ProcessTestMessage();
#endif
	// 消息处理
	if(m_pNetClient)
	{
		msgQueue mQueue;
		m_pNetClient->GetRecvMessages()->GetAllMessage(mQueue);
		while(!mQueue.empty())
		{
			CMessage* pMsg = (CMessage*)(mQueue.front());
			if( pMsg )
			{
				pMsg->Run();
				CBaseMessage::FreeMsg(pMsg);
			}
			mQueue.pop_front();
		}
	}

#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::ProcessMessage() End.");
#endif
}

//------------测试程序消息处理------------
void CGame::ProcessTestMessage()
{
	// 内存影射消息处理
	if(m_pMemClient)
	{
		msgQueue mQueue;
		m_pMemClient->GetRecvMsgs()->GetAllMessage(mQueue);
#ifdef _CLENTTOTEST_
		while(!mQueue.empty())
		{
			CMessage* pMsg = (CMessage*)(mQueue.front());
			if( pMsg )
			{
				pMsg->TestRun();
				CBaseMessage::FreeMsg(pMsg);
			}
			mQueue.pop_front();
		}
#endif
	}
}



/*
* 功能: 游戏显示部分
* 摘要: -
* 参数: -
* 返回值: 总是返回true
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.07.01 - lpf
*		修改了天气处理时的Billboard效果,现在为全角度Billboard
*	2008.09.30 - lpf
*		增加了Debug版本游戏中按下左ALT键可以显示当前摄象机到玩家视线距离
*	2008.12.10 - lpf
*		增加天气处理时的Billboard Y轴数据传入
*	2009.02.24 - lpf
*		修改了天气的渲染,增加了判断
*/
int CGame::Display(void)
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::Display Start.");
#endif
	// 登陆延迟时间控制
	if(m_lLoadingDelayTime > 0)
	{
		long lID = 0;
		if(GetMainPlayer())
			lID = GetMainPlayer()->GetRegionID();
		DisPlayLoading(lID);
		return 0;
	}
	render::Interface::GetInstance()->BeginScene();
	switch( GetGameState() )
	{
	case GS_LOGIN:
		{
			m_pGuiEx->GetLoginPageEx()->Display();
			// 界面显示
			g_pGuangMGUIRoot->RenderGUI();
			// 显示文字的提示信息
			if(m_pGuiEx->GetTextDescPageEx()->IsShow())
				m_pGuiEx->GetTextDescPageEx()->ShowPage();
			/* 绘制IGW界面 */
			if (g_SDOADx9&&!m_pGuiEx->GetSelectSectionPageEx()->IsOpen()&&m_bIsShowSdo)
				g_SDOADx9->RenderEx();
			// 显示对话框
			g_pGuangMGUIRoot->RenderDialogGUI();
			GameGraphics2D::RecoverFogState();
		}
		break;

	case GS_SELECTWS:
		{
			m_pGuiEx->GetLoginPageEx()->Display();
			// 界面显示
			GameGraphics2D::SetUIModelRendState();
			g_pGuangMGUIRoot->RenderGUI();
			// 显示对话框
			g_pGuangMGUIRoot->RenderDialogGUI();
			GameGraphics2D::RecoverFogState();
		}
		break;
	case GS_SELECT:
		{
			m_pGuiEx->GetSelectRolePageEx()->Display();
			// 界面显示
			g_pGuangMGUIRoot->RenderGUI();
			// 显示对话框
			g_pGuangMGUIRoot->RenderDialogGUI();
			m_pGuiEx->GetSelectRolePageEx()->DisplayVersionInfo();
			m_pGuiEx->GetSelectRolePageEx()->DisplayPlayerInfo();
			GameGraphics2D::RecoverFogState();
		}
		break;
	case GS_BLACK:
		break;

	case GS_CREATE:
		{
			m_pGuiEx->GetCreateRolePageEx()->Display();

			g_pGuangMGUIRoot->RenderGUI();
			// 显示对话框
			g_pGuangMGUIRoot->RenderDialogGUI();
			m_pGuiEx->GetCreateRolePageEx()->DisplayVersionInfo();
			m_pGuiEx->GetCreateRolePageEx()->DisplayIntroInfo();
			GameGraphics2D::RecoverFogState();
		}
		break;

	case GS_RUN:
		{
			char str[1024] = "";
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"CGame() GS_RUN Display Start.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			// 地表显示
			if (GetRegion())
			{
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"GetRegion()->DisPlay() Start.%s",GetRegion()->GetName());// 场景开始
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
				// 滚动屏幕
				if (!GetGame()->GetSetup()->bfreecamera)
				{
					SetScrollCenterX(GetMainPlayer()->GetPosX());
					SetScrollCenterY(GetMainPlayer()->GetPosY());
				}
				GetRegion()->ScrollCenter(GetScrollCenterX(), GetScrollCenterY());
				GetRegion()->Display();

				if(GetWeatherManager())
				{
#ifdef _RUNSTACKINFO_
					CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetWeatherManager()->Render() Start.");
#endif
					// 天气处理
					if (GetSetup()->lWeather && m_bPlayerInWeatherEffectArea)
					{
						render::Interface *pInterface = render::Interface::GetInstance();

						// Set up a rotation matrix to orient the billboard towards the camera.
						D3DXMATRIX matBillboard, matBillboardY, matWorld;
						D3DXVECTOR3 *vView = GetRegion()->GetCamera()->GetView();

						matBillboard = *GetRegion()->GetCamera()->GetBillboard();
						matBillboardY = *GetRegion()->GetCamera()->GetBillboardY();
						D3DXMatrixTranslation(&matWorld, vView->x, vView->y, vView->z);

						m_pWeatherManager->SetWorldMatrix(&matWorld);
						m_pWeatherManager->SetBillboard(&matBillboard, &matBillboardY);
						m_pWeatherManager->Update();
					}

					GameMap *pMap = GetRegion()->GetGameMap();
					if (pMap)
					{
						render::Interface *pInterface = render::Interface::GetInstance();
						if(pMap->GetFogEnable())
						{
							DWORD dwFogColor = pMap->GetFogColor();

							if (GetGame()->GetSetup()->lViewRange < 25)
								GetGame()->GetSetup()->lViewRange = 25;
							float fFogStart = pMap->GetFogStart();
							float fFogEnd = pMap->GetFogEnd();
							SetBackColor(dwFogColor);

							pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE, TRUE);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, (DWORD)dwFogColor);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fFogStart));
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&fFogEnd));

						}
						else
						{
							DWORD dwFogColor = pMap->GetFogColor();
							SetBackColor(dwFogColor);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, 0);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGSTART, 0);
							pInterface->GetDevice()->SetRenderState(D3DRS_FOGEND,   0);
						}

					}

					// 天气渲染
					if (GetSetup()->lWeather && m_bPlayerInWeatherEffectArea)
						m_pWeatherManager->Render();

#ifdef _RUNSTACKINFO_
					CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"GetWeatherManager()->Render() End.");
#endif

				}
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"GetRegion()->DisPlay() End.%s",GetRegion()->GetName());// 场景结束
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			}

			//=========================================================================================
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"UI Display Start");// 界面开始
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			// 界面显示
			if(g_pGuangMGUIRoot->GetGUIComp()->IsDrawAllPane())
			{
				GameGraphics2D::SetUIModelRendState();
				m_pGuiEx->GetMainBarPageEx()->Display();
				GameGraphics2D::SetUIModelRendState();
				g_pGuangMGUIRoot->RenderGUI();
				// 大地图绘制
				m_pGuiEx->GetWorldMapPageEx()->Display();
				// 显示对话框
				g_pGuangMGUIRoot->RenderDialogGUI();
				/***********************
				功能:绘制防外挂界面的进度条
				作者:zhaohang(kaba)
				创建日期: 2009/6/10
				修改日志:
				* *****************************/
				m_pGuiEx->GetCheckCheatPageEx()->DisplayProBar();
				// 显示物品提示信息
				if(m_pGuiEx->GetGoodsDescPageEx()->IsShow())
					m_pGuiEx->GetGoodsDescPageEx()->Show();

				// 王峰：绘制 CGoodsDescPageEx 链表
				for( vector<CGoodsDescPageEx*>::iterator p = m_vGoodsDescPageEx.begin(); p != m_vGoodsDescPageEx.end(); p++ )
				{
					(*p)->Show();
				}
				///* 绘制IGW界面 */
				//      if (g_SDOADx9)
				//       g_SDOADx9->RenderEx();
				// 显示文字的提示信息
				if(m_pGuiEx->GetTextDescPageEx()->IsShow())
					m_pGuiEx->GetTextDescPageEx()->ShowPage();

				m_pGuiEx->GetShapeDescPage()->ShowDesc();

				// 绘制帮助聚焦框
				m_pGuiEx->GetTopPage()->Display();

				/* 绘制IGW界面 */
				if (g_SDOADx9)
					g_SDOADx9->RenderEx();
			}
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"UI Display End");// 界面结束
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			//=========================================================================================
			//显示MsgBox
			GetMsgBoxEvent()->Display();

			CPlayer* pPlayer=GetMainPlayer();
			CMainPlayerHand* pHand = GetCGuiEx()->GetPMainPlayerHand();

			// 手上拿的物品
			if(pPlayer &&pHand)
			{
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"Hand Goods Display Start");// 手上物品开始
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
				DWORD dwIconId = 0;					
				int iDrawX = GetMouse()->GetMouseX()-GRID_WIDTH/2;
				int iDrawY = GetMouse()->GetMouseY()-GRID_WIDTH/2;
				if (pHand->GetPGoodsOfMainPlayerHand()!=NULL)
				{
					dwIconId = pHand->GetPGoodsOfMainPlayerHand()->GetProperty()->dwIconId;

				}else if (pHand->GetGoodsIndexOfMainPlayerHand()!=0)
				{
					CGoodsList::tagGoods2* pstGoods2 = CGoodsList::GetProperty(pHand->GetGoodsIndexOfMainPlayerHand());
					if (pstGoods2!=NULL)
					{
						dwIconId = pstGoods2->BaseProperty.dwIconId;
					}
				}
				if (dwIconId!=0)
				{
					pHand->Display(dwIconId, iDrawX, iDrawY);
					m_pHandsBackIcon->Draw(iDrawX-6,iDrawY-6);
				}
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"Hand Goods Display End");// 手上物品结束
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
				// 绘制手上的范围特效
				if(pHand->GetAreaSkillID() > 0)
					pHand->DisplayEffectInHand();
			}	

			// 手上拿的技能图标
			if (pPlayer &&pHand&&pHand->GetSkillOfMainPlayerHand())
			{
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"Hand Skill Display Start");// 手上技能开始
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
				int iDrawX = GetMouse()->GetMouseX() - 16;
				int iDrawY = GetMouse()->GetMouseY() - 16;
				pHand->Display( pHand->GetSkillOfMainPlayerHand(), iDrawX, iDrawY);
				m_pHandsBackIcon->Draw(iDrawX-6,iDrawY-6);
#ifdef _RUNSTACKINFO_
				sprintf_s(str,"Hand Skill Display End");// 手上技能开始
				CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			}
			// 绘制自己头上的提示文字
			if(GetRegion() && pPlayer && pPlayer->GetPlayerInfo().size()>0)
				GetRegion()->ShowPlayerInfo(pPlayer);
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"CGame() GS_RUN Display End.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
		}
		break;
	default:
		//=========================================================================================
		// 界面显示
		g_pGuangMGUIRoot->RenderGUI();
		// 显示对话框
		g_pGuangMGUIRoot->RenderDialogGUI();
		//=========================================================================================
	}

	if (g_bDebug)
	{
		if (GetKeyboard()->IsKeyDown(DIK_LALT) && GetRegion())
		{
			char szTmp[100];
			sprintf(szTmp, "current view distance:%.2f", GetRegion()->GetRegionCamera()->GetDistance());
			TextOut(10, 120, szTmp, strlen(szTmp), 0xffffffff);
		}

		if (GetKeyboard()->IsKeyDown(DIK_RALT))
			ShowFps();

		if (GetKeyboard()->IsKeyDown(DIK_LCONTROL) == false)
		{
			GameGraphics2D::RecoverFogState();
			return true;
		}

		if (GetKeyboard()->GetCurrentKey() == DIK_BACKSPACE && GetKeyboard()->IsKeyDown(DIK_RCONTROL))
		{
			char str[256];

#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_BACKSPACE+DIK_RCONTROL  reload file... Start.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			sprintf(str, "reload all model file...\n reload skill config file...\nreload sound config file...\nreload effect config file...\n");
			int x = SCREEN_WIDTH/2- (lstrlen(str)*GetFontWidth(-1)/2/4);
			int y = SCREEN_HEIGHT/2;
			GetGame()->TextOut(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
			GetGame()->TextOut(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,0,0));
			GetGame()->GetAudioList()->LoadSetup("data\\other.ini");
			CEffectSetup::LoadEffectSetup("data/effectsetup.ini");
			GetGame()->LoadArcheryEffect("data/ArcheryEffect.ini");
			GetGame()->LoadFace("data/Face.ini");	//读取表情文件
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_BACKSPACE+DIK_RCONTROL reload file... End.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
		}

		if (GetKeyboard()->GetCurrentKey()==DIK_F9)
		{
			char str[256];
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F9 reload all model file... Start.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			sprintf(str, "reload all model file...");
			int x = SCREEN_WIDTH/2-(lstrlen(str)*GetFontWidth(-1)/2);
			int y = SCREEN_HEIGHT/2;
			GetGame()->TextOut(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
			GetGame()->TextOut(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));

			CGameModelManager::GetInstance()->ReloadAllModel();
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F9 reload all model file... End.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
		}

		if (GetKeyboard()->GetCurrentKey()==DIK_F10)
		{
			char str[256];
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F10 reload file... Start.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			sprintf(str, "reload skill config file...\nreload sound config file...\nreload effect config file...\nreload state config file...\nreload country config file....\n");
			int x = SCREEN_WIDTH/2-(lstrlen(str)*GetFontWidth(-1)/2);
			int y = SCREEN_HEIGHT/2;
			GetGame()->TextOut(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
			GetGame()->TextOut(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));
			GetGame()->GetAudioList()->LoadSetup("data\\other.ini");
			CEffectSetup::LoadEffectSetup("data/effectsetup.ini");
			CStateListXml::Load("data/StateXml.xml");
			GetGame()->LoadArcheryEffect("data/ArcheryEffect.ini");
			GetGame()->LoadFace("data/Face.ini");	//读取表情文件

			CCountryList::Load("data/CountryList.xml");

#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F10 reload file... End.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
		}

		if (GetKeyboard()->GetCurrentKey()==DIK_F11)
		{
			char str[256];
#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F11 reload color config file... Start.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
			sprintf(str, "reload color config file...");
			int x = SCREEN_WIDTH/2-(lstrlen(str)*GetFontWidth(-1)/2);
			int y = SCREEN_HEIGHT/2;
			GetGame()->TextOut(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
			GetGame()->TextOut(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));
			CGlobeSetup::Load("data/globesetup.ini");

#ifdef _RUNSTACKINFO_
			sprintf_s(str,"DIK_F11 reload color config file... End.");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif

		}
	}
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::Display End.");
#endif
	GameGraphics2D::RecoverFogState();
	return true;
}
void CGame::PresentScence()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if (FAILED(pInterface->GetDevice()->Present( NULL, NULL, NULL, CVideoManager::GetInstance()->GetClipList() )))
	{

		HRESULT hr = pInterface->GetDevice()->TestCooperativeLevel();
		if (hr == D3DERR_DEVICELOST)
		{
			if (GetGame())
			{
				if (GetGame()->GetRegion())
				{
					GetGame()->GetRegion()->GetMiniMap()->DestroyDirect3DResource();

				}
			}
			if(CTopMsg::GetInstance()->GetRollLantern())
			{
				CTopMsg::GetInstance()->GetRollLantern()->ReleaseOffscreen();
			}

			CTextureProjective::CheckReSetRelease();
			pInterface->ResetRelease();
			pInterface->SetDeviceLost(TRUE);
			GetGame()->IGWOnLostDevice();
			return;
		}
		if (hr == D3DERR_DEVICENOTRESET)
		{
			if (FAILED(pInterface->GetDevice()->Reset(&pInterface->GetInfo()->d3dpp)))
			{
				if (pInterface->Reset())
				{
					pInterface->SetDeviceLost(FALSE);
					CTextureProjective::CheckReSetRelease();
					CTextureProjective::CheckReSetReBuild();
					GetGame()->IGWOnResetDevice();
					if(CTopMsg::GetInstance()->GetRollLantern())
					{
						CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
						CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
					}
					if (GetGame())
					{
						if (GetGame()->GetRegion())
						{
							GetGame()->GetRegion()->GetMiniMap()->DestroyDirect3DResource();
							GetGame()->GetRegion()->GetMiniMap()->ResetDirect3DResource();
						}

					}
				}
			}
			else
			{
				pInterface->SetDeviceLost(FALSE);
				pInterface->ResetRebuild();
				CTextureProjective::CheckReSetReBuild();
				GetGame()->IGWOnResetDevice();
				if(CTopMsg::GetInstance()->GetRollLantern())
				{
					CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
					CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
				}
				if (GetGame())
				{
					if (GetGame()->GetRegion())
					{
						GetGame()->GetRegion()->GetMiniMap()->ResetDirect3DResource();
					}

				}

			}

		}
		else
		{
			PutDebugString("Present not device lost fail!");
			if (GetGame())
			{
				if (GetGame()->GetRegion())
				{
					GetGame()->GetRegion()->GetMiniMap()->DestroyDirect3DResource();

				}
			}
			if(CTopMsg::GetInstance()->GetRollLantern())
			{
				CTopMsg::GetInstance()->GetRollLantern()->ReleaseOffscreen();
			}

			CTextureProjective::CheckReSetRelease();
			pInterface->ResetRelease();
			GetGame()->IGWOnLostDevice();
			if (FAILED(pInterface->GetDevice()->Reset(&pInterface->GetInfo()->d3dpp)))
			{
				if (pInterface->Reset())
				{
					pInterface->SetDeviceLost(FALSE);
					CTextureProjective::CheckReSetReBuild();
					GetGame()->IGWOnResetDevice();
					if(CTopMsg::GetInstance()->GetRollLantern())
					{
						CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
						CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
					}
					if (GetGame())
					{
						if (GetGame()->GetRegion())
						{
							GetGame()->GetRegion()->GetMiniMap()->ResetDirect3DResource();
						}
					}
				}
			}
		}
	}
	else
	{
		pInterface->SetDeviceLost(FALSE);

	}

}


/*
* 功能: 从文件载入配置信息
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.05.26 - lpf
*		增加了2D和3D线性过滤配置信息的载入
*	2008.05.26 - 刘科
*		增加了Tab选择对象的设置
*/
void CGame::LoadSetup()
{
	string strTemp;
	ifstream stream;
	stream.open("setup/params.ini");
	if( stream.is_open() )
	{			
		stream	>> strTemp >> m_Setup.bFullscreen
			>> strTemp >> m_Setup.lWidth
			>> strTemp >> m_Setup.lHeight
			>> strTemp >> m_Setup.lBitsPerPel
			>> strTemp >> m_Setup.lFrequency
			>> strTemp >> m_Setup.lTexturemode
			>> strTemp >> m_Setup.b2DLinear
			>> strTemp >> m_Setup.b3DLinear
			>> strTemp >> m_Setup.lLight
			>> strTemp >> m_Setup.lModel
			>> strTemp >> m_Setup.lShadow
			>> strTemp >> m_Setup.lTexture
			>> strTemp >> m_Setup.bRoleFlash
			>> strTemp >> m_Setup.bRoleShake
			>> strTemp >> m_Setup.bShock
			>> strTemp >> m_Setup.lWeather
			>> strTemp >> m_Setup.lControl
			>> strTemp >> m_Setup.lViewRange
			>> strTemp >> m_Setup.bSound
			>> strTemp >> m_Setup.bMusic
			>> strTemp >> m_Setup.bSpeech
			>> strTemp >> m_Setup.fSoundVolume 
			>> strTemp >> m_Setup.fMusicVolume
			>> strTemp >> m_Setup.fSpeechVolume
			>> strTemp >> m_Setup.bOthersFootstep
			>> strTemp >> m_Setup.lAntiAliasing
			>> strTemp >> m_Setup.bUseVSync
			>> strTemp >> m_Setup.lFrameControl
			>> strTemp >> m_Setup.lnotsimplemodelnum
			>> strTemp >> m_Setup.bupdateonlostfoucs
			>> strTemp >> m_Setup.bAotuRestorecamera;
		stream.close();
		m_Setup.fSoundVolume=min(max(0,m_Setup.fSoundVolume/255.f),1);
		m_Setup.fMusicVolume=min(max(0,m_Setup.fMusicVolume/255.f),1);
		m_Setup.fSpeechVolume=min(max(0,m_Setup.fSpeechVolume/255.f),1);
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setup/params.ini", "message", MB_OK);
	}
}



/*
* 功能: 保存配置信息到文件
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.05.26 - lpf
*		增加了2D和3D线性过滤配置信息的保存
*/
void CGame::SaveSetup()
{
	fstream stream;
	stream.open("setup/params.ini");
	if( stream.is_open() )
	{
		stream 
			<< "fullscreen	"	<< m_Setup.bFullscreen << endl
			<< "width		"	<< m_Setup.lWidth << endl
			<< "height		"	<< m_Setup.lHeight << endl
			<< "BitsPerPel	"	<< m_Setup.lBitsPerPel << endl
			<< "frequency	"	<< m_Setup.lFrequency << endl
			<< "texturemode	"	<< m_Setup.lTexturemode << endl
			<< "2DLinear	"	<< m_Setup.b2DLinear << endl
			<< "3DLinear	"	<< m_Setup.b3DLinear << endl
			<< "LIGHT		"	<< m_Setup.lLight << endl
			<< "MODEL		"	<< m_Setup.lModel << endl
			<< "SHADOW		"	<< m_Setup.lShadow << endl
			<< "TEXTURE		"	<< m_Setup.lTexture << endl
			<< "RoleFlash	"	<< m_Setup.bRoleFlash << endl
			<< "RoleShake	"	<< m_Setup.bRoleShake << endl
			<< "ScreenShock	"	<< m_Setup.bShock << endl
			<< "WEATHER		"	<< m_Setup.lWeather << endl
			<< "CONTROL		"	<< m_Setup.lControl << endl
			<< "VIEWRANGE	"	<< m_Setup.lViewRange << endl
			<< "SOUND		"	<< m_Setup.bSound << endl 
			<< "MUSIC		"	<< m_Setup.bMusic << endl
			<< "SPEECH		"	<< m_Setup.bSpeech << endl
			<< "SOUNDVOLUME	"	<< Audio::s_fSoundVolume * 255  << endl 
			<< "MUSICVOLUME	"	<< Audio::s_fMusicVolume * 255  << endl
			<< "SPEECHVOLUME "	<< Audio::s_fSpeechVolume * 255  << endl
			<< "FOOTSTEP	 "	<< m_Setup.bOthersFootstep  << endl
			<< "AntiAliasing "  << m_Setup.lAntiAliasing  << endl
			<< "bUseVSync    "  << m_Setup.bUseVSync << endl
			<< "FrameControl    "   << m_Setup.lFrameControl << endl
			<< "lsimplemodelnum "   << m_Setup.lnotsimplemodelnum << endl
			<< "bupdatefoucs    "   << m_Setup.bupdateonlostfoucs << endl
			<< "bAotuRestorecamera " << m_Setup.bAotuRestorecamera << endl;
		stream.close();
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setup/params.ini", "message", MB_OK);
	}
}

// Tab保存和装载
void CGame::LoadTabSet()
{
	string strTemp;
	ifstream stream;
	stream.open("setup/tabset.ini");
	if( stream.is_open() )
	{			
		stream  >> strTemp >> m_TabSet.bEnemyPlayer
			>> strTemp >> m_TabSet.bPiratePlayer
			>> strTemp >> m_TabSet.bMonster;
		stream.close();
		// 默认为怪物类型
		if(!m_TabSet.bEnemyPlayer && !m_TabSet.bPiratePlayer && !m_TabSet.bMonster)
			m_TabSet.bMonster = true;
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setup/tabset.ini", "message", MB_OK);
	}
}

void CGame::SaveTabSet()
{
	fstream stream;
	stream.open("setup/tabset.ini");
	if( stream.is_open() )
	{
		//Tab选择对象设置
		stream 
			<< "bTabEnemyPlayer		"	<< m_TabSet.bEnemyPlayer << endl
			<< "bTabPiratePlayer	"	<< m_TabSet.bPiratePlayer << endl
			<< "bTabMonster		"		<< m_TabSet.bMonster << endl;
		stream.close();
	}
	else
	{
		MessageBox(g_hWnd, "Can't find file setup/tabset.ini", "message", MB_OK);
	}
}

void CGame::LoadGameViewSetup()
{
	stringstream reader;	
	string setupName;
	CRFile* prfile = rfOpen("data\\GameViewSetup.ini");
	if (!prfile)
	{
		m_GameViewSetup.ViewFrustumRange = 30;
		m_GameViewSetup.ViewPlanformAngle = 44;
		return;
	}
	prfile->ReadToStream(reader);
	rfClose(prfile);
	reader>>setupName>>m_GameViewSetup.ViewFrustumRange;
	reader>>setupName>>m_GameViewSetup.ViewPlanformAngle;
}

void CGame::LoadRegionData()
{
	m_mapRegion.clear();

	stringstream reader;	
	CRFile* prfile = rfOpen("data\\regiondata.ini");
	if (!prfile)
		return;
	prfile->ReadToStream(reader);
	rfClose(prfile);

	while(ReadTo(reader, "*"))
	{
		tagRegion stRegion;
		long lID;
		string filename;
		stRegion.bgMusicID=0;
		stRegion.bgMusicPlaying=false;
		stRegion.strBgMusic="music/";
		reader>>lID>>filename>>stRegion.strLoadingPic;
		stRegion.strBgMusic+=filename;

		///////////////////
		stRegion.strFightMusic="music/";		
		reader>>filename;	//fight music
		stRegion.strFightMusic+=filename;		
		stRegion.FightMusicID=0;
		stRegion.bFightMusicPlaying=false;
		///////////////////

		stRegion.bPlayerPointLight=false;
		reader>>filename;	//player point light on-off
		if(filename!="0")
		{
			stRegion.bPlayerPointLight=true;
		}

		reader>>stRegion.fPlayerPointLightRadius;	//player point light radius

		int a=0,r=0,g=0,b=0;
		reader>>a>>r>>g>>b;
		stRegion.dwPlayerPointLightColor=D3DCOLOR_ARGB(a,r,g,b);

		int nUnderWater;
		reader >> nUnderWater;
		stRegion.bUnderWater = nUnderWater==0?false:true;

		m_mapRegion[lID]=stRegion;
	}

	stringstream stream;
	prfile = rfOpen("data\\loading.ini");
	if (!prfile)
		return;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	m_vectorLoadingTips.clear();
	while(ReadTo(stream, "*"))
	{
		string str;
		stream >> str;
		m_vectorLoadingTips.push_back(str);
	}
}



/*
* 功能: 游戏初始化
* 摘要: -
* 参数: -
* 返回值: 总是返回true
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.04.10 - lpf
*		增加了仪态动作配置文件的读取;
*	2008.06.17 - lpf
*		根据仪态动作新的结构修改了对仪态动作的载入处理;
*	2008.07.07 - lpf
*		增加了选中对象图片的加载,并去掉了原三种选中对象的图片对象的加载;
*	2008.08.21 - lpf
*		增加了视频管理器的初始化工作;
*	2008.11.11 - lpf
*		修改"载入仪态配置信息"时的路径;
*	2009.04.08 - lpf
*		增加了m_pbitmapConTip对象图片资源的处理;
*	2009.11.13 - lpf
*		增加了m_pPKIcon图片资源的创建;
*	2010.06.10 - lpf
*		增加了XML读取类的语言环境设定;
*	2010.06.24 - lpf
*		增加了LoadCameraSetup()调用，以载入摄像机设定;
*/
int CGame::Init(void)
{
#ifdef _RUNSTACKINFO_
	//StartStackInfo();
#endif
	CreateDirectory( "log",	NULL);

	PutDebugString("Init begin...");

	srand((unsigned)time(NULL));
	random(100);

	CHAR* strPath = new CHAR[MAX_PATH];
	string strWorpath("");
	if(::GetCurrentDirectory(MAX_PATH,strPath))
	{
		strWorpath=string(strPath);
	}
	delete []strPath;

	//第一次运行时写入注册表
	WriteReg("Client_Path",strWorpath.c_str());


	string strPackName("Game.pak"); 
	string strFilesInfo("PackRes.cfg");
	CClientResource::InitialInstance();
	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Initial(GAME_RES,strWorpath,strFilesInfo);
	if(!pClientResource->LoadEx())
	{
		//装载客户端资源信息出错
		//ASSERT(false);
		//return false;
	}
	PutDebugString("Init package file OK!");
	//初始化模型管理器
	CGameModelManager::InitInstance();
	//设置纹理共享文件夹
	CGameModelManager::GetInstance()->SetTextureShareFolder("Texture");
	//初始化建物管理器
	GameBuildList::InitInstance();

	m_pDBAllocator = new CDataBlockAllocator(100,1024);
	extern HINSTANCE hInst;
	m_pMouse = new CMouse();
	m_pMouse->Create( hInst, g_hWnd );
	m_pKeyboard = new CKeyBoard();
	m_pKeyboard->Create( hInst, g_hWnd );

	PutDebugString("Init Input OK!");

	CClientWordsFilter::GetInstance()->Initial(string("Data/filtrater.dat"));

	PutDebugString("Init Audio Engine OK!");

	CMessage::Initial(m_pDBAllocator,100);

	PutDebugString("Init Net OK!");

	CGlobeSetup::Load("data/globesetup.ini");

	// 初始化网络
	CMySocket::MySocketInit();
	CMySocket::SetEncryptFlag(CGlobeSetup::GetSetup()->lEncryptType);

	m_pDupliRegionSetup = new CDupliRegionSetup();

	//初始化图形模块
	this->CreateGraphices();

	PutDebugString("Init Graphics OK!");

	m_pPicList=new CPicList;
	m_pAudioList = new CAudioList;
	m_pAudioList->SetMusicEnabled(m_Setup.bMusic);
	m_pAudioList->SetSoundEnabled(m_Setup.bSound);
	m_pAudioList->SetSpeechEnabled(m_Setup.bSpeech);
	m_pAudioList->SetMusicVolume(m_Setup.fMusicVolume);
	m_pAudioList->SetSoundVolume(m_Setup.fSoundVolume);
	m_pAudioList->SetSpeechVolume(m_Setup.fSpeechVolume);


	//=====================游戏界面初始化==========================//
	// 客户端字符串统一管理初始化
	CStringReading::InitUniString("textresource/globaltextres.xml","GlobalFile");
	CStringReading::InitUniString("textresource/c_globaltextres.xml","GlobalFile");

	CPlayer::InitPlayerOccuType();	// 初始化职业描述
	CPlayer::InitPlayerConstType();	// 初始化星座描述

	//构造界面绘图对象
	g_pGuangMGraphics2D = new CGuangMGraphics2D;
	g_pGuangMGraphics2D->SetSize(GetSetup()->lWidth,GetSetup()->lHeight);
	g_pGuangMGraphics2D->SetClip(0,0,GetSetup()->lWidth,GetSetup()->lHeight);

	// 初始化物品操作配置对象
	m_GoodsExSetup.ReadSetup( "Data/GoodsExSetup.xml" );

	//构造界面管理类
	g_pGuangMGUIRoot = new CGuangMGuiRoot(g_pGuangMGraphics2D);
	IGUIRoot::SetGUIInstance(g_pGuangMGUIRoot);
	IGUIRoot::GetGUIInstance()->Initial(GetSetup()->lWidth,GetSetup()->lHeight);

	//构造界面的图像加载器
	g_pGuangMImageLoader = new CGuangMImageLoader;

	//构造界面的图像管理工厂
	g_pGuangMFactory = CImageFactory::GetDefaultFactory();
	g_pGuangMFactory->SetImageLoader(g_pGuangMImageLoader);

	//构造界面启动类
	g_pGuangMGUIApp	= new CGuangMGUIApp;
	g_pGuangMGUIApp->Init(g_pGuangMGUIRoot,g_pGuangMFactory->GetImageLoader());
	g_pGuangMGUIApp->Start();

	//构造界面扩展管理类
	m_pGuiEx = new CGuiEx;
	m_pGuiEx->InitGuiEx();

	CVideoManager::GetInstance()->Init(g_hWnd);

	m_pMsgBoxEvent = new CMsgBoxEvent;

	PutDebugString("Init Interface OK!");

	//读取登陆方式
	ReadLogMode();

	m_pGameControl = new CGameControl;

	SetGameState( GS_LOGIN );
	::SendMessage(g_hWnd, WM_SETCURSOR, 0, (LPARAM)LoadCursor(NULL, IDC_ARROW));
	//-----------------07.9.4(liuke任务)------------------
	CQuestSystem::Init("quest/QuestList.xml");
	// 各个职业的技能列表
	m_pOccupSkillList = new COccupSkillList;
	m_pOccupSkillList->Init("data/OccupSkillList.xml");
	// 技能学习条件表
	if(CStudySkillCondition::Init("data/StudySkillCondition.xml"))
		PutDebugString("Init StudySkillCondition OK!");
	// 转职相关配置
	if(CChangeOccu::Init("data/ChangeOccu.xml"))
		PutDebugString("Init ChangeOccu OK!");
	//=============================================================//
	LoadMonsterQuestEffect("data/MonsterQuestEffect.ini");
	CChat::Init();

	if (CCreateRolePageEx::LoadFaceHairIni("data/FaceHair.ini"))
		PutDebugString("Init FaceHair OK!");
	if( CGoodsList::LoadGoodsList("data/goods.dat") )
		PutDebugString("Init GoodsList OK!");
	if( CCardMutifunPageEx::InitCostData("data/EnchaseCost.ini") )
		PutDebugString("Init EnchaseCost OK!");
	CGoodsList::LoadSuitList("data/suit.dat");
	PutDebugString("Init Suit OK!");
	CGoodsList::LoadCardList("data/enchase.ini");
	PutDebugString("Init Card OK!");
	CGoodsList::InitMoneyGraphics("data/money.ini");
	PutDebugString("Init Money.ini OK!");

	if (CFacturePageEx::LoadFactureData("data/Facture.ini"))
		PutDebugString("Init Facture.ini OK!");

	CBusinessSystem::InitCreditLevel("data/CreditLevel.ini");
	PutDebugString("Init CreditLevel.ini OK!");

	if(CBusinessSystem::InitCreditSystem())
		PutDebugString("Init CreditSystem OK!");

	CEffectSetup::LoadEffectSetup("data/effectsetup.ini");
	PutDebugString("Init Effect OK!");
	CStateListXml::Load("data/StateXml.xml");
	PutDebugString("Init StateXml OK!");

	CSkillListXml::LoadSkillListXml();
	PutDebugString("Init skills OK!");
	CWeaponEffectXml::LoadWeaponEffect("data/WeaponEffectXml.xml");
	CCountryList::Load("data/CountryList.xml");
	PutDebugString("Init Country OK!");

	CRegionList::LoadMap();
	PutDebugString("Init RegionList OK!");

	CMailBoxPosList::LoadMailBoxPos();
	PutDebugString("Init LoadMailBoxPos OK!");

	CDeputyOccuSystemClient::InitOccuSystemClient("data/DeputyOccu/DeputyOccu.xml");
	PutDebugString("Init DeputyOccuSyestem OK!");

	//===07.12.10liuke更改===
	m_pbitmapShapeTalk = CBitmapX::GetBitmapX("interface/other/talkpaopao.tga");
	m_pbitmapTeamTalk = CBitmapX::GetBitmapX("interface/other/teamtalk.tga");

	m_pbitmapIntanateTimePot = CBitmapX::GetBitmapX("interface/other/cast-timepointer.tga");
	m_pbitmapIntanateBarBack = CBitmapX::GetBitmapX("interface/other/cast-understratum.tga");
	m_pbitmapIntanateBar = CBitmapX::GetBitmapX("interface/other/cast-time.tga");
	m_pbitmapHpBarBack = CBitmapX::GetBitmapX("interface/other/npc-understratum.tga");
	m_pbitmapPlayerHpBar = CBitmapX::GetBitmapX("interface/other/npc-HP.tga");
	m_pbitmapMonsterHpBar = CBitmapX::GetBitmapX("interface/other/npc-HP2.tga");
	m_pbitmapHpBar2 = CBitmapX::GetBitmapX("interface/other/npc-HP3.tga");
	m_pbitmapHpBarPet = CBitmapX::GetBitmapX("interface/other/npc-HP.tga");
	//=======================

	m_pbitmapSuck = CBitmapX::GetBitmapX("interface/other/suck.tga");
	m_pbitmapBlastSuck = CBitmapX::GetBitmapX("interface/other/blastsuck.tga");
	m_pbitmapBlastAttack = CBitmapX::GetBitmapX("interface/other/blastattack.tga");
	m_pbitmapBlock = CBitmapX::GetBitmapX("interface/other/block.tga");
	m_pbitmapBlastBlock = CBitmapX::GetBitmapX("interface/other/blastblock.tga");
	m_pbitmapRebound = CBitmapX::GetBitmapX("interface/other/rebound.tga");
	m_pbitmapBlastRebound = CBitmapX::GetBitmapX("interface/other/blastrebound.tga");
	m_pbitmapFullMiss = CBitmapX::GetBitmapX("interface/other/fullmiss.tga");
	m_pbitmapBlastFullMiss = CBitmapX::GetBitmapX("interface/other/blastfullmiss.tga");
	m_pbitmapParry = CBitmapX::GetBitmapX("interface/other/parry.tga");
	m_pbitmapBlastParry = CBitmapX::GetBitmapX("interface/other/blastparry.tga");
	m_pbitmapResist = CBitmapX::GetBitmapX("interface/other/resist.tga");
	m_pbitmapBlastResist = CBitmapX::GetBitmapX("interface/other/blastresist.tga");
	m_pbitmapLost = CBitmapX::GetBitmapX("interface/other/miss.tga");
	m_pbitmapExpTip = CBitmapX::GetBitmapX("interface/other/exp.tga");
	m_pbitmapConTip = CBitmapX::GetBitmapX("interface/other/Contribute.tga");

	m_pHandsBackIcon = CBitmapX::GetBitmapX("interface/public/32pxunder.tga");

	PutDebugString("Init bitmaps OK!");
	LoadRegionData();
	PutDebugString("Init RegionData OK!");

	LoadArcheryEffect("data/ArcheryEffect.ini");
	PutDebugString("Init ArcheryEffect OK!");

	LoadFace("data/Face.ini");	//读取表情文件
	PutDebugString("Init Face OK!");

	LoadTabSet();	//读取Tab设置文件
	PutDebugString("Init tabset OK!");

	m_pFaceActionList = new CFaceActionList();
	if (m_pFaceActionList->LoadFaceAction("data/FaceAction"))		//载入仪态配置信息
		PutDebugString("Init Face Action OK!");

	UpdateSetup();

	LoadMouseCursors("data/cursors.ini");
	PutDebugString("Init cursors OK!");

	CCosmetology::InitCosmetology("data/Cosmetology.xml");
	PutDebugString("Init Cosmetology OK!");

	CFuncKeyMapSystem::LoadDefaultFuncMapKey("data/FuncMapKey.xml");
	PutDebugString("Init FuncKeyMapSystem OK!");

	m_pBitMapFactionWarEnemy= CBitmapX::GetBitmapX("interface/other/FactionWarEnemy.TGA");
	m_pBitMapFactionWarUion= CBitmapX::GetBitmapX("interface/other/FactionWarUnion.TGA");

	m_pBitMapCityWarEnmey= CBitmapX::GetBitmapX("interface/other/CityWarEnemy.TGA");
	m_pBitMapCityWarUnion= CBitmapX::GetBitmapX("interface/other/CityWarUnion.TGA");	

	m_pBitMapOwnedCity= CBitmapX::GetBitmapX("interface/other/OwnedCity.TGA");	
	m_pBitMapOwnedVillage= CBitmapX::GetBitmapX("interface/other/OwnedVillage.TGA");

	m_dwFaceTexture = render::Interface::GetInstance()->CreateTexture("pictures/face/face.tga");

	PutDebugString("Init FaceTexture OK!");	

	m_pSelectShape = CBitmapX::GetBitmapX("pictures/SelectShape.tga");
	m_pPKIcon = CBitmapX::GetBitmapX("pictures/pkico.tga");
	m_pMouseShapeFoot = new CEffect;
	m_pMouseShapeFoot->SetGraphicsID(3000002);
	m_pMouseShapeFoot->SetDisplayModel();
	m_pMouseShapeFoot->SetDisplayModelGroup();
	m_pMouseTeammerFoot = new CEffect;
	m_pMouseTeammerFoot->SetGraphicsID(3000004);
	m_pMouseTeammerFoot->SetDisplayModel();
	m_pMouseTeammerFoot->SetDisplayModelGroup();
	m_pLockAttackShapeFoot = new CEffect;
	m_pLockAttackShapeFoot->SetGraphicsID(3000006);
	m_pLockAttackShapeFoot->SetDisplayModel();
	m_pLockAttackShapeFoot->SetDisplayModelGroup();

	PutDebugString("Init CEffect 3000006 OK!");	

	//读取场景摄像机配置文件
	LoadGameViewSetup();

	//读取其它EXE文件配置
	LoadOtherExeIni();

	RunOtherExe();

	PutDebugString("Init OK!");
	//初始化完成打开用户协议界面
	if(m_bShowUserTreaty&&!CompareVersion())
	{
		GetGame()->GetCGuiEx()->GetTreatyPageEx()->Open();
		GetGame()->GetCGuiEx()->GetTreatyPageEx()->SetTreatyComp(0);
	}
	return true;
}



/*
* 功能: 游戏释放处理
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.04.09 - lpf
*		增加了玩家和怪物的仪态动态数组释放;
*	2008.06.17 - lpf
*		根据仪态动作新的结构修改了对仪态动作的释放处理;
*	2008.06.25 - lpf
*		修改了仪态动作列表的释放位置;
*	2008.07.07 - lpf
*		增加了选中对象图片的释放,并去掉了原三种选中对象的图片对象的释放;
*	2008.08.21 - lpf
*		增加了视频管理器的释放工作;
*	2009.04.08 - lpf
*		增加了m_pbitmapConTip对象图片资源的释放;
*	2009.11.13 - lpf
*		增加了m_pPKIcon图片资源的释放;
*/
int CGame::Release(void)
{
#ifdef _RUNSTACKINFO_
	EndStackInfo();
#endif
	if (g_isSDOLogin && g_SDOAApp)
	{ // 登录成功，退出时要登出
		g_SDOAApp->Logout();
		g_isSDOLogin = false;
	}
	/* 释放IGW图形引擎 */
	if (g_SDOADx9)
		g_SDOADx9->Finalize();
	// 游戏释放
	CloseIGW();

	if(m_pMemClient)
	{
		m_pMemClient->Release();
		SAFE_DELETE(m_pMemClient);
	}

	s_bExit = true;
	SaveSetup();
	SaveTabSet();
	SAFE_DELETE(m_pMouse);
	SAFE_DELETE(m_pKeyboard);
	GetGame()->GetAudioList()->SetSoundEnabled(false);

	render::Interface * pInterface = render::Interface::GetInstance();

	CClientResource* pClientResource = CClientResource::GetInstance();
	pClientResource->Release();

	//释放资源管理器
	CClientResource::ReleaseInstance();
	PutDebugString("Release ClientResource OK!");

	// 发送一个玩家退出游戏消息
	if( m_pMainPlayer && GetNetClient() )
	{
		CMessage Msg( MSG_C2S_RGN_EXIT );
		Msg.Add((char*)GetGame()->GetCDKEY());
		Msg.Send();
	}

	if( m_pRegion )
	{
		m_pRegion->RemoveObject(m_pMainPlayer);
	}

	// 
	CGCMana::Inst()->Clear();
	PutDebugString("Clear CGCMana::Inst() OK!");

	//
	SAFE_DELETE( m_pRegion );	
	PutDebugString("Remove Region OK!");
	SAFE_DELETE( m_pMainPlayer );
	PutDebugString("Remove MainPlayer OK!");
	//=====================界面资源释放==============================
	m_pGuiEx->Release();
	SAFE_DELETE(m_pGuiEx);
	PutDebugString("Release Gui OK!");
	if( g_pGuangMGraphics2D != NULL )
	{
		g_pGuangMGraphics2D = NULL;
	}
	if( g_pGuangMGUIRoot != NULL )
	{
		g_pGuangMGUIRoot = NULL;
	}
	if( g_pGuangMImageLoader != NULL )
	{
		g_pGuangMImageLoader = NULL;
	}
	//释放完APP之后再释放图像（多线程绘制）
	if( g_pGuangMGUIApp != NULL )
	{
		g_pGuangMGUIApp->Stop();
		g_pGuangMGUIApp->Release();
		SAFE_DELETE(g_pGuangMGUIApp);
		g_pGuangMGUIApp	= NULL;

		PutDebugString("Release GUIApp OK!");
	}
	if( g_pGuangMFactory != NULL )
	{
		SAFE_DELETE(g_pGuangMFactory);
		PutDebugString("Release GuangMFactory OK!");
	}
	PutDebugString("Release GuiRes OK!");
	//--------------07.9.4-------------------
	CQuestSystem::Relese();
	PutDebugString("Release CQuestSystem OK!");
	CStudySkillCondition::Release();
	SAFE_DELETE(m_pOccupSkillList);
	PutDebugString("Release SkillCondition OK!");
	CChangeOccu::Release();
	list<stChatGoodsInfo>::iterator stCGInfoIt = m_lsChatGoodsInfo.begin();
	for (; stCGInfoIt != m_lsChatGoodsInfo.end(); ++stCGInfoIt)
	{
		SAFE_DELETE(stCGInfoIt->pGoods);
	}
	m_lsChatGoodsInfo.clear();
	PutDebugString("Release stChatGoodsInfo OK!");
	m_vecMonsterQuestEffect.clear();
	CMailBoxPosList::ReleaseMailBoxPos();
	//===============================================================
	if(m_pNetClient)
	{
		m_pNetClient->Close();
		m_pNetClient->Exit();
	}
	SAFE_DELETE( m_pNetClient );
	PutDebugString("Release Net OK!");
	SAFE_DELETE(m_pMsgBoxEvent);

	SAFE_DELETE(m_pPicList);
	PutDebugString("Release PicList OK!");
	SAFE_DELETE(m_pGameControl);
	PutDebugString("Release GameControl OK!");

	SAFE_DECREF(m_pbitmapShapeTalk);
	SAFE_DECREF(m_pbitmapTeamTalk);

	SAFE_DECREF(m_pbitmapIntanateTimePot);
	SAFE_DECREF(m_pbitmapIntanateBarBack);
	SAFE_DECREF(m_pbitmapIntanateBar);
	SAFE_DECREF(m_pbitmapHpBarBack);
	SAFE_DECREF(m_pbitmapPlayerHpBar);
	SAFE_DECREF(m_pbitmapMonsterHpBar);
	SAFE_DECREF(m_pbitmapHpBar2);
	SAFE_DECREF(m_pbitmapHpBarPet);
	SAFE_DECREF(m_pbitmapSuck);
	SAFE_DECREF(m_pbitmapBlastSuck);
	SAFE_DECREF(m_pbitmapBlastAttack);
	SAFE_DECREF(m_pbitmapBlock);
	SAFE_DECREF(m_pbitmapBlastBlock);
	SAFE_DECREF(m_pbitmapRebound);
	SAFE_DECREF(m_pbitmapBlastRebound);
	SAFE_DECREF(m_pbitmapFullMiss);
	SAFE_DECREF(m_pbitmapBlastFullMiss);
	SAFE_DECREF(m_pbitmapParry);
	SAFE_DECREF(m_pbitmapBlastParry);
	SAFE_DECREF(m_pbitmapResist);
	SAFE_DECREF(m_pbitmapBlastResist);
	SAFE_DECREF(m_pbitmapLost);
	SAFE_DECREF(m_pbitmapExpTip);
	SAFE_DECREF(m_pbitmapConTip);
	SAFE_DECREF(m_pHandsBackIcon);

	SAFE_DECREF(m_pBitMapFactionWarEnemy);
	SAFE_DECREF(m_pBitMapFactionWarUion);

	SAFE_DECREF(m_pBitMapCityWarEnmey);
	SAFE_DECREF(m_pBitMapCityWarUnion);

	SAFE_DECREF(m_pBitMapOwnedCity);
	SAFE_DECREF(m_pBitMapOwnedVillage);
	m_lLoadingDelayTime = 0;
	m_lLoadingTipsIndex = 0;
	SAFE_DECREF(m_pLoadingPic);

	SAFE_DECREF(m_pSelectShape);
	SAFE_DECREF(m_pPKIcon);
	SAFE_DELETE(m_pMouseShapeFoot);
	SAFE_DELETE(m_pMouseTeammerFoot);
	SAFE_DELETE(m_pLockAttackShapeFoot);

	extern CBitmapX *pPlayerShopBitmap;
	extern CBitmapX *pPlayerRecruitBitmap;
	extern CBitmapX *pPasswordBitmap;
	SAFE_DECREF(pPlayerShopBitmap);
	SAFE_DECREF(pPlayerRecruitBitmap);
	SAFE_DECREF(pPasswordBitmap);

	PutDebugString("Release All Game_BitmapX OK!");

	render::Interface::GetInstance()->DestroyTexture(m_dwFaceTexture);
	PutDebugString("Release FaceTexture OK!");

	CSkillListXml::Release();
	CStateListXml::Release();
	PutDebugString("Release Skill,State OK!");

	CWeaponEffectXml::Release();
	PutDebugString("Release Effect OK!");

	GetNetSessionManager()->Release();
	PutDebugString("Release NetSession OK!");

	GetOrganiCtrl()->Release();
	PutDebugString("Release OrganiCtrl OK!");

	SAFE_DELETE(m_pAudioList);
	PutDebugString("Release Audio OK!");

	CBusinessSystem::ReleaseCreditSystem();
	PutDebugString("Release CreditSystem OK!");

	//释放
	GameBuildList::GetInstance()->Release();
	GameBuildList::ReleaseInstance();
	PutDebugString("Release BuildList OK!");

	//释放
	CGameModelManager::GetInstance()->Release();
	CGameModelManager::ReleaseInstance();
	PutDebugString("Release Model OK!");

	CTextureProjective::Destroy();

	SAFE_DELETE(m_pFaceActionList);
	PutDebugString("Release Face Action List OK!");

	//销毁图形模块
	DestroyGraphices();
	PutDebugString("Destroy Graphices OK!");

	m_listFace.clear();

	CClientWordsFilter::Release();

	GetPlayerRanks()->Release();

	CDeputyOccuSystem::Relese();
	PutDebugString("Release deputyoccusystem OK!");

	SAFE_DELETE(m_pDupliRegionSetup);

	CMessage::Release();
	PutDebugString("Release CMessage OK!");

	SAFE_DELETE(m_pDBAllocator);
	PutDebugString("Release Data Block OK!");

#ifdef _DEBUG
#ifdef _CLIENT
#endif
#endif
	CStringReading::Release();

	CVideoManager::GetInstance()->Destroy();
	CVideoManager::ReleaseInstance();

	CCosmetology::Release();
	PutDebugString("Release Cosmetology OK!");

	CFuncKeyMapSystem::Release();
	PutDebugString("Release FuncKeyMapSystem OK!");

	PutDebugString("Release OK!");
	return true;
}

// 重新开始前的准备工作
void CGame::ReNew(void)
{
	m_lScriptTimer=0;
	SetBackColor(255, 0,0,0);
	GetGame()->GetCGuiEx()->CloseAllPage(true,GetGame()->GetCGuiEx()->GetLoginPageEx());
	SAFE_DELETE(m_pRegion);
	m_pMainPlayer = NULL;
	SetBackColor(255, 0,0,0);
	GetOrganiCtrl()->ReNew();

	m_bDisplayInfo = false;
	m_lDisplaySpeed = 1;
	m_TopInfos.clear();
	m_stCurTopInfo.listCurInfo.clear();
	m_stCurTopInfo.lTotalLength = 0;
	m_dwDisplayTime = timeGetTime();
	m_lLoadingDelayTime = 0;
	m_lLoadingTipsIndex = 0;
	SAFE_DECREF(m_pLoadingPic);
	//重置反外挂状态
	ResetCheatSystem();

	// 人物小退时处理下热键设置界面的数据
	GetGame()->GetCGuiEx()->GetHotKeySetupPageEx()->OnPlayerExitGame();
	GetGameControl()->ReNew();
}

//重置反外挂系统为初始状态
void CGame::ResetCheatSystem()
{
	m_bIsBeCheateForbid = false;
	m_pGuiEx->GetCheckCheatPageEx()->SetIsTimeOut(true);//小退后防外挂界面计时开始时间清零。
	m_pGuiEx->GetCheckCheatPageEx()->SetStartTime(0);
	m_pGuiEx->GetCheckCheatPageEx()->Close();
	m_pGuiEx->GetPinxyTalkBoxPageInstEx()->Close();
}
// 退出游戏
void CGame::QuitGame(void)
{
	if (!m_pGuiEx)
		return;
	CSystemPageEx * pSysPage = m_pGuiEx->GetSystemPageEx();
	CMessageBoxPageEx * pMsgBox = m_pGuiEx->GetMessageBoxPageEx();
	if(!pSysPage || !pMsgBox)
		return;

	// 王峰：避免多次调用
	if( ! pSysPage->m_bQuitGameMsgBox )
	{
		pSysPage->SetExitPage(0);
		pMsgBox->m_lCallerID = 0;

		if (GetMainPlayer() && GetMainPlayer()->GetState()==CShape::STATE_FIGHT)
		{
			pMsgBox->Open(pSysPage,CStringReading::LoadString(eIDS_BASEUI,eIDS_WAR_QUITGAME_ONFIGHT), CMessageBoxPageEx::MSG_OK,true);
		}
		else
		{
			pSysPage->m_bQuitGameMsgBox = true;
			pMsgBox->Open(pSysPage,CStringReading::LoadString(eIDS_BASEUI,eIDS_WAR_QUITGAME_TOWINDOWS), CMessageBoxPageEx::MSG_OK_NO,true);
		}

	}
}

// new MainPlayer
void CGame::NewMainPlayer()
{
	if (GetRegion())
	{
		GetRegion()->RemoveObject(m_pMainPlayer);
	}

	SAFE_DELETE(m_pMainPlayer);
	m_pMainPlayer = new CPlayer;
	m_pMainPlayer->SetIsSelf(true);
}

// 断开网络连接
void CGame::CloseNetClient(void)
{
	if(m_pNetClient)
	{
		m_pNetClient->Exit();
		m_pNetClient->Close();
	}
	if (g_isSDOLogin && g_SDOAApp)
	{ 
		// 登录成功，退出时要登出
		g_SDOAApp->Logout();
	}
	SAFE_DELETE( m_pNetClient );
}

//连接LoginServer服务器
bool CGame::ConnectLoginServer(const char* const logIP,long logPort,
							   const char* const cdKey,const vector<BYTE>& password, const char *wsName)
{
	ReNew();

	long hr = 0;
	if(m_pNetClient)
		m_pNetClient->Exit();
	SAFE_DELETE( m_pNetClient );

	if (wsName[0]!=0)
	{
		SetGameState(GS_BLACK);
		Sleep(1000);
	}

	m_pNetClient = new CClient;
	m_pNetClient->Start(m_pDBAllocator,20,NF_LS_Client);

	hr = m_pNetClient->Connect( logIP,logPort);	// 连接服务器
	if( !hr )	// 连接失败
	{
		m_pNetClient->Exit();
		m_pNetClient->Close();
		SAFE_DELETE( m_pNetClient );
		SetGameState(GS_LOGIN);
		return false;
	}

	CMessage::SetClient(m_pNetClient);

	C2L_CDKey_Send(cdKey,password,wsName);
	SetCDKEY(cdKey);
	SetPASSWORD(password);
	SetWSName(wsName);

	return true;
}

CClient* CGame::ConnectGameServer(const char* const gsIP,long gsPort,const CGUID& PlayerID)
{
	long hr = 0;

	// 清理序列
	CSequenceString::Clearup();
	CSequenceString::m_bIsUsable = false;

	// 清除验证时间
	GetGame()->m_ulServerTime = 0;
	GetGame()->m_ulServerTime = 0;
	GetGame()->m_bMsgTimeStart = false;

	////
	CClient* pNetClient = new CClient;
	pNetClient->Start(m_pDBAllocator,20,NF_GS_Client);

	hr = pNetClient->Connect(gsIP,gsPort);	// 连接服务器
	if( !hr )	// 连接失败
	{
		pNetClient->Exit();
		pNetClient->Close();
		SAFE_DELETE(pNetClient);
		return NULL;
	}


	// 发出进入消息
	long lSignCode = GetChangeServerCode();
	CMessage msg(MSG_C2S_LOG_PLAYERENTER);
	msg.Add((LONG)lSignCode);
	msg.Add(PlayerID);
	msg.Add((char *)GetGameCDKEY());
	msg.Send(pNetClient);

	return  pNetClient;
}

void CGame::OnServerLost()
{	
	ReNew();	// 重新开始		
	SetGameState( GS_LOGIN );
	SDOLogOut();
}


// 根据场景ID来绘制Loading画面
void CGame::DisPlayLoading(long lRegionID)
{
	// 显示loading画面
	PutDebugString("QuestEnterRegion(...) Test1!");
	render::Interface *pInterface = render::Interface::GetInstance();
	pInterface->BeginScene();
	pInterface->Clear(0);
	if(NULL == m_pLoadingPic)
	{
		m_lLoadingTipsIndex = 0;
		char strFilename[MAX_PATH] = "pictures/loading/default.tga";
		if (m_mapRegion.find(lRegionID)!=m_mapRegion.end())
		{
			strcpy(strFilename, m_mapRegion[lRegionID].strLoadingPic.c_str());
		}

		PutDebugString("QuestEnterRegion(...) Test2!");
		if (IsFileExist(strFilename)==false)
			strcpy(strFilename, "pictures/loading/default.tga");

		char strErrorInfo[256]="";
		sprintf(strErrorInfo,"fileName:%s",strFilename);
		PutDebugString(strErrorInfo);
		PutDebugString("QuestEnterRegion(...) Test21!");
		m_pLoadingPic = CBitmapX::GetBitmapX(strFilename);
		PutDebugString("QuestEnterRegion(...) Test22!");
	}
	if (m_pLoadingPic)
	{
		PutDebugString("QuestEnterRegion(...) Test23!");
		int x = (SCREEN_WIDTH - (int)m_pLoadingPic->GetWidth()) / 2;
		int y = (SCREEN_HEIGHT - (int)m_pLoadingPic->GetHeight()) / 2;
		m_pLoadingPic->Draw(x, y);
		if(m_lLoadingTipsIndex == 0)
			m_lLoadingTipsIndex = random((int)m_vectorLoadingTips.size());
		const char *pstr = m_vectorLoadingTips[m_lLoadingTipsIndex].c_str();
		PutDebugString("QuestEnterRegion(...) Test24!");

		x = (SCREEN_WIDTH - lstrlen(pstr) * GetGame()->GetFontWidth(-1)) / 2;
		y = y + (int)m_pLoadingPic->GetHeight() - 50;
		PutDebugString("QuestEnterRegion(...) Test25!");
		GetGame()->TextOut(x, y, pstr, lstrlen(pstr), D3DCOLOR_ARGB(255,255,255,255));
		PutDebugString("QuestEnterRegion(...) Test26!");
		PutDebugString("QuestEnterRegion(...) Test27!");
	}
	PutDebugString("QuestEnterRegion(...) Test28!");
	pInterface->EndScene();
	PutDebugString("QuestEnterRegion(...) Test3!");
	PresentScence();
}


/*
* 功能: 请求进入场景
* 摘要: 玩家收到 MSG_L2C_ANSWER_PLAYER 或 MSG_S2C_REGION_COMMOND_CHANGE 后请求进入场景
* 参数: lRegionID	   - 场景ID
*		 RegionID	   - 场景GUID
*		 lResourceID   - 资源ID
*		 strRegionName - 场景名
*		 lPlayerX	   - 玩家X坐标
*		 lPlayerY	   - 玩家Y坐标
*		 lPlayerDir	   - 玩家方向
*		 lUseGoods	   - 玩家使用物品
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.05.30 - lpf
*		增加了场景相机数据在场景切换之前的保存和赋值
*	2009.01.9 - 刘科
*		设置玩家所在场景GUID
*	2008.01.09 - lpf
*		修正了"初始化地图"时,new出来的临时场景相机对象内存没有释放,造成的内存泄露
*	2008.01.13 - lpf
*		修正了"显示loading画面"里,在计算背景图片显示xy坐标时,由于基本数据类型的隐式转换,造成的坐标值为非常大的数据.结果使绘制坐标不对(800*600的分辨率下)
*	2009.03.13 - lpf
*		注释了pPlayer->SetDisplayModelGroup();的设定,因为该设定在后面会调用
*/
#ifdef MOVE_IN_TILE
void CGame::QuestEnterRegion(long lRegionID,const CGUID& RegionID,long lResourceID,  char* strRegionName, float lPlayerX, float lPlayerY, long lPlayerDir, long lUseGoods)
#else
void CGame::QuestEnterRegion(long lRegionID,const CGUID& RegionID,long lResourceID,  char* strRegionName, long lPlayerX, long lPlayerY, long lPlayerDir, long lUseGoods)
#endif
{
	PutDebugString("QuestEnterRegion(...) Start!");
	// 音乐	
	map<long,tagRegion>::iterator RegionDataIter=m_mapRegion.begin();
	for(;RegionDataIter!=m_mapRegion.end();RegionDataIter++)
	{
		if(RegionDataIter->second.bgMusicPlaying || RegionDataIter->second.bFightMusicPlaying)
		{			
			RegionDataIter->second.bgMusicPlaying=false;
			RegionDataIter->second.bFightMusicPlaying=false;
			break;
		}
	}
	// 显示loading画面
	DisPlayLoading(lRegionID);
	PutDebugString("QuestEnterRegion(...) Test4!");

	CPlayer* pPlayer = GetGame()->GetMainPlayer();

	CRegionCamera * pRegionCamera = NULL;

	// 初始化地图
	if( m_pRegion )
	{
		for(long i = PET_TYPE_ELF; i<=PET_TYPE_COUNT;++i)
		{
			CPet *pPet = pPlayer->GetActivePet(i);
			if(pPet)
				m_pRegion->RemoveObject(pPet);
		}
		m_pRegion->RemoveObject(pPlayer);

		pRegionCamera  = new CRegionCamera();
		*pRegionCamera = *m_pRegion->GetRegionCamera();

		SAFE_DELETE(m_pRegion);
	}

	PutDebugString("QuestEnterRegion(...) Test5!");
	m_pRegion = new CClientRegion;
	PutDebugString("QuestEnterRegion(...) Test6!");
	GetRegion()->SetExID(RegionID);
	GetRegion()->SetResourceID(lResourceID);
	PutDebugString("QuestEnterRegion(...) Region Load!");
	GetRegion()->Load();
	PutDebugString("QuestEnterRegion(...) Region Init!");
	GetRegion()->Init();
	GetRegion()->SetName( strRegionName );

	if (pRegionCamera != NULL)
	{
		GetRegion()->SetRegionCamera(pRegionCamera);
		SAFE_DELETE(pRegionCamera);
	}

	// 加入玩家并设置坐标
	pPlayer->SetFather( GetRegion() );
	pPlayer->SetPosXY( lPlayerX+0.5f, lPlayerY+0.5f );
	pPlayer->SetDir( (CShape::eDIR)lPlayerDir );
	pPlayer->SetRegionID( lRegionID );
	pPlayer->GetRegionGUID( RegionID );

	pPlayer->SetDestXY(pPlayer->GetPosX(), pPlayer->GetPosY());
	GetGame()->GetGameControl()->SetUpdateXY(pPlayer->GetPosX(), pPlayer->GetPosY());

	extern bool g_bForceUpdate;
	g_bForceUpdate = true;
	// 玩家不是死亡或假死状态，设置成站立动作
	if(pPlayer->GetState() != CShape::STATE_DIED && pPlayer->GetState() != CShape::STATE_FEIGNDEATH)
		pPlayer->SetAction(CShape::ACT_STAND);
	else
	{
		pPlayer->SetAction(CShape::ACT_ALREADYDIED);
		GetGame()->GetCGuiEx()->GetRelivePageEx()->Open();
	}

	pPlayer->SetFather( GetRegion() );
	GetRegion()->AddObject( pPlayer );
	GetRegion()->ScrollCenter( pPlayer->GetPosX() , pPlayer->GetPosY());
	GetRegion()->SyncUpdate();

	SetGameState( GS_BLACK );
	// 清空走路计数
	//重置自动状态
	GetGameControl()->SetSendedMoveMsgCount(0);
	GetGameControl()->SetMPActionMode(CGameControl::MPAM_No);
	GetGameControl()->ContinueMoveToOtherRegion();

	//加入宠物并重新设置宠物
	for(long i = PET_TYPE_ELF; i<=PET_TYPE_COUNT;++i)
	{
		CPet *pPet = pPlayer->GetActivePet(i);
		if(pPet)
		{
			pPet->SetFather( GetRegion() );
			GetRegion()->AddObject( pPet );
		}
	}
	pPlayer->PetEnterRegion();
	m_lLoadingDelayTime = (long)timeGetTime();
	// 显示loading画面
	DisPlayLoading(lRegionID);
	PutDebugString("QuestEnterRegion(...) End!");
}



/*
* 功能: 游戏初始化图形模块
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.05.26 - lpf
*		增加了szConfig的2D和3D线性过滤配置字符串的构造
*	2009.2.14 - 刘科
*		修改成假全屏模式
*	2009.10.20 - lpf
*		增加了"初始化水面管理器"
*	2010.06.09 - lpf
*		增加了多语言初始化功能
*	2010.06.24 - lpf
*		增加了日语相关初始化
*/
void CGame::CreateGraphices(void)
{
	console::Manager::InitInstance();

#ifdef _DEBUG
	console::Manager::GetInstance()->Create(console::CONSOLE_ERROR | console::CONSOLE_OUTPUT | console::CONSOLE_WARRING);
#else
	console::Manager::GetInstance()->Create(console::CONSOLE_ERROR);
#endif

	render::Interface::InitInstance();
	particle::Manager::InitInstance();

	GameGraphics2D::InitInstance();

	char szConfig[1024];
	// modify by lpf 2007-11-27
	sprintf(szConfig,"render\n{\n	fullscreen = %d\n	hvp = false\n	texturemode = %d\n	2dlinear = %s\n	3dlinear = %s\n	font\n	{\n		style1 = font/12.font\n		style2 = font/1.font\n	}\n	window\n	{\n		x = 0\n		y = 0\n		width = %d\n		height = %d\n		autostyle = false\n		autocenter = false\n	}\n	fullscreen\n	{\n		backbufferfmt = %d\n	}\n}\n bUseVSync = %s\n",
		0,
		GetSetup()->lTexturemode,
		GetSetup()->b2DLinear ? "true" : "false",
		GetSetup()->b3DLinear ? "true" : "false",
		GetSetup()->lWidth,
		GetSetup()->lHeight,
		(GetSetup()->lBitsPerPel == 16) ? 23 : 22,
		GetSetup()->bUseVSync ? "true" : "false");

	//初始化显示部分
	render::Interface::GetInstance()->Create(g_hWnd, szConfig);

	GameGraphics2D::GetInstance()->Create();

	//初始化
	particle::Manager::GetInstance()->Create();

	// 天气
	m_pWeatherManager = new WeatherManager;
	m_pWeatherManager->Create();

	// 初始化水面管理器
	Water::CManager::InitInstance();

	// 初始化平面阴影数据
	m_FlatShadow.Create();
}



/*
* 功能: 销毁图形模块
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2009.10.20 - lpf
*		增加了水面管理器的销毁
*/
void CGame::DestroyGraphices(void)
{
	SAFEDESTROY(m_pWeatherManager);

	m_FlatShadow.Destroy();
	//销毁引擎
	Water::CManager::GetInstance()->Destroy();
	particle::Manager::GetInstance()->Destroy();
	render::Interface::GetInstance()->Destroy();
	console::Manager::GetInstance()->Destroy();

	CBitmapX::ReMoveAllBitmap();
	PutDebugString("Release AllBitmap OK!");
	GameGraphics2D::GetInstance()->Destroy();

	GameGraphics2D::ExitInstance();

	Water::CManager::ExitInstance();
	particle::Manager::ExitInstance();
	render::Interface::ExitInstance();	
	console::Manager::ExitInstance();
}

int CGame::GetFps(void)
{
	static char temp[256];
	static DWORD dwFpsTime = GetCurTickCount();
	static DWORD dwCount   = 0;
	dwCount ++;
	static int fps = 0 ;
	if ( GetCurTickCount() - dwFpsTime > 2000)
	{
		fps = dwCount / 2 ;
		dwCount = 0;		
		dwFpsTime = GetCurTickCount();
	}
	return fps;
}

void CGame::ShowFps(void)
{
	////DRAW FPS
	int iFps = GetFps();
	static char szTemp[256];
	sprintf(szTemp,"FPS: %3.3d %.6d",iFps,m_iCurrentRenderTriangleCount);
	TextOut(SCREEN_WIDTH/2,2,szTemp,lstrlen(szTemp),D3DCOLOR_ARGB(255,255,255,255));

}

////////////////////////////send msg to login server
void CGame::C2L_CDKey_Send(const char *cdkey,const vector<BYTE>& password,const char*wsName)
{
	if(m_pNetClient==NULL)
		return;
	extern long g_lVersion;

	char strVersion[RESOURCE_VERSION_SIZE]="";
	sprintf(strVersion,"%s",CClientResource::GetInstance()->GetVersion().c_str());
	char strcdkey[CDKEY_SIZE]="";
	sprintf(strcdkey,"%s",cdkey);
	char strwsName[WS_NAME_SIZE]="";
	sprintf(strwsName,"%s",wsName);


	CMessage msg( MSG_C2L_LOG_QUESTCDKEY );	
	msg.Add( g_lVersion );	// 游戏版本
	msg.AddEx((void*)&strVersion,RESOURCE_VERSION_SIZE);
	msg.AddEx((void*)&strcdkey,CDKEY_SIZE);
	msg.AddEx((void*)&password[0],password.size());
	msg.AddEx( (void*)&strwsName,WS_NAME_SIZE);
	msg.Send(m_pNetClient);
}

//向loginserver请求基本属性
void CGame::C2L_PlayerBase_Send(const char *wsName,const char *cdkey)
{
	if(m_pNetClient==NULL)
		return;
	CMessage msg(MSG_C2L_LOG_QUEST_PLAYERBASELIST);	
	msg.Add((char*)wsName);
	msg.Add((char*)cdkey);
	msg.Send(m_pNetClient);

	SetWSName(wsName);
}

//向loginserver请求删除一个角色
void CGame::C2L_DeleteRole_Send(const CGUID& ID)
{
	if(m_pNetClient==NULL)
		return;
	CMessage msg(MSG_C2L_LOG_QUESTDELETE);
	msg.Add(ID);
	msg.Send(m_pNetClient);
}

// 想loginserver请求恢复一个角色
void CGame::C2L_RestoreRole_Send(const CGUID& PlayerID)
{
	if(!m_pNetClient) return;

	CMessage msg(MSG_C2L_LOG_QUESTRESTORE);
	msg.Add(PlayerID);
	msg.Send(m_pNetClient);
}

//向loginserver请求添加一个角色
void CGame::C2L_AddRole_Send(const char *strName,char nOccupation, char nSex, BYTE lHair, BYTE lFace, BYTE lCountry,BYTE lConstellation,BYTE bRandCountry)
{
	if(m_pNetClient==NULL)
		return;
	CMessage msg(MSG_C2L_LOG_QUESTCREATE);
	msg.Add(const_cast<char*>(strName));
	msg.Add(nOccupation);
	msg.Add(nSex);
	msg.Add(lHair);
	msg.Add(lFace);
	msg.Add(lCountry);
	msg.Add(lConstellation);
	msg.Add(bRandCountry);
	msg.Send(m_pNetClient);
}

//向loginserver请求详细信息
void CGame::C2L_QuestDetail_Send(const CGUID& ID)
{
	if(m_pNetClient==NULL)
		return;
	long lChangeServer = GetSignCode();
	CMessage msg(MSG_C2L_LOG_QUEST_PLAYERDATA);
	msg.Add((LONG)lChangeServer);
	msg.Add(ID);
	msg.Send(m_pNetClient);
}


// -------------------------------------------------------
// Name: DrawString()
// Describe: 繪製字符串，並自動換行
//           nLength = 每行的字符数量
//           nLineSpace = 行间距
// -------------------------------------------------------
HRESULT CGame::DrawString(int x, int y, char* pString, D3DCOLOR Color, int nLength, int nLineSpace)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	if( pString == NULL || pString[0]==0 ) return false;
	int nStringLength = lstrlen(pString);

	static unsigned char pBuffer[512];

	unsigned char* pSrc = (unsigned char*)pString;
	unsigned char* pDest = pBuffer;

	int nOffsetY = y;
	bool bCheckSize = false;

	for(int i=0; i<(nStringLength / nLength + 1); i++)
	{
		memset(pBuffer, 0, nLength+1);			// clear temp buffer data
		for(int j=0; j<nLength; j++)
		{
			if(*pSrc == '\r' || *pSrc == '\n')	// return
			{
				pSrc++;
				i--;
				break;
			}
			else if(*pSrc == 0)					// end
			{
				break;
			}

			*pDest = *pSrc;

			if(*pSrc > 128)						// double size
			{
				if( bCheckSize )
					bCheckSize = false;
				else
					bCheckSize = true;
			}
			else
				bCheckSize = false;

			pDest++;
			pSrc++;
		}

		if( bCheckSize )
		{
			pDest--;
			*pDest = 0;
			pSrc--;
			bCheckSize = false;
		}

		TextOut(x, nOffsetY, (char*)pBuffer, lstrlen((char*)pBuffer), Color);
		nOffsetY += nLineSpace;
		pDest = pBuffer;
	}

	return S_OK;
}

// -------------------------------------------------------
// Name: DrawStringEx()
// Describe: 绘制带表情的字符串，並自动换行
//           nLength = 每行的字符数量
//           nLineSpace = 行间距
// -------------------------------------------------------
HRESULT CGame::DrawStringEx(int x, int y, char* pString, D3DCOLOR Color, D3DCOLOR BkColor, D3DCOLOR bkBarColor, int nLength, int nLineSpace)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	if( pString == NULL || pString[0]==0 ) return false;
	int nStringLength = lstrlen(pString);

	static unsigned char pBuffer[512];

	unsigned char* pSrc = (unsigned char*)pString;
	unsigned char* pDest = pBuffer;


	bool bCheckSize = false;
	int iCurLength = 0;
	int iLine = 0;
	// add by lpf 2007-11-28
	int iFontWidth = GetGame()->GetFontWidth(1);

	vector<string> strTextLine;	//输出的文字
	string strtemp;
	unsigned char* pTestFace;

	vector<tagFaceInfo> fiDrawFace;
	//匹配文字宽高的背景框
	int nZoneWidth = 0;
	int nZoneMaxWidth = 0;
	int nZoneHeight = 0;

	while (*pSrc)
	{
		if (iCurLength <= nLength)
		{
			//判断是否为表情代码
			if (*pSrc == '\\')
			{
				pTestFace = pSrc;
				pTestFace++;
				if ( *pTestFace >= 48 && *pTestFace <= 57 )
				{
					pTestFace++;
					if ( *pTestFace >= 48 && *pTestFace <= 57 )
					{
						//确定表情代码位置
						if (iCurLength+3 > nLength)
						{
							iCurLength+=3;
							continue;
						}

						string strFaceCode;
						strFaceCode.push_back(*pSrc++);
						strFaceCode.push_back(*pSrc++);
						strFaceCode.push_back(*pSrc++);

						for(list<tagFace>::iterator it = m_listFace.begin(); it!=m_listFace.end(); it++)
						{
							if(strFaceCode == it->strFaceCode)
							{
								//确定为表情代码,变为空格
								*pDest = ' ';
								pDest++;
								*pDest = ' ';
								pDest++;
								*pDest = ' ';
								pDest++;

								//保存要绘制的表情
								tagFaceInfo fitemp;
								int iWidth = it->iCurIndex % 16;
								int iHeight = it->iCurIndex / 16;
								fitemp.iWidth = 
									fitemp.iHeight = 32;

								fitemp.rc.left   =  iWidth * fitemp.iWidth;
								fitemp.rc.right  = fitemp.rc.left + fitemp.iWidth;
								fitemp.rc.top    =  iHeight * fitemp.iHeight;
								fitemp.rc.bottom = fitemp.rc.top + fitemp.iHeight;

								fitemp.iX = x+iCurLength*iFontWidth-4;
								fitemp.iY = y+iLine*nLineSpace-8;

								fiDrawFace.push_back(fitemp);
								iCurLength+=3;

								nZoneWidth += 24;
								if (nZoneWidth > nZoneMaxWidth)
								{
									nZoneMaxWidth = nZoneWidth;
								}
								continue;
							}
						}
						continue;	//是表情代码,但是本代码没有符合的表情
					}
				}
			}

			*pDest = *pSrc;

			if(*pSrc > 128)		//如果是中文
			{
				if( bCheckSize )
					bCheckSize = false;
				else
					bCheckSize = true;
			}
			else
				bCheckSize = false;

			pDest++;
			pSrc++;
			iCurLength++;

			nZoneWidth += iFontWidth;
			if (nZoneWidth > nZoneMaxWidth)
			{
				nZoneMaxWidth = nZoneWidth;
			}
		} 
		else
		{
			if( bCheckSize )
			{
				pDest--;
				*pDest = 0;
				pSrc--;
				bCheckSize = false;
				nZoneWidth -= iFontWidth;
				if (nZoneWidth < nZoneMaxWidth)
				{
					nZoneMaxWidth = nZoneWidth;
				}
			}

			iCurLength = 0;

			strtemp = (char*)pBuffer;
			strTextLine.push_back(strtemp);
			pDest = pBuffer;
			memset(pBuffer, 0, sizeof(pBuffer));
			iLine++;

			if (nZoneWidth > nZoneMaxWidth)
			{
				nZoneMaxWidth = nZoneWidth;
			}
			nZoneWidth = 0;
			nZoneHeight += nLineSpace;
		}
	}
	strtemp = (char*)pBuffer;
	strTextLine.push_back(strtemp);
	memset(pBuffer, 0, sizeof(pBuffer));

	if(m_pbitmapShapeTalk)
	{
		int nW = m_pbitmapShapeTalk->GetWidth()-nZoneMaxWidth;
		if (nW < 0)
			nW = abs(nW);
		m_pbitmapShapeTalk->Draw(x-nW/2+4,y-nLineSpace/2+2,bkBarColor | 0x00ffffff);
	}

	FONTINFO pFontInfo;
	pFontInfo.bShadow = false;
	pFontInfo.bBorder = false;
	pFontInfo.dwColorShadow = BkColor;
	pFontInfo.dwColorBorder = D3DCOLOR_ARGB(255,0,0,0);
	pFontInfo.dwColorFont = Color;
	pFontInfo.dwShadowOffset = 1;
	pFontInfo.iStyleIndex = 1;
	pFontInfo.fScale = 1.f;
	//绘制文字
	for (int iTextCount = 0, nOffsetY = y; iTextCount<strTextLine.size(); iTextCount++, nOffsetY += nLineSpace)
	{
		DrawText(x, nOffsetY, strTextLine[iTextCount].c_str(),&pFontInfo);
	}

	//绘制表情
	for (int ifiCount = 0; ifiCount<fiDrawFace.size(); ifiCount++)
	{
		GameGraphics2D::GetInstance()->DrawImageSubRect(fiDrawFace[ifiCount].iX, 
			fiDrawFace[ifiCount].iY, 
			fiDrawFace[ifiCount].iWidth, 
			fiDrawFace[ifiCount].iHeight, 
			&fiDrawFace[ifiCount].rc, 
			GetGame()->m_dwFaceTexture,
			bkBarColor | 0x00ffffff
			);
	}

	return S_OK;
}

// -------------------------------------------------------
// Name: GetStringLine()
// Describe: 獲得字符串的行數
// -------------------------------------------------------
long CGame::GetStringLine(char* pString, int nLength)
{
	int nLineNumber = 0;
	int nStringLength = lstrlen(pString);

	unsigned char* pBuffer;
	pBuffer = new unsigned char[nLength+1];

	unsigned char* pSrc = (unsigned char*)pString;
	unsigned char* pDest = pBuffer;

	bool bCheckSize = false;

	for(int i=0; i<(nStringLength / nLength + 1); i++)
	{
		memset(pBuffer, 0, nLength+1);			// clear temp buffer data
		for(int j=0; j<nLength; j++)
		{
			if(*pSrc == '\r' || *pSrc == '\n')	// return
			{
				pSrc++;
				i--;
				break;
			}
			else if(*pSrc == 0)					// end
			{
				if (pBuffer[0] == 0)
					nLineNumber--;
				break;
			}
			*pDest = *pSrc;

			if(*pSrc > 128)						// double size
			{
				if( bCheckSize )
					bCheckSize = false;
				else
					bCheckSize = true;
			}
			else
				bCheckSize = false;

			pDest++;
			pSrc++;
		}

		if( bCheckSize )
		{
			pDest--;
			*pDest = 0;
			pSrc--;
			bCheckSize = false;
		}

		nLineNumber++;
		pDest = pBuffer;
	}

	SAFE_DELETE( pBuffer );
	return nLineNumber;
}

void CGame::TextOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color, int iStyleIndex)
{
	if (pszString)
	{
		GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();

		// modify by lpf 2007-11-27 - 取掉了fScale和bScale参数的传入
		pGameGraphics2D->TextOut(iX, iY, pszString, iLen, color, iStyleIndex);
	}
}

/*
* 功能: 根据字体信息来输出文本
* 摘要: -
* 参数: iX		   - 绘制文本左上角X坐标
*		 iY		   - 绘制文本左上角Y坐标
*		 pszString - 绘制文本内容
*		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
* 返回值: -
* 作者: lpf
* 创建日期: 2007.11.28
* 修改日志:
*	2008.04.18 - lpf
*		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
*/
void CGame::DrawText(int iX, int iY, const char * pszString, PFONTINFO pFontInfo)
{
	GameGraphics2D * pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->DrawText(iX, iY, pszString, pFontInfo);
}



/*
* 功能: 根据字体信息来输出指定长度的文本
* 摘要: -
* 参数: iX		   - 绘制文本左上角X坐标
*		 iY		   - 绘制文本左上角Y坐标
*		 pszString - 绘制文本内容
*		 iLen	   - 绘制字符长度
*		 pFontInfo - 绘制的字体信息(如果为空则使用2D渲染层的默认字体信息进行绘制)
* 返回值: -
* 作者: lpf
* 创建日期: 2008.01.03
* 修改日志:
*	2008.04.18 - lpf
*		增加了一个_tagFontInfo *参数,用来区别是否使用2D渲染层默认字体信息进行绘制
*/
void CGame::DrawText(int iX, int iY, const char * pszString, int iLen, PFONTINFO pFontInfo)
{
	GameGraphics2D * pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->DrawText(iX, iY, pszString, iLen, pFontInfo);
}



//-----------------------------------------------------------------------------
// Name: GetFontInfo(PFONTINFO pFontInfo)			输出字体信息
// Desc: 得到2D渲染层当前字体渲染信息
//-----------------------------------------------------------------------------
void CGame::GetFontInfo(PFONTINFO pFontInfo)
{
	render::Interface		   * pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D	= pInterface->GetLayer2D();
	pLayer2D->GetCurrentFontInfo(pFontInfo);
}

//-----------------------------------------------------------------------------
// Name: SetFontInfo(const PFONTINFO pFontInfo)		传入字体信息
// Desc: 设定2D渲染层当前字体渲染信息
//-----------------------------------------------------------------------------
void CGame::SetFontInfo(const PFONTINFO pFontInfo)
{
	render::Interface		   * pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D	= pInterface->GetLayer2D();
	pLayer2D->SetCurrentFontInfo(pFontInfo);
}

long CGame::GetFontWidth(int iStyleIndex)
{
	GameGraphics2D *pGameGraphics = GameGraphics2D::GetInstance();
	return pGameGraphics->GetFontWidth(iStyleIndex);
}

long CGame::GetFontHeight(int iStyleIndex)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	return pLayer2D->GetFontSize(iStyleIndex);
}

void CGame::NumberOut(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale, bool bScale)
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->NumberOut(iX, iY, pszString, iLen, color, fScale);
}

void CGame::NumberOut1(int iX,int iY,const char *pszString,int iLen,D3DCOLOR color,float fScale, bool bScale)
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->NumberOut1(iX, iY, pszString, iLen, color, fScale);
}

long CGame::GetNumberWidth()
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	return pGameGraphics2D->GetNumberWidth();
}

long CGame::GetNumberHeight()
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	return pGameGraphics2D->GetNumberHeight();
}

int	 CGame::CalcTextWidth(const char *pszTextBuffer, int iLen)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	return pLayer2D->CalcTextWidth(pszTextBuffer, iLen);
}

void CGame::DrawLine(int iStartX,int iStartY,int iEndX,int iEndY,D3DCOLOR color, bool bScale)
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->DrawLine(iStartX, iStartY, iEndX, iEndY, color);
}

void CGame::DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->DrawSolidQuad(iX, iY, iWidth, iHeight, color);
}

void CGame::DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
	GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
	pGameGraphics2D->DrawSolidQuad(iX1, iY1, iX2 - iX1, iY2 - iY1, color);
}

void CGame::DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
}

void CGame::DrawToolTip(int nX,int nY,int nWidth,int nHeight,const char* pStr,D3DCOLOR textColor)
{
	GetGame()->DrawSolidQuad(nX,nY,nWidth,nHeight,D3DCOLOR_ARGB(150,0,0,0));
	GetGame()->TextOut(nX+5,nY+3,pStr,(INT)(strlen(pStr)),textColor);
}

// 刷新设置
void CGame::UpdateSetup()
{
	if (GetGame()->GetSetup()->lViewRange < 25)
		GetGame()->GetSetup()->lViewRange = 25;

	if (GetRegion())
		GetRegion()->GetCamera()->SetFarClipPlane((float)GetGame()->GetSetup()->lViewRange);
}

/*
* 功能: 抓取屏幕
* 摘要: -
* 参数: strFileName - 抓屏文件保存地址
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.12.18 - lpf
*		重新实现了抓屏,采用了更快速的方式,并且可以不用锁定后备缓冲,这样可以创建更高效的D3D设备
*/
void CGame::PrintScreen(const char * strFileName)
{
	// 完成Print Screen
	render::Interface * pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9   pDevice	   = pInterface->GetDevice();
	LPDIRECT3DSURFACE9	buffer	   = NULL;		//The surface buffer used to get the screenshot

	if (FAILED(pDevice->GetRenderTarget(0, &buffer)))
	{
		OutputConsole("ERROR FRONT BUFFER AQUASITION FAILED\n");
		buffer->Release();
		return;
	}

	// 保存后备缓冲表面图像信息到文件
	if (FAILED(D3DXSaveSurfaceToFile(strFileName,
		D3DXIFF_JPG,
		buffer,
		NULL,
		NULL)))
	{
		OutputConsole("ERROR SAVE SCREEN FAILED\n");
		buffer->Release();
		return;
	}

	buffer->Release();
}

//////////////////////////////////////////////////////////////////////////
//	猎手武器和特效的对应列表
//////////////////////////////////////////////////////////////////////////
void CGame::LoadArcheryEffect(const char* filename)
{
	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
		return;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);


	m_listArcheryEffect.clear();
	tagArcheryEffect st;
	while(ReadTo(stream, "#"))
	{
		stream >> st.dwWeaponGroundID >> st.dwArcheryEffect;
		m_listArcheryEffect.push_back(st);
	}
}

DWORD CGame::GetArcheryEffect(DWORD dwWeaponGroundID)
{
	for (list<tagArcheryEffect>::iterator it = m_listArcheryEffect.begin(); it!=m_listArcheryEffect.end(); it++)
	{
		if (it->dwWeaponGroundID == dwWeaponGroundID)
		{
			return it->dwArcheryEffect;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//	表情动画读取
//////////////////////////////////////////////////////////////////////////
void CGame::LoadFace(const char* filename)
{
	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
		return;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);


	m_listFace.clear();
	tagFace st;
	while(ReadTo(stream, "*"))
	{
		stream >> st.strFaceCode >> st.iStartIndex >> st.iEndIndex >> st.iCurCycle >> st.iCycle >> st.iCurIndex;
		m_listFace.push_back(st);
	}
}

void CGame::UpdateFace()
{
	//计算动画周期
	for(list<tagFace>::iterator it = m_listFace.begin(); it!=m_listFace.end(); it++)
	{
		it->iCurCycle++;
		if (it->iCurCycle > it->iCycle)
		{
			it->iCurCycle = 0;
			it->iCurIndex++;
			if (it->iCurIndex > it->iEndIndex)
			{
				it->iCurIndex = it->iStartIndex;
			}
		}
	}
}


bool	CGame::IsFileExist(const char*	strFileName)
{
	string strTempt(strFileName);
	CheckRFileStr(strTempt);
	bool bFlag = CClientResource::GetInstance()->IsFileExist(strTempt.c_str());
	if(bFlag==false)
	{
		bFlag = CheckFile(strFileName);
	}
	return bFlag;
}

void	CGame::FindFileList(const char* strFile, const char* strExt, list<string>& listFileName)
{
	string strTemptFolder(strFile);
	CheckRFileStr(strTemptFolder);
	string strTempt(strTemptFolder);
	int nPos = strTemptFolder.find_last_of("\\");
	strTempt.erase(0,nPos+1);
	if(strTempt=="*.*")
		strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);
	if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
	{
		string strTemptExt(strExt);
		CClientResource::GetInstance()->FindFileList(strTemptFolder,strTemptExt,listFileName);
	}
	else
	{
		FindFile(strFile,strExt,&listFileName);
	}
}

//////////////////////////////////////////////////////////////////////////
//	更新Shape的坐标
//////////////////////////////////////////////////////////////////////////
#ifdef MOVE_IN_TILE
void CGame::UpdateMoveShapePositionEx()
{
	if (GetRegion()==NULL)
		return;

	DWORD dwCurTime = GetCurTickCount();
	static DWORD	dwLastUpdateTime = GetCurTickCount();
	list<CBaseObject*> *pObjectList = GetRegion()->GetObjectList();
	for (itBaseObject it = pObjectList->begin(); it!=pObjectList->end(); it++)
	{
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(*it);
		if (pMoveShape)
		{
			// 处于平滑移位状态
			if (pMoveShape->GetIsSlipState())
			{
				// 计算移动的距离
				float fDistance = pMoveShape->GetSlipSpeed()*static_cast<float>(dwCurTime - pMoveShape->GetSlipLastTime());
				//计算出坐标偏移
				float fDir = pMoveShape->GetSlipDir();
				float fx = fDistance * cosf(fDir);
				float fy = fDistance * sinf(fDir);
				float fDistanceX = pMoveShape->GetDestX() - pMoveShape->GetPosX();
				float fDistanceY = pMoveShape->GetDestY() - pMoveShape->GetPosY();
				if( abs(fx) > abs(fDistanceX) )
					fx=fDistanceX;
				if( abs(fy) > abs(fDistanceY) )
					fy=fDistanceY;
				fx += pMoveShape->GetPosX();
				fy += pMoveShape->GetPosY();
				//调试				
				if(pMoveShape == m_pMainPlayer)
				{
					dwLastUpdateTime = dwCurTime;
				}
				pMoveShape->SetPosXY(fx, fy);
				pMoveShape->SetDestXY(pMoveShape->GetPosX(), pMoveShape->GetPosY());
				pMoveShape->SetSlipLastTime(dwCurTime);
			}
			// 正常的行走和跑步
			else if (pMoveShape->GetAction()==CShape::ACT_WALK || pMoveShape->GetAction()==CShape::ACT_RUN)
			{
				// 计算移动的距离
				float fDistance = pMoveShape->GetSpeed()*static_cast<float>(dwCurTime - pMoveShape->GetLastUpdatePosTime());
				//计算出坐标偏移
				float fDir = pMoveShape->GetDirEx();
				float fx = fDistance * cosf(fDir);
				float fy = fDistance * sinf(fDir);
				float fDistanceX = pMoveShape->GetDestX() - pMoveShape->GetPosX();
				float fDistanceY = pMoveShape->GetDestY() - pMoveShape->GetPosY();
				if( abs(fx) > abs(fDistanceX) )
					fx=fDistanceX;
				if( abs(fy) > abs(fDistanceY) )
					fy=fDistanceY;
				fx += pMoveShape->GetPosX();
				fy += pMoveShape->GetPosY();

				pMoveShape->SetPosXY(fx, fy);
				pMoveShape->SetLastUpdatePosTime(dwCurTime);
			}
		}
	}
}
#else
void CGame::UpdateMoveShapePositionEx()
{
	if (GetRegion()==NULL)
		return;

	DWORD dwCurTime = GetCurTickCount();
	static DWORD	dwLastUpdateTime = GetCurTickCount();
	list<CBaseObject*> *pObjectList = GetRegion()->GetObjectList();
	for (itBaseObject it = pObjectList->begin(); it!=pObjectList->end(); it++)
	{
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(*it);
		if (pMoveShape)
		{
			// 处于平滑移位状态
			if (pMoveShape->GetIsSlipState())
			{
				// 计算移动的距离
				float fDistance = pMoveShape->GetSlipSpeed()*static_cast<float>(dwCurTime - pMoveShape->GetSlipLastTime());
				//计算出坐标偏移
				float fDir = pMoveShape->GetSlipDir();
				float fx = pMoveShape->GetPosX() + fDistance * cosf(fDir);
				float fy = pMoveShape->GetPosY() + fDistance * sinf(fDir);
				//调试				
				if(pMoveShape == m_pMainPlayer)
				{
					dwLastUpdateTime = dwCurTime;
				}
				pMoveShape->SetPosXY(fx, fy);
				pMoveShape->SetDestXY(pMoveShape->GetPosX(), pMoveShape->GetPosY());
				pMoveShape->SetSlipLastTime(dwCurTime);
			}
			// 正常的行走和跑步
			else if (pMoveShape->GetAction()==CShape::ACT_WALK || pMoveShape->GetAction()==CShape::ACT_RUN)
			{
				// 计算移动的距离
				float fDistance = pMoveShape->GetSpeed()*static_cast<float>(dwCurTime - pMoveShape->GetLastUpdatePosTime());
				//计算出坐标偏移
				float fDir = pMoveShape->GetDirEx();
				float fx = pMoveShape->GetPosX() + fDistance * cosf(fDir);
				float fy = pMoveShape->GetPosY() + fDistance * sinf(fDir);

				pMoveShape->SetPosXY(fx, fy);
				pMoveShape->SetLastUpdatePosTime(dwCurTime);
			}
		}
	}
}
#endif



/*
* 功能: 更新动画,防止失去焦点后,所有对象动画会同步播放
* 摘要: 目前该方法仅仅在程序失去焦点调用
*		目前该方法仅仅更新了物品模型动画时间
* 参数: -
* 返回值: -
* 作者: lpf
* 创建日期: 2009.03.05
* 修改日志:
*			增加了对主角的动作做更新处理(liuke)
*/
void CGame::UpdateAnimation()
{
	if (GetRegion()==NULL)
		return;

	DWORD dwCurTime = timeGetTime();
	list<CBaseObject*> * pObjectList = GetRegion()->GetObjectList();
	for (itBaseObject it = pObjectList->begin(); it != pObjectList->end(); ++it)
	{
		CShape * pShape = dynamic_cast<CShape*>(*it);
		if (pShape)
		{
			if (pShape->GetType() == TYPE_GOODS || pShape == GetMainPlayer())
				pShape->UpdateAnimation();
		}
	}
}



//添加提示信息
void CGame::AddTopInfo(long lTopInfoID,long lFlag,long lParam,string strInfo)
{
	//查找什么已有这个提示信息
	if(lTopInfoID > 0)
	{
		list<stTopInfo>::iterator itInfo = m_TopInfos.begin();
		for(;itInfo != m_TopInfos.end();)
		{
			if( (*itInfo).lID == lTopInfoID )
			{
				itInfo = m_TopInfos.erase(itInfo);
			}
			else
			{
				itInfo++;
			}
		}
	}
	string strTemptStrInfo = strInfo;
	list<stTopStrInfo>	strInfos;strInfos.clear();

	int iPos1 = strTemptStrInfo.find("[[[");
	int iPos2 = strTemptStrInfo.find("]]]");
	while(iPos1 >= 0 && iPos2 > 0 && iPos2 > iPos1)
	{
		string str1 = strTemptStrInfo.substr(0,iPos1);
		stTopStrInfo TopStrInfo1 = {D3DCOLOR_ARGB(255,255,48,0),str1};
		strInfos.push_back(TopStrInfo1);

		string str2 = strTemptStrInfo.substr(iPos1+3,iPos2-iPos1-3);
		stTopStrInfo TopStrInfo2 = {D3DCOLOR_ARGB(255,0,255,255),str2};
		strInfos.push_back(TopStrInfo2);
		strTemptStrInfo = strTemptStrInfo.substr(iPos2+3,strTemptStrInfo.length() - iPos2-3);

		iPos1 = strTemptStrInfo.find("[[[");
		iPos2 = strTemptStrInfo.find("]]]");
	}

	if(strTemptStrInfo.length() > 0)
	{
		stTopStrInfo TopStrInfo = {D3DCOLOR_ARGB(255,255,48,0),strTemptStrInfo};
		strInfos.push_back(TopStrInfo);
	}
	DWORD dwCurTime = timeGetTime();
	stTopInfo TopInfo = {lTopInfoID,lFlag,lParam,dwCurTime,strInfos};
	m_TopInfos.push_back(TopInfo);

	if(m_bDisplayInfo == false)
	{
		m_dwDisplayTime = timeGetTime();
	}
}

//得到提示信息的内容
bool CGame::GetCurTopInfo(stCurTopInfo& TopInfo)
{
	if(m_bDisplayInfo == false)		return false;

	TopInfo = m_stCurTopInfo;
	return true;
}
//客户端顶部提示信息运行
void CGame::RunTopInfo()
{
	DWORD dwCurTime = timeGetTime();
	if(true == m_bDisplayInfo)
	{
		int lLen = m_stCurTopInfo.lTotalLength;
		int lTotalWidth = lLen * GetGame()->GetFontWidth(-1);
		m_stCurTopInfo.lX -= m_lDisplaySpeed;
		if(m_stCurTopInfo.lX + lTotalWidth <= 0)
		{
			m_bDisplayInfo = false;
			m_dwDisplayTime = dwCurTime + m_lDisplayIntel;
		}
	}
	else
	{
		if(dwCurTime >= m_dwDisplayTime && m_TopInfos.size() > 0)
		{
			m_bDisplayInfo = true;
			m_stCurTopInfo.listCurInfo.clear();
			m_stCurTopInfo.lTotalLength = 0;
			m_stCurTopInfo.lX = SCREEN_WIDTH;
			m_stCurTopInfo.lY = (20-GetGame()->GetFontHeight(-1))/2;

			list<stTopInfo>::iterator it = m_TopInfos.begin();
			for(;it != m_TopInfos.end();)
			{
				stTopInfo& TopInfo = (*it);
				if(TopInfo.lTimerFlag == 1)//次数
				{
					if(TopInfo.lParam > 0)
					{

						list<stTopStrInfo>::iterator itInfo = TopInfo.strInfo.begin();
						for(;itInfo != TopInfo.strInfo.end();itInfo++)
						{
							m_stCurTopInfo.listCurInfo.push_back((*itInfo));
							m_stCurTopInfo.lTotalLength += (*itInfo).strContend.length();
						}

						//加上分割符号
						stTopStrInfo strTemptInfo = {D3DCOLOR_ARGB(255,0,255,255),"     "};
						m_stCurTopInfo.listCurInfo.push_back(strTemptInfo);
						m_stCurTopInfo.lTotalLength += strTemptInfo.strContend.length();

						TopInfo.lParam--;
						it++;
					}
					else
					{
						it = m_TopInfos.erase(it);
					}
				}
				else if(TopInfo.lTimerFlag == 2)//时间
				{
					DWORD dwTime = dwCurTime-TopInfo.dwStartTime;
					if(TopInfo.lParam - 5000> dwTime)
					{
						list<stTopStrInfo>::iterator itInfo = TopInfo.strInfo.begin();
						for(;itInfo != TopInfo.strInfo.end();itInfo++)
						{
							m_stCurTopInfo.listCurInfo.push_back((*itInfo));
							m_stCurTopInfo.lTotalLength += (*itInfo).strContend.length();
						}

						long lDifTime = TopInfo.lParam - dwTime;
						lDifTime = lDifTime/1000;
						long lMinute = lDifTime/60;
						long lSecond = lDifTime%60;
						char strTempt[256]="";
						sprintf(strTempt,"%d",lMinute);

						//分数字
						stTopStrInfo strInfoMinNum = {D3DCOLOR_ARGB(255,0,255,255),strTempt};
						m_stCurTopInfo.listCurInfo.push_back(strInfoMinNum);
						m_stCurTopInfo.lTotalLength += strInfoMinNum.strContend.length();
						//"分"
						stTopStrInfo strInfoMin = {D3DCOLOR_ARGB(255,255,48,0),CStringReading::LoadString(300,109)};
						m_stCurTopInfo.listCurInfo.push_back(strInfoMin);
						m_stCurTopInfo.lTotalLength += strInfoMin.strContend.length();

						sprintf(strTempt,"%d",lSecond);
						//秒数字
						stTopStrInfo strInfoSecondNum = {D3DCOLOR_ARGB(255,0,255,255),strTempt};
						m_stCurTopInfo.listCurInfo.push_back(strInfoSecondNum);
						m_stCurTopInfo.lTotalLength += strInfoSecondNum.strContend.length();
						//"秒"
						stTopStrInfo strInfoSecond = {D3DCOLOR_ARGB(255,255,48,0),CStringReading::LoadString(300,110)};
						m_stCurTopInfo.listCurInfo.push_back(strInfoSecond);
						m_stCurTopInfo.lTotalLength += strInfoSecond.strContend.length();

						//加上分割符号
						stTopStrInfo strTemptInfo = {D3DCOLOR_ARGB(255,0,255,255),"     "};
						m_stCurTopInfo.listCurInfo.push_back(strTemptInfo);
						m_stCurTopInfo.lTotalLength += strTemptInfo.strContend.length();

						it++;
					}
					else
					{
						it = m_TopInfos.erase(it);
					}
				}
				else
				{
					it++;
				}
			}
		}
	}
}

//显示顶部信息
void CGame::DrawTopInfos()
{
	stCurTopInfo TemptTopInfo;
	if(GetCurTopInfo(TemptTopInfo) && TemptTopInfo.lTotalLength > 0)
	{
		GetGame()->DrawSolidQuadRect( 0, 0, SCREEN_WIDTH, 20, D3DCOLOR_ARGB(125,0,0,0) );

		list<stTopStrInfo>::iterator itInfo = TemptTopInfo.listCurInfo.begin();
		int iX = TemptTopInfo.lX;
		int iY = TemptTopInfo.lY;
		for(;itInfo != TemptTopInfo.listCurInfo.end();itInfo++)
		{
			int iDrawX = iX;
			stTopStrInfo& TemptStrInfo = (*itInfo);
			GetGame()->TextOut(iDrawX, iY, TemptStrInfo.strContend.c_str(), TemptStrInfo.strContend.length(),
				TemptStrInfo.d3dColor);
			iDrawX++;
			GetGame()->TextOut(iDrawX,iY, TemptStrInfo.strContend.c_str(),TemptStrInfo.strContend.length(),
				TemptStrInfo.d3dColor);

			iY = TemptTopInfo.lY;
			iX = iX+TemptStrInfo.strContend.length()*GetGame()->GetFontWidth(-1);
		}
	}
}



/*
* 功能: 设定游戏状态
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.09.05 - lpf
*		增加了进入登陆状态时,对之前的状态是否为人物选择状态进行了判断(如果是,则释放该页面)
*		主要用于修正当同样的帐号被别人登陆后,当自己处于人物选择状态,则会被弹到登陆状态,这时自己以一个空人物的帐号登陆,则会重复显示之前的人物模型信息
*/
void CGame::SetGameState(long t)
{
	long gameOldState = m_lGameState;
	m_lGameState = t;
	m_lQueuePos=-1;
	m_dwLastUpdateQueueTime = timeGetTime();
	m_dwCountDownTime = 0;
	g_bLogin = false;
	SetMouseCursor(MC_NORMAL);
	if(m_lGameState != gameOldState)
	{
		g_LoginTime = 0;
		//g_pGuangMGUIApp->CloseAll();
		switch(m_lGameState)
		{
		case GS_LOGIN:		// 游戲登陸狀態
			{
				m_pGuiEx->CloseAllPage(true,m_pGuiEx->GetLoginPageEx());
				if (gameOldState == GS_SELECT)
					m_pGuiEx->GetSelectRolePageEx()->Release();
				m_pGuiEx->GetLoginPageEx()->Open();
			}
			break;
		case GS_PWD_CARD:
			m_pGuiEx->CloseAllPage();
			m_pGuiEx->GetLoginPageEx()->Close();
			m_pGuiEx->GetMatrixCardPageEx()->Open();
			break;
		case GS_SELECTWS:     //选择WS
			{
				m_pGuiEx->GetSelectWsPageEx()->Open();
			}
			break;
		case GS_SELECT:		// 游戲選擇角色狀態
			{
				m_pGuiEx->GetSelectRolePageEx()->Open();	
			}
			break;
		case GS_CREATE:		// 游戲建立角色狀態
			{
				m_pGuiEx->GetCreateRolePageEx()->Open();
			}
			break;
		case GS_WAITING:		// 等待进入状态
			break;
		case GS_BLACK:		// 黑屏状态
			break;
		case GS_RUN:			// 游戲運行
			{
				//打开游戏主界面（快捷键之类的）
				m_pGuiEx->GetMainBarPageEx()->Open();
				//打开游戏的高级窗口（装备栏之类的）
				//g_pGuangMGUIApp->Open(ePane::PANE_CONTENT,"");
			}
			break;
		case GS_STAFF:		// 制作人员
			break;
		}
	}
}

void CGame::SetQueuePos(long lNum)
{
	if(m_lQueuePos == -1)
	{
		m_dwLastUpdateQueueTime = timeGetTime()-1;
		if(lNum>=1)
		{
			m_dwCountDownTime = lNum*2000; 
		}else{
			m_dwCountDownTime = 2000;
		}
	}

	DWORD dwCurTime = timeGetTime();
	DWORD dwInterTime = dwCurTime - m_dwLastUpdateQueueTime;
	long lInterNum  = m_lQueuePos-lNum;

	if(lInterNum > 0)
	{
		m_dwCountDownTime = lNum*(dwInterTime/lInterNum);
	}
	else
	{
		if( m_dwCountDownTime > 4000&&m_lQueuePos!=-1) 
		{
			m_dwCountDownTime -= 4000;
		}
	}
	if(m_lQueuePos != lNum)
		m_dwLastUpdateQueueTime= dwCurTime;

	m_lQueuePos = lNum;
}

// 压入界面的消息处理
bool CGame::PushWinMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message != WM_MOUSEMOVE &&
		message != WM_LBUTTONDOWN  &&
		message != WM_LBUTTONUP  &&
		message != WM_LBUTTONDBLCLK  &&
		message != WM_RBUTTONDOWN &&
		message != WM_RBUTTONUP &&
		message != WM_RBUTTONDBLCLK	&&
		message != WM_MOUSEWHEEL &&
		message != WM_KEYDOWN &&
		message != WM_KEYUP &&
		message != WM_CHAR )
	{
		return false;
	}
	if(m_quMessage.size() > 0)
	{
		if(WM_MOUSEMOVE == message && message == m_quMessage.back().message)
		{
			m_quMessage.pop_back();
		}
	}
	tagWinMessageParam tagMessage={message,wParam,lParam};
	m_quMessage.push_back(tagMessage);
	return true;
}

// 弹出界面的消息处理
bool CGame::PopWinMessage()
{
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::PopWinMessage() Start.");
#endif
	if(m_quMessage.empty())
		return false;
	while(!m_quMessage.empty())
	{
		tagWinMessageParam tagMessage=m_quMessage.front();

		if( GetMouse() && tagMessage.message != WM_MOUSEMOVE )
		{
			GetMouse()->MouseMessageProc(tagMessage.message, g_pointMousePos, tagMessage.wParam);
		}
		if(g_pGuangMGUIRoot)
		{
			extern BYTE g_cPressedKey;
			bool bFlag = g_pGuangMGUIRoot->GetGUIComp()->IsDrawAllPane();
			// 游戏中是否显示所有界面
			if(GetGameState()==GS_RUN)
			{
				// 隐藏界面
				if(GetKeyboard()->GetCurrentKey()==DIK_Z && GetKeyboard()->IsKeyDown(DIK_LCONTROL))
				{
					if(bFlag)
						g_pGuangMGUIRoot->GetGUIComp()->SetIsDrawAllPane(false);
					else
						g_pGuangMGUIRoot->GetGUIComp()->SetIsDrawAllPane(true);
					GetKeyboard()->SetCurrentKey(0);
					g_cPressedKey = 0;
					if(!m_quMessage.empty())
						m_quMessage.pop_front();
					return true;
				}
				// 判断手上是否有物品或技能，如果有，点击鼠标右键清除，返回
				if(GetMouse()->GetCurrentButton() == RB_DOWN || tagMessage.wParam == 27)
				{
					CMainPlayerHand *pHand = m_pGuiEx->GetPMainPlayerHand();
					if(pHand && (pHand->GetPGoodsOfMainPlayerHand() || pHand->GetGoodsIndexOfMainPlayerHand() || pHand->GetSkillOfMainPlayerHand()))
					{
						if(pHand->GetPGoodsOfMainPlayerHand())
						{
							pHand->GetPGoodsOfMainPlayerHand()->SetHaveShadowState(false);
							pHand->DelShadowStateOfGoods();
						}
						pHand->ClearMainPlayerHand();
						GetKeyboard()->SetCurrentKey(0);
						if(!m_quMessage.empty())
							m_quMessage.pop_front();
						return true;
					}
				}
			}
			bFlag = g_pGuangMGUIRoot->GetGUIComp()->IsDrawAllPane();
			if(bFlag && g_pGuangMGUIRoot->ProcessMessage(tagMessage.message,tagMessage.wParam,tagMessage.lParam))
			{
				// 有消息框打开
				if(m_pGuiEx->GetMessageBoxPageEx() && m_pGuiEx->GetMessageBoxPageEx()->IsOpen())
				{
					CGuiExPage * pPage = m_pGuiEx->GetMessageBoxPageEx()->GetBegetPage();
					if(pPage)
					{
						int iClick = -1;
						if(g_cPressedKey==27)					// ESC
						{
							iClick = CGuiExPage::CLICK_NO;
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav"); 
						}
						else if( g_cPressedKey==DIK_EQUALS )	// 回车
						{
							iClick = CGuiExPage::CLICK_AGREE;
							GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav"); 
						}
						if(iClick != -1)
						{
							CMessageBoxPageEx *pMsgBox = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();
							if(!pPage || CMessageBoxPageEx::MSG_OK == pMsgBox->GetMsgType() )
							{
								pMsgBox->m_lCallerID = -2;

								if( pPage )
								{
									pPage->ClickBtnByMsgBox(CGuiExPage::CLICK_CANCEL);
									g_cPressedKey = 0;
								}
								else
								{
									pMsgBox->ClickBtnByMsgBox(CGuiExPage::CLICK_NO);
								}

								pMsgBox->Open();
								GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
							}
							else
							{
								pPage->ClickBtnByMsgBox(iClick);
								pMsgBox->Open();
								g_cPressedKey = 0;
							}
						}
					}
				}
				if(GetGameState()==GS_RUN)
				{
					m_pGuiEx->GetHotKeySetupPageEx()->GetInputKey();
					m_pGuiEx->GetMainBarPageEx()->Run();
					// 按下消息,帮助聚焦框的点击
					if(tagMessage.message == WM_LBUTTONDOWN)
						m_pGuiEx->GetTopPage()->ClickRectFocus();
					if (GetMouse()->GetCurrentButton() == LB_DOWN)
						GetGameControl()->SetMoveAble(true);
					else
						GetGameControl()->SetMoveAble(false);
				}
				if(GetGameState()==GS_SELECTWS)
				{
					if(g_cPressedKey==27)	
					{
						GetGame()->GetCGuiEx()->GetSelectWsPageEx()->ReLoadWs();
						g_cPressedKey=0;
					}
				}
			}
		}
		if(!m_quMessage.empty())
			m_quMessage.pop_front();
	}
#ifdef _RUNSTACKINFO_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::PopWinMessage() End.");
#endif
	return true;
}



// 根据传入的ID，在聊天信息中查找对应的物品信息（该GUID并非是物品的GUID，而是聊天信息中该物品对应的索引ID）
CGoods * CGame::FindGoodsInfo(const CGUID &guid)
{
	list<CGame::stChatGoodsInfo>::iterator it = m_lsChatGoodsInfo.begin();
	for (; it != m_lsChatGoodsInfo.end(); ++it)
	{
		if(guid == (*it).idEx)
			return (*it).pGoods;
	}
	return NULL;
}

// 读取NPC任务特效优先级
void CGame::LoadMonsterQuestEffect(const char *filename)
{
	if(filename == NULL)
		return;
	CRFile* prfile = rfOpen(filename);
	if(prfile==NULL)
		return;
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);
	m_vecMonsterQuestEffect.clear();
	long YellowQuestionMark,
		GreenQuestionMark,
		BlueQuestionMark,
		YellowExcalmatoryMark,
		GreenExcalmatoryMark,
		BlueExcalmatoryMark,
		GrayQuestionMark,
		GrayExcalmatoryMark;
	if(ReadTo(stream, "*"))
	{
		stream >> YellowQuestionMark >> GreenQuestionMark >> BlueQuestionMark >> YellowExcalmatoryMark >> 
			GreenExcalmatoryMark >> BlueExcalmatoryMark >> GrayQuestionMark >> GrayExcalmatoryMark;
		m_vecMonsterQuestEffect.push_back(YellowQuestionMark);
		m_vecMonsterQuestEffect.push_back(GreenQuestionMark);
		m_vecMonsterQuestEffect.push_back(BlueQuestionMark);
		m_vecMonsterQuestEffect.push_back(YellowExcalmatoryMark);
		m_vecMonsterQuestEffect.push_back(GreenExcalmatoryMark);
		m_vecMonsterQuestEffect.push_back(BlueExcalmatoryMark);
		m_vecMonsterQuestEffect.push_back(GrayQuestionMark);
		m_vecMonsterQuestEffect.push_back(GrayExcalmatoryMark);
	}
}

// 根据传入的优先级来得到对应的特效ID
long CGame::GetMonsterQuestEffectID(long lIndex)
{
	if(lIndex < m_vecMonsterQuestEffect.size())
		return m_vecMonsterQuestEffect[lIndex];
	return -1;
}

// 根据传入的特效ID来得到对应的优先级
long CGame::GetMonsterQuestEffectLevel(long lEffectID)
{
	for (int i=0; i<m_vecMonsterQuestEffect.size(); ++i)
	{
		if(m_vecMonsterQuestEffect[i] == lEffectID)
			return i;
	}
	return -1;
}



/*
* 功能: 重新设置键盘设备
* 摘要: -
* 参数: -
* 返回值: -
* 作者: -
* 创建日期: -
* 修改日志:
*	2008.10.21 - lpf
*		注释了对键盘设备对象的删除与创建,替换为仅仅清空数据,以修正重新创建设备时,可能出现的键盘失效情况;
*	2009.01.21 - lpf
*		在该函数中增加了下一次更新无效的设定,让下一次从设备取出的数据不会保存到键盘对象中;
*/
bool CGame::ReSetKeyboard()
{
	extern HINSTANCE hInst;
	m_pKeyboard->SetNextQueryInvalid();
	m_pKeyboard->ClearUpData();
	return true;
}

//写入注册表(strPath为注册表文件名，KeyValue为键值)
bool CGame::WriteReg(string strPath,string KeyValue)
{

	BYTE cValues[] = { 0x11,0x12,0x55 };

	//BYTE Vals[100];
	DWORD lenIt = 100;
	HKEY hk;

	//创建注册表项
	if(!::RegCreateKeyA(HKEY_LOCAL_MACHINE,"SOFTWARE\\xingman\\Luvinia\\",&hk)==ERROR_SUCCESS) 
	{
		return false;
	}

	//设置注册表键值
	if(!::RegSetValueExA(hk,strPath.c_str(),0,REG_SZ,(LPBYTE)(LPCSTR)KeyValue.c_str(),KeyValue.size()+1)==ERROR_SUCCESS)
	{
		return false;
	}

	::RegCloseKey(hk);
	return true;
}

//盛大通行证登陆ls的消息发送
void CGame::ConnectLoginServer(const char* const logIP,long logPort,const char* SonId,string wsName)
{
	ReNew();
	long hr = 0;
	if(m_pNetClient)
		m_pNetClient->Exit();
	SAFE_DELETE( m_pNetClient );

	if (wsName[0]!=0)
	{
		SetGameState(GS_BLACK);
		Sleep(1000);
	}
	CLoginPageEx * pLoginPage = GetGame()->GetCGuiEx()->GetLoginPageEx();
	CMessageBoxPageEx * pMsgBox = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();

	m_pNetClient = new CClient;
	m_pNetClient->Start(m_pDBAllocator,20,NF_LS_Client);

	hr = m_pNetClient->Connect( logIP,logPort);	// 连接服务器
	if( !hr &&pLoginPage!=NULL)	// 连接失败
	{
		pMsgBox->Open(pLoginPage,CStringReading::LoadString(eIDS_PUBLIC,eIDS_LoginError),CMessageBoxPageEx::MSG_OK,true);
		g_LoginTime=0;
		SDOLogOut();
		ShowLoginDialog();
	}else{
		if(m_pNetClient!=NULL&&pLoginPage!=NULL)
		{
			CMessage::SetClient(m_pNetClient);
			extern long g_lVersion;
			char strVersion[RESOURCE_VERSION_SIZE]="";
			sprintf(strVersion,"%s",CClientResource::GetInstance()->GetVersion().c_str());
			char strwsName[WS_NAME_SIZE]="";
			sprintf(strwsName,"%s",wsName.c_str());
			char strSonId[PT_TOKEN_SIZE]="";
			sprintf(strSonId,"%s",SonId);

			CMessage msg( MSG_C2L_LOG_SEND_PT_TOKEN );	
			msg.Add( g_lVersion );	// 游戏版本
			msg.AddEx((void*)&strVersion,RESOURCE_VERSION_SIZE);
			msg.AddEx((void*)&strwsName,WS_NAME_SIZE);
			msg.AddEx((void*)&strSonId,PT_TOKEN_SIZE);
			msg.Send(m_pNetClient);
		}
	}
}

/* 释放IGW */
void CGame::CloseIGW()
{
	if (g_SDOADx9)
	{
		g_SDOADx9->Release();
		g_SDOADx9 = NULL;
	}

	if (g_SDOAApp)
	{
		g_SDOAApp->Release();
		g_SDOAApp = NULL;
	}

	if (g_SDOAAppUtils)
	{
		g_SDOAAppUtils->Release();
		g_SDOAAppUtils = NULL;
	}

	if (g_SDOAClientService)
	{
		g_SDOAClientService->Release();
		g_SDOAClientService = NULL;
	}

	if (igwTerminal)
	{  // 释放DLL前总是先调用igwTerminal
		igwTerminal();
		igwTerminal = NULL;
	}
	if (g_hInstIGWDll)
	{
		FreeLibrary(g_hInstIGWDll);
		g_hInstIGWDll = 0;
	}
	igwInitialize = NULL;
}

/* 加载IGW */
void CGame::OpenIGW()
{
	WCHAR strExePath[MAX_PATH] = {0};
	WCHAR strExeName[MAX_PATH] = {0};
	WCHAR* strLastSlash = NULL;
	GetModuleFileNameW( NULL, strExePath, MAX_PATH );
	strExePath[MAX_PATH-1]=0;
	strLastSlash = wcsrchr( strExePath, TEXT('\\') );
	if( strLastSlash )
	{	// 得到EXE所在路径
		StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
		*strLastSlash = 0;
		strLastSlash = wcsrchr( strExeName, TEXT('.') );
		if( strLastSlash )
			*strLastSlash = 0;
	}

	WCHAR strGameWidgetDll[MAX_PATH] = {0};
	StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\GameWidget.dll", strExePath );

	if ( !(GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF) )
	{ // 如果程序运行路径中不存在则换个路径再试
		StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\..\\..\\..\\..\\bin\\GameWidget.dll", strExePath );
	}

	CloseIGW();
	int iAreaId = GetAreaId();
	int iGroupId = GetGroupId();

	if ( GetFileAttributesW(strGameWidgetDll) != 0xFFFFFFFF )
	{	// 找到组件则加载	
		g_hInstIGWDll = LoadLibraryW(strGameWidgetDll);
		if (g_hInstIGWDll)
		{
			igwInitialize = (LPigwInitialize)GetProcAddress( g_hInstIGWDll, "igwInitialize" );
			igwGetModule = (LPigwGetModule)GetProcAddress( g_hInstIGWDll, "igwGetModule" );
			igwTerminal = (LPigwTerminal)GetProcAddress( g_hInstIGWDll, "igwTerminal" );
			if ( igwInitialize && igwGetModule && igwTerminal)
			{
				// 加载成功
				char *GameName=CStringReading::LoadString(eIDS_PUBLIC,eIDS_GameName);//CStringReading::LoadString(eIDS_BASEUI,402)
				WCHAR Wchar[MAX_PATH] = {0};;
				int nLen = strlen(GameName)+1;
		        int nwLen = MultiByteToWideChar(CP_ACP, 0, GameName, nLen, NULL, 0);
				MultiByteToWideChar(CP_ACP, 0, GameName, nLen, Wchar, nwLen);
				// 注意AppInfo类型所有字段必须填
				AppInfo tmpAppInfo = {
					sizeof(AppInfo),       // 结构体大小，方便扩展
					101,                  // 接入应用ID，从开发者网站中申请
					Wchar,                // 应用名称
					L"0.1.2.0",            // 应用客户端当前版本号
					SDOA_RENDERTYPE_D3D9,  // 客户端支持的图形引擎类型，这里同时支持d3d9和d3d8
					1,                     // 可同时在一台机器上游戏的最大人数（通常为1，例如:KOF应该是2）
					iAreaId,                    // 游戏区ID，不可用时传入-1
					iGroupId                     // 游戏组ID，不可用时传入-1

				};

				if (igwInitialize(SDOA_SDK_VERSION, &tmpAppInfo) == SDOA_OK)
				{	// 初始化组件成功

					igwGetModule(__uuidof(ISDOADx9), (void**)&g_SDOADx9);
					igwGetModule(__uuidof(ISDOAApp), (void**)&g_SDOAApp);
					igwGetModule(__uuidof(ISDOAAppUtils), (void**)&g_SDOAAppUtils);
					igwGetModule(__uuidof(ISDOAClientService), (void**)&g_SDOAClientService);	

					if ( (!g_SDOADx9) || (!g_SDOAApp) || (!g_SDOAAppUtils) || (!g_SDOAClientService))
						CloseIGW();

					if (igwInitialize)
					{	// 说明加载成功，表示下......
						//HWND hMainWin = DXUTGetHWND();
						//SetWindowTextW(g_hWnd, L"星尘传说(测试版本) [加载IGW组件成功]");
						char Text[50];
						sprintf(Text,"Init IGW OK!\n");
						PutDebugString(Text);
					}	
					render::Interface *pInterface = render::Interface::GetInstance();
					if (g_SDOADx9)
					{	// 初始化IGW内部图形引擎
						D3DPRESENT_PARAMETERS d3dpp = pInterface->GetInfo()->d3dpp;
						g_SDOADx9->Initialize( pInterface->GetDevice(), &d3dpp, FALSE );
					}
					if (g_SDOAAppUtils)
					{  // 设置音量控制回调函数
						g_SDOAAppUtils->SetAudioSetting(TestGetAudioSoundVolume, TestSetAudioSoundVolume, TestGetAudioEffectVolume, TestSetAudioEffectVolume);
					}

				}
				else // 初始化失败清除环境
					CloseIGW();
			}
			else  // 加载DLL失败清除环境
				CloseIGW();
		}
	}
}

//IGW相关WINDOOWS消息回调
LRESULT CGame::OnWindowProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(g_SDOADx9) 
	{	// 把消息交给IGW模块处理 
		LRESULT lResult; 
		if(GetGameState()==GS_LOGIN||GetGameState()==GS_RUN)   //只有登陆和游戏运行阶段才响应IGW操作
		{
			if (g_SDOADx9->OnWindowProc( hDlg, message, wParam, lParam, &lResult ) == SDOA_OK ) 
			{ 
				return SDOA_OK; 
			} 
		}


		if ( (UINT)SDOA_WM_CLIENT_RUN == message )
		{
			// 此刻可以开始登录
			if(m_bIsSadoLog)
			{
				GetGame()->ShowLoginDialog();
			}

			return SDOA_OK; 
		}
	} 
	return SDOA_FALSE;
}


//盛大通行证登陆的消息回馈
BOOL CALLBACK OnLogin(int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved) 
{ // 注意该回调返回true则关闭登录对话框，否则登录对话框仍然显示

	g_isSDOLogin = (SDOA_ERRORCODE_OK == nErrorCode); 
	if (g_isSDOLogin) 
	{ 
		g_loginResult = *pLoginResult;
		// 登录成功后,把用户使用的游戏角色信息传入IGW
		if (g_SDOAApp)
		{
			const LPCSTR LszSonId = pLoginResult->szSessionId;
			const char* SonId = LszSonId;

			PutDebugString("Login IGW OK!");

			GetGame()->ConnectLoginServer(GetGame()->GetloginServerIP(),GetGame()->GetLoginServerPort(),SonId);
			GetGame()->SetSonID(SonId);
			GetGame()->SetSndaid(pLoginResult->szSndaid);
			GetGame()->SetIsShowSdo(false);
			g_LoginTime = timeGetTime();
		}
	} 

	if (SDOA_ERRORCODE_SHOWMESSAGE == nErrorCode)
	{ // 登录错误消息处理
		if (nReserved != 0)
		{
			LoginMessage* pCurLoginMsg = (LoginMessage*)(nReserved);
			if (pCurLoginMsg->dwSize >= sizeof(LoginMessage))
			{ // 兼容性处理 
				// 修改错误提示对话框标题
				char *Str=CStringReading::LoadString(eIDS_PUBLIC,eIDS_SDOALogin);//CStringReading::LoadString(eIDS_BASEUI,402)
				WCHAR Wchar[MAX_PATH] = {0};;
				int nLen = strlen(Str)+1;
		        int nwLen = MultiByteToWideChar(CP_ACP, 0, Str, nLen, NULL, 0);
				MultiByteToWideChar(CP_ACP, 0, Str, nLen, Wchar, nwLen);
				
				SysReAllocString(pCurLoginMsg->pbstrTitle,Wchar);

				// 此时返回FALSE则使用自定义提示
			}
		}
	}else if(SDOA_ERRORCODE_CANCEL == nErrorCode)
	{
		GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(true);
		GetGame()->GetCGuiEx()->GetLoginPageEx()->SetSimoLoginEnble(true);
		GetGame()->GetCGuiEx()->GetLoginPageEx()->SetPageFocus(); 
		GetGame()->SetSadoLogPageOpen(false);
	}
	char Text[50];
	sprintf(Text,CStringReading::LoadString(eIDS_PUBLIC,eIDS_SDOALoginError),nErrorCode);
	PutDebugString(Text);
	g_bLogin=g_isSDOLogin;
	return g_isSDOLogin; // 常规逻辑，登录成功则关闭登录对话框
} 

//查询余额操作的回调函数
void CALLBACK QueryCallback(int nRetCode, int nUserData)
{ // 异步查询的回调
	if (g_SDOAClientService)
	{
		if (nRetCode != SDOA_OK)
		{
			BSTR bstrRetKey = SysAllocString(L"descMsg");
			BSTR bstrRetValue = NULL;
			g_SDOAClientService->GetValue(bstrRetKey, &bstrRetValue);
			_bstr_t strRet = bstrRetValue;
			char *szRet = strRet;

			char *Str=CStringReading::LoadString(eIDS_PUBLIC,eIDS_ErrorInfo);
			WCHAR Wchar[MAX_PATH] = {0};;
			int nLen = strlen(Str)+1;
	        int nwLen = MultiByteToWideChar(CP_ACP, 0, Str, nLen, NULL, 0);
			MultiByteToWideChar(CP_ACP, 0, Str, nLen, Wchar, nwLen);
			WCHAR strErrotTitle[100] = {0};
			StringCchPrintfW( strErrotTitle, 100, Wchar, nRetCode); 

			SysFreeString(bstrRetKey);
			SysFreeString(bstrRetValue);
		}
		else
		{
			BSTR bstrKey = SysAllocString(L"BalanceInfo");
			BSTR bstrValue = NULL;
			g_SDOAClientService->GetValue(bstrKey, &bstrValue);
			_bstr_t b = bstrValue;
			char *t = b;

			CPlayer * pPlayer = GetGame()->GetMainPlayer();
			if(pPlayer)
			{
				string strPay = t;
				int nPos=-1;
				nPos=strPay.find_first_of(":")+1;
				string strPayEx=strPay.substr(nPos,strPay.size()-nPos);
				DWORD dwYuanbo= (DWORD)atoi(strPayEx.c_str());
				pPlayer->SetYuanBao(dwYuanbo);
				if(GetGame()->GetCGuiEx()->GetTreasureShopPageEx()->IsOpen())
				{
					GetGame()->GetCGuiEx()->GetTreasureShopPageEx()->UpdataDianQYuanB();
				}
			}

			SysFreeString(bstrKey);
			SysFreeString(bstrValue);			
		}
	}
}

//显示圈圈登陆对话框
void CGame::ShowLoginDialog()
{

	// 此刻可以开始登录
	if (g_SDOAApp)
	{				
		g_SDOAApp->ShowLoginDialog(OnLogin, 0, 0);
		SDOAWinProperty tmpWinP;
		if (g_SDOAApp->GetWinProperty(L"igwUserLoginDialog", &tmpWinP) == SDOA_OK)
		{   // 说明登录窗口igwUserLoginDialog存在

			long lPosX = 0;
			long lPosY = 0;
			GetGame()->GetCGuiEx()->GetLoginPageEx()->GetbtnSdoPos(lPosX,lPosY);
			// 改变窗口的位置
			tmpWinP.nLeft = lPosX-(SCREEN_WIDTH*9)/1024;//(SCREEN_WIDTH*363)/1024;//363;
			tmpWinP.nTop = lPosY-(SCREEN_HEIGHT*50/768);//(SCREEN_HEIGHT*306)/768;//306;

			if (g_SDOAApp->SetWinProperty(L"igwUserLoginDialog", &tmpWinP) == SDOA_OK)
			{
				// 设置成功
				SetSadoLogPageOpen(true);
			}
			else
			{
				// 设置失败
			}
			IGUIRoot::GetGUIInstance()->SetFocusComp(NULL);

			m_bIsShowSdo = true;

			GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(false);
			GetGame()->GetCGuiEx()->GetLoginPageEx()->SetSimoLoginEnble(false);
			GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
		}
	}
}

//查询余额的操作
void CGame::IGWBalance()
{
	// 必须接口存在且登录成功后才可调用该接口，例如：查询余额
	if (g_isSDOLogin)
	{
		if (g_SDOAClientService)
		{
			// 调用前设置参数

			// 订单编号 (请根据实际单号填写，这里仅是样例代码)
			BSTR bstrOrderIdName = SysAllocString(L"orderId");
			BSTR bstrOrderIdValue = SysAllocString(L"01234567890123456789012345678912");
			if (g_SDOAClientService->SetValue(bstrOrderIdName, bstrOrderIdValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue orderId Error! \n");
			}
			SysFreeString(bstrOrderIdValue);
			SysFreeString(bstrOrderIdName);

			// 商户编码 (根据实际情况填写)
			BSTR bstrSpIdName = SysAllocString(L"spId");
			BSTR bstrSpIdValue = SysAllocString(L"101");
			if (g_SDOAClientService->SetValue(bstrSpIdName, bstrSpIdValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue spId Error! \n");
			}
			SysFreeString(bstrSpIdName);
			SysFreeString(bstrSpIdValue);


			// 货币类型
			BSTR bstrPayTypeName = SysAllocString(L"payType");
			BSTR bstrPayTypeValue = SysAllocString(L"1");
			if (g_SDOAClientService->SetValue(bstrPayTypeName, bstrPayTypeValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue payType Error! \n");
			}
			SysFreeString(bstrPayTypeName);
			SysFreeString(bstrPayTypeValue);
			// 命令类型
			BSTR bstrCmdTypeName = SysAllocString(L"cmdType");
			BSTR bstrCmdTypeValue = SysAllocString(L"1");
			if (g_SDOAClientService->SetValue(bstrCmdTypeName, bstrCmdTypeValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue cmdType Error! \n");
			}
			SysFreeString(bstrCmdTypeName);
			SysFreeString(bstrCmdTypeValue);
			// 调用时间
			BSTR bstrCallTimeName = SysAllocString(L"callTime");
			char str[100]="";
			WCHAR strTime[50] = {0};
			SYSTEMTIME sys;
			GetLocalTime( &sys );
			sprintf( str,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond );
			StringCchPrintfW( strTime, 50, L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond );
			PutDebugString(str);
			BSTR bstrCallTimeValue = SysAllocString(strTime);
			if (g_SDOAClientService->SetValue(bstrCallTimeName, bstrCallTimeValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue callTime Error! \n");
			}
			SysFreeString(bstrCallTimeName);
			SysFreeString(bstrCallTimeValue);
			// 附加指令
			BSTR bstrIndicationName = SysAllocString(L"indication");
			BSTR bstrIndicationValue = SysAllocString(L"");
			if (g_SDOAClientService->SetValue(bstrIndicationName, bstrIndicationValue) == SDOA_FALSE)
			{
				PutDebugString("SetValue indication Error! \n");
			}
			SysFreeString(bstrIndicationName);
			SysFreeString(bstrIndicationValue);


			BSTR bstrServiceName = SysAllocString(L"UserBalance");
			if ( g_SDOAClientService->AsyncQuery(bstrServiceName, QueryCallback, 0) == SDOA_OK )
			{
			}else{
			}

			SysFreeString(bstrServiceName);

		}
	}
	else
	{
		GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,CStringReading::LoadString(eIDS_PUBLIC,eIDS_SDOAInfoTOLogin)); 
	}
}

/* 通知IGW d3d设备丢失 */
void CGame::IGWOnLostDevice()
{
	/* 通知IGW d3d设备丢失 */
	if(g_SDOADx9)
		g_SDOADx9->OnDeviceLost();

	if (g_SDOADx9)
		g_SDOADx9->Finalize();
}
//通知IGW d3d设备重置
void CGame::IGWOnResetDevice()
{
	if(g_SDOADx9)
	{	// 通知IGW d3d设备重置成功
		render::Interface *pInterface = render::Interface::GetInstance(); 	
		D3DPRESENT_PARAMETERS d3dpp = pInterface->GetInfo()->d3dpp;
		g_SDOADx9->OnDeviceReset ( &d3dpp ) ;

		if (g_SDOADx9)
		{	// 初始化IGW内部图形引擎
			D3DPRESENT_PARAMETERS d3dpp = pInterface->GetInfo()->d3dpp;
			g_SDOADx9->Initialize( pInterface->GetDevice(), &d3dpp, FALSE );
		}
	}
}
//声音测试改变
void CGame::NodifyAudioChanged()
{
	if (g_SDOAAppUtils)
	{
		g_SDOAAppUtils->NodifyAudioChanged();
	}
}
//设置玩家名字
void CGame::SetPlayerInfo(const char* Name,int iSex)
{
	if(g_isSDOLogin&&g_SDOAApp!=NULL)
	{
		WCHAR strPlayerInfo[MAX_PATH] = {0};
		const char *szName = Name;
		int nLen = strlen(szName)+1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, szName, nLen, NULL, 0);
		if(iSex==0)
		{
			iSex=1;
		}else if(iSex==1)
		{
			iSex=0;
		}

		MultiByteToWideChar(CP_ACP, 0, szName, nLen, strPlayerInfo, nwLen);
		RoleInfo tmpRoleInfo = { sizeof(RoleInfo), strPlayerInfo, iSex } ;
		g_SDOAApp->SetRoleInfo(&tmpRoleInfo);  // 设置角色信息，可多次调用，只要用户切换了角色就调用通知IGW
		IGWBalance();
	}
}
//打开消费操作的回馈
void CGame::OpenUrl(const char* UrlId)
{
	if (g_SDOAApp)
	{    
		WCHAR strUrlTitle[2048] = {0};
		const char *UrId = UrlId;
		int nLen = strlen(UrId)+1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, UrId, nLen, NULL, 0);

		MultiByteToWideChar(CP_ACP, 0, UrId, nLen, strUrlTitle, nwLen);
		if (g_SDOAApp->ShowPaymentDialog(strUrlTitle) == SDOA_OK)
		{
			//打开页面成功
			char Text[2048];
			sprintf(Text,CStringReading::LoadString(eIDS_PUBLIC,eIDS_OpenUrl),UrlId);
			PutDebugString(Text);
		}
	}
}
//igw的登出
void CGame::SDOLogOut()
{
	if (g_isSDOLogin && g_SDOAApp)   //如果登陆盛大通行证，则推出
	{
		// 登录成功，退出时要登出
		g_SDOAApp->Logout();
	}
}
//设置IGW焦点
void CGame::SetSDOFocus()
{
	if(g_SDOAApp)
	{
		g_SDOAApp->SetFocus(true);
	}
}

bool CGame::GetIGWFocus()
{
	if(g_SDOAApp)
	{
		bool b = g_SDOAApp->GetFocus();
		return b;
	}else{
		return false;
	}

}

void CGame::ReleaseIGW()
{
	if (g_isSDOLogin && g_SDOAApp)
	{ // 登录成功，退出时要登出
		g_SDOAApp->Logout();
		g_isSDOLogin = false;
	}
	/* 释放IGW图形引擎 */
	if (g_SDOADx9)
		g_SDOADx9->Finalize();
	// 游戏释放
	CloseIGW();
}

void CGame::ReadLogMode()
{
	bool bIsSdo = false;
	ifstream stream;
	string strTemp;
	stream.open("setup/LoginMode.ini");
	if( stream.is_open() )
	{
		stream >> strTemp >> bIsSdo
			   >> strTemp >> m_bServerlistDat
			   >> strTemp >> m_bQuestTrace
			   >> strTemp >> m_bShowUserTreaty;
		stream.close();
		m_bIsSadoLog = bIsSdo;
	}
}

//用于返回选择人物界面的消息
void CGame::ReLoadGame()
{
	ReNew();
	long hr = 0;
	if(m_pNetClient)
	{
		m_pNetClient->Exit();
		m_pNetClient->Close();
	}
	SAFE_DELETE( m_pNetClient );

	SetGameState(GS_BLACK);
	Sleep(1000);

	m_pNetClient = new CClient;
	m_pNetClient->Start(m_pDBAllocator,20,NF_LS_Client);

	hr = m_pNetClient->Connect( GetGame()->GetloginServerIP(),GetGame()->GetLoginServerPort());	// 连接服务器
	if( !hr )	// 连接失败
	{
		m_pNetClient->Exit();
		m_pNetClient->Close();
		SAFE_DELETE( m_pNetClient );
		SetGameState(GS_LOGIN);
		return ;
	}

	char strSndName[CDKEY_SIZE]="";
	if(GetGame()->GetIsSadoLog())
	{
		string strSnd = GetGame()->GetSndaid();
		sprintf(strSndName,"%s",strSnd.c_str());
	}else{
		string strSnd = GetGame()->GetGameCDKEY();
		sprintf(strSndName,"%s",strSnd.c_str());
	}
	//发送请求角色列表的消息
	CMessage msg( MSG_C2L_LOGIN_AND_QUEST_PLAYERBASELIST );	
	msg.AddEx((void*)&strSndName,CDKEY_SIZE);
	for(int i=0;i<4;i++)
	{
		DWORD dwReCode = m_lReGameCode[i];
		msg.Add((DWORD)dwReCode);
	}
	msg.Send(m_pNetClient);
}

void CGame::SaveLastVersion()
{
	string strCurServion = CClientResource::GetInstance()->GetVersion().c_str();
	FILE *pf = fopen("setup/LastVersion.ini","wb");
	{
		if(pf)
		{
			fwrite(strCurServion.c_str(),strCurServion.size(),1,pf);
			fclose(pf);
		}
	}
}

void CGame::LoadLastVersion()
{
	FILE *pf = fopen("setup/LastVersion.ini","rb");
	{
		if(pf)
		{
			char strbuf[200]= "";
			while(!feof(pf))
			{
				fscanf(pf,"%s\n",strbuf);
				m_LastVersion = strbuf;
			}
			fclose(pf);
		}
	}
}

bool CGame::CompareVersion()
{
	LoadLastVersion();
	string strCurServion = CClientResource::GetInstance()->GetVersion().c_str();
	if(m_LastVersion.compare(strCurServion)==0)
		return true;
    SaveLastVersion();
	return false;
}

void CGame::LoadOtherExeIni()
{
	FILE *pf = fopen("setup/OtherExeFile.ini","rb");
	{
		if(pf)
		{
			char strbuf[200]= "";
			while(!feof(pf))
			{
				string strFile;
				fscanf(pf,"%s\n",strbuf);
				strFile = strbuf;
				m_VecOtherExeFile.push_back(strFile);
			}
			fclose(pf);
		}
	}
}

void CGame::RunOtherExe()
{
	vector<string>::iterator it = m_VecOtherExeFile.begin();
	for(;it!=m_VecOtherExeFile.end();++it)
	{
		ShellExecute(0, NULL, it->c_str(), NULL, NULL, 1); 
	}
}