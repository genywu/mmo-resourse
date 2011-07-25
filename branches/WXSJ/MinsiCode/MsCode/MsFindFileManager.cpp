#include "Precompiled.h"
#include "MsFindFileManager.h"

CMsFindFileManager::CMsFindFileManager(VOID)
:m_nRootPathLen(0)
,m_nFileTypeLen(0)
,m_nRealPathLen(0)
{
    _ENTER_FUN_C;

    // 初始化需要遍历的路径
    ZeroMemory(m_szRootPath, MAX_PATH * sizeof(TCHAR));

    // 初始化当前路径
    ZeroMemory(m_szCurrentDirectory, MAX_PATH * sizeof(TCHAR));

    // 初始化需要查找的文件类型
    ZeroMemory(m_szFileType, MAX_PATH);

    // 初始化需要查找的具体路径与类型的字符串
    ZeroMemory(m_szRealFindPathName, MAX_PATH);

    // 初始化查找结果结构体
    ZeroMemory(&m_FindFileData, sizeof(LPWIN32_FIND_DATA));

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,构造or析构不需要返回值*/;
}

CMsFindFileManager::~CMsFindFileManager(VOID)
{
    _ENTER_FUN_C;

   _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,构造or析构不需要返回值*/;
}

// 设置搜索根目录
BOOL CMsFindFileManager::SetRootPath(CONST TCHAR* strPath)
{
    _ENTER_FUN_C;

    // 真正的查找目录
    CONST TCHAR* strRealPath = NULL;

    // 如果使用默认参数查找当前工作目录
    if (!strPath)
    {
        // 设置当前工作目录为查找目录
        ::GetCurrentDirectory(MAX_PATH, m_szCurrentDirectory);
        _tcsncpy_s(
            m_szRootPath,
            MAX_PATH,
            m_szCurrentDirectory,
            (INT)_tcslen(m_szCurrentDirectory)
            );

        strRealPath = m_szRootPath;
    }
    else
    {
        // 检查传入参数,设置传入目录为查找目录
        AssertEx(AL_SHOW_ONLY, strPath, _T("传入参数错误,错误调用"));
        strRealPath = strPath;
    }

    // 获取路径长度
    m_nRootPathLen = (INT)_tcslen(strRealPath);
    AssertEx(AL_SHOW_ONLY, m_nRootPathLen < MAX_PATH - 2, _T("路径长度错误,错误传参"));

    // 检测目录是否存在
    ::GetCurrentDirectory(MAX_PATH, m_szCurrentDirectory);
    AssertEx(AL_SHOW_ONLY, ::SetCurrentDirectory(strRealPath), _T("路径不存在,错误调用"));
    ::SetCurrentDirectory(m_szCurrentDirectory), _T("路径不存在,错误调用");

    // 拷贝搜索目录到成员
    if (m_szRootPath != strRealPath)
    {
        _tcsncpy_s(m_szRootPath, MAX_PATH, strRealPath, m_nRootPathLen);
    }

    // 在目录路径后追加'\'
    AppPath(m_szRootPath);
    m_nRootPathLen++;

    // 拷贝查找目录到最终查找字符串
    _tcsncpy_s(m_szRealFindPathName, MAX_PATH, m_szRootPath, m_nRootPathLen);

    // 查找字符串最后追加'*'
    AppPath(m_szRealFindPathName, _T('*'));

    // 获取最终查找字符串长度
    m_nRealPathLen = (INT)_tcslen(m_szRealFindPathName);

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
    return FALSE;
}

// 设置搜索文件类型
BOOL CMsFindFileManager::SetFindFileType(CONST TCHAR* strFileType)
{
    _ENTER_FUN_C;

    if (strFileType)
    {
        // 获取新文件类型的长度
        m_nFileTypeLen =  (INT)_tcslen(strFileType);
        AssertEx(AL_SHOW_ONLY, m_nFileTypeLen < MAX_PATH - 2, _T("m_nFileTypeLen异常,错误传参"));

        // 拷贝新文件类型到成员
        _tcsncpy_s(m_szFileType, MAX_PATH, strFileType, m_nFileTypeLen);
    }
    else
    {
        // 检查参数是否有误
        AssertEx(AL_SHOW_ONLY, strFileType, _T("strFileType为空,错误传参"));
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
    return FALSE;
}

// 遍历目录下由指定类型的文件
BOOL CMsFindFileManager::BrowsePath(VOID)
{
    _ENTER_FUN_C;

    // 检查最终查找字符串长度
    AssertEx(AL_SHOW_ONLY, m_nRealPathLen, _T("需要查找的目录错误,错误调用"));

    // 首先查找第一个文件
    HANDLE hFindFile = ::FindFirstFile(m_szRealFindPathName, &m_FindFileData);
    do
    {
        // 检查文件查找是否成功
        AssertEx(
			AL_SHOW_ONLY,
            (INVALID_HANDLE_VALUE != hFindFile),
            _T("FindFirstFile失败,函数失败")
            );

        // 判断找到的如果是个子目录
        if (FILE_ATTRIBUTE_DIRECTORY & m_FindFileData.dwFileAttributes)
        {
            // 如果查找到的子目录为"." 或 ".." 那么放弃进入
            if(*m_FindFileData.cFileName == '.') 
            { 
                continue;
            } 
            else
            {
                // 本级查找字符串备份
                TCHAR   szRealFindPathName_Bk[MAX_PATH] = {0};

                // 备份本级查找字符串
                _tcsncpy_s(
                    szRealFindPathName_Bk,
                    MAX_PATH,
                    m_szRealFindPathName,
                    m_nRealPathLen
                    );

                // 本级查找字符串长度备份
                INT     nRealFindPathNameLen_Bk = 
                    (INT)_tcslen(szRealFindPathName_Bk);

                // 去掉最后一位的'*'
                if (m_nRealPathLen < MAX_PATH)
                {
                    if (m_szRealFindPathName[m_nRealPathLen - 1] == '*')
                    {
                        m_szRealFindPathName[m_nRealPathLen - 1] = '\0';
                        m_nRealPathLen--;
                    }
                    else
                    {
                        AssertEx(AL_CATAST, FALSE, _T("代码异常,错误逻辑"));
                    }
                }
                else
                {
                    AssertEx(AL_CATAST, m_nRealPathLen < MAX_PATH, _T("访问越界"));
                }

                // 更新当前查找的目录
                _tcsncat_s(
                    m_szRealFindPathName,
                    MAX_PATH,
                    m_FindFileData.cFileName,
                    (INT)_tcslen(m_FindFileData.cFileName)
                    );

                // 查找字符串最后追加'\\'
                AppPath(m_szRealFindPathName);

                // 调用查找到目录通知函数
                this->ProcessPath(m_szRealFindPathName);

                // 查找字符串最后追加'*'
                AppPath(m_szRealFindPathName, _T('*'));

                // 更新当前查找的目录长度
                m_nRealPathLen = (INT)_tcslen(m_szRealFindPathName);

                // 进行子目录递归查找
                BrowsePath();

                // 恢复本级查找字符串
                _tcsncpy_s(
                    m_szRealFindPathName,
                    MAX_PATH,
                    szRealFindPathName_Bk,
                    nRealFindPathNameLen_Bk
                    );

                // 更新当前查找的目录长度
                m_nRealPathLen = (INT)_tcslen(m_szRealFindPathName);
            }
        }
        else
        {
            // 查找到的文件全路径名字符串
            TCHAR szFindedFileName[MAX_PATH] = {0};

            // 如果查找到的文件满足查找条件
            if (CheckFileType(m_FindFileData.cFileName))
            {
                // 获得当前查找到的文件的完整路径名
                _tcsncpy_s(
                    szFindedFileName,
                    MAX_PATH,
                    m_szRealFindPathName,
                    m_nRealPathLen
                    );

                // 去掉最后一位的'*'

                if (m_nRealPathLen < MAX_PATH)
                {
                    if (szFindedFileName[m_nRealPathLen - 1] == '*')
                    {
                        szFindedFileName[m_nRealPathLen - 1] = '\0';
                    }
                    else
                    {
                        AssertEx(AL_CATAST, FALSE, _T("代码异常,"));
                    }
                }
                else
                {
                    AssertEx(AL_CATAST, m_nRealPathLen < MAX_PATH, _T("访问越界"));
                }

                // 追加'\\'
                AppPath(szFindedFileName);

                // 追加文件名
                _tcsncat_s(
                    szFindedFileName,
                    MAX_PATH,
                    m_FindFileData.cFileName,
                    (INT)_tcslen(m_FindFileData.cFileName)
                    );

                // 调用查找文件成功通知函数
                this->ProcessFile(szFindedFileName);
            }
        }
        // 查找下一个文件是否成功
    } while (::FindNextFile(hFindFile, &m_FindFileData));

    // 如果目录内没有任何内容
    if (ERROR_NO_MORE_FILES == GetLastError())
    {
        // 返回成功
        return TRUE;
    }
    else
    {
        // 返回失败
        return FALSE;
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
    return FALSE;
}


// 检查搜索文件类型是否匹配
BOOL CMsFindFileManager::CheckFileType(CONST TCHAR* strPath)
{
    _ENTER_FUN_C;

    // 检查参数是否有误
    if (strPath)
    {
        // 获取文件名长度
        SIZE_T sPathLen = _tcslen(strPath);
        AssertEx(AL_SHOW_ONLY, sPathLen < MAX_PATH - 2, _T("参数尺寸错误,错误传参"));

        // 获得查找路径中的理想扩展名首位置
        SIZE_T sFirstFileTypeName = sPathLen - m_nFileTypeLen;

        // 循环比较扩展名是否相同
        for (INT i = 0; i < m_nFileTypeLen; i++)
        {
            // 如果查找扩展名对于路径已经越界,直接返回失败
            if (sFirstFileTypeName + i > sPathLen)
            {
                // 返回失败
                return FALSE;
            }

            // 如果某一位的扩展名不同
            if (m_szFileType[i] != (strPath[sFirstFileTypeName + i]))
            {
                // 返回失败
                return FALSE;
            }
        }
        // 全部比较成功,返回成功
        return TRUE;
    }
    else
    {
        AssertEx(AL_SHOW_ONLY, strPath, _T("strPath为空,错误传参"));
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
    return FALSE;
}

// 路径默认追加"\"
BOOL CMsFindFileManager::AppPath(TCHAR* strPath, TCHAR cChr)
{
    _ENTER_FUN_C;

    // 获取字符串路径长度
    SIZE_T sPathLen = _tcslen(strPath);
    AssertEx(AL_SHOW_ONLY, (sPathLen + 1) < MAX_PATH, _T("查找字符串总长度越界,错误传参"));

    // 如果最后一位追加的是'\\'
    if(_T('\\')== cChr || _T('/')== cChr)
    {
        // 如果最后一位不是'\\'
        if (strPath[sPathLen - 1] != _T('\\') && 
            strPath[sPathLen - 1] != _T('/'))
        {
            // 最后一位更改为'\\'
            strPath[sPathLen] = _T('\\');

            // 最后一位追加一个'\0'
            strPath[sPathLen + 1] = _T('\0');
        }
    }
    else
    {
        // 最后一位更改为cChr
        strPath[sPathLen] = cChr;

        // 最后一位追加一个'\0'
        strPath[sPathLen + 1] = _T('\0');
    }
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
    return FALSE;
}

//// 遍历函数每找到一个文件,就调用ProcessFile,并把文件名作为参数传递过去
//// 用户可以覆写该函数,加入自己的处理代码
//BOOL CMsFindFileManager::ProcessFile(CONST TCHAR* /*strFileName*/)
//{
//    _ENTER_FUN;
//
//    // 返回成功
//    return TRUE;
//
//    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
//    return FALSE;
//}
//
//// 遍历函数每进入一个目录,就调用ProcessPath,
//// 并把正在处理的目录名及上一级目录名作为参数传递过去,
//// 如果正在处理的是初始目录,则strParentPath = NULL
//// 用户可以覆写该函数,加入自己的处理代码
//BOOL CMsFindFileManager::ProcessPath(CONST TCHAR* /*strFileName*/)
//{
//    _ENTER_FUN;
//
//    // 返回成功
//    return TRUE;
//
//    _LEAVE_FUN_DISPOSE_CONTINUE_C/*异常处理,异常返回失败*/;
//    return FALSE;
//}
