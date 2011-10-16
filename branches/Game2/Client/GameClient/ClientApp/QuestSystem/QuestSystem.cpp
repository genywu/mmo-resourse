#include "stdafx.h"
#include "QuestSystem.h"
#include "../Player.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../Goods/GoodsList.h"
#include "../../Game.h"
#include "../../../Public/Common/CStrConverter.h"
#include "../../../Net/Message.h"
#include "../../../ui/Windows/QuestDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CQuestSystem::m_lMercQuestUpdateInter=0x7FFFFFFF;	//俑兵任务刷新间隔时间

map<LONG,CQuestSystem::tagQuestList> CQuestSystem:: m_mapQuestList;
vector<CQuest*> CQuestSystem:: m_vecCanBeSearchQuestList;

vector<LONG> CQuestSystem::m_vecMainQuestList;
multimap<string,LONG> CQuestSystem::m_mmapSpecialQuestList;
vector<int> CQuestSystem::m_vecMerItemList;

map<LONG,CQuestSystem::tagOddsColor> CQuestSystem::m_mapOddsColor;
std::map<long,CQuestEncourMan::QuestAward*>  CQuestEncourMan::m_Award;

CQuestRecord* CQuestSystem::m_TempQuestRecord;

CQuestAttribute::CQuestAttribute()
{
	m_lID=0;							//任务id
	m_lTextTypeID = 0;
	m_lQuestType=0;						//任务类型

	/*for (int a=0; a<(OCC_Max/32+1);a++)
	{
		m_Occupation[a]=0xffffffff;
	}*/
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
	m_lOdds=0;                      //任务稀有度 

	m_lDescTextIDBySearch = 0;		// 查询描述ID
	m_lSpDescTextIDBySearch = 0;	// 查询简述ID
}

BOOL CQuestAttribute::Init(TiXmlElement*p,long lTextTypeID)
{
	if(!p)
		return false;
	m_lID= atoi(p->Attribute("ID"));
	//难度系数
	p->Attribute("HardFactor",(int*)&m_lHardFactor);
	m_lQuestType=atoi(p->Attribute("QuestType"));
	m_lTextTypeID = lTextTypeID;
    m_lIsShareQuest = atoi(p->Attribute("ShareQuest"));

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
		 //任务描述ID
		else if (!strcmp(children->Value(),"DiscribTextID"))
		{
			m_lDescribTextID=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Occupation"))
		{
			//InitOccu(m_vecOccu,children->GetText());
            ParseOccupation( children->GetText());
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
		// 查询描述
		else if (!strcmp(children->Value(),"BeSearchDesc"))
		{
			m_lDescTextIDBySearch=atol(children->GetText());
		}
		// 查询简述
		else if (!strcmp(children->Value(),"BeSearchSpDesc"))
		{
			m_lSpDescTextIDBySearch=atol(children->GetText());
		}
		// 副职业限制
		else if (!strcmp(children->Value(),"DOccu"))
		{
			InitOccu(m_vecDOccu,children->GetText());
		}
		//任务稀有度
		else if(!strcmp(children->Value(),"Odds"))
		{
            m_lOdds=atoi(children->GetText());
		}
		else
		{
		}
	}
	return TRUE;
}

// 初始化前置任务
VOID CQuestAttribute::InitPreQuest(TiXmlElement*p)
{
	if(!p)
		return;
	m_vecPreQuest.clear();
	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"RoleIndex"))
		{
			LONG lID = atol(children->GetText());
			if(lID > 0)
				m_vecPreQuest.push_back(lID);
		}
	}
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
	//for (int a=0; a<(OCC_Max/32+1); a++)
	//{
	//	m_Occupation[a]=0;
	//}
	//char* ptr = new char[strlen(str)+1];
	//memcpy(ptr,str,strlen(str)+1);
	//char* point=ptr;

	//int num=GetNumInParse(point);

	//while(num!=-1)
	//{
	//	if (num>=0 && num<OCC_Max)
	//	{
	//		m_Occupation[OCC_Max/32]|=(1<<(num%64));
	//	}
	//	else
	//	{
	//		//输出错误信息
	//	}
	//	num=GetNumInParse(point);
	//}
	//delete []ptr;
}


// 初始化副职业限制技能
void CQuestAttribute::InitOccu(vector<BYTE>& vecOccu,const char *str)
{
	if(str == NULL || *str == '\0')
		return;
	char * pOccuStr = (char*)str;
	char Num[32]="0";
	int  Index=0;
	vecOccu.clear();
	while (*pOccuStr != '\0')
		{
		if(*pOccuStr != '/')
		{
			Num[Index] = *pOccuStr;
			Num[++Index] = '\0';
		}
		else
		{
			if(strlen(Num) > 0)
			{
				BYTE bOccu = (BYTE)atoi(Num);
				vecOccu.push_back(bOccu);
				Index=0;
			}
		}
		++pOccuStr;
	}
	if(strlen(Num) > 0)
	{
		BYTE bOccu = (BYTE)atoi(Num);
		vecOccu.push_back(bOccu);
		}
	}

// 根据ID查找相应的副职业是否可接该任务
bool CQuestAttribute::GetDOccuRestrict(BYTE bDOccuID)const
{
	// 无副职业限制，直接返回真
	if(m_vecDOccu.empty())
		return true;
	for (BYTE i=0;i<m_vecDOccu.size();++i)
	{
		if(m_vecDOccu[i] == bDOccuID)
			return true;
    }
	return false;
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

	m_lDescIndex=p->Attribute("Desc");
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
			m_lDiscrib=children->GetText();
		}
		else if (!strcmp(children->Value(),"RoleTraceStartRow"))
		{
			m_lQuestTraceStartRow=atoi(children->GetText());
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

//-----------CQusetEncouragement
CQusetEncouragement::CQusetEncouragement()
{
    m_lType=0;
    m_lID=0;
    m_lNum=0;
    next=0;
    memset(m_Name,0,sizeof(m_Name));
}
BOOL CQusetEncouragement::Init(TiXmlElement*p)
{
    if (p==NULL)
    {
        return FALSE;
    }
    m_lType=atoi(p->Attribute("Type"));

    string name=p->Attribute("Name");
    if (name.size()<48)
    {
        memcpy(m_Name,name.c_str(),name.size()+1);
    }
    else
    {
        memcpy(m_Name,name.c_str(),48);
        m_Name[48]=0;
    }

    m_lID=0;

    m_lNum=atoi(p->Attribute("Num"));
    m_fOdds=(float)atof(p->Attribute("Odds"));
    return TRUE;

}

CQuestEncourMan::CQuestEncourMan()
{
    Log4c::Trace(ROOT_MODULE,"CQuestEncourMan");
    m_Award.clear();
}

CQuestEncourMan::~CQuestEncourMan()
{

}

void CQuestEncourMan::Release()
{
    std::map<long,QuestAward*>::iterator it = m_Award.begin();
    for ( ; it!= m_Award.end() ; it++ )
    {
        QuestAward*  award = it->second;
        CQusetEncouragement * p = award->Encour;
        while ( award->Encour != NULL )
        {
            p =  award->Encour->next;
            delete award->Encour;
            award->Encour = p;
        }
        delete award;
    }
    Log4c::Trace(ROOT_MODULE,"~CQuestEncourMan::Release()");
}

string CQuestEncourMan::GetAwardText(long Qid)
{
      string  str=AppFrame::GetText("AwardInfo_0");//"[BR] [BR]任务奖励:[BR] ";
      string  res="";
      char    pfor[128];
      std::map<long,QuestAward*>::iterator it= m_Award.find( Qid );
      if ( it != m_Award.end() )
      {
          QuestAward * award = it->second;
          if ( award )
          {
              long sum = award->Sum;
              CQusetEncouragement* encour = award->Encour;
              /// 输出格式化文本,@前期只能做任务的文字描述
              for ( int  i = 0 ; i < sum && encour ; i++ )
              {
                  /**
                   *   奖励:继续使用以前的类型
                   *   Type ：
                   *   1 经验
                   *   2 SP
                   *   3 物品
                   *   4 硬币
                   *   100 完成触发脚本
                   */
                   CQusetEncouragement * p = encour;
                   if ( p )
                   {
                       char   AwardRes[32];
                       sprintf_s( AwardRes ,"AwardInfo_%d",p->GetType());
                       const char * pStr = AppFrame::GetText(AwardRes);
                       if ( p->GetType() != 3 && p->GetType() < 100 )
                       {
                           sprintf_s<128>( pfor , pStr ,p->GetNum() ,p->GetOdds() );
                           string  pstr = pfor;
                           pstr +=res;
                           res = pstr;
                       }
                       else if ( p->GetType() == 3 )
                       {
                           if( CGoodsList::GetProperty(p->GetName()) )
                                sprintf_s<128>( pfor ,pStr ,CGoodsList::GetProperty(p->GetName())->BaseProperty.strName.c_str(), p->GetNum()  );
                           else
                                sprintf_s<128>( pfor ,pStr , AppFrame::GetText("AwardInfo_5"), p->GetNum()  );
                           string  pstr = pfor;
                           pstr +=res;
                           res = pstr;
                       }
                   }
                   encour = encour->next;
              }
          }
      }
      str +=res;
      return str;
}

BOOL CQuestEncourMan::Init(const char* filename)
{
    Release();

    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return FALSE;
    }

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_15"));
        return FALSE;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("AwardList");

    if (pNode==NULL)
    {
        //无节点，输出错误
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return FALSE;
    }

    TiXmlElement* p=te->FirstChildElement();

    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"list"))
        {
            string  str = p->Attribute("file");

            /// 载入文本
            LoadAward( str.c_str() );
        }
        else
        {
            //输出错误信息
        }
    }
    return TRUE;
}

BOOL CQuestEncourMan::LoadAward(const char* filename)
{
    if( !filename ) return FALSE;

    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return FALSE;
    }

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_15"));
        return FALSE;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("Award");

    if (pNode==NULL)
    {
        //无节点，输出错误
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return FALSE;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return FALSE;
    }

    TiXmlElement* p=te->FirstChildElement();

    for (; p!=NULL; p=p->NextSiblingElement())
    {
        if (!strcmp(p->Value(),"Quest"))
        {
            long    Qid = atol( p->Attribute("id") );
            long    EncouragementCell = atol( p->Attribute("EncouragementCell") );
            QuestAward *  Award = new QuestAward;
            Award->Cell = EncouragementCell;
            CQusetEncouragement *agement = Award->Encour;
            TiXmlElement* Cell = p->FirstChildElement("Encouragement");
            for (; Cell!=NULL; Cell=Cell->NextSiblingElement())
            {
                CQusetEncouragement *  Encouragment = new CQusetEncouragement;
                Encouragment->Init( Cell );
                if( !Award->Encour )
                    Award->Encour = Encouragment;
                else
                {
                    Encouragment->next = Award->Encour;
                    Award->Encour = Encouragment;
                }
                Award->Sum ++ ;     
            }
            //agement = NULL;
            m_Award[ Qid ]  = Award;
        }
        else
        {
            //输出错误信息
        }
    }
    return TRUE;
}

long    CQuestEncourMan::GetEncourSum(long Qid )
{
    std::map<long,QuestAward*>::iterator it = m_Award.find( Qid );
    if (  it != m_Award.end() )
    {
        return it->second->Sum;
    }
    return 0;
}

long    CQuestEncourMan::GetEncourCell(long Qid )
{
    std::map<long,QuestAward*>::iterator it = m_Award.find( Qid );
    if (  it != m_Award.end() )
    {
        return it->second->Cell;
    }
    return 0;
}

CQusetEncouragement* CQuestEncourMan::GetEncouragement(long Qid )
{
    std::map<long,QuestAward*>::iterator it = m_Award.find( Qid );
    if (  it != m_Award.end() )
    {
        return it->second->Encour;
    }
    return NULL;
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
		// 属性
		if (!strcmp(children->Value(),"Attribute"))
		{
			m_Attribute.Init(children,lTextTypeID);
		}
		// 前置任务
		else if (!strcmp(children->Value(),"Quest_LablePrefix"))
		{
			m_Attribute.InitPreQuest(children);
		}
		// 步骤
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
	m_vecCanBeSearchQuestList.clear();
	//=================================================================//
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		return FALSE;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(filename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);
	//=================================================================//
	pNode=m_Tdoc.FirstChild("Quest");

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
			InitAttribute(p);
		}
		else if (!strcmp(p->Value(),"QuestName"))
		{
			int nID = 0;
			int nTextType = 0;
			int nTitileTextID=0;
			int nCanSearch=0;
			
			p->Attribute("ID",&nID);
			p->Attribute("TextTypeID",&nTextType);
			p->Attribute("TitleTextID",&nTitileTextID);
			p->Attribute("BeSearch",&nCanSearch);

			tagQuestList QuestList;
			QuestList.lTextType = nTextType;
			QuestList.lTitleTextID = nTitileTextID;
			QuestList.bIsCanBeSearch = nCanSearch>0?true:false;

			InitOneFile(p->GetText(),QuestList);

			m_mapQuestList[nID] = QuestList;
		}
		else if(!strcmp(p->Value(),"MerItem"))
		{
			int dwItemIndex = 0;
			int nID = 0;
			p->Attribute("ID",&nID);
			p->Attribute("ItemIndex",&dwItemIndex);
			m_vecMerItemList.push_back(dwItemIndex);
		}
		else 
		{
		}
	}

    CQuestEncourMan::Init("quest/AwardList.xml");

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
		}else if(!strcmp(children->Value(),"MercQuestOdds"))
		{
			InitOdds(children);
		}
	}
	return TRUE;
}

BOOL CQuestSystem::InitOneFile(const char* filename,tagQuestList& QuestList )
{
	//=================================================================//
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		return FALSE;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(filename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);
	//=================================================================//
	pNode=m_Tdoc.FirstChild("Quest");

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
		if (quest->Init(p,QuestList.lTextType))
		{
			if (!AddQuest(QuestList,quest))
			{
				delete quest;
			}
		}

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
	m_vecCanBeSearchQuestList.clear();
    CQuestEncourMan::Release();
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
			const CQuestTrace* pTrace = pStep->GetQuestTraceHead();
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
const LONG CQuestSystem::GetMainQuest(LONG chapterindex)
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
		if (quest->GetAttribute().GetQuestType()==QUEST_MAIN)
		{
			AddMainQuest(quest->GetAttribute().GetChapterIndex(),quest->GetID());
		}
		if (strcmp(quest->GetAttribute().GetNPCName().c_str(),"")!=0)
		{
			AddSpecialQuest(quest->GetAttribute().GetNPCName(),quest->GetID());
		}
		QuestList.Quests[quest->GetID()]=quest;
		// 可查询任务
		if(QuestList.bIsCanBeSearch)
			m_vecCanBeSearchQuestList.push_back(quest);
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
	if((size_t)chapterindex < m_vecMainQuestList.size())
	{
		m_vecMainQuestList[chapterindex]=questID;
		return TRUE;
	}
	return FALSE;
}
//加入一个特殊任务npc 和它对应的一个任务id
BOOL CQuestSystem::AddSpecialQuest(string npcName,LONG questID)
{
	m_mmapSpecialQuestList.insert(make_pair(npcName,questID));

	return TRUE;
}

void CQuestSystem::GetmapQuestList(LONG nID,long &TextType,long &lTitleTextID)
{
	if(m_mapQuestList.find(nID)!=m_mapQuestList.end())
	{
		TextType = m_mapQuestList[nID].lTextType;
		lTitleTextID = m_mapQuestList[nID].lTitleTextID;
	}
}

//初始化稀有度对应颜色值
bool CQuestSystem::InitOdds(TiXmlElement *p)
{
	//int a=atoi(p->Attribute("MaxRandomNum"));	
	TiXmlElement* children=p->FirstChildElement();
	int nID = 0;
	for (; children!=NULL;children=children->NextSiblingElement())
	{
		const char * Value = children->Value();
		if (!strcmp(children->Value(),"Odds"))
		{
			int value = 0;
			int Randome = 0;
			string strcolor = "";
			long lColor = 0;
			children->Attribute("value",&value);
			children->Attribute("RandomeValue",&Randome);

			if(children->Attribute("OddsColor",strcolor.c_str())) //如果配置里没有，就跳过
			{
				strcolor = children->Attribute("OddsColor",strcolor.c_str());
				lColor = CStrConverter::parseStringToDec(strcolor.c_str());
			}


			m_mapOddsColor[nID].OddsValue = value;
			m_mapOddsColor[nID].OddsColor = lColor;
		}
		nID++;
	}
	return true;

}

/////////////////////////////////////////////////
// zhaohang  2010/5/27 
// 任务消息管理
QuestMsgMgr::QuestMsgMgr()
{

}

QuestMsgMgr::~QuestMsgMgr()
{

}

void QuestMsgMgr::Send_Disband(uint questID, eQuestType Type)
{ 
    CMessage msg(MSG_C2S_QUEST_Disband);
    msg.Add((LONG)questID);
    msg.Add((BYTE)Type);//不是佣兵任务
    msg.Send();
}

void QuestMsgMgr::Send_Shared(uint questID)
{
	Send_Commend(questID,0);
}

void QuestMsgMgr::Send_TeamQuestUpdate(uint questID)
{
	Send_Commend(questID,1);
}
void QuestMsgMgr::Send_Accept(uint questID,eQuestType Type)
{
	CMessage msg(MSG_C2S_QUEST_Accept);
	msg.Add((LONG)questID);
	msg.Add((BYTE)1);//赏金任务
	msg.Add((BYTE)0);
	msg.Add((BYTE)0);   
	msg.Send();
}

void QuestMsgMgr::Send_Accept_TeamQuest(uint questID,eQuestType Type /* = QUEST_TYPE_MERCENARY */)
{
	CMessage msg(MSG_C2S_QUEST_Accept);
	msg.Add((LONG)questID);
	msg.Add((BYTE)QUEST_TYPE_MERCENARY);   
	msg.Add((BYTE)1);   
	msg.Add((BYTE)0);   
	msg.Send();
}

void QuestMsgMgr::Send_Complete(uint questID,eQuestType Type)
{
	CMessage msg(MSG_C2S_QUEST_Complete);
	msg.Add((LONG)questID);
	msg.Add((BYTE)Type);//佣兵任务
	msg.Send();
}

//发送更新个人任务列表
void QuestMsgMgr::Send_Update(uint questID,bool isUseGoods)
{
	CMessage msg(MSG_C2S_MERCQUEST_Update);
	msg.Add((LONG)questID);
	msg.Add((BYTE)isUseGoods);  //是否使用物品 
	msg.Send();
}

//发送推荐任务，(type参数为0表示向队伍推荐一个任务，1表示请求队伍任务列表)
void QuestMsgMgr::Send_Commend(uint questID,BYTE type)
{
    const CQuest* quest = CQuestSystem::GetQuest(questID);
    if( !quest )    return;
    if (quest->GetAttribute().GetIsShareQuest() == 1 )
    {
        CPlayer* player = GetGame()->GetMainPlayer();
        if(player->GetTeamID() == CGUID::GUID_INVALID)
        {
            GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("TM_43"));
            return ;
        }
        CMessage msg(MSG_C2S_SHARE_QUESTLIST_UPDATE);
        CPlayer *pPlayer = GetGame()->GetMainPlayer();
        msg.Add(pPlayer->GetExID());          //玩家的GUID
        msg.Add((BYTE)type);                  //推荐任务的类型
        msg.Add((LONG)questID);               //推荐的任务ID
        msg.Add((char *)pPlayer->GetName());  //推荐人名字 
        msg.Send();
    }
    else
    {
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("TM_44"));
        return ;
    }
}

/////////////////////////////////////////////////
// zhaohang  2010/6/29 
// 队伍任务数据
////////////////////////////////////////////////
TeamQuestData::TeamQuestData()
{
	//SetTeamQuestID(-1);
	curRunningID = -1;
	Clear();
}
TeamQuestData::~TeamQuestData()
{
	Clear();
}

void TeamQuestData::Add(long ID,const char* srcPlayerName)
{
	//玩家已经推荐过，用新的替换
	for(uint i = 0 ; i < vecTeamQuest.size() ; ++i)
	{
		if(strcmp(vecTeamQuest[i].strCommenderName.c_str(),srcPlayerName)==0)
		{
			vecTeamQuest[i].lQuestID = ID;
			return;
		}
	}
	//没有推荐过,加入列表
	tagTeamQuest tq;
	tq.lQuestID = ID;
	tq.strCommenderName = srcPlayerName;
	vecTeamQuest.push_back(tq);
	NotifyUpdateUI();
}

void TeamQuestData::Del(const char* srcPlayerName)
{
	vector<tagTeamQuest> vecTemp;
	for(uint i = 0 ; i < vecTeamQuest.size(); ++i)
	{
		if(strcmp(vecTeamQuest[i].strCommenderName.c_str(),srcPlayerName)!= 0)
		{
			vecTemp.push_back(vecTeamQuest[i]);
		}
	}
	Clear();
	vecTeamQuest = vecTemp;
	NotifyUpdateUI();
}

bool TeamQuestData::IsCommended(long ID)
{
	for(uint i = 0 ; i < vecTeamQuest.size() ; ++i)
	{
		if(vecTeamQuest[i].lQuestID == ID)
			return true;
	}
	return false;
}

void TeamQuestData::NotifyUpdateUI()
{
	FireUIEvent(QUEST_PAGE_NAME,QUEST_TEAMFUNC_UPDATE);
}

void TeamQuestData::NotifyUIQuestRunning(long ID)
{
	curRunningID = ID;
	FireUIEvent(QUEST_PAGE_NAME,QUEST_TEAMFUNC_UPDATERUNNING);
}

TeamQuestData::tagTeamQuest* TeamQuestData::Get(uint index)
{
	if(index < vecTeamQuest.size() && vecTeamQuest.size() > 0)
		return &vecTeamQuest[index];
	else
		return NULL;
}

const char* CQuestAttribute::GetQuestName() const
{
    char quest[16];
    sprintf_s( quest ,"Q%d", m_lID );
    return AppFrame::GetText( quest );
}