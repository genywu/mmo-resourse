
#pragma once


//! 日志文本最大长度
const DWORD MAX_ACTION_LOG_TEXT_SIZE	= 1024;
//! 一组日志的数量
const DWORD MAX_LOG_GROUP_SIZE			= 16;

struct tagDbLog
{
	DWORD	dwTime;
	DWORD	dwSmID;
	DWORD	dwGappID;
	char	szUserName[MAX_USER_NAME_SZIE];
	char	szInfo[MAX_ACTION_LOG_TEXT_SIZE];

	tagDbLog(VOID){memset(this, 0, sizeof(tagDbLog));}
};


///////////////////////////////////////////////////////////
class ActionLogManage
{
public:
	ActionLogManage(VOID);
	~ActionLogManage(VOID);

public:
	//!
	static	ActionLogManage&		GetInstance(VOID) {return c_ActionLogManage;}
	//!								响应写入完成
	static		VOID				OnDBWriteFinish(tagDbLog *pDbLog, DWORD dwNum, BOOL bSucceed);

public:
	//!								初始化对象
	BOOL							Init(VOID);
	//!								释放对象
	VOID							Release(VOID);

public:
	//!								向DB写入一个日志
	VOID							ActionLog(DWORD dwSmID, DWORD dwGappID, const char * pUserName, const char *pInfo, ...);

private:
	//!								请求向数据库写入数据
	VOID							RequestWriteDB(tagDbLog *pDbLog, DWORD dwNum);

private:
	//!								当前缓存的日志
	tagDbLog						*m_pCurrtagDbLog;
	//!								当前已使用的缓存数
	DWORD							m_dwCurrUseNum;

private:
	//!
	static	ActionLogManage			c_ActionLogManage;
	//!								正在写入数据库的数据
	static	set<tagDbLog*>			m_setWriting;
};