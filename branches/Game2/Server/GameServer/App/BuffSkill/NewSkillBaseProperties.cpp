///////////////////////////////////////////////////////////////////////////////
///  NewSkillBaseProperties.cpp
///
///
///  CREATED: 08/07/2007 12:50:01 PM by 陈先进
///
///  PURPOSE:   技能属性类
///
//////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "NewSkillBaseProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNewSkill::CNewSkill(void)
{

}
CNewSkill::~CNewSkill()
{
	Clear();
}

//添加技能
void CNewSkill::AddNewSkill(tagSkill *pSkill)
{
	m_lSkill.push_back(pSkill);
}
//获取技能
CNewSkill::tagSkill *CNewSkill::GetNewSkill(DWORD dwLevel)
{
	if (m_lSkill.size()==0)
	{
		return NULL;
	}
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

//
/*
CNewSkill::stStaticParam*  CNewSkill::GetStaticParam(DWORD dwLevel)
{
    CNewSkill::tagSkill* skill = GetNewSkill(dwLevel);
    if( skill!=NULL )
        return skill->pStaticParam;
    return NULL;
}
*/

//模块类型
ModuleType	CNewSkill::GetModuleType(string strName)
{	
	if (strName=="ModuType_Buff")
	{
		return ModuType_Buff;
	} 
	else if (strName=="ModuType_DeBuff")
	{
		return ModuType_DeBuff;
	} 
	else if (strName=="ModuType_Skill_Active")
	{
		return ModuType_Skill_Active;
	} 
	else if (strName=="ModuType_Skill_Passive")
	{
		return ModuType_Skill_Passive;
	} 
	else if (strName=="ModuType_Skill_OnOff")
	{
		return ModuType_Skill_OnOff;
	}
	else
	{
		return ModuType_Buff;
	}
}
//释放
//void CNewSkill::Clear()
//{
//	//m_dwID = 0;
//}
void CNewSkill::ClearCondition(void *pCondition,string strType)
{
	if(strType=="BaseProperty")
	{
		tagConProperty *p = reinterpret_cast<tagConProperty*>(pCondition);
		for (list<tagPropertyCondition*>::iterator it=p->ltgProtertyConditon.begin();
			it!=p->ltgProtertyConditon.end();++it)
		{
			OBJ_RELEASE(tagPropertyCondition, *it);
		}
		p->ltgProtertyConditon.clear();
		OBJ_RELEASE(tagConProperty, p);
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
				OBJ_RELEASE(tagGoodAddProperty, *git);
			}
			(*it)->ltgGoodAddProperty.clear();
			OBJ_RELEASE(tagGoodCondition, *it);
		}

		p->ltgGoodCondition.clear();
		OBJ_RELEASE(tagConGood, p);
	}
	else if (strType=="Buff")
	{
		tagConBuff *p = reinterpret_cast<tagConBuff*>(pCondition);
		for (list<tagBuffCondition*>::iterator it=p->ltgBuffCondition.begin();
			it!=p->ltgBuffCondition.end();++it)
		{
			OBJ_RELEASE(tagBuffCondition, *it);
		}
		p->ltgBuffCondition.clear();
		OBJ_RELEASE(tagConBuff, p);
	}
    else if (strType=="Odds")
    {
        tagConOdds *p = reinterpret_cast<tagConOdds*>(pCondition);
        for (list<tagOddsCondition*>::iterator it=p->ltgOddsCondition.begin();
            it!=p->ltgOddsCondition.end();++it)
        {
            OBJ_RELEASE(tagOddsCondition, *it);
        }
        p->ltgOddsCondition.clear();
        OBJ_RELEASE(tagConOdds, p);
    }
    else if (strType=="Dispel")
    {
        tagConDispel *p = reinterpret_cast<tagConDispel*>(pCondition);
        for (list<tagDispelCondition*>::iterator it=p->ltgDispelCondition.begin();
            it!=p->ltgDispelCondition.end();++it)
        {
            OBJ_RELEASE(tagDispelCondition, *it);
        }
        p->ltgDispelCondition.clear();
        OBJ_RELEASE(tagConDispel, p);
    }
	else if (strType=="Skill")
	{
		tagConSkill *p = reinterpret_cast<tagConSkill*>(pCondition);
		for (list<tagSkillCondition*>::iterator it=p->ltgSkillCondition.begin();
			it!=p->ltgSkillCondition.end();++it)
		{
			OBJ_RELEASE(tagSkillCondition, *it);
		}
		p->ltgSkillCondition.clear();
		OBJ_RELEASE(tagConSkill, p);
	}
	else if (strType=="Distance")
	{
		tagConDistance *p = reinterpret_cast<tagConDistance*>(pCondition);
		for (list<tagDistanceCondition*>::iterator it=p->ltgDistanceCondition.begin();
			it!=p->ltgDistanceCondition.end();++it)
		{
			OBJ_RELEASE(tagDistanceCondition, *it);
		}
		p->ltgDistanceCondition.clear();
		OBJ_RELEASE(tagConDistance, p);

	}
	else if (strType=="ValidObject")
	{
		tagConValidObject *p = reinterpret_cast<tagConValidObject*>(pCondition);
		for (list<tagValidObjectCondition*>::iterator it=p->ltgValidObjectCondition.begin();
			it!=p->ltgValidObjectCondition.end();++it)
		{
			OBJ_RELEASE(tagValidObjectCondition, *it);
		}
		p->ltgValidObjectCondition.clear();
		OBJ_RELEASE(tagConValidObject, p);
	}
	else if (strType=="Event")
	{
		tagConEvent *p = reinterpret_cast<tagConEvent*>(pCondition);
		for (list<tagEventCondition*>::iterator it=p->ltgEventCondition.begin();
			it!=p->ltgEventCondition.end();++it)
		{
			OBJ_RELEASE(tagEventCondition, *it);
		}
		p->ltgEventCondition.clear();
		OBJ_RELEASE(tagConEvent, p);
	}
	else if (strType=="Block")
	{
		tagConBlock *p = reinterpret_cast<tagConBlock*>(pCondition);
		for (list<tagBlockCondition*>::iterator it=p->ltgBlockCondition.begin();
			it!=p->ltgBlockCondition.end();++it)
		{
			OBJ_RELEASE(tagBlockCondition, *it);
		}
		p->ltgBlockCondition.clear();
		OBJ_RELEASE(tagConBlock, p);
	}
	else if (strType=="Script")
	{
		tagConScript *p = reinterpret_cast<tagConScript*>(pCondition);			
		p->strScript.clear();
		OBJ_RELEASE(tagConScript, p);
	}
	else if (strType=="Face")
	{
		tagConFace *p = reinterpret_cast<tagConFace*>(pCondition);
		for (list<tagFaceCondition*>::iterator it=p->ltgFaceCondition.begin();
			it!=p->ltgFaceCondition.end();++it)
		{
			OBJ_RELEASE(tagFaceCondition, *it);
		}
		p->ltgFaceCondition.clear();
		OBJ_RELEASE(tagConFace, p);
	}	
	else if(strType=="Timer")
	{
		tagConTimer *p = reinterpret_cast<tagConTimer*>(pCondition);		
		OBJ_RELEASE(tagConTimer, p);
	}	
	else if (strType=="Region")
	{
		tagConRegion *p = reinterpret_cast<tagConRegion*>(pCondition);
		list<tagRegionCondition*>::iterator it=p->ltgRegionCondition.begin();
		for (;it!=p->ltgRegionCondition.end();++it)
		{
			OBJ_RELEASE(tagRegionCondition, *it);
		}
		p->ltgRegionCondition.clear();
		OBJ_RELEASE(tagConRegion, p);
	}



}

void   CNewSkill::ClearBaseChange(tagBaseChange *ptgBaseChange)
{
	if(ptgBaseChange->strChangeType=="ObjectPropertyChange")
	{
		//属性
		tagChangeProperty *pTemp = dynamic_cast<tagChangeProperty*>(ptgBaseChange);				
		pTemp->strName.clear();
		pTemp->strOperator.clear();
		pTemp->strScriptVarName.clear();
        pTemp->paramname.clear();
		OBJ_RELEASE(tagChangeProperty, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="BuffChange")
	{
		//状态
		tagChangeBuff *pTemp = dynamic_cast<tagChangeBuff*>(ptgBaseChange);			
		pTemp->strOperator.clear();
		pTemp->strSIDName.clear();
		pTemp->strSLvName.clear();
        pTemp->strSkillType.clear();
		OBJ_RELEASE(tagChangeBuff, pTemp);

	}
	else if (ptgBaseChange->strChangeType=="PosChange")
	{
		//位置
		tagChangePos *pTemp = dynamic_cast<tagChangePos*>(ptgBaseChange);				
		pTemp->strDir.clear();
		pTemp->strRefObject.clear();
		pTemp->strStyle.clear();
		pTemp->strSpeed.clear();
		OBJ_RELEASE(tagChangePos, pTemp);
	} 
	else if (ptgBaseChange->strChangeType=="AttakObjectChange")
	{
		tagAttackChangeObject *pTemp = dynamic_cast<tagAttackChangeObject*>(ptgBaseChange);
		pTemp->strAttackChangeObject.clear();
		OBJ_RELEASE(tagAttackChangeObject, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="CooldownChange")
	{
		//时间
		tagCoolDownChange *pTemp = dynamic_cast<tagCoolDownChange*>(ptgBaseChange);
		pTemp->strType.clear();
		pTemp->strSVarianame.clear();
        pTemp->paramname.clear();
		OBJ_RELEASE(tagCoolDownChange, pTemp);
	} 
	else if (ptgBaseChange->strChangeType=="DestObjsChange")
	{
		//目标对象
		tagDestObjChange *pTemp = dynamic_cast<tagDestObjChange*>(ptgBaseChange);
		pTemp->strDestObjChange.clear();
		OBJ_RELEASE(tagDestObjChange, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="RegisterTimer")
	{
		//注册定时器
		tagRegisterTimer *pTemp = dynamic_cast<tagRegisterTimer*>(ptgBaseChange);
		pTemp->strSCountName.clear();
		pTemp->strSFutvarname.clear();
		pTemp->strSIntelName.clear();
        pTemp->strVFuturename.clear();
        pTemp->strVIntername.clear();
        pTemp->strVCountname.clear();

		OBJ_RELEASE(tagRegisterTimer, pTemp);
	} 
	else if (ptgBaseChange->strChangeType=="RegisterEvent")
	{
		//注册事件
		tagRegisterEvent *pTemp = dynamic_cast<tagRegisterEvent*>(ptgBaseChange);		
		pTemp->strName.clear();
		OBJ_RELEASE(tagRegisterEvent, pTemp);
	}	
	else if (ptgBaseChange->strChangeType=="SendMsg")
	{
		//发送消息
		tagSendMsg *pTemp = dynamic_cast<tagSendMsg*>(ptgBaseChange);	
		pTemp->strType.clear();
		pTemp->strRange.clear();
		pTemp->strSVName.clear();
		OBJ_RELEASE(tagSendMsg, pTemp);
	} 
	else if (ptgBaseChange->strChangeType=="DamageChange")
	{
		//伤害计算
		tagDamageChange *pTemp = dynamic_cast<tagDamageChange*>(ptgBaseChange);		
		pTemp->strOperator.clear();
		pTemp->strType.clear();
		pTemp->strSVarianame.clear();
        pTemp->strDamagename.clear();
		OBJ_RELEASE(tagDamageChange, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="DoDamage")
	{
		//执行伤害
		tagDoDamage *pTemp = dynamic_cast<tagDoDamage*>(ptgBaseChange);		
		pTemp->strSBlastVariName.clear();
		pTemp->strSResVariName.clear();
        pTemp->strDamagename.clear();
        pTemp->strBlastname.clear();
		OBJ_RELEASE(tagDoDamage, pTemp);
	} 
	else if (ptgBaseChange->strChangeType=="GoodChange")
	{
		//物品
		tagGoodChange *pTemp = dynamic_cast<tagGoodChange*>(ptgBaseChange);
		pTemp->strName.clear();
		pTemp->strOperator.clear();
		OBJ_RELEASE(tagGoodChange, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="DirecChange")
	{
		//方向改变
		tagDirection *pTemp = dynamic_cast<tagDirection*>(ptgBaseChange);
		pTemp->strDir.clear();
		OBJ_RELEASE(tagDirection, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="Script")
	{
		tagScriptID *pTemp = dynamic_cast<tagScriptID*>(ptgBaseChange);
		OBJ_RELEASE(tagScriptID, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="UnRegisterTimer")
	{
		//注消时间
		tagUnRegisterTimer *pTemp = dynamic_cast<tagUnRegisterTimer*>(ptgBaseChange);
		OBJ_RELEASE(tagUnRegisterTimer, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="UnRegisterEvent")
	{
		//注消事件
		tagUnRegisterEvent *pTemp = dynamic_cast<tagUnRegisterEvent*>(ptgBaseChange);
		OBJ_RELEASE(tagUnRegisterEvent, pTemp);
	}
	else if (ptgBaseChange->strChangeType=="SkillChange")
	{
		//技能操作
		tagSkillChange *pTemp = dynamic_cast<tagSkillChange*>(ptgBaseChange);
		OBJ_RELEASE(tagSkillChange, pTemp);
	}
    else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"GeniusChange"))
    {
        tagGeniusChange *ptgChange = dynamic_cast<tagGeniusChange*>(ptgBaseChange);
        OBJ_RELEASE(tagGeniusChange,ptgChange);
    }
}
//释放
void	CNewSkill::ClearCDo(tagCDo *ptgCDo)
{
	for (list<tagDoProcess*>::iterator TDPit=ptgCDo->ltgTrueDoProcess.begin();
		TDPit!=ptgCDo->ltgTrueDoProcess.end();++TDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*TDPit)->ltgBaseChange.begin();
			BCit!=(*TDPit)->ltgBaseChange.end();++BCit)
		{
			ClearBaseChange(*BCit);
			//SAFE_DELETE(*BCit);
		}
		(*TDPit)->ltgBaseChange.clear();	

		for (list<tagCDo*>::iterator Dit=(*TDPit)->ltgCDo.begin();
			Dit!=(*TDPit)->ltgCDo.end();++Dit)
		{						
			ClearCDo((*Dit));
		}					
		(*TDPit)->ltgCDo.clear();

		OBJ_RELEASE(tagDoProcess, *TDPit);
	}
	ptgCDo->ltgTrueDoProcess.clear();

	for (list<tagDoProcess*>::iterator FDPit=ptgCDo->ltgFalseDoProcess.begin();
		FDPit!=ptgCDo->ltgFalseDoProcess.end();++FDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*FDPit)->ltgBaseChange.begin();
			BCit!=(*FDPit)->ltgBaseChange.end();++BCit)
		{
			ClearBaseChange(*BCit);
			//SAFE_DELETE(*BCit);
		}
		(*FDPit)->ltgBaseChange.clear();

		for (list<tagCDo*>::iterator Dit=(*FDPit)->ltgCDo.begin();
			Dit!=(*FDPit)->ltgCDo.end();++Dit)
		{						
			ClearCDo((*Dit));
		}					
		(*FDPit)->ltgCDo.clear();		

		OBJ_RELEASE(tagDoProcess, *FDPit);
	}
	ptgCDo->ltgFalseDoProcess.clear();

	OBJ_RELEASE(tagCDo, ptgCDo);
}
/*
NAME:Clear()
DESC:释放内存
PARA:无
*/
void CNewSkill::Clear()								
{

	//条件
	for (map<DWORD,tagCondition*>::iterator it=m_mapCondition.begin();
		it!=m_mapCondition.end();++it)
	{
		//具体条件
		ClearCondition((it->second)->pVoidCondition,(it->second)->strType);		
		OBJ_RELEASE(tagCondition, it->second);
	}
	m_mapCondition.clear();
	m_mapFormat.clear();

	//条件组
	for (map<DWORD,tagConditionGroup*>::iterator it=m_mapConditionGroup.begin();
		it!=m_mapConditionGroup.end();++it)
	{
		//条件组
		OBJ_RELEASE(tagConditionGroup, it->second);
	}
	m_mapConditionGroup.clear();

	//作用对象
	for(map<DWORD,tagOperateObject*>::iterator it=m_mapOperateObject.begin();
		it!=m_mapOperateObject.end();++it)
	{
		OBJ_RELEASE(tagOperateObject, it->second);
	}
	m_mapOperateObject.clear();

	for (list<tagSkill*>::iterator it=m_lSkill.begin();
		it!=m_lSkill.end(); ++it)
	{
		for (map<DWORD,tagCondition*>::iterator cit=(*it)->mapCondition.begin();
			cit!=(*it)->mapCondition.end();++cit)
		{	
			ClearCondition((cit->second)->pVoidCondition,(cit->second)->strType);		
			OBJ_RELEASE(tagCondition, cit->second);
		}
		(*it)->mapCondition.clear();

		for(list<tagProcess*>::iterator Pit=(*it)->ltgExecuteStep.begin();
			Pit!=(*it)->ltgExecuteStep.end();++Pit)
		{
			for (list<tagStep*>::iterator Sit=(*Pit)->ltgStep.begin();
				Sit!=(*Pit)->ltgStep.end();++Sit)
			{
				for (list<tagDo*>::iterator SDit=(*Sit)->ltgDo.begin();
					SDit!=(*Sit)->ltgDo.end();++SDit)
				{

					for (list<tagCDo*>::iterator Dit=(*SDit)->ltgCDo.begin();
						Dit!=(*SDit)->ltgCDo.end();++Dit)
					{						
						ClearCDo((*Dit));
					}					
					(*SDit)->ltgCDo.clear();
					OBJ_RELEASE(tagDo, *SDit);
				}
				(*Sit)->ltgDo.clear();
				OBJ_RELEASE(tagStep, *Sit);

			}
			(*Pit)->ltgStep.clear();
			OBJ_RELEASE(tagProcess, *Pit);

		}
		(*it)->ltgExecuteStep.clear();
		OBJ_RELEASE(tagSkill, *it);
	}

	m_lSkill.clear();
}