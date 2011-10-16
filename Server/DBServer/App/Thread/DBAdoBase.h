/**
*File:  DBAdoBase.h
*Brief: 对象类:该对象维护一个数据库Cn对象
 ---------------------------------------日志数据库操作的封装---------------------------------------
	只能执行无返回的SQL语句
 对象构造时自动连接DB，析构时释放连接
 连接若失效，每执行SQL语句时都会重新尝试连接
 增加执行结果集的操作(结果集对象指针，该类不设成员，由外部调用传入引用对象指针即可)
---------------------------------------------------------------------------------------------------
*Author:安海川
*Date:  2008.11.03
*/
#ifndef DB_ADOBASE_H
#define DB_ADOBASE_H

#include "../../../../Public/ServerPublic/db/CnPool.h"
class DBAdoBase
{
public:
	DBAdoBase(string strConnectionString);
	virtual ~DBAdoBase(void);

public:
	/// 执行SQL语句
	bool ExecuteCn(LPCSTR sql);
	// 取得连接对象
	_ConnectionPtr& GetConnPtr(void) { return m_cnPtr; }
	
	/// 打开连接
	bool OpenCn(void);
	/// 关闭连接
	bool CloseCn(void);
    /// 创建结果集
    long CreateRs(_RecordsetPtr &rs);
    /// 打开结果集
    long OpenRs(LPCSTR sql, _RecordsetPtr &rs,
        CursorTypeEnum cursorType = adOpenStatic,
        LockTypeEnum lockType = adLockReadOnly,
        long lOptions = adCmdText);

    /// 关闭结果集
    long CloseRs(_RecordsetPtr &rs);
    /// 释放结果集
    long ReleaseRs(_RecordsetPtr &rs);
    /// 执行结果集(创建、执行、释放)，若要返回结果集,不要调用该接口
    bool ExecuteRs(LPCSTR sql, _RecordsetPtr &rs);

	/// 主数据库连接对象
	_ConnectionPtr m_cnPtr;

	/// 连接字符串保存
	std::string m_strConnectionString;

	/// 错误输入#
	void PrintErr(LPCSTR msg, _com_error &e);
	void PrintErr(LPCSTR msg);
};

#endif