


/*
*	file:		LinkmanSystem.h
*	Brief:		好友系统管理类
*	detail:		管理联系人组的建立、操作、相关的DB通讯，缓存DB数据
*
*				DB操作会被缓存在对象中，因此必须在DBS连接有效时释放对象，才可保证所有更新数据被写入到DB中
*				
*	Authtor:	张科智
*	Datae:		2008-02-20
*/
#pragma once

#include "LinkmanGroup.h"

//!				最大联系人条目缓存数量
const	ULONG	MAX_LINKMAN_BUF_NUM = 1024 * 512;

class CEntityGroup;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//!	好友系统管理类
class LinkmanSystem
{
public:
	LinkmanSystem(void);
	~LinkmanSystem(void);

public:
	//!								得到全局唯一实例
	static		LinkmanSystem&		GetInstance						(void);


	//! 配置
	//!-----------------------------------------------------------------------------------------------
public:
	//!								得到配置并且初始化对象
	bool							LoadSetupAndInit				(const char* pFilename);
	//!								将配置信息编码到数组
	void							SetupAddToByteArray				(vector<BYTE>& veData);

private:
	//!								好友系统相关配置
	LGSManage						m_LGSManage;

	//!-----------------------------------------------------------------------------------------------
public:
	void							GetPlayerLinkmanData_FromDBS	(const CGUID &PlayerGuid, CEntityGroup* linkmanGroupOwner, CEntityGroup* linkmanGroupAim);
	//!								把一个玩家的数据编码到数组，用于DBS保存
	void							AddToByteArray_ForDBS			(const CGUID &PlayerGuid, CEntityGroup* linkmanGroupOwner, CEntityGroup* linkmanGroupAim);
	//!								响应玩家上线
	void							OnPlayerOnline					(CPlayer *pPlayer);
	//!								响应玩家转GS
	void							OnPlayerChangeGs				(CPlayer *pPlayer);
	//!								响应玩家离线
	void							OnPlayerOffline					(CPlayer *pPlayer);

	//!								响应玩家等级改变
	void							OnPlayerLevelChange				(CPlayer *pPlayer);
	//!								响应玩家职业改变
	void							OnPlayerOccuChange				(CPlayer *pPlayer);
	//!								响应玩家删除状态改变
	void							OnPlayerDelStateChange			(const CGUID &PlayerGuid, BOOL bDeleted);

	//!								能否留言，返回留言的优先级，eLWPRI_Cant表示不能留言
	ULONG							CanLeaveWord					(const CGUID &SenderGuid, const char *pReceiverName, CGUID &reReceiverGuid);
	//!								能否发信（接收者必须在线）
	BOOL							CanMail							(CPlayer *pReceiver, const char *pSenderName);
	//!								响应DBS的玩家联系人数据保存完毕消息
	void							OnPlayerDataSave				(const CGUID &PlayerGuid);

	//!								响应系统请求添加消息
	void							OnSystemRequestAddMsg			(CMessage *pMsg);						
	//!								响应玩家请求添加消息
	void							OnPlayerRequestAddMsg			(CMessage *pMsg);
	//!								响应玩家回复添加消息
	void							OnPlayerReAddMsg				(CMessage *pMsg);
	//!								响应玩家请求删除消息
	void							OnPlayerRequestDelMsg			(CMessage *pMsg);
	//!								响应玩家请求移动消息
	void							OnPlayerRequestMoveMsg			(CMessage *pMsg);


	//!								响应GS地图改变消息
	void							OnGsMapChange					(const CGUID &PlayerGuid, const char* pMapName);
	//!								响应GS数据发送消息
	void							OnGsDataSend					(BYTE *pData, LONG &lPos);

	//!								删除一个玩家的信息
	void							DelPlayerLinkman				(const CGUID &PlayerGuid);


	void							OutLinkmanInfo(VOID);


private:
	//! 操作类型
	enum tagLinkmanOprType
	{
		eLOT_Add,
		eLOT_Del,
		eLot_Move,
	};
	//! 会话数据对象
	struct tagOptSession
	{
		ULONG _uEndTime;	//! 会话到点时间
		ULONG _uOptType;	//! 操作类型
		ULONG _uSrcGroupID;	//! 操作的起始组ID
		ULONG _uDesGroupID;	//! 操作的终止组ID
	};

	//!								创建
	BOOL							CreateAddSession				(const CGUID& OwnerGuid, const CGUID& FlagGuid, tagOptSession &OptSession);
	//!								移除一个玩家的所有会话
	void							EndAddSession					(const CGUID& OwnerGuid);
	//!								移除两个玩家之间的会话
	BOOL							EndAddSession					(const CGUID& OwnerGuid, const CGUID& FlagGuid, tagOptSession &OptSession);
	//!								清除一个玩家的超时会话
	void							ClearOutTimeSession				(map<CGUID, tagOptSession> &PlayerSession);
	


	//! 功能辅助
	//!-----------------------------------------------------------------------------------------------
private:
	//!											所有好友信息在此存放
	map<CGUID, VecPlayerLinkMan>				m_mapPlayerLinkMan;

	//!											玩家操作缓存
	LinkmanOptCache								m_LinkmanOptCache;

	//! 
	//!-----------------------------------------------------------------------------------------------
	//!											会话保存map<应答者ID, map<等待应答者ID, pair<超时时间点, 标识符>>>
	map<CGUID, map<CGUID, tagOptSession>>		m_Session;

	
};

//! 联系人DB缓存
class LinkmanDbCache
{
	
};