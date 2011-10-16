#ifndef DB_UNION_H
#define DB_UNION_H
#include "BaseDBEntity.h"
#include "../public/DBDefine.h"
#include "../../../public/OrganizingDef.h"
#include <list>

using namespace std;




class CDBUnion : public CBaseDBEntity
{
public:
	CDBUnion();
	CDBUnion(const CGUID& guid);
	~CDBUnion();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);

private:
	// 等待被发送给DBS的属性链表
	vector<long> m_SendAttrEnumVector;
};

#endif//DB_UNION_H