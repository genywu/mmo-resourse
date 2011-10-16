#pragma once
#include "Region.h"
class CPlayer;
class VariableList;

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

enum eCityState
{
	CIS_NO,					
	CIS_DUTH,			//宣战状态
	CIS_Mass,			//集结状态
	CIS_Fight,			//该城进入战斗状态
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
	void operator=(tagRegionParam& right)
	{
		if(this != &right)
		{
			lID = right.lID;							
			lMaxTaxRate = right.lMaxTaxRate;					
			lCurrentTaxRate = right.lCurrentTaxRate;				
			lYuanbaoTaxRate = right.lYuanbaoTaxRate;				
			lBusinessTaxRate = right.lBusinessTaxRate;				
			dwTotalTax = right.dwTotalTax;					
			dwBusinessTotalTax = right.dwBusinessTotalTax;			
			dwTodayTotalTax = right.dwTodayTotalTax;				
			lSupRegionID = right.lSupRegionID;					
			lTurnInTaxRate = right.lTurnInTaxRate;				
			 OwnedFactionID = right.OwnedFactionID;				
			 OwnedUnionID = right.OwnedUnionID;
		}
	}
};

class CWorldRegion :
	public CRegion
{
public:
	CWorldRegion(void);

	CWorldRegion(int type);
	virtual ~CWorldRegion(void);


	// gsid
	long GetGsid(void) { return m_Gsid; }
	void SetGsid(long value) { m_Gsid = value; }

	void		SetNoPk(BYTE b)			{ m_bNoPk = b; }
	void		SetNoContribute(bool b) { m_bNoContribute = b; }
	BYTE		IsNoPk()				{ m_bNoPk; }
	bool		IsNoContribute()		{ m_bNoContribute; }

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

	};

	BOOL LoadMonsterList();					// 读取地图的Monster分布
	tagMonster* GetMonster(long lIndex);	// 获取指定index的Monster分布

	DWORD GetMonsterNum () { return (DWORD)m_mpMonster.size(); }

	map<DWORD, tagMonster>& GetMonsterSetup(void) { return m_mpMonster; }
protected:
	map<DWORD,tagMonster>		m_mpMonster;
///////////////////////////////////////////////////////////
//          地图  collection 设置 
///////////////////////////////////////////////////////////
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
	map<long,tagCollection> m_mapCollection;
public:
	BOOL LoadCollectionList();					// 读取地图的Collection分布
	tagCollection* GetCollection(long lIndex);	// 获取指定index的Collection分布

	long GetColletionNum(void) { return (long)m_mapCollection.size(); }

	map<long,tagCollection>& GetCollectionMap(void) { return m_mapCollection; }
///////////////////////////////////////////////////////////
//          地图  door 设置 
///////////////////////////////////////////////////////////
public:
	struct tagGate
	{
		long	lIndex;			//编号
		long	lxPos;			//x坐标
		long	lyPos;			//y坐标
		long	lDir;			//方向
		int		nOpenState;		//默认打开状态
		string	strOrigName;	//原始名
		string	strAttack;		//受攻击脚本
		string	strOpen;		//打开脚本
		string	strClose;		//关闭脚本
		string	strDestory;		//损毁脚本
		string	strRefresh;		//重刷脚本

		void operator=(tagGate& right)
		{
			if(this != &right)
			{
				lIndex = right.lIndex;		
				lxPos = right.lxPos;		
				lyPos = right.lyPos;		
				lDir = right.lDir;		
				nOpenState = right.nOpenState;	
				strOrigName = right.strOrigName;
				strAttack = right.strAttack;	
				strOpen = right.strOpen;	
				strClose = right.strClose;	
				strDestory = right.strDestory;	
				strRefresh = right.strRefresh;
			}
		}
	};
protected:
	map<long,tagGate> m_mapGate;
public:
	BOOL LoadGateList();

	map<long,tagGate>& GetGateMap(void) { return m_mapGate; }

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
	// 取得据点信息
	std::vector<tagStrongPoint>& GetStrongPointVec(void) { return m_vecStrongPoint; }
private:
	bool m_bStrongPintFlag;
	std::vector<tagStrongPoint> m_vecStrongPoint;
//////////////////////////////////////////////////////////////////////////
//	地图设置
//////////////////////////////////////////////////////////////////////////
public:				//Region Setup
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

		void operator=(tagRegionSetup& right)
		{
			if(this != &right)
			{
				bReLiveIsUsed = right.bReLiveIsUsed;	
				bReLivePointFlag = right.bReLivePointFlag;
				lReLiveRegionID = right.lReLiveRegionID; 
				lReLivePosX1 = right.lReLivePosX1;
				lReLivePosX2 = right.lReLivePosX2;
				lReLivePosY1 = right.lReLivePosY1;
				lReLivePosY2 = right.lReLivePosY2;
				
				bLogoutIsUsed = right.bLogoutIsUsed;	
				lLogoutRegionID = right.lLogoutRegionID;
				lLogoutPosX1 = right.lLogoutPosX1;	 
				lLogoutPosX2 = right.lLogoutPosX2;
				lLogoutPosY1 = right.lLogoutPosY1;
				lLogoutPosY2 = right.lLogoutPosY2;
				
				bRuneStoneIsUsed = right.bRuneStoneIsUsed;	
				lRuneStoneRegionID = right.lRuneStoneRegionID;	
				lRuneStonePosX1 = right.lRuneStonePosX1;	
				lRuneStonePosX2 = right.lRuneStonePosX2;
				lRuneStonePosY1 = right.lRuneStonePosY1;
				lRuneStonePosY2 = right.lRuneStonePosY2;
				bRuneStoneCanRecord = right.bRuneStoneCanRecord;
				
				bDupRgnIsUsed = right.bDupRgnIsUsed;	
				lDupRgnRegionID = right.lDupRgnRegionID;
				lDupRgnPosX1 = right.lDupRgnPosX1;
				lDupRgnPosX2 = right.lDupRgnPosX2;
				lDupRgnPosY1 = right.lDupRgnPosY1;
				lDupRgnPosY2 = right.lDupRgnPosY2;

				bWarningFlag = right.bWarningFlag; // 是否报警
			}
		}
	};

	BOOL LoadSetup();
	tagRegionSetup& GetSetup(void) { return m_stSetup; }

	vector<string>& GetForbidMakeGoodsVec(void) { return m_ForbidMakeGoods; }

protected:
	tagRegionSetup m_stSetup;
	vector<string>	m_ForbidMakeGoods;	//在该城景中禁止使用的物品
	

//////////////////////////////////////////////////////////////////////////
//	地图NPC设置
//////////////////////////////////////////////////////////////////////////
public:

///////////////////////////////////////////////////////
//	地图路点设置
///////////////////////////////////////////////////////
public:
	BOOL	LoadRoads();
	vector<vector<POINT>>& GetRoadList(void) { return m_vecRoadsList; }
protected:
	vector<vector<POINT>> m_vecRoadsList;

public:
	BOOL	LoadRegionDrop();

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual	void Run()		{CRegion::Run();};
	virtual	void AI()		/*{CRegion::AI();}*/;

	virtual BOOL Load();	// 读取
	virtual BOOL New()		{return CRegion::New();};		// 新建

	virtual void SetEnterPosXY(CPlayer* pPlayer);



/////////////////////////////////////////////////////////////////////
// 存取接口
/////////////////////////////////////////////////////////////////////
public:
	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool AddToByteArrayForProxy(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecordRegionParamFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool AddSetupToByteArray(vector<BYTE>* pByteArray, bool bExData = true);// 添加Setup到CByteArray

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
	vector<tagWeatherTime>& GetWeatherTimeVec(void)	{ return m_vectorWeatherSetup; }
protected:
	vector<tagWeatherTime>	m_vectorWeatherSetup;	// 天气设置

	//税收相关功能
private:
	BYTE	m_bNoPk;
	bool	m_bNoContribute;
	//是否允许坐骑
	bool	m_Ride;
	//是否可以改变装备
	bool	m_ChangeEquip;
	//原地复活标志符
	bool	m_bRelive;
	//下线回城标志符，0表示非死亡下线就不回城，1表示死亡下线回城
    bool	m_bDeadReturn;
	//场景的空间类型
	eRegiogSpaceType    m_eSpaceType;
	eRegionWarType		m_WarRegionType;			//战斗场景类型
	tagRegionParam		m_Param;					//税金相关参数

	
	//friend class CRsRegion;

public:
	void	InitOwnerRelation();					//初始化场景的拥有者
	virtual void GetReLiveReturnPoint(CPlayer* pPlayer, long &lRegion, long flag/*是那一种复活方式*/, long &l, long &t, long &r, long &b, long &d);
	virtual void GetLogoutReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);
	virtual void GetRuneStoneReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);
	virtual void GetDupRgnReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);

	CWorldRegion* GenerateSaveData();

	BOOL LoadTaxParam();		//读取税收相关参数
	void SetRegionParam(tagRegionParam& RegionParam)	{m_Param = RegionParam;}
	tagRegionParam& GetRegionParam()						{return m_Param;}
	
	virtual void SetOwnedCityOrg(const CGUID& FactionGuid,const CGUID& UnionGuid)
	{m_Param.OwnedFactionID = FactionGuid;m_Param.OwnedUnionID = UnionGuid;}
	virtual const CGUID& GetOwnedCityFaction()		{return m_Param.OwnedFactionID;}
	virtual const CGUID& GetOwnedCityUnion()		{return m_Param.OwnedUnionID;}

	void SetParamFromDB(const CGUID& OwnedFactionGuid,const CGUID& OwnedUnionGuid,long lCurTaxRate,long lTotadyTotalTax,
						long lTotalTax);
	void SetParamFromGS(long lCurTaxRate,DWORD dwTotadyTotalTax,DWORD dwTotalTax);

	// 副本人数上限
	long GetDupRgnPlayerNumMax(void) { return m_DupRgnPlayerNumMax; };
	void SetDupRgnPlayerNumMax(long value) { m_DupRgnPlayerNumMax = value; };
	// 当前副本人数
	long GetCurDupRgnPlayerNum(void) { return m_CurDupRgnPlayerNum; };
	void SetCurDupRgnPlayerNum(long value) { m_CurDupRgnPlayerNum = value; };

	// 管理权开放（数值，1／0）
	bool GetOperFlag(void) { return b_OperFlag; }
	void SetOperFlag(bool flag) { b_OperFlag = flag; }
	// 删除时间
	long GetDelTime(void) { return m_DelTime; }
	void SetDelTime(long value) { m_DelTime = value; }
	// 维持费用
	long GetCostValue(void) { return m_CostValue; }
	void SetCostValue(long value) { m_CostValue = value; }

	void SetOwnerGUID(const CGUID& guid) { m_OwnerGUID = guid; }
	const CGUID& GetOwnerGUID(void) { return m_OwnerGUID; }

	DWORD GetAreaId(){return m_dwAreaId;}
	VOID SetAreaId(DWORD dwId){m_dwAreaId=dwId;}

	// 是否是模板场景
	bool GetIsTemplateRgnFlag(void) { return m_bIsTemplateRgnFlag; }
	void SetIsTemplateRgnFlag(bool flag) { m_bIsTemplateRgnFlag = flag; }

	eRST GetSpaceType(){return m_eSpaceType;}
	void SetSpaceType(eRST eType){m_eSpaceType=eType;}

	eRWT GetWarType(){return m_WarRegionType;}
	void SetWarType(eRWT eType){m_WarRegionType=eType;}

	bool IsRide()	{return m_Ride;}
	void SetRide(bool b) {m_Ride=b;}

	
	void SetRelive(bool bRelive){m_bRelive = bRelive;}
	bool GetRelive()			{return m_bRelive;}
		
	void SetDeadReturn(bool bDeadReturn){m_bDeadReturn = bDeadReturn;}
	bool GetDeadReturn()				{return m_bDeadReturn;}


	bool IsChangeEquip()	{return m_ChangeEquip;}
	void SetChangeEquip(bool b) {m_ChangeEquip = b;}

	// 根据当前场景复制数据
	bool CloneData(CWorldRegion* pRgn);

	// 复制全部数据
	bool CloneAllData(CWorldRegion* pRgn);

private:
	long        m_ID;
	DWORD		m_dwAreaId;//场景所属区域ID
	CGUID	m_OwnerGUID;
	// 副本人数上限
	long m_DupRgnPlayerNumMax;
	// 当前副本人数
	long m_CurDupRgnPlayerNum;
	// 管理权开放（数值，1／0）
	bool b_OperFlag;
	// 删除时间
	long m_DelTime;
	// 维持费用
	long m_CostValue;

	// gsid
	long m_Gsid;

	// 是否是模板场景
	bool m_bIsTemplateRgnFlag;
};
