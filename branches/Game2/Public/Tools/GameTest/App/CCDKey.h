
//--------------------------------------------------
//brief:将玩家身上某一个类别的数据写入本地磁盘后
//用于在下一次和服务器传过来的数据进行比较,
//由于网络延迟,程序退出时未将消息队列消息处理完等情况的存在
//所以主要用于简单的数据判断,比如物品总的数量---物品是否消失
//  [7/5/2008 Square]
//modify :09/1/5

#pragma once

#include <string.h>
#include <vector>
#include "Player.h"
#include <stdarg.h>
#include <stdio.h>
using namespace std;

enum eCompareGoodsError{
	ONE_INVALID,
	TWO_INVALID,
	RESULT_NO_ERROR,
	RESULT_ERROR,
};
//////////////////////////////////////////////////////////////////////////
#define DEPOT	1
#define PACKET	0
#define GOLD	1
#define SILVER	0
//////////////////////////////////////////////////////////////////////////
class CCDKey{
public:
	CCDKey(const char* strCDKey);

	~CCDKey(void);
public:
	enum eSaveDataType
	{
		eSAVE_GOODS,
		eSAVE_MAIL,
		eSAVE_SKILL,
		eSAVE_QUEST,
		eSAVE_LINKMAN,
		eSAVE_FACTION,
		eSAVE_LEVEL,
		eSAVE_EQUIP,
	};

	enum eContainerID
	{
		//装备
		EQUIP_ITEM,
		//背包
		MAIN_PACK,
		MAIN_PACK_ITEM,
		MAIN_PACK_ITEM_PACK_1,
		MAIN_PACK_ITEM_PACK_2,
		MAIN_PACK_ITEM_PACK_3,
		MAIN_PACK_ITEM_PACK_4,
		MAIN_PACK_ITEM_PACK_5,
		//仓库
		DEPOT_MAIN_PACK,
		DEPOT_Def_SubPACK,
		DEPOT_ITEM,
		DEPOT_ITEM_PACK_1,
		DEPOT_ITEM_PACK_2,
		DEPOT_ITEM_PACK_3,
		DEPOT_ITEM_PACK_4,
		//
		CONTAINER_NUM,
	};

	struct tagCDKeyGoodsInfo 
	{
		//		CGUID guid;				//物品guid
		eContainerID containerID;	//物品所在容器ID
		DWORD dwPos;			//所在容器坐标
		WORD  wNum;				//同一坐标上的数量
		//
		CGoods* pGoods;
		tagCDKeyGoodsInfo(void)
		{
			containerID = (eContainerID)-1;
			dwPos = -1;
			wNum = -1;
			pGoods = NULL;
		}
	};

	hash_map<CGUID,tagCDKeyGoodsInfo,hash_guid_compare>	m_mapContainerID[CONTAINER_NUM];
	typedef hash_map<CGUID,tagCDKeyGoodsInfo,hash_guid_compare>::iterator hashGoodsIter;

	struct tagCDKeyEquipInfo 
	{
		DWORD dwItemPos;
		DWORD dwGoodsIndex;
		char szGuid[128];
		tagCDKeyEquipInfo(void)
		{
			dwItemPos = -1;
			dwGoodsIndex = -1;
			ZeroMemory(szGuid,128);
		}
	};


	//	void AddPlayerDataExAmount(CPlayer* pPlayer);
	//	void AddDepotData(CPlayer* pPlayer);


	// 重新整理的接口 [1/5/2009 Square]
	static void InitCDKeyList(const char* filename);			//读CDKeyList.ini文件
	static void InitCDKeyData(CCDKey* pCDKey);					//读取磁盘上的cdkey数据	cdkey对应了唯一的一个player

	static CCDKey* GetCDKey(const char* strCDKey);				//得到一个strCDKey的CDKey对象
	static BOOL FindCDKey(const char* strCDKey);				//strCDKey是否存在
	static void AddCDKey(CCDKey* pCDKey);						//添加一个cdkey对象

	void AddPlayerData(CPlayer* pPlayer,eSaveDataType eType);	//将玩家eType类别的数据写到磁盘
	void CheckPlayerData(CPlayer* pPlayer,eSaveDataType eType);	//检查玩家eType类别的数据

	void SaveGoodsData(CPlayer* pPlayer);						//保存player身上的物品数据
	void SaveGoodsCounts(CGoods* pGoods);						//物品数量
	void CheckGoods(CPlayer* pPlayer);

	void SaveSkill(CPlayer* pPlayer);
	void SaveQuest(CPlayer* pPlayer);
	void CheckSkill(CPlayer* pPlayer);
	void CheckQuest(CPlayer* pPlayer);

	void SaveLevel(CPlayer * pPlayer);
	void CheckLevel(CPlayer * pPlayer);

	void SaveEquip(CPlayer* pPlyaer);
	void CheckEquip(CPlayer* pPlayer);

	//private:
	map<DWORD,DWORD>	m_mapGoodsAmountByIndex;

	static void Release();

	//
	void WriteOutInfo(std::ofstream & out_f,const char* strOutInfo);
private:
	hash_map<CGUID,CGoods*,hash_guid_compare> m_mapGoodsSaveInfo;
	map<DWORD,BOOL>	m_mapQuestInfo;
	map<DWORD,BOOL>	m_mapSkillInfo;

	map<DWORD,tagCDKeyEquipInfo*>	m_mapCDKeyEquipInfo;

	//-----------------------------------------------------------------------------------------
	// 对玩家身上的邮件进行数据保存和核对
	// by MartySa 2009.1.5
	//-----------------------------------------------------------------------------------------
private:
	std::vector<stMailData> m_vecSaveMailData;							//保存收到的mail数据

public:
	void	AddSaveMailDataByTxt(CPlayer *pPlayer);						//添加保存邮件数据从文本中
	void	ClearAllSaveMailData();										//清空保存邮件所有数据

	void	SavaMailData(CPlayer *pPlayer);								//保存邮件数据
	void	CheckMailData(CPlayer *pPlayer);							//检查邮件数据
	void    PutCheckMailErr(const char *msg);	

	//字符转换功能函数
	string GetWriteCString(int cstrSize, ... );						//取得组合CString
	string  GetWirteString(int strSize, ... );							//取得组合string
	//end save&check mail
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// 对玩家身上的好友进行数据保存和核对
	// by MartySa 2009.1.12
	//-----------------------------------------------------------------------------------------
private:
	SPanel		m_SaveLinkManPanelGroup[MAXNUM_PANELS];

public:
	void	AddSaveLinkManPanleGroupByTxt(CPlayer *pPlayer);					//添加保存好友数据从文本中
	void	ClearLinkManAllSavePanleGroup(CPlayer *pPlayer);					//清空保存好友数据和本身好友数据
	void	SaveLinkManPanleGroup(CPlayer *pPlayer);							//保存好友数据到文本
	void	CheckSaveLinkManPanleGroup(CPlayer *pPlayer);						//检查好友数据
	void	PutCheckLinkManErr(const char *msg);								//输出检测到错误信息
	//需调用到 mail的字符串转换功能函数

	//end save&check linkman
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// 对玩家身上的行会进行数据保存和核对
	// by MartySa 2009.1.13
	//-----------------------------------------------------------------------------------------
public:
	//自定义一个结构体作为保存行会数据的tag
	struct tagSaveFaction 
	{
		CGUID						FamilyGuid;								//家族guid
		string						strFamilyName;							//家族名字
		string						strFamilyTitle;							//家族称号
		CGUID						FamilyMasterGuid;						//家族族长guid
		long						lFamilyJobLevel;
		CGUID						UnionGuid;								//自己所在同盟GUID
		set<CGUID, guid_compare>	EnemyFamily;							//敌对家族
		set<CGUID, guid_compare>	CityWarEnemyFamily;						//城战敌对家族
		list<tagownedReg>	listOwnRgn;								//家族拥有的场景列表
		bool						bInContendState;						//是否争夺状态
		bool						bCityWarDied;							//城战死亡状态

		tagSaveFaction() : FamilyGuid(CGUID::GUID_INVALID), FamilyMasterGuid(CGUID::GUID_INVALID),
			lFamilyJobLevel(0), UnionGuid(CGUID::GUID_INVALID), 
			bInContendState(false), bCityWarDied(false)
		{}
	};
private:
	tagSaveFaction		m_SaveFaction;

public:
	void	AddSaveFactionByTxt(CPlayer *pPlayer);					//添加保存行会数据从文本中
	void	ClearSaveFaction(CPlayer *pPlayer);						//清空保存行会数据和本身行会数据
	void	SaveFaction(CPlayer *pPlayer);							//保存行会数据到文本
	void	CheckFaction(CPlayer *pPlayer);							//检查行会数据
	void	PutCheckFactionErr(const char *msg);					//输出检测到错误信息
	//
	//需调用到 mail的字符串转换功能函数

	//end save&check Faction 
	//-----------------------------------------------------------------------------------------

public:
	struct tagCDKeyDepotPack 
	{
		hash_map<CGUID,tagGoods,hash_guid_compare> mapGoods;
		long	lContainerID;				//ID-----主仓库/默认子背包/子背包/(金币/银币栏位)等等
		long	lGoodsAmount;				//数量
		long	lCubage;					
		tagCDKeyDepotPack() : lContainerID(-1), lGoodsAmount(0), lCubage(0)
		{}
	};
private:
	string m_strCDKey;
	char szPlayerGuid[256];

private:
	//静态 管理所有cdkey
	static hash_map<string,CCDKey*> m_mapCDKeyList;
	//Player基本属性
	tagPlayerProperty m_PlayerBaseProperty;
	//商业声望
	hash_map<DWORD,DWORD> m_mapTradeCredit;
	//player技能
	hash_map<DWORD,tagPlayerSkill>	m_mapSkillEx;
	//仓库
	CGUID	m_DepotMoneyGuid;
	CGUID	m_DepotSilverGuid;
	long	m_DepotMoneyAmount;
	long	m_DepotSilverAmount;
	//背包
	CGUID	m_MoneyGuid;
	CGUID	m_SilverGuid;
	CGUID	m_BaoShiGuid;
	CGUID	m_JiFenGuid;
	long	m_MoneyAmount;
	long	m_SilverAmount;
	long	m_BaoShiAmount;
	long	m_JiFenAmount;

	long	m_lLevel;

	void SetLevel(long l)	{ m_lLevel = l; }

};