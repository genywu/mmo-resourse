#pragma once

#include "../../../Public/ServerPublic/db/CnPool.h"

//  主要传递一个数据库连接对象的指针，其实可以在BounsDB中直接实现
// 
class CBounsAdoBase
{
public:
	CBounsAdoBase(string strConnectionString);
	virtual ~CBounsAdoBase(void);	

	bool OpenCn();
	bool CloseCn();

	bool ExcuteSQL(LPCSTR sql);

	void PrintErr(LPCSTR msg, _com_error &e);
	void PrintErr(LPCSTR msg);

	_ConnectionPtr& GetConnPtr()	{ return m_cnPtr; }

private:
	_ConnectionPtr	m_cnPtr;
	string			m_strConnectionString;
};
