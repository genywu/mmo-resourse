#pragma once
#include "moveshape.h"
#include ".\Container\CAmountLimitGoodsContainer.h"
#include ".\Container\CVolumeLimitGoodsContainer.h"
#include ".\Container\CEquipmentContainer.h"
#include ".\Container\CSubpackContainer.h"
#include ".\Container\CWallet.h"
#include ".\container\CSilverWallet.h"
#include "../../setup/IncrementShopList.h"


#include ".\Container\CBank.h"
#include ".\Container\CDepot.h"
#include "..\..\..\DBAccess\MyAdoBase.h"
//add by chenaly....07.4.5........
#include "..\..\PUBLIC\PlayerDef.h"
//add end.........................
#include "..\..\..\public\RoleRecord.h"
#include "..\..\..\public\DeputyOccuRecord.h"
#include "..\..\..\public\PlayerSetupOnServer.h"
#include "PlayerDepot.h"
#include "../public/DBDefine.h"
#include "SpriteSystem.h"
#include "MedalContainer.h"
#include "Mail/Mail.h"
#include <set>
#include <map>
#include "../public/DBDefine.h"
#include "session/WorldServerSession.h"
#include "Organizingsystem\organizingctrl.h"
#include "business/CBusinessPackage.h"
#include "..\..\public\QuestIndexXml.h"
#include "..\..\..\public\FunctionKeyMapSystem.h"
#include "..\..\public\LotteryXml.h"

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
		DWORD	wCooldown;				//冷却时间
	};

	struct tagShopSelledGoods 
	{
		DWORD dwSelledPrice;
		CGoods* pGoods;
	};

	// 防外挂相关数据中需要存到数据库中的数据 BAIYUN@20090602
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

	// 防外挂相关数据
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
	map<CEquipmentContainer::EQUIPMENT_COLUMN,CGoods*>* GetEquipmentGoodsMap(){return m_cEquipment.GetGoodsMap();}
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
	CWallet						m_cWallet;
	CSilverWallet				m_cSilverWallet;
	GemContainer				m_cGem;


	//!				仓库
	PlayerDepot		m_PlayerDepot;

	char			m_CardPwd[PASSWORD_SIZE];
	DWORD			m_CardPwdThawTime;
private:

	tagPPro   m_Property;                   //  基本属性
	tagPFPro  m_BaseFightProperty;          //  战斗基本属性
	tagPFPro  m_FactFightProperty;           // 战斗实际属性
	tagPlayerAppendProperty  m_AppendProperty;//保存附加属性
	

	bool m_bSaveDepot;

	list<tagSkill>	m_listSkillID;		//	技能列表

	//DWORD m_dwCreateNo;	// 人物创建的次序

	MAPFKEY m_FuncKey;		// 保存角色的功能热键映射表（玩家修改过的，与默认值不同）

	//DWORD m_dwLogoutTime;	// 人物下线时间

	//////////////////////////////////////////////////////////////////////////
	//	脚本变量
	//////////////////////////////////////////////////////////////////////////
public:
	void SetVariableNum(long l)		{m_lVariableNum=l;}
	long GetVariableNum()			{return m_lVariableNum;}
	void SetGuidNum(long l)			{m_lGuidNum = l;}
	long GetGuidNum()				{ return m_lGuidNum; }
	//m_lVariableDataLength+变量个数LONG+GUID个数LONG+变量长度LONG,EntityManager.cpp line.3516
	void SetVariableData(char* p)	{M_FREE(m_pVariableData,m_lVariableDataLength); m_pVariableData=p;}
	char* GetVariableData()			{return m_pVariableData;}
	void SetVariableDataLength(long l)		{m_lVariableDataLength=l;}
	long GetVariableDataLength()			{return m_lVariableDataLength;}

	// 解码玩家的功能热键设置数据
	bool DecodeHotKeyData(DBReadSet& setReadDB,bool bExData = true);
	bool CodeHotKeyData(DBWriteSet& setWriteDB,bool bExData = true);

	// 获取当前正在使用的功能热键映射表
	MAPFKEY& GetCurMapKey() { return m_FuncKey; }

private:
	long m_lVariableNum;		// 变量个数
	long m_lGuidNum;            // GUIDNum
	char* m_pVariableData;		// 变量数据
	long m_lVariableDataLength;	// 变量数据长度

	/////////////////////////////////////////////////////////////////////
	// 属性存取接口
	/////////////////////////////////////////////////////////////////////
public:

	//// 创建的次序号
	//void	SetCreateNo(DWORD dwNo) { m_dwCreateNo = dwNo; }
	//DWORD	GetCreateNo()			{ return m_dwCreateNo; }

	// 角色下线时间
	//void	SetLogoutTime(DWORD dwLogoutTime)	{ m_dwLogoutTime = dwLogoutTime;}
	//DWORD	GetLogoutTime()		{ return m_dwLogoutTime; }

	//changed by chenaly.....07.4.5..............
	//基本属性 接口  m_Property
	eOccupation	GetOccupation()				{return m_Property.byOccu;}			//职业
	void	SetOccupation(BYTE l)	{m_Property.byOccu=(eOccupation)l;}
	//副职业
	BYTE	GetDOccupation()				{return m_Property.byAssOccu;}
	void	SetDOccupation(eDeputyOccu l)	{m_Property.byAssOccu=l;}
	//星座
	BYTE	GetConst()					{return m_Property.byConst;}
	void	SetConst(eConst l)			{m_Property.byConst=l;}
	//巴塔克声望
	DWORD	GetBatakCredit()			{return m_Property.dwBatakCredit;}
	void	SetBatakCredit(DWORD l)		{m_Property.dwBatakCredit=l;}
	//勋章分数
	DWORD	GetMedalScores()			{return m_Property.dwMedalScores;}
	void	SetMedalScores(DWORD l)		{m_Property.dwMedalScores=l;}

	BYTE	GetSex()					{return m_Property.bySex;}				//性别
	void	SetSex(BYTE l)				{m_Property.bySex=l;}
	long	GetHeadPic()				{return m_Property.lHairPic;}			// 头部图片
	void	SetHeadPic(long l)			{m_Property.lHairPic=l;} 
	long	GetFacePic()				{return m_Property.lFacePic;}			//面部图片
	void	SetFacePic(long l)			{m_Property.lFacePic=l;}
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
	BYTE    GetRankOfNobility()         { return m_Property.byRankOfNobility;}	//爵位等级  
	void    SetRankOfNobility(BYTE l)   {m_Property.byRankOfNobility=l;}
	DWORD   GetRankOfNobCredit()        { return m_Property.dwRankOfNobCredit;}	//爵位的声望值  
	void    SetRankOfNobCredit(DWORD l) { m_Property.dwRankOfNobCredit=l;}
	BYTE    GetRankOfMercenary()        { return m_Property.byRankOfMercenary;}	//佣兵等级   
	void    SetRankOfMercenary(BYTE l)  {m_Property.byRankOfMercenary=l;}
	DWORD   GetMercenaryCredit()        { return m_Property.dwMercenaryCredit;}	//佣兵声望  
	void    SetMercenaryCredit(DWORD l) {m_Property.dwMercenaryCredit=l;}
	BYTE	GetCountry()				{ return m_Property.byCountry; }		  //国家
	void	SetCountry(BYTE l)			{ m_Property.byCountry = l; }
	long	GetCountryContribute()		{ return m_Property.lCountryContribute; } //国家贡献
	void	SetCountryContribute(long l){ m_Property.lCountryContribute  = l; }   
	DWORD	GetSpouseId()				{return m_Property.dwSpouseID;}			//配偶身份
	void	SetSpouseId(DWORD l)		{m_Property.dwSpouseID=l;}
	DWORD   GetSpouseParam()            { return m_Property.dwSpouseParam;}		//配偶关系值 
	void    SetSpouseParam(DWORD l)     { m_Property.dwSpouseParam=l;}
	BYTE    GetBusinessLevel()          { return m_Property.byBusinessLevel;}	//商业等级  
	void    SetBusinessLevel(BYTE l)    { m_Property.byBusinessLevel=l;}
	DWORD   GetBusinessExp()            { return m_Property.dwBusinessExp;}		//商业经验  
	void    SetBusinessExp(DWORD l)     { m_Property.dwBusinessExp=l;}
	BYTE    GetArtisanLevel()           { return m_Property.byArtisanLevel;}	//工匠等级  
	void    SetArtisanLevel(BYTE l)     { m_Property.byArtisanLevel=l;}
	DWORD   GetArtisanExp()             {return m_Property.dwArtisanExp;}		//工匠经验 
	void	SetZanDoCredit(DWORD l)		{m_Property.dwZanDoHunterCredit=l;}		
	DWORD	GetZanDoCredit()			{return m_Property.dwZanDoHunterCredit;}//赞多狩猎声望
	void	SetChurchCredit(DWORD l)	{ m_Property.dwChurchCredit=l;}
	DWORD	GetChurchCredit()			{return m_Property.dwChurchCredit;}		//光之教会声望
	void    SetArtisanExp(DWORD l)      { m_Property.dwArtisanExp=l;} 
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
	WORD    GetPKOnOff()                {return m_Property.wPKOnOff;}			//pk状态 
	void    SetPKOnOff(WORD L)          {m_Property.wPKOnOff=L;}
	bool	GetShowCountry(void)		{return m_Property.bShowCountry;}
	void	SetShowCountry(bool bShow)	{m_Property.bShowCountry = bShow;}
	void	SetOccuLvl(BYTE* pBuf,long lSize) {memcpy(&m_Property.byOccuLvl[0],pBuf,lSize);}
	BYTE*	GetOccuLvl()				{return &m_Property.byOccuLvl[0];}
	void	SetOccuExp(BYTE* pBuf,long lSize) {memcpy(&m_Property.dwOccuExp[0],pBuf,lSize);}
	BYTE*	GetOccuExp()				{return (BYTE*)&m_Property.dwOccuExp[0];}
	void	SetOccuSP(BYTE* pBuf,long lSize) {memcpy(&m_Property.dwOccuSP[0],pBuf,lSize);}
	BYTE*	GetOccuSP()					{return (BYTE*)&m_Property.dwOccuSP[0];}

	long GetCurPKCP() const { return m_Property.lCurPKCP; }
	void SetCurPKCP( long v ) 
	{ 
		m_Property.lCurPKCP = v;
	}


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

	/////////基本战斗属性
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
	short   GetBaseEnerRecoverSpeed()             {return m_BaseFightProperty.wEnerRecoverSpeed ;}//能量恢复速度
	void    SetBaseEnerRecoverSpeed(short l)      {m_BaseFightProperty.wEnergySuck =l;}
	short   GetBaseFightEnerRecoverSpeed()        {return m_BaseFightProperty.wFightEnerRecoverSpeed;}	//战斗能量恢复速度 
	void    SetBaseFightEnerRecoverSpeed(short l) {m_BaseFightProperty.wFightEnerRecoverSpeed=l;}
	WORD    GetBaseStamRecoverSpeed()       {return m_BaseFightProperty.wStamRecoverSpeed;}//回体力 
	void    SetBaseStamRecoverSpeed(WORD l) {return;}									 //.....
	WORD    GetBaseStrenth()                {return m_BaseFightProperty.wStrenth;}		 //力量
	void    SetBaseStrenth(WORD l)          {m_BaseFightProperty.wStrenth=l;}     
	WORD    GetBaseAgility()                {return m_BaseFightProperty.wAgility;}		 //敏捷
	void    SetBaseAgility(WORD l)          {m_BaseFightProperty.wAgility=l;}
	WORD    GetBaseConsti()                 {return m_BaseFightProperty.wConsti;}		 //体质
	void    SetBaseConsti(WORD l)           {m_BaseFightProperty.wConsti=l;}
	WORD    GetBaseIntell()                 {return m_BaseFightProperty.wIntell;}		 //智力
	void    SetBaseIntell(WORD l)           {m_BaseFightProperty.wIntell=l;}
	WORD    GetBaseWisdom()                 {return m_BaseFightProperty.wWisdom;}		 //智慧
	void    SetBaseWisdom(WORD l)           {m_BaseFightProperty.wWisdom=l;} 
	WORD    GetBaseSpirit()                 {return m_BaseFightProperty.wSpirit;}		 //精神
	void    SetBaseSpirit(WORD l)           {m_BaseFightProperty.wSpirit=l;}
	WORD    GetBaseBlock()                  {return m_BaseFightProperty.wFightBlock ;}	 //格挡
	void    SetBaseBlock(WORD l)            {m_BaseFightProperty.wFightBlock =l;}
	WORD    GetBaseParry()                  {return m_BaseFightProperty.wParry;}		 //招架
	void    SetBaseParry(WORD l)            {m_BaseFightProperty.wParry=l;}
	WORD    GetBaseDodge()                  {return m_BaseFightProperty.wDodge;}		 //闪避
	void    SetBaseDodge(WORD l)            {m_BaseFightProperty.wDodge=l;}
	
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
	WORD    GetBaseMAtk()                   {return m_BaseFightProperty.wMAtk;}			 //魔法攻击力
	void    SetBaseMAtk(WORD l)             {m_BaseFightProperty.wMAtk=l;}
	WORD    GetBaseMCri()                   {return m_BaseFightProperty.wMCri;}			 //魔法爆击率
	void    SetBaseMCri(WORD l)             {m_BaseFightProperty.wMCri=l;}
	WORD    GetBaseBloodSuk()               {return m_BaseFightProperty.wBloodSuk;}		 //生命吸收率
	void    SetBaseBloodSuk(WORD l)         {m_BaseFightProperty.wBloodSuk=l;}
	WORD    GetBaseManaSuck()               {return m_BaseFightProperty.wManaSuck;}		 //法力吸收率
	void    SetBaseManaSuck(WORD l)         {m_BaseFightProperty.wManaSuck=l;}
	WORD    GetBaseEnergySuck()                 {return m_BaseFightProperty.wEnergySuck;}//能量吸收率
	void    SetBaseEnergySuck(WORD l)           {m_BaseFightProperty.wEnergySuck=l;}
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
	void    SetEnerRecoverSpeed(short l)      {m_FactFightProperty.wEnergySuck =l;}
	short   GetFightEnerRecoverSpeed()        {return m_FactFightProperty.wFightEnerRecoverSpeed;}	//战斗能量恢复速度 
	void    SetFightEnerRecoverSpeed(short l) {m_FactFightProperty.wFightEnerRecoverSpeed=l;}
	WORD    GetStamRecoverSpeed()       {return m_FactFightProperty.wStamRecoverSpeed;}//回体力 &
	void    SetStamRecoverSpeed(WORD l) {} //.....
	WORD    GetStrenth()                {return m_FactFightProperty.wStrenth;}		//力量
	void    SetStrenth(WORD l)          {m_FactFightProperty.wStrenth=l;}     
	WORD    GetAgility()                {return m_FactFightProperty.wAgility;}		//敏捷
	void    SetAgility(WORD l)          {m_FactFightProperty.wAgility=l;}
	WORD    GetConsti()                 {return m_FactFightProperty.wConsti;}		//体质
	void    SetConsti(WORD l)           {m_FactFightProperty.wConsti=l;}
	WORD    GetIntell()                 {return m_FactFightProperty.wIntell;}		//智力
	void    SetIntell(WORD l)           {m_FactFightProperty.wIntell=l;}
	WORD    GetWisdom()                 {return m_FactFightProperty.wWisdom;}		//智慧
	void    SetWisdom(WORD l)           {m_FactFightProperty.wWisdom=l;}
	WORD    GetSpirit()                 {return m_FactFightProperty.wSpirit;}		//精神
	void    SetSpirit(WORD l)           {m_FactFightProperty.wSpirit=l;}
	WORD    GetBlock()                  {return m_FactFightProperty.wFightBlock ;}		//格挡
	void    SetBlock(WORD l)            {m_FactFightProperty.wFightBlock =l;}
	WORD    GetParry()                  {return m_FactFightProperty.wParry;}		//招架
	void    SetParry(WORD l)            {m_FactFightProperty.wParry=l;}
	WORD    GetDodge()                 {return m_FactFightProperty.wDodge;}			//闪避
	void    SetDodge(WORD l)           {m_FactFightProperty.wDodge=l;}
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
	WORD    GetMAtk()                   {return m_FactFightProperty.wMAtk;}			//魔法攻击力
	void    SetMAtk(WORD l)             {m_FactFightProperty.wMAtk=l;}
	WORD    GetMCri()                   {return m_FactFightProperty.wMCri;}			//魔法爆击率
	void    SetMCri(WORD l)             {m_FactFightProperty.wMCri=l;}
	WORD    GetBloodSuk()               {return m_FactFightProperty.wBloodSuk;}		//生命吸收率
	void    SetBloodSuk(WORD l)         {m_FactFightProperty.wBloodSuk=l;}
	WORD    GetEnergySuck()             {return m_FactFightProperty.wEnergySuck;}//能量吸收率
	void    SetEnergySuck(WORD l)       {m_FactFightProperty.wEnergySuck=l;}
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

	//----------------------------------------------------------------
	// 角色创建时间
	//----------------------------------------------------------------
private:
	DWORD m_dwRoleCreateTime;
public:
	DWORD	GetRoleCreateTime()				{ return m_dwRoleCreateTime;  }
	VOID	SetRoleCreateTime( DWORD dwTime )	{ m_dwRoleCreateTime = dwTime; }

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
private:
	CRoleQueue m_RoleQueue;
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
	BOOL AddQuestFromDB(CRoleRecord* rolerecord);	
	const list<CRoleRecord*>* GetPlayerQuests()	{return m_RoleQueue.GetRoleRecordList();}

	bool AddMerQuestDataByteArray(vector<BYTE>* pByteArray);	//添加俑兵任务数据到ByteArray
	bool DecordMerQuestDataFromByteArray(BYTE* pByte, long& pos);//解俑兵任务数据

	bool CodeMerQuestDataToDataBlock(DBWriteSet& setWriteDB);
	bool DecodeMerQuestDataFromDataBlock(DBReadSet& setReadDB);

	bool	GetPlayerLogin()		{return m_bLogin;}
	void	SetPlayerLogin(bool b)	{m_bLogin = b;}

	bool	GetFirstLogin()		{return m_bFirstLogin;}
	void	SetFirstLogin(bool b)	{m_bFirstLogin = b;}
	//任务引导
private:
	map<long,CQuestIndexXml::tagQuestIndexForServer>  m_mapQuestIndex;      //记录任务引导的相关信息
public:
	map<long,CQuestIndexXml::tagQuestIndexForServer>*  GetMapQuestIndex() {return &m_mapQuestIndex;}        //获得任务引导列表
	void AddQuestIndex(CQuestIndexXml::tagQuestIndexForServer tagQuestIndex);     //向列表中加一个任务引导
	void ClearQuestIndexMap(){m_mapQuestIndex.clear();};
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


public:
	const CGUID& GetTeamID()				{return m_TeamGuid; }
	void SetTeamID(const CGUID& guid)		{m_TeamGuid = guid;}

	long GetSentaiPoints(void)				{return m_SentaiPoints;}
	void SetSentaiPoints(long Points, bool SendMsg);

	long GetPenaltyPoints(void)				{return m_PenaltyPoints;}
	void SetPenaltyPoints(long Points)		{m_PenaltyPoints = Points;}

private:
	CGUID m_TeamGuid;

	//! 战队积分
	long m_SentaiPoints;
	//! 战队活动惩罚
	long m_PenaltyPoints;

	// 玩家更名
public:
	int ChangeName(const char *szNewName);

	CGoods* GetGoodsById(const CGUID& guid);			//根据ID得到物品

public:
	const CGUID& GetPersonalRgnGUID(void);
	const CGUID& GetPersonalHouseRgnGUID(void);
	void SetPersonalRgnGUID(const CGUID& guid);
	void SetPersonalHouseRgnGUID(const CGUID& guid);

	long GetCurPRgnNum(void) { return m_CurPRgnNum; }
	long GetCurTRgnNum(void) { return m_CurTRgnNum; }

	void SetCurPRgnNum(long num) { m_CurPRgnNum = num; }
	void SetCurTRgnNum(long num) { m_CurTRgnNum = num; }

	// 取得玩家登录（切服）流程开始的时间
	DWORD			GetBeginLoginTime(void) { return m_BeginLoginTime; }
	// 设置玩家登录（切服）流程开始的时间
	void			SetBeginLoginTime(DWORD time) { m_BeginLoginTime = time; if(0 == m_LoginTime)m_LoginTime = time;}
	// 取得登陆时间
	DWORD			GetLoginTime(void) { return m_LoginTime; }
private:
	CGUID m_PersonalRgnGUID;
	CGUID m_PersonalHouseRgnGUID;

	long m_CurPRgnNum;
	long m_CurTRgnNum;
	bool m_bFirstLogin;

	// 玩家登录（切服）流程开始的时间timeGetTime()
	DWORD			m_BeginLoginTime;
	// 玩家登陆时间
	DWORD			m_LoginTime;

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
	C_PETS GetPets(void) { return m_Pets; }
	CPet* CreatePet(const CGUID& rID);
	void AddPet(CPet* pPet);
	void DelPet(const CGUID& rID);
	void ReleaseAllPets(void);

private:
	void DelPet(ITR_PET& itr);

private:
	PETS					m_Pets;			// 宠物列表

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

	// 防外挂相关数据 BAIYUN@20090602
private:
	AntiCheatData    m_ACProperty;
public:
	AntiCheatData*   GetACProperty()   {return &m_ACProperty;}
	void             SetACProperty(AntiCheatData* ACData)
	                                   {memcpy(&m_ACProperty, ACData, sizeof(AntiCheatData));}
	void             SetDBACProperty(dbAntiCheatData *DBACData)
	                                   {memcpy(&m_ACProperty.m_dbData, DBACData, sizeof(dbAntiCheatData));}
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
	map<DWORD, tagCountTimer>& GetTimerMap(void);
private:
	// 计时器功能
	map<DWORD, tagCountTimer> m_mapCountTimer;
	static tagCountTimer NULL_TIMER;
	DWORD m_dwCountTimerID;
//////////////////////////彩票系统相关////////////////////////////////////////////////////////////
private:
	vector<tagLottery> m_VecLottery;
	long m_lPersonalCredit;       //个人积分
	long m_lPersonalCreditTotal;   //个人累积积分
	int  m_lPersonalCreditVelue;   //个人积分变量
	int  m_iWorldCreditVelue;      //世界积分变量
	int  m_iLotteryLevel;          //轮盘等级
public:
	void SetPersonalCredit(long l){m_lPersonalCredit=l;};         //设置个人积分
	long GetPersonalCredit(){return m_lPersonalCredit;};          //获得个人积分
	vector<tagLottery>* GetVecLottery(){return &m_VecLottery;};    //获得抽奖列表
	void ClearVecLottery() {m_VecLottery.clear();};               //清空抽奖列表
	void SetPersonalCreditTotal(long l){m_lPersonalCreditTotal = l;};  //设置个人累计积分
	long GetPersonalCreditTotal(){return m_lPersonalCreditTotal;};  //获得个人累积积分
	void SetPersonalCreditVelue(long l){m_lPersonalCreditVelue=l;};  //设置个人积分变量
	long GetPersonalCreditVelue(){return m_lPersonalCreditVelue;};   //获得个人积分变量
	void SetWorldCreditVelue(long l){m_iWorldCreditVelue = l;};      //设置世界积分变量
	long GetWorldCreditVelue(){return m_iWorldCreditVelue;};         //获得世界积分变量
	void SetLotteryLevel(long l){m_iLotteryLevel = l;};             //设置轮盘参数
	long GetLotteryLevel(){return m_iLotteryLevel;};                //获得轮盘参数
};
