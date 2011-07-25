#include "Precompiled.h"

// 方块的构造函数
CT_Square::CT_Square(VOID)
: m_SquarePosX              (0)     // 初始化形状在场景中的横坐标为0
, m_SquarePosY              (0)     // 初始化形状在场景中的纵坐标为0
, m_nMetafilePosX           (0)
, m_nMetafilePosY           (0)
, m_wCurrentShapeIndex      (0)
, m_wNext_ShapeIndex        (0)
, m_wNext2_ShapeIndex       (0)
, m_wNext3_ShapeIndex       (0)
, m_pCurrentShape           (NULL)
, m_pNext_Shape             (NULL)
, m_pNext2_Shape            (NULL)
, m_pNext3_Shape            (NULL)
, m_wCurrentMetafileIndex   (0)
, m_wNext_MetafileIndex     (0)
, m_wNext2_MetafileIndex    (0)
, m_wNext3_MetafileIndex    (0)
, m_pBitmapMetafile         (NULL)
, m_VoidCountByLeft         (0)
, m_VoidCountByRight        (0)
, m_VoidCountByUp           (0)
, m_VoidCountByDown         (0)
, m_dwShapeCount            (0)
, m_SquareShadowPosX        (0)
, m_SquareShadowPosY        (0)
, m_pShapeFile              (NULL)
{
    m_pShapeFile = NEW CT_ShapeFile(DEFAULT_SHAPE_FILE_NAME);
    AssertEx(AL_CATAST, m_pShapeFile, _T("内存不足"));
    m_dwShapeCount = m_pShapeFile->GetShapeCount();
}


// 方块的析构函数
CT_Square::~CT_Square(VOID)
{
    SAFE_DELETE(m_pShapeFile);
}

// 初始化图元数据成员
BOOL CT_Square::InitMetafile(WORD wMetafileIndex)
{
    _ENTER_FUN_C;

    // 检查传入的索引值是否正确
    AssertEx(AL_CATAST, wMetafileIndex <= MAX_METAFILE_COUNT, _T("传入的索引范围错误,错误调用"));

    // 更新图元索引值
    m_wCurrentMetafileIndex = wMetafileIndex;

    // 计算指定索引的图元在图片中的坐标,并更新
    m_nMetafilePosX = m_wCurrentMetafileIndex * MAX_METAFILE_WIDTH;
    m_nMetafilePosY = 0;

    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 初始化形状数据成员
BOOL CT_Square::InitShape(WORD wShapeIndex)
{
    _ENTER_FUN_C;

    m_wCurrentShapeIndex = wShapeIndex;

    // 根据形状索引获取形状
    m_pCurrentShape = m_pShapeFile->GetShapeByIndex(wShapeIndex);
    AssertEx(AL_CATAST, m_pCurrentShape, _T("获取形状失败,函数失败"));

    // 获取形状周边空白个数信息
    m_VoidCountByLeft   = m_pShapeFile->GetShapeUsedSquareByLeft(m_wCurrentShapeIndex);
    AssertEx(AL_CATAST, m_VoidCountByLeft!=INVALID_BID, _T("获取当前形状使用了几个宽度的方块失败,函数失败"));

    m_VoidCountByRight  = m_pShapeFile->GetShapeUsedSquareByRight(m_wCurrentShapeIndex);
    AssertEx(AL_CATAST, m_VoidCountByRight!=INVALID_BID, _T("获取当前形状使用了几个宽度的方块失败,函数失败"));

    m_VoidCountByUp     = m_pShapeFile->GetShapeUsedSquareByUp(m_wCurrentShapeIndex);
    AssertEx(AL_CATAST, m_VoidCountByDown!=INVALID_BID, _T("获取当前形状使用了几个宽度的方块失败,函数失败"));

    m_VoidCountByDown   = m_pShapeFile->GetShapeUsedSquareByDown(m_wCurrentShapeIndex);
    AssertEx(AL_CATAST, m_VoidCountByDown!=INVALID_BID, _T("获取当前形状使用了几个宽度的方块失败,函数失败"));

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;
}

// 形状变形
BOOL CT_Square::Distortion(DWORD dwBackSceneWidth, DWORD dwBackSceneHeight, BOOL bOrder)
{
    _ENTER_FUN_C;

    WORD    wNextShapeIndex = m_wCurrentShapeIndex;
    NPOS    SquarePosX = m_SquarePosX;
    NPOS    SquarePosY = m_SquarePosY;

    // 如果是顺序旋转
    if (bOrder)
    {
        // 如果是当前形状中最后一个
        if (((m_wCurrentShapeIndex + 1) % SHAPE_DISTORTION_STATE) == 0)
        {
            // 返回到第一个形状
            wNextShapeIndex -= SHAPE_DISTORTION_STATE - 1;
        }
        // 不是最后一个
        else
        {
            // ++到下一个形状
            wNextShapeIndex++;
        }
    }
    // 如果是倒序旋转
    else
    {
        // 如果是当前形状中第一个
        if (((m_wCurrentShapeIndex + 1) % SHAPE_DISTORTION_STATE) == 1)
        {
            // 返回到最后一个形状
            wNextShapeIndex += SHAPE_DISTORTION_STATE - 1;
        }
        // 不是第一个
        else
        {
            // --到上一个形状
            wNextShapeIndex--;
        }
    }

    // 获取当前形状使用了几个宽度的方块
    BYTE NextShape_FirstHaveIndexByLeft = m_pShapeFile->GetShapeUsedSquareByLeft(wNextShapeIndex);
    BYTE NextShape_FirstHaveIndexByRight= m_pShapeFile->GetShapeUsedSquareByRight(wNextShapeIndex);
    BYTE NextShape_FirstHaveIndexByUp   = m_pShapeFile->GetShapeUsedSquareByUp(wNextShapeIndex);
    BYTE NextShape_FirstHaveIndexByDown = m_pShapeFile->GetShapeUsedSquareByDown(wNextShapeIndex);

    // 如果新形状位置在右墙需要纠正
    if (SquarePosX + (SHAPE_MAX_WIDTH - NextShape_FirstHaveIndexByRight) >= (INT)dwBackSceneWidth)
    {
        //if (m_VoidCountByRight > NextShape_FirstHaveIndexByRight)
        {
            SquarePosX -= ((SquarePosX + (SHAPE_MAX_WIDTH - NextShape_FirstHaveIndexByRight)) - dwBackSceneWidth);
        }
    }

    // 如果新形状位置在左墙需要纠正
    if (SquarePosX + NextShape_FirstHaveIndexByLeft < 0)
    {
        //if (m_VoidCountByLeft > NextShape_FirstHaveIndexByLeft)
        {
            SquarePosX = (-NextShape_FirstHaveIndexByLeft);
        }
    }

    // 如果新形状位置在上墙需要纠正
    else if (SquarePosY + NextShape_FirstHaveIndexByUp < 0)
    {
        SquarePosY = (-NextShape_FirstHaveIndexByUp);
    }

    // 如果新形状位置在上墙需要纠正
    if (SquarePosY + NextShape_FirstHaveIndexByUp < 0)
    {
        SquarePosY = (-NextShape_FirstHaveIndexByUp);
    }
    // 如果新形状位置在下墙需要纠正
    else if (SquarePosY + NextShape_FirstHaveIndexByDown > (INT)dwBackSceneHeight)
    {
        return TRUE;
    }

    if (g_pGame->CheckShapeCanHere(SquarePosX, SquarePosY, wNextShapeIndex))
    {
        // 获取新的形状索引和新的使用宽度
        m_VoidCountByLeft   = NextShape_FirstHaveIndexByLeft;
        m_VoidCountByRight  = NextShape_FirstHaveIndexByRight;
        m_VoidCountByUp     = NextShape_FirstHaveIndexByUp;
        m_VoidCountByDown   = NextShape_FirstHaveIndexByDown;
        m_wCurrentShapeIndex= wNextShapeIndex;
        m_SquarePosX        = SquarePosX;
        m_SquarePosY        = SquarePosY;

        // 获取新形状
        this->InitShape(m_wCurrentShapeIndex);
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_END_C;
    return FALSE;

}

// 获取形状在场景中的坐标
VOID CT_Square::GetShapePos(NPOS& nX, NPOS& nY)
{
    // 返回形状在场景中的坐标
    nX = m_SquarePosX;
    nY = m_SquarePosY;
}

// 获取形状阴影在场景中的坐标
VOID CT_Square::GetShapeShadowPos(NPOS& uX, NPOS& uY)
{
    // 返回形状阴影在场景中的坐标
    uX = m_SquareShadowPosX;
    uY = m_SquareShadowPosY;
}

// 获取形状在场景中的坐标
VOID CT_Square::SetShapePos(NPOS X, NPOS Y)
{
    // 获取形状在场景中的坐标
    m_SquarePosX = X;
    m_SquarePosY = Y;
}

// 获取形状阴影在场景中的坐标
VOID CT_Square::SetShapeShadowPos(NPOS X, NPOS Y)
{
    // 获取形状在场景中的坐标
    m_SquareShadowPosX = X;
    m_SquareShadowPosY = Y;
}

// 获取形状文件
CT_ShapeFile* CT_Square::GetShapeFile(VOID)
{
    return m_pShapeFile;
}

// 创建方块
BOOL CT_Square::CreateShape(DWORD dwBackSceneWidth)
{
    // 随机得到方块颜色
    DOUBLE fRadix;
    m_wCurrentMetafileIndex = m_wNext_MetafileIndex ? m_wNext_MetafileIndex : ((fRadix = RADIXN) , (R_BYTE(fRadix, 3, 11)));
    m_wNext_MetafileIndex = m_wNext2_MetafileIndex ? m_wNext2_MetafileIndex : ((fRadix = RADIXN) , (R_BYTE(fRadix, 3, 11)));
    m_wNext2_MetafileIndex = m_wNext3_MetafileIndex ? m_wNext3_MetafileIndex : ((fRadix = RADIXN) , (R_BYTE(fRadix, 3, 11)));
    fRadix = RADIXN;
    m_wNext3_MetafileIndex = R_BYTE(fRadix, 3, 11);

    m_wCurrentShapeIndex = m_wNext_ShapeIndex ? m_wNext_ShapeIndex : ((fRadix = RADIXN) , (R_WORD(fRadix, 0, m_dwShapeCount)));
    m_wNext_ShapeIndex = m_wNext2_ShapeIndex ? m_wNext2_ShapeIndex : ((fRadix = RADIXN) , (R_WORD(fRadix, 0, m_dwShapeCount)));
    m_wNext2_ShapeIndex = m_wNext3_ShapeIndex ? m_wNext3_ShapeIndex : ((fRadix = RADIXN) , (R_WORD(fRadix, 0, m_dwShapeCount)));
    fRadix = RADIXN;
    m_wNext3_ShapeIndex = R_WORD(fRadix, 0, m_dwShapeCount);
    this->InitShape(m_wCurrentShapeIndex);

    // 根据形状索引获取形状
    m_pNext_Shape = m_pShapeFile->GetShapeByIndex(m_wNext_ShapeIndex);
    m_pNext2_Shape = m_pShapeFile->GetShapeByIndex(m_wNext2_ShapeIndex);
    m_pNext3_Shape = m_pShapeFile->GetShapeByIndex(m_wNext3_ShapeIndex);

    // 设置当前形状的位置在最上面中间
    this->SetShapePos((dwBackSceneWidth / 2) - (SHAPE_MAX_WIDTH / 2), (-m_VoidCountByUp));

    // 返回成功
    return TRUE;
}

// 清除形状
VOID CT_Square::Clear(VOID)
{
    this->InitMetafile(0);
    m_pCurrentShape = NULL;
    this->SetShapePos(INVALID_UID, INVALID_UID);
}