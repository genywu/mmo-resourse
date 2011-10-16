#ifndef BASE_DB_ENTITY_H
#define BASE_DB_ENTITY_H

#include <map>
#include <list>
#include <vector>
#include <string>

#include "..\..\..\nets\netdb\Message.h"
#include "../public/GUID.h"
#include "../../../public/entityproperty.h"

/*
消息设计：
|----
|opType			4byte, 操作对象类型
|OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
|DBOP Type:     1byte, 数据库操作类型（枚举值）
|MSG ID:        4byte, 该消息的MSGID
|Entity Num:	2byte, 该实体个数(X)
---------------从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码----------------
|	|----
|   |EntityID:		4byte, 实体ID
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|	 |该属性个数:		2byte, 属性数据个数(M)
|X*	|N * |该属性类型:		4byte, 属性类型枚举值
|	|	 |		|----
|	|	 |	M *	|该属性数据块:	nbtye, 该数据块长度由属性数据长度决定 (若消息为LOAD则该数据块为1byte，值为0)
|	|	 |		|----
|	|	 |----
|	|----

*/

#include "dbentitydef.h"

class CBaseDBEntity
{
public:
	CBaseDBEntity()
		: m_GUID(0),b_IsFirstCreated(true)
	{
	};
	CBaseDBEntity(const CGUID& guid)
		: m_GUID(guid),b_IsFirstCreated(true)
	{
	};

	virtual ~CBaseDBEntity()
	{
	};

	virtual void ProcessMsg(DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long& msgBufPos, long bufSize=-1) = 0;
	
	// 是否是刚创建还未被Load/Save线程处理过的对象
	bool	GetIsFirstCreated(void) { return b_IsFirstCreated; }
	void	SetIsFirstCreated(bool flag) { b_IsFirstCreated = flag; }

	// GetID
	const CGUID& GetExID(void) { return m_GUID; }
	// SetID
	void SetExID(const CGUID& id) { m_GUID = id; }

	virtual CBaseDBEntity& operator=(CBaseDBEntity& right)
	{
		if(this != &right)
		{
			m_GUID = right.m_GUID;
			b_IsFirstCreated = right.b_IsFirstCreated;
			CDataEntityManager::EntityPropertyMapItr itr = right.m_DataEntityManager.GetEntityPropertyMap().begin();
			for(; itr != right.m_DataEntityManager.GetEntityPropertyMap().end(); itr++)
			{
				CEntityProperty* ep = m_DataEntityManager.CreateEntityProperty(itr->first, itr->second->GetDBTableName(), itr->second->GetDataType());
				*ep = *itr->second;
			}
			m_mapDirtyAttrData = right.m_mapDirtyAttrData;
		}
		return *this;
	}

	// 初始化属性配置
	virtual void InitProperty(const char* szFilePath) {};

	CDataEntityManager& GetDataEntityManager(void) { return m_DataEntityManager; }

	void    PushDirtyAttrData(string attrName) { m_mapDirtyAttrData.insert(attrName); }
	string  PopDirtyAttrData() 
	{ 
		string tmp = *m_mapDirtyAttrData.begin(); 
		m_mapDirtyAttrData.erase(m_mapDirtyAttrData.begin());
		return tmp; 
	}
	long    DirtyAttrDataSize(void) { return m_mapDirtyAttrData.size(); }
	set<string>& GetDirtyAttrDataMap(void) { return m_mapDirtyAttrData; }


	// 设置数据对象值
	void			SetLongAttr(const std::string& name, const std::string& tableName, long pos, long value);
	void			SetStringAttr(const std::string& name, const std::string& tableName, long pos, const char* value);
	void			SetBufAttr(const std::string& name, const std::string& tableName, long pos, void* buf, long bufSize);
	void			SetTimeAttr(const std::string& name, const std::string& tableName, long pos, long* buf, long bufSize);
	void			SetGuidAttr(const std::string& name, const std::string& tableName, long pos, const CGUID& guid);

	// 设置数据对象值
	long			GetLongAttr(const std::string& name, long pos);
	const char*		GetStringAttr(const std::string& name, long pos);
	void			GetBufAttr(const std::string& name, long pos, void** buf, long bufSize);
	void			GetTimeAttr(const std::string& name, long pos, long* buf, long bufSize);
	void			GetGuidAttr(const std::string& name, long pos, CGUID& guid);

	// 创建数据对象值
	void			AddLongAttr		(const std::string& name, const std::string& tableName, long pos, DATA_OBJECT_TYPE type, long value);
	void			AddStringAttr	(const std::string& name, const std::string& tableName, long pos, const char* value);
	void			AddBufAttr		(const std::string& name, const std::string& tableName, long pos, void* buf, long bufSize);
	void			AddTimeAttr		(const std::string& name, const std::string& tableName, long pos, long* buf, long bufSize);
	void			AddGuidAttr		(const std::string& name, const std::string& tableName, long pos, const CGUID& guid);

	// 删除数据对象值
	void			DelLongAttr		(const std::string& name, const std::string& tableName, long pos);
	void			DelStringAttr	(const std::string& name, const std::string& tableName, long pos);
	void			DelBufAttr		(const std::string& name, const std::string& tableName, long pos);
	void			DelTimeAttr		(const std::string& name, const std::string& tableName, long pos);
	void			DelGuidAttr		(const std::string& name, const std::string& tableName, long pos);

protected:
	CGUID m_GUID;
	// 是否是刚创建还未被Load/Save线程处理过的对象
	bool	b_IsFirstCreated;
	CDataEntityManager m_DataEntityManager;	
	// 脏属性数据 在向客户端发送时重新获取最新数据
	set<string>  m_mapDirtyAttrData;
};
#endif//BASE_DB_ENTITY_H