#pragma once
//OrganizingCtrl.h
//Function:	//该类负责控制行会的建立、解散、以及对帮、同盟的管理控制.
			//负责对Organizings变量的管理
			//负责行会系统的装载、存储，更新
//Author:Joe
//Create Time:2004.6.11
//Note:该类全局只有唯一的实例
class COrganizing;
class CFaction;
class CUnion;
struct tagTime;

//行会排行榜结构
struct tagFacBillboard
{
	long	lFactionID;		//行会ID
	string	strName;		//行会名字
	long	lNum;			//数量
};

//行会信息结构，用于宣战时用
struct DeclareWarFacInfo
{
	CGUID	FactionID;		//行会ID
	BYTE	btCountry;		//国家ID
	char	strName[20];	//行会名字
	long	lRelations;		//和本帮的关系,0:无关系,1:本帮,2:同盟,3:敌对,4:本国
};

struct tagAllFacState
{
	CGUID	FactionID;
	BYTE	btCountry;
	char	strFactionName[20];
	bool	bUionRelation;			//是否是同盟关系
	bool	bFactWarEnemyRela;		//是否是帮战敌对关系
    bool	bCityWarEnemyRela;		//是否是城战敌对关系
};


struct KeyCompareByFaction
{
	bool operator()(const tagAllFacState& KeyX,const tagAllFacState& KeyY)
	{
		if(KeyX.bUionRelation != KeyY.bUionRelation )
		{
			if(KeyX.bUionRelation == true)
				return true;
			else
				return false;
		}
		else
		{
			if(KeyX.bFactWarEnemyRela != KeyY.bFactWarEnemyRela)
			{
				if(KeyX.bFactWarEnemyRela == true)
					return true;
				else
					return false;
			}
			else
			{
				if(KeyX.bCityWarEnemyRela != KeyY.bCityWarEnemyRela)
				{
					if(KeyX.bCityWarEnemyRela)
						return true;
					else
						return false;
				}
				else
				{
					if(KeyX.FactionID < KeyY.FactionID)
						return true;
					else
						return false;
				}
			}
		}
		return false;
	}
};

class COrganizingCtrl
{
private:
	COrganizingCtrl(void);
	~COrganizingCtrl(void);
public:
	void DecordFactionListFromByteArray(BYTE* pByte, long& pos,long lPage,long lFactionNum);	//解压行会列表
	void DecordFactionBillboardFromByteArray(BYTE* pByte,long& pos,string& str);				//解压行会排行榜
	void DecordDeclareFactionInfoFromByteArray(BYTE* pByte, long& pos,long lPage,long lFactionNum);//解呀宣战行会列表

	void DecordAllFactionsFromByteArray(BYTE* pByte,long& pos);					//解压行会数据

public:
	struct tagApplyFaction
	{
		CGUID ID;
		char strName[20];
	};

	
	//属性
private:
	static COrganizingCtrl* instance;										//该类唯一实例句柄

	CFaction* m_FactionOrganizing;											//玩家的行会组织
	CUnion* m_ConfedeOrganizing;											//玩家的同盟组织

	CGUID	m_ApplyFactionID;
	list<tagApplyFaction>	m_FactionList;									//行会列表
	long	m_lFactinNum;
	long	m_lCurMaxPage;

	string	m_strFactionBillboardTitle;										//排行榜标题
	list<tagFacBillboard>	m_FactionBillboard;								//成员数量排行榜

	list<DeclareWarFacInfo>		m_DecWarFationList;							//宣战的行会列表
	long	m_lDecWarFactionNum;
	long	m_lDecWarCurMaxPage;

	//保存所有行会
	set<tagAllFacState,KeyCompareByFaction>			m_AllFactions;			//当前系统的所有行会

	//内部操作接口
private:
	bool m_bIsSendGetOrgaData;
	//公共属性,提供给外部操作的接口
public:
	const list<tagApplyFaction>& GetFactionList( long lPage,LONG64 lNetSessionID,long lPassword);
	void SetApplyFaction(const CGUID& ID)	{m_ApplyFactionID=ID;	}
	const CGUID& GetApplyFaction()			{return m_ApplyFactionID;	}
	bool IsNextFactionPage(long lPage);

	list<DeclareWarFacInfo>& GetDecWarFactionList(long lPage,LONG64 lNetSessionID,long lPassword);
	bool IsNetDecWarFactionPage(long lPage);
	bool IsDecWarFaction(const CGUID& FactionID);					//是否可以向这个行会宣战
	void SetDecWarFactionRelation(const CGUID& FactionID,long lRel);	//设置一个宣战行会的关系

	void GetFactionStatInfo(set<tagAllFacState,KeyCompareByFaction>& Factions,long lPage,long lPageSize);//得到一页行会状态列表
	bool IsNextFactionFromAllFaction(long lPage,long lPageSize);

	const string& GetFactionBillboardTitle()	{return m_strFactionBillboardTitle;	}
	//得到行会排行榜
	const list<tagFacBillboard>& GetFactionBillboard() const {return m_FactionBillboard;}
		
	//添加一个行会组织
	void SetFactionOrganizing(CFaction* pOrgan);
	void DelFactionOrganizing();
	
	//得到某个行会组织
	CFaction* GetFactionOrganizing();						//根据组织ID得到相应的组织实例
	
	//添加一同盟组织
	void SetConfederationOrganizing(CUnion* pOrgan);
	void DelConfederationOrganizing();
	//得到某个同盟组织
	CUnion* GetConfederationOrganizing();				//根据组织ID得到相应的组织实例

	bool GetIsSendingGetOrgaData()	{return m_bIsSendGetOrgaData;}
	void SetIsSendingGetOrgaData(bool b){m_bIsSendGetOrgaData=b;}
	//对相关组织进行的操作
	bool ApplyForJoin(const CGUID& FactionID);															//无行会成员申请加入
	bool DoJoin(const CGUID& BeExecutedID);															//处理要加入组织的人员

	bool CreateFaction(long lPlayerID,long lLvl,tagTime& Time,string& strName = string(""));	//创建行会
	bool DisbandFaction(const CGUID& PlayerID,const CGUID& FactionID);										//解散行会
	bool CreateConfederation(long lTargFactionID,string& strName= string(""));					//创建同盟
	bool DisbandConferation(const CGUID& PlayerID,const CGUID& ConfeID);										//解散同盟
	bool TransferIOwnerCity(long lOwnedID,long lBeTransferedID,long lCiytID);					//转让主城
	bool BuyFactionLogo(long lPlayerID,long lIconID,tagTime& CurrTime);							//购买徽标

	long IsFactionMaster(long lPlayerID);										//判断该玩家是否是帮主
	long IsConferationMaster(long lFactionID);									//判断该帮是否是盟主
	long IsFreePlayer(long lPlayerID);											//判断该玩家是否不属于任何行会
	long IsFreeFaction(long lFactionID);										//判断该帮是否不属于任何同盟
	long RemovePersonFromApplyFactionList(long lPlayerID);						//从申请所有行会列表中移出一个申请人员
	long RemoveFactionFromApplyConfederList(long lFactionID);					//从申请所有联盟列表中移出一个申请组织
	bool IsAggressCityTime();													//得到当前是否是攻城战时间 

	//相应服务器的行会变化,删除一个行会
	void DelFaction(const CGUID& FactionID);
	//相应服务器的行会变化,添加一个行会
	void AddFaction(const CGUID& FactionID,char* strFactionName);
	string GetFactionNameByID(const CGUID& FactionID);									//得到行会的名字

	BYTE	GetFactionCountry(const CGUID& ID);

	bool	IsUnionRelation(const CGUID& FactionID);									//得到自己行会与此行会是否是同盟关系
	bool	IsEnemyRelationWithFactionWar(const CGUID& FactionID);						//是否是帮战的敌对关系
	bool	IsEnemyRelationWithCityWar(const CGUID& FactionID);						//判断是否是城战的敌对关系
	
	static COrganizingCtrl* getInstance();										//得到唯一实例句柄
	bool   Initialize();														//初始化
	void   Release();															//释放资源
	void   ReNew();

};

//得到组织控制类句柄
inline COrganizingCtrl* GetOrganiCtrl()
{
	return COrganizingCtrl::getInstance();
}