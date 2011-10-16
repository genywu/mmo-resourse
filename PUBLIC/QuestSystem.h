#pragma once
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include <map>
#include <vector>
#include "BaseDef.h"
using namespace std;


#define  ROEL_MAIN  1

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
	BOOL Init(TiXmlElement*);
public:
//取得值的接口
	//取得就任务id
	LONG GetID()const{return m_lID;}
	//取得任务类型
	LONG GetRoleType()const{return m_lRoleType;}
	//接受任务的npc的id
	//const CGUID& GetEmbracerNPCID()const{return m_EmbracerNPCID;}
	//const char*	GerNPCName()const{return &m_cEmbracerNPCname[0];}
	//取得  任务是否被显示
	LONG GetShowAble()const{return m_bShowAble;}
	//任务的最小等级限制
	LONG GetMinGrade()const{return m_lMinGrade;}
	//任务的最大等级限制
	LONG GetMaxGrade()const{return m_lMaxGrade;}

	//取得职业限制
	BOOL GetOccupationRestrict(INT index)const
	{
		if (index>=0 && index< OCC_Max)
		{
			return m_Occupation[OCC_Max] & (1<<(index%64)) ;
		}
		else 
			return FALSE;
	}
	//取得章节数
	LONG GetChapterIndex()const{return m_lChapterIndex;}
	//取得任务名字
	const string& GetQuestName() const {return m_strQuestName;}
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

	//取得 任务发送者的id（特殊）
	//const CGUID& GetNPCID()const {return m_NPCID;}
	const string& GetNPCName()const{return m_strNPCName;}
	LONG GetNPCShowType()const{return m_lNPCShowType;}
	LONG GetNPCClewType()const{return m_lNPCClewType;}
private:
	VOID ParseOccupation(const char *str);
	int  GetNumInParse(char*&);
private:
	LONG			m_lID;							//任务id
	LONG			m_lRoleType;					//任务类型
	//CGUID			m_EmbracerNPCID;				//任务接受npc的id
	//char			m_cEmbracerNPCname[50];			//任务接受npc的名字
	unsigned int	m_Occupation[OCC_Max/32+1];		//职业标识
	BOOL			m_bShowAble;					//能否被显示
	LONG			m_lMinGrade;					//最小职业限制	
	LONG			m_lMaxGrade;					//最大职业限制

	LONG			m_lChapterIndex;				//章节数

	//CGUID			m_NPCID;						//接任务npc ID （特殊任务）
	string			m_strNPCName;						//接任务npc ID （特殊任务）
	LONG			m_lNPCShowType;					//npc 可接任务提示（特殊任务）
	LONG			m_lNPCClewType;					//npc 任务提示(暂时不能接，
													//但是几级以后可以接的任务在npc身上的提示，特殊任务

	string			m_strQuestName;					//任务名
	LONG			m_lEmbracerEffect;				//任务效果标识
	LONG			m_lFinishEmbracerEffect;		//任务完成效果标识
	LONG			m_lEmbracerMapEffect;			//任务地图标识
	LONG			m_lIcon;						//任务图标（主线）
	LONG			m_lPicture;						//任务图片标识（主线）
	LONG			m_lStoryDescIndex;					//故事描述（主线）
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
	LONG GetDescIndex()const {return m_lDescIndex;}
	//	是否显示完成数
	BOOL IsShowNum()const {return m_bShowNum;}

	// 设置该追踪是否达到完成条件
	//void SetTraceIsFinish(bool IsFinish) {m_bIsFinish = IsFinish;}
	//BOOL IsTraceFinish()const;

	//	该条追踪需要完成总数
	LONG GetSum()const{return m_lSum;}
	//	对应的变量索引
	LONG GetParam()const{return m_lParam;}
	//	对应的对象原始名ID
	string GetGoodsStrID()const{return m_strID;}
	//	取得npc的id
	//LONG GetNPCID()const{return	m_lNPCID;}
	string GetNPCName()const{return m_strNPCName;}
	//	取得npc的显示效果（1）
	LONG GetNPCEffect_1()const{return m_lNPCEffect_1;}
	//	取得npc的显示效果（2）
	LONG GetNPCEffect_2()const{return m_lNPCEffect_2;}
private:
	LONG	m_lDescIndex;
	BOOL	m_bShowNum;
	//BOOL	m_bIsFinish;
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
	CQuestStep(){m_lID=0;m_lQuestTraceNum=0;m_QuestTrace=0;m_lRoleTraceStartRow=0;}
	~CQuestStep();
	BOOL Init(TiXmlElement *);  
public:
	//取得步骤id
	LONG  GetID() const {return m_lID;}
	//取得步骤描述
	LONG GetDiscrib() const {return m_lDiscrib;}
	//取得步骤追踪
	//const char *GetRoleTrace() const {return m_strRoleTrace.c_str();}
	const CQuestTrace* GetRoleTraceHead() const {return m_QuestTrace;}

	// 获得任务追踪的起始显示位置
	LONG GetTraceStartRow() const {return m_lRoleTraceStartRow;}

	// 判断该步骤是否做完，主要判断该步骤的每个任务追踪是否达到完成条件
	//BOOL IsStepFinish() const;

	//取得 index 对应的文本
	LONG GetText(LONG index) const
	{
		if (index >=0 && index <QUEST_TEXT_SUM)
		{
			return m_lTextIndex[index];
		}
		else
			return 0;
	}
	BOOL AddQuestTrace(CQuestTrace*&);
	const string & GetNPCName() const {return m_strNPCName;}
	LONG GetNPCEffect_1()const{return m_lNPCEffect_1;}
	LONG GetNPCEffect_2()const{return m_lNPCEffect_2;}
private:
	LONG			m_lID;						//任务id
	LONG			m_lDiscrib;				//任务描述
	LONG			m_lRoleTraceStartRow;			//任务追踪的起始显示位置
	LONG			m_lQuestTraceNum;
	CQuestTrace*	m_QuestTrace;
	LONG			m_lTextIndex[QUEST_TEXT_SUM];	//任务文本
	string			m_strNPCName;
	LONG			m_lNPCEffect_1;
	LONG			m_lNPCEffect_2;
public:
	CQuestStep*		next;
};

class CQuest
{
public:
	CQuest(){m_lStepNum=0;m_pQuestStep=0;} 
	~CQuest();
	BOOL Init(TiXmlElement*);
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
class CQuestSystem
{
	CQuestSystem();
	~CQuestSystem();
public:
	static BOOL Init(const char* filename);
	static BOOL InitOneFile(const char* filename );
	static BOOL InitText(const char* filename);
	static VOID Relese();
public:
	//取得指定任务id，指定步骤下，指定索引的任务追踪
	static const CQuestTrace* GetQuestTrace(LONG lQuestID,LONG lStep,LONG lIndex);
	//取得任务id对应的任务
	static const CQuest* GetQuest(LONG id);
	//取得章节数对应的主线任务
	static const LONG GetMainRole(LONG chapterindex);
	//取得特殊任务npc的特殊任务列表
	static vector<LONG> GetSpecialQuestByNPCID(string npcName);
	//
	static string* GetTextByIndex(LONG index);
private:
	//添加一个任务到任务列表中
	static BOOL AddQuest(CQuest *&);
	//加入一个主线任务
	static BOOL AddMainQuest(LONG chapterindex, LONG questID);
	//加入一个特殊任务npc 和它对应的一个任务id
	static BOOL AddSpecialQuest(string npcName,LONG questID);
private:
	static map<LONG,CQuest*> m_mapQuestList;
	static map<LONG,string*> m_mapTextList;
	static vector<LONG> m_vecMainQuestList;
	static multimap<string,LONG> m_mmapSpecialQuestList;
};