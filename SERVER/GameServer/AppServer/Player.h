#pragma once
//#define MAX_MONEY 4000000000
#include "moveshape.h"
#include "..\..\setup\playerlist.h"
#include "..\..\setup\IncrementShopList.h"

#include "Container\CAmountLimitGoodsContainer.h"
#include "Container\CAmountLimitGoodsShadowContainer.h"
#include "Container\CVolumeLimitGoodsContainer.h"
#include "Container\CEquipmentContainer.h"
#include "Container\CSubpackContainer.h"
#include "Container\CWallet.h"
#include "container\CSilverWallet.h"

#include "..\..\..\public\PlayerDef.h"
#include "..\..\..\public\BasePetServerDef.h"
#include "..\..\..\public\RoleRecord.h"
#include "PlayerDepot.h"
#include "..\..\public\DeputyOccu.h"
#include "..\..\public\DeputyOccuRecord.h"
#include "..\public\OrganizingDef.h"
#include "..\public\StrType.h"
#include "SpriteSystem.h"
#include "..\public\PlayerSetupOnServer.h"
#include "..\public\teamdef.h"
#include "..\public\Cosmetology.h"
#include "MedalContainer.h"
#include "Mail/MailManager.h"
#include "business\CBusinessPackage.h"
#include "GoodsAllPurposeOpt.h"
#include "GoodsTimerHandler.h"
#include "..\..\public\QuestIndexXml.h"
#include "..\..\..\public\FunctionKeyMapSystem.h"
#include "Lottery\CLottery.h"

#include <set>
#include <map>

using namespace std;

class CGoods;
class CMoveShape;
class CVariableList;
class CMyServerClient;
class CMonster;
class CQuest;
class CQuestStep;
class CQuestAim;
class CQusetEncouragement;
class CScript;
struct tagTestResult;
struct tagDelTestResult;
//移动验证时候,接受移动消息的最大个数
const static int MoveValiMaxMsgCount = 15;



//死亡死后损失经验和掉落物品的几率
struct tagDiedLost
{	
	float fDropEquip_Head;//(头盔掉落比例)
	float fDropEquip_Necklace;//(项链掉落比例)	
	float fDropEquip_Wing;//(翅膀比例)	
	float fDropEquip_Body;//(盔甲掉落比例)	
	float fDropEquip_Back;//(腰带掉落比例)	
	float fDropEquip_Glove;//(手套掉落比例)	
	float fDropEquip_Boot;//(鞋子掉落比例)	
	float fDropEquip_Headgear;//(头饰掉落比例)
	float fDropEquip_Frock;//(外套掉落比例)	
	float fDropEquip_LRing;//(左戒指掉落比例)
	float fDropEquip_RRing;//(右戒指掉落比例)
	float fDropEquip_Medal1;//(勋章1掉落比例)
	float fDropEquip_Medal2;//(勋章2掉落比例)
	float fDropEquip_Medal3;//(勋章3掉落比例)
	float fDropEquip_Fairy;//(小精灵掉落比例)
	float fDropEquip_Weapon;//(武器掉落比例)
	float fDropEquip_Weapon2;//(副手武器掉落比例)	
	float fDropEquip_Cloak;//(披风掉落比例)	
	float fDropEquip_Horse;//(坐骑掉落比例)
	float fDropEquip_Bag;//(背包物品掉落比例)
	float fDropEquip_Money;//(金钱掉落比例)	
	float fDropEquip_MoneyPercent;//(掉金钱数量比例)
};

class CPacketListener : public CContainerListener
{
public:
	DBWriteSet* m_pDBWriteData;
	CPacketListener();
	~CPacketListener();

	virtual BOOL OnTraversingContainer(CContainer* pContainer, CBaseObject* pObject);
};

class CPlayer :
	public CMoveShape, public CContainerListener
{
public:
	CPlayer(void);
	virtual ~CPlayer(void);

public:
	// 人物技能
	struct tagSkill
	{
		DWORD	dwID;					//编号
		WORD	wLevel;					//等级
		DWORD	wCooldown;				//冷却时间
		DWORD   dwOldCooldown;			//原始冷却时间

		bool	bCommonCooldown;		//是否受公共冷却时间影响
		DWORD	dwCooldownStartTime;	//冷却开始时间
		time_t	tCooldownEndtime;		//冷却结束时间
	};

	struct tagSuitGoods
	{
		BOOL bActived;
		CGoods* pGoods;
	};

	struct tagExpendableEffect
	{
		DWORD dwType;	//附加属性的编号
		DWORD dwStartTime;	//开始作用时间
		DWORD dwEffectTime;	//有效时间
		long  lValue;	//作用值
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
		SKILL_NOTFIT,		//技能不对
		REGION_FORBID,		//场景禁止
		COOLDOWN_NOTFIT,	//冷却不够
		EQUIP_TYPE_NOTFIT,	//防具类型不对
		BUSINESS_STATE,		//处于跑商状态
		EQUIPFIT,			//可以装备
	};
	//战斗状态下pk值和pvp值不减小的临界值
	enum UNDECPKPVP
	{
		eUnDecPKPVP_Value = 20,
	};
	//恶名值的区间值
	enum PVPLEVEL
	{
		ePvp_First_Level = 36000,
		ePvp_Second_Level = 72000,
		ePvp_Third_Level = 144000,
	};

	//////////////////////////////////////////////////////////////////////////
	/*修改人物属性															*/
	//////////////////////////////////////////////////////////////////////////
public:
	tagPlayerProperty& GetProperty(void) { return m_Property; }
private:

	//人物属性
	tagPlayerProperty			m_Property;			//人物属性
	tagPlayerFightProperty		m_FightProperty;		//战斗属性
	tagPlayerFightProperty		m_BaseFightProperty;	//基本战斗属性
	tagPlayerAppendProperty		m_AppendProperty;		//附加属性
	tagPlayerElementProperty	m_arrElementProperty[MT_Max];//元素属性

	MAPFKEY m_FuncKey;		// 保存角色的功能热键映射表（玩家修改过的，与默认值不同）

	float	m_fExpScale;//获得经验比例
	float	m_fOccuExpScale;// 获得职业经验系数
	float	m_fSpScale;//获取元气的比例
	float	m_fPresentExpScale;//获取赠送经验的比例
	float	m_fActiveScale;			//活力值倍率

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// 脏属性数据 在向客户端发送时重新获取最新数据
	set<string>  m_mapDirtyAttrData;
	map<string, P_ATTRIBUTE_TYPE>		    m_mapAttrType;		// 变量名字对应的枚举值

	//表示玩家是否已经进入游戏
	bool m_bAlreadyEnterGame;

	void    PushDirtyAttrData(string attrName) { m_mapDirtyAttrData.insert(attrName); }
	string  PopDirtyAttrData() 
	{ 
		string tmp = *m_mapDirtyAttrData.begin(); 
		m_mapDirtyAttrData.erase(m_mapDirtyAttrData.begin());
		return tmp; 
	}
	long    DirtyAttrDataSize(void) { return m_mapDirtyAttrData.size(); }

	typedef map<string, tagPty>::iterator mapNameValueItr; 
	typedef map<string, P_ATTRIBUTE_TYPE>::iterator mapAttrTypeItr;

	// 周围需要更新的数据
	static set<string>  m_mapOtherUpdateDirtyAttrData;
	set<string>			m_mapOtherDirtyAttrData;

	void InsertOtherDirtyName(string& attrName)
	{
		if(m_mapOtherUpdateDirtyAttrData.find(attrName) != m_mapOtherUpdateDirtyAttrData.end())
			m_mapOtherDirtyAttrData.insert(attrName);
	}

	void SetEnterGame(bool b)	{m_bAlreadyEnterGame = b;}
	bool IsEnterGame()	{return m_bAlreadyEnterGame;}
	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
public:
	static map<GOODS_ADDON_PROPERTIES,string> m_mapFightEnum;
	static VOID InitFightEnumMap();
	static string GetFightPropertyNameByEnum(GOODS_ADDON_PROPERTIES);

	// [070608 AHC]
	virtual bool  SetAttribute(std::string& attrName, long value);
	virtual bool  SetAttribute(char* attrName, long value);
	virtual long  GetAttribute(std::string attrName);
	virtual bool  ChangeAttribute(std::string& attrName, long value);
	virtual long  UpdateAttributeToClient();
	virtual long  UpdateHPMPToClient(set<string>& dirtyAttr);
	long	UpdateOtherAttributeToClient();

	long  UpdateAttribute(BOOL bImmediately=0);
	long  OnUpdateAttributeTimeOut();

	virtual VOID  ChangeStateTo(CShape::eSTATE state);

	void          CodeChangeAttrToDataBlock(DBWriteSet& setWriteDB);
	bool          CodeOtherUpdateAttrToDataBlock(DBWriteSet& setWriteDB);
	bool	CodeDirtyAttrToDataBlock(set<string>& dirtyAttr, DBWriteSet& setWriteDB);

	void		  AddFightPropertyTODirty();
	//添加变量名到胀数据
	void  AddAttributeToDirty(string attrName);

	BOOL SerializeSubDepot(DBWriteSet& setWriteDB, LONG eContainerId);

	// 取得当前值的合法值
	virtual long  GetCurrentValue(char* valueName, const tagPty& tagValue, long evalue/*改变值*/, long tmpValue/*初次计算得到的只*/);

	VOID ChangePropertyByGoods(CGoods* pGoods,GOODS_ADDON_PROPERTIES addon,int whichVal,string strAttrName,bool bMount);


	VOID UpdatePropertyByGoodsTrigger(CGoods* pGoods,bool bMount);

	//改变职业
	void ChangeOccu(eOccupation eNewOccu);		

	//玩家属性初始化
	void InitProperty(void);
	//初始化赠送经验值
	void InitPresentExp(void);
	//重设战斗属性中的力量、敏捷、体质、智利、智慧、精神、生命值、法力值等基本属性
	void ResetBaseFightProperty(void);
	//重新计算万家属性,当力量,敏捷,体质,智利,智慧,精神的基本属性和变化属性发生改变的时候，都要重新计算格挡,招架,闪避等基本属性
	void ResetFightProperty(void);
	//因pk值/pvp值成一定比例触发一个脚本
	void OnPKOrPVPValueChange(long lOldValue,long lNewValue);

	//SCRIPT FUNTIONS
	bool IsGM();
	long GetGMLevel();

	// 检测玩家的TileX,Y是否在指定范围内 (一个正方行内)
	// x,y中心坐标坐标，range跨度，
	//
	//	■■■■■■■
	//	■p1■■■■■	r
	//	■■■■■■■	a
	//	■■■xy■■■	n = 3
	//	■■■■■■■	g
	//	■■■■■■■	e
	//	■■■■■■■
	//		p2
	//
	// p1 在区域内， p2 在区域外
	bool IsInArea(/*long regionid,*/ long x, long y, long range = 3);
	// 检测场景
	bool IsInRegion(long regionid);

public:
	struct tagShopSelledGoods 
	{
		DWORD dwSelledPrice;
		CGoods* pGoods;
	};

private:
	//最后拾取物品时间
	long m_lLastPickGoodsTime;
	//声望<贸易区ID，声望值>
	map<DWORD,DWORD> m_mapCredit;
	//投资记录<贸易点ID，投资额>
	map<DWORD,DWORD> m_mapInvestRecord;
	long m_lInvestLimitTimeMark;

	//临时保存玩家卖到NPC商店的物品，用于物品回购功能
	list<tagShopSelledGoods> m_listNpcShopSellGoods;
	CGUID m_TradeSessionId;
	LONG m_lTradePlugId;
	VOID SetTradeSessionId(const CGUID& sessionId){m_TradeSessionId=sessionId;}
	VOID SetTradePlugId(const LONG lId){m_lTradePlugId=lId;}
public:
	const CGUID& GetTradeSessionId(){return m_TradeSessionId;}
	const LONG GetTradePlugId(){return m_lTradePlugId;}
public:
	VOID SetCredit(DWORD dwAreaId,DWORD dwCredit);
	VOID AddCredit(DWORD dwAreaId,DWORD dwCredit);
	VOID ChangeCredit(DWORD dwAreaId,long lCredit);
	DWORD GetCredit(DWORD dwAreaId);
	BOOL SetInvestRecord(DWORD dwSpotId,DWORD dwInvestVal);
	DWORD GetInvestRecord(DWORD dwSpotId);
	VOID ClearInvestRecord(){m_mapInvestRecord.clear();}
	VOID SetInvestLimitTimeMark(long lVal){m_lInvestLimitTimeMark=lVal;}
	long GetInvestLimitTimeMark(){return m_lInvestLimitTimeMark;}

	VOID PushbakNpcShopSellGoods(CGoods* pGoods,DWORD dwPrice);
	CGoods* FetchNpcShopSellGoods(const CGUID& guid);
	CGoods* FindNpcShopSellGoods(CGUID& guid,DWORD* dwPrice);
	VOID ReleaseNpcShopSellGoods();

	VOID SetLastPickGoodsTime(long t){m_lLastPickGoodsTime=t;}
	long GetLastPickGoodsTime(){return m_lLastPickGoodsTime;}

	BOOL UpdateCooldown(DWORD dwGoodsId);

	long GetCreditLevel(DWORD dwAreaID); //获取指定区域的声望等级
	long GetOrganLevel(const char* strName);//获取指定组织的声望等级
	void AddOrganCredit(const char* strName,DWORD dwCredit);//增加组织声望
	void ChangeOrganCredit(const char* strName,DWORD dwCredit);//修改组织声望
	long GetOrganCredit(DWORD dwOrganID);//获取组织声望
private:
	//背包
	CVolumeLimitGoodsContainer* m_cOriginPack;
	CSubpackContainer* m_pSubpackContainer;
	VOID gcPacks();		
	map<long,vector<tagSuitGoods>*> m_mSuitState;
	vector<CGoods*> m_vecInPersonalShopGoods;
	DWORD m_dwYuanbao;
	/// 商业背包
	Business::CPackage m_BusinessContainer;
public:	
	/// 获取商业背包
	Business::CPackage &GetBusinessContainer() { return m_BusinessContainer; }
	/// 判断该玩家是否是商人
	bool IsBusinessMan() const { return m_Property.byIsBusinessMan == 1; }
	/// 改变商人身份
	void SetBusinessMan( bool bBusinessMan ) { m_Property.byIsBusinessMan = ( bBusinessMan ? 1 : 0 ); }
	/// 检查商人是否处于跑商状态
	bool IsBusinessState() 
	{
		return IsBusinessMan() && m_BusinessContainer.GetCurGoodsAmount() != 0;
	}

	CVolumeLimitGoodsContainer* getOriginPack(){return m_cOriginPack;}
	CSubpackContainer* getSubpackContainer(){return m_pSubpackContainer;}
	CEquipmentContainer* GetEquipmentContainer(){return &m_cEquipment;} 
	CVolumeLimitGoodsContainer* GetPackContainerById(DWORD dwId);
	int GetSubpackCanPlace();
	VOID OnSuitNumChanged(long suit_id,int suit_num);
	CGoods* FindGoods(LONG lContainerId,LONG lPos);
	//根据物品原始名在所有背包中查找第一个物品
	CGoods* FindGoodInPack(const char* strGoodsName);
	//! 根据物品原始名在仓库中找到第一个物品
	CGoods* FindGoodInDepot(const char* strGoodsName);

	BOOL IsGoodsUnique(const char* strGoodsName);

	BOOL DelGoods(LONG lContainerId,LONG lPos);

	VOID ClearGoods(LONG lContainerId,LONG lPos);

	//! 找到指定原始名、数量的[未锁定]物品
	BOOL FindGoods(char* strOriginName,int num);
	//! 删除指定原始名、数量的[未锁定]物品
	BOOL DelGoods(char* strOriginName,int num);
	//! 删除指定GUID、数量的[未锁定]物品
	BOOL DelGoods(const CGUID &GoodsGuid,int num);
	//! 找到指定索引、数量的[未锁定]物品
	BOOL FindGoodsByIndex(LONG lGoodsIdx, LONG num);
	//! 删除指定索引、数量的[未锁定]物品
	BOOL DelGoodsByIndex(LONG lGoodsIdx,  LONG num);

	//! 按测试结果删除物品
	VOID DelGoodsByTestResult(vector<tagDelTestResult> &vecDelResult);

	DWORD FindGoods(LONG lContainerId,LONG lPos,const char* strGoodsName);
	virtual DWORD FindGoodsByName(LONG lContainerId,const char* strGoodsName);
	virtual DWORD FindGoodsByType(LONG lContainerId,int nType);
	DWORD GetTotalEmptySpaceNum();
	DWORD GetTotalSpaceNum();//获取玩家所有背包格子数 090916by tanglei
	DWORD GetGoodsAmountByOriginName(const char* strOriginName);
	BOOL IsEquipBusinessPack();

	VOID ClearPersonalShopGoodsData(){m_vecInPersonalShopGoods.clear();}
	VOID RemovePersonalShopGoodsData(CGUID& guid);
	BOOL IsInPersonalShop(CGoods* pGoods);
	VOID AddPersonalShopGoodsData(CGoods* pGoods);

	BOOL CheckChangeGoods(vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsIn,vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsOut);
	VOID GoodsChangeGoods(vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsIn,vector<CGoodsBaseProperties::tagGoodsInfo>& vecGoodsOut);

	BOOL SerializeSubpack(DBWriteSet& setWriteDB,CS2CContainerObjectMove::PLAYER_EXTEND_ID extendId);
	VOID AllEquipmentRefineUpdate(long lVal);
	hash_map<CGUID,CGoods*,hash_guid_compare>* GetOriginPackGoodsMap(){return m_cOriginPack->GetGoodsMap();}
	hash_map<CGUID,CGoods*,hash_guid_compare>* GetSubPackGoodsMap(int i);
	map<CEquipmentContainer::EQUIPMENT_COLUMN,CGoods*>* GetEquipmentGoodsMap(){return m_cEquipment.GetGoodsMap();}
	//  [3/15/2010 chenxianj]
	//设置物品的附加经验
	void SetGoodsExtraExp(long lExp);

	//! 物品的限制时间类型
	//! 所在位置：GAP_TIME_LIMIT第一个值
	//! 作用：标识别GAP_TIME_LIMIT的第二个值含义
	enum eGoodsTimeLimit
	{
		eGTL_Minute = 1,	//! 以分为单位的时段，物品可使用的时间
		eGTL_Date = 2,		//! 以秒为单位的时刻，物品的消失时刻
	};

	//!								找到容器对象（不包括钱包、宝石、元宝、积分等）
	CGoodsContainer*				FindContainer(long lContainerId);
	//!								通过物品索引，在背包和子背包中找到第一个一个物品的信息
	BOOL							FindFirstGoodsByIndex(LONG lGoodsIndex, tagGoodsParam &GoodsParam);

	//当速度发生改变以后触发
	void OnSpeedChanged();
	//响应位置改变
	void OnPosChanged();
	//! 响应物品获得
	VOID OnGetGoods(DWORD dwGoodsId,CGoods* pGoods=NULL);
	//! 响应物品失去
	VOID OnLoseGoods(CGoods* pGoods);
	//! 响应仓库物品获得
	VOID OnDepotGetGoods(CGoods* pGoods);
	//! 响应仓库物品失去
	VOID OnDepotLoseGoods(CGoods* pGoods);
	//! 删除物品并通知玩家
	void DelGoodsAndSendMsg(long lContainerId, long lPos);
	//! 在玩家身上（所有背包、装备栏位、仓库）找到一个物品（不忽略锁定物品）
	CGoods* FindGoodByGuid(const CGUID &GoodsGUID,bool NoTest=false);
	CGoods* FindGoodsAllPlaceRemove(const CGUID& guid);
	//! 在玩家所有背包（包括子背包）中找到一个物品（不忽略锁定物品）
	CGoods* FindGoodsByGuidInPack(const CGUID &GoodsGUID);
	BOOL FindGoodsByGuid(const CGUID& guid,DWORD& dwContainer,DWORD& dwPos);
	CGoods* RemoveGoods(long lContainerID,long lPos);
	//!								物品限制时间处理
	//!								若未超时设置定时器，若超时，返回true，应当再接口外部删除物品
	bool							TestGoodsTimeout(CGoods* pGoods);

	//!								通过物品guid找到物品所在位置
	//!								ReValue：pair<容器ID，物品位置>
	//!								找到返回真，否则返回假
	bool							FindGoodsByGuid(const CGUID &GoodsGUID,  pair<long, long> &ReValue);
private:
	/// 检查’星座能量‘衰减
	bool CheckGuardianEnergy( CGoods *pGoods, bool bLogin );	
	/// 注册‘星座能量’递减定时器
	long RegisterGuardianEnergyDecTimer( const CGUID &goodsID );
	/// 响应‘星座能量’定时器
	void OnGuardianEnergyTimeOut( const CGUID &goodsID );
	//!								为物品注册一个定时器，并返回定时器ID
	long							RegisterLimitGoodsTimerID(const CGUID &GoodsGUID, ULONG dwFutureTime);
	//!								取消一个物品的Timer
	void							CancelLimitGoodsTimer(const CGUID &GoodsGUID);
	//!								响应定时器触发的物品删除事件
	void							OnLimitGoodsTimeout(long lTimerID);
	//!								物品GUID和定时器ID关联保存
	map<CGUID, long>				m_GoodsTimerID;
	long							RegisterInvestTimer();

public:
	//!								注册主手和副手武器换装定时器
	void							RegisterChangeModelTimer();
	//!								为换装武器注册一个定时器，并返回定时器ID
	void							RegisterChangeGoodsTimerID(const CGUID &GoodsGUID, long lFutureTime);
	//!								取消一个换装武器的定时器
	void							CancelChangeTimer(const CGUID &GoodsGUID);
	//!								响应换装武器的定时器
	void							OnChangeGoodsTimeOut(long lTimerID);
private:
	//!								换装武器GUID和定时器ID关联保存
	map<CGUID, long>				m_ChangeGoodsTimerID;

	//! 仓库操作
	////////////////////////////////////////////////////////////////////////////////////////////
public:
	PlayerDepot*				GetPlayerDepot(void);

	//!							卡片操作密码
	CPlayerPwd					m_CardPwd;

private:
	//! 仓库对象
	PlayerDepot					m_pPlayerDepot;
	////////////////////////////////////////////////////////////////////////////////////////////

public:	
	//##装备
	CEquipmentContainer			m_cEquipment;
	//##钱包
	CWallet						m_cWallet;

	//银币钱包
	CSilverWallet				m_cSilverWallet;

	//! 宝石包
	GemContainer				m_cGem;

	DWORD						DeleteGoods( CS2CContainerObjectMove::PLAYER_EXTEND_ID pei, const CGUID& guid, DWORD dwAmount );
	DWORD						DeleteDepotGoods( DWORD dwContainerType, const CGUID& guid, DWORD dwAmount );
	DWORD						DeleteGoods(CS2CContainerObjectMove::PLAYER_EXTEND_ID pei,eGoodsBaseType baseType,DWORD dwWeaponType,DWORD dwNum);
	BOOL						DropGoods( CS2CContainerObjectMove::PLAYER_EXTEND_ID pei, const CGUID& guid );
	int							CheckGoods(const char *goodoriname);//检查物品是否有该物品，返回物品的个数
	BOOL						AddGoodsWithProtect(CGoods* pGoods,BOOL bProtect=TRUE, LONG *pOptRerult = NULL);	
	BOOL						AddAndReturnGoodsWithProtect(CGoods **ppGoods,BOOL bProtect=TRUE, LONG *pOptRerult = NULL);
	BOOL						EquipBackToPack();

	int							check_item_in_packet(const int item_idx);//检查物品是否有该物品，返回物品的个数

	int							remove_item_in_packet(const int item_idx, const int item_num);
	BOOL						DelGoodsAmountByOriginName( const char* goodsname, int goodsnum);

	//##特殊物品相关的
	VOID						DropParticularGoodsWhenDead();
	VOID						DropParticularGoodsWhenLost();
	VOID						DropParticularGoodsWhenRecall();

	//!							技能加成开始（返回一个KEY）
	long						SkillReinforceBegin(bool isMagic, bool isAttack);
	//!							技能加成结束（清空效果缓存）
	void						SkillReinforceEnd(long lKey);
	//!							获得技能加成值（first：吟唱时间变化比例；second：效果值变化比例）
	const pair<float, float>&	GetSkillReinforceAmend(long lKey);
	//!							返回物品当前能量（没有冲能的武器返回-1）
	long						GetArmCurrPower(void);
	//!							返回物品当前能量与最大值的比例（用百分数分子表示）
	long						GetArmCurrPowerSacle(void);

	//!							根据输入的条件得到一个配置的加成值
	//!							eValuePos的类型定义在GoodsExSetup::eAmendCoefficientType中
	float						GetReinforceByEnum(long eValuePos, BOOL isMagic);

	//!							扣除一次消耗的武器的能量
	bool						ExpendPower(BOOL isMagic);

private:
	list<tagExpendableEffect> m_listExpendableEffect;	//消耗道具暂时提升属性的作用表

	long							m_EffectIdSeed;
	//! 使用技能加成比例（first：吟唱时间变化比例；second：效果值变化比例）
	map<long, pair<float, float>>	m_mapReturnReinforce;

	//已接收系统接收邮件列表
	list<CGUID>		m_lReceiveMail;
	bool			m_bReceiveMail;
	map<long,long>	m_mapSysMail;

public:
	void				InsertSysMail(long lID,long lEndTime);//插入系统邮件列表
	void				CheckSysMailTime();					  //检查系统邮件列表
	bool				CheckSysMail(long lSysID);			  //检查有没有接收过lSysID系统邮件
	map<long,long>		&GetSysMailList()	  { return m_mapSysMail;}
	void				SetReceiveMail(bool b){ m_bReceiveMail = b; }
	bool				GetReceiveMail()	  { return m_bReceiveMail; }
	list<CGUID>			&GetReceiveMailList() { return m_lReceiveMail;}
	void				AddReceiveMailList(CGUID &MailGuid);
	void				DeleteReceiveMailList(CGUID &MailGuid);

	/////////////////////////////////////////////////////////////////////
	// 属性存取接口
	/////////////////////////////////////////////////////////////////////
private:
	DWORD	m_dwMurdererTimeStamp;
	long	m_lPKTimerID;			//PK定时器id
	long	m_lPvPTimerID;			//PVP定时器id
	long	m_lHpMpTimerID;			//自动恢复Hp,Mp定时器id
	long	m_lReliveTimerID;		//死亡回城复活的定时器ID

public:
	void	RegisterReliveTimerID();			//死亡注册回城复活定时器
	void	UnRegisterReliveTimerID();			//复活注销回城复活定时器
public:
	////////////////////////////////////////////////////////////////////////////////
	//设置活力值倍率
	void    SetActiveRatio(float f)	{m_fActiveScale = f;}
	float	GetActiveRatio()		{return m_fActiveScale;}
	//是否充值
	void	SetCharged(bool b=true)		{m_AppendProperty.bIsCharged = b;}
	bool	IsCharged()					{return m_AppendProperty.bIsCharged;}

	const char *GetAccount()			{return m_AppendProperty.strAccount;}
	void	SetAccount(char *l)			{strcpy_s(m_AppendProperty.strAccount, ACCOUNT_SIZE, l);}
	const char *GetTitle()				{return m_AppendProperty.strTitle;}
	void	SetTitle(char *l)			{strcpy_s(m_AppendProperty.strTitle, TITLE_SIZE, l);}
	//头盔 披风 外套
	BYTE	GetShowFashion()		{return m_AppendProperty.bShowFashion;}
	void	SetShowFashion(BYTE b)	{m_AppendProperty.bShowFashion = b;}

	////////////////////////////////////////////////////////////////////////////////
	// 区域
	virtual void SetArea(CArea* p);

public:
	//等级
	WORD   GetLevel()					{return m_Property.wLevel;}
	void   SetLevel(WORD l, BOOL bScriptSet);

	//星座
	eConst GetConst() const { return m_Property.byConst; }
	void SetConst( eConst c ) { m_Property.byConst = c; }

	//得到职业等级
	BYTE	GetOccuLvl(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.byOccuLvl[byOccu];
		return 0;
	}
	void  SetOccuLvl(eOccupation byOccu,BYTE l, BOOL bScriptSet);

	//经验值
	LONG64  GetTotalExp(void)			
	{
		LONG64 llTmp = (LONG64)dwExpMutipleValue*m_Property.dwExpMultiple;
		LONG64 llRet = llTmp + m_Property.dwExp;
		return llRet;
	}
	DWORD	GetExp()					{return m_Property.dwExp;}
	void	SetExp(LONG64 l);
	DWORD	GetExpMultiple(void)		{return m_Property.dwExpMultiple;}
	void	SetExpMultiple(DWORD l)		{m_Property.dwExpMultiple = l;}

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
	//赠送经验值
	DWORD	GetPresentExp()				{return m_Property.dwPresentExp;}
	void	SetPresentExp(DWORD l)		{m_Property.dwPresentExp=l;}

	//升级所需经验值
	DWORD	GetUpgradeExp()				{return m_Property.dwUpgradeExp;}
	void	SetUpgradeExp(DWORD l)		{m_Property.dwUpgradeExp = l;}
	//头部图片
	long	GetHeadPic()				{return m_Property.lHairPic;}
	void	SetHeadPic(long l)			{m_Property.lHairPic = l;}
	//面部图片
	long	GetFacePic()				{return m_Property.lFacePic;}
	void	SetFacePic(long l)			{m_Property.lFacePic = l;}
	//职业	
	BYTE	GetOccupation()				{return m_Property.byOccu;}
	void	SetOccupation(eOccupation l)		{m_Property.byOccu = l;}
	//副职业
	BYTE	GetDOccupation()			{return m_Property.byAssOccu;}
	void	SetDOccupation(eDeputyOccu l)	{m_Property.byAssOccu=l;}
	//性别
	BYTE	GetSex()					{return m_Property.bySex;}
	void	SetSex(BYTE l)				{m_Property.bySex = l;}
	//生物种类
	BYTE	GetClass()					{return m_Property.byClass;}
	void	SetClass(BYTE l)				{m_Property.byClass = l;}
	//配偶ID
	DWORD	GetSpouseId()				{return m_Property.dwSpouseID;}
	void	SetSpouseId(DWORD l)		{m_Property.dwSpouseID = l;}
	//pk次数
	DWORD	GetPkCount()				{return m_Property.dwPkCount;}
	void	SetPkCount(DWORD l)			{m_Property.dwPkCount = l;}
	//杀死人数
	DWORD	GetPkValue()				{return m_Property.dwPkValue;}
	void	SetPkValue(DWORD l)	;
	//HP
	DWORD	GetHP()						{return m_Property.dwHp;}
	void	SetHP(DWORD l);				
	//MP
	WORD	GetMP()						{return m_Property.wMp;}
	void	SetMP(WORD l);
	//热键
	DWORD	GetHotKey(long lpos){if(lpos < 0){lpos = 0;}else if(lpos > 23){lpos = 23;}return m_Property.dwHotKey[lpos];}
	void	SetHotKey(long lpos,DWORD l){if(lpos < 0){lpos = 0;}else if(lpos > 23){lpos = 23;}m_Property.dwHotKey[lpos] = l;}

	//PK开关(b0:全体保护)
	BOOL	IsPk_Normal();
	void	SetPk_Normal(bool b);
	//b1:组队保护
	BOOL	IsPk_Team();
	void	SetPk_Team(bool b);
	//b2:工会保护,
	BOOL	IsPk_Union();
	void	SetPk_Union(bool b);
	BOOL	IsPk_Badman();
	void	SetPk_Badman(bool b);
	//b4:本国保护
	BOOL	IsPk_Country();
	void	SetPk_Country(bool b);
	//b5:阵营保护
	BOOL	IsPk_Camp();
	void	SetPk_Camp(bool b);
	//b6.海盗保护
	BOOL	IsPk_Pirate();
	void	SetPk_Pirate(bool b);
	//b7.攻击提示
	BOOL	IsPk_Hint();
	void	SetPk_Hint(bool b);
	//b8.自动选人
	BOOL	IsPk_AutoPlayer();
	void	SetPk_AutoPlayer(bool b);
	//b9.自动选怪
	BOOL	IsPk_AutoMonster();
	void	SetPk_AutoMonster(bool b);



	//能量值
	WORD	GetEnergy()					{return m_Property.wEnergy;}
	void	SetEnergy(WORD l);
	//爵位等级
	BYTE	GetRankofNobility()			{return m_Property.byRankOfNobility;}
	void	SetRankofNobility(BYTE l)	{m_Property.byRankOfNobility = l;}
	//当前体力
	WORD	GetStamina()				{return m_Property.wStamina;}
	void	SetStamian(WORD l)			{m_Property.wStamina = l;}
	//爵位声望值
	DWORD   GetRankOfNobCredit()		{return m_Property.dwRankOfNobCredit;}
	void	SetRankOfNobCredit(DWORD l){m_Property.dwRankOfNobCredit = l;}
	//佣兵等级
	BYTE	GetRankOfMercenary()		{return m_Property.byRankOfMercenary;}
	void	SetRankOfMercenary(DWORD l) {m_Property.byRankOfMercenary = l;}
	//佣兵声望值
	DWORD	GetMercenaryCredit()		{return m_Property.dwMercenaryCredit;}
	void	SetMercenaryCredit(DWORD l){m_Property.dwMercenaryCredit = l;}
	//国家ID
	BYTE	GetCountry()				{return m_Property.byCountry;}
	void	SetCountry(BYTE l)			{m_Property.byCountry = l;}
	//国家贡献值
	long	GetCountryContribute()		{return m_Property.lCountryContribute;}
	void	SetCountryContribute(long l);
	//配偶关系值
	DWORD	GetSpouseParam()			{return	m_Property.dwSpouseParam;}
	void	SetSpouseParam(DWORD l)		{m_Property.dwSpouseParam = l;}
	//商业等级
	BYTE	GetBusinessLevel()			{return m_Property.byBusinessLevel;}
	void	SetBusinessLevel(BYTE l)	{m_Property.byBusinessLevel = l;}
	//商业经验
	DWORD	GetBusinessExp()			{return	m_Property.dwBusinessExp;}
	void	SetBusinessExp(DWORD l)		{m_Property.dwBusinessExp = l;}
	//工匠声望
	DWORD   GetArtisanCredit()			{return m_Property.dwArtisanCredit;}
	void	SetArtisanCredit(DWORD l)			{m_Property.dwArtisanCredit = l;}
	//工匠等级
	WORD	GetArtisanLevel()			{return m_Property.byArtisanLevel;}
	void	SetArtisanLevel(WORD l)		{m_Property.byArtisanLevel = l;}
	//工匠经验
	DWORD	GetArtisanExp()				{return	m_Property.dwArtisanExp;}
	void	SetArtisanExp(DWORD	l)		{m_Property.dwArtisanExp = l;}
	//杀外国人数
	DWORD	GetPVPCount()				{return m_Property.dwPVPCount;}
	void	SetPVPCount(DWORD l)		{m_Property.dwPVPCount = l;}
	//PVP值
	DWORD	GetPVPValue()				{return	m_Property.dwPVPValue;}
	void	SetPVPValue(DWORD l)		{m_Property.dwPVPValue = l;}

	DWORD	GetLastExitGameTime()		{return m_Property.dwLastExitGameTime;}
	void	SetLastExitGameTime(DWORD l){m_Property.dwLastExitGameTime=l;}

	///////////// PK CP  ////////////////////////////////////////
	long GetPKCP() const { return m_Property.lCurPKCP; }
	void SetPKCP( long v );
	long GetMaxPKCP();

	long GetBaseMaxPKCP() const { return m_BaseFightProperty.lMaxPKCP; }
	void SetBaseMaxPKCP( long v );

	/// 计算玩家身上所有装备CP倍率系数
	long GetAllEquipTimesCP();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//战斗属性
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//最大HP
	DWORD	GetMaxHP()
	{
		long lMaxHp =m_FightProperty.dwMaxHp+m_BaseFightProperty.dwMaxHp;
		if(lMaxHp<1)	return 1;
		return lMaxHp;
	}
	void	SetMaxHP(DWORD l);
	//最大MP
	WORD	GetMaxMP()
	{
		long lMaxMp = m_FightProperty.wMaxMp+m_BaseFightProperty.wMaxMp;
		if(lMaxMp<1)	return 1;
		return lMaxMp;
	} 
	void	SetMaxMP(WORD l);
	//体力上限
	WORD	GetMaxStamina()							{return m_FightProperty.wMaxStamina;}
	void	SetMaxStamina(WORD l);
	//最大能量值
	WORD	GetMaxEnergy()							{return m_FightProperty.wMaxEnergy;}
	void	SetMaxEnergy(WORD l);

	//最小攻击力
	WORD	GetMinAtk()								{return m_FightProperty.wMinAtk;}
	void	SetMinAtk(WORD l);	
	//最大攻击力
	WORD	GetMaxAtk()								{return m_FightProperty.wMaxAtk;}
	void	SetMaxAtk(WORD l);
	//命中能力
	WORD	GetHit()								{return m_FightProperty.wHit;}
	void	SetHit(WORD l);		
	//物理防御
	WORD	GetDef()								{return m_FightProperty.wDef;}
	void	SetDef(WORD l);			
	//闪避
	WORD	GetDodge()								{return m_FightProperty.wDodge;}
	void	SetDodge(WORD l);		
	//元素抗性
	WORD	GetElementResistant(long lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_FightProperty.wElemDef[lpos];}
	void	SetElementResistant(long lpos,WORD l);	

	//生命恢复速度
	WORD	GetHpRecoverSpeed()						{return m_FightProperty.wHpRecoverSpeed;}
	void	SetHpRecoverSpeed(WORD l);
	//法力恢复速度
	WORD	GetMpRecoverSpeed()						{return m_FightProperty.wMpRecoverSpeed;}
	void	SetMpRecoverSpeed(WORD l);
	//体力恢复速度
	WORD	GetStamRecoverSpeed()					{return	m_FightProperty.wStamRecoverSpeed;}
	void	SetStamRecoverSpeed(WORD l)	;
	//力量
	WORD	GetStrenth()							{return m_FightProperty.wStrenth;}
	void	SetStrenth(WORD l)	;
	//敏捷	
	WORD	GetAgility()							{return m_FightProperty.wAgility;}
	void	SetAgility(WORD l)	;
	//体质
	WORD	GetConsti()								{return m_FightProperty.wConsti;}
	void	SetConsti(WORD l)	;
	//智力
	WORD	GetIntell()								{return	m_FightProperty.wIntell;}
	void	SetIntell(WORD l)	;
	//智慧
	WORD	GetWisdom()								{return	m_FightProperty.wWisdom;}
	void	SetWisdom(WORD l)	;
	//精神
	WORD	GetSpirit()								{return	m_FightProperty.wSpirit;}
	void	SetSpirit(WORD l)	;
	//格挡
	WORD	GetFightBlock()							{return m_FightProperty.wFightBlock;}
	void	SetFightBlock(WORD l)	;
	//招架
	WORD	GetParry()								{return m_FightProperty.wParry;}
	void	SetParry(WORD l)	;

	//魅力
	WORD	GetCharm()								{return	m_FightProperty.wCharm;}
	void	SetCharm(WORD l)	;
	//专注
	WORD	GetAbsorb()								{return	m_FightProperty.wAbsorb;}
	void	SetAbsorb(WORD l)	;
	//魔法抗性
	WORD	GetMdef()								{return m_FightProperty.wMdef;}
	void	SetMdef(WORD l)		;
	//物理暴击
	WORD	GetCri()								{return	m_FightProperty.wCri;}
	void	SetCri(WORD l)		;

	WORD	GetMAtkSpeed()							{return m_FightProperty.wMAtkSpeed;}
	void	SetMAtSpeed(WORD l)	;
	//Buff攻击强度
	WORD	GetBuffPower()							{return m_FightProperty.wBuffPower;}
	void	SetBuffPower(WORD l)	;
	//魔法攻击力
	WORD	GetMAtk()								{return m_FightProperty.wMAtk;}
	void	SetMAtk(WORD l)			;
	//魔法爆击率
	WORD	GetMCri()								{return	m_FightProperty.wMCri;}
	void	SetMCri(WORD l)			;
	//生命吸收率
	WORD	GetBloodSuk()							{return m_FightProperty.wBloodSuk;}
	void	SetBloodSuk(WORD l)		;
	//法力吸收率
	WORD	GetManaSuck()							{return m_FightProperty.wManaSuck;}
	void	SetManaSuck(WORD l)		;
	//意志抗性
	WORD	GetWillDef()							{return	m_FightProperty.wWillDef;}
	void	SetWillDef(WORD l)		;
	//体质抗性
	WORD	GetConstiDef()							{return m_FightProperty.wConstiDef;}
	void	SetConstiDef(WORD l)	;
	//Debuff抗性
	WORD	GetDeBuffDef(long lpos)					{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1; return	m_FightProperty.wDeBuffDef[lpos]+m_BaseFightProperty.wDeBuffDef[lpos];}
	void	SetDefBuffDef(long lpos,WORD l);			
	//对各个种族伤害
	WORD	GetClassDam(long lpos)					{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; return	m_FightProperty.wClassDam[lpos];}
	void	SetClassDam(long lpos,WORD l);			
	WORD	GetElemDam(long lpos)					{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return	m_FightProperty.wElemDam[lpos];}
	void	SetElemDam(long lpos,WORD l)	;
	//战斗时生命恢复速度
	WORD	GetFightHPRecoverSpeed()				{return m_FightProperty.wFightHpRecoverSpeed;}
	void	SetFightHPRecoverSpeed(WORD l)	;
	//战斗时法力恢复速度
	WORD	GetFightMPRecoverSpeed()				{return m_FightProperty.wFightMpRecoverSpeed;}
	void	SetFightMPRecoverSpeed(WORD l)	;
	//能量恢复速度
	short	GetEnterRecoverSpeed()					{return m_FightProperty.wEnerRecoverSpeed;}
	void    SetEnterRecoverSpeed(WORD l)	;
	//战斗能量恢复速度
	short	GetFightEnterRecoverSpeed()				{return m_FightProperty.wFightEnerRecoverSpeed;}
	void	SetFightEnterRecoverSpeed(WORD l);
	//能量吸收率
	WORD	GetEnergySuck()							{return m_FightProperty.wEnergySuck;}
	void	SetEnergySuck(WORD l)			;
	//物理爆击抗性
	WORD	GetCriDef()								{return m_FightProperty.wCriDef;}
	void    SetCriDef(WORD l)				;
	//魔法爆击抗性
	WORD	GetMCriDef()							{return m_FightProperty.wMCriDef;}
	void	SetMCriDef(WORD l)				;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//基本战斗属性
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//最大HP
	DWORD	GetBaseMaxHP()								{return m_BaseFightProperty.dwMaxHp;}
	void	SetBaseMaxHP(DWORD l)						{m_BaseFightProperty.dwMaxHp = l; }
	//最大MP
	WORD	GetBaseMaxMP()								{return m_BaseFightProperty.wMaxMp;} 
	void	SetBaseMaxMP(WORD l)						{m_BaseFightProperty.wMaxMp = l; }
	//体力上限
	WORD	GetBaseMaxStamina()							{return m_BaseFightProperty.wMaxStamina;}
	void	SetBaseMaxStamina(WORD l)					{m_BaseFightProperty.wMaxStamina = l;}
	//最大能量值
	WORD	GetBaseMaxEnergy()							{return m_BaseFightProperty.wMaxEnergy;}
	void	SetBaseMaxEnergy(WORD l);				

	//最小攻击力
	WORD	GetBaseMinAtk()								{return m_BaseFightProperty.wMinAtk;}
	void	SetBaseMinAtk(WORD l)						{m_BaseFightProperty.wMinAtk = l;}
	//最大攻击力
	WORD	GetBaseMaxAtk()								{return m_BaseFightProperty.wMaxAtk;}
	void	SetBaseMaxAtk(WORD l)						{m_BaseFightProperty.wMaxAtk = l;}
	//命中能力
	WORD	GetBaseHit()								{return m_BaseFightProperty.wHit;}
	void	SetBaseHit(WORD l)							{m_BaseFightProperty.wHit = l;}
	//物理防御
	WORD	GetBaseDef()								{return m_BaseFightProperty.wDef;}
	void	SetBaseDef(WORD l)							{m_BaseFightProperty.wDef = l;}
	//闪避
	WORD	GetBaseDodge()								{return m_BaseFightProperty.wDodge;}
	void	SetBaseDodge(WORD l)						{m_BaseFightProperty.wDodge = l;}
	//元素抗性
	WORD	GetBaseElementResistant(long lpos)			{if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return m_BaseFightProperty.wElemDef[lpos];}
	void	SetBaseElementResistant(long lpos,WORD l)	{m_BaseFightProperty.wElemDef[lpos] = l;}
	//生命恢复速度
	WORD	GetBaseHpRecoverSpeed()						{return m_BaseFightProperty.wHpRecoverSpeed;}
	void	SetBaseHpRecoverSpeed(WORD l)				{m_BaseFightProperty.wHpRecoverSpeed = l;}
	//法力恢复速度
	WORD	GetBaseMpRecoverSpeed()						{return m_BaseFightProperty.wMpRecoverSpeed;}
	void	SetBaseMpRecoverSpeed(WORD l)				{m_BaseFightProperty.wMpRecoverSpeed = l;}
	//体力恢复速度
	WORD	GetBaseStamRecoverSpeed()					{return	m_BaseFightProperty.wStamRecoverSpeed;}
	void	SetBaseStamRecoverSpeed(WORD l)				{m_BaseFightProperty.wStamRecoverSpeed = l;}
	//力量
	WORD	GetBaseStrenth()							{return m_BaseFightProperty.wStrenth;}
	void	SetBaseStrenth(WORD l)						{m_BaseFightProperty.wStrenth = l;}
	//敏捷	
	WORD	GetBaseAgility()							{return m_BaseFightProperty.wAgility;}
	void	SetBaseAgility(WORD l)						{m_BaseFightProperty.wAgility = l;}
	//体质
	WORD	GetBaseConsti()								{return m_BaseFightProperty.wConsti;}
	void	SetBaseConsti(WORD l)						{m_BaseFightProperty.wConsti = l;}
	//智力
	WORD	GetBaseIntell()								{return	m_BaseFightProperty.wIntell;}
	void	SetBaseIntell(WORD l)						{m_BaseFightProperty.wIntell = l;}
	//智慧
	WORD	GetBaseWisdom()								{return	m_BaseFightProperty.wWisdom;}
	void	SetBaseWisdom(WORD l)						{m_BaseFightProperty.wWisdom = l;}
	//精神
	WORD	GetBaseSpirit()								{return	m_BaseFightProperty.wSpirit;}
	void	SetBaseSpirit(WORD l)						{m_BaseFightProperty.wSpirit = l;}
	//格挡
	WORD	GetBaseBlock()								{return m_BaseFightProperty.wFightBlock;}
	void	SetBaseBlock(WORD l)						{m_BaseFightProperty.wFightBlock = l;}
	//招架
	WORD	GetBaseParry()								{return m_BaseFightProperty.wParry;}
	void	SetBaseParry(WORD l)						{m_BaseFightProperty.wParry = l;}
	//魅力
	WORD	GetBaseCharm()								{return	m_BaseFightProperty.wCharm;}
	void	SetBaseCharm(WORD l)						{m_BaseFightProperty.wCharm = l;}
	//专注
	WORD	GetBaseAbsorb()								{return	m_BaseFightProperty.wAbsorb;}
	void	SetBaseAbsorb(WORD l)						{m_BaseFightProperty.wAbsorb = l;}
	//魔法抗性
	WORD	GetBaseMdef()								{return m_BaseFightProperty.wMdef;}
	void	SetBaseMdef(WORD l)							{m_BaseFightProperty.wMdef = l;}
	//物理暴击
	WORD	GetBaseCri()								{return	m_BaseFightProperty.wCri;}
	void	SetBaseCri(WORD l)							{m_BaseFightProperty.wCri = l;}
	//施法速度
	WORD	GetBaseMAtkSpeed()							{return m_BaseFightProperty.wMAtkSpeed;}
	void	SetBaseMAtSpeed(WORD l)						{m_BaseFightProperty.wMAtkSpeed = l;}
	//Buff攻击强度
	WORD	GetBaseBuffPower()							{return m_BaseFightProperty.wBuffPower;}
	void	SetBaseBuffPower(WORD l)					{m_BaseFightProperty.wBuffPower = l;}
	//魔法攻击力
	WORD	GetBaseMAtk()								{return m_BaseFightProperty.wMAtk;}
	void	SetBaseMAtk(WORD l)							{m_BaseFightProperty.wMAtk = l;}
	//魔法爆击率
	WORD	GetBaseMCri()								{return	m_BaseFightProperty.wMCri;}
	void	SetBaseMCri(WORD l)							{m_BaseFightProperty.wMCri = l;}
	//生命吸收率
	WORD	GetBaseBloodSuk()							{return m_BaseFightProperty.wBloodSuk;}
	void	SetBaseBloodSuk(WORD l)						{m_BaseFightProperty.wBloodSuk = l;}
	//法力吸收率
	WORD	GetBaseManaSuck()							{return m_BaseFightProperty.wManaSuck;}
	void	SetBaseManaSuck(WORD l)						{m_BaseFightProperty.wManaSuck = l;}
	//意志抗性
	WORD	GetBaseWillDef()							{return	m_BaseFightProperty.wWillDef;}
	void	SetBaseWillDef(WORD l)						{m_BaseFightProperty.wWillDef = l;}
	//体质抗性
	WORD	GetBaseConstiDef()							{return m_BaseFightProperty.wConstiDef;}
	void	SetBaseConstiDef(WORD l)					{m_BaseFightProperty.wConstiDef = l;}
	//Debuff抗性
	WORD	GetBaseDeBuffDef(long lpos)					{if(lpos<0) {lpos = 0;}else if((lpos+1)>DET_Max) lpos = DET_Max -1;  return	m_BaseFightProperty.wDeBuffDef[lpos];}
	void	SetBaseDefBuffDef(long lpos,WORD l)			{m_BaseFightProperty.wDeBuffDef[lpos] = l;}

	//对各个种族伤害
	WORD	GetBaseClassDam(long lpos)					{if(lpos<0) {lpos = 0;}else if((lpos+1)>C_Max) lpos = C_Max -1; if(lpos<0) {lpos = 0;}else if((lpos+1)>ET_Max) lpos = ET_Max -1; return	m_BaseFightProperty.wClassDam[lpos];}
	void	SetBaseClassDam(long lpos,WORD l)			{m_BaseFightProperty.wClassDam[lpos] = l;}

	//对各个元素伤害
	WORD	GetBaseElemDam(long lpos)					{return	m_BaseFightProperty.wElemDam[lpos];}
	WORD	SetBaseElemDam(long lpos,WORD l)		    {m_BaseFightProperty.wElemDam[lpos] = l;}

	//战斗时生命恢复速度
	WORD	GetBaseFightHPRecoverSpeed()				{return m_BaseFightProperty.wFightHpRecoverSpeed;}
	void	SetBaseFightHPRecoverSpeed(WORD l)			{m_BaseFightProperty.wFightHpRecoverSpeed = l;}

	//战斗时法力恢复速度
	WORD	GetBaseFightMPRecoverSpeed()				{return m_BaseFightProperty.wFightMpRecoverSpeed;}
	void	SetBaseFightMPRecoverSpeed(WORD l)			{m_BaseFightProperty.wFightMpRecoverSpeed = l;}

	//能量恢复速度
	short	GetBaseEnterRecoverSpeed()					{return m_BaseFightProperty.wEnerRecoverSpeed;}
	void    SetBaseEnterRecoverSpeed(WORD l)			{m_BaseFightProperty.wEnerRecoverSpeed = (short)l;}

	//战斗能量恢复速度
	short	GetBaseFightEnterRecoverSpeed()				{return m_BaseFightProperty.wFightEnerRecoverSpeed;}
	void	SetBaseFightEnterRecoverSpeed(WORD l)		{m_BaseFightProperty.wFightEnerRecoverSpeed = (short)l;}

	//能量吸收率
	WORD	GetBaseEnergySuck()							{return m_BaseFightProperty.wEnergySuck;}
	void	SetBaseEnergySuck(WORD l)					{m_BaseFightProperty.wEnergySuck = l;}

	//物理爆击抗性
	WORD	GetBaseCriDef()								{return m_BaseFightProperty.wCriDef;}
	void    SetBaseCriDef(WORD l)						{m_BaseFightProperty.wCriDef = l;}

	//魔法爆击抗性
	WORD	GetBaseMCriDef()							{return m_BaseFightProperty.wMCriDef;}
	void	SetBaseMCriDef(WORD l)						{m_BaseFightProperty.wMCriDef = l;};

	//! ----------------------原属属性----------------------
	//!		得到元素属性
	DWORD	GetElementProperty(DWORD dwElementType, DWORD dwAttackType);
	//!		设置元素属性，成功返回当前最新属性，失败返回0
	DWORD	SetElementProperty(DWORD dwElementType, DWORD dwAttackType, long lNewValue);
	//!		添加元素属性，成功返回当前最新属性，失败返回0
	DWORD	AddElementProperty(DWORD dwElementType, DWORD dwAttackType, long lAddValue);


	float	GetExpScale()			{return m_fExpScale;}
	void	SetExpScale(float f)	{m_fExpScale=f;}
	float	GetOccuExpScale()		{return m_fOccuExpScale;}
	void	SetOccuExpScale(float f){m_fOccuExpScale=f;}
	float	GetSpScale()			{return m_fSpScale;}
	void	SetSpScale(float f)		{m_fSpScale=f;}
	float	GetPresentExpScale()			{return m_fPresentExpScale;}
	void	SetPresentExpScale(float f)		{m_fPresentExpScale=f;}
	//力量
	void	AddStr()					{m_FightProperty.wStrenth++;}
	//敏捷
	void	AddDex()					{m_FightProperty.wAgility++;}
	//体质
	void	AddCon()					{m_FightProperty.wConsti++;}

	void	IncExp(LONG64 llExp, DWORD dwAddType);		//给玩家增加经验
	//!		添加经验日志
	void	AddIncExpLog(DWORD dwOldExp, DWORD dwOldExpMultiple, DWORD dwAddExp, DWORD dwAddExpMultiple, DWORD dwAddType, const char *pSrcName);
	void	IncOccuSp(eOccupation eOccu,long lSp,long lType);		//给玩家增加技能点数
	void	DecOccuSp(eOccupation eOccu,long lSp);		//扣除SP
	//获取赠送经验
	DWORD	AcquirePresentExp(DWORD dwMaxAcqExp);

	//增加某个职业的职业经验
	void	IncOccuExp(eOccupation byOccu,long lExp, DWORD dwAddType);//给玩家增加职级经验

	//!		添加职业经验日志
	void	AddIncOccuExpLog(BYTE byOccu,DWORD dwOldExp, DWORD dwAddExp, DWORD dwAddType, const char *pSrcName);

	DWORD	GetMoney();
	//!		得到可装的最大钱数
	DWORD	GetMaxMoney(void);
	bool	SetMoney(DWORD l,int nFlag);

	DWORD   GetSilver();
	DWORD   GetMaxSilver();
	bool    SetSilver(DWORD l);

	DWORD	GetGem(void);
	bool	SetGem(DWORD lNum);

	DWORD   GetYuanbao(){return m_dwYuanbao;}
	VOID	SetYuanbao(DWORD dwVal);	

	////////////////////! 行动点//////////////////
	//!		得到玩家活力点
	long	GetActive(void);
	//!		重置玩家活力点
	void	ResetActive(DWORD dwCurrTime);
	//!		添加扩展活力点
	bool	AddExActive(long lAddNum);
	//!		扣除活力点
	bool	TakeActive(long lTakeNum);
	//! 每天0点以后玩家登录游戏时重置玩家CP
	void    ResetActive(void);

	////////////////////! 结婚//////////////////
	//!				得到玩家结婚步骤
	long			GetMarriageStep(void);
	//!				设置玩家结婚步骤
	long			SetMarriageStep(long lStep);

	//!				设置结婚对象
	bool			SetSpouse(CPlayer *pPlayer);
	//!				得到对象ID
	const CGUID&	GetSpouseGuid(void);
	//!				得到对象名字
	const char*		GetSpouseName(void);

	//////////////////////////////////////////////////////

	//##杀人数的管理.
	VOID	OnDecreaseMurdererSign		();
	VOID	OnUpdateMurdererSign		();
	VOID	OnClearMurdererSign			();

	void	OnDecreasePKValue			();//减pk值
	void	OnDecreasePvPValue			();//减pvp值
	void	OnIncreaseHpMpValue			();//自动增加hp,mp				
	void	RegisterPKValue				();//pk值注册定时器
	void	RegisterPvPValue			();//pvp值注册定时器
	void	RegisterAutoReNewHpMp		();//自动恢复Hp,Mp定时器
	void	UnRegisterPKValue			();//pk值注消册定时器
	void	UnRegisterPvPValue			();//pvp值注消定时器
	void	UnRegisterAutoReNewHpMp		();//注消自动恢复Hp,Mp定时器

	void	UpdateHpMpRecoverSpeed		();//更新HPMP恢复速度
	//  [1/20/2009 chenxianj]
	void    ChangeRgnToIsland			();//流放流亡岛
	bool	ChangeRgnToVillage			();//流放流亡村
	void	PostCountryInfo				(const char *strName);//发送国家公告



	DWORD	GetNextExp()				{return m_Property.dwUpgradeExp;}
	DWORD	GetNextExpMultiple(void)	{return m_Property.dwUpgradeExpMultiple;}
	LONG64	GetNextTotalExp(void)		
	{
		LONG64 llTmp = (LONG64)dwExpMutipleValue*m_Property.dwUpgradeExpMultiple;
		LONG64 llRet = llTmp+m_Property.dwUpgradeExp;
		return llRet;
	}

	long	CheckLevel();				// 检查等级变化

	DWORD	GetNextOccuExp()			{return m_Property.dwCurOccuUpgradeExp;}
	long	CheckOccuLevel(eOccupation byOccu);	// 检查职业等级变化

	VOID    MountAllEquip();
	/// 9.28.2009 新增bGuardianFlag专用于装备‘星座守护石’类物品的特殊标志
	VOID	MountEquipEx(CGoods *pGoods,bool bMount, bool bGuardianFlag = false );
	void	UseItem(WORD wPlace,BYTE lPos,long lTargetType,CGUID& lTargetGuid);	//使用消耗性道具的逻辑
	void	UseItem(DWORD dwContainerId,DWORD dwPos);
	BOOL	AutoUseItem(WORD wPlace,BYTE lPos,long lTargetType,CGUID& lTargetGuid);	//! 自动使用道具
	long	CanMountEquip(CGoods *pGoods); //是否能装备一个道具
	long    CanMountEquip(CGoods *pGoods,DWORD dwPos,BOOL bMsgNotify=FALSE);
	BOOL	CanMountSubpack(CGoods* pGoods);
	long	CanUseItem(CGoods* pGoods);	//是否能使用一个消耗性道具

	VOID    SendEquipNotifyMsg(long lVal);


	CGoods* GetGoodsById(const CGUID& guid);				//根据ID得到物品
	CGoods* GetGoodsById_FromPackage(const CGUID& guid);	//根据ID得到背包物品
private:
	map<string, tagPty>		m_mapNameValue;		// 变量名字对应的变量地址

	void InitNameValueMap();	// 初始化

	map<DWORD,tagGoodsCommonCooldown>  m_mapGoodsIdCoolDown;
	map<DWORD,tagGoodsCommonCooldown>  m_mapClassIdCoolDown;

public:
	virtual long	GetValue(const char* strName);
	virtual long	SetValue(const char* strName, long dwValue);

	//当属性发生改变时候调用
	virtual void OnPropertyValueChanged(const char* strName, LONG lValue);
	static void InitUpdateDirtyAttrData();

	///////////////////////////////////////////////////////////////////
	// 玩家的脚本变量
	///////////////////////////////////////////////////////////////////
public:
	virtual CVariableList* GetVariableList()	{ return m_pVariableList; }
private:
	// 玩家的私有脚本变量列表
	CVariableList* m_pVariableList;
	//////////////////////////////////////////////////////////////////////////
	//	接口
	//////////////////////////////////////////////////////////////////////////

public:
	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);	// 解码
	bool CodeOrgSysToDataBlock(DBWriteSet& setWriteDB, bool bExData);
	bool DecodeOrgSysFromDataBlock(DBReadSet& setReadDB,bool bExData);
	bool CodeToDataBlock_ForClient(DBWriteSet& setWriteDB, bool bExData = true);
	void CodeRecruitToDataBlock_ForClient(DBWriteSet& setWriteDB);
	BOOL CodeDOccuToDataBlock(DBWriteSet& setWriteDB){return m_DOccuRecord.CodeToDataBlock(setWriteDB);}
	VOID CodeGoodsCooldown(DBWriteSet& setWriteDB,bool bExData=true);

	// 与WS编解码热键接口
	bool CodeHotKeyData(DBWriteSet& setWriteDB,bool bExData = true);
	bool DecodeHotKeyData(DBReadSet& setReadDB,bool bExData = true);

	// 获取当前正在使用的功能热键映射表
	MAPFKEY& GetCurMapKey() { return m_FuncKey; }

	//战斗下线的时间戳
	DWORD												m_dwLostTimeStamp;
	void PetsChangeRegionLocal(INIT_TYPE lInitType);
	bool ChangeRegion(eRgnType type, const CGUID& lRegionID, long x, long y, long dir, long templateID=0, long lUseGoods=0, long lRange=0, long lCarDistance=0);

	//! 结束所有影响移动的逻辑状态（如摆摊、交易）
	void EndAllStateWithMove(void);

	//移动时触发
	VOID OnMove(float fNewX, float fNewY);
	// 请求复活(0:安全回城 1:原地复活)
	virtual void OnRelive(long lType,long lRecoverRatio=50);

	//当玩家等级发生变化的时候触发，主要指的是升级
	void OnLevelChanged();
	//当玩家职业等级增发生变化的时候，主要指升职业等级的时候
	void OnOccuLevelChanged();

	//##进入某个场景后，场景会自动调用这个函数通知该类进入场景成功。
	//##可能召唤兽也有这个需求.
	virtual VOID OnEnterRegion(bool bIsRelive=false);
	virtual VOID OnExitRegion();

	//当进入GS触发,包括第一次进入GS，或者以后的切换服务器
	bool OnEnter(void);
	//当进入游戏时触发,第一次登陆的时候触发，以后切换服务器不会触发
	void OnEnterGame(void);

	//玩家选择退出游戏时退出GS触发，包括切换法服务器的退出GS也触发
	void OnExit ( BOOL bOnChangeServer = FALSE );
	//当退出游戏时触发,切换服务器的时候会触发
	void OnExitGame(BOOL bOnChangeServer = FALSE);

	// Fox@20081126 宠物
public:
	C_PETS GetPets(void) { return m_Pets; }
	long GetPetsNum(void) { return m_Pets.size(); }
	C_ACT_PETS GetActivePets(void) { return m_ActivePets; }
	CPet* CreatePet(const CGUID& rID);
	CPet* GetPet(const CGUID& rID);
	CPet* GetPet(long lID);
	void AddPet(CPet* pPet);
	void DelPet(const CGUID& rID);
	void ReleaseAllPets(void);
	void ActivateAllPets(bool bActive = true);
	void SetFocusPet(long lType);
	const CGUID& GetFocusPet(void);
	CPet* GetActivePet(long lType);
	bool AddActivePet(CPet* pPet);
	void DelActivePet(long lType);
	void CheckPetsMove(void);
	void ChangePetsState(long lState);

private:
	void DelPet(ITR_PET& itr);					// 玩家删除宠物
	void AddPetsExp(long lDelta, long lType);	// 修正宠物经验
	void AddPetsLev(void);						// 修正宠物等级
	void AddActivePet(long lType, const CGUID& rID);

private:
	PETS					m_Pets;				// 所有宠物列表
	ACT_PETS				m_ActivePets;		// 激活宠物列表
	CMonster*				m_pTameTarget;		// 驯服的目标(主要针对巡守)
	long					m_lFocusPet;		// 焦点宠物

protected:
	bool m_bInChangingRegion;			// 是否在切换场景	

	/////////////////////////////////////////////////////////////////////
	// 消息响应
	/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage*);
	long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);


private:
	long m_lAICount;			// AI时间
	//移动验证数据 
	DWORD	m_dwMoveValiStartTime;//移动验证开始的计时
	DWORD	m_dwLastRecvMsgTime;  //上次接受到移动消息的时间
	float	m_fMaxPeriodMoveSpeed;	//在一个时间段里,统计出的最大移动速度。
	CGUID	m_guMoveValiStartRgnID;//开始验证的场景ID
	long	m_lMoveValiStartPosX;//开始移动的X位置
	long	m_lMoveValiStartPosY;//开始移动的Y位置
	float	m_fMoveValiStartSpeed;//移动验证开始速度
	float	m_fMoveValiTotalDistance;//移动的总距离
	float	m_fMoveValiRecvMsgCount;//收到的移动消息个数
public:
	//添加一个移动消息
	void AddMoveMsg(float fNewX,float fNewY);
	//验证每一步的移动
	bool CheckMoveStep(float fCurX,float fCurY);
	//移动速度验证
	bool CheckMoveSpeedValidate();
	//重设相关数据
	void ResetMoveValiData();
	// 处理行走请求
	void	OnQuestMoveStep	(float fCurX,float fCurY,float fDestX,float fDestY);
	void	OnQuestStopMove	(float fCurX,float fCurY,short lDir);
	virtual bool	CanMove				();
	virtual VOID	OnCannotMove		();
	virtual void EndBuff(DWORD dwStateID);//根据ID结束状态

	VOID SetPosition( LONG lX, LONG lY );

	VOID SetSlipPos( LONG lX, LONG lY ,float fSpeed, float fDir);

	//##切换点的检测。
	BOOL			CheckSwitchPoint();

	/////////////////////////////////////////////////////////////////////
	// 补充HP/MP/MA
	/////////////////////////////////////////////////////////////////////
private:
	//##最后一次加血的时间戳.
	DWORD								m_dwLastRestoreHpTimeStamp;
	//##最后一次加MP的时间戳.
	DWORD								m_dwLastRestoreMpTimeStamp;

public:
	BOOL			RestoreHp			( DWORD dwHp, DWORD dwPersistTime, DWORD dwFrequency );
	BOOL			RestoreMp			( DWORD dwMp, DWORD dwPersistTime, DWORD dwFrequency );

	/////////////////////////////////////////////////////////////////////
	// 定时PING
	/////////////////////////////////////////////////////////////////////
protected:
	DWORD m_dwLastPingTime;		// 上次发送ping的时间
public:
	DWORD GetLastPingTime()			{return m_dwLastPingTime;}
	void SetLastPingTime(DWORD l)	{m_dwLastPingTime=l;}

	//-------------------------------------------------------------------
	// 禁言处理的逻辑:
	// 每次服务器启动,或者用户登陆的时候,首先从数据库中加载禁言时间,如果
	// 没有被禁言,那么就设置m_lSilenceTime和m_lSilenceTimeStamp为0,否则
	// 就设置m_lSilenceTimeStamp为登陆时间,m_lSilenceTime为数据库的禁言
	// 时间.
	// 在用户运行期间禁言，则直接设置好m_lSilenceTimeStamp 和
	// m_lSilenceTime 这两个变量。
	// 注意: 在切换服务器的时候一定要检查剩余的禁言时间.
	//--------------------------------
	//-----------------------------------
private:
	long m_lSilenceTime;			// 禁言的时间（单位：分钟）
	long m_lSilenceTimeStamp;		// 禁言的时间戳（单位：分钟）
	bool m_bGMInvincible;			// 表示GM是否无敌
	bool m_bGMConcealment;			// GM是否隐蔽
	bool m_bNetTransferStatus;		//　该玩家的网络传输状态false:表示良好,1:表示拥塞

public:
	/**## 设置禁言的时间（单位：分钟）*/
	void					SetSilence			(long lTime);

	/**## 判断是否为禁言时间内. */
	bool					IsInSilence			();

	/**## 对此函数，直接返回m_lSilenceTime而不经过逻辑处理的理由：       */
	/**## 用户在禁言期间退出的话，那么在系统OnLost消息相应的期间，        */
	/**## 直接计算 m_lSilenceTime 为剩余的禁言时间，m_lSilenceTimeStamp */
	/**## 设置为用户OnLost的时刻.                                         */ 
	/**## 此函数只设计为数据库存储接口使用                                */
	long					GetSilence			()			{ return m_lSilenceTime; }

	bool GetIsGMInvincible()	{return m_bGMInvincible;}
	bool GetIsGMConcealment()	{return m_bGMConcealment;}

	void SetNetTransferStatus(bool b)	{m_bNetTransferStatus = b;}
	bool GetNetTransferStatus()			{return m_bNetTransferStatus;}

	//////////////////////////////////////////////////////////////////////////
	//	战斗
	//////////////////////////////////////////////////////////////////////////
public:
	bool	IsVisible(CMoveShape* pTarget);
	virtual bool IsAttackAble(CMoveShape* pAttackShape);	// 可否攻击
	long				OnBeenAttacked			( CMoveShape* pAttacker,long lHp,long lDamageResult=0);
	void	OnBeenKilled(CMoveShape* pAttacker);

	virtual long GetAckRangeMin()					{return 1;}
	virtual long GetAckRangeMax()					{return 1;}

	//##盔甲的损耗
	VOID					OnWeaponDamaged		();
	//##护具的损耗
	VOID					OnArmorDamaged		();
	//##道具损耗的通用函数
	VOID					OnEquipmentWaste	( CEquipmentContainer::EQUIPMENT_COLUMN ec, CGoods* pEquipment );

	virtual void			OnDied				();								// 死亡处理
	//##被攻击后的AI处理
	virtual VOID			OnBeenHurted		( LONG lAttackerType, const CGUID& AttackerID );
	virtual VOID			OnBeenMurdered		( LONG lMurdererType, const CGUID& MurdererGuid );
	VOID					ReportMurderer		( const CGUID& MurdererID );
	bool					IsBadman			();

	//##一些战斗相关的状态函数
	virtual VOID			EnterCriminalState	();
	virtual VOID			EnterResidentState	();

	//##检查战斗/和平,罪犯/正常玩家的状态.
	virtual VOID			UpdateCurrentState	();

	// 通告玩家被攻击消息
	VOID		SendNotifyWarningMsg(bool isKilled, CPlayer* pAttacker);

protected:
	long					m_lFightStateCount;				// 攻击状态记数器(递减，到0表示结束攻击状态)
	DWORD					m_dwSinStateTimeStamp;

	DWORD					m_dwContinuousKillAmount;		// 当前连击的数量
	DWORD					m_dwContinuousKillTimeStamp;	// 上一次击中的时间

	long					m_lFightStateTimerID;//战斗状态定时器
public:

	//##增加连击数
	VOID					IncreaseContinuousKill();

	virtual FLOAT			GetWeaponModifier( LONG lTargetLevel );
	//////////////////////////////////////////////////////////////////////////
	// 组队
	//////////////////////////////////////////////////////////////////////////

public:
	const CGUID& GetTeamID() { return m_TeamID; }
	void SetTeamID(const CGUID& guid) { m_TeamID = guid;}
	void SetCaptain(bool b) { m_bCaptain = b; }
	bool GetCaptain(void) { return m_bCaptain; }

	virtual	void	OnAddBuff(DWORD dwStateID, DWORD dwBuffLevel);
	virtual	void	OnDelBuff(DWORD dwStateID, DWORD dwBuffLevel);

	void SetRecruitState(bool bIsRecruiting, char* pRecruitName, char* pPassword);
	tagRecruitState& GetRecruitState(void);

	long GetSentaiPoints(void)				{return m_SentaiPoints;}
	void SetSentaiPoints(long Points)		{m_SentaiPoints = Points;}


private:
	CGUID m_TeamID;
	bool m_bCaptain;
	tagRecruitState m_RecruitState;

	//! 战队积分
	long m_SentaiPoints;
	//////////////////////////////////////////////////////////////////////////
	//	技能
	//////////////////////////////////////////////////////////////////////////

public:
	typedef map<long,tagSkill>::iterator itSkill;
	//判断技能冷却时间是否到达
	bool CooldownPast(long lSkillID);
	//判断物品冷却
	bool GoodsCoolDown(long lGoodsIndex);
	//使用物品成功
	void SendUseItemSucceed(long lGoodsIndex);

	bool CheckStudySkill(DWORD dwID,DWORD &dwSkillLv);//学习新技能判断
	void StudySkill(DWORD dwID,DWORD dwSkillLv);//学习新技能


	//职业等级判断
	bool OccuLvJudgeStudy(DWORD dwID,DWORD dwLv);
	//职业学习条件
	bool OccuJudgeStudy(DWORD dwID,DWORD dwLv);
	//等级学习条件	
	bool LevelJudgeStudy(DWORD dwID,DWORD dwLv);
	//SP学习条件		
	int SPJudgeStudy(DWORD dwID,DWORD dwLv);
    //Exp学习条件
    LONG64 ExpJudgeStudy(DWORD dwID, DWORD dwLv);
	//前置技能学习条件	
	bool SkillJudgeStudy(DWORD dwID,DWORD dwLv);
	//前置限制技能学习条件
	bool LimitSkillJudgeStudy(DWORD dwID,DWORD dwLv);
	//物品条件
	bool GoodJudgeStudy(DWORD dwID,DWORD dwLv);
	//技能学习失败
	void StudySkillFailed(long lValue);
	//学习技能触发脚本
	void StudyKillScript(DWORD dwID,DWORD dwLv);
	//添加技能判断
	bool CanAddSkill(DWORD dwID, DWORD dwLv);

	//返回技能列表
	void SendBackSkill(string strName,CGUID guidNpc=NULL_GUID);
	//职业判断
	bool CareerJudge(string strName);
	//启动被动技能
	void StartUpPassiveSkill();

	map<long,tagSkill>& GetSkillMap(void) { return m_Skills; }
	virtual void SetCommonCooldown(long lID, DWORD dwTime);
	virtual void SetSkillCooldown(long lID,DWORD dwTime);
	virtual void AddSkill(long id,long lv,long lCoolDown=0,time_t tEndTime=0);


	LONG CheckSkill(long id);

	virtual VOID OnChangeStates();								// 改变状态后，发送消息给玩家
	LONG GetNumSkills();										// 获取技能的个数
	LONG GetSkillLv(long lSkillID);

	VOID CodeSkillsToDataBlock_ForClient(DBWriteSet& setWriteDB, BOOL bAddRestoreTime = FALSE );	// 将所有的技能添加到队列	
	VOID DecodeSkillsFromDataBlock(DBReadSet& setReadDB);	// 将缓冲中的技能数据解码

	virtual long AddActiveSkill(DWORD dwSkillID, DWORD dwLevel,CMoveShape *pTarget=NULL,long lGoodsIndex=0,long lContainerID=0,long lPos=0,CGUID GoodsGuid=NULL_GUID);
	virtual BOOL DelSkill( long lSkillID ,long lSkillLv);
	void DeleteGoodsInPacket(char* strGoodName);
private:

	//玩家所有的技能
	map<long,tagSkill> m_Skills;
public:
	struct tagOwnedReg
	{
		long lRegionID;						//场景的ID
		WORD wType;							//场景的类型
	};
	//帮会系统
private:
	CGUID					m_FactionID;							//! 自己帮会ID
	CGUID					m_UnionID;								//! 自己帮会ID
	const tagFacMemInfo*	m_pFacMemInfo;							//! 自己的工会信息	
	char					m_szFactionName[MAXSIZE_FactionName];	//! 帮会名字
	char					m_szUnionName[MAXSIZE_FactionName];		//! 联盟名字
	LONG					m_lFacDemiseWarning;					//! 帮会传位提醒倒记时
	LONG					m_lFacDisbandWarning;					//! 帮会解散提醒倒记时

public:


	const CGUID&			GetFactionID(void)								{return m_FactionID;}
	void					SetFactionID(const CGUID& FactionID);
	void					SetFactionInfo(GameFaction *pFaction);
	const CGUID&			GetUnionID(void)								{return m_UnionID;}
	void					SetUnionID(const CGUID& UnionID);
	const char*				GetFactionName()								{ return m_szFactionName; }
	const char*				GetUnionName()									{ return m_szUnionName; }
	const tagFacMemInfo*	GetFacMemInfo(void)								{return m_pFacMemInfo;}
	void					SetFacMemInfo(const tagFacMemInfo* pFacMemInfo)	{m_pFacMemInfo = pFacMemInfo;}
	//物品触发技能
	bool	ItemBeginSkill(DWORD dwID, DWORD dwLevel, CMoveShape *pDestShape);
	int     JudgeSkillBegining(DWORD dwSkillID, DWORD dwLevel,CMoveShape *pTarget=NULL,long lGoodsIndex=0);//技能启动前的判断


private:
	CRoleQueue m_RoleQueue;
	//俑兵任务
	CMercenaryQuestSys	m_MerQuestSys;
	bool       m_bLogin;
public:
	bool CodeQuestDataToDataBlock(DBWriteSet& setWriteDB);		//添加任务数据到ByteArray
	bool CodeQuestDataToDataBlock_ForClient(DBWriteSet& setWriteDB);//添加任务数据到ByteArray
	bool DecodeQuestDataFromDataBlock(DBReadSet& setReadDB);

	eQuestState GetQuestState(LONG wQuestID);					//得到一个任务的状态
	long GetValidQuestNum();									//得到有效的任务数量
	BOOL AddQuest(LONG lQuestID);								//给该玩家添加一个任务
	BOOL AddQuest(CRoleRecord*& p);								//添加一个任务

	bool CompleteQuest(DWORD dwQuestID);						//该玩家完成一个任务
	void RemoveQuest(LONG lQuestID);							//移出一个任务
	void UpdateQuestStep(DWORD dwQuestID,long lStep);			//更新一个任务的特效显示坐标
	void UpdataQuestData(DWORD dwQuestID,long lParamIndex,long lVal);
	void UpdataQuestParamData(DWORD dwQuestID,long lParamIndex, long lParam,long RegionID,long RegionX, long RegionY);
	void ShowTalkBox(DWORD wQuestID,long lTextStep);
	void PlayerRunScript(char *strScriptName);					//给该玩家运行一个脚本
	long GetCurrentMasterRoleID();								//取得当前主线任务id

	//用兵任务//start
	bool CodeMerQuestDataToDataBlock(DBWriteSet& setWriteDB);	//添加俑兵任务数据到ByteArray
	bool DecodeMerQuestDataFromDataBlock(DBReadSet& setReadDB);//解俑兵任务数据

	//玩家进入GS以后初始化相关俑兵任务数据
	void IniMercQuest();

	//是否能刷新
	enum FreshType{FT_NO=0 , FT_YES , FT_NEEDGOODS};
	long CanFreshMercQuest(long lMercListID);
	////刷新
	void FreshMercQuest(long lMercListID,bool usegoods=false);

	//判断是否是俑兵列表任务
	bool IsMercListQuest(long lQuestID);
	//判断是否是放弃任务列表中的任务
	bool IsDisbandQuest(long lQuestID);
	//玩家退出GS时,释放相关俑兵数据
	//获得某个列表ID的俑兵任务
	void AcquireMercQuestList(long lMercListID,bool usegoods=false);
	bool IsMerQuestInit(long lMercListID);
	//发送俑兵任务数据到客户端
	void SendMercQuestDataToClient(long lMercListID);
	//打开俑兵任务界面
	void OpenMercQuestInterface(long lMercListID);
	//重置俑兵任务刷性时间
	void SetMercQuestTime(long lMercListID);
	DWORD GetMercQuestTime(long lMercListID);
	//俑兵任务
	long GetMercQuest(){return m_MerQuestSys.GetCurQuestID();}
	void SetMercQuest(long id,bool bcall=true);
	//赏金任务
	void OpenShareQuestInterface();
	long GetTeamCurrQuest();
	void SetTeamCurrQuest(long lQuestID);
	void DeleteTeamQuest(long lQuestID);
	bool UpdateTeamQuestList(long lQuestID);
	void OnCompleteQuest(long lQuestID);
	bool IsCompleteQuest(long lQuestID);
	DWORD GetQuestOdds(long lQuestID);
	long CanAcceptMercQuest(long lQuestID);
	void SetMasterQuest(long lQuestID);
	//用兵任务//end
	//任务索引
	void OpenIndexQuestInterface();
	void SetIndexQuestParamNum(long lQuest,long lParam);
	void SetIndexQuestState(long lQuest,long lState);
	long GetIndexQuestParamNum(long lQuest);
	long GetIndexQuestState(long lQuest);

	// 玩家是否登录该GS标志，用于清除因远程创建副本造成的代理玩家对象数据累积
	bool	GetPlayerLogin()		{return m_bLogin;}
	void	SetPlayerLogin(bool b)	{m_bLogin = b;}

	DWORD	GetWeaponDamageLevel();		// 获取当前使用的武器的攻击强度
	//任务脚本函数

public:

	//。。。。。。。。。对玩家身上的数据的操作。。。。。。。。。

	//
	LONG GetRoleStepNum(LONG ID){return m_RoleQueue.GetRoleStepNum(ID);}
	//寻找某个任务的某一步骤是否存在
	BOOL FindRoleStep(LONG ID , LONG Step){return m_RoleQueue.FindRoleStep(ID,Step);}		

	//取得 某一任务某一步骤的Index对应的值
	LONG GetRoleParam(LONG RoleID, LONG Step ,LONG Index){return m_RoleQueue.GetParam(RoleID,Step,Index);}

	//设置 某一任务某一步骤的Index对应的值
	BOOL SetRoleParam(LONG RoleID,  LONG Step,LONG Index, LONG val){return m_RoleQueue.SetParam(RoleID,Step,Index,val);}

	//增加 某一任务某一步骤的Index对应的值
	BOOL AddRoleParam(LONG RoleID, LONG Step ,LONG Index, LONG val){return m_RoleQueue.AddParam(RoleID,Step,Index,val);}

	//取得 某一任务某一步骤的Index对应的值
	LONG GetRoleAccParam(LONG RoleID, LONG Step ,LONG Index){return m_RoleQueue.GetAccParam(RoleID,Step,Index);}

	//设置 某一任务某一步骤的Index对应的值
	BOOL SetRoleAccParam(LONG RoleID,  LONG Step,LONG Index, LONG val){return m_RoleQueue.SetAccParam(RoleID,Step,Index,val);}

	//增加 某一任务某一步骤的Index对应的值
	BOOL AddRoleAccParam(LONG RoleID, LONG Step ,LONG Index, LONG val){return m_RoleQueue.AddAccParam(RoleID,Step,Index,val);}

	//取得 某一任务某一步骤的Index对应的场景id
	LONG GetRoleRegionID(LONG RoleID, LONG Step ,LONG Index){return m_RoleQueue.GetRegionID( RoleID,  Step , Index);}

	//设置 某一任务某一步骤的Index对应的场景id
	BOOL SetRoleRegionID(LONG RoleID,  LONG Step,LONG Index, LONG val){return m_RoleQueue.SetRegionID( RoleID,   Step, Index,  val);}

	//取得 某一任务某一步骤的Index对应的场景x坐标
	LONG GetRoleRegionX(LONG RoleID, LONG Step ,LONG Index){return m_RoleQueue.GetRegionX( RoleID,  Step , Index);}

	//设置 某一任务某一步骤的Index对应的场景x坐标
	BOOL SetRoleRegionX(LONG RoleID,  LONG Step,LONG Index, LONG val){return m_RoleQueue.SetRegionX( RoleID,   Step, Index,  val);}

	//取得 某一任务某一步骤的Index对应的场景y坐标
	LONG GetRoleRegionY(LONG RoleID, LONG Step ,LONG Index){return m_RoleQueue.GetRegionY( RoleID,  Step , Index);}

	//设置 某一任务某一步骤的Index对应的场景y坐标
	BOOL SetRoleRegionY(LONG RoleID,  LONG Step,LONG Index, LONG val){return m_RoleQueue.SetRegionY( RoleID,   Step, Index,  val);}
	//*******************************************
	//取得任务列表的任务数量
	LONG GetRoleSum(){return m_RoleQueue.GetRoleSum();}

	BOOL AddOneRecord(LONG roleid);
	//删除一个任务
	BOOL DelOneRecord(LONG ID){return m_RoleQueue.DelOneRecord(ID);}

	//取得所在主线任务章节
	LONG GetChapter(){return m_RoleQueue.GetChapter();}

	//设置主线任务章节数
	BOOL SetChapter(LONG num);

	//移动主线任务章节数到下一章节
	BOOL JumpChapterNext(){return m_RoleQueue.JumpChapterNext();}

	//添加就完成任务记录
	BOOL AddFinishedRoleRecord(LONG index);
	BOOL DelFinishedRoleRecord(LONG index);

	//取得一个索引的任务的状态（是否完成）
	BOOL GetRoleFinishedRecord(LONG index){return m_RoleQueue.GetRoleFinishedRecord(index);}
	//获取一个任务现在对应的文本index
	LONG GetStepText(LONG lRoleID,LONG lTextID);
	LONG GetText(LONG lRoleID,LONG StepID, LONG lTextID);
	//。。。。。。。。。。对任务系统的操作。。。。。。。。。。。。。

	//得到俑兵任务参数值
	long GetMercQuestData(long lQuestListID,long lPos){return m_MerQuestSys.GetData(lQuestListID,lPos);}
	//设置俑兵参数值 
	void SetMercQuestData(long lQuestListID,long lPos,long lValue){ m_MerQuestSys.SetData(lQuestListID,lPos,lValue);}
private:
	const CQuest*		GetQuest(LONG RoleID);								//寻找一个任务
	const CQuestStep* GetStep(LONG RoleID, LONG StepID);				//寻找步骤
	const CQuestAim*  GetAim(LONG RoleID, LONG StepID,LONG AimID);	//寻找任务目标
	const CQusetEncouragement *GetQuestEncouragement(LONG RoleID,LONG Index);
	const CQusetEncouragement *GetStepEncouragement(LONG RoleID, LONG StepID,LONG Index);
public:
	//取得文本类型
	LONG GetTextType(LONG RoleID);
	//获取任务名字
	const char* GetAttributeRoleName(LONG);
	//是否被记录
	LONG GetAttributeBeRecord(LONG RoleID);
	//任务类型
	LONG GetAttributeRoleType(LONG RoleID);
	//任务接受npc的id
	LONG GetAttributeEmbracerNPCID(LONG RoleID);
	//是否被显示
	LONG GetAttributeShowAble(LONG RoleID);
	//是否能反复接
	LONG GetAttributeRepeated(LONG RoleID);
	//死亡是否失败
	LONG GetAttributeDeadLost(LONG RoleID);
	//任务失败后能否重新接
	LONG GetAttributeLostRepeated(LONG RoleID);
	//最小等级限制
	LONG GetAttributeMinGrade(LONG RoleID);
	//最大等级限制
	LONG GetAttributeMaxGrade(LONG RoleID);
	//职业限制
	BOOL GetAttributeOccupationRestrict(LONG RoleID,INT index);
	//任务几率
	LONG GetAttributeOdds(LONG RoleID);
	//需要佣兵声望
	LONG GetAttributeEngageArmsRepute(LONG RoleID);
	//下一任务的id
	LONG GetAttributeNextRoleID(LONG RoleID);
	//章节数
	LONG GetAttributeChapterNum(LONG RoleID);
	//完成效果
	LONG GetAttributeFinishEffect(LONG RoleID);
	//是否受经验倍率影响
	LONG GetAttributeExpRatio(LONG RoleID);


	//取得index对应的前置任务
	LONG GetLabelPrefix(LONG RoleID,LONG Index);
	//取得前置任务数量
	LONG GetLabelSum(LONG RoleID);

	//目标模板类型
	LONG GetAimAMTType(LONG RoleID,LONG StepID,LONG AimID);
	//目标类型
	LONG GetAimType(LONG RoleID,LONG StepID,LONG AimID);
	//取得目标id
	LONG GetAimId(LONG RoleID,LONG StepID,LONG AimID);
	//取得目标数量
	LONG GetAimNum(LONG RoleID,LONG StepID,LONG AimID);
	//取得 变量索引
	LONG GetAimParameIndex(LONG RoleID,LONG StepID,LONG AimID);
	//掉落这种物品的生物类型（对物品）
	LONG GetDropFrom(LONG RoleID,LONG StepID,LONG AimID);
	//几率
	LONG GetDropOdds(LONG RoleID,LONG StepID,LONG AimID);
	//取得目标的名字
	const char* GetAimName(LONG RoleID,LONG StepID,LONG AimID);
	//取得备用属性
	LONG GetPrepareParam(LONG RoleID,LONG StepID,LONG AimID,LONG index);


	LONG GetStepEncouragementSum(LONG RoleID,LONG StepID);
	LONG GetQuestEncouragementSum(LONG RoleID);
	//取得奖励的名字
	const char* GetSetpEncouragementName(LONG RoleID,LONG Step,LONG Index);
	const char* GetQuestEncouragementName(LONG RoleID,LONG Index);
	//取得奖励类型
	LONG GetStepEncouragementType(LONG RoleID,LONG Step,LONG Index);
	LONG GetQuestEncouragementType(LONG RoleID,LONG Index);
	//奖励的具体id
	LONG GetStepEncouragementID(LONG RoleID,LONG Step,LONG Index);
	LONG GetQuestEncouragementID(LONG RoleID,LONG Index);
	//奖励数量
	LONG GetStepEncouragementNum(LONG RoleID,LONG Step,LONG Index);
	LONG GetQuestEncouragementNum(LONG RoleID,LONG Index);
	//奖励所占的格子数
	LONG GetStepEncouragementCell(LONG RoleID,LONG Step);
	LONG GetQuestEncouragementCell(LONG RoleID);

	//执行奖励
	BOOL PerformEncouragement(LONG RoleID,LONG StepID);
	BOOL PerformEncouragement(LONG RoleID);
	//单个任务相关
	LONG GetStepSum(LONG RoleID);
	//取得步骤下的目标数量
	LONG GetStepAimNum(LONG RoleID,LONG StepID);
	//任务系统相关
	LONG FindRoleIsInSystem(LONG RoleID);
	//一个任务的某一步骤是否在任务系统中
	BOOL FindRoleStepIsInSystem(LONG RoleID,LONG StepID);
	//任务引导相关
private:
	map<long,CQuestIndexXml::tagQuestIndexForServer>  m_mapQuestIndex;      //记录任务引导的相关信息
public:
    void AddQuestIndex_Param(long lQuest,long lParam);
	void AddQuestIndex_State(long lQuest,long lState);
	void ResetQuestIndex(long lQuest);    //重新设置一个任务引导的状态，实际从列表中移除
	void CheckUpateTime();     //检查任务引导是否已达到刷新的时间
	bool IsResetTime(CQuestIndexXml::tagQuestIndexForServer IndexQuest);   //检查一个任务是否到了刷新重置时间，在打开界面和改变一个属性时调用

	//任务end
	//-----------------------------------------------------------------------------------------
	//  自动断线功能
	//  延迟n秒后断开Socket连接，缺省的是0秒，立刻断开。
	//-----------------------------------------------------------------------------------------
private:
	DWORD	m_dwDisconnectTimeStamp;
	DWORD	m_dwDisconnectDelayTime;

public:
	VOID	Disconnect( DWORD dwDelayTime = 0 );

	//-----------------------------------------------------------------------------------------
	//  交易，仓库，摆摊等过程中的过程标记．
	//  
	//-----------------------------------------------------------------------------------------
	enum eProgress
	{
		PROGRESS_NONE,									//##没有任何状态
		PROGRESS_BANKING,								//##仓库
		PROGRESS_TRADING,								//##交易
		PROGRESS_SHOPPING,								//##从NPC处购买物品
		PROGRESS_OPEN_STALL,							//##摆摊
		PROGRESS_INCREMENT,								// 使用增值商店
		PROGRESS_BOUNS,									// 领奖
		PROGRESS_FBUSINESS,								// 家族跑商商店
		PROGRESS_FORCE_DWORD	= 0x7fffffff
	};

private:
	eProgress	m_eProgress;
	CGUID		m_PersonalShopSessionID;
	long		m_PersonalShopPlugID;

public:
	eProgress	GetCurrentProgress						();
	VOID		SetCurrentProgress						( eProgress ep );
	VOID		SetPersonalShopFlag						( const CGUID& SessionID, long PlugID );
	CGUID       GetPersonalShopSessionID()              { return m_PersonalShopSessionID;};
	long        GetPersonalShopPlugID()                 { return m_PersonalShopPlugID;};

	//-----------------------------------------------------------------------------------------
	//  发送消息给当前用户.显示到聊天对话框内
	//  
	//-----------------------------------------------------------------------------------------

public:
	VOID		SendNotifyMessage( char* szText, DWORD dwColor = 0xffffffff, DWORD dwBkColor = 0,eNOTIFY_POS ePos=eNOTIFYPOS_CENTER);
	VOID		SendSystemInfo( char* szText, DWORD dwColor = 0xffffffff );
	VOID		SendOtherInfo( char* szText );
	VOID		SendPlayerInfo( char* szText, DWORD dwColor = 0xffffffff, DWORD dwBkColor = 0 );
	friend class CMonster;


	//-----------------------------------------------------------------------------------------
	//  表情相关
	//  
	//-----------------------------------------------------------------------------------------
private:
	LONG		m_lEmotionIndex;
	DWORD		m_dwEmotonTimeStamp;

public:
	VOID		PerformEmotion( LONG lID );
	VOID		ClearEmotion();
	VOID		PackGlint();

	//-----------------------------------------------------------------------------------------
	//  坐骑相关
	//  
	//-----------------------------------------------------------------------------------------

	BOOL		Mount( DWORD dwMountType, DWORD dwMountLevel, DWORD dwRoleLimit,const char* strGoodName );
private:
	DWORD m_dwClientIP;
public:
	DWORD GetClientIP() {return m_dwClientIP;}
	void SetClientIP(DWORD dwClientIP) {m_dwClientIP=dwClientIP;}

	//-----------------------------------------------------------------------------------------
	//  拾取,丢弃物品相关
	//  
	//-----------------------------------------------------------------------------------------
public:

	virtual BOOL OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);

	virtual BOOL OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid);
	//容器测试成功后，添加物品并通知客户端
	BOOL AddGoodsByTest(tagTestResult* tgResult,CGoods* pGoods);
	//容器测试成功后，删除物品并通知客户端
	BOOL DelGoodsByTest(tagDelTestResult* tgResult);
	//////////////////////////////////////////////////////////////////////////
	//	容器脚本
	//////////////////////////////////////////////////////////////////////////
public:
	void	SetLastContainerScript(const char* szScript)	{m_strLastContainerScript = szScript;}
	const char *GetLastContainerScript()					{return m_strLastContainerScript.c_str();}

	//添加一个帮助按钮提示
	long AddHelpHint(const char* pszScript,long lFutureDelTime);
	//移出一个帮助提示
	void RemoveHelpHint(long lHintID);
	//当玩家点击帮助提示按钮
	void OnPlayerHitHelpHint(long lHintID,long lFlag);
	//添加一个界面提示
	long AddInterfaceHint(const char* pszScript,long lFutureDelTime,
		const char* pszInterName,long lX,long lY,long lWidth,long lHeight);
	//移出一个界面提示
	void RemoveInterfaceHint(long lHintID);
	//当玩家点击界面提示
	void OnPlayerHitInterfaceHint(long lHintID,long lFlag);
	//当提示超时时
	void OnHintTimeOut(long lHintID);
	//清除所有提示
	void ClearAllHint();

private:
	string	m_strLastContainerScript;		// 保存最后一个准备执行的容器脚本

	struct tagHint
	{
		//点击之后执行的脚本
		string	strScript;
		//定时器ID
		long	lTimerID;
		//标记
		long	lFlag;
	};
	typedef map<long,tagHint>	mapHints;
	typedef map<long,tagHint>::iterator itHint;

	//提示的计数器
	long m_nHintCounter;
	//帮助提示
	mapHints m_HelpHints;
	//界面提示
	mapHints m_InterfaceHints;

public:
	const CGUID& GetPersonalRgnGUID(void);
	const CGUID& GetPersonalHouseRgnGUID(void);
	void SetPersonalRgnGUID(const CGUID& guid);
	void SetPersonalHouseRgnGUID(const CGUID& guid);

	//!	注册一个排队定时器，并返回定时器ID
	long RegisterQueueTimerID(long lFutureTime);
	//!	取消一个排队的Timer
	void CancelQueueTimer(void);
	//!	响应排队定时器触发的事件
	void OnQueueTimeout(long lTimerID);

	//!	注册一个创建P副本定时器，并返回定时器ID
	long RegisterCreatePRgnTimerID(long lFutureTime);
	//!	取消一个创建P副本的Timer
	void CancelCreatePRgnTimer(void);
	//!	响应创建P副本定时器触发的事件
	void OnCreatePRgnTimeout(long lTimerID);

	//!	注册一个创建T副本定时器，并返回定时器ID
	long RegisterCreateTRgnTimerID(long lFutureTime);
	//!	取消一个创建T副本的Timer
	void CancelCreateTRgnTimer(void);
	//!	响应创建T副本定时器触发的事件
	void OnCreateTRgnTimeout(long lTimerID);

	//!	注册一个副本清人定时器，并返回定时器ID
	long RegisterRgnKickPlayerTimerID(long lFutureTime);
	//! 注册一个防沉迷系统踢人的定时器，并返回定时器ID
	long RegisterFcmKickPlayerTimerID(long lFutureTime);
	//!	取消一个创建T副本的Timer
	void CancelRgnKickPlayerTimer(void);
	//!	响应创建T副本定时器触发的事件
	void OnRgnKickPlayerTimeout(long lTimerID);

	// 得到创建副本队列定时器ID
	long GetQueueTimerID(void) { return m_lQueueTimerID; }
	// 设置创建副本队列定时器ID
	void SetQueueTimerID(long value) { m_lQueueTimerID = value; }

	// 得到创建副本队列定时器ID
	long GetPRgnTimerID(void) { return m_lPRgnTimerID; }
	// 设置创建副本队列定时器ID
	void SetPRgnTimerID(long value) { m_lPRgnTimerID = value; }

	// 得到创建副本队列定时器ID
	long GetTRgnTimerID(void) { return m_lTRgnTimerID; }
	// 设置创建副本队列定时器ID
	void SetTRgnTimerID(long value) { m_lTRgnTimerID = value; }

	// 玩家希望创建的DupRgn类型
	eRgnType GetCreateDupRgnType(void) { return m_CreateDupRgnType; }
	void SetCreateDupRgnType(eRgnType type) { m_CreateDupRgnType = type; }
	// 玩家希望创建的DupRgn的模板ID
	long GetlCreateDupRgnTID(void) { return m_lCreateDupRgnTID; }
	void SetlCreateDupRgnTID(long value) { m_lCreateDupRgnTID = value; }

	// 玩家希望创建的DupRgn的脚本ID
	const CGUID& GetlCreateDupRgnScriptID(void) { return m_DupRgnScriptID; }
	void SetlCreateDupRgnScriptID(const CGUID& value) { m_DupRgnScriptID = value; }

	long GetCurPRgnNum(void) { return m_CurPRgnNum; }
	long GetCurTRgnNum(void) { return m_CurTRgnNum; }

	void SetCurPRgnNum(long num) { m_CurPRgnNum = num; }
	void SetCurTRgnNum(long num) { m_CurTRgnNum = num; }

	// 得到玩家状态类型
	ePlayerState	GetPlayerState(void) { return m_PlayerState; }
	// 设置玩家状态类型
	void			SetPlayerState(ePlayerState state) { m_PlayerState = state; }

	DWORD GetSavePartFlag(void) { return m_PlayerSavePartFlag; }
	void  SetSavePartFlag(DWORD flag) { m_PlayerSavePartFlag = flag; } 

	bool	GetFirstLogin()		{return b_FirstEnterGame;}
	void	SetFirstLogin(bool b)	{b_FirstEnterGame = b;}

	long    GetSignCode(void) { return m_lSignCode; }
	void	SetSignCode(long code) { m_lSignCode = code; }

private: 
	// 是否第一次登录
	bool	b_FirstEnterGame;
	CGUID	m_PersonalRgnGUID;
	CGUID	m_PersonalHouseRgnGUID;	
	long	m_lQueueTimerID;//创建副本队列定时器

	long m_lPRgnTimerID;
	long m_lTRgnTimerID;
	long m_lRgnKickPlayerTimerID;

	long m_CurPRgnNum;
	long m_CurTRgnNum;
	// 玩家希望创建的DupRgn类型
	eRgnType   m_CreateDupRgnType;
	// 玩家希望创建的DupRgn的模板ID
	long		m_lCreateDupRgnTID;
	// 创建副本的脚本ID（被挂起）
	CGUID        m_DupRgnScriptID;
	// 玩家状态类型
	ePlayerState	m_PlayerState;
	// 玩家要保存的属性部分枚举值
	DWORD m_PlayerSavePartFlag;
	// 登录验证码
	long m_lSignCode;

	//-------------------------------------------------------------------------------
	// 会话ID. by Fox.		2008-01-22
	// func : 对应玩家在该GS上的计费唯一标识
	//-------------------------------------------------------------------------------
public:
	char *GetSessionID()			{ return m_szSessionID; }
	void SetSessionID(char *szID)	{ strcpy_s(m_szSessionID, 33, szID); }

private:
	char		m_szSessionID[33];
public:
	typedef map<DWORD,list<DWORD>> IncShopCur10Map;
	typedef IncShopCur10Map::iterator IncShopCur10Iter;

	//-------------------------------------------------------------------------------
	// 增值商店flag. by Fox.		2008-02-14
	// func : flag为true则发送商品信息，为false则不发送
	//-------------------------------------------------------------------------------
public:
	bool GetIncShopFlag()					{ return m_bIncShopFlag; }
	void SetIncShopFlag(bool bFlag = false)	{ m_bIncShopFlag = bFlag; }

	//增值商店最近购买的Top10
	void Add2IncShopCur10( DWORD dwMode,DWORD dwGoodsID );
	IncShopCur10Map &GetIncShopCur10()			{ return m_mapIncShopCur10; }
	void ClearIncShopCur10();

	void Add2IncTradeList( DWORD dwMode, DWORD dwGoodsId, DWORD dwGoodsNum, long lTotalAmount);
	CIncrementShopList::VEC_BUYLIST GetIncTradeList() { return m_vecTradeList; }
	void ClearIncTradeList() { m_vecTradeList.clear(); }

	//增值商店限量购买添加记录
	void AddLimitGoodsRecord(DWORD dwGoodsId,DWORD dwGoodsNum);
	//（增值商店）获取已经购买数量
	DWORD GetLimitGoodsBuyNum(DWORD dwGoodsId);

private:
	bool		m_bIncShopFlag;
	//增值商店最近购买的Top10记录
	IncShopCur10Map m_mapIncShopCur10;
	//增值商店购买详细记录列表
	CIncrementShopList::VEC_BUYLIST m_vecTradeList;
	//增值商店数量限购物品购买记录
	map<DWORD,DWORD> m_mapLimitGoodsRecord;


	//-------------------------------------------------------------------------------
	// 测试用点券. by Fox.		2008-02-15
	// func : 测试用点券
	//-------------------------------------------------------------------------------
public:

	long GetTicket()				{ return m_lTicket; }
	void SetTicket(long	lTicket);

private:
	long		m_lTicket;

private:
	//是否能采集
	tagUseFailed CanDOccuCollect(CMoveShape* moveshape);
	//采集
	BOOL DOccuCollect(CMoveShape* moveshape);
	//是否能任务采集
	tagUseFailed CanRoleCollect(CMoveShape* pMoveShape);
	//任务采集
	BOOL RoleCollect(CMoveShape* pMoveShape);
public:
	tagUseFailed CanCollect(CMoveShape* pMoveShape);
	tagUseFailed CheckCanCollect(CMoveShape* pMoveShape);
	BOOL CheckCollect(CMoveShape* pMoveShape);
	//锁定采集物
	VOID LockCollection(CMoveShape* pMoveShape);
	//解除采集物锁定
	VOID OutlockCollection(CMoveShape* pMoveShape);
	//是否能制作
	tagUseFailed CanFacture();
	//制作
	BOOL Facture();
	//制作的学习
	VOID FactureSkillStudyBegin(CMoveShape* moveshape);
	VOID StudyFactureSkill(CMoveShape* moveshape,DWORD factureID);
	VOID CleanFacture();
	VOID AddCollectionExp(DWORD exp);
	VOID AddFactureExp(DWORD exp);

	BOOL AddFacture(DWORD l);
	BOOL DeleteFacture(DWORD l);
	BOOL FindFacture(DWORD l){return m_DOccuRecord.FindFacture(l);}
	//发送副职业失败消息
	void SendDOccuSKillFailMsg(int nValue,int iSkillID, int iSkillLevel);
	//获取， 设置  选择出的制作id
	DWORD GetElectFactureID(){return m_dwElectFactureID;}
	void  SetElectFactureID(DWORD id){m_dwElectFactureID= id;}

	//对副职业系统的查询
	long GetFactureNeedsLevel(DWORD ID);
	long GetFactureNeedsDOccu(DWORD ID);

private:
	//副职业记录的操作
	LONG GetCollectionExp(){return m_DOccuRecord.m_lCollectionExp;}
	VOID SetCollectionExp(LONG l);
	LONG GetCollectionGrade(){return m_DOccuRecord.m_lCollectionGrade;}
	VOID SetCollectionGrade(LONG l);
	LONG GetFactureExp(){return m_DOccuRecord.m_lFactureExp;}
	VOID SetFactureExp(LONG l);
	LONG GetFactureGrade(){return m_DOccuRecord.m_lFactureGrade;}
	VOID SetFactureGrade(LONG l);
	LONG GetFactureSuccRatio(){return m_DOccuRecord.GetFactureSuccRatio();}
	VOID AddFactureSuccRatio(LONG l){m_DOccuRecord.AddFactureSuccRatio(l);}
	VOID ClearFactureSuccRatio(){m_DOccuRecord.ClearFactureSuccRatio();}

public:
	void GetPlayerIDAndTeammateID(hash_set<CGUID,hash_guid_compare>& hsPlayers);
private:
	CDeputyOccuRecord m_DOccuRecord;
	DWORD m_dwElectFactureID;

public:
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const char* value);
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray=0, double value=0);
	virtual const char* GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);
	virtual double GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray);

	virtual int SetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const CGUID& guid);
	virtual const CGUID& GetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);
	//刷新场景特效
	void UpdateEffect();//UPDATEEFFECT();


public:
	VOID SetServerKey(DWORD dwIdx, DWORD dwKey){if(SERVER_KEY_DWORD_SIZE - 1 < dwIdx)return; m_dwServerKey[dwIdx] = dwKey;}
	DWORD GetServerKey(DWORD dwIdx){if(SERVER_KEY_DWORD_SIZE - 1 < dwIdx)return 0; DWORD dwRe = m_dwServerKey[dwIdx]; m_dwServerKey[dwIdx] = 0; return dwRe;}

private:
	DWORD m_dwServerKey[SERVER_KEY_DWORD_SIZE];

	//-------------------------------------------------------------------------------
	// 小精灵系统
	// 
	//-------------------------------------------------------------------------------
public:
	SpriteSystem			m_SpriteSystem;

	//-------------------------------------------------------------------------------
	// 玩家需要保存在服务器的设置
	// 
	//-------------------------------------------------------------------------------
public:
	PlayerSetupOnServer		m_SetupOnServer;
	//-------------------------------------------------------------------------------
	// 勋章容器、当前称号
	// 
	//-------------------------------------------------------------------------------
public:
	//!						勋章容器
	MedalContainer			m_MedalContainer;

	//!						设置称号的显示类型
	BOOL					SetTitleType(LONG lType);
	//!						得到称号的显示类型
	LONG					GetTitleType(void){return m_lCurrTitleType;};
	//!						更新称号
	void					UpdateTitle(BOOL bSendNotify);

	//!						得到当前显示的勋章的原始名
	const	char*			GetShowMedalBaseName(void);
private:
	//!						当前称号类型
	LONG					m_lCurrTitleType;

	//-------------------------------------------------------------------------------
	// 通用物品操作容器
	// 
	//-------------------------------------------------------------------------------
public:
	GoodsAllPurposeOpt&		GetGoodsAllPurposeOpt(VOID){return m_GoodsAllPurposeOpt;}
private:
	//!						
	GoodsAllPurposeOpt		m_GoodsAllPurposeOpt;
private:
	long					m_lTotalSkillNum;
	DWORD					m_dwSkillStartTime;	
	long					m_lTotalNpcScriptsNum;
	DWORD					m_dwNpcScriptsStartTIme;
	CGUID					m_guidRelivScript;					//死亡挂起脚本GUID
	bool					m_bReliveSkill;						//技能复活标志符

public:
	//判断技能是否发送正常
	bool					JudgeSkilllegality();		
	//判断NPC脚本是否发送正常
	bool					JudgeNpcScriptslegality(void);
	//判断当前场景能否原地复活
	bool					CheckReliveByOrigin();
	//判断原地复活物品是否满足条件
	bool					CheckReliveGoods(long &lIndex, long &lRatio, long &lCostNum,char* strOriginName);			
	//设置死亡挂起脚本
	void					SetReliveScript(CGUID guid){ m_guidRelivScript = guid; }
	//释放挂起的死亡复活脚本
	void					FreeReliveScript();
	//设置技能复活标志符
	void					SetReliveSkill(bool bSkill)	   {m_bReliveSkill = bSkill;}
	//获取技能复活标志符
	bool					GetReliveSkill()			   {return m_bReliveSkill ;}
	//美容处理,成功返回true,否则返回false
	long					ChangeFaceHair(tagCosKey tgKey,long lHairColor);
	//美容失败，给客户端返回失败消息(bType为1表示材料不足，2表示已有款型)
	//void					FailedChangeFaceHair(BYTE bType);

private:
	GoodsTimerHandler m_GoodsTimerHandler;

	//////////////////////////////////////////////////////////////////////////
	// 奖励时间戳
	//////////////////////////////////////////////////////////////////////////
private:
	DWORD m_dwBounsTime;

public:
	void	SetBounsTime(DWORD dwBountTime)		{ m_dwBounsTime = dwBountTime; }
	DWORD	GetBounsTime()						{ return m_dwBounsTime; }
private:
	DWORD m_dwReportTime;
	long  m_lCostYuanBao;
public:
	void SetReportTime(DWORD dwTime)			{m_dwReportTime=dwTime;}
	DWORD GetReportTime()						{return m_dwReportTime;}

//消费星钻
	void OnCostYuanBao(long lNum);
	//设置消耗元宝数量
	void SetCostYuanBao(long lNum)				{m_lCostYuanBao = lNum;}
	long GetCostYuanBao()						{return m_lCostYuanBao;}

public:

	struct tagCountTimer
	{
		tagCountTimer()
			: m_bCountType(false)
			, m_dwCountTime(0)
			, m_dwID(-1)
			, m_dwRetTime(0)
			, m_dwTimerType(-1)
			, m_dwStartRetTime(0)
		{

		}
		~tagCountTimer()
		{

		}

		void operator=(const tagCountTimer& r)
		{
			m_bCountType = r.m_bCountType;
			m_dwCountTime = r.m_dwCountTime;
			m_dwID = r.m_dwID;
			m_dwRetTime = r.m_dwRetTime;
			m_dwTimerType = r.m_dwTimerType;
			m_dwStartRetTime = r.m_dwStartRetTime;
		}

		bool m_bCountType; //计时类型，0：顺序 1：倒序
		DWORD m_dwCountTime; // 计时开始点， 单位毫秒
		DWORD m_dwID; // 定时器ID
		DWORD m_dwRetTime; // 计时剩余值,单位毫秒
		DWORD m_dwTimerType; // 定时器类型，从配置获取其属性
		DWORD m_dwStartRetTime; // 剩余计时的最大值
	};

	void AddCountTimer(tagCountTimer& timer);
	tagCountTimer& GetCountTimer(DWORD id);
	void DelCountTimer(DWORD id);
	DWORD GetCountTimerNum(void);
	void ClearCountTimer(void);
	DWORD GetCurCountTimerID(void);
	void SetCurCountTimerID(DWORD id);
	
private:
	// 计时器功能
	map<DWORD, tagCountTimer> m_mapCountTimer;
	static tagCountTimer NULL_TIMER;
	DWORD m_dwCountTimerID;

public:
	//!                         向服务器添加一个物品（该接口不向客户端发送AddGoods消息）
	BOOL                        AddGoodsToServer(CGoods* pGoods);
	//!                         向服务器添加物品(参数1：物品编号；参数2:数量)
	bool                        AddGoodsForServer(DWORD dwGoodsIndex,int n);        
	//!                         向客户端添加一个物品 （该接口与AddGoodsToServer接口对应）
	BOOL                        AddGoodsToClient();
private:
	bool    m_bAddGoodsToServer ;     //向服务器添加物品是否成功
	CS2CContainerObjectMove m_comMsg; //用于在服务器添加物品时记录
	CGUID  m_GoodsToServerGUID;        //记录添加到服务器的物品的GUID
	DWORD m_AddGoodsContainerId;
	DWORD m_AddGoodsPos;
	int    m_iAddGoodsType;           //添加物品的方式[新加物品或改变物品的数量]
private:
	bool m_bSortContainer;		 //整理背包标志符
public:
	bool GetSortSontainr()	{return m_bSortContainer;}
	void SetSortContainer(bool b){m_bSortContainer = b;}
//////////////////////////彩票系统相关////////////////////////////////////////////////////////////
private:
	CLottery m_Lottery;               //设置抽奖机
	vector<tagLottery> m_VecLottery;
	DWORD m_dwLotteryRe;              //一次抽奖的奖品号
	DWORD m_dwLastLotteryTime;	      //上次抽奖时间
	BYTE m_btFileId;					//中奖物品栏位编号

	long m_lPersonalCredit;       //个人积分
	long m_lPersonalCreditTotal;  //个人累计积分

	int m_iPersonalCreditVelue;  //个人积分参数
	int m_iWorldCreditVelue;     //世界积分变量
	int m_iLotteryLevel;         //抽奖档次
	bool m_bSendWorldInfo;       //是否发送世界信息
	

public:
	bool DoLottery();                 //开始抽奖
	bool RefreshVecLottery(bool bCost=true);         //刷新奖品栏位
	bool RefreshVecConstel(bool bCost=true);         //刷新星座栏位
	void OpenLotteryPage();           //打开抽奖界面
	void SendVecLottery();            //发送栏位信息
	void SendVecConstel();            //发送星座信息
	void SendLotteryGoods();		  //发送物品以及强制重置消息.
	//void UpdateLotteryInf();		  //更新玩家相关的信息
	void SystemReset();
	void SetPersonalCredit(long l){m_lPersonalCredit=l;};   //设置个人积分
	long GetPersonalCredit(){return m_lPersonalCredit;};    //获得个人积分
	DWORD GetLastLotteryTime(){return m_dwLastLotteryTime;};
	void SetPersonalCreditTotal(long l){m_lPersonalCreditTotal=l;};  //设置个人累计积分
	long GetPersonalCreditTotal(){return m_lPersonalCreditTotal;};   //获得个人累计积分
	void SendPersonalCreditInfo();
	void SetPersonalCreditVelue(int i){m_iPersonalCreditVelue=i;};  //设置玩家的积分变量
	int GetPersonalCreditVelue(){return m_iPersonalCreditVelue;};
	void SetWorldCreditVelue(long l){ m_iWorldCreditVelue = l;};			//设置世界积分变量
	int GetWorldCreditVelue(){return m_iWorldCreditVelue;};		//获得世界积分变量
	void SetLotteryLevel(int i){m_iLotteryLevel = i;};
	int  GetLotteryLevel(){return m_iLotteryLevel;};
	void ResetPersonalCredit();
	void SendFieldType();
	void SetSendWorldInfo(bool b){m_bSendWorldInfo=b;};
	bool GetIsSendWorldInfo(){return m_bSendWorldInfo;};
	DWORD GetLotteryRe(){return m_dwLotteryRe;};
};
