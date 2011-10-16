#pragma once

#include "..\..\..\dbaccess\MyAdoBase.h"

enum eOperType
{
	OT_BOUNS_INFO = 1,			//	查询奖励
	OT_BOUNS_FREEZE,			//	冻结奖励
	OT_BOUNS_GET,				//	领取奖励
	OT_BOUNS_GET_ROLLBACK,		//	回滚奖励

	OT_BOUNS_EXIT_THREAD,		//	退出操作线程
};

class CBounsOper
{
public:
	CBounsOper(void);
	CBounsOper(eOperType eOt);
	virtual ~CBounsOper(void);

protected:
	eOperType		m_eOperType;		// 操作类型
	_ConnectionPtr	m_cnConnPtr;
	long			m_lGSSocketID;		// socket id 

	CGUID					m_guidPlayer;



public:
	eOperType GetOperType()				{ return m_eOperType; }
	void SetOperType(eOperType eOt)		{ m_eOperType = eOt; }

	void SetConnectionPtr(_ConnectionPtr cn)	{ m_cnConnPtr = cn; }

	void SetGSSocketID(long lSocketID)		{ m_lGSSocketID = lSocketID; }
	long GetGSSocketID()					{ return m_lGSSocketID; }

	void SetPlayerID(CGUID &guid)			{ m_guidPlayer = guid; }
	const CGUID& GetPlayerID()				{ return m_guidPlayer; } 

	virtual	BOOL DoOperator();

	void GetCurrentTimeString(string& strTime);

};
