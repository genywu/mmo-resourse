
#pragma once
#include "..\..\public\BaseDef.h"
#include "..\..\public\tinystr.h"
#include "..\..\public\tinyxml.h"


//enum ROLE_TYPE{MOSTLY_ROLE=1,BAYOU_ROLE,ENGAGEARMS_ROLE};//任务类型（主线，支线，佣兵）

const int LABELPREFIX_NUM=5;							//前置任务数量

enum AIM_MODEL_TYPE{AMT_KILL=0,AMT_COLLECT,AMT_TALK,AMT_ESCORT,AMT_KILL_COLLECT};//目标模式类型

const LONG	PREPARE_PARAM_NUM=10;						//后备变量数（aim）
const LONG  STEP_AIM_NUM=10;							//步骤目标最大数量
const LONG  STEP_ENCOURAGEMENT_NUM=10;					//步骤获取物品最大数量
const LONG  QUEST_STEP_NUM=20;							//任务最大步骤数
const LONG  QUEST_ENCOURAGEMENT_NUM=20;					//任务最大奖励数量

const LONG  GOODS_TYPE_INDEX=1;							//任务配置中的物品类型标识

const int QUEST_MAX_ODDS = 20;						//任务的最大稀有度值
const int MAX_MERC_QUEST_NUM = 4;					//单个列表ID中最大的俑兵任务数量
//任务属性
class CQuestAttribute
{
public:
	CQuestAttribute();
	~CQuestAttribute(){}
	BOOL Init(TiXmlElement*);
public:
//取得值的接口
	//取得任务的名字
	const char* GetName()const{return m_cName;}
	//取得任务id
	LONG GetID()const{return m_lID;}
	//是否被记录
	LONG GetBeRecord()const{return m_bBeRecord;}
	//任务类型
	LONG GetRoleType()const{return m_lRoleType;}
	//任务接受npc的id
	LONG GetEmbracerNPCID()const{return m_lEmbracerNPCID;}
	//是否被显示
	LONG GetShowAble()const{return m_bShowAble;}
	//是否能反复接
	LONG GetRepeated()const{return m_bRepeated;}
	//死亡是否失败
	LONG GetDeadLost()const{return m_bDeadLost;}
	//任务失败后能否重新接
	LONG GetLostRepeated()const{return m_bLostRepeated;}
	//最小等级限制
	LONG GetMinGrade()const{return m_lMinGrade;}
	//最大等级限制
	LONG GetMaxGrade()const{return m_lMaxGrade;}
	//是否受经验倍率影响
	LONG GetExpRatio()const{return m_bExpRatio;}
	//职业限制
	BOOL GetOccupationRestrict(INT index)const
	{
		if (index>=0 && index< OCC_Max)
		{
			return (m_Occupation[index/32] & (1<<(index%32)))==0 ? 0:1 ;
		}
		else 
			return FALSE;
	}
	//任务几率
	LONG GetOdds()const{return m_lOdds;}
	//需要佣兵声望
	LONG GetEngageArmsRepute()const{return m_lEngageArmsRepute;}
	//下一个任务的id
	LONG GetNextRoleID()const{return m_lNextRoleID;}
	//章节数
	LONG GetChapterNum()const{return m_lChapterNum;};
	//
	LONG GetFinishEffect()const{return m_lFinishEffect;}
	//
	const char* GetDeleteScript()const{return m_cDeleteScript;}

	LONG GetMassPoint() const	{return m_lMassPoint;}
private:
	//解析   职业限制
	VOID ParseOccupation(const char *str);
	int  GetNumInParse(char*&);
private:
	char		m_cName[50];
	LONG		m_lID;							//任务id
	LONG		m_lRoleType;					//任务类型
	LONG		m_lEmbracerNPCID;				//任务接受npc的id

	BOOL		m_bBeRecord;					//是否被记录
	BOOL		m_bShowAble;					//能否被显示
	BOOL		m_bRepeated;					//能否反复接受
	BOOL		m_bDeadLost;					//死亡任务失败
	BOOL		m_bLostRepeated;				//失败重接开关
	BOOL		m_bExpRatio;					//是否受经验倍率影响

	LONG		m_lMinGrade;					//等级下限
	LONG		m_lMaxGrade;					//等级上限
	int 		m_Occupation[OCC_Max/32 +1];	//职业限制 b
	LONG		m_lOdds;						//任务稀有程度
	LONG		m_lEngageArmsRepute;			//需要佣兵声望
	
	LONG		m_lNextRoleID;					//下一个任务的id
	LONG		m_lFinishEffect;				//任务完成效果
	LONG		m_lChapterNum;					//章节数
	char        m_cDeleteScript[50];			//删除任务脚本

	LONG		m_lMassPoint;					//集结点
};

//前置任务
class CQuestLabelPrefix
{
public:
	CQuestLabelPrefix();
	~CQuestLabelPrefix(){}
	BOOL Init(TiXmlElement*);
public:
	//取得index对应的前置任务
	LONG GetLabelPrefix(LONG Index)const;
	//加入一个前置任务id记录
	BOOL AddLabelPrefix(LONG val);
	//取得前置任务数量
	LONG GetLabelSum()const{return m_lLabelSum;}
private:
	LONG		m_lLabelSum;						//前置任务数量
	LONG		m_lLabelPrefix[LABELPREFIX_NUM];	//前置任务列表
};
//任务目标
class CQuestAim
{
public:
	CQuestAim();
	~CQuestAim(){}
	BOOL Init(TiXmlElement*);
public:
//取值接口
	//取得id
	LONG GetID()const					{return m_lID;}
	//目标模板类型
	AIM_MODEL_TYPE	GetAMTType()const	{return m_AMTType;}
	//目标类型
	LONG GetAimType()const				{return m_lAimType;}
	//取得目标id
	LONG GetAimId()const				{return m_lAimID;}
	//取得目标数量
	LONG GetAimNum()const				{return m_lAimNum;}
	//取得 变量索引
	LONG GetParameIndex()const			{return m_lParamIndex;}
	//凋落这种物品的生物类型（对物品）
	LONG GetDropFrom()const				{return m_lDropFrom;}
	//几率
	LONG GetDropOdds()const				{return m_lDropOdds;}
	//取得目标的名字
	const char* GetName()const			{return (char*)&m_cAimName[0];}

	//取得备用属性
	LONG GetPrepareParam(LONG index)const
	{
		if (index>=0 && index <PREPARE_PARAM_NUM)
		{
			return  m_lPrepareParam[index];
		}
		else
			return 0;
	}

private:
	LONG			m_lID;					//id
	AIM_MODEL_TYPE	m_AMTType;				//模板类型
	LONG			m_lAimType;				//目标类型
	char			m_cAimName[50];			//目标名字
	LONG			m_lAimID;				//目标id
	LONG			m_lAimNum;				//目标数量
	LONG			m_lParamIndex;			//变量索引
	LONG			m_lDropFrom;			//掉落对象
	LONG			m_lDropOdds;			//几率
	LONG			m_lPrepareParam[PREPARE_PARAM_NUM];		//备用属性列表
public:
	CQuestAim* next;
};
//任务奖励
class CQusetEncouragement
{
public:
	CQusetEncouragement();
	~CQusetEncouragement(){}
	BOOL Init(TiXmlElement*);
public:
	//取得奖励的名字
	const char* GetName()const{return m_Name;}
	//取得奖励类型
	LONG GetType()const{return m_lType;}
	//奖励的具体id
	LONG GetID()const{return m_lID;}
	//奖励数量
	LONG GetNum()const{return m_lNum;}
private:
	char m_Name[50];		//奖励物的名字
	LONG m_lType;			//类型
	LONG m_lID;				//id
	LONG m_lNum;			//数量
public:
	CQusetEncouragement *next;
};
//任务步骤
class CQuestStep
{
public:
	CQuestStep();
	~CQuestStep();
	BOOL Init(TiXmlElement*);
	BOOL AddToByteArray(vector<BYTE>* pByteArray);
	BOOL DecordFromByteArray(BYTE* pByte, long& pos);
private:
	struct SQuestText
	{
		LONG TextID;
		LONG TextIndex;
	};
public:
	//
	LONG GetStepAimNum()const{return m_lAimNum;}
	//取得步骤id
	LONG GetStepID()const{return m_lStepID;}
	//取得textid 对应的text的索引
	LONG GetTextIndex(LONG lTextID);
	//
	LONG GetRolePropSum()const{return m_lRolePropNum;}
	//取得奖励列表的头指针
	const CQusetEncouragement* GetRolePropHead()const{return m_RoleProp;}
	//获取奖励物品所占格子数
	LONG GetRolePropCell()const{return m_lRolePropCell;}
	//取得目标
	const CQuestAim* GetAim(LONG ID)const;
	//取得奖励列表的头指针
	const CQuestAim* GetAimHead()const{return m_Aim;}
private:
	//加入一个目标
	BOOL AddAim(CQuestAim*&);
	//加入一个（获得品）
	BOOL AddEncouragement(CQusetEncouragement*&);
private:
	LONG				m_lStepID;				//步骤id
	LONG				m_lRolePropNum;			//任务奖励数量
	LONG				m_lRolePropCell;	//任务所占格子数
	CQusetEncouragement*m_RoleProp;				//步骤奖励列表头
	LONG				m_lAimNum;				//任务目标数量
	CQuestAim*			m_Aim;					//任务目标头指针
	vector<SQuestText>   m_vecQuestText;		//任务目标的文本
public:
	CQuestStep* next;
};
//单个任务
class CQuest
{
public:
	CQuest();
	~CQuest();
	BOOL Init(TiXmlElement* ,LONG lTextType);
	BOOL AddToByteArray(vector<BYTE>* pByteArray);
	BOOL DecordFromByteArray(BYTE* pByte, long& pos);
public:
	LONG GetTextType()const{return m_lTextType;}
	LONG GetID()const{return m_Attribute.GetID();}
	const CQuestAttribute& GetAttribute()const{return m_Attribute;}
	const CQuestLabelPrefix& GetLabelPrefix()const{return m_LabelPrefix;}
	LONG GetStepSum()const{return m_lStepSum;}
	const CQuestStep*	GetStep(LONG ID=0x7fffffff)const;
	LONG GetEncouragementSum()const{return m_lEncouragementSum;}
	CQusetEncouragement* GetEncouragementHead()const{return m_Encouragement;}
	LONG GetEncouragementCell()const{return m_lEncouragementCell;}
private:
	BOOL AddStep(CQuestStep *& step);
	BOOL AddEncouragement(CQusetEncouragement *& Encouragement);
private:
	LONG					m_lTextType;				//文本类型索引
	CQuestAttribute			m_Attribute;				//属性

	CQuestLabelPrefix		m_LabelPrefix;				//前置任务

	LONG					m_lStepSum;					//任务总步骤数
	CQuestStep*				m_Step;						//任务步骤列表

	LONG					m_lEncouragementSum;		//任务总奖励数
	LONG					m_lEncouragementCell;		//任务奖励所占的格子总数
	CQusetEncouragement*	m_Encouragement;			//任务奖励列表
};

//任务列表
class CQuestSystem
{
	CQuestSystem();
	~CQuestSystem();

	struct tagQuestList
	{
		long lTextType;//字符资源类型
		long lTitleTextID;//标题字符资源ID
		long lType;	//任务组类型： -1 错误（不存在任务组） 0 一般任务组 1国家任务组 2佣兵任务组 
		DWORD dwFreshTime;//刷新时间
		//该列表下的所有任务
		map<LONG,CQuest*>	Quests;
	};
public:
	static BOOL Init(const char* filename);
	static BOOL InitAttribute(TiXmlElement* p);
	static BOOL InitOneFile(const char* filename,tagQuestList& QuestList);
	static BOOL Relese();
	static BOOL AddToByteArray(vector<BYTE>* pByteArray);
	static BOOL DecordFromByteArray(BYTE* pByte, long& pos);
	//任务接口
	static const  CQuest*			GetQuest(LONG ID);
	static		  LONG				GetMainRoleByChapterID(LONG chapterID);
	static const  CQuestStep*		GetStep(LONG ID,LONG stepID);
	static const  CQuestAim*	GetAim(LONG ID,LONG step,LONG aimID);
	static const  CQusetEncouragement* GetEncouragementHead(LONG ID);
	static const  CQusetEncouragement* GetStepEncouragementHead(LONG ID,LONG stepID);
	//
	//static const char* GetPlayerLoginScript()	{return 0;}//m_strPlayerLoginScript.c_str();}	//得到该玩家的登陆脚本
	//static const char* GetPlayerUpLevelScrip()	{return 0;}//m_strPlayerUpLevelScript.c_str();}	//得到玩家的升级脚本
	//static const char* GetPlayerDiedScrip()	{return 0;}//m_strPlayerDiedScript.c_str();}		//得到玩家死亡脚本
	static const char* GetCompleteScripById(WORD wQuestID);
	static map<long,long>&		GetMerItem()	{return m_mapMerItem;}

	static long	GetMaxQuestNum()	{return m_lMaxQuestNum;}
	static long GetMercQuestUpdateInter() {return m_lMercQuestUpdateInter;}
	static const char* GetAddQuestScript(){return m_strAddQuestScript.c_str();}
	static const char* GetCompleteQuestScript(){return m_strCompleteQuestScript.c_str();}
	static const char* GetQuestAcceptScript(){return m_strQuestAccept.c_str();}
	static const char* GetMercUpdateScript() {return m_pszUpdateScript;}
	static const char* GetQuestUpdate() {return m_strQuestUpdate.c_str();}
	//获取俑兵任务
	static bool GetMercQuests(long lListID,long lPlayerLvl,long lPlayerOccu,
								long lRankOfMercenary,set<long>& BesidesQuests,
								vector<long>& RequireQuests,long lMaxRequireNum,
								bool bUseGoods=false);
	static long GetQuestListType(long lListID);
	static DWORD GetQuestListFreshTime(long lListID);
	static long GetQuestListID(long lQuestID);
private:
	static BOOL AddQuest(tagQuestList& QuestList, CQuest* quest);
	static BOOL AddMainQuest(LONG index,LONG roleID);
	//得到随机的稀有度
	static long GetRadomOdds(bool bUseGoods = false);
private:
	static long	m_lMaxQuestNum;					//任务的最大数量
	static long	m_lMercQuestUpdateInter;		//俑兵任务刷新价格时间
	static char m_pszUpdateScript[MAX_PATH];	//俑兵任务刷新脚本
	static long m_lQuestLvelSpan;				//任务等级跨度
	static long m_lMaxRandomNum;				//最大随机数
	static long m_lOddsRandomNum[QUEST_MAX_ODDS];//稀有度相关的随机数值(稀有度最大值为4)
	static long m_lOddsRandomNumUseGoods[QUEST_MAX_ODDS];//稀有度相关的随机数值(稀有度最大值为4),使用道具刷新时
	static map<long,long> m_mapMerItem;

	static string m_strAddQuestScript;
	static string m_strCompleteQuestScript;
	static string m_strQuestAccept;
	static string m_strQuestUpdate;
	static map<long,vector<CQuest*>>	m_mapQuestsWithOdds[QUEST_MAX_ODDS];//按稀有度分类的任务数据结构
	typedef map<LONG,CQuest*>	mapQuests;
	typedef map<LONG,CQuest*>::iterator itQuests;

	typedef map<LONG,tagQuestList> mapQuestList;
	typedef map<LONG,tagQuestList>::iterator itQuestList;

	static mapQuestList	m_mapQuestList;
	static vector<LONG>			m_vecMainQuestList;				//主线任务列表
};

