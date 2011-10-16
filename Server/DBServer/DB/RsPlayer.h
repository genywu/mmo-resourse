#pragma once
#include "..\myadobase.h"
#include "RSGoods.h"
#include "Largess.h"
class CDBPlayer;
class CDBLoginPlayer;
class CMessage;
class CDBGoods;
class CPlayerRanks;
struct LoadThreadData;
class CDBLinkmanGroup;


class CRsPlayer :
	public CMyAdoBase
{
public:

	CRsPlayer(void);
	virtual ~CRsPlayer(void);

	bool ValidatePlayerIDInCdkey(LPCSTR szCdkey, const CGUID& nID, _ConnectionPtr& cn);

	BYTE GetPlayerCountInCdkey(LPCSTR szCdkey);
	BYTE GetPlayerCountInDBbyCdkey(LPCSTR szCdkey,_ConnectionPtr& cn);

	bool IsNameExist(LPCSTR szName);

	CGUID GetPlayerExID(const char* strName);

	void GetPlayerNameByID(IN const CGUID& dwPlayerID, OUT char *szPlayerName);

	string GetCDKey(const char* strName);

	bool LoadPlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn);//读玩家属性

	bool StatRanks(CPlayerRanks* pPlayerRanks,_ConnectionPtr& cn);			//统计玩家排行榜


	// 开打登录角色信息
public:
	bool OpenPlayerBase(LPCSTR szCdkey, std::list<LoadThreadData*>& loadDataList, _ConnectionPtr& cn);//打开玩家基本属性
private:
	bool OpenPlayerBaseInDB(LPCSTR szCdkey, std::list<LoadThreadData*>& loadDataList, long &cnt, _ConnectionPtr& cn);//打开玩家基本属性



	// check list and database ,get resault of frist empty location (ranger 1-3, 0 no location)
	//BYTE GetFristEmptyLocation(LPCSTR szCdkey);
	//////////////////////////////////////////////////////////////////////////
	// 创建角色
	//////////////////////////////////////////////////////////////////////////
public:
	bool CreatePlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn);	// save thread call
private:
	bool CreatePlayerBase(CDBLoginPlayer *pPlayer, _ConnectionPtr& cn);
	bool CreatePlayerAbilities(CDBPlayer *pPlayer, _ConnectionPtr& cn);
	//////////////////////////////////////////////////////////////////////////

public:
	// 逻辑删除一个角色
	bool DeletePlayer(const CGUID& nID, time_t tDelDate, _ConnectionPtr& cn);	// save thread call

	bool RestorePlayer(const CGUID& nPlayerID, _ConnectionPtr& cn);	// save thread call

	// 获取标记了删除的角色的删除时间
	time_t GetPlayerDeletionDate(const CGUID& nID, _ConnectionPtr& cn);
	time_t GetPlayerDeletionDateByRs(const CGUID& nID, _RecordsetPtr& rs);
	// 设置登录时间,在WS发送MSG_W2DB_PASS_LOG_PLAYERBASELIST消息时处理
	void UpdateLogoutTime(const CGUID& nID, WORD* szLoginTime, _ConnectionPtr& cn);
	// 获取标记了登录的角色的上次下线的时间(毫秒)
	time_t GetPlayerLastLogoutDate(const CGUID& nID, _ConnectionPtr& cn);
	//////////////////////////////////////////////////////////////////////////
	//存玩家属性
	//////////////////////////////////////////////////////////////////////////
public:
	// 保存玩家全部数据
	bool SavePlayer(CDBPlayer *pPlayer, _ConnectionPtr& cn);		// save thread call

	// 保存玩家基本属性
	bool SavePlayerBase(CDBLoginPlayer *pPlayer, _ConnectionPtr& cn);
	
	// 保存玩家普通能力属性(除技能,状态,好友名字,脚本数据等)
	bool SavePlayerAbilities(CDBPlayer *pPlayer, _ConnectionPtr& cn);
	//////////////////////////////////////////////////////////////////////////

public:
	// 预读策略
	void PreLoadPlayerBase(_ConnectionPtr& cn);
	// 预读策略
	void PreLoadPlayerAttrs(_ConnectionPtr& cn);

	// only check database
	BYTE GetLocationStateInCdkey(LPCSTR szCdkey);

	//存Buffer
	bool SaveBufferField(CDBPlayer *pPlayer, const char* szFiledName, _RecordsetPtr &pRs);
	//读Buffer
	bool LoadBufferField(CDBPlayer *pPlayer, const char* szFiledName, _RecordsetPtr &pRs);

	//读取小精灵数据
	bool LoadSpriteSystem(CDBPlayer* pPlayer, _ConnectionPtr &cn);
	//保存小精灵数据
	bool SaveSpriteSystem(CDBPlayer* pPlayer, _ConnectionPtr &cn);

	//读取玩家需要保存在服务器的设置
	bool LoadSetupOnServer(CDBPlayer* pPlayer, _ConnectionPtr &cn);
	//保存玩家需要保存在服务器的设置
	bool SaveSetupOnServer(CDBPlayer* pPlayer, _ConnectionPtr &cn);

	//读取联系人数据
	bool LoadLinkmanData(CDBPlayer* pPlayer, CDBLinkmanGroup* linkmanGroup, _ConnectionPtr cn);
	//保存联系人信息
	static bool SaveLinkmanData(CDBLinkmanGroup* linkmanGroup, _ConnectionPtr cn);

	// 根据玩家名字查找玩家对象
	bool CheckPlayerIsExist(const char* szName, _ConnectionPtr& cn);
};
