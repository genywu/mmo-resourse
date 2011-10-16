#ifndef DB_RGN_GOOD_H
#define DB_RGN_GOOD_H
#include "BaseDBEntity.h"
#include <list>
#include "..\..\..\public\GUID.h"
#include "../public/DBDefine.h"

using namespace std;


class CDBRgnGoods : public CBaseDBEntity
{
public:


private:
	CDBRgnGoods();
public:
	CDBRgnGoods(const CGUID& guid);
	virtual ~CDBRgnGoods();

	// virtual interfaces
	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);


	//void Load(long msgid, BYTE* msgBuf, long& msgBufPos, vector<BYTE>& pBA);
	void Load(long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1);
	
		
	void AddEntityDataToMsg(DB_OPERATION_TYPE opType,vector<BYTE>& pBA);

	CDBRgnGoods& operator=(CDBRgnGoods& right)
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

	// 检查更新属性里是否有指定表名的属性名
	bool				FindTableUpdateFlag(const string& tableName);

private:
	bool b_IsFirstCreated;
};

#endif//DB_GOOD_H