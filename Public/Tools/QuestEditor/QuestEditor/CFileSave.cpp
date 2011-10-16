// CFileSave.cpp : 实现文件
// 实现CFileSave.h所申明的函数
#include "StdAfx.h"
#include "CFileSave.h"
#include "QuestEditor.h"
#include "tinyxml.h"
#include "CXmlEdit.h"
#include <vector>
#include <Filter.h>
#include <algorithm>
#include "QuestEditorDlg.h"

#define AddQuestValueNode(Element,name,value,Default,Comment)\
    { \
        if ( value > Default ) \
        { \
            if (strcmp(Comment,""))  \
            {	\
                TiXmlComment *pComTemp = new TiXmlComment();\
                pComTemp->SetValue(Comment);\
                Element->InsertEndChild(*pComTemp); \
            } \
             TiXmlElement *pNodeName = new TiXmlElement(name); \
             Element->LinkEndChild(pNodeName); \
             char pStr[100];\
             sprintf_s( pStr ,"%d",value ); \
             TiXmlText *pNodeText = new TiXmlText(pStr);  \
             pNodeName->LinkEndChild(pNodeText);    \
        }\
   }

#define AddQuestTextNode(Element,name,value,Default,Comment) \
  {\
     if ( strcmp(value,Default)!=0 ) \
     { \
        if (strcmp(Comment,""))  \
        {	\
               TiXmlComment *pComTemp = new TiXmlComment();\
               pComTemp->SetValue(Comment);\
               Element->InsertEndChild(*pComTemp); \
        } \
        TiXmlElement *pNodeName = new TiXmlElement(name); \
        Element->LinkEndChild(pNodeName); \
        if (strcmp(value,""))   \
        {  \
           TiXmlText *pNodeText = new TiXmlText(value);  \
                pNodeName->LinkEndChild(pNodeText);    \
             } \
        }\
  }

#define AddQuestBaseNode(Element,name,value,Default) \
            { \
                if ( value > Default ) \
                {                    \
                    Element->SetAttribute(name,value ); \
                }  \
            }

#define AddQuestStrNode(Element,name,value,Default) \
            { \
              if ( strcmp(value,Default) != 0 ) \
              {   \
                Element->SetAttribute(name,value ); \
              }  \
            }

#define AddQuestBaseDefNode(Element,name,value,Default) \
            { \
            if ( value > Default ) \
                {                    \
                    Element->SetAttribute(name,value ); \
                }  \
                else\
                {\
                    Element->SetAttribute(name,"" ); \
                }\
            }

#define AddQuestStrDefNode(Element,name,value,Default) \
            { \
              if ( strcmp(value,Default) != 0 ) \
              {   \
                Element->SetAttribute(name,value ); \
              }  \
              else\
              {\
                Element->SetAttribute(name,"" ); \
              }\
            }


vector<int>EditRoleList;
vector<string>EditFileList;
//构造函数
CFileSave::CFileSave(void)
{
}
//析构函数
CFileSave::~CFileSave(void)
{
}                    

void  CFileSave::OnSaveRes()
{

    /// textres
    CreateDirectory("textres",NULL);
    {
        CXmlEdit::MapTextFileTable::iterator  MapIter = GetInst(CXmlEdit).m_AllQuestTextFile.begin() ;

        for ( ;MapIter!= GetInst(CXmlEdit).m_AllQuestTextFile.end() ; MapIter++ )
        {
            TiXmlElement * pElm = NULL;
            TiXmlDeclaration XmlDeclar("1.0","gb2312","no");

            TiXmlDocument XmlDoc( MapIter->first ) ;
            XmlDoc.InsertEndChild(XmlDeclar);

            pElm = new TiXmlElement("StringTable");

            map<CString,CString>::iterator  TextIter = MapIter->second.begin();

            for ( int i = 0 ; TextIter!= MapIter->second.end() ; i++ , TextIter++)
            {
                TiXmlElement *Res = new  TiXmlElement("String");
                Res->SetAttribute("id",TextIter->first);
                Res->SetAttribute("text",TextIter->second);
                pElm->InsertEndChild( *Res );
            }

            XmlDoc.InsertEndChild( *pElm );
            XmlDoc.SaveFile();
            delete pElm;
        }
    }

}


void  CFileSave::OnReWrite()
{
    /// 重新写任务配置
    CXmlEdit::MapFileTable::iterator  QuestIter = GetInst(CXmlEdit).m_AllQuestList.begin() ;
    for ( ;QuestIter != GetInst(CXmlEdit).m_AllQuestList.end() ; QuestIter++ )
    {
        TiXmlElement * pElm = NULL;
        TiXmlDeclaration XmlDeclar("1.0","gb2312","no");

        TiXmlDocument XmlDoc( QuestIter->first ) ;
        XmlDoc.InsertEndChild(XmlDeclar);

        pElm = new TiXmlElement("Quest");

        vector<tagRole*>::iterator Qiter = QuestIter->second.begin(); 
        /// 所有任务
        for ( ; Qiter != QuestIter->second.end() ; Qiter ++ )
        {
            /// 任务
            TiXmlElement Elment("QuestId");
            
            /// 任务基本属性
            TiXmlElement *pAttr= new TiXmlElement("Attribute");              
            {
                AddQuestBaseNode(pAttr,"ID",(*Qiter)->vcRole_Attribute->m_nID,0);
                AddQuestBaseNode(pAttr,"QuestType",(*Qiter)->vcRole_Attribute->m_QuestType,-1);
                AddQuestBaseNode(pAttr,"ShareQuest",(*Qiter)->vcRole_Attribute->m_ShareQuest,-1);
                AddQuestStrNode(pAttr,"DeleteScript",(*Qiter)->vcRole_Attribute->m_cDeleteScript.c_str(),"");

                AddQuestValueNode(pAttr,"Repeated",(*Qiter)->vcRole_Attribute->m_nRepeated,-1,"//是否能重复");
                AddQuestValueNode(pAttr,"BeRecord",(*Qiter)->vcRole_Attribute->m_nBeRecord,-1,"//是否被记录");
                AddQuestValueNode(pAttr,"MinGrade",(*Qiter)->vcRole_Attribute->m_nMiniGrade,-1,"//最小等级");
                AddQuestValueNode(pAttr,"MaxGrade",(*Qiter)->vcRole_Attribute->m_nMaxGrade,-1,"//最大等级");
                AddQuestValueNode(pAttr,"TotePet",(*Qiter)->vcRole_Attribute->m_TotePet,-1,"//是否需要携带宠物ID");
                AddQuestValueNode(pAttr,"QuestSex",(*Qiter)->vcRole_Attribute->m_QuestSex,-1,"//是否性别限制");
                AddQuestValueNode(pAttr,"ReqSkill",(*Qiter)->vcRole_Attribute->m_ReqSkill,-1,"//是否技能限定");
                AddQuestTextNode(pAttr,"Doccu",(*Qiter)->vcRole_Attribute->m_Doccu.c_str(),"","//是否副职业限定");
                AddQuestTextNode(pAttr,"Occupation",(*Qiter)->vcRole_Attribute->m_Occupation.c_str(),"","//是否职业限定");
                AddQuestValueNode(pAttr,"NextQuest",(*Qiter)->vcRole_Attribute->m_NextQuest,-1,"//后序任务");

                AddQuestValueNode(pAttr,"ShowAble",(*Qiter)->vcRole_Attribute->m_nShowAble,-1,"//能否被显示");
                AddQuestValueNode(pAttr,"NPCShowType",(*Qiter)->vcRole_Attribute->m_nNPCShowType,-1,"//提示类型");
                AddQuestValueNode(pAttr,"NPCClewType",(*Qiter)->vcRole_Attribute->m_nNPCClewType,-1,"//任务提示");
                AddQuestTextNode(pAttr,"DiscribTextID",(*Qiter)->vcRole_Attribute->m_nDiscribTextID,"","//任务描述文本");
                AddQuestTextNode(pAttr,"NPCID",(*Qiter)->vcRole_Attribute->m_cNPCID.c_str(),"","//NPC原始名");
                AddQuestTextNode(pAttr,"BeSearchDesc",(*Qiter)->vcRole_Attribute->m_nBeSearchDesc,"","//查询任务描述索引");
                AddQuestTextNode(pAttr,"BeSearchSpDesc",(*Qiter)->vcRole_Attribute->m_nBeSearchSpDesc,"","//查询任务简述索引");
                AddQuestValueNode(pAttr,"FinishEffect",(*Qiter)->vcRole_Attribute->m_nFinishEffect,-1,"//完成特效");
                AddQuestValueNode(pAttr,"EmbracerMapEffect",(*Qiter)->vcRole_Attribute->m_nEmbracerMapEffect,-1,"//任务地图表示");
                AddQuestValueNode(pAttr,"EmbracerEffect",(*Qiter)->vcRole_Attribute->m_nEmbracerEffect,-1,"//任务接受者效果");
                AddQuestValueNode(pAttr,"FinishEmbracerEffect",(*Qiter)->vcRole_Attribute->m_nFinishEmbracerEffect,-1,"//任务后完成效果");

                AddQuestValueNode(pAttr,"HardFactor",(*Qiter)->vcRole_Attribute->m_nHardFactor,-1,"//难度");
                AddQuestValueNode(pAttr,"Odds",(*Qiter)->vcRole_Attribute->m_nOdds,-1,"//任务稀有度");
                AddQuestValueNode(pAttr,"MassPoint",(*Qiter)->vcRole_Attribute->m_nMassPoint,-1,"//集结点");
                AddQuestValueNode(pAttr,"DeadLost",(*Qiter)->vcRole_Attribute->m_nDeadLost,-1,"//死亡后任务是否失败");
                AddQuestValueNode(pAttr,"LostRepeated",(*Qiter)->vcRole_Attribute->m_nLostRepeated,-1,"//死亡后任务是否能够重接");
                AddQuestValueNode(pAttr,"ChapterNum",(*Qiter)->vcRole_Attribute->m_nChapterNum,-1,"//章节");
                AddQuestValueNode(pAttr,"Picture",(*Qiter)->vcRole_Attribute->m_nPicture,-1,"//图片");
                AddQuestValueNode(pAttr,"Icon",(*Qiter)->vcRole_Attribute->m_nIcon,-1,"//IOCN");
                AddQuestTextNode(pAttr,"StoryDesc",(*Qiter)->vcRole_Attribute->m_nStoryDesc,"","//描述");
            }
            Elment.InsertEndChild( *pAttr );
            
            /// 任务前置任务列表
            TiXmlElement *pPreFix = new  TiXmlElement("Quest_LablePrefix");
            {
                for ( int i = 0 ; i < (int) (*Qiter)->m_nRoleIndex.size() ; i++)
                {
                    AddQuestValueNode(pPreFix,"QuestIndex",(*Qiter)->m_nRoleIndex[i],0,"");
                }
            }
            Elment.InsertEndChild( *pPreFix );

            /// 任务步骤 
            TiXmlElement *pStep= new TiXmlElement("Step");
            {
                AddQuestBaseNode(pStep,"ID",1,0);
     
                AddQuestTextNode(pStep,"NPCID",(*Qiter)->vcRole_Step->m_cNPCID.c_str(),"","//步骤NPC");
                AddQuestValueNode(pStep,"NPCEffect_1",(*Qiter)->vcRole_Step->m_nNPCEffect_1,-1,"");
                AddQuestValueNode(pStep,"NPCEffect_2",(*Qiter)->vcRole_Step->m_nNPCEffect_2,-1,"");
                AddQuestValueNode(pStep,"EncouragementCell",(*Qiter)->vcRole_Step->m_nEncouragementCell,-1,"//任务所占格子数");

                /// 任务追踪
                vector<tagStepTrace*>::iterator  TraceIter = (*Qiter)->vcRole_Step->vcStepRoleTrace.begin();
                for ( ; TraceIter != (*Qiter)->vcRole_Step->vcStepRoleTrace.end() ; TraceIter ++ )
                {
                    TiXmlElement *Trace = new  TiXmlElement("RoleTrace");
                    Trace->SetAttribute("Desc",(*TraceIter)->m_nDesc.c_str());
                    AddQuestBaseDefNode(Trace,"ShowNum",(*TraceIter)->m_nShowNum,0);
                    AddQuestBaseDefNode(Trace,"Sum",(*TraceIter)->m_nSum,0);
                    AddQuestBaseDefNode(Trace,"Param",(*TraceIter)->m_nParam,0);
                    AddQuestBaseDefNode(Trace,"ID",(*TraceIter)->m_nID,0);

                    pStep->InsertEndChild( *Trace );
                }

                sort( (*Qiter)->vcRole_Step->vcStepAim.begin(),(*Qiter)->vcRole_Step->vcStepAim.end(),AimCMp);

                /// 任务目标
                vector<tagStepAim*>::iterator  AimIter = (*Qiter)->vcRole_Step->vcStepAim.begin();
                for ( ; AimIter != (*Qiter)->vcRole_Step->vcStepAim.end() ; AimIter ++ )
                {
                    TiXmlElement *Aim = new  TiXmlElement("Aim");
                    Aim->SetAttribute("ID",(*AimIter)->m_nID);
                    Aim->SetAttribute("AimModel",(*AimIter)->m_nAimModel); 
                    Aim->SetAttribute("AimType",(*AimIter)->m_nAimType);
                    Aim->SetAttribute("AimName",(*AimIter)->m_cAimName.c_str());
                    Aim->SetAttribute("AimNum",(*AimIter)->m_nAimNum);

                    AddQuestTextNode(Aim,"DropFrom",(*AimIter)->m_cDropFrom.c_str(),"NULL","")
                    AddQuestValueNode(Aim,"DropOdds",(*AimIter)->m_nDropOdds,0,"");
                    AddQuestValueNode(Aim,"Param",(*AimIter)->m_nParam,0,"");

                    pStep->InsertEndChild( *Aim );
                }
            }
            Elment.InsertEndChild( *pStep );
            /// 插入任务
            pElm->InsertEndChild( Elment );
        }
        XmlDoc.InsertEndChild( *pElm );
        XmlDoc.SaveFile();
        delete pElm;
    }

    /// 重新奖励配置
    CXmlEdit::MapAwardFileTable::iterator  AwardIter = GetInst(CXmlEdit).m_AllAwardList.begin() ;
    for ( ; AwardIter != GetInst(CXmlEdit).m_AllAwardList.end() ; AwardIter++ )
    {

        TiXmlElement * pElm = NULL;
        TiXmlDeclaration XmlDeclar("1.0","gb2312","no");

        TiXmlDocument XmlDoc( AwardIter->first ) ;
        XmlDoc.InsertEndChild(XmlDeclar);

        pElm = new TiXmlElement("Award");

        CXmlEdit::MapAwardTable * AwardTable = AwardIter->second;

        for (CXmlEdit::MapAwardTable::iterator AwdTter = AwardTable->begin() ; AwdTter != AwardTable->end() ;   AwdTter ++ )
        {
            /// 任务
            TiXmlElement *Elment = new TiXmlElement("Quest");
            
            AddQuestBaseNode(Elment,"id",AwdTter->first,0);
            if ( AwdTter->second.size() > 0 )
            {
                 AddQuestBaseNode(Elment,"EncouragementCell",AwdTter->second[0]->m_nEncouragementCell,-1);
            }
            else
            {
                AddQuestBaseNode(Elment,"EncouragementCell",0,0);
            }

            for ( int i = 0 ; i < (int)AwdTter->second.size() ; i++ )
            {
                TiXmlElement *Encour = new  TiXmlElement("Encouragement");

                Encour->SetAttribute("Type",AwdTter->second[i]->m_nType);
                AddQuestStrDefNode(Encour,"Name",AwdTter->second[i]->m_cName.c_str(),"");
                Encour->SetAttribute("Num",AwdTter->second[i]->m_nNum);
                char strVal[16];
                sprintf_s( strVal ,"%lf", AwdTter->second[i]->m_fOdds);
                Encour->SetAttribute("Odds",strVal);

                Elment->InsertEndChild( *Encour );
            }
            
            pElm->InsertEndChild( *Elment );
        }

        XmlDoc.InsertEndChild( *pElm );

        XmlDoc.SaveFile();
        delete pElm;
    }
    OnSaveFileList();
    /// 重新资源配置
}

void  CFileSave::OnSaveFileList()
{
    /// QuestList.xml
    {
        TiXmlElement * pElm = NULL;
        TiXmlDeclaration XmlDeclar("1.0","gb2312","no");

        TiXmlDocument XmlDoc( "quest/QuestList.xml") ;
        XmlDoc.InsertEndChild(XmlDeclar);

        pElm = new TiXmlElement("Quest");

        {
            /// 任务
            TiXmlElement *Elment = new TiXmlElement("Attribute");
            AddQuestBaseDefNode(Elment,"MaxRoleNum",GetInst(CXmlEdit).m_QuestListInfo.MaxRoleNum,0);
            AddQuestBaseDefNode(Elment,"MaxMainRoleNum",GetInst(CXmlEdit).m_QuestListInfo.MaxMainRoleNum,0);
            AddQuestStrDefNode(Elment,"AddQuestScript",GetInst(CXmlEdit).m_QuestListInfo.AddQuestScript.c_str(),"");
            AddQuestStrDefNode(Elment,"CompleteQuestScript",GetInst(CXmlEdit).m_QuestListInfo.CompleteQuestScript.c_str(),"");
            AddQuestStrDefNode(Elment,"QuestAccept",GetInst(CXmlEdit).m_QuestListInfo.QuestAccept.c_str(),"");
            AddQuestStrDefNode(Elment,"QuestUpdate",GetInst(CXmlEdit).m_QuestListInfo.QuestUpdate.c_str(),"");
            pElm->InsertEndChild( *Elment );
        }
       

        CXmlEdit::MapFileTable::iterator  QuestIter = GetInst(CXmlEdit).m_AllQuestList.begin() ;

        int i = 1;
        for ( ;QuestIter != GetInst(CXmlEdit).m_AllQuestList.end() ; QuestIter++ )
        {
            TiXmlElement *QuestName = new TiXmlElement("QuestName");
            AddQuestBaseDefNode(QuestName,"ID",i,0);
            TiXmlText *pNodeText = new TiXmlText(QuestIter->first);
            QuestName->LinkEndChild(pNodeText);
            pElm->InsertEndChild(*QuestName);
            //QuestName->SetAttribute("ID",i);
            //AddQuestTextNode(pElm,"QuestName",QuestIter->first,"","");
            i++;
        }

        XmlDoc.InsertEndChild( *pElm );
        XmlDoc.SaveFile();
        delete pElm;

    }

    /// QuestAward.xml
    {
        TiXmlElement * pElm = NULL;
        TiXmlDeclaration XmlDeclar("1.0","gb2312","no");

        TiXmlDocument XmlDoc( "quest/AwardList.xml") ;
        XmlDoc.InsertEndChild(XmlDeclar);

        pElm = new TiXmlElement("AwardList");


        CXmlEdit::MapAwardFileTable::iterator  AwardIter = GetInst(CXmlEdit).m_AllAwardList.begin() ;
        for ( ;AwardIter != GetInst(CXmlEdit).m_AllAwardList.end() ; AwardIter++ )
        {
            TiXmlElement *Award = new  TiXmlElement("list");
            Award->SetAttribute("file",AwardIter->first);
            pElm->InsertEndChild( *Award );
        }

        XmlDoc.InsertEndChild( *pElm );
        XmlDoc.SaveFile();
        delete pElm;
    }
}


