//DBAdoBase.h/////////////////////////////////////////////////////////////////////
//对象类:该对象维护一个数据库Cn对象
//Author:安海川
//Create Time:2008.11.03
#ifndef DB_ADOBASE_H
#define DB_ADOBASE_H

#pragma warning (disable: 4146)
#import "../../dbaccess/msado15.dll" no_namespace rename("EOF","adoEOF")

#include "../public/GUID.h"
#include <string>



//! ---------------------------------------日志数据库操作的封装---------------------------------------
//!	只能执行无返回的SQL语句
//! 对象构造时自动连接DB，析构时释放连接
//! 连接若失效，每执行SQL语句时都会重新尝试连接
//!---------------------------------------------------------------------------------------------------
class DBAdoBase
{
public:
	DBAdoBase(	std::string strConnectionString);

	virtual ~DBAdoBase(void);

public:
	//! 执行SQL语句
	bool ExecuteCn(LPCSTR sql);
	// 取得连接对象
	_ConnectionPtr& GetConnPtr(void) { return m_cnPtr; }
	
	//! 打开连接
	bool OpenCn(void);
	//! 关闭连接
	bool CloseCn(void);

	//! 主数据库连接对象
	_ConnectionPtr m_cnPtr;

	//! 连接字符串保存
	std::string m_strConnectionString;

	// 错误输入#
	void PrintErr(LPCSTR msg, _com_error &e);
	void PrintErr(LPCSTR msg);
};

#endif