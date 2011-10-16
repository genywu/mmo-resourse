/**
*	file:		
*	Brief:		
*	detail:		
*	
*				
*	Authtor:	张科智
*	Date:		2008-10-23
*/
#pragma once
#include "../../../Public/ServerPublic/db/CnPool.h"

const DWORD MAX_SQL_STRING_SIZE = 4096;

class DbOperator
{
public:
	static	VOID			Release(VOID);

public:
	//!						添加一个执行数据信息
	static	BOOL			AddExecuteData(LONG lDbID, string &strDbConn, string &strErrorLogFileName, string &strBusyLogFileName);
	//!						释放所有执行数据信息
	static	VOID			ReleaseExecuteData(VOID);

	//!						创建一个操作对象
	static	DbOperator*		CreateDbOperator(LONG lDbID);
	//!						释放对象
	static	VOID			ReleaseDbOperator(DbOperator **ppDbOperator);

public:
	//!						添加一条执行语句
	VOID					AddSql(LPCSTR pSql){m_strSql.append(pSql);}
	//!						添加一条执行语句
	size_t					GetSqlSize(VOID) {return m_strSql.size();}
	//!						执行
	VOID					ExecuteSql(VOID);

private:
	//!						执行线程
	static	LPVOID			ExecuteThread(LPVOID pThis);
	//!						DB失败处理
	static	LPVOID			OnExecuteFinish(LPVOID pThis);

private:
	struct tagExecuteData
	{
		//!					数据库连接字符串
		//string				_strDbConn;
		CnPool				*pCnPool;
		//!					错误记录文件名称
		string				_strErrorLogFileName;
		//!					繁忙记录文件名称
		string				_strBusyLogFileName;
		//!					写文件临界区
		CRITICAL_SECTION	_FileCriticalSection;
	};

	tagExecuteData			*m_pExecuteData;
	string					m_strSql;

private:

	static	map<LONG, tagExecuteData>				c_mapExecuteData;

	static	MultiObjPool<DbOperator, LockOfWin32>	c_MultiObjPool;

private:
	DbOperator(VOID){}
	DbOperator(DbOperator&);
	~DbOperator(VOID){}
};