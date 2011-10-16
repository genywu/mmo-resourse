//EntityManager.h/////////////////////////////////////////////////////////////////////
//对象类:该对象管理所有的对象实例和对象的配置信息数据以及与DBS的会话管理
//Author:安海川
//Create Time:2008.11.03
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Account.h"

class CCountry;
class CGoods;
class CEntityGroup;

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
	void DecodeFromByteArray(BYTE* buf, long& pos, long bufSize=-1);
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
	string							strPrimarykey;		// 主键值名
	long							lHhasAttrFlag;		// 是否有属性
	long							lLeafComType;		// 叶子类型
	string							strLeafComFlag;		// 叶子名字
	long							lHasDetailLeaves;	// 是否有分散叶子节点(类似goodsgroup)
	string							strDbQueryName;		// 数据库查找变量名
	long							lDbQueryType;		// 数据库查找变量类型
	vector<string>					vecIndexStr;		// 数据表索引名
	map<string, tagEntityAttrInfo>	pDataMgr;			// 属性数据
	map<string, tagBuildFileInfo*>	mapBuildFileInfo;	// 结构数据
	BYTE							pQueryVar[256];		// 数据库查找变量属性信息

	void DecodeFromByteArray(BYTE* buf, long& pos, long bufSize);
};

class CEntityManager
{
public:
	CEntityManager();
	~CEntityManager();

	// EntityMap
	typedef std::map<string, std::map<CGUID, CEntityGroup*>> DBEntityComponentMap;
	typedef DBEntityComponentMap::iterator DBEntityComponentMapItr;

	typedef std::map<std::string, Account*> AccountMap;
	typedef AccountMap::iterator AccountMapItr;

	typedef std::map<std::string, tagEntityBuildInfo*> AttrDefMap;
	typedef AttrDefMap::iterator AttrDefMapItr;

	void ProcessMsg(CMessage* msg);

	//-------------- interfaces
	// CBaseEntity
	// 添加一个BaseEntity到Map末尾
	void AddOneRootEntity(CEntityGroup*entity);
	// 删除一个BaseEntity
	void DelOneRootEntity(CEntityGroup*entity);
	// 删除一个BaseEntity
	void DelOneRootEntity(const string& comFlag, const CGUID& playerID);
	// 清空链表
	void ClearRootEntityMap(void);

	DBEntityComponentMap& GetBaseEntityMap(void) { return m_mapEntitys; }
	CEntityGroup*FindBaseEntity(string, const CGUID& guid);

	// 产生ID
	bool GenerateGUID(CGUID& guid);

	// 从消息取得操作对象
	CEntityGroup*FindRootEntity(const string& flag, const CGUID& guid);
	// 创建对象
	CEntityGroup* CreateRootEntity(const string& flag, const CGUID& guid);

	AccountMap&	GetAccountMap(void) { return m_mapAccounts; }
	// 添加一个Account到Map末尾
	bool AddAccount(Account* acc);
	// 删除一个Account
	void DelAccount(Account* acc);
	Account* FindAccount(const std::string& strCdkey);
	
	//interface--DB对象和游戏对象转换接口
	void CDBCountryToCCountry(CBaseEntity* pEntity, CCountry* country);
	void CCountryToCDBCountry(CCountry* country, CBaseEntity* pEntity); 

	void CDBLoginPlayerToCPlayer(CEntityGroup* pEntity, CPlayer* player);
	void CPlayerToCDBLoginPlayer(CPlayer* player, CEntityGroup* pEntity); 

	void CDBPlayerToCPlayer(CEntityGroup* pEntity, CPlayer* player);
	void CPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pEntity); 

	void CDBGoodToCGoods(CBaseEntity* pBaseEntity, CGoods* goods, const CGUID& ownerID);
	void CGoodsToCDBGood(CGoods* goods, CBaseEntity* pBaseEntity, const CGUID& ownerID);

	void CPlayerFriendToCDBFriendGroup(CPlayer* player, CEntityGroup*pBaseEntityOwner, CEntityGroup* pBaseEntityAim); 
	void CDBFriendGroupToCPlayerFriend(CPlayer* player, CEntityGroup*linkmanGroupOwner, CEntityGroup*linkmanGroupAim); 
	
	void CDBRegionToCRegion(CEntityGroup* pBaseEntity, CWorldRegion* pRegion);
	void CRegionToCDBRegion(CWorldRegion* pRegion, CEntityGroup* pBaseEntity); 
	
	void CDBGlobalVarGroupToCGlobalVar(CEntityGroup* pBaseEntity);
	void CGlobalVarGroupToCGDBlobalVar(CEntityGroup* pBaseEntity);

	void CDBPhRgnToCPhRgn(CEntityGroup* pBaseEntity, CWorldRegion* pRegion);
	void CPhRgnToCDBPhRgn(CWorldRegion* pRegion, CEntityGroup* pBaseEntity);

	void CFactionToCDBFaction(CFaction* fac, CEntityGroup* pBaseEntity);
	void CDBFactionToCFaction(CEntityGroup* pBaseEntity, CFaction* fac);

	void CUnionToCDBUnion(CUnion* pUnion, CEntityGroup* pBaseEntity);
	void CDBUnionToCUnion(CEntityGroup* pBaseEntity, CUnion* pUnion);
	void CDBMailToCMail(CEntityGroup* pEntity, CMail* pMail);
	void CMailToCDBMail(CMail* pMail, CEntityGroup* pEntity); 

	void CLimitGoodsRecordToCDBRecord(CEntityGroup* pEntity);
	void CDBLimitGoodsRecordToCRecord(CEntityGroup* pEntity);
	void CDBMailGoodsToCGoods(CBaseEntity* dbGood, CGoods* good, const CGUID& ownerID);
	void CGoodsToCDBMailGoods(CGoods* pGoods, CBaseEntity* pDBMailGoods, const CGUID& ownerID);

	void CDBPhRgnGroupToCPhRgnGroup(CEntityGroup* pPhRgnGroup);
	void CPhRgnGroupToCDBPhRgnGroup(CEntityGroup* pPhRgnGroup);
	// 分层转换 CDBPlayer 到 CPlayer
	void DetailPropertyCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
    void DetailGoodsCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer, PLAYER_EXTEND_ID cID);
	void DetailEquipmentCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailOrignalPackCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailSubpackCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* player, long subPackPos);
	void DetailWalletCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailSilverCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailQuestCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailMerQuestCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailSpriteCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* player);
	void DetailSkillCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
    void DetailBrotherCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailStateCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailVarDataCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailMedalCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailAreaCreditCDBPlayerToCPlayer	(CEntityGroup* pBaseEntity, CPlayer* player);
	void DetailLimitGoodsRecordCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* player);
	void DetailIncGoodsCDBPlayerToCPlayer(CEntityGroup* pBaseEntity, CPlayer* pPlayer);
	void DetailIncTradeRecordCDBPlayerToCPlayer( CEntityGroup* pBaseEntity, CPlayer* playe );
	void DetailOrignalDepotCDBPlayerToCPlayer	(CEntityGroup* pBaseEntity, CPlayer* player);
	void DetailSubDepotCDBPlayerToCPlayer		(CEntityGroup* pBaseEntity, CPlayer* player, long subpackPos);
	void DetailDepotWalletCDBPlayerToCPlayer	(CEntityGroup* pBaseEntity, CPlayer* player);
	void DetailDepotSilverCDBPlayerToCPlayer	(CEntityGroup* pBaseEntity, CPlayer* player);
	//void DetailSpriteCPlayerToCDBPlayer		(CEntityGroup* pBaseEntity, CPlayer* player); 
	void DetailSysMailCDBPlayerToCPlayer	(CEntityGroup* pBaseEntity, CPlayer* player);

	// 分层转换 CPlayer 到 CDBPlayer
	void DetailSpriteCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity); 
	void DetailPropertyCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailEquipmentCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
    void DetailEquipGodCPlayerToCDBPlayer(CPlayer* pPlayer, CEntityGroup* pBaseEntity);
    void DetailEquipLandCPlayerToCDBPlayer(CPlayer* pPlayer, CEntityGroup* pBaseEntity);
    void DetailEquipHumanCPlayerToCDBPlayer(CPlayer* pPlayer, CEntityGroup* pBaseEntity);
	void DetailOrignalPackCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailSubpackCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity, long subpackPos);
	void DetailWalletCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailSilverCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailQuestCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailMerQuestCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);//
	void DetailSkillCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
    void DetailBrotherCPlayerToCDBPlayer(CPlayer* pPlayer, CEntityGroup* pBaseEntity);
	void DetailStateCPlayerToCDBPlayer		(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailVarDataCPlayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailMedalCPlayerToCDBPlayer		(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailAreaCreditCPlayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailLimitGoodsRecordCPlayerToCDBPlayer(CPlayer* player,CEntityGroup* pBaseEntity);
	void DetailIncTradeRecordCPlayerToCDBPlayer( CPlayer* player,CEntityGroup* pBaseEntity );
	void DetailGoodsCooldownCPlayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailIncGoodsCPlayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailDeOccuCPlayerToCDBPlayer		(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailSysMailCplayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity);

	void DetailDepotCPlayerToCDBPlayer(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailSubDepotCPlayerToCDBPlayer	(CPlayer* player, CEntityGroup* pBaseEntity, long subpackPos);
	void DetailDepotWalletCPlayerToCDBPlayer		(CPlayer* player, CEntityGroup* pBaseEntity);
	void DetailDepotSilverCPlayerToCDBPlayer		(CPlayer* player, CEntityGroup* pBaseEntity);

	/// 商业背包到DBS的编解码
	void DetailBusinessPackCPlayerToCDBPlayer( CPlayer *player, CEntityGroup *pBaseEntity );
	void DetailBusinessPackCDBPlayerToCPlayer( CEntityGroup *pBaseEntity, CPlayer *player );

    /// 天赋编解码
    void DetailGeniusDataCPlayerToCDPlayer( CPlayer *player, CEntityGroup *pBaseEntity );
    void DetailGeniusDataCDPlayerToCPlayer( CEntityGroup *pBaseEntity, CPlayer *player );

	bool LoadPlayerPet(CEntityGroup* pDBPlayer,
		CPlayer* pPlayer);						// 加载玩家宠物列表
	bool SavePlayerPet(CPlayer* pPlayer,
		CEntityGroup* pDBPlayer);				// 存储玩家宠物列表

	//------玩家对象操作工具接口--------
	// 清空玩家装备物品map
	void ClearPlayerContainerMap(CEntityGroup* pBaseEntity, PLAYER_EXTEND_ID containerID);
	// 清除玩家子背包对象
	void ClearPlayerSubpackSelfMap(CEntityGroup* pBaseEntity, long subPos);
	// 清除玩家仓库子背包对象
	void ClearPlayerSubDepotSelfMap(CEntityGroup* pBaseEntity, long subPos);
	// 根据PLAYER_EXTEND_ID取得Entity的类型值
	string GetComFlagByExtendID(PLAYER_EXTEND_ID containerID);
	// 添加一个物品到玩家容器
	void AddGoodsToPlayer(CEntityGroup* pBaseEntity, CGoods* pGoods, long pos, PLAYER_EXTEND_ID containerID);
	// 计算LoginPlayer的删除时间
	long ComputeLoginPlayerDelTime(CEntityGroup* pBaseEntity);
	// 设置玩家节点上某些子节点的数据库操作标志位
	void SetEntityLeavesDbOperFlag(CBaseEntity* pBaseEntity, const string& leafComFlag, bool operFlag);
	// 将玩家节点上所有子节点数据库操作标志位清零
	void ResetEntityAllDbOperFlag(CBaseEntity* pBaseEntity, bool flag = false);
	// 设置从根结点开始的所有节点数据库操作标志
	void SetEntityAllDbOperType(CBaseEntity* pBaseEntity, DB_OPERATION_TYPE opType);
	// 根据叶子类型在其根结点下查找其父节点
	CEntityGroup* FindLeafFatherByComFlag(CEntityGroup* pRoot, const string& leafComFlag);
	// 根据叶子类型在其根结点下查找其节点
	CBaseEntity* FindLeafByComFlag(CEntityGroup* pRoot, const string& leafComFlag);
	
	// 映射属性字符串名和枚举值名
	void AddAttrEnumAndStr(const string& strName, long lEnum);
	long GetAttrEnumByStr(const string& strComFlag, const string& strName);
	const string& GetAttrStrByEnum(long lEnum);
	// 取得配置文件的数据信息
	tagEntityBuildInfo* GetEntityBuildInfo(const string& strComFlag);
	// DATA_OBJECT_TYPE
	long GetDataObjectType(const string& strComFlag, const string& strName);
	long GetDataObjectType(const string& strComFlag, long lEnum);
	// 数据库属性标志位: 1|0: 1(数据库自动操作) 0程序操作(默认)
	BYTE GetDbUseageFlag(const string& strComFlag, const string& strName);
	BYTE GetDbUseageFlag(const string& strComFlag, long lEnum);
	// 数据块最大长度
	long GetBufMaxSize(const string& strComFlag, const string& strName);
	long GetBufMaxSize(const string& strComFlag, long lEnum);
	
	long GetAttrEnumValue(void) { return m_lAttrEnumValue; }

	//! 需要手动释放该接口分配的内存
	CBaseEntity*	NewBaseEntity(const string& strComFlag, const CGUID& guid);
	//! 对象内存回收接口
	void			DelBaseEntity(CBaseEntity* pEntity);

	//! 输出对象类型及个数
	void			OutEntityInfo(VOID);
public:
	std::map<std::string/*attrname*/, tagEntityBuildInfo*>& GetObjAttrDef(void)		{ return m_mapObjAttrDef; }

	void CreateChangeNameProcSession(const CGUID& guid, const char* szName, long gsscoketid);

	void CreateSaveAccountSession(Account* pAcc);
	void CreateLoadAccountSession(Account* pAcc);
	void CreateLoadCountrySession(void);
	BOOL CreateDelFactionSession(const CGUID &FactionGuid);
	void CreateDelUnionSession(const CGUID &UnionGuid);
	void CreatePlayerCreateSession(const char* szCdkey, const char* szName, BYTE nOccu,
		BYTE sex,BYTE nHead, BYTE nFace, BYTE btCountry, BYTE btMaxCharactersNum,
		BYTE byConstellation,bool bSelectRecommCountry);
	void CreateLoadPlayerDataSession(const char* szCdkey, const CGUID& guid);
	void CreateSaveLoginPlayerDataSession(const CGUID& guid);
	void CreateSavePlayerDataSession(const CGUID& guid, long flag/*SAVE_DETAIL_ATTR_TYPE*/);
	void CreateLoadFactionGroupSession(void);
	void CreateLoadUnionGroupSession(void);
	void CreateSaveFactionSession(vector<CEntityGroup*> &vCEntityGroup);
	void CreateSaveUnionSession(CEntityGroup* pUnion);
	void CreateInsertFactionSession(CEntityGroup* pFaction);
	void CreateInsertUnionSession(CEntityGroup* pUnion);
	void CreateLoadRegionSession(void);
	void CreateSaveRegionSession(CEntityGroup* pRegion);
	void CreateCreateRegionSession(CEntityGroup* pRegion);
	void CreateLoadPhRgnSession(void);
	void CreateSavePhRgnSession(CEntityGroup* pPhRgn);
	void CreateLoadGlobalVarSession(void);
	void CreateSaveGlobalVarSession(void);
	
	void CreateSaveBusinessSession();
	void CreateLoadBusinessSession();

    void CreateLoadAgentOrdersSession();
    void CreateSaveAgentOrdersSession();

	void CreateSaveLimitGoodsRecordSession();
	void CreateLoadLimitGoodsRecordSession();
	// 更新玩家的删除时间
	void CreateUpdateLoginPlayerDelTimeSession(const char* szCdkey, const CGUID& guid);
	// 恢复玩家的删除时间
	void CreateRestoreLoginPlayerDelTimeSession(const char* szCdkey, const CGUID& guid);

	void CreateBanPlayerSession(const char* szName, long time);
	
	// 玩家初始化读取信件
	void CreateInitLoadMailSession(const CGUID& guid);
	// 玩家添加信件
	void CreateInsertMailSession(CMail *Mails);
	// 在线玩家保存信件
	void CreateSaveOnlineMailSession(const CGUID& ownerID, vector<CMail*> &mails);
	// 在线玩家删除信件
	void CreateDeleteOnlineMailSession(const CGUID& ownerID, vector<CMail*> &mail);
	// 非在线玩家添加信件
	//void CreateInsertOfflineMailsSession(const char* szOwnerName, CMail* pMail);
	// 系统群发信件
	void CreateInsertGroupMailSession(const char* szOwnerName, long level, long sex, vector<long>& countryIDVec);
	// 系统删除非在线玩家信件
	void CreateSystemDeleteOfflineMailsSession(long lCurTime);
	// 发送给玩家加钱(付款新建的钱)
	void CreateAddOfflinePlayerCostMailGoldProcSession(const char* szPlayerName, long goldNum);
	
	long GetDbPlayerNewNum(void) { return m_lNewDbPlayerNum; }
	void SetDbPlayerNewNum(long lNum) { m_lNewDbPlayerNum = lNum; }

private:
	DBEntityComponentMap		m_mapEntitys;
	AccountMap					m_mapAccounts;

	// Obj属性信息,Obj对象的属性名,id
	std::map<std::string, tagEntityBuildInfo*> m_mapObjAttrDef;

	long m_lAttrEnumValue;
	// 映射属性字符串名和枚举值名
	map<long, string> m_mapAttrEnumToStr;

	long			m_lNewDbPlayerNum;
};

	extern std::string NULL_STRING;
#endif//DB_ENTITY_MANAGER_H