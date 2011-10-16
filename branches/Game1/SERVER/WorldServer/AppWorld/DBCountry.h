#ifndef DB_COUNTRY_H
#define DB_COUNTRY_H
#include "BaseDBEntity.h"
#include <list>
#include "../public/DBDefine.h"

using namespace std;


class CDBCountry : public CBaseDBEntity
{
public:
	CDBCountry();
	CDBCountry(const CGUID& id);
	~CDBCountry();

	BYTE GetID(void) { return m_ID; }
	void SetID(BYTE id) { m_ID = id; }

	// virtual interfaces
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos);
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos);


	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

	long	GetPower(void)		{ return m_lPower; }
	long	GetTechLevel(void)	{ return m_lTechLevel;}
	const CGUID&	GetKing(void)		{ return m_lKing;}

	void	SetPower	(long value)	{ m_lPower = value; }
	void	SetTechLevel(long value)	{ m_lTechLevel = value;}
	void	SetKing		(const CGUID& value)	{ m_lKing = value;}

	// Other interfaces
	// 准备发送的属性
	//vector<DB_CARD_ATTR_TYPE>& GetSendAttrEnumVector(void) { return m_SendAttrEnumVector; }
	// 打开准备发送的属性在Vector中的相应位置的标志
	void OpenAttrFlagInVector(DB_COUNTRY_ATTR_TYPE attrType);
	// 关闭准备发送的属性在Vector中的相应位置的标志
	void CloseAttrFlagInVector(DB_COUNTRY_ATTR_TYPE attrType);
	// 组装消息
	void AddEntityDataToMsg(DB_OPERATION_TYPE opType, vector<BYTE>& pBA);

private:
	// 等待被发送给DBS的属性链表
	vector<long> m_SendAttrEnumVector;
	//////////////////////////////////////////////////////////////////////////
	long	m_lPower;
	long	m_lTechLevel;
	CGUID	m_lKing;

	BYTE    m_ID;
};

#endif//DB_COUNTRY_H