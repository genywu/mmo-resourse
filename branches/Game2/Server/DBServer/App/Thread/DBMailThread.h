/**
*File:  DBMailThread.h
*brief: 对象类:该对象维护一个主线程使用的数据库Cn对象
---------------------------------------数据库线程类---------------------------------------
	工作线程由调用Begin（）开始，调用Exit（）结束。
 一旦调用了Exit（）接口，将不再接受新的任务，但已经压入的语句会继续执行完成。
 析构时会自动结束工作线程，但推荐手动调用Exit（）。
 主线程使用的数据库访问类,该类不会自己创建线程
------------------------------------------------------------------------------------------
*Author:安海川
*Date:  2009.02.17
*/
#pragma once

#include "ThreadDataDef.h"
#include "DBThread.h"

class DBAdoBase;
class DBMailThread : public DBThread
{
public:
	DBMailThread(std::string strProvider, 
				std::string strDataSource, 
				std::string strInitialCatalog,
				std::string strUserID, 
				std::string strPassword
				);
	~DBMailThread(void);

public:
	//!		
	virtual bool					ProcessData(void);
};

