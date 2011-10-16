#pragma once

#include "Area.h"
#include "Region.h"


class CShape;
class CPlayer;
class CMonster;
class CPet;			// Fox@20081121 宠物
class CBuild;
class CGate;
class CCollection;
class CArea;
class CMessage;
class CMoveShape;
class CGoods;
class CNpc;
class VariableList;
class CColony;

typedef enum eRegionWarType
{
	RWT_Normal=0,	//普通场景
	RWT_NormalWar,	//普通战场
	RWT_VillageWar,	//村战
	RWT_CityWar		//城战
}eRWT;

//场景空间类型
typedef enum eRegiogSpaceType
{
	RST_Land=0,	//陆地
	RST_Warter,	//水下
	RST_Sky,	//天空
}eRST;

//当前城市的状态
enum eCityState
{
	CIS_NO,					
	CIS_DUTH,			//宣战状态
	CIS_Mass,			//集结状态
	CIS_Fight,			//该城进入战斗状态
};
//对城门的操作类型
enum eOperCityGate
{
	OC_Open=0,
	OC_Close,
	OC_Refresh,
};

//税
struct tagRegionParam
{
	long  lID;							//场景的ID
	long  lMaxTaxRate;					//最大税率
	long  lCurrentTaxRate;				//当前税率
	long  lYuanbaoTaxRate;				//元宝税率
	long  lBusinessTaxRate;				//跑商税率
	DWORD dwTotalTax;					//税金总合
	DWORD dwBusinessTotalTax;			//跑商税金总和
	DWORD dwTodayTotalTax;				//当天税金累计值
	long  lSupRegionID;					//供奉税金的场景ID(0表示不上缴税金)
	long  lTurnInTaxRate;				//上缴到其他场景的税率
	CGUID  OwnedFactionID;				//拥有这个场景的帮会
	CGUID  OwnedUnionID;				//拥有这个场景的同盟ID 
};

class CServerRegion :
	public CRegion
{

public:
	CServerRegion(void);
	CServerRegion(long id);
	CServerRegion(const CGUID& guid);
	CServerRegion(void* pParam);
	CServerRegion(const CGUID& guid, bool bNoPk);
	virtual ~CServerRegion(void);
private:
	hash_map<CGUID, CShape*, hash_guid_compare>		m_mOtherShapes;
	hash_map<CGUID, CCollection*, hash_guid_compare> m_mCollection;
	hash_map<CGUID, CBuild*, hash_guid_compare> m_mBuild;
	hash_map<CGUID, CMonster*, hash_guid_compare>	m_mMonsters;
	hash_map<CGUID, CPet*, hash_guid_compare>			m_Pets;		// 宠物
	hash_map<CGUID, CGoods*, hash_guid_compare>		m_mGoods;

	typedef hash_map<CGUID, CGoods*, hash_guid_compare>::iterator goodIt;
	hashsetGUID		m_setPlayers;
	map<DWORD,CColony*>		m_mapColony;
public:
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var);
private:
	void AddRebornTimerID(CGUID guid,long lTimerID);
	void DelReboreTimerID(CGUID guid);
	typedef map<CGUID,long>		mapGuid2Long;
	typedef map<CGUID,long>::iterator itGuid2Long;
	mapGuid2Long m_mReBornTimerIDList;

	map<string, tagPty>		m_mapNameValue;		// 变量名字对应的变量地址

	void InitNameValueMap();	// 初始化
public:
	size_t GetPlayerAmout() { return m_setPlayers.size(); }

	long GetCampPlayerNum(eCampType type);

	long GetID(void) { return m_ID; }
	void SetID(long id) { m_ID = id; }

	bool  SetAttribute(char* attrName, long value);
    long  GetAttribute(char* attrName);
    bool  ChangeAttribute(char* attrName, long value);
private:
	long        m_ID;
	BYTE		m_bNoPk;		// 场景是否允许PK
	bool		m_bNoContribute;
	DWORD       m_dwAreaId;
	//是否允许坐骑
	bool	m_Ride;
	//是否可以改变装备
	bool	m_ChangeEquip;
	//场景的空间类型
	eRegiogSpaceType    m_eSpaceType;
	eRegionWarType		m_WarRegionType;			//战斗场景类型
	//原地复活标志符
	bool	m_bRelive;
	////下线回城标志符
	bool    m_bDeadReturn;

	
public:
	BYTE		IsNoPk()		{ return m_bNoPk; }
	bool		IsNoContribute(){ return m_bNoContribute; }
	void		SetAreaId(DWORD dwId){m_dwAreaId=dwId;}
	DWORD		GetAreaId(){return m_dwAreaId;}

//////////////////////////////////////////////////////////////////////////
//	Region 变量设置
//////////////////////////////////////////////////////////////////////////
public:
	VariableList* GetVariableList(void);
	void SetVariableList(VariableList* vl);
	void UpdateVariableList(VariableList* vl);
private:
	VariableList* m_pVariableList;
//////////////////////////////////////////////////////////////////////////
//	地图Monster设置
//////////////////////////////////////////////////////////////////////////
public:

	// 刷新的怪物几率
	struct tagMonsterList
	{
		WORD wOdds;				// 刷新几率(n%)
		WORD wSign;				// 该类Monster唯一标识
		WORD wLeaderSign;		// 该类Monster领导标识(注:作为领导类Monster的数量应该只有一个)
		WORD wLeaderDistance;	// 和领导者保持的最大距离
		WORD wRoadIndex;		// 路径标识
		string strName;			// 名字
		string strScript;		// 脚本
		string strTalkScript;	// 对话脚本
	};

	// Monster
	struct tagMonster
	{
		long lIndex;			// 编号
		RECT rectRange;			// 范围(格子)
		long lNum;				// 数量
		long lResetTime;		// 刷新周期(秒)
		long lStartTime;		// 刷新其始时间
		long lDir;				// 方向(-1=随机)
		vector<tagMonsterList> vectorMonsterList;
		DWORD dwLastResetTime;	// 上次刷新时间
		long lLivingCount;		// 该刷新区域内活着的数量

	};


protected:
	map<DWORD,tagMonster>		m_mpMonster;


public:
	BOOL AddMonsterRect(tagMonster&, long lNum, bool bGMCreate=false);		// 添加一个刷新区域的Monsters
	CMonster*	AddMonster(const char* strOrginName, long x, long y, long lDir, bool bGMCreate=false);		// 添加一个Monster
	BOOL		AddMonster(const char* strOrginName, long count, long left, long right, long top, long bottom);
	CMonster*	AddMonster(long lGID, long x, long y, long lDir, bool bGMCreate=false);		// 添加一个Monster
	//!			删除原始名为pMonsterName的怪物，返回删除数量
	long		DelMonster(const char *pMonsterName);
	
	tagMonster* GetMonsterRefeash(long lIndex);		// 获取刷新怪物的结构指针
	//void SetMonsterLeader();
	//void FreeMonsterLockPlayer(CPlayer* pPlayer);	// 释放该地图上monster对pPlayer的锁定
	void RefeashMonster();							// 刷新地图上的Monster
	
private:
	CGate* GetOneGate();
	CMonster* GetOneMonster();
	void DelMonster(CMonster* pMonster);
	
	void DelCollection(CCollection* pCollection);
	//从场景中删除一个死亡的monster，并为此monster 注册一个复活事件
	//void DeleteOneMonster(CMonster*pMonster);
	//在场景中添加一个monster
public:
	BOOL AddOneMonster(CMonster*pMonster);

//////////////////////////////////////////////////////////////////////////
//	Colony
//////////////////////////////////////////////////////////////////////////
public:
	CColony* GetOneColony(DWORD dwColonyIndex);
	void	ReleaseAllColony();

//////////////////////////////////////////////////////////////////////////
//	Collection
//////////////////////////////////////////////////////////////////////////
public:
	struct tagCollectionList
	{
		char cName[50];
		LONG lOdds;
	};

	struct tagCollection 
	{
		long lIndex;			// 编号
		RECT rectRange;			// 范围(格子)
		long lNum;				// 数量
		long lResetTime;		// 刷新周期(秒)
		long lStartTime;		// 刷新其始时间
		long lDir;				// 方向(-1=随机)
		long lLivingCount;		// 该刷新区域内采集物的数量

		vector<tagCollectionList> vectorCollectionList;

	};
protected:
	list<tagCollection> m_listCollection;
public:
	BOOL AddCollectionRect(tagCollection&, long lNum);		// 添加一个刷新区域的Collection
	BOOL		AddCollection(const char* strOrginName, long count, long left, long right, long top, long bottom);
	BOOL			AddCollection_i (CCollection*collection ,long x,long y,long lDir);
	CCollection* AddCollection(const char* strOrginName,long x, long y, long lDir);

	BOOL RefreshCollection(CCollection* pCollection);
	tagCollection* GetCollectionRefeash(long lIndex);
protected:
	
	BOOL DeleteOneCollection(CCollection*);
	CCollection* GetOneCollection();
///////////////////////////////////////////////////////////
//          地图  door 设置 
///////////////////////////////////////////////////////////
public:
	struct tagGateProperty
	{
		long	lIndex;			//编号
		long	lxPos;			//x坐标
		long	lyPos;			//y坐标
		long	lDir;			//方向
		WORD	wOpenState;		//默认打开状态
		string	strOrigName;	//原始名
		string	strAttack;		//受攻击脚本
		string	strOpen;		//打开脚本
		string	strClose;		//关闭脚本
		string	strDestory;		//损毁脚本
		string	strRefresh;		//重刷脚本
	};
	//添加一个门
	CGate*		AddGate(tagGateProperty *pGateProperty);
	long		GetDoorState(long id);
	bool		SetDoorState(long id,WORD n);
	int			GetDoorDestroySwitch(long id);
	bool		SetDoorDestroySwitch(long id,bool n);
	long		GetDoorMaxHP(long id);
	bool		SetDoorMaxHP(long id,WORD n);
	long		GetDoorHP(long id);
	bool		SetDoorHP(long id,WORD n);
protected:
	map<long,tagGateProperty>m_mapGate;


//////////////////////////////////////////////////////////////////////////
//	NpcnameList
//////////////////////////////////////////////////////////////////////////
public:
	vector<BYTE>& GetNpcNameList(){return m_vectorNpcNameList;}			// 添加NPC名字列表
	long GetNpcNameListNum();						// 获取NPC数量

private:
	vector<BYTE>		m_vectorNpcNameList;
	long				m_lNpcNameListNum;

	///////////////////////////////////////////////////////////
	//          地图  据点 设置 
	///////////////////////////////////////////////////////////
public:
	struct tagStrongPoint
	{
		tagStrongPoint(void)
			:lPosX(0),lPosY(0),lState(0)
		{
			memset(szName, 0, 64);
		}
		~tagStrongPoint(void)
		{

		}

		long lPosX; // 据点X坐标
		long lPosY; // 据点Y坐标
		char szName[64]; // 据点名字
		long lState; // 据点状态
	};
	// 取得据点是否开启 false:关闭 true：开启
	bool GetStrongPointFlag(void) { return m_bStrongPintFlag; }
	// 设置据点是否开启
	void SetStrongPointFlag(bool flag) { m_bStrongPintFlag = flag; }
	// 取得据点个数
	long GetStronPointNum(void) { return (long)m_vecStrongPoint.size(); }
	// 取得据点状态值
	long GetStrongPointState(const char* szName);
	// 设置据点状态值
	void SetStrongPointState(const char* szName, long lState);
	// 取得据点坐标
	void GetStrongPointPos(const char* szName, long& lX, long& lY);
	// 取得据点数据
	std::vector<tagStrongPoint>& GetStrongPointVec(void) { return m_vecStrongPoint; }
private:
	bool m_bStrongPintFlag;
	std::vector<tagStrongPoint> m_vecStrongPoint;

//////////////////////////////////////////////////////////////////////////
//	地图设置
//////////////////////////////////////////////////////////////////////////
public:
	struct tagRegionSetup
	{
		tagRegionSetup()
		{
			// 初始化该数据结构
			memset(this, 0, sizeof(tagRegionSetup));
		}
		bool	bReLiveIsUsed;	 // 是否使用
		bool	bReLivePointFlag;//复活点类型（type） 0：原地复活 1：指定地图复活
		long	lReLiveRegionID; //复活点地图ID
		long	lReLivePosX1;	 //复活点坐标范围
		long	lReLivePosX2;
		long	lReLivePosY1;
		long	lReLivePosY2;

		bool	bLogoutIsUsed;	 // 是否使用
		long	lLogoutRegionID; //下线回城点地图ID
		long	lLogoutPosX1;	 //下线回城点坐标范围
		long	lLogoutPosX2;
		long	lLogoutPosY1;
		long	lLogoutPosY2;

		bool	bRuneStoneIsUsed;	 // 是否使用
		long	lRuneStoneRegionID;	//符文石回城点地图ID
		long	lRuneStonePosX1;	//符文石回城点坐标范围
		long	lRuneStonePosX2;
		long	lRuneStonePosY1;
		long	lRuneStonePosY2;
		bool	bRuneStoneCanRecord; // 符文石回城点是否可以记录

		bool	bDupRgnIsUsed;	 // 是否使用
		long	lDupRgnRegionID;//副本回城点地图ID
		long	lDupRgnPosX1;	//副本回城点坐标范围
		long	lDupRgnPosX2;
		long	lDupRgnPosY1;
		long	lDupRgnPosY2;

		bool	bReLiveCurPosFlag;	//原地复活开关 1//1：开 0：关
		bool	bForbiddenFlag;		//禁制复活开关 1//1：开 0：关

		bool	bWarningFlag;	// 是否报警
	};

	tagRegionSetup& GetSetup(void) { return m_stSetup; }
protected:
	tagRegionSetup m_stSetup;
	set<string>	m_ForbidMakeGoods;									//在该城景中禁止使用的物品
public:
	tagRegionSetup* GetRegionSetup(){return &m_stSetup;}
	bool FindForbidGood(const char* strOriName);					//查找该物品是否是禁止物品

	virtual void GetReLiveReturnPoint(CPlayer* pPlayer, long &lRegion, long flag/*是那一种复活方式*/, long &l, long &t, long &r, long &b, long &d);
	virtual void GetLogoutReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);
	virtual void GetRuneStoneReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);
	virtual void GetDupRgnReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);

	virtual BOOL DoesRecallWhenLost(CPlayer* pPlayer);
	virtual bool BackToCity(CPlayer* pPlayer);						// 让player回城

	//处理Player进入场景
	void PlayerEnter(CPlayer* pPlayer,long lUseGoods);
	//处理Player退出场景
	void PlayerExit(CPlayer* pPlayer);

//////////////////////////////////////////////////////////
//	路径
//////////////////////////////////////////////////////////
protected:
	vector<vector<POINT>> m_vecRoadsList;

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual	void Run()		{CRegion::Run();};
	virtual BOOL Load();	// 读取
	virtual BOOL Save();	// 存储
	virtual BOOL New();		// 新建

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	bool CodeToDataBlock(DBWriteSet& setWriteDB);		// 添加到CByteArray
	bool		 AddSetupToByteArray(vector<BYTE>* pByteArray, bool bExData);
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecordSetupFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码Setup
	virtual bool AddRegionParamToByteArray(vector<BYTE>* pByteArray, bool bExData = true);//添加参数到ByteArray

	//void FreeMonsterLockedPlayer(CPlayer* pPlayer);	// 释放该地图上NPC对pPlayer的锁定

/////////////////////////////////////////////////////////////////////
// 消息响应	函数实现在 RegionMessage.cpp 中
/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage*);

/////////////////////////////////////////////////////////////////////
// 场景管理
/////////////////////////////////////////////////////////////////////
public:
	virtual void ReBornObject(CBaseObject* pObject);			// 使OBJECT重新刷出
	virtual void AddObject(CBaseObject* pObject);				// 添加OBJECT
	virtual void RemoveObject(CBaseObject* pObject);			// 移出OBJECT

	virtual void DeleteChildObject(long typeID, const CGUID& guid);		// 删除子OBJECT
	virtual void DeleteChildObject(CBaseObject* p);				// 删除子OBJECT

	virtual void DeleteAllChildObject(CBaseObject* pExpect=NULL);// 删除所有子OBJECT(pExpect = 除了)

	//virtual CBaseObject* RecursiveFindObject(long typeID,long lID);		// 递归查找
	//virtual CBaseObject* RecursiveFindObject(long typeID,char* strName);// 按名字查找
	virtual CBaseObject* FindChildObject(long typeID,const CGUID& guid);	// 查找子object!
	virtual bool FindChildObject(CBaseObject* p);				// 查找子object!
	CBaseObject* FindChildObjectByName(long typeID, const char* strName);	// 按名字查找

	CBaseObject* FindAroundObject(CShape* pShape, long type, const CGUID& guid);	// 查找pShape周围的object
	CBaseObject* FindAroundObject(CShape* pShape, long type);
	CBaseObject* FindNearestAroundObject(CShape* pMoveShape,long type);	//查找pShape周围的object
	CBaseObject* FindNearestObjectInArea(CShape* pMoveShape,long type,long x,long y,WORD& wDistance);
	void		 FindAroundObject(CShape* pShape, long type, vector<CShape*>& vOut);
	void		 FindAllPlayer(hashsetGUID& setOut)	{setOut=m_setPlayers;return;}
	void		 FindAroundPlayer(CShape* pShape, vector<CPlayer*>& vOut);

	//在一个范围里找寻指定数量最近的玩家
	void		 FindLimitPlayersAround(CMoveShape* pMoveShape,long lMaxNum,
										hashsetGUID& vOut,const CGUID& guidExceptID);
	//在周围找出指定距离的玩家
	void		 FindNearestPlayer(long lX,long lY,long lDis,vector<CGUID>& vOut);
	//void		 FindAroundPets(CShape* pShape, vector<CMonster*>& vOut);

	CMonster*	FindMonsterByID(const CGUID& guid);
	/// 在场景上根据宠物GUID查找宠物
	CPet* FindPetByID(const CGUID& guid);

	virtual void	AddGuardIndex(long lIndex){}


	void CreateAreaArray();		// 创建Area数组
	CArea* GetArea(long x, long y);
	CArea* GetArea(long num)	{return &m_pArea[num];}
	long GetAreaX()	{return m_lAreaX;}
	long GetAreaY() {return m_lAreaY;}
	void OnShapeChangeArea(CShape* pShape,long lNextAreaX,long lNextAreaY);	// 处理shape改变area
	

	CShape* GetShape(long x, long y);				// 获取一个格子上是否有Shape
	void	GetShape( LONG lX, LONG lY, vector<CShape*>& vOut );
	long GetDropGoodsPos(long sx, long sy, long& dx, long& dy, long lStartPos);
	virtual	void OnSymbolDestroy(const CGUID& SymID,const CGUID& PlayerID,const CGUID& FacID,const CGUID& UnionID){}											//当标志物摧毁时 
	virtual bool OperatorCityGate(long lCityGateID,eOperCityGate eOp){return false;}		// 对城门进行操作
	virtual void UpdateCityGateToClient(long lCityGateID) {return;}
	virtual bool OperatorSymbol(long lSymbolID,eOperCityGate eOp){return false;}			// 对标志物进行操作 
	virtual void OnWarDeclare(long lWarNum)		{m_CityState=CIS_DUTH;m_lWarNum=lWarNum;}	//开始宣战
	virtual void OnWarStart(long lWarNum)		{m_CityState=CIS_Fight;}					//攻城开始
	virtual void OnWarTimeOut(long lWarNum){}												//攻城时间结束
	virtual void OnWarEnd(long lWarNum)			{m_CityState=CIS_NO;m_lWarNum=0;}			//攻城结束
	virtual void OnWarMass(long lWarNum)		{m_CityState=CIS_Mass;}	
	virtual void OnClearOtherPlayer(long lWarNum){}											//清除其它人员
	virtual void OnRefreshRegion(long lWarNum);										//刷新场景
	virtual bool SymbolIsAttackAble(CMoveShape* pAttackShape){return true;	}
	virtual bool GuardIsAttackAble(CMoveShape* pAttackShape){return true;	}
	virtual void AddGurdMonster(const CGUID& guid){}
	virtual void DelGurdMonster(const CGUID& guid){}


	void SendMsgToAroundShape(CShape* pMoveShape,CMessage* pMsg);
protected:
	long m_lAreaX;									// 场景上的区域X
	long m_lAreaY;									// 场景上的区域Y
	CArea* m_pArea;									// 场景上的区域

	// 废弃了。用vector代替，理由问雷雨，bugs注释
	//std::map<long, CPlayer*>	m_mapPlayer;		// 场景上面存在的玩家

//////////////////////////////////////////////////////////////////////////
//	SHAPE位置变化
//////////////////////////////////////////////////////////////////////////
protected:
	list<CShape*>	m_listRemoveShape;			// 需要从场景移出的Shape列表
	list<CShape*>	m_listDeleteShape;			// 需要删除的shape
	list<CShape*>	m_listChangeAreaShape;		// 需要改变area的Shape列表
	list<CShape*>	m_listChangeRegionShape;	// 需要改变region的Shape列表
public:
	list<CShape*>* GetRemoveShapeList()			{return &m_listRemoveShape;}
	list<CShape*>* GetDeleteShapeList()			{return &m_listDeleteShape;}
	list<CShape*>* GetChangeAreaShapeList()		{return &m_listChangeAreaShape;}
	list<CShape*>* GetChangeRegionShapeList()	{return &m_listChangeRegionShape;}

//////////////////////////////////////////////////////////////////////////
//	天气效果
//////////////////////////////////////////////////////////////////////////
public:
	struct tagWeatherTime
	{
		struct tagOption
		{
			struct tagWeather
			{
				long	lWeatherIndex;				// 天气效果编号
				DWORD	dwFogColor;					// 雾的颜色

				tagWeather()
				{
					lWeatherIndex = 0;
					dwFogColor = 0;
				}
			};

			long	lOdds;							// 几率(% 0-100)
			vector<tagWeather>		vectorWeather;	// 天气效果
		};

		long	lTime;								// 持续时间（单位：分）
		vector<tagOption>		vectorOption;		// 侯选天气
	};

	BOOL LoadWeatherSetup();
	void SendWeatherInfo(CPlayer *pPlayer=NULL);	// 如果不指定具体玩家，那么发送给整个场景的人
	void ChangeWeather( long lId );					// 改变天气
public:
	void RegisterWeatherTimer();
	void UnRegisterWeatherTimer();
	void OnChangeWeather();
private:
	//场景天气定时器
	long m_lWeatherTimerID;

protected:
	vector<tagWeatherTime>	m_vectorWeatherSetup;	// 天气设置
	long m_lCurWeatherTimeSegment;					// 当前时段(0-m_vectorWeatherSetup.size())
	long m_lCurWeatherCount;						// 记数

	vector<tagWeatherTime::tagOption::tagWeather>	m_vectorCurWeather;	// 当前天气

public:
	//##物品保护相关
	void SetGoodsProtection( CPlayer* pPlayer, CGoods* pGoods ,BOOL bTeamProtected=TRUE);
	eGoodsPickValue CanPickUpGoods( CPlayer* pPlayer, CGoods* pGoods );

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
public:
	void RefeashBlock();		// 刷新地图阻挡

///税收系统
private:
	tagRegionParam		m_Param;				//税金相关参数
	long				m_lWarNum;				//战斗的标示
	eCityState			m_CityState;			//状态

	
protected:
	bool	m_bKickOutPlayer;		//是否清除这个场景上的玩家
	long	m_lKickOutPlayerTime;	//清除该场景上的玩家倒计时
	DWORD	m_dwLastTime;			//上一次计时时间

	void	ClearPlayerAI();
public:
	void	StartClearPlayerOut(long lCountDownTime);
	virtual void SetEnterPosXY(CPlayer* pPlayer){}

	eRST GetSpaceType(){return m_eSpaceType;}
	void SetSpaceType(eRST eType){m_eSpaceType=eType;}

	eRWT GetWarType(){return m_WarRegionType;}
	void SetWarType(eRWT eType){m_WarRegionType=eType;}

	bool IsRide()	{return m_Ride;}
	void SetRide(bool b) {m_Ride=b;}

	bool GetRelive()				{return m_bRelive;}
	bool GetDeadReturn()			{return m_bDeadReturn;}

	



	bool IsChangeEquip()	{return m_ChangeEquip;}
	void SetChangeEquip(bool b) {m_ChangeEquip;}

	void SetRegionParam(tagRegionParam param)	{m_Param=param;}
	tagRegionParam* GetRegionParam(){return &m_Param;}

	virtual void SetOwnedCityOrg(const CGUID& FactionGuid,const CGUID& UnionGuid)
	{m_Param.OwnedFactionID = FactionGuid;m_Param.OwnedUnionID = UnionGuid;}
	virtual const CGUID& GetOwnedCityFaction()	{return m_Param.OwnedFactionID;}
	virtual const CGUID& GetOwnedCityUnion()	{return m_Param.OwnedUnionID;}

	virtual long GetTaxRate(){return m_Param.lCurrentTaxRate;	}			//得到税率
	virtual long GetMaxTaxRate(){return m_Param.lMaxTaxRate;	}			//得到最大税率
	virtual void SetTaxRateAndUpdate(long lTaxRate);
	virtual void SetTaxRate(long lTaxRate)	{m_Param.lCurrentTaxRate = lTaxRate;}
	virtual void AddTaxMoney(long lMoney);//加税钱
	virtual void CollectTodayTax();

	long GetBusinessTaxRate(){return m_Param.lBusinessTaxRate;}//跑商税率
	void SetBusinessTaxRate(long lRate){m_Param.lBusinessTaxRate=lRate;}
	long GetYuanbaoTaxRate(){return m_Param.lYuanbaoTaxRate;}//元宝税率
	void SetYuanbaoTaxRate(long lRate){m_Param.lYuanbaoTaxRate=lRate;}
	DWORD GetNormalTotalTax(){return m_Param.dwTotalTax;}
	DWORD GetBusinessTotalTax(){return m_Param.dwBusinessTotalTax;}
	DWORD GetTotalTax(){return m_Param.dwTotalTax+m_Param.dwBusinessTotalTax;}
	void AddBusinessTaxMoney(long lMoney);//加跑商税金

	DWORD GetTotalTaxPayment()			{return m_Param.dwTotalTax;	}
	void  SetTotalTaxPayment(DWORD lMoney)	{m_Param.dwTotalTax=lMoney;	}

	DWORD GetTodayTaxPayment()			{return m_Param.dwTodayTotalTax;	}
	void  SetTodayTaxPayment(DWORD lMoney)	{m_Param.dwTodayTotalTax=lMoney;	}

	virtual void ObtainTaxPayment(CPlayer* pPlayer);									//获取税金
	virtual void AdjustTaxRate(CPlayer* pPlayer);										//调整税率

	void UpdateTaxToWorldServer();

	virtual void SetWarNum(long lNum)	{m_lWarNum = lNum;}			//设置战斗的标示符号
	virtual long GetWarNum()			{return m_lWarNum;}

	virtual eCityState	GetCityState()		{return m_CityState;}
	virtual void SetCityState(eCityState eState){m_CityState = eState;}
	virtual void ReSetWarState(long lWarID,eCityState eState)	{m_lWarNum = lWarID; m_CityState=eState;} 

	virtual bool IsApplyWarFacsMem(const CGUID& FactionGuid){ return false;}
	virtual void AddContend(CPlayer* pPlayer,long lID,const char* chName,long lTime){}	//添加一个争夺
	virtual bool CancelContendByPlayerID(CPlayer* pPlayer){return false;}				//取消某个玩家的争夺
	virtual void OnPlayerDamage(CPlayer* pPlayer,long lDamHp){}							//当玩家攻击受到伤害
	virtual void DecContendTime(CPlayer* pPlayer,long lDecTime){}						//减少时间
	virtual long GetDiedStateTime()	{return 0;}
	virtual void UpdateContendPlayer(){}												//根据申请的帮会更新争夺玩家
	
	void	KickOutAllPlayerToReturnPoint();											//剔出所有玩家到复活点
	void	KickOutAllPlayerToReturnPointExceptOwner();									//剔出所有玩家到复活点除开本城的拥有者
	void	DelOneGoodFromAllPlayer(char* strGoodName);

	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void) { return m_OwnerGUID; }

	// 副本人数上限
	long GetDupRgnPlayerNumMax(void) { return m_DupRgnPlayerNumMax; };
	void SetDupRgnPlayerNumMax(long value) { m_DupRgnPlayerNumMax = value; };
	// 当前副本人数
	//long GetCurDupRgnPlayerNum(void) { return m_CurDupRgnPlayerNum; };
	//void SetCurDupRgnPlayerNum(long value) { m_CurDupRgnPlayerNum = value; };

	// 管理权开放（数值，1／0）
	bool GetOperFlag(void) { return b_OperFlag; }
	void SetOperFlag(bool flag) { b_OperFlag = flag; }
	// 删除时间
	long GetDelTime(void) { return m_DelTime; }
	void SetDelTime(long value) { m_DelTime = value; }
	// 维持费用
	long GetCostValue(void) { return m_CostValue; }
	void SetCostValue(long value) { m_CostValue = value; }

	// 设置定时器
	void SetRgnDelTimerID(long id) { m_lRgnDelTimerID = id; }
	// 取得定时器
	long GetRgnDelTimerID(void) { return m_lRgnDelTimerID; }

	//!	注册一个删除副本定时器，并返回定时器ID
	long RegisterDeleteRgnTimerID(long lFutureTime);
	//!	取消一个删除副本的Timer
	void CancelDeleteRgnTimer(void);
	//!	响应删除副本定时器触发的事件
	void OnDeleteRgnTimeout(long lTimerID);
	//! 被删除
	void OnDeleteObject(CShape* pShape);
	//! 被移出
	void OnRemoveObject(CShape*pShape,LONG lTime);
	//! 被移出一段时间
	void OnRemoveSomeTime(CShape* pShape,LONG lTime);
	// 是否被强制删除
	bool GetForceDelFlag(void)		{ return b_IsForceDelFlag; }
	void SetForceDelFlag(bool flag) { b_IsForceDelFlag = flag; }

	// 设置上次报警时间
	void SetLastWarningTime(DWORD lTime) { m_dwLastWarningTime = lTime; }
	DWORD GetLastWarningTime(void) { return m_dwLastWarningTime; }
private:

	DWORD		m_dwLastWarningTime;		//上次警告时间

	//! 响应重生Object定时器触发事件
	void OnRebornObject(LONG lTimerID,stDestObj* pDestObj);
public:
	void RunScriptAllPlayer(char* strPath);

private:
	CGUID m_OwnerGUID;
	// 副本人数上限
	long m_DupRgnPlayerNumMax;
	// 当前副本人数
	//long m_CurDupRgnPlayerNum;
	// 管理权开放（数值，1／0）
	bool b_OperFlag;
	// 删除时间
	long m_DelTime;
	// 维持费用
	long m_CostValue;
	// 副本删除定时器ID
	long m_lRgnDelTimerID;
	// 是否被强制删除
	bool b_IsForceDelFlag;
	
public:
	void WakeUp();
	void Hibernate(){m_bIsHibernate=TRUE;}
	BOOL IsHibernate(){return m_bIsHibernate;}
private:
	BOOL m_bIsHibernate;

private:
	//实现向客户端添加对象的缓冲机制
	
	//添加Shape对象到客户端的结构
	struct tagAddShapeToClient
	{
		//添加者的类型
		long lType;
		//发送时间
		DWORD dwSendTime;
		//被添加的客户端玩家列表
		hash_set<CGUID,hash_guid_compare>	Players;
	};

	typedef hash_set<CGUID,hash_guid_compare>	SendPlayers;
	typedef hash_set<CGUID,hash_guid_compare>::iterator itSendPlayer;

	typedef hash_map<CGUID,tagAddShapeToClient,hash_guid_compare>		AddShapeList;
	typedef hash_map<CGUID,tagAddShapeToClient,hash_guid_compare>::iterator itAddShape;
	//添加Shape的对象列表
	AddShapeList	m_AddShapes;
	long m_lAddShapeTimerID;
public:
	void RegisterAddShapeTimer();
	void AddShapeToAround(CShape* pShape);
	void AddShapeToArea(CArea* pArea,CShape* pShape);
	void AddShapeToPlayer(CShape* pShape,const CGUID& PlayerID);
	void DelShapeToAround(CShape* pShape);
	void DelShapeToArea(CArea* pArea,CShape* pShape);
	void DelShapeToPlayer(CShape* pShape,const CGUID& PlayerID); 
	void SendAddShapeMsgToClient();
	void SendAddShapeMsgToClient(CShape* pShape,hashsetGUID& guidPlayers);
	//定时器触发向客户端添加消息
	long OnAddShapeTimeOut(DWORD timerid);
	//得到对象和集合中的玩家距离小于等于5格的集合
	void GetNearestShapes(CShape* pShape,hashsetGUID& InPlayers,hashsetGUID& OutPlayers);
};
