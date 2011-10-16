#ifndef DB_LINKMAN_H
#define DB_LINKMAN_H
#include "BaseDBEntity.h"
#include <list>
#include "..\..\..\public\GUID.h"
#include "../public/DBDefine.h"

using namespace std;

class CDBLinkMan : public CBaseDBEntity
{
public:
private:
	CDBLinkMan();
public:
	CDBLinkMan(const CGUID& guid);
	virtual ~CDBLinkMan();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	
	void Save(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void AddEntityDataToMsg(DB_OPERATION_TYPE opType,vector<BYTE>& pBA);

	CDBLinkMan& operator=(CDBLinkMan& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{
			b_IsFirstCreated = right.b_IsFirstCreated;
		}

		return *this;
	}

	// 是否是刚创建还未被Load/Save线程处理过的对象
	bool	GetIsFirstCreated(void) { return b_IsFirstCreated; }
	void	SetIsFirstCreated(bool flag) { b_IsFirstCreated = flag; }

	// 初始化属性配置
	void				InitProperty();

	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);

private:
	bool b_IsFirstCreated;
};

#endif//DB_GOOD_H