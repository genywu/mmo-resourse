#ifndef DB_SETUP_H
#define DB_SETUP_H
#include "BaseDBEntity.h"
#include "..\..\..\public\Date.h"
#include "../public/DBDefine.h"
#include <list>

using namespace std;

class CDBSetup : public CBaseDBEntity
{
public:	
	CDBSetup();
	CDBSetup(const CGUID& id);
	~CDBSetup();

	long GetID(void) { return m_ID; }
	void SetID(long id) { m_ID = id; }

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	void Load(long msgid, BYTE* msgBuf, long& msgBufPos);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);


	long				GetLeaveWordID(void) { return m_lLeaveWordID;}
	const CGUID&		GetPlayerID(void) { return m_lPlayerID;}

	void SetLeaveWordID(long value) { m_lLeaveWordID = value;}
	void SetPlayerID(const CGUID& value) { m_lPlayerID = value;}

	// Other interfaces
	// 准备发送的属性
	//vector<DB_CARD_ATTR_TYPE>& GetSendAttrEnumVector(void) { return m_SendAttrEnumVector; }
	// 打开准备发送的属性在Vector中的相应位置的标志
	void OpenAttrFlagInVector(DB_SETUP_ATTR_TYPE attrType);
	// 关闭准备发送的属性在Vector中的相应位置的标志
	void CloseAttrFlagInVector(DB_SETUP_ATTR_TYPE attrType);
	// 组装消息
	void AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA);

private:
	// 等待被发送给DBS的属性链表
	vector<long> m_SendAttrEnumVector;
	long	m_lLeaveWordID;
	CGUID	m_lPlayerID;
	long	m_ID;
};

#endif//DB_SETUP_H