#pragma once                        //┃  //┃
#ifndef __SQLSERVERMANAGER_H__      //┃  //┃
#define __SQLSERVERMANAGER_H__      //┃  //┃
//━━━━━━━━━━━━━━━━━━┛  //┃
//文件名称：SqlServerManager.h            //┃
//功能描述：数据库管理器                  //┃
//平台相关：Windows                       //┃
//━━━━━━━━━━━━━━━━━━━━━┛

// 最大预备SQL语句长度
#define MAX_PREPARE_SQL_SENTENCE 1024

// 最大预备SQL语句个数
#define MAX_PREPARE_SQL_COUNT 1024

// 预备SQL语句最大参数个数
#define PREPARE_MAX_PARAMETER_COUNT 100

// SQL数据库类型
enum SQL_TYPE
{
    MS_SQL_TYPE_UNKNOW      = SQL_UNKNOWN_TYPE,     // 未知类型
    MS_SQL_TYPE_INT         = SQL_INTEGER,          // 整型
    MS_SQL_TYPE_TCHAR       = SQL_C_TCHAR,          // 字符型
    MS_SQL_TYPE_FLOAT       = SQL_REAL,             // 浮点型
    MS_SQL_TYPE_DATETIME    = SQL_C_TYPE_TIMESTAMP, // 日期时间型
    MS_SQL_TYPE_TO_DWORD    = 0x7FFFFFFF,
};

// 从文件加载SQL脚本如何处理
typedef enum LOAD_SQL_FROM_FILE_FLAG
{
    LSFFF_LOAD_AND_IMMEDIATELY_EXECUTE, // 立即执行
    LSFFF_LOAD_AND_ADD_TO_PREPARE,      // 加入到准备语句中
    LSFFF_LOAD_ONLY,                    // 仅读取到待执行状态
}LSFFF;

typedef enum SQL_SENTENCE_TYPE
{
    SSET_CREATE, // 创建
    SSET_SELECT, // 查询
    SSET_INSERT, // 插入
    SSET_UPDATA, // 更新
    SSET_DELETE, // 删除
}SSET;

// 查询语句条件关系
typedef enum SQL_SELECT_WHERE_NEXUS
{
    SSWN_UNKNOW = 0,    // 未知
    SSWN_EQUAL,         // 相等
    SSWN_NOTEQUAL,      // 不等
    SSWN_BIG,           // 大于
    SSWN_SMALL,         // 小于
    SSWN_BIG_EQUAL,     // 大于等于
    SSWN_SMALL_EQUAL,   // 小于等于
    SSWN_LIKE,          // LIKE
    SSWN_TO_DWORD = 0x7FFFFFFF,
}SSWN;

// SQL管理器函数执行返回结果
typedef enum SQL_RESULT
{
    SR_OK,              // 执行正常
    SR_DB_NAME_ERROR,   // 数据库名字错误
    SR_CODE_THROW,      // 执行中检测到异常
    SR_ERROR,
}SR;

// SQL管理器使用的准备语句结构体
typedef struct SQL_PREPARE_SENTENCE
{
    inline VOID Release(VOID)
    {
        ZeroMemoryArray(m_szSQLPrepare);
        ZeroMemoryArray(m_SqlType);
        m_ParameterNumber = 0;
    }
    SQLTCHAR        m_szSQLPrepare[MAX_PREPARE_SQL_SENTENCE];   // 准备语句数组
    SQL_TYPE        m_SqlType[MAX_PREPARE_SQL_SENTENCE];        // 准备语句中参数类型的数组
    SQLUSMALLINT    m_ParameterNumber;                          // 准备语句使用的参数个数
}SPS;

// 创建表格信息结构体
typedef struct SQL_TABLE_ITEM_ATTRIBUTE
{
    TCHAR       m_szTableItemName[MAX_PATH];// 表项名称
    SQL_TYPE    m_SqlType;                  // 数据库类型
    BOOL        m_IsCanNull;                // 是否允许为空
    BOOL        m_IsAutoAdd;                // 是否自增量
    TCHAR       m_strLen[RADIX_10];         // 带有长度类型的长度
}STIA;

// 创建表格最终使用结构体
typedef struct STIA_STRUCT
{
    STIA*   m_pStia;                    // 表格项属性指针
    INT  m_Count;                    // 表格项个数
    TCHAR   m_szTableKeyName[MAX_PATH]; // 表主键项名称
}STIAC;

// 查询语句的类型
typedef enum SELECT_TYPE
{
    SELECT_TYPE_COUNT,      // 查询个数
    SELECT_TYPE_SUM,        // 查询总和
    SELECT_TYPE_AVG,        // 查询平均值
    SELECT_TYPE_SUM_TO_AVG, // 查询总值,自己获取平均值
    SELECT_TYPE_ALL_ITEM,   // 查询所有项
    SELECT_TYPE_ONE_ITEM,   // 查询指定一项
}ST;

// 查询表项信息结构体
class CAutoTableManager;
typedef struct SQL_SELECT_ITEM_ATTRIBUTE
{
    TCHAR*  m_szItemName;               // 表项名称
    TCHAR*  m_szSelectItemName;         // 表项名称
    TCHAR*  m_strTableName;             // 查询表名称
    SSWN    m_WhereAttrNexus;           // 查询关系
    TCHAR*  m_szWhereAttrTeam;          // 表项查询条件
    CAutoTableManager*  m_pTable;       // 返回结果表格指针
    ST      m_SelectType;               // 查询类型
}SSIA;

// 插入表项信息结构体
typedef struct SQL_INSERT_ITEM_ATTRIBUTE
{
    TCHAR       m_szItemName[MAX_PATH]; // 表项名称
    SQL_TYPE    m_SqlType;              // 表项类型
    TCHAR       m_szItemValue[MAX_PATH];// 表项值
}SIIA;

// 插入表项信息最终使用结构体
typedef struct SIIA_STRUCT
{
    SIIA*   m_pSiia;    // 插入表项属性指针
    INT  m_Count;       // 插入表项个数
}SIIAC;

class CSqlServerManager
{
public: // 构造、析构
    CSqlServerManager(VOID);           // 数据库管理器构造函数
    virtual ~CSqlServerManager(VOID);  // 数据库管理器析构函数

public: // 接口
    BOOL Init(VOID);    // 初始化数据库管理器
    BOOL Release(VOID); // 释放数据库管理器

    // 连接数据库
    BOOL ConnectW(CONST WCHAR* szDBName, CONST WCHAR* szUserName, CONST WCHAR* szPassword);
    BOOL ConnectA(CONST CHAR* szDBName, CONST CHAR* szUserName, CONST CHAR* szPassword);
    BOOL Connect(CONST TCHAR* szDBName, CONST TCHAR* szUserName, CONST TCHAR* szPassword);

    // 执行SQL语句
    SR ExecuteSQLSentence(TCHAR* szSQLSentence, BOOL IsNeedResult = FALSE, CAutoTableManager* pTable = NULL);

    // 执行预备SQL语句
    SR ExecuteSQLPrepare(INT nIndex, TCHAR* szSQLParameter, ...);

    // 从文件加载SQL语句
    BOOL LoadSqlSentenceFromFile(TCHAR* szFileName, LSFFF nFlag = LSFFF_LOAD_AND_IMMEDIATELY_EXECUTE);

    //// 检查预备SQL语句是否匹配
    //BOOL CheckSQLPrepare(INT nIndex, TCHAR* szSQLParameter);

    // 添加预备SQL语句
    INT AddSQLPrepare(TCHAR* szSQLSentence);

    // 删除预备SQL语句
    BOOL RemoveSQLPrepare(INT nIndex);

    // 创建SQL语句
    BOOL CreateSQLSentence(OUT TCHAR* szSQLSentence, SIZE_T MaxSQLLen, SSET sset, TCHAR* szTableName, LPVOID pData);

    // 获取语句执行结果
    FLOAT GetSQLSentenceResult(VOID);

private:// 私有

    inline VOID CheckFileSQL();

    // 创建SQL语句之创建表格
    BOOL CreateSQLSentenceAtCreateTable(OUT TCHAR* szSQLSentence, SIZE_T MaxSQLLen, TCHAR* szTableName, STIAC* pStiac);

    // 创建SQL语句之查询项
    BOOL CreateSQLSentenceAtInsertItem(OUT TCHAR* szSQLSentence, SIZE_T MaxSQLLen, TCHAR* szTableName, SIIAC* pSiiac);

    // 创建SQL语句之查询项
    BOOL CreateSQLSentenceAtSelectItem(OUT TCHAR* szSQLSentence, SIZE_T MaxSQLLen, SSIA* pSsia);

    BOOL        m_IsInit;       // 当前对象是否已经初始化标记
    SQLHENV     m_hSqlHenv;     // SQL数据库环境句柄
    SQLHDBC     m_hDbc;         // SQL数据库连接句柄
    SQLHSTMT    m_hStmt;        // SQL数据库语句句柄
    BOOL        m_bConnectOK;   // 数据库是否成功连接

    // SQL数据库预备语句
    SPS         m_SQLPrepare[MAX_PREPARE_SQL_COUNT];
    SQLTCHAR    m_szSQLSentenceT[MAX_PREPARE_SQL_SENTENCE];
    SQLCHAR     m_szSQLSentenceA[MAX_PREPARE_SQL_SENTENCE];
    SQLTCHAR    m_SQLMessageText[MAX_PREPARE_SQL_SENTENCE];
    INT         m_SQLPrepareCount;      // 预备语句个数
    FLOAT       m_fSQLSentenceResult;   // SQL语句执行返回值
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
