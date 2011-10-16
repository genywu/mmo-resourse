
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

#include "../../../../Public/Common/LinkmanDef.h"
#include "../../../../Public/Setup/LinkmanGroupSetup.h"

class LinkmanGroup
{
public:
	explicit LinkmanGroup(const LinkmanGroupSetup* pSetup, const CGUID &OwnerGuid);
	~LinkmanGroup(void);

public:
	//!										添加一个联系人
	void									AddMember						(tagLinkman &Linkman, BOOL bSend2Client);
	//!										删除一个联系人
	void									DelMember						(const CGUID &PlayerGuid, BOOL bSend2Client, const char *pAimName);
	//!										找到一个联系人
	BOOL									FindMember						(const CGUID &PlayerGuid);
	//!										找到一个联系人的GUID
	CGUID									FindMemberID					(const char *pPlayerName);
	//!										得到联系人数量
	LONG									GetMemberNum					(void);
	//!										得到第一个联系人的GUID
	const	tagLinkman*						GetFirstMember					(void);

	//!										添加一个添加我为联系人的人
	void									AddCompriseMePlayer				(tagLinkman_DB &Linkman, const char *pOptName);
	//!										删除一个添加我为联系人的人
	void									DelCompriseMePlayer				(const CGUID &PlayerGuid, const char *pOptName);

	//!										一个联系人上线
	void									LinkmanLogin					(tagLinkman_data &Linkman_data);
	//!										一个联系人下线
	void									LinkmanLogout					(const CGUID &PlayerGuid);
	//!										一个联系人地图改变
	void									LinkmanMapChange				(const CGUID &PlayerGuid, const char *pMapName);
	//!										一个联系人等级改变
	void									LinkmanLevelChange				(const CGUID &PlayerGuid, LONG lLevel);
	//!										一个联系人职业改变
	void									LinkmanOccuChange				(const CGUID &PlayerGuid, LONG lOccu);
	//!										一个联系人删除状态改变
	void									LinkmanDelStateChange			(const CGUID &PlayerGuid, BOOL bDeleted);

	//!										解码初始化数据
	void									Decode_FromWs					(CMessage *pMsg);
	//!										编码到客户端
	void									Serialize_ForClient				(CMessage *pMsg);
	//!										编码到WS
	void									Serialize_ForWS					(vector<BYTE> *pVecData);
	//!
	void									AddMemberToVec					(vector<tagLinkman> &vLinkman);
	void									AddCompriseMePlayerToVec		(vector<tagLinkman_DB> &vCompriseMePlayer);

private:
	//!										我添加的玩家
	map<CGUID, tagLinkman>					m_mapMeAdd;
	//!										添加我的玩家
	map<CGUID, tagLinkman_DB>				m_mapAddMe;
	//!										相关配置指针
	const LinkmanGroupSetup*				m_pSetup;
	//!										所有者ID
	CGUID									m_OwnerGuid;

};


//!									玩家的联系组数组
typedef		vector<LinkmanGroup>	VecLinkmanGroup;