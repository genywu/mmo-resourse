
#pragma once
#include "stdafx.h"
#include "CAttribute.h"
#include "CStep.h"
#include "CEncouragement.h"
#include "tinyxml.h"
#include <vector>
#include <map>

class CXmlEdit
{      
public:
    struct  QuestListInfo 
    {
       long     MaxRoleNum;
       long     MaxMainRoleNum;
       string   AddQuestScript;
       string   CompleteQuestScript;				
       string   QuestAccept;
       string   QuestUpdate;

       QuestListInfo()
       {
           MaxRoleNum =1000;
           MaxMainRoleNum=1000;
           AddQuestScript="scripts/questmodel/quest_model_add_a.script";
           CompleteQuestScript="scripts/questmodel/quest_model_complete.script";
           QuestAccept="scripts/questmodel/quest_model_add_c.script";
           QuestUpdate="scripts/questmodel/quest_model_Update.script";
       }
    };

    typedef std::map<CString,vector<tagRole*> >            MapFileTable;
    typedef std::map<long,vector<tagRoleEncouragement*> >  MapAwardTable;
    typedef std::map<CString,map<CString,CString> >        MapTextFileTable;
    typedef std::map<CString,MapAwardTable*>               MapAwardFileTable;

	//定义向量来存储所有的任务
	vector<tagRole*>            m_vcAllRole;
	//tagRole*类型的迭代器
	vector<tagRole*>::iterator  tagRoleIter;
	//保存任务文件列表
	vector<CString>             m_vcRoleFileList;

    //任务奖励文件列表
    vector<CString>             m_AwardList;
    //
    MapAwardFileTable           m_AllAwardList;
    //
    std::map<long,CString>      m_AwardIdFileList;


    //任务文件映射任务
    MapFileTable                m_AllQuestList;
    //任务ID对应任务名
    std::map<long,CString>      m_AllQuestStrId;


    //任务文本资源
    map<CString,CString>        m_TextRes;
    //带文件名任务文本资源
    MapTextFileTable            m_AllQuestTextFile;
    //List其他配置
    QuestListInfo               m_QuestListInfo;

	//tagRoleAttribute指针
	tagRoleAttribute *pRoleAttribute;
	//tagRoleStep指针
	tagRoleStep *pRoleStep;
	//tagRoleEncouragement指针
	tagRoleEncouragement *pRoleEncouragement;

public:
	CXmlEdit(void);
	~CXmlEdit(void);
    //读完res
    bool LoadTextFile(const string& file_name, bool sync);
	//读取所有任务文件中的任务信息.
	bool ReadeRoleFile();
    //读取所有任务文件中的任务奖励信息.
    bool ReadeQuestAwardFile();
    //读取所有的任务文本资源
    bool ReadQuestTextRes();
	//获任务文件的文件名列表
	vector<CString> GetRoleFileList();
    //获得奖励文件列表
    vector<CString> GetQuestAwardFileList();
	//获取任务信息
	bool GetRoleInfor(TiXmlElement *pRole,tagRole*& Role);
	//获取任务信息___任务属性
	bool GetRoleAttributeInfor(tagRoleAttribute*& pRoleAttribute,TiXmlElement* pRoleElem);
	//获取任务信息___任务步骤
	bool GetRoleStepInfor(tagRoleStep*& pRoleStep,TiXmlElement* children);
	//获取任务信息___任务奖励
	bool GetRoleEnInfor(TiXmlElement* pRoleElem );

};
