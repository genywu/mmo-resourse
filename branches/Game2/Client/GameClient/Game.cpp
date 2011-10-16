#include "stdafx.h"
#include "game.h"
#include "SDOA4Client.h"
#include "SDOADx9.h"
#include "GameControl.h"
#include "CollectionGoods.h"
#include "../../EngineExtend/WeatherManager.h"
#include "../../EngineExtend/GameMap.h"
#include "../../EngineExtend/GameModelManager.h"
#include "../../EngineExtend/TextureProjective.h"
#include "Nets/Clients.h"
#include "Nets/MemClient.h"
#include "MsgBoxEvent.h"
#include "MiniMap.h"
#include "Other/CountryList.h"
#include "Other/AudioList.h"
#include "../Audio/FmodAudioDef.h"
#include "ClientWordsFilter.h"
#include "Other/AudioList.h"
#include "Other/VideoManager.h"
#include "Other/PicList.h"
#include "Other/Chat.h"
#include "Other/BusinessSystem.h"
#include "Skills/SkillListXml.h"
#include "Other/WeaponEffectXml.h"
#include "Other/RegionList.h"
#include "Other/DeputyOccuSystemClient.h"
#include "Other/FaceActionList.h"
#include "QuestSystem/QuestSystem.h"
#include "Other/EffectSetup.h"
#include "Other/StateListXml.h"
#include "Other/AutoSupply.h"
#include "Effect.h"
#include "PetSystem/Pet.h"
#include "organizingsystem/OrganizingCtrl.h"
#include "organizingsystem/Faction.h"
#include "organizingsystem/Union.h"
#include "PlayerRanks.h"
#include "message/sequencestring.h"
#include "../GameClient/ClientApp/Shape.h"
#include "../GameClient/ClientApp/Player.h"
#include "../../Public/Common/StudySkillCondition.h"
#include "../../Public/Common/NetSessionManager.h"
#include "../../Public/Common/DupliRegionSetup.h"
#include "../../Public/Common/OccupSkillList.h"
#include "../../Public/Common/AccountServerDef.h"
#include "../../Public/Common/PetSetup.h"
#include "../UI/UIDef.h"
#include "../UI/UIManager.h"
#include "../UI/GamePage/Login/LoginPage.h"
#include "../UI/GamePage/SelectWorldServer/WorldServerPage.h"
#include "../UI/GamePage/SelectRole/SelectRolePage.h"
#include "../UI/GamePage/CreateRole/CreateRolePage.h"
#include "../UI/GamePage/MainBar/MainBarPage.h"
#include "MainPlayerHand.h"
#include <comutil.h>
#include "../GameClient/ClientApp/Other/NpcTalkBox.h"
#include "../GameClient/ClientApp/GameDesp/GameDespParser.h"
/***********************************************************************/
/* zhaohang add */
#include "../GameClient/ClientApp/Skills/SkillIni.h"
//#include "../GameClient/ClientApp/Skills/SkillListIni.h"
/***********************************************************************/
//#include "vld.h"

#ifdef _DEBUG
#ifdef ENABLE_ALOADER
#error "predefinied ENABLE_ALOADER"
#endif
//#define ENABLE_ALOADER // 开关"Game::Init"异步读取
#endif
#include "../../Public/Package/ClientResLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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

string g_strLastWisper;      //最后一个私聊对象
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
BOOL g_bLogin = false; 

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
    //GetGame()->GetCGuiEx()->GetSystemSetupPageEx()->SetDrugPos(1,(float)GetGame()->GetSetup()->fMusicVolume,true);
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
    //GetGame()->GetCGuiEx()->GetSystemSetupPageEx()->SetDrugPos(2,(float)GetGame()->GetSetup()->fSoundVolume,true);
    GetGame()->GetAudioList()->SetSoundVolume(GetGame()->GetSetup()->fSoundVolume);
}

DWORD WINAPI TestGetAudioEffectVolume()
{
    return (DWORD)(GetGame()->GetSetup()->fSoundVolume*100);
}

CGame::CGame(void):m_gdParer(0)
{
    SetBackColor(255, 0,0,255);
    m_pMouse = NULL;
    m_pKeyboard = NULL;
    //m_pAudioEng = NULL;

    m_uGameServerPort = 2347;
    m_strGameServerIP = "127.0.0.1";
    m_uLoginServerPort = 2346;
    m_strLoginServerIP = "127.0.0.1";

    m_pNetClient = NULL;
    //=====================界面相关属性===============================
    //m_pInterface = NULL;

    m_pOccupSkillList = NULL;
	/***********************************************************************/
	/* zhaohang add */
	m_pSkillDocIni = NULL;
	m_pSkillShowIni	  = NULL;
	m_pSkillPoseIni = NULL;
	//m_pSkillAttrIniSetup = NULL;
	/***********************************************************************/
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
    m_Setup.ulFrameControl = 10;
    m_Setup.bupdateonlostfoucs  = false;
    m_Setup.bfreecamera  = false;

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
    m_pKingIcon = NULL;
    m_pBitmapFemaleIcon = NULL;
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
    
    g_bOutPut = g_bDebug;
    m_strSndaid = "";

    m_bIsBeCheateForbid = false;
    m_bIsSadoLog = false;
    m_bSadoLogPageOpen = false;
    m_iAreaID = 1;
    m_iGroupID = 1;
    m_bIsShowSdo = true;
    m_AutoSupply = NULL;
    m_NpcTalkBox = NULL;
    m_FactionManager = new CFactionManager;
    m_UnionManager   = new CUnionManager;
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

    //	static FLOAT invert_width = 1.0f / WINDOW_SCALE_WIDTH;
    //	static FLOAT invert_height = 1.0f / WINDOW_SCALE_HEIGHT;

    //	g_pointMousePos.x = (LONG)(g_pointMousePos.x * invert_width);
    //	g_pointMousePos.y = (LONG)(g_pointMousePos.y * invert_height);

    if(GetMouse())
    {
        GetMouse()->SetPosition(g_pointMousePos.x, g_pointMousePos.y);
    }

    if(GetKeyboard() && !g_bInIME)
    {
        GetKeyboard()->RecieveKeyboardInput();	// 获得键盘数据
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
    //GetAudioList()->GarbageColloction();	// 垃圾音效/音乐回收
    CVideoManager::GetInstance()->GarbageColloction();
    UpdateSetup();
    GetInputInfo();
    // 清除鼠标缓冲区
    if( GetMouse() )
    {
        GetMouse()->RecieveMouseInput();			// 获得鼠标数据
    }
    GetAudioList()->Run();

    //cegui处理
    UIManager::Run();

    // 登陆延迟时间控制
    if(m_lLoadingDelayTime > 0)
    {
        DWORD dwDelayTime = 0;
        CGlobeSetup::tagSetup* pSetup = CGlobeSetup::GetSetup();
        if(pSetup)
            dwDelayTime = pSetup->dwLoadingDelayTime;
        DWORD time = timeGetTime();
        if(time - m_lLoadingDelayTime > dwDelayTime)
        {
            m_lLoadingDelayTime = 0;
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
    case GS_STAFF:
        
        break;

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
            /////////////////////////////////////////////////////////////////////////////////////////////

            //显示等候队列
            if(m_lQueuePos > 0)
            {
                char str[100];
                sprintf_s<100>(str,"正在连接游戏服务器……\n队列第%d位,估计用时%d秒。",m_lQueuePos,m_dwCountDownTime/1000);

            }

            PopWinMessage();
        }
        break;
    case GS_PWD_CARD:
        {

        }
        break;
    case GS_SELECTWS:
        {
            if (bControl == false)	break;

            //显示等候队列
            if(m_lQueuePos >= 0)
            {
                char str[100];
                sprintf_s<100>(str,AppFrame::GetText("Base_115"),m_lQueuePos,m_dwCountDownTime/1000); 
                //MessageBox(g_hWnd,str,"Info",MB_OK);
            }
            PopWinMessage();
        }
        break;

    case GS_SELECT:
        {
            if (bControl == false)	break;

            //显示等候队列
            if(m_lQueuePos > 0)
            {
                char str[100];
                sprintf_s<100>(str,AppFrame::GetText("Base_115"),m_lQueuePos,m_dwCountDownTime/1000);
                //GetInterface()->GetSelectRolePage()->SetMsgBoxText(str);
            }
            PopWinMessage();
            //GetInterface()->GetSelectRolePage()->Run();
            /*if(PopWinMessage(message,wParam,lParam) && g_pGuangMGUIRoot!=NULL)
            {
            g_pGuangMGUIRoot->ProcessMessage(message,wParam,lParam);
            }*/
            //GetInterface()->GetSelectRolePage()->Run();
        }
        break;

        // 等待进入游戏
    case GS_WAITING:
        break;

    case GS_BLACK:
        break;

    case GS_CREATE:
        {
            if (bControl == false)	break;
            PopWinMessage();
            /*if(PopWinMessage(message,wParam,lParam) && g_pGuangMGUIRoot!=NULL)
            {
            g_pGuangMGUIRoot->ProcessMessage(message,wParam,lParam);
            }*/
            //GetInterface()->GetCreateRolePage()->Run();
        }
        break;

    case GS_RUN:
        {
            if (IsReGame())
            {
                SetReGame(false);
                //if(!g_isSDOLogin)
                //{
                //ConnectLoginServer(GetloginServerIP(), GetLoginServerPort(), GetCDKEY(), GetPASSWORD(), GetWSName());
                //}else{
                //ConnectLoginServer(GetloginServerIP(), GetLoginServerPort(), GetSonID(),GetWSName());
                ReLoadGame();
                //}

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
                                                CQuestQueue* pQueueList = pMPlayer->GetRoleQueue();
                                                if (pQueueList!=NULL)
                                                {
                                                    if (pQueueList->GetRecordByID(QustIter->nQuestId)==NULL||
                                                        pQueueList->GetQuestStepNum(QustIter->nQuestId)!=QustIter->dwRoleStep)
                                                    {
                                                        continue;
                                                        //bShowResults = false;
                                                        //PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUEST));
                                                    }
                                                    else if (pQueueList->GetRecordByID(QustIter->nQuestId)!=NULL&&pQueueList->GetQuestStepNum(QustIter->nQuestId)==QustIter->dwRoleStep)
                                                    {
                                                        //PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUESTSTEP));
                                                        //bShowResults = false;
                                                        break;
                                                    }
                                                }
                                                else
                                                {
                                                    //PutDebugString(CStringReading::LoadText(eIDS_CONTROL,eIDS_CONTROL_NOQUEST));
                                                    //bShowResults = false;
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
                                                Log4c::Trace(ROOT_MODULE,GetText("GC_8"));
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
                GetInst(MainBarPage).Run();

#ifdef _RUNSTACKINFO_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN UIPage Updata.");
#endif

                int iAct = GetMainPlayer()->GetAction();

#ifdef _RUNSTACKINFO_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN PopWinMessage Start.");
#endif
                PopWinMessage();
#ifdef _RUNSTACKINFO_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::GS_RUN PopWinMessage End.");
#endif
                if (bClick)
                {
                    // ---- 如果操作了界面，那么就不允许玩家移动 ----
                    if ( GetMouse()->GetCurrentButton() != LB_DOWN && GetMouse()->GetCurrentButton() != RB_DOWN )
                        GetGameControl()->SetMoveAble(false);
                    else
                    {
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
                //DWORD dwCurTime = GetCurTickCount();
                //更新移动计时器
                //CMoveShape::UpdateMoveTimeCounter(dwCurTime);

#ifdef _RUNSTACKINFO_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CClientRegion::Run() Start.");
#endif
                GetRegion()->AI();

#ifdef _RUNSTACKINFO_
                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CClientRegion::Run() End.");
#endif
                //CMoveShape::UpdateLastMoveTimeCounter(dwCurTime);

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
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"表情动画更新 Start.");
#endif
            //表情动画更新
            UpdateFace();
#ifdef _RUNSTACKINFO_
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"表情动画更新 End.");
#endif

            s_lAITick++;
        }
        break;
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
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"抓屏 Start.");
#endif
    // 抓屏
    if ( GetKeyboard()->GetCurrentKey() == DIK_SYSRQ )
    {
        GetKeyboard()->SetCurrentKey(NONE);

        static SYSTEMTIME st;
        GetLocalTime(&st);
        static char strFileName[_MAX_PATH];
        sprintf_s<_MAX_PATH>(strFileName,"PrintScreen/Luvinia %d-%d-%d[%2.2d.%2.2d.%2.2d].jpg",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
        //GetGame()->GetInterface()->AddText(strFileName);

        //创建一个拷屏的保存目录
        CreateDirectory("PrintScreen",NULL);

        PrintScreen(strFileName);
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"抓屏 End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"生成地图BUG报告 Start.");
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
                sprintf_s<64>(buf,"%s\t%d\t%d\r\n",GetRegion()->GetName(),GetMainPlayer()->GetTileX(),GetMainPlayer()->GetTileY());
                fwrite(buf,strlen(buf),1,hFile);
                fclose(hFile);
                sprintf_s<64>(buf,"%s:%d,%d\r\n",GetRegion()->GetName(),GetMainPlayer()->GetTileX(),GetMainPlayer()->GetTileY());
                //GetInterface()->AddText(buf);
            }
        }
    }
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"生成地图BUG报告 End.");
#endif

#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::Run() End.");
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

    // ChatServer消息处理
    if(CMessage::GetClient4Cs() != NULL)
    {
        msgQueue mQueue;
        CMessage::GetClient4Cs()->GetRecvMessages()->GetAllMessage(mQueue);
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
    GetInst(UIManager).Clear2DBuff();
    //////////////////////////////////////////////////////////////////
    render::Interface::GetInstance()->BeginScene();
    // 登陆延迟时间控制
    if(m_lLoadingDelayTime > 0)
    {
        long lID = 0;
        if(GetMainPlayer())
            lID = GetMainPlayer()->GetRegionID();
        GetInst(UIManager).Render();
        return 0;
    }

/*-------------------游戏调试功能--------------*/
    static bool isEditUI = false;
    if (g_bDebug)
    {
        if (GetKeyboard()->IsKeyDown(DIK_LALT) && GetRegion())
        {
            char szTmp[100];
            sprintf_s<100>(szTmp, "当前视线距离:%.2f", GetRegion()->GetRegionCamera()->GetDistance());
            OutPutText(10, 120, szTmp, strlen(szTmp), 0xffffffff);
        }

        if (GetKeyboard()->IsKeyDown(DIK_RALT))
            ShowFps();

        //if (GetKeyboard()->IsKeyDown(DIK_LCONTROL) == false)
        //{
        //    GameGraphics2D::RecoverFogState();
        //    return true;
        //}

        if (GetKeyboard()->GetCurrentKey() == DIK_BACKSPACE && GetKeyboard()->IsKeyDown(DIK_RCONTROL))
        {
            char str[256];

#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_BACKSPACE+DIK_RCONTROL重读文件... Start.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
            sprintf_s<256>(str, "重读技能配置文件.../n重读声音配置文件.../n重读特效配置文件.../n重读表情文件/n");
            int x = SCREEN_WIDTH/2- (GetFontWidth(str)/2/4);
            int y = SCREEN_HEIGHT/2;
            GetGame()->OutPutText(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
            GetGame()->OutPutText(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,0,0));

            CSkillListXml::LoadSkillListXml();
            CStateListXml::Load("data/StateXml.xml");

            GetGame()->GetAudioList()->LoadSetup("data\\other.ini");
            CEffectSetup::LoadEffectSetup("data/effectsetup.ini");
            //GetGame()->LoadArcheryEffect("data/ArcheryEffect.ini");
            GetGame()->LoadFace("data/Face.ini");	//读取表情文件
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_BACKSPACE+DIK_RCONTROL重读文件... End.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
        }

        if (GetKeyboard()->GetCurrentKey()==DIK_F9 && GetKeyboard()->IsKeyDown(DIK_RCONTROL))
        {
            char str[256];
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F9重读所有模型文件... Start.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
            sprintf_s<256>(str, "重读所有模型文件...");
            int x = SCREEN_WIDTH/2-(GetFontWidth(str)/2);
            int y = SCREEN_HEIGHT/2;
            GetGame()->OutPutText(x+1, y+1, str,lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
            GetGame()->OutPutText(x, y, str,lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));

            CGameModelManager::GetInstance()->ReloadAllModel();
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F9重读所有模型文件... End.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
        }

        if (GetKeyboard()->GetCurrentKey()==DIK_F10 && GetKeyboard()->IsKeyDown(DIK_RCONTROL))
        {
            char str[256];
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F10重读文件... Start.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
            sprintf_s<256>(str, "重读状态配置文件.../n重读国家配置文件..../n");
            int x = SCREEN_WIDTH/2-(GetFontWidth(str)/2);
            int y = SCREEN_HEIGHT/2;
            GetGame()->OutPutText(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
            GetGame()->OutPutText(x, y, str, lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));
            CStateListXml::Load("data/StateXml.xml");

            GetGame()->LoadFace("data/Face.ini");	//读取表情文件

            CCountryList::Load("data/CountryList.xml");

            m_gdParer->LoadDespCfg();   //重新读取描述规则配置

#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F10重读文件... End.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
        }

        if (GetKeyboard()->GetCurrentKey()==DIK_F11 && GetKeyboard()->IsKeyDown(DIK_RCONTROL))
        {
            char str[256];
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F11重读颜色配置文件... Start.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
            sprintf_s<256>(str, "重读颜色配置文件...");
            int x = SCREEN_WIDTH/2-(GetFontWidth(str)/2);
            int y = SCREEN_HEIGHT/2;
            GetGame()->OutPutText(x+1, y+1, str, lstrlen(str), D3DCOLOR_ARGB(255,0,0,0));
            GetGame()->OutPutText(x, y, str,lstrlen(str), D3DCOLOR_ARGB(255,255,255,255));
            CGlobeSetup::Load("data/globesetup.ini");

#ifdef _RUNSTACKINFO_
            sprintf_s(str,"DIK_F11重读颜色配置文件... End.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
        }
        //重读当前鼠标所再界面的配置
        if(GetKeyboard()->GetCurrentKey() == DIK_R && GetKeyboard()->IsKeyDown(DIK_LALT))
        {
            char Info[128] = "";
            const char * layoutName = ReloadUILayout(Info);

            if (strcmp(layoutName,"") != 0)
            {
                GetInst(UIManager).FreshCurEditComponent();
                sprintf_s<128>(Info,"%s Reload success!",layoutName);
                MessageBox(g_hWnd, Info, "Info", MB_OK);
            } 
            else
            {
                MessageBox(g_hWnd,"Can not find the window layout!","WARNNING!",MB_OK);
            }
        }
        
        //编辑界面
        if (GetKeyboard()->GetCurrentKey() == DIK_E && GetKeyboard()->IsKeyDown(DIK_LCONTROL))
        {
            isEditUI = !isEditUI;
        }
        if (isEditUI)
        {            
            GetInst(UIManager).EditUI();
            if (GetKeyboard()->GetCurrentKey() == DIK_L)
            {
                GetInst(UIManager).FreshCurEditComponent();
            }
            if (GetKeyboard()->GetCurrentKey() == DIK_S && GetKeyboard()->IsKeyDown(DIK_LALT))
            {
                SaveLayout();
            }
        }
        //隐藏游戏UI
        static bool isShowUI = true;
        if (GetKeyboard()->GetCurrentKey() == DIK_HOME && GetKeyboard()->IsKeyDown(DIK_LCONTROL))
        {
            isShowUI = !isShowUI;
            UIManager::ShowUI(isShowUI);
        }
    }
    
    switch( GetGameState() )
    {
    case GS_STAFF:

        break;
    case GS_LOGIN:
        GetInst(LoginPage).Display();
        break;
    case GS_PWD_CARD:

    case GS_SELECTWS:
        GetInst(LoginPage).Display();
        break;
    case GS_SELECT:
        GetInst(SelectRolePage).Display();
        break;
    case GS_BLACK:
        GetInst(UIManager).Render();
        break;
    case GS_CREATE:
        GetInst(CreateRolePage).Display();
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
                            /*float fFogStart  = (float)GetGame()->GetSetup()->lViewRange * 4.f / 5.f;
                            float fFogEnd	 = (float)GetGame()->GetSetup()->lViewRange;*/
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
            
            /* 绘制IGW界面 */
            if (g_SDOADx9)
                g_SDOADx9->RenderEx();
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"UI Display End");// 界面结束
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
            //=========================================================================================
            GetInst(MainBarPage).Display();
            // 界面显示
            UIManager::Render();
            CPlayer* pPlayer=GetMainPlayer();
            // 手上拿的物品
//            if(pPlayer && m_MainPlayerHand)
//            {
//#ifdef _RUNSTACKINFO_
//                sprintf_s(str,"Hand Goods Display Start");// 手上物品开始
//                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
//#endif
//                DWORD dwIconId = 0;
//                long GRID_WIDTH = 33;
//                int iDrawX = GetMouse()->GetMouseX()- GRID_WIDTH/2;
//                int iDrawY = GetMouse()->GetMouseY()- GRID_WIDTH/2;
//                if (m_MainPlayerHand->GetPGoodsOfMainPlayerHand()!=NULL)
//                {
//                    dwIconId = m_MainPlayerHand->GetPGoodsOfMainPlayerHand()->GetProperty()->dwIconId;
//
//                }
//                else if (m_MainPlayerHand->GetGoodsIndexOfMainPlayerHand()!=0)
//                {
//                    CGoodsList::tagGoods2* pstGoods2 = CGoodsList::GetProperty(m_MainPlayerHand->GetGoodsIndexOfMainPlayerHand());
//                    if (pstGoods2!=NULL)
//                    {
//                        dwIconId = pstGoods2->BaseProperty.dwIconId;
//                    }
//                }
//                if (dwIconId!=0)
//                {
//                    m_MainPlayerHand->Display(dwIconId, iDrawX, iDrawY);
//                    m_pHandsBackIcon->Draw(iDrawX-6,iDrawY-6);
//                }
//#ifdef _RUNSTACKINFO_
//                sprintf_s(str,"Hand Goods Display End");// 手上物品结束
//                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
//#endif
//                // 绘制手上的范围特效
//                if(m_MainPlayerHand->GetAreaSkillID() > 0)
//                    m_MainPlayerHand->DisplayEffectInHand();
//            }	
//
//            // 手上拿的技能图标
//            if (pPlayer && m_MainPlayerHand && m_MainPlayerHand->GetSkillOfMainPlayerHand())
//            {
//#ifdef _RUNSTACKINFO_
//                sprintf_s(str,"Hand Skill Display Start");// 手上技能开始
//                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
//#endif
//                int iDrawX = GetMouse()->GetMouseX() - 16;
//                int iDrawY = GetMouse()->GetMouseY() - 16;
//                m_MainPlayerHand->Display( m_MainPlayerHand->GetSkillOfMainPlayerHand(), iDrawX, iDrawY);
//                m_pHandsBackIcon->Draw(iDrawX-6,iDrawY-6);
//#ifdef _RUNSTACKINFO_
//                sprintf_s(str,"Hand Skill Display End");// 手上技能开始
//                CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
//#endif
//            }
            // 绘制自己头上的提示文字
            if(GetRegion() && pPlayer && pPlayer->GetPlayerInfo().size()>0)
                GetRegion()->ShowPlayerInfo(pPlayer);
#ifdef _RUNSTACKINFO_
            sprintf_s(str,"CGame() GS_RUN Display End.");
            CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),str);
#endif
       }
    default:
       break;
    }
        
#ifdef _RUNSTACKINFO_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CGame::Display End.");
#endif
    if (g_bDebug && isEditUI)
    {
        //绘制当前编辑组件的颜色区域
        GetInst(UIManager).ShowEditColorArea();
    }
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
            //if(CTopMsg::GetInstance()->GetRollLantern())
            //{
            //	CTopMsg::GetInstance()->GetRollLantern()->ReleaseOffscreen();
            //}

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
                    //if(CTopMsg::GetInstance()->GetRollLantern())
                    //{
                    //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
                    //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
                    //}
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
                //if(CTopMsg::GetInstance()->GetRollLantern())
                //{
                //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
                //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
                //}
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
            Log4c::Warn(ROOT_MODULE,"Present非设备丢失失败!");
            if (GetGame())
            {
                if (GetGame()->GetRegion())
                {
                    GetGame()->GetRegion()->GetMiniMap()->DestroyDirect3DResource();

                }
            }
            //if(CTopMsg::GetInstance()->GetRollLantern())
            //{
            //    CTopMsg::GetInstance()->GetRollLantern()->ReleaseOffscreen();
            //}

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
                    //if(CTopMsg::GetInstance()->GetRollLantern())
                    //{
                    //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenLeft();
                    //    CTopMsg::GetInstance()->GetRollLantern()->CreateOffscreenRight();
                    //}
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
            >> strTemp >> m_Setup.ulFrameControl
            >> strTemp >> m_Setup.lnotsimplemodelnum
            >> strTemp >> m_Setup.bupdateonlostfoucs;
        stream.close();
        //m_Setup.fSoundVolume=min(max(0,m_Setup.fSoundVolume/255.f),1);
        //m_Setup.fMusicVolume=min(max(0,m_Setup.fMusicVolume/255.f),1);
        //m_Setup.fSpeechVolume=min(max(0,m_Setup.fSpeechVolume/255.f),1);
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
            << "FrameControl    "   << m_Setup.ulFrameControl << endl
            << "lsimplemodelnum "   << m_Setup.lnotsimplemodelnum << endl
            << "bupdatefoucs    "   << m_Setup.bupdateonlostfoucs << endl;
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
            >> strTemp >> m_TabSet.bMonster
			>> strTemp >> m_TabSet.bPet;
        stream.close();
        // 默认为怪物类型
        if(!m_TabSet.bEnemyPlayer && !m_TabSet.bPiratePlayer && !m_TabSet.bMonster && !m_TabSet.bPet)
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
            << "bTabMonster		"		<< m_TabSet.bMonster << endl
			<< "bTabPet		"			<< m_TabSet.bPet << endl;
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
        //MessageBox(g_hWnd, "Can't open file data/GameViewSetup.ini", "message", MB_OK);
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

//初始化mapfile
bool CGame::InitMapFile()
{
    if(g_bDebug)
    {
        m_pMemClient = new CMemClient();
        if( !m_pMemClient->Initial("GameClient",m_pDBAllocator) )
        {
            Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"设置内存影射文件(Name:GameClient)出错!");
            SAFE_DELETE(m_pMemClient);
            return false;
        }
        //初始化文件名
        SYSTEMTIME stTime;
        GetLocalTime(&stTime);
        sprintf_s(m_pszRunStackFile,MAX_PATH,"ClientRunStackInfo[%02d_%02d_%02d_%02d_%02d].txt",stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

        CMessage::SetMemClient(m_pMemClient);

        return true;
    }
    return false;
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
*/
int CGame::Init(void)
{
#ifdef _RUNSTACKINFO_
    //StartStackInfo();
#endif
    ALOADER_STARTUP( GetCPUNum() - 1 );
    ALOADER_TIME_STAT( "AllRes", 
    CreateDirectory( "log",	NULL);

    Log4c::Trace(ROOT_MODULE,"Init begin...");

    srand((unsigned)time(NULL));
    random(100);

    /*char szFileName[MAX_PATH];
    GetModuleFileName( NULL, szFileName, MAX_PATH);
    string strPathTempt = szFileName;
    strPathTempt = strPathTempt.substr(0, strPathTempt.find_last_of('\\'));*/


    CHAR* strPath = new CHAR[MAX_PATH];
    string strWorpath("");
    if(::GetCurrentDirectory(MAX_PATH,strPath))
    {
        strWorpath=string(strPath);
    }
    delete []strPath;

    //第一次运行时写入注册表
    WriteReg("Client_Path",strWorpath.c_str());

    ALOADER_TIME_STAT( "1", 
    string strPackName("Game.pak"); 
    //string strFilesInfo("FilesInfo.ril");
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
    )
    Log4c::Trace(ROOT_MODULE,"Init package file OK!");
    //初始化模型管理器
    ALOADER_TIME_STAT( "2", 
    CGameModelManager::InitInstance();
    //设置纹理共享文件夹
    CGameModelManager::GetInstance()->SetTextureShareFolder("Texture");
    //初始化建物管理器
    GameBuildList::InitInstance();
    )
    m_pDBAllocator = new CDataBlockAllocator(100,1024);

    //m_Setup.bMusic = true;
    //m_Setup.bSound = true;

    extern HINSTANCE hInst;
    ALOADER_TIME_STAT( "3", 
    m_pMouse = new CMouse();
    m_pMouse->Create( hInst, g_hWnd );
    m_pKeyboard = new CKeyBoard();
    m_pKeyboard->Create( hInst, g_hWnd );
    )
    Log4c::Trace(ROOT_MODULE,"Init Input OK!");

    CClientWordsFilter::GetInstance()->Initial(string("Data/filtrater.dat"));

    Log4c::Trace(ROOT_MODULE,"Init Audio Engine OK!");

    CMessage::Initial(m_pDBAllocator,100);

    //初始化内存隐射文件
    //InitMapFile();

    // 初始化网络
    CMySocket::MySocketInit();

    Log4c::Trace(ROOT_MODULE,"Init Net OK!");

    CGlobeSetup::Load("data/globesetup.ini");
    CMySocket::SetEncryptFlag(CGlobeSetup::GetSetup()->lEncryptType);

    m_pDupliRegionSetup = new CDupliRegionSetup();

    ALOADER_TIME_STAT( "4", 
    //初始化图形模块
    this->CreateGraphices();

    )
    Log4c::Trace(ROOT_MODULE,"Init Graphics OK!");

    m_pPicList = new CPicList;
    m_pAudioList = new CAudioList;
    m_pAudioList->SetMusicEnabled(m_Setup.bMusic);
    m_pAudioList->SetSoundEnabled(m_Setup.bSound);
    m_pAudioList->SetSpeechEnabled(m_Setup.bSpeech);
    m_pAudioList->SetMusicVolume(m_Setup.fMusicVolume);
    m_pAudioList->SetSoundVolume(m_Setup.fSoundVolume);
    m_pAudioList->SetSpeechVolume(m_Setup.fSpeechVolume);

    //=====================游戏界面初始化==========================//
    // 客户端字符串统一管理初始化
    AppFrame::LoadTextRes();
    ALOADER_TIME_STAT( "5", 
    GetInst(UIManager).Init();
    )
   
    //===================玩家手上容器================================
    m_MainPlayerHand = new CMainPlayerHand;

    
    //ALOADER_TIME_STAT( "Text", 
    //ALOADER_LOAD( Text, 0, 
    //AppFrame::LoadTextRes();
    //);
    //)

    CPlayer::InitPlayerOccuType();	// 初始化职业描述
    CPlayer::InitPlayerConstType();	// 初始化星座描述
   

    // 初始化物品操作配置对象
    //m_GoodsExSetup.ReadSetup( "Data/GoodsExSetup.xml" );

    //创建FreeType对象
    //m_pFreeType = new FreeTypeFont(render::Interface::GetInstance()->GetDevice());
    //m_pFreeType->InitialFreeTypeFont();

    ALOADER_TIME_STAT( "7", 
    CVideoManager::GetInstance()->Init(g_hWnd);
    )


    Log4c::Trace(ROOT_MODULE,"Init Interface OK!");

    m_pGameControl = new CGameControl;

    //-----------------07.9.4(liuke任务)------------------
    ALOADER_TIME_STAT( "Quest", 
    ALOADER_LOAD( Quest, CQuestSystem::Init, 
    CQuestSystem::Init("quest/QuestList.xml");
    );
    )
	/***********************************************************************/
	/* zhaohang add */
	/***********************************************************************/
	//技能文案配置
	m_pSkillDocIni = new SkillDocIni();
	m_pSkillDocIni->LoadFile(CGlobeSetup::GetSetup()->strSkillDocCon);
	//技能显示配置
	m_pSkillShowIni = new SkillShowIni();
	m_pSkillShowIni->LoadFile(CGlobeSetup::GetSetup()->strSkillShowIni);
	//技能姿态配置
	m_pSkillPoseIni = new SkillPoseIni();
	m_pSkillPoseIni->LoadFile(CGlobeSetup::GetSetup()->strSkillPoseIni);
	//技能属性配置
	/*m_pSkillAttrIniSetup = new SkillAttrIniSetup();
	m_pSkillAttrIniSetup->LoadFile(CGlobeSetup::GetSetup()->strSkillAttrIniSetup);*/
	//SkillAttrIniSetup::LoadFile(CGlobeSetup::GetSetup()->strSkillAttrIniSetup);
    // 各个职业的技能列表
    m_pOccupSkillList = new COccupSkillList;
    ALOADER_TIME_STAT( "OccuSkill", 
    ALOADER_LOAD( OccuSkill, m_pOccupSkillList, 
    m_pOccupSkillList->Init("data/OccupSkillList.xml");
    );
    )
    ALOADER_TIME_STAT( "StudySkill",
    // 技能学习条件表
    if(CStudySkillCondition::Init("data/StudySkillCondition.xml"))
        Log4c::Trace(ROOT_MODULE,"Init StudySkillCondition OK!");
    // 转职相关配置
    if(CChangeOccu::Init("data/ChangeOccu.xml"))
       Log4c::Trace(ROOT_MODULE,"Init ChangeOccu OK!");
    )
    
    //=============================================================//
    LoadMonsterQuestEffect("data/MonsterQuestEffect.ini");
    CChat::Init();

    //if (CCreateRolePageEx::LoadFaceHairIni("data/FaceHair.ini"))
    //    PutDebugString("Init FaceHair OK!");
    ALOADER_TIME_STAT( "Goods", 
    ALOADER_LOAD( Goods, CGoodsList::LoadGoodsList, 
    if( CGoodsList::LoadGoodsList("data/goods.dat") )
        Log4c::Trace(ROOT_MODULE,"Init GoodsList OK!");
    );
    )

    //if( CCardMutifunPageEx::InitCostData("data/EnchaseCost.ini") )
    //    PutDebugString("Init EnchaseCost OK!");
    ALOADER_TIME_STAT( "Suit",
    CGoodsList::LoadSuitList("data/suit.dat");
    Log4c::Trace(ROOT_MODULE,"Init Suit OK!");
    CGoodsList::LoadCardList("data/enchase.ini");
    Log4c::Trace(ROOT_MODULE,"Init Card OK!");
    CGoodsList::InitMoneyGraphics("data/money.ini");
    Log4c::Trace(ROOT_MODULE,"Init Money.ini OK!");
    )

    //if (CFacturePageEx::LoadFactureData("data/Facture.ini"))
    //    PutDebugString("Init Facture.ini OK!");

    CBusinessSystem::InitCreditLevel("data/CreditLevel.ini");
    Log4c::Trace(ROOT_MODULE,"Init CreditLevel.ini OK!");

    if(CBusinessSystem::InitCreditSystem())
        Log4c::Trace(ROOT_MODULE,"Init CreditSystem OK!");

    CEffectSetup::LoadEffectSetup("data/effectsetup.ini");
    Log4c::Trace(ROOT_MODULE,"Init Effect OK!");
    ALOADER_TIME_STAT( "State", 
    ALOADER_LOAD( State, CStateListXml::Load, 
    CStateListXml::Load("data/StateXml.xml");
    Log4c::Trace(ROOT_MODULE,"Init StateXml OK!");
    );
    )

    ALOADER_TIME_STAT( "Skill", 
    ALOADER_LOAD( Skill, CSkillListXml::LoadSkillListXml, 
    CSkillListXml::LoadSkillListXml();
    );
    )
    Log4c::Trace(ROOT_MODULE,"Init skills OK!");
    CWeaponEffectXml::LoadWeaponEffect("data/WeaponEffectXml.xml");
    CCountryList::Load("data/CountryList.xml");
    Log4c::Trace(ROOT_MODULE,"Init Country OK!");

    ALOADER_TIME_STAT( "Map", 
    ALOADER_LOAD( Map, CRegionList::LoadMap,
    CRegionList::LoadMap();
    Log4c::Trace(ROOT_MODULE,"Init RegionList OK!");
    );
    )

    CMailBoxPosList::LoadMailBoxPos();
    Log4c::Trace(ROOT_MODULE,"Init LoadMailBoxPos OK!");

    ALOADER_TIME_STAT( "DeputyOccu",
    ALOADER_LOAD( DeputyOccu, CDeputyOccuSystemClient::InitOccuSystemClient, 
    CDeputyOccuSystemClient::InitOccuSystemClient("data/DeputyOccu/DeputyOccu.xml");
    );
    )
    Log4c::Trace(ROOT_MODULE,"Init DeputyOccuSyestem OK!");

    //===07.12.10liuke更改===
    ALOADER_TIME_STAT( "14", 
    m_pbitmapShapeTalk = CBitmapX::GetBitmapX("datafiles/imagesets/other/talkpaopao.tga");
    m_pbitmapTeamTalk = CBitmapX::GetBitmapX("datafiles/imagesets//other/teamtalk.tga");

    m_pbitmapIntanateTimePot = CBitmapX::GetBitmapX("datafiles/imagesets/other/cast-timepointer.tga");
    m_pbitmapIntanateBarBack = CBitmapX::GetBitmapX("datafiles/imagesets/other/cast-understratum.tga");
    m_pbitmapIntanateBar = CBitmapX::GetBitmapX("datafiles/imagesets/other/cast-time.tga");
    m_pbitmapHpBarBack = CBitmapX::GetBitmapX("datafiles/imagesets/other/npc-understratum.tga");
    m_pbitmapPlayerHpBar = CBitmapX::GetBitmapX("datafiles/imagesets/other/npc-HP.tga");
    m_pbitmapMonsterHpBar = CBitmapX::GetBitmapX("datafiles/imagesets/other/npc-HP2.tga");
    m_pbitmapHpBar2 = CBitmapX::GetBitmapX("datafiles/imagesets/other/npc-HP3.tga");
    m_pbitmapHpBarPet = CBitmapX::GetBitmapX("datafiles/imagesets/other/npc-HP.tga");
    //=======================

    m_pbitmapSuck = CBitmapX::GetBitmapX("datafiles/imagesets/other/suck.tga");
    m_pbitmapBlastSuck = CBitmapX::GetBitmapX("datafiles/imagesets/other/blastsuck.tga");
    m_pbitmapBlastAttack = CBitmapX::GetBitmapX("datafiles/imagesets/other/blastattack.tga");
    m_pbitmapBlock = CBitmapX::GetBitmapX("datafiles/imagesets/other/block.tga");
    m_pbitmapBlastBlock = CBitmapX::GetBitmapX("datafiles/imagesets/other/block.tga");
    m_pbitmapRebound = CBitmapX::GetBitmapX("datafiles/imagesets/other/rebound.tga");
    m_pbitmapBlastRebound = CBitmapX::GetBitmapX("datafiles/imagesets/other/blastrebound.tga");
    m_pbitmapFullMiss = CBitmapX::GetBitmapX("datafiles/imagesets/other/fullmiss.tga");
    m_pbitmapBlastFullMiss = CBitmapX::GetBitmapX("datafiles/imagesets/other/fullmiss.tga");
    m_pbitmapParry = CBitmapX::GetBitmapX("datafiles/imagesets/other/parry.tga");
    m_pbitmapBlastParry = CBitmapX::GetBitmapX("datafiles/imagesets/other/parry.tga");
    m_pbitmapResist = CBitmapX::GetBitmapX("datafiles/imagesets/other/resist.tga");
    m_pbitmapBlastResist = CBitmapX::GetBitmapX("datafiles/imagesets/other/resist.tga");
    m_pbitmapLost = CBitmapX::GetBitmapX("datafiles/imagesets/other/miss.tga");
    m_pbitmapExpTip = CBitmapX::GetBitmapX("datafiles/imagesets/other/exp.tga");
    m_pbitmapConTip = CBitmapX::GetBitmapX("datafiles/imagesets/other/Contribute.tga");

    m_pHandsBackIcon = CBitmapX::GetBitmapX("datafiles/imagesets/other/32pxunder.tga");
    )

   Log4c::Trace(ROOT_MODULE,"Init bitmaps OK!");
    ALOADER_TIME_STAT( "RegionData",
    LoadRegionData();
    )
    Log4c::Trace(ROOT_MODULE,"Init RegionData OK!");

    LoadFace("data/Face.ini");	//读取表情文件
    Log4c::Trace(ROOT_MODULE,"Init Face OK!");

    LoadTabSet();	//读取Tab设置文件
    Log4c::Trace(ROOT_MODULE,"Init tabset OK!");

    m_pFaceActionList = new CFaceActionList();
    //ALOADER_TIME_STAT( "FaceAction",
    //if (m_pFaceActionList->LoadFaceAction("data/FaceAction"))		//载入仪态配置信息
    //    PutDebugString("Init Face Action OK!");
    //)

    UpdateSetup();

    LoadMouseCursors("data/cursors.ini");
   Log4c::Trace(ROOT_MODULE,"Init cursors OK!");

    m_pBitMapFactionWarEnemy= CBitmapX::GetBitmapX("datafiles/imagesets/other/FactionWarEnemy.TGA");
    m_pBitMapFactionWarUion= CBitmapX::GetBitmapX("datafiles/imagesets/other/FactionWarUnion.TGA");

    m_pBitMapCityWarEnmey= CBitmapX::GetBitmapX("datafiles/imagesets/other/CityWarEnemy.TGA");
    m_pBitMapCityWarUnion= CBitmapX::GetBitmapX("datafiles/imagesets/other/CityWarUnion.TGA");	

    m_pBitMapOwnedCity= CBitmapX::GetBitmapX("datafiles/imagesets/other/OwnedCity.TGA");	
    m_pBitMapOwnedVillage= CBitmapX::GetBitmapX("datafiles/imagesets/other/OwnedVillage.TGA");

    m_pKingIcon = CBitmapX::GetBitmapX("datafiles/imagesets/other/KingIcon.TGA");
    m_pBitmapFemaleIcon = CBitmapX::GetBitmapX("pictures/female.TGA");

    m_dwFaceTexture = render::Interface::GetInstance()->CreateTexture("pictures/face/face.tga");

    Log4c::Trace(ROOT_MODULE,"Init FaceTexture OK!");	

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

    Log4c::Trace(ROOT_MODULE,"Init CEffect 3000006 OK!");	

	// 加载宠物强化材料配置
	GetInst(PetSetup).LoadPetMaterialConfig();

    //自动使用物品初始化
    m_AutoSupply = new AutoSupply;
    //COffciailRankConfig::LoadOfficialRankConfig();

    //PutDebugString("Init COffciailRankConfig OK!");	

    //读取场景摄像机配置文件
    LoadGameViewSetup();

    //初始化NPCTALKBOX
    m_NpcTalkBox = new NpcTalkBox;

    //加载物品描述解析配置
    m_gdParer = new GameDespParser;
    //读取登陆方式
    ReadLogMode();
    Log4c::Trace(ROOT_MODULE,"Init OK!");

    //加载成功，播放入场动画
    //if(GetGame()->GetIsSadoLog())
    //CVideoManager::GetInstance()->PlayVideo("MUSIC\\SDLog.wmv");
    //if (igwInitialize)
    //{	// 说明加载成功，表示下......
    //	//HWND hMainWin = DXUTGetHWND();
    //	SetWindowTextW(g_hWnd, L"星尘传说(测试版本) [加载IGW组件成功]");
    //}	
    //render::Interface *pInterface = render::Interface::GetInstance();
    //if (g_SDOADx9)
    //{	// 初始化IGW内部图形引擎
    //	D3DPRESENT_PARAMETERS d3dpp = pInterface->GetInfo()->d3dpp;
    //	g_SDOADx9->Initialize( pInterface->GetDevice(), &d3dpp, FALSE );
    //}
    //if (g_SDOAAppUtils)
    //{  // 设置音量控制回调函数
    //	g_SDOAAppUtils->SetAudioSetting(TestGetAudioSoundVolume, TestSetAudioSoundVolume, TestGetAudioEffectVolume, TestSetAudioEffectVolume);
    //}
    )
    ALOADER_TIME_STAT( "LoginScene",
    SetGameState( GS_LOGIN );
    )
    ::SendMessage(g_hWnd, WM_SETCURSOR, 0, (LPARAM)LoadCursor(NULL, IDC_ARROW));

    // 保证字符资源已经加载完毕
    ALOADER_TIME_STAT( "WaitTextLoading",
    ALOADER_WAIT( "Text" );
    )

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

    //界面释放
    //=====================界面资源释放==============================
    GetInst(UIManager).Fini();
    Log4c::Trace(ROOT_MODULE,"Remove Cegui UI OK!");
    //======================释放玩家手上容器========================
    SAFE_DELETE(m_MainPlayerHand);
    render::Interface * pInterface = render::Interface::GetInstance();
    CClientResource* pClientResource = CClientResource::GetInstance();
    pClientResource->Release();

    //释放资源管理器
    CClientResource::ReleaseInstance();
    Log4c::Trace(ROOT_MODULE,"Release ClientResource OK!");

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
    //CGCMana::Inst()->Clear();
    Log4c::Trace(ROOT_MODULE,"Clear CGCMana::Inst() OK!");

    //
    SAFE_DELETE( m_pMainPlayer );
    Log4c::Trace(ROOT_MODULE,"Remove MainPlayer OK!");

    SAFE_DELETE( m_pRegion );	
    Log4c::Trace(ROOT_MODULE,"Remove Region OK!");
    

    //--------------07.9.4-------------------
    CQuestSystem::Relese();
    Log4c::Trace(ROOT_MODULE,"Release CQuestSystem OK!");
    CStudySkillCondition::Release();
	/***********************************************************************/
	/* zhaohang add*/
	SAFE_DELETE(m_pSkillDocIni);
	SAFE_DELETE(m_pSkillShowIni);
	SAFE_DELETE(m_pSkillPoseIni);
	//SAFE_DELETE(m_pSkillAttrIniSetup);
	/***********************************************************************/
	
    SAFE_DELETE(m_pOccupSkillList);
    Log4c::Trace(ROOT_MODULE,"Release SkillCondition OK!");
    CChangeOccu::Release();
    list<stChatGoodsInfo>::iterator stCGInfoIt = m_lsChatGoodsInfo.begin();
    for (; stCGInfoIt != m_lsChatGoodsInfo.end(); ++stCGInfoIt)
    {
        SAFE_DELETE(stCGInfoIt->pGoods);
    }
    m_lsChatGoodsInfo.clear();
    Log4c::Trace(ROOT_MODULE,"Release stChatGoodsInfo OK!");
    m_vecMonsterQuestEffect.clear();
    CMailBoxPosList::ReleaseMailBoxPos();
    //===============================================================
    if(m_pNetClient)
    {
        m_pNetClient->Close();
        m_pNetClient->Exit();
    }
    SAFE_DELETE( m_pNetClient );
    Log4c::Trace(ROOT_MODULE,"Release Net OK!");

    CMessage::ResetClient4Cs();
    Log4c::Trace(ROOT_MODULE,"Release Net For Chat Server OK!");

    SAFE_DELETE(m_pPicList);
    Log4c::Trace(ROOT_MODULE,"Release PicList OK!");
    SAFE_DELETE(m_pGameControl);
    Log4c::Trace(ROOT_MODULE,"Release GameControl OK!");

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
    SAFE_DECREF(m_pKingIcon);
    SAFE_DECREF(m_pBitmapFemaleIcon);
    m_lLoadingDelayTime = 0;
    m_lLoadingTipsIndex = 0;

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

    Log4c::Trace(ROOT_MODULE,"Release All Game_BitmapX OK!");

    render::Interface::GetInstance()->DestroyTexture(m_dwFaceTexture);
    Log4c::Trace(ROOT_MODULE,"Release FaceTexture OK!");

    //CSkillList::Release();
    CSkillListXml::Release();
    CStateListXml::Release();
    Log4c::Trace(ROOT_MODULE,"Release Skill、State OK!");

    CWeaponEffectXml::Release();
    Log4c::Trace(ROOT_MODULE,"Release Effect OK!");
    //	GetCountryHandler()->Release();

    GetNetSessionManager()->Release();
    Log4c::Trace(ROOT_MODULE,"Release NetSession OK!");

    GetOrganiCtrl()->Release();
    Log4c::Trace(ROOT_MODULE,"Release OrganiCtrl OK!");

    SAFE_DELETE(m_pAudioList);
    Log4c::Trace(ROOT_MODULE,"Release Audio OK!");

    CBusinessSystem::ReleaseCreditSystem();
    Log4c::Trace(ROOT_MODULE,"Release CreditSystem OK!");

    //释放
    GameBuildList::GetInstance()->Release();
    GameBuildList::ReleaseInstance();
    Log4c::Trace(ROOT_MODULE,"Release BuildList OK!");

    //释放
    CGameModelManager::GetInstance()->Release();
    CGameModelManager::ReleaseInstance();
    Log4c::Trace(ROOT_MODULE,"Release Model OK!");

    CTextureProjective::Destroy();

    SAFE_DELETE(m_pFaceActionList);
    Log4c::Trace(ROOT_MODULE,"Release Face Action List OK!");

    //销毁图形模块
    DestroyGraphices();
    Log4c::Trace(ROOT_MODULE,"Destroy Graphices OK!");

    //m_listHelmHair.clear();
    //m_listRenderTrail.clear();
    m_listFace.clear();

    CClientWordsFilter::Release();

    GetPlayerRanks()->Release();

    CDeputyOccuSystem::Relese();
    Log4c::Trace(ROOT_MODULE,"Release deputyoccusystem OK!");

    SAFE_DELETE(m_pDupliRegionSetup);

    CMessage::Release();
    Log4c::Trace(ROOT_MODULE,"Release CMessage OK!");

    SAFE_DELETE(m_pDBAllocator);
    Log4c::Trace(ROOT_MODULE,"Release Data Block OK!");

#ifdef _DEBUG
#ifdef _CLIENT
    //CMsgDescribe::FinalRelease();
#endif
#endif

    CVideoManager::GetInstance()->Destroy();
    CVideoManager::ReleaseInstance();
    
    SAFE_DELETE(m_AutoSupply);
    //SAFEDELETE(m_pFreeType);
    SAFE_DELETE(m_NpcTalkBox);
    SAFE_DELETE(m_gdParer);
    SAFE_DELETE(m_FactionManager);
    SAFE_DELETE(m_UnionManager);
    Log4c::Trace(ROOT_MODULE,"Release OK!");
    return true;
}

// 重新开始前的准备工作
void CGame::ReNew(void)
{
    m_lScriptTimer=0;
    SetBackColor(255, 0,0,0);
    SAFE_DELETE(m_pRegion);
    m_pMainPlayer = NULL;
    //	GetInterface()->ReleaseWindow(GetInterface()->GetMainBar());
    GetOrganiCtrl()->ReNew();
   
    m_bDisplayInfo = false;
    m_lDisplaySpeed = 1;
    m_TopInfos.clear();
    m_stCurTopInfo.listCurInfo.clear();
    m_stCurTopInfo.lTotalLength = 0;
    m_dwDisplayTime = timeGetTime();
    m_lLoadingDelayTime = 0;
    m_lLoadingTipsIndex = 0;
    
    //重置反外挂状态
    ResetCheatSystem();
    GetGameControl()->ReNew();
    GetInst(UIManager).ReleaseAllPage();
}

//重置反外挂系统为初始状态
void CGame::ResetCheatSystem()
{
    m_bIsBeCheateForbid = false;
}
// 退出游戏
void CGame::QuitGame(void)
{
    PostQuitMessage(0);
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
    { // 登录成功，退出时要登出
        g_SDOAApp->Logout();
        //g_isSDOLogin = false;
        //if(!GetIsSadoLog())
        //{
        //	GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
        //	GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(true);
        //	GetGame()->GetCGuiEx()->GetLoginPageEx()->SetPageFocus(); 
        //}else{
        //	GetGame()->ShowLoginDialog();
        //}

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

bool CGame::ConnectChatServer(const char* ip, int port)
{
    CClient* client = new CClient;
    client->Start(m_pDBAllocator, 20, NF_CS_Client);
    if(!client->Connect(ip, port))
    {
        client->Exit();
        client->Close();
        SAFE_DELETE(client);
        return false;
    }
    CMessage::SetClient4Cs(client);
    return true;
}

void CGame::OnServerLost()
{	
    ReNew();	// 重新开始		
    SetGameState( GS_LOGIN );
    SDOLogOut();
    //g_bHandleSocketClose=true;
}


// 根据场景ID来绘制Loading画面
void CGame::DisPlayLoading(long lRegionID)
{
    // 显示loading画面
    //GameGraphics2D::SetUIModelRendState();
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Test1!");
    //render::Interface *pInterface = render::Interface::GetInstance();
    //pInterface->BeginScene();
    //pInterface->Clear(0);
    
    m_lLoadingTipsIndex = 0;
    char strFilename[MAX_PATH] = "pictures/loading/default.tga";
    if (m_mapRegion.find(lRegionID)!=m_mapRegion.end())
    {
        strcpy_s(strFilename,256, m_mapRegion[lRegionID].strLoadingPic.c_str());
    }

   
    if (IsFileExist(strFilename)==false)
        strcpy_s(strFilename,256, "pictures/loading/default.tga");
    
    
    if (false == GetInst(UIManager).isImageSetDefined(strFilename))
    {
        GetInst(UIManager).CreateImageSetFromFile(strFilename, strFilename);
    }

    int iWidth = GetInst(UIManager).GetImageWidth(strFilename);

    int iHeight = GetInst(UIManager).GetImageHeight(strFilename);
    int x = (SCREEN_WIDTH - iWidth) / 2;
    int y = (SCREEN_HEIGHT - iHeight) / 2;
   
    GetInst(UIManager).DrawImage(x, y, strFilename);
    if(m_lLoadingTipsIndex == 0)
        m_lLoadingTipsIndex = random((int)m_vectorLoadingTips.size());
    const char *pstr = m_vectorLoadingTips[m_lLoadingTipsIndex].c_str();
    
    x = (SCREEN_WIDTH - GetFontWidth(pstr)) / 2;
    y = y + iHeight - 50;
   
    GetGame()->OutPutText(x, y, pstr, lstrlen(pstr), D3DCOLOR_ARGB(255,255,255,255));
    UIManager::Render();
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
void CGame::QuestEnterRegion(long lRegionID,const CGUID& RegionID,long lResourceID,  char* strRegionName, long lPlayerX, long lPlayerY, long lPlayerDir, long lUseGoods)
{
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Start!");
    // 音乐	
    map<long,tagRegion>::iterator RegionDataIter=m_mapRegion.begin();
    for(;RegionDataIter!=m_mapRegion.end();RegionDataIter++)
    {
        if(RegionDataIter->second.bgMusicPlaying || RegionDataIter->second.bFightMusicPlaying)
        {			
            //GetAudioEngine()->FadeOut(GetGame()->GetMusicID(),2000);
            RegionDataIter->second.bgMusicPlaying=false;
            RegionDataIter->second.bFightMusicPlaying=false;
            break;
        }
    }
    // 显示loading画面
    DisPlayLoading(lRegionID);
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Test4!");

    CPlayer* pPlayer = GetGame()->GetMainPlayer();

    CRegionCamera * pRegionCamera = NULL;

    // 初始化地图
    if( m_pRegion )
    {
		// @todo 王晓轩
		CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
		CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
		if(pPet)
			m_pRegion->RemoveObject(pPet);
		m_pRegion->RemoveObject(pPlayer);

        pRegionCamera  = new CRegionCamera();
        *pRegionCamera = *m_pRegion->GetRegionCamera();

        SAFE_DELETE(m_pRegion);
    }

    ALOADER_TIME_STAT( "WaitAllRes",
    ALOADER_WAITALL();
    )
    ALOADER_SHUTDOWN();

    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Test5!");
    m_pRegion = new CClientRegion;
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Test6!");
    GetRegion()->SetExID(RegionID);
    GetRegion()->SetResourceID(lResourceID);
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Region Load!");
    GetRegion()->Load();
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) Region Init!");
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
        //GetGame()->GetCGuiEx()->GetRelivePageEx()->Open();
    }

    pPlayer->SetFather( GetRegion() );
    pPlayer->RefeashState();

    //	pPlayer->SetDisplayModelGroup();
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
	//@todo 王晓轩
	CGUID activePetGuid = GetGame()->GetMainPlayer()->GetActivePetGUID();
	CPet *pPet = GetGame()->GetMainPlayer()->GetPet(activePetGuid);
	if(pPet)
	{
		pPet->SetFather( GetRegion() );
		GetRegion()->AddObject( pPet );
	}

    pPlayer->PetEnterRegion();
    m_lLoadingDelayTime = (long)timeGetTime();
    
    Log4c::Trace(ROOT_MODULE,"QuestEnterRegion(...) End!");
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
    sprintf_s<1024>(szConfig,"render\n{\n	fullscreen = %d\n	hvp = false\n	texturemode = %d\n	2dlinear = %s\n	3dlinear = %s\n	font\n	{\n		style1 = font/12.font\n		style2 = font/1.font\n	}\n	window\n	{\n		x = 0\n		y = 0\n		width = %d\n		height = %d\n		autostyle = false\n		autocenter = false\n	}\n	fullscreen\n	{\n		backbufferfmt = %d\n	}\n}\n bUseVSync = %s\n",
        //GetSetup()->bFullscreen,
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

    //	utility::File file;
    //	file.Open("data/weather/envfog/400");

    //TCHAR *temp = new TCHAR[file.GetLength() + 1];
    //memcpy(temp, file.GetData(), sizeof(TCHAR) * file.GetLength());
    //temp[file.GetLength()] = 0;
    //GetWeatherManager()->ModifyEnvFogParameter(temp);
    ////GetWeatherManager()->EnableEnvFog();
    //delete [] temp;
    //file.Close();
    //GetWeatherManager()->SetEnvFogColor(0xffffffff);

    // 渲染阴影需要的环境变量
    //InitRenderStage();

    // 初始化平面阴影数据
    //m_FlatShadow.Create();
    //m_theShadowMapping.Create();
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

    //m_FlatShadow.Destroy();
    //m_theShadowMapping.Destroy();
    //销毁引擎
    Water::CManager::GetInstance()->Destroy();
    particle::Manager::GetInstance()->Destroy();
    render::Interface::GetInstance()->Destroy();
    console::Manager::GetInstance()->Destroy();

    CBitmapX::ReMoveAllBitmap();
    Log4c::Trace(ROOT_MODULE,"Release AllBitmap OK!");
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
    sprintf_s<256>(szTemp,"FPS: %3.3d %.6d",iFps,m_iCurrentRenderTriangleCount);
    OutPutText(SCREEN_WIDTH/2,2,szTemp,lstrlen(szTemp),D3DCOLOR_ARGB(255,255,255,255));

}

////////////////////////////send msg to login server
void CGame::C2L_CDKey_Send(const char *cdkey,const vector<BYTE>& password,const char*wsName)
{
    if(m_pNetClient==NULL)
        return;
    extern long g_lVersion;

    char strVersion[RESOURCE_VERSION_SIZE]="";
    sprintf_s<RESOURCE_VERSION_SIZE>(strVersion,"%s",CClientResource::GetInstance()->GetVersion().c_str());
    char strcdkey[CDKEY_SIZE]="";
    sprintf_s<CDKEY_SIZE>(strcdkey,"%s",cdkey);
    char strwsName[WS_NAME_SIZE]="";
    sprintf_s<WS_NAME_SIZE>(strwsName,"%s",wsName);


    CMessage msg( MSG_C2L_LOG_QUESTCDKEY );	
    msg.Add( g_lVersion );	// 游戏版本
    msg.AddEx((void*)&strVersion,RESOURCE_VERSION_SIZE);//资源版本
    msg.AddEx((void*)&strcdkey,CDKEY_SIZE);             //账号
    msg.AddEx((void*)&password[0],(long)password.size());     //密码
    msg.AddEx( (void*)&strwsName,WS_NAME_SIZE);         //ws服务器名字
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
void CGame::C2L_AddRole_Send(const char *strName,char nOccupation, char nSex, BYTE lHead, BYTE lFace, BYTE lCountry,BYTE lConstellation,BYTE bRandCountry)
{
    if(m_pNetClient==NULL)
        return;
    CMessage msg(MSG_C2L_LOG_QUESTCREATE);
    msg.Add(const_cast<char*>(strName));
    msg.Add(nOccupation);
    msg.Add(nSex);
    msg.Add(lHead);
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

    //if(nStringLength <= nLength)			// 沒有超長，則自己進行繪製
    //{
    //	OutPutText(x, y, pString, nStringLength, Color);
    //	return S_OK;
    //}

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

        OutPutText(x, nOffsetY, (char*)pBuffer, lstrlen((char*)pBuffer), Color);
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
                                
                                string tempTxt = pString;
                                tempTxt=tempTxt.substr(0,iCurLength);
                                fitemp.iX = x+GetFontWidth(tempTxt.c_str());
                                fitemp.iY = y+iLine*nLineSpace-8;

                                fiDrawFace.push_back(fitemp);
                                iCurLength+=3;
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
        } 
        else
        {
            if( bCheckSize )
            {
                pDest--;
                *pDest = 0;
                pSrc--;
                bCheckSize = false;
            }

            iCurLength = 0;

            strtemp = (char*)pBuffer;
            strTextLine.push_back(strtemp);
            pDest = pBuffer;
            memset(pBuffer, 0, sizeof(pBuffer));
            iLine++;
        }
    }
    strtemp = (char*)pBuffer;
    strTextLine.push_back(strtemp);
    memset(pBuffer, 0, sizeof(pBuffer));

    long lFontWidth = GetGame()->GetFontWidth(pString);
    if(lFontWidth > nLength)
        nZoneMaxWidth = nLength;
    else
        nZoneMaxWidth = lFontWidth;
    //绘制背景框
    if (bkBarColor)
    {
        GetGame()->DrawSolidQuadRect(x, y, x+nZoneMaxWidth, y+nZoneHeight+nLineSpace, bkBarColor);
    }
    if(m_pbitmapShapeTalk)
    {
        int nW = m_pbitmapShapeTalk->GetWidth()-nZoneMaxWidth;
        if (nW < 0)
            nW = abs(nW);
        m_pbitmapShapeTalk->Draw(x,y,bkBarColor | 0x00ffffff);
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
    for (int iTextCount = 0, nOffsetY = y; iTextCount< (int)strTextLine.size(); iTextCount++, nOffsetY += nLineSpace)
    {
        int alpha = (Color) >> 24;
        D3DCOLOR future= D3DCOLOR_ARGB(alpha,255,255,255);
        OutPutText(x-1,(int)nOffsetY,strTextLine[iTextCount].c_str(),(int)strTextLine[iTextCount].length(),Color);
        OutPutText(x+1,(int)nOffsetY,strTextLine[iTextCount].c_str(),(int)strTextLine[iTextCount].length(),Color);
        OutPutText(x,(int)nOffsetY-1,strTextLine[iTextCount].c_str(),(int)strTextLine[iTextCount].length(),Color);
        OutPutText(x,(int)nOffsetY+1,strTextLine[iTextCount].c_str(),(int)strTextLine[iTextCount].length(),Color);
        OutPutText(x,(int)nOffsetY,strTextLine[iTextCount].c_str(),(int)strTextLine[iTextCount].length(),future);
    }

    //绘制表情
    for (size_t ifiCount = 0; ifiCount<fiDrawFace.size(); ifiCount++)
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

    //if(nStringLength <= nLength)			// 沒有超長，則自己進行繪製
    //{
    //	return 1;
    //}

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

void CGame::OutPutText(int iX,int iY,const char *pszString,size_t iLen,D3DCOLOR color, int iStyleIndex,
                       const float space_extra, const float x_scale, const float y_scale)
{
    if (pszString)
    {
        GetInst(UIManager).OutPutText(iX,iY,pszString,color,iStyleIndex,
                                      space_extra, x_scale, y_scale);
    }
}

void CGame::RenderTTF_Text(int ix, int iy, const WCHAR* strText, long fontType,long lFontSize,DWORD color, long spacebetween)
{
    //m_pFreeType->RenderText(ix,iy,strText,(FreeTypeFont::FT_FontType)fontType,color,(FreeTypeFont::FT_FontSize)lFontSize,spacebetween);
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

long CGame::GetFontWidth(const char * Text, int iStyleIndex, float x_scale)
{
   return GetInst(UIManager).GetFontWidth(Text,iStyleIndex,x_scale);
}

long CGame::GetFontHeight(int iStyleIndex , float y_scale)
{
    return GetInst(UIManager).GetFontHeight(iStyleIndex,y_scale);
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
    pGameGraphics2D->SetUIModelRendState();
    pGameGraphics2D->DrawLine(iStartX, iStartY, iEndX, iEndY, color);
}

void CGame::DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
{
    GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
    pGameGraphics2D->SetUIModelRendState();
    pGameGraphics2D->DrawSolidQuad(iX, iY, iWidth, iHeight, color);
}

void CGame::DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
    GameGraphics2D *pGameGraphics2D = GameGraphics2D::GetInstance();
    pGameGraphics2D->SetUIModelRendState();
    pGameGraphics2D->DrawSolidQuad(iX1, iY1, iX2 - iX1, iY2 - iY1, color);
}

void CGame::DrawWireQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
{
    //render::Interface *pInterface = render::Interface::GetInstance();
    //render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
    //pLayer2D->DrawWireQuad(iX, iY, iWidth, iHeight, color);
}

void CGame::DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
    //render::Interface *pInterface = render::Interface::GetInstance();
    //render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
    //pLayer2D->DrawWireQuadRect(iX1, iY1, iX2, iY2, color);
}

void CGame::DrawToolTip(int nX,int nY,int nWidth,int nHeight,const char* pStr,D3DCOLOR textColor)
{
    DrawSolidQuad(nX,nY,nWidth,nHeight,D3DCOLOR_ARGB(150,0,0,0));
    OutPutText(nX+5,nY+3,pStr,strlen(pStr),textColor);
}

// 刷新设置
void CGame::UpdateSetup()
{
    if (GetGame()->GetSetup()->lViewRange < 25)
        GetGame()->GetSetup()->lViewRange = 25;

    if (GetRegion())
        GetRegion()->GetCamera()->SetFarClipPlane((float)GetGame()->GetSetup()->lViewRange);
    //	if(GetWeatherManager())
    //		GetWeatherManager()->SetEnvFogDistance((float)GetGame()->GetSetup()->lViewRange/2, (float)GetGame()->GetSetup()->lViewRange);

    /*if(GetGame()->GetAudioList())
    {
    GetGame()->GetAudioList()->SetSoundEnabled(m_Setup.bSound);
    GetGame()->GetAudioList()->SetMusicEnabled(m_Setup.bMusic);
    GetGame()->GetAudioList()->SetSoundVolume(m_Setup.fSoundVolume);
    GetGame()->GetAudioList()->SetMusicVolume(m_Setup.fMusicVolume);
    }*/
}

//// 初始化阴影渲染所需状态
//void CGame::InitRenderStage(void)
//{
//	render::Interface *pInterface = render::Interface::GetInstance();
//	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
//
//	//检查当前模式是否支持阴影渲染
//	if (pInterface->CheckDepthStencilFormat(pInterface->GetInfo()->d3dpp.BackBufferFormat) == D3DFMT_D16)
//	{
//		//不支持
//		m_bSupportStencils = FALSE;
//		return;
//	}
//
//	m_bSupportStencils = TRUE;
//
//	//setup basic render states
//	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
//
//	//set up texture information
//	pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
//	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//	pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//
//	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//
//	//configure stencil modes - saves re-setting them every frame..
//	pDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE );
//	pDevice->SetRenderState( D3DRS_STENCILZFAIL,	D3DSTENCILOP_KEEP );
//	pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP );
//	pDevice->SetRenderState( D3DRS_STENCILREF,		0x1 );
//	pDevice->SetRenderState( D3DRS_STENCILMASK,		0xffffffff );
//	pDevice->SetRenderState( D3DRS_STENCILWRITEMASK,0xffffffff );
//	pDevice->SetRenderState( D3DRS_CCW_STENCILFUNC,	D3DCMP_ALWAYS );
//	pDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL,D3DSTENCILOP_KEEP );
//	pDevice->SetRenderState( D3DRS_CCW_STENCILFAIL,	D3DSTENCILOP_KEEP );
//	pDevice->SetRenderState( D3DRS_CCW_STENCILPASS,	D3DSTENCILOP_DECR );
//
//	D3DCAPS9 caps;
//	pDevice->GetDeviceCaps(&caps);
//	m_b2SidedStencils = ( ( caps.StencilCaps & D3DSTENCILCAPS_TWOSIDED ) != 0 );
//
//}
//
//// 计算体积阴影
//void CGame::CalculateShadowVolumes(CShape *pShape,D3DXVECTOR3 &vLighDirectional)
//{
//	//if (!m_bSupportStencils)
//	//	return;
//	//if(pShape == NULL)
//	//	return;
//
//	//// the light same as using in modellist.cpp
//	//static D3DXVECTOR3 vecLight;
//	//static D3DXMATRIX matWorld, mat;
//
//	//m_dwNumOfVol = 0;
//
//	//CModelList *pModelList = GetGame()->GetModelList();
//	//CModelList::tagModel *pModel = pModelList->GetModel(pShape->GetType(),pShape->GetGraphicsID());
//
//	//// 遍历mesh中的每个组件
//	//std::map<DWORD, GameModel::Group*>  *mapGroupArray = pModel->pModel->GetGroupArray();
//	//for(std::map<DWORD,GameModel::Group*>::iterator itGroup = mapGroupArray->begin() ; itGroup != mapGroupArray->end(); itGroup++)
//	//{
//	//	GameModel::Group *pGroup = itGroup->second;
//	//	if (pGroup->IsVisible())
//	//	{
//	//		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
//	//		if (pPart)
//	//		{
//	//			GameModel::Group::Part::tagRenderMesh *pRMA = pPart->GetRenderMeshArray();
//	//			GameModel::Group::Part::Level *pLevel = pPart->GetSelectLevel();
//	//			for(int n = 0; n < pLevel->GetRMIndexCount();n++)
//	//			{
//	//				DWORD dwIndex = pLevel->GetRMIndex(n);
//
//	//				model::Buffer *pBuffer = pRMA[dwIndex].pBuffer;
//
//	//				m_pVol[m_dwNumOfVol].Reset();
//	//				m_pVol[m_dwNumOfVol].SetWorldMatrix(NULL);		// input NULL to use identity matrix
//	//				if (m_pVol[m_dwNumOfVol].BuildFromMesh(pBuffer, vLighDirectional))
//	//				{
//	//					++m_dwNumOfVol;
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//}
//void CGame::SetShadowMappingLight(D3DXVECTOR3 vL)
//{
//	//m_theShadowMapping.SetLight(vL);
//}
//void CGame::BeginShadowMapping()
//{
//	//m_theShadowMapping.BeginShadowMapping();
//}
//void CGame::RenderShadowMapping(CShape *pShape)
//{
//	//m_theShadowMapping.Render(pShape);
//}
//void CGame::EndShadowMapping()
//{
//	//m_theShadowMapping.EndShadowMapping();
//}
//void CGame::SetShadowMappingBlend()
//{
//	//m_theShadowMapping.SetFloorShadowShaderTex(1);
//}
//void CGame::AddShadowVolumeMesh(CShape *pShape,D3DXVECTOR3 &vLighDirectional)
//{
//	//if (!m_bSupportStencils)
//	//	return ;
//	//if(pShape == NULL)
//	//	return;
//
//	//static D3DXVECTOR3 vecLight;
//	//static D3DXMATRIX matWorld, mat;
//
//	//CModelList *pModelList = GetGame()->GetModelList();
//	//CModelList::tagModel *pModel = pModelList->GetModel(pShape->GetType(),pShape->GetGraphicsID());
//
//	//// 遍历mesh中的每个组件
//	//std::map<DWORD, GameModel::Group*>  *mapGroupArray = pModel->pModel->GetGroupArray();
//	//for(std::map<DWORD,GameModel::Group*>::iterator itGroup = mapGroupArray->begin() ; itGroup != mapGroupArray->end(); itGroup++)
//	//{
//	//	GameModel::Group *pGroup = itGroup->second;
//	//	if (pGroup->IsVisible())
//	//	{
//	//		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
//	//		if (pPart)
//	//		{
//	//			GameModel::Group::Part::tagRenderMesh *pRMA = pPart->GetRenderMeshArray();
//	//			GameModel::Group::Part::Level *pLevel = pPart->GetSelectLevel();
//	//			for(DWORD n = 0; n < (DWORD)pLevel->GetRMIndexCount();n++)
//	//			{
//	//				DWORD dwIndex = pLevel->GetRMIndex(n);
//
//	//				model::Buffer *pBuffer = pRMA[dwIndex].pBuffer;
//
//	//				m_pVol[m_dwNumOfVol].Reset();
//	//				m_pVol[m_dwNumOfVol].SetWorldMatrix(NULL);		// input NULL to use identity matrix
//	//				if (m_pVol[m_dwNumOfVol].BuildFromMesh(pBuffer, vLighDirectional))
//	//				{
//	//					++m_dwNumOfVol;
//	//				}
//	//			}
//	//		}
//	//	}
//	//}
//}
//
//// 渲染体积阴影
//void CGame::RenderShadow()
//{
//if (!m_bSupportStencils)
//	return;
//
//	render::Interface *pInterface = render::Interface::GetInstance();
//	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();
//
//	if(m_dwNumOfVol <= 0)
//	return;
//
//	// Disable z-buffer writes (note: z-testing still occurs), and enable the
//	// stencil-buffer
//	pDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
//	pDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
//
//	// Dont bother with interpolating color
//	pDevice->SetRenderState( D3DRS_SHADEMODE,     D3DSHADE_FLAT );
//
//	// Set up stencil compare fuction, reference value, and masks.
//	// Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true.
//	// Note: since we set up the stencil-test to always pass, the STENCILFAIL
//	// renderstate is really not needed.
//	pDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
//	pDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
//	pDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
//
//	// If ztest passes, inc/decrement stencil buffer value
//	pDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
//	pDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
//	pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
//	pDevice->SetRenderState( D3DRS_STENCILPASS,      D3DSTENCILOP_INCR );
//
//	// Make sure that no pixels get drawn to the frame buffer
//	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
//	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
//
//	pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
//
//	if(m_b2SidedStencils)
//{
//		// With 2-sided stencil, we can avoid rendering twice:
//		pDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
//		pDevice->SetRenderState( D3DRS_CCW_STENCILFUNC,  D3DCMP_ALWAYS );
//		pDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
//		pDevice->SetRenderState( D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP );
//		pDevice->SetRenderState( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_DECR );
//
//		pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
//
//		// Draw both sides of shadow volume in stencil/z only
//		for(DWORD i = 0; i < m_dwNumOfVol; ++i)
//	{
//			m_pVol[i].Render();
//		}
//
//		pDevice->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, FALSE );
//	}
//	else
//		{
//		// Draw front-side of shadow volume in stencil/z only
//		for(DWORD i = 0; i < m_dwNumOfVol; ++i)
//			{
//			m_pVol[i].Render();
//		}
//
//		// Now reverse cull order so back sides of shadow volume are written.
//		pDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
//
//		// Decrement stencil buffer value
//		pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );
//
//		// Draw back-side of shadow volume in stencil/z only
//		for(DWORD i = 0; i < m_dwNumOfVol; ++i)
//				{
//			m_pVol[i].Render();
//				}
//			}
//
//	// Restore render states
//	pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
//	pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
//	pDevice->SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
//	pDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
//	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//
//	// Set renderstates (disable z-buffering, enable stencil, disable fog, and
//	// turn on alphablending)
//	pDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
//	pDevice->SetRenderState( D3DRS_STENCILENABLE,    TRUE );
//	pDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
//	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//	pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//	pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//
//	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//
//	// Only write where stencil val >= 1 (count indicates # of shadows that
//	// overlap that pixel)
//	pDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
//	pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
//	pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
//
//	// Draw a big, gray square
//	FLOAT sx = (FLOAT)pInterface->GetWndWidth();
//	FLOAT sy = (FLOAT)pInterface->GetWndHeight();
//	m_pShadowVertex[0].p = D3DXVECTOR4(  0, sy, 0.0f, 1.0f );
//	m_pShadowVertex[1].p = D3DXVECTOR4(  0,  0, 0.0f, 1.0f );
//	m_pShadowVertex[2].p = D3DXVECTOR4( sx, sy, 0.0f, 1.0f );
//	m_pShadowVertex[3].p = D3DXVECTOR4( sx,  0, 0.0f, 1.0f );
//	m_pShadowVertex[0].color = 0x20000000;
//	m_pShadowVertex[1].color = 0x20000000;
//	m_pShadowVertex[2].color = 0x20000000;
//	m_pShadowVertex[3].color = 0x20000000;
//
//	pDevice->SetFVF( SHADOWVERTEX::FVF );
//	if (FAILED(pDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_pShadowVertex, sizeof(SHADOWVERTEX))))
//	{
//		MessageBox(NULL,"DP失败","错误",MB_OK);
//		}
//	// Restore render states
//	pDevice->SetRenderState( D3DRS_ZENABLE,          TRUE );
//	pDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
//	pDevice->SetRenderState( D3DRS_FOGENABLE,        TRUE );
//	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
//	}
//
//// 渲染阴影体积
//void CGame::RenderShadowVolume()
//{
//if (!m_bSupportStencils)
//	return ;
//	for(DWORD i = 0; i < m_dwNumOfVol; i++)
//	{
//		m_pVol[i].Render();
//	}
//}
//
//DWORD CGame::GetMeshCount(GameModel *pModel)
//{
//	DWORD count = 0;
//
//	std::map<DWORD, GameModel::Group*>  *mapGroupArray = pModel->GetGroupArray();
//for(std::map<DWORD,GameModel::Group*>::iterator itGroup = mapGroupArray->begin() ; itGroup != mapGroupArray->end(); itGroup++)
//{
//	GameModel::Group *pGroup = itGroup->second;
//	if (pGroup->IsVisible())
//	{
//		GameModel::Group::Part * pPart = pGroup->GetSelectPart();
//		if (pPart)
//		{
//				++count;
//			}
//				}
//			}
//
//	return count;
//		}
//
//void CGame::RenderFlatShadow(CShape* pShape)
//{
//	//if(pShape)
//	//m_FlatShadow.Render(pShape->GetPosY(),pShape->GetHeight(),pShape->GetPosX(),pShape->GetFigure(CShape::DIR_RIGHT)*2+1);
//	if (m_pRegion)
//	{
//		GameMap *pGameMap = m_pRegion->GetGameMap();
//		if(pShape)
//			m_FlatShadow.Render(pShape->GetPosY(),pShape->GetHeight(),pShape->GetPosX(),pShape->GetFigure(CShape::DIR_RIGHT)*2+1, pGameMap);
//		float posx = pShape->GetPosY();
//		float posz = pShape->GetPosX();
//	}
//}



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



////////////////////////////////////////////////////////////////////////////
////	需要显示头发的列表
////////////////////////////////////////////////////////////////////////////
//
//// 读取带头盔要显示头发的列表
//void CGame::LoadNeedDisplayHairList(const char* filename)
//{
//    CRFile* prfile = rfOpen(filename);
//    if(prfile==NULL)
//        return;
//    stringstream stream;
//    prfile->ReadToStream(stream);
//    rfClose(prfile);
//
//    m_listHelmHair.clear();
//    tagHelmHair st;
//    while(ReadTo(stream, "*"))
//    {
//        int tmp1, tmp2;
//        stream >> tmp1 >> tmp2;
//        st.lOccupation = tmp1;
//        st.lEquipID = tmp2;
//        m_listHelmHair.push_back(st);
//    }	
//}
//
//// 查询是否需要显示头发
//bool CGame::IsNeedDisplayHair(BYTE lOccupation, BYTE lEquipID)
//{
//    for (list<tagHelmHair>::iterator it = m_listHelmHair.begin(); it!=m_listHelmHair.end(); it++)
//    {
//        if (it->lOccupation == lOccupation && it->lEquipID == lEquipID)
//        {
//            return true;
//        }
//    }
//    return false;
//}

////////////////////////////////////////////////////////////////////////////
////	需要显示刀光的列表
////////////////////////////////////////////////////////////////////////////
//
//// 读取带头盔要显示头发的列表
//void CGame::LoadRenderTrailList(const char* filename)
//{
//
//    CRFile* prfile = rfOpen(filename);
//    if(prfile==NULL)
//        return;
//    stringstream stream;
//    prfile->ReadToStream(stream);
//    rfClose(prfile);
//
//
//    m_listRenderTrail.clear();
//    tagRenderTrail st;
//    while(ReadTo(stream, "*"))
//    {
//        int tmp1, tmp2, tmp3;
//        stream >> tmp1 >> tmp2 >> tmp3;
//        st.lOccupation = tmp1;
//        st.wAction = tmp2;
//        st.wWeaponType = tmp3;
//
//        char c1, c2, c3, c4;
//        stream >> c1 >> c2 >> c3 >> c4;
//        st.dwTrailName = MAKEFOURCC(c1, c2, c3, c4);
//        stream >> c1 >> c2 >> c3 >> c4;
//        st.dwTrailColorIndex = MAKEFOURCC(c1, c2, c3, c4);
//
//        m_listRenderTrail.push_back(st);
//    }
//}

//// 查询是否需要显示刀光
//CGame::tagRenderTrail* CGame::GetRenderTrail(BYTE lOccupation, WORD wAction, WORD wWeaponType)
//{
//    for (list<tagRenderTrail>::iterator it = m_listRenderTrail.begin(); it!=m_listRenderTrail.end(); it++)
//    {
//        if (it->lOccupation == lOccupation && it->wAction == wAction && it->wWeaponType == wWeaponType)
//        {
//            return &(*it);
//        }
//    }
//    return NULL;
//}

////////////////////////////////////////////////////////////////////////////
////	猎手武器和特效的对应列表
////////////////////////////////////////////////////////////////////////////
//void CGame::LoadArcheryEffect(const char* filename)
//{
//    CRFile* prfile = rfOpen(filename);
//    if(prfile==NULL)
//        return;
//    stringstream stream;
//    prfile->ReadToStream(stream);
//    rfClose(prfile);
//
//
//    m_listArcheryEffect.clear();
//    tagArcheryEffect st;
//    while(ReadTo(stream, "#"))
//    {
//        stream >> st.dwWeaponGroundID >> st.dwArcheryEffect;
//        m_listArcheryEffect.push_back(st);
//    }
//}
//
//DWORD CGame::GetArcheryEffect(DWORD dwWeaponGroundID)
//{
//    for (list<tagArcheryEffect>::iterator it = m_listArcheryEffect.begin(); it!=m_listArcheryEffect.end(); it++)
//    {
//        if (it->dwWeaponGroundID == dwWeaponGroundID)
//        {
//            return it->dwArcheryEffect;
//        }
//    }
//    return 0;
//}

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
    size_t nPos = strTemptFolder.find_last_of("\\");
    strTempt.erase(0,nPos+1);
    if(strTempt=="*.*")
        strTemptFolder.erase(nPos,strTemptFolder.length()- nPos);
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
void CGame::UpdateMoveShapePosition()
{
    if (GetRegion()==NULL)
        return;

    DWORD dwCurTime = GetCurTickCount();

    list<CBaseObject*> *pObjectList = GetRegion()->GetObjectList();
    for (itBaseObject it = pObjectList->begin(); it!=pObjectList->end(); it++)
    {
        CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(*it);
        if (pMoveShape)
        {
            if (pMoveShape->GetAction()==CShape::ACT_WALK || pMoveShape->GetAction()==CShape::ACT_RUN)
            {
                if ( pMoveShape->GetPosX() == pMoveShape->GetDestX() && pMoveShape->GetPosY() == pMoveShape->GetDestY() )
                {
                    continue;
                }

                // 计算当前的坐标值
                /*float fx = pMoveShape->GetDestX() - pMoveShape->GetBeginX();
                float fy = pMoveShape->GetDestY() - pMoveShape->GetBeginY();
                float fDistance = sqrt(fx*fx + fy*fy);
                float fMoveOneCeleTime = pMoveShape->GetSpeed() * fDistance;

                fx = pMoveShape->GetBeginX() + fx * static_cast<float>(dwCurTime - pMoveShape->GetBeginMoveTime()) / fMoveOneCeleTime;
                fy = pMoveShape->GetBeginY() + fy * static_cast<float>(dwCurTime - pMoveShape->GetBeginMoveTime()) / fMoveOneCeleTime;

                pMoveShape->SetPosXY(fx, fy);*/
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
//	更新Shape的坐标
//////////////////////////////////////////////////////////////////////////
void CGame::UpdateMoveShapePositionEx()
{
    if (GetRegion()==NULL)
        return;

    DWORD dwCurTime = GetCurTickCount();
    list<CBaseObject*> *pObjectList = GetRegion()->GetObjectList();
    for (itBaseObject it = pObjectList->begin(); it!=pObjectList->end(); it++)
    {
        CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(*it);
        if (pMoveShape)
        {
            // 处于平滑移位状态
            if (pMoveShape->GetIsSlipState())
            {
                /*DWORD elapseTime = dwCurTime - pMoveShape->GetSlipLastTime();
                pMoveShape->SetSlipLastTime(dwCurTime);
                float fx = pMoveShape->GetPosX() + elapseTime * pMoveShape->GetMoveOffsetX();
                float fy = pMoveShape->GetPosY() + elapseTime * pMoveShape->GetMoveOffsetY();
                pMoveShape->SetDestXY(fx, fy);
                pMoveShape->SetPosXY(fx, fy);*/
                // 计算移动的距离
                float fDistance = pMoveShape->GetSlipSpeed()*static_cast<float>(dwCurTime - pMoveShape->GetSlipLastTime());
                //计算出坐标偏移
                float fDir = pMoveShape->GetSlipDir();
                float fx = pMoveShape->GetPosX() + fDistance * cosf(fDir);
                float fy = pMoveShape->GetPosY() + fDistance * sinf(fDir);
                
                pMoveShape->SetPosXY(fx, fy);
                pMoveShape->SetDestXY(pMoveShape->GetPosX(), pMoveShape->GetPosY());
                pMoveShape->SetSlipLastTime(dwCurTime);
            }
            // 正常的行走和跑步
            else if (pMoveShape->GetAction()==CShape::ACT_WALK || pMoveShape->GetAction()==CShape::ACT_RUN)
            {
                DWORD elapseTime = dwCurTime - pMoveShape->GetLastUpdatePosTime();
                pMoveShape->SetLastUpdatePosTime(dwCurTime);
                float fx = pMoveShape->GetPosX() + elapseTime * pMoveShape->GetMoveOffsetX();
                float fy = pMoveShape->GetPosY() + elapseTime * pMoveShape->GetMoveOffsetY();
                pMoveShape->SetPosXY(fx, fy);
            }
        }
    }
}



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

    size_t iPos1 = strTemptStrInfo.find("[[[");
    size_t iPos2 = strTemptStrInfo.find("]]]");
    while(iPos1 >= 0 && iPos2 > 0 && iPos2 > iPos1)
    {
        string str1 = strTemptStrInfo.substr(0,iPos1);
        stTopStrInfo TopStrInfo1 = {D3DCOLOR_ARGB(255,255,48,0),str1};
        strInfos.push_back(TopStrInfo1);

        string str2 = strTemptStrInfo.substr(iPos1+3,iPos2-iPos1-3);
        stTopStrInfo TopStrInfo2 = {D3DCOLOR_ARGB(255,0,255,255),str2};
        strInfos.push_back(TopStrInfo2);
        strTemptStrInfo = strTemptStrInfo.substr(iPos2+3,strTemptStrInfo.length() - iPos2-3);

        iPos1 = (int)strTemptStrInfo.find("[[[");
        iPos2 = (int)strTemptStrInfo.find("]]]");
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
        int lTotalWidth = m_stCurTopInfo.lTotalLength;
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
                            m_stCurTopInfo.lTotalLength += GetFontWidth((*itInfo).strContend.c_str());
                        }

                        //加上分割符号
                        stTopStrInfo strTemptInfo = {D3DCOLOR_ARGB(255,0,255,255),"     "};
                        m_stCurTopInfo.listCurInfo.push_back(strTemptInfo);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strTemptInfo.strContend.c_str());

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
                    if(TopInfo.lParam - 5000> (long)dwTime)
                    {
                        list<stTopStrInfo>::iterator itInfo = TopInfo.strInfo.begin();
                        for(;itInfo != TopInfo.strInfo.end();itInfo++)
                        {
                            m_stCurTopInfo.listCurInfo.push_back((*itInfo));
                            m_stCurTopInfo.lTotalLength += GetFontWidth((*itInfo).strContend.c_str());
                        }

                        long lDifTime = TopInfo.lParam - dwTime;
                        lDifTime = lDifTime/1000;
                        long lMinute = lDifTime/60;
                        long lSecond = lDifTime%60;
                        char strTempt[256]="";
                        sprintf_s<256>(strTempt,"%d",lMinute);

                        //分数字
                        stTopStrInfo strInfoMinNum = {D3DCOLOR_ARGB(255,0,255,255),strTempt};
                        m_stCurTopInfo.listCurInfo.push_back(strInfoMinNum);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strInfoMinNum.strContend.c_str());
                        //"分"
                        stTopStrInfo strInfoMin = {D3DCOLOR_ARGB(255,255,48,0),"分"};
                        m_stCurTopInfo.listCurInfo.push_back(strInfoMin);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strInfoMin.strContend.c_str());

                        sprintf_s<256>(strTempt,"%d",lSecond);
                        //秒数字
                        stTopStrInfo strInfoSecondNum = {D3DCOLOR_ARGB(255,0,255,255),strTempt};
                        m_stCurTopInfo.listCurInfo.push_back(strInfoSecondNum);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strInfoSecondNum.strContend.c_str());
                        //"秒"
                        stTopStrInfo strInfoSecond = {D3DCOLOR_ARGB(255,255,48,0),"秒"};
                        m_stCurTopInfo.listCurInfo.push_back(strInfoSecond);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strInfoSecond.strContend.c_str());

                        //加上分割符号
                        stTopStrInfo strTemptInfo = {D3DCOLOR_ARGB(255,0,255,255),"     "};
                        m_stCurTopInfo.listCurInfo.push_back(strTemptInfo);
                        m_stCurTopInfo.lTotalLength += GetFontWidth(strTemptInfo.strContend.c_str());

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
            GetGame()->OutPutText(iDrawX, iY, TemptStrInfo.strContend.c_str(), 
				(int)TemptStrInfo.strContend.length(),
                TemptStrInfo.d3dColor);
            iDrawX++;
            GetGame()->OutPutText(iDrawX,iY, TemptStrInfo.strContend.c_str(),
				(int)TemptStrInfo.strContend.length(),
                TemptStrInfo.d3dColor);

            iY = TemptTopInfo.lY;
            iX = iX+GetFontWidth(TemptStrInfo.strContend.c_str());
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
        switch(m_lGameState)
        {
        case GS_LOGIN:		// 游戲登陸狀態
            GetInst(UIManager).CloseAllPage("MessageBox");
            GetInst(LoginPage).Open();
            break;
        case GS_PWD_CARD:   //密保卡
          
            break;
        case GS_SELECTWS:   //选择WS
            GetInst(UIManager).CloseAllPage("MessageBox");
            GetInst(WorldServerPage).Open();
			/////////////////////////////////////////////////
			// zhaohang  2010/3/17 
			//GetInst(GoodsViewerPage).Open();
			/////////////////////////////////////////////////
           
            break;
        case GS_SELECT:		// 游戏选择角色状态
            GetInst(UIManager).CloseAllPage("MessageBox");
            GetInst(SelectRolePage).Open();
            break;
        case GS_CREATE:		// 创建角色组状态
            GetInst(UIManager).CloseAllPage("MessageBox");
            GetInst(CreateRolePage).Open();
            break;
        case GS_WAITING:	// 等待进入状态
            break;
        case GS_BLACK:		// 黑屏状态
            break;
        case GS_RUN:
            {
                // 游戏运行
                GetInst(MainBarPage).Open();
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
    if(lIndex < (long)m_vecMonsterQuestEffect.size())
        return m_vecMonsterQuestEffect[lIndex];
    return -1;
}

// 根据传入的特效ID来得到对应的优先级
long CGame::GetMonsterQuestEffectLevel(long lEffectID)
{
    for (size_t i=0; i<m_vecMonsterQuestEffect.size(); ++i)
    {
        if(m_vecMonsterQuestEffect[i] == lEffectID)
            return (long)i;
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
    if(!::RegSetValueExA(hk,strPath.c_str(),0,REG_SZ,(LPBYTE)(LPCSTR)KeyValue.c_str(),(DWORD)KeyValue.size()+1)==ERROR_SUCCESS)
    {
        return false;
    }

    //获得注册表键值
    //if(::RegQueryValueExA(hk,"Client_Path",0,NULL,(BYTE*)Vals,&lenIt)==ERROR_SUCCESS)
    //{}
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
    //CLoginPageEx * pLoginPage = GetGame()->GetCGuiEx()->GetLoginPageEx();
    //CMessageBoxPageEx * pMsgBox = GetGame()->GetCGuiEx()->GetMessageBoxPageEx();

    m_pNetClient = new CClient;
    m_pNetClient->Start(m_pDBAllocator,20,NF_LS_Client);

    hr = m_pNetClient->Connect( logIP,logPort);	// 连接服务器
    if(!hr)	// 连接失败
    {
        //pMsgBox->Open(pLoginPage,"连接服务器失败，请稍后重试！",CMessageBoxPageEx::MSG_OK,true);
        g_LoginTime=0;
        SDOLogOut();
        ShowLoginDialog();
    }
    else
    {
        if(m_pNetClient!=NULL)
        {
            CMessage::SetClient(m_pNetClient);
            extern long g_lVersion;
            char strVersion[RESOURCE_VERSION_SIZE]="";
            sprintf_s<RESOURCE_VERSION_SIZE>(strVersion,"%s",CClientResource::GetInstance()->GetVersion().c_str());
            char strwsName[WS_NAME_SIZE]="";
            sprintf_s<WS_NAME_SIZE>(strwsName,"%s",wsName.c_str());
            char strSonId[PT_TOKEN_SIZE]="";
            sprintf_s<PT_TOKEN_SIZE>(strSonId,"%s",SonId);

            CMessage msg( MSG_C2L_LOG_SEND_PT_TOKEN );	
            msg.Add( g_lVersion );	// 游戏版本
            msg.AddEx((void*)&strVersion,RESOURCE_VERSION_SIZE);
            msg.AddEx((void*)&strwsName,WS_NAME_SIZE);
            //msg.Add((long)m_lReGameCode);
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
#pragma warning(push)
#pragma warning(disable:4129)
        StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\../\../\../\../\bin\\GameWidget.dll", strExePath );
#pragma warning (pop)
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

                // 注意AppInfo类型所有字段必须填
                AppInfo tmpAppInfo = {
                    sizeof(AppInfo),       // 结构体大小，方便扩展
                    101,                  // 接入应用ID，从开发者网站中申请
                    L"星尘传说",           // 应用名称
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
                        Log4c::Trace(ROOT_MODULE,"加载IGW组件成功！");
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
                //*pbNoFurtherProcessing = true;// 不让自身再处理消息 			
                return SDOA_OK; 
            } 
        }


        if ( (UINT)SDOA_WM_CLIENT_RUN == message )
        {
            // 此刻可以开始登录
            if(m_bIsSadoLog)
            {
                GetGame()->ShowLoginDialog();
                //GetGame()->ShowLoginDialog();
                //IGUIRoot::GetGUIInstance()->SetFocusComp(NULL);

                //       // GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(false);
                //        GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(false);
                //  GetGame()->GetCGuiEx()->GetLoginPageEx()->SetSimoLoginEnble(false);
                //  GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
                //  GetGame()->GetSetup()->bupdateonlostfoucs = false;
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
            //CPlayer *pPlayer=GetGame()->GetMainPlayer();
            //if(pPlayer==NULL) return false;

            const LPCSTR LszSonId = pLoginResult->szSessionId;
            const char* SonId = LszSonId;

            Log4c::Trace(ROOT_MODULE,"Login IGW OK!");
            //PutDebugString(SonId);
            //PutDebugString(pLoginResult->szSndaid);

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
#pragma warning (push)
#pragma warning(disable:4312)
            LoginMessage* pCurLoginMsg = (LoginMessage*)(nReserved);
#pragma warning (pop)
            if (pCurLoginMsg->dwSize >= sizeof(LoginMessage))
            { // 兼容性处理 
                // 修改错误提示对话框标题
                SysReAllocString(pCurLoginMsg->pbstrTitle, L"[盛大通行证用户登陆]");

                // 此时返回FALSE则使用自定义提示

                // 此时返回TRUE则使用自定义的提示
                //isSDOLogin = TRUE;
                //::MessageBox(0, *pCurLoginMsg->pbstrContent, *pCurLoginMsg->pbstrTitle, MB_OK);
            }
        }
    }
    else if(SDOA_ERRORCODE_CANCEL == nErrorCode)
    {
        GetGame()->SetSadoLogPageOpen(false);
    }
    char Text[50];
    sprintf_s<50>(Text,"登陆圈圈错误：%d",nErrorCode);
    Log4c::Trace(ROOT_MODULE,Text);
    //GetGame()->SetIsSadoLog((bool)g_isSDOLogin);
    g_bLogin= g_isSDOLogin;
    return g_isSDOLogin; // 常规逻辑，登录成功则关闭登录对话框
} 

//查询余额操作的回调函数
void CALLBACK QueryCallback(int nRetCode, int nUserData)
{ // 异步查询的回调
    if (g_SDOAClientService)
    {
        if (nRetCode != SDOA_OK)
        {
            //g_GameAudioSet.GetStatic( IDC_BALANCELABEL )->SetText( L"查询失败" );
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,"查询失败"); 
            BSTR bstrRetKey = SysAllocString(L"descMsg");
            BSTR bstrRetValue = NULL;
            g_SDOAClientService->GetValue(bstrRetKey, &bstrRetValue);
            _bstr_t strRet = bstrRetValue;
            //LPCWSTR szRet = strRet;
            //string strValue = szRet; 
            char *szRet = strRet;
            //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,szRet); 

            WCHAR strErrotTitle[100] = {0};
            StringCchPrintfW( strErrotTitle, 100, L"错误信息(%d)", nRetCode);

            //			::MessageBox(0, szRet, strErrotTitle, MB_OK);	 
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
                size_t nPos=-1;
                nPos=(int)strPay.find_first_of(":")+1;
                string strPayEx=strPay.substr(nPos,strPay.size()-nPos);
                DWORD dwYuanbo= (DWORD)atoi(strPayEx.c_str());
                pPlayer->SetYuanBao(dwYuanbo);
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
            //GetGame()->GetCGuiEx()->GetLoginPageEx()->GetbtnSdoPos(lPosX,lPosY);
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
            m_bIsShowSdo = true;
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
                Log4c::Trace(ROOT_MODULE,"SetValue orderId Error! \n");
            }
            SysFreeString(bstrOrderIdValue);
            SysFreeString(bstrOrderIdName);

            // 商户编码 (根据实际情况填写)
            BSTR bstrSpIdName = SysAllocString(L"spId");
            BSTR bstrSpIdValue = SysAllocString(L"101");
            if (g_SDOAClientService->SetValue(bstrSpIdName, bstrSpIdValue) == SDOA_FALSE)
            {
                Log4c::Trace(ROOT_MODULE,"SetValue spId Error! \n");
            }
            SysFreeString(bstrSpIdName);
            SysFreeString(bstrSpIdValue);


            // 货币类型
            BSTR bstrPayTypeName = SysAllocString(L"payType");
            BSTR bstrPayTypeValue = SysAllocString(L"1");
            if (g_SDOAClientService->SetValue(bstrPayTypeName, bstrPayTypeValue) == SDOA_FALSE)
            {
                Log4c::Trace(ROOT_MODULE,"SetValue payType Error! \n");
            }
            SysFreeString(bstrPayTypeName);
            SysFreeString(bstrPayTypeValue);
            // 命令类型
            BSTR bstrCmdTypeName = SysAllocString(L"cmdType");
            BSTR bstrCmdTypeValue = SysAllocString(L"1");
            if (g_SDOAClientService->SetValue(bstrCmdTypeName, bstrCmdTypeValue) == SDOA_FALSE)
            {
                Log4c::Trace(ROOT_MODULE,"SetValue cmdType Error! \n");
            }
            SysFreeString(bstrCmdTypeName);
            SysFreeString(bstrCmdTypeValue);
            // 调用时间
            BSTR bstrCallTimeName = SysAllocString(L"callTime");
            char str[100]="";
            WCHAR strTime[50] = {0};
            SYSTEMTIME sys;
            GetLocalTime( &sys );
            sprintf_s<100>( str,"%.4d-%.2d-%.2d %.2d:%.2d:%.2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond );
            StringCchPrintfW( strTime, 50, L"%.4d-%.2d-%.2d %.2d:%.2d:%.2d", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond );
            Log4c::Trace(ROOT_MODULE,str);
            BSTR bstrCallTimeValue = SysAllocString(strTime);
            if (g_SDOAClientService->SetValue(bstrCallTimeName, bstrCallTimeValue) == SDOA_FALSE)
            {
                Log4c::Warn(ROOT_MODULE,"SetValue callTime Error! \n");
            }
            SysFreeString(bstrCallTimeName);
            SysFreeString(bstrCallTimeValue);
            // 附加指令
            BSTR bstrIndicationName = SysAllocString(L"indication");
            BSTR bstrIndicationValue = SysAllocString(L"");
            if (g_SDOAClientService->SetValue(bstrIndicationName, bstrIndicationValue) == SDOA_FALSE)
            {
                Log4c::Warn(ROOT_MODULE,"SetValue indication Error! \n");
            }
            SysFreeString(bstrIndicationName);
            SysFreeString(bstrIndicationValue);


            BSTR bstrServiceName = SysAllocString(L"UserBalance");
            if ( g_SDOAClientService->AsyncQuery(bstrServiceName, QueryCallback, 0) == SDOA_OK )
            {
                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,"开始查询"); 
            }else{
                //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,"查询失败"); 
            }
            //g_GameAudioSet.GetStatic( IDC_BALANCELABEL )->SetText( L"开始查询" );
            //else
            //g_GameAudioSet.GetStatic( IDC_BALANCELABEL )->SetText( L"查询失败" );
            SysFreeString(bstrServiceName);

        }
    }
    else
    {
        //g_GameAudioSet.GetStatic( IDC_BALANCELABEL )->SetText( L"登录成功后才可用！" );
        //GetGame()->GetCGuiEx()->GetMainBarPageEx()->AddText(eNOTIFYPOS_LEFT_BOTTOM,"登录成功后才可用！"); 
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
        int nLen = (int)strlen(szName)+1;
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
        int nLen = (int)strlen(UrId)+1;
        int nwLen = MultiByteToWideChar(CP_ACP, 0, UrId, nLen, NULL, 0);

        MultiByteToWideChar(CP_ACP, 0, UrId, nLen, strUrlTitle, nwLen);
        if (g_SDOAApp->ShowPaymentDialog(strUrlTitle) == SDOA_OK)
        {
            //打开页面成功
            Log4c::Trace(ROOT_MODULE,"打开网站：%s\n",UrlId);
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
        //g_isSDOLogin = false;
        //g_bLogin = false;

        //GetGame()->GetCGuiEx()->GetLoginPageEx()->SetLogePageEnable(true);
        //GetGame()->GetCGuiEx()->GetLoginPageEx()->SetNamePassVisble(true);
        //GetGame()->GetCGuiEx()->GetLoginPageEx()->SetPageFocus(); 
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
        stream >> strTemp >> bIsSdo;
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

    //if (wsName[0]!=0)
    //{
    SetGameState(GS_BLACK);
    Sleep(1000);
    //}

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
        sprintf_s<CDKEY_SIZE>(strSndName,"%s",strSnd.c_str());
    }else{
        string strSnd = GetGame()->GetGameCDKEY();
        sprintf_s<CDKEY_SIZE>(strSndName,"%s",strSnd.c_str());
    }
    //发送请求角色列表的消息
  //  Sleep(1000);
    CMessage msg( MSG_C2L_LOGIN_AND_QUEST_PLAYERBASELIST );	
    msg.AddEx((void*)&strSndName,CDKEY_SIZE);
    for(int i=0;i<4;i++)
    {
        DWORD dwReCode = m_lReGameCode[i];
        msg.Add((DWORD)dwReCode);
    }
    msg.Send(m_pNetClient);
}
//打开网页
void CGame::OpenUrlEx(const char* UrlId)
{
    if (g_SDOAApp)
    {    
        WCHAR strUrlTitle[2048] = {0};
        const char *UrId = UrlId;
        int nLen = (int)strlen(UrId)+1;
        int nwLen = MultiByteToWideChar(CP_ACP, 0, UrId, nLen, NULL, 0);

        MultiByteToWideChar(CP_ACP, 0, UrId, nLen, strUrlTitle, nwLen);
        if (g_SDOAApp->OpenWindow(L"HTML", L"UrlLink", strUrlTitle, 
            0, 0, 800, 700, L"xul=WebBrowser.xml;sf=true;cp=true;ap=center") == SDOA_OK) 
        {
            int nOk=1;
            //打开页面成功
            Log4c::Trace(ROOT_MODULE,"打开网站：%s\n",UrlId);
        }
    }
}