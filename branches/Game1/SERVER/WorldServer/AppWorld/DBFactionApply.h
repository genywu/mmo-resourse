#ifndef DB_FactionApply_H
#define DB_FactionApply_H
#include "BaseDBEntity.h"
#include <list>
#include "..\..\..\public\GUID.h"
#include "../public/DBDefine.h"

using namespace std;


class CDBFactionApply : public CBaseDBEntity
{
public:


private:
	CDBFactionApply();
public:
	CDBFactionApply(const CGUID& guid);
	virtual ~CDBFactionApply();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	
		
	void AddEntityDataToMsg(DB_OPERATION_TYPE opType,vector<BYTE>& pBA);

	CDBFactionApply& operator=(CDBFactionApply& right)
	{
		CBaseDBEntity::operator=(right);
		if(this != &right)
		{

		}

		return *this;
	}

	// 是否是刚创建还未被Load/Save线程处理过的对象
	bool	GetIsFirstCreated(void) { return b_IsFirstCreated; }
	void	SetIsFirstCreated(bool flag) { b_IsFirstCreated = flag; }

	// 初始化属性配置
	void				InitProperty();

private:
	bool b_IsFirstCreated;
};

#endif//DB_GOOD_H