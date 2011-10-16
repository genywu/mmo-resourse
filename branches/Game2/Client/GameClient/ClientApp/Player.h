#pragma once
#include "MoveShape.h"
#include "Goods/Goods.h"
#include "Horse.h"
#include "Talisman.h"
#include "Wing.h"
#include "HeadWear.h"
#include "Weapon.h"

//----------------------------------------
#include "../../GameClient/ClientApp/Goods/GoodsList.h"
#include "../../../Public/Common/PlayerSetupOnServer.h"
#include "../../../Public/Common/BaseDef.h"
#include "../../../Public/Common/PlayerDef.h"
#include "../../../Public/Common/TeamDef.h"
#include "../../../Public/Common/QuestRecord.h"
//-----------------08.12.11.nikai----------------
class CPet;
//-----------------------------------------------
#define HOTDOG_SUM	8				
#define MAX_MONEY	2000000000		
#define MAX_SILVER	2000000000		
#define COTE_NUM	5				//子背包的个数
#define PACK_WIDE   5				//子背包的列数
#define	PACK_NUM	6				//所有背包的个数
#define EX_EQUIP_NUM    8           //装备法宝天字、地字、人字栏位均为8个
#define HOST_GRIDITEM	20			//原始背包的总格子数


//-----------------09.03.11.lpf----------------
#define FASHION_TYPE_GREATCOAT	0x01		//外套显示状态
#define FASHION_TYPE_MANTEAU	0x02		//披风显示状态
#define FASHION_TYPE_HELM		0x04		//头盔显示状态(兼容原有设计,但目前没有使用)
//---------------------------------------------


//-------------------07.9.6liuke------------------
//class CRoleQueue;
class CMonster;
class CQuest;
//------------------------------------------------
namespace CEGUI
{
    class Window;
    class EventArgs;
};
class CRegion;
class CEmotionShower;
class CPlayer : public CMoveShape
{
	PlayerSetupOnServer	m_PlayerSetupOnServer;

public:

	// 职业
	static string s_strOccupation[OCC_Max];
	static void InitPlayerOccuType();
	static const char* GetPlayerOccuType(long lOccuTypeIndex);

	// 星座
	static string s_strConstellation[CONST_Pisces+1];
	static void InitPlayerConstType();
	static const char* GetPlayerConstType(long lConstTypeIndex);

	CPlayer(void);
	virtual ~CPlayer(void);

	// 初始化变量名到值得影射
	void InitNameValueMap();

	//初始化显示模型
	void SetDisplayModelGroup(bool bAsyn=true);
	virtual bool IsNeedDisplay();	// 该Player是否需要显示
	void RefeashState()	{};
	bool Display(void);
	void RendShadow(bool bsimpleshadow);
	void DisplayText(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0, bool bIsDisplayHPBar = false);				// 在shape身上显示文字
	void DisplayHideInfo(const char* str, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0);			// 在shape身上显示隐藏信息后的文字
	void DisplayWarBitmap();								//显示战斗状态的相关图标

	WORD GetOccuLvlCoe(eOccupation byOccu,WORD wOccuLevel);	// 求职业等级系数

	// 人物技能
	struct tagSkill
	{
		DWORD	dwID;					//	编号
		WORD	wLevel;					//	等级
		DWORD	dwRestoreTime;			//	冷却时间 (毫秒)
		DWORD	dwCoolDownStartTime;	//	开始播放冷却特效时间 timeGetTime (为0表示不显示CoolDown特效)
		DWORD	dwCoolDownGrayStartTime;//	图标变灰时候的时间，如果超过3秒灰色，那么自动切换回来
		DWORD	dwIntonateStartTime;	//	技能吟唱开始时间
		DWORD	dwIntonateTime;			//	技能的吟唱时间
		float	fAffectRangeMin;		//	最小释放距离
		float   fAffectRangeMax;		//	最大释放距离
		WORD	wCostMP;				//	消耗的MP
		bool	bIsPublicCoolEffect;	//	是否处于公共冷却时间中
		DWORD	dwUseRestoreTime;		//	使用的过程中,在快捷栏上播放使用效果的持续时间
		DWORD	dwUseingStartTime;		//	开始使用该技能的时间

		tagSkill()
		{
			dwID = 0;
			wLevel = 0;
			dwRestoreTime = 0;
			dwCoolDownStartTime = 0;
			dwCoolDownGrayStartTime = 0;
			dwIntonateStartTime = 0;	//	技能吟唱开始时间
			dwIntonateTime = 0;			//	技能的吟唱时间
			fAffectRangeMin = 0.0f;
			fAffectRangeMax = 0.0f;
			wCostMP = 0;
			bIsPublicCoolEffect = false;
			dwUseRestoreTime = 3200;
			dwUseingStartTime = 0;
		}
	};


	// 放上快捷栏里的物品
	struct  tagShortCutGoods
	{
		ulong	wID;					//	编号
		ushort	wPublicCooldownType;	//	公共冷却类型
		ulong	dwRestoreTime;			//	物品总的冷却时间(毫秒)
		ulong	dwRemainTime;			//  剩余的冷却时间（毫秒）
		ulong	dwCoolDownStartTime;	//	开始播放冷却特效时间 timeGetTime (为0表示不显示CoolDown特效)
		ulong	dwCoolDownGrayStartTime;//	图标变灰时候的时间，如果超过3秒灰色，那么自动切换回来
		bool	bCanBeUsed;				//  当前物品是否可被使用了

		tagShortCutGoods()
		{
			wID = 0;
			wPublicCooldownType = 0;
			dwRestoreTime = 0;
			dwRemainTime = 0;
			dwCoolDownStartTime = 0;
			dwCoolDownGrayStartTime = 0;
			bCanBeUsed = true;
		}
	};

	//人物已拥有的套装
	struct  tagSuitAttribute_P
	{
		map<string,bool> Suit_Equit;			//套装装备名及它是否被装备上
		WORD	Suit_Num;						//已拥有的件数
		WORD	Suit_SetupedNum;				//已穿上的件数
	};

public:
    /// 玩家相位坐标信息
    struct  PhasePlace
    {
        /// 坐标
        float  x_point;
        float  y_point;
        /// 范围1，2的大小
        float  a_size;
        float  b_size;
    };

    void   SetPhasePlace(const PhasePlace& place)   { m_PhasePlace = place ;} 
    const  PhasePlace&  GetPhasePlace() { return m_PhasePlace ; }

private:
    PhasePlace                     m_PhasePlace;

private:
	map<WORD,tagSuitAttribute_P>	m_SuitList;			//存放人物已拥有的所有套装信息
	//表示是否是主角自己
	bool m_bSelf;

public:	
	bool IsSelf() {return m_bSelf;}
	void SetIsSelf(bool b) {m_bSelf=b;}
	////////////////套装接口/////////////////////////
	map<WORD,tagSuitAttribute_P>*  GetPSuitList()		{return &m_SuitList;}								//获取人物身上拥有的套装列表
	bool IsSuitBeHold(WORD SuitID);																			//判断人物是否有此ID的套装
	bool IsSuitBeSetuped(WORD SuitID);																		//判断是否有此ID的装备被穿上了
	void AddSuit(WORD SuitID,string SuitOriginName);														//将此ID原始名为SuitOriginName的套装加入m_SuitList中
	map<string,bool>&	GetSuitEquipList(WORD SuitID)		{return m_SuitList[SuitID].Suit_Equit;}			//返回此ID的套装已拥有的装备的原始名表及是否装备上等信息
	WORD GetNumOfPossessionSuit(WORD SuitID);																//获取此ID的套装已拥有的件数
	WORD GetNumOfSetupEquip(WORD SuitID);																	//获取此ID的套装已装备上的件数
	bool CompareSuitOriginName(WORD SuitID,string SuitOriginName);											//比较判断已拥有的套装中是否有此原始名的装备
	bool CompareSetEquipOriName(WORD SuitID,string SuitOriginName);											//检查此原始名的装备是否已被装备上
	void SetupEquip(WORD SuitID,string SuitOriginName);														//将此原始名的装备穿在身上
	void UnfixEquip(WORD SuitID,string SuitOriginName);														//将此原始名的装备从身上脱下
	bool IsSuitComplete(WORD SuitID);																		//判断此ID的套装是否已凑齐
	bool RemoveEquipFromSuit(WORD SuitID,string SuitOriginName);											//把此ID的套装中原始名为SuitOriginName的装备从tagSuitAttribute_P中删除
	bool RemoveSuit(WORD SuitID);																			//把此ID的套装从m_SuitList中删除
	WORD FindEquipByOrName(WORD SuitID,string SuitOriginName);												//在人物所有的存物容器中查找套装ID为SuitID、原始名为SuitOriginName的装备，统计此种物品的数量
	WORD FindSetupEquipForOriName(WORD SuitID,string SuitOriginName);										//检查装备栏中此ID原始名为SuitOriginName的物品数量

	// 物品结构
	struct tagGoods
	{
		bool	bChange;				//	是否会发生属性变化
		long	lNum;					//	数量
		DWORD	lPos;					//	在背包栏中的坐标
		DWORD	lType;					//	物品所在的容器类型

		int	GetPosX()	{return (int)lPos%5;}
		int	GetPosY()	{return (int)lPos/5;}
		CGoods *pGoods;
		tagGoods()
		{
			bChange = false;
			lNum = 0;
			lPos = 0;
			lType = 0;
			pGoods = NULL;
		}
		~tagGoods()
		{
			bChange = false;
			lNum = 0;
			lPos = 0;
			lType = 0;
			pGoods = NULL;
		}
		//union
		//{
		//	DWORD	dwType;				//	物品类型编号
		//	CGoods	*pGoods;			//	可改变属性的物品指针
		//};
	};

	//子背包栏位结构
	struct tagCote
	{
		CGoods* pGoods;
		list<tagGoods> m_listCoteGoods;
		tagCote()
		{
			pGoods = NULL;
			m_listCoteGoods.clear();
		}
	};

	/*
	*	功能：自动装备物品到装备栏
	*	细节描述：在背包加入一个新物品的时候，如果这个物品可以装备，
	*				且原装备栏无装备，则向服务器发送装备消息。
	*	参数：	pGoods - 物品
	*			lSExtend - 来源容器（只接收背包、子背包或银行中的来源）
	*			lDExtend - 目的容器（一般是背包或子背包有效）
	*			dwDPos	 - 目的的位置（背包或子背包中的位置）
	*	返回值： true - 可以装备，向服务器发送装备消息；false - 不能装备
	*	作者: 刘科
	*	创建日期: 09.10.23
	*	修改日志: -
	*/
	bool	AutoEquip(CGoods * pGoods,long lSExtend,long lDExtend,DWORD dwDPos);
	////////////////////////#新增加的物品管理的函数/////////////////////////////////////
	bool	AddGoods(CGoods *pGoods,DWORD dwPos,LONG lExtendID);
	CGoods	*RemoveGoods(const CGUID& SourceGUID,const CGUID& DesGUID,DWORD dwPos,LONG lExtendID,DWORD dwAmount);
	CGoods	*GetGoodsByGUID(const CGUID& guID,int *pPos=NULL,long* lExtendID = NULL);
	WORD	GetGoodsContainerID(CGoods* pGoods);						//获取人物身上的物品所在的容器ID
	WORD	FindContainerIDOfGoods(CGoods* pGoods);						//查找人物身上(仅仅指主背包和子背包范围内)是否有某种类型的物品，如果有返回所在的容器ID
	CGoods* GetGoodsByPosition(long lExtendID,DWORD dwPos);				//通过位置查找物品
	CGoods* FindFirstGoodsByIndex(DWORD dwIndex,int& lExtendID,int& pPos);						//查找第一个物编ID等于传入的ID的未被锁定的物品
	// 根据传入的物品ID和要增加的数量查找一个堆叠数不会超过堆叠上限并且未被锁定的物品
	CGoods* FindOverLapGoodsByIndex(DWORD dwIndex,DWORD dwGoodsNum,int& lExtendID,int& pPos);
	//根据物品类型在人物身上来查找是否有这个物品，并且这个物品所在的位置，及它的帮定位置
	bool	FindAppointGoods(long lGoodType,long lGoodId, long lGoodNum, long lContainerID);									
	//根据物编号查找人物身上所有的此类物品的总量
	long	GetGoodsAmount(long nIdx);
	//根据物品名称查找人物身上所有的此类物品的总量
	long	GetGoodsAmount(const char* cGoodsName);

	bool	ClearGoods();
	bool	ChangeObjectAmount(LONG lContainerExtendID,LONG lObjectType,CGUID& guObjectID,DWORD dwAmount);

	//背包接口
	//根据容器ID和指定的位置获取人身上的物品（包括原始背包和子背包）
	tagGoods* GetGoodsOnPlayer(WORD ExtendID,WORD lPos);

	//获取指定背包的物品链表（包括原始背包）
	list<tagGoods>* GetGoodsList(WORD index=0);

	//获取原始背包的指定位置的物品
	tagGoods* GetGoods(WORD wPos);

	//获取指定序号的子背包的物品链表	
	list<tagGoods>* GetSubGoodslist(WORD index);

	//获取指定序号的子背包的某个位置的物品
	tagGoods* GetSubGoods(WORD ExtendID,WORD lPos);	

	//获取指定序号的子背包自身作为物品的属性
	CGoods* GetSubPGoods(WORD index);	

	//设置指定序号的子背包的pGoods
	void SetSubPGoods(WORD index,CGoods* pGoods);

	//清空指定序号的子背包
	void ClearCoteGoods(WORD index);

	//获取指定序号的子背包横排列数
	WORD GetPackageXRowNum(WORD index)		  {return (WORD)PACK_WIDE;}		
	
	//获取指定序号的子背包的容积
	WORD GetPackageDimen(WORD index);	
	
	//获取原始背包的容积
	WORD GetHostPackgeDimen()				  {return (WORD)HOST_GRIDITEM;}		

	//获取指定序号的背包的容积（包括主背包和子背包）
	WORD GetPacketDimen(DWORD dwPacketIndex);

	//是否所有的背包都是满的
	bool IsAllPackageFull();

	//获取第一个未满的背包的序号(从原始背包开始记数，-1表示没有找到未满的背包)
	int GetNotFullPackage();

	//获取指定序号的背包的第一个空格子的位置(dwExtendID>=0&&dwExtendID<=5)
	int GetFirstEmptyGrid(DWORD dwExtendID);	

	//获取人物指定序号的子背包数据结构
	tagCote* GetCotePackage(WORD index);	

	//判断某个背包是否已满
	bool IsFullForPackage(long PackageIndex);	

	//获取当前背包的空格子数量
	long GetEmptyGridNum();	

	//检测此背包是否有物品被锁定了 
	bool IsHaveLockedGoodsOnPackage(DWORD dwPacketIndex);												

	////////////跑墒背包接口///////////
	
	///获取跑墒背包指定位置的物品
	tagGoods* GetBusinessGoods(WORD wPos);
	///获取跑墒背包栏位上的背包
	CGoods* GetBusinessPackGoods();
	///设置跑墒背包
	void SetBussinesPack(CGoods*pPackGoods);
	///清理已有的跑墒背包（跑墒背包的CGoods对象，背包里面的物品链表没有变化）
	bool ClearBussinesPackGoods();
	///获取跑墒背包的格子数
	long GetBussinesPackDimen();
	///获取跑墒背包的容量（此处的“容量”指能存放的跑墒物品总的数量，跟背包的耐久值有关）
	long GetBussinesPackCapacity();
	///获取跑墒背包的当前容量
	long GetBussinesPackCurCapacity();
	///跑墒背包格子是否满了
	bool IsGrimFull();
	///跑墒背包容量是否满了
	bool IsCapacityFull();
	///获取跑墒背包的附加属性值
	long GetEffectValueOfPackGoods(WORD wEffectType,int iWhich);
	///是否装上了可用的跑墒背包
	bool IsHaveBusinessPack();

	///////////////////////////////////

	struct tagPlayerShopGoods
	{
		BYTE	btNum;
		BYTE	btOldPos;
		BYTE	btPos;
		long	lValue;

		union
		{
			DWORD dwType;
			CGoods* pGoods;
		};
	};


	// 装备栏索引
	enum eEquipIndex
	{
        // 装备类
        EQUIP_HEAD,				  // 头盔1
        EQUIP_NECKLACE,			  // 项链
        EQUIP_WING,				  // 翅膀
        EQUIP_BODY,				  // 盔甲
        EQUIP_BACK,				  // 腰带5
        EQUIP_HAND,                // 手部
        EQUIP_SHOULDER,            // 肩部
        EQUIP_WRIST,			      // 手腕	
        EQUIP_LEG,                 // 腿部
        EQUIP_BOOT,			      // 鞋子10	
        EQUIP_LRING,			      // 左戒指
        EQUIP_RRING,			      // 右戒指		
        EQUIP_WEAPON,			  // 武器
        EQUIP_CARD,			      // 护符
        EQUIP_GODGOODS,            // 圣物15
        EQUIP_EFFIGY,              // 雕像
        EQUIP_LIFECOWRY,           // 本命法宝
        EQUIP_SOULGUARD,           // 魂守
        EQUIP_RIDER,               // 坐骑19   


        // 时装类    
        EQUIP_P_HEADGEAR,			// 头饰
        EQUIP_P_FROCK,			    // 外套	
        EQUIP_P_WRIST,             // 手腕
        EQUIP_P_LOIN,              // 腰
        EQUIP_P_NECK,              // 颈
        EQUIP_P_AMULET,            // 护符
        EQUIP_P_FOOT,              // 脚
        EQUIP_P_LEG,               // 腿

        //称号类
        EQUIP_MEDAL1,			//	称号1
        EQUIP_MEDAL2,			//	称号2
        EQUIP_MEDAL3,			//	称号3
        EQUIP_SUM,
	};

	enum EquipRestrict
	{
		UNKNOW_NOTFIT,
		LEVEL_NOTFIT,		//等级不够
		STR_NOTFIT,			//力量不够
		DEX_NOTFIT,			//敏捷不够
		CON_NOTFIT,			//体质不够
		INT_NOTFIT,			//灵力不够
		OCCUPATION_NOTFIT,	//职业不对
		SEX_NOTFIT,			//性别不对
		REGION_FORBID,		//场景禁止
		EQUIPFIT,			//可以装备
	};


	// PVP级别判断标准值（刘科09.9.14）
	enum ePVPType
	{
		PVP_PIRATE1		=	36000,				//	1类海盗图标
		PVP_PIRATE2		=	72000,				//	2类海盗图标
		PVP_PIRATE3		=	144000,				//	3类海盗图标
	};
	///////////////////////////////////////////////////////////////////////////
	//玩家等级系数
	///////////////////////////////////////////////////////////////////////////
private:
	static long m_iEnemyPlayerLevelFirstQuotiety ;		//服务器敌国玩家等级系数1
	static long m_iEnemyPlayerLevelSecondQuotiety ;	//服务器敌国玩家等级系数1
public:
	static	long	GetEnemyPlayerLevelFirstQuotiety() {return m_iEnemyPlayerLevelFirstQuotiety;}
	static	long	GetEnemyPlayerLevelSecondQuotiety() {return m_iEnemyPlayerLevelSecondQuotiety;}
	static	void	SetEnemyPlayerLevelFirstQuotiety(long i){m_iEnemyPlayerLevelFirstQuotiety = i;}
	static	void	SetEnemyPlayerLevelSecondQuotiety(long i){m_iEnemyPlayerLevelSecondQuotiety = i;}
	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
public:
	/// 检查玩家周围指定范围内是否有别的玩家在开店
	bool	CheckIsAnyBodyShopOpenedOnRange(DWORD dwRangeSize);
	list<tagPlayerShopGoods>&	GetPlayerShopList() {return m_lstPlayerShop;}
	bool	IsPlayerShopOpened() {return m_bPlayerShopOpened; }
	void	SetPlayerShopOpen(bool bOpen) {m_bPlayerShopOpened = bOpen;}
	void	SetPlayerShopName(const char* pName) {m_strShopName = pName;}
	const char* GetPlayerShopName() {return m_strShopName.c_str();}
	const CGUID& GetPlayerShopSessionID(){return m_PlayerShopSessionID;}
	long	GetPlayerShopPlugID(){return m_lPlayerShopPlugID;}
	void	SetPlayerShopSessionID(const CGUID& SessionID){m_PlayerShopSessionID=SessionID;}
	void	SetPlayerShopPlugID(long lPlugID){m_lPlayerShopPlugID=lPlugID;}


	/*//////////////旧的人物属性接口/////////////////////////////////////////////////
	tagBaseProperty*	GetBaseProperty()		{return &m_BaseProperty;}
	const tagProperty*		GetProperty()		{return &m_Property;} 
	*////////////////////////////////////////////////////////////////////////////////

	list<tagSkill>& GetSkillList()				{return m_listSkillID;}

	eEquipLimitType QueryPlayerEquipLimitType(CPlayer* pPlayer); // 得到某个Player能够穿的防具类型
	bool	CanEquip(CGoods* pGoods);			// 检查此物品当前是否能装备
	//bool	CanEquip(DWORD dwGoodsIndex);		
	CGoods* GetEquip(int index);
	void	SetEquip(int index,CGoods *pGood);
	//tagGoods* GetHand() { return &m_stHand;}
	//void SetHand(tagGoods* pstGood){m_stHand=*pstGood;}
	//void SetHand(bool bChanged,WORD lNum,DWORD lPos,CGoods *pGood,DWORD lType=0);
	//void ClearHand(){SetHand(true,0,0,NULL);}


	CGoods *pMoney;
	/////////////////////////////////////////////////////////////////////////////////////
	// 更新属性
	void UpdateWeaponShow(CGoods* pGoods);
	void UpdateWeaponShow(DWORD dwPos);
	void RemoveWeaponShow(CGoods* pGoods);
	void RemoveWeaponShow(DWORD dwPos);
	void UpdateRideShow();
	void UpdateAllWeaponShow();
	void UpdateProperty(CMessage* pMsg/*,char** &strName,DWORD* &dwValue,char** &str,WORD& wNum*/);
	void DecordChangeProperty(BYTE* pByte, long& pos);
	void DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB);
	/////////////////////////////设置人物身上装备的接口//////////////////////////////////////
	DWORD GetEquipIndex(long l)	{return m_dwEquipIndex[l];}
	void SetEquipIndex(long lIndex, DWORD dwIndex);
	DWORD GetEquipLevel(long l)	{return m_btEquipLevel[l];}
	void SetEquipLevel(long lIndex,DWORD level);
	DWORD TotalRepairPrice();
	void UseItem(DWORD dwIndex,int nExtendId = -1,int iPos = -1);				//nExtendId是容器的ID(-1表示从玩家身上的所有容器中查找第一个物品编号为dwIndex的物品)，dwIndex是使用的物品的ID
	// 外部使用的一个静态调用接口
	static void CALLBACK CallUseItem(DWORD dwIndex,int nExtendId,int iPos,CPlayer*pPlayer=NULL);
	long CanUseItem(DWORD dwIndex);
	///////////////////////////////////////////////////////////////////////////
	void SetPlayerRemainDays(char remainDay){m_cRemainDays = remainDay;}
	char GetPlayerRemainDays(){return m_cRemainDays;}
    ///////////////////////////////////////////////////////////////////////////

	//////////////////商业系统接口////////////////
	//void InvestOnPoint(DWORD dwPoint,DWORD dwValue);			//玩家投资某个贸易点
	//设置玩家在某个区域的声望值
	void SetCreditForZone(DWORD dwZoneID,DWORD dwCreditValue) {m_mapPlayerCreditForZone[dwZoneID] = dwCreditValue;}
	//获得玩家在某个区域的声望值
	DWORD GetCreditForZone(DWORD dwZoneID);
	//获得玩家能建立关系的区域数量
	long GetCreditNum(){return m_lCreditNum;};
	//设置区域数量
	void SetCreditNum(long l){m_lCreditNum=l;};
	//////////////////XY 2008.2.2/////////////////

	//设置当前所拿的工具ID（根据副职业类型和副职业行为类型dwBeavioralID）
	void SetToolsID(DWORD dwBeavioralID,DWORD dwActionID = 1);
	//获取所拿工具的ID
	DWORD GetToolsID() {return m_dwToolIndex;}

	// ----------拷贝指定玩家的所有装备信息08.8.28-------------
	void CopyAllEquip(CShape *pShape);

private:
	static		BYTE  BuyBackGoodsNum;
	string		m_strPlayerOfficialRank;	//国家官阶
	D3DCOLOR	m_OfficialRankTextColor;	//国家官阶字的颜色
	D3DCOLOR	m_OfficialRankBkTextColor;	//国家官阶字背景颜色

	list<tagPlayerShopGoods>	m_lstPlayerShop;
	bool						m_bPlayerShopOpened;				//玩家是否处于摆摊状态
	string						m_strShopName;
	CGUID						m_PlayerShopSessionID;				
	long						m_lPlayerShopPlugID;

	bool						m_bIsShowLoverName;					//是否显示恋人名字 （好友界面中）
	//////////商业系统中商人的相关属性/////////
	string						m_strTraderName;				//商人称号
	//bool						m_bTradeModeSign;				//商业模式标志（进入跑商模式的开关）
	map<DWORD,DWORD>			m_mapPlayerCreditForZone;		//玩家在各个贸易区的声望（对应每个贸易区的ID值）
	map<DWORD,DWORD>			m_mapPlayerInvestmentForPoint;	//玩家在各个贸易点的投资额（对应每个贸易点的ID值）
	long                        m_lCreditNum;                   //区域的数量。
	//////////////XY 2008.1.16/////////////////

	///////////采集、制作属性//////////////////
	DWORD						m_dwCollectExp;					//采集经验
	DWORD						m_dwCollectLevel;				//采集等级
	DWORD						m_dwFactureExp;					//制作经验
	DWORD						m_dwFactureLevel;				//制作等级
	vector<DWORD>				m_vecFactureEntrys;				//学会的制作条目列表
	DWORD						m_dwToolIndex;					//当前使用的工具物编ID
	//////////////XY 2008.1.24////////////////

	////////////当前要用的人物属性结构////////////////////////////////////////
	tagPPro						m_Property;
	tagPFPro					m_FightProperty;
	tagPAPro					m_AppendProperty;
	tagFriendshipCounty			m_FriendshipCounty;
	tagPlayerElementProperty	m_arrElementProperty[MT_Max];	//王峰：魔化元素属性

	///////////////////////////////////////////////////////////////////////////

	////////////旧的人物属性结构///////////////////////////////////////////////
	//tagBaseProperty m_BaseProperty;		//	基本属性
	//tagProperty		m_Property;			//	实际属性
	///////////////////////////////////////////////////////////////////////////

	CGUID			m_guMoneyID;
	CGUID			m_guSilverMoneyID;
	CGUID			m_guBaoShiID;
	CGUID			m_guYuanBaoID;
	CGUID			m_guJiFenID;

	//	装备物品
	//	0：头		//	1：盔甲		//	2：武器
	//	3：护腕		//	4：鞋子		//	5：饰物1
	//	6：饰物2	//	7：饰物3	//	8：护甲
	CGoods*	m_Equip[EQUIP_SUM];

    CGoods* m_eEquipGod[EX_EQUIP_NUM];        //装备法宝天字栏位
    CGoods* m_eEquipLand[EX_EQUIP_NUM];       //装备法宝地字栏位
    CGoods* m_eEquipHuman[EX_EQUIP_NUM];      //装备法宝人字栏位
	//tagGoods m_stHand;	//手
	//tagGoods m_stPreHand;  //用于存放某个物品在被拿起到手上前所在的容器、栏位等信息
	bool m_bTradeDisAble;
	bool m_bFriendDisAble;

	//人物背包
	tagCote*			m_Cote[COTE_NUM];	  //	背包快捷栏
	list<tagGoods>		m_listGoods;		  //	管理人物原始背包物品的链表
    

	tagCote*		m_CoteBussinesPack;	  //	管理人物的跑墒背包中的物品

	deque<CGoods*>	m_queBuyBackGoodsItem;	  //    存储可以购回的物品

	list<tagShortCutGoods>	m_listShortCutGoods;	//冷却物品列表

	typedef DWORD PCooldownRTime;			
	map<WORD,PCooldownRTime> m_mapPublicCooldown;	// 物品公共冷却表

	// 人物技能
	list<tagSkill>		m_listSkillID;		//	技能列表
    map<CGUID, tagBrotherInfo>    m_mapBrotherInfo;      //结义金兰

	// -----------人物的职业列表（职业链）liuke08.8.29-------------
	vector<DWORD>			m_listOccuped;
	bool		IsAddOccupedList(DWORD dwSkillID);		// 指定的技能是否能触使职业链的变化

	bool m_bCanSetHotKey;					// 能否设置快捷栏

	bool m_bGMConcealment;			        // GM是否隐蔽

    std::map<long,long>   m_GeniusTable;    // 天赋配置表,ID-level

	//bool	m_bIsHideInfo;			// 是否隐藏角色信息
	//string	m_strHideInfoName;		// 隐藏信息之后要显示的名字

    /// 天赋相关@2010/10/21
public:
    void DecodeGeniusFromDataBlock(DBReadSet& setReadDB);	// 将缓冲中的天赋数据解码
	std::map<long,long>* GetGenius(){ return &m_GeniusTable ;}
public:
	/////////测试接口////////
	void RandomMoveGoods();
	/////////////////////////
	bool AddBuyBackGoods(CGoods* pBuyBackGoods);								// 加入要购回的物品
	bool DelBuyBackGoods(int index = -1);										// 删除一个回购队列中的物品(默认按队列的先进先出删除队列头的数据) 
	deque<CGoods*>& GetQueueBuyBackGoods()	{ return m_queBuyBackGoodsItem; }	// 获取购回物品队列	
	bool		IsInOccupedList(DWORD dwOccupID);								// 查看指定的职业是否存在于玩家的职业链之中
	vector<DWORD> GetOccupedList()	{return m_listOccuped;}						// 获得人物的职业链
	vector<DWORD>	GetPlayerFactureList() {return m_vecFactureEntrys;}
	bool AddFactureSkill(DWORD dwFactureID);
	void DeleteFactureSkill(DWORD dwFactureID);
	void DeleteAllFactureSkill();

	DWORD	m_dwEquipIndex[EQUIP_SUM];		// 装备编号（其他玩家使用）
	DWORD	m_btEquipLevel[EQUIP_SUM];		// 装备等级（其他玩家使用）

	char    m_cRemainDays;				//  被删除帐号的剩余天数


	bool GetIsGMConcealment()	{return m_bGMConcealment;}
	void SetGMConcealment(bool b) {m_bGMConcealment=b;}


	// 角色信息是否隐藏
	//bool GetIsHideInfo()		{return m_bIsHideInfo;}
	//void SetIsHideInfo(bool b)	{m_bIsHideInfo=b;}
	// 角色信息隐藏后的显示名
	//const char * GetHideInfoName()				{return m_strHideInfoName.c_str();}
	//void SetIsHideInfoName(string strName)		{m_strHideInfoName = strName;}
	/////////////////////////////////////////////////////////////////////
	// 属性存取接口
	/////////////////////////////////////////////////////////////////////
public:

	/////////已改过的人物属性接口//////////////////////////////////////////////////////////////////////
	DWORD   GetCollectSkillLevel()			  {return m_dwCollectLevel;}			//采集等级
	void	SetCollectSkillLevel(DWORD dwLevel) {m_dwCollectLevel = dwLevel;}

	DWORD	GetCollectSkillExp()			{return m_dwCollectExp;}			//采集经验
	void	SetCollectSkillExp(DWORD dwExp)	{m_dwCollectExp = dwExp;}

	DWORD	GetFactureSkillExp()			{return m_dwFactureExp;}			//制作经验
	void	SetFactureSkillExp(DWORD dwExp) {m_dwFactureExp = dwExp;}			

	DWORD	GetFactureSkillLevel()			 {return m_dwFactureLevel;}			//制作等级
	void	SetFactureSkillLevel(DWORD dwLevel){m_dwFactureLevel = dwLevel;}

	//DWORD	GetOccu()					{return m_Property.byAssOccu;}		
	//void	SetOccu(DWORD dwOccu)		{m_Property.byAssOccu = (eDeputyOccu)dwOccu;}	

	DWORD	GetAssOccu()				{return (DWORD)m_Property.byAssOccu;}			//获取副职业
	void	SetAssOccu(DWORD dwOccu)	{m_Property.byAssOccu = (eDeputyOccu)dwOccu;}

	DWORD	GetHp()						{return	m_Property.dwHp;}			//当前生命值
	void	SetHp(DWORD l)				{m_Property.dwHp=l;}

	WORD	GetMp()						{return	m_Property.wMp;}			//当前魔法值
	void	SetMp(WORD l)				{m_Property.wMp=l;}

	WORD	GetStamina()				{return	m_Property.wStamina;}		//当前体力
	void	SetStamina(WORD l)			{m_Property.wStamina=l;}

	WORD	GetEnergy()					{return	m_Property.wEnergy;}		//当前能量值 
	void	SetEnergy(WORD l)			{m_Property.wEnergy=l;}

	//BYTE	GetRankOfNobility()			{return m_Property.byRankOfNobility;}		//爵位等级
	//void	SetRankOfNobility(BYTE l)	{m_Property.byRankOfNobility=l;}

	//DWORD	GetRanOfNobCredit()			{return m_Property.dwRankOfNobCredit;}	//爵位的声望值
	//void	SetRanOfNobCredit(DWORD l)	{m_Property.dwRankOfNobCredit=l;}

	//BYTE	GetRankOfMercenary()		{return m_Property.byRankOfMercenary;}	//佣兵等级
	//void	SetRankOfMercenary(BYTE l)	{m_Property.byRankOfMercenary=l;}

	//DWORD	GetMercenaryCredit()		{return m_Property.dwMercenaryCredit;}	//佣兵的声望值
	//void	SetMercenaryCredit(DWORD l)	{m_Property.dwMercenaryCredit=l;}

	BYTE	GetCountry()				{return	m_Property.byCountry;}			//国家ID
	void	SetCountry(BYTE l)			{m_Property.byCountry=l;}

	long	GetCountryContribute()		{return m_Property.lCountryContribute;}	//对国家的贡献值
	void	SetCountryContribute(long l){m_Property.lCountryContribute=l;}

	DWORD	GetUpgradeExp()				{return	m_Property.dwUpgradeExp;}			//下一级升级所需经验
	void	SetUpgradeExp(DWORD l)		{m_Property.dwUpgradeExp=l;}				

	DWORD	GetOccuUpgradeExp()			{return	m_Property.dwCurOccuUpgradeExp;}	//下一级职业等级升级所需经验
	void	SetOccuUpgradeExp(DWORD l)	{m_Property.dwCurOccuUpgradeExp=l;}	

	BYTE	GetBusinessLevel()			{return m_Property.byBusinessLevel;}		//商业等级
	void	SetBusinessLevel(BYTE l)	{m_Property.byBusinessLevel=l;}

	DWORD	GetBusinessExp()			{return m_Property.dwBusinessExp;}		//商业经验
	void	SetBusinessExp(DWORD l)		{m_Property.dwBusinessExp = l;}

	//BYTE	GetArtisanLevel()			{return	m_Property.byArtisanLevel;}		//工匠等级
	//void	SetArtisanLevel(BYTE l)		{m_Property.byArtisanLevel=l;}

	//DWORD	GetArtisanExp()				{return m_Property.dwArtisanExp;}			//工匠经验
	//void    SetArtisanExp(DWORD l)		{m_Property.dwArtisanExp=l;}

	//DWORD	GetArtisanCredit()			{return m_Property.dwArtisanCredit;}			//工匠声望
	//void    SetArtisanCredit(DWORD l)	{m_Property.dwArtisanCredit=l;}

	WORD	GetLevel()					{return m_Property.wLevel;}				//玩家当前等级
	void	SetLevel(WORD l)			{m_Property.wLevel=l;}	

	DWORD	GetExp()					{return m_Property.dwExp;}				//经验值
	void	SetExp(DWORD l)				{m_Property.dwExp=l;}

	DWORD	GetPresentExp()				{return m_Property.dwPresentExp;}				//经验值
	void	SetPresentExp(DWORD l)		{m_Property.dwPresentExp=l;}

	BYTE	GetHairPic()				{return m_Property.byHairPic;}			//得到头发信息
	void	SetHairPic(BYTE by)			{m_Property.byHairPic=by;}				//设定头发信息

	BYTE	GetHairStyle()				{return (m_Property.byHairPic & 0xF0) >> 4;}					//得到发型编号
	void	SetHairStyle(BYTE by)		{m_Property.byHairPic = (m_Property.byHairPic & 0x0F) | by;}	//设定发型编号

	BYTE	GetHairColor()				{return (m_Property.byHairPic & 0x0F);}									//得到发色编号
	void	SetHairColor(BYTE by)		{m_Property.byHairPic = (m_Property.byHairPic & 0xF0) | (by << 4);}		//设定发色编号

	BYTE	GetFacePic()				{return m_Property.byFacePic;}			//面部图片ID
	void	SetFacePic(BYTE l)			{m_Property.byFacePic=l;}

	eOccupation	GetOccupation()			{return m_Property.byOccu;}				//职业
	void	SetOccupation(BYTE l)		{m_Property.byOccu = (eOccupation)l;}

	//eConst	GetConst()					{return m_Property.byConst;}			//星座
	//void	SetConst(BYTE l)			{m_Property.byConst=(eConst)l;}

	BYTE	GetSex()					{return m_Property.bySex;}				//性别(0:男 1:女) 
	void	SetSex(BYTE l)				{m_Property.bySex=l;}

	DWORD	GetSpouseID()				{return m_Property.dwSpouseID;}			//配偶身份ID
	void	SetSpouseID(DWORD l)		{m_Property.dwSpouseID=l;}

	DWORD	GetSpouseParam()			{return m_Property.dwSpouseParam;}		//和配偶的关系值
	void	SetSpouseParam(DWORD l)		{m_Property.dwSpouseParam=l;}

	DWORD	GetPkCount()				{return m_Property.dwPkCount;}			//玩家PK杀死国内人数
	void	SetPkCount(DWORD l)			{m_Property.dwPkCount=l;}

	DWORD	GetPkValue()				{return m_Property.dwPkValue;}			//玩家PK值(犯罪值)
	void	SetPkValue(DWORD l)			{m_Property.dwPkValue=l;}

	DWORD	GetPVPCount()				{return m_Property.dwPVPCount;}			//玩家PVP杀外国人数量
	void	SetPVPCount(DWORD l)		{m_Property.dwPVPCount=l;}

	DWORD	GetPVPValue()				{return m_Property.dwPVPValue;}			//玩家PVP值(恶名值)
	void	SetPVPValue(DWORD l)		{m_Property.dwPVPValue=l;}

	//DWORD   GetMedalScores()            { return m_Property.dwMedalScores; }    //玩家勋章分数
	//void    SetMedalScores(DWORD l)     { m_Property.dwMedalScores = l;}

	bool	GetShowCountry()			{ return m_Property.bShowCountry; }		//玩家是否显示国家标志
	void	SetShowCountry(bool b)		{ m_Property.bShowCountry = b; }

	DWORD	GetHotKey(long lPos)		{return m_Property.dwHotKey[lPos];}		//热健(F1~F12,1~+)
	void	SetHotKey(long lPos,DWORD l){m_Property.dwHotKey[lPos]=l;}

	bool	IsCanSetHotKey()					{return m_bCanSetHotKey;}		// 获得能否设置快捷键
	void	SetIsCanSetHotKey(bool bIsCan)		{m_bCanSetHotKey = bIsCan;}		// 设置能否设置快捷键的装态

	DWORD   GetExpMultiple()             {return m_Property.dwExpMultiple;}     // 获得经验倍数
	void    SetExpMultiple(DWORD l)      {m_Property.dwExpMultiple = l;}        // 设置经验倍数

	DWORD   GetNextExpMultiple()         {return m_Property.dwUpgradeExpMultiple;} //获得下级经验倍数
	void    SetNextExpMultiple(DWORD l)  {m_Property.dwUpgradeExpMultiple = l;}   //设置下级经验倍数

	//DWORD   GetBatakCredit()             {return m_Property.dwBatakCredit;}     //获得巴塔克声望
	//void    SetBatakCredit(DWORD l)      {m_Property.dwBatakCredit = l;}        //设置巴塔克声望

	//DWORD   GetZandoCredit()             {return m_Property.dwZanDoHunterCredit;}     //获得赞多狩猎声望
	//void    SetZandoCredit(DWORD l)      {m_Property.dwZanDoHunterCredit = l;}        //设置赞多狩猎声望

	long	GetCurrBaseCP()				 {return m_Property.lCurrBaseActive;}		//剩余基本CP
	void	SetCurrBaseCP(long cp)		 {m_Property.lCurrBaseActive = cp;}
	long	GetCurrExCP()				 {return m_Property.lCurrExActive;}			//剩余扩展CP
	void	SetCurrExCP(long cp)		 {m_Property.lCurrExActive = cp;}

	long    GetCurPKCP()                 {return m_Property.lCurPKCP;}           //获得当前CP值
	void    SetCurPKCP(long l)           {m_Property.lCurPKCP = l;}              //设置当前CP值

//	DWORD   GetChurchCredit()            {return m_Property.dwChurchCredit;}      //获得光之教会声望
//    void    SetChurchCredit(DWORD l)     {m_Property.dwChurchCredit = l;}          //获得光之教会声望

	//PK开关(b1:全体保护,b2:组队保护,b4:工会保护,b8:红名保护,b16:本国保护,b32:阵营保护,b64.海盗保护)
	bool IsPk_All()												
	{
		if(m_Property.byPKOnOff&0x01) 
			return true;
		else
			return false;
	}		
	bool IsPk_Team()												
	{
		if(m_Property.byPKOnOff&0x02) 
			return true;
		else
			return false;
	}
	bool IsPk_Union()											
	{
		if(m_Property.byPKOnOff&0x04) 
			return true;
		else
			return false;
	}
	bool IsPk_RedName()												
	{
		if(m_Property.byPKOnOff&0x08) 
			return true;
		else
			return false;
	}
	bool IsPk_National()												
	{
		if(m_Property.byPKOnOff&16) 
			return true;
		else
			return false;
	}
	bool IsPk_Camp()												
	{
		if(m_Property.byPKOnOff&32) 
			return true;
		else
			return false;
	}
	bool IsPk_Pirates()												
	{
		if(m_Property.byPKOnOff&64) 
			return true;
		else
			return false;
	}
	void	SetPKOnOff(BYTE b)			{ m_Property.byPKOnOff = b; }				//设置PK开关
	BYTE	GetPKOnOff()				{ return m_Property.byPKOnOff; }

	//得到当前职业拥有的技能点数(SP)
	DWORD	GetSkillPoint()
	{
		return	m_Property.dwOccuSP[m_Property.byOccu];
	}	

	//得到职业等级
	BYTE	GetOccuLvl(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.byOccuLvl[byOccu];
		return 0;
	}
	void  SetOccuLvl(eOccupation byOccu,BYTE l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.byOccuLvl[byOccu] = l;
	}

	//得到职业经验
	DWORD	GetOccuExp(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.dwOccuExp[byOccu];
		return 0;
	}
	void SetOccuExp(eOccupation byOccu,DWORD l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.dwOccuExp[byOccu] = l;
	}
	
	DWORD GetOccuSp(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.dwOccuSP[byOccu];
		return 0;
	}
	void SetOccuSp(eOccupation byOccu,DWORD l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.dwOccuSP[byOccu] = l;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	string	GetOfficialRank()							{return m_strPlayerOfficialRank;}
	void	SetOfficialRank(string str)					{m_strPlayerOfficialRank = str;}

	void	SetOffcialRankTextColor(D3DCOLOR color)		{m_OfficialRankTextColor = color;}
	D3DCOLOR	GetOffcialRankTextColor()				{return m_OfficialRankTextColor;}

	void	SetOffcialRankBkTextColor(D3DCOLOR color)	{m_OfficialRankBkTextColor = color;}
	D3DCOLOR	GetOffcialRankBkTextColor()				{return m_OfficialRankBkTextColor;}
	///////////////////////////////////////////////////////////////////////////////////////////////
	//										人物战斗属性接口
	///////////////////////////////////////////////////////////////////////////////////////////////

	DWORD	GetMaxHp()					{return m_FightProperty.dwMaxHp;}				//生命上限
	void	SetMaxHp(DWORD l)			{m_FightProperty.dwMaxHp=l;}

	WORD	GetMaxMp()					{return m_FightProperty.wMaxMp;}				//法力上限
	void	SetMaxMp(WORD l)			{m_FightProperty.wMaxMp=l;}

	WORD	GetMaxStamina()				{return m_FightProperty.wMaxStamina;}			//体力上限
	void	SetMaxStamina(WORD l)		{m_FightProperty.wMaxStamina=l;}

	WORD	GetMaxEnergy()				{return	m_FightProperty.wMaxEnergy;}			//能量上限 
	void	SetMaxEnergy(WORD l)		{m_FightProperty.wMaxEnergy=l;}

	WORD	GetHpRecoverSpeed()			{return	m_FightProperty.wHpRecoverSpeed;}		//生命恢复速度
	void	SetHpRecoverSpeed(WORD l)	{m_FightProperty.wHpRecoverSpeed;}

	WORD	GetMpRecoverSpeed()			{return	m_FightProperty.wMpRecoverSpeed;}		//法力恢复速度
	void	SetMpRecoverSpeed(WORD l)	{m_FightProperty.wMpRecoverSpeed=l;}

	WORD	GetStamRecoverSpeed()		{return m_FightProperty.wStamRecoverSpeed;}		//体力恢复速度	
	void	SetStamRecoverSpeed(WORD l)	{m_FightProperty.wStamRecoverSpeed=l;}

	//WORD	GetStrenth()				{return m_FightProperty.wStrenth;}				//力量
	//void	SetStrenth(WORD l)			{m_FightProperty.wStrenth=l;}

	//WORD	GetAgility()				{return	m_FightProperty.wAgility;}				//敏捷
	//void	SetAgility(WORD l)			{m_FightProperty.wAgility=l;}

	//WORD	GetConsti()					{return	m_FightProperty.wConsti;}				//体质 
	//void	SetConsti(WORD l)			{m_FightProperty.wConsti=l;}

	//WORD	GetIntell()					{return	m_FightProperty.wIntell;}				//智力
	//void	SetIntell(WORD l)			{m_FightProperty.wIntell=l;}

	//WORD	GetWisdom()					{return	m_FightProperty.wWisdom;}				//智慧
	//void	SetWisdom(WORD l)			{m_FightProperty.wWisdom=l;}

	//WORD	GetSpirit()					{return	m_FightProperty.wSpirit;}				//精神
	//void	SetSpirit(WORD l)			{m_FightProperty.wSpirit=l;}

    WORD   GetStr()                     { return m_FightProperty.wStr; }
    void   SetStr(WORD s)               { m_FightProperty.wStr = s; }
    WORD   GetDex()                     { return m_FightProperty.wDex; }
    void   SetDex(WORD s)               { m_FightProperty.wDex = s; }
    WORD   GetCon()                     { return m_FightProperty.wCon; }
    void   SetCon(WORD s)               { m_FightProperty.wCon = s; }
    WORD   GetIntellect()               { return m_FightProperty.wIntellect; }
    void   SetIntellect(WORD s)         { m_FightProperty.wIntellect = s; }    
    WORD   GetSpiritualism()            { return m_FightProperty.wSpiritualism; }
    void   SetSpiritualism(WORD s)      { m_FightProperty.wSpiritualism = s; }
    WORD   GetPoint()                   { return m_FightProperty.wPoint; }
    void   SetPoint(WORD s)             { m_FightProperty.wPoint = s; }

	WORD	GetFightBlock()				{return	m_FightProperty.wFightBlock;}			//格挡
	void	SetFightBlock(WORD l)		{m_FightProperty.wFightBlock=l;}

	WORD	GetParry()					{return	m_FightProperty.wParry;}				//招架
	void	SetParry(WORD l)			{m_FightProperty.wParry=l;}

	WORD	GetDodge()					{return	m_FightProperty.wDodge;}				//闪避 
	void	SetDodge(WORD l)			{m_FightProperty.wDodge=l;}

	WORD	GetCharm()					{return	m_FightProperty.wCharm;}				//魅力
	void	SetCharm(WORD l)			{m_FightProperty.wCharm=l;}

	WORD	GetLuck()					{return	m_FightProperty.wLuck;}					//幸运
	void	SetLuck(WORD l)				{m_FightProperty.wLuck=l;}

	WORD	GetHit()					{return m_FightProperty.wHit;}					//命中
	void	SetHit(WORD l)				{m_FightProperty.wHit=l;}

	WORD	GetAbsorb()					{return m_FightProperty.wAbsorb;}				//专注
	void	SetAbsorb(WORD l)			{m_FightProperty.wAbsorb=l;}

	long	GetDef()					{return m_FightProperty.wDef;}					//物理防御
	void	SetDef(WORD l)				{m_FightProperty.wDef=l;}

	long	GetMdef()					{return m_FightProperty.wMdef;}					//魔法防御
	void	SetMdef(WORD l)				{m_FightProperty.wMdef=l;}

	long	GetCri()					{return m_FightProperty.wCri;}					//物理爆击
	void	SetCri(WORD l)				{m_FightProperty.wCri=l;}

	WORD	GetMAtkSpeed()				{return m_FightProperty.wMAtkSpeed;}			//施法速度
	void	SetMAtkSpeed(WORD l)		{m_FightProperty.wMAtkSpeed=l;}

	WORD	GetBuffPower()				{return m_FightProperty.wBuffPower;}			//Buff攻击强度
	void	SetBuffPower(WORD l)		{m_FightProperty.wBuffPower=l;}

	long	GetMinAtk()					{return m_FightProperty.wMinAtk;}				//最小攻击力
	void	SetMinAtk(WORD l)			{m_FightProperty.wMinAtk=l;}

	long	GetMaxAtk()					{return m_FightProperty.wMaxAtk;}				//最大攻击力
	void	SetMaxAtk(WORD l)			{m_FightProperty.wMaxAtk=l;}

	long	GetMAtk()					{return m_FightProperty.wMAtk;}					//魔法攻击力
	void	SetMAtk(WORD l)				{m_FightProperty.wMAtk=l;}

	long	GetMCri()					{return m_FightProperty.wMCri;}					//魔法爆击率
	void	SetMCri(WORD l)				{m_FightProperty.wMCri=l;}

	WORD	GetBloodSuk()				{return m_FightProperty.wBloodSuck;}				//生命吸收率
	void	SetBloodSuk(WORD l)			{m_FightProperty.wBloodSuck=l;}

	WORD	GetManaSuck()				{return m_FightProperty.wManaSuck;}				//法力吸收率
	void	SetManaSuck(WORD l)			{m_FightProperty.wManaSuck=l;}

	WORD	GetWillDef()				{return m_FightProperty.wWillDef;}				//意志抗性
	void	SetWillDef(WORD l)			{m_FightProperty.wWillDef=l;}

	WORD	GetConstiDef()				{return m_FightProperty.wConstiDef;}			//体质抗性
	void	SetConstiDef(WORD l)		{m_FightProperty.wConstiDef=l;}

	WORD	GetImmunity()				{return m_FightProperty.wImmunity;}				//免伤
	void	SetImmunity(WORD l)			{m_FightProperty.wImmunity=l;}

	WORD	GetPierce()					{return m_FightProperty.wPierce;}				//穿透
	void	SetPierce(WORD l)			{m_FightProperty.wPierce=l;}

	WORD	GetDeBuffDef(long pos)				{return m_FightProperty.wDeBuffDef[pos];}		//Debuff抗性
	void	SetDeBuffDef(long pos,WORD l)		{m_FightProperty.wDeBuffDef[pos]=l;}

	WORD	GetElemDef(long pos)				{return m_FightProperty.wElemDef[pos];}		//各个元素抗性
	void	SetElemDef(long pos,WORD l)			{m_FightProperty.wElemDef[pos]=l;}

	WORD	GetClassDam(long pos)				{return m_FightProperty.wClassDam[pos];}		//对各个种族伤害
	void	SetClassDam(long pos,WORD l)		{m_FightProperty.wClassDam[pos]=l;}

	WORD	GetElemDam(long pos)				{return m_FightProperty.wElemDam[pos];}		//对各个元素伤害
	void	SetElemDam(long pos,WORD l)			{m_FightProperty.wElemDam[pos]=l;}

	WORD	GetFightHpRecoverSpeed()			{return	m_FightProperty.wFightHpRecoverSpeed;}	//战斗时生命恢复速度
	void	SetFightHpRecoverSpeed(WORD l)		{m_FightProperty.wFightHpRecoverSpeed=l;}

	WORD	GetFightMpRecoverSpeed()			{return	m_FightProperty.wFightMpRecoverSpeed;}	//战斗时法力恢复速度
	void	SetFightMpRecoverSpeed(WORD l)		{m_FightProperty.wFightMpRecoverSpeed=l;}

	short	GetEnerRecoverSpeed()				{return m_FightProperty.wEnerRecoverSpeed;}		//能量恢复速度
	void	SetEnerRecoverSpeed(short l)		{m_FightProperty.wEnerRecoverSpeed=l;}

	short	GetFightEnerRecoverSpeed()			{return m_FightProperty.wFightEnerRecoverSpeed;}	//战斗能量恢复速度
	void	SetFightEnerRecoverSpeed(short l)	{m_FightProperty.wFightEnerRecoverSpeed=l;}

//	WORD	GetEnergySuck()						{return	m_FightProperty.wEnergySuck;}			//能量吸收率
//	void	SetEnergySuck(WORD l)				{m_FightProperty.wEnergySuck=l;}

	WORD	GetCriDef()							{return	m_FightProperty.wCriDef;}				//物理爆击抗性
	void	SetCriDef(WORD l)					{m_FightProperty.wCriDef=l;}

	WORD	GetMCriDef()						{return	m_FightProperty.wMCriDef;}				//魔法爆击抗性
	void	SetMCriDef(WORD l)					{m_FightProperty.wMCriDef=l;}
	long    GetMaxPKCP()                        {return m_FightProperty.lMaxPKCP;}             //获得最大的PKCP值
	void    SetMaxPKCP(long l)                  {m_FightProperty.lMaxPKCP=l;}                   //设置最大的PKCP值

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////人物附加属性接口////////////////////////////////////////////////////////////

	BYTE GetShowFashion()				{ return m_AppendProperty.bShowFashion; }		//是否显示披风外套
	void SetShowFashion(BYTE by)		{ m_AppendProperty.bShowFashion = by; }

	const char *GetAccount()			{return m_AppendProperty.strAccount;}			//帐号
	void	SetAccount(const char* l)	{strcpy_s(m_AppendProperty.strAccount,ACCOUNT_SIZE,l);}

	const char *GetTitle()				{return m_AppendProperty.strTitle;}				//称号
	void	SetTitle(const char* l)		{strcpy_s(m_AppendProperty.strAccount,ACCOUNT_SIZE,l);}

	bool	GetIsCharged()				{return m_AppendProperty.bIsCharged;}			//是否充值
	void	SetIsCharged(bool l)		{m_AppendProperty.bIsCharged=l;}

	/////////////////////////////////////////////////其他属性接口/////////////////////////////////////////////////
	DWORD	GetCountyID()				{return m_FriendshipCounty.dwCountyID;}				//所属郡ID
	void	SetCountyID(DWORD id)		{m_FriendshipCounty.dwCountyID=id;}

	DWORD	GetFrienDeg()				{return	m_FriendshipCounty.dwFrienDeg;}				//友好度
	void	SetFrienDeg(DWORD deg)		{m_FriendshipCounty.dwFrienDeg=deg;}

	long	CheckSpace(CGoods* pGoods);//检测空间
	void	RefreshProperty();			// 根据当前装备刷新人物属性
	void	MountEquip(CGoods *pGood,bool bPositive=true);
	void	UnMountEquip(CGoods *pGood);
	long	CanMountEquip(CGoods *pGoods);

    void	SetguID(LONG lExtendID,const CGUID& guid);
	void	SetguMoneyID(const CGUID& guid)		{m_guMoneyID =guid; }
	void	SetguSilverMoneyID(const CGUID& guid){m_guSilverMoneyID = guid;}
	void	SetguBaoShiID(const CGUID& guid)	{m_guBaoShiID =guid; }
	void	SetguYuanBaoID(const CGUID& guid)	{m_guYuanBaoID =guid;}
	void	SetguJiFenID(const CGUID& guid)		{m_guJiFenID =guid;}

	CGUID&	GetguMoneyID()		{return m_guMoneyID;}
	CGUID&	GetguSilverID()		{return m_guSilverMoneyID;}
	CGUID&	GetguBaoShiID()		{return m_guBaoShiID;}
	CGUID&	GetguYuanBaoID()	{return m_guYuanBaoID;}
	CGUID&	GetguJiFenID()		{return m_guJiFenID;}

	//当属性值改变以后出发
	virtual bool  OnPropertyChanged(const char* attrName);
	//得到属性值的字符串表示
	virtual const char* GetPropertyStringValue(const char* pszPropertyName);
	//是否显示配偶名字
	bool IsShowLoverName() {return m_bIsShowLoverName;}
	void SetIsShowLoverName(bool isShow) {m_bIsShowLoverName = isShow;}
public:
	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecordOrgSysFromByteArray(BYTE* pByte, long& pos);

	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);
	virtual bool DecodeOrgSysFromDataBlock(DBReadSet& setReadDB);

	//////////////////////////////////////////////////////////////////////////
	//	战斗
	//////////////////////////////////////////////////////////////////////////
public:
	void SetSinState(bool b)		{m_bSinState=b;}
	bool IsSinState()				{return m_bSinState;}
	void SetHitNum(long l)			{m_lHitNum=l;}
	long GetHitNum()				{return m_lHitNum;}
	void SetLastHitTimer(DWORD dw)	{m_dwLastHitTimer=dw;}
	DWORD GetLastHitTimer()			{return m_dwLastHitTimer;}
	DWORD GetDefaultAttackSkillID();	//技能的选择逻辑

	// 王峰：魔化属性接口
	void SetMagicElem( eAttackDefend eAD, eMagicElement eMT, long lVal );
	long GetMagicElem( eAttackDefend eAD, eMagicElement eMT );
	long GetNumOfXiangXing();

protected:
	bool m_bSinState;			// 犯罪状态
	long m_lHitNum;				// 当前连击的数量
	DWORD m_dwLastHitTimer;		// 上一次击中的时间

	/////////////////////////////////////////////////////////////////////
	// AI
	/////////////////////////////////////////////////////////////////////
public:
	virtual void AI();		// AI处理

	/////////////////////////////////////////////////////////////////////
	// 消息响应
	/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage*);

	/////////////////////////////////////////////////////////////////////
	// 缓慢补充HP/MP/MA
	/////////////////////////////////////////////////////////////////////
protected:
	long m_lGrowTimer;

public:
	list<short>	m_listGrowHP;
	list<short> m_listGrowMP;

	//////////////////////////////////////////////////////////////////////////
	// 组队相关
	//////////////////////////////////////////////////////////////////////////
public:
	struct tagTeammate
	{
		tagTeammate(CGUID& id):ID(id) {}
		tagTeammate(){}
		bool operator == (const tagTeammate & t) { return t.ID == ID; }
		CGUID ID;
		WORD wHP;
		WORD wMaxHP;
		BYTE occupation;
		CGUID RegionID;
		string strName;
	};
	struct tagPlayerInfo
	{
		DWORD dwForeColor;
		DWORD dwBackColor;
		DWORD dwStartTime;
		BYTE backAlpha;
		BYTE foreAlpha;
		string strText;
	};

private:
	long lTest;
	vector<tagTeammate> m_vecTeammates;
	CGUID m_CaptainID;
	string m_strTeamname;
	CGUID m_TeamID;
	bool m_bIsRecruitingTeam;
	long m_lTeammateAmount;
	bool m_bTeamHasPassword;
	long m_lLimtSkillID;				// 组队限制技能ID
	long m_lLimtSkillLev;				// 组队限制技能的等级
	long m_lAllotMade;					// 队伍的分配方式

private:
	vector<tagPlayerInfo> m_vecPlayerInfo;
	//-----------07.8.3(liuke)----------
	vector<tagWSMemberInfo> m_vecTeamMateInfo;// 队伍成员的基本信息
	//vector<tagMemberIdioinfo> m_vecMemberInfo;// 成员详细信息
	void DecordRecruitDataFromByteArray(BYTE* pByte, long &pos);		// 招募信息
	void DecodeRecruitDataFromDataBlock(DBReadSet& setReadDB);
	//----------------------------------

public:
	vector<tagPlayerInfo> & GetPlayerInfo(){return m_vecPlayerInfo;}

public:
	//-----------07.8.3添加新的队友信息列表(liuke)-----------
	vector<tagWSMemberInfo> & GetTeamMateInfo() { return m_vecTeamMateInfo; }
	//-------------------------------------------------------
	vector<tagTeammate> & GetTeam() { return m_vecTeammates; }
	// 设置和获得队伍的经验分配方式
	void SetAllotMade(long lMode)	{m_lAllotMade = lMode;}
	long GetAllotMade()				{return m_lAllotMade;}
	// 察看指定的玩家是否是本队伍的
	bool IsTeammate(const CGUID& id);
	// 察看指定的玩家是否是本队伍的
	bool IsTeammate(const char *szName);
	// 获得指定ID成员的信息
	tagWSMemberInfo * GetWsMemberInfo(const CGUID& id);
	// 获得和设置队长
	const CGUID& GetCaptainID() const { return m_CaptainID; }
	void SetCaptainID(CGUID& id);// { m_CaptainID = id; }
	// 获得和设置队伍
	const CGUID& GetTeamID(){return m_TeamID;}
	void SetTeamID(CGUID& ID){m_TeamID=ID;}
	// 获得和设置招募状态
	bool IsRecruitingTeam(){return m_bIsRecruitingTeam;}
	void SetRecruitingTeam(bool b){m_bIsRecruitingTeam=b;}
	// 获得和设置队伍成员的数量
	long GetTeammateAmount(){return m_lTeammateAmount;}
	void SetTeammateAmount(long l){m_lTeammateAmount=l;}
	// 获得和设置队伍的名字
	const char * GetTeamname(){return m_strTeamname.c_str();}
	void SetTeamname(const char *strName){m_strTeamname=strName;}
	// 获得和设置队伍是否存在密码
	bool IsTeamHasPassword(){return m_bTeamHasPassword;}
	void SetTeamHasPassword(bool b){m_bTeamHasPassword=b;}
	// 是否可以操作组队相关
	bool IsOperforTeam();
	// 设置组队限制技能
	void SetLimtSkill(long ID, long lLev)	{m_lLimtSkillID = ID;m_lLimtSkillLev = lLev;}

	//////////////////////////////////////////////////////////////////////////
	//	技能
	//////////////////////////////////////////////////////////////////////////
public:
	bool AddSkill(long lSkillId, WORD wLevel=1, DWORD dwRestoreTime=0, float RangeMin=1.0f, float fRangMax=1.0f, WORD wCostMP=0);		// 添加技能
	bool DelSkill(DWORD lSkillId);						// 删除技能
	bool DelSkill(long lPos);								// 删除技能（位置)
	tagSkill*		GetSkill(DWORD dwID);						// 获取技能
	bool SetSkillLevel(const char* strName, WORD wLevel, DWORD dwRestoreTime=0, float RangeMin=1.0f, float RangMax=1.0f, WORD wCostMP=0);	// 设置技能等级
	WORD CheckSkill(DWORD dwIndex);							// 检测是否有指定技能，返回等级 (0代表没有)
	void UpdateSkillCoolDownTime(bool bIsPublicTime=false);	// 刷新技能冷却时间(参数标示是否刷新公共冷却时间)
	void SortSkillList();									// 为技能列表重新排序
	/***********************************************************************/
	/*  zhaohang 2010-10-21 add*/
	void UpdateSkillAttr(long skillid,long level,
		const char* strKey,long value);						//更新技能属性
	/***********************************************************************/
	////////////////////07.12.17 XY添加操作消耗性物品，设置其冷却效果的接口///////////////////
	list<tagShortCutGoods>* GetShortCutGoodsList() {return &m_listShortCutGoods;}	
	bool AddShortCutGoods(ulong wGoodsIndex);				//添加物编号为wGoodsIndex的物品到列表中
	bool DelShortCutGoods(ulong wGoodsIndex);				//删除物编号为wGoodsIndex的物品
	void SetGoodsRestoreTime(ulong wGoodsIndex,ulong dwTime);		//设置物编号为wGoodsIndex的物品的冷却时间
	void SetGoodsCoolDownStartTime(ulong wGoodsIndex,ulong dwTime); //设置物编号为wGoodsIndex的物品的冷却开始时间
	ulong GetGoodsRestoreTime(ulong wGoodsIndex);					//得到物编号为wGoodsIndex的物品的冷却时间
	ulong GetGoodsCoolDownStartTime(ulong wGoodsIndex);		//得到物编号为wGoodsIndex的物品的冷却开始时间
	void SetGoodsUseState(ulong wGoodsIndex,bool bState);	//设置物编号为wGoodsIndex的物品当前是否可被使用的状态
	bool GetGoodsUseState(ulong wGoodsIndex);				//取得物编号为wGoodsIndex的物品当前是否可被使用的状态

	///设置物编号为wGoodsIndex的物品的剩余冷却时间
	void SetGoodsRemainTime(ulong wGoodsIndex,ulong dwTime);
	///得到物编号为wGoodsIndex的物品的冷却时间
	DWORD GetGoodsRemainTime(ulong wGoodsIndex);

	///添加物品公共冷却类型
	bool AddPublicCooldownType(WORD wType,DWORD dwTime);

	///获取某个类型的物品公共冷却剩余时间
	DWORD GetReTimeOfPCooldownType(WORD wType);
	////////////////////////////////////////////////////////////////////////////

	//------------------07.11.22liuke技能学习条件判断--------------
	bool IsCanStudySkill(DWORD dwSkillID, WORD wLevel);		// 根据指定的想提升的技能和等级，判断是否能学习
	static WORD m_wPublicCoolTime;							// 公用冷却时间（使用技能或消耗道具之后的基本冷却时间）
	//------------------------------------------------------------------------
	// GM权限标志
	//------------------------------------------------------------------------
private:
	
	long m_lGMLevel;
public:
	void SetGMLevel( long lLevel ) {m_lGMLevel = lLevel;}
	long GetGMLevel() {return m_lGMLevel;}

public:
	struct tagownedReg
	{
		long lRegionID;						//场景ID
		WORD wType;							//场景类型
	};
	//行会系统
private:
	CGUID	m_FactionID;					//自己行会ID
	string	m_strFactionName;				//行会名字
	string	m_strFactionTitle;				//行会的称号
	CGUID	m_FacMasterID;					//在所在行会的帮主ID;
	long	m_lFactionJob;					//行会官阶
	CGUID	m_UnionID;						//自己所在的同盟ID
	set<CGUID,guid_compare> m_EnemyFactions;				//敌对行会列表
	set<CGUID,guid_compare> m_CityWarEnemyFactions;		//城战的敌对行会列表
	list<tagownedReg> m_OwnedRegions;		//拥有的场景列表

	bool	m_bContendState;				//是否出于争夺状态
	bool	m_lCityWarDiedState;			//城战死亡状态

	DWORD	m_dwFacIconCRC;					//行会图标CRC

public:
	const CGUID& GetFactionID()			{return m_FactionID;}
	void	SetFactionID(const CGUID& ID)	{m_FactionID=ID;	}

	const string& GetFactionName() {return m_strFactionName;	}
	void	SetFactionName(string& strName){m_strFactionName=strName;}

	const string& GetFactionTitle() {return m_strFactionTitle;	}
	void	SetFactionTitle(string& strTitle){m_strFactionTitle=strTitle;}

	const CGUID& GetFactionMasterID()	{return m_FacMasterID;}
	void	SetFacMasterID(const CGUID& id){m_FacMasterID = id;}

	long	GetFactionJob()				{return m_lFactionJob;}
	void	SetFactionJob(long lFacJob)	{m_lFactionJob = lFacJob;}

	bool	GetIsContendState()		{return m_bContendState;}
	void	SetContendState(bool b)	{m_bContendState = b;}

	bool	GetCityWarDiedState()	{return m_lCityWarDiedState;}
	void	SetCityWarDiedState(bool b)	{m_lCityWarDiedState = b;}

	DWORD	GetFacIconCRC()						{return m_dwFacIconCRC;}
	void	SetFacIconCRC(DWORD dwFacIconCRC)	{m_dwFacIconCRC = dwFacIconCRC;}

	const CGUID&	GetUnionID()			{return m_UnionID;}
	void	SetUnionID(const CGUID& ID)	{m_UnionID=ID;}

	void	SetEnemyFactions(set<CGUID,guid_compare>& Factions) { m_EnemyFactions = Factions; }
	bool	IsHaveEnemyFactions()		{return m_EnemyFactions.size() > 0;}
	bool	IsEnemyFaction(const CGUID& FactionID)
	{
		return m_EnemyFactions.find(FactionID) != m_EnemyFactions.end();
	}
	bool	IsHaveCityWarEnemyFactions(){return m_CityWarEnemyFactions.size()>0;}
	bool	IsCityWarEnemyFaction(const CGUID& ID)
	{
		return m_CityWarEnemyFactions.find(ID) != m_CityWarEnemyFactions.end();
	}
	bool	IsFriendFaction(const CGUID& FactionID,const CGUID& UnionID)
	{
		if(FactionID == CGUID::GUID_INVALID || GetFactionID() == CGUID::GUID_INVALID )	return false;
		if(FactionID == GetFactionID())	return true;
		if(UnionID == CGUID::GUID_INVALID  || GetUnionID() == CGUID::GUID_INVALID )	return false;
		if(UnionID == GetUnionID())	return true;
		return false;
	}
	WORD GetDisplayOwneRegionIcon();							//得到显示拥有场景的IcongID
	//任务系统-----------------07.9.6liuke修改--------------------
public:
	// 记录玩家的任务中与场景信息相关的特效图标
	struct stQuestInfoByRgn
	{
		long lQuestID;
		long lIndex;
		long lRgnID;
		long lRgnX;
		long lRgnY;
		long lEffectIDOK;
		long lEffectIDNO;
		CGUID guidEffect;
		stQuestInfoByRgn()
		{
			lQuestID	= 0;
			lIndex		= 0;
			lRgnID		= 0;
			lRgnX		= 0;
			lRgnY		= 0;
			lEffectIDOK = 0;
			lEffectIDNO = 0;
			guidEffect	= CGUID::GUID_INVALID;
		}
	};
	list<stQuestInfoByRgn>* GetQuestInfoByRgn(){return &m_lsQuestInfoByRgn;} // 任务相关的场景信息列表
	CQuestQueue * GetRoleQueue()	{return &m_RoleQueue;}
	CMercenaryQuestSys* GetMercQuestSys() {return &m_MerQuestSys;}

	bool AddQuest(long lQuestID, long lStepID);					// 添加指定步骤的一个任务
	const char * RemoveQuest(long lQuestID);					// 删除一个任务
	bool UpdataQuestStep(long lQuestID, long lStepIndex);		// 更新一个任务步骤
	bool UpdataQuestTarget(long lQuestID, long lIndex, long lValue,long lRegionID=0, long x=0, long y=0);	// 更新一个任务的任务追踪
	void CollectionQuestClewInfo(CMonster *pNpc);				// 采集场景上NPC对应的任务提示信息
	void UpdataSpecialQuestInfoByNpc(CMonster *pNpc);			// 更新场景上的特殊任务相关NPC的提示信息
	void UpdataQuestInfoAboutNpc(CMonster *pNpc=NULL);			// 更新指定NPC，在玩家任务列表中的相关任务提示信息(默认为更新场景中所见的所有NPC的任务提示状态)
	void UpdataQuestInfoAboutNpcByQuestID(long lQuestID);		// 更新指定任务的NPC的相关任务提示信息

	void UpdataQuestByGoods(CGoods *pGoods);					// 物品变化引起的任务更新

	void SetQuestInfoByRgn(long lQuestID, long lIndex, stQuestInfoByRgn tagInfo);// 设置指定任务的指定序列的地图特效
	void DelQuestInfoByRgn(long lQuestID, long lIndex);			// 删除指定任务的指定序列的地图特效
	void DelAllQuestInfoByRgn(long lQuestID);					// 删除指定任务的所有地图特效
	//****************************刘科08.11.11***************************//
	void TidySearchQuestList();									// 整理可搜索的任务列表（达到可接条件的所有任务）
	vector<CQuest *> GetCanSearchQuest()	{return m_vecCanSearchQuest;}	// 获得当前任务符合搜索条件的任务列表
	CQuest * RedomCanSearchQuest(bool bIsReSearch=false);					// 随机获得一个符合搜索条件的任务(参数表示是否更新任务搜索列表)
	//*******************************************************************//
private:
    // friend CEGUI::Window* InitQuest();
    // friend bool OnQuestTabSelectChanged(const CEGUI::EventArgs& args);
	CQuestQueue m_RoleQueue;		// 任务列表对象
	vector<CQuest *>	m_vecCanSearchQuest;					// 当前任务符合搜索条件的任务列表
	//俑兵任务
	CMercenaryQuestSys	m_MerQuestSys;
	list<stQuestInfoByRgn> m_lsQuestInfoByRgn;					// 任务相关的场景信息列表

	////-------------------------------07.9.6liuke注释-------------------------------
	//public:
	//	struct tagPlayerQuest
	//	{
	//		WORD	wQuestID;			//任务的ID
	//		bool	bDisplay;			//是否显示
	//		long	lEffectID;			//任务标示特效ID
	//		long	lRegionID;			//任务标记特效位置
	//		long	lTileX;
	//		long	lTileY;
	//		CEffect*	pEffect;
	//
	//		string	strName;			//任务名字
	//		string	strDesc;			//任务描述
	//	};
	//private:
	//	long						m_lMaxQuestNum;	//最大任务数量
	//	map<WORD,tagPlayerQuest>	m_PlayerQuests;	//玩家已获得的任务列表
	//
	//public:
	//	bool DecordQuestDataFromByteArray(BYTE* pByte, long& pos);
	//
	//	map<WORD,tagPlayerQuest> * GetPlayerQuests(){return &m_PlayerQuests;}
	//	long GetMaxQuestNum()		{return m_lMaxQuestNum;}
	//	void AddQuest(WORD wQuestID,char* strName,char* strDesc,bool bDisplay,long lEffectID,long lRegionID,long lTileX,long lTileY);//给该玩家添加一个任务
	//	void RemoveQuest(WORD wQuestID);							//移出一个任务
	//	void UpdateQuestPosXY(WORD wQuestID,long lReginID,long lTileX,long lTileY);
	//===============================================================================


    // 宠物代码注释 Bugs 2008-9-12
//	//宠物相关
//private:
//	set<CGUID> m_setPetID;
//public:
//	set<CGUID> *GetPetIDSet()	{return &m_setPetID;}
//	bool HasPetArount();		//判断周围有没有自己的宠物
//	long GetPetAmount();		//查找自己周围宠物的数量


	//表情
private:
	CEmotionShower *m_pEmotionShower;
public:
	virtual void SetAction(WORD l);
	virtual void SetState(WORD l);
	void SetEmotion(long lIndex,DWORD dwTime=0);
	bool IsEmotionShowing(); 
	///////////////////////////////////////////////////////////////
	//座骑
	///////////////////////////////////////////////////////////////
private:
	CHorse	   m_Horse;
	CTalisman  m_Talisman;
	CWing	   m_Wing;
	CHeadWear  m_HeadWear;
	CWeapon    m_LeftHandWeapon;
	CWeapon    m_RightHandWeapon;
	CWeapon    m_REnergy;
	CWeapon    m_LEnergy;
	CWeapon    m_CollectTool;
	BOOL       m_bPlayEnergyEffect;
	BOOL       m_bEnergyTimeStarted;
	DWORD      m_dwEnergyHitEffectID;
	DWORD      m_dwEnergyStartTime;
public:
	CHorse * GetHorse() { 
		return &m_Horse;
	}
	CTalisman * GetTalisman() { return &m_Talisman;}
	void UpdateSpecialEquipment();
	CWing * GetWing() { return &m_Wing;}
	CHeadWear * GetHeadWear() { return &m_HeadWear;}
	CWeapon * GetLeftHandWeapon() { return &m_LeftHandWeapon;}
	CWeapon * GetRightHandWeapon() { return &m_RightHandWeapon;}
	CWeapon * GetCollectTool() { return &m_CollectTool;}
	CWeapon * GetLEnergy() { return &m_LEnergy;}
	CWeapon * GetREnergy() { return &m_REnergy;}
	void      PlayEnergyEffect(DWORD energylevle);
	void      EndEnergyEffect();
	//根据充能强度更新充能特效
	void      UpdateEnergyEffect(DWORD energylevle);
	void      SetEnergyHitEffectID(DWORD ID ){m_dwEnergyHitEffectID = ID;}
	DWORD     GetEnergyHitEffectID(){return m_dwEnergyHitEffectID;}

	void SetGreatcoatVisible(BOOL bIsVisible);			//设定外套的可见性
	void SetManteauVisible(BOOL bIsVisible);			//设定披风的可见性

private:
	DWORD	m_dwMoney;				//	金币
	DWORD	m_dwSilverMoney;		//	银币
	DWORD	m_dwBaoShi;
	DWORD	m_dwYuanBao;
	DWORD	m_dwJiFen;
public:
	DWORD	GetMoney()					{return m_dwMoney;}
	void	SetMoney(DWORD l)			{m_dwMoney=l;}
	DWORD	GetSilverMoney()			{return m_dwSilverMoney;}
	void	SetSilverMoney(DWORD l)		{m_dwSilverMoney=l;}
	DWORD	GetBaoShi()					{return m_dwBaoShi;}
	void	SetBaoShi(DWORD l)			{m_dwBaoShi=l;}
	DWORD	GetYuanBao()				{return m_dwYuanBao;}
	void	SetYuanBao(DWORD l)			{m_dwYuanBao=l;}
	DWORD	GetJiFen()					{return m_dwJiFen;}
	void	SetJiFen(DWORD l)			{m_dwJiFen=l;}
	//当前的称号
private:
	long    m_lTitleType;                       //玩家当前使用的称号类型
	string  m_pPlayerTitle;                      //玩家当前使用的称号的值
	vector<DWORD>  m_VecMedalOnPlayer;          //玩家拥有的勋章索引
public:
	void SetPlayerTitle(string strTitle) {m_pPlayerTitle = strTitle;}                // 设置玩家当前的称号
	//string GetPlayerTitle()  {return m_pPlayerTitle;};                                  // 获得玩家当前的称号
	string GetPlayerTitle();                                 // 获得玩家当前的称号
	void SetTitleType(long nTitleType) {m_lTitleType = nTitleType;}                    // 设置当前称号的类型
	long GetTitleType() {return m_lTitleType;}                                         // 获得当前称号的类型
	void DecodeMedalFromDataBlockAndInit(DBReadSet& setReadDB);                             // 解析角色的勋章列表
	vector<DWORD> GetVecMedalOnPlayer() { return m_VecMedalOnPlayer;};                 // 获得角色的勋章
	void AddMedal(DWORD Index);          //添加一个勋章
	void DeleteMedal(DWORD Index);       //删除一个勋章
	bool FindMedalOnPlayer(DWORD Index);  //根据物编查找玩家身上的勋章

	void UpdateAccouterment();					//更新饰品

	//宠物功能
private:
	map<CGUID, CPet*> m_PetList;	///< 所有宠物列表
	set<CGUID> m_ShowPets;			///< 展示宠物列表
	CGUID m_ActivePetGUID;			///< 激活宠物GUID
	CGUID m_CurPetGUID;				///< 当前捕获宠物GUID
	CGUID m_CurOperPetGUID;			///< 当前操作宠物GUID
	
public:
	/// 取得玩家宠物列表
	map<CGUID, CPet*>* GetPetList() { return &m_PetList; }
	/// 取得玩家拥有宠物数目
	long GetPetNum(void) { return (long)m_PetList.size(); }
	/// 取得激活宠物GUID
	const CGUID& GetActivePetGUID(void) { return m_ActivePetGUID; }
	/// 设置激活宠物GUID
	void SetActivePetGUID(CGUID guid) { m_ActivePetGUID = guid; }
    /// 取得激活宠物
    CPet* GetActivePet() const;
	/// 创建宠物
	CPet* CreatePet(const CGUID& guid);
	/// 删除宠物
	void DelPet(const CGUID& guid);
	/// 展示宠物
	void ShowPet(const CGUID& guid);
	/// 收回宠物
	void UnShowPet(const CGUID& guid);
	/// 根据宠物GUID取得宠物
	CPet* GetPet(const CGUID& guid);
	/// 宠物解码
	void DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData);
	/// 添加宠物
	void AddPet(CPet *pPet);
	/// 取得出战状态宠物数目
	long GetFightPetNum();
	/// 设置宠物为出战状态
	void SetPetFighting(CPet *pPet);
	/// 宠物进行场景
	void PetEnterRegion();
	/// 保存宠物技能设置
	void SavePetSkillSetting();

	CPet* GetCurPet() { return GetPet(m_CurPetGUID); }
	void SetCurPetGUID(const CGUID& guid) {m_CurPetGUID = guid;}

	CPet* GetCurOperPet() { return GetPet(m_CurOperPetGUID); }
	void SetCurOperPetGUID(const CGUID& guid);

	void ValidatePetInterface();				//效验当前宠物相关界面打开或关闭
private:
	/// 清除玩家宠物列表
	void RemoveAllPet();


//////////////////////////////////////////////////////////////////////////
// 玩家经验提示显示
//////////////////////////////////////////////////////////////////////////
public:
	enum TIPTEXT_TYPE
	{
		TT_EXP,			//经验值类型提示文本
		TT_CON,			//功勋值类型提示文本
	};

	struct tagTipText
	{
		TIPTEXT_TYPE eType;			//提示类型
		string		 sNumber;		//数字
		float		 fAlpha;		//透明度
		DWORD		 dwStartTime;	//开始时间
		D3DXVECTOR3  vPos;			//位置坐标
};

	void AddTipText(TIPTEXT_TYPE eType, long lNumber);		//添加提示文本
	void DisplayTipText();									//显示玩家的提示文本

private:
	vector<tagTipText> m_vecTipText;	//游戏中玩家获得经验和功勋提示表
	DWORD m_dwLevelCurrentDisplayNum;	//当前正在显示的层级数量
	DWORD m_dwLevelMax;					//最大层级(层级从0开始,由下往上累加)

	long m_lOldContribute;		//玩家上次的功勋值

public:
	void SetOldContribute(long lOldContribute)		{ m_lOldContribute = lOldContribute; };		//设定玩家上次的功勋值
};



//表情显示类
class CEmotionShower
{
public:
	//表情结构
	struct tagEmotion
	{
		DWORD dwType;
		DWORD dwIndex;
		int nSoundLoops;
		int nSoundChannel;
		char sounds[128];

		vector<DWORD>vecActions;
		vector<DWORD>vecTypes;
		string strName;
		string strText;
		string strSound;
	};
private:
	CPlayer *m_pSource;
	int m_nCurIndex;
	tagEmotion m_stEmotion;
	//CDisplayModel::AnimInfo::tagActionInfo *m_pActionInfo;
public:
	CEmotionShower(CPlayer *m_pSource);
	~CEmotionShower();
	//void SetActionInfo(GameModel::AnimInfo::tagActionInfo *pActionInfo);
	bool IsStopped();
	void Init(int emotionType);
	void Start(DWORD dwStartTime=0);
	void Stop();
	void Update();

private:
	void ShowNextAction();
	void RestoreToNormal();
};