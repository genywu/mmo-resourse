//////////////////////////////////////////////////////////////////////////
//serverwarregion.h/
//战斗争夺场景的基类
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "serverregion.h"

//争夺状态的结构
struct tagContend
{
	long lID;				//标示ID
	string strName;			//标志物名字
	CGUID PlayerID;			//点击的玩家
	const CGUID& FactionID;		//帮会ID
	long lCurTime;			//进行的时间(毫秒)
	long lMaxTime;			//最大的时间
	DWORD dwStartTime;		//上一次计时时间
};

class CServerWarRegion :
	public CServerRegion
{
public:
	CServerWarRegion(void);
	CServerWarRegion(const CGUID& id);
	CServerWarRegion(long id);
	~CServerWarRegion(void);

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

private:
	list<tagContend>		m_listContend;				//争夺
	typedef list<tagContend>::iterator itContend;
	typedef map<long,CGUID>::iterator itFacSym;
protected:
	map<long,CGUID>			m_FacWinSymbol;				//标志物被占领的帮会
	long	m_lSymbolTotalNum;							//标志物总数量
	long	m_lWinVicSymbolNum;							//获胜夺取标志物的数量
	long	m_lVicSymbolNum;							//坚持到最后获胜的标志物数量
public:
	virtual void OnWarDeclare(long lWarNum);				//开始宣战
	virtual void OnWarStart(long lWarNum);					//攻城开始
	virtual void OnWarEnd(long lWarNum);					//攻城结束
	virtual void OnWarMass(long lWarNum);					//攻城集结
	virtual void OnFactionVictory(const CGUID& FactionID,const CGUID& UnionID);	//当一个帮会获胜时
	virtual void ClearRegion();								//结束时清场景

	void ReSetWarState(long lWarID,eCityState eState);

	virtual bool IsOwner(const CGUID& FactionID)	{return false;}
	virtual void OnEnterContend(CPlayer* pPlayer,long lID,const char* chName,long lTime,string strGood1,string strGood2,
								string strGood3,string strGood4);
    
	void OnPlayerDamage(CPlayer* pPlayer,long lDamHp);		//当玩家攻击受到伤害
	void AddContend(CPlayer* pPlayer,long lID,const char* chName,long lTime);	//添加一个争夺
	void DecContendTime(CPlayer* pPlayer,long lDecTime);	//减少时间
	bool CancelContendByPlayerID(CPlayer* pPlayer);			//取消某个玩家的争夺
	void CancelContendBySymbolID(long lSymbolID);			//取消所有玩家对某个标志物的争夺
	void SetFacWinSymbol(const CGUID& FactionID,const CGUID& UnionID,long lSymbol);//设置某个帮会赢得某个标志物
	virtual void AddNeedGood(string& strGoodName){}			//添加需要的道具名
	virtual void OnContendTimeOver(tagContend Contend);		//某个争夺时间到
	bool GetIsFacWinSymbol(const CGUID& FactionID,long lSymbolID);	//判断某个帮会是否赢得了某个标志物
	bool GetIsPlayerIsContendSymbol(const CGUID& PlayerID,long lSymbolID);//判断某个玩家是否正在争夺某个标志物中
	virtual void OnFactionWinOneSymbol(const CGUID& FactionID,long lSymbolID){}//当一个帮会赢得了一个标志物触发

	void UpdateContentTime(const CGUID& PlayerID,long lPercent);	//刷新争斗状态时间到客户端
	void UpdateContendPlayer();								//根据申请的帮会更新争夺玩家
};
