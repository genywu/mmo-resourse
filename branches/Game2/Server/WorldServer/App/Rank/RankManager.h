/**
*File:  RankManager.h
*Brief: 管理所有排行数据
*Author:吴畏
*Date:  2010.03.08
*/
#pragma once

#include "../../../../Public/Common/RankDef.h"

class CRankManager
{
public:
    CRankManager(void);
    virtual ~CRankManager(void);

    void AddRankInfo(tagRankInfo *pRankInfo); 
    void ClearRankInfo();
    void RemoveRankInfo(long lRankID);

    // 通过排行名称找排行信息
    tagRankInfo* FindRankInfo(long lRankID);

    void ProcMsg(CMessage* pMsg);

private:
    map<long, tagRankInfo*> m_mapRankInfo;
 //   CRITICAL_SECTION    m_csRankInfo;           // 排行榜信息临界对象
};
