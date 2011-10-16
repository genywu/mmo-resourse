#include "StdAfx.h"
#include "OccupSkillList.h"
#include "Public.h"

#include "../AppFrame/TextRes.h"
using namespace AppFrame;

#ifdef _DEBUG
#define ENABLE_ALOADER
#include "../Package/ClientResLoader.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
list<COccupSkill *> COccupSkillList::m_OccupSkillList;


COccupSkill::COccupSkill()
{
	m_strName = "";
	m_strDesc = "";
	m_lOccupId = -1;
}

COccupSkill::~COccupSkill()
{
	Release();
}

// 解析XML中关于技能的配置
bool COccupSkill::Init(TiXmlElement* pElem)
{
	if(pElem != NULL)
	{
		// 遍历技能节点，将每个技能节点的技能存储起来
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			if(!strcmp(pChildElem->Value(),"Skill")
				&& NULL!=pChildElem->Attribute("name")
				&& NULL!=pChildElem->Attribute("ID")
				&& NULL!=pChildElem->Attribute("lev"))
			{
				tagOccupSkill *stOccupSkill= new tagOccupSkill;
				stOccupSkill->strName = pChildElem->Attribute("name");
				stOccupSkill->lSkillID = (long)atoi(pChildElem->Attribute("ID"));
				stOccupSkill->lLeve = (long)atoi(pChildElem->Attribute("lev"));
				m_vecSkill.push_back(stOccupSkill);
			}
		}
		return true;
	}
	return false;
}


// 释放资源
void COccupSkill::Release()
{
	m_strName = "";
	m_strDesc = "";
	m_lOccupId = -1;
	for (vector<tagOccupSkill*>::iterator it= m_vecSkill.begin();
		it!=m_vecSkill.end();++it)
	{
		SAFE_DELETE(*it);
	}
	m_vecSkill.clear();
}


// 获得该职业下的指定技能的最大等级
long COccupSkill::GetOccupSkillMaxLev(ulong dwSkillID)
{
	for (vector<tagOccupSkill*>::iterator it= m_vecSkill.begin();
		it!=m_vecSkill.end();++it)
	{
		if((*it)->lSkillID == dwSkillID)
			return (*it)->lLeve;
	}
	return 0;
}



// 所有职业技能列表
COccupSkillList::COccupSkillList()
{}

COccupSkillList::~COccupSkillList()
{
	Release();
}

// 初始化，读XML配置文件
bool COccupSkillList::Init(const char *filename)
{
	if(!filename)	return false;
	Release();
	//=================================================================//
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
        Log4c::Error(ROOT_MODULE,"%-15s LoadFileError:%s",__FUNCTION__,filename);
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(filename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SKILL_FOURTH",filename));

		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("OccupSkillList");
	if (pNode==NULL)
	{
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_SEVENTH"));
		return false;
	}
	//类型转换
	TiXmlElement* pElem = pNode->ToElement();
	// 便利子节点，设置属性
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if(!strcmp(pChildElem->Value(),"Occup")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("ID"))
		{
			COccupSkill * pOccupSkill = new COccupSkill;
			pOccupSkill->SetName(pChildElem->Attribute("name"));
			pOccupSkill->SetOccupId((long)atoi(pChildElem->Attribute("ID")));
			// 职业描述
			if(NULL!=pChildElem->Attribute("Desc"))
			{
				pOccupSkill->SetDesc(pChildElem->Attribute("Desc"));
			}
			// 读每个职业的技能列表
			if(pOccupSkill->Init(pChildElem))
				m_OccupSkillList.push_back(pOccupSkill);
			// 读取职业技能列表失败
			else
			{
				delete pOccupSkill;
				pOccupSkill = NULL;
			}
		}
	}
	m_Tdoc.Clear();
	return true;
}

//编码
bool COccupSkillList::AddToByteArray(vector<uchar>  &pByteArray)
{
	 //技能列表个数
	 _AddToByteArray(&pByteArray,static_cast<long>(m_OccupSkillList.size()));
	 for (list<COccupSkill*>::iterator it = m_OccupSkillList.begin();it!=m_OccupSkillList.end();++it)
	 {
		//string m_strName;		// 职业名
		 //_AddToByteArray(pByteArray,(*it)->GetName());
		 // 职业描述
		 //_AddToByteArray(pByteArray,(*it)->GetDesc());
		 //职业ID(图片ID，或者对应enum eOccupation中的值)
		 _AddToByteArray(&pByteArray,(*it)->GetOccupId());
		 // 该职业对应的技能列表
		 vector<COccupSkill::tagOccupSkill*> vcSkill = (*it)->GetOccupSkill();
		 //技能个数
		 _AddToByteArray(&pByteArray,static_cast<long>(vcSkill.size()));
		 
		 vector<COccupSkill::tagOccupSkill*>::iterator cit=vcSkill.begin();
		 for (;cit!=vcSkill.end();++cit)
		 {
			 //long lSkillID;		// 技能ID
			 _AddToByteArray(&pByteArray,(*cit)->lSkillID);			 
			 //long lLeve;			// 最多能够升的级数
			 _AddToByteArray(&pByteArray,(*cit)->lLeve);
		 }
	 }
	 return true;
}

bool COccupSkillList::DecordFromByteArray(uchar* pByte, long& pos)
{
	//技能列表个数
	Release();
	long lSkillListNum = _GetLongFromByteArray(pByte,pos);
	for (long i=0; i<lSkillListNum; ++i)
	{
		COccupSkill *pOccupSkill = new COccupSkill;
		pOccupSkill->SetOccupId(_GetLongFromByteArray(pByte,pos));
		//技能个数
		long lSkillNum = _GetLongFromByteArray(pByte,pos);
		for (long j=0; j<lSkillNum; ++j)
		{
			 COccupSkill::tagOccupSkill * ptgOccupSkill= new COccupSkill::tagOccupSkill;
			 ptgOccupSkill->lSkillID = _GetLongFromByteArray(pByte,pos);
			 ptgOccupSkill->lLeve = _GetLongFromByteArray(pByte,pos);
			 pOccupSkill->SetSkill(ptgOccupSkill);
		}
		m_OccupSkillList.push_back(pOccupSkill);	 		
	}
	return true;
}
 
// 根据职业ID获得对应职业的技能列表类
COccupSkill * COccupSkillList::GetOccupSkill(long lOccupID)
{
	if(!m_OccupSkillList.empty())
	{
		list<COccupSkill *>::iterator it = m_OccupSkillList.begin();
		for(; it != m_OccupSkillList.end(); ++it)
		{
			// 找到返回
			if(lOccupID == (*it)->GetOccupId())
				return (*it);
		}
	}
	return NULL;
}
bool COccupSkillList::OccuJudgeStudy(ulong dwID,ulong dwLv,long lOCcupID)
{
	//vector<COccupSkill::tagOccupSkill> vcOccupSkill = COccupSkill::get
	//string str="Occu";
	COccupSkill* pOccupSkill = COccupSkillList::GetOccupSkill(lOCcupID);
	vector<COccupSkill::tagOccupSkill*> vcOccupSkill = pOccupSkill->GetOccupSkill();
	vector<COccupSkill::tagOccupSkill*>::iterator it=vcOccupSkill.begin();
	for (;it!=vcOccupSkill.end();++it)
	{
		if (((*it)->lSkillID==(long)dwID)&&((*it)->lLeve>=(long)dwLv))
		{
			return true;
		}
	}	
	return false;
}

// 释放资源
void COccupSkillList::Release()
{
	list<COccupSkill *>::iterator it = m_OccupSkillList.begin();
	for(; it != m_OccupSkillList.end(); ++it)
	{
		SAFE_DELETE(*it);
		//*it = NULL;
	}
	m_OccupSkillList.clear();
}

/*******************************************************************
* 转职系统配置
*******************************************************************/
vector<CChangeOccu::tagChangeOccu>	CChangeOccu::m_vecSubOccu;

CChangeOccu::CChangeOccu()
{}

CChangeOccu::~CChangeOccu()
{}

// 释放资源
void CChangeOccu::Release()
{
	m_vecSubOccu.clear();
}

// 初始化，读XML配置文件
bool CChangeOccu::Init(const char *filename)
{
	if(filename)
	{
		m_vecSubOccu.clear();
		//=================================================================//
		CRFile *prfile = rfOpen(filename);
		if(NULL == prfile)
		{
			//输出错误信息
            Log4c::Error(ROOT_MODULE,"%-15s LoadFileError:%s",__FUNCTION__,filename);
			return false;
		}
		TiXmlNode *pNode = NULL; 		
		TiXmlDocument m_Tdoc(filename);
		//载入技能配置文件
		if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
		{
			//输出错误信息
			rfClose(prfile);
			return false;
		}
		rfClose(prfile);
		//=================================================================//
		//找到根节点，如果无节点，输出错误提示
		pNode = m_Tdoc.FirstChild("ChangeOccu");
		if (pNode==NULL)
		{
			return false;
		}
		//类型转换
		TiXmlElement* pElem = pNode->ToElement();
		// 便利子节点，设置属性
		for (TiXmlElement *pOccuElem=pElem->FirstChildElement(); pOccuElem!=NULL; 
			pOccuElem=pOccuElem->NextSiblingElement())
		{
			if(!strcmp(pOccuElem->Value(),"Occu")
				&& NULL!=pOccuElem->Attribute("name")
				&& NULL!=pOccuElem->Attribute("id")
				&& NULL!=pOccuElem->Attribute("parentid")
				&& NULL!=pOccuElem->Attribute("value")
				&& NULL!=pOccuElem->Attribute("goods")
				&& NULL!=pOccuElem->Attribute("goodsNum"))
			{
				tagChangeOccu stChangeOccu;
				stChangeOccu.strName		= pOccuElem->Attribute("name");
				stChangeOccu.strGoodsName	= pOccuElem->Attribute("goods");
				stChangeOccu.nID			= atoi(pOccuElem->Attribute("id"));
				stChangeOccu.nParentID		= atoi(pOccuElem->Attribute("parentid"));
				stChangeOccu.nValue			= atoi(pOccuElem->Attribute("value"));
				stChangeOccu.lGoodsNum		= atoi(pOccuElem->Attribute("goodsNum"));
				m_vecSubOccu.push_back(stChangeOccu);
			}
		}
		return true;
	}
	return false;
}


// 根据传入的职业，获得转职条件（物品及其数量）
void CChangeOccu::GetChangeOccuCondision(uchar bOccu,string &str,int &num)
{
	if(bOccu >= 0 && bOccu <= 21 && !m_vecSubOccu.empty())
	{
		str = m_vecSubOccu[bOccu].strGoodsName;
		num = m_vecSubOccu[bOccu].lGoodsNum;
	}
}

// 根据开始和结束的职业，查找转职路径
void CChangeOccu::GetChangeOccuPath(vector<uchar> &vecOccu,uchar bStart,uchar bEnd)
{
	vecOccu.clear();
	// 本职业
	if(bStart == bEnd || m_vecSubOccu.empty())
		return;
	// 传入起始职业和目标职业为父子关系
	if(m_vecSubOccu[bStart].nParentID == bEnd || m_vecSubOccu[bEnd].nParentID == bStart)
	{
		vecOccu.push_back(bStart);
		vecOccu.push_back(bEnd);
		return;
	}
	vector<char> vecChar;
	char start[6] = "";
	sprintf_s(start,"%d",m_vecSubOccu[bStart].nValue);
	char end[6] = "";
	sprintf_s(end,"%d",m_vecSubOccu[bEnd].nValue);
	size_t bStartLen = strlen(start);
	size_t bEndLen = strlen(end);
	start[bStartLen] = '\0';
	end[bEndLen] = '\0';
	// 查找中转节点
	while (bStartLen > 0 && bEndLen > 0)
	{
		if(start[bStartLen - 1] == end[bEndLen - 1])
			vecChar.push_back(start[bStartLen - 1]);
		else
			break;
		start[bStartLen-1] = '\0';
		end[bEndLen-1] = '\0';
		bStartLen = strlen(start);
		bEndLen = strlen(end);
	}
	// 反向整理中转节点
	int i = 0;
	while(!vecChar.empty())
	{
		start[i] = vecChar.back();
		start[i+1] = '\0';
		vecChar.pop_back();
		++i;
	}
	// 中转节点ID
	int nNode = atoi(start);
	tagChangeOccu stChangeOccu;
	for (size_t i=0; i<m_vecSubOccu.size(); ++i)
	{
		if(m_vecSubOccu[i].nValue == nNode)
		{
			stChangeOccu = m_vecSubOccu[i];
			break;
		}
	}
	// 中转节点的职业ID合法
	if(stChangeOccu.nID >= 0 && stChangeOccu.nID <= 21)
	{
		uchar bSOccu = bStart;
		// 从开始职业到中转职业路径
		while (bSOccu != stChangeOccu.nID)
		{
			vecOccu.push_back(bSOccu);
			bSOccu = m_vecSubOccu[bSOccu].nParentID;
		}
		// 压入中转职业
		vecOccu.push_back(stChangeOccu.nID);
		// 从中转职业到目标职业路径
		vector<uchar> vecROccu;
		bSOccu = bEnd;
		while (bSOccu != stChangeOccu.nID)
		{
			vecROccu.push_back(bSOccu);
			bSOccu = m_vecSubOccu[bSOccu].nParentID;
		}
		// 整合职业列表
		while (!vecROccu.empty())
		{
			vecOccu.push_back(vecROccu.back());
			vecROccu.pop_back();
		}
	}
}
