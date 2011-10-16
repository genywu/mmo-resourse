#pragma once
#include "tinyxml.h"
#include "CStrConverter.h"

//用于对客户端控制的人物应用模块相关操作配置的读取
//2010-1-11添加对个人商店的模型显示特效的配置
class CQuestIndexXml
{
public:
	CQuestIndexXml();
	~CQuestIndexXml();
	void Release();
	//任务索引的结构，包括该任务的任务id,任务树中的id,
	struct tagQuestNode{
		long lNodeID;           //配置的人物身上的BUFF；
		string strNodeName;         //BUFF对应的特效显示ID；
		long lParentID;         //该结点的父结点ID,一级结点为0;
		long lTree;             //该结点所在的树，（目前有两个树）
		tagQuestNode()
		{
            lNodeID = 0;
			strNodeName = "";
            lParentID = 0;
			lTree = 0;
		}
	};	
	struct tagQuestIndex{
		long lQuestID;
		long lTreeID;		
		int  iQuestNote;        //任务标记，如推荐任务或新加任务
		long lNumSum;           //该任务每日能做的总数
		long lNumParam;      //该任务已完成数
		long lParamTextPos;   //任务完成数显示的X坐标
		long lTree;           //该任务所在的树
		int iState;           //显示状态，正常0，过期1，完成2，不显示3
		int iLevelLimit;      //显示任务是否有等级限制 (0,有等级限制;1,无等级限制)
		int iStep;            //需要显示的任务步骤
		tagQuestIndex()
		{
			lQuestID = 0;
			lTreeID = 0;			
			iQuestNote = 0;
			lNumSum = 0;
			lNumParam = 0;
			lParamTextPos = 0;
			lTree =0;
			iState=0;
			iLevelLimit = 1;
			iStep = 1;
		}
	};
	bool LoadQuestSetupXml();   //读取data下对人物配置文件
///////////////////////////////////////////////////////////////////////
//////////////////配置相关/////////////////////////////////
	bool LoadQuestNode_New(TiXmlElement* pElem);    //读取任务树结点
	bool LoadQuestIndex_New(TiXmlElement* pElem);
	vector<tagQuestNode>& GetQuestNode(){return m_vecQuestNode;}
	vector<tagQuestIndex>& GetQuestIndex() {return m_vecQuestIndex;}
	bool FindIndexQuest(long Index);            //查找一个任务是否存在于索引任务表中
	void SetIndextQuest(long Index,long NumParam);   //设置一个索引任务的完成数
	bool GetIndextQuest(long Index,long &NumParam);          //获得一个索引任务的完成数
	void SetIndexQuestState(long Index,int istate);   //设置一个任务的显示状态
    int GetIndextQuestState(long Index);     //获得一个任务的显示状态
	int GetIndexQuestStep(long Index);       //获得任务配置中需要显示的步骤
private:

	vector<tagQuestNode> m_vecQuestNode;
	vector<tagQuestIndex> m_vecQuestIndex;
/////*************************************************服务器相关*********************************/////
public:
	struct tagQuestIndexForServer{
		long lQuestID;
		long lNumSum;        //该任务每日能做的总数
		long lNumParam;      //该任务已完成数
		int iState;          //显示状态，正常0，过期1，完成2，不显示3
		DWORD lReSetTime;     //任务重置的时间点(保存该条任务的实际重设时间)
		long lReSetTimeToXml; //配置中的任务重设时间点
		
        tagQuestIndexForServer()
		{
			lQuestID = 0;
			lNumSum = 0;
			lNumParam = 0;
			iState = 0;
			lReSetTime = 0;
			lReSetTimeToXml = 0;
		}
	};

	bool LoadQuestSetupXmlForServer();     //用于服务器读取配置
	bool LoadQuestIndex_NewEx(TiXmlElement* pElem);
	bool FindIndexQuestEx(long Index);
	void SetIndextQuestEx(long Index,long NumParam);
	bool GetIndextQuestEx(long Index,long &NumParam);
	void SetIndexQuestStateEx(long Index,int istate); 
    CQuestIndexXml::tagQuestIndexForServer GetIndextQuestFromXML(long Index);

	bool CodeToByteArray(DBWriteSet& dbWriteSet);
	bool DecodeToByteArray(DBReadSet& dbReadSet);

private:
	map<long ,tagQuestIndexForServer> m_tagQuestIndexForServer;
};