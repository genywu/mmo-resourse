//=============================================================================
/**
*  file: GameManager.h
*
*  Brief:游戏实体的管理类，包括事件和消息
*
*  Authtor:wangqiao
*	
*	Datae:2007-5-17
*/
//=============================================================================
#ifndef __GAME_MANAGER__
#define __GAME_MANAGER__
#pragma once
#include <set>
#include "..\public\GameEvent.h"
#include "..\public\TimerQueue.h"
#include "..\public\BaseDef.h"
#include "..\public\GUID.h"
#include "..\AppServer\goods\CGoods.h"
#include "..\AppServer\Build.h"
#include "..\Public\UseMemManage.h"

typedef CTimerQueue<CGameEvent*> Timer;
class CBaseObject;
class CPlayer;
class CMonster;

// 全局对象

class CGlobalRgnManager;	
class CRgnManager;
class CMailManager;
typedef hash_map<CGUID, CBaseObject*, hash_guid_compare> OBJMAP;

class GameManager : public BaseMemObj
{
public:
	GameManager(void);
	~GameManager(void);
private:
	//缺省的事件类，用来辅助type+guid的调度方式
	class CGUIDEvent: public BaseMemObj, public CGameEvent
	{
	public:
		OBJECT_TYPE		m_ObjectType;
		unsigned long	m_nID;
		CGUID			m_Guid;
		//计数值，用于累计该Event使用的次数
		long	m_nCount;
		//保存用这个GUID定制的定时器,便于在取消的时候使用
		typedef set<unsigned long>::iterator itGUIDTimer;
		set<unsigned long>	m_GUIDTimers;
		GameManager	*m_pManager;
	public:
		//缺省的构造函数
		CGUIDEvent(GameManager *pManager);
		~CGUIDEvent(){}

		//超时响应
		long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
		//当定时器被取消的调用
		void OnTimerCancel(DWORD timerid,const void* var);
		//网络消息响应
		void OnMessage(CMessage*){}
		//超时响应
		//处理实体间消息函数
		long Proc(eEventType type,const tagProcVar *pVar){return 0;}
	};

private:
	static GameManager* instance;

	Timer	*m_pTimer;

	typedef hash_map<CGUID,CGUIDEvent*,hash_guid_compare> GUIDEvents;
	typedef hash_map<CGUID,CGUIDEvent*,hash_guid_compare>::iterator itGUIDEvent;
	GUIDEvents	m_GuidEvents;

	//最大的预分配GUIDEvent节点
	unsigned long m_nMaxFreeGuidEventNum;
	list<CGUIDEvent*>	m_FreeGUIDEvents;

	//管理角色的登陆会话
	typedef map<long,long> MapLoginLL;
	typedef map<long,long>::iterator itMapLoginLL;

	//登陆会<socketid,count>
	MapLoginLL	m_LoginSession;

public:
	static GameManager* GetInstance();

	bool   Initialize(unsigned long nMaxFreeGuidEventNum = 10);
	void   Release();
	static void ReleaseInstance();

	//分配一个CGUIDEvent*
	CGUIDEvent*	AllocGUIDEvent();
	//释放一个CGUIDEvent*
	void	FreeGUIDEvent(CGUIDEvent*);

	//对<type>类型预定一个定时器,这个定时器将要在绝对时间futuretime触发
	//interval:如果大于0,表示这是一个周期性定时器,是一个相对时间
	//表示定时器下一次触发的间隔时间,该函数返回-1表示调度失败
	//arg是一个用户自定义参数	
	long Schedule(	CGameEvent* const pEvent,
		const void *arg,
		unsigned long futuretime,
		unsigned long interval = 0);

	//返回值表示取消的定时器个数
	int Cancel(CGameEvent* const pEvent);
	//取消单个<m_dwTimerID>等于<timerid>的定时器
	//如果arg不会空,将要设置调度时候设置的<arg>参数
	//返回值表示取消的定时器个数
	int Cancel(long timerid, const void** arg = 0);

	//依type+guid的方式定制定时器
	long Schedule(	OBJECT_TYPE objecttype,
		unsigned long nID,
		const CGUID& guid,
		const void *arg,
		unsigned long futuretime,
		unsigned long interval = 0);
	//取消guid的定时器
	int Cancel(const CGUID& guid);

	void Run();

//////////////////////////////////////////////////////////////////////////////////////////////
//下边的变量和函数用于管理游戏的具体实体类
public:
	//! 设置分配策略
	void   GameManager::SetPolicy(void);

	//! 创建一个object
	CBaseObject* CreateObject(long typeID, const CGUID& guid, const char *pInfo);
	//! 释放一个object
	void FreeObject(CBaseObject** ppObj);

	//! 一些其他的对象
	enum eOtherType
	{
		eOT_TimerVar,	//! 定时器参数对象
	};
	//! 创建一个上面类型的对象
	void*		CreateBlock(long lSize);
	//! 释放一个对象
	void		FreeBlock(void* pBlock, long lSize);

	//添加一个登陆会话,返回计数
	long AddLoginSession(long lSocketID);
	//移出一个计数值得session,返回保存的sokcetid
	long RemoveLoginSessionByCountNum(long lNum);
	//根据socketID移出会话
	void RemoveLoginSessionBySocketID(long lSocketID);

	void PutObjNumInfo(const char* pszFileName);
	
public:
	//! 查找匹配type+guid的CBaseObject;
	CBaseObject	*FindObject(OBJECT_TYPE objecttype,const CGUID& guid);

#ifdef DEBUG_NEW
	//! buf空间要大于512BYTE
	void	GetPoolStateString(char *buf, long bufSize);
#endif

	
public:
	CRgnManager*		GetRgnManager(void);
private:
	// 全局对象
	CRgnManager*		g_RgnManager;
public:
	CMailManager*		GetMailManager(void);
private:
	CMailManager*		g_MailManager;

private:
	OBJMAP m_GoodsList;					// TYPE_GOODS
	OBJMAP m_MonsterList;				// TYPE_MONSTER
	OBJMAP m_CollectionList;			// TYPE_COLLECTION
	OBJMAP m_PlayerList;				// TYPE_PLAYER
	OBJMAP m_RegionList;				// TYPE_REGION
	OBJMAP m_BuildList;					// TYPE_BUILD
	OBJMAP m_GateList;					// TYPE_CITYGATE
	OBJMAP m_OtherList;					// 其他对象

//////////////////////////////////////////////////////////////////////////////////////////////
//下边是实体间消息处理函数
	void SendMessage(OBJECT_TYPE objecttype,unsigned long nID,const CGUID& guid,
					unsigned long type,const void* var1,const void* var2);
	void SendMessage(CGameEvent* pGameEvent,
					unsigned long type,const void* var1,const void* var2);
};

#define TIMER_SCHEDULE GameManager::GetInstance()->Schedule
#define TIMER_CANCEL GameManager::GetInstance()->Cancel

//定义消息发送宏
#define SEND_MESSAGE GameManager::GetInstance()->SendMessage





#endif

