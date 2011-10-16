//CXmlEdit.cpp
//CXmlEdit.h头文件中的函数的实现.
#include "StdAfx.h"
#include "CXmlEdit.h"
#include "CAttribute.h"
#include "CStep.h"
#include "CEncouragement.h"
#include "tinyxml.h"
#include "QuestEditor.h"
#include "Tools.h"
#include <vector>
//构造函数
CXmlEdit::CXmlEdit(void)
{ 
	ReadeRoleFile();
    ReadeQuestAwardFile();
    ReadQuestTextRes();
}
//析构函数
CXmlEdit::~CXmlEdit()
{
    /// 删除奖励
    MapAwardFileTable::iterator AwardIt = m_AllAwardList.begin();
    for ( ; AwardIt != m_AllAwardList.end() ; AwardIt ++ )
    {
        MapAwardTable::iterator  award = (*AwardIt->second).begin();
         
        for ( ; award != (*AwardIt->second).end() ; award ++ )
        {
            for (vector<tagRoleEncouragement*>::iterator Encour = award->second.begin() ; Encour != award->second.end() ; Encour ++ )
            {
                delete *Encour;
                *Encour = NULL;
            }
        }
        delete AwardIt->second;
    }

    /// 删除任务
    MapFileTable::iterator  QuestItr = m_AllQuestList.begin();
    for ( ; QuestItr != m_AllQuestList.end() ; QuestItr++ )
    {
        vector<tagRole*>::iterator  RoleIter = QuestItr->second.begin();
        for ( ; RoleIter != QuestItr->second.end() ;  RoleIter ++ )
        {
            //RoleIter;
            delete *RoleIter;
            *RoleIter  = NULL;
        }
    }
}

//返回工作目录下所有的任务文件
/*功能:获取任务文件列表。
*摘要:获得在工作目录下的所有的任务文件文件名
*参数:
*返回值:返回一个包含了所有的任务文件的文件名的vector
*作者：王 钦
*创建日期:2010-7-7
*/
vector<CString> CXmlEdit::GetRoleFileList()
{
	//申明变量存在文件路径
	CString			QuestDirName,AwardList;
	//quest文件夹QuestList文件列表
	vector<CString>   vcRoleFileList;

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox("quest\\QuestList.xml");
    TiXmlDocument* m_Tdoc = & T_dox; 
    //打开文件	
    if (!T_dox.LoadFile())									
    {
        MessageBox(NULL,"任务编辑器路径不对，或者任务列表QuestList.xml不存在！","",MB_OK); 
        CreateDirectory("quest",NULL);
        return  vcRoleFileList; 
    }
    pNode=m_Tdoc->FirstChild("Quest");
    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        return  vcRoleFileList;
    }
    TiXmlElement* p=te->FirstChildElement();
    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"QuestName"))
        {
            vcRoleFileList.push_back( p->GetText() );
        }
        else if (!strcmp(p->Value(),"Attribute"))
        {
            const char * p1 = p->Attribute("MaxRoleNum");
            const char * p2 = p->Attribute("MaxMainRoleNum");
            const char * p3 = p->Attribute("AddQuestScript");
            const char * p4 = p->Attribute("CompleteQuestScript");
            const char * p5 = p->Attribute("QuestAccept");
            const char * p6 = p->Attribute("QuestUpdate");

            m_QuestListInfo.MaxRoleNum = atol( p1 );
            m_QuestListInfo.MaxMainRoleNum = atol( p2 );
            m_QuestListInfo.AddQuestScript = p3 ;
            m_QuestListInfo.CompleteQuestScript = p4 ;
            m_QuestListInfo.QuestAccept = p5;
            m_QuestListInfo.QuestUpdate = p6;
        }
    }
	return vcRoleFileList;
}

vector<CString> CXmlEdit::GetQuestAwardFileList()
{
    //申明变量存在文件路径
    CString			QuestDirName,AwardList;
    //quest文件夹QuestList文件列表
    vector<CString>   vcRoleFileList;
    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox("quest\\AwardList.xml");
    TiXmlDocument* m_Tdoc = & T_dox; 
    //打开文件	
    if (!T_dox.LoadFile())									
    {
        MessageBox(NULL,"任务编辑器路径不对，或者任务奖励列表AwardList.xml不存在！","",MB_OK); 
        CreateDirectory("quest",NULL);
        return  vcRoleFileList; 
    }
    pNode=m_Tdoc->FirstChild("AwardList");
    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        return  vcRoleFileList;
    }
    TiXmlElement* p=te->FirstChildElement();
    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"list"))
        {
            vcRoleFileList.push_back( p->Attribute("file") );
        }
    }
    return vcRoleFileList;
}

/*功能:打开任务文件。
*摘要:遍历工作目录下的所有的任务文件,并读取其中的任务信息。
      并将信息保存在内存中.
*参数:
*返回值:成功返回true,失败返回false
*作者：王 钦
*创建日期:2010-7-7
*/
bool CXmlEdit::ReadeRoleFile()
{
	//定义向量来存储所有文件的名称
	vector<CString>           vcRoleFile;
	//定义向量的迭代器并初始化
	vector<CString>::iterator RoleFileIter;
	//初始化向量
	vcRoleFile = GetRoleFileList();
	//通过向量的迭代器对所有的任务文件进行访问
	RoleFileIter = vcRoleFile.begin();
	for (;RoleFileIter!=vcRoleFile.end();RoleFileIter++)
	{
        vector<tagRole*>    AllRole;
        CString RoleFileName = *RoleFileIter;
        TiXmlDocument Q_doc(RoleFileName);	
        //打开文件	
        if (!Q_doc.LoadFile())									
        {
            MessageBox(NULL,"打开文件"+RoleFileName+"失败,可能是该文件不具有正确的文档格式或文件属性为只读","",MB_OK);  return false; 
        }
        TiXmlNode *pNode=Q_doc.FirstChild("Quest");					
        if (NULL==pNode)
        {   
            MessageBox(NULL,"没有相关节点","",MB_OK); return false;
        }
        //读取节点信息
        TiXmlElement *pElement = pNode->ToElement();
        for (TiXmlElement *pRole=pElement->FirstChildElement(); 
            pRole!=NULL; pRole=pRole->NextSiblingElement())
        {
            tagRole *Role  =  new tagRole;
            GetRoleInfor( pRole,Role);
            Role->m_strFileName = *RoleFileIter;
            AllRole.push_back(Role);
            m_AllQuestStrId[ Role->m_nID ] = RoleFileName ;
        }
        m_AllQuestList[ RoleFileName ]  = AllRole ;
	}
	return true;
}

bool CXmlEdit::ReadeQuestAwardFile()
{
    vector<CString>           vcRoleFile;
    vector<CString>::iterator RoleFileIter;
    vcRoleFile = GetQuestAwardFileList();
    RoleFileIter = vcRoleFile.begin();
    for (;RoleFileIter!=vcRoleFile.end();RoleFileIter++)
    {
        //任务奖励
        MapAwardTable  *AllQuestAward = new MapAwardTable ;

        CString RoleFileName = *RoleFileIter;
        TiXmlDocument Q_doc(RoleFileName);	
        //打开文件	
        if (!Q_doc.LoadFile())									
        {
            MessageBox(NULL,"打开文件"+RoleFileName+"失败,可能是该文件不具有正确的文档格式或文件属性为只读","",MB_OK);  return false; 
        }
        TiXmlNode *pNode=Q_doc.FirstChild("Award");					
        if (NULL==pNode)
        {   
            MessageBox(NULL,"没有相关节点","",MB_OK); return false;
        }
        //读取节点信息
        TiXmlElement *pElement = pNode->ToElement();
        for (TiXmlElement *pRole=pElement->FirstChildElement(); 
            pRole!=NULL; pRole=pRole->NextSiblingElement())
        {
            if (!strcmp(pRole->Value(),"Quest"))
            {
                 long    Qid = atol( pRole->Attribute("id") );
                 long    EncouragementCell = atol( pRole->Attribute("EncouragementCell") );

                 TiXmlElement* Cell = pRole->FirstChildElement("Encouragement");
                 for (; Cell!=NULL; Cell=Cell->NextSiblingElement())
                 {
                     tagRoleEncouragement* tagAward = new  tagRoleEncouragement;
                     tagAward->m_nType=atoi(Cell->Attribute("Type"));
                     tagAward->m_cName=Cell->Attribute("Name");
                     tagAward->m_nNum=atoi(Cell->Attribute("Num"));
                     tagAward->m_fOdds=atof(Cell->Attribute("Odds"));
                     tagAward->m_nEncouragementCell  = EncouragementCell;
                     (*AllQuestAward)[ Qid ].push_back( tagAward );

                     m_AwardIdFileList[ Qid ] = RoleFileName;
                 }
            }
            else
            {
                //输出错误信息
            }
        }
        m_AllAwardList[ RoleFileName ]  = AllQuestAward;
    }

    return true;
}

bool CXmlEdit::LoadTextFile(const string& file_name, bool sync)
{

    TiXmlDocument Q_doc(file_name.c_str());	
    const char* pfile = file_name.c_str();
    if ( strstr(file_name.c_str(),"questtext") == NULL )
    {
        return false;
    }
    //打开文件	
    if (!Q_doc.LoadFile())									
    {
        //MessageBox(NULL,"打开文件"+file_name.c_str()+"失败,可能是该文件不具有正确的文档格式或文件属性为只读","",MB_OK);  return false; 
        return false;
    }
    map<CString,CString>        TextRes;
    TextRes.clear();
    TiXmlNode *pNode=Q_doc.FirstChild("StringTable");					
    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        return  false;
    }
    TiXmlElement* p=te->FirstChildElement();
    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"String"))
        {
            const char * p1 = p->Attribute("id");
            const char * p2 = p->Attribute("text");
            m_TextRes[ p1 ] = p2; 
            TextRes  [ p1 ] = p2;
        }
    }
    m_AllQuestTextFile [ file_name.c_str() ] = TextRes ; 
    return true;
}

bool  CXmlEdit::ReadQuestTextRes()
{
    list<string> files;
    FindFile("textres/*.xml", ".xml", &files);
    if (files.empty())
    {
        return false;
    }

    m_AllQuestTextFile.clear();
    list<string>::iterator itr = files.begin();
    for(; itr != files.end(); ++itr)
    {
        LoadTextFile(*itr, false);
    }
    return true;
}

/*功能:读取文件中详细的任务信息。
*摘要:将打开了的文件中指定的节点信息读取出来.
*参数:pRole指定的节点的指针
    　Role用于保存该节点信息的指针
*返回值:成功返回true,失败返回false
*作者：王 钦
*创建日期:2010-7-7
*/
bool CXmlEdit::GetRoleInfor(TiXmlElement *pRole,tagRole *&Role)
{
	for (TiXmlElement* pRoleElem = pRole->FirstChildElement();
		pRoleElem!=NULL;pRoleElem=pRoleElem->NextSiblingElement())
	{
		//****************读取任务属性******************
		if (!strcmp(pRoleElem->Value(),"Attribute"))
		{
			pRoleAttribute = new tagRoleAttribute;
			GetRoleAttributeInfor(pRoleAttribute,pRoleElem);
			Role->m_nID= atoi(pRoleElem->Attribute("ID"));
			Role->vcRole_Attribute = pRoleAttribute ;
		}
        //任务前置任务
		if (!strcmp(pRoleElem->Value(),"Quest_LablePrefix"))
		{
			if (pRoleElem->FirstChildElement()!=NULL)
			{
				TiXmlElement* children=pRoleElem->FirstChildElement();
                for (; children!=NULL; children=children->NextSiblingElement())
                {
                    if( !strcmp(children->Value(),"RoleIndex") )
                    {
                        int  value = atoi(children->GetText());
                        Role->m_nRoleIndex.push_back( value );
                    }
                    
                }
			}
		}
		//****************读取任务步骤******************
		if (!strcmp(pRoleElem->Value(),"Step"))
		{
			pRoleStep = new tagRoleStep;
			pRoleStep->m_nID = atoi(pRoleElem->Attribute("ID"));
			TiXmlElement* children=pRoleElem->FirstChildElement();
			for (; children!=NULL; children=children->NextSiblingElement())
			{
			    GetRoleStepInfor(pRoleStep,children);
			}
			Role->vcRole_Step = pRoleStep;
		}
	}
	return true;
}
/*功能:读取文件中任务信息___任务属性。
*摘要:将打开了的文件中指定的节点信息读取出来.
*参数:pRole指定的节点的指针
　Role用于保存该节点信息的指针
 *返回值:成功返回true,失败返回false
 *作者：王 钦
 *创建日期:2010-7-7
 */
bool CXmlEdit::GetRoleAttributeInfor(tagRoleAttribute*& pRoleAttribute,
									 TiXmlElement* pRoleElem)
{
	if (pRoleElem->Attribute("ID")!=0)
	{
		pRoleAttribute->m_nID = atoi(pRoleElem->Attribute("ID"));
	}
	if (pRoleElem->Attribute("QuestType")!=0)
	{
		pRoleAttribute->m_QuestType = atoi(pRoleElem->Attribute("QuestType"));
	}
    if (pRoleElem->Attribute("ShareQuest")!=0)
    {
        pRoleAttribute->m_ShareQuest = atoi(pRoleElem->Attribute("ShareQuest"));
    }
	if (pRoleElem->Attribute("HardFactor")!=0)
	{
		pRoleAttribute->m_nHardFactor = atoi(pRoleElem->Attribute("HardFactor"));
	}
	if (pRoleElem->Attribute("RoleEmbracer")!=0)
	{
		pRoleAttribute->m_nRoleEmbracer = atoi(pRoleElem->Attribute("RoleEmbracer"));
	}
	if (pRoleElem->Attribute("DeleteScript")!=0)
	{
		pRoleAttribute->m_cDeleteScript = pRoleElem->Attribute("DeleteScript");
	}
	TiXmlElement* children=pRoleElem->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"ShowAble"))
		{
			pRoleAttribute->m_nShowAble = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"BeRecord"))
		{
			pRoleAttribute->m_nBeRecord = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MassPoint"))
		{
			pRoleAttribute->m_nMassPoint = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Repeated"))
		{
			pRoleAttribute->m_nRepeated =atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"DeadLost"))
		{
			pRoleAttribute->m_nDeadLost = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"LostRepeated"))
		{
			pRoleAttribute->m_nLostRepeated = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"EngageArmsRepute"))
		{
			pRoleAttribute->m_nEngageArmsRepute = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"FinishEffect"))
		{
			pRoleAttribute->m_nFinishEffect = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"ChapterNum"))
		{
			pRoleAttribute->m_nChapterNum= atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Name"))
		{
			pRoleAttribute->m_cName=children->GetText();
		}
		else if (!strcmp(children->Value(),"DiscribTextID"))
		{
			pRoleAttribute->m_nDiscribTextID=children->GetText();
		}
		else if (!strcmp(children->Value(),"EmbracerEffect"))
		{
			pRoleAttribute->m_nEmbracerEffect=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"FinishEmbracerEffect"))
		{
			pRoleAttribute->m_nFinishEmbracerEffect=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"EmbracerMapEffect"))
		{
			pRoleAttribute->m_nEmbracerMapEffect=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Icon"))
		{
			pRoleAttribute->m_nIcon=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Picture"))
		{
			pRoleAttribute->m_nPicture = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"StoryDesc"))
		{
			pRoleAttribute->m_nStoryDesc = children->GetText();
		}
		else if (!strcmp(children->Value(),"NPCID"))
		{
			pRoleAttribute->m_cNPCID = children->GetText();
		}
		else if (!strcmp(children->Value(),"NPCShowType"))
		{
			pRoleAttribute->m_nNPCShowType = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"NPCClewType"))
		{
			pRoleAttribute->m_nNPCClewType = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MinGrade"))
		{
			pRoleAttribute->m_nMiniGrade = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MaxGrade"))
		{
			pRoleAttribute->m_nMaxGrade = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"BeSearchDesc"))
		{
			pRoleAttribute->m_nBeSearchDesc = children->GetText();
		}
		else if (!strcmp(children->Value(),"BeSearchSpDesc"))
		{
			pRoleAttribute->m_nBeSearchSpDesc = children->GetText();
		}
		else if(!strcmp(children->Value(),"Odds"))
		{
			pRoleAttribute->m_nOdds = atoi(children->GetText());
		}
        else if (!strcmp(children->Value(),"TotePet"))
        {
            pRoleAttribute->m_TotePet = atol(children->GetText());
        }
        else if (!strcmp(children->Value(),"QuestSex"))
        {
            pRoleAttribute->m_QuestSex= atol(children->GetText());
        }
        else if(!strcmp(children->Value(),"ReqSkill"))
        {
            pRoleAttribute->m_ReqSkill = atoi(children->GetText());
        }
        else if (!strcmp(children->Value(),"Occupation"))
        {
            pRoleAttribute->m_Occupation= children->GetText();
        }
        else if(!strcmp(children->Value(),"Doccu"))
        {
            pRoleAttribute->m_Doccu = children->GetText();
        }
	}
	return true;
}
/*功能:读取文件中任务信息___任务步骤。
*摘要:将打开了的文件中指定的节点信息读取出来.
*参数:pRole指定的节点的指针
　Role用于保存该节点信息的指针
 *返回值:成功返回true,失败返回false
 *作者：王 钦
 *创建日期:2010-7-7
 */
bool CXmlEdit::GetRoleStepInfor(tagRoleStep*& pRoleStep,
								TiXmlElement* children)
{
	if (!strcmp(children->Value(),"NPCID"))
	{
		pRoleStep->m_cNPCID = children->GetText();
	}
	else if (!strcmp(children->Value(),"QuestCell"))
	{
		pRoleStep->m_nEncouragementCell = atoi(children->GetText());
	}
	else if (!strcmp(children->Value(),"NPCEffect_1"))
	{
		pRoleStep->m_nNPCEffect_1 = atoi(children->GetText());
	}
	else if (!strcmp(children->Value(),"NPCEffect_2"))
	{
		pRoleStep->m_nNPCEffect_2 = atoi(children->GetText());
	}
	else if (!strcmp(children->Value(),"RoleTraceStartRow"))
	{
//		pRoleStep->m_nRoleTraceStartRow = atoi(children->GetText());
	}
	else if (!strcmp(children->Value(),"RoleTrace"))
	{
		tagStepTrace *pStepTrace = new tagStepTrace;
		if (children->Attribute("Desc")!=0)
		{
			pStepTrace->m_nDesc		= children->Attribute("Desc");
		}
		if (children->Attribute("ID")!=0)
		{
			pStepTrace->m_nID		= atoi(children->Attribute("ID"));
		}
		if (children->Attribute("Param")!=0)
		{
			pStepTrace->m_nParam		= atoi(children->Attribute("Param"));
		}
		if (children->Attribute("ShowNum")!=0)
		{
			pStepTrace->m_nShowNum	= atoi(children->Attribute("ShowNum"));
		}
		if (children->Attribute("Sum")!=0)
		{
			pStepTrace->m_nSum		= atoi(children->Attribute("Sum"));
		}
		pRoleStep->vcStepRoleTrace.push_back(pStepTrace);
	}
	else if (!strcmp(children->Value(),"Aim"))
	{
		tagStepAim *pStepAim = new tagStepAim;
		pStepAim->m_nID = atoi(children->Attribute("ID"));
		pStepAim->m_nAimModel = atoi(children->Attribute("AimModel"));
		pStepAim->m_cAimName  = children->Attribute("AimName");
		pStepAim->m_nAimNum   = atoi(children->Attribute("AimNum"));
		pStepAim->m_nAimType	 = atoi(children->Attribute("AimType"));
		TiXmlElement *pSubchild = children->FirstChildElement();
		for (;pSubchild!=NULL;pSubchild = pSubchild->NextSiblingElement())
		{
			if (!strcmp(pSubchild->Value(),"DropOdds"))
			{
				pStepAim->m_nDropOdds = atoi(pSubchild->GetText());
			}
			else if (!strcmp(pSubchild->Value(),"DropFrom"))
			{
				pStepAim->m_cDropFrom = pSubchild->GetText();
			}
            else if (!strcmp(pSubchild->Value(),"Param"))
			{
				pStepAim->m_nParam = atoi(pSubchild->GetText());
			}
		}
		pRoleStep->vcStepAim.push_back(pStepAim);
	}
	return true;
}
/*功能:读取文件中任务信息___任务奖励。
*摘要:将打开了的文件中指定的节点信息读取出来.
*参数:pRole指定的节点的指针
　Role用于保存该节点信息的指针
 *返回值:成功返回true,失败返回false
 *作者：王 钦
 *创建日期:2010-7-7
 */
bool CXmlEdit::GetRoleEnInfor(TiXmlElement* pRoleElem )
{
	if (pRoleElem->Attribute("Name")!=0)
	{
		pRoleEncouragement->m_cName = pRoleElem->Attribute("Name");
	}
	if (pRoleElem->Attribute("Num")!=0)
	{
		pRoleEncouragement->m_nNum = atoi(pRoleElem->Attribute("Num"));
	}
	if (pRoleElem->Attribute("Type")!=0)
	{
		pRoleEncouragement->m_nType = atoi(pRoleElem->Attribute("Type"));
	}
	return true;
}