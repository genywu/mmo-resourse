#include "Precompiled.h"

// 全局游戏对象
extern CT_Game* g_pGame = NULL;

CT_Game::CT_Game(HWND hWnd)
:m_pScene           (NULL)						// 背景模块指针
,m_pSquare              (NULL)                  // 方块模块指针
,m_pRender              (NULL)                  // 渲染模块指针
,m_pSound               (NULL)                  // 声音模块指针
,m_GameState            (GAME_STATE_NO_INIT)    // 游戏状态未初始化的
,m_hWnd                 (hWnd)                  // 初始化游戏主窗口句柄
,m_uTimerId             (1)                     // 初始化游戏定时器ID为0
,m_dwGameLevel          (0)                     // 初始化游戏速度级别为无效
,m_dwGameSpeed          (INVALID_UID)           // 游戏速度
,m_pLuaManager          (NULL)                  // 初始化脚本管理器指针为空
,m_IsAutoBack           (FALSE)
,m_IsAutoFront          (FALSE)
,m_IsDrawShadow         (TRUE)
,m_uIntegral            (0)
,m_dwGameTollGateLevel  (0)
,m_dwEachLevelIntegral  (0)
,m_bGameNeedReset       (FALSE)
,m_IsWin                (FALSE)
,m_dwBackMusicCount     (0)
,m_dwCurrentMusicIndex  (0)
{
    ;
}

CT_Game::~CT_Game(VOID)
{
    this->ReleaseGame();
}

// 初始化游戏
BOOL CT_Game::InitGame(VOID)
{
    // 布尔类型返回值
    BOOL bRet = TRUE;
    // 定义脚本类型返回值
    LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);

    AssertEx(AL_CATAST, m_GameState == GAME_STATE_NO_INIT, _T("游戏状态错误,调用错误"));

    // 申请背景对象
    m_pScene = NEW CT_Scene;
    AssertEx(AL_CATAST, m_pScene, _T("内存不足"));

    // 申请方块对象
    m_pSquare = NEW CT_Square;
    AssertEx(AL_CATAST, m_pSquare, _T("内存不足"));

    // 申请渲染对象
    m_pRender = NEW CRender(m_hWnd, BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT, m_pScene, m_pSquare);
    if (m_pRender)
    {
        m_lpD3DFrameFont = CMs2DEngine::s_CreateD3D9Font(20, FW_BOLD, 1, FALSE, _T("宋体"));
        RECT rc = {0, 0, BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT};
        m_pRender->EnableFrame(m_lpD3DFrameFont, TRUE, rc);
    }
    else
    {
        AssertEx(AL_CATAST, m_pRender, _T("内存不足"));
    }

    // 申请声音对象
    m_pSound = NEW CMsSound(GAME_SOUND_INDEX_COUNT);
    AssertEx(AL_CATAST, m_pSound, _T("内存不足"));
    bRet = m_pSound->Init(FALSE);

    // 申请脚本管理器
    m_pLuaManager = NEW CMsLuaManager;
    if (m_pLuaManager)
    {
        m_pLuaManager->Init();
        // 打开脚本配置文件
        TCHAR szFileNameBuff[MAX_PATH] = {0};
        CreateFullPath(szFileNameBuff, _T(SCRIPT_FILE_NAME));
        m_pLuaManager->LoadLuaFile(szFileNameBuff);

        // 加载升级需要的分数
        m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("GetEachLevelNeedIntegral"), NULL);
        m_dwEachLevelIntegral = (DWORD)LuaFunRetVal.m_lLuaFunRet;
    }
    else
    {
        AssertEx(AL_CATAST, m_pLuaManager, _T("内存不足"));
    }

    // 加载游戏声音
    this->LoadGameSound();

    // 将游戏状态改为停止
    m_GameState = GAME_STATE_STOP;

    // 开始渲染
    //m__pRender1 = NEW CTestRender(m_hWnd, BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT);
    //m_lpD3DFrameFont = CMs2DEngine::s_CreateD3D9Font(20, FW_BOLD, 1, FALSE, _T("宋体"));
    //RECT rc = {0, 0, BACKSCENE_MAX_WIDTH, BACKSCENE_MAX_HEIGHT};
    //m__pRender1->EnableFrame(m_lpD3DFrameFont, TRUE, rc);

    ////m__pRender2 = NEW CTestRender(m_hWnd, 460, 331);
    ////m_lpD3DFrameFont = CMs2DEngine::s_CreateD3D9Font(20, FW_BOLD, 1, FALSE, _T("宋体"));

    //m__pRender1->Run();
    //m__pRender2->Run();

    m_pRender->Run();

    // 返回成功
    return TRUE;
}

// 释放游戏
BOOL CT_Game::ReleaseGame(VOID)
{
    CMs2DEngine::s_DeleteD3D9();
    CMs2DEngine::s_Release();

    // 停止游戏
    this->EndGame();

    if (m_pRender)
    {
        m_pRender->Stop(TRUE);
    }
    //if (m__pRender1)
    //{
    //    m__pRender1->Stop(TRUE);
    //}
    //SAFE_DELETE(m__pRender1);

    //if (m__pRender2)
    //{
    //    m__pRender2->Stop(TRUE);
    //}
    //SAFE_DELETE(m__pRender2);

    // 安全删除脚本模块
    SAFE_DELETE(m_pLuaManager);

    // 安全删除声音模块
    SAFE_DELETE(m_pSound);

    // 安全删除渲染模块
    SAFE_DELETE(m_pRender);

    // 安全删除方块模块
    SAFE_DELETE(m_pSquare);

    // 安全删除背景模块
    SAFE_DELETE(m_pScene);

    // 将游戏状态修改为未初始化
    m_GameState = GAME_STATE_NO_INIT;

    // 返回成功
    return TRUE;
}

// 开始游戏
BOOL CT_Game::StartGame(VOID)
{
    // 布尔类型返回值
    BOOL bRet = TRUE;
    // 定义脚本类型返回值
    LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);

    // 检查游戏是否已经初始化成功
    if (m_GameState != GAME_STATE_STOP)
    {
        return TRUE;
    }

    // 设置游戏关卡等级为1级
    m_dwGameTollGateLevel = 1;
    // 设置游戏等级为1级
    m_dwGameLevel = 1;

    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("GetTollGateSize"), _T("%d"), m_dwGameTollGateLevel);
    DWORD SceneSize = (DWORD)LuaFunRetVal.m_lLuaFunRet;
    this->Reset(HIWORD(SceneSize), LOWORD(SceneSize));

    // 创建一个新方块
    m_pSquare->CreateShape(HIWORD(SceneSize));

    // 将标记置成游戏中
    m_GameState = GAME_STATE_PLAYING;

    // 播放声音
    bRet = m_pSound->Play(GAME_SOUND_READYGO, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("播放声音失败,函数失败"));

    //// 重置整个窗口
    //::InvalidateRect(m_hWnd, NULL, FALSE);

    // 返回成功
    return TRUE;
}

// 暂停游戏
BOOL CT_Game::PauseGame(VOID)
{
    // 如果游戏运行中
    if (m_GameState == GAME_STATE_PLAYING)
    {

        // 设置音乐暂停
        m_pSound->Pause(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);

        // 设置游戏暂停标记
        m_GameState = GAME_STATE_PAUSE;
    }
    else
    {
        AssertEx(AL_CATAST, FALSE, _T("游戏状态异常,逻辑错误"));
    }
    return TRUE;
}

// 继续游戏
BOOL CT_Game::ContinueGame(VOID)
{
    // 如果游戏暂停中
    if (m_GameState == GAME_STATE_PAUSE)
    {
        // 继续游戏
        m_pSound->Continue(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);

        // 设置游戏运行标记
        m_GameState = GAME_STATE_PLAYING;
    }
    else
    {
        AssertEx(AL_CATAST, FALSE, _T("游戏状态异常,逻辑错误"));
    }
    return TRUE;
}

// 获取游戏状态
GAME_STATE CT_Game::GetGameState(VOID)
{
    // 返回游戏状态
    return m_GameState;
}

// 结束游戏
BOOL CT_Game::EndGame(VOID)
{
    // 如果游戏未开启
    if (m_GameState == GAME_STATE_NO_INIT ||
        m_GameState == GAME_STATE_STOP)
    {
        // 返回成功
        return TRUE;
    }
    // 绘制游戏结束时的最后画面
    //::InvalidateRect(m_hWnd, NULL, FALSE);
    //m_pDlg->UpDataDlg();

    // 将游戏状态置为停止
    m_GameState = GAME_STATE_STOP;

    // 停止播放音乐
    m_pSound->Stop(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);

    // 关闭定时心跳
    ::KillTimer(m_hWnd, m_uTimerId);
    //m_pDlg->KillTimer(m_uTimerId);

    // 返回成功
    return TRUE; 
}

//// 画背景
//BOOL CT_Game::DrawBack(VOID)
//{
//    // 绘制背景
//    m_pRender->DrawBackScene();
//    return m_pBuffImage->EndDraw();
//}

//// 绘制游戏
//BOOL CT_Game::Render(VOID)
//{
//    // 布尔类型返回值
//    BOOL bRet = TRUE;
//    USE_PARAM(bRet);
//    //UPDATA_LOCALTIME;
//    //FILETIME FileTime;
//    //INT64* lpn64Time = (INT64*)&FileTime;
//    //::SystemTimeToFileTime(&g_LocalTime, &FileTime);
//    //static INT64 OldTime = *lpn64Time;
//    //static INT64 CurTime = *lpn64Time;
//    //FLOAT Time  = (CurTime  - OldTime)/1000.0f/10000.f;
//    //OldTime = CurTime;
//    //CurTime = *lpn64Time;
//    //FLOAT FPS = 1.0f/Time;
//    //TCHAR szBuff[FWORD_NUMBER_SIZE] = {0};
//    //_stprintf_s(szBuff,FWORD_NUMBER_SIZE,_T("%3.7f"),FPS);
//
//   
//
//    //DWORD dwBackSceneWidth,dwBackSceneHeight;
//    //m_pScene->GetBackSceneSize(dwBackSceneWidth, dwBackSceneHeight);
//
//    //m_pBuffImage->BeginDraw(FALSE);
//
//    // 重置背景
//    m_pRender->ResetBackScene();
//    
//    switch (m_GameState)
//    {
//    //游戏开始状态
//    case GAME_STATE_PLAYING:
//        {
//            // 绘制背景
//            bRet = m_pRender->DrawBackScene();
//            AssertEx(AL_NORMAL, bRet, _T("DrawBackScene返回失败,函数失败"));
//
//            // 绘制文本
//            bRet = m_pRender->DrawText();
//            AssertEx(AL_NORMAL, bRet, _T("DrawText返回失败,函数失败"));
//
//            // 绘制后续方块
//            bRet = m_pRender->DrawNextShape();
//            AssertEx(AL_NORMAL, bRet, _T("DrawNextShape返回失败,函数失败"));
//
//            // 计算方块阴影位置
//            this->CalculateShadowPos();
//
//            // 绘制方块
//            bRet = m_pRender->DrawActivityShape(m_IsDrawShadow);
//            AssertEx(AL_NORMAL, bRet, _T("DrawActivityShape返回失败,函数失败"));
//        }break;
//    //// 游戏准备开始状态
//    //case GAME_STATE_START:
//    //    {
//    //        // 绘制文本
//    //        m_wTime += Time;
//    //        bRet = m_pRender->DrawText();
//    //        AssertEx(bRet, _T("DrawText返回失败,函数失败"));
//    //        if(m_wTime < 0.5f)
//    //        {
//    //            // 绘制文本
//    //            RECT rc = {280,250,480,350};
//    //            bRet = m_pRender->DrawCenterText(_T("READY!!!"), &rc, RGB(255,255,0));
//    //            AssertEx(bRet, _T("DrawCenterText返回失败,函数失败"));
//    //        }else
//    //        {
//    //            // 绘制文本
//    //            RECT rc = {280,250,480,350};
//    //            bRet = m_pRender->DrawCenterText(_T("GO!!!"), &rc, RGB(255,255,0));
//    //            AssertEx(bRet, _T("DrawCenterText返回失败,函数失败"));
//    //        }
//
//
//    //        if ( !m_pSound->IsPlaying(GAME_SOUND_READYGO))
//    //        {
//    //            m_GameState = GAME_STATE_PLAYING;
//    //            m_wTime = 0;
//    //            this->PlayBackMusic();
//    //            this->ResetIntegral();
//    //        }
//    //    }break;
//    //游戏暂停状态
//    case GAME_STATE_PAUSE:
//        {
//            // 绘制文本
//            bRet = m_pRender->DrawText();
//            AssertEx(AL_NORMAL, bRet, _T("DrawText返回失败,函数失败"));
//            // 绘制文本
//            RECT rc = {280,250,480,350};
//            bRet = m_pRender->DrawCenterText(_T("游戏暂停"), &rc, RGB(255,255,0));
//            AssertEx(AL_NORMAL, bRet, _T("DrawCenterText返回失败,函数失败"));
//           
//        }break;
//    }
//    // 绘制文本
//    //RECT rc1 = {100,100,480,350};
//    //bRet = m_pRender->DrawCenterText(szBuff, &rc1, RGB(255,255,0));
//
//    //m_pRender->Render();
//    // 返回成功
//    return TRUE;
//}

// 增加游戏音量
BOOL CT_Game::AddGameVolume(VOID)
{
    return FALSE;
}

// 降低游戏音量
BOOL CT_Game::SubGameVolume(VOID)
{
    return FALSE;
}

// 增加音乐音量
BOOL CT_Game::AddMusicVolume(VOID)
{
    // 检查游戏状态是否为游戏中
    if (m_GameState == GAME_STATE_PLAYING)
    {
        // 增加音量
        m_pSound->AddVolume(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);
    }

    // 返回成功
    return TRUE;
}

// 降低音乐音量
BOOL CT_Game::SubMusicVolume(VOID)
{
    // 检查游戏状态是否为游戏中
    if (m_GameState == GAME_STATE_PLAYING)
    {
        // 降低音量
        m_pSound->SubVolume(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);
    }

    // 返回成功
    return TRUE;
}

// 游戏心跳(定时触发的游戏时间片处理)
BOOL CT_Game::GameHeartbeat(VOID)
{
    if (m_GameState != GAME_STATE_PLAYING)
    {
        return TRUE;
    }

    if (m_IsAutoBack)
    {
        static WORD wBackSceneMetafileIndex = 0;
        wBackSceneMetafileIndex++;
        wBackSceneMetafileIndex %= 1;
        m_pScene->SetBackMetafileIndex(wBackSceneMetafileIndex);
    }

    UINT uRadix = R_BYTE_A(1, 100);
    if (1 == uRadix)
    {
        this->PlayBackMusic();
    }

    if (m_IsAutoFront)
    {
        static WORD wFrontSceneMetafileIndex = 0;
        wFrontSceneMetafileIndex++;
        wFrontSceneMetafileIndex %= 9;
        m_pScene->SetFrontMetafileIndex(wFrontSceneMetafileIndex);
    }
    this->MoveShape(VK_DOWN);

    return TRUE;
}

// 检查形状是否可以到这来
BOOL CT_Game::CheckShapeCanHere(NPOS SquarePosX, NPOS SquarePosY, WORD wShapeIndex)
{
    CT_ShapeFile* pShapeFile = m_pSquare->GetShapeFile();
    BYTE* pShape = pShapeFile->GetShapeByIndex(wShapeIndex);
    SHORT VoidCountByLeft = pShapeFile->GetShapeUsedSquareByLeft(wShapeIndex);
    SHORT VoidCountByRight= pShapeFile->GetShapeUsedSquareByRight(wShapeIndex);
    SHORT VoidCountByDown = pShapeFile->GetShapeUsedSquareByDown(wShapeIndex);
    INT nBackSceneWidth,nBackSceneHeight;
    m_pScene->GetBackSceneSize((DWORD&)nBackSceneWidth, (DWORD&)nBackSceneHeight);
    BACK_SCENE_BYTE* pSceneElement = m_pScene->GetBackSceneBuff();

    // 循环遍历纵行的方块
    for (INT Y = 0; Y < SHAPE_MAX_HEIGHT ; Y++)
    {
        // 循环遍历指定行的横排方块
        for (INT X = 0; X < SHAPE_MAX_WIDTH; X++)
        {
            if (((X+SquarePosX+VoidCountByLeft) < 0) ||
                ((Y+SquarePosY-VoidCountByDown) >= nBackSceneHeight) ||
                ((X+SquarePosX-VoidCountByRight) >= nBackSceneWidth))
            {
                return FALSE;
            }
            if ((pSceneElement[(Y+SquarePosY) * nBackSceneWidth + (X+SquarePosX)].m_Square) &&
                pShape[Y * SHAPE_MAX_WIDTH + X])
            {
                return FALSE;
            }
        }
    }

    // 返回成功
    return TRUE;
}

// 移动方块
BOOL CT_Game::MoveShape(UINT uWay)
{
    if (m_GameState != GAME_STATE_PLAYING)
    {
        return TRUE;
    }

    // 布尔返回值
    BOOL bRet = TRUE;

    bRet = this->IsGameOver();
    if (bRet)
    {
        return TRUE;
    }

    NPOS SquarePosX, SquarePosY;
    m_pSquare->GetShapePos(SquarePosX, SquarePosY);
    WORD wShapeIndex        = m_pSquare->GetCurrentShapeIndex();
    BYTE VoidCountByLeft    = m_pSquare->GetVoidCountByLeft();
    BYTE VoidCountByRight   = m_pSquare->GetVoidCountByRight();
    BYTE VoidCountByDown     = m_pSquare->GetVoidCountByDown();
    //BACK_SCENE_BYTE* pSceneElement = m_pScene->GetBackSceneBuff();
    DWORD   dwBackSceneWidth, dwBackSceneHeight;
    m_pScene->GetBackSceneSize(dwBackSceneWidth, dwBackSceneHeight);

    switch (uWay)
    {
    case VK_LEFT:
        {
            if (SquarePosX > (-VoidCountByLeft))
            {
                if (this->CheckShapeCanHere(SquarePosX-1, SquarePosY, wShapeIndex))
                {
                    m_pSquare->SetShapePos(SquarePosX-1, SquarePosY);
                }
            }
        }break;
    case VK_RIGHT:
        {
            // 判断这个方块可以移动到哪个位置
            if ((SquarePosX + (SHAPE_MAX_WIDTH - VoidCountByRight)) < (INT)dwBackSceneWidth)
            {
                if (this->CheckShapeCanHere(SquarePosX+1, SquarePosY, wShapeIndex))
                {
                    m_pSquare->SetShapePos(SquarePosX+1, SquarePosY);
                }
            }
        }break;
    case VK_DOWN:
        {
            // 如果方块还没到最下面
            if ((SquarePosY + (SHAPE_MAX_HEIGHT - VoidCountByDown)) < (INT)dwBackSceneHeight)
            {
                if (this->CheckShapeCanHere(SquarePosX, SquarePosY+1, wShapeIndex))
                {
                    m_pSquare->SetShapePos(SquarePosX, SquarePosY+1);
                }
                // 方块下落中碰撞
                else
                {
                    // 固定形状到背景
                    this->FixationShape();
                }
            }
            // 方块到底了
            else if (SquarePosY + (SHAPE_MAX_HEIGHT - VoidCountByDown) == (INT)dwBackSceneHeight)
            {
                // 固定形状到背景
                this->FixationShape();
            }
        }break;
    case VK_UP:
        {
            m_pSquare->Distortion(dwBackSceneWidth, dwBackSceneHeight);
            bRet = m_pSound->Play(GAME_SOUND_DISTORTION, FALSE);
            AssertEx(AL_SHOW_ONLY, bRet, _T("播放声音失败,函数失败"));
        }break;
    case WM_USER:
    case VK_SPACE:
        {
            for (;;)
            {
                // 如果方块还没到最下面
                if ((SquarePosY + (SHAPE_MAX_HEIGHT - VoidCountByDown)) < (INT)dwBackSceneHeight)
                {
                    if (this->CheckShapeCanHere(SquarePosX, SquarePosY+1, wShapeIndex))
                    {
                        SquarePosY += 1;
                    }
                    // 方块下落中碰撞
                    else
                    {
                        if (WM_USER == uWay)
                        {
                            m_pSquare->SetShapeShadowPos(SquarePosX, SquarePosY);
                            return TRUE;
                        }
                        else if (VK_SPACE == uWay)
                        {
                            // 固定形状到背景
                            m_pSquare->SetShapePos(SquarePosX, SquarePosY);
                            this->FixationShape();
                        }
                        break;
                    }
                }
                // 方块到底了
                else if (SquarePosY + (SHAPE_MAX_HEIGHT - VoidCountByDown) == (INT)dwBackSceneHeight)
                {
                    if (WM_USER == uWay)
                    {
                        m_pSquare->SetShapeShadowPos(SquarePosX, SquarePosY);
                        return TRUE;
                    }
                    else if (VK_SPACE == uWay)
                    {
                        // 固定形状到背景
                        m_pSquare->SetShapePos(SquarePosX, SquarePosY);
                        this->FixationShape();
                    }
                    break;
                }
            }
        }break;
    default:
        {
            // 返回成功
            return FALSE;
        }
    }

    // 重新绘制
    //::InvalidateRect(m_hWnd, NULL, FALSE);

    // 返回成功
    return FALSE;
}

// 固定方块
BOOL CT_Game::FixationShape(VOID)
{
    // 布尔返回值
    BOOL bRet = TRUE;

    if (m_GameState != GAME_STATE_PLAYING)
    {
        return TRUE;
    }

    NPOS SquarePosX, SquarePosY;
    m_pSquare->GetShapePos(SquarePosX, SquarePosY);
    WORD wShapeIndex        = m_pSquare->GetCurrentShapeIndex();
    SHORT VoidCountByLeft   = m_pSquare->GetVoidCountByLeft();
    SHORT VoidCountByRight  = m_pSquare->GetVoidCountByRight();
    SHORT VoidCountByDown   = m_pSquare->GetVoidCountByDown();
    BYTE* pShape           = m_pSquare->GetShapeFile()->GetShapeByIndex(wShapeIndex);
    WORD wMetafileIndex     = m_pSquare->GetCurrentMetafileIndex(); 
    BACK_SCENE_BYTE* pSceneElement = m_pScene->GetBackSceneBuff();
    INT nBackSceneWidth,nBackSceneHeight;
    m_pScene->GetBackSceneSize((DWORD&)nBackSceneWidth, (DWORD&)nBackSceneHeight);

    m_pSound->Play(GAME_SOUND_FIXATIONSOUND_0, FALSE);

    // 循环遍历纵行的方块
    for (INT Y = 0; Y < SHAPE_MAX_HEIGHT ; Y++)
    {
        // 循环遍历指定行的横排方块
        for (INT X = 0; X < SHAPE_MAX_WIDTH; X++)
        {
            if (((X+SquarePosX+VoidCountByLeft) < 0) ||
                ((Y+SquarePosY-VoidCountByDown) >= nBackSceneHeight) ||
                ((X+SquarePosX-VoidCountByRight) >= nBackSceneWidth))
            {
                AssertEx(AL_CATAST, FALSE, _T("场景与方块数据异常,错误逻辑"));
            }
            if ((pSceneElement[(Y+SquarePosY) * nBackSceneWidth + (X+SquarePosX)].m_Square) &&
                pShape[Y * SHAPE_MAX_WIDTH + X])
            {
                bRet = this->IsGameOver();
                AssertEx(AL_CATAST, bRet, _T("游戏到此没有结束,错误逻辑"));
                return TRUE;
            }
            else
            {
                // 拷贝形状到背景
                if (pShape[Y * SHAPE_MAX_WIDTH + X])
                {
                    pSceneElement[(Y+SquarePosY) * nBackSceneWidth + (X+SquarePosX)].m_Square = pShape[Y * SHAPE_MAX_WIDTH + X];
                    pSceneElement[(Y+SquarePosY) * nBackSceneWidth + (X+SquarePosX)].m_MetafileIndex = wMetafileIndex;
                }
            }
        }
    }

    // 清除当前形状
    m_pSquare->Clear();

    this->AddIntegral(m_dwGameLevel);

    // 清理满行
    this->RemoveFullLine();

    if (m_bGameNeedReset)
    {
        SIZE_T SceneSize = 0;
        if (m_IsWin)
        {
            LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);
            m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("GetTollGateSize"), _T("%d"), 11);
            SceneSize = LuaFunRetVal.m_lLuaFunRet;
        }
        else
        {
            LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);
            m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("GetTollGateSize"), _T("%d"), m_dwGameTollGateLevel);
            SceneSize = LuaFunRetVal.m_lLuaFunRet;
        }
        this->Reset(HIWORD(SceneSize), LOWORD(SceneSize));
        m_bGameNeedReset = FALSE;
    }

    // 创建新的形状
    m_pScene->GetBackSceneSize((DWORD&)nBackSceneWidth, (DWORD&)nBackSceneHeight);
    m_pSquare->CreateShape((DWORD)nBackSceneWidth);
    this->SetTimer(m_dwGameSpeed);
    this->IsGameOver();

    // 返回成功
    return TRUE;
}

#pragma warning(push)
#pragma warning(disable : 6385)
// 消除满行的方块
BOOL CT_Game::RemoveFullLine(VOID)
{
    DWORD X,Y;
    DWORD dwBackSceneWidth, dwBackSceneHeight;
    m_pScene->GetBackSceneSize(dwBackSceneWidth, dwBackSceneHeight);
    BACK_SCENE_BYTE* pSceneElement = m_pScene->GetBackSceneBuff();

    UINT uClearCount = 0;
    // 循环遍历纵行的方块
    for (Y = 0; Y <  dwBackSceneHeight; Y++)
    {
        // 循环遍历指定行的所有横排方块
        for (X = 0; X < dwBackSceneWidth; X++)
        {
            // 如果横排方块中有空格
            if (pSceneElement[Y * dwBackSceneWidth + X].m_Square == NO_SQUARE)
            {
                // 跳出当前横排,继续遍历下一纵行
                break;
            }
        }
        // 如果横排已经成功遍历到最后一个,说明当前横行已满,进行消行
        if (X == dwBackSceneWidth)
        {
            uClearCount++;
            m_pSound->Play(GAME_SOUND_BLASTSOUND_1 + (uClearCount - 1), FALSE);
            m_pSound->SetVolume(GAME_SOUND_BLASTSOUND_1 + (uClearCount - 1), uClearCount * GAME_NORMAL_VOLUME);
            this->RemoveEffect(&pSceneElement[Y * dwBackSceneWidth], dwBackSceneWidth);
            // 将数组头开始到消除行之间的数据拷贝到第二个
            memcpy(&pSceneElement[dwBackSceneWidth], pSceneElement, Y*dwBackSceneWidth*sizeof(BACK_SCENE_BYTE));
            // 将第一行初始化为空
            ZeroMemory(pSceneElement, dwBackSceneWidth * sizeof(BACK_SCENE_BYTE));
            for (DWORD i = 0; i < dwBackSceneWidth; i++)
            {
                pSceneElement[i].m_MetafileIndex = m_pScene->GetBackMetafileIndex();
            }
            this->AddIntegral(dwBackSceneWidth);
        }
    }

    switch (uClearCount)
    {
    case 0x01:
        {
            this->AddIntegral(dwBackSceneWidth);
        }break;
    case 0x02:
        {
            this->AddIntegral(dwBackSceneWidth*3);
            m_pSound->Play(GAME_SOUND_HURRAY01, FALSE);
            m_pSound->SetVolume(GAME_SOUND_HURRAY01, VOLUME_MAX_VALUE);
        }break;
    case 0x03:
        {
            this->AddIntegral(dwBackSceneWidth*5);
            m_pSound->Play(GAME_SOUND_HURRAY02, FALSE);
            m_pSound->SetVolume(GAME_SOUND_HURRAY02, VOLUME_MAX_VALUE);
        }break;
    case 0x04:
        {
            this->AddIntegral(dwBackSceneWidth*7);
            m_pSound->Play(GAME_SOUND_HURRAY03, FALSE);
            m_pSound->SetVolume(GAME_SOUND_HURRAY03, VOLUME_MAX_VALUE);
        }break;
    }
    return TRUE;
}
#pragma warning(pop)

// 消除满行的方块
BOOL CT_Game::RemoveEffect(BACK_SCENE_BYTE* pSceneElement, DWORD Count)
{
    DWORD dwRemoveSpeed;
    if (Count > 11)
    {
        dwRemoveSpeed = 3;
    }
    else if (Count > 5)
    {
        dwRemoveSpeed = 2;
    }
    else
    {
        dwRemoveSpeed = 1;
    }
    for (DWORD i = 0; i < Count; i++)
    {
        pSceneElement[i].m_MetafileIndex = 0x01;
        if (i % dwRemoveSpeed == 0 || i == Count)
        {
			::SleepEx(5, TRUE);
        }
    }

    for (DWORD i = 0; i < Count; i++)
    {
        if (0 == i)
        {
            pSceneElement[i].m_MetafileIndex = 0x0A;
        }
        else if (1 == i)
        {
            pSceneElement[i - 1].m_MetafileIndex = 0x02;
            pSceneElement[i].m_MetafileIndex = 0x0A;
        }
        else
        {
            pSceneElement[i-2].m_MetafileIndex = 0x01;
            pSceneElement[i-1].m_MetafileIndex = 0x02;
            pSceneElement[i].m_MetafileIndex = 0x0A;
        }
        ::SleepEx(20,TRUE);
    }

    return TRUE;
}

// 计算阴影位置
VOID CT_Game::CalculateShadowPos(VOID)
{
    NPOS SquarePosX, SquarePosY;
    m_pSquare->GetShapePos(SquarePosX, SquarePosY);
    WORD wShapeIndex = m_pSquare->GetCurrentShapeIndex();
    CT_ShapeFile* pShapeFile = m_pSquare->GetShapeFile();
    BYTE VoidCountByDown = pShapeFile->GetShapeUsedSquareByDown(wShapeIndex);
    DWORD dwBackSceneWidth,dwBackSceneHeight;
    m_pScene->GetBackSceneSize(dwBackSceneWidth, dwBackSceneHeight);

    while (m_IsDrawShadow)
    {
        // 如果方块还没到最下面
        if (((SquarePosY + (SHAPE_MAX_HEIGHT - VoidCountByDown)) < (INT)dwBackSceneHeight) &&
            (this->CheckShapeCanHere(SquarePosX, SquarePosY+1, wShapeIndex)))
        {
            if (this->CheckShapeCanHere(SquarePosX, SquarePosY+1, wShapeIndex))
            {
                SquarePosY += 1;
            }
        }
        else
        {
            m_pSquare->SetShapeShadowPos(SquarePosX, SquarePosY);
            return;
        }
    }
}

// 设置自动背景
VOID CT_Game::SetAutoBack(BOOL bOpen)
{
    m_IsAutoBack = bOpen;
    return;
}

// 是否自动背景
BOOL CT_Game::IsAutoBack(VOID)
{
    return m_IsAutoBack;
}

// 检查是否游戏结束(并处理游戏结束的事情)
BOOL CT_Game::IsGameOver(VOID)
{
    // 布尔返回值
    BOOL bRet = TRUE;

    // 检查方块是否可以放入当前位置
    NPOS SquarePosX, SquarePosY;
    m_pSquare->GetShapePos(SquarePosX, SquarePosY);
    bRet = this->CheckShapeCanHere(SquarePosX, SquarePosY, m_pSquare->GetCurrentShapeIndex());

    // 如果不可以
    if (!bRet)
    {
        // 释放游戏
        this->EndGame();
        m_pSound->Play(GAME_SOUND_DEAD, FALSE);
        if (m_IsWin)
        {
            // 游戏结束
            return TRUE;
        }
        if (IDYES == ::MessageBox(m_hWnd, _T("是否重新开始？"),_T("游戏结束"), MB_YESNO))
        {
            // 初始化游戏
            this->StartGame();
        }
        else
        {
            // 清理游戏界面
        //::InvalidateRect(m_hWnd, NULL, FALSE);
        }
        // 游戏结束
        return TRUE;
    }
    else
    {
        // 游戏正常
        return FALSE;
    }
}

// 设置游戏心跳间隔
UINT CT_Game::SetTimer(UINT uTime)
{
    // 记录以前的时间间隔
    UINT uOldTime = m_dwGameSpeed;

    // 获取新时间间隔
    m_dwGameSpeed = uTime;

    // 更新新的时间间隔
    ::KillTimer(m_hWnd, m_uTimerId);
    ::SetTimer(m_hWnd, m_uTimerId, m_dwGameSpeed, NULL);

    // 返回以前的时间间隔
    return uOldTime;
}

// 重置游戏
BOOL CT_Game::Reset(DWORD dwBackSceneWidth, DWORD dwBackSceneHeight)
{
    this->GetHeartbeatTime();
    this->SetTimer(m_dwGameSpeed);

    m_pScene->Reset(0, dwBackSceneWidth, dwBackSceneHeight);
    m_pRender->ResetBackScene();
    //::InvalidateRect(m_hWnd, NULL, FALSE);

    return TRUE;
}

//// 获取主窗口指针
//CVSTServerDlg* CT_Game::GetDlgPoint(VOID)
//{
//    _ENTER_FUN;
//
//    return m_pDlg;
//
//    _LEAVE_FUN_KEEP_NO_DISPOSE;
//    return FALSE;
//}
//
//

// 获取游戏级别对应心跳间隔
INT CT_Game::GetHeartbeatTime(VOID)
{
    // 定义脚本类型返回值
    LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("GetHeartbeatTime"), _T("%d"), m_dwGameLevel);
    return m_dwGameSpeed = (DWORD)LuaFunRetVal.m_lLuaFunRet;
}

// 获取游戏积分
DWORD CT_Game::GetIntegral(VOID)
{
    return m_uIntegral;
}

// 增加游戏积分
DWORD CT_Game::AddIntegral(UINT uMete)
{
    static UINT uLastIntegral = 0;

    if ((uMete + m_uIntegral) < m_uIntegral)
    {
        AssertEx(AL_CATAST, FALSE, _T("积分溢出"));
    }
    else
    {
        m_uIntegral += uMete;
    }

    DWORD dwBackSceneWidth,dwBackSceneHeight;
    m_pScene->GetBackSceneSize(dwBackSceneWidth, dwBackSceneHeight);

    uLastIntegral -= (uLastIntegral % m_dwEachLevelIntegral);
    UINT uTemp = m_uIntegral - (m_uIntegral % m_dwEachLevelIntegral);
    if (uTemp > uLastIntegral && !m_IsWin)
    {
        this->LevelUp();
        uLastIntegral = m_uIntegral;
        return FALSE;
    }
    uLastIntegral = m_uIntegral;
    return TRUE;
}

// 减少游戏积分
DWORD CT_Game::SubIntegral(UINT uMete)
{
    if (uMete > m_uIntegral)
    {
        m_uIntegral = 0;
    }
    else
    {
        m_uIntegral -= uMete;
    }

    return TRUE;
}

// 重置游戏积分
VOID CT_Game::ResetIntegral(VOID)
{
    m_uIntegral = 0;
}

// 游戏升级
BOOL CT_Game::LevelUp(VOID)
{
    if (m_dwGameTollGateLevel < 10)
    {
        m_dwGameTollGateLevel++;
        this->SetReset();
        m_pSound->Play(GAME_SOUND_LEVELUP, FALSE);
    }
    else if (m_dwGameTollGateLevel == 10)
    {
        if (m_dwGameLevel == 10)
        {
            this->GameWin();
        }
        else
        {
            m_dwGameLevel++;
            this->GetHeartbeatTime();
            this->SetTimer(m_dwGameSpeed);
            m_pSound->Play(GAME_SOUND_LEVELCOMPLETE, FALSE);
            m_dwGameTollGateLevel = 1;
            this->SetReset();
        }
    }

    this->PlayBackMusic(TRUE);
    return TRUE;
}

// 设置游戏在下一心跳时将重置
VOID CT_Game::SetReset(VOID)
{
    m_bGameNeedReset = TRUE;
}

// 游戏胜利
VOID CT_Game::GameWin(VOID)
{
    m_IsWin = TRUE;
    m_IsAutoFront = TRUE;
    this->SetReset();
}

// 播放背景音乐
BOOL CT_Game::PlayBackMusic(BOOL IsRePlay)
{
    // 布尔返回值
    BOOL bRet = TRUE;

    bRet = m_pSound->IsPlaying(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);
    if (bRet)
    {
        if (IsRePlay)
        {
            m_pSound->Stop(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex);
        }
        else
        {
            return TRUE;
        }
    }
    DOUBLE fRadix = RADIXN;
    DWORD nRadix = R_BYTE(fRadix, 1, m_dwBackMusicCount);
    if (m_dwCurrentMusicIndex == nRadix)
    {
        nRadix = R_BYTE(fRadix, 1, (m_dwBackMusicCount - 1));
        if (m_dwCurrentMusicIndex == nRadix)
        {
            nRadix++;
        }
    }
    m_dwCurrentMusicIndex = nRadix;
    bRet = m_pSound->Play(GAME_SOUND_INDEX_BACK_MUSIC00 + m_dwCurrentMusicIndex, FALSE, TRUE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("播放声音失败,函数失败"));
    
    return bRet;
}

// 加载游戏声音
BOOL CT_Game::LoadGameSound(VOID)
{
    // 布尔返回值
    BOOL bRet = TRUE;
    // 定义脚本类型返回值
    LUA_FUN_RET_VALUE LuaFunRetVal(LUA_RET_INT);

    // 获取背景音乐个数
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetBackMusicCount"), _T("%d"), 0);
    m_dwBackMusicCount = (DWORD)LuaFunRetVal.m_lLuaFunRet;

    LuaFunRetVal.m_eType = LUA_RET_STRING;
    FOR_x_TO(i, m_dwBackMusicCount)
    {
        // 获取背景音乐文件路径名
        m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetBackMusic"), _T("%d"), (i));
        AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载背景音乐配置信息失败,函数失败"));
        bRet = m_pSound->Open(GAME_SOUND_INDEX_BACK_MUSIC00 + i, LuaFunRetVal.m_String.m_str, FALSE);
        AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    }

    // 获取开始游戏声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetReadyGoSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_READYGO, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取固定方块声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetFixationSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_FIXATIONSOUND_0, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取旋转方块声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetDistortioSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_DISTORTION, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取消除方块声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetBlastSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_BLASTSOUND_1, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_BLASTSOUND_2, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_BLASTSOUND_3, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_BLASTSOUND_4, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取游戏失败声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetDeadSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_DEAD, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取游戏喝彩声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_Gethurray01Sound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_HURRAY01, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_Gethurray02Sound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_HURRAY02, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_Gethurray03Sound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_HURRAY03, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取游戏升级声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetLevelUpSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_LEVELUP, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    // 获取游戏进阶声音
    m_pLuaManager->RunLuaFun(&LuaFunRetVal, 1, _T("Sound_GetLevelCompleteSound"), NULL);
    AssertEx(AL_SHOW_ONLY, LuaFunRetVal.m_String.m_str, _T("加载游戏音效配置信息失败,函数失败"));
    bRet = m_pSound->Open(GAME_SOUND_LEVELCOMPLETE, LuaFunRetVal.m_String.m_str, FALSE);
    AssertEx(AL_SHOW_ONLY, bRet, _T("打开声音文件失败,函数失败"));

    return TRUE;
}

// 获取游戏速度等级
DWORD CT_Game::GetSpeedLevel(VOID)
{
    return m_dwGameLevel;
}

// 获取游戏关卡
DWORD CT_Game::GetGameLevel(VOID)
{
    return m_dwGameTollGateLevel;
}
