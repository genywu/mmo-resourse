//DBThread.h/////////////////////////////////////////////////////////////////////
//对象类:该对象维护一个数据库操作使用的线程,所有操作都在线程内完成
//Author:安海川
//Create Time:2008.11.03
#pragma once
#include "..\..\dbaccess\myadobase.h"
#include <list>
#include "ThreadDataDef.h"


class DBAdoBase;
class CBaseEntity;

//! ---------------------------------------数据库线程类---------------------------------------
//!	工作线程由调用Begin（）开始，调用Exit（）结束。
//! 一旦调用了Exit（）接口，将不再接受新的任务，但已经压入的语句会继续执行完成。
//! 析构时会自动结束工作线程，但推荐手动调用Exit（）。
//!---------------------------------------------------------------------------------------------------
class DBThread
{
public:
	DBThread(std::string strProvider, 
				std::string strDataSource, 
				std::string strInitialCatalog,
				std::string strUserID, 
				std::string strPassword,
				eThreadType type);
	~DBThread(void);

public:
	//!								开始工作线程工作
	bool							Begin(void);
	//!								结束工作线程
	void							Exit(void);
	//!								取得线程ID
	long							GetThreadID(void) { return m_lThreadID; }
	//!								线程退出等待句柄
	HANDLE&							GetQuitWaitHandle(void) { return m_hQuitWait; }
protected:
	DBThread(void);

	//!								递归执行根对象的数据库操作
	HRESULT							TravelRootDbOper(CEntityGroup* pRoot, _ConnectionPtr& cn);
	//!								执行一次数据库操作
	HRESULT							DbOperEntityAttrs(CEntityGroup* pRoot, _ConnectionPtr& cn);
	HRESULT							DbOperAllEntityGroup(CEntityGroup* pRoot, _ConnectionPtr& cn);
	HRESULT							DbOperLeaves(DB_OPERATION_TYPE dbOperType, const char* szSql, 
										const char* szTblName, const string& strComFlag, 
										map<CGUID, CBaseEntity*>& entityMap, CEntityGroup* pRoot, _ConnectionPtr& cn);
	//!		
	virtual bool					ProcessData(void);
	//!								线程
	static	unsigned	__stdcall	RunThread(void *pThis);

	bool							OpenCn(void);
	DBAdoBase*						GetAdoBasePtr(void);

	//!
	DBAdoBase*			m_pConnPtr;
	//!					数据库名称
	std::string			m_strInitialCatalog;
	//!					线程ID
	unsigned			m_lThreadID;
	//!					线程退出等待句柄
	HANDLE				m_hQuitWait;
	map<CGUID,ThreadData*>   m_listData;
	eThreadType			m_ThreadType;
};

