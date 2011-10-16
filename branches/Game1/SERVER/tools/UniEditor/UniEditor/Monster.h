#pragma once
#include "info.h"

// 具体怪物信息
class CMonsterInfo
{
public:
	CMonsterInfo(void);
	~CMonsterInfo(void);

	// 读Monster文件 "*.monster"
	friend ifstream &operator>>(ifstream &rstream, CMonsterInfo &rMonsterInfo); 
	friend ofstream &operator<<(ofstream &rstream, const CMonsterInfo &rMonsterInfo); 

// 数据读写
public:
	const inline string& GetName()const						{return m_strName;}
	const inline unsigned GetProbability()const				{return m_uProbability;}
	const inline unsigned GetIdentifier()const				{return m_uIdentifier;}
	const inline unsigned GetLeader()const					{return m_uLeader;}

	const inline unsigned GetDistance()const				{return m_uDistance;}
	const inline unsigned GetPathIdentifier()const				{return m_uPathID;}
	const inline string& GetScript()const					{return m_strScript;}
	const inline string& GetTalkScript()const					{return m_strTalkScript;}

	inline void SetName(const string &strName)				{m_strName = strName;}
	inline void SetProbability(const unsigned uProbability)	{m_uProbability = uProbability;}
	inline void SetIdentifier(const unsigned uIdentifier)	{m_uIdentifier = uIdentifier;}
	inline void SetLeader(const unsigned uLeader)			{m_uLeader = uLeader;}
	inline void SetDistance(const unsigned uDistance)		{m_uDistance = uDistance;}
	inline void SetPathIdentifier(const unsigned uIdentifier)	{m_uPathID = uIdentifier;}
	inline void SetScript(const string &strScript)			{m_strScript = strScript;}
	inline void SetTalkScript(const string &strScript)			{m_strTalkScript = strScript;}
	
	void MonsterInfo2String(CString &str);

// 怪物具体信息
protected:
	string		m_strName;			// 怪物名称
	unsigned	m_uProbability;		// 产生概率
	unsigned	m_uIdentifier;		// 标识
	unsigned	m_uLeader;			// 领导
	unsigned	m_uDistance;		// 距离
	unsigned	m_uPathID;			//路径标识
	string		m_strScript;		// 动作脚本
	string		m_strTalkScript;	//对话脚本
};

typedef vector<CMonsterInfo *>	VECMONINFO;
typedef VECMONINFO::iterator	VECMONINFOITR;

class CMonster :
	public CInfo
{
public:
	CMonster(void);
	CMonster(const CPoint pt);
	CMonster(const long x, const long y);
	CMonster(CMonster &rMonster);
public:
	~CMonster(void);

public:
	// 读Monster文件 "*.monster"
	friend ifstream &operator>>(ifstream &rstream, CMonster &rMonster);
	friend ofstream &operator<<(ofstream &rstream, const CMonster &rMonster); 

// 数据读写
public:
	const inline unsigned GetTimeBorn()const				{return m_uTimeBorn;}
	const inline unsigned GetTimeStart()const				{return m_uTimeStart;}
	inline VECMONINFO &GetMonsterInfo()						{return m_vecInfo;}

	inline void SetTimeBorn(const unsigned utime)			{m_uTimeBorn = utime;}
	inline void SetTimeStart(const unsigned utime)			{m_uTimeStart = utime;}

	// 添加MONSTERINFO
	bool AddMonsterInfo(CMonsterInfo *pMonInfo = NULL);

	// 删除MONSTERINFO
	bool DelMonsterInfo(CMonsterInfo *pMonInfo);

	// 将MONSTER信息转变为字符串
	void Monster2String(CString &str);

	// 怪物具体信息
	VECMONINFO	m_vecInfo;			// 怪物具体信息

// 怪物信息
protected:
	unsigned	m_uTimeBorn;		// 产生时间
	unsigned	m_uTimeStart;		// 开始时间
};

//ifstream &operator>>(ifstream &rstream, CMonsterInfo &rMonsterInfo); 
//ofstream &operator<<(ofstream &rstream, const CMonsterInfo &rMonsterInfo); 
//
//ifstream &operator>>(ifstream &rstream, CMonster &rMonster);
//ofstream &operator<<(ofstream &rstream, const CMonster &rMonster);
