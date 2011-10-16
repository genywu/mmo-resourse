#include "stdafx.h"
#include "QuestSystem.h"
#include "RoleRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<LONG,CQuest*> CQuestSystem::m_mapQuestList;
vector<LONG> CQuestSystem::m_vecMainQuestList;
multimap<string,LONG> CQuestSystem::m_mmapSpecialQuestList;
map<LONG,string*> CQuestSystem::m_mapTextList;

CQuestAttribute::CQuestAttribute()
{
	m_lID=0;							//任务id
	m_lRoleType=0;						//任务类型

	for (int a=0; a<(OCC_Max/32+1);a++)
	{
		m_Occupation[a]=0xffffffff;
	}
	m_bShowAble=0;						//能否被显示

	m_lMinGrade=0;						//最小职业限制	
	m_lMaxGrade=0x7fffffff;				//最大职业限制

	m_lChapterIndex=0;					//章节数

	m_lNPCShowType=0;					//npc 可接任务提示（特殊任务）
	m_lNPCClewType=0;					//npc 任务提示(暂时不能接
										//但是几级以后可以接的任务在npc身上的提示，特殊任务)

	m_lEmbracerEffect=0;
	m_lFinishEmbracerEffect=0;
	m_lEmbracerMapEffect=0;
	m_lIcon=0;
	m_lPicture=0;



}

BOOL CQuestAttribute::Init(TiXmlElement*p)
{
	m_lID= atoi(p->Attribute("ID"));
	m_lRoleType=atoi(p->Attribute("RoleType"));

	//m_lEmbracerNPCID=atoi(p->Attribute("RoleEmbracer"));

	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		const char* ppp=children->Value();

		if (!strcmp(children->Value(),"ShowAble"))
		{
			m_bShowAble=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"ChapterNum"))
		{
			m_lChapterIndex= atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Name"))
		{
			m_strQuestName=children->GetText();
			cout<<m_strQuestName.c_str()<<endl;
		}
		else if (!strcmp(children->Value(),"Occupation"))
		{
			ParseOccupation(children->GetText());
		}
		else if (!strcmp(children->Value(),"EmbracerEffect"))
		{
			m_lEmbracerEffect=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"FinishEmbracerEffect"))
		{
			m_lFinishEmbracerEffect=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"EmbracerMapEffect"))
		{
			m_lEmbracerMapEffect=atoi(children->GetText());
		}
		
		else if (!strcmp(children->Value(),"Icon"))
		{
			m_lIcon=atoi(children->GetText());

		}
		else if (!strcmp(children->Value(),"Picture"))
		{
			m_lPicture=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"StoryDesc"))
		{
			m_lStoryDescIndex=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"NPCID"))
		{
			m_strNPCName=children->GetText();
		}
		else if (!strcmp(children->Value(),"NPCShowType"))
		{
			m_lNPCShowType=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"NPCClewType"))
		{
			m_lNPCClewType=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MinGrade"))
		{
			m_lMinGrade=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MaxGrade"))
		{
			m_lMaxGrade=atoi(children->GetText());
		}
		else
		{
		}
	}
	return TRUE;
}

int CQuestAttribute::GetNumInParse(char*& point)
{
	if(point==0 || *point=='\0')
	{
		return -1;
	}

	char Num[32]="0";
	int  Index=0;

	while (*point!='\0' )
	{
		if (isdigit(*point))
		{
			Num[Index]=*point;
			Index++;
			if (Index >=31)
			{
				//输出错误信息
				return atoi(Num);
			}
		}
		else
		{
			if (Index>0)
			{
				return atoi(Num);
			}
		}
		point++;
	}

	return -1;
}

VOID CQuestAttribute::ParseOccupation(const char * str)
{
	for (int a=0; a<(OCC_Max/32+1); a++)
	{
		m_Occupation[a]=0;
	}
	char* ptr = new char[strlen(str)+1];
	memcpy(ptr,str,strlen(str)+1);
	char* point=ptr;

	int num=GetNumInParse(point);

	while(num!=-1)
	{
		if (num>=0 && num<OCC_Max)
		{
			m_Occupation[OCC_Max/32]|=(1<<(num%64));
		}
		else
		{
			//输出错误信息
		}
		num=GetNumInParse(point);
	}
	delete []ptr;
}
//////////////////////////////
CQuestTrace::CQuestTrace()
{
	m_bShowNum=FALSE;
	//m_bIsFinish = FALSE;
	m_lSum=0;
	m_lParam=99;
	m_strID="";
	m_lNPCEffect_1=0;
	m_lNPCEffect_2=0;
	next=0;
}

CQuestTrace::~CQuestTrace()
{
}

BOOL CQuestTrace::Init(TiXmlElement* p)
{
	if (p==0)
	{
		return FALSE;
	}

	m_lDescIndex=atoi(p->Attribute("Desc"));
	m_bShowNum=atoi(p->Attribute("ShowNum"));
	m_lSum=atoi(p->Attribute("Sum"));
	m_lParam=atoi(p->Attribute("Param"));
	m_strID=p->Attribute("ID");

	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		const char* ppp=children->Value();

		if (!strcmp(children->Value(),"NPCID"))
		{
			m_strNPCName=children->GetText();
		}
		else if (!strcmp(children->Value(),"NPCEffect_1"))
		{
			m_lNPCEffect_1=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"NPCEffect_2"))
		{
			m_lNPCEffect_2=atoi(children->GetText());
		}
		else
		{

		}
	}

	return TRUE;

}

//// 该追踪是否达到完成条件
//BOOL CQuestStep::IsTraceFinish()const
//{
//	return m_bIsFinish;
//}

////////////////CQuestStep

BOOL CQuestStep::Init(TiXmlElement *p)
{
	if ( p==NULL)
	{
		return FALSE;
	}

	m_lID=atoi(p->Attribute("ID"));

	TiXmlElement*children= p->FirstChildElement();
	for (; children!=NULL ;children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"Discrib"))
		{
			m_lDiscrib=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"RoleTraceStartRow"))
		{
			m_lRoleTraceStartRow=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"RoleTrace"))
		{
			CQuestTrace* trace=new CQuestTrace;
			trace->Init(children);
			if (!AddQuestTrace(trace))
			{
				delete trace;
			}
		}
		else if (!strcmp(children->Value(),"Text"))
		{
			int index=atoi(children->Attribute("ID"));

			if (index>=0 && index<QUEST_TEXT_SUM)
			{
				m_lTextIndex[index]=atoi(children->Attribute("Name"));
			}
			else
			{
				//输出错误信息：错误的任务步骤文本id。
			}
		}
		else if (!strcmp(children->Value(),"NPCID"))
		{
			m_strNPCName=children->GetText();
		}
		else if (!strcmp(children->Value(),"NPCEffect_1"))
		{
			m_lNPCEffect_1=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"NPCEffect_2"))
		{
			m_lNPCEffect_2=atoi(children->GetText());
		}
	}
	return TRUE;
}

BOOL CQuestStep::AddQuestTrace(CQuestTrace*& p)
{
	if (m_lQuestTraceNum>QUEST_TRACE_SUM)
	{
		return FALSE;
	}
	if(m_QuestTrace==0)
	{
		m_QuestTrace=p;
		return true;
	}
	CQuestTrace* end=m_QuestTrace;
	while(end->next!=0)
	{
		end=end->next;
	}
	end->next=p;
	p->next=0;
	p=0;
	//
	//m_lQuestTraceNum++;
	//p->next=m_QuestTrace;
	//m_QuestTrace=p;
	//p=0;
	return TRUE;

}

// 判断该步骤是否做完，主要判断该步骤的每个任务追踪是否达到完成条件
//BOOL CQuestStep::IsStepFinish() const
//{
//	CQuestTrace* pTrace=m_QuestTrace;
//	while (pTrace)
//	{
//		// 任其一个追踪未完成，则该步骤亦未完成
//		if(pTrace->IsTraceFinish())
//			pTrace=pTrace->next;
//		else
//			return false;
//	}
//	return true;
//}

CQuestStep::~CQuestStep()
{
	while (m_QuestTrace!=0)
	{
		CQuestTrace* trace=m_QuestTrace;
		m_QuestTrace=m_QuestTrace->next;
		delete trace;
	}
}
//////////////////////CQuest
CQuest::~CQuest()
{
	

	while (m_pQuestStep!=0)
	{
		CQuestStep* step=m_pQuestStep;
		m_pQuestStep=m_pQuestStep->next;
		delete step;
	}
}

BOOL CQuest::Init(TiXmlElement* p)
{
	if (p==NULL)
	{
		return FALSE;
	}

	//m_lID=atoi(p->Attribute("ID"));

	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"Attribute"))
		{
			m_Attribute.Init(children);
		}
		else if (!strcmp(children->Value(),"Step"))
		{
			CQuestStep* step=new CQuestStep;
			step->Init(children);
			if (!AddStep(step))
			{
				//输出信息：加入步骤不成功
				delete step;
			}
		}
		else
		{
		}
	}
	return TRUE;
}
const CQuestStep* CQuest::GetStep(LONG ID)const
{
	CQuestStep* step=m_pQuestStep;
	while (step!=NULL)
	{
		if (step->GetID()==ID)
		{
			return step;
		}
		step = step->next;
	}
	return 0;
}

BOOL CQuest::AddStep(CQuestStep *&step)
{
	if (m_lStepNum>= QUEST_STEP_SUM)
	{
		return FALSE;
	}
	step->next=m_pQuestStep;
	m_pQuestStep=step;
	step=0;
	m_lStepNum++;
	return TRUE;
}

CQuestSystem::CQuestSystem()
{
}
CQuestSystem::~CQuestSystem()
{
	Relese();
}

BOOL CQuestSystem::Init(const char* filename)
{
	if (m_mapQuestList.size()!=0)
	{
		return FALSE;
	}

	TiXmlNode *pNode = NULL;

	//TiXmlDocument* m_Tdoc = new TiXmlDocument(filename);
	TiXmlDocument m_Tdoc;

	if(!m_Tdoc.LoadFile(filename))
	{
		//输出错误信息：错误的文件名
		return FALSE;
	}

	pNode=m_Tdoc.FirstChild("role");

	if (pNode==NULL)
	{
		//无节点，输出错误
		return FALSE;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		const char * ppp=p->Value();
		if (!strcmp(p->Value(),"Attribute"))
		{
			int a=atoi(p->Attribute("MaxMainRoleNum"));
			m_vecMainQuestList.resize(a);
		}
		else if (!strcmp(p->Value(),"RoleName"))
		{
			InitOneFile(p->GetText());
		}
		else
		{
		}
	}

	return TRUE; 
}

BOOL CQuestSystem::InitOneFile(const char* filename )
{

	TiXmlNode *pNode = NULL;

	//TiXmlDocument* m_Tdoc = new TiXmlDocument(filename);
	TiXmlDocument m_Tdoc;

	if(!m_Tdoc.LoadFile(filename))
	{
		//输出错误信息：错误的文件名
		return FALSE;
	}

	pNode=m_Tdoc.FirstChild("role");

	if (pNode==NULL)
	{
		//无节点，输出错误
		return FALSE;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{

		CQuest* quest=new CQuest;
		if (quest->Init(p))
		{
			if (!AddQuest(quest))
			{
				delete quest;
			}
		}

	}
	return TRUE;
}

BOOL CQuestSystem::InitText(const char* filename)
{
	if (m_mapTextList.size()!=0)
	{
		return FALSE;
	}

	TiXmlNode *pNode = NULL;

	//TiXmlDocument* m_Tdoc = new TiXmlDocument(filename);
	TiXmlDocument m_Tdoc;

	if(!m_Tdoc.LoadFile(filename))
	{
		//输出错误信息：错误的文件名
		return FALSE;
	}

	pNode=m_Tdoc.FirstChild("node");

	if (pNode==NULL)
	{
		//无节点，输出错误
		return FALSE;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"Text"))
		{
			LONG index=atoi(p->Attribute("ID"));
			
			//m_mapTextList[index]=str;
			map<LONG,string*>::iterator it=m_mapTextList.find(index);
			if (it==m_mapTextList.end())
			{
				string* str=new string(p->GetText());
				m_mapTextList[index]=str;
			}
		}
		else
		{

		}
	}
	return TRUE;
}


VOID CQuestSystem::Relese()
{
	map<LONG ,CQuest*>::iterator it= m_mapQuestList.begin();
	for ( ; it!=m_mapQuestList.end();it++)
	{
		delete it->second;
	}
	m_mapQuestList.clear();

	map<LONG,string*>::iterator ite=m_mapTextList.begin();
	for (; ite!=m_mapTextList.end();ite++)
	{
		delete ite->second;
	}
	m_mapTextList.clear();
}

//取得指定任务id，指定步骤下，指定索引的任务追踪
const CQuestTrace* CQuestSystem::GetQuestTrace(LONG lQuestID,LONG lStep,LONG lIndex)
{
	const CQuest *pQuest = CQuestSystem::GetQuest(lQuestID);
	if(pQuest)
	{
		const CQuestStep* pStep = pQuest->GetStep(lStep);
		if(pStep)
		{
			const CQuestTrace* pTrace = pStep->GetRoleTraceHead();
			while(pTrace)
			{
				if(pTrace->GetParam()==lIndex)
					return pTrace;
			}
		}
	}
	return NULL;
}

//取得一个任务
const CQuest* CQuestSystem::GetQuest(LONG index)
{
	map<LONG,CQuest*>::iterator it=m_mapQuestList.find(index);
	if (it!=m_mapQuestList.end())
	{
		return it->second;
	}
	return 0;
}
//取得章节数对应的主线任务
const LONG CQuestSystem::GetMainRole(LONG chapterindex)
{
	if ( (chapterindex>= 0) && (chapterindex< (LONG)m_vecMainQuestList.size()))
	{
		return m_vecMainQuestList[chapterindex];
	}
	return -1;
}
//取得特殊任务npc的特殊任务列表
vector<LONG> CQuestSystem::GetSpecialQuestByNPCID(string npcName)
{
	vector<LONG> vec;
	typedef multimap<string,LONG>::iterator IT;
	pair<IT,IT> _pair=m_mmapSpecialQuestList.equal_range(npcName);
	IT it=_pair.first;
	for (; it!=_pair.second; it++)
	{
		vec.push_back(it->second);
	}
	return vec;
}

string* CQuestSystem::GetTextByIndex(LONG index)
{
	map<LONG,string*>::iterator it=m_mapTextList.find(index);
	if (it== m_mapTextList.end())
	{
		return 0;
	}
	return it->second;
}


BOOL CQuestSystem::AddQuest(CQuest *&quest)
{

	map<LONG,CQuest*>::iterator it=m_mapQuestList.find(quest->GetID());
	if (it== m_mapQuestList.end())
	{
		//m_mapQuestList[quest->GetID()]=quest;
		if (quest->GetAttribute().GetRoleType()==ROEL_MAIN)
		{
			AddMainQuest(quest->GetAttribute().GetChapterIndex(),quest->GetID());
		}
		if (strcmp(quest->GetAttribute().GetNPCName().c_str(),"")!=0)
		{
			AddSpecialQuest(quest->GetAttribute().GetNPCName(),quest->GetID());
		}
		m_mapQuestList[quest->GetID()]=quest;
		quest=0;
		return TRUE;
	}
	return FALSE;
}
//加入一个主线任务
BOOL CQuestSystem::AddMainQuest(LONG chapterindex, LONG questID)
{
	if (chapterindex>=MAINQUEST_SUM)
	{
		return FALSE;
	}
	m_vecMainQuestList[chapterindex]=questID;
	return TRUE;
}
//加入一个特殊任务npc 和它对应的一个任务id
BOOL CQuestSystem::AddSpecialQuest(string npcName,LONG questID)
{
	m_mmapSpecialQuestList.insert(make_pair(npcName,questID));

	return TRUE;
}

