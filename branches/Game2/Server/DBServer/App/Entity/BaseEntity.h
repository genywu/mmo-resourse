/**
*File:  BaseEntity.h
*brief: 基本对象类:所有对象从其继承
*Author:安海川
*Date:  2008.11.03
*/
#ifndef BASEENTITY_H
#define BASEENTITY_H

#include "../../../../Public/ServerPublic/Server/Common/EntityProperty.h"

/*
消息设计(每条消息只更新一个节点以及该节点的所有叶子节点数据)：
|----接收消息--------------------------------------
|SessionID:		17byte, 需要返回的SessionID
|rootComFlag:	4byte, 根节点flag
|rootGuid:		17byte,根节点guid
|rootName:		32byte,该对象所有者名字
|findKey:		1byte, 根结点查找方式 0:guid,1:Set String,2:Find String
--该节点的属性值----从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码-----------
|	|QueryAttrNum:	2byte, 查询字段个数(N)
|	|	 |----
|	|N * |szName;   nbyte,  查询字段名字符串
|	|	 |varType:  4byte,  查询字段类型
|	|	 |varLen:	4byte,  查询字段数据长度
|	|	 |varBuf:   nbyte,  查询字段数据
|	|	 |QueryType:1byte,  查询字段连接类型(AND,OR…)
|X*	|	 |----
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|	 |nameLen:  4byte, 变量名字符串长度
|	|N * |szName;   nbyte, 变量名字符串
|	|	 |varType:  4byte, 变量类型
|	|	 |varLen:	4byte, 变量数据长度
|	|	 |varBuf:   nbyte, 变量数据
|	|	 |----
|childNum:		4byte, 子节点个数(X)
|	|----
|   |compositeTyoe: 1byte, 该子节点Type
|   |compositeFlag: 4byte, 该子节点flag
|   |EntityID:		17byte,该子节点guid
|	|QueryAttrNum:	2byte, 查询字段个数(N)
|	|	 |----
|	|N * |szName;   nbyte,  查询字段名字符串
|	|	 |varType:  4byte,  查询字段类型
|	|	 |relate:   1byte,  与下一个查询字段的关系(AND OR)
|	|	 |----
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|	 |nameLen:  4byte, 变量名字符串长度
|X*	|N * |szName;   nbyte, 变量名字符串
|	|	 |varType:  4byte, 变量类型
|	|	 |varLen:	4byte, 变量数据长度
|	|	 |varBuf:   nbyte, 变量数据
|	|	 |----
|----接收消息--------------------------------------

|----发送消息--------------------------------------
|SessionID:		17byte, 需要返回的SessionID
|retFlag:		1byte, 需要返回的标志位
|rootComFlag:	4byte, 根节点flag
|rootGuid:		17byte,根节点guid
|rootName:		32byte,该对象所有者名字
|findKey:		1byte, 根结点查找方式 0:guid,1:Set String,2:Find String
--该节点的属性值----从此处开始使用 _AddToByteArray和_GetXXXXByArray 接口编解码-----------
|	|comType:		1byte, comtype
|	|comFlag:		4byte, comFlag
|	|DBOP Type:     1byte, 数据库操作类型（枚举值）
|	|entityID:		17byte,guid
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----	
|	|N * |szName;   nbyte, 变量名字符串
|	|	 |varType:  4byte, 变量类型
|	|	 |varLen:	4byte, 变量数据长度
|	|	 |varBuf:   nbyte, 变量数据
|	|	 |----
|childNum:		2byte, 子节点个数(X)
|	|----
|   |compositeTyoe: 1byte, 该子节点Type
|   |compositeFlag: 4byte, 该子节点flag
|	|DBOP Type:     1byte, 数据库操作类型（枚举值）
|   |EntityID:		17byte,该子节点guid
|	|entityName:	32byte,该对象名字
|	|Attr Num:		2byte, 属性种类个数(N)
|	|	 |----
|	|N * |szName;   nbyte, 变量名字符串
|	|	 |varType:  4byte, 变量类型
|	|	 |varLen:	4byte, 变量数据长度
|	|	 |varBuf:   nbyte, 变量数据
|	|	 |----
|----发送消息--------------------------------------
*/

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
