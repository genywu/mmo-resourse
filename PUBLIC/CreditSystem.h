#pragma once

#include "tinystr.h"
#include "tinyxml.h"
#include <map>
#include <vector>
const char MAXAREANAMESIZE=32;
const char MAXCREDITNAMESIZE=16;
typedef struct  
{
	int  _iIndex;//索引，现在服务器上是通过此索引区分区域	
	DWORD _dwAreaID;//区域ID
	char _cName[MAXAREANAMESIZE];//区域名字
}AREA;

//世界区域声望
typedef struct  
{
	DWORD _dwValue;//声望值
	DWORD _dwLevelID;//等级
	char _cName[MAXCREDITNAMESIZE];//名称
}CREDITLEVEL;
	
//世界组织声望
typedef struct  
{
	DWORD _dwOrganizeID;//组织ID
	char _cDirtyName[MAXAREANAMESIZE];//玩家脏数据中的如BatakCredit字段名称
	char _cName[MAXAREANAMESIZE];//声望名称
	vector<CREDITLEVEL> _vecLevel;//等级列表
}ORGANCREDIT;

class CCreditSystem
{
public:
	CCreditSystem(void);
	~CCreditSystem(void);
	bool LoadAreaCredit();
	void Release(){m_mapTradeAreas.clear();m_vecCreditLevel.clear();}
	typedef map<DWORD,ORGANCREDIT>::iterator ORGANITR;
	typedef vector<CREDITLEVEL>::iterator	CREDITITR;
	long CreditLevel(DWORD dwValue);//根据声望值获取区域声望等级
	const char* CreditName(DWORD dwValue);//根据声望值获取区域声望名字
	long MaxAreaCreditLevel();//最高等级声望
	long MaxAreaCredit();//最高等级声望
	//服务器用
	bool CodeToByteArray(DBWriteSet& dbWriteSet);
	bool DecodeToByteArray(DBReadSet& dbReadSet);
	const char* GetDirtyName(DWORD dwOrganID);		 //获取脏数据字段名
	long MaxOrganValByDirtyName(const char* strDirtyName);//指字组织的最高等级声望
	long CreditOrganLeveyByDirtyName(const char* strDirtyName,DWORD dwValue);//通过脏数据名字得组织等级
	
	//客户端用
	long CreditOrganLevelByName(const char* strName,DWORD dwValue);//通过声望名字得组织等级
	long MaxAreaValue(DWORD dwLevel);//指定等级的区域最大声望
	long MaxOrganValue(const char* strName);//指字组织的最高等级声望
	const char* CreditOrganName(DWORD dwOrganID,DWORD dwValue);//获取指定组织的指定声望值 所对应的名字
	long CreditOrganLevel(DWORD dwOrganID,DWORD dwValue);//获取指定组织的指定声望值 所对应的等级
	long MaxOrganValue(DWORD dwOrganID,DWORD dwLevel);//指定组织指定等级的最大声望
	const char* CreditAreaNameByIndex(int index);         //通过索引查找区域名
	const char* CreditAreaNameByID(DWORD dwID);           //通过地图id查找区域名
	const char* GetOrganName(DWORD dwOrganID);			  //获取声望名称
	int GetCreditAreaSize();                              //获得声望区域的数量
	int GetCreditOrganSize();                              //获得声望组织的数量
	long  GetMaxCreditByOrgan(DWORD dwOrganID);             //获得一个组织的最大声望值
	long  GetMaxCreditLevelByOrgan(DWORD dwOrganID);             //获得一个组织的最大声望等级


private:
	bool LoadOrganizeCredit(TiXmlElement* p);//载入组织声望
	bool LoadCreditLevel(TiXmlElement* p,vector<CREDITLEVEL>& vecCreditLevel);
	CREDITLEVEL* GetLevel(DWORD dwValue,vector<CREDITLEVEL>& vecCreditLevel);
	CREDITLEVEL* GetOrganLevel(DWORD dwOrganID,DWORD dwLevel);
	long GetOrganID(const char* Name);//获取组织ID
	long GetOrganIDByDirtyName(const char* DirtyName);
	map<DWORD,AREA>m_mapTradeAreas;
	vector<CREDITLEVEL>m_vecCreditLevel;
	map<DWORD,ORGANCREDIT>m_mapOrganCredit;
};
