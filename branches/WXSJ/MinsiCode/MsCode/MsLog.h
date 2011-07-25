#pragma once          //┃  //┃
#ifndef __MSLOG_H__   //┃  //┃
#define __MSLOG_H__   //┃  //┃
//━━━━━━━━━━━┛  //┃
//文件名称：MsLog.h         //┃
//功能描述：日志相关定义    //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

//断言日志文本最大长度
#define LOG_STR_MAX_LEN 1256

typedef struct LOG_FILE_INFO
{
    HANDLE  m_hLogFile;
    TCHAR   m_szLogFileName[MAX_PATH];
}LFI;

enum CMSLOG_FLAG
{
    CMSLOG_FLAG_TIME = 0,           // 打印日期(多态字符)
    CMSLOG_FLAG_TIMEW,              // 打印日期(宽字符)
    CMSLOG_FLAG_TIMEA,              // 打印日期(多字符)
    CMSLOG_FLAG_NOSAVE,             // 不保存
    CMSLOG_FLAG_SAVE,               // 保存
    CMSLOG_FLAG_NEWLINE,            // 换行
    CMSLOG_FLAG_TIME_BEGIN,         // 计时开始
    CMSLOG_FLAG_TIME_END,           // 计时结束(多态字符)
    CMSLOG_FLAG_TIME_ENDA,          // 计时结束(宽字符)
    CMSLOG_FLAG_TIME_ENDW,          // 计时结束(多字符)
    CMSLOG_FLAG_END,                // 结束日志
    CMSLOG_FLAG_NEWLINE_END,        // 结束换行
    CMSLOG_FLAG_PAUSE,              // 按任意键继续
    CMSLOG_FLAG_PAUSE_REPEAT,       // 按任意键继续并重复显示
    CMSLOG_FLAG_FC_FINTENSITY,      // 设置字体颜色高亮
    CMSLOG_FLAG_FC_UN_FINTENSITY,   // 设置字体颜色非高亮
    CMSLOG_FLAG_FC_COLOR_BLACK,     // 设置字体颜色为黑
    CMSLOG_FLAG_FC_COLOR_WHITE,     // 设置字体颜色为白
    CMSLOG_FLAG_FC_COLOR_BLUE,      // 设置字体颜色为蓝
    CMSLOG_FLAG_FC_COLOR_GREEN,     // 设置字体颜色为绿
    CMSLOG_FLAG_FC_COLOR_RED,       // 设置字体颜色为红
    CMSLOG_FLAG_FC_COLOR_BLUE_GREEN,// 设置字体颜色为蓝绿
    CMSLOG_FLAG_FC_COLOR_BLUE_RED,  // 设置字体颜色为蓝红
    CMSLOG_FLAG_FC_COLOR_GREEN_RED, // 设置字体颜色为绿红
    CMSLOG_FLAG_FC_COLOR_RV,        // 设置字体颜色为反色
    CMSLOG_FLAG_LOCK,               // 加小锁
    CMSLOG_FLAG_UNLOCK,             // 解小锁
    CMSLOG_FLAG_LOCK_A,             // 加大锁
    CMSLOG_FLAG_UNLOCK_A,           // 解大锁
    //CMSLOG_FLAG_BC_COLOR_BLACK,     // 设置字体颜色为黑
    //CMSLOG_FLAG_BC_COLOR_WHITE,     // 设置字体颜色为白
    //CMSLOG_FLAG_BC_COLOR_BLUE,      // 设置字体颜色为蓝
    //CMSLOG_FLAG_BC_COLOR_GREEN,     // 设置字体颜色为绿
    //CMSLOG_FLAG_BC_COLOR_RED,       // 设置字体颜色为红
    //CMSLOG_FLAG_BC_COLOR_BLUE_GREEN,// 设置字体颜色为蓝绿
    //CMSLOG_FLAG_BC_COLOR_BLUE_RED,  // 设置字体颜色为蓝红
    //CMSLOG_FLAG_BC_COLOR_GREEN_RED, // 设置字体颜色为绿红
};

#define L_TIME      CMSLOG_FLAG_TIME
#define L_TIMEW     CMSLOG_FLAG_TIMEW
#define L_TIMEA     CMSLOG_FLAG_TIMEA
#define L_NOSAVE    CMSLOG_FLAG_NOSAVE
#define L_SAVE      CMSLOG_FLAG_SAVE
#define L_NEWLINE   CMSLOG_FLAG_NEWLINE
#define L_TB        CMSLOG_FLAG_TIME_BEGIN
#define L_TE        CMSLOG_FLAG_TIME_END
#define L_TEA       CMSLOG_FLAG_TIME_ENDA
#define L_TEW       CMSLOG_FLAG_TIME_ENDW
#define L_END       CMSLOG_FLAG_END
#define L_NEND      CMSLOG_FLAG_NEWLINE_END
#define L_PAUSE     CMSLOG_FLAG_PAUSE
#define L_RPAUSE    CMSLOG_FLAG_PAUSE_REPEAT
#define L_FCF       CMSLOG_FLAG_FC_FINTENSITY
#define L_FCNF      CMSLOG_FLAG_FC_UN_FINTENSITY
#define L_FCK       CMSLOG_FLAG_FC_COLOR_BLACK
#define L_FCW       CMSLOG_FLAG_FC_COLOR_WHITE
#define L_FCB       CMSLOG_FLAG_FC_COLOR_BLUE
#define L_FCG       CMSLOG_FLAG_FC_COLOR_GREEN
#define L_FCR       CMSLOG_FLAG_FC_COLOR_RED
#define L_FCBG      CMSLOG_FLAG_FC_COLOR_BLUE_GREEN
#define L_FCBR      CMSLOG_FLAG_FC_COLOR_BLUE_RED
#define L_FCGR      CMSLOG_FLAG_FC_COLOR_GREEN_RED 
#define L_FCRV      CMSLOG_FLAG_FC_COLOR_RV
#define L_LOCK      CMSLOG_FLAG_LOCK
#define L_UNLOCK    CMSLOG_FLAG_UNLOCK
#define L_LOCK_A    CMSLOG_FLAG_LOCK_A
#define L_UNLOCK_A  CMSLOG_FLAG_UNLOCK_A
//#define L_BCK       CMSLOG_FLAG_BC_COLOR_BLACK
//#define L_BCW       CMSLOG_FLAG_BC_COLOR_WHITE
//#define L_BCB       CMSLOG_FLAG_BC_COLOR_BLUE
//#define L_BCG       CMSLOG_FLAG_BC_COLOR_GREEN
//#define L_BCR       CMSLOG_FLAG_BC_COLOR_RED
//#define L_BCBG      CMSLOG_FLAG_BC_COLOR_BLUE_GREEN
//#define L_BCBR      CMSLOG_FLAG_BC_COLOR_BLUE_RED
//#define L_BCGR      CMSLOG_FLAG_BC_COLOR_GREEN_RED 

class CMsLog
{
public:
    // 构造
    CMsLog(LOG_FILE_INFO* pLogFileInfo, DWORD dwLogFileCount, BOOL bLock);
    CMsLog(TCHAR* szLogFileName, BOOL bLock);
    ~CMsLog(VOID);

    CMsLog& operator << (WCHAR Inof);       // 字符
    CMsLog& operator << (ACHAR Inof);

    CMsLog& operator << (WCHAR* strInof);   // 字符串
    CMsLog& operator << (ACHAR* strInof);

    CMsLog& operator << (INT nInof);        // 四字
    CMsLog& operator << (DWORD dwInof);

    CMsLog& operator << (INT64 n64Inof);    // 八字
    CMsLog& operator << (UINT64 u64Inof);

    CMsLog& operator << (FLOAT fInof);      // 浮点
    CMsLog& operator << (DOUBLE dInof);

    CMsLog& operator << (CMSLOG_FLAG LogFlag);  // 功能

    // 日志时间,指定属性
    BOOL Time(DWORD dwLogIndex, BOOL bSaveLog);
    BOOL TimeW(DWORD dwLogIndex, BOOL bSaveLog);
    BOOL TimeA(DWORD dwLogIndex, BOOL bSaveLog);

    // 日志时间,默认属性
    BOOL Time(VOID);
    BOOL TimeW(VOID);
    BOOL TimeA(VOID);

    // 记录日志,指定属性
    BOOL Log(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof);
    BOOL LogW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof);
    BOOL LogA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof);

    // 记录日志,默认属性
    BOOL Log(TCHAR* strInof);
    BOOL LogW(WCHAR* strInof);
    BOOL LogA(ACHAR* strInof);

    // 记录格式化日志,指定属性
    BOOL Logf(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof, ...);
    BOOL LogfW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof, ...);
    BOOL LogfA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof, ...);
    BOOL Logf(DWORD dwLogIndex, BOOL bSaveLog, TCHAR* strInof, va_list ap);
    BOOL LogfW(DWORD dwLogIndex, BOOL bSaveLog, WCHAR* strInof, va_list ap);
    BOOL LogfA(DWORD dwLogIndex, BOOL bSaveLog, ACHAR* strInof, va_list ap);

    // 记录格式化日志,默认属性
    BOOL Logf(TCHAR* strInof, ...);
    BOOL LogfW(WCHAR* strInof, ...);
    BOOL LogfA(ACHAR* strInof, ...);
    BOOL Logf(TCHAR* strInof, va_list ap);
    BOOL LogfW(WCHAR* strInof, va_list ap);
    BOOL LogfA(ACHAR* strInof, va_list ap);

    // 设置默认存储日志的索引
    DWORD SetFileIndex(DWORD dwFileIndex)
    {
        DWORD dwTemp = m_dwFileIndex;
        m_dwFileIndex = dwFileIndex;
        return dwTemp;
    }
    DWORD GetFileIndex(VOID){return m_dwFileIndex;}

    // 设置是否本地保存日志
    VOID  SetSaveLog(BOOL bSave){m_bSaveLog = bSave;}
    BOOL  GetSaveLog(VOID){return m_bSaveLog;}

    // 设置是否启用日志
    VOID  SetEnableLog(BOOL bEnable){m_bEnable = bEnable;}
    BOOL  GetEnableLog(VOID){return m_bEnable;}

    // 解析错误代码
    TCHAR* SwitchError(
        DWORD dwErrorCode,
        BOOL bShow = FALSE
        );

    // 查询最后错误
    TCHAR* ShowLastError(BOOL bShow = FALSE);

    // 按任意键继续
    VOID Pause(BOOL bRepeatShow = FALSE);
    VOID UnPause(VOID);
    VOID RePause(VOID);

    // 设置前景字颜色
    E_FOREGOUND_TEXT_COLOR SetFontColor(
        E_FOREGOUND_TEXT_COLOR eColor,
        OUT IN BOOL& bfIntensity
        );

    // 初始化、释放
    BOOL Init(VOID);
    BOOL Release(VOID);

private:

    // 送往日志服务器
    inline BOOL SendLogW(DWORD dwLogIndex, WCHAR* szLog);
    inline BOOL SendLogA(DWORD dwLogIndex, ACHAR* szLog);
    inline BOOL SendLog(DWORD dwLogIndex, TCHAR* szLog);

    // 显示日志
    inline BOOL ShowLogW(WCHAR* szLog);
    inline BOOL ShowLogA(ACHAR* szLog);
    inline BOOL ShowLog(TCHAR* szLog);

    // 写入本地日志
    inline BOOL WriteLogA(DWORD dwLogIndex, ACHAR* szLog);
    inline BOOL WriteLogW(DWORD dwLogIndex, WCHAR* szLog);
    inline BOOL WriteLog(DWORD dwLogIndex, TCHAR* szLog);

    // 送往日志服务器
    inline BOOL SendLogWPrivate(DWORD /*dwLogIndex*/, WCHAR* /*szLog*/);
    inline BOOL SendLogAPrivate(DWORD /*dwLogIndex*/, ACHAR* /*szLog*/);

    // 显示日志
    inline BOOL ShowLogWPrivate(WCHAR* szLog);
    inline BOOL ShowLogAPrivate(ACHAR* szLog);

    // 写入本地日志
    inline BOOL WriteLogWPrivate(DWORD dwLogIndex, WCHAR* szLog);
    inline BOOL WriteLogAPrivate(DWORD dwLogIndex, ACHAR* szLog);

    static CMsConsole*  s_pCMsConsole;      // 日志控制台对象
    static DWORD        s_dwLogCount;       // 日志实例个数
    //static MsLock       s_Loglock;          // 个数锁
    static MsLock       s_LogLock;          // 日志大锁
    MsLock              m_Loglock;          // 日志小锁

    // 成员
    LOG_FILE_INFO*  m_pLogFileInfo;         // 日志数组
    TCHAR           m_szLog[LOG_STR_MAX_LEN];
    DWORD           m_dwLogFileCount;       // 日志个数
    DWORD           m_dwFileIndex;          // 默认日志索引
    BOOL            m_bSaveLog;             // 是否保存日志
    BOOL            m_QuondamValueSaveLog;  // 之前日志是否保存
    BOOL            m_bEnable;              // 是否启用日志
    UINT64          m_u64TimeBegin;         // 日志计时起始时间
    BOOL            m_bPause;               // 是否是暂停等待状态
    BOOL            m_bfIntensity;          // 控制台前景是否高亮
    BOOL            m_bLock;                // 是否使用日志锁
};

//━━━━━━━━━━┓
#endif//#pragma once//┃
//━━━━━━━━━━┛
