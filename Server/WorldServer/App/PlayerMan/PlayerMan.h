// Fox@20081104----------------------------------------------
// File:    PlayerMan.h
// Brief:   CPlayerMan用于管理玩家位置, 性别, 等级, 国籍等
//			关系映射, 目前主要用于在线玩家搜索功能.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Nov.04, 2008
// Fox@20081104----------------------------------------------

#pragma once

class CPlayer;
class CMessage;

//#define PutoutPMLog(strLog) \
//PutoutLog("PM", LT_ERROR, strLog); \
//AppFrame::AddStdLog(strLog)

// Fox@20081107----------------------------------------------
// 自定义结构
enum PLAYER_ELEM_TYPE					// 玩家信息类型
{
	PET_SEX			= 0x00000001,
	PET_LEVEL		= 0x00000002,
	PET_COUNTRY		= 0x00000004,
	PET_REGION		= 0x00000008,
	PET_AREA		= 0x00000010,
	PET_OCCUPATION	= 0x00000020,
	PEI_CONST		= 0x00000040,
	PET_HIDENAME	= 0x00000080,
	PET_NAME		= 0x10000040,
	PET_FACTION		= 0x10000080,
	PET_UNION		= 0x10000100,
	PET_STR_DESC	= 0x10000000		// 字符串描述的标志
};

enum PET_SEX_TYPE						// 性别类型
{
	PST_MALE		= 0x00000000,
	PST_FEMALE		= 0x00000001,
	PST_SIG_FEMALE	= 0x10000001
};

enum PET_REGION_TYPE					// 场景类型
{
	PRT_NULL		= 0xFFFFFFFF,
	PRT_CUR_REGION	= 0x00000001,
	PRT_CUR_AREA	= 0x00000002
};

struct tagPosition						// 位置信息
{
	tagPosition()
		: lPosX(0)
		, lPosY(0)
	{
	}

	long			lPosX;
	long			lPosY;
};

struct tagPlayerInfo					// 玩家信息
{
	tagPlayerInfo()
		: ExID(NULL_GUID)
		, strName("")
		, strFaction("")
		, strUnion("")
		, lOccupation(0)
		, lSex(-1)
		, lLevel(0)
		, lCountryID(-1)
		, lConst( -1 )
		, lRegionID(-1)
		, lAreaID(-1)
		, btHideName(0)
	{
	}

	tagPlayerInfo& operator=(tagPlayerInfo& info)
	{
		if( this == &info )
			return *this;

		ExID		= info.ExID;
		strName		= info.strName;
		strFaction	= info.strFaction;
		strUnion	= info.strUnion;
		lOccupation	= info.lOccupation;
		lSex		= info.lSex;
		lLevel		= info.lLevel;
		lCountryID	= info.lCountryID;
		lConst		= info.lConst;
		lRegionID	= info.lRegionID;
		lAreaID		= info.lAreaID;
		btHideName	= info.btHideName;
		Position.lPosX	= info.Position.lPosX;
		Position.lPosY	= info.Position.lPosY;

		return *this;
	}

	CGUID			ExID;		// 玩家ID
	string			strName;
	string			strFaction;
	string			strUnion;
	long			lOccupation;
	long			lSex;
	long			lLevel;
	long			lCountryID;
	long			lConst;
	long			lRegionID;
	long			lAreaID;
	BYTE			btHideName;
	tagPosition		Position;
};
// Fox@20081107----------------------------------------------

class CPlayerMan
{
public:
	CPlayerMan(void);
	~CPlayerMan(void);

	// Fox@20081120 玩家管理编解码
public:
	void Initial(void);
	void Release(void);
	bool LoadConfig(void);
	bool CodeToDataBlock(DBWriteSet& setWriteDB);		// 编码
	bool DecodeFromDataBlock(DBReadSet& setReadDB);		// 解码

	// Fox@20081107----------------------------------------------
public:
	// 玩家管理, 供外部调用
	bool AddPlayer(CPlayer* pPlayer);		// 添加玩家
	bool DelPlayer(CPlayer* pPlayer);		// 释放玩家
	long RefreshElem(long eType,
		CPlayer* pPlayer);					// 修改玩家单项信息
	long RefreshElem(long eType,
		CPlayer* pPlayer,
		long lNewVal);						// 修改玩家单项信息
	bool RefreshElems(long eTypes,
		CPlayer* pPlayer);					// 修改玩家多项信息
	bool RefreshPlayer(CPlayer* pPlayer,
		bool bAll=false);					// 刷新玩家
	bool ChangeGS(long lNew,
		const CGUID& rGuid);				// GS切换
	bool Query(CMessage* pMsg);				// 客户端查询

	// Fox@20081107----------------------------------------------
protected:
	// 玩家管理, 仅供内部使用
	tagPlayerInfo* FindPlayer(const CGUID& rGuid);	// 查找玩家
	bool FillPlayerInfo(CPlayer* pPlayer,
		tagPlayerInfo& rPlayerInfo);		// 根据CPlayer对象填充PlayerInfo
	bool AddPlayerMap(tagPlayerInfo* pInfo);	// 根据CPlayer对象加入映射
	bool DelPlayerMap(tagPlayerInfo* pInfo,
		bool bComplete = false);			// 根据CPlayer对象清理映射
	tagPlayerInfo* AllocPlayer(void);		// 分配玩家
	void FreePlayer(tagPlayerInfo* pInfo);	// 回收玩家
	long CalcAreaID(tagPosition& rPos,
		long lRegionID);					// 计算AreaID
	long CalcLevel(long lLevel);			// 计算等级索引
	bool CheckCountry(long lID, long lIDs);	// 国家比较

	// Fox@20081107----------------------------------------------
protected:
	// 玩家集合
	typedef std::list<tagPlayerInfo*> SET_PLAYER;
	typedef SET_PLAYER::iterator ITR_SET_PLAYER;
	SET_PLAYER			m_PrePlayers;				// 预分配玩家集合

	typedef std::map<CGUID, tagPlayerInfo*> MAP_PLAYER;
	typedef MAP_PLAYER::iterator ITR_MAP_PLAYER;
	MAP_PLAYER			m_mapPlayers;

	// 关系映射
	typedef std::list<CGUID> LIST_GUID;
	typedef LIST_GUID::iterator ITR_LIST_GUID;
	typedef std::map<long, LIST_GUID > MAP_PLAYER_MAP;
	typedef MAP_PLAYER_MAP::iterator ITR_MAP_PLAYER_MAP;
	MAP_PLAYER_MAP		m_Region2Players;
	MAP_PLAYER_MAP		m_Area2Players;
	MAP_PLAYER_MAP		m_Sex2Players;
	MAP_PLAYER_MAP		m_Country2Players;
	MAP_PLAYER_MAP		m_Level2Players;
	MAP_PLAYER_MAP		m_Const2Players;

	// 配置
	long				m_lMaxPlayerNum;	// 单GS最大玩家数
	long				m_lAreaWidth;		// 区域边长
	long				m_lPageLength;		// 每页显示玩家数
	long				m_lLevelLength;		// 等级跨度
	long				m_lGSNum;			// GS数量
};