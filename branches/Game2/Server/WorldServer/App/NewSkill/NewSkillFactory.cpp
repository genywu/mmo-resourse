///////////////////////////////////////////////////////////////////////////////
///  NewSkillFactory.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\Server/WorldServer\App/New Skill
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能工厂类
///
// NewSkillFactory.h: interface for the CNewSkillFactory class
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NewSkillFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<DWORD, CNewSkill*>	CNewSkillFactory::g_mapNewSkill;
DWORD					CNewSkillFactory::g_dwVersion = 0;
map<DWORD, TiXmlElement*>		CNewSkillFactory::g_mapTemplateElem;
map<DWORD, TiXmlElement*>		CNewSkillFactory::g_mapComTemplateElem;
list<long>						CNewSkillFactory::g_lInitSkillList;
map<DWORD,list<long>>			CNewSkillFactory::g_mapOccuInitSkill;
CNewSkillFactory::tagSkillCommon                  CNewSkillFactory::g_skillCommon;
/*
载入技能配置文件
*/
bool CNewSkillFactory::LoadNewSkillCache()
{
	ClearNewSkillCache();

    if( !LoadSkillCommon("skill/ComSkillConfig.xml") )
    {
        Log4c::Warn(ROOT_MODULE,FormatText("WS_SKILL_3","skill/ComSkillConfig.xml"));
    }

	// 读取技能配置文件
	list<string> listFileName;	
	LoadSkillList("skill/SkillList.xml",&listFileName);

	bool bResult = true;
	for( list<string>::iterator it = listFileName.begin(); it != listFileName.end(); it++ )
	{
		const char* pFileName = it->c_str();
        Log4c::Trace(ROOT_MODULE,FormatText("WS_SKILL_16", pFileName));

		//载入一个配置文件		
		if( FAILED( LoadConfigration( pFileName ) ) )
		{
			bResult = false;
			Log4c::Warn(ROOT_MODULE,FormatText("WS_SKILL_3",pFileName));
		}
	}
	LoadInitSkillConfig("data/InitSkill.xml");
	return bResult;
}

bool    CNewSkillFactory::LoadSkillCommon( const char* filename )
{
    CRFile *prfile = rfOpen(filename);
    if(NULL == prfile)	return false;

    TiXmlNode *pNode = NULL;
    TiXmlDocument doc(filename);

    //载入技能配置文件
    doc.LoadData(prfile->GetData(),prfile->GetDatalen());
    rfClose(prfile);

    //取得第一个节点
    pNode = doc.FirstChild("Module");	

    if (NULL == pNode)
    {
        return false;
    }
    
    TiXmlElement *pElem = pNode->ToElement();
    TiXmlElement *pChildElem=pElem->FirstChildElement();
    if( pChildElem && stricmp(pChildElem->Value(),"CommonCondition") == 0 )
    {
         TiXmlElement* children=pChildElem->FirstChildElement();
         for ( ; children!=NULL; children=children->NextSiblingElement() )
         {
             if ( !stricmp(children->Value(),"Expression") )
             {
                 CNewSkill::tagExpression Expression ;
                 DWORD dwExpressionID;
                 string strFormat;

                 dwExpressionID     = GetDwordAttribute(children,"id");
                 Expression.strFileName = GetStringAttribute(children,"filename");
   
                 g_skillCommon.m_mapFormat[ dwExpressionID ] = Expression;
             }
             else if (!stricmp(children->Value(),"Condition"))
             {
                 //条件体
                 string strType;
                 DWORD  dwID;		
                 strType = GetStringAttribute(children,"type");
                 dwID	 = GetDwordAttribute(children,"id");	
                 CNewSkill::tagCondition * Condition = LoadConditionConfigration(children,strType,dwID);

                 g_skillCommon.m_mapCondition [ dwID ] = Condition;
             }
             else if (!stricmp(children->Value(),"ConditionGroup"))
             {
                 DWORD dwGroupID = GetDwordAttribute(children,"id");
                 CNewSkill::tagConditionGroup *ptgConGroup = new CNewSkill::tagConditionGroup;
                 ptgConGroup->strMode = GetStringAttribute(children,"mode");
                 for (TiXmlElement *pGroupElem=children->FirstChildElement(); pGroupElem!=NULL; 
                     pGroupElem=pGroupElem->NextSiblingElement())
                 {
                     if (!stricmp(pGroupElem->Value(),"ConditionID"))
                     {
                         DWORD dwConID = GetDwordAttribute(pGroupElem ,"value");

                         ptgConGroup->lConditonGroup.push_back(dwConID);
                     }
                 }
                 g_skillCommon.m_mapConditionGroup[dwGroupID] = ptgConGroup ;	

             }
             else if (!stricmp(children->Value(),"OperatObjects"))
             {
                 CNewSkill::tagOperateObject *ptgOperateObject = new CNewSkill::tagOperateObject;
                 if (ptgOperateObject)
                 {
                     ptgOperateObject->dwID = GetDwordAttribute(children,"id");
                     ptgOperateObject->strType = GetStringAttribute(children,"type");
                     ptgOperateObject->strPosition = GetStringAttribute(children,"position");
                     ptgOperateObject->distance = GetFloatAttribute(children,"distance");
                     ptgOperateObject->radius = GetFloatAttribute(children,"radius");
                     ptgOperateObject->angle = GetFloatAttribute(children,"angle");
                     ptgOperateObject->width = GetFloatAttribute(children,"width");
                     ptgOperateObject->dwSkillID = GetDwordAttribute(children,"skillid");
                     ptgOperateObject->dwSkillLevel =  GetDwordAttribute(children,"skilllvl");
                     ptgOperateObject->strCenterObject = GetStringAttribute(children,"center");
                     ptgOperateObject->strObject = GetStringAttribute(children,"object");
                     ptgOperateObject->dwConGroupID = GetDwordAttribute(children,"condigroup");
                     ptgOperateObject->dwNumObject = GetDwordAttribute(children,"numobject"); 
                     ptgOperateObject->IsWithIn   = GetIntAttribute(children,"withIn");
                     ptgOperateObject->NumObjectName=GetStringAttribute(children,"numobjectname");
                     
                     g_skillCommon.m_mapOperateObject[ptgOperateObject->dwID] = ptgOperateObject ;				
                 }

             }
         }
    }


    return true;
}

HRESULT CNewSkillFactory::LoadConfigration( const string pFileName )
{
	if( pFileName.empty()) 
		return E_FAIL;

	CRFile *prfile = rfOpen(pFileName.c_str());
	if(NULL == prfile)	return E_FAIL;

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(pFileName.c_str());

	//载入技能配置文件
	doc.LoadData(prfile->GetData(),prfile->GetDatalen());
	rfClose(prfile);

	//取得第一个节点
	pNode = doc.FirstChild("Module");	

	if (NULL == pNode)
	{
		return E_FAIL;
	}

	TiXmlElement *pElem = pNode->ToElement();

	if(LoadConfigration(pElem))
	{
		return S_OK;	
	}
	else
	{
		return E_FAIL;
	}
}
//获取双字
DWORD CNewSkillFactory::GetDwordAttribute(TiXmlElement *pElem,string strName)
{
	double dValue;
	if(pElem->QueryDoubleAttribute(strName.c_str(),&dValue))
	{
		dValue = 0;
	}
	return (DWORD)dValue;
}

float CNewSkillFactory::GetFloatAttribute( TiXmlElement *elem, const std::string &attrName )
{
    double dValue;
	if(elem->QueryDoubleAttribute(attrName.c_str(),&dValue))
	{
		dValue = 0;
	}
	return (float) dValue;
}

//获取初始化职业技能
list<long>	CNewSkillFactory::GetOccuSkillList(DWORD dwOccu)
{
	list<long> lSkill;
	map<DWORD,list<long>>::iterator it = g_mapOccuInitSkill.find(dwOccu);
	if (it!=g_mapOccuInitSkill.end())
	{
		return it->second;
	}
	return lSkill;
}
//获取位型
BYTE CNewSkillFactory::GetIntAttribute(TiXmlElement *pElem,string strName)
{
	int iValue;
	if(pElem->QueryIntAttribute(strName.c_str(),&iValue))
	{
		iValue = 0;
	}
	return (BYTE)iValue;
}

//获取字符串
string CNewSkillFactory::GetStringAttribute(TiXmlElement *pElem,string strName)
{
	string newStr;
	if (NULL != pElem->Attribute(strName.c_str()))
	{
		newStr = pElem->Attribute(strName.c_str());
	} 
	else
	{
		newStr = "";
	}
	return newStr;
}
//获取文本
string CNewSkillFactory::GetTextAttribute(TiXmlElement *pElem)
{
	string strValue;
	if (NULL != pElem->GetText())
	{
		strValue = pElem->GetText();
	} 
	else
	{
		strValue = "";
	}
	return strValue;
}

//技能列表
HRESULT	CNewSkillFactory::LoadSkillList(const string pFileName, list<string>* listFileName)
{
	if( pFileName.empty()) 
		return E_FAIL;

	CRFile *prfile = rfOpen(pFileName.c_str());
	if(NULL == prfile)	return E_FAIL;

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(pFileName.c_str());

	//载入技能配置文件
	doc.LoadData(prfile->GetData(),prfile->GetDatalen());
	rfClose(prfile);

	//取得第一个节点
	pNode = doc.FirstChild("SkillList");	

	if (NULL == pNode)
	{
		return E_FAIL;
	}

	TiXmlElement *pElem = pNode->ToElement();

	LoadSkillListConfigration(pElem,listFileName);

	return S_OK;	
}

void CNewSkillFactory::LoadSkillListConfigration(TiXmlElement *pElem,list<string>* listFileName)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"Skill"))
		{
			string strSkillName = "skill\\";
			strSkillName = strSkillName + GetStringAttribute(pChildElem,"name");
			listFileName->push_back(strSkillName);			
		}
	}
}


HRESULT CNewSkillFactory::LoadInitSkillConfig( const string pFileName )
{
	if( pFileName.empty()) 
		return E_FAIL;

	CRFile *prfile = rfOpen(pFileName.c_str());
	if(NULL == prfile)	return E_FAIL;

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(pFileName.c_str());

	//载入技能配置文件
	doc.LoadData(prfile->GetData(),prfile->GetDatalen());
	rfClose(prfile);

	//取得第一个节点
	pNode = doc.FirstChild("InitSkill");	

	if (NULL == pNode)
	{
		return E_FAIL;
	}

	TiXmlElement *pElem = pNode->ToElement();

	LoadInitSkillConfigration(pElem);

	return S_OK;	
}

void CNewSkillFactory::LoadInitSkillConfigration(TiXmlElement *pElem)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"Skill"))
		{
			long lSkillID = GetDwordAttribute(pChildElem,"id");
			g_lInitSkillList.push_back(lSkillID);
		}
		else if (!stricmp(pChildElem->Value(),"Occu"))
		{
			DWORD dwOccuID = GetDwordAttribute(pChildElem,"id");
			list<long> lSkillID;
			for (TiXmlElement *pSubElem=pChildElem->FirstChildElement(); pSubElem!=NULL; 
				pSubElem=pSubElem->NextSiblingElement())
			{
				if (!stricmp(pSubElem->Value(),"Skill"))
				{
					long lInitSkill = GetDwordAttribute(pSubElem,"id");
					lSkillID.push_back(lInitSkill);
				}
			}
			g_mapOccuInitSkill[dwOccuID]=lSkillID;
		}
	}
}

/*
NAME:LoadConfigration
DESC:通过节点载入
PARA:pElem节点元素
*/
bool	CNewSkillFactory::LoadConfigration	( TiXmlElement *pElem )
{
	DWORD dwID = 0;	
	string strName;
	string strDesc;
	DWORD  dwElement;
	string strType;
    string strAttType;
	DWORD  dwUseType;
	BYTE   bOnly;
	DWORD  dwOnlyType;
	BYTE   bNameOnly;
	BYTE   bRoleOnly;
	DWORD  dwTotalNum;
    DWORD  dwCDTime;
	string strAddSub;    
	BYTE	bAttack;
	BYTE	bTarget;
	BYTE	bCoolDown;
	BYTE	bDeadUse;
	BYTE	bDeadEnd;
	BYTE	bBreak;
	BYTE	bPetType;
	BYTE	bTime;
	BYTE	bSystem;

	static CNewSkill *pNewSkill	= NULL;
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"BaseModule"))
		{
			strName = GetStringAttribute(pChildElem,"name");
			dwID = GetDwordAttribute(pChildElem, "id");
			strDesc = GetStringAttribute(pChildElem,"sriptparam");	
			dwElement = GetDwordAttribute(pChildElem,"element"); 
			strType	= GetStringAttribute(pChildElem,"type");
            strAttType=GetStringAttribute(pChildElem,"eSkillAttType");
			dwUseType = GetDwordAttribute(pChildElem,"usetype");	
			bOnly = GetIntAttribute(pChildElem,"only");
			dwOnlyType = GetDwordAttribute(pChildElem,"onlytype");
			bNameOnly = GetIntAttribute(pChildElem,"nameonly");
			bRoleOnly = GetIntAttribute(pChildElem,"roleonly");
			dwTotalNum = GetDwordAttribute(pChildElem,"totalnum");
			strAddSub = GetStringAttribute(pChildElem,"addsub");
			bAttack	= GetIntAttribute(pChildElem,"attack");
			bTarget = GetIntAttribute(pChildElem,"target");
			bCoolDown = GetIntAttribute(pChildElem,"cooldown");
			bDeadUse = GetIntAttribute(pChildElem,"deaduse");
			bDeadEnd = GetIntAttribute(pChildElem,"deadend");
			bBreak	= GetIntAttribute(pChildElem,"break");
			bPetType = GetIntAttribute(pChildElem,"pettype");
			bTime	= GetIntAttribute(pChildElem,"time");
			bSystem = GetIntAttribute(pChildElem,"system");
            dwCDTime= GetIntAttribute(pChildElem,"CDControl");
			
			map<DWORD, CNewSkill*>::iterator it = g_mapNewSkill.find(dwID);
			if (it!=g_mapNewSkill.end())
			{
				Log4c::Warn(ROOT_MODULE,FormatText("WS_SKILL_8", dwID));
				return false;
			} 
			else
			{
				pNewSkill = new CNewSkill(dwID);			//建立状态
				g_mapNewSkill[dwID] = pNewSkill;
			}

			pNewSkill->SetNewSkillName(strName);
			pNewSkill->SetDescription(strDesc);
			pNewSkill->SetElement(dwElement);
			pNewSkill->SetSkillType(strType);
            pNewSkill->SetSkillAttType(strAttType);
			pNewSkill->SetUseType((uchar)dwUseType);
			pNewSkill->SetOnly(bOnly);	
			pNewSkill->SetOnlyType(dwOnlyType);
			pNewSkill->SetNameOnly(bNameOnly);
			pNewSkill->SetRoleOnly(bRoleOnly);
			pNewSkill->SetTotalNum(dwTotalNum);
			pNewSkill->SetAddSub(strAddSub);		
			pNewSkill->SetAttack(bAttack);
			pNewSkill->SetTarget(bTarget);
			pNewSkill->SetCoolDown(bCoolDown);
			pNewSkill->SetDeadUse(bDeadUse);
			pNewSkill->SetDeadEnd(bDeadEnd);
			pNewSkill->SetBreak(bBreak);
			pNewSkill->SetPetType(bPetType);
			pNewSkill->SetTime(bTime);
			pNewSkill->SetSystem(bSystem);
            pNewSkill->SetCDControl( dwCDTime );

			LoadConfigration(pChildElem);		
		}	
		else if (!stricmp(pChildElem->Value(),"Template"))
		{		
			//LoadTemplateConfig(pChildElem);			
            LoadLevelConfigration( pChildElem,pNewSkill);
		}	
		else if(!stricmp(pChildElem->Value(),"CommonTemplate"))
		{
			LoadComTemplate(pChildElem);
		}	
		else if(!stricmp(pChildElem->Value(),"ComTemplate"))
		{
			DWORD dwTempID = GetDwordAttribute(pChildElem,"id");
			TiXmlElement *pTElem = g_mapComTemplateElem[dwTempID];
			if (pTElem)
			{
				LoadComTemplate(pTElem,pNewSkill);
			}
		}
	}	
	return true;
}

//读取公共模版
void CNewSkillFactory::LoadComTemplate(TiXmlElement *pElem,CNewSkill *pNewSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Expression"))
		{
			//公式
			LoadExpresstionConfigration(pChildElem,pNewSkill);	
		}
		else if (!stricmp(pChildElem->Value(),"Condition"))
		{
			//条件体
			string strType;
			DWORD  dwID;		
			strType = GetStringAttribute(pChildElem,"type");
			dwID	= GetDwordAttribute(pChildElem,"id");			
			pNewSkill->SetCondition(dwID,LoadConditionConfigration(pChildElem,strType,dwID));	
		}
		else if (!stricmp(pChildElem->Value(),"ConditionGroup"))
		{
			//条件组
			LoadConditionGroupfigration(pChildElem,pNewSkill);
		}		
		else if (!stricmp(pChildElem->Value(),"OperatObjects"))
		{
			LoadOperatObjects(pChildElem,pNewSkill);
		}
	}
}
//各个等级的配置
void CNewSkillFactory::LoadNewLevelConfigration(TiXmlElement *pElem,CNewSkill *pNewSkill)
{
	DWORD dwLevel;
	DWORD dwTemplateID;
	TiXmlElement *pTeplateElem = NULL;
	map<string,string>				mapVarablist;				//变量列表

	dwLevel = GetDwordAttribute(pElem,"value");
	dwTemplateID = GetDwordAttribute(pElem,"tempid");

	map<DWORD, TiXmlElement*>::iterator it = g_mapTemplateElem.find(dwTemplateID);
	if (it!=g_mapTemplateElem.end())
	{
		pTeplateElem = it->second;
	}

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if(!stricmp(pChildElem->Value(),"Variablelist"))
		{				
			string strVaName = GetStringAttribute(pChildElem,"name");
			string  strValue	= GetStringAttribute(pChildElem,"value");
			mapVarablist[strVaName] = strValue;
		}
	}
	if (pTeplateElem)
	{
		LoadLevelConfigration(pTeplateElem,pNewSkill);
	}


}
//主要结构
void CNewSkillFactory::LoadLevelConfigration(TiXmlElement *pElem, CNewSkill *pNewSkill)
{

	CNewSkill::tagSkill *ptgSkill = new CNewSkill::tagSkill;
	//ptgSkill->pStaticParam = NULL;

	ptgSkill->dwTemplateId = GetDwordAttribute(pElem,"id");

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		//技能静态参数
		if(!stricmp(pChildElem->Value(),"StaticParam"))
		{
			//LoadLevelStatiParam(pChildElem,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"ComTemplate"))
		{
			DWORD dwID = GetDwordAttribute(pChildElem,"id");
			TiXmlElement *pSubElem = g_mapComTemplateElem[dwID];
			LoadCommonConfig(pSubElem,pNewSkill,ptgSkill);
		}

		else if (!stricmp(pChildElem->Value(),"CProcedure"))
		{
			//过程阶段
			LoadProcesseExecute(pChildElem,ptgSkill,pNewSkill);
		}

	}
	pNewSkill->AddNewSkill(ptgSkill);
}

//公共配置
void	CNewSkillFactory::LoadCommonConfig( TiXmlElement *pElem, CNewSkill *pNewSkill,CNewSkill::tagSkill *ptgSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Condition"))
		{
			//条件体
			string strType;
			DWORD  dwID;		
			strType = GetStringAttribute(pChildElem,"type");
			dwID	= GetDwordAttribute(pChildElem,"id");					   		
			ptgSkill->mapCondition[dwID] = LoadConditionConfigration(pChildElem,strType,dwID,ptgSkill);
		}
	}
}

//条件组
void CNewSkillFactory::LoadConditionGroupfigration(TiXmlElement *pElem,CNewSkill *pNewSkill)
{
	DWORD dwGroupID = GetDwordAttribute(pElem,"id");
	CNewSkill::tagConditionGroup *ptgConGroup = new CNewSkill::tagConditionGroup;
	ptgConGroup->strMode = GetStringAttribute(pElem,"mode");
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"ConditionID"))
		{
			DWORD dwConID = GetDwordAttribute(pChildElem ,"value");

			ptgConGroup->lConditonGroup.push_back(dwConID);
		}
	}
	pNewSkill->SetConGroup(dwGroupID,ptgConGroup);
}



//条件
CNewSkill::tagCondition *CNewSkillFactory::LoadConditionConfigration(TiXmlElement *pElem,string strType,DWORD dwID,CNewSkill::tagSkill *ptgSkill)//,  CNewSkill::tagCondition *ptgCondition
{
	CNewSkill::tagCondition *ptgCondition = new CNewSkill::tagCondition;

	if (strType=="BaseProperty")
	{
		//基本属性条件		
		CNewSkill::tagConProperty *ptgConProperty = new CNewSkill::tagConProperty;		
		LoadPropertyCondition(pElem,ptgConProperty); 
		ptgCondition->dwLength = (DWORD)ptgConProperty->ltgProtertyConditon.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConProperty) ;
	} 
	else if(strType=="Good")
	{
		//物品条件		
		CNewSkill::tagConGood *ptgConGood = new CNewSkill::tagConGood;
		LoadGoodCondition(pElem,ptgConGood);
		ptgCondition->dwLength = (DWORD)ptgConGood->ltgGoodCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConGood);
	}
	else if (strType=="Buff")
	{
		//状态条件
		CNewSkill::tagConBuff *ptgConBuff = new CNewSkill::tagConBuff;
		LoadBuffCondition(pElem,ptgConBuff);
		ptgCondition->dwLength = (DWORD)ptgConBuff->ltgBuffCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConBuff);		
	}
    else if (strType=="Dispel")
    {
        CNewSkill::tagConDispel* ptgConDispel = new CNewSkill::tagConDispel;
        LoadDispelCondition(pElem,ptgConDispel);
        ptgCondition->dwLength = (DWORD)ptgConDispel->ltgDispelCondition.size();
        ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConDispel);
    }
    else if (strType=="Odds")
    {
        CNewSkill::tagConOdds * ptgConOdds = new CNewSkill::tagConOdds;
        LoadOddsCondition(pElem,ptgConOdds);
        ptgCondition->dwLength = (DWORD)ptgConOdds->ltgOddsCondition.size();
        ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConOdds);
    }
	else if (strType=="Skill")
	{
		//载入技能条件
		CNewSkill::tagConSkill *ptgConSkill = new CNewSkill::tagConSkill;
		LoadSkillCondition(pElem,ptgConSkill);
		ptgCondition->dwLength = (DWORD)ptgConSkill->ltgSkillCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConSkill);
	}
	else if (strType=="Distance")
	{
		//载入距离条件
		CNewSkill::tagConDistance *ptgConDistance = new CNewSkill::tagConDistance;
		LoadDistanceCondition(pElem,ptgConDistance);
		ptgCondition->dwLength = (DWORD)ptgConDistance->ltgDistanceCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConDistance);
	}
	else if (strType=="Event")
	{
		//事件
		CNewSkill::tagConEvent *ptgConEvent = new CNewSkill::tagConEvent;
		LoadEventCondition(pElem,ptgConEvent);
		ptgCondition->dwLength = (DWORD)ptgConEvent->ltgEventCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConEvent);

	} 
	else if(strType=="ValidObject")
	{
		//对象
		CNewSkill::tagConValidObject *ptgConValidObject = new CNewSkill::tagConValidObject;
		LoadValidObject(pElem,ptgConValidObject);
		ptgCondition->dwLength = (DWORD)ptgConValidObject->ltgValidObjectCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConValidObject);
	}
	else if (strType=="Script")
	{
		//脚本
		CNewSkill::tagConScript *ptgConScript =new CNewSkill::tagConScript;
		LoadScript(pElem,ptgConScript);
		ptgCondition->dwLength = (DWORD)ptgConScript->strScript.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgConScript);
	}
	else if (strType=="Timer")
	{
		CNewSkill::tagConTimer *ptgTimer = new CNewSkill::tagConTimer;
		LoadTimerCondition(pElem,ptgTimer);
		ptgCondition->dwLength = (DWORD)ptgTimer->ldwConTimer.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgTimer);
	}
	else if (strType=="CoolDown")
	{
		ptgCondition->dwLength = 0;
		ptgCondition->strType = strType;
		ptgCondition->pVoidCondition = NULL;
	}
	else if (strType=="Block")
	{
		CNewSkill::tagConBlock *ptgBlock = new CNewSkill::tagConBlock;
		LoadBlockCondition(pElem,ptgBlock);
		ptgCondition->dwLength = (DWORD)ptgBlock->ltgBlockCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgBlock);
	}
	else if (strType=="Face")
	{
		CNewSkill::tagConFace *ptgFace = new CNewSkill::tagConFace;
		LoadFaceCondition(pElem,ptgFace);
		ptgCondition->dwLength = (DWORD)ptgFace->ltgFaceCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgFace);
	}
	else if (strType=="Region")
	{
		CNewSkill::tagConRegion *ptgRegion = new CNewSkill::tagConRegion;
		LoadRegionCondition(pElem,ptgRegion);
		ptgCondition->dwLength = (DWORD)ptgRegion->ltgRegionCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgRegion);
	}
	/*else if (strType=="Occupation")
	{
		CNewSkill::tagConOccupation *ptgOccupation = new CNewSkill::tagConOccupation;
		LoadOccupationCondition(pElem,ptgOccupation,ptgSkill);
		ptgCondition->dwLength = ptgOccupation->ltgOccupationCondition.size();
		ptgCondition->pVoidCondition = reinterpret_cast<void*>(ptgOccupation);
	}*/

	if (ptgCondition)
	{
		ptgCondition->strType = strType;
		return ptgCondition;
	}
	return NULL;
}

//载入时间条件
void CNewSkillFactory::LoadTimerCondition(TiXmlElement *pElem,CNewSkill::tagConTimer *ptgTimer)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			DWORD dwFlag = GetDwordAttribute(pChildElem,"flag");
			ptgTimer->ldwConTimer.push_back(dwFlag);
		}
	}
}


//载入事件
void CNewSkillFactory::LoadEventCondition(TiXmlElement *pElem,CNewSkill::tagConEvent *ptgConEvent)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagEventCondition *ptgEventConditon = new CNewSkill::tagEventCondition;

			ptgEventConditon->strEventName  = GetStringAttribute(pChildElem,"eventtype");
            ptgEventConditon->dwSkillId     =GetStringAttribute(pChildElem,"skillid");
			ptgEventConditon->dwFlag = GetDwordAttribute(pChildElem,"flag");
			ptgEventConditon->dwVar1 = GetDwordAttribute(pChildElem,"var1");			
			ptgEventConditon->dwVar2 = GetDwordAttribute(pChildElem,"var2");
			ptgEventConditon->dwVar3 = GetDwordAttribute(pChildElem,"var3");
			ptgEventConditon->dwVar4 = GetDwordAttribute(pChildElem,"var4");
			ptgEventConditon->strName = GetStringAttribute(pChildElem,"var5");	            
			ptgConEvent->ltgEventCondition.push_back(ptgEventConditon);
		}
	}

}

//载入对象
void CNewSkillFactory::LoadValidObject(TiXmlElement *pElem,CNewSkill::tagConValidObject *ptgConValidObject)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagValidObjectCondition *ptgValidObjectCondition = new CNewSkill::tagValidObjectCondition;

			ptgValidObjectCondition->strType = GetStringAttribute(pChildElem,"type");

			for (TiXmlElement *pSubElem=pChildElem->FirstChildElement(); pSubElem!=NULL; 
				pSubElem=pSubElem->NextSiblingElement())
			{
				if (!stricmp(pSubElem->Value(),"Object"))
				{
					string strObjectType = GetStringAttribute(pSubElem,"value");
					ptgValidObjectCondition->lObjType.push_back(strObjectType);
				}
			}

			ptgConValidObject->ltgValidObjectCondition.push_back(ptgValidObjectCondition);
		}
	}
}

void CNewSkillFactory::LoadOccupationCondition(TiXmlElement *pElem,CNewSkill::tagConOccupation *ptgConOccupation)
{
	CNewSkill::tagOccupationCondition *ptgOccupationCondition = new CNewSkill::tagOccupationCondition;
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			long lOccupationID = GetDwordAttribute(pChildElem,"value");
			ptgOccupationCondition->lOccu.push_back(lOccupationID);
		}
	}
	ptgConOccupation->ltgOccupationCondition.push_back(ptgOccupationCondition);
}


//载入脚本
void CNewSkillFactory::LoadScript(TiXmlElement *pElem,CNewSkill::tagConScript *ptgConScript)
{	
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			ptgConScript->strScript = GetStringAttribute(pChildElem,"value");
			break;
		}

	}


}
//阻挡
void CNewSkillFactory::LoadBlockCondition(TiXmlElement *pElem,CNewSkill::tagConBlock *ptgConBlock)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagBlockCondition *ptgBlockCondition = new CNewSkill::tagBlockCondition;
			ptgBlockCondition->strType = GetStringAttribute(pChildElem,"type");
			ptgBlockCondition->strDir = GetStringAttribute(pChildElem,"dir");
			ptgBlockCondition->dwBlockType = GetDwordAttribute(pChildElem,"blocktype");
			ptgBlockCondition->dwShapeType = GetDwordAttribute(pChildElem,"shapetype");
			ptgConBlock->ltgBlockCondition.push_back(ptgBlockCondition);
		}
	}
}
//载入场景条件
void CNewSkillFactory::LoadRegionCondition(TiXmlElement *pElem,CNewSkill::tagConRegion *ptgRegion)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagRegionCondition *ptgRegionCondition = new CNewSkill::tagRegionCondition;
			ptgRegionCondition->strName = GetStringAttribute(pChildElem,"name");
			ptgRegionCondition->dwType = GetDwordAttribute(pChildElem,"value");
			ptgRegion->ltgRegionCondition.push_back(ptgRegionCondition);
		}
	}
}
//面向
void CNewSkillFactory::LoadFaceCondition(TiXmlElement *pElem,CNewSkill::tagConFace *ptgFace)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagFaceCondition *ptgFaceCondition = new CNewSkill::tagFaceCondition;
			ptgFaceCondition->strFaceType = GetStringAttribute(pChildElem,"value");
			ptgFace->ltgFaceCondition.push_back(ptgFaceCondition);
		}
	}
}

//载入距离条件
void CNewSkillFactory::LoadDistanceCondition(TiXmlElement *pElem,CNewSkill::tagConDistance *ptgConDistance)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagDistanceCondition *ptgDistanceCondition = new CNewSkill::tagDistanceCondition;
			ptgDistanceCondition->strCmpFun = GetStringAttribute(pChildElem,"cmpfun");
			ptgDistanceCondition->value = GetFloatAttribute(pChildElem,"value");
			ptgDistanceCondition->strRefObj = GetStringAttribute(pChildElem,"refobj"); 


			ptgDistanceCondition->strValueName = GetStringAttribute(pChildElem,"valuename");

			ptgConDistance->ltgDistanceCondition.push_back(ptgDistanceCondition);
		}
	}	
}



//技能条件
void CNewSkillFactory::LoadSkillCondition(TiXmlElement *pElem,CNewSkill::tagConSkill *ptgConSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagSkillCondition *ptgSkillCondition = new CNewSkill::tagSkillCondition;
			ptgSkillCondition->dwID = GetDwordAttribute(pChildElem,"id");
			ptgSkillCondition->dwLevel = GetDwordAttribute(pChildElem,"level");
			ptgSkillCondition->strObject = GetStringAttribute(pChildElem,"object");
			ptgConSkill->ltgSkillCondition.push_back(ptgSkillCondition);
		}
	}	
}

//状态条件
void  CNewSkillFactory::LoadBuffCondition(TiXmlElement *pElem,CNewSkill::tagConBuff *ptgConBuff)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagBuffCondition *ptgBuffCondition = new CNewSkill::tagBuffCondition;
			ptgBuffCondition->dwID = GetDwordAttribute(pChildElem,"id");
			ptgBuffCondition->dwLevel = GetDwordAttribute(pChildElem,"level");
			ptgBuffCondition->dwNum	  = GetDwordAttribute(pChildElem,"num");
			ptgBuffCondition->strObject = GetStringAttribute(pChildElem,"object");
            ptgBuffCondition->strOper   = GetStringAttribute(pChildElem,"Oper");
            ptgBuffCondition->strBufObject=GetStringAttribute(pChildElem,"BufObject");
		    ptgBuffCondition->strLevel = GetStringAttribute(pChildElem,"levelname");

// 			string strNumName = GetStringAttribute(pChildElem,"numname");
// 			if (strNumName.size()>0)
// 			{
// 				ptgBuffCondition->dwNum = atol(ptgSkill->mapVarible[strNumName].c_str());
// 			}
			
			ptgConBuff->ltgBuffCondition.push_back(ptgBuffCondition);
		}
	}	
}

void  CNewSkillFactory::LoadOddsCondition(TiXmlElement *pElem,CNewSkill::tagConOdds *ptgConOdds)
{
    for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
        pChildElem=pChildElem->NextSiblingElement())
    {
        if (!stricmp(pChildElem->Value(),"Check"))
        {
            CNewSkill::tagOddsCondition *ptgOddsCondition = new CNewSkill::tagOddsCondition;
            ptgOddsCondition->value = (short)GetDwordAttribute(pChildElem,"value");
            ptgOddsCondition->strValueName = GetStringAttribute(pChildElem,"valuename");
            ptgOddsCondition->strCmpFun	  = GetStringAttribute(pChildElem,"cmpfun");

            ptgConOdds->ltgOddsCondition.push_back(ptgOddsCondition);
        }
    }
}

void  CNewSkillFactory::LoadDispelCondition(TiXmlElement *pElem,CNewSkill::tagConDispel *ptgConDispel)
{
    for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
        pChildElem=pChildElem->NextSiblingElement())
    {
        if (!stricmp(pChildElem->Value(),"Check"))
        {
            CNewSkill::tagDispelCondition *ptgDispelCondition = new CNewSkill::tagDispelCondition;
            ptgDispelCondition->Num = (short)GetDwordAttribute(pChildElem,"value");
            ptgDispelCondition->strDispelNum = GetStringAttribute(pChildElem,"valuename");
            ptgDispelCondition->strDispelType= GetStringAttribute(pChildElem,"skilltype");

            ptgConDispel->ltgDispelCondition.push_back(ptgDispelCondition);
        }
    }
}


//物品条件
void CNewSkillFactory::LoadGoodCondition(TiXmlElement *pElem,CNewSkill::tagConGood *ptgConGood)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagGoodCondition *ptgGoodCondition = new CNewSkill::tagGoodCondition;
			ptgGoodCondition->strObject = GetStringAttribute(pChildElem,"object");
			ptgGoodCondition->dwPlace = GetDwordAttribute(pChildElem,"place");
			ptgGoodCondition->dwPos = GetDwordAttribute(pChildElem,"pos");
			ptgGoodCondition->strName = GetStringAttribute(pChildElem, "goodname");
			ptgGoodCondition->dwType = GetDwordAttribute(pChildElem,"goodtype");
			ptgGoodCondition->strCmpFun = GetStringAttribute(pChildElem,"cmpfun");
			ptgGoodCondition->dwValue = GetDwordAttribute(pChildElem,"value");
			string strValueName = GetStringAttribute(pChildElem,"valuename");

			for (TiXmlElement *pSubElem=pChildElem->FirstChildElement(); pSubElem!=NULL; 
				pSubElem=pSubElem->NextSiblingElement())
			{
				if (!stricmp(pSubElem->Value(),"CheckValue"))
				{
					CNewSkill::tagGoodAddProperty *ptgGoodAddProperty = new CNewSkill::tagGoodAddProperty;
					ptgGoodAddProperty->dwAddProType = GetDwordAttribute(pSubElem,"type");
					ptgGoodAddProperty->dwValue1 = GetDwordAttribute(pSubElem,"value1");
					ptgGoodAddProperty->dwValue2 = GetDwordAttribute(pSubElem,"value2");
					ptgGoodCondition->ltgGoodAddProperty.push_back(ptgGoodAddProperty);
				}

			}

			ptgConGood->ltgGoodCondition.push_back(ptgGoodCondition);
		}
	}

}

//基本属性条件
void CNewSkillFactory::LoadPropertyCondition(TiXmlElement *pElem,CNewSkill::tagConProperty *ptgConProperty)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Check"))
		{
			CNewSkill::tagPropertyCondition *ptgPropertyCondition =  new CNewSkill::tagPropertyCondition;

			ptgPropertyCondition->cmpareOp = GetStringAttribute(pChildElem,"cmpfun");
			ptgPropertyCondition->dwValue = GetDwordAttribute(pChildElem,"value");
			ptgPropertyCondition->strName = GetStringAttribute(pChildElem,"name");
			ptgPropertyCondition->strObject = GetStringAttribute(pChildElem,"object");

			ptgPropertyCondition->paramvalue=  GetStringAttribute(pChildElem,"valuename");

			ptgConProperty->ltgProtertyConditon.push_back(ptgPropertyCondition);
		}
	}																			   	
}


//公式
void CNewSkillFactory::LoadExpresstionConfigration(TiXmlElement *pElem, CNewSkill *pNewSkill)
{
	DWORD dwExpressionID;
	string strFormat;

	dwExpressionID = GetDwordAttribute(pElem,"id");
	string strFileName = GetStringAttribute(pElem,"filename");
	string strContend = GetTextAttribute(pElem);

    if( pNewSkill !=NULL)
	pNewSkill->SetFormat(dwExpressionID,strFileName,strContend);	
}

//载入各个等级的静态参数
void CNewSkillFactory::LoadLevelStatiParam( TiXmlElement *pElem, CNewSkill::tagSkill *pSkill)
{
}

/*
NAME:ClearNewSkillCache()
DESC:清除技能缓冲
PARA:无
*/


//过程处理阶段
void CNewSkillFactory::LoadProcesseExecute(TiXmlElement *pElem,CNewSkill::tagSkill *ptgSkill,CNewSkill *pNewSkill)
{
	CNewSkill::tagProcess *ptgProcess = new CNewSkill::tagProcess;
	if (ptgProcess)
	{
		ptgProcess->strName = GetStringAttribute(pElem,"name");
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{	
			if (!stricmp(pChildElem->Value(),"Step"))
			{
				CNewSkill::tagStep *ptgStep = new CNewSkill::tagStep;
				ptgStep->strName = GetStringAttribute(pChildElem,"name");
				LoadStep(pChildElem,ptgStep,pNewSkill,ptgSkill);
				ptgProcess->ltgStep.push_back(ptgStep);
			}
		}
        if( ptgSkill !=NULL)
		ptgSkill->ltgExecuteStep.push_back(ptgProcess);

	}
}
void CNewSkillFactory::LoadComTemplate(TiXmlElement *pElem)
{
	DWORD dwID = GetDwordAttribute(pElem,"id");
	g_mapComTemplateElem[dwID] = pElem;
}
void CNewSkillFactory::LoadTemplateConfig(TiXmlElement *pElem)
{  	
	DWORD dwID = GetDwordAttribute(pElem,"id");  				
	g_mapTemplateElem[dwID] = pElem;	
}

//载入执行步骤
void CNewSkillFactory::LoadStep(TiXmlElement *pElem,CNewSkill::tagStep *ptgStep,CNewSkill *pNewSkill,CNewSkill::tagSkill *ptgSkill)
{


	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Do"))
		{
			CNewSkill::tagDo *ptgDo = new CNewSkill::tagDo;						
			ptgDo->dwOperobjid = GetDwordAttribute(pChildElem,"operobjid");		
			ptgDo->strRet = GetStringAttribute(pChildElem,"ret");
			LoadCDoProcess(pChildElem,ptgDo,ptgSkill);	
			ptgStep->ltgDo.push_back(ptgDo);			
		}
		else if (!stricmp(pChildElem->Value(),"Template"))
		{
			CNewSkill::tagDo *ptgDo = new CNewSkill::tagDo;									
			DWORD dwTemplateID = GetDwordAttribute(pChildElem,"id");			
		}

	}

}

//
void CNewSkillFactory::LoadCDoProcess(TiXmlElement *pElem,CNewSkill::tagDo *ptgDo,CNewSkill::tagSkill *ptgSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"CondiGroup"))
		{
			CNewSkill::tagCDo *ptgCDo = new CNewSkill::tagCDo;
			ptgCDo->tgCondition.dwConditionGroup = GetDwordAttribute(pChildElem,"id");
			ptgCDo->tgCondition.strTrueRet = GetStringAttribute(pChildElem,"trueret");
			ptgCDo->tgCondition.strFalseRet = GetStringAttribute(pChildElem,"falseret");
			LoadStepProcess(pChildElem,ptgCDo,ptgSkill);
			ptgDo->ltgCDo.push_back(ptgCDo);				
		}
	}
}


//载入基本属性修改
void CNewSkillFactory::LoadPropertyChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagChangeProperty *ptgChangeProperty = new CNewSkill::tagChangeProperty;
	if (ptgChangeProperty)
	{
		ptgChangeProperty->strObj = GetStringAttribute(pElem,"object");
		ptgChangeProperty->strName = GetStringAttribute(pElem,"name");
		ptgChangeProperty->dwValue = GetDwordAttribute(pElem,"value");
		ptgChangeProperty->strOperator = GetStringAttribute(pElem,"operator");
		ptgChangeProperty->strScriptVarName = GetStringAttribute(pElem,"valuesname");
        ptgChangeProperty->paramname = GetStringAttribute(pElem,"proparamname");
		ptgChangeProperty->strChangeType = pElem->Value();

// 		string strValueName = GetStringAttribute(pElem,"valuename");
// 		if (strValueName.size()>0)
// 		{
// 			ptgChangeProperty->dwValue = atol(ptgSkill->mapVarible[strValueName].c_str());
// 		}
		ptgDoProcess->ltgBaseChange.push_back(ptgChangeProperty);

	}
}
//状态
void CNewSkillFactory::LoadBuffChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagChangeBuff *ptgChangeBuff = new CNewSkill::tagChangeBuff;
	if (ptgChangeBuff)
	{
		ptgChangeBuff->strObj = GetStringAttribute(pElem,"object");
		ptgChangeBuff->dwID = GetDwordAttribute(pElem,"bufid");
		ptgChangeBuff->dwLevel = GetDwordAttribute(pElem,"buflvl");
		ptgChangeBuff->dwCount = GetDwordAttribute(pElem,"bufcount");

		ptgChangeBuff->strSIDName = GetStringAttribute(pElem,"bufidname");
		ptgChangeBuff->strSLvName = GetStringAttribute(pElem,"buflvlname");
		ptgChangeBuff->strSCountName = GetStringAttribute(pElem,"bufcountname");
        ptgChangeBuff->strSkillType = GetStringAttribute(pElem,"clearbuftype");

// 		string strbufid = GetStringAttribute(pElem,"bufidname");
// 		if (strbufid.size()>0)
// 		{
// 			ptgChangeBuff->dwID = atol(ptgSkill->mapVarible[strbufid].c_str());
// 		}		
// 		string strbuflv = GetStringAttribute(pElem,"buflvlname");
// 		if (strbuflv.size()>0)
// 		{
// 			ptgChangeBuff->dwLevel = atol(ptgSkill->mapVarible[strbuflv].c_str());
// 		}
// 		string strbufcount = GetStringAttribute(pElem,"bufcountname");
// 		if (strbufcount.size()>0)
// 		{
// 			ptgChangeBuff->dwCount = atol(ptgSkill->mapVarible[strbufcount].c_str());
// 		}

		ptgChangeBuff->strOperator = GetStringAttribute(pElem,"operator");
		ptgChangeBuff->strChangeType = pElem->Value();		
		ptgDoProcess->ltgBaseChange.push_back(ptgChangeBuff);		
	}
}

//位置
void CNewSkillFactory::LoadPosChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagChangePos *ptgChangePos = new CNewSkill::tagChangePos;
	if (ptgChangePos)
	{
		ptgChangePos->strObj = GetStringAttribute(pElem,"object");
		ptgChangePos->strRefObject = GetStringAttribute(pElem,"referobj");
		ptgChangePos->strDir = GetStringAttribute(pElem,"dir");
		ptgChangePos->strStyle = GetStringAttribute(pElem,"style");
		ptgChangePos->strSpeed = GetStringAttribute(pElem,"speed");
		ptgChangePos->strChangeType = pElem->Value();
        ptgChangePos->dis = GetFloatAttribute( pElem, "dis" );
		ptgDoProcess->ltgBaseChange.push_back(ptgChangePos);
	}
}
//冷却时间
void CNewSkillFactory::LoadCooldownChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagCoolDownChange *ptgCoolDownChange = new CNewSkill::tagCoolDownChange;
	if (ptgCoolDownChange)
	{
		ptgCoolDownChange->strObj = GetStringAttribute(pElem,"object");
		ptgCoolDownChange->strType = GetStringAttribute(pElem,"type");
		ptgCoolDownChange->dwValue = GetDwordAttribute(pElem,"value");
		ptgCoolDownChange->strSVarianame = GetStringAttribute(pElem,"valuesname");
        ptgCoolDownChange->paramname= GetStringAttribute(pElem,"coolparamname");

// 		string strValueName = GetStringAttribute(pElem,"valuename");
// 		if (strValueName.size()>0)
// 		{
// 			ptgCoolDownChange->dwValue = atol(ptgSkill->mapVarible[strValueName].c_str());
// 		}

		ptgCoolDownChange->strChangeType = pElem->Value();
		ptgDoProcess->ltgBaseChange.push_back(ptgCoolDownChange);
	}
}
//技能操作
void	CNewSkillFactory::LoadSkillChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagSkillChange *pTemp = new CNewSkill::tagSkillChange;
	pTemp->strObj   = GetStringAttribute(pElem,"object"); 
	pTemp->strType  = GetStringAttribute(pElem,"type");
	pTemp->strOpera = GetStringAttribute(pElem,"operator");
	pTemp->lID      = GetDwordAttribute(pElem,"id");
	pTemp->lLv		= GetDwordAttribute(pElem,"lv");

	pTemp->strSIDName = GetStringAttribute(pElem,"idsname");
	pTemp->strSLvName = GetStringAttribute(pElem,"lvsname");

// 	string strid = GetStringAttribute(pElem,"idname");
// 	if (strid.size()>0)
// 	{
// 		pTemp->lID  = atol(ptgSkill->mapVarible[strid].c_str());
// 	}		
// 	string strlv = GetStringAttribute(pElem,"lvname");
// 	if (strlv.size()>0)
// 	{
// 		pTemp->lID  = atol(ptgSkill->mapVarible[strlv].c_str());
// 	}
	pTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(pTemp);
}

void CNewSkillFactory::LoadGeniusChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
    CNewSkill::tagGeniusChange *pTemp = new CNewSkill::tagGeniusChange;
    pTemp->strGenius = GetStringAttribute(pElem,"geniusid");
    pTemp->strChangeType = pElem->Value();
    ptgDoProcess->ltgBaseChange.push_back(pTemp);
}

//对象列表
void CNewSkillFactory::LoadDestObjChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagDestObjChange *pTemp = new CNewSkill::tagDestObjChange;
	pTemp->strDestObjChange = GetStringAttribute(pElem,"type");
	pTemp->strObj = GetStringAttribute(pElem,"object");
	pTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(pTemp);
}

//注消时间
void CNewSkillFactory::LoadUnRegisterTimer(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagUnRegisterTimer *pTemp = new CNewSkill::tagUnRegisterTimer;
	pTemp->dwUnRegisterTimer = GetDwordAttribute(pElem,"flag");
	pTemp->strObj = GetStringAttribute(pElem,"object");
	pTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(pTemp);

}

//注消事件
void CNewSkillFactory::LoadUnRegisterEvent(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagUnRegisterEvent *pTemp = new CNewSkill::tagUnRegisterEvent;
	pTemp->strObj = GetStringAttribute(pElem,"object");
	pTemp->dwUnRegisterEvent = GetDwordAttribute(pElem,"flag");
	pTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(pTemp);
}
//方向
void CNewSkillFactory::LoadDirection(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagDirection *pTemp = new CNewSkill::tagDirection;
	pTemp->strObj = GetStringAttribute(pElem,"object");
	pTemp->strDir = GetStringAttribute(pElem,"direction");
	pTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(pTemp);
}

//注册时间
void CNewSkillFactory::LoadRegisterTimer(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagRegisterTimer *ptgRegisterTime = new CNewSkill::tagRegisterTimer;
	ptgRegisterTime->strObj = GetStringAttribute(pElem,"object");
	ptgRegisterTime->dwFlag = GetDwordAttribute(pElem,"flag");
	ptgRegisterTime->dwCount = GetDwordAttribute(pElem,"count");
	ptgRegisterTime->strSCountName = GetStringAttribute(pElem,"countvarsname");
	ptgRegisterTime->dwIntelTime = GetDwordAttribute(pElem,"intel");
	ptgRegisterTime->strSIntelName = GetStringAttribute(pElem,"intervarsname");
	ptgRegisterTime->dwFuture = GetDwordAttribute(pElem,"future");
	ptgRegisterTime->strSFutvarname = GetStringAttribute(pElem,"futvarsname");
    ptgRegisterTime->strVFuturename = GetStringAttribute(pElem,"FutureParamname");
    ptgRegisterTime->strVIntername = GetStringAttribute(pElem,"InterParamname");
    ptgRegisterTime->strVCountname = GetStringAttribute(pElem,"CountParamname");

// 	string strFu = GetStringAttribute(pElem,"futvarname");
// 	if (strFu.size()>0)
// 	{
// 		ptgRegisterTime->dwFuture = atol(ptgSkill->mapVarible[strFu].c_str());
// 	}
// 	string strIntel = GetStringAttribute(pElem,"intervarname");
// 	if (strIntel.size()>0)
// 	{
// 		ptgRegisterTime->dwIntelTime = atol(ptgSkill->mapVarible[strIntel].c_str());
// 	}					
// 	string strCount = GetStringAttribute(pElem,"countvarname");
// 	if (strCount.size()>0)
// 	{
// 		ptgRegisterTime->dwCount = atol(ptgSkill->mapVarible[strCount].c_str());
// 	}

	ptgRegisterTime->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(ptgRegisterTime);
}

//注册事件
void CNewSkillFactory::LoadRegisterEvent(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagRegisterEvent *ptgRegisterEvent = new CNewSkill::tagRegisterEvent;
	ptgRegisterEvent->strObj = GetStringAttribute(pElem,"object"); 
	ptgRegisterEvent->dwFlag = GetDwordAttribute(pElem,"flag");
	ptgRegisterEvent->strName = GetStringAttribute(pElem,"name");
	ptgRegisterEvent->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(ptgRegisterEvent);	
}
//发送消息
void CNewSkillFactory::LoadSendMsg(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagSendMsg *ptgSendMsg = new CNewSkill::tagSendMsg;
	ptgSendMsg->strObj = GetStringAttribute(pElem,"object"); 
	ptgSendMsg->strType = GetStringAttribute(pElem,"type");
	ptgSendMsg->strRange = GetStringAttribute(pElem,"range");
	ptgSendMsg->dwValue = GetDwordAttribute(pElem,"value");
	ptgSendMsg->strSVName = GetStringAttribute(pElem,"valuesname");
	ptgSendMsg->nAdd	= GetIntAttribute(pElem,"update");

// 	string strValue = GetStringAttribute(pElem,"valuename");
// 	if (strValue.size()>0)
// 	{
// 		ptgSendMsg->dwValue = atol(ptgSkill->mapVarible[strValue].c_str()); 
// 	}
	ptgSendMsg->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(ptgSendMsg);	
}
//伤害计算
void CNewSkillFactory::LoadDamageChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagDamageChange *ptDamageChange = new CNewSkill::tagDamageChange;
	ptDamageChange->strOperator = GetStringAttribute(pElem,"operator");
	ptDamageChange->strType = GetStringAttribute(pElem,"type");
	ptDamageChange->strSVarianame = GetStringAttribute(pElem,"valuesname");
	ptDamageChange->dwValue = GetDwordAttribute(pElem,"value");
	ptDamageChange->strObj = GetStringAttribute(pElem,"object");
	ptDamageChange->strChangeType = pElem->Value();
    ptDamageChange->strDamagename = GetStringAttribute(pElem,"damparamname");

// 	string strValue = GetStringAttribute(pElem,"valuename");
// 	if (strValue.size()>0)
// 	{
// 		ptDamageChange->dwValue = atol(ptgSkill->mapVarible[strValue].c_str());
// 	}

	ptgDoProcess->ltgBaseChange.push_back(ptDamageChange);
}
//DO伤害
void CNewSkillFactory::LoadDoDamage(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagDoDamage *ptgDoDamage = new CNewSkill::tagDoDamage;
	ptgDoDamage->strObj = GetStringAttribute(pElem,"object");
	ptgDoDamage->dwBlastAttack = GetDwordAttribute(pElem,"blastvalue");
	ptgDoDamage->dwResult = GetDwordAttribute(pElem,"damageresvalue");	
	ptgDoDamage->strSResVariName = GetStringAttribute(pElem,"damageresvaluesname");
	ptgDoDamage->strSBlastVariName = GetStringAttribute(pElem,"blastvaluesname");
    ptgDoDamage->strDamagename = GetStringAttribute(pElem,"damageparamname");
    ptgDoDamage->strBlastname = GetStringAttribute(pElem,"blastparamname");

	ptgDoDamage->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(ptgDoDamage);	
}
//物品
void CNewSkillFactory::LoadGoodChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	CNewSkill::tagGoodChange *ptgGoodChange = new CNewSkill::tagGoodChange;
	ptgGoodChange->strObj = GetStringAttribute(pElem,"object");
	ptgGoodChange->strOperator = GetStringAttribute(pElem,"operator");
	ptgGoodChange->strName = GetStringAttribute(pElem,"name");
	ptgGoodChange->dwPlace = GetDwordAttribute(pElem,"place");
	ptgGoodChange->dwPos  = GetDwordAttribute(pElem,"pos");
	ptgGoodChange->dwValue = GetDwordAttribute(pElem,"value");
	ptgGoodChange->dwGoodsType = GetDwordAttribute(pElem,"goodtype");
	ptgGoodChange->dwWeaponType = GetDwordAttribute(pElem,"weapontype");
	ptgDoProcess->ltgBaseChange.push_back(ptgGoodChange);
	ptgGoodChange->strChangeType = pElem->Value();
}




//攻击对象
void CNewSkillFactory::LodAttackObject(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	CNewSkill::tagAttackChangeObject *ptgTemp = new CNewSkill::tagAttackChangeObject;
	ptgTemp->strObj = GetStringAttribute(pElem,"object");
	ptgTemp->strAttackChangeObject = GetStringAttribute(pElem,"selobj");	
	ptgTemp->strChangeType = pElem->Value();
	ptgDoProcess->ltgBaseChange.push_back(ptgTemp);

}

//执行操作
void CNewSkillFactory::LoadDoProcess(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess,CNewSkill::tagSkill *ptgSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"ObjectPropertyChange"))
		{
			//对象基本属性
			LoadPropertyChange(pChildElem,ptgDoProcess,ptgSkill);

		}
		else if (!stricmp(pChildElem->Value(),"BuffChange"))
		{
			//状态
			LoadBuffChange(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"PosChange"))
		{
			//位置
			LoadPosChange(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"AttakObjectChange"))
		{
			//改变攻击对象
			LodAttackObject(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"Script"))
		{			
			//脚本
			LoadScriptChange(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"CooldownChange"))
		{
			//冷却时间
			LoadCooldownChange(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"DestObjsChange"))
		{
			//对象列表
			LoadDestObjChange(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"RegisterTimer"))
		{
			//注册时间
			LoadRegisterTimer(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"RegisterEvent"))
		{
			//注册事件
			LoadRegisterEvent(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"UnRegisterTimer"))
		{
			//注消时间
			LoadUnRegisterTimer(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"UnRegisterEvent"))
		{
			//注消事件
			LoadUnRegisterEvent(pChildElem,ptgDoProcess);
		}

		else if (!stricmp(pChildElem->Value(),"SendMsg"))
		{
			//发送消息
			LoadSendMsg(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"DamageChange"))
		{
			//伤害计算
			LoadDamageChange(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"DoDamage"))
		{
			LoadDoDamage(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"GoodChange"))
		{
			LoadGoodChange(pChildElem,ptgDoProcess,ptgSkill);
		}
		else if (!stricmp(pChildElem->Value(),"DirecChange"))
		{
			LoadDirection(pChildElem,ptgDoProcess);
		}
		else if (!stricmp(pChildElem->Value(),"SkillChange"))
		{
			LoadSkillChange(pChildElem,ptgDoProcess,ptgSkill);
		}
        else if (!stricmp(pChildElem->Value(),"GeniusChange"))
        {
            LoadGeniusChange(pChildElem,ptgDoProcess);
        }
		else if (!stricmp(pChildElem->Value(),"CondiGroup"))
		{
			CNewSkill::tagCDo *ptgCDo = new CNewSkill::tagCDo;
			ptgCDo->tgCondition.dwConditionGroup = GetDwordAttribute(pChildElem,"id");
			ptgCDo->tgCondition.strTrueRet = GetStringAttribute(pChildElem,"trueret");
			ptgCDo->tgCondition.strFalseRet = GetStringAttribute(pChildElem,"falseret");
			LoadStepProcess(pChildElem,ptgCDo,ptgSkill);
			ptgDoProcess->ltgCDo.push_back(ptgCDo);			
		}
	}

}



//脚本
void CNewSkillFactory::LoadScriptChange(TiXmlElement *pElem,CNewSkill::tagDoProcess *ptgDoProcess)
{
	string strType = pElem->Value();
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"ExpressionID"))
		{

			CNewSkill::tagScriptID *ptgScripID = new CNewSkill::tagScriptID;
			ptgScripID->dwScriptID = GetDwordAttribute(pChildElem, "value");
			ptgScripID->strObj = GetStringAttribute(pChildElem,"object");
			ptgScripID->strChangeType = strType;
			ptgDoProcess->ltgBaseChange.push_back(ptgScripID);
		}
	}

}


//载入处理步骤
void CNewSkillFactory::LoadStepProcess(TiXmlElement *pElem,CNewSkill::tagCDo *ptgCDo,CNewSkill::tagSkill *ptgSkill)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"True"))
		{
			CNewSkill::tagDoProcess *ptgDoProcess = new CNewSkill::tagDoProcess;
			LoadDoProcess(pChildElem,ptgDoProcess,ptgSkill);			
			ptgCDo->ltgTrueDoProcess.push_back(ptgDoProcess);
		} 
		else if (!stricmp(pChildElem->Value(),"False")) 
		{
			CNewSkill::tagDoProcess *ptgDoProcess = new CNewSkill::tagDoProcess;
			LoadDoProcess(pChildElem,ptgDoProcess,ptgSkill);
			ptgCDo->ltgFalseDoProcess.push_back(ptgDoProcess);
		}


	}
}


//操作对象
void CNewSkillFactory::LoadOperatObjects(TiXmlElement *pElem,CNewSkill *pNewSkill)
{
	CNewSkill::tagOperateObject *ptgOperateObject = new CNewSkill::tagOperateObject;
	if (ptgOperateObject)
	{
		ptgOperateObject->dwID = GetDwordAttribute(pElem,"id");
		ptgOperateObject->strType = GetStringAttribute(pElem,"type");
		ptgOperateObject->strPosition = GetStringAttribute(pElem,"position");
		ptgOperateObject->distance = GetFloatAttribute(pElem,"distance");
		ptgOperateObject->radius = GetFloatAttribute(pElem,"radius");
        ptgOperateObject->angle = GetFloatAttribute(pElem,"angle");
        ptgOperateObject->width = GetFloatAttribute(pElem,"width");
		ptgOperateObject->dwSkillID = GetDwordAttribute(pElem,"skillid");
		ptgOperateObject->dwSkillLevel =  GetDwordAttribute(pElem,"skilllvl");
		ptgOperateObject->strCenterObject = GetStringAttribute(pElem,"center");
		ptgOperateObject->strObject = GetStringAttribute(pElem,"object");
		ptgOperateObject->dwConGroupID = GetDwordAttribute(pElem,"condigroup");
		ptgOperateObject->dwNumObject = GetDwordAttribute(pElem,"numobject"); 
        ptgOperateObject->IsWithIn   = GetIntAttribute(pElem,"withIn");
        ptgOperateObject->NumObjectName=GetStringAttribute(pElem,"numobjectname");
		pNewSkill->SetOperateObject(ptgOperateObject->dwID,ptgOperateObject);				
	}		
}

/*
NAME:ClearNewSkillCache()
DESC:清除技能缓冲
PARA:无
*/
void CNewSkillFactory::ClearNewSkillCache()
{	
    g_skillCommon.m_mapFormat.clear();

    map<DWORD,CNewSkill::tagConditionGroup*>::iterator  GroupItr = g_skillCommon.m_mapConditionGroup.begin();
    for ( ; GroupItr != g_skillCommon.m_mapConditionGroup.end() ; GroupItr++ )
    {
          SAFE_DELETE( GroupItr->second );
    }
    g_skillCommon.m_mapConditionGroup.clear();

    map<DWORD,CNewSkill::tagCondition*>::iterator  CondItr = g_skillCommon.m_mapCondition.begin();
    for ( ; CondItr != g_skillCommon.m_mapCondition.end() ; CondItr++ )
    {
        SAFE_DELETE( CondItr->second );
    }
    g_skillCommon.m_mapCondition.clear();

    map<DWORD,CNewSkill::tagOperateObject*>::iterator  ObjectItr = g_skillCommon.m_mapOperateObject.begin();
    for ( ; ObjectItr != g_skillCommon.m_mapOperateObject.end() ; ObjectItr++ )
    {
        SAFE_DELETE( ObjectItr->second );
    }
    g_skillCommon.m_mapOperateObject.clear();


	for( map<DWORD, CNewSkill*>::iterator it = g_mapNewSkill.begin(); 
		it != g_mapNewSkill.end(); it ++ )
	{
		SAFE_DELETE( it->second );
	}
	g_mapNewSkill.clear();
}


/*
NAME:CodeNewSkill
DESC:状态编码
PARA:vRet缓冲
*/
bool CNewSkillFactory::CodeNewSkill(vector<BYTE>& vRet )
{
	_AddToByteArray( &vRet, static_cast<DWORD>( g_mapNewSkill.size() ) );
	//版本号
	_AddToByteArray(&vRet,++g_dwVersion);
	//职业初始化技能列表
	_AddToByteArray(&vRet,(long)g_mapOccuInitSkill.size());
	for (map<DWORD,list<long>>::iterator it = g_mapOccuInitSkill.begin(); 
		 it != g_mapOccuInitSkill.end(); ++it)
	{
		_AddToByteArray(&vRet,(DWORD)it->first);
		_AddToByteArray(&vRet,(long)(it->second.size()));
		for (list<long>::iterator sit = it->second.begin(); sit!=it->second.end(); ++sit)
		{
			_AddToByteArray(&vRet,(long)(*sit));
		}
	}
    
    //CodeSkillCommon( vRet ) ;

	size_t	lLength	= 0;
	BYTE*	pBuffer	= NULL;
	for( map<DWORD, CNewSkill*>::iterator it = g_mapNewSkill.begin(); 
		it != g_mapNewSkill.end(); it ++ )
	{
		if( it->second )
		{			
			it ->second ->AddNewSkillToByteArray(vRet);		
		}
		else 
		{
			_AddToByteArray( &vRet, static_cast<DWORD>( 0 ) );
		}
	}
	return true;
}

bool  CNewSkillFactory::CodeSkillCommon(vector<BYTE>& vRet)
{

    /// 全局公式个数
    _AddToByteArray(&vRet,(long)g_skillCommon.m_mapFormat.size());
    for (map<DWORD,CNewSkill::tagExpression>::iterator it=g_skillCommon.m_mapFormat.begin();
        it!=g_skillCommon.m_mapFormat.end();++it)
    {
        //公式ID
        _AddToByteArray(&vRet,it->first);
        //公式
        _AddToByteArray(&vRet,(char*)((*it).second.strFileName.c_str() ));
        _AddToByteArray(&vRet,(char*)((*it).second.strContent.c_str() ));
    }

    //条件组
    //个数
    _AddToByteArray(&vRet,(long)g_skillCommon.m_mapConditionGroup.size());
    for (map<DWORD,CNewSkill::tagConditionGroup*>::iterator it=g_skillCommon.m_mapConditionGroup.begin();
        it!=g_skillCommon.m_mapConditionGroup.end();++it)
    {
        //ID
        _AddToByteArray(&vRet,it->first);
        //组合模式
        _AddToByteArray(&vRet,(char*)((it->second)->strMode.c_str()));
        //子条件个数
        _AddToByteArray(&vRet,(long)(it->second)->lConditonGroup.size());		

        for (list<DWORD>::iterator cit=(it->second)->lConditonGroup.begin();
            cit!=(it->second)->lConditonGroup.end();++cit)
        {				
            _AddToByteArray(&vRet,*cit);
        }
    }


    //条件体
    _AddToByteArray(&vRet,(long)g_skillCommon.m_mapCondition.size());

    for (map<DWORD,CNewSkill::tagCondition*>::iterator it=g_skillCommon.m_mapCondition.begin();
        it!=g_skillCommon.m_mapCondition.end();++it)
    {
        //ID
        _AddToByteArray(&vRet,it->first);			
        CNewSkill::tagCondition *ptgCondition = it->second;


        _AddToByteArray(&vRet,(char*)ptgCondition->strType.c_str());
        CNewSkill::AddConditionLength(vRet,ptgCondition);
    }


    //作用对象
    //个数
    _AddToByteArray(&vRet,(long)g_skillCommon.m_mapOperateObject.size());

    for (map<DWORD,CNewSkill::tagOperateObject*>::iterator Oit=g_skillCommon.m_mapOperateObject.begin();
        Oit!=g_skillCommon.m_mapOperateObject.end();++Oit)
    {			
        _AddToByteArray(&vRet,(Oit->second)->dwID);
        _AddToByteArray(&vRet,(Oit->second)->distance);
        _AddToByteArray(&vRet,(Oit->second)->radius);
        _AddToByteArray(&vRet,(Oit->second)->angle);
        _AddToByteArray(&vRet,(Oit->second)->width);
        _AddToByteArray(&vRet,(Oit->second)->dwSkillID);
        _AddToByteArray(&vRet,(Oit->second)->dwSkillLevel);
        _AddToByteArray(&vRet,(Oit->second)->dwConGroupID);		
        _AddToByteArray(&vRet,(Oit->second)->dwNumObject);

        _AddToByteArray(&vRet,(char*)(Oit->second)->strType.c_str());
        _AddToByteArray(&vRet,(char*)(Oit->second)->strPosition.c_str());
        _AddToByteArray(&vRet,(char*)(Oit->second)->strCenterObject.c_str());
        _AddToByteArray(&vRet,(char*)(Oit->second)->strObject.c_str());
        _AddToByteArray(&vRet,(char*)(Oit->second)->NumObjectName.c_str());
        _AddToByteArray(&vRet,(Oit->second)->IsWithIn);
    }

    return true;
}