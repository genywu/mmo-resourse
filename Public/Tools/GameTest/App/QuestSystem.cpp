#include "stdafx.h"
#include "../App/QuestSystem.h"
//#include "RoleRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CQuestSystem::m_lMercQuestUpdateInter=0x7FFFFFFF;	//俑兵任务刷新间隔时间
map<long,CQuestContent::tagQuestContent*> CQuestContent::m_mapMainQuest;

map<LONG,CQuestSystem::tagQuestList> CQuestSystem::m_mapQuestList;

vector<LONG> CQuestSystem::m_vecMainQuestList;
multimap<string,LONG> CQuestSystem::m_mmapSpecialQuestList;

CQuestAttribute::CQuestAttribute()
{
	m_lID=0;							//任务id
	m_lTextTypeID = 0;
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

	m_lDescribTextID=0;				//任务的描述文本ID
	m_lHardFactor=0;				//任务的难度系数
}

BOOL CQuestAttribute::Init(TiXmlElement*p,long lTextTypeID)
{
	if(!p)
		return false;
	m_lID= atoi(p->Attribute("ID"));
	//难度系数
	p->Attribute("HardFactor",(int*)&m_lHardFactor);
	m_lRoleType=atoi(p->Attribute("RoleType"));
	m_lTextTypeID = lTextTypeID;

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
		//任务描述ID
		else if (!strcmp(children->Value(),"DiscribTextID"))
		{
			m_lDescribTextID=atoi(children->GetText());
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

BOOL CQuest::Init(TiXmlElement*p,long lTextTypeID)
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
			m_Attribute.Init(children,lTextTypeID);
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

	TiXmlDocument strQuestDoc(filename);
	if( !strQuestDoc.LoadFile() )
	{
		PutDebugString("装载配置文件<QuestList.xml>出错！");
		return FALSE;
	}

	TiXmlNode *pQuestNode = NULL;
	pQuestNode = strQuestDoc.FirstChild("role");
	if( pQuestNode == NULL ) return FALSE;

	TiXmlElement* pElem = pQuestNode->ToElement();
	if( pElem ==NULL) return FALSE;

	TiXmlElement* pAttribute = pElem->FirstChildElement();
	for(; pAttribute != NULL; pAttribute = pAttribute->NextSiblingElement() )
	{
		const char* pStrAttr = pAttribute->Value();
		if( !strcmp(pStrAttr,"Attribute"))
			InitAttribute(pAttribute);

		else if( !strcmp(pStrAttr,"RoleName") )
		{
			int iID = 0;
			int iTextType = 0;
			int iTitleTextID = 0;

			pAttribute->Attribute("ID",&iID);
			pAttribute->Attribute("TextTypeID",&iTextType);
			pAttribute->Attribute("TitleTextID",&iTitleTextID);

			tagQuestList QuestList;
			QuestList.lTextType = iTextType;
			QuestList.lTitleTextID = iTitleTextID;

			InitOneFile(pAttribute->GetText(),QuestList);
			m_mapQuestList[iID] = QuestList;
		}
	}
	return TRUE;
}

BOOL CQuestSystem::InitAttribute(TiXmlElement* p)
{
	int a=atoi(p->Attribute("MaxMainRoleNum"));					
	m_vecMainQuestList.resize(a);								//初始化最大主线任务数量
	//获取俑兵任务刷新间隔
	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL;children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"UpdateIntTime"))
		{
			m_lMercQuestUpdateInter=atoi(children->GetText());
		}
	}
	return TRUE;
}

BOOL CQuestSystem::InitOneFile(const char* filename,tagQuestList& QuestList )
{
	TiXmlNode* pQuestNode = NULL;
	TiXmlDocument pStrDoc(filename);
	if( !pStrDoc.LoadFile() )
	{
		char s[126];
		sprintf(s,"装载配置文件<%s>出错！",filename);
		PutDebugString(s);
		return FALSE;
	}

	pQuestNode = pStrDoc.FirstChild("role");
	if( pQuestNode == NULL) return FALSE;

	TiXmlElement* pEle = pQuestNode->ToElement();
	if( pEle == NULL ) return FALSE;

	TiXmlElement* pAttr = pEle->FirstChildElement();
	for(; pAttr != NULL; pAttr = pAttr->NextSiblingElement() )
	{
		CQuest* quest = new CQuest;
		if( quest->Init(pAttr,QuestList.lTextType))
			if( !AddQuest(QuestList,quest))
				delete quest;
	}
	return TRUE;
}


VOID CQuestSystem::Relese()
{
	itQuestList itList = m_mapQuestList.begin();
	for(;itList != m_mapQuestList.end();itList++)
	{
		tagQuestList& QuestList = (*itList).second;
		itQuests itQS = QuestList.Quests.begin();
		for(;itQS!=QuestList.Quests.end();itQS++)
		{
			delete itQS->second;
		}
		QuestList.Quests.clear();
	}
	m_mapQuestList.clear();
	return;
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
				pTrace = pTrace->next;
			}
		}
	}
	return NULL;
}

//取得一个任务
const CQuest* CQuestSystem::GetQuest(LONG index)
{
	itQuestList itList = m_mapQuestList.begin();
	for(;itList != m_mapQuestList.end();itList++)
	{
		tagQuestList& QuestList = (*itList).second;
		itQuests itQS = QuestList.Quests.find(index);
		if(itQS != QuestList.Quests.end())
			return (*itQS).second;
	}
	return NULL;
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


BOOL CQuestSystem::AddQuest(tagQuestList& QuestList, CQuest* quest)
{

	itQuests it=QuestList.Quests.find(quest->GetID());
	if ( it== QuestList.Quests.end())
	{		
		if (quest->GetAttribute().GetRoleType()==ROEL_MAIN)
		{
			AddMainQuest(quest->GetAttribute().GetChapterIndex(),quest->GetID());
		}
		if (strcmp(quest->GetAttribute().GetNPCName().c_str(),"")!=0)
		{
			AddSpecialQuest(quest->GetAttribute().GetNPCName(),quest->GetID());
		}

		QuestList.Quests[quest->GetID()]=quest;
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

void CQuestSystem::GetmapQuestList(LONG nID,long &TextType,long &lTitleTextID)
{
	TextType = m_mapQuestList[nID].lTextType;
	lTitleTextID = m_mapQuestList[nID].lTitleTextID;
}

BOOL CQuestContent::InitQuest(const char* filename)
{
	if (m_mapMainQuest.size()!=0)
	{
		return FALSE;
	}

	TiXmlDocument strQuestDoc(filename);
	if( !strQuestDoc.LoadFile() )
	{
		PutDebugString("装载配置文件<QuestContent.xml>出错！");
		return FALSE;
	}

	TiXmlNode *pQuestNode = NULL;
	pQuestNode = strQuestDoc.FirstChild("quest");
	if( pQuestNode == NULL ) return FALSE;

	TiXmlElement* pElem = pQuestNode->ToElement();
	if( pElem ==NULL) return FALSE;

	TiXmlElement* pAttribute = pElem->FirstChildElement();
	for(; pAttribute != NULL; pAttribute = pAttribute->NextSiblingElement() )
	{
		const char* pStrAttr = pAttribute->Value();
		if( strcmp(pStrAttr,"Role") == 0 )
		{
			tagQuestContent* pQuest = new tagQuestContent;
			TiXmlElement* pContent = pAttribute->FirstChildElement();
			for(;pContent!=NULL;pContent = pContent->NextSiblingElement())
			{
				if(!strcmp(pContent->Value(),"Cont"))
				{
					int iRegionID =0;
					int iPosX = 0;
					int iPosY = 0;
					pContent->Attribute("RegionID",&iRegionID);
					pContent->Attribute("PosX",&iPosX);
					pContent->Attribute("PosY",&iPosY);

					pQuest->strName = pContent->Attribute("Name");
					pQuest->lPosX = iPosX;
					pQuest->lPosY = iPosY;
					pQuest->lRegionID = iRegionID;
				}
			}
			int lQuestID = 0;
			pAttribute->Attribute("ID",&lQuestID);
			m_mapMainQuest.insert(make_pair(lQuestID,pQuest));
			//			m_mapMainQuest[lQuestID] = quest;
		}
	}
	return TRUE;
}

CQuestContent::tagQuestContent* CQuestContent::GetQuest(long lQuestID)
{
	if( m_mapMainQuest.find(lQuestID) != m_mapMainQuest.end() )
	{
		tagQuestContent* quest = (tagQuestContent*)m_mapMainQuest[lQuestID];
		return quest;
	}
	return NULL;
}

void CQuestContent::Release()
{
	if( m_mapMainQuest.size()>0)
	{
		map<long,tagQuestContent*>::iterator it = m_mapMainQuest.begin();
		for(;it!=m_mapMainQuest.end();)
		{
			SAFE_DELETE((*it).second);
			m_mapMainQuest.erase(it++);
			//			it++;
		}
		m_mapMainQuest.clear();
	}
}
