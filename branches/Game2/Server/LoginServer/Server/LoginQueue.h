#pragma once
//////////////////////////////////////////////////////////////////////////
//LoginQueue.h
//Fun:	登陆缓冲队列
//Author:Joe
//Create Time:2005.1.31
//Modify Time:
//////////////////////////////////////////////////////////////////////////
#include "..\public\GUID.h"
#include "Lock.h"

#include <set>
using namespace std;

class CLoginQueue
{
public:
	//请求验证Cdkey队列
	struct tagQuestCdkey
	{
		long	lSocketID;				// socketID
		DWORD	dwIP;					// IP
		long	lVersion;				// 版本号
		string	strCdkey;				// Cdkey
		vector<BYTE>	strPassWord;	// 密码
		string	strWSName;				// WorldServer的密码
		long	lChallengeCode;			// 挑战码
		long	lEkey;					// 密宝
		DWORD   dwSendMessageTime;		// 发送消息的时间
		bool	bLimit;					// 未成年（防沉迷，0：未成年，1：成年）
		int		nRes;					// 验证结果
		bool	bISCPTAuthen;			// 是否通过了PT验证请求
        tagQuestCdkey()
			: lSocketID(0), dwIP(0), lVersion(0), strCdkey(0) ,strPassWord(0), strWSName(0),
			lChallengeCode(0), lEkey(0), dwSendMessageTime(0), bLimit(false), nRes(0),bISCPTAuthen(false)
		{
		}
		tagQuestCdkey( long SocketID, DWORD IP, long Version, string Cdkey, vector<BYTE> PassWord,
			string WSName, long ChallengeCode, long Ekey, DWORD SendMessageTime, bool Limit, int Res )
			: lSocketID(SocketID), dwIP(IP), lVersion(Version),strCdkey(Cdkey) ,
			strPassWord(PassWord), strWSName(WSName), lChallengeCode(ChallengeCode),
			lEkey(Ekey), dwSendMessageTime(SendMessageTime), bLimit(Limit), nRes(Res),bISCPTAuthen(false)
		{}
       tagQuestCdkey( const tagQuestCdkey& rQuestCdkey )
			: lSocketID(rQuestCdkey.lSocketID), dwIP(rQuestCdkey.dwIP), lVersion(rQuestCdkey.lVersion),
			strCdkey(rQuestCdkey.strCdkey) ,strPassWord(rQuestCdkey.strPassWord), strWSName(rQuestCdkey.strWSName),
			lChallengeCode(rQuestCdkey.lChallengeCode), lEkey(rQuestCdkey.lEkey), dwSendMessageTime(rQuestCdkey.dwSendMessageTime),
			bLimit(rQuestCdkey.bLimit), nRes(rQuestCdkey.nRes),bISCPTAuthen(false)
		{}
	};

	//请求基本信息队列
	struct tagQuestPlayerList
	{
		long	lSocketID;				// SocketID
		string	strCdkey;				// Cdkey
		string	strWSName;				// WSName
		DWORD   dwSendMessageTime;		// 发送消息的时间
	};
	//请求玩家信息列表
	struct tagQuestPlayerData
	{
		long	lSocketID;				// socketID
		string	strCdkey;				// Cdkey
		CGUID	PlayerID;				// 请求的ID
		DWORD	dwIP;					// 客户端IP
		DWORD   dwSendMessageTime;		// 发送消息的时间
		DWORD   dwSignCode;				// 验证码
	};
	
private:

	std::set<string>						NoQueueCDkeyList;	//不排队的CDKey列表

	typedef std::list<tagQuestCdkey>	    QuestCdkey;			//验证Cdkey队列列表
	typedef std::list<tagQuestCdkey *>	    QuestPtCheck;		//验证Cdkey队列列表
	typedef std::list<tagQuestPlayerList>   QuestPlayerList;	//获取基本信息列表队列
	typedef std::list<tagQuestPlayerData>	QuestPlayerData;	//请求玩家详细信息列表

	typedef std::list<tagQuestCdkey>::iterator itCdkey;
	typedef std::list<tagQuestPlayerList>::iterator itPlayerList;
	typedef std::list<tagQuestPlayerData>::iterator itPlayerData;


	QuestCdkey								m_QuestCdkey;		//验证Cdkey队列列表
	set<string>								m_setQuestCdkey;
	std::map<string,QuestPlayerList>		m_QuestPlayerList;
	map<string, set<string>>				m_mapQuestBaseInfo;
	std::map<string,QuestPlayerData>		m_QuestPlayerData;

	QuestCdkey								m_NoQueueQuestCdkey;		//不用排队的验证Cdkey队列列表
	std::map<string,QuestPlayerList>		m_NoQueueQuestPlayerList;
	std::map<string,QuestPlayerData>		m_NoQueueQuestPlayerData;

	typedef std::map<string,QuestPlayerList>::iterator itPlayerListMap;
	typedef std::map<string,QuestPlayerData>::iterator itPlayerDataMap;

	long m_nWordNum;				//World的总数量
	DWORD m_dwInter;				//处理队列的间隔时间
	DWORD m_dwSendMessageInter;		//给客户端发送消息的间隔
	long m_nWorldMaxPlayerNum;		//WorldServer的最大登陆数量

	DWORD m_dwDoLogQueueTime;		//处理队列的时间
	DWORD m_dwWorldQueueTime;		//处理登陆World的时间
public:
	CLoginQueue(void);
	~CLoginQueue(void);

	void	OnInitial();			//初始化
	void	LoadNoQueueCdkeyList();
public:

	BOOL	IsWaitCdKey(const char* szCdkey){return m_setQuestCdkey.end() != m_setQuestCdkey.find(szCdkey);}
	bool	IsInNoQueueList(const char* szCdkey);	//该CDKey是否在不用排队的列表中
	void	SetWorlNum(int nNum)		{m_nWordNum = nNum;}

	void	AddQuestCdkey(long lSocketID,DWORD dwIP,long lVersion,const char* szCdkey,
					  vector<BYTE>& bytePassWord,char* szWSName, long lChallengeCode, long lEkey);
	void	AddQuestPlayerList(long lSocketID,const char* szCdkey,const char* szWSName);
	void	AddQuestPlayerData(long lSocketID,const char* szCdkey,CGUID& playerid,DWORD dwIP, DWORD dwSignCode);

	void	Run();

	void	OnQuestCdkey(tagQuestCdkey& QuestCdkey);
	void	OnQuestPlayerList(tagQuestPlayerList& QuestPlayerList);
	void	OnQuestPlayerData(tagQuestPlayerData& QuestPlayerData);

	void	OnClientLost(const char* szCdkey);

private:
	// playerid, lastTime(curTime)
	typedef map<CGUID,DWORD,guid_compare> LoginList;
	typedef map<CGUID,DWORD,guid_compare>::iterator itLogList;
	LoginList			m_mLoginList;				//已经登陆过的

public:
	// 把角色加入登陆表
	bool	PushLoginList(CGUID playerid);
	// 玩家是否合法请求
	bool	IsValidQuest(CGUID playerid);

	void	ClearTimeoutList();

private:
	QuestPtCheck				m_lstPtChecked;		// 执行完PT验证帐号队列
    Lock						lockPtChecked;		// 执行完PT验证帐号队列的锁

public:
    tagQuestCdkey* PushBackPtChecked( tagQuestCdkey* pAcc );	// 把一个账号压入队尾
	tagQuestCdkey* PopFrontPtChecked();							// 把一个账号弹出队列
	void CLoginQueue::ClearPtChecked(QuestPtCheck &lstChecked);	// 清空帐号验证队列
    bool IsPtCheckedEmpty();									// 帐号队列是否为空
    void HandlePtChecked();										// 处理已经通过密码验证的帐号
};
