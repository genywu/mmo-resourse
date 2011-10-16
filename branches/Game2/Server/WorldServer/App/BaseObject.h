#pragma once

class CMessage;
class CBaseObject;
typedef list< CBaseObject* > listBaseObject;
typedef list< CBaseObject* >::iterator itBaseObject;

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
	long		m_lType;		// 类型
	//long		m_lID;			// 游戏中唯一编号
	CGUID		m_guExID;
	long		m_lGraphicsID;	// 图片ID
	string		m_strName;		// 名称 < 20

public:
	long GetType()					{ return m_lType; }
	void SetType(long l)			{ m_lType = l; }
	/*long GetID()					{ return m_lID; }
	void SetID(long l)				{ m_lID = l; }*/
	virtual const CGUID& GetExID()	{ return m_guExID;}
	virtual void SetExID(const CGUID& guid){m_guExID = guid;};
	long GetGraphicsID()			{ return m_lGraphicsID; }
	void SetGraphicsID(long l)		{ m_lGraphicsID=l; }
	const char* GetName()			{ return (LPCSTR)m_strName.c_str(); }
	void SetName(const char* str)	{ if(NULL == str) return; m_strName = str; }

/////////////////////////////////////////////////////////////////////
// 存取接口
/////////////////////////////////////////////////////////////////////
public:
	bool	m_bIncludeChild;	// 是否存储子节点数据

	virtual BOOL Load();		// 读取
	virtual BOOL Save();		// 存储

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);		// 解码


	// 隐匿功能
public:
	BYTE			GetCanHide(void) { return m_byCanHide; }// 能否隐匿
	void			SetCanHide(BYTE flag) { m_byCanHide = flag; }
	BYTE			GetHideFlag(void) { return m_byHideFlag; }// 当前隐匿状态
	void			SetHideFlag(BYTE flag) { m_byHideFlag = flag; }
private:
	BYTE			m_byCanHide; // 能否隐匿
	BYTE			m_byHideFlag; // 当前隐匿状态

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

	CBaseObject* IsThisObject(long typeID, const CGUID& guid)			// 是否是本object
	{
		return (m_lType==typeID && guid==m_guExID)?this:NULL;
	}

	virtual CBaseObject* RecursiveFindObject(long typeID,const CGUID& guid);		// 递归查找
	virtual CBaseObject* RecursiveFindObject(long typeID,char* strName);// 按名字查找
	virtual CBaseObject* FindChildObject(long typeID,const CGUID& guExID = CGUID::GUID_INVALID);// 查找子object!
	virtual bool FindChildObject(CBaseObject* p);				// 查找子object!

	//virtual CBaseObject* CreateChildObject(long typeID,const CGUID& guid,const char* name=NULL, long lGraphicsID=0 );
	static CBaseObject* CreateObject(long typeID, const CGUID& guid);		// 创建OBJECT

public:
	virtual void DeleteChildObject(long typeID, CGUID& guExID = CGUID::GUID_INVALID);		// 删除子OBJECT
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


//对象泄露检测代码,author:wangqiao,date:2008.10.11
//支持单线程
#ifdef __MEMORY_LEAK_CHECK__
private:
	static long	m_nConstructFlag;	//用来标记对象某次构造的标示
	long	m_lSelfFlag;
	typedef map<long,long>	mapLL;
	typedef map<long,long>::iterator itLL;
	typedef map<long,map<long,long> >	mapLLL;
	typedef map<long,map<long,long> >::iterator itLLL;
	
	static mapLLL	m_mapConstructObjects;
public:
	void SetSelfConsflag(long l)	{m_lSelfFlag = l;}
	long GetSelfConsFlag()			{return m_lSelfFlag;}
	static void ClearAllConstrucRef()	{ m_mapConstructObjects.clear(); }
	static void SetConstructFlag(long nFlag)	{m_nConstructFlag=nFlag;}
	static long GetConstructFlag()				{return m_nConstructFlag;}
	static void AddConstructRef(long lType);
	static void RefConstructRef(long lType,long lConFlag);
	static void OutputConstructRef(long lType);
#endif
};
