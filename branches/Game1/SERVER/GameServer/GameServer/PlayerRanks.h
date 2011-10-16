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
		CGUID	PlayerID;		//玩家ID
		string	strName;		//万家名字
		WORD	wOccupation;	//职业
		WORD	wLevel;		//等级
	
		string	strFacName;	//所在帮会名字
	};

private:
	static CPlayerRanks* instance;											//该类唯一实例句柄

	list<tagRank>	m_PlayerRanks;

	map<CGUID,DWORD>				m_PlayerGetTime;							//用来控制玩家获取该数据的间隔

public:
	static CPlayerRanks* getInstance();										//得到唯一实例句柄
	bool   Initialize();													//初始化
	void   Release();

	bool AddToByteArray(vector<BYTE>& ByteArray,int nAddNum);				// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& lPos);

	void OnPlayerGetRanks(const CGUID& lPlayerID,long lMaxNum);
};

CPlayerRanks* GetPlayerRanks();
