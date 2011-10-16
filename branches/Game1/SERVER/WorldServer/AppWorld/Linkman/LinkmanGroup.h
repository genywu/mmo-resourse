


/*
*	file:		LinkmanGroup.h
*	Brief:		好友组类
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-20
*/

#pragma once

#include "../../public/LinkmanGroupSetup.h"





///////////////////////////////////////////////////////////////////////////////////////////////////////
class LinkmanGroup
{
public:
	explicit LinkmanGroup(const LinkmanGroupSetup* pSetup);
	~LinkmanGroup(void);

	void									Release							(void);
public:
	//!										添加一个联系人
	BOOL									AddMember						(tagLinkman_DB &Linkman_DB);
	//!										删除一个联系人
	BOOL									DelMember						(const CGUID &PlayerGuid);
	//!										找到一个联系人
	CGUID									FindMember						(const char *pPlayerName);

	//!										成员地图改变
	void									MemberMapChange					(const CGUID &PlayerGuid, const char *pMapName);
	//!										成员等级改变
	void									MemberLevelChange				(const CGUID &PlayerGuid, LONG lLevel);
	//!										成员职业改变
	void									MemberOccuChange				(const CGUID &PlayerGuid, LONG lOccu);

	//!										添加一个添加我为联系人的人
	BOOL									AddCompriseMePlayer				(tagLinkman_DB &Linkman_DB);
	//!										删除一个添加我为联系人的人
	BOOL									DelCompriseMePlayer				(const CGUID &PlayerGuid);
	//!										找到一个添加我为联系人的人
	BOOL									FindCompriseMePlayer			(const CGUID &PlayerGuid);

	//!										一个联系人上线
	void									LinkmanLogin					(const CGUID &PlayerGuid);
	//!										一个联系人下线
	void									LinkmanLogout					(const CGUID &PlayerGuid);
	//!										联系人的删除状态改变
	void									LinkmanDelStateChange			(const CGUID &PlayerGuid, BOOL bDeleted);

	//!										更新联系人在线状态
	void									UpdateOnlineState				(void);
	//!										编码到消息
	void									Serialize_ForGs					(CMessage *pMsg);
	//!										从GS发过来的数据中解码
	void									Decode_FromGs					(BYTE *pData, LONG &lPos);
	//!										转换成员列表到参数
	void									Serialize_ForDBS				(vector<tagLinkman_DB> &vLinkman);
	//！									转换添加我的玩家ID列表到参数
	void									AddMeGuidToVector				(vector<tagLinkman_DB> &vLinkman_DB);

	//!										取得我添加的玩家
	const	map<CGUID, tagMeAddPlayer>&		GetMeAddMap						(void);
	//!										取得添加我的玩家
	const	map<CGUID, tagLinkman_DB>&		GetAddMeMap						(void);

	//!										得到已添加的人数
	LONG									GetMemberNum					(void);


private:
	//!										我添加的玩家
	map<CGUID, tagMeAddPlayer>				m_mapMeAdd;
	//!										添加我的玩家
	map<CGUID, tagLinkman_DB>				m_mapAddMe;

	//!										相关配置指针
	const LinkmanGroupSetup*				m_pSetup;


	//! 静态成员
	//!-----------------------------------------------------------------------------------------------
public:
	//!										更新数据状态
	static	void							UpdateLinkman_data				(tagLinkman_data* pLinkman_data);
private:
	//!										按要求创建一个tagLinkman_data对象
	static	tagLinkman_data*				CreateLinkman_data				(const CGUID &PlayerGuid, const char *pPlayerName);
	//!										释放一个tagLinkman_data对象
	static	void							ReleaseLinkman_data				(tagLinkman_data **ppLinkman_data);
};



//!			玩家的好友数据
typedef		vector<LinkmanGroup>	VecPlayerLinkMan;





///////////////////////////////////////////////////////////////////////////////////////////////////////
//! 操作缓冲
//!		由于联系人的数据的准确性需要全局的数据来确定，但DB的保存有一定的延时，造成了在读取时的一些数据偏差，
//!		本类实现的目的就是为能够预计到的这些数据偏差缓冲提供效验的缓冲数据
class LinkmanOptCache
{
public:
	LinkmanOptCache(void);
	~LinkmanOptCache(void);

public:
	//!					保存一个添加操作
	void				PushLinkmanAddOpt(tagLinkman_DB &Linkman_DB);
	//!					保存一个删除操作
	void				PushLinkmanDelOpt(tagLinkman_DB &Linkman_DB);

	//!					确认一个对象没有被删除
	BOOL				AffirmLinkmanDataBeing(tagLinkman_DB &Linkman_DB);
	//!					查找一个玩家被哪些人新添加
	void				FindNewAddPlayerData(list<tagLinkman_DB> &listLinkman_DB, const CGUID& AimGuid);

	//!					清除一个玩家的缓存
	void				ClearOnePlayerData(const CGUID& OwnerGuid);

private:
	//! 数据比较仿函数
	class OptCacheData_compare
	{
	public:
		inline bool operator()( const tagLinkman_DB& Key1, const tagLinkman_DB& Key2 ) const
		{
			return Key1 < Key2;
		}
	};

	//!												添加操作缓冲
	set<tagLinkman_DB, OptCacheData_compare>		m_AddCache;
	//!												删除操作缓冲
	set<tagLinkman_DB, OptCacheData_compare>		m_DelCache;
};