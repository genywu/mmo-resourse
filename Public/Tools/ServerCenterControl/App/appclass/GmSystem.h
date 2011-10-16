
#include "SmManage.h"
#include "UserManage.h"
#include "ReportDbManage.h"

#pragma once


class GmSystem
{
public:
	//!
	static GmSystem& GetInstance(VOID) {static GmSystem GmSystmereturn; return GmSystmereturn;}

public:
	//! 接收SM发来的举报
	VOID OnReport(tagReportInfo &ReportInfo);
	//! 监视开始/结束
	VOID OnWatch(DWORD SMID, DWORD GappID, BOOL IsBegin);
	//! 响应GM命令信息发送
	VOID OnCommandToGm(DWORD WsSmid, DWORD GappID, DWORD GmCommand, const char *pChatTxt);

public:
	//! GM接管一个WS
	VOID GmInWs(DWORD WsSmid, DWORD GappID, const OnlineUser *pUser);
	//! GM退出一个WS
	VOID GmOutWs(LONG UserID);


	//! 以下接口，需要在接管WS后才能调用
public:
	//! 响应SUC发来的举报查看请求
	VOID OnViewReport(LONG UserID);
	//! 响应SUC发来的处理结果的查看请求
	//! 可查看指定时间开始，一天内的处理结果
	VOID OnViewResult(LONG UserID, DWORD BeginDate);
	//! 响应SUC的账号监视请求
	VOID OnScoutPlayer(const char *PlayerName, BOOL IsWatch, LONG UserID);
	//! 响应SUC的账号处理请求
	VOID OnOptPlayer(tagReportResultInfo &ReportResultInfo, LONG UserID);
	//! 响应SUC发来的发送公告请求
	VOID OnSendAffiche(const char *pAffiche, LONG UserID, DWORD SMID, DWORD GappID);
	//! 响应GM命令发送
	VOID OnCommandToPlayer(DWORD GmCommand, const char *pChatTxt, LONG UserID);

	//! 删除一个GM的管理信息
	VOID DelListen(LONG UserID);

public:
	//!
	ReportDbManage& GetReportDbManage(VOID) {return m_ReportDbManage;}
	//!
	VOID EndScoutPlayer(string *pStrLog, const char *PlayerName, LONG UserID);
	//! 创建一个SMID+玩家名字关联的名字
	VOID CreatePlayerName(string &outName, const char *PlayerName, DWORD SMID);

public:
	union tagKey
	{
		__int64 _64Key;
		struct  
		{
			DWORD _SMID;
			DWORD _GappID;
		};

		tagKey(VOID):_64Key(0){}
		tagKey(__int64 Key64):_64Key(Key64){}
		tagKey(DWORD SMID, DWORD GappID):_SMID(SMID),_GappID(GappID){}
	};

private:
	struct tagListenInfo
	{
		tagKey _ServerID;
		LONG _SmSocket;
		const OnlineUser *_pUser;
		BOOL _IsWatching;
		string _WatchPlayerName;
		//! 最后一次操作的时间，如果这个时间过早，在其他用户的请求进入下，当前用户被清除
		DWORD LastOptTime;
		//! 上一次读取举报信息的时间（ms）
		DWORD LastLoadReportTime;
		//! 上一次读取处理结果的时间（ms）
		DWORD LastLoadResultTime;
	};

	struct tagWatchInfo
	{
		LONG _UserID;
		string _strInfo;
	};

private:
	
	//! 找到GM管理的一个WS
	tagListenInfo* FindGmWs(LONG UserID, eGMTPurviewType TestPurview=ePT_Penniless);
	//! 格式化一个命令到日志字符串
	static VOID FormatLogToString(string &strLog, DWORD GmCommand, const char *pChatTxt, BOOL GmToS);


private:
	//! 数据库管理对象
	ReportDbManage				m_ReportDbManage;
	//! 接管信息 map<UserID, SMID>
	map<LONG, tagListenInfo>	m_UserToSm;
	map<__int64, LONG>			m_SmToUser;
	//! 被监视的player
	map<string, tagWatchInfo>	m_PlayerToUser;
};