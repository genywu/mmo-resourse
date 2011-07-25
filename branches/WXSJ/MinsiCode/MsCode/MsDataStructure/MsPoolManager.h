#pragma once                //┃//┃
#ifndef __MSPOOLMANAGER_H__ //┃//┃
#define __MSPOOLMANAGER_H__ //┃//┃
//━━━━━━━━━━━━━━┛//┃
//文件名称：MsPoolManager.h     //┃
//功能描述：索引池管理相关封装  //┃
//平台相关：Windows             //┃
//━━━━━━━━━━━━━━━━┛

// 池管理器
class CMsPoolManager
{
public:
    CMsPoolManager(                 // 构造函数
        DWORD dwPoolMaxCount,
        BOOL bNoData = FALSE,
        BOOL bAutoInit = TRUE,
        BOOL bAutoLock = TRUE
        );

    ~CMsPoolManager(VOID);          // 析构函数

    VOID operator = (CMsPoolManager){AssertEx(AL_NORMAL, FALSE, _T("禁用赋值运算符"));}
public:
    BOOL    Init(VOID);                             // 池子管理器初始化函数
    BOOL    Release(VOID);                          // 释放池子管理器
    BOOL    Reset(VOID);                            // 重置索引池子
    DWORD   GetCurrentCount(VOID);                  // 获取当前元素个数
    DWORD   GetMaxCount(VOID);                      // 获取最大元素个数
    BOOL    IsFull(VOID);                           // 检查池是否已经满了
    BOOL    IsEmpty(VOID);                          // 检查池是否已经空了
    BOOL    Append(CPoolMember* lpData);            // 添加元素
    BOOL    Remove(CPoolMember* lpData);            // 移除元素
    DWORD   GetNextIndex(BOOL bPossess = TRUE);     // 获取索引位置
    BOOL    FreeIndexByPool(DWORD dwIndexInPool);   // 根据池子释放索引位置
    BOOL    FreeIndexByLand(DWORD dwIndexInLand);   // 根据登陆释放索引位置
    BOOL    RemoveByPoolIndex(DWORD dwIndexInPool); // 根据池子索引删除
    BOOL    RemoveByLandIndex(DWORD dwIndexInLand); // 根据登陆索引删除
    VOID    PoolLock(VOID);                         // 池子加锁
    VOID    PoolUnLock(VOID);                       // 池子解锁

    // 搜寻索引转换成池子索引
    DWORD   LocateIndexToPoolIndex(DWORD dwIndexInLocate);

    // 获取错误代码
    E_MS_ERROR      GetLastErrorCMsPoolManager(VOID);

    // 根据池子索引获取元素
    CPoolMember*    GetDataByPool(DWORD dwIndexByPool);

    // 根据登陆索引获取元素
    CPoolMember*    GetDataByLand(DWORD dwIndexByLand);

    // 根据池子索引获取元素登陆时间
    LPFILETIME      GetDataLandTime(DWORD dwIndexByPool);

private:
    MsLock          m_MsPoolLock;
    CONST DWORD     m_dwPoolMaxCount;       // 池子最大的成员个数
    CPoolMember**   m_lppCPoolMember;       // 池子成员数组指针
    LPDWORD         m_lpdwLandArray;        // 池子登陆数组
    DWORD           m_dwCurrentMemberCount; // 当前池子成员数量
    SYSTEMTIME      m_stLandTime;           // 登陆时间
    E_MS_ERROR      m_ePoolErrorCode;       // 池子错误代码
    BOOL            m_bInit;                // 是否已经初始化
    CONST BOOL      m_bLock;                // 是否进行多线程加锁
    BOOL            m_bNoData;              // 是否存储数据
};

typedef CMsPoolManager* lpCMsPoolManager;
//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
