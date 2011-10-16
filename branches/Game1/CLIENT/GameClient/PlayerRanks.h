//玩家排行榜
//joe
//20006-3-16

#pragma once
class CPlayerRanks
{
public:
	CPlayerRanks(void);
	~CPlayerRanks(void);

	struct tagRank
	{
		int		nPlayerID;		//玩家ID
		string	strName;		//万家名字
		WORD	wOccupation;	//职业
		WORD	wLevel;		//等级
	
		string	strFacName;	//所在行会名字
	};

private:
	static CPlayerRanks* instance;											//该类唯一实例句柄

	list<tagRank>	m_PlayerRanks;

public:
	static CPlayerRanks* getInstance();										//得到唯一实例句柄
	bool   Initialize();													//初始化
	void   Release();

	bool DecordFromByteArray(BYTE* pByte, long& lPos);

	list<tagRank> *GetPlayerRanksList()		
	{
		return &m_PlayerRanks;
	}
};

CPlayerRanks* GetPlayerRanks();
