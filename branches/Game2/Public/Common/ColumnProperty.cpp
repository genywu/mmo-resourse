/**ColumnProperty.cpp
**列表属性类:
**Author:吴畏
**Create Time:2010.03.23
*/
#include "StdAfx.h"
#include "ColumnProperty.h"


CDataObject::CDataObject()
: m_Buf(NULL),m_Size(0)
{

}
CDataObject::~CDataObject()
{
	if(m_Size != -1)
	{
		delete[] m_Buf;
		m_Buf = NULL;
	}
	m_Size = 0;	
}
CDataObject& CDataObject::operator=(CDataObject& right)
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
bool CDataObject::AddValueArray(void* pVoid, long lSize)
{
	if(m_Size != -1)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}

	if(lSize == 0)
	{
		m_Size = 1;
		m_Buf = new uchar[1];
		((uchar*)m_Buf)[0] = '\0';
		return true;
	}
	else if(lSize > 0)
	{
		m_Size = lSize;		
		m_Buf = new uchar[lSize];
		memcpy(m_Buf, pVoid, lSize);
	}
	return true;
}

//	DEL------------------------------------------------------------
bool CDataObject::DelValue()
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

long CDataObject::GetBufValue(void* buf, long lBufSize)
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
long CDataObject::GetLongValue()
{
	long ret = 0;
	if(m_Buf && m_Size >= sizeof(long))
	{
		ret = *(long*)m_Buf;
	}
	return ret;
}
const char* CDataObject::GetStringValue()
{
	if(m_Buf && m_Size >= 0)
		return (char*)m_Buf;
	return NULL;
}
const CGUID& CDataObject::GetGUIDValue()
{
	if(m_Buf && sizeof(CGUID) <= m_Size)
		return *(CGUID*)m_Buf;
	return NULL_GUID;
}

//	SET------------------------------------------------------------
bool CDataObject::SetValueArray(void* pVoid, long lBufSize)
{
	if(lBufSize == 0 || pVoid == 0) return false;

	if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}

	if(lBufSize > 0)
	{
		m_Buf = (void*)new uchar[lBufSize];
		m_Size = lBufSize;
		memcpy(m_Buf, pVoid, lBufSize);
	}
	return true;
}
bool CDataObject::SetLongValue(long& lVal)
{
	/*if(m_Size >= 0)
	{
		delete[] m_Buf;
		m_Buf = NULL;
		m_Size = 0;
	}
	m_Buf = (void*)&lVal;
	m_Size = -1;
	return true;*/
    return SetValueArray((void*)&lVal, sizeof(long));
}

bool CDataObject::SetStringValue(const char* value, long lSize)
{
	if(lSize == 0)
		return SetValueArray("", 1);

	return SetValueArray((void*)value, lSize+1);
}

bool CDataObject::SetBufValue(void* pVoid, long lSize)
{
	return SetValueArray(pVoid, lSize);
}

bool CDataObject::SetGUIDValue(const CGUID& guid)
{
	return SetValueArray((void*)&guid, sizeof(CGUID));
}

// 清除数据
void CDataObject::Release(void)
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

CTableManager::CTableManager()
{
	m_mapColumnData.clear();
}
CTableManager::~CTableManager()
{
	Release();
}
CTableManager& CTableManager::operator=(CTableManager& right)
{
	if(this != &right)
	{
		Release();
		ColumnDataMapItr itr = right.m_mapColumnData.begin();
		for(; itr != right.m_mapColumnData.end(); itr++)
		{
			CDataObject* pEP = new CDataObject();
			*pEP = *(itr->second);
			m_mapColumnData[itr->first] = pEP;
		}
	}
	return *this;
}
// 创建数据对象
CDataObject* CTableManager::CreateColumnData(long lAttrEnum)
{
	CDataObject* ep = NULL;
	ColumnDataMapItr itr = m_mapColumnData.find(lAttrEnum);
	if(itr == m_mapColumnData.end())
	{
		ep = new CDataObject();
		if(ep)
		{
			m_mapColumnData[lAttrEnum] = ep;
		}
	}
	else
		ep = itr->second;

	return ep;
}

// 删除数据对象
bool CTableManager::DelColumnyData(long lAttrEnum)
{
	ColumnDataMapItr itr = m_mapColumnData.find(lAttrEnum);
	if(itr != m_mapColumnData.end())
	{
		delete itr->second;
		itr->second = NULL;
		m_mapColumnData.erase(itr);
		return true;
	}

	return false;
}

// 取得数据对象
CDataObject* CTableManager::GetColumnData(long lAttrEnum)
{
	CDataObject* ep = NULL;
	ColumnDataMapItr itr = m_mapColumnData.find(lAttrEnum);
	if(itr != m_mapColumnData.end())
	{
		ep = itr->second;
	}

	return ep;
}

// 取得数据对象值
long CTableManager::GetLongAttr(long lAttrEnum)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(pData)
    {
        return pData->GetLongValue();
    }
	return 0;
}
const char* CTableManager::GetStringAttr(long lAttrEnum)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(pData)
		return pData->GetStringValue();
	return NULL;
}
long CTableManager::GetBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(pData)
	{
        return pData->GetBufValue(buf, bufSize);
	}
	return 0;
}
long CTableManager::GetGuidAttr(long lAttrEnum, CGUID& guid)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(pData)
	{
        guid = pData->GetGUIDValue();
		return 1;
	}
	return 0;
}
// 设置数据对象值
bool CTableManager::SetLongAttr(long lAttrEnum, long value)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(!pData) 
        return false;
    pData->SetLongValue(value);
	return true;
}
bool CTableManager::SetStringAttr(long lAttrEnum, const char* value)
{
	if(NULL == value) return false;
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(!pData) 
        return false;

    pData->SetStringValue(value, sizeof(value));
	return true;
}
bool CTableManager::SetBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(!pData) 
        return false;

    pData->SetBufValue(buf, bufSize);
	return true;
}
bool CTableManager::SetGuidAttr(long lAttrEnum, const CGUID& guid)
{
	CDataObject *pData = GetColumnData(lAttrEnum);
	if(!pData) 
        return false;

    pData->SetGUIDValue(guid);
	return true;
}
// 清除数据
void CTableManager::Release(void)
{
	ColumnDataMapItr itr = m_mapColumnData.begin();
	for(; itr != m_mapColumnData.end(); itr++)
	{
		delete itr->second;
	}
	m_mapColumnData.clear();
}
// 创建数据对象值
void CTableManager::AddLongAttr(long lAttrEnum, long value)
{
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(!pData) pData = CreateColumnData(lAttrEnum);
	if(pData)
	{
		pData->SetLongValue(value);
		return;
	}
}
void CTableManager::AddStringAttr(long lAttrEnum, const char* value)
{
	if(value == NULL) return;
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(!pData) pData = CreateColumnData(lAttrEnum);
	if(pData)
	{
        pData->SetStringValue(value, sizeof(value));
		return;
	}
}
void CTableManager::AddBufAttr(long lAttrEnum, void* buf, long bufSize)
{
	if(buf == NULL || bufSize == 0) return;
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(!pData) pData = CreateColumnData(lAttrEnum);
	if(pData)
	{
        pData->SetBufValue(buf, bufSize);
		return;
	}
}
void CTableManager::AddTimeAttr(long lAttrEnum, long* buf, long bufSize)
{
	if(buf == NULL || bufSize == 0) return;
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(!pData) pData = CreateColumnData(lAttrEnum);
	if(pData)
	{
        pData->SetBufValue(buf, bufSize);
		return;
	}
}
void CTableManager::AddGuidAttr(long lAttrEnum, const CGUID& guid)
{
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(!pData) pData = CreateColumnData(lAttrEnum);
	if(pData)
	{
		pData->SetGUIDValue(guid);
		return;
	}
}

// 删除数据对象值
void CTableManager::DelAttr(long lAttrEnum)
{
	CDataObject* pData = GetColumnData(lAttrEnum);
	if(pData)
	{
		pData->DelValue();
	}
}
