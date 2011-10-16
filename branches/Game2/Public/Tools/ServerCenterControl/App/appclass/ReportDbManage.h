

#include "DataCache.h"


#pragma once



class ReportDbManage
{
public:
	ReportDbManage(VOID);

public:
	//! 保存一个举报信息
	VOID SaveReportInfo(tagReportInfo &ReportInfo);
	//! 响应举报保存完成
	VOID OnSaveReportFinish(const DataCache<tagReportInfo>::tagDataBlock *pDataBlock);

	//! 检测缓存中是否有超时的信息
	VOID CheckCache(VOID);

	//! 读取一个WS上的举报列表到SUC
	//! 读取的数量细节由存储过程把握，方便改动
	VOID LoadReportList(__int64 ServerKey, LONG UserID);

	//! 读取一个WS上的举报处理结果到SUC
	VOID LoadResultList(__int64 ServerKey, LONG UserID, DWORD BeginDat);

	//! 记录一个GM对player的操作
	VOID LogOptPlayer(tagReportResultInfo &ReportResultInfo, const CGUID &OptGuid, __int64 ServerKey);

	//! 记录GM对player操作的执行结果
	VOID SaveOptPlayerResult(const CGUID &OptGuid, DWORD OptSuccp);

	

	
private:
	//! 
	DataCache<tagReportInfo> m_ReportInfoCache;
};