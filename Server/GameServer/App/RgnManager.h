#ifndef RGN_MANAGER_H
#define RGN_MANAGER_H

//#include "ProxyServerRegion.h"
#include "ServerRegion.h"

// 定义场景定时器
typedef CTimerQueue<CServerRegion*> RegionTimer;

// 定义排队定时器
typedef CTimerQueue<CPlayer*> QueueTimer;

// 定义玩家创建个人副本定时器
typedef CTimerQueue<CPlayer*> CreatePRgnTimer;
// 定义玩家创建组队副本定时器
typedef CTimerQueue<CPlayer*> CreateTRgnTimer;
// 定义玩家组队副本清人定时器
typedef CTimerQueue<CPlayer*> RgnKickPlayerTimer;

class CMessage;
class CPlayer;
class CScript;

class CRgnManager
{
public:
	// 个人房屋副本物品类
	struct PHGoods
	{
		// 物品GUID
		CGUID guid;
		// 物品名字
		char name[64];
		// 朝向
		int dir;
		// X长度
		int width;
		// Y长度
		int height;
		// 坐标
		float posX;
		float posY;

		void operator=(const PHGoods& right)
		{
			if(&right != this)
			{
				// 物品GUID
				guid = right.guid;
				// 物品名字
				memcpy(name, right.name, sizeof(right.name));
				// 朝向
				dir = right.dir;
				// X长度
				width = right.width;
				// Y长度
				height = right.height;
				// 坐标
				posX = right.posX;
				posY = right.posY;
			}
		}
		bool operator==(const PHGoods& right)
		{
			if(this != &right)
			{
				if(
				guid == right.guid &&
				strcmp(name, right.name) == 0 &&
				dir == right.dir &&
				width == right.width &&
				height == right.height &&
				posX == right.posX &&
				posY == right.posY)
				return true;
				else
					return false;
			}
			return true;
		}
	};

	struct tagDupRgnData
	{
	public:
		tagDupRgnData(void);
		~tagDupRgnData(void);
		long lBufSize;
		const BYTE* pData;
	};

	CRgnManager();
	~CRgnManager();

	typedef std::map<CGUID/*RgnID*/, CServerRegion*> RgnByIDMap;
	typedef RgnByIDMap::iterator RgnByIDMapItr;

	// 个人房屋副本物品管理容器
	typedef std::vector<PHGoods> vecPHGoods;
	typedef vecPHGoods::iterator vecPHGoodsItr;

	typedef std::map<CGUID/*PHRgnID*/, vecPHGoods> PHGoodsMap;
	typedef PHGoodsMap::iterator PHGoodsMapItr;
	
	//! 处理定时,等功能的接口
	void Run(void);
	//! 创建Rgn,向WS发送RGN_CREATE消息
	long CreateRgnMsg(Script* pScript/*挂起的脚本ID*/, long templateRgnID/*模板Region的ID*/, eRgnType rgnType, const CGUID& ownerID=NULL_GUID/*NORMAL类型则ID为空*/, const char* szRetVarName=NULL/*异步返回值变量名*/, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);
	//! 删除Rgn,向WS发送RGN_DELETE消息
	long DeleteRgnMsg(const CGUID& runScriptID/*挂起的脚本ID*/, eRgnType rgnType, const CGUID& ownerID, const CGUID& rgnID, const char* szRetVarName=NULL/*异步返回值变量名*/, const char* szRetRgnIDName=NULL/*异步返回值变量名*/);

	//! 创建Rgn,向WS发送RGN_CREATE消息
	long OnCreateRgnMsg(CMessage* pMsg);
	//! 删除Rgn,向WS发送RGN_DELETE消息
	long OnDeleteRgnMsg(CMessage* pMsg);

	//! 查找Rgn
	CServerRegion* FindRgnByRgnGUID(eRgnType rgnType, const CGUID& rgnID);
	//! 遍历查找region
	CServerRegion* FindRgnByRgnGUID(const CGUID& rgnID);
	//! 遍历查找region
	CServerRegion* FindRgnByTemplateID(eRgnType rgnType, long templateID);
	
	// 创建个人房屋副本物品(Monster)
	void CreatePHGoods(const CGUID& rgnID, const char* name, long nCounts, long posX, long posY,
						long width, long height, long dir, const char* strScript, const char* strTalk);
	// 删除个人房屋副本物品(Monster)
	void DeletePHGoods(const CGUID& rgnID, const CGUID& goodsID);
	// 删除个人房屋副本物品(Monster)
	void DeletePHGoods(const CGUID& rgnID, const char* goodsName);

	RgnByIDMap& GetPersonalDupRgnMap() { return m_mapPersonalDupRgn; };
	RgnByIDMap& GetTeamDupRgnMap() { return m_mapTeamDupRgn; };
	RgnByIDMap& GetPersonalHouseDupRgnMap() { return m_mapPersonalHouseDupRgn; };
	RgnByIDMap& GetNormalRgnMap() { return m_mapNormalRgn; };

	CServerRegion* AddNormalRgn(const CGUID& rgnID, long templateRgnID);
	void DelNormalRgn(const CGUID& rgnID, long templateRgnID);

	CServerRegion* AddCreatedDupRgn(const CGUID& rgnID, eRgnType type);
	void DelCreatedDupRgn(const CGUID& rgnID, eRgnType type);

	// 取得副本场景的当前值
	long GetCurNormalRgnNum(void) { return m_CurNormalRgnNum; }
	long GetCurPersonalDupRgnNum(void) { return m_CurPersonalDupRgnNum; }
	long GetCurPersonalHouseDupRgnNum(void) { return m_CurPersonalHouseDupRgnNum; }
	long GetCurTeamDupRgnNum(void) { return m_CurTeamDupRgnNum; }

	// 设置副本场景的当前值
	void SetCurNormalRgnNum(long value) { m_CurNormalRgnNum = value; }
	void SetCurPersonalDupRgnNum(long value) { m_CurPersonalDupRgnNum = value; }
	void SetCurPersonalHouseDupRgnNum(long value) { m_CurPersonalHouseDupRgnNum = value; }
	void SetCurTeamDupRgnNum(long value) { m_CurTeamDupRgnNum = value; }


	// 取得该服务器的其实排队数
	long GetQueueMaxNum(void) { return m_QueueMaxNum; }
	void SetQueueMaxNum(long value) { m_QueueMaxNum = value; }

	// 取得该服务器的当前排队数
	long GetCurQueueNum(void) { return m_CurQueueNum; }
	void SetCurQueueNum(long value) { m_CurQueueNum = value; }

	RegionTimer* GetRegionTimer(void) { return &m_RegionTimer; }

	QueueTimer* GetQueueTimer(void) { return &m_QueueTimer; }

	CreatePRgnTimer* GetCreatePRgnTimer(void) { return &m_CreatePRgnTimer; }
	CreateTRgnTimer* GetCreateTRgnTimer(void) { return &m_CreateTRgnTimer; }
	RgnKickPlayerTimer* GetRgnKickPlayerTimer(void) { return &m_RgnKickPlayerTimer; }
	// 当前正在排队的玩家 
	std::map<long/*起始时间*/, CPlayer*>& GetPlayerQueue(void) { return m_PlayerQueue; }

	// 个人房屋副本物品容器
	PHGoodsMap& GetPHGoodsMap(void) { return m_PHGoodsMap; }

	//! 解码分线场景数据
	bool DecodeLinedRgnInfoFromByteArray(BYTE* buf, long& pos, long bufSize=-1);

	bool AddLinedRgnInfoToByteArray(long linedId, DBWriteSet& setWriteDB);

	// 分线场景数据
	map<long/*分线ID*/, vector<long/*templateID*/>>& GetLinedRgnIdByLinedIdMap(void) { return m_mapLinedRgnIdByLinedId; }

	// 玩家副本数据操作接口
	//!					添加一个玩家队伍副本GUID
	void				AddTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID, const CGUID& rgnGuid, BOOL bCover);
	//!					删除一个玩家队伍副本GUID
	void				DelTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID);
	void				DelTeamRgnGUID(const CGUID& ownerGuid, const CGUID& rgnGuid);
	//!					清除一个玩家的队伍副本
	void				ClearTeamRgnGUID(const CGUID& ownerGuid);
	//!					得到一个队伍副本的GUID
	const	CGUID&		GetTeamRgnGUID(const CGUID& ownerGuid, LONG lTemplateRgnID);
	//!					通过顺序得到一个玩家的队伍的副本GUID
	const	CGUID&		GetTeamRgnGuidByIndex(const CGUID& ownerGuid, LONG lIndex);
	//!					通过顺序得到一个玩家的队伍的副本模板ID
	LONG				GetTeamRgnTemplateIdByIndex(const CGUID& ownerGuid, LONG lIndex);
	//!					得到玩家的队伍副本GUID列表
	map<LONG, CGUID>*	GetTeamRgnGuidMap(const CGUID& ownerGuid);//{return m_mapTeamRgnGUID;}
	//!					玩家的队伍副本GUID编码
	void				RgnGuidCode(const CGUID& ownerGuid, vector<BYTE> &vData);
	//!					玩家的队伍副本GUID解码
	void				RgnGuidDecode(const CGUID& ownerGuid, BYTE *pData, LONG &lPos);
	//!					解码交换玩家的队伍副本（不覆盖相同模板的副本）
	void				RgnGuidSwapFromArray(const CGUID& ownerGuid, BYTE *pData, LONG &lPos);

	//!					玩家的队伍副本GUID编码
	void				RgnGuidCode(const CGUID& ownerGuid, DBWriteSet& setWriteDB);
	//!					玩家的队伍副本GUID解码
	void				RgnGuidDecode(const CGUID& ownerGuid, DBReadSet& setReadDB);
	//!					解码交换玩家的队伍副本（不覆盖相同模板的副本）
	void				RgnGuidSwapFromArray(const CGUID& ownerGuid, DBReadSet& setReadDB);
	//!					发送更新数据到WS
	void				SendTeamRgnToWs(const CGUID& ownerGuid);


	//!					得到玩家的个人副本GUID列表
	map<CGUID, CGUID>&	GetPersonalRgnGuidMap(const CGUID& ownerGuid);
	void				AddPersonalRgnGUID(const CGUID& ownerGuid, const CGUID& rgnGuid);
	//!					删除一个玩家个人副本GUID
	void				DelPersonalRgnGUID(const CGUID& ownerGuid);
	//!					得到一个个人副本的GUID
	const	CGUID&		GetPersonalRgnGUID(const CGUID& ownerGuid);

	//!					添加一个副本数据块
	bool				AddDupRgnData(long lTemplateID, const BYTE* pData, long lBufSize);
	//!					判断是否有该副本数据块
	long				FindDupRgnData(long lTemplateID);
	//!					得到该副本数据块
	const BYTE*			GetDupRgnData(long lTemplateID);
	//!					清除所有副本数据块
	void				DelAllDupRgnData(void);
	//!					数据块个数
	long				GetDupRgnDataNum(void) { return (long)m_mapDupRgnData.size(); }

private:

	map<CGUID/*ownerID*/, map<LONG/*templateID*/, CGUID/*rgnGuid*/>> m_mapTeamRgnGUID;
	map<CGUID/*ownerID*/, CGUID/*rgnGuid*/> m_mapPersonalRgnGUID;

	//! 所有副本对于每个Player同时只能有1个,故采用OwnerID来对应CServerRegion*
	RgnByIDMap m_mapPersonalDupRgn;
	RgnByIDMap m_mapTeamDupRgn;
	RgnByIDMap m_mapPersonalHouseDupRgn;
	RgnByIDMap m_mapNormalRgn;

	long	m_CurNormalRgnNum;
	long	m_CurPersonalDupRgnNum;
	long	m_CurPersonalHouseDupRgnNum;
	long	m_CurTeamDupRgnNum;

	// 创建副本排队起始数
	long    m_QueueMaxNum;
	// 创建副本排队最大数
	long    m_CurQueueNum;

	// 场景定时器
	RegionTimer m_RegionTimer;
	// 创建副本排队定时器
	QueueTimer  m_QueueTimer;

	// 定义玩家创建个人副本定时器
	CreatePRgnTimer m_CreatePRgnTimer;
	// 定义玩家创建组队副本定时器
	CreateTRgnTimer m_CreateTRgnTimer;
	// 副本清人定时器
	RgnKickPlayerTimer m_RgnKickPlayerTimer;

	// 当前正在排队的玩家 
	std::map<long/*起始时间*/, CPlayer*> m_PlayerQueue;

	// 个人房屋副本物品容器
	PHGoodsMap m_PHGoodsMap;

	// 分线场景数据
	map<long/*分线ID*/, vector<long/*templateID*/>> m_mapLinedRgnIdByLinedId;

	map<long/*TemplateID*/, tagDupRgnData*> m_mapDupRgnData;
};

extern CRgnManager g_RgnManager;

#endif//RGN_MANAGER_H