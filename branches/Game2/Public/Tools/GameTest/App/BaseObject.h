#pragma once
#include "../Utility/timerdef.h"
#define D3DX_PI    ((FLOAT)  3.141592654f)

class CBaseObject;
typedef list<CBaseObject*>	listBaseObject;
typedef list<CBaseObject*>::iterator itBaseObject;

typedef LRESULT (CALLBACK* P_FUNCTION_DO)(LPARAM lparam1, LPARAM lparam2);

// CBaseObject command target
// 游戏物体基类，所有对象和游戏相关的类由该类派生
class CBaseObject:public CGameEvent
{
public:
	CBaseObject();
	virtual ~CBaseObject();

	// 基本属性
protected:
	long		m_lType;		// 类型
	CGUID		m_guExID;		// 游戏中唯一编号
	//	long		m_lGraphicsID;	// 图片ID
	char		m_strName[32];		// 名称 < 20

	//新加的编解码成员
	bool		m_bIsHide;

	//基本属性接口
public:
	long GetType() const 			{ return m_lType; }
	void SetType(long l)			{ m_lType = l; }

	const CGUID& GetExID()			{ return m_guExID;}
	void SetExID(const CGUID& guid){m_guExID = guid;};

	//	long GetGraphicsID() const		{ return m_lGraphicsID; }
	//	void SetGraphicsID(long l)		{ m_lGraphicsID=l; }

	const char* GetName()			{ return /*(LPCSTR)*/m_strName; }	//.c_str()返回为const char* 不用强转
	void SetName(const char* str)	{ strcpy(m_strName,str); }

	virtual const char* GetOrigName()			{ return GetName();}			// 获得原始名(Monster或采集物需要重写GetOrigName接口返回原始名)

	//	virtual void GetCurGraphicTypeID(long& lType, long& lID)	{ lType = m_lType; lID = m_lGraphicsID; }

	// 存取接口
public:
	bool	m_bIncludeChild;			//是否存储子节点数据
	virtual BOOL Load();		// 读取
	virtual BOOL Save();		// 存储

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool DecordFromDataBlock(DBReadSet& db_r,bool bEx = true);

	/////////////////////////////////////////////////////////////////////
	// 子节点列表（构成世界树）
	/////////////////////////////////////////////////////////////////////
protected:
	CBaseObject* m_pFather;							// 父节点（只有一个）
	list<CBaseObject*>	m_listObject;				// 子节点
public:
	list<CBaseObject*>* GetObjectList()		{return &m_listObject;}
	CBaseObject* GetFather()  {return m_pFather;}				// 获取父节点
	void SetFather(CBaseObject* p)	{ m_pFather = p; }			// 设置父节点

	CBaseObject* IsThisObject(long typeID, const CGUID& gID)			// 是否是本object
	{
		return (m_lType==typeID && m_guExID==gID)?this:NULL;
	}

	virtual CBaseObject* RecursiveFindObject(long typeID,CGUID& gID);	// 递归查找
	virtual CBaseObject* RecursiveFindObject(long typeID,char* strName);// 按名字查找

	virtual CBaseObject* FindChildGoods(const CGUID& guExID = CGUID::GUID_INVALID);// 查找子goods
	virtual CBaseObject* FindChildObject(long typeID,const CGUID& gId);
	virtual CBaseObject* FindChildObject(const CGUID & guid);
	virtual bool FindChildObject(CBaseObject* p);				// 查找子object!


public:	
	virtual void DeleteChildObject(long typeID,const CGUID& gID);		// 删除子OBJECT
	virtual void DeleteChildObject(CBaseObject* p);						// 删除子OBJECT
	virtual void DeleteAllChildObject(CBaseObject* pExpect=NULL);		// 删除所有子OBJECT(pExpect = 除了)
	virtual void RemoveObject(CBaseObject* pObject);					// 移出OBJECT
	virtual void AddObject(CBaseObject* pObject);						// 添加OBJECT

	// 消息响应
	virtual void OnMessage(CMessage*)	{ return; }
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var){return 0;};
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var){};
	//处理实体间消息函数
	//virtual long Proc(DWORD type,const void* var1,const void* var2){return 0;};
    virtual long Proc(eEventType type,const tagProcVar *pVar) { return 0; }
	// 回调一个函数对所有查找到的对应TYPE对象做处理
	virtual void DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam);
};

//
// Kevin Lynx, ugly codes, to access UI.
//
Timer *QueryTimer();