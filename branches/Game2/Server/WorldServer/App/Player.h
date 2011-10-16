#pragma once

#include "Business/BusinessPackage.h"
#include "Container/AmountLimitGoodsContainer.h"
#include "Container/Bank.h"
#include "Container/Depot.h"
#include "Container/EquipmentContainer.h"
#include "Container/SubpackContainer.h"
#include "Container/VolumeLimitGoodsContainer.h"
#include "Container/SpecialEquipContainer.h"
#include "Container/Wallet.h"
#include "Mail/Mail.h"
#include "MedalContainer.h"
#include "MoveShape.h"
#include "OrganizingSystem/OrganizingCtrl.h"
#include "PlayerDepot.h"
#include "Session/WorldServerSession.h"
#include "SpriteSystem.h"
#include "../../../Public/Common/QuestRecord.h"
#include "../../../Public/Common/PlayerSetupOnServer.h"
#include "../../../Public/Common/PlayerDef.h"
#include "../../../Public/ServerPublic/Server/Common/DeputyOccuRecord.h"
#include "../../../Public/ServerPublic/Server/BaseDef/PhaseSeverDef.h"
#include "../../../Public/Setup/IncrementShopList.h"

class VariableList;
class CGoods;
class CPet;

typedef map<CGUID, CPet*> PETS;
typedef const PETS& C_PETS;
typedef PETS::iterator ITR_PET;
typedef PETS::const_iterator C_ITR_PET;

class CPlayer :
	public CMoveShape
{
public:
	CPlayer(void);

	virtual ~CPlayer(void);

public:
	// 人物技能
	struct tagSkill
	{
		DWORD	wID;					//	编号
		WORD	wLevel;					//	等级
		DWORD	wCooldown;				//  冷却时间
	};

	struct tagShopSelledGoods 
	{
		DWORD dwSelledPrice;
		CGoods* pGoods;
	};

	// 反外挂相关数据中需要存到数据库中的数据 BAIYUN@20090602
	struct dbAntiCheatData
	{
		int   m_nActFactor;				// 活跃度
		int   m_nAccWrongTimes;			// 累计答错次数
		int   m_nCurWrongTimes;			// 当前答错次数
		int   m_nCurRightTimes;			// 连续答对次数
		int   m_nLastGameTime;		    // 上次游戏时间
		unsigned char m_bBanFlag;		// 封号标志
		unsigned char m_bScriptExecuted; // 为1时表示之前的错误脚本未触发
		int m_nBonusTimes;				// 当前奖励次数
		unsigned char m_bRHAnswered;	/// 是否在黄金时间答题正确过
	};

	// 反外挂相关数据
	struct AntiCheatData 
	{
		AntiCheatData()
		{
			memset(this, 0, sizeof(*this));
		}
		dbAntiCheatData m_dbData;
		int	            m_nAntiCheatPK;		    // PK值 注:与PKValue不同	
		DWORD           m_dwLastAnswerTime;     // 上次答题时间
		long			m_nRefuseTimes;			// 拒绝回答题目次数
		long			m_nAtkAct;				// 战斗活跃度
	};
	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
public:
	tagPlayerProperty& GetPlayerProperty(void) { return m_Property; }


	list<tagSkill>& GetSkillList()				{return m_listSkillID;}

    map<CGUID, tagBrotherInfo>& GetBrotherMap() { return m_mapBrotherInfo; }

	void SetSaveDepotFlag(bool bFlag){m_bSaveDepot=bFlag;}
	bool IsSaveDepot(){return m_bSaveDepot;}

	CSubpackContainer* GetSubpackContainer(void) { return m_pSubpackContainer; }

private:

	//临时保存玩家卖到NPC商店的物品，用于物品回购功能
	list<tagShopSelledGoods> m_listNpcShopSellGoods;

	map<DWORD,DWORD> m_mapCredit;
	map<DWORD,DWORD> m_mapInvestRecord;
	long m_lInvestLimitTimeMark;
	CVolumeLimitGoodsContainer* m_cOriginPack;
	CSubpackContainer* m_pSubpackContainer;
	VOID gcPacks();


public:
	VOID SetCredit(DWORD dwAreaId,DWORD dwCredit);
	VOID AddCredit(DWORD dwAreaId,long lVal);
	DWORD GetCredit(DWORD dwAreaId);
	VOID ClearCredit(){m_mapCredit.clear();}
	CVolumeLimitGoodsContainer* getOriginPack(){return m_cOriginPack;}
	CSubpackContainer* getSubpackContainer(){return m_pSubpackContainer;}

	BOOL UnserializeSubDepot(BYTE* pStream, LONG& lOffset);
	BOOL UnserializeSubDepot(DBReadSet& setReadDB);

	VOID setSubPackGoods(BYTE pos,CGoods** pGoods);

	BOOL SerializeSubpack(vector<BYTE>* pStream,PLAYER_EXTEND_ID extendId);
	BOOL UnserializeSubpack(BYTE* pStream, LONG& lOffset);
	BOOL UnserializeSubpack(DBReadSet& setReadDB);

	BOOL SerializeSubDepot(vector<BYTE>* pStream, LONG eContainerID);

	hash_map<CGUID,CGoods*,hash_guid_compare>* GetOriginPackGoodsMap(){return m_cOriginPack->GetGoodsMap();}
	hash_map<CGUID,CGoods*,hash_guid_compare>* GetSubPackGoodsMap(int i);
	map<eEquipType,CGoods*>* GetEquipmentGoodsMap(){return m_cEquipment.GetGoodsMap();}
    hash_map<CGUID,CGoods*, hash_guid_compare>* GetEquipGodGoodsMap() { return m_cEquipmentGod.GetGoodsMap(); }
    hash_map<CGUID,CGoods*, hash_guid_compare>* GetEquipLandGoodsMap()  { return m_cEquipmentLand.GetGoodsMap(); }
    hash_map<CGUID,CGoods*, hash_guid_compare>* GetEquipHumanGoodsMap() { return m_cEquipmentHuman.GetGoodsMap(); }

	map<DWORD,DWORD>* GetCreditMap(){return &m_mapCredit;}

	VOID PushbakNpcShopSellGoods(CGoods* pGoods,DWORD dwPrice);
	CGoods* FetchNpcShopSellGoods(const CGUID& guid);
	CGoods* FindNpcShopSellGoods(CGUID& guid,DWORD* dwPrice);
	VOID ReleaseNpcShopSellGoods();


private:
	long		m_lTicket;
	map<DWORD,DWORD>  m_mapGoodsIdCoolDown;
	map<DWORD,DWORD>  m_mapClassIdCoolDown;
public:
	long GetTicket()				{ return m_lTicket; }
	void SetTicket(long	lTicket)	{ m_lTicket = lTicket; }

	map<DWORD,DWORD>& GetClassIdCooldownMap(){return m_mapClassIdCoolDown;}
	map<DWORD,DWORD>& GetGoodsIdCooldownMap(){return m_mapGoodsIdCoolDown;}

public:

	CEquipmentContainer			m_cEquipment;
    CSpecialEquipContainer      m_cEquipmentGod;
    CSpecialEquipContainer      m_cEquipmentLand;
    CSpecialEquipContainer      m_cEquipmentHuman;
	CWallet						m_cWallet;
	CWallet				        m_cSilverWallet;
	CWallet				        m_cGem;
		

	//!				仓库
	PlayerDepot		m_PlayerDepot;
private:
	//changed by chenaly .....07.4.5.................
	//tagBaseProperty m_BaseProperty;		//	基本属性 
	//tagProperty		m_Property;			//	实际属性
	tagPPro   m_Property;                   //  基本属性
	tagPFPro  m_BaseFightProperty;          //  战斗基本属性
	tagPFPro  m_FactFightProperty;           // 战斗实际属性
	tagPlayerAppendProperty  m_AppendProperty;//保存附加属性
	tagPlayerElementProperty	m_arrElementProperty[MT_Max];//元素属性
	// end change......................................

    bool m_bSaveDepot;

	list<tagSkill>	m_listSkillID;		//	技能列表
    map<CGUID, tagBrotherInfo>    m_mapBrotherInfo;      //结义金兰

    ///@by:expter 存储玩家天赋列表
private:
    std::map<long,long>         m_GeniusList;//天赋配置，iD对应等级

public:
    /// 增加天赋配置
    void  AddGenius(long Geniusid,long Lvl)     {  m_GeniusList[Geniusid]=Lvl;}
    std::map<long,long>&  GetGeniusList()       {  return m_GeniusList;       }

public:

	//changed by chenaly.....07.4.5..............
	//基本属性 接口  m_Property
	eOccupation	GetOccupation()				{return m_Property.byOccu;}			//职业
	void	SetOccupation(BYTE l)	{m_Property.byOccu=(eOccupation)l;}
	//副职业
	BYTE	GetDOccupation()				{return m_Property.byAssOccu;}
	void	SetDOccupation(eDeputyOccu l)	{m_Property.byAssOccu=l;}


	BYTE	GetSex()					{return m_Property.bySex;}				//性别
	void	SetSex(BYTE l)				{m_Property.bySex=l;}
	BYTE	GetHeadPic()				{return m_Property.byHairPic;}			// 头部图片
	void	SetHeadPic(BYTE l)			{m_Property.byHairPic=l;} 
	BYTE	GetFacePic()				{return m_Property.byFacePic;}			//面部图片
	void	SetFacePic(BYTE l)			{m_Property.byFacePic=l;}
	DWORD	GetHP()						{return m_Property.dwHp;}				//当前hp
	void	SetHP(DWORD l)				{m_Property.dwHp=l;}
	WORD	GetMP()						{return m_Property.wMp;}				//当前mp
	void	SetMP(WORD l)				{m_Property.wMp=l;}
	WORD    GetEnergy()                 {return m_Property.wEnergy;}			//当前能量
	void    SetEnergy(WORD l)           {m_Property.wEnergy=l;}
	WORD    GetStamina()                {return m_Property.wStamina;}			//当前体力
	void    SetStamina(WORD l)          {m_Property.wStamina=l;}
	WORD	GetLevel()					{return m_Property.wLevel;}				//等级
	void	SetLevel(WORD l)			{m_Property.wLevel =l;}
	DWORD	GetExp()					{return m_Property.dwExp;}			    //经验值
	void	SetExp(DWORD l)				{m_Property.dwExp=l;}
	DWORD	GetExpMultiple()			{return m_Property.dwExpMultiple;}		//经验倍数
	void	SetExpMultiple(DWORD l)		{m_Property.dwExpMultiple=l;}
	DWORD	GetPresentExp()				{return m_Property.dwPresentExp;}		//赠送经验值
	void	SetPresentExp(DWORD l)		{m_Property.dwPresentExp=l;}
	DWORD   GetUpgradeExp()             { return m_Property.dwUpgradeExp ;}
	void    SetUpGradeExp(DWORD l)      { m_Property.dwUpgradeExp=l;}
    ushort  GetTaoismLvl()              { return m_Property.wTaoismLvl;}
    void    SetTaoismLvl(ushort l)      { m_Property.wTaoismLvl = l; }
    ulong   GetTaoismExp()              { return m_Property.dwTaoismExp; }
    void    SetTaoismExp(ulong l)       { m_Property.dwTaoismExp = l; }
    ulong   GetRelationVal()            { return m_Property.dwRelationVal; }
    void    SetRelationVal(ulong l)     { m_Property.dwRelationVal = l; }
    ulong   GetHiddenVirtue()           { return m_Property.dwHiddenVirtue; }
    void    SetHiddenVirtur(ulong l)    { m_Property.dwHiddenVirtue = l; }
    ushort  GetVigour()                 { return m_Property.wVigour; }
    void    SetVigour(ushort l)         { m_Property.wVigour = l; }
    ushort  GetThew()                   { return m_Property.wThew; }
    void    SetThew(ushort l)           { m_Property.wThew = l; }
    short   GetPoint2Str()              { return m_Property.wPoint2Str; }
    void    SetPoint2Str(short s)       { m_Property.wPoint2Str = s; }
    short   GetPoint2Dex()              { return m_Property.wPoint2Dex; }
    void    SetPoint2Dex(short s)       { m_Property.wPoint2Dex = s; }
    short   GetPoint2Con()              { return m_Property.wPoint2Con; }
    void    SetPoint2Con(short s)       { m_Property.wPoint2Con = s; }
    short   GetPoint2Intellect()              { return m_Property.wPoint2Intellect; }
    void    SetPoint2Intellect(short s)       { m_Property.wPoint2Intellect = s; }
    short   GetPoint2Spiritualism()              { return m_Property.wPoint2Spiritualism; }
    void    SetPoint2Spiritualism(short s)       { m_Property.wPoint2Spiritualism = s; }

	BYTE	GetCountry()				{ return m_Property.byCountry; }		  //国家
	void	SetCountry(BYTE l)			{ m_Property.byCountry = l; }
	long	GetCountryContribute()		{ return m_Property.lCountryContribute; } //国家贡献
	void	SetCountryContribute(long l){ m_Property.lCountryContribute  = l; }   
    BYTE*   GetCountryCredit()          { return (BYTE*)&m_Property.dwCountryCredit[0]; }
    void    SetCountryCredit(BYTE* pBuf, long lSize)    { memcpy(&m_Property.dwCountryCredit[0], pBuf, lSize); }
    BYTE    GetCountryStatus()          { return m_Property.byCountryStatus; }  //阵营地位
    void    SetCountryStatus(BYTE l)    { m_Property.byCountryStatus = l; }
	DWORD	GetSpouseId()				{return m_Property.dwSpouseID;}			//配偶身份
	void	SetSpouseId(DWORD l)		{m_Property.dwSpouseID=l;}
	DWORD   GetSpouseParam()            { return m_Property.dwSpouseParam;}		//配偶关系值 
	void    SetSpouseParam(DWORD l)     { m_Property.dwSpouseParam=l;}
	BYTE    GetBusinessLevel()          { return m_Property.byBusinessLevel;}	//商业等级  
	void    SetBusinessLevel(BYTE l)    { m_Property.byBusinessLevel=l;}
	DWORD   GetBusinessExp()            { return m_Property.dwBusinessExp;}		//商业经验  
	void    SetBusinessExp(DWORD l)     { m_Property.dwBusinessExp=l;} 
	DWORD   GetPkCount()                { return m_Property.dwPkCount;}			//pk杀本国玩家数 
	void    SetPkCount(DWORD l)         { m_Property.dwPkCount=l;}
	DWORD   GetPkValue()                { return m_Property.dwPkValue;}			//犯罪值 
	void    SetPkValue( DWORD l)        { m_Property.dwPkValue=l;}
	DWORD   GetPVPCount()               { return m_Property.dwPVPCount;	}		//玩家PVP杀外国人数量 
	void    SetPVPCount(DWORD l)		{ m_Property.dwPVPCount=l;}
	DWORD   GetPVPValue()               {return m_Property.dwPVPValue;}			//玩家PVP值(恶名值)
	void    SetPVPValue(DWORD l)        { m_Property.dwPVPValue=l;}
	DWORD	GetHotKey(long lPos)		{return m_Property.dwHotKey[lPos];}		//热键
	DWORD*	GetHotKeyArray(void)		{return &m_Property.dwHotKey[0];}		//热键
	void	SetHotKey(long lPos,DWORD l){m_Property.dwHotKey[lPos]=l;}
	BYTE    GetPKOnOff()                {return m_Property.byPKOnOff;}			//pk状态 
	void    SetPKOnOff(BYTE L)          {m_Property.byPKOnOff=L;}
	bool	GetShowCountry(void)		{return m_Property.bShowCountry;}
	void	SetShowCountry(bool bShow)	{m_Property.bShowCountry = bShow;}
	void	SetOccuLvl(BYTE* pBuf,long lSize) {memcpy(&m_Property.byOccuLvl[0],pBuf,lSize);}
	BYTE*	GetOccuLvl()				{return &m_Property.byOccuLvl[0];}
	void	SetOccuExp(BYTE* pBuf,long lSize) {memcpy(&m_Property.dwOccuExp[0],pBuf,lSize);}
	BYTE*	GetOccuExp()				{return (BYTE*)&m_Property.dwOccuExp[0];}
	void	SetOccuSP(BYTE* pBuf,long lSize) {memcpy(&m_Property.dwOccuSP[0],pBuf,lSize);}
	BYTE*	GetOccuSP()					{return (BYTE*)&m_Property.dwOccuSP[0];}

	long GetCurPKCP() const { return m_Property.lCurPKCP; }
	void SetCurPKCP( long v ) 	{ m_Property.lCurPKCP = v; }

	//pk具体状态的设定与获得接口
	bool	IsPk_Normal()				{return true;}							//PK开关-普通
	void	SetPk_Normal(bool b)		{return ;}
	bool	IsPk_Team()					{return true;}							//PK开关-队伍
	void	SetPk_Team(bool b)			{return ;}
	bool	IsPk_Union()				{return true;}							//PK开关-行会
	void	SetPk_Union(bool b)			{return ;}
	bool	IsPk_Badman()				{return true;}							//PK开关-罪犯
	void	SetPk_Badman(bool b)		{return ;}
	bool	IsPk_Country()				{return true;}							//pk他国玩家
	void	SetPk_Country(bool b)		{return ;}

	DWORD	GetLastExitGameTime()		{return m_Property.dwLastExitGameTime;}
	void	SetLastExitGameTime(DWORD l){m_Property.dwLastExitGameTime=l;}

	/////////基本战斗属性 接口 m_BaseFightProperty
	DWORD	GetBaseMaxHP()					{return m_BaseFightProperty.dwMaxHp;}		//生命
	void	SetBaseMaxHP(DWORD l)			{m_BaseFightProperty.dwMaxHp=l;}
	WORD	GetBaseMaxMP()					{return m_BaseFightProperty.wMaxMp;}		//法力
	void	SetBaseMaxMP(WORD l)			{m_BaseFightProperty.wMaxMp=l;}
	WORD    GetBaseMaxStamina()             {return m_BaseFightProperty.wMaxStamina;}	//体力
	void    SetBaseMaxStamina(WORD l)       {m_BaseFightProperty.wMaxStamina=l;}
	WORD	GetBaseMaxEnergy()				{return m_BaseFightProperty.wMaxEnergy;}	//能量
	void	SetBaseMaxEnergy(WORD l)		{m_BaseFightProperty.wMaxEnergy = l;}
	WORD	GetBaseHpRecoverSpeed()			{return m_BaseFightProperty.wHpRecoverSpeed;}//回hp
	void	SetBaseHpRecoverSpeed(WORD l)	{m_BaseFightProperty.wHpRecoverSpeed=l;}
	WORD    GetBaseFightHpRecoverSpeed()          {return m_BaseFightProperty.wFightHpRecoverSpeed ;} //战斗回hp
	void    SetBaseFightHpRecoverSpeed(WORD l)    {m_BaseFightProperty.wFightHpRecoverSpeed=l;}
	WORD	GetBaseMpRecoverSpeed()			{return m_BaseFightProperty.wMpRecoverSpeed;}//回mp
	void	SetBaseMpRecoverSpeed(WORD l)	{m_BaseFightProperty.wMpRecoverSpeed=l;}   
	WORD    GetBaseFightMpRecoverSpeed()          {return m_BaseFightProperty.wFightMpRecoverSpeed;} //战斗回mp
	void    SetBaseFightMpRecoverSpeed(WORD l)    {m_BaseFightProperty.wFightMpRecoverSpeed=l;}
	short   GetBaseFightEnerRecoverSpeed()        {return m_BaseFightProperty.wFightEnerRecoverSpeed;}	//战斗能量恢复速度 
	void    SetBaseFightEnerRecoverSpeed(short l) {m_BaseFightProperty.wFightEnerRecoverSpeed=l;}
	WORD    GetBaseStamRecoverSpeed()       {return m_BaseFightProperty.wStamRecoverSpeed;}//回体力 
	void    SetBaseStamRecoverSpeed(WORD l) {return;}									 //.....
    short   GetBaseFightStamRecoverSpeed()  { return m_BaseFightProperty.wFightStamRecoverSpeed; }
    void    SetBaseFightStamRecoverSpeed(short l)   { m_BaseFightProperty.wFightStamRecoverSpeed = l; }
    
    short   GetBaseManly()                  { return m_BaseFightProperty.wManly; }
    void    SetBaseManly(short l)           { m_BaseFightProperty.wManly = l; }
    short   GetBasePerverseTrend()          { return m_BaseFightProperty.wPerverseTrend; }
    void    SetBasePerverseTrend(short l)   { m_BaseFightProperty.wPerverseTrend = l; }
    short   GetBaseComplaint()              { return m_BaseFightProperty.wComplaint; }
    void    SetBaseComplaint(short l)       { m_BaseFightProperty.wComplaint = l; }
    short   GetBaseMurderousLook()          { return m_BaseFightProperty.wMurderousLook; }
    void    SetBaseMurderousLook(short l)   { m_BaseFightProperty.wMurderousLook; }

    short   GetBaseStr()                    { return m_BaseFightProperty.wStr; }
    void    SetBaseStr(short s)             { m_BaseFightProperty.wStr = s; }
    short   GetBaseDex()                    { return m_BaseFightProperty.wDex; }
    void    SetBaseDex(short s)             { m_BaseFightProperty.wDex = s; }
    short   GetBaseCon()                    { return m_BaseFightProperty.wCon; }
    void    SetBaseCon(short s)             { m_BaseFightProperty.wCon = s; }
    short   GetBaseIntellect()              { return m_BaseFightProperty.wIntellect; }
    void    SetBaseIntellect(short s)       { m_BaseFightProperty.wIntellect = s; }
    short   GetBaseSpiritualism()           { return m_BaseFightProperty.wSpiritualism; }
    void    SetBaseSpiritualism(short s)    { m_BaseFightProperty.wSpiritualism = s; }
    short   GetBasePoint()                  { return m_BaseFightProperty.wPoint; }
    void    SetBasePoint(short s)           { m_BaseFightProperty.wPoint = s; }
    short   GetBaseParry()                  { return m_BaseFightProperty.wParry; }		 //招架
    void    SetBaseParry(short l)           { m_BaseFightProperty.wParry=l; }
	
	WORD    GetBaseBlock()                  {return m_BaseFightProperty.wFightBlock ;}	 //格挡
	void    SetBaseBlock(WORD l)            {m_BaseFightProperty.wFightBlock =l;}
	
	WORD    GetBaseDodge()                  {return m_BaseFightProperty.wDodge;}		 //闪避
	void    SetBaseDodge(WORD l)            {m_BaseFightProperty.wDodge=l;}
	//	WORD    GetBaseShot()                   {return m_BaseFightProperty.wShot;}			 //射程
	//	void    SetBaseShot(WORD l)             {m_BaseFightProperty.wShot=l;}
	WORD    GetBaseCharm()                  {return m_BaseFightProperty.wCharm;}		 //魅力
	void    SetBaseCharm(WORD l)            {m_BaseFightProperty.wCharm=l;}
	WORD    GetBaseHit()                    {return m_BaseFightProperty.wHit;}			 //命中
	void    SetBaseHit(WORD l)              {m_BaseFightProperty.wHit=l;}
	WORD    GetBaseAbsorb()                 {return m_BaseFightProperty.wAbsorb;}		 //专注
	void    SetBaseAbsorb(WORD l)           {m_BaseFightProperty.wAbsorb=l;}
	WORD    GetBaseDef()                    {return m_BaseFightProperty.wDef;}			 //物理防御
	void    SetBaseDef(WORD l)              {m_BaseFightProperty.wDef=l;}
	WORD    GetBaseMdef()                   {return m_BaseFightProperty.wMdef;}			 //魔法抗性
	void    SetBaseMdef(WORD l)             {m_BaseFightProperty.wMdef=l;}
	WORD    GetBaseCri()                    {return m_BaseFightProperty.wCri;}			 //物理爆击
	void    SetBaseCri(WORD l)              {m_BaseFightProperty.wCri=l;}
	WORD    GetBaseMAtkSpeed()              {return m_BaseFightProperty.wMAtkSpeed;}	 //施法速度
	void    SetBaseMAtkSpeed(WORD l)        {m_BaseFightProperty.wMAtkSpeed=l;}
	WORD    GetBaseBuffPower()              {return m_BaseFightProperty.wBuffPower;}	 //buffer攻击强度
	void    SetBaseBuffPower(WORD l)        {m_BaseFightProperty.wBuffPower=l;}
	WORD    GetBaseMinAtk()                 {return m_BaseFightProperty.wMinAtk;}		 //最小攻击力
	void    SetBaseMinAtk(WORD l)           {m_BaseFightProperty.wMinAtk=l;}
	WORD    GetBaseMaxAtk()                 {return m_BaseFightProperty.wMaxAtk;}		 //最大攻击力
	void    SetBaseMaxAtk(WORD l)           {m_BaseFightProperty.wMaxAtk=l;}
	WORD    GetBaseMAtk()                   {return (WORD)m_BaseFightProperty.wMAtk;}			 //魔法攻击力
	void    SetBaseMAtk(WORD l)             {m_BaseFightProperty.wMAtk=l;}
	WORD    GetBaseMCri()                   {return (WORD)m_BaseFightProperty.wMCri;}			 //魔法爆击率
	void    SetBaseMCri(WORD l)             {m_BaseFightProperty.wMCri=l;}
	WORD    GetBaseBloodSuk()               {return m_BaseFightProperty.wBloodSuck;}		 //生命吸收率
	void    SetBaseBloodSuk(WORD l)         {m_BaseFightProperty.wBloodSuck=l;}
	WORD    GetBaseManaSuck()               {return m_BaseFightProperty.wManaSuck;}		 //法力吸收率
	void    SetBaseManaSuck(WORD l)         {m_BaseFightProperty.wManaSuck=l;}


	WORD    GetBaseCriDef()                     {return m_BaseFightProperty.wCriDef;}//物理爆击抗性
	void    SetBaseCriDef(WORD l)               {m_BaseFightProperty.wCriDef=l;}
	WORD    GetBaseMCriDef()                    {return m_BaseFightProperty.wMCriDef;}//魔法爆击抗性
	void    SetBaseMCriDef(WORD l)              {m_BaseFightProperty.wMCriDef=l;}
	WORD    GetBaseWillDef()                {return m_BaseFightProperty.wWillDef;}		 //意志抗性
	void    SetBaseWillDef(WORD l)          {m_BaseFightProperty.wWillDef=l;}
	WORD    GetBaseConstiDef()              {return m_BaseFightProperty.wConstiDef;}	 //体质抗性
	void    SetBaseConstiDef(WORD l)        {m_BaseFightProperty.wConstiDef=l;}

	//buffer 抗性
	WORD    GetBaseDeBuffDef(long lPos)              {return m_BaseFightProperty.wDeBuffDef[lPos];}
	void    SetBaseDeBuffDef(long lPos ,WORD l)      {m_BaseFightProperty.wDeBuffDef[lPos]=l;}
	//各个元素的抗性
	WORD    GetBaseElemDef(long lPos)                {return m_BaseFightProperty.wElemDef[lPos] ;}
	void    SetBaseElemDef(long lPos ,WORD l)        {m_BaseFightProperty.wElemDef[lPos]=l;}
	//各个种族的伤害
	WORD    GetBaseClassDam(long lPos )              {return m_BaseFightProperty.wClassDam[lPos];}
	void    SetBaseClassDam(long lPos ,WORD l)       {m_BaseFightProperty.wClassDam[lPos]=l;}
	//各个元素的伤害
	WORD    GetBaseElemDam(long lPos )                {return m_BaseFightProperty.wElemDam[lPos];}
	void    SetBaseElemDam(long lPos ,WORD l)         {m_BaseFightProperty.wElemDam[lPos]=l;}
	//////实际战斗属性  接口 m_FactFightProperty
	DWORD	GetMaxHP()					{return m_FactFightProperty.dwMaxHp;}		//生命
	void	SetMaxHP(DWORD l)			{m_FactFightProperty.dwMaxHp=l; 
	if(GetHP()>GetMaxHP()) SetHP(GetMaxHP());}
	WORD	GetMaxMP()					{return m_FactFightProperty.wMaxMp;}		//法力
	void	SetMaxMP(WORD l)			{m_FactFightProperty.wMaxMp=l; 
	if(GetMP()>GetMaxMP()) SetMP(GetMaxMP());}
	WORD    GetMaxStamina()             {return m_FactFightProperty.wMaxStamina;}	//体力
	void    SetMaxStamina(WORD l)       {m_FactFightProperty.wMaxStamina=l;
	if( GetStamina()>GetMaxStamina()) SetStamina(GetMaxStamina());}
	WORD	GetMaxEnergy()				{return m_FactFightProperty.wMaxEnergy;}	//能量
	void	SetMaxEnergy(WORD l)		{m_FactFightProperty.wMaxEnergy = l; 
	if(GetEnergy()>GetMaxEnergy()) m_Property.wEnergy = GetMaxEnergy();}
	WORD	GetHpRecoverSpeed()			{return m_FactFightProperty.wHpRecoverSpeed;}//回hp
	void	SetHpRecoverSpeed(WORD l)	{m_FactFightProperty.wHpRecoverSpeed=l;}
	WORD    GetFightHpRecoverSpeed()          {return m_FactFightProperty.wFightHpRecoverSpeed ;} //战斗回hp
	void    SetFightHpRecoverSpeed(WORD l)    {m_FactFightProperty.wFightHpRecoverSpeed=l;}
	WORD	GetMpRecoverSpeed()			{return m_FactFightProperty.wMpRecoverSpeed;}//回mp
	void	SetMpRecoverSpeed(WORD l)	{m_FactFightProperty.wMpRecoverSpeed=l;}
	WORD    GetFightMpRecoverSpeed()          {return m_FactFightProperty.wFightMpRecoverSpeed;} //战斗回mp
	void    SetFightMpRecoverSpeed(WORD l)    {m_FactFightProperty.wFightMpRecoverSpeed=l;}
	short   GetEnerRecoverSpeed()             {return m_FactFightProperty.wEnerRecoverSpeed ;}//能量恢复速度
	short   GetFightEnerRecoverSpeed()        {return m_FactFightProperty.wFightEnerRecoverSpeed;}	//战斗能量恢复速度 
	void    SetFightEnerRecoverSpeed(short l) {m_FactFightProperty.wFightEnerRecoverSpeed=l;}
	WORD    GetStamRecoverSpeed()       {return m_FactFightProperty.wStamRecoverSpeed;}//回体力 &
	void    SetStamRecoverSpeed(WORD l) {} //.....

    short   GetStr()                    { return m_FactFightProperty.wStr; }
    void    SetStr(short s)             { m_FactFightProperty.wStr = s; }
    short   GetDex()                    { return m_FactFightProperty.wDex; }
    void    SetDex(short s)             { m_FactFightProperty.wDex = s; }
    short   GetCon()                    { return m_FactFightProperty.wCon; }
    void    SetCon(short s)             { m_FactFightProperty.wCon = s; }
    short   GetIntellect()              { return m_FactFightProperty.wIntellect; }
    void    SetIntellect(short s)       { m_FactFightProperty.wIntellect = s; }    
    short   GetSpiritualism()           { return m_FactFightProperty.wSpiritualism; }
    void    SetSpiritualism(short s)    { m_FactFightProperty.wSpiritualism = s; }
    short   GetPoint()                  { return m_FactFightProperty.wPoint; }
    void    SetPoint(short s)           { m_FactFightProperty.wPoint = s; }
	WORD    GetBlock()                  {return m_FactFightProperty.wFightBlock ;}		//格挡
	void    SetBlock(WORD l)            {m_FactFightProperty.wFightBlock =l;}
	WORD    GetParry()                  {return m_FactFightProperty.wParry;}		//招架
	void    SetParry(WORD l)            {m_FactFightProperty.wParry=l;}
	WORD    GetDodge()                 {return m_FactFightProperty.wDodge;}			//闪避
	void    SetDodge(WORD l)           {m_FactFightProperty.wDodge=l;}
	//	WORD    GetShot()                   {return m_BaseFightProperty.wShot;}			//射程
	//	void    SetShot(WORD l)             {m_BaseFightProperty.wShot=l;}
	WORD    GetCharm()                  {return m_FactFightProperty.wCharm;}		//魅力
	void    SetCharm(WORD l)            {m_FactFightProperty.wCharm=l;}
	WORD    GetHit()                    {return m_FactFightProperty.wHit;}			//命中
	void    SetHit(WORD l)              {m_FactFightProperty.wHit=l;}
	WORD    GetAbsorb()                 {return m_FactFightProperty.wAbsorb;}		//专注
	void    SetAbsorb(WORD l)           {m_FactFightProperty.wAbsorb=l;}
	WORD    GetDef()                    {return m_FactFightProperty.wDef;}			//物理防御
	void    SetDef(WORD l)              {m_FactFightProperty.wDef=l;}
	WORD    GetMdef()                   {return m_FactFightProperty.wMdef;}			//魔法抗性
	void    SetMdef(WORD l)             {m_FactFightProperty.wMdef=l;}
	WORD    GetCri()                    {return m_FactFightProperty.wCri;}			//物理爆击
	void    SetCri(WORD l)              {m_FactFightProperty.wCri=l;}
	WORD    GetMAtkSpeed()              {return m_FactFightProperty.wMAtkSpeed;}	//施法速度
	void    SetMAtkSpeed(WORD l)        {m_FactFightProperty.wMAtkSpeed=l;}
	WORD    GetBuffPower()              {return m_FactFightProperty.wBuffPower;}	//buffer攻击强度
	void    SetBuffPower(WORD l)        {m_FactFightProperty.wBuffPower=l;}
	WORD    GetMinAtk()                 {return m_FactFightProperty.wMinAtk;}		//最小攻击力
	void    SetMinAtk(WORD l)           {m_FactFightProperty.wMinAtk=l;}
	WORD    GetMaxAtk()                 {return m_FactFightProperty.wMaxAtk;}		//最大攻击力
	void    SetMaxAtk(WORD l)           {m_FactFightProperty.wMaxAtk=l;}
	WORD    GetMAtk()                   {return (WORD)m_FactFightProperty.wMAtk;}			//魔法攻击力
	void    SetMAtk(WORD l)             {m_FactFightProperty.wMAtk=l;}
	WORD    GetMCri()                   {return (WORD)m_FactFightProperty.wMCri;}			//魔法爆击率
	void    SetMCri(WORD l)             {m_FactFightProperty.wMCri=l;}
	WORD    GetBloodSuk()               {return m_FactFightProperty.wBloodSuck;}		//生命吸收率
	WORD    GetCriDef()                 {return m_FactFightProperty.wCriDef;}//物理爆击抗性
	void    SetCriDef(WORD l)           {m_FactFightProperty.wCriDef=l;}
	WORD    GetMCriDef()                {return m_FactFightProperty.wMCriDef;}//魔法爆击抗性
	void    SetMCriDef(WORD l)          {m_FactFightProperty.wMCriDef=l;}
	WORD    GetManaSuck()               {return m_FactFightProperty.wManaSuck;}		//法力吸收率
	void    SetManaSuck(WORD l)         {m_FactFightProperty.wManaSuck=l;}
	WORD    GetWillDef()                {return m_FactFightProperty.wWillDef;}		//意志抗性
	void    SetWillDef(WORD l)          {m_FactFightProperty.wWillDef=l;}
	WORD    GetConstiDef()              {return m_FactFightProperty.wConstiDef;}		//体质抗性
	void    SetConstiDef(WORD l)        {m_FactFightProperty.wConstiDef=l;}

	//buffer 抗性
	WORD    GetDeBuffDef(long lPos)              {return m_FactFightProperty.wDeBuffDef[lPos];}
	void    SetDeBuffDef(long lPos ,WORD l)      {m_FactFightProperty.wDeBuffDef[lPos]=l;}
	//各个元素的抗性
	WORD    GetElemDef(long lPos)                {return m_FactFightProperty.wElemDef[lPos] ;}
	void    SetElemDef(long lPos ,WORD l)        {m_FactFightProperty.wElemDef[lPos]=l;}
	//各个种族的伤害
	WORD    GetClassDam(long lPos )              {return m_FactFightProperty.wClassDam[lPos];}
	void    SetClassDam(long lPos ,WORD l)       {m_FactFightProperty.wClassDam[lPos]=l;}
	//各个元素的伤害
	WORD    GetElemDam(long lPos )                {return m_FactFightProperty.wElemDam[lPos];}
	void    SetElemDam(long lPos ,WORD l)         {m_FactFightProperty.wElemDam[lPos]=l;}
	/////////附加属性接口  m_AppendProperty
	bool	IsCharged()					{return m_AppendProperty.bIsCharged;}	//是否冲值
	void	SetCharged(bool b=true)		{m_AppendProperty.bIsCharged=b;}

    const   char *GetAccount()			{return m_AppendProperty.strAccount;}    //
	void	SetAccount(const char* l)	{strncpy(m_AppendProperty.strAccount ,l,ACCOUNT_SIZE);TrimRight(m_AppendProperty.strAccount);}		
	const   char *GetTitle()			{return m_AppendProperty.strTitle;}
	void	SetTitle(const char* l)		{strncpy(m_AppendProperty.strTitle ,l,TITLE_SIZE);}

	//未完
	DWORD	GetMoney();              
	void	SetMoney(DWORD l)			{return ;}

	DWORD   GetSilver();
	
	void	ResetActive(void);
	long	GetCurrBaseActive(void)				{return m_Property.lCurrBaseActive;}
	long	GetCurrExActive(void)				{return m_Property.lCurrExActive;}
	ULONG	GetActiveTime(void)					{return m_Property.lLastResetActiveTime;}
	void	SetCurrBaseActive(long lBaseActive)	{m_Property.lCurrBaseActive = lBaseActive;}
	void	SetCurrExActive(long lExActive)		{m_Property.lCurrExActive = lExActive;}
	void	SetActiveTime(ULONG uTime)			{m_Property.lLastResetActiveTime = uTime;}

	//!				得到玩家结婚步骤
	long			GetMarriageStep(void)					{return m_Property.lMarriageStep;}
	//!				设置玩家结婚步骤
	void			SetMarriageStep(long lStep)				{m_Property.lMarriageStep = lStep;}

	DWORD   GetGem(void);
	void	SetGem(DWORD l)				{return ;}
	//剩余的接口

	map<CGUID,CMail*>	m_mapMail;
	map<long,long>		m_mapSysMail;									//系统邮件接收列表
	list<CGUID>			m_lNewMail;										//新邮件列表
	list<CGUID>			m_lReceiveMail;									//已收邮件列表

public:


	
	CMail				*GetMail(CGUID &guid);				
	void				DeleteMail(list<CGUID> &lguid);
	void				EraseMail(const CGUID &guid);
	void				ClearMailMap();
	
	long	CheckGoodsInPacket(char* strGoodName);						//检查在背包中是否存在某个名字的物品
	void				AddSysMail(long lID,long lEndTime);				//
	void				AddMailList(CGUID &MailGuid);					//添加新邮件列表

	list<CGUID>			&GetNewMailList();
	void				AddReceiveMailList(CGUID MailGuid);			//添加已收邮件列表
	list<CGUID>			&GetReceiveMailList();							
	map<long,long>      &GetSysMail();									//获取邮件列表
	

	// 判国处理
	int	ChangeCountry(BYTE c);


public:
	bool  LoadData();		// 读取
	// 存储
	//bool SaveData(_ConnectionPtr cn = NULL);
	//bool SaveCdkeyData(_ConnectionPtr cn = NULL);

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 编码 添加到CByteArray
	virtual bool AddOrgSysToByteArray(vector<BYTE>* pByteArray);
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData = true);		// 添加到CByteArray
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);		// 解码
	bool CodeOrgSysToDataBlock(DBWriteSet& setWriteDB);

	void LoadDefaultProperty(BYTE lOccupation,BYTE lSex, BYTE byCountry,BYTE byConstellation);	//LOAD玩家的初始默认属性
	void InitSkill();
	void AddSkill(DWORD dwID,WORD Lv,WORD wCool);
	void UpdateProperty();		// 更新属性
	//----------------------------------------------------------------
	// 禁言时间
	//----------------------------------------------------------------
private:
	long	m_lSilienceTime;

public:
	long	GetSilenceTime()			{ return m_lSilienceTime;  }
	void	SetSilenceTime( long lTime)	{ m_lSilienceTime = lTime; }

	//----------------------------------------------------------------
	// 杀人数时间
	//----------------------------------------------------------------
private:
	DWORD	m_dwMurdererTime;

public:
	DWORD	GetMurdererTime()				{ return m_dwMurdererTime;  }
	VOID	SetMurdererTime( DWORD dwTime )	{ m_dwMurdererTime = dwTime; }

	//----------------------------------------------------------------
	// 战斗状态时间
	//----------------------------------------------------------------
private:
	LONG	m_lFightStateCount;

public:
	LONG	GetFightStateCount()			{ return m_lFightStateCount; }
	VOID	SetFightStateCount( LONG l )	{ m_lFightStateCount = 2; }
public:
	struct tagOwnedReg
	{
		long lRegionID;						//场景的ID
		WORD wType;							//场景的类型
	};
	//帮会系统
private:
	CGUID	m_FactionID;							//自己帮会ID
	CGUID	m_UnionID;								//自己帮会ID
	char	m_szFactionName[MAXSIZE_FactionName];	//! 帮会名字
	char	m_szUnionName[MAXSIZE_FactionName];		//! 联盟名字
	LONG	m_lFacDemiseWarning;					//! 帮会传位提醒倒记时
	LONG	m_lFacDisbandWarning;					//! 帮会解散提醒倒记时

public:
	const CGUID&	GetFactionID		(void)						{return m_FactionID;}
	void			SetFactionID		(const CGUID& guid);
	void			SetFactionInfo		(CFaction *pFaction);
	const CGUID&	GetUnionID			(void)						{return m_UnionID;}
	void			SetUnionID			(const CGUID& UnionID);
	void			SetUnionInfo		(CUnion *pUnion);
	const char*		GetFactionName		(void)						{return m_szFactionName; }
	const char*		GetUnionName		(void)						{return m_szUnionName; }
	void			SetFacDemiseWarning	(LONG lTime)				{m_lFacDemiseWarning = lTime;}
	void			SetFacDisbandWarning(LONG lTime)				{m_lFacDisbandWarning = lTime;}

	long	GetNetExID()
	{
		static long lID = 0;
		return ++lID;
	}

	//任务系统
	/*
	public:
	struct tagPlayerQuest
	{
	WORD	wQuestID;				//任务的ID
	BYTE	byComplete;				//是否完成
	};
	private:
	map<WORD,tagPlayerQuest>	m_PlayerQuests;	//玩家已获得的任务列表
	bool	m_bLogin;					//表示玩家是否已登陆
	*/
private:
	CQuestQueue m_RoleQueue;
	//俑兵任务
	CMercenaryQuestSys	m_MerQuestSys;
	bool		m_bLogin;
public:
	BOOL AddQuestDataToByteArray(vector<BYTE>* pByteArray);		//添加任务数据到ByteArray
	BOOL DecordQuestDataFromByteArray(BYTE* pByte, long& pos);

	BOOL CodeQuestDataToDataBlock(DBWriteSet& setWriteDB);
	BOOL DecodeQuestDataFromDataBlock(DBReadSet& setReadDB);

	//得到一个任务的状态(0:存在但没完成,1:存在且完成,2:没有  [未使用 也为实现函数])
	long GetQuestState(WORD wQuestID);							
	void AddFinishedQuestFromDB(WORD wQuestID);				//从数据库添加一个任务记录
	BOOL AddQuestFromDB(CQuestRecord* rolerecord);
	//map<WORD,tagPlayerQuest>& GetPlayerQuests()	{return m_PlayerQuests;}
	const list<CQuestRecord*>* GetPlayerQuests()	{return m_RoleQueue.GetQuestRecordList();}

	bool AddMerQuestDataByteArray(vector<BYTE>* pByteArray);	//添加俑兵任务数据到ByteArray
	bool DecordMerQuestDataFromByteArray(BYTE* pByte, long& pos);//解俑兵任务数据

	bool CodeMerQuestDataToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeMerQuestDataFromDataBlock(DBReadSet& setReadDB);

	bool	GetPlayerLogin()		{return m_bLogin;}
	void	SetPlayerLogin(bool b)	{m_bLogin = b;}

	bool	GetFirstLogin()		{return m_bFirstLogin;}
	void	SetFirstLogin(bool b)	{m_bFirstLogin = b;}
	//任务系统end

	//副职业系统

	//头盔 披风 外套
	BYTE	GetShowFashion()		{return m_AppendProperty.bShowFashion;}
	void	SetShowFashion(BYTE b)	{m_AppendProperty.bShowFashion = b;}

private:
	CDeputyOccuRecord m_DOccuRecord;
//副职业系统end
public:
	BOOL AddDOccuDataToByteArray(vector<BYTE>* pByteArray){return m_DOccuRecord.AddDeputyOccuDataByteArray(pByteArray);}
	BOOL DecordDOccuDataFromByteArray(BYTE* pByte, long& pos){return m_DOccuRecord.DecordDeputyOccuDataFromByteArray(pByte,pos);}
	BOOL DecodeDOccuFromDataBlock(DBReadSet& setReadDB){return m_DOccuRecord.DecodeFromDataBlock(setReadDB);}

protected:
	friend void OnLogMessage(CMessage* pMsg);

	//////////////////////////////////////////////////////////////////////////
	// 组队
	//////////////////////////////////////////////////////////////////////////
private:
	CGUID m_TeamGuid;

public:
	const CGUID& GetTeamID() { return m_TeamGuid; }
	void SetTeamID(const CGUID& guid) { m_TeamGuid = guid;	}

    ///Expter 相位的功能模块
private:
    CGUID           m_PhaseId;
    ePhaseRest      m_PhaseRest;

public:
    const CGUID& GetPhaseID()          { return m_PhaseId; }
    void  SetPhaeID(const CGUID& guid) { m_PhaseId = guid; }
    ePhaseRest&  GetPhaseRest()        { return m_PhaseRest;}
    void  SetPhaseRest(ePhaseRest e )  { m_PhaseRest = e ; }

	// 玩家更名
public:
	int ChangeName(const char *szNewName);

	CGoods* GetGoodsById(const CGUID& guid);			//根据ID得到物品

public:
	const CGUID& GetPersonalRgnGUID(void);
	const CGUID& GetPersonalHouseRgnGUID(void);
	void SetPersonalRgnGUID(const CGUID& guid);
	void SetPersonalHouseRgnGUID(const CGUID& guid);

	////!					添加一个队伍副本GUID
	//void				AddTeamRgnGUID(LONG lTemplateRgnID, const CGUID& guid, BOOL bCover);
	////!					删除一个队伍副本GUID
	//void				DelTeamRgnGUID(LONG lTemplateRgnID);
	//void				DelTeamRgnGUID(const CGUID& guid);
	////!					得到一个队伍副本的GUID
	//const	CGUID&		GetTeamRgnGUID(LONG lTemplateRgnID);
	////!					得到队伍副本GUID列表
	//map<LONG, CGUID>&	GetTeamRgnGuidMap(VOID){return m_mapTeamRgnGUID;}
	////!					队伍副本GUID编码
	//VOID				RgnGuidCode(vector<BYTE> &vData);
	////!					队伍副本GUID解码
	//VOID				RgnGuidDecode(BYTE *pData, LONG &lPos);
	////!					队伍副本GUID编码
	//VOID				RgnGuidCode(DBWriteSet& setWriteDB);
	////!					队伍副本GUID解码
	//VOID				RgnGuidDecode(DBReadSet& setReadDB);

	long GetCurPRgnNum(void) { return m_CurPRgnNum; }
	long GetCurTRgnNum(void) { return m_CurTRgnNum; }

	void SetCurPRgnNum(long num) { m_CurPRgnNum = num; }
	void SetCurTRgnNum(long num) { m_CurTRgnNum = num; }

	// 取得玩家登录时间
	DWORD			GetLoginTime(void) { return m_dwLoginTime; }
	// 设置玩家登录时间
	void			SetLoginTime(DWORD time) { m_dwLoginTime = time; }
private:
	CGUID m_PersonalRgnGUID;
	CGUID m_PersonalHouseRgnGUID;
	//map<LONG, CGUID> m_mapTeamRgnGUID;

	long m_CurPRgnNum;
	long m_CurTRgnNum;
	bool m_bFirstLogin;

	// 玩家登录时间
	DWORD			m_dwLoginTime;

	//-------------------------------------------------------------------------------
	// 最近购买的10种商品列表. by Fox.		2008-02-14
	// func : 由GS记录，WS只负责编解码
	//-------------------------------------------------------------------------------
public:
	typedef map<DWORD,list<DWORD>> IncShopCur10Map;
	typedef IncShopCur10Map::iterator IncShopCur10Iter;
public:
	IncShopCur10Map &GetIncShopCur10()			{ return m_mapIncShopCur10; }
	void ClearIncShopCur10();

	CIncrementShopList::VEC_BUYLIST GetIncTradeList() { return m_vecTradeList; }
	void ClearIncTradeList() { m_vecTradeList.clear(); }
	void AddIncTradeRecord( CIncrementShopList::IncShopTradeLog stIncTradeElem );

	void AddLimitGoodsRecord(DWORD dwGoodsId,DWORD dwGoodsNum);
	map<DWORD,DWORD>& GetLimitGoodsMap(){return m_mapLimitGoodsRecord;}
	void ClearLimitGoodsRecord(){m_mapLimitGoodsRecord.clear();}
private:
	//增值商店最近购买的Top10记录
	IncShopCur10Map m_mapIncShopCur10;
	//增值商店购买详细记录列表
	CIncrementShopList::VEC_BUYLIST m_vecTradeList;
	//增值商店数量限购物品购买记录
	map<DWORD,DWORD> m_mapLimitGoodsRecord;


	////////////////////////////////////
public:
	//!	得到玩家所在GS的socket
	LONG	GetGsSocket(void)				{return m_GsSocket;}
	//!		设置玩家所在GS的socket
	void	SetGsSocket(LONG lGsSocket)		{m_GsSocket = lGsSocket;}
	LONG	GetIP(VOID)						{return m_lIP;}
	VOID	SetIP(LONG lIP)					{m_lIP = lIP;}
private:
	LONG m_GsSocket;
	LONG m_lIP;
	//////////////////////////////////////////////////////////////////////////
	// 小精灵辅助
	//////////////////////////////////////////////////////////////////////////
public:
	SpriteSystem		m_SpriteSystem;

	//////////////////////////////////////////////////////////////////////////
	// 玩家需要保存在服务器的设置
	//////////////////////////////////////////////////////////////////////////
public:
	PlayerSetupOnServer	m_SetupOnServer;

	//////////////////////////////////////////////////////////////////////////
	// 勋章容器、当前称号
	//////////////////////////////////////////////////////////////////////////
public:
	MedalContainer			m_MedalContainer;

	//!						设置称号的显示类型
	void					SetTitleType(LONG lType)	{m_lCurrTitleType = lType;};
	//!						得到称号的显示类型
	LONG					GetTitleType(void)			{return m_lCurrTitleType;};

private:
	//!						当前称号类型
	LONG					m_lCurrTitleType;

	// Fox@20081223 宠物
public:
	/// 取得宠物列表
	C_PETS GetPets(void) { return m_Pets; }
	/// 创建宠物
	CPet* CreatePet(const CGUID& guid);
	/// 添加宠物
	void AddPet(CPet* pet);
	/// 删除指定GUID宠物
	void DelPet(const CGUID& guid);
	/// 删除所有宠物
	void ReleaseAllPets(void);

private:
	/// 删除宠物
	void DelPet(ITR_PET& itr);

private:
	PETS m_Pets;			///< 宠物列表

public:
	Business::CPackage &GetBusinessPackage()
	{
		return m_BusinessContainer;
	}
	/// 判断该玩家是否是商人
	bool IsBusinessMan() const { return m_Property.byIsBusinessMan == 1; }
	/// 改变商人身份
	void SetBusinessMan( bool bBusinessMan ) { m_Property.byIsBusinessMan = ( bBusinessMan ? 1 : 0 ); }
private:
	Business::CPackage m_BusinessContainer;

	// 反外挂相关数据 BAIYUN@20090602
private:
	AntiCheatData    m_ACProperty;
public:
	AntiCheatData*   GetACProperty()   {return &m_ACProperty;}
	void             SetACProperty(AntiCheatData* ACData)
	                                   {memcpy(&m_ACProperty, ACData, sizeof(AntiCheatData));}
	void             SetDBACProperty(dbAntiCheatData *DBACData)
	                                   {memcpy(&m_ACProperty.m_dbData, DBACData, sizeof(dbAntiCheatData));}
private:
	VariableList *m_VarList;
public:
	VariableList *GetVariableList() { return m_VarList; }
};
