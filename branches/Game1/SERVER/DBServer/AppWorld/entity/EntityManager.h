//EntityManager.h/////////////////////////////////////////////////////////////////////
//对象类:该对象管理所有的对象实例和对象的配置信息数据
//Author:安海川
//Create Time:2008.11.03
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H
#include "../public/dbdefine.h"
#include "../public/basedef.h"

#include "../../../../dbaccess/MyAdoBase.h"
#include <set>
#include <map>
#include <string>
#include "../public/GUID.h"
#include "Account.h"

class CMessage;
class CDataEntityManager;
class CEntityGroup;

class DBAdoBase;
class DBThread;
class DBAccessThread;
class DBMailThread;

struct ThreadData;


#define MAX_DB_ENTITY_NUM 1024

// 对象构造配置文件信息数据
struct tagBuildFileInfo
{
	CGUID	leafGuid;
	long	lLevel;
	long	lComType;
	string	strLeafComFlag;	// 叶子对象名字
	string	strRootComFlag;	// 根对象名字
	CGUID	rootGuid;


	tagBuildFileInfo()
		:leafGuid(NULL_GUID),rootGuid(NULL_GUID),lLevel(0),lComType(-1),
		strLeafComFlag(""),strRootComFlag("")
	{

	}
	~tagBuildFileInfo()
	{

	}
	void AddToByteArray(vector<BYTE>& pBA)
	{
		_AddToByteArray(&pBA, leafGuid);
		_AddToByteArray(&pBA, lLevel);
		_AddToByteArray(&pBA, lComType);
		_AddToByteArray(&pBA, strLeafComFlag.c_str());
		_AddToByteArray(&pBA, strRootComFlag.c_str());
		_AddToByteArray(&pBA, rootGuid);
	}

};
struct tagEntityAttrInfo
{
	tagEntityAttrInfo()
		:eNumDataType(DATA_OBJECT_UNKNOWN),lMaxSize(0),lUseage(0),lAttrTypeEnum(0)
	{}
	~tagEntityAttrInfo(){}

	// 属性类型枚举值
	long lAttrTypeEnum;
	// 最大长度
	long lMaxSize;
	// 用法
	long lUseage;
	DATA_OBJECT_TYPE eNumDataType;
};
struct tagDbIdxInfo
{
	tagDbIdxInfo::tagDbIdxInfo()
		: strIndex(""),lUnique(0)
	{

	}
	tagDbIdxInfo::tagDbIdxInfo(const std::string& stridx, long uni)
		: strIndex(stridx),lUnique(uni)
	{

	}
	tagDbIdxInfo::~tagDbIdxInfo()
	{

	}

	std::string strIndex;
	long		lUnique;
};
struct tagEntityBuildInfo
{
	tagEntityBuildInfo();
	~tagEntityBuildInfo();

	long							lComType;			// 对象类型
	string							strComFlag;			// 对象名字
	long							lHeight;			// 树高度
	CGUID							guid;				// guid
	string							strTblName;			// 数据库表名
	long							lDbOperFlag;		// 数据库操作标志
	long							lLeafNum;			// 叶子个数
	string							strPrimarykey;		// MEM主键值名
	string							strDBPrimarykey;	// DB主键值名
	long							lHhasAttrFlag;		// 是否有属性
	long							lLeafComType;		// 叶子类型
	string							strLeafComFlag;		// 叶子名字
	long							lHasDetailLeaves;	// 是否有分散叶子节点(类似goodsgroup)
	string							strDbQueryName;		// 数据库查找变量名
	long							lDbQueryType;		// 数据库查找变量类型
	vector<tagDbIdxInfo>			vecIndex;		// 数据表聚集索引名
	vector<tagDbIdxInfo>			vecNoCluIndex;	// 数据表非聚集索引名
	vector<tagDbIdxInfo>			vecCoIndex;		// 数据表复合索引名
	map<string, tagEntityAttrInfo>	pDataMgr;			// 属性数据
	map<string, tagBuildFileInfo*>	mapBuildFileInfo;	// 结构数据
	vector<long>					vecAttrEnum;		// 按顺序的属性枚举值vector
	BYTE							pQueryVar[256];		// 数据库查找变量属性信息

	void AddToByteArray(vector<BYTE>& pBA)
	{
		_AddToByteArray(&pBA, lComType);
		_AddToByteArray(&pBA, strComFlag.c_str());
		_AddToByteArray(&pBA, lHeight);
		_AddToByteArray(&pBA, guid);
		_AddToByteArray(&pBA, strTblName.c_str());
		_AddToByteArray(&pBA, lDbOperFlag);
		_AddToByteArray(&pBA, lLeafNum);
		_AddToByteArray(&pBA, strPrimarykey.c_str());
		_AddToByteArray(&pBA, lHhasAttrFlag);
		_AddToByteArray(&pBA, lLeafComType);
		_AddToByteArray(&pBA, strLeafComFlag.c_str());
		_AddToByteArray(&pBA, lHasDetailLeaves);

		_AddToByteArray(&pBA, strDbQueryName.c_str());		//数据库查找变量名	
		if(strDbQueryName == "NULL")
			_AddToByteArray(&pBA, (BYTE)0);
		else
		{
			_AddToByteArray(&pBA, (BYTE)1);
			_AddToByteArray(&pBA, (long)lDbQueryType);//数据库查找变量类型
			_AddToByteArray(&pBA, pQueryVar, sizeof(pQueryVar));
		}

		_AddToByteArray(&pBA, (WORD)pDataMgr.size()); // 属性种类个数
		map<string, tagEntityAttrInfo>::iterator itr = pDataMgr.begin();
		for(; itr != pDataMgr.end(); itr++)
		{
			_AddToByteArray(&pBA, itr->first.c_str());//变量名字符串
			_AddToByteArray(&pBA, (long)itr->second.lAttrTypeEnum);//变量枚举值
			_AddToByteArray(&pBA, (long)itr->second.eNumDataType);//变量类型（long）
		}

		_AddToByteArray(&pBA, (WORD)mapBuildFileInfo.size());
		map<string, tagBuildFileInfo*>::iterator bfInfoItr = mapBuildFileInfo.begin();
		for(; bfInfoItr != mapBuildFileInfo.end(); bfInfoItr++)
		{
			bfInfoItr->second->AddToByteArray(pBA);
		}
	}
};

class CEntityManager
{
public:
	CEntityManager();
	~CEntityManager();

	typedef std::map<std::string, std::map<CGUID, CEntityGroup*>> DBEntityComponentMap;
	typedef DBEntityComponentMap::iterator DBEntityComponentMapItr;

	typedef std::map<std::string, Account*> AccountMap;
	typedef AccountMap::iterator AccountMapItr;

	typedef std::map<std::string, tagEntityBuildInfo*> AttrDefMap;
	typedef AttrDefMap::iterator AttrDefMapItr;

	// 初始化DBThread
	void InitDBThread(std::string strProvider, 
		std::string strDataSource, 
		std::string strInitialCatalog,
		std::string strUserID, 
		std::string strPassword);

	void ProcessMsg(CMessage* msg);

	//-------------- interfaces
	// CBaseEntity
	// 添加一个BaseEntity到Map末尾
	void AddOneRootEntity(CEntityGroup* entity);
	// 删除一个BaseEntity
	void DelOneRootEntity(CEntityGroup* entity);
	// 删除一个BaseEntity
	void DelOneRootEntity(const string& comFlag, const CGUID& playerID);
	// 清空链表
	void ClearBaseEntityMap(void);

	DBEntityComponentMap&		GetBaseEntityMap	(void)											{ return m_mapEntitys; }
	CEntityGroup*				FindBaseEntity		(const string& comFlag, const CGUID& guid);

	AccountMap&					GetAccountMap		(void)											{ return m_mapAccounts; }

	// 添加一个Account到Map末尾
	bool						AddAccount			(Account* acc);
	// 删除一个Account
	void						DelAccount			(const char* szCdkey);
	Account*					FindAccount			(const std::string& strCdkey);

	// 产生ID
	bool			GenerateGUID					(CGUID& guid);

	// 从消息取得操作对象
	CEntityGroup*	FindRootEntity					(const string& flag, const CGUID& guid);
	// 创建对象
	CEntityGroup*	CreateRootEntity				(const string& flag, const CGUID& guid);

	//! 遍历所有对象，产生消息
	void			TravelForGenerateThreadData		(void);
	//! 处理从线程返回的消息
	void			ProcessRetThreadData			(void);

	//! 取得存储过程的对象指针
	CEntityGroup*	ReleaseProcedurePtr				(const CGUID& guid);
	//! 取得指定类型对象指针
	CEntityGroup*	ReleaseEntityPtr				(const string& strComFlag, const CGUID& guid);
	//! 取得account对象指针
	CEntityGroup*	ReleaseAccountPtr				(const string& szCdkey);

	//! 需要手动释放该接口分配的内存
	CBaseEntity*	NewBaseEntity					(const string& strComFlag, const CGUID& guid);
	//! 对象内存回收接口
	void			DelBaseEntity					(CBaseEntity** pEntity);

	//!	清除玩家
	bool			ClearEntitys					(void);

public:
	std::map<long, DBThread*>&	GetLoadThreadMap	(void)		{ return m_mapLoadThread; }
	std::map<long, DBThread*>&	GetSaveThreadMap	(void)		{ return m_mapSaveThread; }
	DBAccessThread*				GetAccessThread		(void)		{ return m_pAccessThread; }

private:
	std::map<long, DBThread*>	m_mapLoadThread;
	std::map<long, DBThread*>	m_mapSaveThread;
	DBAccessThread*				m_pAccessThread;
	DBMailThread*				m_pMailThread;

public:
	//!	压入一个ThreadData
	bool						PushLoadThreadData			(ThreadData* loadData);
	bool						PushSaveThreadData			(ThreadData* saveData);

	//!	压入多个ThreadData
	bool						PushLoadThreadData			(list<ThreadData*>& loadDataList);
	bool						PushSaveThreadData			(list<ThreadData*>& saveDataList);
	//!	取出一个ThreadData
	bool						GetThreadData				(ThreadData** loadData, long dwThreadID, eThreadType type);
	//! 压入一个返回ThreadData
	bool						PushRetThreadData			(ThreadData* loadData);
	bool						GetRetThreadData			(list<ThreadData*>& loadDataList);
	long						GetSaveThreadOperDataSize	(list<CGUID>& saveBeginGuidList);
	//! 弹出一个正在被线程处理的guid
	bool						PopBeginEntityGuid			(const CGUID& guid, eThreadType type);
	
	//! 取得LoadThreadDataSize
	long						GetLoadThreadDataSize		(void);

	//!	返回等待操作对象个数
	long						GetLoadWaitOperEntityNum(void) { return m_listLoadThreadOperData.size(); }
	long						GetSaveWaitOperEntityNum(void) { return m_listDirtyEntityGuid.size() + m_listDirtyEntityName.size() + m_listSaveThreadOperData.size(); }
	//!	压入一个ThreadData
	bool						PushMailThreadData		(list<ThreadData*>& loadDataList);
	//!	压入一个ThreadData
	bool						PushMailThreadData		(ThreadData* loadData);
	//!	取出一个ThreadData
	bool						GetMailThreadData		(list<ThreadData*>& loadDataList, long dwThreadID);
	bool						SetMailThreadData		(list<ThreadData*>& loadDataList);

	//! 压入一个线程推出事件句柄
	bool						PushThreadQuitEventHandle		(HANDLE& handle);
	//
	bool						AddMailThreadQuitEventHandle	(HANDLE& handle);

	//!	设置LoadEvent
	static void					SetLoadThreadEvent			(void);
	//!	等待LoadEvent
	static void					WaitLoadThreadEvent			(void);
	//! 重设LoadEvent
	static void					ResetLoadThreadEvent		(void);

	//!	设置SaveEvent
	static void					SetSaveThreadEvent			(void);
	//!	等待SaveEvent
	static void					WaitSaveThreadEvent			(void);
	//! 重设SaveEvent
	static void					ResetSaveThreadEvent		(void);

	//!	设置MailEvent
	static void					SetMailThreadEvent		(void);
	//! 等待MailEvent
	static void					WaitMailThreadEvent		(void);
	//! 重设MailEvent
	static void					ResetMailThreadEvent	(void);

	//!	在DB中创建表
	void						CreateOtherTableInDb	(const std::string& tableName, const std::string& iniName);
	
	//!	创建聚集/非聚集索引
	void						CreateOtherTableIndexInDb(const string& tableName, std::vector<tagDbIdxInfo>& vecIdx, bool isClustered);
	//!	创建复合索引
	void						CreateOtherTableComIndexInDb(const string& tableName, std::vector<tagDbIdxInfo>& vecIdx);

	//!	根据对象创建其Insert和Update存储过程，并保存至m_mapTblProc中以供查询
	void						CreateTblProcedure		(CBaseEntity* pRoot);
	//!	查询一个对象某个存储过程名
	const string&				FindTblProcedure		(CBaseEntity* pRoot);
	//! 添加一个对象的存储过程
	void						AddTblProcedure			(CBaseEntity* pRoot, DB_OPERATION_TYPE opType, const string& name);
	//!	运行一个清除超时邮件的存储过程
	void						RunDeleteTimeoutMailProc(void);
	//! 初始化属性配置
	void						InitPropertyIniData();
	//! 读取配置属性文件
	const char* 				LoadObjAttrInfo			(const char* szPath);
	void						LoadObjStructInfo		(const char* szPath);
	void						LoadProcedureInfo		(const char* szPath);

	AttrDefMap&					GetObjAttrDef(void) { return m_mapObjAttrDef; }
	void						ExecSqlString(const std::string& sqlStr);
	void						Release(void);

	std::vector<std::string>& GetProcedureNameVec(void) { return m_vecProcedureName; }

	//------玩家对象操作工具接口--------
	// 根据叶子类型在其根结点下查找其父节点
	CEntityGroup*	FindLeafFatherByComFlag		(CEntityGroup* pRoot, const string& leafComFlag);
	// 根据叶子类型在其根结点下查找其节点
	CBaseEntity*	FindLeafByComFlag			(CEntityGroup* pRoot, const string& leafComFlag, const CGUID& leafGuid);
	// 设置玩家节点上某些子节点的数据库操作标志位
	void			SetEntityLeavesDbOperFlag	(CEntityGroup* pBaseEntity, const string& leafComFlag, bool operFlag);
	// 将玩家节点上所有子节点数据库操作标志位清零
	void			ResetEntityAllDbOperFlag	(CBaseEntity* pBaseEntity, bool operFlag=false);
	// 设置从根结点开始的所有节点数据库操作标志
	void			SetEntityAllDbOperType		(CBaseEntity* pBaseEntity, DB_OPERATION_TYPE opType);

	// 计算一个EntityGroup以及其子结点的Size
	long			ComputeEntityGroupMemSize	(CEntityGroup* pBaseEntity);
	// 根据叶子类型在其根结点下查找其节点
	CEntityGroup*	FindLeafGroupByComFlag		(CEntityGroup* pRoot, const string& leafComFlag);


	// 映射属性字符串名和枚举值名
	void				AddAttrEnumAndStr		(const string& strName, long lEnum);
	long				GetAttrEnumByStr		(const string& strComFlag, const string& strName);
	const string&		GetAttrStrByEnum		(long lEnum);
	// 取得配置文件的数据信息
	tagEntityBuildInfo* GetEntityBuildInfo		(const string& strComFlag);
	// DATA_OBJECT_TYPE
	long				GetDataObjectType		(const string& strComFlag, const string& strName);
	long				GetDataObjectType		(const string& strComFlag, long lEnum);
	// 数据库属性标志位: 1|0: 1(数据库自动操作) 0程序操作(默认)
	BYTE				GetDbUseageFlag			(const string& strComFlag, const string& strName);
	BYTE				GetDbUseageFlag			(const string& strComFlag, long lEnum);
	// 数据块最大长度
	long				GetBufMaxSize			(const string& strComFlag, const string& strName);
	long				GetBufMaxSize			(const string& strComFlag, long lEnum);

	// 根据对象的节点信息产生查询条件
	bool				GenerateQuerySavePlayerInfo(CEntityGroup* pPlayer);
	//	不重复的插入一个变化对象的GUID
	void				AddDirtyEntityGuid		(const CGUID& guid);
	void				AddDirtyEntityName		(const char* szName);

	// 保存全部对象
	void				SaveAllEntity			(void);

	//! 取得正在被线程操作的对象数量
	long				GetBeginLoadEntityNum		(void)	{ return m_listLoadBeginEntityGuid.size(); }
	long				GetBeginSaveEntityNum		(void)	{ return m_listSaveBeginEntityGuid.size(); }

	//! 查找是否在操作队列有该账号
	bool				FindAccountIsInOperQueue(const string& strAcc);
	//! 查找是否再操作队列有该对象
	bool				FindEntityIsInOperQueue(const CGUID& guid);
private:
	// 加载表字段配置
	void LoadObjInfo(vector<const char*>& AttrInfoList,
		const char* szStructInfo,
		const char* szTable);

	//!					SQL队列Load临界对象
	CRITICAL_SECTION	m_LoadCriticalSection;

	//!					SQL队列Save临界对象
	CRITICAL_SECTION	m_SaveCriticalSection;

	//!					邮件队列临界对象
	CRITICAL_SECTION	m_MailCriticalSection;
	//!					线程返回队列临界对象
	CRITICAL_SECTION	m_RetCriticalSection;
	//!					邮件线程返回队列临界对象
	CRITICAL_SECTION	m_RetMailCriticalSection;

	CRITICAL_SECTION	m_ThreadQuitCS;

	DBEntityComponentMap	m_mapEntitys;
	AccountMap				m_mapAccounts;

	//! 记录有变化的对象
	std::list<CGUID>		m_listDirtyEntityGuid;
	std::list<string>		m_listDirtyEntityName;

	//! 记录正在被线程操作的对象
	std::list<CGUID>		m_listLoadBeginEntityGuid;
	std::list<CGUID>		m_listSaveBeginEntityGuid;

	// 投递到线程的操作数据
	std::list<ThreadData*> m_listLoadThreadOperData;
	std::list<ThreadData*> m_listSaveThreadOperData;
	// 读取或存储过程返回数据
	std::list<ThreadData*> m_listThreadRetData;

	//!					Load事件句柄
	static HANDLE		m_hLoadThreadEvent;
	//!					Save事件句柄
	static HANDLE		m_hSaveThreadEvent;
	//!					邮件线程事件句柄
	static HANDLE		m_hMailThreadEvent;

	// Obj属性信息,Obj对象的属性名,id
	AttrDefMap			m_mapObjAttrDef;

	std::vector<std::string> m_vecProcedureName;

	std::vector<HANDLE> m_vecThreadQuitEvent;

	// 自动生成属性的枚举值
	long m_lAttrEnumValue;
	// 映射属性字符串名和枚举值名
	map<long, string> m_mapAttrEnumToStr;

	// 邮件操作完全独立
	list<ThreadData*> m_listMailThreadData;
	// 邮件操作返回数据
	list<ThreadData*> m_listMailThreadRetData;

	HANDLE	m_MailThreadQuitEvent;
	map<string, map<DB_OPERATION_TYPE,string>> m_mapTblProc;
};

extern std::string NULL_STRING;

#endif//DB_ENTITY_MANAGER_H