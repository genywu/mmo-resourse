#include "Precompiled.h"

CT_ShapeFile::CT_ShapeFile(TCHAR* strShapeFileName)
:m_hShapeFile(NULL)
,m_pData(NULL)
,m_dwFileSize(0)
{
    // 初始化形状文件头
    ZeroMemory(&m_ShapeFileHead, sizeof(m_ShapeFileHead));

    TCHAR szShapeFile[MAX_PATH] = {0};
    CreateFullPath(szShapeFile, strShapeFileName);

    // 打开形状文件
    m_hShapeFile = ::CreateFile(
        szShapeFile,       // 打开文件的路径名
        FILE_READ_DATA,         // 打开文件的权限为读
        FILE_SHARE_READ,        // 以共享读的方式打开文件
        NULL,                   // 安全结构体为空
        OPEN_EXISTING,          // 以保持打开的方式打开
        NULL,                   // 文件属性为空
        NULL
        );

    // 检查打开形状文件是否成功
    AssertEx(AL_CATAST, m_hShapeFile != INVALID_HANDLE_VALUE, _T("打开形状文件失败,函数失败"));

    // 获取形状文件的尺寸
    m_dwFileSize = ::GetFileSize(m_hShapeFile, NULL);

    // 检查形状文件尺寸是否合法
    AssertEx(AL_CATAST, m_dwFileSize, _T("形状文件尺寸异常,函数失败"));

    // 定义形状文件头长度
    DWORD dwShapeFileSize = sizeof(m_ShapeFileHead);

    // 读取形状文件内容到缓冲区
    BOOL bRet = ::ReadFile(
        m_hShapeFile,           // 读取文件的句柄
        &m_ShapeFileHead,       // 形状文件内容缓冲区
        dwShapeFileSize,          // 读取形状文件头的尺寸
        &dwShapeFileSize,         // 读取形状文件头的尺寸校验
        NULL                    // 读取属性结构体空
        );
    AssertEx(AL_CATAST, bRet, _T("读取形状文件头失败,函数失败"));

    // 检查读取形状文件内容是否成功
    AssertEx(AL_CATAST, dwShapeFileSize == sizeof(m_ShapeFileHead), _T("读取形状文件头失败,函数失败"));

    // 检查打开的形状文件是否合法
    AssertEx(AL_CATAST, SHAPE_FILE_FILG == m_ShapeFileHead.m_FileFlag, _T("形状文件异常,文件异常"));

    // 检查打开的形状文件版本是否匹配
    AssertEx(AL_CATAST, SHAPE_FILE_VERSION == m_ShapeFileHead.m_Version, _T("形状文件版本异常,文件异常"));

    // 获取出去形状文件头剩余内容的总长度
    m_dwFileSize -= sizeof(m_ShapeFileHead);

    // 申请文件内容缓冲区空间
    m_pData = NEW BYTE[m_dwFileSize];

    // 检查缓冲区空间是否申请成功
    AssertEx(AL_CATAST, m_pData, _T("m_pData为NULL,内存不足"));

    // 读取形状文件内容到缓冲区
    bRet = ::ReadFile(
        m_hShapeFile,   // 读取文件的句柄
        m_pData,        // 形状文件内容缓冲区
        m_dwFileSize,    // 读取内容的尺寸
        &m_dwFileSize,   // 读取内容的尺寸校验
        NULL            // 读取属性结构体空
        );
    AssertEx(AL_CATAST, bRet, _T("读取形状文件头失败,函数失败"));

    // 检查读取形状文件内容是否成功
    AssertEx(
		AL_CATAST,
        m_dwFileSize == (SHAPE_FILE_ONE_SHAPE_SIZE * m_ShapeFileHead.m_ShapeCount),
        _T("读取形状文件内容失败,函数失败")
        );
}

CT_ShapeFile::~CT_ShapeFile(VOID)
{
    // 释放申请到的空间
    SAFE_DELETE_ARRAY(m_pData);

    if (m_hShapeFile)
    {
        ::CloseHandle(m_hShapeFile);
        m_hShapeFile = NULL;
    }
}

// 使用索引获取形状
LPBYTE CT_ShapeFile::GetShapeByIndex(WORD wIndex)
{
    // 检查传入的索引是否合法
    AssertEx(AL_CATAST, wIndex < m_ShapeFileHead.m_ShapeCount, _T("索引越界,错误调用"));

    // 检查形状文件是否已被打开
    AssertEx(AL_CATAST, m_hShapeFile, _T("形状文件打开失败,调用错误"));

    // 检查形状文件内容是否已被读取
    AssertEx(AL_CATAST, m_pData, _T("形状文件内容异常,调用错误"));

    // 返回索引指定的形状数组的首地址
    return (LPBYTE)(m_pData + (wIndex * SHAPE_FILE_ONE_SHAPE_SIZE));
}


// 获取形状个数
DWORD CT_ShapeFile::GetShapeCount(VOID)
{
    return m_ShapeFileHead.m_ShapeCount;
}

// 获取第一个有方块的位置从左
BYTE CT_ShapeFile::GetShapeUsedSquareByLeft(WORD wIndex)
{
    LPBYTE pBYTE = this->GetShapeByIndex(wIndex);
    for (BYTE BYTE_X = 0; BYTE_X < SHAPE_MAX_WIDTH; BYTE_X++)
    {
        for (BYTE BYTE_Y = 0; BYTE_Y < SHAPE_MAX_HEIGHT; BYTE_Y++)
        {
            if (HAVE_SQUARE == (pBYTE[BYTE_Y * SHAPE_MAX_WIDTH + BYTE_X]))
            {
                AssertEx(AL_CATAST, (BYTE_X <= (BYTE)(INVALID_NID)), _T("数据异常,错误逻辑"));
                return BYTE_X;
            }
        }
    }
    return INVALID_BID;
}

// 获取第一个有方块的位置从右
BYTE CT_ShapeFile::GetShapeUsedSquareByRight(WORD wIndex)
{
    LPBYTE pBYTE = this->GetShapeByIndex(wIndex);
    for (BYTE BYTE_X = 0; BYTE_X < SHAPE_MAX_WIDTH; BYTE_X++)
    {
        for (BYTE BYTE_Y = 0; BYTE_Y < SHAPE_MAX_HEIGHT; BYTE_Y++)
        {
            if (HAVE_SQUARE == (pBYTE[BYTE_Y * SHAPE_MAX_WIDTH + ((SHAPE_MAX_WIDTH-1) - BYTE_X)]))
            {
                AssertEx(AL_CATAST, (BYTE_X <= (BYTE)(INVALID_NID)), _T("数据异常,错误逻辑"));
                return BYTE_X;
            }
        }
    }
    return INVALID_BID;
}

// 获取第一个有方块的位置从上
BYTE CT_ShapeFile::GetShapeUsedSquareByUp(WORD wIndex)
{
    LPBYTE pBYTE = this->GetShapeByIndex(wIndex);
    for (BYTE BYTE_Y = 0; BYTE_Y < SHAPE_MAX_HEIGHT ; BYTE_Y++)
    {
        for (BYTE BYTE_X = 0; BYTE_X < SHAPE_MAX_WIDTH; BYTE_X++)
        {
            if (HAVE_SQUARE == (pBYTE[BYTE_Y * SHAPE_MAX_WIDTH + BYTE_X]))
            {
                AssertEx(AL_CATAST, (BYTE_Y <= (BYTE)(INVALID_NID)), _T("数据异常,错误逻辑"));
                return BYTE_Y;
            }
        }
    }
    return INVALID_BID;
}

// 获取第一个有方块的位置从下
BYTE CT_ShapeFile::GetShapeUsedSquareByDown(WORD wIndex)
{
    LPBYTE pBYTE = this->GetShapeByIndex(wIndex);
    for (BYTE BYTE_Y = 0; BYTE_Y < SHAPE_MAX_HEIGHT ; BYTE_Y++)
    {
        for (BYTE BYTE_X = 0; BYTE_X < SHAPE_MAX_WIDTH; BYTE_X++)
        {
            if (HAVE_SQUARE == (pBYTE[((SHAPE_MAX_HEIGHT-1) - BYTE_Y) * SHAPE_MAX_WIDTH + BYTE_X]))
            {
                AssertEx(AL_CATAST, (BYTE_Y <= (BYTE)(INVALID_NID)), _T("数据异常,错误逻辑"));
                return BYTE_Y;
            }
        }
    }
    return INVALID_BID;
}