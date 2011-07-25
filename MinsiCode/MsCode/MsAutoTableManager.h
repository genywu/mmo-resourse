#pragma once                        //┃  //┃
#ifndef __AUTOTABLEMANAGER_H__      //┃  //┃
#define __AUTOTABLEMANAGER_H__      //┃  //┃
//━━━━━━━━━━━━━━━━━━┛  //┃
//文件名称：AutoTableManager.h            //┃
//功能描述：表格管理器                    //┃
//平台相关：Windows                       //┃
//━━━━━━━━━━━━━━━━━━━━━┛

enum SQL_TYPE;
// 创建自动表格信息结构体
typedef struct ATA_STRUCT
{
    SQL_TYPE*   m_pAta;         // 表格项类型
    UINT        m_uItemCount;   // 表格项个数
    UINT        m_uLineCount;   // 表格列个数
}ATAC;

typedef struct ATA_ITEM 
{
    SQL_TYPE    m_pAta;     // 表格项类型
    LPVOID      m_lpItem;   // 表格项头指针
}AI;

typedef struct TCHAR_ARRAY 
{
    TCHAR   m_strText[MAX_PATH];
}TCA;

class CAutoTableManager
{
public: // 构造、析构
    CAutoTableManager(ATAC* pAtac);     // 表格管理器构造函数
    virtual ~CAutoTableManager(VOID);   // 表格管理器析构函数

public: // 接口
    BOOL                Init(VOID);     // 初始化表格管理器
    BOOL                IsInit(VOID);   // 检查是否已经初始化表格管理器
    BOOL                Release(VOID);  // 释放表格管理器

    VOID                SetTablePointer(UINT uItemIndex, UINT uLineIndex);
    VOID                GetTablePointer(UINT& uItemIndex, UINT& uLineIndex);

    VOID                SetValidLine(UINT uCount);
    INT                 GetValidLine(VOID);

    VOID                GetTableSize(UINT& uItemCount, UINT& uLineCount);

    SQL_TYPE            GetTableItemType(UINT uItemIndex);
    INT               GetTableItemSize(UINT uItemIndex, UINT uLineIndex, BOOL IsUsable);

    BOOL                AddTableItemPointer(VOID);
    BOOL                SubTableItemPointer(VOID);
    BOOL                AddTableLinePointer(VOID);
    BOOL                SubTableLinePointer(VOID);
    BOOL                operator ++ (VOID);
    BOOL                operator ++ (INT);
    BOOL                operator -- (VOID);
    BOOL                operator -- (INT);

    // 在指定项目指定行返回首地址
    LPVOID              GetTableItemInLine(UINT uItemIndex, UINT uLineIndex);
    INT*                GetTableItemInLineAsINT(UINT uItemIndex, UINT uLineIndex);
    TCA*                GetTableItemInLineAsTCHAR(UINT uItemIndex, UINT uLineIndex);
    FLOAT*              GetTableItemInLineAsFLOAT(UINT uItemIndex, UINT uLineIndex);
    TIMESTAMP_STRUCT*   GetTableItemInLineAsDATETIME(UINT uItemIndex, UINT uLineIndex);

private:// 私有
    AI*                 m_pAi;          // 表格信息指针
    INT*                m_pInt;
    TCA*                m_pTchar;
    FLOAT*              m_pFloat;
    TIMESTAMP_STRUCT*   m_pDataTime;
    BOOL                m_IsInit;       // 当前对象是否已经初始化标记
    UINT                m_uItemCount;   // 表格项个数
    UINT                m_uLineCount;   // 表格列个数
    UINT                m_uItemIndex;   // 当前项位置
    UINT                m_uLineIndex;   // 当前行位置
    UINT                m_uValidLine;   // 有效记录的条数
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
