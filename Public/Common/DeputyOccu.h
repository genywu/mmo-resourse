/// 副职业系统: 重写该模块时必须把名称规范一下
/// 副职业在WOW中代表专业技能, 使用 PROFESSION 表示
/// --FOX SEAL 07212010--

/**
 *		文件名:		CDeputyOccu..h
 *		作者:		heliangchen
 *		时间:		2008/1/26
 *		简介:		副职业系统: 副职业系统的配置,用于副职业数据的查询
 *      Modify By:  Expter [1/25/ 2010 ] 
 */
#pragma once   

#include "BaseDef.h"
const int GOODSNAME_LEN_ =50;	//物品名字长度
const int NPCNAME_LEN_	 =50;	//npc名字的长度
const int E_DOCCU_NUM    = 3;	//副职业数量

struct tgGoods 
{
	char	cGoodsName[GOODSNAME_LEN_];
	ulong	dwGoodsIndex;
	int		iGoodNum;
	uchar	bMaker;
	tgGoods(){bMaker = 0;}
};

struct tgOddsGoods 
{
	char	cGoodsName[GOODSNAME_LEN_];
	ulong	dwGoodsIndex;
	int		iGoodNum;
	float	fOdds;
};

struct tgConditionGoods
{
	char	cGoodsName[GOODSNAME_LEN_];
	ulong	dwGoodsIndex;
	int		iNumType;
	int		iNum;
};

struct tgNpcDistance 
{
	char 	cNpcName[NPCNAME_LEN_];
	int		iDistace;
};
	
struct tagQuestGoods
{
	int nQuestId;
	ulong dwRoleStep;
};

///
/// 采集物配置
/// 
class CCollectionLayout
{

    typedef list<tgConditionGoods>::iterator CONDITIONGOODS_ITTER;
    typedef list<tgOddsGoods>::iterator ODDSGOODS_ITTER;
    typedef vector<tagQuestGoods>::iterator QUESTGOODS_ITER;

public:
	CCollectionLayout():m_byBlockType(0),m_dwActionID(1){}
	~CCollectionLayout(){}

	bool Init(TiXmlElement*);

	bool AddToByteArray(vector<uchar>* pByteArray);
	bool DecordFromByteArray(uchar* pByte, long& pos);

public:
	const string&	GetOrginName()const			{return m_strOrginName;}
	const string&	GetCollectionName()const	{return m_strCollectionName;}
	const string&	GetStartScriptName()const	{return m_strStartScriptName;}
	const string&	GetEndScriptName()const		{return m_strEndScriptName;}
	const string&   GetSuccScriptName()const	{return m_strSuccScriptName;}
	
	eDeputyOccu		GetOccuID()const			{return m_eOccuID;}

	ulong			GetPicID()const				{return m_dwPicID;}
	ulong			GetRadarID()const			{return m_dwRadarID;}
	ulong			GetMinTimes()const			{return	m_dwMixTimes;}
	ulong			GetMaxTime()const			{return m_dwMaxTimes;}
	ulong			GetCollectLevel()const		{return m_dwCollectLevel;}
	ulong			GetRoleID()const			{return 0;}
	ulong			GetRoleStep()const			{return 0;}
	uchar			GetBringBlock()const		{return m_byBlockType;}
	ulong			GetActionID()const			{return m_dwActionID;}
    ulong			GetCollectionID()const		{return m_dwCollectionID;}

public:
	const vector<tagQuestGoods>&	GetRoleInfo()const			{return m_vecQuestGoods;}
	const list<tgConditionGoods>&	GetConditionList()const		{return m_listGoodsCondition;}
	const list<tgOddsGoods>&		GetGainedList()const		{return m_listGainedGoods;}

protected:
	//原始名
	string			m_strOrginName;
	//采集物名字
	string			m_strCollectionName;	
	//采集物id
	ulong			m_dwCollectionID;
	//副职业id
	eDeputyOccu		m_eOccuID;
	//图形id
	ulong			m_dwPicID;
	//小地图标志(雷达id)
	ulong			m_dwRadarID;
	//最小采集次数
	ulong			m_dwMixTimes;
	//最大采集次数
	ulong			m_dwMaxTimes;
	//采集等级
	ulong			m_dwCollectLevel;
	vector<tagQuestGoods>	m_vecQuestGoods;
	//阻挡类型
	uchar			m_byBlockType;
	//采集开始时执行脚本名
	string			m_strStartScriptName;
	//采集完成时执行脚本名字
	string			m_strEndScriptName;
	//采集任务成功时执行的脚本
	string			m_strSuccScriptName;
	//任务采集时动作ID
	ulong			m_dwActionID;

protected:
	//物品条件列表
	list<tgConditionGoods>		m_listGoodsCondition;	
	//获得物品列表
	list<tgOddsGoods>			m_listGainedGoods;

};

///
/// 制作配置
///
class CFacture
{
public:
	CFacture():m_fQuotiety(1){}
	bool Init(TiXmlElement*);
	bool AddToByteArray(vector<uchar>* pByteArray);
	bool DecordFromByteArray(uchar* pByte, long& pos);
public:
	ulong			GetFactureID()const		{return m_dwFactureID;}
	const string&	GetFactureName()const	{return m_strFactureName;}
	eDeputyOccu		GetArtisanID()const		{return m_eArtisanID;}
	ulong			GetType()const			{return m_wType;}
	ulong			GetFactureLevel()const	{return m_dwFactureLevel;}
	float			GetQuotiety()const		{return m_fQuotiety;}
	const list<tgGoods>&		GetToolList()const			{return m_listTools;}
	const list<tgNpcDistance>&	GetNpcList()const			{return m_listNpcDis;}
	const list<tgGoods>&		GetMaterialList()const		{return m_listMaterial;}
	const list<tgGoods>&		GetGainedList()const		{return m_listGainedGoods;}
	const list<tgGoods>&		GetAdvanceGainedList()const	{return m_listAdvancedGainedGood;}
protected:
	//制造物id
	ulong		m_dwFactureID;
	//制造名字
	string		m_strFactureName;
	//工匠id
	eDeputyOccu m_eArtisanID;
	//产生的物品类型
	ulong		m_wType;
	//需要制作技能等级
	ulong		m_dwFactureLevel;
	//制作系数
	float		m_fQuotiety;
protected:
	// 工具列表
	list<tgGoods> m_listTools;
	// npc距离列表
	list<tgNpcDistance> m_listNpcDis;
	// 材料列表
	list<tgGoods> m_listMaterial;
	// 获取物品列表
	list<tgGoods> m_listGainedGoods;
	// 高级获取物品列表
	list<tgGoods> m_listAdvancedGainedGood;
public:
	typedef list<tgGoods>::iterator GOODS_ITTER;
	typedef list<tgNpcDistance>::iterator NPC_ITTER;
};
struct CGapExp
{
public:
	CGapExp()
	{
		MinLevelGap	=0;
		MaxLevelGap	=0;
		GapExp		=0;
		GapCredit	=0;
		Color		=0;
	}
	ulong TranslateColorCode(const char* chr);

public:
	long MinLevelGap;	//等级差最小值
	long MaxLevelGap;	//等级差最大值
	long GapExp;		//经验值
	long GapCredit;	    //威望值
	ulong Color;
};


struct CGradeExp 
{
	CGradeExp()
	{
		Level		=0;
		NeededExp	=0;
	}
	ulong Level;		//等级
	ulong NeededExp;	//该等级的升级经验
};


class CFactureStudy
{
public:
	CFactureStudy();
	~CFactureStudy();
	
    bool  Init(TiXmlElement*);
	bool  AddToByteArray(vector<uchar>* pByteArray);
	bool  DecordFromByteArray(uchar* pByte, long& pos);
	bool  FindFacture(ulong dwFactureID)const;
	ulong FindFactureCost(ulong dwFactureID)const;


public:
	const string& GetNpcName()const			{return m_strFactureNpcName;}
	ulong		  GetStudySum()const		{return (ulong)m_vecFactureList.size();}
	ulong		  GetFactureID(size_t studynum)const
	{
		if (m_vecFactureList.size()<= studynum)return 0;
		return m_vecFactureList[studynum].FactureID;
	}
	ulong		  GetFactureCost(ulong studynum)const
	{
		if (m_vecFactureList.size()<=studynum)return 0;
		return m_vecFactureList[studynum].Cost;
	}
private:
	struct study  
	{
		ulong FactureID;
		ulong Cost;
	};
	string m_strFactureNpcName;
	vector<study> m_vecFactureList;
};

struct DeputyOccuSkillRecord 
{
	DeputyOccuSkillRecord()
	{
		memset(CollectiontSkill,0,sizeof(CollectiontSkill));
		memset(FactureSkill,0,sizeof(FactureSkill));
		TaskSkill=0;
	}
	ulong CollectiontSkill[E_DOCCU_NUM];	//采集技能列表
	ulong FactureSkill[E_DOCCU_NUM];		//制作技能列表
	ulong TaskSkill;						//任务技能列表
};

class DOccuTypeLevel
{
public:
	DOccuTypeLevel(){Type=0; Level=0;}
	void Set(ulong type,ulong level)
    {   Type= type; Level= level;   }
	bool operator <(const DOccuTypeLevel& m)const
	{
		if (Type==m.Type)
		{
			return Level<m.Level;
		}
		return Type< m.Type;
	}

private:
	ulong Type;
	ulong Level;
};


class CDeputyOccuSystem
{ 

public:
    /// 副职业所拥有的行为类型
	enum OccuBeavioral
	{
		OB_Collection = 1,
		OB_Facture,
		OB_TaskCollection,
		OB_NUM
	};

public:
	CDeputyOccuSystem();
	~CDeputyOccuSystem();
	static bool Init(const char* filename);
	static bool Relese();

	static bool AddToByteArray(vector<uchar>* pByteArray);
	static bool DecordFromByteArray(uchar* pByte, long& pos);

public:

	/// 获取一个采集物的配置表（采集物名字）
	static const CCollectionLayout* GetCollectionLayout(const char* Name);
	/// 获取一个制作的配置表（制作的名字）
	static const CFacture*			GetFacture(ulong ID);
	//获取制作中高级物品获得的基础几率
	static		 ulong				GetAdvancedGainedBacePro(){return m_dwAdvancedGainedBacePro;}
	/// 获取采集一次的经验（人物采集等级与采集物的等级差）
	static		 long				GetCollectionExp(long level);
	/// 获取采集物的颜色（人物采集等级与采集物的等级差）
	static		 ulong				GetCollectionColor(long level);
	/// 获取此采集等级需要的经验（当前的人物采集等级）
	static		 ulong				GetCollectionNeededExp(ulong level);
	/// 取得一次制作的经验（人物制作与此制作的等级差）
	static		 long				GetFactureExp(long level);
	/// 取得一次制作的声望（人物制作与此制作的等级差）
	static		 long				GetFactureCredit(long level);
	/// 取得制作的颜色（人物制作与此制作的等级差）
	static		 ulong				GetFactureColor(long level);
	/// 取得当前制作等级的升级需要经验（当前人物制作等级）
	static		 ulong				GetFactureNeededExp(ulong level);
	/// 取得一个npc的制作条目（npc名字）
	static const CFactureStudy*		GetFactureStudy(const char*NpcName);
	/// 取得某等级
	static const vector<string>*	GetStudyStudyClew(ulong dwDOccuType,ulong dwLevel);		
	/// 取得某一制作类型id对应的制作类别名字（制作类别id）
	static const string*			GetFactureSortName(ulong FactureType);
	/// 取得采集最高等级
	static		ulong				GetMaxCollectionLevel(){return m_dwCollectMaxGrade;}
	/// 取得制作最高等级
	static		ulong				GetMaxFactureLevel(){return m_dwFactureMaxGrade;}
	/// 获取副职业技能
	static		ulong				GetDeputyOccuSkillID(OccuBeavioral type,eDeputyOccu DOccuType);
	/// 获取采集成功系统触发
	static      string&				GetCollectionSuccSpring(){return m_strCollectionSuccSpring;}
	/// 获取制作成功系统触发
	static		string&				GetFactureSuccSpring(){return m_strFactureSuccSpring;}


protected:

	/// 读取副职业系统的数据，分别初始化变量
	static bool InitCollectionList(const char* filename);
	static bool InitFactureList(const char* filename);
	static bool InitCollectionGapExpList(const char* filename);
	static bool InitFactureGapExpList(const char* filename);
	static bool InitCollectionGradeExpList(const char* filename);
	static bool InitFactureGradeExpList(const char* filename);
	static bool InitFactureStudyList(const char* filename);
	static bool	InitFactureSortList(const char* filename);
	static bool InitDeputyOccuSkillID(const char* filename);

protected:
	/// 释放资源
	static bool ReleseCollectionList();
	static bool ReleseFactureList();
	static bool ReleseCollectionGapExpList();
	static bool ReleseFactureGapExpList();
	static bool ReleseCollectionGradeExpList();
	static bool ReleseFactureGradeExpList();
	static bool ReleseFactureStudyList();
	static bool ReleseFactureSortList();
	static bool ReleseDeputyOccuSkillID();

protected:
	//采集列表（string是采集物的名字）
	static map<string,CCollectionLayout*>		m_mapCollectionList;
	//制作列表
	static map<ulong,CFacture*>			m_mapFactureList;
	//制作高级获得物品的基础几率
	static ulong						m_dwAdvancedGainedBacePro;
	//采集经验列表
	static list<CGapExp>				m_listCollectionGapExpList;
	//制作经验列表
	static list<CGapExp>				m_listFacturenGapExpList;
	//采集等级列表
	static vector<CGradeExp>			m_vecCollectionGradeExpList;
	//制作等级列表
	static vector<CGradeExp>			m_vecFactureGradeExpList;
	//制作学习列表（string是NPC的名字）
	static map<string,CFactureStudy*>	m_mapStudyList;
	//制作学习提示列表
	static map<DOccuTypeLevel,vector<string>>		m_mapStudyClew;
	//制作分类
	static map<ulong,string>			m_mapFactureSort;
	//采集的最高等级
	static ulong						m_dwCollectMaxGrade;
	//制作的最高等级
	static ulong						m_dwFactureMaxGrade;
	//副职业技能记录
	static DeputyOccuSkillRecord		m_tgDOccuSkillRecord;
	//采集成功系统触发
	static string						m_strCollectionSuccSpring;
	//制作成功系统触发
	static string						m_strFactureSuccSpring;
};
