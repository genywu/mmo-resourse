#include "Precompiled.h"

CMsFileFromRes::CMsFileFromRes(HINSTANCE hInstance)
:m_hInstance(hInstance)     // 应用程序实例句柄
,m_hResFile(NULL)           // 临时资源文件句柄
{
    _ENTER_FUN_C;

    // 初始化资源类型字符串
    ZeroMemory(m_strResType, MAX_PATH);

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

CMsFileFromRes::~CMsFileFromRes(VOID)
{
    _ENTER_FUN_C;


    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

BOOL CMsFileFromRes::CreateFileFromRes(INT ID, TCHAR* strResType, TCHAR* szFilePath)
{
    _ENTER_FUN_C;

    // 布尔类型返回值
    BOOL bRet = TRUE;

    // 资源句柄
    HRSRC hRes = NULL;

    // 调用检查
    if (strResType)
    {
        TCHAR szResID[DWORD_NUMBER_SIZE] = {0};
        _itot_s(ID, szResID, DWORD_NUMBER_SIZE, RADIX_10);

        // 根据ID与类型查找资源
        if (strResType)
        {
            hRes = ::FindResource(m_hInstance, MAKEINTRESOURCE(ID), strResType);
        }
        else
        {
            hRes = ::FindResource(m_hInstance, MAKEINTRESOURCE(ID), m_strResType);
        }

        if (hRes)
        {
            // 装载资源
            HGLOBAL hLoadRes = ::LoadResource(m_hInstance, hRes);

            if (hLoadRes)
            {
                // 锁定资源
                LPVOID  lpLockRes = ::LockResource(hLoadRes);
                AssertEx(AL_NORMAL, lpLockRes, _T("LoadResource失败,函数失败"));

                // 创建资源文件
                m_hResFile = ::CreateFile(
                    szFilePath,
                    GENERIC_WRITE,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    0,
                    NULL
                    );
                AssertEx(AL_NORMAL, m_hResFile, _T("CreateFile失败,函数失败"));

                // 获取资源尺寸
                DWORD nResSize = ::SizeofResource(NULL, hRes);

                // 资源写入文件
                bRet = ::WriteFile(m_hResFile, lpLockRes, nResSize, &nResSize, NULL);
                AssertEx(AL_NORMAL, bRet, _T("WriteFile失败,函数失败"));

                // 关闭文件句柄
                bRet = ::CloseHandle(m_hResFile);
                AssertEx(AL_NORMAL, bRet, _T("CloseHandle失败,函数失败"));
                m_hResFile = NULL;

                // 释放资源文件
                ::FreeResource(hLoadRes);

                // 返回成功
                return TRUE;
            }
            else
            {
                AssertEx(AL_NORMAL, hLoadRes, _T("LoadResource失败,函数失败"));
            }
        }
        else
        {
            AssertEx(AL_NORMAL, hRes, _T("FindResource失败,函数失败"));
        }
    }
    else
    {
        AssertEx(AL_NORMAL, strResType, _T("strResType不能为空"));
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 加载资源
LPVOID CMsFileFromRes::LoadFileFromRes(INT ID, TCHAR* strResType, DWORD* lpdwFileSize)
{
    _ENTER_FUN_C;

    // 调用检查
    if (strResType)
    {
        // 资源句柄
        HRSRC hRes = ::FindResource(m_hInstance, MAKEINTRESOURCE(ID), strResType);

        if (hRes)
        {
            // 装载资源
            HGLOBAL hLoadRes = ::LoadResource(m_hInstance, hRes);

            if (hLoadRes)
            {
                *lpdwFileSize = ::SizeofResource(NULL, hRes);

                // 锁定资源
                LPVOID lpRes = ::LockResource(hLoadRes);

                // 释放资源文件
                ::FreeResource(hLoadRes);

                return lpRes;
            }
            else
            {
                AssertEx(AL_NORMAL, hLoadRes, _T("LoadResource失败,函数失败"));
            }
        }
        else
        {
            AssertEx(AL_NORMAL, hRes, _T("FindResource失败,函数失败"));
        }
    }
    else
    {
        AssertEx(AL_NORMAL, strResType, _T("strResType不能为空"));
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}
