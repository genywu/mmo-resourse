#pragma once
using namespace std;
#include "../../../Public/Module/GUID.h"
#include "../../../Public/DataStream/DataBlockSetWriteRead.h"
class CMessage;
class CBaseObject;
typedef list<CBaseObject*>	listBaseObject;
typedef list<CBaseObject*>::iterator itBaseObject;

typedef LRESULT (CALLBACK* P_FUNCTION_DO)(LPARAM lparam1, LPARAM lparam2);


// CBaseObject command target
// 游戏物体基类，所有对象和游戏相关的类由该类派生
class CBaseObject 
{
public:
	CBaseObject();
	virtual ~CBaseObject();

/////////////////////////////////////////////////////////////////////
// 基本属性
/////////////////////////////////////////////////////////////////////
protected:
	bool		m_bIsHide;		// 是否隐藏个人信息
	long		m_lType;		// 类型
	//long		m_lID;			// 游戏中唯一编号
	CGUID		m_guExID;
	long		m_lGraphicsID;	// 图片ID
	string		m_strName;		// 名称 < 20

public:
	bool GetIsHideInfo()			{ return m_bIsHide; }
	void SetIsHideInfo(bool l)		{ m_bIsHide = l; }
	long GetType()					{ return m_lType; }
	void SetType(long l)			{ m_lType = l; }
	//long GetID()					{ return m_lID; }
	//void SetID(long l)				{ m_lID = l; }
	const CGUID& GetExID() const	{ return m_guExID;}
	void SetExID(const CGUID& guid){m_guExID = guid;};
	long GetGraphicsID()			{ return m_lGraphicsID; }
	void SetGraphicsID(long l)		{ m_lGraphicsID=l; }
	const char* GetName()			{ return (LPCSTR)m_strName.c_str(); }
	void SetName(const char* str)	{ m_strName = str; }

	// 获得原始名(Monster或采集物需要重写GetOrigName接口返回原始名)
	virtual const char* GetOrigName()			{ return GetName();}

	virtual void GetCurGraphicTypeID(long &lType,long &lID){lType = m_lType;lID=m_lGraphicsID;}

/////////////////////////////////////////////////////////////////////
// 存取接口
/////////////////////////////////////////////////////////////////////
public:
	bool	m_bIncludeChild;	// 是否存储子节点数据

	virtual BOOL Load();		// 读取
	virtual BOOL Save();		// 存储

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);


/////////////////////////////////////////////////////////////////////
// 子节点列表（构成世界树）
/////////////////////////////////////////////////////////////////////
protected:
	CBaseObject* m_pFather;					// 父节点（只有一个）
	list<CBaseObject*> m_listObject;		// 子节点

public:
	list<CBaseObject*>* GetObjectList()		{return &m_listObject;}
	CBaseObject* GetFather() {return m_pFather;}				// 获取父节点
	void SetFather(CBaseObject* p)	{ m_pFather = p; }			// 设置父节点

	CBaseObject* IsThisObject(long typeID, const CGUID& id)			// 是否是本object
	{
		return (m_lType==typeID && m_guExID==id)?this:NULL;
	}

	virtual CBaseObject* RecursiveFindObject(long typeID,CGUID& id);		// 递归查找
	virtual CBaseObject* RecursiveFindObject(long typeID,char* strName);// 按名字查找
	
	virtual CBaseObject* FindChildGoods(const CGUID& guExID = CGUID::GUID_INVALID);// 查找子goods
	virtual CBaseObject* FindChildObject(long typeID,const CGUID& id);	// 查找子object!
	virtual CBaseObject* FindChildObject(const CGUID& id);	// 查找子object!
	virtual bool FindChildObject(CBaseObject* p);				// 查找子object!


public:
	virtual void DeleteChildObject(long typeID,const CGUID& ID);		// 删除子OBJECT
	virtual void DeleteChildObject(CBaseObject* p);				// 删除子OBJECT
	virtual void DeleteAllChildObject(CBaseObject* pExpect=NULL);	// 删除所有子OBJECT(pExpect = 除了)
	virtual void RemoveObject(CBaseObject* pObject);			// 移出OBJECT
	virtual void AddObject(CBaseObject* pObject);				// 添加OBJECT

/////////////////////////////////////////////////////////////////////
// AI
/////////////////////////////////////////////////////////////////////
	virtual void AI();		// AI处理

/////////////////////////////////////////////////////////////////////
// 消息响应
/////////////////////////////////////////////////////////////////////
	virtual void OnMessage(CMessage*)	{ return; }

/////////////////////////////////////////////////////////////////////
// 广播消息
/////////////////////////////////////////////////////////////////////
	virtual void BoardCast(long lCommand, long lParam);		// 广播

	// 回调一个函数对所有查找到的对应TYPE对象做处理
	virtual void DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam);
};
