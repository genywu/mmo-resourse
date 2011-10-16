//		文件名:		CDeputyOccu..h
//		作者:		heliangchen
//		时间:		2008/1/26
//		简介:		副职业系统: 副职业系统的配置,用于副职业数据的查询
#pragma once
#include"BaseDef.h"
#include "tinystr.h"
#include "tinyxml.h"

const int GOODSNAME_LEN_ =50;	//物品名字长度
const int NPCNAME_LEN_	=50;	//npc名字的长度
const int E_DOCCU_NUM= 3;		//副职业数量

struct tgGoods 
{
	char	cGoodsName[GOODSNAME_LEN_];
	DWORD	dwGoodsIndex;
	int		iGoodNum;
	BYTE	bMaker;
	tgGoods(){bMaker = 0;}
};

struct tgOddsGoods 
{
	char	cGoodsName[GOODSNAME_LEN_];
	DWORD	dwGoodsIndex;
	int		iGoodNum;
	float	fOdds;
};

struct tgConditionGoods
{
	char	cGoodsName[GOODSNAME_LEN_];
	DWORD	dwGoodsIndex;
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
	DWORD dwRoleStep;
};
//采集物配置
class CCollectionLayout
{
public:
	CCollectionLayout():m_byBlockType(0),m_dwActionID(1){}
	~CCollectionLayout(){}
	BOOL Init(TiXmlElement*);
	BOOL AddToByteArray(vector<BYTE>* pByteArray);
	BOOL DecordFromByteArray(BYTE* pByte, long& pos);
public:
	const string&	GetOrginName()const			{return m_strOrginName;}
	const string&	GetCollectionName()const	{return m_strCollectionName;}
	const string&	GetStartScriptName()const	{return m_strStartScriptName;}
	const string&	GetEndScriptName()const		{return m_strEndScriptName;}
	const string&   GetSuccScriptName()const	{return m_strSuccScriptName;}
	DWORD			GetCollectionID()const		{return m_dwCollectionID;}
	eDeputyOccu		GetOccuID()const			{return m_eOccuID;}
	DWORD			GetPicID()const				{return m_dwPicID;}
	DWORD			GetRadarID()const			{return m_dwRadarID;}
	DWORD			GetMinTimes()const			{return	m_dwMixTimes;}
	DWORD			GetMaxTime()const			{return m_dwMaxTimes;}
	DWORD			GetCollectLevel()const		{return m_dwCollectLevel;}
	DWORD			GetRoleID()const				{return 0;}
	DWORD			GetRoleStep()const			{return 0;}
	BYTE			GetBringBlock()const		{return m_byBlockType;}
	DWORD			GetActionID()const			{return m_dwActionID;}
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
	DWORD			m_dwCollectionID;
	//副职业id
	eDeputyOccu		m_eOccuID;
	//图形id
	DWORD			m_dwPicID;
	//小地图标志(雷达id)
	DWORD			m_dwRadarID;
	//最小采集次数
	DWORD			m_dwMixTimes;
	//最大采集次数
	DWORD			m_dwMaxTimes;
	//采集等级
	DWORD			m_dwCollectLevel;
	////需要任务ID
	//int				m_iRoleID;
	////需要任务步骤
	//DWORD			m_dwRoleStep;
	vector<tagQuestGoods>	m_vecQuestGoods;
	//阻挡类型
	BYTE			m_byBlockType;
	//采集开始时执行脚本名
	string			m_strStartScriptName;
	//采集完成时执行脚本名字
	string			m_strEndScriptName;
	//采集任务成功时执行的脚本
	string			m_strSuccScriptName;
	//任务采集时动作ID
	DWORD			m_dwActionID;
protected:
	//物品条件列表
	list<tgConditionGoods>		m_listGoodsCondition;	
	//获得物品列表
	list<tgOddsGoods>			m_listGainedGoods;
public:
	typedef list<tgConditionGoods>::iterator CONDITIONGOODS_ITTER;
	typedef list<tgOddsGoods>::iterator ODDSGOODS_ITTER;
	typedef vector<tagQuestGoods>::iterator QUESTGOODS_ITER;
};

//制作配置
class CFacture
{
public:
	CFacture():m_fQuotiety(1){}
	BOOL Init(TiXmlElement*);
	BOOL AddToByteArray(vector<BYTE>* pByteArray);
	BOOL DecordFromByteArray(BYTE* pByte, long& pos);
public:
	DWORD			GetFactureID()const		{return m_dwFactureID;}
	const string&	GetFactureName()const	{return m_strFactureName;}
	eDeputyOccu		GetArtisanID()const		{return m_eArtisanID;}
	DWORD			GetType()const			{return m_wType;}
	DWORD			GetFactureLevel()const	{return m_dwFactureLevel;}
	float			GetQuotiety()const		{return m_fQuotiety;}
	const list<tgGoods>&		GetToolList()const			{return m_listTools;}
	const list<tgNpcDistance>&	GetNpcList()const			{return m_listNpcDis;}
	const list<tgGoods>&		GetMaterialList()const		{return m_listMaterial;}
	const list<tgGoods>&		GetGainedList()const		{return m_listGainedGoods;}
	const list<tgGoods>&		GetAdvanceGainedList()const	{return m_listAdvancedGainedGood;}
protected:
	//制造物id
	DWORD		m_dwFactureID;
	//制造名字
	string		m_strFactureName;
	//工匠id
	eDeputyOccu m_eArtisanID;
	//产生的物品类型
	DWORD		m_wType;
	//需要制作技能等级
	DWORD		m_dwFactureLevel;
	//制作系数
	float		m_fQuotiety;
protected:
	//工具列表
	list<tgGoods> m_listTools;
	//npc距离列表
	list<tgNpcDistance> m_listNpcDis;
	//材料列表
	list<tgGoods> m_listMaterial;
	//获取物品列表
	list<tgGoods> m_listGainedGoods;
	//高级获取物品列表
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
	DWORD TranslateColorCode(const char* chr);

public:
	LONG MinLevelGap;	//等级差最小值
	LONG MaxLevelGap;	//等级差最大值
	LONG GapExp;		//经验值
	LONG GapCredit;	//威望值
	DWORD Color;
};


struct CGradeExp 
{
	CGradeExp()
	{
		Level		=0;
		NeededExp	=0;
	}
	DWORD Level;		//等级
	DWORD NeededExp;	//该等级的升级经验
};


class CFactureStudy
{
public:
	CFactureStudy();
	~CFactureStudy();
	BOOL Init(TiXmlElement*);
	BOOL AddToByteArray(vector<BYTE>* pByteArray);
	BOOL DecordFromByteArray(BYTE* pByte, long& pos);
	BOOL FindFacture(DWORD dwFactureID)const;
	DWORD FindFactureCost(DWORD dwFactureID)const;
public:
	const string& GetNpcName()const			{return m_strFactureNpcName;}
	DWORD		  GetStudySum()const		{return m_vecFactureList.size();}
	DWORD		  GetFactureID(DWORD studynum)const
	{
		if (m_vecFactureList.size()<= studynum)return 0;
		return m_vecFactureList[studynum].FactureID;
	}
	DWORD		  GetFactureCost(DWORD studynum)const
	{
		if (m_vecFactureList.size()<=studynum)return 0;
		return m_vecFactureList[studynum].Cost;
	}
private:
	struct study  
	{
		DWORD FactureID;
		DWORD Cost;
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
	DWORD CollectiontSkill[E_DOCCU_NUM];	//采集技能列表
	DWORD FactureSkill[E_DOCCU_NUM];		//制作技能列表
	DWORD TaskSkill;						//任务技能列表
};

class DOccuTypeLevel
{
public:
	DOccuTypeLevel(){Type=0; Level=0;}
	void Set(DWORD type,DWORD level){Type= type; Level= level;}
	bool operator <(const DOccuTypeLevel& m)const
	{
		if (Type==m.Type)
		{
			return Level<m.Level;
		}
		return Type< m.Type;
	}
private:
	DWORD Type;
	DWORD Level;
};


class CDeputyOccuSystem
{
public:
	// 副职业所拥有的行为类型
	enum OccuBeavioral
	{
		OB_Collection = 1,
		OB_Facture,
		OB_TaskCollection,

		OB_NUM
	};

	CDeputyOccuSystem();
	~CDeputyOccuSystem();
	static BOOL Init(const char* filename);
	static BOOL Relese();
	static BOOL AddToByteArray(vector<BYTE>* pByteArray);
	static BOOL DecordFromByteArray(BYTE* pByte, long& pos);
public:

	//获取一个采集物的配置表（采集物名字）
	static const CCollectionLayout* GetCollectionLayout(const char* Name);
	//获取一个制作的配置表（制作的名字）
	static const CFacture*			GetFacture(DWORD ID);
	//获取制作中高级物品获得的基础几率
	static		 DWORD				GetAdvancedGainedBacePro(){return m_dwAdvancedGainedBacePro;}

	//获取采集一次的经验（人物采集等级与采集物的等级差）
	static		 LONG				GetCollectionExp(LONG level);
	//获取采集物的颜色（人物采集等级与采集物的等级差）
	static		 DWORD				GetCollectionColor(LONG level);
	//获取此采集等级需要的经验（当前的人物采集等级）
	static		 DWORD				GetCollectionNeededExp(DWORD level);
	//取得一次制作的经验（人物制作与此制作的等级差）
	static		 LONG				GetFactureExp(LONG level);
	//取得一次制作的声望（人物制作与此制作的等级差）
	static		 LONG				GetFactureCredit(LONG level);
	//取得制作的颜色（人物制作与此制作的等级差）
	static		 DWORD				GetFactureColor(LONG level);
	//取得当前制作等级的升级需要经验（当前人物制作等级）
	static		 DWORD				GetFactureNeededExp(DWORD level);
	//取得一个npc的制作条目（npc名字）
	static const CFactureStudy*		GetFactureStudy(const char*NpcName);
	//取得某等级
	static const vector<string>*	GetStudyStudyClew(DWORD dwDOccuType,DWORD dwLevel);		
	//取得某一制作类型id对应的制作类别名字（制作类别id）
	static const string*			GetFactureSortName(DWORD FactureType);
	//取得采集最高等级
	static		DWORD				GetMaxCollectionLevel(){return m_dwCollectMaxGrade;}
	//取得制作最高等级
	static		DWORD				GetMaxFactureLevel(){return m_dwFactureMaxGrade;}
	//获取副职业技能
	static		DWORD				GetDeputyOccuSkillID(OccuBeavioral type,eDeputyOccu DOccuType);
	//获取采集成功系统触发
	static      string&				GetCollectionSuccSpring(){return m_strCollectionSuccSpring;}
	//获取制作成功系统触发
	static		string&				GetFactureSuccSpring(){return m_strFactureSuccSpring;}
protected:
	//读取副职业系统的数据，分别初始化变量

	static BOOL InitCollectionList(const char* filename);
	static BOOL InitFactureList(const char* filename);
	static BOOL InitCollectionGapExpList(const char* filename);
	static BOOL InitFactureGapExpList(const char* filename);
	static BOOL InitCollectionGradeExpList(const char* filename);
	static BOOL InitFactureGradeExpList(const char* filename);
	static BOOL InitFactureStudyList(const char* filename);
	static BOOL	InitFactureSortList(const char* filename);
	static BOOL InitDeputyOccuSkillID(const char* filename);
protected:
	//释放资源

	static BOOL ReleseCollectionList();
	static BOOL ReleseFactureList();
	static BOOL ReleseCollectionGapExpList();
	static BOOL ReleseFactureGapExpList();
	static BOOL ReleseCollectionGradeExpList();
	static BOOL ReleseFactureGradeExpList();
	static BOOL ReleseFactureStudyList();
	static BOOL ReleseFactureSortList();
	static BOOL ReleseDeputyOccuSkillID();
protected:
	//采集列表（string是采集物的名字）
	static map<string,CCollectionLayout*>		m_mapCollectionList;
	//制作列表
	static map<DWORD,CFacture*>			m_mapFactureList;
	//制作高级获得物品的基础几率
	static DWORD						m_dwAdvancedGainedBacePro;
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
	static map<DWORD,string>			m_mapFactureSort;
	//采集的最高等级
	static DWORD						m_dwCollectMaxGrade;
	//制作的最高等级
	static DWORD						m_dwFactureMaxGrade;
	//副职业技能记录
	static DeputyOccuSkillRecord		m_tgDOccuSkillRecord;
	//采集成功系统触发
	static string						m_strCollectionSuccSpring;
	//制作成功系统触发
	static string						m_strFactureSuccSpring;
	

};
