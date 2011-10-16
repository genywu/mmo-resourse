//EntityProperty.cpp/////////////////////////////////////////////////////////////////////
//动态属性类:
//Author:安海川
//Create Time:2008.11.03
#include "StdAfx.h"
#include "EntityProperty.h"


CWrapDataObject::CWrapDataObject()
: m_Buf(NULL),m_Size(0)
{

}
CWrapDataObject::~CWrapDataObject()
{
	if(m_Size != -1)
	{
		delete[] m_Buf;
		m_Buf = NULL;
	}
	m_Size = 0;	
}
CWrapDataObject& CWrapDataObject::operator=(CWrapDataObject& right)
{
	if(this != &right)
	{		
		if(m_Size != -1)
		{
			delete[] m_Buf;
			m_Buf = NULL;
			m_Size = 0;
		}

		if( right.m_Size == 0 )
		{
			m_Size = 0;
		}
		else if( right.m_Size > 0 )
		{
			if(right.m_Buf)
			{
				m_Buf = new uchar[right.m_Size];
				memcpy(m_Buf, right.m_Buf, right.m_Size);
				m_Size = right.m_Size;
			}
		}
		else
		{
			m_Buf = right.m_Buf;
			m_Size = -1;
		}
	}
	return *this;
}
//	ADD------------------------------------------------------------
bool CWrapDataObject::AddValueArray(void* buf, long size)
{
	if(m_Size != -1)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}

	if(size == 0)
	{
		m_Size = 1;
		m_Buf = new uchar[1];
		((uchar*)m_Buf)[0] = '\0';
		return true;
	}
	else if(size > 0)
	{
		m_Size = size;		
		m_Buf = new uchar[size];
		memcpy(m_Buf, buf, size);
	}
	return true;
}
bool CWrapDataObject::AddLongValue(long value)
{
	if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
	}

	*(long*)m_Buf = value;
	m_Size = 0;
	return true;
}
bool CWrapDataObject::AddStringValue(const char* value, long size)
{
	return AddValueArray((void*)value, size);
}
bool CWrapDataObject::AddBufValue(void* value, long size)
{
	return AddValueArray(value, size);
}
bool CWrapDataObject::AddGUIDValue(const CGUID& guid)
{
	return AddValueArray((void*)&guid, sizeof(CGUID));
}
//	DEL------------------------------------------------------------
bool CWrapDataObject::DelValue()
{
	if(m_Buf)
	{
		if(m_Size >= 0)
		{
			delete[] m_Buf;
			m_Buf = NULL;
			m_Size = 0;
		}
	}
	return true;
}
//	GET------------------------------------------------------------
long CWrapDataObject::GetValueArray(void** outBuf, long lBufSize)
{
	if(m_Buf && *outBuf && m_Size > 0)
	{
		if( lBufSize <= m_Size )
		{
			memcpy(*outBuf, m_Buf, lBufSize);
			return lBufSize;
		}
		else
		{
			memcpy(*outBuf, m_Buf, m_Size);
			return m_Size;
		}
	}
	return 0;
}
long CWrapDataObject::GetBufValue(void* buf, long lBufSize)
{
	if(m_Buf && buf && m_Size > 0)
	{
		if( lBufSize <= m_Size )
		{
			memcpy(buf, m_Buf, lBufSize);
			return lBufSize;
		}
		else
		{
			memcpy(buf, m_Buf, m_Size);
			return m_Size;
		}
	}
	return 0;
}
long CWrapDataObject::GetLongValue()
{
	long ret = 0;
	if(-1 == m_Size)
	{
		ret = (long)m_Buf;
	}
	return ret;
}
const char* CWrapDataObject::GetStringValue()
{
	if(m_Buf && m_Size >= 0)
		return (char*)m_Buf;
	return NULL;
}
const CGUID& CWrapDataObject::GetGUIDValue()
{
	if(m_Buf && sizeof(CGUID) <= m_Size)
		return *(CGUID*)m_Buf;
	return NULL_GUID;
}

//	SET------------------------------------------------------------
bool CWrapDataObject::SetValueArray(void* tbuf, long bufSize)
{
	if(bufSize == 0 || tbuf == 0) return false;

	if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}

	if(bufSize > 0)
	{
		m_Buf = (void*)new uchar[bufSize];
		m_Size = bufSize;
		memcpy(m_Buf, tbuf, bufSize);
	}
	return true;
}
bool CWrapDataObject::SetLongValue(long value)
{
	if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}
	m_Buf = (void*)value;
	m_Size = -1;
	return true;
}
bool CWrapDataObject::SetStringValue(const char* value, long size)
{
	if(size == 0)
		return SetValueArray("", 1);

	return SetValueArray((void*)value, size+1);
}
bool CWrapDataObject::SetBufValue(void* value, long size)
{
	return SetValueArray(value, size);
}
bool CWrapDataObject::SetGUIDValue(const CGUID& guid)
{
	return SetValueArray((void*)&guid, sizeof(CGUID));
}
// 清除数据
void CWrapDataObject::Release(void)
{
	// 保存数据的相关信息	
	if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
	}	
	m_Size = 0;		
}
//////////////////////////////////////////////////////////////////////////
CEntityProperty::CEntityProperty(long lAttrEnum)
:m_pDataObject(NULL),m_lAttrNumEnum(lAttrEnum),m_DbRelateType(DB_RELATE_NULL)
{

}
CEntityProperty::~CEntityProperty()
{
	SAFE_DELETE(m_pDataObject);
}

CEntityProperty& CEntityProperty::operator=(CEntityProperty& right)
{
	if(this != &right)
	{
		if(right.GetWrapDataObject())
		{
			SAFE_DELETE(m_pDataObject);
			m_pDataObject = new CWrapDataObject();
			*m_pDataObject = *(right.GetWrapDataObject());
		}

		m_DbRelateType = right.m_DbRelateType;
		m_lAttrNumEnum = right.m_lAttrNumEnum;
	}
	return *this;
}

int  CEntityProperty::GetLongAttr(  )
{
	int ret = 0;
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		ret = (int)wdo->GetLongValue();

	return ret;
}
void CEntityProperty::SetLongAttr( int p_val )
{
	CWrapDataObject* wdo = GetDataObject();
	if(!wdo)
		wdo = AddDataObject();
	if(wdo)
		wdo->SetLongValue(p_val);
}
bool CEntityProperty::HasLongAttr(  )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		return true;
	return false;
}
void CEntityProperty::DelLongAttr(  )
{
	if(HasLongAttr())
	{
		DelDataObject();
	}
}

const char* CEntityProperty::GetStringAttr()
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		return wdo->GetStringValue();
	return NULL;
}
void CEntityProperty::SetStringAttr( const std::string& p_name )
{
	CWrapDataObject* wdo = GetDataObject();
	if(!wdo)
		wdo = AddDataObject();
	if(wdo)
		wdo->SetStringValue(p_name.c_str(), (long)p_name.size());
}
bool CEntityProperty::HasStringAttr()
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		return true;
	return false;
}
void CEntityProperty::DelStringAttr(  )
{
	if(HasStringAttr())
	{
		DelDataObject();
	}
}
long CEntityProperty::GetBufSize(  )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
	{
		return wdo->GetSize();
	}

	return 0;
}
long CEntityProperty::GetBufAttr( void* buf, long bufSize )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
	{
		return wdo->GetBufValue(buf, bufSize);
	}

	return 0;
}
void CEntityProperty::SetBufAttr( void* buf, long bufSize )
{
	CWrapDataObject* wdo = GetDataObject();
	if(!wdo)
		wdo = AddDataObject();
	if(wdo)
		wdo->SetBufValue(buf, bufSize);
}
bool CEntityProperty::HasBufAttr(  )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		return true;
	return false;
}
void CEntityProperty::DelBufAttr(  )
{
	if(HasBufAttr())
	{
		DelDataObject();
	}
}

bool  CEntityProperty::GetGuidAttr( CGUID& guid )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
	{
		guid = wdo->GetGUIDValue();
		return true;
	}

	return false;
}
void CEntityProperty::SetGuidAttr( const CGUID& guid )
{
	CWrapDataObject* wdo = GetDataObject();
	if(!wdo)
		wdo = AddDataObject();
	if(wdo)
		wdo->SetGUIDValue(guid);
}
bool CEntityProperty::HasGuidAttr(  )
{
	CWrapDataObject* wdo = GetDataObject();
	if(wdo)
		return true;
	return false;
}
void CEntityProperty::DelGuidAttr(  )
{
	if(HasGuidAttr())
	{
		DelDataObject();
	}
}

// Add DataObject
CWrapDataObject* CEntityProperty::AddDataObject()
{
	if(m_pDataObject)
		return m_pDataObject;

	m_pDataObject = new CWrapDataObject();
	return m_pDataObject;
}

// Get DataObject
CWrapDataObject* CEntityProperty::GetDataObject()
{
	return m_pDataObject;
}

// Del DataObject
bool CEntityProperty::DelDataObject()
{
	SAFE_DELETE(m_pDataObject);
	return false;
}

// 清除数据
void CEntityProperty::Release(void)
{
	SAFE_DELETE(m_pDataObject);
}
//////////////////////////////////////////////////////////////////////////
CDataEntityManager::CDataEntityManager()
{
	m_mapEntityProperty.clear();
}
CDataEntityManager::~CDataEntityManager()
{
	Release();
}
CDataEntityManager& CDataEntityManager::operator=(CDataEntityManager& right)
{
	if(this != &right)
	{
		Release();
		EntityPropertyMapItr itr = right.m_mapEntityProperty.begin();
		for(; itr != right.m_mapEntityProperty.end(); itr++)
		{
			CEntityProperty* pEP = new CEntityProperty(itr->first);
			*pEP = *(itr->second);
			m_mapEntityProperty[itr->first] = pEP;
		}
	}
	return *this;
}
// 创建数据对象
CEntityProperty* CDataEntityManager::CreateEntityProperty(long lAttrEnum)
{
	CEntityProperty* ep = NULL;
	EntityPropertyMapItr itr = m_mapEntityProperty.find(lAttrEnum);
	if(itr == m_mapEntityProperty.end())
	{
		ep = new CEntityProperty(lAttrEnum);
		if(ep)
		{
			m_mapEntityProperty[lAttrEnum] = ep;
		}
	}
	else
		ep = itr->second;

	return ep;
}

// 删除数据对象
bool CDataEntityManager::DelEntityProperty(long lAttrEnum)
{
	EntityPropertyMapItr itr = m_mapEntityProperty.find(lAttrEnum);
	if(itr != m_mapEntityProperty.end())
	{
		delete itr->second;
		itr->second = NULL;
		m_mapEntityProperty.erase(itr);
		return true;
	}

	return false;
}

// 取得数据对象
CEntityProperty* CDataEntityManager::GetEntityProperty(long lAttrEnum)
{
	CEntityProperty* ep = NULL;
	EntityPropertyMapItr itr = m_mapEntityProperty.find(lAttrEnum);
	if(itr != m_mapEntityProperty.end())
	{
		ep = itr->second;
	}

	return ep;
}

// 取得数据对象值
long CDataEntityManager::GetLongAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
		return retEP->GetLongAttr();
	return 0;
}
const char* CDataEntityManager::GetStringAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
		return retEP->GetStringAttr();
	return NULL;
}
long CDataEntityManager::GetBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		return retEP->GetBufAttr(buf, bufSize);
	}
	return 0;
}
long CDataEntityManager::GetGuidAttr(long lAttrEnum, CGUID& guid)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->GetGuidAttr(guid);
		return 1;
	}
	return 0;
}
// 设置数据对象值
bool CDataEntityManager::SetLongAttr(long lAttrEnum, long value)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) return false;
	retEP->SetLongAttr(value);
	return true;
}
bool CDataEntityManager::SetStringAttr(long lAttrEnum, const char* value)
{
	if(NULL == value) return false;
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) return false;
	retEP->SetStringAttr(string(value));
	return true;
}
bool CDataEntityManager::SetBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) return false;
	retEP->SetBufAttr(buf, bufSize);
	return true;
}
bool CDataEntityManager::SetGuidAttr(long lAttrEnum, const CGUID& guid)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) return false;
	retEP->SetGuidAttr(guid);
	return true;
}
// 清除数据
void CDataEntityManager::Release(void)
{
	EntityPropertyMapItr itr = m_mapEntityProperty.begin();
	for(; itr != m_mapEntityProperty.end(); itr++)
	{
		delete itr->second;
	}
	m_mapEntityProperty.clear();
}
// 创建数据对象值
void CDataEntityManager::AddLongAttr(long lAttrEnum, long value)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) retEP = CreateEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->SetLongAttr(value);
		return;
	}
}
void CDataEntityManager::AddStringAttr(long lAttrEnum, const char* value)
{
	if(value == NULL) return;
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) retEP = CreateEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->SetStringAttr(string(value));
		return;
	}
}
void CDataEntityManager::AddBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	if(buf == NULL || bufSize == 0) return;
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) retEP = CreateEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->SetBufAttr(buf, bufSize);
		return;
	}
}
void CDataEntityManager::AddTimeAttr(long lAttrEnum, long* buf, long bufSize)
{
	if(buf == NULL || bufSize == 0) return;
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) retEP = CreateEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->SetBufAttr(buf, bufSize);
		return;
	}
}
void CDataEntityManager::AddGuidAttr(long lAttrEnum, const CGUID& guid)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(!retEP) retEP = CreateEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->SetGuidAttr(guid);
		return;
	}
}

// 删除数据对象值
void CDataEntityManager::DelLongAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->DelLongAttr();
	}
}
void CDataEntityManager::DelStringAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->DelStringAttr();
	}
}
void CDataEntityManager::DelBufAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->DelBufAttr();
	}
}
void CDataEntityManager::DelTimeAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->DelBufAttr();
	}
}
void CDataEntityManager::DelGuidAttr(long lAttrEnum)
{
	CEntityProperty* retEP = GetEntityProperty(lAttrEnum);
	if(retEP)
	{
		retEP->DelGuidAttr();
	}
}
// 定义临时属性
CTempEntityProperty::CTempEntityProperty(const string& strName, DATA_OBJECT_TYPE type)
:CEntityProperty(0),m_strVarName(strName),m_eDataType(type)
{
}
CTempEntityProperty::~CTempEntityProperty()
{
}
CTempEntityProperty& CTempEntityProperty::operator=(CTempEntityProperty& right)
{
	CEntityProperty::operator=(right);
	if(this != &right)
	{
		m_strVarName = right.m_strVarName;
		m_eDataType = right.m_eDataType;
	}
	return *this;
}