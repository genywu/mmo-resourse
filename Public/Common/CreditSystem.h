///
/// 声望系统: 重写该模块时必须把名称规范一下
/// credit 主要指信用, 声誉; reputation 才是指声望, 威望
/// --FOX SEAL 07212010--
/// Modify By:Expter [1/25/ 2010 ] 
/// 
#pragma once

const char MAXAREANAMESIZE  =32;
const char MAXCREDITNAMESIZE=16;

typedef struct  
{
	int   _iIndex;                       //索引，现在服务器上是通过此索引区分区域	
	ulong _dwAreaID;                     //区域ID
	char  _cName[MAXAREANAMESIZE];       //区域名字
}AREA;


//世界区域声望
typedef struct  
{
	ulong _dwValue;                      //声望值
	ulong _dwLevelID;                    //等级
	char  _cName[MAXCREDITNAMESIZE];     //名称
}CREDITLEVEL;
	

//世界组织声望
typedef struct  
{
	ulong _dwOrganizeID;                //组织ID
	char _cDirtyName[MAXAREANAMESIZE];  //玩家脏数据中的如BatakCredit字段名称
	char _cName[MAXAREANAMESIZE];       //声望名称
	vector<CREDITLEVEL> _vecLevel;      //等级列表
}ORGANCREDIT;

class CCreditSystem
{
    typedef map<ulong,ORGANCREDIT>::iterator ORGANITR;
    typedef vector<CREDITLEVEL>::iterator	CREDITITR;

public:
	CCreditSystem(void);
	~CCreditSystem(void);
	bool LoadAreaCredit();
    void Release(){m_mapTradeAreas.clear();m_vecCreditLevel.clear(); }
	long CreditLevel(ulong dwValue);                                //根据声望值获取区域声望等级
	const char* CreditName(ulong dwValue);                          //根据声望值获取区域声望名字
	long MaxAreaCreditLevel();                                      //最高等级声望
	long MaxAreaCredit();                                           //最高等级声望


	//服务器用
	bool CodeToByteArray(DBWriteSet& dbWriteSet);
	bool DecodeToByteArray(DBReadSet& dbReadSet);
	const char* GetDirtyName(ulong dwOrganID);		                //获取脏数据字段名
	long MaxOrganValByDirtyName(const char* strDirtyName);          //指字组织的最高等级声望
	long CreditOrganLeveyByDirtyName(const char* strDirtyName,
                                     ulong dwValue);                //通过脏数据名字得组织等级
	
	//客户端用
	long MaxAreaValue(ulong dwLevel);                               //指定等级的区域最大声望
	long MaxOrganValue(const char* strName);                        //指字组织的最高等级声望
	long MaxOrganValue(ulong dwOrganID,ulong dwLevel);              //指定组织指定等级的最大声望
    long  GetMaxCreditByOrgan(ulong dwOrganID);                     //获得一个组织的最大声望值
    long  GetMaxCreditLevelByOrgan(ulong dwOrganID);                //获得一个组织的最大声望等级
    long  CreditOrganLevelByName(const char* strName,ulong dwValue);//通过声望名字得组织等级
    long  CreditOrganLevel(ulong dwOrganID,ulong dwValue);          //获取指定组织的指定声望值 所对应的等级


    const char* CreditAreaNameByIndex(int index);                   //通过索引查找区域名
	const char* CreditAreaNameByID(ulong dwID);                     //通过地图id查找区域名
	const char* GetOrganName(ulong dwOrganID);			            //获取声望名称
    const char* CreditOrganName(ulong dwOrganID,ulong dwValue);     //获取指定组织的指定声望值 所对应的名字

	size_t   GetCreditAreaSize();                                      //获得声望区域的数量
	size_t   GetCreditOrganSize();                                     //获得声望组织的数量
	

private:
     //载入组织声望
	bool LoadOrganizeCredit(TiXmlElement* p);                  
	bool LoadCreditLevel(TiXmlElement* p,
                         vector<CREDITLEVEL>& vecCreditLevel);

    CREDITLEVEL* GetLevel(ulong dwValue ,
                          vector<CREDITLEVEL>& vecCreditLevel);
    CREDITLEVEL* GetOrganLevel(ulong dwOrganID,ulong dwLevel);

    //获取组织ID
	long GetOrganID(const char* Name);                          
	long GetOrganIDByDirtyName(const char* DirtyName);

private:

	map<ulong, AREA >               m_mapTradeAreas;
	vector<CREDITLEVEL>             m_vecCreditLevel;
	map<ulong,ORGANCREDIT>          m_mapOrganCredit;
};
