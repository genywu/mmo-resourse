//BaseEntity.h/////////////////////////////////////////////////////////////////////
//基本对象类:所有对象从其继承
//Author:安海川
//Create Time:2008.11.03
#ifndef BASEENTITY_H
#define BASEENTITY_H
#include <list>
#include <vector>
#include <string>
#include <set>
#include "../public/msgtype.h"
#include "../public/GUID.h"
#include "../public/dbdefine.h"
#include "../public/entityproperty.h"


// 游戏物体基类，所有对象和游戏相关的类由该类派生
class CBaseEntity
{
public:
	CBaseEntity();
	CBaseEntity(const CGUID& guid);

	virtual ~CBaseEntity();

	const string& GetCompositeFlag(void) { return m_CompositeFlag; }

	virtual void GetFromByteArray(BYTE* msgBuf, long& msgBufPos, long bufSize=-1) = 0;
	virtual void AddToByteArray(std::vector<BYTE>& pBA, bool isLoadOperFlag) = 0;

	// GetID
	const CGUID& GetGUID(void) { return m_GUID; }
	// SetID
	void SetGUID(const CGUID& id) { m_GUID = id; }

	// 初始化属性配置
	virtual void InitProperty() {};

	CDataEntityManager& GetDataEntityManager(void) { return m_DataEntityManager; }

	// 设置数据对象值
	void			SetLongAttr		(const std::string& name, long value);
	void			SetStringAttr	(const std::string& name, const char* value);
	void			SetBufAttr		(const std::string& name, void* buf, long bufSize);
	void			SetTimeAttr		(const std::string& name, long* buf, long bufSize);
	void			SetGuidAttr		(const std::string& name, const CGUID& guid);

	// 设置数据对象值
	long			GetLongAttr		(const std::string& name);
	const char*		GetStringAttr	(const std::string& name);
	long			GetBufAttr		(const std::string& name,void* buf, long bufSize);
	void			GetTimeAttr		(const std::string& name,long* buf, long bufSize);
	void			GetGuidAttr		(const std::string& name,CGUID& guid);

	// 创建数据对象值
	void			AddLongAttr		(const std::string& name,DATA_OBJECT_TYPE type, long value);
	void			AddStringAttr	(const std::string& name,const char* value);
	void			AddBufAttr		(const std::string& name,void* buf, long bufSize);
	void			AddTimeAttr		(const std::string& name,long* buf, long bufSize);
	void			AddGuidAttr		(const std::string& name,const CGUID& guid);

	// 删除数据对象值
	void			DelLongAttr		(const std::string& name);
	void			DelStringAttr	(const std::string& name);
	void			DelBufAttr		(const std::string& name);
	void			DelTimeAttr		(const std::string& name);
	void			DelGuidAttr		(const std::string& name);

	virtual CBaseEntity& operator=(CBaseEntity& right);

	void SetCompositeFlag(const string& type)	{ m_CompositeFlag = type; }

	void SetCompositeType(COMPOSITE_TYPE type)	{ m_CompositeType = type; }
	COMPOSITE_TYPE GetCompositeType(void)		{ return (COMPOSITE_TYPE)m_CompositeType; }

	void SetHasAttrFlag(BYTE flag)				{ m_bHasAttrs = flag; }
	BYTE GetHasAttrFlag(void)					{ return m_bHasAttrs;}

	BYTE GetCurDbOperFlag(void)			{ return m_bCurDbOperFlag; }
	void SetCurDbOperFlag(BYTE flag)	{ m_bCurDbOperFlag = flag; }
	BYTE GetCurDbOperType(void)			{ return m_bCurDbOperType; }
	void SetCurDbOperType(BYTE flag)	{ m_bCurDbOperType = flag; }
	
	// 是否临时对象，Load操作时判断为1则不能直接内存返回WS消息
	BYTE		GetIsTempEntityFlag(void)		{ return m_bIsTempEntityFlag; }
	void		SetIsTempEntityFlag(BYTE flag)	{ m_bIsTempEntityFlag = flag; }

protected:
	BYTE m_bCurDbOperFlag; // 是否执行当前数据库操作标志
	BYTE m_bCurDbOperType; // 当前数据库操作类型
	BYTE m_bHasAttrs;// 该对象是否有数据保存在数据库
	BYTE m_bIsTempEntityFlag;// 是否临时对象，Load操作时判断为1则不能直接内存返回WS消息
	BYTE m_CompositeType;
	CGUID m_GUID;
	string m_CompositeFlag;
	CDataEntityManager m_DataEntityManager;	
};



#endif//DB_BASEENTITY_H
