///////////////////////////////////////////////////////////////////////////////
///  NewSkillList.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\setup
///
///  CREATED: 08/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   技能类
//////////////////////////////////////////////////////////////////////--

#include "StdAfx.h"
#include "NewSkillList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CNewSkillProperties::tagChangeProperty::~tagChangeProperty()
//{
//	strName.clear();
//	strOperator.clear();
//	strScriptVarName.clear();
//}

/*
NAME:CState()
DESC:默认构造函数
PARA:无
*/

CNewSkillProperties::CNewSkillProperties()
{
	m_dwID = 0;	
}


/*
NAME:CState(long lStateID)
DESC:构造函数
PARA:lStateID为状态的ID
*/

CNewSkillProperties::CNewSkillProperties(DWORD dwID)
{
	m_dwID = dwID;
}

/*
NAME:~CNewSkill()
DESC:析造函数
PARA:无
*/

CNewSkillProperties::~CNewSkillProperties()
{
	//Clear();
}

//CNewSkillProperties::tagChangeProperty::~tagChangeProperty()
//{
//	delete strName.c_str();
//}
/*
NAME:AddNewSkill()
DESC:添加技能的主要结构
PARA:pSkill技能的主要结构
*/
//
void CNewSkillProperties::AddNewSkill( tagSkill *pSkill)
{
	m_lSkill.push_back(pSkill);
}


//脚本
void	CNewSkillProperties::SetFormat( DWORD dwID, string& strFileName,string& strContend)	
{
	tagExpression Express = {strFileName,strContend};
	m_mapFormat[dwID] = Express;
} 
/*
NAME:SetCondition()
DESC:添加条件
PARA:dwID条件ID
	 tgConditon条件体
*/
void	CNewSkillProperties::SetCondition( DWORD dwID,tagCondition *tgCondtition)
{
	map<DWORD, tagCondition*>::iterator it = m_mapCondition.find(dwID);
	if (it!=m_mapCondition.end())
	{
		Log4c::Trace(ROOT_MODULE,FormatText("WS_SKILL_2", this->GetNewSkillID(),dwID));

		//清除条件，避免内存泄露
		ClearCondition(tgCondtition->pVoidCondition, tgCondtition->strType);
		SAFE_DELETE(tgCondtition);
		return;
	}
	else
	{
		m_mapCondition[dwID] = tgCondtition;
	}
}

/*
NAME:SetConGroup()
DESC:添加条件组
PARA:dwGroupID条件组ID
	 ptgConditionGroup条件组
*/
void CNewSkillProperties::SetConGroup(DWORD dwGroupID,tagConditionGroup *ptgConditionGroup)
{
	map<DWORD,tagConditionGroup*>::iterator it = m_mapConditionGroup.find(dwGroupID);
	if (it!=m_mapConditionGroup.end())
	{
		Log4c::Trace(ROOT_MODULE,FormatText("WS_SKILL_1", this->GetNewSkillID(),dwGroupID));
		return;
		
	} 
	else
	{
		m_mapConditionGroup[dwGroupID]=ptgConditionGroup;
	}
 
}

/*
NAME:GetNewSkill()
DESC:通过等级获取相应技能的主要结构
PARA:dwLevel技能等级
*/
/*
CNewSkillProperties::tagSkill *CNewSkillProperties::GetNewSkill(DWORD dwLevel)
{
	if (m_lSkill.size()==0)
	{
		return NULL;
	}
// 	list<tagSkill*>::iterator it=m_lSkill.begin();
// 	for (; it!=m_lSkill.end(); ++it)
// 	{
// 		//if (dwLevel == (*it)->dwLevel)
// 		{
// 			tagSkill *ptgSkill = *it;
// 			return ptgSkill;
// 		}
// 	}
    /*
    long  TmpId = GetInst(SkillAttribute).GetStrCondif(m_dwID,dwLevel,"TemplateId");
    list<tagSkill*>::iterator it=m_lSkill.begin();
    for (; it!=m_lSkill.end(); ++it)
    {
        if (TmpId == (*it)->dwTemplateId)
        {
            tagSkill *ptgSkill = *it;
            return ptgSkill;
        }
    }
    
	return NULL;
}
*/

/*
CNewSkillProperties::stStaticParam	*CNewSkillProperties::GetStaticParam(DWORD dwLevel)	
{
	CNewSkillProperties::tagSkill *ptgSkill = GetNewSkill(dwLevel);
	if (ptgSkill)
	{
		return ptgSkill->pStaticParam;
	}
	return NULL;
}

*/

void CNewSkillProperties::ClearCondition(void *pCondition,string strType)
{
	if(strType=="BaseProperty")
	{
		tagConProperty *p = reinterpret_cast<tagConProperty*>(pCondition);
		for (list<tagPropertyCondition*>::iterator it=p->ltgProtertyConditon.begin();
			it!=p->ltgProtertyConditon.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgProtertyConditon.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Good")
	{
		tagConGood *p = reinterpret_cast<tagConGood*>(pCondition);

		for (list<tagGoodCondition*>::iterator it=p->ltgGoodCondition.begin();
			it!=p->ltgGoodCondition.end();++it)
		{
			for (list<tagGoodAddProperty*>::iterator git=(*it)->ltgGoodAddProperty.begin();
				git!=(*it)->ltgGoodAddProperty.end();++git)
			{
				SAFE_DELETE(*git);
			}
			(*it)->ltgGoodAddProperty.clear();
			SAFE_DELETE(*it);
		}

		p->ltgGoodCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Buff")
	{
		tagConBuff *p = reinterpret_cast<tagConBuff*>(pCondition);
		for (list<tagBuffCondition*>::iterator it=p->ltgBuffCondition.begin();
			it!=p->ltgBuffCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgBuffCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Skill")
	{
		tagConSkill *p = reinterpret_cast<tagConSkill*>(pCondition);
		for (list<tagSkillCondition*>::iterator it=p->ltgSkillCondition.begin();
			it!=p->ltgSkillCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgSkillCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Distance")
	{
		tagConDistance *p = reinterpret_cast<tagConDistance*>(pCondition);
		for (list<tagDistanceCondition*>::iterator it=p->ltgDistanceCondition.begin();
			it!=p->ltgDistanceCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgDistanceCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="ValidObject")
	{
		tagConValidObject *p = reinterpret_cast<tagConValidObject*>(pCondition);
		for (list<tagValidObjectCondition*>::iterator it=p->ltgValidObjectCondition.begin();
			it!=p->ltgValidObjectCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgValidObjectCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Event")
	{
		tagConEvent *p = reinterpret_cast<tagConEvent*>(pCondition);
		for (list<tagEventCondition*>::iterator it=p->ltgEventCondition.begin();
			it!=p->ltgEventCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgEventCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Block")
	{
		tagConBlock *p = reinterpret_cast<tagConBlock*>(pCondition);
		list<tagBlockCondition*>::iterator it=p->ltgBlockCondition.begin();
		for (;it!=p->ltgBlockCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgBlockCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Script")
	{
		tagConScript *p = reinterpret_cast<tagConScript*>(pCondition);
		p->strScript.clear();		
		SAFE_DELETE(p);
	}
	else if (strType=="Timer")
	{
		tagConTimer *p = reinterpret_cast<tagConTimer*>(pCondition);
		SAFE_DELETE(p);
	}
	else if (strType=="Face")
	{
		tagConFace *p = reinterpret_cast<tagConFace*>(pCondition);
		for (list<tagFaceCondition*>::iterator it=p->ltgFaceCondition.begin();
			it!=p->ltgFaceCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgFaceCondition.clear();
		SAFE_DELETE(p);
	}
	else if (strType=="Region")
	{
		tagConRegion *p = reinterpret_cast<tagConRegion*>(pCondition);
		list<tagRegionCondition*>::iterator it=p->ltgRegionCondition.begin();
		for (;it!=p->ltgRegionCondition.end();++it)
		{
			SAFE_DELETE(*it);
		}
		p->ltgRegionCondition.clear();
		SAFE_DELETE(p);		
	}


}

//释放Do
void CNewSkillProperties::ClearDo(tagBaseChange *ptgBaseChange)
{
	if (!stricmp(ptgBaseChange->strChangeType.c_str(),"ObjectPropertyChange"))
	{
		//对象基本属性
		tagChangeProperty *ptgChange = dynamic_cast<tagChangeProperty*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"BuffChange"))
	{
		//状态
		tagChangeBuff *ptgChange = dynamic_cast<tagChangeBuff*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"PosChange"))
	{
		//位置
		tagChangePos *ptgChange = dynamic_cast<tagChangePos*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"AttakObjectChange"))
	{
		//改变攻击对象
		tagAttackChangeObject *ptgChange = dynamic_cast<tagAttackChangeObject*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"Script"))
	{			
		//脚本
		tagScriptID *ptgChange = dynamic_cast<tagScriptID*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"CooldownChange"))
	{
		//冷却时间
		tagCoolDownChange *ptgChange = dynamic_cast<tagCoolDownChange*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DestObjsChange"))
	{
		//对象列表
		tagDestObjChange *ptgChange = dynamic_cast<tagDestObjChange*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"RegisterTimer"))
	{
		//注册时间
		tagRegisterTimer  *ptgChange = dynamic_cast<tagRegisterTimer*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"RegisterEvent"))
	{
		//注册事件
		tagRegisterEvent  *ptgChange = dynamic_cast<tagRegisterEvent*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"UnRegisterTimer"))
	{
		//注消时间
		tagUnRegisterTimer  *ptgChange = dynamic_cast<tagUnRegisterTimer*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"UnRegisterEvent"))
	{
		//注消事件
		tagUnRegisterEvent  *ptgChange = dynamic_cast<tagUnRegisterEvent*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		
	}

	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"SendMsg"))
	{
		//发送消息
		tagSendMsg  *ptgChange = dynamic_cast<tagSendMsg*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		

	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DamageChange"))
	{
		//伤害计算
		tagDamageChange  *ptgChange = dynamic_cast<tagDamageChange*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		


	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DoDamage"))
	{
		tagDoDamage  *ptgChange = dynamic_cast<tagDoDamage*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);			
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"GoodChange"))
	{
		tagGoodChange *ptgChange = dynamic_cast<tagGoodChange*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(), "DirecChange"))
	{
		tagDirection *ptgChange = dynamic_cast<tagDirection*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"SkillChange"))
	{
		tagSkillChange *ptgChange = dynamic_cast<tagSkillChange*>(ptgBaseChange);
		SAFE_DELETE(ptgChange);
	}
    else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"GeniusChange"))
    {
        tagGeniusChange *ptgChange = dynamic_cast<tagGeniusChange*>(ptgBaseChange);
        SAFE_DELETE(ptgChange);
    }

}
//释放CDo
void	CNewSkillProperties::ClearCDo(tagCDo* ptgCDo)
{
	for (list<tagDoProcess*>::iterator TDPit=ptgCDo->ltgTrueDoProcess.begin();
		TDPit!=ptgCDo->ltgTrueDoProcess.end();++TDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*TDPit)->ltgBaseChange.begin();
			BCit!=(*TDPit)->ltgBaseChange.end();++BCit)
		{
			ClearDo(*BCit);
			/*tagBaseChange *ptgBaseChange = (*BCit);

			SAFE_DELETE(ptgBaseChange);*/
		}
		(*TDPit)->ltgBaseChange.clear();	

		for (list<tagCDo*>::iterator Dit=(*TDPit)->ltgCDo.begin();
			Dit!=(*TDPit)->ltgCDo.end();++Dit)
		{
			ClearCDo((*Dit));
		}					
		(*TDPit)->ltgCDo.clear();


		SAFE_DELETE(*TDPit);					
	}
	ptgCDo->ltgTrueDoProcess.clear();

	for (list<tagDoProcess*>::iterator FDPit=ptgCDo->ltgFalseDoProcess.begin();
		FDPit!=ptgCDo->ltgFalseDoProcess.end();++FDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*FDPit)->ltgBaseChange.begin();
			BCit!=(*FDPit)->ltgBaseChange.end();++BCit)
		{
			//SAFE_DELETE(*BCit)
			ClearDo(*BCit);
		}
		(*FDPit)->ltgBaseChange.clear();

		for (list<tagCDo*>::iterator Dit=(*FDPit)->ltgCDo.begin();
			Dit!=(*FDPit)->ltgCDo.end();++Dit)
		{
			ClearCDo((*Dit));
		}					
		(*FDPit)->ltgCDo.clear();



		SAFE_DELETE(*FDPit);	
	}
	ptgCDo->ltgFalseDoProcess.clear();
	SAFE_DELETE(ptgCDo);
}