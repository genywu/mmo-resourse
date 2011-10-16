
#include "../../../../../public/Common/BaseDef.h"
#include "../../../../../Public/ServerPublic/Server/Common/AsynchronousExecute.h"
#include "../../../../../Public/ServerPublic/db/CnPool.h"
#include "../../../../SCMPublic/class/CGameInfo.h"
#include "DataCache.h"

struct tagDbLog;

#pragma once




class DbOperator
{
public:
	//!										初始化对象
	static		BOOL						Init(VOID);
	//!										释放对象
	static		VOID						Release(VOID);

public:
	//!										创建一个密码比较操作
	static		DbOperator*					CreatePwdCmpOperator(const char *pCdkey, const char *pMD5Pwd);
	//!										创建一个密码修改操作
	static		DbOperator*					CreatePwdChangeOperator(const char *pCdkey, const char *pMD5Pwd);
	//!										创建一个硬件信息组写入操作
	static		DbOperator*					CreateInHardOperator(DWORD dwSMID, DWORD dwNum, tagHardwareInfo *pHardwareInfo);
	//!										创建一个写入DB的日志组写入操作
	static		DbOperator*					CreateIDBLogOperator(tagDbLog *pDbLog, DWORD dwNum);
	//!										创建一个游戏信息组写入操作
	static		DbOperator*					CreateGameInfoOperator(DWORD dwSMID, DWORD dwGappID, tagDbGameInfo *pDbGameInfo, DWORD dwNum);
	//!										创建一个游戏信息组读取操作
	static		DbOperator*					CreateLoadGameInfo(DWORD dwSMID, DWORD dwGappID, DWORD dwInfoType, tagDbGameInfo *pDbGameInfo, DWORD dwNum, DWORD dwBeginTime, DWORD dwEndTime, LONG lUserID);

	//!										创建一个保存举报信息的操作
	static		DbOperator*					CreateSaveReport(VOID *pDataBlock);
	//!										创建一个读取举报列表的操作
	static		DbOperator*					CreateLoadReport(__int64 ServerKey, LONG UserID);
	//!										创建一个读取玩家处理结果的操作
	static		DbOperator*					CreateLoadResult(__int64 ServerKey, LONG UserID, DWORD BeginDat);
	//!										创建一个保存玩家处理结果成功的操作
	static		DbOperator*					CreateSaveOptResult(const CGUID &OptGuid, DWORD OptSuccp);
	//!										创建一个执行SQL语句的操作
	static		DbOperator*					CreateRunSql(const char *pSql, DWORD SqlSize);

	//!										销毁操作
	static		VOID						ReleaseOperator(DbOperator **ppOperator);

public:
	//!										执行操作
	BOOL									Begin(VOID);

	//! 私有对象
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	//!										读取配置
	static		BOOL						LoadSetup(VOID);

	BOOL									CreateData(VOID);
	VOID									ReleaseData(VOID);

	
private:
	//!										密码比较操作
	static		LPVOID						PwdCmp(LPVOID pThis);
	//!										密码修改操作
	static		LPVOID						PwdChange(LPVOID pThis);
	//!										硬件信息组写入操作
	static		LPVOID						InHard(LPVOID pThis);
	//!										日志组写入操作
	static		LPVOID						DbLog(LPVOID pThis);
	//!										日志组写入操作
	static		LPVOID						GameInfo(LPVOID pThis);
	//!										日志读取操作
	static		LPVOID						LoadGameInfo(LPVOID pThis);

	//!										保存举报信息的操作
	static		LPVOID						SaveReport(LPVOID pThis);
	//!										读取举报信息
	static		LPVOID						LoadReport(LPVOID pThis);
	//!										读取处理结果信息
	static		LPVOID						LoadResult(LPVOID pThis);
	//!										保存玩家处理结果成功的操作
	static		LPVOID						SaveOptResult(LPVOID pThis);
	//!										执行一段无返回值的SQL语句
	static		LPVOID						RunSql(LPVOID pThis);

private:
	//! 数据库操作对象类型
	enum eDbOptObjType
	{
		//! 比较用户密码
		eDOOT_PwdCmp,
		//! 修改密码
		eDOOT_PwdChange,
		//! 硬件信息组
		eDOOT_InHard,
		//! 要写入DB的日志组
		eDOOT_DBLogGroup,
		//! 要写入DB的游戏信息组
		eDOOT_GameInfoGroup,
		//! 读取DB游戏信息
		eDOOT_LoadGameInfo,

		//! 保存举报信息
		eDOOT_SaveReport,
		//! 读取举报信息
		eDOOT_LoadReport,
		//! 保存处理结果
		eDOOT_SaveOptResult,
		//! 读取玩家处理信息
		eDOOT_LoadResult,
		//! 执行一段无返回值的SQL语句
		eDOOT_RunSql,
	};

	//! 数据库操作对象
	//!-------------------------------------------------
	struct tagUserInfo
	{
		char szCdkey[MAX_USER_NAME_SZIE];
		char szPwd[BASE_PASSWORD_SIZE];
		tagUserInfo(VOID){memset(this, 0, sizeof(tagUserInfo));}
	};

	struct tagHardInfoGroup
	{
		DWORD			dwSMID;
		DWORD			dwNum;
		tagHardwareInfo	*pHardwareInfo;	
	};

	struct tagDBLogGroup
	{
		tagDbLog		*pDbLog;
		DWORD			dwNum;
	};

	struct tagGameInfoGroup 
	{
		DWORD			dwSMID;
		DWORD			dwGappID;
		tagDbGameInfo	*pDbGameInfo;
		DWORD			dwNum;
	};

	struct tagLoadGameInfo
	{
		DWORD			dwSMID;
		DWORD			dwGappID;
		tagDbGameInfo	*pDbGameInfo;
		DWORD			dwNum;
		DWORD			dwInfoType;
		DWORD			dwBeginTime;
		DWORD			dwEndTime;
		LONG			lUserID;
	};

	struct tagLoadReport
	{
		__int64			ServerKey;
		LONG			UserID;
	};
	struct tagLoadResult
	{
		__int64			ServerKey;
		LONG			UserID;
		DWORD			BeginDate;
	};

	struct tagSaveOptResult 
	{
		CGUID OptGuid;
		DWORD OptSuccp;
	};

	struct tagDbSql 
	{
		const char		*pSql;
		DWORD			SqlSize;
	};

	//!-------------------------------------------------

private:
	//!										对象类型
	DWORD									m_dwDOOT;
	//!										数据指针
	VOID									*m_pData;

private:
	static		AsynchronousExecute			c_AsynchronousExecute;
	static		CnPool						c_CnPool;

	//! 不允许构造和拷贝
private:
	DbOperator(DWORD dwDOOT);
	DbOperator(DbOperator&);
	~DbOperator(void);
};