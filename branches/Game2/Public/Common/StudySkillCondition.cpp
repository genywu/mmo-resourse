#include "StdAfx.h"
#include "StudySkillCondition.h"
#include "Public.h"

#include "../AppFrame/TextRes.h"
using namespace AppFrame;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

list<CStudySkillCondition::tagCondition *> CStudySkillCondition::m_StudyConditionList;
map<string,CStudySkillCondition::tagTeachSkill*> CStudySkillCondition::m_mapTeachSkill;
list<long>										 CStudySkillCondition::m_lFactureSkillList;

CStudySkillCondition::CStudySkillCondition()
{}

CStudySkillCondition::~CStudySkillCondition()
{}

// 解析技能的附加学习条件
bool CStudySkillCondition::InitConditionEx(vector<tagConditionEx*> &vec_ConditionEx, TiXmlElement* pElem)
{
	if(pElem !=  NULL)
	{
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			tagConditionEx *stConditionEx = new tagConditionEx;
			// 道具限制
			if(!strcmp(pChildElem->Value(),"Goods"))
			{
				if(NULL!=pChildElem->Attribute("name") && NULL!=pChildElem->Attribute("num"))
				{
					stConditionEx->wConditionExType = CONDITIONEX_GOODS;
					stConditionEx->strValue = pChildElem->Attribute("name");
					stConditionEx->lValue = ((long)atoi(pChildElem->Attribute("num")));
				}
			}
			// 前置任务条件
			else if(!strcmp(pChildElem->Value(),"PreSkill"))
			{
				if(NULL!=pChildElem->Attribute("ID") && NULL!=pChildElem->Attribute("Levlimit"))
				{
					stConditionEx->wConditionExType = CONDITIONEX_PRESKILL;
					stConditionEx->strValue = pChildElem->Attribute("ID");
					stConditionEx->lValue = ((long)atoi(pChildElem->Attribute("Levlimit")));
				}
			}
			// 冲突任务限制
			else if(!strcmp(pChildElem->Value(),"ConflictSkill"))
			{
				if(NULL!=pChildElem->Attribute("ID"))
				{
					stConditionEx->wConditionExType = CONDITIONEX_CONFLICTSKILL;
					stConditionEx->strValue = pChildElem->Attribute("ID");
				}
			}
			else if (!strcmp(pChildElem->Value(),"Script"))
			{
				if (NULL!=pChildElem->Attribute("name"))
				{
					stConditionEx->wConditionExType = CONDITIONEX_SCRIPT;
					stConditionEx->strValue = pChildElem->Attribute("name");
				}
			}
			// 其它
			else
				continue;
			vec_ConditionEx.push_back(stConditionEx);
		}
		return true;
	}
	return false;
}

// 解析技能的学习条件
bool CStudySkillCondition::InitCondition(ulong dwID, TiXmlElement* pElem)
{
	if(pElem !=  NULL)
	{
		// 遍历技能节点，将每个技能节点的技能存储起来
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL;
			pChildElem=pChildElem->NextSiblingElement())
		{
			if(!strcmp(pChildElem->Value(),"Lev")
				&& NULL!=pChildElem->Attribute("value")
				&& NULL!=pChildElem->Attribute("PlayerLev")
				&& NULL!=pChildElem->Attribute("Sp")
				&& NULL!=pChildElem->Attribute("OccuLv"))
			{
				tagCondition * pCondition = new tagCondition;
				
				pCondition->dwSkillID =  dwID; 
				pCondition->wSkillLev = ((ushort)atoi(pChildElem->Attribute("value")));
				pCondition->wPlayerLev = ((ushort)atoi(pChildElem->Attribute("PlayerLev")));
				pCondition->dwSp = ((ulong)atoi(pChildElem->Attribute("Sp")));
				pCondition->dwOccuLv = ((ulong)atoi(pChildElem->Attribute("OccuLv")));
				if(NULL!=pChildElem->Attribute("StudyMode"))
					pCondition->wStudyMode = ((ushort)atoi(pChildElem->Attribute("StudyMode")));
				InitConditionEx(pCondition->vec_ConditionEx,pChildElem);
				m_StudyConditionList.push_back(pCondition);
			}
		}
	}
	return false;
}
	//初始化专业制作技能列表
bool CStudySkillCondition::InitFactureSkillList(const char *filename)
{
	if (NULL==filename)
	{
		return false;
	}
	//char strError[128];
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
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FOURTH"),filename);
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("SkillList");
	if (pNode==NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FIFTH"));
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"Skill"))
		{
			long lID = GetDwordAttribute(pChildElem,"id");
			m_lFactureSkillList.push_back(lID);		
		}
	}
	return true;
}

//解析学习技能列表
bool CStudySkillCondition::InitSkillList(const char *filename)
{
	if (NULL==filename)
	{
		return false;
	}
	//char strError[128];
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
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FOURTH"),filename);
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("StudySkillList");
	if (pNode==NULL)
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FIFTH"));
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();
	// 便利子节点，设置技能ID
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if(!strcmp(pChildElem->Value(),"StudyTeacher")
			&& NULL!=pChildElem->Attribute("name")
			&& NULL!=pChildElem->Attribute("occ"))
		{
			string strTeacherName = pChildElem->Attribute("name");
			CStudySkillCondition::tagTeachSkill *ptgTeachSkill = new CStudySkillCondition::tagTeachSkill;
			ptgTeachSkill->strOcc = pChildElem->Attribute("occ");

			list<ulong> lSkillList;
			for (TiXmlElement *pSub=pChildElem->FirstChildElement(); pSub!=NULL; 
				pSub=pSub->NextSiblingElement())
			{
				if(!strcmp(pSub->Value(),"Skill")
					&& NULL!=pSub->Attribute("id"))
				{
					ulong dwSkillID = GetDwordAttribute(pSub,"id");
					ptgTeachSkill->lSkillList.push_back(dwSkillID);
					//lSkillList.push_back(dwSkillID);
				}
				else if (!strcmp(pSub->Value(),"Occ")
					&& NULL!=pSub->Attribute("id"))
				{
					ulong dwOccu = GetDwordAttribute(pSub,"id");
					ptgTeachSkill->lOccList.push_back(dwOccu);
				}
			}
			m_mapTeachSkill[strTeacherName] = ptgTeachSkill;

		}
	}

	return true;

}
// 解析XML中关于技能的配置
bool CStudySkillCondition::Init(const char *filename)
{
	if(filename != NULL)
	{
		Release();

		//char strError[128];
		CRFile *prfile = rfOpen(filename);
		if(NULL == prfile)
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FOURTH"),filename);
			return false;
		}
		
		TiXmlNode *pNode = NULL;
		TiXmlDocument m_Tdoc;		

		if(! m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FOURTH"),filename);
			return false;
		}
		rfClose(prfile);

		//找到根节点，如果无节点，输出错误提示
		pNode = m_Tdoc.FirstChild("StudySkillCondition");
		if (pNode==NULL)
		{
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_SIXTH"));
			return false;
		}
		//类型转换
		TiXmlElement* pElem = pNode->ToElement();
		// 便利子节点，设置技能ID
		for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
			pChildElem=pChildElem->NextSiblingElement())
		{
			if(!strcmp(pChildElem->Value(),"Skill")
				&& NULL!=pChildElem->Attribute("ID"))
			{				
				ulong wSkillID = GetDwordAttribute(pChildElem,"ID");
				// 读每个技能的学习条件
 				InitCondition(wSkillID, pChildElem);		
			}
		}
		m_Tdoc.Clear();
		return true;
	}
	return false;
}

ulong CStudySkillCondition::GetDwordAttribute(TiXmlElement *pElem,string strName)
{
	double dValue;
	if(pElem->QueryDoubleAttribute(strName.c_str(),&dValue))
	{
		dValue = 0;
	}
	return (ulong)dValue;
}

// 根据要学习技能的ID以及需要提升的等级，找出这个技能的附加学习限制
CStudySkillCondition::tagCondition * CStudySkillCondition::GetCondition(ulong dwSkillID, ushort wSkillLev)
{
	tagCondition *pCondition = NULL;
	list<tagCondition *>::iterator it = m_StudyConditionList.begin();
	for(; it != m_StudyConditionList.end(); ++it)
	{
		pCondition = (*it);
		if(pCondition)
		{
			// 找到对应技能的对应等级
			if(pCondition->dwSkillID == dwSkillID && pCondition->wSkillLev == wSkillLev)
			{
				return pCondition;
			}
		}
	}
	return NULL;
}

//学习条件编码
bool  CStudySkillCondition::AddToByteArray(vector<uchar> &pByteArray)		// 添加到CByteArray
{
		//技能学习条件列表个数		
		_AddToByteArray(&pByteArray,static_cast<long>(m_StudyConditionList.size()));
		
		for (list<tagCondition *>::iterator it = m_StudyConditionList.begin();it!=m_StudyConditionList.end();++it)
		{
			//ulong dwSkillID;			// 技能ID
			_AddToByteArray(&pByteArray,(*it)->dwSkillID);			
			//ushort wSkillLev;			// 当前技能的等级
			_AddToByteArray(&pByteArray,(*it)->wSkillLev);
			//ushort wStudyMode;		// 当前等级的学习模式
			_AddToByteArray(&pByteArray,(*it)->wStudyMode);
			//ushort wPlayerLev;		// 要求玩家的等级限制
			_AddToByteArray(&pByteArray,(*it)->wPlayerLev);
			//ushort wSp;				// 学习需要消耗的SP值
			_AddToByteArray(&pByteArray,(*it)->dwSp);
			//职业等级
			_AddToByteArray(&pByteArray,(*it)->dwOccuLv);
			//附加条件个数
			_AddToByteArray(&pByteArray,static_cast<long>((*it)->vec_ConditionEx.size()));
			//
			 for (vector<tagConditionEx*>::iterator eit=(*it)->vec_ConditionEx.begin(); eit!=(*it)->vec_ConditionEx.end();++eit)
			 {
				 //附加条件的类型
				 _AddToByteArray(&pByteArray,static_cast<ulong>((*eit)->wConditionExType));
				 // 第一个值（有可能是描述物品的原始名，也有可能描述技能ID，XML解析时都用字符串形式保存）
				 _AddToByteArray(&pByteArray,(char*)((*eit)->strValue.c_str()));																	 
				 //第二个值（对于附加道具来说，描述道具数，对于附加前置技能，描述技能等级，限制技能则无此项，始终为-1）
				 _AddToByteArray(&pByteArray,(*eit)->lValue);
			 }
			//vector<tagConditionEx*>  vec_ConditionEx;	// 附加学习限制列表
		}
		//导师个数
		_AddToByteArray(&pByteArray,static_cast<long>(m_mapTeachSkill.size()));
	
		for (map<string,tagTeachSkill*>::iterator it = m_mapTeachSkill.begin();it!=m_mapTeachSkill.end();++it)
		{
			//导师名称
			_AddToByteArray(&pByteArray,(char*)it->first.c_str());
			//职业
			_AddToByteArray(&pByteArray,(char*)it->second->strOcc.c_str());
			//职业范围
			_AddToByteArray(&pByteArray,static_cast<long>(it->second->lOccList.size()));
			list<ulong>::iterator Oit=it->second->lOccList.begin();
			for(;Oit!=it->second->lOccList.end();++Oit)
			{
				_AddToByteArray(&pByteArray,(*(Oit)));
			}
			//技能个数
			_AddToByteArray(&pByteArray,static_cast<long>(it->second->lSkillList.size()));
			list<ulong>::iterator sit=it->second->lSkillList.begin();
			for(;sit!=it->second->lSkillList.end();++sit)
			{
				_AddToByteArray(&pByteArray,(*sit));
			}
		}
	//专业制作技能个数
	_AddToByteArray(&pByteArray,static_cast<long>(m_lFactureSkillList.size()));
	for (list<long>::iterator it=m_lFactureSkillList.begin();it!=m_lFactureSkillList.end();++it)
	{
		_AddToByteArray(&pByteArray,(*it));
	}
	return true;	
		
		
}
// 解码
bool  CStudySkillCondition::DecordFromByteArray(uchar* pByte, long& pos)
{
	//技能学习条件列表个数
	Release();
	char strTemp[256];
	long lSkillNum = _GetLongFromByteArray(pByte,pos);
	for (long l=0; l<lSkillNum; ++l)
	{
		tagCondition *ptgCondition = new tagCondition;
		ptgCondition->dwSkillID = _GetDwordFromByteArray(pByte,pos);
		ptgCondition->wSkillLev = _GetWordFromByteArray(pByte,pos);
		ptgCondition->wStudyMode = _GetWordFromByteArray(pByte,pos);
		ptgCondition->wPlayerLev = _GetWordFromByteArray(pByte,pos);
		ptgCondition->dwSp = _GetDwordFromByteArray(pByte,pos);
		ptgCondition->dwOccuLv = _GetDwordFromByteArray(pByte,pos);
		//附加条件个数
		long  lESkillNum =	_GetLongFromByteArray(pByte,pos);
		for (long j=0; j<lESkillNum; ++j)
		{
			tagConditionEx *ptgConditionEx = new tagConditionEx;
			ptgConditionEx->wConditionExType = (eConditionEx)_GetDwordFromByteArray(pByte,pos);// _GetDWordFromByteArray();
			_GetStringFromByteArray(pByte,pos,strTemp);
			ptgConditionEx->strValue = strTemp;
			ptgConditionEx->lValue = _GetLongFromByteArray(pByte,pos);
			ptgCondition->vec_ConditionEx.push_back(ptgConditionEx);			
		}		
		m_StudyConditionList.push_back(ptgCondition);		
	}
	long lTNum = _GetLongFromByteArray(pByte,pos);
	for(long l=0; l<lTNum; ++l)
	{
		//导师名称
		_GetStringFromByteArray(pByte,pos,strTemp);
		string strTeacherName = strTemp;//_GetDwordFromByteArray(pByte,pos);
		tagTeachSkill *ptgTeachSkill = new tagTeachSkill;
		//职业
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgTeachSkill->strOcc = strTemp;
		long lONum = _GetLongFromByteArray(pByte,pos);
		for (long j=0;j<lONum;++j)
		{
			//职业范围
			ulong dwOccu = _GetDwordFromByteArray(pByte,pos);
			ptgTeachSkill->lOccList.push_back(dwOccu);
		}
		long lSNum = _GetLongFromByteArray(pByte,pos);
		//list<ulong> lSkillList;
		for(long j=0; j<lSNum; ++j)
		{
			//技能列表
			ulong dwSkillID = _GetDwordFromByteArray(pByte,pos);
			ptgTeachSkill->lSkillList.push_back(dwSkillID);
		}
		m_mapTeachSkill[strTeacherName]=ptgTeachSkill;
	}

	//专业制作技能个数
	long lFNum = _GetLongFromByteArray(pByte,pos);
	for (long l=0; l<lFNum; ++l);
	{
		long lID = _GetLongFromByteArray(pByte,pos);
		m_lFactureSkillList.push_back(lID);
	}
   return false;
}
bool CStudySkillCondition::CheckFactureSkill(long lID)
{
	list<long>::iterator it =find(m_lFactureSkillList.begin(),m_lFactureSkillList.end(),lID);
	if (it!=m_lFactureSkillList.end())
	{
		return true;
	}
	return false;
}


// 释放资源
void CStudySkillCondition::Release()
{
	list<tagCondition *>::iterator it = m_StudyConditionList.begin();
	for(; it != m_StudyConditionList.end(); ++it)
	{
		//(*it)->vec_ConditionEx.clear();
		vector<tagConditionEx*>::iterator eit = (*it)->vec_ConditionEx.begin();
		for (;eit!=(*it)->vec_ConditionEx.end();++eit)
		{
			SAFE_DELETE(*eit);
		}
		(*it)->vec_ConditionEx.clear();
		SAFE_DELETE(*it);
	}
	m_StudyConditionList.clear();
	map<string,tagTeachSkill*>::iterator mit = m_mapTeachSkill.begin();
	for (;mit!=m_mapTeachSkill.end();++mit)
	{
		SAFE_DELETE(mit->second);
	}
	m_mapTeachSkill.clear();
	
}