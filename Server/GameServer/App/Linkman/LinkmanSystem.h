

/*
*	file:		LinkmanSystem.h
*	Brief:		联系人管理类
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-27
*/

#include "LinkmanGroup.h"


class LinkmanSystem
{
public:
	LinkmanSystem(void);
	~LinkmanSystem(void);

public:
	//!								得到唯一实例
	static	LinkmanSystem&			GetInstance(void);

public:
	//!								判断一个玩家【OwnerGuid】是否与另一个玩家【pOtherOneName】存在在指定的关系
	BOOL							CheckRelation(const CGUID &OwnerGuid, const char *pOtherOneName, eLinkmanGroupType eRelationType);
	//!								判断一个玩家【OwnerGuid】是否与另一个玩家【OtherOneGuid】存在在指定的关系
	BOOL							CheckRelation(const CGUID &OwnerGuid, const CGUID &OtherOneGuid, eLinkmanGroupType eRelationType);
	

	//! 响应WS消息
public:
	//!								初始化配置
	void							InitSetup(BYTE* pByte, LONG& pos);
	//!								响应初始化消息
	void							OnInitMessage(CMessage *pMsg);
	//!								清除玩家数据通知
	void							OnReleaseMessage(CMessage *pMsg);

	//!								响应联系人登陆消息
	void							OnLinkmanLoginMessage(CMessage *pMsg);
	//!								响应联系人离线消息
	void							OnLinkmanLogoutMessage(CMessage *pMsg);
	//!								响应玩家信息改变消息
	void							OnPlayerInfoChange(CMessage *pMsg);
	//!								响应联系人删除状态改变
	void							OnPlayerDelStateChange(CMessage *pMsg);

	//!								响应添加联系人操作
	void							OnAddLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lGroupID, tagLinkman_data &Linkman_data);
	//!								响应删除联系人操作
	void							OnDelLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lGroupID, const CGUID &AimGuid, const char *pAimName);
	//!								响应移动联系人操作
	void							OnMoveLinkMan(const CGUID &OwnerGuid, const char *pOptName, LONG lSrcGroupID, LONG lDesGroupID, tagLinkman_data &Linkman_data);


	//!	响应客户端消息
public:						
	//!								响应申请添加联系人
	void							OnRequestAdd(CMessage *pMsg);
	//!								响应请求删除联系人
	void							OnRequestDel(CMessage *pMsg);
	//!								响应请求移动联系人
	void							OnRequestMove(CMessage *pMsg);
	//!								响应玩家回复被添加
	void							OnAnswerAdd(CMessage *pMsg);
	
public:
	//!								发送联系人数据到WS
	void							SendDataToWs(void);
	//!								响应玩家退出服务器
	void							OnPlayerExit(const CGUID &PlayerGuid);
	//!								记录PK致死名单
	void							NotePkPlayer(CPlayer *pDecedent, CPlayer *pMurderer);
	//!								记录结婚对象
	void							AddSpouse(CPlayer *pSelf, CPlayer *pSide);
	//!								删除结婚对象
	void							DelSpouse(CPlayer *pSelf);
	//!								找到结婚对象信息
	const	tagLinkman*				FindSpouseInfo(CPlayer *pSelf);




private:
	//!											好友系统相关配置
	LGSManage									m_LGSManage;
	//!											所有玩家的联系人数据
	map<CGUID, VecLinkmanGroup>					m_mapPlayerLinkman;


	
	
};