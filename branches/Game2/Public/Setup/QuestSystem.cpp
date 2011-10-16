#include "StdAfx.h"
#include "QuestSystem.h"
#include "GoodsSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


long CQuestSystem::m_lMaxQuestNum=0;					//任务的最大数量

long CQuestSystem::m_lMercQuestUpdateInter=0x7FFFFFFF;	//俑兵任务刷新间隔时间
char CQuestSystem::m_pszUpdateScript[MAX_PATH];			//俑兵任务刷新脚本

long CQuestSystem::m_lQuestLvelSpan = 0;				//任务等级跨度
long CQuestSystem::m_lMaxRandomNum = 10000;				//最大随机数
long CQuestSystem::m_lOddsRandomNum[QUEST_MAX_ODDS];//稀有度相关的随机数值(稀有度最大值为4)
long CQuestSystem::m_lOddsRandomNumUseGoods[QUEST_MAX_ODDS];
map<long,vector<CQuest*>> CQuestSystem::m_mapQuestsWithOdds[QUEST_MAX_ODDS];//按稀有度分类的任务数据结构
string CQuestSystem::m_strAddQuestScript;
string CQuestSystem::m_strCompleteQuestScript;
string CQuestSystem::m_strQuestAccept;
string CQuestSystem::m_strQuestUpdate;
map<long,long>CQuestSystem::m_mapMerItem;//用来刷新赏金任务的物品编号
std::map<long,CQuestEncourMan::QuestAward*>  CQuestEncourMan::m_Award;


map<long,CQuestSystem::tagQuestList> CQuestSystem:: m_mapQuestList;

vector<long> CQuestSystem::m_vecMainQuestList;				//主线任务列表
//---------CQuestAttribute
CQuestAttribute::CQuestAttribute()
:m_lNextRoleID(0)
{
	m_lID=0;							//任务id
	m_lQuestType=0;					//任务类型
	m_bShowAble=false;					//能否被显示
	m_bRepeated=false;					//能否反复接受
	m_bDeadLost=false;					//死亡任务失败
	m_bLostRepeated=false;				//失败重接开关
	m_lMinGrade=0;					//等级下限
	m_lMaxGrade=0x7fffffff;					//等级上限
	
	m_lOdds=0;						//任务稀有程度
	m_lChapterNum=-1;					//章节数
	m_lFinishEffect=0;
	m_lMassPoint=0;					//集结点
    m_QuestSex  =2;
    m_ReqSkill  =-1;
    m_lShareQuest=0;  
    m_lTotePet  =-1;
    for (int a=0; a<(OCC_Max/32+1); a++)
    {
        m_Occupation[a]=0xffffffff;
    }
}

bool CQuestAttribute::Init(TiXmlElement* p)
{
	p->Attribute("ID",(int*)&m_lID);
	p->Attribute("QuestType",(int*)&m_lQuestType);
	p->Attribute("ShareQuest",(int*)&m_lShareQuest);
	if( p->Attribute("DeleteScript") )
	{
		strncpy(m_cDeleteScript,p->Attribute("DeleteScript"),50);
	}
	else
	{
		strncpy(m_cDeleteScript,"",50);
	}
	

	TiXmlElement* children=p->FirstChildElement();

	for (; children!=NULL;children=children->NextSiblingElement())
	{
		//集结点
		if (!strcmp(children->Value(),"MassPoint"))
		{
			m_lMassPoint=atoi(children->GetText());
		}

		else if (!strcmp(children->Value(),"ShowAble"))
		{
			m_bShowAble=atoi(children->GetText())>0 ? true:false;
		}
		else if (!strcmp(children->Value(),"BeRecord"))
		{
			m_bBeRecord=atoi(children->GetText())>0 ? true: false;
		}
		else if (!strcmp(children->Value(),"MinGrade"))
		{
			m_lMinGrade=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MaxGrade"))
		{
			m_lMaxGrade=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Occupation"))
		{
			ParseOccupation(children->GetText());
		}
        else if (!strcmp(children->Value(),"Doccu"))
        {
            /// 服务器增加副职业验证
        }
		else if (!strcmp(children->Value(),"Repeated"))
		{
			m_bRepeated=atoi(children->GetText())>0 ? true:false;
		}
		else if (!strcmp(children->Value(),"DeadLost"))
		{
			m_bDeadLost=atoi(children->GetText())>0 ? true:false;
		}
		else if (!strcmp(children->Value(),"LostRepeated"))
		{
			m_bLostRepeated=atoi(children->GetText())>0 ? true:false;
		}
		else if (!strcmp(children->Value(),"NextRole"))
		{
			m_lNextRoleID=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"ChapterNum"))
		{
			m_lChapterNum=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Odds"))
		{
			m_lOdds=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"TotePet"))
		{
			m_lTotePet=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"FinishEffect"))
		{
			m_lFinishEffect=atoi(children->GetText());
		}
        else if (!strcmp(children->Value(),"QuestSex"))
        {
            m_QuestSex = (BYTE)(atoi(children->GetText()));    
        }
        else if (!strcmp(children->Value(),"ReqSkill"))
        {
            m_ReqSkill = atoi( children->GetText() );
        }
		else
		{
			//输出错误信息
		}
	}
	return true;
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

void CQuestAttribute::ParseOccupation(const char * str)
{
	for (int a=0; a<(OCC_Max/32+1); a++)
	{
		m_Occupation[a]=0;
	}
	char* ptr=new char[strlen(str)+1];
	memcpy(ptr,str,strlen(str)+1);
	char* point=ptr;

	int num=GetNumInParse(point);

	while(num!=-1)
	{
		if (num>=0 && num<OCC_Max)
		{
			m_Occupation[OCC_Max/32]|=(1<<(num%32));
		}
		else
		{
			//输出错误信息
		}
		num=GetNumInParse(point);
	}
	delete []ptr;
}

//----------------CQuestLabelPrefix

CQuestLabelPrefix::CQuestLabelPrefix()
{
	m_lLabelSum=0;
	for (int a=0  ; a<LABELPREFIX_NUM; a++)
	{
		m_lLabelPrefix[a]=0;
	}
}
bool CQuestLabelPrefix::Init(TiXmlElement* p)
{
	TiXmlElement * children=p->FirstChildElement();

	for (; children != NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"RoleIndex"))
		{
			AddLabelPrefix(atoi(children->GetText()));
		}
		else
		{
			//输出错误信息：不合法的前置任务配置
		}
	}
	return true;
}
long CQuestLabelPrefix::GetLabelPrefix(long Index)const
{
	if (Index <=0 || Index>m_lLabelSum || Index>LABELPREFIX_NUM)
	{
		return 0;
	}
	return m_lLabelPrefix[Index-1];
}
//加入一个前置任务id记录
bool CQuestLabelPrefix::AddLabelPrefix(long val)
{
	if(m_lLabelSum>=LABELPREFIX_NUM)
		return false;
	m_lLabelPrefix[m_lLabelSum]=val;
	m_lLabelSum++;
	return true;
}

//---------------tagQuestAim 
CQuestAim::CQuestAim()
{
	m_lID=0;
	m_AMTType=AMT_KILL;
	m_lAimType=0;
	m_lAimID=0;
	m_lAimNum=0;
	m_lParamIndex=0;
	m_lDropFrom=0;
	m_lDropOdds=0;
	for (int a=0; a<PREPARE_PARAM_NUM ; a++)
	{
		m_lPrepareParam[a]=0;
	}
	next=0;
}

bool CQuestAim::Init(TiXmlElement*p)
{
	if (p==NULL)
	{
		return false;
	}
	p->Attribute("ID",(int*)&m_lID);
	p->Attribute("AimModel",(int*)&m_AMTType);
	p->Attribute("AimType",(int*)&m_lAimType);

	//解析 目标ID	根据不同的类型来解析
	if (p->Attribute("AimName"))
	{
		strncpy(m_cAimName,p->Attribute("AimName"),50);
	}
	else
	{
		strncpy(m_cAimName,"",50);
	}

#ifndef _DB_SERVER_
		m_lAimID=GoodsSetup::QueryGoodsIDByOriginalName(m_cAimName);
#else
		m_lAimID=0;
#endif


	//解析完毕
	m_lAimNum=atoi(p->Attribute("AimNum"));

	TiXmlElement * children=p->FirstChildElement();
	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"Param"))
		{
			m_lParamIndex=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"DropFrom"))
		{
			m_lDropFrom=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"DropOdds"))
		{
			m_lDropOdds=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_1"))
		{
			m_lPrepareParam[0]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_2"))
		{
			m_lPrepareParam[1]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_3"))
		{
			m_lPrepareParam[2]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_4"))
		{
			m_lPrepareParam[3]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_5"))
		{
			m_lPrepareParam[4]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_6"))
		{
			m_lPrepareParam[5]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_7"))
		{
			m_lPrepareParam[6]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_8"))
		{
			m_lPrepareParam[7]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_9"))
		{
			m_lPrepareParam[8]=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"PrepareParam_10"))
		{
			m_lPrepareParam[9]=atoi(children->GetText());
		}
		else
		{
			//错误配置： 没有的目标成员变量配置
		}
	}
	return true;
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
bool CQusetEncouragement::Init(TiXmlElement*p)
{
    if (p==NULL)
    {
        return false;
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

#ifndef _DB_SERVER_
    m_lID=GoodsSetup::QueryGoodsIDByOriginalName(name.c_str());
#else
    m_lID=0;
#endif

    m_lNum=atoi(p->Attribute("Num"));
    m_fOdds=(float)atof(p->Attribute("Odds"));
    return true;

}

void  CQusetEncouragement::AddToByteArray(vector<uchar>*& pByteArray)
{
    _AddToByteArray( pByteArray , m_lType);
    _AddToByteArray( pByteArray , m_lID );
    _AddToByteArray( pByteArray , m_lNum);
    _AddToByteArray( pByteArray , m_fOdds );  
    _AddToByteArray( pByteArray , m_Name );
}

void  CQusetEncouragement::DecodeQuestAward(uchar* pByte, long& pos)
{
    m_lType = _GetLongFromByteArray(pByte,pos);
    m_lID   = _GetLongFromByteArray(pByte,pos);
    m_lNum  = _GetLongFromByteArray(pByte,pos);
    m_fOdds = _GetFloatFromByteArray(pByte,pos);   
    _GetStringFromByteArray( pByte ,pos , m_Name);
}

CQuestEncourMan::CQuestEncourMan()
{
    //PutLogInfo("CQuestEncourMan");
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
    //PutLogInfo("~CQuestEncourMan::Release()");
}

bool CQuestEncourMan::AddToByteArray(vector<uchar>* pByteArray)
{
    _AddToByteArray(pByteArray , (ulong)(m_Award.size()) );
    std::map<long,QuestAward*>::iterator it = m_Award.begin();
    for ( ; it!= m_Award.end() ; it++ )
    {
       QuestAward* point= it->second;
       _AddToByteArray( pByteArray , it->first  );
       _AddToByteArray( pByteArray , point->Sum );
       _AddToByteArray( pByteArray , point->Cell );
       CQusetEncouragement* Encour= point->Encour;
       while ( Encour != NULL )
       {
           //_AddToByteArray(pByteArray,Encour,sizeof(CQusetEncouragement));
           Encour->AddToByteArray(pByteArray);
           Encour = Encour->next;
       }
    }
    return true;
}

bool CQuestEncourMan::DecodeQuestAward(uchar* pByte, long& pos)
{
    long  sum = _GetLongFromByteArray(pByte,pos);

    for ( int i = 0 ; i < sum ; i++ )
    {
        QuestAward* Award = new QuestAward;
        long  QId = _GetLongFromByteArray(pByte,pos);
        long  QNum= _GetLongFromByteArray(pByte,pos);
        long  QCell=_GetLongFromByteArray(pByte,pos);
        for ( int j = 0 ; j < QNum ; j++  )
        {
            CQusetEncouragement * Encouragement=new CQusetEncouragement;
            //_GetBufferFromByteArray(pByte,pos,Encouragement,sizeof(CQusetEncouragement));
            Encouragement->DecodeQuestAward( pByte,pos);
            if( !Award->Encour )
                Award->Encour = Encouragement;
            else
            {
                Encouragement->next = Award->Encour;
                Award->Encour = Encouragement;
            }
        }
        Award->Cell = QCell;
        Award->Sum  = QNum;

        m_Award[ QId ] = Award ;
    }
    return true;
}

bool CQuestEncourMan::Init(const char* filename)
{
    Release();

    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return false;
    }

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_15"));
        return false;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("AwardList");

    if (pNode==NULL)
    {
        //无节点，输出错误
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return false;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
       Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return false;
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
    return true;
}

bool CQuestEncourMan::LoadAward(const char* filename)
{
    if( !filename ) return false;

    CRFile* prFile = rfOpen(filename);
    if (prFile == NULL)
    {
        return false;
    }

    TiXmlNode *pNode = NULL; 		
    TiXmlDocument T_dox(filename);
    TiXmlDocument* m_Tdoc = & T_dox; 

    if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
    {
        //输出错误信息：错误的文件名
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_15"));
        return false;
    }
    rfClose(prFile);
    pNode=m_Tdoc->FirstChild("Award");

    if (pNode==NULL)
    {
        //无节点，输出错误
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return false;
    }

    TiXmlElement* te=pNode->ToElement();//获取node 的指针
    if (te==NULL)
    {
        //输出错误信息
        Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
        return false;
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
    return true;
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

//---------------CQuestStep
CQuestStep::CQuestStep()
{
	m_lStepID=0; 
	m_lRolePropCell=0;
	m_lAimNum=0;
	m_Aim=NULL;
	next=0;
}
CQuestStep::~CQuestStep()
{
	CQuestAim* aim=0;
	while (m_Aim!=0)
	{
		aim=m_Aim;
		m_Aim=m_Aim->next;
		delete aim;
	}
}
bool CQuestStep::Init(TiXmlElement* p)
{
	if (p==NULL)
	{
		return false;
	}
	m_lStepID=atoi(p->Attribute("ID"));
	TiXmlElement* children= p->FirstChildElement();
	for (; children!=NULL; children = children->NextSiblingElement())
	{
		const char * ppp=children->Value();

		if (!strcmp(children->Value(),"Aim"))
		{
			CQuestAim*aim =new CQuestAim;
			aim->Init(children);
			if (!AddAim(aim))
			{
				//输出错误信息：超出步骤目标个数的目标配置
				Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_11"));
				delete aim;
			}
		}
		else if (!strcmp(children->Value(),"EncouragementCell"))
		{
			m_lRolePropCell=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"Discrib"))
		{
		}
		else if (!strcmp(children->Value(),"RoleTrace"))
		{

		}
		else
		{
			//错误输出：错误的步骤配置
		}
	}
	return true;
}

bool CQuestStep::AddToByteArray(vector<uchar>* pByteArray)
{
	_AddToByteArray(pByteArray,(long)m_lStepID);
	_AddToByteArray(pByteArray,(long)m_lAimNum);
	CQuestAim* Aim=m_Aim;
	while (Aim!=NULL)
	{
		_AddToByteArray(pByteArray,Aim,sizeof(CQuestAim));
		Aim=Aim->next;
	}
	return true;
}
bool CQuestStep::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_lStepID=_GetLongFromByteArray(pByte,pos);
	int aimnum=_GetLongFromByteArray(pByte,pos);
	for (int b=0; b<aimnum; b++)
	{
		CQuestAim* Aim= new CQuestAim;
		_GetBufferFromByteArray(pByte,pos,Aim,sizeof(CQuestAim));
		Aim->next=0;
		if (!AddAim(Aim))
		{
			//输出错误信息
			delete Aim;
		}
	}
	return true;
}
const CQuestAim* CQuestStep::GetAim(long ID)const
{
	const CQuestAim* aim=GetAimHead();
	while (aim!=0)
	{
		if (aim->GetID()==ID)
		{
			return aim;
		}
		aim=aim->next;
	}
	return 0;
}

bool CQuestStep::AddAim(CQuestAim*& aim)
{
	if (m_lAimNum>STEP_AIM_NUM)
	{
		return false;
	}
	else
	{
		aim->next=m_Aim;
		m_Aim=aim;
		aim=0;
		m_lAimNum++;
		return true;
	}
}
//-----------------CQuest
CQuest::CQuest()
{
	//m_ID=0;
	m_lStepSum=0;				//任务总步骤数
	m_Step=0;					//任务步骤列表
}

CQuest::~CQuest()
{
	CQuestStep* step=0;
	while (m_Step!=NULL)
	{
		step=m_Step;
		m_Step=m_Step->next;
		delete step;
	}
}

bool CQuest::Init(TiXmlElement*p, long lTextType)
{
	if (p==NULL)
	{
		return false;
	}
	m_lTextType=lTextType;
	TiXmlElement* children=p->FirstChildElement();

	for (; children!=NULL; children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"Attribute"))
		{
			m_Attribute.Init(children);
		}
		else if (!strcmp(children->Value(),"Quest_LablePrefix"))
		{
			m_LabelPrefix.Init(children);
		}
		else if (!strcmp(children->Value(),"Step"))
		{
			CQuestStep* step=new CQuestStep;
			step->Init(children);
			if (!AddStep(step))
			{
				//输出错误信息：任务步骤数多于步骤最大限度
				Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_13"));
				delete step;
			}
		}
// 		else if (!strcmp(children->Value(),"Encouragement"))
// 		{
// 			const char *pp=children->Value();
// 			CQusetEncouragement* Encouragement=new CQusetEncouragement;
// 			Encouragement->Init(children);
// 			if (!AddEncouragement(Encouragement))
// 			{
// 				//输出错误信息：任务奖励数多于奖励最大限度
// 				Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_14"));
// 				delete Encouragement;
// 			}
// 		}
// 		else if (!strcmp(children->Value(),"EncouragementCell"))
// 		{
// 			m_lEncouragementCell=atoi(children->GetText());
// 		}
	}
	return true;
}

bool CQuest::AddToByteArray(vector<uchar>* pByteArray)
{
	//_AddToByteArray(pByteArray,(long)GetID());
	_AddToByteArray(pByteArray,m_lTextType);
	_AddToByteArray(pByteArray,&m_Attribute,sizeof(m_Attribute));
	_AddToByteArray(pByteArray,&m_LabelPrefix,sizeof(m_LabelPrefix));

	_AddToByteArray(pByteArray,(long)m_lStepSum);
	CQuestStep* step=m_Step;
	while (step!=NULL)
	{
		step->AddToByteArray(pByteArray);
		step=step->next;
	}
	return true;
}
bool CQuest::DecordFromByteArray(uchar* pByte, long& pos)
{
	m_lTextType=_GetLongFromByteArray(pByte,pos);
	_GetBufferFromByteArray(pByte,pos,&m_Attribute,sizeof(m_Attribute));
	_GetBufferFromByteArray(pByte,pos,&m_LabelPrefix,sizeof(m_LabelPrefix));

	int stepsum=_GetLongFromByteArray(pByte,pos);
	for (int a=0; a<stepsum; a++)
	{
		CQuestStep* step= new CQuestStep;
		step->DecordFromByteArray(pByte,pos);
		step->next=0;
		if (!AddStep(step))
		{
			//输出错误信息
			delete step;
		}
	}
	return true;
}

bool CQuest::AddStep(CQuestStep *& step)
{
	if (m_lStepSum>=QUEST_STEP_NUM)
	{
		return false;
	}

	else
	{

		CQuestStep* point=m_Step;
		if (point==0)
		{
			step->next=m_Step;
			m_Step=step;
			step=0;
			m_lStepSum++;
			return true;
		}
		else
		{
			if (step->GetStepID()< point->GetStepID())
			{
				step->next=m_Step;
				m_Step=step;
				step=0;
				m_lStepSum++;
				return true;
			}

			while (point->next!=0)
			{
				if (step->GetStepID()<point->next->GetStepID())
				{
					step->next=point->next;
					point->next=step;
					step=0;
					m_lStepSum++;
					return true;
				}
				point=point->next;
			}

			step->next=0;
			point->next=step;
			step=0;
			m_lStepSum++;
			return true;
		}
	}
}

const CQuestStep*	CQuest::GetStep(long ID)const
{
	CQuestStep* _step=m_Step;
	if (ID==0x7fffffff)
	{
		return m_Step;
	}
	while (_step!=NULL)
	{
		if (_step->GetStepID()==ID)
		{
			return _step;
		}
		_step=_step->next;
	}
	return 0;
}
//----------------------------CQuestSystem

CQuestSystem::CQuestSystem()
{
}
CQuestSystem::~CQuestSystem()
{
}

bool CQuestSystem::Init(const char* filename)
{
	CRFile* prFile = rfOpen(filename);
	if (prFile == NULL)
	{
		return false;
	}

	TiXmlNode *pNode = NULL; 		
	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox; 

	if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
	{
		//输出错误信息：错误的文件名
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_15"));
		return false;
	}
	rfClose(prFile);
	pNode=m_Tdoc->FirstChild("Quest");

	if (pNode==NULL)
	{
		//无节点，输出错误
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		//输出错误信息
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_16"));
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();

	for (; p!=NULL; p=p->NextSiblingElement())
	{
		if (!strcmp(p->Value(),"Attribute"))
		{
			InitAttribute(p);
		}
		else if (!strcmp(p->Value(),"QuestName"))
		{
			int nID = 0;
			int nType=0;
			int iFreshTime=0;
			int nTextType = 0;
			int nTitileTextID=0;
			
			p->Attribute("ID",&nID);
			p->Attribute("Type",&nType);
			p->Attribute("FreshTime",&iFreshTime);
			p->Attribute("TextTypeID",&nTextType);
			p->Attribute("TitleTextID",&nTitileTextID);

			tagQuestList QuestList;
			QuestList.lType= nType;
			QuestList.dwFreshTime=iFreshTime;
			QuestList.lTextType = nTextType;
			QuestList.lTitleTextID = nTitileTextID;

			InitOneFile(p->GetText(),QuestList);

			m_mapQuestList[nID] = QuestList;
		}
		else if (!strcmp(p->Value(),"MerItem"))
		{
			int nId = 0;
			int goodsfresh=0;
			p->Attribute("ID",&nId);
			p->Attribute("ItemIndex",&goodsfresh);
			m_mapMerItem[nId] = goodsfresh;
		}
		else
		{
			//输出错误信息
		}
	}
    CQuestEncourMan::Init("quest/AwardList.xml");
	return  true;
}

bool CQuestSystem::InitAttribute(TiXmlElement* p)
{
	m_lMaxQuestNum=atoi(p->Attribute("MaxRoleNum"));			//玩家可接任务最大数量
	int a=atoi(p->Attribute("MaxMainRoleNum"));					

	m_vecMainQuestList.resize(a);								//初始化最大主线任务数量
	for (vector<long>::iterator it=m_vecMainQuestList.begin();it!=m_vecMainQuestList.end();it++)
	{
		*it=0;
	}
	const char* str=p->Attribute("AddQuestScript");
	if (str!=NULL)
	{	
		m_strAddQuestScript=str;
	}
	str=p->Attribute("CompleteQuestScript");
	if (str != NULL )
	{
		m_strCompleteQuestScript=str;
	}
	str=p->Attribute("QuestAccept");
	if (str != NULL )
	{
		m_strQuestAccept=str;
	}
	str=p->Attribute("QuestUpdate");
	if (str != NULL )
	{
		m_strQuestUpdate=str;
	}
	//获取俑兵任务刷新间隔
	TiXmlElement* children=p->FirstChildElement();
	for (; children!=NULL;children=children->NextSiblingElement())
	{
		if (!strcmp(children->Value(),"UpdateIntTime"))
		{
			m_lMercQuestUpdateInter=atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"UpdateScript"))
		{
			strncpy(m_pszUpdateScript,children->GetText(),MAX_PATH);
		}
		else if (!strcmp(children->Value(),"QuestLevelSpan"))
		{
			m_lQuestLvelSpan = atoi(children->GetText());
		}
		else if (!strcmp(children->Value(),"MercQuestOdds"))
		{
			children->Attribute("MaxRandomNum",(int*)&m_lMaxRandomNum);
			TiXmlElement* oddschildren=children->FirstChildElement();
			for (; oddschildren!=NULL;oddschildren=oddschildren->NextSiblingElement())
			{
				if (!strcmp(oddschildren->Value(),"Odds"))
				{
					int nvalue = 0;
					int nRandomeValue = 0;
					oddschildren->Attribute("value",&nvalue);
					oddschildren->Attribute("RandomeValue",&nRandomeValue);
					if(nvalue <= QUEST_MAX_ODDS)
					{
						m_lOddsRandomNum[nvalue-1] = nRandomeValue;
					}
				}
			}
			//初始化随机范围值
			long lStartNum = 0;
			for(int i=QUEST_MAX_ODDS-1;i>=0;i--)
			{
				int nNum = m_lOddsRandomNum[i];
				m_lOddsRandomNum[i] =lStartNum;
				lStartNum += nNum;
			}
		}
		else if (!strcmp(children->Value(),"MercQuestOddsUseGoods"))
		{
			children->Attribute("MaxRandomNum",(int*)&m_lMaxRandomNum);
			TiXmlElement* oddschildren=children->FirstChildElement();
			for (; oddschildren!=NULL;oddschildren=oddschildren->NextSiblingElement())
			{
				if (!strcmp(oddschildren->Value(),"Odds"))
				{
					int nvalue = 0;
					int nRandomeValue = 0;
					oddschildren->Attribute("value",&nvalue);
					oddschildren->Attribute("RandomeValue",&nRandomeValue);
					if(nvalue <= QUEST_MAX_ODDS)
					{
						m_lOddsRandomNumUseGoods[nvalue-1] = nRandomeValue;
					}
				}
			}
			//初始化随机范围值
			long lStartNum = 0;
			for(int i=QUEST_MAX_ODDS-1;i>=0;i--)
			{
				int nNum = m_lOddsRandomNumUseGoods[i];
				m_lOddsRandomNumUseGoods[i] =lStartNum;
				lStartNum += nNum;
			}

		}
	}
	return true;
}

bool CQuestSystem::InitOneFile(const char* filename,tagQuestList& QuestList)
{
	CRFile* prFile = rfOpen(filename);
	if (prFile == NULL)
	{
		return false;
	}


	TiXmlNode *pNode = NULL; 		

	TiXmlDocument T_dox(filename);
	TiXmlDocument* m_Tdoc = & T_dox; 

	if(!m_Tdoc->LoadData(prFile->GetData(),prFile->GetDatalen()))
	{
		//输出错误信息：错误的文件名
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_17"));
		return false;
	}
	rfClose(prFile);
	pNode=m_Tdoc->FirstChild("Quest");

	if (pNode==NULL)
	{
		//无节点，输出错误
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_18"));
		return false;
	}

	TiXmlElement* te=pNode->ToElement();//获取node 的指针
	if (te==NULL)
	{
		Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_18"));
		return false;
	}

	TiXmlElement* p=te->FirstChildElement();
	for (; p!=NULL; p=p->NextSiblingElement())
	{

		CQuest* quest=new CQuest;
		if (quest->Init(p,QuestList.lTextType))
		{
			if (!AddQuest(QuestList,quest))
			{
				Log4c::Warn(ROOT_MODULE, GetText("WS_QUEST_19"));
				delete quest;
			}
		}
		
	}

	return true; 
}

bool CQuestSystem::Relese()
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

	memset(m_lOddsRandomNum,0,sizeof(m_lOddsRandomNum));
	memset(m_lOddsRandomNumUseGoods,0,sizeof(m_lOddsRandomNumUseGoods));
	for(int i=0;i<QUEST_MAX_ODDS;i++)
	{
		//m_QuestsWithOdds[i].clear();
		m_mapQuestsWithOdds[i].clear();
	}
	
    CQuestEncourMan::Release();

	return true;
}

bool CQuestSystem::AddToByteArray(vector<uchar>* pByteArray)
{

	_AddToByteArray(pByteArray,m_lMaxQuestNum);
	_AddToByteArray(pByteArray,m_lMercQuestUpdateInter);
	_AddToByteArray(pByteArray,m_pszUpdateScript);
	_AddToByteArray(pByteArray,m_lQuestLvelSpan);
	_AddToByteArray(pByteArray,m_lMaxRandomNum);
	_AddToByteArray(pByteArray,m_lOddsRandomNum,sizeof(m_lOddsRandomNum));
	_AddToByteArray(pByteArray,m_lOddsRandomNumUseGoods,sizeof(m_lOddsRandomNumUseGoods));
	_AddToByteArray(pByteArray,m_strAddQuestScript.c_str());
	_AddToByteArray(pByteArray,m_strCompleteQuestScript.c_str());
	_AddToByteArray(pByteArray,m_strQuestAccept.c_str());
	_AddToByteArray(pByteArray,m_strQuestUpdate.c_str());
	//090409多个刷新物品　bytanglei
	_AddToByteArray(pByteArray,(long)m_mapMerItem.size());
	map<long,long>::iterator itItem = m_mapMerItem.begin();
	for (;itItem!=m_mapMerItem.end();itItem++)
	{
		_AddToByteArray(pByteArray,itItem->first);
		_AddToByteArray(pByteArray,itItem->second);
	}
	

	_AddToByteArray(pByteArray,(long)m_mapQuestList.size());
	itQuestList itList = m_mapQuestList.begin();
	for(;itList != m_mapQuestList.end();itList++)
	{
		tagQuestList& QuestList = (*itList).second;
		_AddToByteArray(pByteArray,(*itList).first);
		_AddToByteArray(pByteArray,QuestList.lTextType);
		_AddToByteArray(pByteArray,QuestList.lTitleTextID);
		_AddToByteArray(pByteArray,QuestList.lType);
		_AddToByteArray(pByteArray,QuestList.dwFreshTime);

		_AddToByteArray(pByteArray,(long)QuestList.Quests.size());
		itQuests itQS = QuestList.Quests.begin();
		for(;itQS!=QuestList.Quests.end();itQS++)
		{
			CQuest* quest=itQS->second;
			quest->AddToByteArray(pByteArray);
		}		
	}

	//添加主任务列表数据
	_AddToByteArray(pByteArray,(long)m_vecMainQuestList.size());
	vector<long>::iterator itMQ = m_vecMainQuestList.begin();
	for(;itMQ!=m_vecMainQuestList.end();itMQ++)
	{
		_AddToByteArray(pByteArray,(*itMQ));
	}

    //添加任务的所有奖励
    CQuestEncourMan::AddToByteArray(pByteArray);

	return true;
}

bool CQuestSystem::DecordFromByteArray(uchar* pByte, long& pos)
{
	Relese();
	m_lMaxQuestNum=_GetLongFromByteArray(pByte,pos);					//任务的最大数量
	m_lMercQuestUpdateInter=_GetLongFromByteArray(pByte,pos);
	_GetStringFromByteArray(pByte,pos,m_pszUpdateScript);
	m_lQuestLvelSpan=_GetLongFromByteArray(pByte,pos);
	m_lMaxRandomNum=_GetLongFromByteArray(pByte,pos);
	_GetBufferFromByteArray(pByte,pos,m_lOddsRandomNum,sizeof(m_lOddsRandomNum));
	_GetBufferFromByteArray(pByte,pos,m_lOddsRandomNumUseGoods,sizeof(m_lOddsRandomNumUseGoods));
	char block[1024];
	m_strAddQuestScript=_GetStringFromByteArray(pByte,pos,block);
	memset(block,0,1024);
	m_strCompleteQuestScript=_GetStringFromByteArray(pByte,pos,block);
	memset(block,0,1024);
	m_strQuestAccept=_GetStringFromByteArray(pByte,pos,block);
	memset(block,0,1024);
	m_strQuestUpdate=_GetStringFromByteArray(pByte,pos,block);

	int nMerItemNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0;i<nMerItemNum;i++)
	{
		long lID = _GetLongFromByteArray(pByte,pos);
		long lIndex = _GetLongFromByteArray(pByte,pos);
		m_mapMerItem[lID]=lIndex;
	}

	int nQuestListNum=_GetLongFromByteArray(pByte,pos);
	tagQuestList QuestList;
	for (int i=0;i<nQuestListNum;i++)
	{
		long lQuestListID = _GetLongFromByteArray(pByte,pos);
		QuestList.lTextType = _GetLongFromByteArray(pByte,pos);
		QuestList.lTitleTextID = _GetLongFromByteArray(pByte,pos);
		QuestList.lType= _GetLongFromByteArray(pByte,pos);
		QuestList.dwFreshTime= _GetDwordFromByteArray(pByte,pos);
		QuestList.Quests.clear();
		long nQuestNum = _GetLongFromByteArray(pByte,pos);
		for(int j=0;j<nQuestNum;j++)
		{
			CQuest* quest=new CQuest;
			quest->DecordFromByteArray(pByte,pos);
			QuestList.Quests[quest->GetID()] = quest;

			long lOdds = quest->GetAttribute().GetOdds();
			if(lOdds <= QUEST_MAX_ODDS && lOdds > 0)
			{
				m_mapQuestsWithOdds[lOdds-1][lQuestListID].push_back(quest);
			}
			//按稀有度分配保存
			//QS_TYPE_ODDS qs_typeodds(quest->GetID(),quest->GetAttribute().GetOdds());
			//m_mapQuestsWithOdds[qs_typeodds].pushback(quest);
			//long lOdds = quest->GetAttribute().GetOdds();
			//
			//	m_QuestsWithOdds[lOdds-1].push_back(quest);
		}
		m_mapQuestList[lQuestListID] = QuestList;
	}

	int nMQNum = _GetLongFromByteArray(pByte,pos);
	for(int i=0;i<nMQNum;i++)
	{
		long lID = _GetLongFromByteArray(pByte,pos);
		m_vecMainQuestList.push_back(lID);
	}
    CQuestEncourMan::DecodeQuestAward(pByte,pos);
	return true;
}

	//任务接口
const  CQuest*			CQuestSystem::GetQuest(long ID)
{
	itQuestList itList = m_mapQuestList.begin();
	for(;itList != m_mapQuestList.end();itList++)
	{
		tagQuestList& QuestList = (*itList).second;
		itQuests itQS = QuestList.Quests.find(ID);
		if(itQS != QuestList.Quests.end())
			return (*itQS).second;
	}
	return NULL;
}

long CQuestSystem::GetMainRoleByChapterID(long chapterID)
{
	if (chapterID< (long)m_vecMainQuestList.size())
	{
		return m_vecMainQuestList[chapterID];
	}
	return 0;
}

const  CQuestStep*  CQuestSystem::GetStep(long ID,long stepID)
{
	const CQuest* _quest=GetQuest(ID);
	const CQuestStep* _step=0;
	if (_quest!=NULL)
	{
		_step= (*_quest).GetStep(stepID);//->GetStep(stepID);
	}
	else 
	{
		return 0;
	}
	return _step;
}

const  CQuestAim*		CQuestSystem::GetAim(long ID,long step,long aimID)
{
	const CQuestStep* _step=GetStep(ID,step);
	if (_step!= NULL)
	{
		return _step->GetAim(aimID); 
	}
	else
	{
		return 0;
	}
	return 0;
}

bool CQuestSystem::AddQuest(tagQuestList& QuestList, CQuest* quest)
{
	itQuests it=QuestList.Quests.find(quest->GetID());
	if (it== QuestList.Quests.end())
	{
		QuestList.Quests[quest->GetID()]=quest;
		if (quest->GetAttribute().GetQuestType()==QUEST_MAIN)
		{
			AddMainQuest(quest->GetAttribute().GetChapterNum(),quest->GetAttribute().GetID());
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool CQuestSystem::AddMainQuest(long index,long roleID)
{
	if (index>=0 && index < (long)m_vecMainQuestList.size() )
	{
		m_vecMainQuestList[index]=roleID;
		return true;
	}
	return false;
}

//得到随机的稀有度
long CQuestSystem::GetRadomOdds(bool bUseGoods /* = false */)
{
	long lOdds = QUEST_MAX_ODDS-1;
	long lRadom = random(m_lMaxRandomNum);
	if (bUseGoods)
	{
		for(int i = QUEST_MAX_ODDS-2;i>=0;i--)
		{
			if(lRadom > m_lOddsRandomNumUseGoods[i])
				lOdds = i;
		}
	}
	else
	{
		for(int i = QUEST_MAX_ODDS-2;i>=0;i--)
		{
			if(lRadom > m_lOddsRandomNum[i])
				lOdds = i;
		}
	}
	return lOdds;
}
//获取俑兵任务
bool CQuestSystem::GetMercQuests(long lListID,long lPlayerLvl,long lPlayerOccu,
								long lRankOfMercenary,set<long>& BesidesQuests,
								vector<long>& RequireQuests,long lMaxRequireNum,
								bool bUseGoods)
{
	
	itQuestList it = m_mapQuestList.find(lListID);
	if( it == m_mapQuestList.end())	return false;

	//复制数据
	vector<CQuest*>	TemptQuestsWithOdds[QUEST_MAX_ODDS];
	for(int a=0 ;a<QUEST_MAX_ODDS; a++)
	{
		map<long,vector<CQuest*>>::iterator	it=m_mapQuestsWithOdds[a].find(lListID);
		if(it==m_mapQuestsWithOdds[a].end()) continue;
		TemptQuestsWithOdds[a]=it->second;
	}

	vector<CQuest*>::iterator newEnd;
	for(int i=0;i<lMaxRequireNum;i++)
	{	
		vector<CQuest*> selectedList;
		int nRadom = GetRadomOdds(bUseGoods);
		vector<CQuest*>::iterator it = TemptQuestsWithOdds[nRadom].begin();
		//查找所有满足条件的任务
		for(;it != TemptQuestsWithOdds[nRadom].end();it++)
		{
			CQuest* quest = (*it);
			if(quest)
			{	
				if( //abs(lPlayerLvl-quest->GetAttribute().GetMinGrade()) <= m_lQuestLvelSpan &&//等级跨度判断
					//quest->GetAttribute().GetOccupationRestrict(lPlayerOccu) &&//职业判断
					//lRankOfMercenary >= quest->GetAttribute().GetEngageArmsRepute() &&//俑兵声望判断
					BesidesQuests.find(quest->GetID()) == BesidesQuests.end()//是排除在外的任务
					)
				{
					selectedList.push_back(quest);
				}
			}
		}
		int nSize = (int)selectedList.size();
		if(nSize > 0)
		{//如果有选择的，从其中随机一个
			int nPos = random(nSize);
			CQuest* pQuest = selectedList[nPos];
			RequireQuests.push_back(pQuest->GetID());
			newEnd = remove(TemptQuestsWithOdds[nRadom].begin(),TemptQuestsWithOdds[nRadom].end(),pQuest);
			TemptQuestsWithOdds[nRadom].erase(newEnd,TemptQuestsWithOdds[nRadom].end());
		}
		else
		{	//如果当前稀有度没有，则从稀有度为1的任务中选取
			nSize = (int)TemptQuestsWithOdds[0].size();
			if(nSize > 0)
			{
				int nPos = random(nSize);
				CQuest* pQuest = TemptQuestsWithOdds[0][nPos];
				RequireQuests.push_back(pQuest->GetID());
				newEnd = remove(TemptQuestsWithOdds[0].begin(),TemptQuestsWithOdds[0].end(),pQuest);
				TemptQuestsWithOdds[0].erase(newEnd,TemptQuestsWithOdds[0].end());
			}
		}
	}

	return true;
}

long CQuestSystem::GetQuestListType(long lListID)
{
	itQuestList it = m_mapQuestList.find(lListID);
	if( it == m_mapQuestList.end())	return -1;
	return it->second.lType;
}

ulong CQuestSystem::GetQuestListFreshTime(long lListID)
{
	itQuestList it = m_mapQuestList.find(lListID);
	if( it == m_mapQuestList.end())	return GetMercQuestUpdateInter();
	return it->second.dwFreshTime;
}

long CQuestSystem::GetQuestListID(long lQuestID)
{
	itQuestList it = m_mapQuestList.begin();
	for (;it!=m_mapQuestList.end();it++)
	{
		map<long,CQuest*>mapquest = (*it).second.Quests;
		map<long,CQuest*>::iterator itquest=mapquest.find(lQuestID);
		if (itquest!=mapquest.end())
		{
			return it->first;
		}
	}
	return 0;
}

long  CQuestSystem::GetEncouragementCell(long lQuestID)
{
    return CQuestEncourMan::GetEncourCell( lQuestID );
}

long  CQuestSystem::GetEncouragementSum(long lQuestID)
{
     return CQuestEncourMan::GetEncourSum( lQuestID );
}

CQusetEncouragement* CQuestSystem::GetEncouragementHead(long lQuestID)
{
    return CQuestEncourMan::GetEncouragement( lQuestID );
}
