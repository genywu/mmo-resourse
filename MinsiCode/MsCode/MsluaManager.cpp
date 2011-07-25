#include "Precompiled.h"
#include "MsLuaManager.h"

// LUA脚本管理器全局指针
CMsLuaManager* g_pCMsLuaManager = NULL;

CMsLuaManager::CMsLuaManager()
: m_hFilePath               (NULL)
, m_pExplainOrgan           (NULL)
, m_pLuaChangeNotify        (NULL)
, m_nThreadId               (0)
, m_hThread                 (NULL)
, m_IsChecking              (FALSE)
, m_lpLuaHookCallBackFun    (NULL)
{
    _ENTER_FUN_C;

    // 初始化LUA文件根目录
    ZeroMemoryArray(m_szTLuaFilePath);
    ZeroMemoryArray(m_strLuaFileName);
    ZeroMemoryArray(m_szALuaFilePath);
    ZeroMemoryArray(m_strLuaParameter);

    // 将通知指针指向缓冲区
    m_pLuaChangeNotify = (FILE_NOTIFY_INFORMATION*)m_pLuaChangeNotifyBuff;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}
CMsLuaManager::~CMsLuaManager(VOID)
{
    _ENTER_FUN_C;

    // 布尔类型返回值
    BOOL bRet = TRUE;

    // 停止监控LUA文件变化线程
    bRet = this->StopCheckLuaFileIsChange();
    AssertEx(AL_NORMAL, bRet,_T("StopCheckLuaFileIsChange失败,函数失败"));

    // 清除Lua
    if (m_pExplainOrgan)
    {
        lua_close(m_pExplainOrgan);
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
}

// 遍历到符合条件的文件时所调用的通知函数
BOOL CMsLuaManager::ProcessFile(CONST TCHAR* strFileName)
{
    _ENTER_FUN_C;

    // 加载找到的LUA文件
    LoadLuaFile(strFileName);

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 遍历进入新目录时所调用的通知函数
BOOL CMsLuaManager::ProcessPath(CONST TCHAR* /*strCurrentPath*/)
{
    return TRUE;
}

BOOL CMsLuaManager::Init(VOID)
{
    _ENTER_FUN_C;

    BOOL bRet = TRUE;

    // 初始化Lua
    m_pExplainOrgan = lua_open();

    // 载入Lua基本库
    luaL_openlibs(m_pExplainOrgan);

    // 设置查找文件类型
    bRet = this->SetFindFileType(_T("lua"));
    AssertEx(AL_NORMAL, bRet, _T("设置查找lua文件类型失败,"));

    // 注册系统默认LTC函数
    bRet = this->RegisterCFunToLua(NULL, 0);
    AssertEx(AL_NORMAL, bRet, _T("注册系统默认LTC函数失败,"));

    //if (bNeedWinApi)
    //{
    //    luaL_Reg reg;
    //    reg.func = open_dll;
    //    reg.name = "lua_open_dll";
    //    this->RegisterCFunToLua(reg);

    //    reg.func = get_procaddress;
    //    reg.name = "lua_get_procaddress";
    //    this->RegisterCFunToLua(reg);

    //    reg.func = CreateRect;
    //    reg.name = "lua_CreateRect";
    //    this->RegisterCFunToLua(reg);
    //}

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 启动脚本调试
VOID CMsLuaManager::EnableDebug(BOOL bEnable, lua_Hook lpLuaHookCallBackFun, DWORD dwDebugFlag, DWORD dwRunCount)
{
    if (bEnable)
    {
        lua_sethook(m_pExplainOrgan, lpLuaHookCallBackFun, dwDebugFlag, dwRunCount);
    }
    else
    {
        lua_sethook(m_pExplainOrgan, lpLuaHookCallBackFun, 0, 0);
    }
}

BOOL CMsLuaManager::LoadLuaFile(CONST TCHAR* strLuaFileName)
{
    _ENTER_FUN_C;

    TCharToAChar(strLuaFileName, m_strLuaFileName, MAX_PATH);

    INT nRet = TRUE;

    LUA_FUN_RET_VALUE pLuaRet(LUA_RET_STRING);

    if (strLuaFileName)
    {
        // 运行脚本
        nRet = luaL_loadfile(m_pExplainOrgan, m_strLuaFileName);
        switch (nRet)
        {
        case LUA_OK:
            {
                luaL_dofile(m_pExplainOrgan, m_strLuaFileName);
            }break;
        case LUA_ERRSYNTAX:
            {
                // 获取错误信息
                pLuaRet.m_String.m_str = 
                    lua_tolstring(m_pExplainOrgan, -1, &pLuaRet.m_String.m_Size);

                MESSAGEBOXA(pLuaRet.m_String.m_str);
                return FALSE;
            }break;
        case LUA_NOEXIST:
            {
                fAssertExA(AL_NORMAL, FALSE,"脚本文件不存在\n%s", m_strLuaFileName);
            }break;
        default:
            {
                fAssertExA(AL_NORMAL, FALSE,"脚本文件加载失败,未知错误\n%s", m_strLuaFileName);
            }break;
        }
    }
    else
    {
        AssertEx(AL_NORMAL, m_strLuaFileName,_T("strLuaFileName不能为空"));
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

INT CMsLuaManager::RunLuaFun(
    LUA_FUN_RET_VALUE* pLuaRet,
    DWORD dwParamCount,
    TCHAR* strLuaFunName,
    TCHAR* strLuaParameter,
    ...
    )
{
    // 定义参数列表
    va_list vaList;
    // 根据固定参数寻找参数地址
    va_start(vaList, strLuaParameter);
    return this->RunLuaFun_Va(pLuaRet, dwParamCount, strLuaFunName, strLuaParameter, vaList);
}

BOOL CMsLuaManager::RunLuaFun_Va(
    LUA_FUN_RET_VALUE* pLuaRet,
    DWORD dwParamCount,
    TCHAR* strLuaFunName,
    TCHAR* strLuaParameter,
    va_list vaList
    )
{
    _ENTER_FUN_C;

    // LUA脚本传入参数个数
    INT nLuaParameterCount = 0;

    if (strLuaFunName)
    {
        TCharToAChar(strLuaFunName, m_strLuaFileName, MAX_PATH);

        // 压入LUA错误处理函数
        lua_getglobal(m_pExplainOrgan, LUA_ERROR_DISPOSE_FUN_NAME);

        // 将要调用的函数压入堆栈
        lua_getglobal(m_pExplainOrgan, m_strLuaFileName);
    }

    if (strLuaParameter)
    {
        TCharToAChar(strLuaParameter, m_strLuaParameter, MAX_PATH);
        ACHAR* lpParameter = m_strLuaParameter;

        // 根据固定参数查找后续参数
        //strFindNextParameter = strLuaLoadFunBuff;

        while(*lpParameter)
        {
            if('%' == *lpParameter)
            {
                lpParameter++;
                switch(*lpParameter)
                {
                case 's':
                    {
                        // 根据%s确定下一个参数为字符串型
                        // 将参数以此压入堆栈
                        lua_pushstring(m_pExplainOrgan, va_arg(vaList, ACHAR*));

                        // 传入参数个数++
                        nLuaParameterCount++;
                    }break;
                case 'd':
                    {
                        // 根据%s确定下一个参数为整型
                        // 将参数以此压入堆栈
                        lua_pushinteger(m_pExplainOrgan, va_arg(vaList, INT));

                        // 传入参数个数++
                        nLuaParameterCount++;
                    }break;
                case 'f':
                    {
                        // 根据%s确定下一个参数为浮点型
                        // 将参数以此压入堆栈
                        lua_pushnumber(m_pExplainOrgan, va_arg(vaList, DOUBLE));

                        // 传入参数个数++
                        nLuaParameterCount++;
                    }break;
                case 'c':
                    {
                        // 根据%s确定下一个参数为字符型
                        // 将参数以此压入堆栈
                        lua_pushinteger(m_pExplainOrgan, va_arg(vaList, ACHAR));

                        // 传入参数个数++
                        nLuaParameterCount++;
                    }break;
                case 'b':
                    {
                        // 根据%s确定下一个参数为布尔型
                        // 将参数以此压入堆栈
                        lua_pushboolean(m_pExplainOrgan, va_arg(vaList, BOOL));

                        // 传入参数个数++
                        nLuaParameterCount++;
                    }break;
                default:
                    {
                        AssertEx(AL_NORMAL, FALSE, _T("无法识别的传参类型"));
                    }break;
                }
            }
            else
            {
                lpParameter++;
                continue;
            }
        }
    }
    // 结束任意参数列表
    va_end(vaList);

    // 调用函数，传入参数个数与返回值个数
    INT nLuaCallRet = lua_pcall(
        m_pExplainOrgan,
        nLuaParameterCount,
        1,
        nLuaParameterCount + 1
        );

    switch(nLuaCallRet)
    {
    case LUA_OK:
        break;
    case LUA_YIELD:
        {
            AssertEx(AL_NORMAL, FALSE, _T("LUA_YIELD,"));
        }break;
    case LUA_ERRRUN:
        {
            AssertEx(AL_NORMAL, FALSE, _T("运行时错误,"));
        }break;
    case LUA_ERRSYNTAX:
        {
            AssertEx(AL_NORMAL, FALSE, _T("错误的LUA语法,"));
        }break;
    case LUA_ERRMEM:
        {
            AssertEx(AL_NORMAL, FALSE, _T("内存分配错误,"));
        }break;
    case LUA_ERRERR:
        {
            fAssertExA(AL_NORMAL, FALSE, "在运行错误处理函数时发生的错误\n%s", lua_tostring(m_pExplainOrgan, -1));
        }break;
    default:
        {
            AssertEx(AL_NORMAL, FALSE, _T("未知LUA错误,"));
        }break;
    }

    FOR_x_TO(i, dwParamCount)
    {
        switch(pLuaRet[i].m_eType)
        {
        case LUA_RET_INT:
            {
                // 获取返回值
                pLuaRet[i].m_lLuaFunRet = lua_tointeger(m_pExplainOrgan, -1);
                lua_pop(m_pExplainOrgan, 1);
            }break;
        case LUA_RET_DOUBLE:
            {
                // 获取返回值
                pLuaRet[i].m_dLuaFunRet = lua_tonumber(m_pExplainOrgan, -1);
                lua_pop(m_pExplainOrgan, 1);
            }break;
        case LUA_RET_BOOL:
            {
                // 获取返回值
                pLuaRet[i].m_bLuaFunRet = lua_toboolean(m_pExplainOrgan, -1);
                lua_pop(m_pExplainOrgan, 1);
            }break;
        case LUA_RET_STRING:
            {
                // 获取返回值
                pLuaRet[i].m_String.m_str = 
                    lua_tolstring(m_pExplainOrgan, -1, &pLuaRet->m_String.m_Size);
                lua_pop(m_pExplainOrgan, 1);
            }break;
        case LUA_RET_VOID:
        default:
            {
                // 获取返回值
                pLuaRet[i].m_lpVoid = lua_touserdata(m_pExplainOrgan, -1);
                lua_pop(m_pExplainOrgan, 1);
            }break;
        }
    }
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLuaManager::SetLuaFileRootPath(TCHAR* strLuaFilePath)
{
    _ENTER_FUN_C;

    if (NULL == strLuaFilePath)
    {
        // 设置查找文件目录
        SetRootPath();

        // 遍历目录加载文件
        BrowsePath();

        // 启动LUA文件变化检测
        BeginCheckLuaFileIsChange();

        // 检测传入参数是否为空
        AssertEx(AL_NORMAL, m_szCurrentDirectory, _T("m_szCurrentDirectory为空,错误传参"));

        // 获取新路径长度
        SIZE_T sFilePathLen = _tcslen(m_szCurrentDirectory);

        // 检测新路径长度是否越界
        AssertEx(AL_NORMAL, sFilePathLen < MAX_PATH, _T("传参有误,越界"));

        // 拷贝新路径到成员变量
        _tcsncpy_s(
            m_szTLuaFilePath,
            MAX_PATH,
            m_szCurrentDirectory,
            sFilePathLen
            );
    }
    else
    {
        // 设置查找文件目录
        this->SetRootPath(strLuaFilePath);

        // 遍历目录加载文件
        this->BrowsePath();

        // 启动LUA文件变化检测
        this->BeginCheckLuaFileIsChange();

        // 检测传入参数是否为空
        AssertEx(AL_NORMAL, strLuaFilePath, _T("strLuaFilePath为空,错误传参"));

        // 获取新路径长度
        SIZE_T sFilePathLen = _tcslen(strLuaFilePath);
        AssertEx(AL_NORMAL, sFilePathLen < MAX_PATH, _T("nFilePathLen越界,错误传参"));

        // 拷贝新路径到成员变量
        _tcsncpy_s(m_szTLuaFilePath, MAX_PATH, strLuaFilePath, sFilePathLen);
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 开始检测LUA文件是否有改变
BOOL CMsLuaManager::BeginCheckLuaFileIsChange(VOID)
{
    _ENTER_FUN_C;

    // 创建LUA脚本文件监控线程
    m_hThread = ::CreateThread(
        NULL,
        0,
        CheckLuaFileIsChangeThreadProc,
        (LPVOID)this,
        0,
        &m_nThreadId
        );

    AssertEx(AL_NORMAL, m_hThread, _T("创建LUA脚本文件监控Thread失败,函数失败"));

    // 将检查中的标记置位
    m_IsChecking = TRUE;

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 停止检测LUA文件是否有改变
BOOL CMsLuaManager::StopCheckLuaFileIsChange(VOID)
{
    _ENTER_FUN_C;

    // 布尔类型返回值
    BOOL bRet = TRUE;

    // 将检查中的标记清除
    m_IsChecking = FALSE;

    // 检查线程是否已经停止
    if(m_hThread && m_nThreadId)
    {
        // 关闭线程
        bRet = ::CloseHandle(m_hThread);
        AssertEx(
            AL_NORMAL,
            bRet,
            _T("关闭CheckLuaFileIsChangeThreadProc线程失败,函数失败")
            );

        // 将线程ID,线程句柄置空
        m_nThreadId = 0;
        m_hThread   = NULL;
    }
    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

// 注册C++函数到LUA
BOOL CMsLuaManager::RegisterCFunToLua(luaL_Reg& NewRegLTC)
{
    _ENTER_FUN_C;

    // 测试C++函数注册
    lua_pushcfunction(m_pExplainOrgan, NewRegLTC.func);
    lua_setglobal(m_pExplainOrgan, NewRegLTC.name);

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

BOOL CMsLuaManager::RegisterCFunToLua(luaL_Reg* pRegLTC, UINT uRegLTCCount)
{
    _ENTER_FUN_C;

    BOOL bRet = TRUE;
    // 按系统默认注册LTC函数
    if ((0 == uRegLTCCount) || (pRegLTC == NULL))
    {
        // 获取系统默认注册LTC数组指针与个数
        pRegLTC         = g_RegLTCArray;
        uRegLTCCount    = sizeof(g_RegLTCArray) / sizeof(luaL_Reg);
    }
    // 自定义注册LTC函数
    else
    {
        // 获取自定义注册LTC数组指针与个数
        pRegLTC         = pRegLTC;
        uRegLTCCount    = uRegLTCCount;
    }

    // 循环对LTC函数注册数组进行注册
    while((pRegLTC->func != NULL) && (pRegLTC->name != NULL))
    {
        // 检测是否注册失败
        bRet = this->RegisterCFunToLua(*pRegLTC);
        AssertEx(AL_NORMAL, bRet, _T("CTL函数注册失败,函数失败"));
        pRegLTC++;
    }

    // 返回成功
    return TRUE;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}


BOOL CMsLuaManager::OnLuaFileIsChange(TCHAR* strLuaFileName)
{
    _ENTER_FUN_C;

    // 重新加载指定改变的LUA文件
    TCHAR szLuaFilePath[MAX_PATH] = {0};

    if (strLuaFileName)
    {
        {
            _tcscpy_s(szLuaFilePath, MAX_PATH, m_szTLuaFilePath);

            INT nPathLen = (INT)_tcslen(szLuaFilePath);
            // 如果最后一位不是'\\'
            if (szLuaFilePath[nPathLen - 1] != _T('\\') && 
                szLuaFilePath[nPathLen - 1] != _T('/'))
            {
                // 最后一位更改为'\\'
                szLuaFilePath[nPathLen] = _T('\\');

                // 最后一位追加一个'\0'
                szLuaFilePath[nPathLen + 1] = _T('\0');
            }
        }

        {
            _tcscat_s(szLuaFilePath, MAX_PATH, strLuaFileName);
        }
        return this->LoadLuaFile(szLuaFilePath);
    }

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return FALSE;
}

DWORD WINAPI CheckLuaFileIsChangeThreadProc(LPVOID lpParameter)
{
    _ENTER_FUN_C;

    // 布尔类型返回值
    BOOL bRet = TRUE;

    // 打开监控目录,得到目录的句柄
    ((CMsLuaManager*)lpParameter)->m_hFilePath = CreateFile(
        ((CMsLuaManager*)lpParameter)->m_szTLuaFilePath,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
        ); 

    // 检查目录是否打开成功
    AssertEx(
        AL_NORMAL,
        ((CMsLuaManager*)lpParameter)->m_hFilePath,
        _T("LUA脚本目录打开失败,")
        );

    DWORD dwBytesReturned = 0;        
    while(((CMsLuaManager*)lpParameter)->m_IsChecking)
    {
        if(::ReadDirectoryChangesW(

            // 监控的文件路径
            ((CMsLuaManager*)lpParameter)->m_hFilePath, 

            // 监控通知结构体
            ((CMsLuaManager*)lpParameter)->m_pLuaChangeNotify,

            // 通知结构体大小
            sizeof(((CMsLuaManager*)lpParameter)->m_pLuaChangeNotifyBuff),

            // 是否监控子目录
            TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME|   // 监控文件改名
            FILE_NOTIFY_CHANGE_DIR_NAME|
            FILE_NOTIFY_CHANGE_ATTRIBUTES|
            FILE_NOTIFY_CHANGE_SIZE|        // 监控文件大小改变
            FILE_NOTIFY_CHANGE_LAST_WRITE|  // 监控文件最后写入改变
            FILE_NOTIFY_CHANGE_LAST_ACCESS|
            FILE_NOTIFY_CHANGE_CREATION|    // 监控文件创建
            FILE_NOTIFY_CHANGE_SECURITY,
            &dwBytesReturned,
            NULL,
            NULL))
        {
            switch(((CMsLuaManager*)lpParameter)->m_pLuaChangeNotify->Action)
            {
            case FILE_ACTION_ADDED:
                {
                    //MESSAGEBOX(AL_SHOW_ONLY, _T("添加了新文件"));
                }break;
            case FILE_ACTION_REMOVED:
                {
                    //MESSAGEBOX(AL_SHOW_ONLY, _T("删除了文件"));
                }break;
            case FILE_ACTION_MODIFIED:
                {
                    // 获取文件路径字符串长度
                    DWORD dwFileNameLength = (((CMsLuaManager*)lpParameter)->
                        m_pLuaChangeNotify->
                        FileNameLength)/(DWORD)sizeof(TCHAR);

                    // 检查文件路径字符串长度
                    AssertEx(
                        AL_NORMAL,
                        (dwFileNameLength < MAX_PATH), _T("文件路径长度越界,"));

                    // 根据返回的文件路径长度截断字符串
                    ((CMsLuaManager*)lpParameter)->
                        m_pLuaChangeNotify->FileName[dwFileNameLength] = '\0';

                    // 宽字符转换成多字符
                    WCharToAChar(
                        ((CMsLuaManager*)lpParameter)->
                        m_pLuaChangeNotify->FileName,
                        ((CMsLuaManager*)lpParameter)->m_strLuaFileName,
                        MAX_PATH
                        );

                    ((CMsLuaManager*)lpParameter)->OnLuaFileIsChange(
                        ((CMsLuaManager*)lpParameter)->
                        m_pLuaChangeNotify->FileName
                        );

                }break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                {
                    MESSAGEBOX(AL_SHOW_ONLY, _T("重命名了文件"));
                }break;
            default:
                {
                    MESSAGEBOX(AL_SHOW_ONLY, _T("未知文件变化"));
                }break;
            }        
        }
    }

    // 关闭当前线程
    bRet = ::CloseHandle(((CMsLuaManager*)lpParameter)->m_hThread);
    AssertEx(AL_NORMAL, bRet, _T("关闭CheckLuaFileIsChangeThreadProc线程失败,函数失败"));

    // 将线程ID,线程句柄置空
    ((CMsLuaManager*)lpParameter)->m_nThreadId = 0;
    ((CMsLuaManager*)lpParameter)->m_hThread   = NULL;

    // 返回成功
    return 0;

    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return LUA_ERROR;
}

//int __fastcall api_call(lua_State *L)
//{
//    int n = lua_gettop(L);
//    func_call fc = (func_call)lua_touserdata(L, lua_upvalueindex(2));
//    DWORD arg;
//    for (int type, i = n; i > 0; i--)
//    {
//        type = lua_type(L, i);
//        switch(type)
//        {
//        case LUA_TNIL:
//            {
//                arg = 0;
//            }break;
//        case LUA_TNUMBER:
//            {
//                arg = (DWORD)lua_tointeger(L,i);
//            }break;
//        case LUA_TBOOLEAN:
//            {
//                arg = (DWORD)lua_toboolean(L,i);
//            }break;
//        case LUA_TSTRING:
//            {
//                arg = (DWORD)lua_tostring(L,i);
//            }break;
//        case LUA_TLIGHTUSERDATA:
//            {
//                arg = (DWORD)lua_touserdata(L,i);
//            }break;
//        default:
//            {
//                lua_pushstring(L,"unknown argument type");
//                lua_error(L);
//            }break;
//        }
//        __asm
//        {
//            push arg;
//        }
//    }
//    int* pRet;
//    __asm
//    {
//        call fc;
//        mov pRet,eax;
//    }
//    lua_pushinteger(L, (int)pRet);
//
//    return 1;
//}

//int __fastcall open_dll(lua_State *L)
//{
//    CONST ACHAR* Name = lua_tostring(L,1);
//
//#ifdef UNICODE
//    TCHAR strFileName[MAX_PATH] = {0};
//    ::MultiByteToWideChar(
//        CP_ACP, 
//        0, 
//        Name,
//        strlen(Name) + 1,
//        strFileName,
//        MAX_PATH);
//#else
//    CONST ACHAR* strFileName = Name;
//#endif
//    HMODULE hm = ::LoadLibrary(strFileName);
//    lua_pushlightuserdata(L, hm);
//    return 1;
//}
//
//int __fastcall get_procaddress(lua_State *L)
//{
//    HMODULE hm          = (HMODULE)lua_touserdata(L, 1);
//    const char *name    = lua_tostring(L, 2);
//    void *func          = ::GetProcAddress(hm, name);
//    lua_pushvalue(L, 3);
//    lua_pushlightuserdata(L, func);
//    lua_pushcclosure(L, api_call, 2);
//    return 1;
//}