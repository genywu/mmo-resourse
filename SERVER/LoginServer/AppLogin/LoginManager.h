

/*
*	file:		LoginManager.h
*	Brief:		玩家登陆的排队管理
*	detail:		
*				
*				
*				
*				
*				
*	Authtor:	张科智
*	Datae:		2008
*/




/*
--------登陆流程--------
MSG_C2L_LOG_QUESTCDKEY				//客户CDKEY验证
MSG_C2L_LOG_QUEST_PLAYERBASELIST	//客户请求自己的基本信息
							--->进入排队
MSG_W2L_LOG_ANSWER_PLAYERBASELIST	//worldserver返回一个账号的玩家基本信息列表
MSG_C2L_LOG_QUEST_PLAYERDATA		//客户请求自己的详细信息 -> 进入游戏
MSG_W2L_LOG_ANSWER_PLAYERDATA		//worldserver返回客户的详细信息是否成功取出

--------小退登陆流程--------
MSG_W2L_LOG_PLAYERQUIT				//worldserver发送退出玩家的cdkey
MSG_C2L_LOG_QUESTCDKEY				//客户CDKEY验证
MSG_W2L_LOG_ANSWER_PLAYERBASELIST	//worldserver返回一个账号的玩家基本信息列表
MSG_C2L_LOG_QUEST_PLAYERDATA		//客户请求自己的详细信息 -> 进入游戏
MSG_W2L_LOG_ANSWER_PLAYERDATA		//worldserver返回客户的详细信息是否成功取出
*/
#include "../public/AccountServerDef.h"
#include "../public/ConvenientQueue.h"
#include "..\public\BaseDef.h"

#pragma once

#define USE_GAME_WS_PLAYER_NUM
//#define _OUT_LOGIN_FLOW_INFO_




class LoginManager
{
public:
	LoginManager(VOID);
	~LoginManager(VOID);

private:
	//!														等待进入WS的排队
	typedef ConvenientQueue<string, tagAccountInfo_ToAS>	WaitInWsQueue;

public:
	struct tagLimitIpSetup
	{
		//!			是否限制IP登陆次数
		BOOL		m_bIpLogin;
		//!			一个IP同一个账号最大登陆次数
		DWORD		m_MaxIpLoginOncCdKeyCount;
		//!			一个IP登陆不同账号最大数
		DWORD		m_MaxIpLoginCdKeyCount;
		//!			清除登陆IP记录时间（毫秒）
		DWORD		m_ClearIpLoginTime;
		//!			是否限制IP创建角色次数
		BOOL		m_bIpCreate;
		//!			同IP最大创建角色次数
		DWORD		m_MaxIpCreateCount;
		//!			清除创建角色IP记录时间（毫秒）
		DWORD		m_ClearIpCreateTime;
		//!			屏蔽IP时间（毫秒）
		DWORD		m_FrozenIpTime;

		tagLimitIpSetup(VOID){memset(this, 0, sizeof(tagLimitIpSetup));}
	};

public:
	//!						改变设置
	BOOL					ChangeSetup(DWORD dwMaxWsMum, DWORD dwProcessRankInterval, DWORD dwSendQueueMsgInterval, DWORD dwOneProcessNum);
	//!						改变IP限制设置
	BOOL					ChangeIpSetup(tagLimitIpSetup &LimitIpSetup);
	//!						重读不排队账号
	BOOL					ReLoadNoQueue(VOID);
	
	//!						添加一个WS排队
	BOOL					AddWsWaitQueue(LONG lWsID);
	//!						删除一个WS排队
	BOOL					DelWsWaitQueue(LONG lWsID);
	//!						得到一个WS的进入队列
	WaitInWsQueue*			GetWaitInWsQueue(LONG lWsID);
	//!						处理排队等待的玩家
	VOID					ProcessQueue(VOID);

	//!						添加一个正在验证的账号（用于PT验证）
	VOID					AddValidating(tagAccountInfo_ToAS &AccountInfo_ToAS);

	//!						输出登陆调试信息
	VOID					OutLoginInfo(VOID);

	//!						发送等待消息给玩家
	static	VOID			SendWaitMsg(tagAccountInfo_ToAS &AccountInfo_ToAS, LPVOID pParam);
	//!						发送响应登陆消息给玩家
	static	VOID			SendAnswerCdkeyMsg(tagAccountInfo_ToAS &AccountInfo_ToAS, LPVOID pParam);
public:

	//! 客户端消息处理
	///////////////////////////////////////////////////////
	//!						响应客户端登陆请求
	VOID					OnClientLoginRequest(tagAccountInfo_ToAS &AccountInfo_ToAS);
	//!						响应客户端小退后的登陆请求
	VOID					OnClientAgainLoginRequest(tagAccountInfo_ToAS &AccountInfo_ToAS, DWORD dwServerKey[]);
	//!						响应客户端输入安全信息
	VOID					OnClientSecurityInput(const char *pCdKey, const char arrCard[], LONG lSocketID);
	//!						响应客户端请求角色基本信息列表
	VOID					OnClientQuestPlayerBaseList(const char *pCdKey, const char *pWsName);
	//!						响应客户端请求详细数据
	VOID					OnClientQuestPlayerData(LONG lWsID, const char *pCdKey, CGUID& PlayerID, DWORD dwSignCode);

	//!						响应客户端断开连接
	VOID					OnClientNetClose(const char *pCdKey, BOOL bInWs);


	//! AS消息处理
	///////////////////////////////////////////////////////
	//!						响应AS验证结果（uValidateType：0=AS验证结果，1=局部验证结果 2=盛大验证结果）
	VOID					OnValidateReturn(tagAccountInfo_FromAS &AccountInfo_FromAS, ULONG uValidateType);
	//!						响应安全信息通知
	VOID					OnSecurityData(tagAccountInfo_FromAS &AccountInfo_FromAS, tagSecurityData &SecurityData, BOOL bIsLocalValidate = FALSE);

	//!						添加一个防沉迷账号的计时
	VOID					AddFcmCdkey(const char *pCdKey, DWORD dwHoldSecond);
	//!						删除一个防沉迷账号的计时
	VOID					DelFcmCdkey(const char *pCdKey);
	//!						删除一个时间点的所有的防沉迷账号计时
	VOID					OnTimeOutFcmCdkey(DWORD dwCurrTime);
	//!						清除所有计时
	VOID					ClearFcm(VOID);


	//! WS消息处理
	///////////////////////////////////////////////////////
	//!						响应WS返回详细数据成功
	VOID					OnWsAnswerPlayerDataSucceed(const char *pCdKey, LONG lWsID);
	//!						响应WS发送的玩家退出游戏消息
	VOID					OnWsPlayerQuit(const char *pCdKey, LONG lWsID, DWORD dwServerKey[]);


	//! 账号安全
	///////////////////////////////////////////////////////
	//!						判断是否为小退玩家
	BOOL					IsJustOutPlayer(const char *pCdKey, LONG &lWsID, DWORD dwServerKey[], LONG bDel);
	//!						判断是否为不排队的玩家
	BOOL					IsNotQueueCdKey(const char *pCdKey);

	//!						响应用户密码输入错误，返回账户是否已经冻结
	BOOL					OnUserPwdErr(tagAccountInfo_FromAS &AccountInfo_FromAS);
	//!						得到因密码多次错误被冻结的账号数量
	LONG					GetFrozenCdkeyNum(VOID){return (LONG)m_mapFrozenCdkey.size();}


	//! 服务器安全
	////////////////////////////////////////////////////////
	//!						响应一个IP登陆账号，返回值（0=执行失败 1=允许登陆 LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
	DWORD					OnIpLogin(LONG lIP, LPCTSTR pCdKey);
	//!						响应一个IP创建角色，返回值（0=执行失败 1=允许登陆 LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
	DWORD					OnIpCreate(LONG lIP);

	//!						返回值（0=执行失败 1=允许登陆 LOGIN_IP_LOGIN_MAX_SAME_CDKEY, LOGIN_IP_LOGIN_NIMIETY_CDKEY,LOGIN_IP_CREATE_MAX）
	DWORD					IsFrozenIp(LONG lIP, LPCTSTR pCdKey, DWORD dwCurrTime);


public:
	//!						响应验证失败
	VOID					OnValidateReturnLost(tagAccountInfo_FromAS &AccountInfo_FromAS);


private:
	//!						发送安全验证信息到客户端
	VOID					SendSecurityToC(tagLSSecurityData& LSSecurityData);

private:
	//!						WS最大人数
	DWORD					m_dwMaxWsMum;
	//!						处理排队间隔（毫秒）
	DWORD					m_dwProcessRankInterval;
	//!						发送排队消息间隔（毫秒）
	DWORD					m_dwSendQueueMsgInterval;
	//!						每次处理人数（每个WS）
	DWORD					m_dwOneProcessNum;

	//!						上次处理的时间
	DWORD					m_dwLastProcessTime;
	//!						上次发送排队消息的时间
	DWORD					m_dwLastSendQueueMsgTime;

	tagLimitIpSetup			m_LimitIpSetup;
	

	//!													记录密码连续错误次数和时间<账号, <第一次错误的时间, 错误的次数>>
	map<string, pair<DWORD, DWORD>>						m_mapPwdErrLog;
	//!													冻结登陆账号列表
	map<string, DWORD>									m_mapFrozenCdkey;

	//!													正在验证的账号<账号， 验证超时时间>
	ConvenientQueue<string, tagAccountInfo_ToAS>		m_cqueueValidatingCdKey;
	//!													账号的安全验证信息
	map<string, tagLSSecurityData>						m_mapLSSecurityData;
	//!													通过验证的账号
	ConvenientQueue<string, tagAccountInfo_FromAS>		m_cqueuePassedAccount;
	//!													所有WS的排队列表
	map<LONG, WaitInWsQueue>							m_mapAllWaitInWsQueue;
	
	
	//!													一分钟内从WS退出的账号（用于小退验证）
	struct tagWSIDAndPwd
	{
		DWORD _dwEndTime;
		LONG _lWSID;
		DWORD _dwServerKey[SERVER_KEY_DWORD_SIZE];
	};
	ConvenientQueue<string, tagWSIDAndPwd>				m_cqueueJustOutCdKey_FromWS;

	//!													不排队的玩家
	set<string>											m_setNotQueueCdKey;
	//!													可以进入游戏的玩家
	typedef ConvenientQueue<string, tagAccountInfo_ToAS> QueueWsCanInGamePlayer;
	map<LONG, QueueWsCanInGamePlayer>					m_mapCanInGamePlayer;

	//!													为防沉迷账号的计时记录
	multimap<DWORD, string>								m_multimapFcmTimeCdkey;
	//!													对应于m_multimapFcmTimeCdkey的map
	map<string, DWORD>									m_mapCdkeyToFcmTime;

	//!													同IP登陆每个账号次数计<IP,<账号, 次数>>
	map<LONG, map<string, LONG>>						m_cqueueIpLoginCount;
	//!													下一次清除记录的时间
	DWORD												m_dwClearIpLoginTime;

	//!													同IP登陆次数计时<IP, 次数>
	map<LONG, LONG>										m_cqueueIpCreateCount;
	//!													下一次清除记录的时间
	DWORD												m_dwClearIpCreateTime;	

#ifndef USE_GAME_WS_PLAYER_NUM
	//!													已经进入游戏的玩家set<WsName, set<CdKey>>
	map<LONG, set<string>>							m_mapInGamePlayer;
#endif
	
};