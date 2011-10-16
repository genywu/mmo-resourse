//玩家排行榜
//joe
//20006-3-16

#pragma once
#include "..\public\Date.h"

class CPlayer;

class CPlayerRanks
{
public:
	CPlayerRanks(void);
	~CPlayerRanks(void);

	struct tagRank
	{
		CGUID	PlayerID;		//玩家ID
		string	strName;		//万家名字
		WORD	wOccupation;	//职业
		WORD	wLevel;		//等级
	
		string	strFacName;	//所在帮会名字
	};

private:
	static CPlayerRanks* instance;											//该类唯一实例句柄

	bool	m_bStat;														//是否进行统计
	tagTime m_StatTime;														//统计排名时间点
	ULONG	m_lStatRanksEventID;											//统计排名的时间计时器

	int				m_nMaxNum;												//最大的排行榜数量
	list<tagRank>	m_PlayerRanks;

public:
	static CPlayerRanks* getInstance();										//得到唯一实例句柄
	bool   Initialize();													//初始化
	void   Release();

	bool AddToByteArray(vector<BYTE>& ByteArray);							// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& lPos);

	bool	GetIsStat()						{return m_bStat;}
	void	SetIsStat(bool b)				{m_bStat = b;}

	int		GetMaxNum()						{return m_nMaxNum;}

	tagTime& GetStatTime()					{return m_StatTime;}
	void	SetStatRanksEventID(ULONG lID)	{m_lStatRanksEventID = lID;}

	void Clear()		{m_PlayerRanks.clear();}
	void StatPlayerRanks();
	void AddRank(const CGUID& PlayerID, string& strName,WORD wOccu,WORD wLevl);
	void UpdateRanksToGameServer();

	static	void CALLBACK OnStatRanks(LPARAM lparam);						//统计排行
};

CPlayerRanks* GetPlayerRanks();
