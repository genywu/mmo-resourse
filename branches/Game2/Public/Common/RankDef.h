/**
*File:  RankDef.h
*Brief: 排行榜数据定义
*Author:吴畏
*Date:  2010.03.10
*/

#ifndef _RANKDEF_H
#define _RANKDEF_H

#include "ColumnProperty.h"

///列类型
enum COL_TYPE
{
    CT_UNKNOWN = 0,
    CT_FLOAT,
    CT_LONG,
    CT_SHORT,
    CT_ULONG,
    CT_USHORT,
    CT_BOOL,
    CT_CHAR,
    CT_BYTE,
    CT_WORD,
    CT_DWORD,
    CT_STRING,
    CT_BUFFER,
    CT_GUID,
    CT_STRUCT,
    CT_TIME,
    CT_BIGINT,
};

///排行榜的类型枚举值
enum eRankType
{
    RT_NOMATCH = 0, /// 无匹配的排行数据
    RT_UPDATE,      /// 正在数据更新
    RT_OWNER,       /// 自己排行
    RT_OTHER,       /// 其他玩家排行
    RT_RANK,        /// 排名查询

};

/// 表列信息
struct tagColInfo
{
    string	            strColName;
    COL_TYPE	        dt;
};

/// 排行配置结构
struct tagRankConfig
{
    long    lRankID;            /// 排行榜编号
    long    lTopRecord;         /// 返回前N名
    long    lRefreshTime;       /// 定时更新该排行(分)
    string     strCurSQL;         /// 当前排行查询语句
    string     strUpdateTable;
    string     strUpdateSQL;      ///
};

/// 表数据信息
struct tagRankInfo
{
    long          lRankID;          /// 排行榜编号
    ulong         ulUpdateTime;     /// 更新时间
    long          lTotalPage;       /// 数据总页面
    long          lCurPage;         /// 当前数据所在页
    CRITICAL_SECTION    csRankInfo;
    vector<tagColInfo> vecColInfo;
    vector<CTableManager*> vecColData;

    tagRankInfo(long lID = 0)
        :lRankID(lID),ulUpdateTime(0),lTotalPage(0),lCurPage(0)
    {
        vecColInfo.clear();
        vecColData.clear();
        InitializeCriticalSection(&csRankInfo);
    }
    ~tagRankInfo()
    {
        EnterCS();
        Clear();
        LeaveCS();
        DeleteCriticalSection(&csRankInfo);
    }

    void Clear()
    {
        vecColInfo.clear();
        vector<CTableManager*>::iterator itor = vecColData.begin();
        for(; itor != vecColData.end(); ++itor)
        {
            SAFE_DELETE((*itor));
        }
        vecColData.clear();
    }

    bool TryEnterCS()  
    { 
        if(TryEnterCriticalSection(&csRankInfo))
        {
            return true;
        }
        return false;
    }

    void EnterCS()  { EnterCriticalSection(&csRankInfo); }
    void LeaveCS()  { LeaveCriticalSection(&csRankInfo); }

    bool AddToByteArray(vector<BYTE>* pByte);
    void DecordFromByteArray(BYTE* pByte, long& lPos);

    /// 条件筛选
    bool AddToByteArray(vector<BYTE>* pByte, long lPosBegin, long lPosEnd);
    bool AddToByteArray(vector<BYTE>* pByte, CDataObject* pEntity, long lColPos);
    bool AddToByteArray(vector<BYTE>* pByte, CDataObject* pEntity, const char* szColName);
    bool AddToByteArray(vector<BYTE>* pByte, long lPage);

    /// 寻找表列在Col Vector中的位置
    long FindColPos(const char* szColName);
    /// 寻找条件数据在Data Vector中的位置
    long FindDataPos(CDataObject* pEntity, long lColPos);

    /// 总页面数
    long GetPageCount();
    /// 找到数据所在页面
    long FindPage(long lDataPos);
};

#endif //_RANKDEF_H