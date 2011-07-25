#include "Precompiled.h"

CT_Scene::CT_Scene(VOID)
: m_wBackSceneMetafileIndex (0)     // 初始化场景使用的图元索引为0
, m_pSceneBYTE              (NULL)  // 初始化场景元素数组指针为空
, m_dwBackSceneWidth        (0)     // 初始化场景宽度为0
, m_dwBackSceneHeight       (0)     // 初始化场景高度为0
{
    ;
}

CT_Scene::~CT_Scene(VOID)
{
    this->Release();
}


// 初始化场景类
BOOL CT_Scene::Init(
    WORD wBackSceneMetafileIndex,
    DWORD dwBackSceneWidth,
    DWORD dwBackSceneHeight
    )
{
    _ENTER_FUN_C;

    // 检查传入的图元索引是否合法,获取传入的图元索引
    AssertEx(AL_CATAST, wBackSceneMetafileIndex < MAX_METAFILE_COUNT, _T("传入的图元索引非法,错误调用"));
    m_wBackSceneMetafileIndex = wBackSceneMetafileIndex;

    // 检查传入的场景宽度是否合法,获取传入的场景宽度
    AssertEx(AL_CATAST, dwBackSceneWidth <= BACKSCENE_MAX_SQUARE_COUNT_X, _T("传入的场景宽度异常,错误调用"));
    m_dwBackSceneWidth = dwBackSceneWidth;

    // 检查传入的场景宽度是否合法,获取传入的场景高度
    AssertEx(AL_CATAST, dwBackSceneHeight <= BACKSCENE_MAX_SQUARE_COUNT_Y, _T("传入的场景高度异常,错误调用"));
    m_dwBackSceneHeight = dwBackSceneHeight;

    // 申请场景元素数组空间
    m_pSceneBYTE = NEW BACK_SCENE_BYTE[m_dwBackSceneWidth * m_dwBackSceneHeight];
    AssertEx(AL_CATAST, m_pSceneBYTE, _T("m_pSceneBYTE指针为空,内存不足"));

    // 初始化场景元素数组为空
    ZeroMemory(m_pSceneBYTE, m_dwBackSceneWidth * m_dwBackSceneHeight * sizeof(BACK_SCENE_BYTE));

    // 返回初始化成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}


// 释放场景
VOID CT_Scene::Release(VOID)
{
    // 释放场景方块数组空间
    SAFE_DELETE_ARRAY(m_pSceneBYTE);

    m_dwBackSceneWidth = 0;
    m_dwBackSceneHeight = 0;
}

// 重置场景
BOOL CT_Scene::Reset(
    WORD wBackSceneMetafileIndex,
    DWORD dwBackSceneWidth,
    DWORD dwBackSceneHeight
    )
{
    this->Release();
    return this->Init(wBackSceneMetafileIndex, dwBackSceneWidth, dwBackSceneHeight);
}

// 获取场景元素数组首地址
BACK_SCENE_BYTE* CT_Scene::GetBackSceneBuff(VOID)
{
    // 返回元素数组首地址
    return m_pSceneBYTE;
}

// 获取场景元素数组首地址
VOID CT_Scene::GetBackSceneSize(DWORD& dwBackSceneWidth, DWORD& dwBackSceneHeight)
{
    // 返回场景方块宽度与高度
    dwBackSceneWidth = m_dwBackSceneWidth;
    dwBackSceneHeight = m_dwBackSceneHeight;
}

// 设置场景图元索引
BOOL CT_Scene::SetBackMetafileIndex(WORD wBackSceneMetafileIndex)
{
    _ENTER_FUN_C;

    // 检查传入的图元索引是否合法,获取传入的图元索引
    AssertEx(AL_CATAST, wBackSceneMetafileIndex < MAX_METAFILE_COUNT, _T("传入的图元索引非法,错误调用"));

    FOR_x_TO(i, (m_dwBackSceneWidth * m_dwBackSceneHeight))
    {
        if (m_pSceneBYTE[i].m_MetafileIndex == m_wBackSceneMetafileIndex)
        {
            m_pSceneBYTE[i].m_MetafileIndex = wBackSceneMetafileIndex;
        }
    }
    m_wBackSceneMetafileIndex = wBackSceneMetafileIndex;

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return NULL;
}

// 设置前景图元索引
BOOL CT_Scene::SetFrontMetafileIndex(WORD wFrontSceneMetafileIndex)
{
    _ENTER_FUN_C;

    // 检查传入的图元索引是否合法,获取传入的图元索引
    AssertEx(AL_CATAST, wFrontSceneMetafileIndex < MAX_METAFILE_COUNT, _T("传入的图元索引非法,错误调用"));

    FOR_x_TO(i, (m_dwBackSceneWidth * m_dwBackSceneHeight))
    {
        if (m_pSceneBYTE[i].m_MetafileIndex != m_wBackSceneMetafileIndex)
        {
            m_pSceneBYTE[i].m_MetafileIndex = wFrontSceneMetafileIndex;
        }
    }
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return NULL;
}

// 设置场景图元索引
WORD CT_Scene::GetBackMetafileIndex(VOID)
{
    return m_wBackSceneMetafileIndex;
}