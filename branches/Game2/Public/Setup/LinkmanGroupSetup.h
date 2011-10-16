

/*
*	file:		LinkmanGroupSetup.h
*	Brief:		好友组配置类	LinkmanGroupSetup
*				配置管理类		LGSManage
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-02-18
*/

#pragma once

#include "../DefPublic.h"
#include "../Common/LinkmanDef.h"

//! 配置类
///////////////////////////////////////////////////////////////////////////////////////////////////
class LinkmanGroupSetup
{
public:
	LinkmanGroupSetup(void);
	~LinkmanGroupSetup(void);

public:
	//!							重置对象
	void						Reset(void);
	//!							从传入的XML节点中获取配置信息
	bool						LoadSetup(TiXmlElement* pGroupElement);

	//! 基本
	//! ----------------------------------------------------------------------------------
public:
	//!							配置定义的唯一ID
	ULONG						m_uID;
	//!							类型(eLinkmanGroupType)
	ULONG						m_uGroupType;
	//!							本组成员的留言优先级（eLeaveWordPRI）
	ULONG						m_uLeaveWordPRI;
	//！						密聊频道类型（eSecretTalkChannel）
	ULONG						m_uSecretTalkChannelType;
	//！						启用互加区别
	bool						m_bPartMutual;
	

	//! 行为
	//! ----------------------------------------------------------------------------------
public:
	//!							最大人数限制
	ULONG						m_uMaxMemberNum;
	//!							存储方式（eKeepWay）
	ULONG						m_uKeepWay;
	//！						即时显示信息（eTimelyInfoType）
	ULONG						m_uTimelyInfo;
	//！						屏蔽功能（eFunctionFiltrateType）
	ULONG						m_uFunctionFiltrate;

	

	//! 成员操作
	//! ----------------------------------------------------------------------------------
public:
	//!							提供用户添加操作
	bool						m_bUserAdd;
	//!							自动添加办法（eAutoAddType）
	ULONG						m_uAutoAddWay;
	//!							添加时是否请求目标确认
	bool						m_bAddAimAffirm;

	//!							提供用户删除操作
	bool						m_bUserDel;
	
	//!							操作由谁完成（eGroupOperatorType）
	ULONG						m_uOperater;
	//！						成员是否具有唯一性
	bool						m_bMemberIsOnly;
	//！						成员移动优先级（eOnlyPRT）
	ULONG						m_uOnlyPRT;
	//！						同级删除
	bool						m_bSameDel;
	

	//! 提示
	//! ----------------------------------------------------------------------------------
public:
	//! 提示配置结构
	struct tagNotifySetup 
	{
//#ifndef USE_IN_SERVER
		//! 			上线提示信息
		ULONG			uMessage_Login;
		//! 			下线提示信息
		ULONG			uMessage_Logout;
		//! 			上线提示音
		char			szMusic_Login[MAX_FILE_PATH_SIZE];
		//! 			下线提示音
		char			szMusic_Logout[MAX_FILE_PATH_SIZE];
		//! 			收到密聊提示音
		char			szMusic_SecretTalk[MAX_FILE_PATH_SIZE];
//#endif //! USE_IN_SERVER
		//! 			向被添加的目标发送提示信息
		bool			bMessage_AddAim;
		//! 			向被删除的目标发送提示信息
		bool			bMessage_DelAim;
	};

	//!							提示信息
	tagNotifySetup				m_NotifySetup;

	//! 外观
	//! ----------------------------------------------------------------------------------
public:

//#ifndef USE_IN_SERVER
	//! 颜色配置结构
	struct tagColorSetup 
	{
		//!				在线色
		ULONG			uNameColor_Online;
		//!				离线色
		ULONG			uNameColor_Leave;
		//!				互加关系在线色
		ULONG			uNameColor_Online_Mutual;
		//!				角色已删除色
		ULONG			uNameColor_Player_Deleted;
		//!				密聊显示色
		ULONG			uSecretTalkColor;

	};

	//! 颜色配置
	tagColorSetup				m_ColorSetup;
	//! 本组角色称呼
	char						m_szMemberAppellative[MAX_APPELLATIVE_SIZE];
	char						m_szFileName[MAX_APPELLATIVE_SIZE];

//#endif //! USE_IN_SERVER


};



//! 配置管理类
///////////////////////////////////////////////////////////////////////////////////////////////////
class LGSManage
{
public:
	LGSManage(void);
	~LGSManage(void);

public:
	//!							读取配置
	bool						LoadSetup						(const char *pFilename);
	//!							得到配置的好友组数量
	ulong						GetGroupNum						(void);
	//!							按指定的顺序得到一个组的配置
	const	LinkmanGroupSetup*	GetLinkmanGroupSetup_ByOrder	(ulong lOrder);
	//!							通过ID得到组所在顺序
	ulong						GetIndexByID					(ulong uID);
	//!							通过组类型得到组所在顺序
	ulong						GetIndexByType					(eLinkmanGroupType eRelationType);


//#ifdef USE_IN_SERVER
	//!							编码
	void						AddToByteArray					(vector<uchar> *pVecData);
	//!							解码
	void						DecordFromByteArray				(uchar* pByte, long& pos);

//#endif //! USE_IN_SERVER

private:
	//!							所有配置
	vector<LinkmanGroupSetup>	m_LinkmanGroupSetup;
	//!							读取成功标记
	bool						m_bLoadSucceed;
};