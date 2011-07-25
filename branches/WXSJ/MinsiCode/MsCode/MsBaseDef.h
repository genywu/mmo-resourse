#pragma once              //┃                                          //┃
#ifndef __MSBASEDEF_H__   //┃                                          //┃
#define __MSBASEDEF_H__   //┃                                          //┃
//━━━━━━━━━━━━━┛                                          //┃
//文件名称：MsBaseDef.h                                                 //┃
//功能描述：基本类型定义，基本头文件包含，基本宏定义，全局初始化、释放  //┃
//平台相关：WINDOWS                                                     //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
#ifndef I_KNOW_THE_BASE_TYPE                                            //┃
#error 请查看MyBaseType.h文件后，增加【I_KNOW_THE_BASE_TYPE】宏去除本错误
#endif                                                                  //┃
                                                                        //┃
#ifdef NO_COMPILER_MESSAGE                                              //┃
#pragma message("注意:您已禁用编译消息!")                               //┃
#pragma message("想查看编译进度,请去掉【NO_COMPILER_MESSAGE】")         //┃
#endif                                                                  //┃
#if defined(DLL_EXPORTS)                                                //┃
#define USE_DLL __declspec(dllexport)                                   //┃
#else                                                                   //┃
#define USE_DLL __declspec(dllimport)                                   //┃
#endif                                                                  //┃
                                                                        //┃
#ifndef WINVER                                                          //┃
#define WINVER 0x0500                                                   //┃
#endif                                                                  //┃
                                                                        //┃
// 不使用MFC                                                            //┃
#ifndef USE_MFC                                                         //┃
// 网络套接字                                                           //┃
#include <Winsock2.h>                                                   //┃
#define NO_MFC_CODE(code) code                                          //┃
#else                                                                   //┃
#include <afxsock.h>    // MFC 套接字扩展                               //┃
#define NO_MFC_CODE(code)                                               //┃
#endif                                                                  //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


//━━━━━━━━━━━━━━━━━━━━━━┓
#ifdef USE_MS_DX                            //┃
#define USE_MS_QUEUE_MANAGER                //┃
#include "d3d9.h"                           //┃
#include "d3dx9.h"                          //┃
#include "d3d10.h"                          //┃
#include "d3dx10.h"                         //┃
#include "dds.h"                            //┃
#ifndef MS_LIB                              //┃
#pragma comment(lib, "d3d9.lib")            //┃
#pragma comment(lib, "d3d10.lib")           //┃
#pragma comment(lib, "dxguid.lib")          //┃
#pragma comment(lib, "dinput8.lib")         //┃
#   ifdef _DEBUG                            //┃
#       pragma comment(lib, "d3dx9d.lib")   //┃
#       pragma comment(lib, "d3dx10d.lib")  //┃
#   else                                    //┃
#       pragma comment(lib, "d3dx9.lib")    //┃
#       pragma comment(lib, "d3dx10.lib")   //┃
#   endif                                   //┃
#endif                                      //┃
#endif                                      //┃
//━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━┓
//包含基本定义头文件        //┃
//--------------------------//┃
// 标准输入输出             //┃
#include <Stdio.h>          //┃
                            //┃
// C++库文件                //┃
#include <Stdlib.h>         //┃
                            //┃
// 字符头                   //┃
#include <Tchar.h>          //┃
                            //┃
// 字符串头                 //┃
#include <String>           //┃
                            //┃
// GDI+头文件               //┃
#include <Gdiplus.h>        //┃
                            //┃
// MFC_DAO                  //┃
#ifdef USE_MFC              //┃
#define _AFX_DISABLE_DEPRECATED
#include <afx.h>            //┃
#else                       //┃
                            //┃
// 标准错误                 //┃
#include <Errno.h>          //┃
                            //┃
// 标准输入输出             //┃
#include <Iostream>         //┃
                            //┃
// 时间头                   //┃
#include <Time.h>           //┃
                            //┃
// 数学公式头               //┃
#include <Math.h>           //┃
                            //┃
// 队列头                   //┃
#include <Queue>            //┃
                            //┃
// 断言头                   //┃
#include <Assert.h>         //┃
                            //┃
// 线程头                   //┃
#include <Process.h>        //┃
                            //┃
// 控制台相关               //┃
#include <Conio.h>          //┃
                            //┃
// Windows头文件            //┃
#include <Windows.h>        //┃
                            //┃
// Windows进程管理          //┃
#include <Psapi.h>          //┃
                            //┃
// Windows辅助工具          //┃
#include <Tlhelp32.h>       //┃
                            //┃
#endif                      //┃
//━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━┓
//标准数据类型定义                              //┃
                                                //┃
typedef double      DOUBLE;     //双精度        //┃
typedef INT64       IP;         //IP类型        //┃
typedef INT64       ID;         //ID类型        //┃
typedef INT         NFLAG;      //标记类型      //┃
typedef INT         NPOS;       //坐标类型      //┃
typedef INT         NSIZE;      //尺寸类型      //┃
typedef UINT        UFLAG;      //标记类型      //┃
typedef UINT        UPOS;       //坐标类型      //┃
typedef UINT        USIZE;      //尺寸类型      //┃
typedef FLOAT       FFLAG;      //标记类型      //┃
typedef FLOAT       FPOS;       //坐标类型      //┃
typedef FLOAT       FSIZE;      //尺寸类型      //┃
typedef CHAR        ACHAR;      //多字符重定义  //┃
typedef TCHAR*      LPTCHAR;    //宽(多)字符    //┃
typedef ACHAR*      LPCHAR;     //字符指针      //┃
typedef WCHAR*      LPWCHAR;    //宽字符指针    //┃
typedef SHORT*      LPSHORT;    //短整指针      //┃
typedef INT*        LPINT;      //整型指针      //┃
typedef LONG*       LPLONG;     //长整指针      //┃
typedef INT64*      LPINT64;    //64整指针      //┃
typedef FLOAT*      LPFLOAT;    //浮点指针      //┃
typedef DOUBLE*     LPDOUBLE;   //双精指针      //┃
typedef UCHAR*      LPUCHAR;    //无符号字符    //┃
typedef USHORT*     LPUSHORT;   //无符号短整    //┃
typedef UINT*       LPUINT;     //无符号整型    //┃
typedef ULONG*      LPULONG;    //无符号长整    //┃
typedef UINT64*     LPUINT64;   //无符号64整    //┃
typedef BOOL*       LPBOOL;     //布尔指针      //┃
typedef BYTE*       LPBYTE;     //字节指针      //┃
typedef IP*         LPIP;       //IP类型指针    //┃
typedef ID*         LPID;       //ID类型指针    //┃
typedef NFLAG*      LPNFLAG;    //标记类型      //┃
typedef NPOS*       LPNPOS;     //坐标类型      //┃
typedef UFLAG*      LPUFLAG;    //标记类型      //┃
typedef UPOS*       LPUPOS;     //坐标类型      //┃
//━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━┓
// 错误代码                     //┃
#include "MsBaseErrorCode.h"    //┃
                                //┃
// 线程消息                     //┃
#include "MsThreadMessageDef.h" //┃
//━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//基本数据宏定义                                        //┃
                                                        //┃
//IP地址的字符最大长度                                  //┃
#ifndef IP_SIZE                                         //┃
#define IP_SIZE         16                              //┃
#endif                                                  //┃
                                                        //┃
//无效的句柄                                            //┃
#ifndef INVALID_HANDLE                                  //┃
#define INVALID_HANDLE  INVALID_HANDLE_VALUE            //┃
#endif                                                  //┃
                                                        //┃
//无效的ID值                                            //┃
#define INVALID_NID     -1                              //┃
#define INVALID_UID     (DWORD)-1                       //┃
#define INVALID_WID     (WORD)-1                        //┃
#define INVALID_BID     (BYTE)-1                        //┃
                                                        //┃
#define GB_SIZE         (MB_SIZE*1024)                  //┃
#define MB_SIZE         (KB_SIZE*1024)                  //┃
#define KB_SIZE         (BYTE_SIZE*1024)                //┃
#define BYTE_SIZE       (sizeof(BYTE))                  //┃
                                                        //┃
//无效的指针值                                          //┃
#ifndef INVALID_POINTER                                 //┃
#define INVALID_POINTER (VOID*)INVALID_NID              //┃
#endif                                                  //┃
                                                        //┃
// 宏定义参数引用                                       //┃
#define USE_PARAM(...)(__VA_ARGS__)                     //┃
                                                        //┃
#ifdef UNICODE                                          //┃
#define _tcout std::wcout                               //┃
#define _tcin  std::wcin                                //┃
#else                                                   //┃
#define _tcout std::cout                                //┃
#define _tcin  std::cin                                 //┃
#endif                                                  //┃
                                                        //┃
//错误文本长度                                          //┃
#ifndef MAX_ERROR_TEXT_LEN                              //┃
#define MAX_ERROR_TEXT_LEN      160                     //┃
#endif                                                  //┃
                                                        //┃
// 数字转换到字符串所需长度                             //┃
#define WORD_NUMBER_SIZE        6                       //┃
#define DWORD_NUMBER_SIZE       11                      //┃
#define FWORD_NUMBER_SIZE       21                      //┃
#define MAX_USER_NAME_LEN       20                      //┃
#define MAX_USER_PASSWORD_LEN   32                      //┃
                                                        //┃
// 进制宏定义                                           //┃
#define RADIX_2 2       // 2进制的宏定义                //┃
#define RADIX_8 8       // 8进制的宏定义                //┃
#define RADIX_10 10     // 10进制的宏定义               //┃
#define RADIX_16 16     // 16进制的宏定义               //┃
                                                        //┃
// 16位数字转换8进制字符串最大长度                      //┃
#define N16_TO_S_MAX_LEN_BY_R8  7                       //┃
                                                        //┃
// 16位数字转换10进制字符串最大长度                     //┃
#define N16_TO_S_MAX_LEN_BY_R10 6                       //┃
                                                        //┃
// 16位数字转换16进制字符串最大长度                     //┃
#define N16_TO_S_MAX_LEN_BY_R16 5                       //┃
                                                        //┃
// 32位数字转换8进制字符串最大长度                      //┃
#define N32_TO_S_MAX_LEN_BY_R8  12                      //┃
                                                        //┃
// 32位数字转换10进制字符串最大长度                     //┃
#define N32_TO_S_MAX_LEN_BY_R10 11                      //┃
                                                        //┃
// 32位数字转换16进制字符串最大长度                     //┃
#define N32_TO_S_MAX_LEN_BY_R16 9                       //┃
                                                        //┃
// 64位数字转换8进制字符串最大长度                      //┃
#define N64_TO_S_MAX_LEN_BY_R8  23                      //┃
                                                        //┃
// 64位数字转换10进制字符串最大长度                     //┃
#define N64_TO_S_MAX_LEN_BY_R10 21                      //┃
                                                        //┃
// 64位数字转换16进制字符串最大长度                     //┃
#define N64_TO_S_MAX_LEN_BY_R16 17                      //┃
                                                        //┃
#define FOR_x_TO(x,end) for(DWORD x=0; x<end; x++)      //┃
                                                        //┃
#define NO_CONTINUOUS   ::SleepEx(1, TRUE)              //┃
                                                        //┃
#ifdef _DEBUG                                           //┃
#define _DEBUG_CODE(code) code                          //┃
#else                                                   //┃
#define _DEBUG_CODE(code)                               //┃
#endif                                                  //┃
                                                        //┃
                                                        //┃
#define MS_LOCK_BEGIN   // 此宏定义开始说明此段代码具有 //┃
                    // 特殊含义,擅自改动将付法律责任!   //┃
                                                        //┃
#define MS_LOCK_END     // 此宏定义之前说明此段代码具有 //┃
                    // 特殊含义,擅自改动将付法律责任!   //┃
                                                        //┃
#define MS_LOFTY    // 此宏定义代表此处处于高级调用,    //┃
                    // 请详细了解上下文架构后再使用.    //┃
                                                        //┃
#define P_OPER_S(p1,p2) ((SIZE_T)(p1)-(SIZE_T)(p2))     //┃
#define P_OPER_A(p1,p2) ((SIZE_T)(p1)+(SIZE_T)(p2))     //┃
#define P_OPER_D(p1,p2) ((SIZE_T)(p1)/(SIZE_T)(p2))     //┃
#define P_OPER_M(p1,p2) ((SIZE_T)(p1)*(SIZE_T)(p2))     //┃
                                                        //┃
// 大BOOL类型转换到小bool类型的宏定义                   //┃
#define BOOL_To_bool(val)           ((val)?true:false)  //┃
                                                        //┃
#define MSCALL  __fastcall                              //┃
                                                        //┃
#define MS_DEPRECATEE           __declspec(deprecated)  //┃        
                                                        //┃
typedef INT (MSCALL *RUNCALLBACK)(INT,LPTCHAR*);        //┃
typedef BOOL (MSCALL *HOOKWNDPROC)(MSG*);               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//基本操作宏定义                                                        //┃
                                                                        //┃
//根据指针值删除内存                                                    //┃
#ifndef SAFE_DELETE                                                     //┃
#define SAFE_DELETE(p)      ((p) ? (delete (p), (p)=NULL) : ((p) = (p)))//┃
#endif                                                                  //┃
                                                                        //┃
//根据指针值删除数组类型内存                                            //┃
#ifndef SAFE_DELETE_ARRAY                                               //┃
#define SAFE_DELETE_ARRAY(p)    ((p)?(delete[] (p),(p)=NULL):((p)=(p))) //┃
#endif                                                                  //┃
                                                                        //┃
//根据指针值释放对象                                                    //┃
#ifndef SAFE_RELEASE                                                    //┃
#define SAFE_RELEASE(p)     ((p)?((p)->Release(),(p)=NULL):((p) = (p))) //┃
#endif                                                                  //┃
                                                                        //┃
// 交换数据                                                             //┃
#ifndef EXCHANGE_DATA                                                   //┃
#define EXCHANGE_DATA(a,b)      (a) ^= (b), (b) ^= (a), (a) ^= (b);     //┃
#endif                                                                  //┃
                                                                        //┃
// 自动长度初始化结构体                                                 //┃
#ifndef ZeroMemoryStruct                                                //┃
#define ZeroMemoryStruct(p)   ZeroMemory(&(p), sizeof(p))               //┃
#endif                                                                  //┃
                                                                        //┃
// 自动长度初始化数组(仅限原始数组,指针数组禁用)                        //┃
#ifndef ZeroMemoryArray                                                 //┃
#define ZeroMemoryArray(a)   ZeroMemory((a), sizeof(a))                 //┃
#endif                                                                  //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//Windows结构化异常                                                     //┃
                                                                        //┃
#ifndef NO_THROW                                                        //┃
// 进入函数                                                             //┃
#define _ENTER_FUN                  {__try{                             //┃
#define _BENTER_FUN(b,ret)          if(!b)return ret;{__try{            //┃
#define _ENTER_FUN_C                {try{                               //┃
#define _BENTER_FUN_C(b,ret)        if(!b)return ret;{try{              //┃
                                                                        //┃
// 离开函数                                                             //┃
#define _LEAVE_FUN                  }                                   //┃
#define _LEAVE_FUN_C                }                                   //┃
                                                                        //┃
// 处理异常并终止继续抛出                                               //┃
#define _DISPOSE_END                }__except(EXCEPTION_EXECUTE_HANDLER)//┃
#define _DISPOSE_CUSTOM_END(type)   }__except(type)                     //┃
#define _DISPOSE_END_C              }catch(...)                         //┃
#define _DISPOSE_CUSTOM_END_C(type) }catch(type)                        //┃
#define _LEAVE_FUN_DISPOSE_END      _DISPOSE_END{} _LEAVE_FUN           //┃
#define _LEAVE_FUN_DISPOSE_END_C    _DISPOSE_END_C{} _LEAVE_FUN         //┃
                                                                        //┃
// 重试异常代码                                                         //┃
#define _DISPOSE_REPEAT     }__except(EXCEPTION_CONTINUE_EXECUTION)     //┃
#define _LEAVE_FUN_DISPOSE_REPEAT   _DISPOSE_REPEAT{} _LEAVE_FUN        //┃
                                                                        //┃
// 继续向上抛异常                                                       //┃
#define _DISPOSE_CONTINUE           }__except(EXCEPTION_CONTINUE_SEARCH)//┃
#define _DISPOSE_CONTINUE_C         }catch(...)                         //┃
#define _LEAVE_FUN_DISPOSE_CONTINUE     _DISPOSE_CONTINUE{} _LEAVE_FUN  //┃
#define _LEAVE_FUN_DISPOSE_CONTINUE_C   _DISPOSE_CONTINUE_C{} _LEAVE_FUN//┃
                                                                        //┃
// 仅仅提示异常                                                         //┃
#define _LEAVE_FUN_ONLY_CUE     _DISPOSE_CONTINUE{\
                                AssertEx(AL_NORMAL, FALSE, _T(__FUNCTION__));\
                                }_LEAVE_FUN                             //┃
#define _LEAVE_FUN_ONLY_CUE_C   _DISPOSE_CONTINUE_C{\
                                AssertEx(AL_NORMAL, FALSE, _T(__FUNCTION__));\
                                }_LEAVE_FUN                             //┃
                                        //┏━━━━━━━━━━━━━━━┛
#else                                   //┃
                                        //┃
#pragma warning(disable : 4702)         //┃
// 进入函数                             //┃
#define _ENTER_FUN                      //┃
#define _ENTER_FUN_C                    //┃
                                        //┃
// 离开函数                             //┃
#define _LEAVE_FUN                      //┃
#define _LEAVE_FUN_C                    //┃
                                        //┃
// 处理异常并终止继续抛出               //┃
#define _DISPOSE_END                    //┃
#define _DISPOSE_END_C                  //┃
#define _LEAVE_FUN_DISPOSE_END          //┃
#define _LEAVE_FUN_DISPOSE_END_C        //┃
                                        //┃
// 重试异常代码                         //┃
#define _DISPOSE_REPEAT                 //┃
#define _LEAVE_FUN_DISPOSE_REPEAT       //┃
                                        //┃
// 继续向上抛异常                       //┃
#define _DISPOSE_CONTINUE               //┃
#define _DISPOSE_CONTINUE_C             //┃
#define _LEAVE_FUN_DISPOSE_CONTINUE     //┃
#define _LEAVE_FUN_DISPOSE_CONTINUE_C   //┃
                                        //┃
// 仅仅提示异常                         //┃
#define _LEAVE_FUN_ONLY_CUE             //┃
#define _LEAVE_FUN_DISPOSE_CONTINUE_C   //┃
                                        //┃
#endif                                  //┃
//━━━━━━━━━━━━━━━━━━━━┛

// 永远都使用的
#define _ENTER_FUN_FOREVER  {__try{
// 无论是否异常,离开函数必须执行
#define _DISPOSE_MUST_RUN   }__finally
#define _LEAVE_FUN_DISPOSE_MUST_RUN _DISPOSE_MUST_RUN{} }

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//更新当前时间到全局变量                                        //┃
extern SYSTEMTIME g_LocalTime;                                  //┃
#if !defined(UPDATA_LOCALTIME) && !defined(LOCALTIME_YEAR)      //┃
#define UPDATA_LOCALTIME        ::GetLocalTime(&g_LocalTime)    //┃
#define LOCALTIME_YEAR          g_LocalTime.wYear               //┃
#define LOCALTIME_MONTH         g_LocalTime.wMonth              //┃
#define LOCALTIME_DAY           g_LocalTime.wDay                //┃
#define LOCALTIME_WEEK          g_LocalTime.wDayOfWeek          //┃
#define LOCALTIME_HOUR          g_LocalTime.wHour               //┃
#define LOCALTIME_MINUTE        g_LocalTime.wMinute             //┃
#define LOCALTIME_SECOND        g_LocalTime.wSecond             //┃
#define LOCALTIME_MILLISECOND   g_LocalTime.wMilliseconds       //┃
#define UPDATA_FILETIME(ft)     UPDATA_LOCALTIME;\
    ::SystemTimeToFileTime(&g_LocalTime, (FILETIME*)(&ft));

#define UPDATA_FILETIME_MILLISECOND(ft) \
    UPDATA_FILETIME(ft); ft/=10000;
#endif                                                          //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//随机使用的频率，设置越高同一时间使用随机函数重码几率越低      //┃
#define RFREQUENCY 4096                                         //┃
                                                                //┃
//全局防重码随机种子数组                                        //┃
extern DOUBLE   g_dRandRadix[RFREQUENCY];                       //┃
                                                                //┃
//全局防重码随机种子索引                                        //┃
extern UINT     g_uRandRadixIndex;                              //┃
                                                                //┃
//产生浮点随机数                                                //┃
#ifndef DOUBLE_RAND                                             //┃
#define DOUBLE_RAND ((DOUBLE)rand())                            //┃
#endif                                                          //┃
                                                                //┃
//生成随机种子，本定义可在同一时间内生成不同种子                //┃
inline DOUBLE MY_RAND(DOUBLE& Radix)                            //┃
{                                                               //┃
    UPDATA_LOCALTIME;                                           //┃
        (Radix) =                                               //┃
        (DOUBLE_RAND+LOCALTIME_MILLISECOND+LOCALTIME_SECOND)/   //┃
        (RAND_MAX+DOUBLE_RAND/                                  //┃
            (RAND_MAX+LOCALTIME_MILLISECOND+LOCALTIME_SECOND)   //┃
        );                                                      //┃
        (Radix) += g_dRandRadix[g_uRandRadixIndex++%RFREQUENCY];//┃
        (Radix) *= 100;                                         //┃
        (Radix) = (Radix)-(INT)(Radix);                         //┃
    return (Radix);                                             //┃
}                                                               //┃
inline DOUBLE MY_RAND()                                         //┃
{                                                               //┃
    DOUBLE Radix;                                               //┃
    UPDATA_LOCALTIME;                                           //┃
        (Radix) =                                               //┃
        ( DOUBLE_RAND+LOCALTIME_MILLISECOND+LOCALTIME_SECOND )/ //┃
        (RAND_MAX+DOUBLE_RAND/                                  //┃
            (RAND_MAX+LOCALTIME_MILLISECOND+LOCALTIME_SECOND)   //┃
        );                                                      //┃
        (Radix) += g_dRandRadix[g_uRandRadixIndex++%RFREQUENCY];//┃
        (Radix) *= 100;                                         //┃
        (Radix) = (Radix)-(INT)(Radix);                         //┃
    return (Radix);                                             //┃
}                                                               //┃
                                                                //┃
//根据已有种子产生随机数                                        //┃
#define RADIX(Radix)    MY_RAND(Radix)                          //┃
                                                                //┃
//根据新生成种子产生随机数                                      //┃
#define RADIXN          MY_RAND()                               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//根据种子在指定范围内随机TCHAR                                 //┃
#ifndef R_BYTE                                                  //┃
#define R_BYTE(rx,ri,ra)    (TCHAR)(rx*(ra-ri)+(ri))            //┃
#define R_BYTE_A(ri,ra)     (TCHAR)(RADIXN*(ra-ri)+(ri))        //┃
#endif                                                          //┃
                                                                //┃
//根据种子在指定范围内随机WORD                                  //┃
#ifndef R_WORD                                                  //┃
#define R_WORD(rx,ri,ra)    (WORD)(rx*(ra-ri)+(ri))             //┃
#define R_WORD_A(ri,ra)     (WORD)(RADIXN*(ra-ri)+(ri))         //┃
#endif                                                          //┃
                                                                //┃
//根据种子在指定范围内随机DWORD                                 //┃
#ifndef R_DWORD                                                 //┃
#define R_DWORD(rx,ri,ra)   (DWORD)(rx*(ra-ri)+(ri))            //┃
#define R_DWORD_A(ri,ra)    (DWORD)(RADIXN*(ra-ri)+(ri))        //┃
#endif                                                          //┃
                                                                //┃
//根据种子在指定范围内随机FLOAT                                 //┃
#ifndef R_FLOAT                                                 //┃
#define R_FLOAT(rx,ri,ra)   (FLOAT)(rx*(ra-ri)+(ri))            //┃
#define R_FLOAT_A(ri,ra)    (FLOAT)(RADIXN*(ra-ri)+(ri))        //┃
#endif                                                          //┃
                                                                //┃
//根据种子在指定范围内随机DOUBLE                                //┃
#ifndef R_DOUBLE                                                //┃
#define R_DOUBLE(rx,ri,ra)  (DOUBLE)(rx*(ra-ri)+(ri))           //┃
#define R_DOUBLE_A(ri,ra)   (DOUBLE)(RADIXN*(ra-ri)+(ri))       //┃
#endif                                                          //┃
                                                                //┃
//根据种子在指定范围内随机INT64                                 //┃
#ifndef R_INT64                                                 //┃
#define R_INT64(rx,ri,ra)   (INT64)(rx*(ra-ri)+(ri))            //┃
#define R_INT64_A(ri,ra)    (INT64)(RADIXN*(ra-ri)+(ri))        //┃
#endif                                                          //┃
                                                                //┃
//一定几率等于指定值                                            //┃
#ifndef R_ODDS                                                  //┃
#define R_ODDS(ri,ra,va,vb) ((RADIXN*(ra))<=(ri))?(va):(vb)     //┃
#endif                                                          //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//共享锁                                                        //┃
class MsLock                                                    //┃
{                                                               //┃
public :                                                        //┃
    //构造中初始化共享标记                                      //┃
    MsLock();                                                   //┃
                                                                //┃
    //析构中删除共享标记                                        //┃
    ~MsLock();                                                  //┃
                                                                //┃
    //重置临界区                                                //┃
    VOID ReSet();                                               //┃
                                                                //┃
    //进入临界区                                                //┃
    VOID Lock();                                                //┃
                                                                //┃
    //离开临界区                                                //┃
    VOID UnLock();                                              //┃
private:                                                        //┃
    //共享标记                                                  //┃
    CRITICAL_SECTION m_Lock;                                    //┃
};                                                              //┃
                                                                //┃
//自动加锁解锁器                                                //┃
class AutoLock                                                  //┃
{                                                               //┃
public:                                                         //┃
    //构造中获得共享锁并加锁                                    //┃
    AutoLock(MsLock& rLock, BOOL bUse = TRUE);                  //┃
                                                                //┃
    //析构中解锁                                                //┃
    ~AutoLock();                                                //┃
private:                                                        //┃
                                                                //┃
    //默认构造禁止使用                                          //┃
    AutoLock();                                                 //┃
                                                                //┃
    //共享锁指针                                                //┃
    MsLock* m_pLock;                                            //┃
    BOOL    m_bUse;                                             //┃
};                                                              //┃
                                                                //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//已经有共享标示时，进入作用域自动加锁、离开作用域自动解锁          //┃
#define AUTO_LOCK(lock) AutoLock __LINE__##lock(lock)               //┃
#define IS_AUTO_LOCK(lock,buse) AutoLock __LINE__##lock(lock,buse)  //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
// 池子成员基类                                                 //┃
class CPoolMember                                               //┃
{                                                               //┃
public:                                                         //┃
    CPoolMember(VOID)                                           //┃
    : m_dwIndexInPool   (INVALID_UID)                           //┃
    , m_dwIndexInLocate (INVALID_UID)                           //┃
    , m_n64EnterIndex   (INVALID_NID)                           //┃
    , m_lpData          (NULL)                                  //┃
    , m_dwDataSize      (0){}                                   //┃
    virtual ~CPoolMember(VOID){};                               //┃
    virtual DWORD  GetIndex(VOID){return m_dwIndexInPool;}      //┃
    virtual LPVOID GetData(VOID){return m_lpData;}              //┃
    virtual DWORD  GetDataSize(VOID){return m_dwDataSize;}      //┃
    virtual LPFILETIME  GetLandTime(VOID)                       //┃
    {return (LPFILETIME)&m_n64EnterIndex;}                      //┃
                                                                //┃
    VOID CopyMember(CPoolMember* pMember){CopyMember(*pMember);}//┃
    VOID CopyMember(CPoolMember& Member)                        //┃
    {                                                           //┃
        Member.m_dwIndexInPool      = m_dwIndexInPool;          //┃
        Member.m_dwIndexInLocate    = m_dwIndexInLocate;        //┃
        Member.m_n64EnterIndex      = m_n64EnterIndex;          //┃
        Member.m_lpData             = m_lpData;                 //┃
        Member.m_dwDataSize         = m_dwDataSize;             //┃
    }                                                           //┃
    friend class CMsPoolManager;                                //┃
protected:                                                      //┃
    virtual BOOL   SetData(LPVOID lpData, DWORD dwDataSize) = 0;//┃
    DWORD   m_dwIndexInPool;                                    //┃
    DWORD   m_dwIndexInLocate;                                  //┃
    INT64   m_n64EnterIndex;                                    //┃
    LPVOID  m_lpData;                                           //┃
    DWORD   m_dwDataSize;                                       //┃
};                                                              //┃
                                                                //┃
class CNoDataPoolMember                                         //┃
    : public CPoolMember                                        //┃
{                                                               //┃
private:                                                        //┃
    virtual BOOL SetData(LPVOID/*lpData*/,DWORD/*dwDataSize*/)  //┃
    {return TRUE;}                                              //┃
};                                                              //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//使用错误检查NEW                                               //┃
#ifndef NEW                                                     //┃
    #if defined(_DEBUG)         // 调试版本                     //┃
        #if defined(USE_MFC)                                    //┃
            #define NEW new                                     //┃
        #else                                                   //┃
            #define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)  //┃
        #endif                                                  //┃
    #else                       // 发布版本                     //┃
        #if defined(USE_MFC)                                    //┃
            #define NEW new                                     //┃
        #else                                                   //┃
            #define NEW new(std::nothrow)                       //┃
        #endif                                                  //┃
    #endif                                                      //┃
#endif                                                          //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//防止子类指针指向父类的使用错误的基类                          //┃
class BaseClass                                                 //┃
{                                                               //┃
public:                                                         //┃
    enum E_CLASS_NAME                                           //┃
    {                                                           //┃
        CLASS_NAME_BASECLASS = 0xFFFFFFFFFFFFFFFF,              //┃
    };                                                          //┃
    BaseClass():m_ClassFlag(0){;}                               //┃
    virtual ~BaseClass() = 0;                                   //┃
protected:                                                      //┃
    BOOL IsLegal(E_CLASS_NAME ClassFlag)                        //┃
    {                                                           //┃
        return (0 != (m_ClassFlag & ClassFlag));                //┃
    }                                                           //┃
    INT64 m_ClassFlag;                                          //┃
};                                                              //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
// GDI+图片对象                                                 //┃
struct MsImage                                                  //┃
{                                                               //┃
    MsImage(TCHAR* szImageFileName)                             //┃
        : m_Image(szImageFileName){}                            //┃
    MsImage(HBITMAP hBitmap, HPALETTE hPalette = NULL)          //┃
        : m_Image(hBitmap, hPalette){}                          //┃
    Gdiplus::Bitmap m_Image;                                    //┃
};                                                              //┃
                                                                //┃
// GDI+画法对象                                                 //┃
struct MsGraphics                                               //┃
{                                                               //┃
    MsGraphics(HDC hDc)                                         //┃
        : m_Graphics(hDc){}                                     //┃
    Gdiplus::Graphics m_Graphics;                               //┃
};                                                              //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//指向二维数组的指针模板                                        //┃
template <class T>                                              //┃
class PlanarPoINTer                                             //┃
{                                                               //┃
public:                                                         //┃
    PlanarPoINTer(T* p,UINT nDimensionA, UINT nDimensionB)      //┃
        :m_p(p)                                                 //┃
        ,m_nDimensionA(nDimensionA)                             //┃
        ,m_nDimensionB(nDimensionB)                             //┃
    {                                                           //┃
        ;                                                       //┃
    }                                                           //┃
    ~PlanarPoINTer(VOID)                                        //┃
    {                                                           //┃
        ;                                                       //┃
    }                                                           //┃
    T* operator [](T i)                                         //┃
    {                                                           //┃
        return &(m_p[i*m_nDimensionB]);                         //┃
    }                                                           //┃
    T* operator ++(INT)                                         //┃
    {                                                           //┃
        m_p += m_nDimensionA * m_nDimensionB;                   //┃
        return m_p;                                             //┃
    }                                                           //┃
    T* operator --(INT)                                         //┃
    {                                                           //┃
        m_p -= m_nDimensionA * m_nDimensionB;                   //┃
        return m_p;                                             //┃
    }                                                           //┃
    T* operator +=(INT nExcursion)                              //┃
    {                                                           //┃
        m_p += m_nDimensionA * m_nDimensionB * nExcursion;      //┃
        return m_p;                                             //┃
    }                                                           //┃
    T* operator -=(INT nExcursion)                              //┃
    {                                                           //┃
        m_p -= m_nDimensionA * m_nDimensionB * nExcursion;      //┃
        return m_p;                                             //┃
    }                                                           //┃
    T*              m_p;                                        //┃
    UINT    m_nDimensionA;                                      //┃
    UINT    m_nDimensionB;                                      //┃
};                                                              //┃
typedef PlanarPoINTer<INT> INT_ARRAY_POINTER;                   //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━〖构造完整路径〗━━━━━━━━━━━━━━┓
#ifdef UNICODE                                                  //┃
#define CreateFullPath CreateFullPathW                          //┃
#else                                                           //┃
#define CreateFullPath CreateFullPathA                          //┃
#endif                                                          //┃
inline BOOL CreateFullPathW(WCHAR* szBuff, WCHAR* szFile)       //┃
{                                                               //┃
    if (szBuff)                                                 //┃
    {                                                           //┃
        WCHAR szCurrentDirectory[MAX_PATH] = {0};               //┃
        ::GetCurrentDirectoryW(MAX_PATH, szCurrentDirectory);   //┃
        swprintf_s(                                             //┃
            szBuff,                                             //┃
            MAX_PATH,                                           //┃
            szFile,                                             //┃
            szCurrentDirectory                                  //┃
            );                                                  //┃
        return TRUE;                                            //┃
    }                                                           //┃
    return FALSE;                                               //┃
}                                                               //┃
inline BOOL CreateFullPathA(ACHAR* szBuff, ACHAR* szFile)       //┃
{                                                               //┃
    if (szBuff)                                                 //┃
    {                                                           //┃
        ACHAR szCurrentDirectory[MAX_PATH] = {0};               //┃
        ::GetCurrentDirectoryA(MAX_PATH, szCurrentDirectory);   //┃
        sprintf_s(                                              //┃
            szBuff,                                             //┃
            MAX_PATH,                                           //┃
            szFile,                                             //┃
            szCurrentDirectory                                  //┃
            );                                                  //┃
        return TRUE;                                            //┃
    }                                                           //┃
    return FALSE;                                               //┃
}                                                               //┃
                                                                //┃
inline VOID ACharToWChar(                                       //┃
    CONST ACHAR* szABuff,                                       //┃
    WCHAR* szWBuff,                                             //┃
    DWORD dwLen                                                 //┃
    )                                                           //┃
{                                                               //┃
    // 多字符转宽字符                                           //┃
    ::MultiByteToWideChar(                                      //┃
        CP_ACP,                                                 //┃
        0,                                                      //┃
        szABuff,                                                //┃
        (INT)strlen(szABuff) + 1,                               //┃
        szWBuff,                                                //┃
        dwLen                                                   //┃
        );                                                      //┃
}                                                               //┃
inline VOID WCharToAChar(                                       //┃
    CONST WCHAR* szWBuff,                                       //┃
    ACHAR* szABuff,                                             //┃
    DWORD dwLen                                                 //┃
    )                                                           //┃
{                                                               //┃
    // 多字符转宽字符                                           //┃
    ::WideCharToMultiByte(                                      //┃
        CP_ACP,                                                 //┃
        0,                                                      //┃
        szWBuff,                                                //┃
        (INT)wcslen(szWBuff) + 1,                               //┃
        szABuff,                                                //┃
        dwLen,                                                  //┃
        NULL,                                                   //┃
        NULL                                                    //┃
        );                                                      //┃
}                                                               //┃
                                                                //┃
#ifdef UNICODE                                                  //┃
#   define ACharToTChar ACharToWChar                            //┃
#   define WCharToTChar (w,t,l) _tcscpy_s(t,w,l);               //┃
#   define TCharToAChar WCharToAChar                            //┃
#   define TCharToWChar (t,w,l) _tcscpy_s(w,t,l);               //┃
#else                                                           //┃
#   define ACharToTChar (a,t,l) _tcscpy_s(t,a,l);               //┃
#   define WCharToTChar WCharToAChar                            //┃
#   define TCharToAChar (t,a,l) _tcscpy_s(a,t,l);               //┃
#   define TCharToWChar ACharToWChar                            //┃
#endif                                                          //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━〖申请构造带参对象数组模板,参数仅限DWORD〗━━━━━┓
extern DWORD g_dwArrayHelperClassParameter;                     //┃
extern MsLock g_ArrayHelperlock;                                //┃
template <class T>                                              //┃
class CMsApplyArrayHelper                                       //┃
    :public T                                                   //┃
{                                                               //┃
public:                                                         //┃
    CMsApplyArrayHelper(VOID)                                   //┃
        : T(g_dwArrayHelperClassParameter){}                    //┃
};                                                              //┃
                                                                //┃
template <class T>                                              //┃
T* HelperNew(DWORD dwParameter, DWORD dwApplyCount)             //┃
{                                                               //┃
    AUTO_LOCK(g_ArrayHelperlock);                               //┃
    g_dwArrayHelperClassParameter = dwParameter;                //┃
    return (T*)(NEW CMsApplyArrayHelper<T>[dwApplyCount]);      //┃
}                                                               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
// 网络模块                                                     //┃
#ifdef USE_MS_NET                                               //┃
#   define USE_MS_LOG                                           //┃
#   define USE_MS_THREAD                                        //┃
#   define USE_MS_SOCKET                                        //┃
#   define USE_MS_PACKET                                        //┃
#   define USE_MS_PACKET_FACTORY                                //┃
#   define USE_MS_IOCP                                          //┃
#   define USE_MS_BUFFER                                        //┃
#   define USE_MS_INDEX_POOL                                    //┃
#   ifdef USE_MS_SOCKET                                         //┃
        class CMsSocket;                                        //┃
        typedef CMsSocket* lpCMsSocket;                         //┃
        class CMsServerSocket;                                  //┃
        typedef CMsServerSocket* lpCMsServerSocket;             //┃
        class CMsClientSocket;                                  //┃
        typedef CMsClientSocket* lpCMsClientSocket;             //┃
#   endif                                                       //┃
#   define SERVER_SOCKET_INDEX 0    // 服务器套接字索引值       //┃
#endif                                                          //┃
                                                                //┃
#ifdef USE_MS_WINDOW                                            //┃
#   define USE_MS_THREAD                                        //┃
#endif                                                          //┃
                                                                //┃
#ifdef USE_MS_LUA                                               //┃
#   define USE_MS_FIND_FILE                                     //┃
#endif                                                          //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


//━━━━━━〖非MFC 相关〗━━━━━┓
// 断言引擎                         //┃
#include "MsAssertx.h"              //┃
                                    //┃
// GDI+图片                         //┃
#ifdef USE_MS_GDI_PLUS_IMAGE        //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用MsGDI+")       //┃
#endif                              //┃
#include "MsGdiBuffImage.h"         //┃
#include "MsGdiPlusImage.h"         //┃
#endif                              //┃
                                    //┃
// 数据库引擎                       //┃
#ifdef USE_MS_DATA_BASE             //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用MsMFC数据库")  //┃
#endif                              //┃
#include "MsAccess.h"               //┃
#endif                              //┃
                                    //┃
// 日志引擎                         //┃
#ifdef USE_MS_LOG                   //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms日志系统")   //┃
#endif                              //┃
#include "MsConsole.h"              //┃
#include "MsLog.h"                  //┃
#endif                              //┃
                                    //┃
// 配置引擎                         //┃
#ifdef USE_MS_INI                   //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms配置系统")   //┃
#endif                              //┃
#include "MsIni.h"                  //┃
#endif                              //┃
                                    //┃
// 线程对象                         //┃
#ifdef USE_MS_THREAD                //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms线程对象")   //┃
#endif                              //┃
#include "MsThread.h"               //┃
#include "MsMsgThread.h"            //┃
#endif                              //┃
                                    //┃
// 索引池管理                       //┃
#ifdef USE_MS_INDEX_POOL            //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms索引池结构") //┃
#endif                              //┃
#include "MsPoolManager.h"          //┃
#endif                              //┃
                                    //┃
// 队列管理                         //┃
#ifdef USE_MS_QUEUE_MANAGER         //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms队列结构")   //┃
#endif                              //┃
#include "MsQueueManager.h"         //┃
#endif                              //┃
                                    //┃
// 封包结构                         //┃
#ifdef USE_MS_PACKET                //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms封包")       //┃
#endif                              //┃
#include "MsPacketDef.h"            //┃
#endif                              //┃
                                    //┃
// 缓冲区类                         //┃
#ifdef USE_MS_BUFFER                //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms缓冲区")     //┃
#endif                              //┃
#include "MsBuffer.h"               //┃
#endif                              //┃
                                    //┃
// 完成端口                         //┃
#ifdef USE_MS_IOCP                  //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms完成端口")   //┃
#endif                              //┃
#include "MsIocpManager.h"          //┃
#endif                              //┃
                                    //┃
// 封包工厂                         //┃
#ifdef USE_MS_PACKET_FACTORY        //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms封包")       //┃
#endif                              //┃
#include "CMsPacketFactory.h"       //┃
#endif                              //┃
                                    //┃
// 套接字类                         //┃
#ifdef USE_MS_SOCKET                //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms套接字")     //┃
#endif                              //┃
#include "MsSocket.h"               //┃
#include "MsServerSocket.h"         //┃
#include "MsClientSocket.h"         //┃
#endif                              //┃
                                    //┃
// 资源文件                         //┃
#ifdef USE_MS_FILE_FROM_RES         //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms资源文件")   //┃
#endif                              //┃
#include "MsFileFromRes.h"          //┃
#endif                              //┃
                                    //┃
// 查找文件                         //┃
#ifdef USE_MS_FIND_FILE             //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms查找文件")   //┃
#endif                              //┃
#include "MsFindFileManager.h"      //┃
#endif                              //┃
                                    //┃
// 数据库类                         //┃
#ifdef USE_MS_SQLDB                 //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms数据库系统") //┃
#endif                              //┃
#include <sqlext.h>                 //┃
#include "MsSqlServerManager.h"     //┃
#pragma comment(lib, "odbc32.lib")  //┃
#endif                              //┃
                                    //┃
// 表格管理                         //┃
#ifdef USE_MS_TABLE                 //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms数据库系统") //┃
#endif                              //┃
#include "MsAutoTableManager.h"     //┃
#endif                              //┃
                                    //┃
// 脚本引擎                         //┃
#ifdef USE_MS_LUA                   //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms脚本系统")   //┃
#endif                              //┃
#include "MsLuaManager.h"           //┃
#endif                              //┃
                                    //┃
// 客户端网络管理器                 //┃
#ifdef USE_MS_CLIENT_NET            //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms客户端网络") //┃
#endif                              //┃
#include "MsClientNetManager.h"     //┃
#endif                              //┃
                                    //┃
// 服务器网络管理器                 //┃
#ifdef USE_MS_SERVER_NET            //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms服务器网络") //┃
#endif                              //┃
#include "MsServerNetManager.h"     //┃
#endif                              //┃
                                    //┃
// 声音管理器                       //┃
#ifdef USE_MS_SOUND                 //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用Ms声音管理器") //┃
#endif                              //┃
#include "MsSound.h"                //┃
#endif                              //┃
                                    //┃
#ifdef USE_MS_2DENGINE              //┃
#ifdef USE_COMPILER_MESSAGE         //┃
#pragma message("使用MsDirectX")    //┃
#endif                              //┃
#include "Ms2DEngineDef.h"          //┃
#include "Ms2DEngine.h"             //┃
#include "Ms2DInput.h"              //┃
#include "Ms2DTextureManager.h"     //┃
#include "Ms2DRender_DX9.h"         //┃
#include "Ms2DEngine.h"             //┃
#endif                              //┃
//━━━━━━━━━━━━━━━━━━┛

//━━━━━━〖MFC 相关〗━━━━━━━━┓
// 不使用MFC                            //┃
#ifndef USE_MFC                         //┃
    // 控制台类                         //┃
    #ifdef USE_MS_CONSOLE               //┃
    #ifdef USE_COMPILER_MESSAGE         //┃
    #pragma message("使用Ms控制台系统") //┃
    #endif                              //┃
    #include "MsConsole.h"              //┃
    #endif                              //┃
    // 窗口类                           //┃
    #ifdef USE_MS_WINDOW                //┃
    #ifdef USE_COMPILER_MESSAGE         //┃
    #pragma message("使用Ms窗口系统")   //┃
    #endif                              //┃
    #include "MsWinMain.h"              //┃
    #endif                              //┃
#endif                                  //┃
//━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
//全局基本类型初始化线程句柄                                        //┃
extern HANDLE   g_hBaseInitThread;                                  //┃
                                                                    //┃
//全局基本类型释放线程句柄                                          //┃
extern HANDLE   g_hBaseReleaseThread;                               //┃
                                                                    //┃
//全局基本类型初始化线程ID                                          //┃
extern DWORD    g_BaseInitthreadID;                                 //┃
                                                                    //┃
// GDI+对象标记                                                     //┃
extern ULONG_PTR g_lToken;                                          //┃
                                                                    //┃
BOOL InitRand(VOID* /*Parameter*/);                                 //┃
                                                                    //┃
extern BOOL g_IsMsBaseInit;                                         //┃
                                                                    //┃
// 宏定义初始化并等待线程结束                                       //┃
VOID MSCALL InitMsBase(                                             //┃
    TCHAR* szAssertFileName,                                        //┃
    BOOL bConsole,                                                  //┃
    RUNCALLBACK pRunCallBack,                                       //┃
    LPTSTR szClassName,                                             //┃
    LPTSTR szWndName,                                               //┃
    HOOKWNDPROC HookWndProc,                                        //┃
    WNDPROC WndProc,                                                //┃
    PTHREAD_START_ROUTINE ThcreadProc                               //┃
    );                                                              //┃
VOID MSCALL ReleaseMsBase(LPVOID lpNeedKnowIsFunForUse);            //┃
class CMsBaseRun                                                    //┃
{                                                                   //┃
public:                                                             //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
    CMsBaseRun(                                                     //┃
        TCHAR* szAssertFileName,                                    //┃
        RUNCALLBACK pRunCallBack                                    //┃
        )                                                           //┃
    {                                                               //┃
        InitMsBase(                                                 //┃
            szAssertFileName,   // TCHAR* szAssertFileName          //┃
            TRUE,               // BOOL bConsole                    //┃
            pRunCallBack,       // RUNCALLBACK pRunCallBack         //┃
            NULL,               // LPTSTR szClassName               //┃
            NULL,               // LPTSTR szWndName                 //┃
            NULL,               // HOOKWNDPROC HookWndProc          //┃
            NULL,               // WNDPROC WndProc                  //┃
            NULL                // PTHREAD_START_ROUTINE ThcreadProc//┃
            );                                                      //┃
    }                                                               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
    CMsBaseRun(                                                     //┃
        TCHAR* szAssertFileName,                                    //┃
        LPTSTR szClassName,                                         //┃
        LPTSTR szWndName,                                           //┃
        HOOKWNDPROC HookWndProc,                                    //┃
        WNDPROC WndProc,                                            //┃
        PTHREAD_START_ROUTINE ThcreadProc                           //┃
        )                                                           //┃
    {                                                               //┃
        InitMsBase(                                                 //┃
            szAssertFileName,   // TCHAR* szAssertFileName          //┃
            FALSE,              // BOOL bConsole                    //┃
            NULL,               // RUNCALLBACK pRunCallBack         //┃
            szClassName,        // LPTSTR szClassName               //┃
            szWndName,          // LPTSTR szWndName                 //┃
            HookWndProc,        // HOOKWNDPROC HookWndProc          //┃
            WndProc,            // WNDPROC WndProc                  //┃
            ThcreadProc         // PTHREAD_START_ROUTINE ThcreadProc//┃
            );                                                      //┃
    }                                                               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
    CMsBaseRun(TCHAR* szAssertFileName)                             //┃
    {                                                               //┃
        InitMsBase(                                                 //┃
            szAssertFileName,   // TCHAR* szAssertFileName          //┃
            FALSE,              // BOOL bConsole                    //┃
            NULL,               // RUNCALLBACK pRunCallBack         //┃
            NULL,               // LPTSTR szClassName               //┃
            NULL,               // LPTSTR szWndName                 //┃
            NULL,               // HOOKWNDPROC HookWndProc          //┃
            NULL,               // WNDPROC WndProc                  //┃
            NULL                // PTHREAD_START_ROUTINE ThcreadProc//┃
            );                                                      //┃
    }                                                               //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
    ~CMsBaseRun(VOID){ReleaseMsBase(NULL);}                         //┃
private:                                                            //┃
    CMsBaseRun(VOID){};                                             //┃
};                                                                  //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

#if     (_OPENMP == 0)
#   error  无法识别编译器版本, 请打开工程 -> C/C++ -> 语言 -> OpenMP 支持 -> 是
#elif   (_OPENMP == 200805) && !defined(_INTEL)
#   error  Intel编译器，请使用Intel编译选项进行编译!
#elif   (_OPENMP == 200203) && defined(_INTEL)
#   error  Visual C++编译器，请使用VC编译选项进行编译!
#endif

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
#ifdef _INTEL                                                               //┃
#pragma warning(disable : 1684)                                             //┃
#pragma warning(disable : 111)                                              //┃
#else                                                                       //┃
#endif                                                                      //┃
                                                                            //┃
// Windows Lib                                                              //┃
#pragma comment(lib, "Psapi.lib")                                           //┃
                                                                            //┃
#if defined(WIN64)                                                          //┃
#   ifdef _DEBUG                                                            //┃
#       ifdef _INTEL                                                        //┃
#           pragma comment(lib, "Lib_ExotericCode_Intel_Debug_x64.lib")     //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC_Intel_Debug_x64.lib")        //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode_Intel_Debug_x64.lib")       //┃
#           endif                                                           //┃
#       else                                                                //┃
#           pragma comment(lib, "Lib_ExotericCode__VC_Debug_x64.lib")       //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC__VC_Debug_x64.lib")          //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode__VC_Debug_x64.lib")         //┃
#           endif                                                           //┃
#       endif                                                               //┃
#   else                                                                    //┃
#       ifdef _INTEL                                                        //┃
#           pragma comment(lib, "Lib_ExotericCode_Intel_Release_x64.lib")   //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC_Intel_Release_x64.lib")      //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode_Intel_Release_x64.lib")     //┃
#           endif                                                           //┃
#       else                                                                //┃
#           pragma comment(lib, "Lib_ExotericCode__VC_Release_x64.lib")     //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC__VC_Release_x64.lib")        //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode__VC_Release_x64.lib")       //┃
#           endif                                                           //┃
#       endif                                                               //┃
#   endif                                                                   //┃
#elif defined(WIN32)                                                        //┃
#   ifdef _DEBUG                                                            //┃
#       ifdef _INTEL                                                        //┃
#           pragma comment(lib, "Lib_ExotericCode_Intel_Debug_Win32.lib")   //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC_Intel_Debug_Win32.lib")      //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode_Intel_Debug_Win32.lib")     //┃
#           endif                                                           //┃
#       else                                                                //┃
#           pragma comment(lib, "Lib_ExotericCode__VC_Debug_Win32.lib")     //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC__VC_Debug_Win32.lib")        //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode__VC_Debug_Win32.lib")       //┃
#           endif                                                           //┃
#       endif                                                               //┃
#   else                                                                    //┃
#       ifdef _INTEL                                                        //┃
#           pragma comment(lib, "Lib_ExotericCode_Intel_Release_Win32.lib") //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC_Intel_Release_Win32.lib")    //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode_Intel_Release_Win32.lib")   //┃
#           endif                                                           //┃
#       else                                                                //┃
#           pragma comment(lib, "Lib_ExotericCode__VC_Release_Win32.lib")   //┃
#           ifdef USE_MFC                                                   //┃
#               pragma comment(lib, "Lib_MsMFC__VC_Release_Win32.lib")      //┃
#           else                                                            //┃
#               pragma comment(lib, "Lib_MsCode__VC_Release_Win32.lib")     //┃
#           endif                                                           //┃
#       endif                                                               //┃
#   endif                                                                   //┃
#endif                                                                      //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
