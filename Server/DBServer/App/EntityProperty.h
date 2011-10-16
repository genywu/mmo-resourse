#ifndef ENTITY_PROPERTY_H
#define ENTITY_PROPERTY_H
#include <map>
#include <string>
#include <list>
#include "DBEntityDef.h"

#define MAX_DEFINED_ID		1024
#define INVILID_ENTITY_ID	-99999999
#define OPER_ALL_DATA 0xFFFF
typedef unsigned long ulong;
typedef unsigned short ushort;
/*
|----
|OPEntityFlag:  1byte, 操作实体数量的标志 0: 单个, 1:多个
|DBOP Type:     1byte, 数据库操作类型（枚举值）
|MSG ID:        4byte, 该消息的MSGID
|Entity Num:	2byte, 该实体个数(X)
---------------从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码----------------
|	|----
|   |EntityID:		4byte, 实体ID
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|	 |该属性个数:		2byte, 属性数据个数(M) 全部： 0xFFFF OPER_ALL_DATA
|X*	|N * |该属性类型:		4byte, 属性类型枚举值
|	|	 |该属性ID:			4byte, 属性ID, 
|	|	 |该属性名字:		nbyte, 属性名字,字符串
|	|	 |		|----
|	|	 |		|该属性POS:		2byte, 属性POS, 
|	|	 |		|该属性size:	2byte, 属性SIZE, 
|	|	 |	M *	|该属性数据块:	nbtye, 该数据块长度由属性数据长度决定 (若消息为LOAD则该数据块为1byte，值为0)
|	|	 |		|----
|	|	 |----
|	|----
*/
enum DATA_OBJECT_TYPE
{
	DATA_OBJECT_UNKNOWN = 0,
	DATA_OBJECT_FLOAT,
	DATA_OBJECT_LONG,
	DATA_OBJECT_SHORT,
	DATA_OBJECT_ULONG,
	DATA_OBJECT_USHORT,
	DATA_OBJECT_BOOL,
	DATA_OBJECT_CHAR,
	DATA_OBJECT_BYTE,
	DATA_OBJECT_WORD,
	DATA_OBJECT_DWORD,
	DATA_OBJECT_STRING,

	DATA_OBJECT_FLOAT_ARRAY,
	DATA_OBJECT_LONG_ARRAY,
	DATA_OBJECT_SHORT_ARRAY,
	DATA_OBJECT_ULONG_ARRAY,
	DATA_OBJECT_USHORT_ARRAY,
	DATA_OBJECT_BOOL_ARRAY,
	DATA_OBJECT_CHAR_ARRAY,
	DATA_OBJECT_BYTE_ARRAY,
	DATA_OBJECT_WORD_ARRAY,
	DATA_OBJECT_DWORD_ARRAY,

	DATA_OBJECT_STRING_ARRAY, // 如:  {"124", "2235",...}
	DATA_OBJECT_BUFFER_ARRAY, // 如:  {tagTime[6]}
};

// 封装C++基本数据对象
class CWrapDataObject
{
public:
	// 保存数据的相关信息
	typedef struct tagValueInfo
	{
		long				pos;		// 其所在位置,如list,vector数据结构中的pos
		DATA_OBJECT_TYPE	dataType;	// 数据类型
		void*				buf;		// 数据区指针
		long				size;		// 数据区长度(BYTE)

		tagValueInfo()
		{

		}
		~tagValueInfo()
		{

		}

		bool operator ==(const tagValueInfo& right)
		{
			if( (&right == this)
				|| (right.pos == pos && right.dataType == dataType
				&& right.buf == buf && right.size == size) )
				return true;

			return false;
		}
	}ValueInfo;

	typedef std::map<long, ValueInfo*> ValueInfoMap;
	typedef ValueInfoMap::iterator ValueInfoMapItr;

	CWrapDataObject();
	CWrapDataObject(DATA_OBJECT_TYPE type, long id=INVILID_ENTITY_ID, std::string& name=string("object"));
	~CWrapDataObject();

	

	// 类接口
	void SetID(long id) { m_ID = id; }
	long GetID(void)	{ return m_ID; }

	void SetDataType(DATA_OBJECT_TYPE type) { m_DataType = type; }
	DATA_OBJECT_TYPE GetDataType(void)		{ return m_DataType; }

	void SetName(const std::string name)	{ m_Name = name; }
	const std::string& GetName(void)		{ return m_Name; }

	//	--------------------- 数据操作接口 ---------------------	//
	//	ADD------------------------------------------------------------
	bool AddValueArray(long pos, DATA_OBJECT_TYPE dataType, void* buf, long size);

	//	DEL------------------------------------------------------------
	bool DelValueArray(long pos);

	//	GET------------------------------------------------------------
	ValueInfo* GetValueArray(long pos);

	// FIND------------------------------------------------------------
	long GetValueArray(ValueInfo& valueInfo);

	// 标志操作
	// 从队列后加入
	void PushBackOperValuePosFlag(DB_OPERATION_TYPE op, WORD pos);
	// 从队列前弹出
	WORD PopFrontOperValuePosFlag(DB_OPERATION_TYPE op);
	// 清空队列
	void ClearOperValuePosFlag(DB_OPERATION_TYPE op);
	// num
	long GetOperValuePosFlagNum(DB_OPERATION_TYPE op);

private:
	std::map<long, tagValueInfo*> m_mapDataValue;

	long				m_ID;		// 在该ObjectValue数据结构中的位置
	std::string			m_Name;		// 名字
	DATA_OBJECT_TYPE	m_DataType; // 数据类型

	// 记录某位置属性被操作的状态
	std::list<WORD> m_LoadValuePosFlag;
	std::list<WORD> m_SaveValuePosFlag;
};

// 数据对象(数据表)
class CEntityProperty
{
public:
	typedef std::map<std::string, CWrapDataObject*> WrapDataObjectMap;
	typedef WrapDataObjectMap::iterator WrapDataObjectMapItr;

	CEntityProperty(long id=INVILID_ENTITY_ID, std::string& name=string("EP"));
	~CEntityProperty();

	// 类接口
	void SetID(long id) { m_ID = id; }
	long GetID(void)	{ return m_ID; }

	void GetName(const std::string& name) { m_Name = name; }
	const std::string& GetName(void) { return m_Name; }

	// Add DataObject
	bool AddDataObject(CWrapDataObject* dataObj);
	// Add DataObject
	bool AddDataObject(DATA_OBJECT_TYPE type, std::string& objName);

	// Get DataObject
	CWrapDataObject* GetDataObject(std::string& name);

	// Del DataObject
	bool DelDataObject(std::string& name);

	// 分配一个ID
	long AllocDataObjectID();
	// 释放一个ID
	void FreeDataObjectID(long id);

	// 标志操作
	// 从队列后加入
	void PushBackOperDataObjFlag(DB_OPERATION_TYPE op, string& name);
	// 从队列前弹出
	string PopFrontOperDataObjFlag(DB_OPERATION_TYPE op);
	// num
	long GetOperDataObjFlagNum(DB_OPERATION_TYPE op);

private:
	std::map<std::string, CWrapDataObject*> m_mapDataObject;

	// ID
	long		m_ID;
	// 数据库表名根据此name:  [name]_Table 
	std::string m_Name;

	// 当前DataObjectID
	long m_CurDOID;
	// 空闲DataObjectID
	std::list<long> m_FreeDOID;

	// 记录某位置属性被操作的状态
	std::list<string> m_LoadDataObjFlag;
	std::list<string> m_SaveDataObjFlag;
};


// 数据对象管理器
class CDataEntityManager
{
public:
	typedef std::map<std::string, CEntityProperty*> EntityPropertyMap;
	typedef EntityPropertyMap::iterator EntityPropertyMapItr;

	CDataEntityManager();
	~CDataEntityManager();

	// 创建数据对象
	CEntityProperty* CreateEntityProperty(std::string& name);

	// 删除数据对象
	bool DelEntityProperty(std::string& name);

	// 取得数据对象
	CEntityProperty* GetEntityProperty(std::string& name);

	// 处理消息
	void ProcessMsg(CMessage* msg);
	// 处理消息
	void ProcessMsg(CEntityProperty* ep, DB_OPERATION_TYPE op, long msgid, BYTE* msgBuf, long msgBufPos, vector<BYTE>& pBA);

	// 添加消息
	void AddMsg(CMessage* msg);
	// 添加消息
	void AddMsg(CEntityProperty* ep, DB_OPERATION_TYPE op, long msgid, CWrapDataObject* wdo, vector<BYTE>& pBA);

	// 分配一个ID
	long AllocEntityPropertyID();
	// 释放一个ID
	void FreeEntityPropertyID(long id);

private:
	std::map<std::string, CEntityProperty*> m_mapEntityProperty;

	// 当前EntityPropertyID
	long m_CurEPID;
	// 空闲EntityPropertyID
	std::list<long> m_FreeEPID;
};
#endif//ENTITY_PROPERTY_H