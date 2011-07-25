#pragma once

// 宏定义背景音乐文件名
#define SCRIPT_FILE_NAME    "%s/RrTetris/Script/Config.lua"

// 俄罗斯方块背景宽度(游戏设计决定,不要轻易改动)
#define BACKSCENE_MAX_WIDTH     800

// 俄罗斯方块背景高度(游戏设计决定,不要轻易改动)
#define BACKSCENE_MAX_HEIGHT    600

// 游戏声音索引
enum GAME_SOUND_INDEX
{
    GAME_SOUND_INDEX_BACK_MUSIC00 = 0,
    GAME_SOUND_INDEX_BACK_MUSIC01,
    GAME_SOUND_INDEX_BACK_MUSIC02,
    GAME_SOUND_INDEX_BACK_MUSIC03,
    GAME_SOUND_INDEX_BACK_MUSIC04,
    GAME_SOUND_INDEX_BACK_MUSIC05,
    GAME_SOUND_INDEX_BACK_MUSIC06,
    GAME_SOUND_READYGO,
    GAME_SOUND_DISTORTION,
    GAME_SOUND_DEAD,
    GAME_SOUND_FIXATIONSOUND_0,
    GAME_SOUND_BLASTSOUND_1,
    GAME_SOUND_BLASTSOUND_2,
    GAME_SOUND_BLASTSOUND_3,
    GAME_SOUND_BLASTSOUND_4,
    GAME_SOUND_HURRAY01,
    GAME_SOUND_HURRAY02,
    GAME_SOUND_HURRAY03,
    GAME_SOUND_LEVELUP,
    GAME_SOUND_LEVELCOMPLETE,
    GAME_SOUND_INDEX_COUNT,
};

enum GAME_STATE
{
    GAME_STATE_NO_INIT = 0, // 游戏未初始化状态
    GAME_STATE_STOP,        // 游戏停止状态
    GAME_STATE_PAUSE,       // 游戏暂停状态
    GAME_STATE_PLAYING,     // 游戏运行中状态
};

class CT_Game
{
public:
    CT_Game(HWND hWnd);
    virtual ~CT_Game(VOID);

    // 初始化游戏
    BOOL    InitGame(VOID);
    BOOL    ReleaseGame(VOID);      // 释放游戏
    BOOL    StartGame(VOID);        // 开始游戏
    BOOL    PauseGame(VOID);        // 暂停游戏
    BOOL    ContinueGame(VOID);     // 继续游戏
    BOOL    EndGame(VOID);          // 结束游戏
    BOOL    AddGameVolume(VOID);    // 增加游戏音量
    BOOL    SubGameVolume(VOID);    // 降低游戏音量
    BOOL    AddMusicVolume(VOID);   // 增加音乐音量
    BOOL    SubMusicVolume(VOID);   // 降低音乐音量
    BOOL    GameHeartbeat(VOID);    // 游戏心跳(定时触发的游戏时间片处理)
    BOOL    MoveShape(UINT uWay);   // 移动方块
    BOOL    FixationShape(VOID);    // 固定方块
    BOOL    RemoveFullLine(VOID);   // 消除满行的方块
    VOID    SetAutoBack(BOOL bOpen);// 设置自动背景
    BOOL    IsAutoBack(VOID);       // 是否自动背景
    BOOL    IsGameOver(VOID);       // 检查是否游戏结束(如果结束一并处理游戏结束的事情)
    UINT    SetTimer(UINT uTime);   // 设置游戏心跳间隔
    HWND    GetMainWnd(VOID);       // 获取主窗口句柄
    INT     GetHeartbeatTime(VOID); // 获取游戏级别对应心跳间隔
    DWORD   GetIntegral(VOID);      // 获取游戏积分
    DWORD   AddIntegral(UINT uMete);// 增加游戏积分
    DWORD   SubIntegral(UINT uMete);// 减少游戏积分
    VOID    ResetIntegral(VOID);    // 重置游戏积分
    DWORD   GetSpeedLevel(VOID);    // 获取游戏速度等级
    DWORD   GetGameLevel(VOID);     // 获取游戏关卡
    BOOL    LevelUp(VOID);          // 游戏升级
    VOID    GameWin(VOID);          // 游戏胜利
    BOOL    LoadGameSound(VOID);    // 加载游戏声音
    VOID    SetReset(VOID);         // 设置游戏在下一心跳时将重置

    // 获取游戏状态
    GAME_STATE GetGameState(VOID);

    // 绘制游戏
    BOOL Render(VOID); 

    // 播放背景音乐
    BOOL PlayBackMusic(
        BOOL IsRePlay =FALSE
        );

    // 检查方块是否可以放在此处
    BOOL CheckShapeCanHere(
        NPOS SquarePosX,
        NPOS SquarePosY,
        WORD wShapeIndex
        );

    // 消除满行方块的特效函数
    BOOL RemoveEffect(
        BACK_SCENE_BYTE* pSceneElement,
        DWORD dwCount
        );

    // 计算阴影位置
    VOID CalculateShadowPos(VOID);

    // 重置游戏
    BOOL Reset(
        SIZE_T BackSceneWidth,
        SIZE_T BackSceneHeight
        );

// 私有数据成员
private:

    ID3DXFont*          m_lpD3DFrameFont;       // D3D帧数字体对象
    BOOL                m_IsInit;               // 游戏类是否初始化标记
    HWND                m_hWnd;                 // 游戏主窗口句柄
    CT_Scene*           m_pScene;               // 场景模块
    CT_Square*          m_pSquare;              // 方块对象指针
    CRender*            m_pRender;              // 渲染对象指针
    CMsSound*           m_pSound;               // 声音对象指针
    GAME_STATE          m_GameState;            // 游戏状态
    UINT                m_uTimerId;             // 游戏定时器ID
    DWORD               m_dwGameLevel;          // 游戏等级
    DWORD               m_dwGameTollGateLevel;  // 游戏关卡等级
    DWORD               m_dwGameSpeed;          // 游戏速度
    CMsLuaManager*      m_pLuaManager;          // 脚本管理器指针
    BOOL                m_IsAutoBack;           // 是否自动切换背景
    BOOL                m_IsAutoFront;          // 是否自动切换前景
    BOOL                m_IsDrawShadow;         // 是否绘制阴影
    UINT                m_uIntegral;            // 游戏积分
    BOOL                m_bGameNeedReset;       // 游戏需要重置
    BOOL                m_IsWin;                // 游戏是否已经胜利
    DWORD               m_dwBackMusicCount;     // 背景音乐个数
    DWORD               m_dwEachLevelIntegral;  // 升级每级所需积分
    DWORD               m_dwCurrentMusicIndex;  // 当前背景音乐索引
};

// 全局游戏对象
extern CT_Game* g_pGame;