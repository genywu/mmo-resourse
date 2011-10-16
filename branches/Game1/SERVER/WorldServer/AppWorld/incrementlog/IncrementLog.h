#pragma once
#ifndef _IncrementLog_h_
#define	_IncrementLog_h_

//#include"../../../../DBAccess/worlddb/dbIncrementLog.h"

class CIncrementLog
{
public:
	CIncrementLog(void);
	~CIncrementLog(void);

private:
	static CIncrementLog* _instance;	
public:
	static CIncrementLog* GetInstance();

public:
	struct tagIncrementLog
	{
		SYSTEMTIME	m_tTime;
		BYTE	m_byType;
		int		m_nMoney;
		string	m_sDesc;
	};

private:
//	CDbIncrementLog _DbIncrementLog;

	map<CGUID, vector<tagIncrementLog*>*> m_IncrementLogs;

public:
	// 在服务器启动的时候，载入所有增值日志。
	bool Load();
	void UnInitialize();
	
	// 追加一条。
	void Add(const CGUID& PlayerGuid, LPSYSTEMTIME pTime, BYTE byType, int nMoeny, const char *szDesc);
	// 删除指定玩家的所有日志
	void DelAllPlayerIncLog(const CGUID& playerGUID);
	vector<tagIncrementLog*>* GetIncrementLogsByPlayerID(const CGUID& PlayerGuid);
	int	GetIncrementLogsSizeByPlayerID(const CGUID& PlayerGuid);
	bool AddIncrementLogsToByteArray(vector<BYTE>& ByteArray, const CGUID& PlayerGuid);
};

#endif