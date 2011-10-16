//EntityProperty.h/////////////////////////////////////////////////////////////////////
//动态属性类:
//Author:安海川
//Create Time:2008.11.03
#ifndef ENTITY_PROPERTY_H
#define ENTITY_PROPERTY_H

#include "../../DB/DBDefine.h"

#define MAX_DEFINED_ID		1024
#define INVILID_ENTITY_ID	-99999999
#define OPER_ALL_DATA 0xFFFF

// 封装C++基本数据对象
class CWrapDataObject
{
public:
	bool operator ==(const CWrapDataObject& right)
	{
		if(&right != this)
		{
			if( (right.m_Buf == m_Buf) && (right.m_Size == m_Size) )
				return true;
			else
				return false;
		}

		return false;
	}

	CWrapDataObject();
	~CWrapDataObject();

	// 清除数据接口
	void Release(void);

	// 类接口
	// 保存数据的相关信息
	void*				GetBuf		(void)	{ return m_Buf;			}		// 数据区指针 / 保存4字节数据
	long				GetSize		(void)	{ return m_Size;		}		// 数据区长度(BYTE)

	void	SetBuf		(void*				value)	{ m_Buf			= value;}		// 数据区指针 / 保存4字节数据
	void	SetSize		(long				value)	{ m_Size		= value;}		// 数据区长度(BYTE)

	//	--------------------- 数据操作接口 ---------------------	//
	//	ADD------------------------------------------------------------
	bool AddLongValue(long value);
	bool AddBufValue(void* buf, long size);
	bool AddStringValue(const char* value, long size);
	bool AddGUIDValue(const CGUID& value);

	//	DEL------------------------------------------------------------
	bool DelValue();

	//	GET------------------------------------------------------------
	long GetLongValue();
	long GetBufValue(void* buf, long bufSize);
	const char* GetStringValue();
	const CGUID& GetGUIDValue();

	//	SET------------------------------------------------------------
	bool SetLongValue	(long value);
	bool SetBufValue	(void* value, long size);
	bool SetStringValue	(const char* value, long size);
	bool SetGUIDValue	(const CGUID& value);

	CWrapDataObject& operator=(CWrapDataObject& right);

private:
	bool SetValueArray	(void* tbuf, long bufSize);
	// char类型
	bool AddValueArray(void* buf, long size);
	// time类型
	bool AddValueArray(long* buf, long size);
	long GetValueArray(void** buf, long bufSize);

	// 保存数据的相关信息
	//DATA_OBJECT_TYPE	m_DataType;	// 数据类型
	void*				m_Buf;		// 数据区指针 / 保存4字节数据
	long				m_Size;		// 数据区长度(BYTE), -1表示整数
};

// 数据对象(数据表)
class CEntityProperty
{
public:

	//CEntityProperty(const std::string& name=std::string("EP"), DATA_OBJECT_TYPE type=DATA_OBJECT_UNKNOWN);
	CEntityProperty(long lAttrEnum);
	~CEntityProperty();

	// 清除数据接口
	void Release(void);

	// 类接口
	//void SetEPName(const std::string& name) { m_EPName = name; }
	//const std::string& GetEPName(void) { return m_EPName; }

	long	GetAttrNumEnum(void) { return m_lAttrNumEnum; }
	void	SetAttrNumEnum(long value) { m_lAttrNumEnum = value; }

	// push DataObject
	CWrapDataObject* AddDataObject();

	// Get DataObject
	CWrapDataObject* GetDataObject();

	// Del DataObject
	bool DelDataObject();

	// 
	void CopyWrapDataObject(CWrapDataObject* desVec)
	{
		if(desVec)
			*desVec = *m_pDataObject;
	}

	CWrapDataObject* GetWrapDataObject(void)
	{
		return m_pDataObject;
	}

	//void SetAttrType( , DATA_OBJECT_TYPE type);

	int  GetLongAttr(  );
	void SetLongAttr( int p_val );
	bool HasLongAttr(  );
	void DelLongAttr(  );

	const char*  GetStringAttr	(  );
	void SetStringAttr			( const std::string& p_name );
	bool HasStringAttr			(  );
	void DelStringAttr			(  );

	long GetBufSize(  );
	long GetBufAttr( void* buf, long bufSize );
	void SetBufAttr( void* buf, long bufSize );
	bool HasBufAttr(  );
	void DelBufAttr(  );

	bool GetGuidAttr( CGUID& guid );
	void SetGuidAttr( const CGUID& guid );
	bool HasGuidAttr( );
	void DelGuidAttr( );

	CEntityProperty& operator=(CEntityProperty& right);

	BYTE	GetDbRelateType(void)					{ return m_DbRelateType; }
	void	SetDbRelateType(DB_RELATE_TYPE type)	{ m_DbRelateType = type; }

protected:
	BYTE	m_DbRelateType;
	long    m_lAttrNumEnum;// 属性名枚举值
	CWrapDataObject* m_pDataObject;
};


// 数据对象管理器
class CDataEntityManager
{
public:
	typedef std::map<long, CEntityProperty*> EntityPropertyMap;
	typedef EntityPropertyMap::iterator EntityPropertyMapItr;

	CDataEntityManager();
	~CDataEntityManager();

	CDataEntityManager& operator=(CDataEntityManager& right);

	// 清除数据
	void Release(void);

	// 创建数据对象
	CEntityProperty* CreateEntityProperty(long lAttrEnum);

	// 删除数据对象
	bool DelEntityProperty(long lAttrEnum);

	// 取得数据对象
	CEntityProperty* GetEntityProperty(long lAttrEnum);

	// 取得数据对象值
	long			GetLongAttr(long lAttrEnum);
	const char*		GetStringAttr(long lAttrEnum);
	long			GetBufAttr(long lAttrEnum, void* buf, long bufSize);
	long			GetGuidAttr(long lAttrEnum,  CGUID& guid);

	// 设置数据对象值
	bool			SetLongAttr(long lAttrEnum, long value);
	bool			SetStringAttr(long lAttrEnum, const char* value);
	bool			SetBufAttr(long lAttrEnum, void* buf, long bufSize);
	bool			SetGuidAttr(long lAttrEnum, const CGUID& guid);

	// 创建数据对象值
	void			AddLongAttr		(long lAttrEnum, long value);
	void			AddStringAttr	(long lAttrEnum, const char* value);
	void			AddBufAttr		(long lAttrEnum, void* buf, long bufSize);
	void			AddTimeAttr		(long lAttrEnum, long* buf, long bufSize);
	void			AddGuidAttr		(long lAttrEnum, const CGUID& guid);

	// 删除数据对象值
	void			DelLongAttr		(long lAttrEnum);
	void			DelStringAttr	(long lAttrEnum);
	void			DelBufAttr		(long lAttrEnum);
	void			DelTimeAttr		(long lAttrEnum);
	void			DelGuidAttr		(long lAttrEnum);

	EntityPropertyMap& GetEntityPropertyMap(void) { return m_mapEntityProperty; }

private:
	EntityPropertyMap m_mapEntityProperty;
};


// 定义临时属性
class CTempEntityProperty : public CEntityProperty
{
public:
	CTempEntityProperty(const string& strName, DATA_OBJECT_TYPE type);
	~CTempEntityProperty();

	const string& GetVarName(void) { return m_strVarName; }
	DATA_OBJECT_TYPE GetDataType(void) { return m_eDataType; }

	CTempEntityProperty& operator=(CTempEntityProperty& right);
private:
	string m_strVarName;
	DATA_OBJECT_TYPE m_eDataType;
};

#endif//ENTITY_PROPERTY_H