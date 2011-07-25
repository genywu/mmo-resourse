#pragma once              //┃  //┃
#ifndef __LUAMANAGER_H__  //┃  //┃
#define __LUAMANAGER_H__  //┃  //┃
//━━━━━━━━━━━━━┛  //┃
//文件名称：LuaManager.h        //┃
//功能描述：Lua脚本管理器       //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

#include "MsLuaLTCFun.h"

// LUA返回错误
#define LUA_ERROR               0xFFFFFFFF

// LUA脚本函数名最大尺寸
#define MAX_LUA_FUN_NAME_LEN    255

// LUA文件改变检测通知缓冲区大小
#define MAX_LUA_FILE_CHANGE_NOTIFY_BUFF_LEN 1024

enum RETURN_VALUE_TYPE
{
    LUA_RET_VOID = 0,
    LUA_RET_INT,
    LUA_RET_DOUBLE,
    LUA_RET_BOOL,
    LUA_RET_STRING,
};

typedef struct LUA_FUN_RET_VALUE
{
    LUA_FUN_RET_VALUE(RETURN_VALUE_TYPE type):m_eType(type){}
    union
    {
        SIZE_T OUT  m_lLuaFunRet;
        DOUBLE OUT  m_dLuaFunRet;
        BOOL   OUT  m_bLuaFunRet;
        VOID*  OUT  m_lpVoid;
        struct STR
        {
            size_t       OUT  m_Size;
            CONST ACHAR* OUT  m_str;
        }m_String;
    };
    RETURN_VALUE_TYPE   m_eType;
}LFRV;


class CMsLuaManager:
    public CMsFindFileManager
{
public:
    CMsLuaManager();
    virtual ~CMsLuaManager(VOID);

    // 重载CMsFindFileManager中的通知消息函数
public:
    // 遍历到符合条件的文件时所调用的通知函数
    virtual BOOL    ProcessFile(CONST TCHAR* strFileName);

    // 遍历进入新目录时所调用的通知函数
    virtual BOOL    ProcessPath(CONST TCHAR* strCurrentPath);

    // lua脚本管理器公开成员函数
public:
    // 初始化
    BOOL Init(VOID);

    // 启动脚本调试
    VOID EnableDebug(BOOL bEnable, lua_Hook lpLuaHookCallBackFun, DWORD dwDebugFlag, DWORD dwRunCount = 0);

    BOOL LoadLuaFile(CONST TCHAR* strLuaFileName);

    BOOL RunLuaFun(
        LUA_FUN_RET_VALUE* pLuaRet,
        DWORD dwParamCount,
        TCHAR* strLuaFunName,
        TCHAR* strLuaParameter,
        ...
        );

    BOOL RunLuaFun_Va(
        LUA_FUN_RET_VALUE* pLuaRet,
        DWORD dwParamCount,
        TCHAR* strLuaFunName,
        TCHAR* strLuaParameter,
        va_list vaList
        );

    // 设置LUA文件根目录
    BOOL    SetLuaFileRootPath(TCHAR* strLuaFilePath = NULL);

    // 开始检测LUA文件是否有改变
    BOOL    BeginCheckLuaFileIsChange(VOID);

    // 停止检测LUA文件是否有改变
    BOOL    StopCheckLuaFileIsChange(VOID);

    // 检测到LUA文件已经修改
    BOOL    OnLuaFileIsChange(TCHAR* strLuaFileName);

    // 注册C++函数到LUA
    BOOL    RegisterCFunToLua(luaL_Reg& NewRegLTC);

    // 默认注册系统全局数组中的LTC函数请使用默认参数
    BOOL    RegisterCFunToLua(luaL_Reg* pRegLTC, UINT uRegLTCCount);

    // 设置检测LUA文件修改线程为友元函数
    friend DWORD WINAPI CheckLuaFileIsChangeThreadProc(LPVOID lpParameter);
protected:
    // Lua解释器指针
    lua_State* m_pExplainOrgan;

    // 定义LUA函数名缓冲区
    //ACHAR m_strLuaFunName[MAX_LUA_FUN_NAME_LEN];

    // LUA文件改变检测通知缓冲区
    //ACHAR m_ChangeNotify[MAX_LUA_FILE_CHANGE_NOTIFY_BUFF_LEN];  

    // LUA文件根目录
    TCHAR m_szTLuaFilePath[MAX_PATH];

    // LUA文件根目录文件句柄
    HANDLE m_hFilePath;

    // LUA文件监控通知缓冲区
    FILE_NOTIFY_INFORMATION* m_pLuaChangeNotify;
    ACHAR m_pLuaChangeNotifyBuff[(sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH)];

    // LUA文件监控线程ID
    DWORD m_nThreadId;

    // LUA文件监控线程句柄
    HANDLE m_hThread;

    // 标记LUA文件是否监控中
    BOOL m_IsChecking;

    // 改变的LUA文件路径
    ACHAR m_strLuaFileName[MAX_PATH];
    ACHAR m_szALuaFilePath[MAX_PATH];
    ACHAR m_strLuaParameter[MAX_PATH];

    lua_Hook m_lpLuaHookCallBackFun;

private:
    CMsLuaManager& operator = (CMsLuaManager&){};
};

//typedef void* (__stdcall *func_call)();
//int __fastcall api_call(lua_State *L);
//int __fastcall open_dll(lua_State *L);
//int __fastcall get_procaddress(lua_State *L);

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
