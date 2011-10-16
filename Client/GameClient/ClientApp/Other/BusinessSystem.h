#pragma once

class CCreditSystem;
class CBusinessSystem 
{
public:
	// 声望等级结构
	struct tagCreditLevel
	{
		long lMinNum;		//声望下限
		long lMaxNum;		//声望上限
		char LevelName[64];	//等级名称
	};

	//贸易区
	struct tagBusinessZone
	{
		string						strZoneName;			//贸易区名字
		WORD						wZoneID;				//贸易区ID
		WORD						wOccupyCountryID;		//军事占领国ID
		map<WORD/*各国家的ID*/,DWORD/*声望值*/>		map_CountryCredit;		//每个国家在此贸易区的声望
		vector<WORD>				vec_PointID;			//存放此贸易区下各贸易点的ID
	};

	//贸易点
	struct tagBusinessPoint
	{
		string			strPointName;	   					//(贸易点的名字)
		WORD			wPointID;		   					//(贸易点的ID)
		WORD			wSuperiorZoneID; 					//(从属的贸易区的ID)
		WORD			wBusinessAdscriptionID;  			//(商业归属的国家ID)
		map<WORD/*各国家的ID*/,DWORD/*投资总额*/>	map_CountryInvest;		//(每个国家在此贸易点的投资总额)
	};

	//某类物品当前卖NPC的价格信息
	struct  tagTradeGoodsInfor
	{
		DWORD dwGoodsIndex;									//物品的ID
		DWORD dwPrice;										//当前卖出价格
		int PriceScale;										// 物价比 PriceScale%
	};

	//一次投资中的信息
	struct tagInvestInfor 
	{
		CGUID NpcGUID;										//点击的NPC的GUID
		DWORD dwPointID;									//此次进行投资的贸易点的ID
		DWORD dwValue;										//此次投资的数量
		DWORD dwMaxValue;									//此次投资的上限
		tagInvestInfor()
		{
			NpcGUID = CGUID::GUID_INVALID;
			dwPointID = 0;
			dwValue = 0;
			dwMaxValue = 0;
		}
	};

	//声望区域
	struct tagAreaCredit
	{
		int id;
		DWORD dwAreaId;
		char chName[100];
		tagAreaCredit()
		{
			id=0;
			dwAreaId=0;
		}
	};

	CBusinessSystem();
	~CBusinessSystem() {}

	static map<WORD,CBusinessSystem::tagBusinessZone>* GetAllZonesInfor() {return &m_mapBusinessZones;}
	static map<WORD,CBusinessSystem::tagBusinessPoint>* GetAllPointInfor(){return &m_mapBusinessPoint;}
	static tagBusinessZone* GetZoneInfor(WORD wZoneID);
	static tagBusinessPoint* GetPointInfor(WORD wPointID);
	static bool CreateBusinessSystem();
	//static bool InitBusinessSystem();
	static void Release();
	static bool ResetSystem();
	static void UpdateZoneData(WORD wZoneID);
	static void UpdatePointData(WORD wPointID);
	static map<DWORD,tagTradeGoodsInfor>* GetTradeGoodsInforList() { return &m_mapTradeGoods; }
	static tagInvestInfor* GetInvestInfor()		{ return &m_tagInvest; }
	
	static bool InitCreditLevel(const char* filename);								//读取配置文件中的声望等级等信息
	static char* GetCreditLevelName(long lCurCreditValue);							//根据传入的声望值获取声望等级名称
	static long GetMaxCreditValue(long lCurCreditValue);							//根据传入的声望值获取当前等级的最大声望值
	static long GetCurCreditLevel(long lCurCreditValue);							//根据传入的声望值获取当前等级
private:
	static map<WORD/*各贸易区的ID*/,tagBusinessZone>  m_mapBusinessZones;			//各贸易区数据
	static map<WORD/*各贸易点的ID*/,tagBusinessPoint> m_mapBusinessPoint;			//各贸易点数据
	static CBusinessSystem* m_pBusinessSystem;										//唯一的BusinessSystem对象的指针
	static map<DWORD,tagTradeGoodsInfor> m_mapTradeGoods;							//当前所有物品卖某个NPC的价格表
	static tagInvestInfor m_tagInvest;												//玩家在某个贸易点的一次投资信息
	
	static vector<tagCreditLevel> m_vecCreditLevel;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	static size_t  GetAreaNumSize();                                                 //获得配置中区域的数量 
	static size_t  GetOrganNumSize();                                                 //获得配置中组织的数量 
	static bool InitCreditSystem();
	static long GetOrgeCreditMaxValue(DWORD dwOrganID,DWORD dwValue);                   //根据传入的组织ID 和对应的声望等级返回当前等级最大的声望值上限
	static const char* CreditOrganName(DWORD dwOrganID,DWORD dwValue);                //获取指定组织的指定声望值 所对应的名字
	static long CreditOrganLevel(DWORD dwOrganID,DWORD dwValue);                       //根据传入的组织id和声望值得到对应的声望等级
	static long MaxAreaValue(DWORD dwValue);                                          //获取指定等级的区域声望上限
	static long AreaLevel(DWORD dwValue);                                          //获取指定值获取声望的等级
	static const char* AreaCreditName(DWORD dwValue);                                 //根据声望值获取当前声望的显示名
	static const char* GetAreaName(int id);                                        //根据传入的索引id查找改区域的显示名
	static void ReleaseCreditSystem();                                        //释放声望系统资源
	static long GetMaxCreditByOrgan(long Id);                                  //查找一个组织声望的最大值
	static long MaxAreaCredit();                                               //获取区域声望的最大值
	static long GetMaxCreditLevelByOrgan(long id);                                    //获得一个组织的最大声望等级
	static long MaxAreaCreditLevel();                                          //获得区域的最大声望等级
	static const char* GetOrganName(DWORD dwOrganID);							//根据组织声望的ID获取此声望的名称
	static const char* GetDirtyName(DWORD dwOrganID);							//根据组织声望的ID获取此声望的名称
private:
	static map<int,tagAreaCredit> m_mapCreditArea;                                           //记录配置中ID对应的地图id
	static long m_lAreaNumSize;                                                      //配置文件中区域的数量，用于配置区域列表
	static CCreditSystem  *m_pCreditSystem;                                            //声望管理。 
};	