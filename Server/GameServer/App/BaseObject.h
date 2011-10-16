#pragma once

class CMessage;
class CBaseObject;

const LONG MOSTER_ROW_LENTH= 1024;
typedef hash_set<CGUID,hash_guid_compare>	hashsetGUID;
typedef hash_set<CGUID,hash_guid_compare>::iterator hsGUIDIt;

const long MIN_VAL[7] = { 0,		0,		0,			(long)0.0f,				-2147483647,	-32767, 0};
const long MAX_VAL[7] = { 256,	65536,	0x7FFFFFFF, (long)99999999.999f,	0x7FFFFFFF,		32767,	1};

// CBaseObject command target
// 游戏物体基类，所有对象和游戏相关的类由该类派生
class CBaseObject: public CGameEvent
{
public:
	CBaseObject();
	virtual ~CBaseObject();

public:
	/*static INT64	GetHashValue	( LONG, LONG );
	static LONG		CalculateType	( INT64 );
	static LONG		CalculateID		( INT64 );*/

/////////////////////////////////////////////////////////////////////
// 基本属性
/////////////////////////////////////////////////////////////////////
protected:
	long		m_lType;		// 类型
	//long		m_lID;			// 游戏中唯一编号
	CGUID		m_guid;		// 游戏中唯一编号
	long		m_lGraphicsID;	// 图片ID
	string		m_strName;		// 名称 < 20

	struct tagPty
	{
		BYTE lType;		// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool 7:CGUID
		union
		{
			BYTE	*plValue;
			WORD	*pwValue;
			DWORD	*pdwValue;
			long	*pValue;
			float	*pfValue;
			short	*psValue;
			bool	*pbValue;
			//CGUID	*pGUIDValue;
		};

		long  lCount;
		long  maxValue; // 自定义最大值
		long  minValue; // 自定义最小值

		DWORD attrEnumValue; // 该属性枚举值
        
        void SetType(BYTE type)
        {
            lType = type;
            minValue = MIN_VAL[type];
            maxValue = MAX_VAL[type];
        }
	};

public:
	long GetType() const								{ return m_lType;					};
	void SetType(long l)								{ if(0 == m_lType) m_lType = l;		};
	//long GetID()										{ return m_lID;						};
	//void SetID(long l)								{ m_lID = l;						};
	const CGUID&	GetExID() const						{return m_guid;						};
	void			SetExID(const CGUID& guid)			{m_guid = guid;						};
	long GetGraphicsID() const							{ return m_lGraphicsID;				};
	void SetGraphicsID(long l)							{ m_lGraphicsID=l;					};
    const char* GetName() const                         { return (LPCSTR)m_strName.c_str(); };
	void SetName(const char* str)						{ m_strName = str;					};

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
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码

	bool CodeToDataBlock_ForClient(DBWriteSet& setWriteDB,bool bExData = true/*是否发送给自己*/);		// 添加到CByteArray

	// 隐匿功能
public:
	BYTE			GetCanHide(void) { return m_byCanHide; }// 能否隐匿
	void			SetCanHide(BYTE flag) { m_byCanHide = flag; }
	BYTE			GetHideFlag(void) { return m_byHideFlag; }// 当前隐匿状态
	void			SetHideFlag(BYTE flag);// { m_byHideFlag = flag; }
private:
	BYTE			m_byCanHide; // 能否隐匿	0：不隐匿 1：隐匿
	BYTE			m_byHideFlag; // 当前隐匿状态 0:不隐匿 1：隐匿
/////////////////////////////////////////////////////////////////////
// 子节点列表（构成世界树）
/////////////////////////////////////////////////////////////////////
protected:
	CBaseObject* m_pFather;					// 父节点（只有一个）
	//list<CBaseObject*> m_listObject;		// 子节点

public:
	//list<CBaseObject*>* GetObjectList()		{return &m_listObject;}
	CBaseObject* GetFather()const {return m_pFather;}				// 获取父节点
	void SetFather(CBaseObject* p)	{ m_pFather = p; }			// 设置父节点

	//CBaseObject* IsThisObject(long typeID, long lID)			// 是否是本object
	//{
	//	return (m_lType==typeID && m_lID==lID)?this:NULL;
	//}

	//virtual CBaseObject* RecursiveFindObject(long typeID,long lID);		// 递归查找
	//virtual CBaseObject* RecursiveFindObject(long typeID,char* strName);// 按名字查找
	virtual CBaseObject* FindChildObject(long typeID,const CGUID& guid);// 查找子object!
	virtual bool FindChildObject(CBaseObject* p);				// 查找子object!

	//这个可以用CreateObjct+AddObject来实现
	//virtual CBaseObject* CreateChildObject(long typeID,const CGUID& guid,const char* name=NULL, long lGraphicsID=0 );
	//static CBaseObject* CreateObject(long typeID, const CGUID& guid);		// 创建OBJECT

public:
	virtual void DeleteChildObject(long typeID,long lID,CGUID& guExID = CGUID::GUID_INVALID);		// 删除子OBJECT
	virtual void DeleteChildObject(CBaseObject* p);				// 删除子OBJECT
	virtual void DeleteAllChildObject(CBaseObject* pExpect=NULL);	// 删除所有子OBJECT(pExpect = 除了)

	virtual void RemoveObject(CBaseObject* pObject);			// 移出OBJECT
	virtual void RemoveObject( LONG lType, LONG lID );
	virtual void AddObject(CBaseObject* pObject);				// 添加OBJECT
private:
	//被注册的事件结构
	typedef map<long,tagEvenRegVar>::iterator itEventVar;
	typedef map<long,tagEvenRegVar>	EventVar;

	//被注册的所有事件
	typedef map<eEventType,EventVar>::iterator itEvent;
	typedef map<eEventType,EventVar> RegEvents;
	RegEvents m_Events;

	long m_lEventIDCount;


	struct tagDelEventNode
	{
		eEventType type;
		long lEventID;
	};

	typedef list<tagDelEventNode>::iterator itDelEvent;
	list<tagDelEventNode> m_DelEventNodes;
public:

	//网络消息响应
	virtual void OnMessage(CMessage*)	{ return; }
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var) {return 0;}
	//当定时器被取消的调用
	void OnTimerCancel(DWORD timerid,const void* var);
	//处理实体间消息函数
	virtual long Proc(eEventType type,const tagProcVar *pVar){return 0;}

	long GetEventIDCount()	{m_lEventIDCount++; return m_lEventIDCount;}
	//注册事件
	virtual void RegisterEvent(eEventType nType,tagEvenRegVar &RegVar);
	//注销事件
	virtual void UnRegisterEvent(eEventType nType,long lEventID);
	//产生事件
	virtual void ProduceEvent(eEventType nType,long lvar1=0,long lVar2=0,long lVar3=0,long lVar4=0, void* pVar3=NULL);
	//删除注消事件节点
	virtual	void ClearEventNode();
	void ClearAllEventNode();
	//
/////////////////////////////////////////////////////////////////////
// 广播消息
/////////////////////////////////////////////////////////////////////
	//virtual void BoardCast(long lCommand, long lParam);		// 广播

	// 回调一个函数对所有查找到的对应TYPE对象做处理
	//virtual void DgFindObjectsByTypes( long lType, P_FUNCTION_DO p_function_do, LPARAM lparam);
};
