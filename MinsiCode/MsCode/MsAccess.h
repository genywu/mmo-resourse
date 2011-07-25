#pragma once            //┃//┃
#ifndef __MSACCESS_H__  //┃//┃
#define __MSACCESS_H__  //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsAccess.h      //┃
//功能描述：MFC_DAO_DB      //┃
//平台相关：WINDOWS         //┃
//━━━━━━━━━━━━━━┛

// SQL查询结果字符串类型最大长度
#define MAX_RESULT_STRING_LEN   4096

class CMsAccess
{
public:
    // 构造、析构
    CMsAccess(VOID);
    virtual ~CMsAccess(VOID);

    // 打开数据库文件
    BOOL Open(TCHAR* szDBName);

    BOOL Close(VOID);                   // 关闭数据库文件
    VOID ExecuteSQL(TCHAR* szSQLText);  // 执行SQL语句
    LONG GetResultCount(VOID);          // 获取结果记录个数
    SHORT GetFieldCount(VOID);          // 获取结果字段个数
    
    VOID MoveFirst(VOID);   // 第一个
    VOID MoveLast(VOID);    // 最后一个
    VOID MovePrev(VOID);    // 上一个
    VOID MoveNext(VOID);    // 下一个

    COleVariant* GetResultValue(TCHAR* szFieldName);// 获取结果记录,根据字段名
    COleVariant* GetResultValue(INT nIndex);        // 获取结果记录,根据字段索引
private:
    CDaoDatabase*   m_pCDaoDatabase;    // DAO数据库对象指针
    CDaoRecordset*  m_pCDaoRecordset;   // DAO数据库结果指针
    CDaoException*  m_pCDaoException;   // DAO数据库异常指针
    COleVariant     m_COleVariant;      // DAO执行结果
    TCHAR           m_ResultBuff[MAX_RESULT_STRING_LEN];
    CDaoTableDefInfo    m_CDaoTableDefInfo; // DAO数据库表信息
};  

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
