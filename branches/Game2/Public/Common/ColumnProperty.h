/**ColumnProperty.h
** 列表属性类: 数据字段及表的数据管理操作
**Author: 吴畏
**Create Time:2010.03.23
*/

#ifndef COLUMN_PROPERTY_H
#define COLUMN_PROPERTY_H

class CGUID;
/// 列数据对象
class CDataObject
{
public:
	///构造函数
	CDataObject();
	///析构函数
	~CDataObject();

	///
    bool operator ==(const CDataObject& right)
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

	/// 清除数据接口
	void Release(void);

	/// 类接口
	/// 保存数据的相关信息// 数据区指针 / 保存4字节数据
	void*   GetBuf(void)	{ return m_Buf;	}		
	/// 数据区长度(BYTE)
	long	GetSize(void)	{ return m_Size;}		
	/// 数据区指针 / 保存4字节数据
	void	SetBuf(void* pVoid)	{ m_Buf	= pVoid;}	
	/// 数据区长度(BYTE)
	void	SetSize(long lVal)	{ m_Size = lVal;}		

	//	--------------------- 数据操作接口 ---------------------	//
	///
	bool DelValue();
	///
	long GetLongValue();
	///
	long GetBufValue(void* buf, long bufSize);
	///
	const char* GetStringValue();
	///
	const CGUID& GetGUIDValue();
	///
	bool SetLongValue(long& lVal);
	///
	bool SetBufValue(void* pVoid, long lSize);
	///
	bool SetStringValue(const char* value, long lSize);
	///
	bool SetGUIDValue(const CGUID& value);

	///
	CDataObject& operator=(CDataObject& right);

private:
	///
	bool SetValueArray	(void* pVoid, long lBufSize);
	///
	bool AddValueArray(void* pVoid, long lSize);

	// 保存数据的相关信息
	/// 数据区指针 / 保存4字节数据
	void*	m_Buf;		
	/// 数据区长度(BYTE), -1表示整数
	long	m_Size;		
};

/// 数据对象管理器
class CTableManager
{
public:
	///
	typedef std::map<long, CDataObject*> ColumnDataMap;
	///
	typedef ColumnDataMap::iterator ColumnDataMapItr;

	///
	CTableManager();
	///
	~CTableManager();
	///
	CTableManager& operator=(CTableManager& right);

	/// 清除数据
	void Release(void);

	/// 创建数据对象
	CDataObject* CreateColumnData(long lAttrEnum);

	/// 删除数据对象
	bool DelColumnyData(long lAttrEnum);

	/// 取得数据对象
	CDataObject* GetColumnData(long lAttrEnum);

	/// 取得数据对象值
	long	GetLongAttr(long lAttrEnum);
	///
	const char*	GetStringAttr(long lAttrEnum);
	///
	long	GetBufAttr(long lAttrEnum, void* buf, long bufSize);
	///
	long	GetGuidAttr(long lAttrEnum,  CGUID& guid);

	/// 设置数据对象值
	bool	SetLongAttr(long lAttrEnum, long value);
	///
	bool	SetStringAttr(long lAttrEnum, const char* value);
	///
	bool	SetBufAttr(long lAttrEnum, void* buf, long bufSize);
	///
	bool	SetGuidAttr(long lAttrEnum, const CGUID& guid);
	

	/// 创建数据对象值
	void	AddLongAttr(long lAttrEnum, long value);
	///
	void	AddStringAttr(long lAttrEnum, const char* value);
	///
	void	AddBufAttr(long lAttrEnum, void* buf, long bufSize);
	/// 
	void	AddTimeAttr(long lAttrEnum, long* buf, long bufSize);
	///
	void	AddGuidAttr(long lAttrEnum, const CGUID& guid);

	/// 删除数据对象值
    void    DelAttr(long lAttrEnum);

	///
	ColumnDataMap& GetColumnPropertyMap(void) { return m_mapColumnData; }

private:
	///
	ColumnDataMap m_mapColumnData;
};


#endif//COLUMN_PROPERTY_H