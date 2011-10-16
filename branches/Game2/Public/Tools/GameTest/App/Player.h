#pragma once
//#include "../GameTest/resource.h"	
//#include "..\GameTest\PlayerInfo.h"

#include "moveshape.h"

#include "../Utility/gtDef.h"
// #include "Goods.h"
// #include "../../public/BaseDef.h"
// 
// //#include "..\Utility\PlayerDef.h"
// 
// 
// #include "..\Utility\OrganizingParam.h"
//#include ".\PetSystem\CPet.h"
// #include "../../public/BounsDef.h"

class CGameClient;
class CMonster;
class CPet;
class CGoodsList;

struct tagWSMemberInfo;



// 玩家容器编号 - 对应服务器的 CSCContainerObjectMove.h


class CRegion;
class CPlayer : public CMoveShape
{
public:

	CPlayer(void);
	
	~CPlayer(void);

	class CMailMgr*	GetMailMgr(void)	{ return m_pMailMgr; }

	class CSkillMgr*	GetSkillMgr(void)	{ return m_pSkillMgr; }

	class CPetMgr*		GetPetMgr(void)	{ return m_pPetMgr; }

	class CGoodsMgr*	GetGoodsMgr(void)	{ return m_pGoodsMgr; }

	class CTeamMgr*	GetTeamMgr(void)	{ return m_pTeamMgr; }

	class CPlayerShop*	GetShopMgr(void)	{ return m_pShop; }

	class CLinkMan*	GetLinkMan(void)	{ return m_pLinkMan; }

	class CMedalMgr*	GetMedalMgr(void)	{ return m_pMedalMgr; }

	class COrganiseSys* GetOrganiseSys(void)	{ return m_pOrganiseSys; }

	class CSprite*		GetSpriteMgr(void)	{ return m_pSpriteMgr; }

	class CTask*		GetTaskMgr(void)		{ return m_pTaskMgr; }

	class CPlayerCollect*	GetCollectMgr(void)	{ return m_pCollect; }

	class CTradeMgr*	GetTradeMgr(void)			{ return m_pTradeMgr; }


	bool IsSelf()	const				{ return m_bSelf;}

	void SetIsSelf(const bool b)		{ m_bSelf = b; }

private:

	void Release();

	//	技能管理器
	class CSkillMgr*	m_pSkillMgr;

	//	邮件管理器
	class CMailMgr*	m_pMailMgr;

	//	宠物管理器
	class CPetMgr*		m_pPetMgr;

	class CGoodsMgr*	m_pGoodsMgr;

	class CTeamMgr*	m_pTeamMgr;

	class CPlayerShop*	m_pShop;

	class CPlayerCollect*	m_pCollect;

	class CFamily*			m_pFamily;

	class CLinkMan*		m_pLinkMan;

	class CMedalMgr*		m_pMedalMgr;

	class COrganiseSys*	m_pOrganiseSys;

	class CSprite*			m_pSpriteMgr;

	class CTask*			m_pTaskMgr;

	class CTradeMgr*		m_pTradeMgr;

	//	是否主角自己	
  	bool	m_bSelf;		
	//	人物 属性数据结构
  	tagPPro						m_Property;						//基础属性
  	tagPFPro					m_FightProperty;					//战斗属性
  	tagPlayerElementProperty	m_arrElementProperty[MT_Max];

	CGUID			m_PlayerMoneyGuid;				//	金币
  	CGUID			m_PlayerBaoShiGuid;				//	宝石
  	CGUID			m_PlayerYuanBaoGuid;				//	元宝
  	CGUID			m_PlayerJiFenGuid;				//	积分
  	CGUID			m_PlayerSilverMoneyGuid;			//	银币
  
  	DWORD			m_dwPlayerMoneyNum;				//	金币
  	DWORD			m_dwPlayerBaoShiNum;				//	宝石
  	DWORD			m_dwPlayerYuanBaoNum;				//	元宝
  	DWORD			m_dwPlayerJiFenNum;				//	积分
  	DWORD			m_dwPlayerSilverMoneyNum;		//	银币

	char			m_strPlayerOfficialRank[64];		//	国家官阶

	std::map<long,long>   m_GeniusTable;    // 天赋配置表,ID-level

 public:
 	virtual void SetAction(WORD l);
 	virtual void SetState(WORD l);

 	DWORD	GetPlayerMoneyNum()					{ return m_dwPlayerMoneyNum; }
 	DWORD	GetPlayerJiFenNum()					{ return m_dwPlayerJiFenNum; }
 	DWORD	GetPlayerBaoShiNum()					{ return m_dwPlayerBaoShiNum; }
 	DWORD	GetPlayerYuanBaoNum()					{ return m_dwPlayerYuanBaoNum; }
 	DWORD	GetPlayerSilverMoneyNum()				{ return m_dwPlayerSilverMoneyNum; }
 
 	void	SetPlayerJiFenNum(DWORD l)			{ m_dwPlayerJiFenNum = l; }
 	void	SetPlayerMoneyNum(DWORD l)			{ m_dwPlayerMoneyNum = l; }
 	void	SetPlayerBaoShiNum(DWORD l)			{ m_dwPlayerBaoShiNum = l; }
 	void	SetPlayerYuanBaoNum(DWORD l)			{ m_dwPlayerYuanBaoNum = l; }
 	void	SetPlayerSilverMoneyNum(DWORD l)		{ m_dwPlayerSilverMoneyNum = l; }
 
 	void	SetPlayerMoneyGuid(const CGUID& guid)			{m_PlayerMoneyGuid	= guid; }
 	void	SetPlayerBaoShiGuid(const CGUID& guid)			{m_PlayerBaoShiGuid	= guid; }
 	void	SetPlayerYuanBaoGuid(const CGUID& guid)			{m_PlayerYuanBaoGuid= guid;}
 	void	SetPlayerJiFenGuid(const CGUID& guid)			{m_PlayerJiFenGuid	= guid;}
 	void	SetPlayerSilverMoneyGuid(const CGUID& guid)	{m_PlayerSilverMoneyGuid	= guid;}
 
 	CGUID&	GetPlayerSilverGuid()								{return m_PlayerSilverMoneyGuid;}
 	CGUID&	GetPlayerMoneyGuid()								{return m_PlayerMoneyGuid;}
 	CGUID&	GetPlayerBaoShiGuid()								{return m_PlayerBaoShiGuid;}
 	CGUID&	GetPlayerYuanBaoGuid()							{return m_PlayerYuanBaoGuid;}
 	CGUID&	GetPlayerJiFenGuid()								{return m_PlayerJiFenGuid;}

	tagPPro &GetPlayerProperty()						{ return m_Property;}

	DWORD	GetOccu()					{return m_Property.byAssOccu;}		
	void	SetOccu(DWORD dwOccu)		{m_Property.byAssOccu = (eDeputyOccu)dwOccu;}	

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
	void	SetCountryContribute(long l)	{m_Property.lCountryContribute=l;}

	DWORD	GetUpgradeExp()				{return	m_Property.dwUpgradeExp;}			//下一级升级所需经验
	void	SetUpgradeExp(DWORD l)		{m_Property.dwUpgradeExp=l;}				

	BYTE	GetBusinessLevel()			{return m_Property.byBusinessLevel;}		//商业等级
	void	SetBusinessLevel(BYTE l)	{m_Property.byBusinessLevel=l;}

	DWORD	GetBusinessExp()			{return m_Property.dwBusinessExp;}		//商业经验
	void	SetBusinessExp(DWORD l)		{m_Property.dwBusinessExp = l;}

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

	DWORD	GetHotKey(long lPos)		{return m_Property.dwHotKey[lPos];}		//热健(F1~F12,1~+)
	void	SetHotKey(long lPos,DWORD l){m_Property.dwHotKey[lPos]=l;}

	//	//PK开关(b0:普通,b1:队伍,b2:行会,b3:罪犯,b4:他国玩家)
	bool IsPk_Normal()	{ return m_Property.byPKOnOff & 0x01 ? true : false ;}		
	bool IsPk_Team()		{ return m_Property.byPKOnOff & 0x02 ? true : false ;}
	bool IsPk_Union()		{ return m_Property.byPKOnOff & 0x03 ? true : false ;}
	bool IsPk_Badman()	{ return m_Property.byPKOnOff & 0x04 ? true : false ;}
	bool IsPk_Country()	{ return m_Property.byPKOnOff & 0x05 ? true : false ;}
	void	SetPKOnOff(BYTE b)			{ m_Property.byPKOnOff = b; }				//设置PK开关
	DWORD	GetSkillPoint()				{return	m_Property.dwOccuSP[m_Property.byOccu];}			//当前拥有的技能点数(SP)
	//得到职业等级
	inline BYTE	GetOccuLvl(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.byOccuLvl[byOccu];
		return 0;
	}

	inline void  SetOccuLvl(eOccupation byOccu,BYTE l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.byOccuLvl[byOccu] = l;
	}

	//得到职业经验
	inline DWORD	GetOccuExp(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.dwOccuExp[byOccu];
		return 0;
	}
	inline void SetOccuExp(eOccupation byOccu,DWORD l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.dwOccuExp[byOccu] = l;
	}

	inline DWORD GetOccuSp(eOccupation byOccu)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			return m_Property.dwOccuSP[byOccu];
		return 0;
	}

	inline void SetOccuSp(eOccupation byOccu,DWORD l)
	{
		if(byOccu >= 0 && byOccu < OCC_Max)
			m_Property.dwOccuSP[byOccu] = l;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	const char*	GetOfficialRank() const							{ return m_strPlayerOfficialRank;}
	void	SetOfficialRank(const char* str)							{ strcpy(m_strPlayerOfficialRank ,str); }

	//人物战斗属性接口
	tagPlayerFightProperty GetPlayerFightProperty()		{ return m_FightProperty; }


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

	//WORD	GetFightBlock()				{return	m_FightProperty.wFightBlock;}			//格挡
	//void	SetFightBlock(WORD l)		{m_FightProperty.wFightBlock=l;}

	WORD	GetParry()					{return	m_FightProperty.wParry;}				//招架
	void	SetParry(WORD l)			{m_FightProperty.wParry=l;}

	WORD	GetDodge()					{return	m_FightProperty.wDodge;}				//闪避 
	void	SetDodge(WORD l)			{m_FightProperty.wDodge=l;}

	WORD	GetCharm()					{return	m_FightProperty.wCharm;}				//魅力
	void	SetCharm(WORD l)			{m_FightProperty.wCharm=l;}

	WORD	GetHit()					{return m_FightProperty.wHit;}					//命中
	void	SetHit(WORD l)				{m_FightProperty.wHit=l;}

	WORD	GetAbsorb()					{return m_FightProperty.wAbsorb;}				//专注
	void	SetAbsorb(WORD l)			{m_FightProperty.wAbsorb=l;}

	WORD	GetDef()					{return m_FightProperty.wDef;}					//物理防御
	void	SetDef(WORD l)				{m_FightProperty.wDef=l;}

	WORD	GetMdef()					{return m_FightProperty.wMdef;}					//魔法防御
	void	SetMdef(WORD l)				{m_FightProperty.wMdef=l;}

	WORD	GetCri()					{return m_FightProperty.wCri;}					//物理爆击
	void	SetCri(WORD l)				{m_FightProperty.wCri=l;}

	WORD	GetMAtkSpeed()				{return m_FightProperty.wMAtkSpeed;}			//施法速度
	void	SetMAtkSpeed(WORD l)		{m_FightProperty.wMAtkSpeed=l;}

	WORD	GetBuffPower()				{return m_FightProperty.wBuffPower;}			//Buff攻击强度
	void	SetBuffPower(WORD l)		{m_FightProperty.wBuffPower=l;}

	WORD	GetMinAtk()					{return m_FightProperty.wMinAtk;}				//最小攻击力
	void	SetMinAtk(WORD l)			{m_FightProperty.wMinAtk=l;}

	WORD	GetMaxAtk()					{return m_FightProperty.wMaxAtk;}				//最大攻击力
	void	SetMaxAtk(WORD l)			{m_FightProperty.wMaxAtk=l;}

	WORD	GetMAtk()					{return m_FightProperty.wMAtk;}					//魔法攻击力
	void	SetMAtk(WORD l)				{m_FightProperty.wMAtk=l;}

	WORD	GetMCri()					{return m_FightProperty.wMCri;}					//魔法爆击率
	void	SetMCri(WORD l)				{m_FightProperty.wMCri=l;}

	//WORD	GetBloodSuk()				{return m_FightProperty.wBloodSuk;}				//生命吸收率
	//void	SetBloodSuk(WORD l)			{m_FightProperty.wBloodSuk=l;}

	//WORD	GetManaSuck()				{return m_FightProperty.wManaSuck;}				//法力吸收率
	//void	SetManaSuck(WORD l)			{m_FightProperty.wManaSuck=l;}

	WORD	GetWillDef()				{return m_FightProperty.wWillDef;}				//意志抗性
	void	SetWillDef(WORD l)			{m_FightProperty.wWillDef=l;}

	WORD	GetConstiDef()				{return m_FightProperty.wConstiDef;}			//体质抗性
	void	SetConstiDef(WORD l)		{m_FightProperty.wConstiDef=l;}

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

	//WORD	GetEnergySuck()						{return	m_FightProperty.wEnergySuck;}			//能量吸收率
	//void	SetEnergySuck(WORD l)				{m_FightProperty.wEnergySuck=l;}

	WORD	GetCriDef()							{return	m_FightProperty.wCriDef;}				//物理爆击抗性
	void	SetCriDef(WORD l)					{m_FightProperty.wCriDef=l;}

	WORD	GetMCriDef()						{return	m_FightProperty.wMCriDef;}				//魔法爆击抗性
	void	SetMCriDef(WORD l)					{m_FightProperty.wMCriDef=l;}
	//更新的扩展解码接口
	void UpdateProperty(CMessage* pMsg);
	void DecordChangePropertyFromDataBlock(DBReadSet& readDB);
 	virtual bool DecordFromDataBlock(DBReadSet& readDB,bool bExData = true);
// // 	virtual void DecordOrgSysFromDataBlock(DBReadSet& readDB);
// // 	void DecordRecruitDataFromDataBlock(DBReadSet& readDB);
  	void DecordChangeProperyFromDataBlock(DBReadSet& readDB);
	bool DecodeHotKeyData(DBReadSet& setReadDB,bool bExData = true);
// // 
// // 	void DecordPlayerDepotFromDataBlock(DBReadSet& dbr);
// // 	virtual bool DecordPixieFromDataBlock(DBReadSet& dbr);
// // 	virtual bool DecordQuestFromDataBlock(DBReadSet& dbr);
// // 	void DecordRecruitFromDataBlock(DBReadSet& dbr);
// // 
// // 
// // 	//	virtual void DecordDepotGoodsInfoFromDataBlock(DBReadSet& db_read);
// // 
// // private:
// // 
// // 
// // public:
// // 
// // 	tagAutoSupplyData* GetOneSupplySetup()	{ return & m_OneSupplySetup; }
// // 	void SetAutoSupplySetup(long lIndex);			//服务器返回,设置正确的补给配置
// // 	tagAutoSupplyData* GetAutoSupplySetup(long lIndex)		{ return &m_AutoSupplySetup[lIndex]; }
// // 
// // 	long GetItemIndex(long lIndex)		{ return m_AutoSupplySetup[lIndex].lGoodsIndex; }
// // 	long GetItemAmount(long lIndex);
// // 	
// // private:
// // 
// // public:
// // 	HSM_OneTM_Info* GetOneTeammateInfo()	{ return &m_mapOneTeammateInfo; }
// // 
// // 
// // private:
// // //	hash_map<CGUID,tagWSMemberInfo,hash_guid_compare> m_hmapTeamMemberInfo;
// // //	hash_map<CGUID,tagTeammate,hash_guid_compare> m_hmapTeamInfo;
// // 
// // 
// // 	//-----------------------------------------------
// // 	// 重写队伍接口 [12/26/2008 Square]
// // public:
// // 	long GetTeamQuestID()	{ return m_lTeamQuestID; }
// // 	void SetTeamQuestID(long l)	{ m_lTeamQuestID = l; }
// // 	//--
// // //	hash_map<CGUID,tagWSMemberInfo,hash_guid_compare>& GetTeamMemberInfo()	{ return m_hmapTeamMemberInfo; }
// // //	hash_map<CGUID,tagTeammate,hash_guid_compare>& GetTeamInfo()	{ return m_hmapTeamInfo; }
// // 	vector<tagTeammate>& GetTeammateList()		{ return m_vecTeammates; }
// // 	//-----------------------------------------------
// // 	void SetTeamRequesterGuid(const CGUID& guid)	{ m_TeamRequesterGuid = guid; }
// // 	const CGUID& GetTeamRequesterGuid()		{ return m_TeamRequesterGuid; }
// // private:
// // public:
// // 	vector<tagPlayerInfo>	 &GetPlayerInfo()			{ return m_vecPlayerInfo; }
// // 
// // 	vector<tagWSMemberInfo>  &GetTeamMateInfo()			{ return m_vecTeamMateInfo; }
// // 
// // 	vector<tagTeammate>		 &GetTeam()					{ return m_vecTeammates; }
// // 	// 察看指定的玩家是否是本队伍的
// // 	bool IsTeammate(const CGUID& id);
// // 	// 获得指定ID成员的信息
// // 	tagWSMemberInfo * GetWsMemberInfo(const CGUID& id);
// // 	// 获得和设置队长
// // 	const CGUID& GetCaptainID() const					{ return m_CaptainID; }
// // 	void SetCaptainID(const CGUID& id)					{ m_CaptainID = id; }
// // 	// 获得和设置队伍
// // 	const CGUID& GetTeamID()							{ return m_TeamID; }
// // 	void SetTeamID(const CGUID& ID)						{ m_TeamID=ID; }
// // 	// 获得和设置招募状态
// // 	bool IsRecruitingTeam()								{ return m_bIsRecruitingTeam; }
// // 	void SetRecruitingTeam(bool b)						{ m_bIsRecruitingTeam=b; }
// // 	// 获得和设置队伍成员的数量
// // 	long GetTeammateAmount()							{ return m_lTeammateAmount; }
// // 	void SetTeammateAmount(long l)						{ m_lTeammateAmount=l; }
// // 	// 获得和设置队伍的名字
// // 	const char * GetTeamname()							{ return m_strTeamname; }
// // 	void SetTeamname(const char *strName)				{ strcpy(m_strTeamname,strName); }
// // 	// 获得和设置队伍是否存在密码
// // 	bool IsTeamHasPassword()							{ return m_bTeamHasPassword; }
// // 	void SetTeamHasPassword(bool b)						{ m_bTeamHasPassword=b; }
// // 	// 是否可以操作组队相关
// // 	bool IsOperforTeam();
// // 	// 设置组队限制技能
// // 	void SetLimtSkill(long ID, long lLev)				{ m_lLimtSkillID = ID; m_lLimtSkillLev = lLev;}
// // 
// // 	//主要用于Lua下正确发送消息所提供的接口判断函数
// // 	// 是否为一个队的队长
// // 	void SetIsTeamLeader(bool b)						{ m_bIsTeamLeader = b; }
// // 	bool IsTeamLeader()									{ return m_bIsTeamLeader; }
// // 
// // 	//----------------------------------------------------------------------------------------------------------------/*//////////////////////////////////////////////////////////////////////////*/
// // 	//	技能
// // 	//----------------------------------------------------------------------------------------------------------------
// // public:
// // 
// // 	////////////////////07.12.17 XY添加操作消耗性物品，设置其冷却效果的接口///////////////////
// // 
// // 	//	list<tagShortCutGoods>* GetShortCutGoodsList() {return &m_listShortCutGoods;}	
// // 	bool  AddShortCutGoods(WORD wGoodsIndex);							//添加物编号为wGoodsIndex的物品到列表中
// // 	bool  DelShortCutGoods(WORD wGoodsIndex);							//删除物编号为wGoodsIndex的物品
// // 	void  SetGoodsRestoreTime(WORD wGoodsIndex,DWORD dwTime);			//设置物编号为wGoodsIndex的物品的冷却时间
// // 	void  SetGoodsCoolDownStartTime(WORD wGoodsIndex,DWORD dwTime);		//设置物编号为wGoodsIndex的物品的冷却开始时间
// // 	DWORD GetGoodsRestoreTime(WORD wGoodsIndex);						//得到物编号为wGoodsIndex的物品的冷却时间
// // 	DWORD GetGoodsCoolDownStartTime(WORD wGoodsIndex);					//得到物编号为wGoodsIndex的物品的冷却开始时间
// // 	void  SetGoodsUseState(WORD wGoodsIndex,bool bState);				//设置物编号为wGoodsIndex的物品当前是否可被使用的状态
// // 	bool  GetGoodsUseState(WORD wGoodsIndex);							//取得物编号为wGoodsIndex的物品当前是否可被使用的状态
// // public:
// // 
// // 
// // 	//--------------------------------------------------------------------------------------------------------------------------
// // 	//帮会系统(就是行会,家族)
// // 	//  [12/20/2008 Square]
// // 	//--------------------------------------------------------------------------------------------------------------------------
// // private:
// // 
// // 
// // 	// [12/20/2008 Square]
// // 	//是否用Vector更好一些？ hash_map内存占用大了？ MartySa 
// // 	//hash_map<CGUID,FamilyInfo,hash_guid_compare> m_hmapFamily;
// // public:
// // //	hash_map<CGUID,FamilyInfo,hash_guid_compare> *GethmapFamily()	{ return &m_hmapFamily; }
// // 	bool AddFamily(FamilyInfo* info);
// // 	void DelFamilyByGuid(const CGUID& guid);
// // //	FamilyInfo* GetFamilyInfoByGuid(const CGUID& guid)	
// // // 	{
// // // 		hash_map<CGUID,FamilyInfo,hash_guid_compare>::iterator it = m_hmapFamily.find(guid);
// // // 		if( /*m_hmapFamily.find(guid)*/it != m_hmapFamily.end() )		//效率更改
// // // 			return &((*it).second);
// // // 		return NULL;
// // // 	}
// // 	//用于保存和核对行会数据添加
// // 	// 2009.1.13 MartySa
// // //	void ClearhmapFamilyData()					{ if( !m_hmapFamily.empty() ) { m_hmapFamily.clear(); } }
// // public:
// // 	const CGUID& GetFamilyGuid()	{return m_FamilyGuid;}
// // 	void  SetFamilyGuid(const CGUID& guid)	{ m_FamilyGuid = guid; }
// // 
// // 	const CGUID& GetFamilyMasterGuid()	{ return m_FamilyMasterGuid;}
// // 	void SetFamilyMasterGuid(const CGUID& guid)	{m_FamilyMasterGuid = guid; }
// // 
// // 	const long GetFamilyJobLev()	{return m_lFamilyJobLevel;}
// // 	void SetFamilyJobLev(long l)	{ m_lFamilyJobLevel = l; }
// // 
// // 	const char * GetFamilyName() const	{ return m_strFamilyName;}
// // 	void SetFamilyName(const char* strName)	{ strcpy(m_strFamilyName,strName);}
// // 
// // 	const char* GetFamilyTitle() const	{ return m_strFamilyTitle; }
// // 	void SetFamilyTitle(const char* strTitle)	{ strcpy(m_strFamilyTitle,strTitle); }
// // 
// // 	bool GetIsContendState()		{return m_bInContendState;}
// // 	void SetContendState(bool b)	{m_bInContendState = b;}
// // 
// // 	bool GetCityWarDiedState()	{return m_bCityWarDied;}
// // 	void SetCityWarDiedState(bool b)	{m_bCityWarDied = b;}
// // 
// // 	DWORD	GetFacIconCRC()						{return m_dwFacIconCRC;}
// // 	void	SetFacIconCRC(DWORD dwFacIconCRC)	{m_dwFacIconCRC = dwFacIconCRC;}
// // 
// // 	const CGUID& GetUnionID()			{return m_UnionGuid;}
// // 	void SetUnionID(const CGUID& ID)	{m_UnionGuid = ID;}
// // 
// // 	void SetEnemyFamily(set<CGUID,guid_compare>& setFamily)	{ m_EnemyFamily = setFamily; }
// // 	bool IsHaveEnemyFamily()								{ return /*m_EnemyFamily.size() > 0*/!m_EnemyFamily.empty(); }
// // 	bool IsEnemyFamily(const CGUID& familyGuid)
// // 	{
// // 		return m_EnemyFamily.find(familyGuid) != m_EnemyFamily.end();
// // 	}
// // 
// // 	bool IsHaveCityWarEnemyFamily()	{ return /*m_CityWarEnemyFamily.size() > 0*/!m_CityWarEnemyFamily.empty(); }
// // 	bool IsCityWarEnemyFamily(const CGUID& guid)
// // 	{
// // 		return m_CityWarEnemyFamily.find(guid) != m_CityWarEnemyFamily.end();
// // 	}
// // 
// // 	bool IsFriendFamily(const CGUID& familyGuid,const CGUID& unionGuid)
// // 	{
// // 		//传入或者自己的家族guid是无效,则无友好
// // 		if( familyGuid == CGUID::GUID_INVALID || m_FamilyGuid == CGUID::GUID_INVALID)
// // 			return false;
// // 		//如果是自己
// // 		if( familyGuid == m_FamilyGuid )
// // 			return true;
// // 		//如果传入或者自己的同盟guid是无效,则非友好
// // 		if( unionGuid == CGUID::GUID_INVALID || m_UnionGuid == CGUID::GUID_INVALID)
// // 			return false;
// // 		//如果是自己
// // 		if( unionGuid == m_UnionGuid )
// // 			return true;
// // 		//否则
// // 		return false;
// // 	}
// // 
// // 
// // 	/*******************************************************************************\
// // 	|*************************   任务接口/属性   ***********************************|
// // 	\*******************************************************************************/
// // private:
// // public:
// // 	//解码
// // 	//	BOOL DecordRoleRecordFromByteArray(BYTE *pByte,long &pos, tagRoleRecord *record);
// // 	//增加一个任务
// // 	BOOL AddOneRoleRecord(tagRoleRecord*& pRole);
// // 	//
// // 	BOOL AddOneRoleRecord(LONG roleid,LONG step);
// // 	//删除一个任务
// // 	//const char* RemoveQuest(long lQuestID);
// // 	//任务
// // 	list<tagRoleRecord*>*	GetQuestList()		{ return &m_listRoleRecord; }
// // 	//主线任务
// // 	list<tagRoleRecord*>*	GetMainRoleList() { return &m_listRoleRecord; }	
// // 	//判断是否存在一个任务
// // 	BOOL IsRoleBeingByID(long id);
// // 	//镖车GUID
// // 	void SetCarriageID(const CGUID& id)	{m_CarriageID=id;}
// // 	const CGUID& GetCarriageID()		{return m_CarriageID;}
// // 	//	宠物ID
// // 	set<CGUID> *GetPetIDSet()	{return &m_setPetID;}
// // 	bool HasPetArount();										//判断周围有没有自己的宠物
// // 	long GetPetAmount();										//查找自己周围宠物的数量
// // 
// // private:
// // public:
// // 	//	void DecordMedalFromByteArray(BYTE* pByte,long &lPos);
// // 	void DecordMedalFromDataBlock(DBReadSet& dbr);
// // 
// // 	//--------------------------------------------------------------------------------------------------------
// // 	//物品合成部分
// // 	//---------------------------------------------------------------------------------------------------------
// // private:
// // 
// // public:
// // 	tagSynthesizeGoods &GetSyntsGoods() { return m_SyntsGoods; }	//获取合成物品结构体对象
// // 
// // 
// // 	//--------------------------------------------------------------------------------------
// // 	//好友部分
// // 	//--------------------------------------------------------------------------------------
// // public:
// // 
// // private:
// // 	SPanel		m_PanelGroup[MAXNUM_PANELS];
// // public:
// // 	//获取分组在服务器端id
// // 	int			GroupIDtoPanelID(int groupid)	     { return groupid/10-1; }
// // 	SPanel      &GetSPanle(int spanelid)			 { return m_PanelGroup[spanelid]; }
// // 
// // 	void		SetLinkManPanleData(tagLinkman *data, int spanelid);
// // 	tagLinkman	*GetLinkManPanleDataForName(int spanelid, const string &strName);
// // 	void		DelteLinkManPanleData(int spanelid,  const string &strName);
// // 	//通过传入的名字来查找是否该玩家已经添加过这个人物，从而用来判断Lua下是否该发送相关AutoMsg
// // 	bool		isLinkManRepeatForName(const string &strName); 
// // 	void		ClearLinkmanData();
// // 
// // 	//------------------------------------------------------------------------------------------
// // 	//玩家交易 新加
// // 	//-------------------------------------------------------------------------------------------
// // public:
// // 
// // 
// // private:
// // 
// // public:
// // 	bool IsRequestTrade()				{ return m_bRequestTrade; }
// // 	void SetRequestTrade(bool b)		{ m_bRequestTrade = b; }
// // 	bool IsCanTrade()					{ return m_bCanTread; }
// // 	void SetCanTrade(bool b)			{ m_bCanTread = b; }
// // 
// // 	void AddMyTradeGoods(CGoods* pGoods,long lPos,long lSocketID);
// // 	void AddYouTradeGoods(CGoods* pGoods,long lPos);
// // 	//void ClearTradeGoods();
// // 
// // 	const CGUID		&GetTradeRequestGUID()					{return m_TradeRequestGUID;}	//获得交易发起方的GUID(作为LUA接口)
// // 	void			SetTradeRequestGUID(CGUID guid)			{m_TradeRequestGUID = guid;}	//设置交易发起方的GUID(处理S2C消息时调用)
// // 
// // 	const CGUID		&GetTradeSessionID()					{return m_TradeSessionGUID;}	//获得交易时的SESSIONID(作为LUA接口)
// // 	void			SetTradeSessionID(CGUID &guid)			{m_TradeSessionGUID = guid;}	//设置交易时的SESSIONID(处理S2C消息时调用)
// // 
// // 	const CGUID		&GetMyID()								{return m_MyID;}	//获得自己的GUID(作为LUA接口)
// // 	void			SetMyID(CGUID &guid)					{m_MyID = guid;}	//设置自己的GUID(处理S2C消息时调用)
// // 	const CGUID		&GetYourID()							{return m_YourID;}	//获得对方的GUID(作为LUA接口)
// // 	void			SetYourID(CGUID &guid)					{m_YourID = guid;}	//设置对方的GUID(处理S2C消息时调用)
// // 
// // 	long			GetlMPlugID()							{return m_lMPlugID;}	//获得自己的交易时的面板ID(作为LUA接口)
// // 	void			SetlMPlugID(long l)						{m_lMPlugID = l;}		//设置自己的交易时的面板ID(处理S2C消息时调用)
// // 	long			GetlYPlugID()							{return m_lYPlugID;}	//获得对方交易时的面板ID(作为LUA接口)
// // 	void			SetlYPlugID(long l)						{m_lYPlugID = l;}		//设置对方交易时的面板ID(处理S2C消息时调用)
// // 	bool			GetbMeIsLocked()						{return m_bMeIsLocked;}	
// // 	void			SetbMeIsLocked(bool b)					{m_bMeIsLocked = b;}	
// // 	bool			GetbYouIsLocked()						{return m_bYouIsLocked;}//获得对方是否已经交易锁定(作为LUA接口)
// // 	void			SetbYouIsLocked(bool b)					{m_bYouIsLocked = b;}	
// // 	bool			GetTradeResult()						{return m_bResult;}		//获得交易的结果(作为LUA接口)
// // 	void			SetTradeResult(bool b)					{m_bResult = b;}		//设置交易的结果(处理S2C消息时调用)
// // 
// // 	//--------------------------------------------------------------------------------------------------
// // 	//NPC商店交易
// // 	//--------------------------------------------------------------------------------------------------
// // private:
// // public:
// // 	void SetShopNPCGuid(const CGUID &guid)	{m_ShopNPCGuid = guid; }	//设置当前交易的NPC的GUID(处理S2C时调用)
// // 	CGUID &GetShopNPCGuid()				{return m_ShopNPCGuid; }	//获取当前交易的NPC的GUID(作为LUA接口)
// // 	void SetGoodsNum(DWORD num)			{m_dwGoodsNum = num;}		//设置当前一共有多少种物品可以买(处理S2C时调用)
// // 	void AddShopGoodsID(DWORD goodsid)  {m_ShopGoodsID.push_back(goodsid);}		//把所有可以买的物品的id保存(处理S2C时调用)
// // 	void AddEveryGoodsNum(DWORD num)	{m_EveryGoodsNum.push_back(num);}		//把每种可以买的物品的数量保存(处理S2C时调用)
// // 	DWORD GetBuyGoodsID();		//随机获取一个准备购买的物品的ID(作为LUA接口)
// // 	DWORD GetEveryGoodsNum()			{return m_EveryGoodsNum.at(m_RandomNum);}	//获得每种物品可以购买的数量(作为LUA接口)
// // 
// // 
// // private:
// // 
// // 	//----------------------------------------------------------------------------------------------------------------
// // 	//邮件相关
// // 	//----------------------------------------------------------------------------------------------------------------
// // 
// // public:	
// // 
// // 
// // 
// // private:
// // 
// 
	//////////////////////////////////////////////////////////////////////////
	// 领奖 [11/3/2009 Square]
	//////////////////////////////////////////////////////////////////////////
public:
 	void AddBounsInfo(tagBounsInfo* info);  
 	void QuestGetBouns(long lSocket);
  	void OnGetBounsResult(const CGUID & guid,bool bSucced);
	void DecodeGeniusFromDataBlock(DBReadSet& setReadDB);
private:
	list<tagGtBounsInfo*>	m_listGtBounsInfo;
// // 
// // 	// For DriveClient
// // 	// 玩家操作界面后，界面的状态
// // 	// by MartySa 2009.2.11
// // 
// // 
// // protected:
// // public:
// // 	bool		GetCharacterPageState()					    { return m_bCharacterPageState; }
// // 	bool		GetSkillPageState()						    { return m_bSkillPageState; }
// // 	bool		GetDepotUIPageState()					    { return m_bDepotUIPageState; }
// // 	bool		GetMailPageState()						    { return m_bMailPageState; }
// // 	bool		GetFriendsUIPageState()					    { return m_bFriendsUIPageState; }
// // 	bool		GetTeamPageState()						    { return m_bTeamPageState; }
// // 	bool		GetItemPageState()						    { return m_bItemPageState; }
// // 	bool		GetSubBackUIPageState()					    { return m_bSubBackUIPageState; }
// // 
// // 	void		SetCharacterPageState(bool state)			{ m_bCharacterPageState = state; }
// // 	void		SetSkillPageState(bool state)				{ m_bSkillPageState = state; }
// // 	void		SetDepotUIPageState(bool state)				{ m_bDepotUIPageState = state; }
// // 	void		SetMailPageState(bool state)				{ m_bMailPageState = state; }
// // 	void		SetFriendsUIPageState(bool state)			{ m_bFriendsUIPageState = state; }
// // 	void		SetTeamPageState(bool state)				{ m_bTeamPageState = state; }
// // 	void		SetItemPageState(bool state)				{ m_bItemPageState = state; }
// // 	void		SetSubBackUIPageState(bool state)			{ m_bSubBackUIPageState = state; }
// // 
// // 	// 答题
// // 	// By MartySa 2009.6.15
// // private:
// // 
//  public:
//  	bool		GetIsAnswerQuestion()	    { return bIsAnswerQuestion; }
//  	void		SetIsAnswerQuestion(bool b)	{ bIsAnswerQuestion = b;}
// 
// 
// 	//玩家选择一个怪物后保存的 GUID
// private:
// 
// public:
// 	CGUID		&GetSelectedShapeGuid()						{ return m_SelectedShapeGuid; }
// 	void		SetSelectedShapeGuid(CGUID guid)			{ m_SelectedShapeGuid = guid; }
// 
// 	//取得玩家当前身上背包里Goods总数 For DriveClient
// 	// by MartySa 2009.2.19
// public:
// 	long		GetPlayerPackGoodsSum();
};