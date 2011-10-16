#include "StdAfx.h"
#include "BaseDBEntity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 设置数据对象值
long CBaseDBEntity::GetLongAttr(const std::string& name, long pos)
{
	return m_DataEntityManager.GetLongAttr(name, pos);
}
const char* CBaseDBEntity::GetStringAttr(const std::string& name, long pos)
{
	return m_DataEntityManager.GetStringAttr(name, pos);
}
void CBaseDBEntity::GetBufAttr(const std::string& name, long pos, void** buf, long bufSize)
{
	m_DataEntityManager.GetBufAttr(name, pos, buf, bufSize);
}
void CBaseDBEntity::GetTimeAttr(const std::string& name, long pos, long* buf, long bufSize)
{
	m_DataEntityManager.GetBufAttr(name, pos, (void*)buf, bufSize);
}
void CBaseDBEntity::GetGuidAttr(const std::string& name, long pos, CGUID& guid)
{
	m_DataEntityManager.GetGuidAttr(name, pos, guid);
}


// 设置数据对象值
void CBaseDBEntity::SetLongAttr(const std::string& name, const std::string& tableName, long pos, long value)
{
	if(m_DataEntityManager.SetLongAttr(name, tableName, pos, value))
		m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::SetStringAttr(const std::string& name, const std::string& tableName, long pos, const char* value)
{
	if(m_DataEntityManager.SetStringAttr(name, tableName, pos, value))
		m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::SetBufAttr(const std::string& name, const std::string& tableName, long pos, void* buf, long bufSize)
{
	if(m_DataEntityManager.SetBufAttr(name, tableName, pos, buf, bufSize))
		m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::SetTimeAttr(const std::string& name, const std::string& tableName, long pos, long* buf, long bufSize)
{
	if(m_DataEntityManager.SetBufAttr(name, tableName, pos, (void*)&buf[0], bufSize))
		m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::SetGuidAttr(const std::string& name, const std::string& tableName, long pos, const CGUID& guid)
{
	if(m_DataEntityManager.SetGuidAttr(name, tableName, pos, guid))
		m_mapDirtyAttrData.insert(name);
}
// 创建数据对象值
void CBaseDBEntity::AddLongAttr		(const std::string& name, const std::string& tableName, long pos, DATA_OBJECT_TYPE type, long value)
{
	m_DataEntityManager.AddLongAttr(name, tableName, pos, type, value);
	m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::AddStringAttr	(const std::string& name, const std::string& tableName, long pos, const char* value)
{
	m_DataEntityManager.AddStringAttr(name, tableName, pos, value);
	m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::AddBufAttr		(const std::string& name, const std::string& tableName, long pos, void* buf, long bufSize)
{
	m_DataEntityManager.AddBufAttr(name, tableName, pos, buf, bufSize);
	m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::AddTimeAttr		(const std::string& name, const std::string& tableName, long pos, long* buf, long bufSize)
{
	m_DataEntityManager.AddTimeAttr(name, tableName, pos, buf, bufSize);
	m_mapDirtyAttrData.insert(name);
}
void CBaseDBEntity::AddGuidAttr		(const std::string& name, const std::string& tableName, long pos, const CGUID& guid)
{
	m_DataEntityManager.AddGuidAttr(name, tableName, pos, guid);
	m_mapDirtyAttrData.insert(name);
}

// 删除数据对象值
void CBaseDBEntity::DelLongAttr		(const std::string& name,const std::string& tableName, long pos)
{
	m_DataEntityManager.DelLongAttr(name, tableName, pos);
}
void CBaseDBEntity::DelStringAttr	(const std::string& name,const std::string& tableName, long pos)
{
	m_DataEntityManager.DelStringAttr(name, tableName, pos);
}
void CBaseDBEntity::DelBufAttr		(const std::string& name,const std::string& tableName, long pos)
{
	m_DataEntityManager.DelBufAttr(name, tableName, pos);
}
void CBaseDBEntity::DelTimeAttr		(const std::string& name,const std::string& tableName, long pos)
{
	m_DataEntityManager.DelTimeAttr(name, tableName, pos);
}
void CBaseDBEntity::DelGuidAttr		(const std::string& name,const std::string& tableName, long pos)
{
	m_DataEntityManager.DelGuidAttr(name, tableName, pos);
}
