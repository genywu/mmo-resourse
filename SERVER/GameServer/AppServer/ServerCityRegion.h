///////////////////////////////////////////////////////////////
//Serverregion.h///////////////////////////////////////////////
//攻城战地图类，负责攻城战的处理
//Author：Joe
//Create Date:2004.7.19
#pragma once
#include "serverwarregion.h"
#include <map>
class CBuild;
class CCityGate;
class CMoveShape;
class CMonster;
using namespace std;

class CServerCityRegion :
	public CServerWarRegion
{
public:
	CServerCityRegion(void);
	CServerCityRegion(const CGUID& id);
	CServerCityRegion(long id);
	~CServerCityRegion(void);
	//Build
	struct tagBuild
	{
		long lID;				// 城门的编号
		long lPicID;			// 图片ID
		long lDir;				// 方向
		long lAction;			// 初始Action
		long lMaxHP;			// 最大生命上限
		long lDef;				// 防御力
		long lElementResistant;	//元素抗性
		long lTitleX;			// 坐标
		long lTitleY;
		long lWidthInc;			// 体型
		long lHeightInc;

		string strName;			// 名字
		string strScript;		// 击毁执行脚本
	};

	struct tagCityGate
	{
		long lID;
		CGUID CityGateID;
		CCityGate* pCityGate;
		string strName;
	};
private:
	CBuild*					m_pSymbol;					//标志物
	map<long,tagCityGate>	m_CityGates;				//城门
	tagRegionSetup			m_DefenceSideRS;			//防守方复活区域


	set<CGUID>				m_GurdMonsters;				//场景中的活的守卫
	list<long>				m_GuardIndexs;				//守卫的刷新列表

	long	m_lAttackCityGateType;
	long	m_lAttackCityGateID;
	CGUID	m_DefSideRSFactionID;					//内部复活点复活的帮会ID

	//////////////////////////////////////////////////////////////////////////
	//	接口
	//////////////////////////////////////////////////////////////////////////
public:
	BOOL Save();	// 读取

	virtual bool AddToByteArray(vector<BYTE>* pByteArray, bool bExData = true);		// 添加到CByteArray
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	
	void	SetAttackCityGateMoveShape(long lType,long lID)
	{m_lAttackCityGateType=lType;m_lAttackCityGateID=lID;}

public:
	
	virtual void SetEnterPosXY(CPlayer*pPlayer);
	void SetOwnedCityOrg(const CGUID& FactionID,const CGUID& UnionID);

	CCityGate* AddCityGate(tagBuild&);											// 添加一个城门
	virtual void AddGurdMonster(const CGUID& guid);
	virtual void DelGurdMonster(const CGUID& guid){m_GurdMonsters.erase(guid);}
	virtual void	AddGuardIndex(long lIndex);
	void	RefreshGuard();

	virtual bool OperatorCityGate(long lCityGateID,eOperCityGate eOp);		// 对城门进行操作 
	bool OperatorCityGate(CCityGate* pCityGate,eOperCityGate eOp);
	virtual void UpdateCityGateToClient(long lCityGateID);					//更新城门信息到客户端
	virtual bool OperatorSymbol(long lSymbolID,eOperCityGate eOp);				// 对标志物进行操作 

	virtual	void OnSymbolDestroy(long lSymID,long lPlayerID,long lFacID,const CGUID& UnionID);	//当标志物摧毁时 
	virtual void OnWarDeclare(long lWarNum);											//开始宣战
	virtual void OnWarStart(long lWarNum);												//攻城开始
	virtual void OnWarTimeOut(long lWarNum);											//攻城时间结束
	virtual void OnWarEnd(long lWarNum);												//攻城结束
	virtual void OnWarMass(long lWarNum);												//攻城集结
	virtual void OnClearOtherPlayer(long lWarNum);										//清除其它人员
	virtual void OnRefreshRegion(long lWarNum);											//刷新场景
	virtual bool SymbolIsAttackAble(CMoveShape* pAttackShape);
	virtual bool GuardIsAttackAble(CMoveShape* pAttackShape);
	bool	CityGateIsClose(long lCityGateID);											//判断某个城门是否可以关闭
	bool	CityGateIsClose(CCityGate* pCityGate);										//判断某个城门是否可以关闭
	virtual long	GetCityGateState(long lCityGateID);									//得到某个城门的状态

	bool IsOwner(const CGUID& FactionID);														//判断某个帮会是否能争夺
	void OnContendTimeOver(tagContend Contend);											//某个争夺时间到
	void OnFactionVictory(const CGUID& FactionID,const CGUID& UnionID);								//当一个帮会获胜时
	void ClearRegion();																	//结束时清场景
	virtual void OnFactionWinOneSymbol(const CGUID& FactionID,long lSymbolID);					//当一个帮会赢得了一个标志物触发

	virtual void GetReturnPoint(CPlayer* pPlayer, long &lRegion, long &l, long &t, long &r, long &b, long &d);
	virtual CRegion::eSecurity GetSecurity(int iTileX, int iTileY);
	virtual long GetDiedStateTime();

	bool IsApplyWarFacsMem(const CGUID& FactionID);
};
