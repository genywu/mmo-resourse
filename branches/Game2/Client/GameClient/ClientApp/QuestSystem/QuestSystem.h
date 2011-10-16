/**
 * @file  QuestSystem.h
 * @brief 任务的逻辑层
 *
 * @modify expter  2010/07/05
 */

#pragma once

//#define  ROEL_MAIN44  1

const LONG  QUEST_TEXT_SUM= 10;							//任务最大文本数量
const LONG  QUEST_STEP_SUM=20;							//任务最大步骤数
const LONG  QUEST_TRACE_SUM=10;
const LONG  MAINQUEST_SUM=1024*16;						//主线任务总数
const LONG  CHAPTER_MAINQUEST_NUM = 6;					// 每章主线任务的数量


//任务属性
class CQuestAttribute
{
public:
	CQuestAttribute();
	~CQuestAttribute(){}
	BOOL Init(TiXmlElement*p,long lTextTypeID);
	VOID InitPreQuest(TiXmlElement*p);				// 初始化前置任务
public:
//取得值的接口
	//取得就任务id
	LONG GetID()const{return m_lID;}
	// 设置和获得文本类型编号
	LONG GetTextTypeID()const{return m_lTextTypeID;}
	//取得任务类型
	LONG GetQuestType()const{return m_lQuestType;}
	//取得  任务是否被显示
	LONG GetShowAble()const{return m_bShowAble;}
	//任务的最小等级限制
	LONG GetMinGrade()const{return m_lMinGrade;}
	//任务的最大等级限制
	LONG GetMaxGrade()const{return m_lMaxGrade;}

	//取得职业限制
	BOOL GetOccupationRestrict(BYTE bOccu)const
	{
		// 无职业限制，直接返回真
		if(m_vecOccu.empty())
			return TRUE;
		for (BYTE i=0;i<m_vecOccu.size();++i)
		{
			if(m_vecOccu[i] == bOccu)
				return TRUE;
		}
		return FALSE;
	}
	//取得章节数
	LONG GetChapterIndex()const{return m_lChapterIndex;}
	//取得任务名字
	const char* GetQuestName() const;
	//取得任务  图标
	LONG GetIcon() const {return m_lIcon;}
	//取得任务图片
	LONG GetPicture() const {return m_lPicture;}
	//取得任务接受者效果
	LONG GetEmbracerEffect() const {return m_lEmbracerEffect;}
	//
	LONG GetFinishEmbracerEffect()const{return m_lFinishEmbracerEffect;}
	//取得任务接受者地图效果
	LONG GetEmbracerMapEffect() const {return m_lEmbracerMapEffect;}
	//取得故事描述
	LONG GetStoryDescIndex() const {return m_lStoryDescIndex;} 
	//取得任务的描述文本ID
	LONG GetDescribTextID() const {return m_lDescribTextID;}

	//取得 任务发送者的id（特殊）
	const string& GetNPCName()const{return m_strNPCName;}
	LONG GetNPCShowType()const{return m_lNPCShowType;}
	LONG GetNPCClewType()const{return m_lNPCClewType;}
	LONG GetHardFactor()const{return m_lHardFactor;}
	LONG GetOdds()const{return m_lOdds;}
    BOOL GetIsShareQuest()const { return m_lIsShareQuest == 1; }

    /// 得到前置任务列表
    const vector<LONG>& GetPreQuestList()const { return m_vecPreQuest; }

private:

	VOID ParseOccupation(const char *str);
	int  GetNumInParse(char*&);
	LONG			m_lID;							//任务id
	LONG			m_lTextTypeID;					// 对应的文本类型
	LONG			m_lQuestType;					//任务类型
	BOOL			m_bShowAble;					//能否被显示
	LONG			m_lMinGrade;					//最小职业限制	
	LONG			m_lMaxGrade;					//最大职业限制

	LONG			m_lChapterIndex;				//章节数

	string			m_strNPCName;					//接任务npc ID （特殊任务）
	LONG			m_lNPCShowType;					//npc 可接任务提示（特殊任务）
	LONG			m_lNPCClewType;					//npc 任务提示(暂时不能接，
													//但是几级以后可以接的任务在npc身上的提示，特殊任务
    LONG            m_lIsShareQuest;                //是否是共享任务  

	LONG			m_lEmbracerEffect;				//任务效果标识
	LONG			m_lFinishEmbracerEffect;		//任务完成效果标识
	LONG			m_lEmbracerMapEffect;			//任务地图标识
	LONG			m_lIcon;						//任务图标（主线）
	LONG			m_lPicture;						//任务图片标识（主线）
	LONG			m_lStoryDescIndex;				//故事描述（主线）

	LONG			m_lDescribTextID;				//任务的描述文本ID
	LONG			m_lHardFactor;					//任务的难度系数
	LONG            m_lOdds;                        //任务稀有度

	/*****************************08.11.6刘科添加***********************************
		添加任务的副职业限制，可搜索任务的配置以及可搜索任务对应的查询描述和查询简述
	********************************************************************************/
	vector<LONG>	m_vecPreQuest;					// 前置任务列表
	vector<BYTE>	m_vecOccu;						// 职业限制列表
	vector<BYTE>	m_vecDOccu;						// 副职业限制列表
	long			m_lDescTextIDBySearch;			// 查询描述ID
	long			m_lSpDescTextIDBySearch;		// 查询简述ID
public:
	void			InitOccu(vector<BYTE>& vecOccu,const char *str);	// 初始化职业限制技能(副职业或主职业)
	bool			GetDOccuRestrict(BYTE bDOccuID)const;	// 根据ID查找相应的副职业是否可接该任务
	long			GetDescTextIDBySearch()	const			// 获得查询描述ID
					{return m_lDescTextIDBySearch;}
	long			GetSpDescTextIDBySearch() const			// 获得查询简述ID
					{return m_lSpDescTextIDBySearch;}
};
//任务追踪
class CQuestTrace
{
public:
	CQuestTrace();
	~CQuestTrace();
	BOOL Init(TiXmlElement*);
public:
	//	本条追踪的描述
	string GetDescIndex()const {return m_lDescIndex;}
	//	是否显示完成数
	BOOL IsShowNum()const {return m_bShowNum;}

	//	该条追踪需要完成总数
	LONG GetSum()const{return m_lSum;}
	//	对应的变量索引
	LONG GetParam()const{return m_lParam-1;}
	//	对应的对象原始名ID
	string GetGoodsStrID()const{return m_strID;}
	//	取得npc的id
	string GetNPCName()const{return m_strNPCName;}
	//	取得npc的显示效果（1）
	LONG GetNPCEffect_1()const{return m_lNPCEffect_1;}
	//	取得npc的显示效果（2）
	LONG GetNPCEffect_2()const{return m_lNPCEffect_2;}
    //  取得当前追逐的任务文本
    const char* GetTraceText() { return NULL;}
private:
	string	m_lDescIndex;
	BOOL	m_bShowNum;
	LONG	m_lSum;
	LONG	m_lParam;
	string	m_strID;
	string	m_strNPCName;
	LONG	m_lNPCEffect_1;
	LONG	m_lNPCEffect_2;
public:
	CQuestTrace* next;

};
//任务 步骤
class CQuestStep
{
public:
	CQuestStep()
    {
        m_lID = 0;
        m_lQuestTraceNum = 0;
        m_QuestTrace = 0;
        m_lQuestTraceStartRow = 0;
        m_lNPCEffect_1 = 0;
        m_lNPCEffect_2 = 0;
    }

    ~CQuestStep();
	BOOL Init(TiXmlElement*);

	//取得步骤id
	LONG  GetID() const {return m_lID;}
	//取得步骤描述
	string GetDiscrib() const {return m_lDiscrib;}
	//取得步骤追踪
	const CQuestTrace* GetQuestTraceHead() const {return m_QuestTrace;}

	// 获得任务追踪的起始显示位置
	LONG GetTraceStartRow() const {return m_lQuestTraceStartRow;}

	BOOL AddQuestTrace(CQuestTrace*&);
	const string & GetNPCName() const {return m_strNPCName;}
	LONG GetNPCEffect_1()const{return m_lNPCEffect_1;}
	LONG GetNPCEffect_2()const{return m_lNPCEffect_2;}
private:
	LONG			m_lID;						//任务id
	string			m_lDiscrib;				//任务描述
	LONG			m_lQuestTraceStartRow;			//任务追踪的起始显示位置
	LONG			m_lQuestTraceNum;
	CQuestTrace*	m_QuestTrace;
	string			m_strNPCName;
	LONG			m_lNPCEffect_1;
	LONG			m_lNPCEffect_2;
public:
	CQuestStep*		next;
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
    long GetType()const{return m_lType;}
    //奖励的具体id
    long GetID()const{return m_lID;}
    //奖励数量
    long GetNum()const{return m_lNum;}
    //获得比例
    float GetOdds()const {return m_fOdds;}
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

    static BOOL  Init(const char* filename);

    static BOOL  LoadAward(const char* filename);

    static long  GetEncourCell( long Qid );

    static long  GetEncourSum( long Qid ) ;

    static CQusetEncouragement* GetEncouragement(long Qid);

    static string GetAwardText(long Qid);
private:

    static std::map<long,QuestAward*>  m_Award;

};

class CQuest
{
public:
	CQuest(){m_lStepNum=0;m_pQuestStep=NULL;} 
	~CQuest();
	BOOL Init(TiXmlElement*p,long lTextTypeID);
public:
	//取得任务id
	LONG GetID() const {return m_Attribute.GetID();}
	//取得任务步骤数
	LONG GetStepNum() const {return m_lStepNum;}
	//取得任务属性
	const CQuestAttribute & GetAttribute() const { return m_Attribute;}
	//取得任务步骤id 对应的 步骤 
	const CQuestStep* GetStep(LONG ID)const;
private:
	//加入一个步骤
	BOOL AddStep(CQuestStep *&step);
private:
	LONG m_lStepNum;				//步骤数
	CQuestAttribute m_Attribute;	//属性
	CQuestStep* m_pQuestStep;		//任务步骤列表头
};



//-----------------------------------------------------------
class CQuestRecord;
class CQuestSystem
{
	CQuestSystem();
	~CQuestSystem();

	struct tagQuestList
	{
		long lTextType;//字符资源类型
		long lTitleTextID;//标题字符资源ID
		bool bIsCanBeSearch;	//该列表下的所有任务是否能被搜索
		//该列表下的所有任务
		map<LONG,CQuest*>	Quests;
	};

	struct tagOddsColor
	{
		long OddsValue; //任务稀有度
		long OddsColor; //稀有度对应名字的颜色值
	};
public:
	static BOOL Init(const char* filename);
	static BOOL InitAttribute(TiXmlElement* p);
	static VOID Relese();

    /// 取得指定任务id，指定步骤下，指定索引的任务追踪
	static const CQuestTrace* GetQuestTrace(LONG lQuestID,
        LONG lStep,
        LONG lIndex);

    /// 取得任务id对应的任务
	static const CQuest* GetQuest(LONG id);

    /// 取得章节数对应的主线任务
	static const LONG GetMainQuest(LONG chapterindex);

    /// 取得特殊任务npc的特殊任务列表
	static vector<LONG> GetSpecialQuestByNPCID(string npcName);
	static long GetMercQuestUpdateInter() { return m_lMercQuestUpdateInter; }
	static void GetmapQuestList(LONG nID, long& TextType, long& lTitleTextID);

    /// 佣兵刷新物品列表
    static vector<int> m_vecMerItemList;                   

    /// 取得所有能查询的任务列表
	static vector<CQuest*>& GetBeSearchQuestList()
    {
        return m_vecCanBeSearchQuestList;
    }

private:
	//添加一个任务到任务列表中
	static BOOL AddQuest(tagQuestList& QuestList, CQuest* quest);
	//加入一个主线任务
	static BOOL AddMainQuest(LONG chapterindex, LONG questID);
	//加入一个特殊任务npc 和它对应的一个任务id
	static BOOL AddSpecialQuest(string npcName,LONG questID);
	static BOOL InitOneFile(const char* filename,tagQuestList& QuestList);
	static long	m_lMercQuestUpdateInter;		//俑兵任务刷新价格时间

	typedef map<LONG,CQuest*>	mapQuests;
	typedef map<LONG,CQuest*>::iterator itQuests;

	typedef map<LONG,tagQuestList> mapQuestList;
	typedef map<LONG,tagQuestList>::iterator itQuestList;

	static vector<CQuest*>	m_vecCanBeSearchQuestList;				// 所有能查询的任务列表

	static mapQuestList	m_mapQuestList;							// 所有任务列表
	static vector<LONG>			m_vecMainQuestList;				//主线任务列表

	static multimap<string,LONG> m_mmapSpecialQuestList;

public:
	typedef map<LONG,tagOddsColor> mapQuestOddsColor;
	typedef map<LONG,tagOddsColor>::iterator itQuestOddsColor;
	static bool InitOdds(TiXmlElement* p);       //初始化稀有度对应的颜色值
	static mapQuestOddsColor m_mapOddsColor;     //稀有度列表

private:
    static CQuestRecord* m_TempQuestRecord;
public:
    static CQuestRecord* GetTempQuestRecord() {return m_TempQuestRecord;}
    static void SetTempQuestRecord(CQuestRecord* q) {m_TempQuestRecord = q;}
};		

/////////////////////////////////////////////////
// zhaohang  2010/5/27 
// 任务消息通信封装,使用单件模式
//任务类型枚举值
enum eQuestType{
	QUEST_TYPE_NOT_MERCENARY = 0,		//非佣兵任务
	QUEST_TYPE_MERCENARY = 1,			//佣兵任务
	QUEST_TYPE_UNKNOW					//未知类型
};

//任务消息通信
class QuestMsgMgr
{
public:
	QuestMsgMgr();
	~QuestMsgMgr();

	/// @brief
	/// 发送接受个人任务
	/// @param questID
	///  任务ID
	/// @param Type
	/// 默认为赏金任务
	/// @return
	/// 
	void Send_Accept(uint questID,eQuestType Type = QUEST_TYPE_MERCENARY);

	/// @brief
	/// 发送接收团队任务
	/// @param
	/// 
	/// @return
	/// 
	void Send_Accept_TeamQuest(uint questID,eQuestType Type = QUEST_TYPE_MERCENARY);
	/// @brief
	///  发送放弃任务消息 
	/// @param questID
	/// 放弃的任务ID
	/// @param questType
	/// 任务类型
	/// @return
	/// 
	void Send_Disband(uint questID,eQuestType Type = QUEST_TYPE_NOT_MERCENARY);

	/// @brief
	/// 发送共享任务消息
	/// @param questID
	/// 任务ID
	/// @return
	/// 
	void Send_Shared(uint questID);

	/// @brief
	/// 发送队伍任务更新请求
	/// @param questID
	/// 任务ID
	/// @return
	/// 
	void Send_TeamQuestUpdate(uint questID);
	/// @brief
	/// 发送完成任务消息
	/// @param questID
	/// 任务ID
	/// @return
	/// 
	void Send_Complete(uint questID,eQuestType Type = QUEST_TYPE_MERCENARY);

	/// @brief
	/// //发送更新个人任务列表
	/// @param questID
	/// 任务ID
	/// @param isUseGoods
	/// 是否使用物品
	/// @return
	/// 
	void Send_Update(uint questID,bool isUseGoods);
private:
	/// @brief
	/// //发送推荐任务
	/// @param type
	/// 0表示推荐一个任务，1表示请求队伍任务列表
	/// @return
	/// 
	void Send_Commend(uint questID,BYTE type);
};

/////////////////////////////////////////////////
// zhaohang  2010/6/29 
// 玩家队伍任务数据组织
////////////////////////////////////////////////
class TeamQuestData
{
public:
	struct tagTeamQuest
	{
		long lQuestID;					//任务ID
		string strCommenderName;		//推荐者名称
		tagTeamQuest()
		{
			lQuestID = 0;
			strCommenderName = "";
		}
	};
	TeamQuestData();
	~TeamQuestData();
	/// @brief
	/// //根据ID和推荐者名称添加一项任务,如果玩家已经推荐过任务，则之前推荐任务被当前任务替换
	/// @param ID
	/// 任务ID
	/// @param srcPlayerName
	/// 推荐此任务的玩家
	/// @return
	///
	void Add(long ID,const char* srcPlayerName);	

	/// @brief
	/// 根据推荐者，删除对应的队伍任务
	/// @param srcPlayerName
	/// 推荐者(玩家)名字
	/// @return
	/// 
	void Del(const char* srcPlayerName);

	/// @brief
	/// 指定ID的任务是否已经推荐过
	/// @param ID
	/// 任务ID
	/// @return
	/// 已经推荐过返回true，否则返回false
	bool IsCommended(long ID);
	void NotifyUpdateUI();						//通知界面更新显示
	void NotifyUIQuestRunning(long ID);			//通知界面更新显示正在进行的任务

	tagTeamQuest* Get(uint index);				//获取指定索引的队伍任务
	void Clear(){ vecTeamQuest.clear();}		//清空队伍任务列表
	//void SetTeamQuestID(long id) { teamQuestID = id; }
	//const long GetTeamQuestID() const { return teamQuestID;}
	const long GetRunningID() const { return curRunningID;}		//获取队伍正在进行的任务ID
private:
	vector<tagTeamQuest> vecTeamQuest;		//队伍任务列表
	//long teamQuestID;                       //全队任务ID
	long curRunningID;						//当前队伍正在进行的任务
};