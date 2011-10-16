#pragma once

#include "../Common/BaseDef.h"


//enum ROLE_TYPE{MOSTLY_ROLE=1,BAYOU_ROLE,ENGAGEARMS_ROLE};//任务类型（主线，支线，佣兵）

const int LABELPREFIX_NUM=5;							//前置任务数量

enum AIM_MODEL_TYPE{AMT_KILL=0,AMT_COLLECT,AMT_TALK,AMT_ESCORT,AMT_KILL_COLLECT};//目标模式类型

const long	PREPARE_PARAM_NUM=10;						//后备变量数（aim）
const long  STEP_AIM_NUM=10;							//步骤目标最大数量
const long  STEP_ENCOURAGEMENT_NUM=10;					//步骤获取物品最大数量
const long  QUEST_STEP_NUM=20;							//任务最大步骤数
const long  QUEST_ENCOURAGEMENT_NUM=20;					//任务最大奖励数量
const long  GOODS_TYPE_INDEX=1;							//任务配置中的物品类型标识
const int QUEST_MAX_ODDS = 20;						    //任务的最大稀有度值
const int MAX_MERC_QUEST_NUM = 4;					    //单个列表ID中最大的俑兵任务数量

//任务属性
class CQuestAttribute
{
public:
	CQuestAttribute();
	~CQuestAttribute(){}
	bool Init(TiXmlElement*);
public:
//取得值的接口
	//取得任务id
	long GetID()const{return m_lID;}
	//是否被记录
	long GetBeRecord()const{return m_bBeRecord;}
	//任务类型
	long GetQuestType()const{return m_lQuestType;}
    //任务是否可共享
    long GetQuestIsShare()const{ return m_lShareQuest;}
	//是否被显示
	long GetShowAble()const{return m_bShowAble;}
	//是否能反复接
	long GetRepeated()const{return m_bRepeated;}
	//死亡是否失败
	long GetDeadLost()const{return m_bDeadLost;}
	//任务失败后能否重新接
	long GetLostRepeated()const{return m_bLostRepeated;}
	//最小等级限制
	long GetMinGrade()const{return m_lMinGrade;}
	//最大等级限制
	long GetMaxGrade()const{return m_lMaxGrade;}
	//职业限制
	bool GetOccupationRestrict(INT index)const
	{
		if (index>=0 && index< OCC_Max)
		{
			return (m_Occupation[index/32] & (1<<(index%32)))==0 ? 0:1 ;
		}
		else 
			return false;
	}
	//任务几率
	long GetOdds()const{return m_lOdds;}
	//下一个任务的id
	long GetNextRoleID()const{return m_lNextRoleID;}
	//章节数
	long GetChapterNum()const{return m_lChapterNum;};
	// 完成特效
	long GetFinishEffect()const{return m_lFinishEffect;}
	// 任务删除脚本
	const char* GetDeleteScript()const{return m_cDeleteScript;}
    // 得到集结点
	long GetMassPoint() const	{return m_lMassPoint;}
    // 性别要求
    uchar GetQuestSex()const     {return m_QuestSex; }
    // 需要携带宠物ID
    long GetTotePet()const      {return m_lTotePet; }
    // 需求技能限制
    long GetReqSkill()const     {return m_ReqSkill; }
private:

    /// 注：原有解析算法可能有误或者与现在需求不同，待改正 by:expter

	//解析 职业限制
	void ParseOccupation(const char *str);
	int  GetNumInParse(char*&);

private:   
    bool		m_bBeRecord;					//是否被记录
	bool		m_bShowAble;					//能否被显示
	bool		m_bRepeated;					//能否反复接受
	bool		m_bDeadLost;					//死亡任务失败
	bool		m_bLostRepeated;				//失败重接开关
    uchar        m_QuestSex;                     //性别要求 0男，1女, 2不要求

	long		m_lID;							//任务id
	long		m_lQuestType;					//任务类型
    long        m_lShareQuest;                  //是否可以共享	
	long		m_lMinGrade;					//等级下限
	long		m_lMaxGrade;					//等级上限	
	long		m_lOdds;						//任务稀有程度
	long		m_lNextRoleID;					//下一个任务的id
	long		m_lFinishEffect;				//任务完成效果
	long		m_lChapterNum;					//章节数	  
    long        m_lTotePet;                     //需要携带宠物ID
    long        m_ReqSkill;                     //需求技能
	long		m_lMassPoint;					//集结点   

    int 		m_Occupation[OCC_Max/32 +1];	//职业限制 b  
    char        m_cDeleteScript[64];			//删除任务脚本
};

//前置任务
class CQuestLabelPrefix
{
public:
	CQuestLabelPrefix();
	~CQuestLabelPrefix(){}
	bool Init(TiXmlElement*);
public:
	//取得index对应的前置任务
	long GetLabelPrefix(long Index)const;
	//加入一个前置任务id记录
	bool AddLabelPrefix(long val);
	//取得前置任务数量
	long GetLabelSum()const{return m_lLabelSum;}
private:
	long		m_lLabelSum;						//前置任务数量
	long		m_lLabelPrefix[LABELPREFIX_NUM];	//前置任务列表
};
//任务目标
class CQuestAim
{
public:
	CQuestAim();
	~CQuestAim(){}
	bool Init(TiXmlElement*);
public:
//取值接口
	//取得id
	long GetID()const					{return m_lID;}
	//目标模板类型
	AIM_MODEL_TYPE	GetAMTType()const	{return m_AMTType;}
	//目标类型
	long GetAimType()const				{return m_lAimType;}
	//取得目标id
	long GetAimId()const				{return m_lAimID;}
	//取得目标数量
	long GetAimNum()const				{return m_lAimNum;}
	//取得 变量索引
	long GetParameIndex()const			{return m_lParamIndex;}
	//凋落这种物品的生物类型（对物品）
	long GetDropFrom()const				{return m_lDropFrom;}
	//几率
	long GetDropOdds()const				{return m_lDropOdds;}
	//取得目标的名字
	const char* GetName()const			{return (char*)&m_cAimName[0];}

	//取得备用属性
	long GetPrepareParam(long index)const
	{
		if (index>=0 && index <PREPARE_PARAM_NUM)
		{
			return  m_lPrepareParam[index];
		}
		else
			return 0;
	}

private:
	long			m_lID;					//id
	AIM_MODEL_TYPE	m_AMTType;				//模板类型
	long			m_lAimType;				//目标类型
	char			m_cAimName[50];			//目标名字
	long			m_lAimID;				//目标id
	long			m_lAimNum;				//目标数量
	long			m_lParamIndex;			//变量索引
	long			m_lDropFrom;			//掉落对象
	long			m_lDropOdds;			//几率
	long			m_lPrepareParam[PREPARE_PARAM_NUM];		//备用属性列表
public:
	CQuestAim* next;
};
//任务奖励
class CQusetEncouragement
{
public:
	CQusetEncouragement();
	~CQusetEncouragement(){}
	bool Init(TiXmlElement*);

    void AddToByteArray(vector<uchar>*& pucharArray);
    void DecodeQuestAward(uchar* puchar, long& pos);
public:
	//取得奖励的名字
	const char* GetName()const{return m_Name;}
	//取得奖励类型
	long GetType()const{return m_lType;}
	//奖励的具体id
	long GetID()const{return m_lID;}
	//奖励数量
	long GetNum()const{return m_lNum;}
private:
	char  m_Name[128];		//奖励物的名字
	long  m_lType;			//类型
	long  m_lID;			//id
	long  m_lNum;			//数量
    float m_fOdds;          //倍率
public:
	CQusetEncouragement *next;
};
// 任务奖励管理
class  CQuestEncourMan
{
public:
    struct QuestAward
    {
        long  Sum;
        long  Cell;
        CQusetEncouragement *Encour;
        QuestAward()
        {
            Cell= 0;
            Sum = 0 ;
            Encour = NULL;
        }
    };
    
    CQuestEncourMan()  ;

    ~CQuestEncourMan() ;

    static void  Release();

    static bool  Init(const char* filename);

    static bool  LoadAward(const char* filename);
    
    static bool  DecodeQuestAward(uchar* puchar, long& pos);

    static bool  AddToByteArray(vector<uchar>* pucharArray);

    static long  GetEncourCell( long Qid );

    static long  GetEncourSum( long Qid ) ;

    static CQusetEncouragement* GetEncouragement(long Qid);
private:

    static std::map<long,QuestAward*>  m_Award;

};

//任务步骤
class CQuestStep
{
public:
	CQuestStep();
	~CQuestStep();
	bool Init(TiXmlElement*);
	bool AddToByteArray(vector<uchar>* pucharArray);
	bool DecordFromByteArray(uchar* puchar, long& pos);
private:
	struct SQuestText
	{
		long TextID;
		long TextIndex;
	};
public:
	//
	long GetStepAimNum()const{return m_lAimNum;}
	//取得步骤id
	long GetStepID()const{return m_lStepID;}
	//获取奖励物品所占格子数
	long GetRolePropCell()const{return m_lRolePropCell;}
	//取得目标
	const CQuestAim* GetAim(long ID)const;
	//取得奖励列表的头指针
	const CQuestAim* GetAimHead()const{return m_Aim;}
private:
	//加入一个目标
	bool AddAim(CQuestAim*&);
private:
	long				m_lStepID;				//步骤id
	long				m_lRolePropCell;	    //任务所占格子数
	long				m_lAimNum;				//任务目标数量
	CQuestAim*			m_Aim;					//任务目标头指针
public:
	CQuestStep* next;
};
//单个任务
class CQuest
{
public:
	CQuest();
	~CQuest();
	bool Init(TiXmlElement* ,long lTextType);
	bool AddToByteArray(vector<uchar>* pucharArray);
	bool DecordFromByteArray(uchar* puchar, long& pos);
public:
	long GetTextType()const{return m_lTextType;}
	long GetID()const{return m_Attribute.GetID();}
	const CQuestAttribute& GetAttribute()const{return m_Attribute;}
	const CQuestLabelPrefix& GetLabelPrefix()const{return m_LabelPrefix;}
	long GetStepSum()const{return m_lStepSum;}
	const CQuestStep*	GetStep(long ID=0x7fffffff)const;
private:
	bool AddStep(CQuestStep *& step);
	bool AddEncouragement(CQusetEncouragement *& Encouragement);
private:
	long					m_lTextType;				//文本类型索引
	CQuestAttribute			m_Attribute;				//属性
	CQuestLabelPrefix		m_LabelPrefix;				//前置任务
	long					m_lStepSum;					//任务总步骤数
	CQuestStep*				m_Step;						//任务步骤列表};
};
//任务列表
class CQuestSystem
{
	CQuestSystem();
	~CQuestSystem();

	struct tagQuestList
	{
		long lTextType;   //字符资源类型
		long lTitleTextID;//标题字符资源ID
		long lType;	      //任务组类型： -1 错误（不存在任务组） 0 一般任务组 1国家任务组 2佣兵任务组 
		ulong dwFreshTime;//刷新时间
		//该列表下的所有任务
		map<long,CQuest*>	Quests;
	};
public:
	static bool Init(const char* filename);
	static bool InitAttribute(TiXmlElement* p);
	static bool InitOneFile(const char* filename,tagQuestList& QuestList);
	static bool Relese();
	static bool AddToByteArray(vector<uchar>* pucharArray);
	static bool DecordFromByteArray(uchar* puchar, long& pos);
	//任务接口
	static const  CQuest*			GetQuest(long ID);
	static		  long				GetMainRoleByChapterID(long chapterID);
	static const  CQuestStep*		GetStep(long ID,long stepID);
	static const  CQuestAim*	GetAim(long ID,long step,long aimID);
	//static const  CQusetEncouragement* GetEncouragementHead(long ID);
	//static const  CQusetEncouragement* GetStepEncouragementHead(long ID,long stepID);
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
	static long  GetQuestListType(long lListID);
	static ulong GetQuestListFreshTime(long lListID);
	static long  GetQuestListID(long lQuestID);

    static long  GetEncouragementCell(long lQuestID);
    static long  GetEncouragementSum(long lQuestID);
    static CQusetEncouragement* GetEncouragementHead(long lQuestID);
//    static CQusetEncouragement* GetRolePropHead(long lQuestID);
//    static long  GetRolePropSum(long lQuestID);

private:
	static bool AddQuest(tagQuestList& QuestList, CQuest* quest);
	static bool AddMainQuest(long index,long roleID);
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
	typedef map<long,CQuest*>	mapQuests;
	typedef map<long,CQuest*>::iterator itQuests;

	typedef map<long,tagQuestList> mapQuestList;
	typedef map<long,tagQuestList>::iterator itQuestList;

	static mapQuestList	m_mapQuestList;
	static vector<long>			m_vecMainQuestList;				//主线任务列表
};
