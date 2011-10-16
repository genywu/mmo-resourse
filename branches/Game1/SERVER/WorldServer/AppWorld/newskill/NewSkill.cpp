//////////////////////////////////////////////////////////////////////////////////////////////////
///  NewSkill.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\WorldServer\AppWorld\New Skill
///
///  CREATED: 08/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   技能类
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NewSkill.h"

/*
NAME:CNewSkill()
DESC:默认构造函数
PARA:无
*/

CNewSkill ::CNewSkill()
{
	m_dwID = 0;	
}

/*
NAME:CNewSkill(DWORD dwID)
DESC:构造函数
PARA:dwID为技能的ID
*/

CNewSkill::CNewSkill(DWORD dwID)
{
	m_dwID = dwID;
}

/*
NAME:~CNewSkill()
DESC:析构函数
PARA:无
*/
CNewSkill::~CNewSkill()
{
	Clear();
}

/*
NAME:AddNewSkill()
DESC:添加技能
PARA:pSkill技能的主要结构
*/
void CNewSkill::AddNewSkill( tagSkill *pSkill)
{
	m_lSkill.push_back(pSkill);
}

/*
NAME:AddConditionLength()
DESC:条件编码
PARA:vRet数组
	 ptgCondition条件指针
*/
void CNewSkill::AddConditionLength( vector<BYTE>& vRet,tagCondition *ptgCondition)
{
	DWORD length=0;
	string strType = ptgCondition->strType;

	length = ptgCondition->dwLength;
	_AddToByteArray(&vRet,length);

	if (strType=="BaseProperty")
	{	
		//基本属性
		tagConProperty *ptgConProperty = reinterpret_cast<tagConProperty*>(ptgCondition->pVoidCondition);
		for (list<tagPropertyCondition*>::iterator it = ptgConProperty->ltgProtertyConditon.begin();
			it!=ptgConProperty->ltgProtertyConditon.end();++it)
		{			
			_AddToByteArray(&vRet,(*it)->dwValue);
			_AddToByteArray(&vRet,(char*)(*it)->cmpareOp.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strName.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strObject.c_str());		
		}		
	}
	else if (strType=="Good")
	{
		//物品
		tagConGood *ptgConGood = reinterpret_cast<tagConGood*>(ptgCondition->pVoidCondition);
		for (list<tagGoodCondition*>::iterator it = ptgConGood->ltgGoodCondition.begin();
			it!=ptgConGood->ltgGoodCondition.end();++it)
		{			
			_AddToByteArray(&vRet,(*it)->dwPlace);
			_AddToByteArray(&vRet,(*it)->dwPos);
			_AddToByteArray(&vRet,(*it)->dwValue);
			_AddToByteArray(&vRet,(char*)(*it)->strCmpFun.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strName.c_str());
			_AddToByteArray(&vRet,(*it)->dwType);
			_AddToByteArray(&vRet,(char*)(*it)->strObject.c_str());
			_AddToByteArray(&vRet,(long)((*it)->ltgGoodAddProperty.size()));
			for (list<tagGoodAddProperty*>::iterator git=(*it)->ltgGoodAddProperty.begin();git!=(*it)->ltgGoodAddProperty.end();++git)
			{
				_AddToByteArray(&vRet,(*git),sizeof(tagGoodAddProperty));
			}
		}
	}
	else if (strType=="Buff")
	{
		//状态
		tagConBuff *ptgConBuff= reinterpret_cast<tagConBuff*>(ptgCondition->pVoidCondition);
		for (list<tagBuffCondition*>::iterator it = ptgConBuff->ltgBuffCondition.begin();
			it!=ptgConBuff->ltgBuffCondition.end();++it)
		{
			_AddToByteArray(&vRet,(*it)->dwID);
			_AddToByteArray(&vRet,(*it)->dwLevel);
			_AddToByteArray(&vRet,(*it)->dwNum);
			_AddToByteArray(&vRet,(char*)(*it)->strObject.c_str());
		}
	}
	else if (strType=="Distance")
	{
		//距离
		tagConDistance *ptgConDistance = reinterpret_cast<tagConDistance*>(ptgCondition->pVoidCondition);
		for (list<tagDistanceCondition*>::iterator it = ptgConDistance->ltgDistanceCondition.begin();
			it!=ptgConDistance->ltgDistanceCondition.end();++it)
		{			
			_AddToByteArray(&vRet,(*it)->dwValue);
			_AddToByteArray(&vRet,(char*)(*it)->strCmpFun.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strRefObj.c_str());
		}
	}
	else if (strType=="ValidObject")
	{
		//有效对象
		tagConValidObject *ptgConValidObject = reinterpret_cast<tagConValidObject*>(ptgCondition->pVoidCondition);
		for (list<tagValidObjectCondition*>::iterator it = ptgConValidObject->ltgValidObjectCondition.begin();
			it!=ptgConValidObject->ltgValidObjectCondition.end();++it)
		{			
			_AddToByteArray(&vRet,(char*)(*it)->strType.c_str());			
			_AddToByteArray(&vRet,(long)(*it)->lObjType.size());
			for(list<string>::iterator Oit= (*it)->lObjType.begin();Oit!=(*it)->lObjType.end();++Oit)
			{
				_AddToByteArray(&vRet,(char*)(*Oit).c_str());
			}
		}
	}
	else if (strType=="Event")
	{
		//事件
		tagConEvent *ptgConEvent = reinterpret_cast<tagConEvent*>(ptgCondition->pVoidCondition);
		for (list<tagEventCondition*>::iterator it = ptgConEvent->ltgEventCondition.begin();
			it!=ptgConEvent->ltgEventCondition.end();++it)
		{
			_AddToByteArray(&vRet,(*it)->dwFlag);
			_AddToByteArray(&vRet,(char*)(*it)->strEventName.c_str());
			_AddToByteArray(&vRet,(*it)->dwVar1);
			_AddToByteArray(&vRet,(*it)->dwVar2);
			_AddToByteArray(&vRet,(*it)->dwVar3);
			_AddToByteArray(&vRet,(*it)->dwVar4);
			_AddToByteArray(&vRet,(char*)(*it)->strName.c_str());
		}

	}
	else if (strType=="Timer")
	{
		//时间
		tagConTimer *ptgConTimer = reinterpret_cast<tagConTimer*>(ptgCondition->pVoidCondition);
		for (list<DWORD>::iterator it=ptgConTimer->ldwConTimer.begin();
			it!=ptgConTimer->ldwConTimer.end();++it)
		{
			_AddToByteArray(&vRet,*it);
		}

	}
	else if (strType=="Script")
	{
		//脚本
		tagConScript *ptgConScript = reinterpret_cast<tagConScript*>(ptgCondition->pVoidCondition);
		_AddToByteArray(&vRet,(char*)ptgConScript->strScript.c_str());
	}
	else if (strType=="Block")
	{
		//阻挡
		tagConBlock *ptgConBlcok = reinterpret_cast<tagConBlock*>(ptgCondition->pVoidCondition);
		for (list<tagBlockCondition*>::iterator it=ptgConBlcok->ltgBlockCondition.begin();
			 it!=ptgConBlcok->ltgBlockCondition.end();++it)
		{
			_AddToByteArray(&vRet,(char*)(*it)->strType.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strDir.c_str());
			_AddToByteArray(&vRet,(*it)->dwBlockType);
			_AddToByteArray(&vRet,(*it)->dwShapeType);
		}
	}
	else if (strType=="Face")
	{
		tagConFace *ptgConFace = reinterpret_cast<tagConFace*>(ptgCondition->pVoidCondition);
		for (list<tagFaceCondition*>::iterator it=ptgConFace->ltgFaceCondition.begin();
			it!=ptgConFace->ltgFaceCondition.end();++it)
		{
			_AddToByteArray(&vRet,(char*)(*it)->strFaceType.c_str());
		}
	}
	else if (strType=="Region")
	{
		tagConRegion *ptgConRegion = reinterpret_cast<tagConRegion*>(ptgCondition->pVoidCondition);
		for(list<tagRegionCondition*>::iterator it=ptgConRegion->ltgRegionCondition.begin();
			it!=ptgConRegion->ltgRegionCondition.end();++it)
		{
			_AddToByteArray(&vRet,(char*)(*it)->strName.c_str());
			_AddToByteArray(&vRet,(DWORD)(*it)->dwType);
		}
	}	
}
/*
NAME:AddNewSkillToByteArray()
DESC:编码
PARA:vRet数组
*/
bool CNewSkill::AddNewSkillToByteArray(vector<BYTE>& vRet)
{
	_AddToByteArray(&vRet, m_dwID);
	_AddToByteArray(&vRet, (char*)m_strName.c_str());
	_AddToByteArray(&vRet, (char*)m_strInitSricpt.c_str());
	_AddToByteArray(&vRet, m_dwElement);
	_AddToByteArray(&vRet, (char*)m_strType.c_str());
	_AddToByteArray(&vRet, m_bUseType);
	_AddToByteArray(&vRet, m_bOnly);
	_AddToByteArray(&vRet, m_dwOnlyType);
	_AddToByteArray(&vRet, m_bNameOnly);
	_AddToByteArray(&vRet, m_bRoleOnly);
	_AddToByteArray(&vRet, m_bAttack);
	_AddToByteArray(&vRet, m_bTarget);
	_AddToByteArray(&vRet, m_bCooldown);
	_AddToByteArray(&vRet, m_bDeadUse);
	_AddToByteArray(&vRet, m_bDeadEnd);
	_AddToByteArray(&vRet, m_bBreak);
	_AddToByteArray(&vRet, m_bPetType);
	_AddToByteArray(&vRet, m_bTime);
	_AddToByteArray(&vRet, m_bSystem);
	_AddToByteArray(&vRet, m_dwTotalNum);
	_AddToByteArray(&vRet,(char*)m_strAddSub.c_str());
	
	//公式个数
	_AddToByteArray(&vRet,(long)m_mapFormat.size());
	for (map<DWORD,tagExpression>::iterator it=m_mapFormat.begin();
		it!=m_mapFormat.end();++it)
	{
		//公式ID
		_AddToByteArray(&vRet,it->first);
		//公式
		_AddToByteArray(&vRet,(char*)((*it).second.strFileName.c_str() ));
		_AddToByteArray(&vRet,(char*)((*it).second.strContent.c_str() ));
	}

	//条件组
	//个数
	_AddToByteArray(&vRet,(long)m_mapConditionGroup.size());
	for (map<DWORD,tagConditionGroup*>::iterator it=m_mapConditionGroup.begin();
		it!=m_mapConditionGroup.end();++it)
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
	_AddToByteArray(&vRet,(long)m_mapCondition.size());

	for (map<DWORD,tagCondition*>::iterator it=m_mapCondition.begin();
		it!=m_mapCondition.end();++it)
	{
		//ID
		_AddToByteArray(&vRet,it->first);			
		tagCondition *ptgCondition = it->second;


		_AddToByteArray(&vRet,(char*)ptgCondition->strType.c_str());
		AddConditionLength(vRet,ptgCondition);
	}

	//作用对象
	//个数
	_AddToByteArray(&vRet,(long)m_mapOperateObject.size());

	for (map<DWORD,tagOperateObject*>::iterator Oit=m_mapOperateObject.begin();
		Oit!=m_mapOperateObject.end();++Oit)
	{			
		_AddToByteArray(&vRet,(Oit->second)->dwID);
		_AddToByteArray(&vRet,(Oit->second)->dwDistance);
		_AddToByteArray(&vRet,(Oit->second)->dwRadius);
		_AddToByteArray(&vRet,(Oit->second)->dwSkillID);
		_AddToByteArray(&vRet,(Oit->second)->dwSkillLevel);
		_AddToByteArray(&vRet,(Oit->second)->dwConGroupID);		
		_AddToByteArray(&vRet,(Oit->second)->dwNumObject);

		_AddToByteArray(&vRet,(char*)(Oit->second)->strType.c_str());
		_AddToByteArray(&vRet,(char*)(Oit->second)->strPosition.c_str());
		_AddToByteArray(&vRet,(char*)(Oit->second)->strCenterObject.c_str());
		_AddToByteArray(&vRet,(char*)(Oit->second)->strObject.c_str());
	}

	//各个等级技能个数
	_AddToByteArray(&vRet,(long)m_lSkill.size());

	for (list<tagSkill*>::iterator it=m_lSkill.begin(); it!=m_lSkill.end(); ++it)
	{
		if((*it)->pStaticParam)
		{
			_AddToByteArray(&vRet, (BYTE)1);
			_AddToByteArray(&vRet, (*it)->pStaticParam->lCoolDownTime);
			_AddToByteArray(&vRet, (*it)->pStaticParam->lConsMp);
			_AddToByteArray(&vRet, (*it)->pStaticParam->lMinAtkDistance);
			_AddToByteArray(&vRet, (*it)->pStaticParam->lMaxAtkDistance);
			_AddToByteArray(&vRet, (*it)->pStaticParam->lActTime);
			_AddToByteArray(&vRet,(char*)(*it)->pStaticParam->strBuffID.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->pStaticParam->strNBuffID.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->pStaticParam->strScriptParam.c_str());
		}
		else
		{
			_AddToByteArray(&vRet, (BYTE)0);
		}
		//等级
		_AddToByteArray(&vRet,(*it)->dwLevel);
		//内部条件个数
		_AddToByteArray(&vRet,(long)(*it)->mapCondition.size()); 

		for (map<DWORD,tagCondition*>::iterator Cit=(*it)->mapCondition.begin();
			Cit!=(*it)->mapCondition.end();++Cit)
		{
			//ID
			_AddToByteArray(&vRet,Cit->first);			
			tagCondition *ptgCondition = Cit->second;
			_AddToByteArray(&vRet,(char*)ptgCondition->strType.c_str());
			AddConditionLength(vRet,ptgCondition);
		}
		

		//步骤
		//个数
		_AddToByteArray(&vRet,(long)(*it)->ltgExecuteStep.size());
		for (list<tagProcess*>::iterator Sit=(*it)->ltgExecuteStep.begin();
			Sit!=(*it)->ltgExecuteStep.end();++Sit)
		{
			//过程名称
			_AddToByteArray(&vRet,(char*)(*Sit)->strName.c_str());

			//执行步骤个数
			_AddToByteArray(&vRet,(long)(*Sit)->ltgStep.size());
			for(list<tagStep*>::iterator Stepit=(*Sit)->ltgStep.begin();
				Stepit!=(*Sit)->ltgStep.end();++Stepit)
			{
				//名称
				_AddToByteArray(&vRet,(char*)(*Stepit)->strName.c_str());

				//Do个数
				_AddToByteArray(&vRet,(long)(*Stepit)->ltgDo.size());

				for (list<tagDo*>::iterator SDit=(*Stepit)->ltgDo.begin();
					SDit!=(*Stepit)->ltgDo.end();++SDit)
				{
					//条件						
					//作用对象
					_AddToByteArray(&vRet,(*SDit)->dwOperobjid);
					//返回结果
					_AddToByteArray(&vRet,(char*)(*SDit)->strRet.c_str());

					//CDo个娄
					_AddToByteArray(&vRet,(long)(*SDit)->ltgCDo.size());

					for (list<tagCDo*>::iterator Dit=(*SDit)->ltgCDo.begin();
						Dit!=(*SDit)->ltgCDo.end();++Dit)
					{
						//真结果
						_AddToByteArray(&vRet,(char*)(*Dit)->tgCondition.strTrueRet.c_str());
						//假结果
						_AddToByteArray(&vRet,(char*)(*Dit)->tgCondition.strFalseRet.c_str());
						//条件组ID
						_AddToByteArray(&vRet,(*Dit)->tgCondition.dwConditionGroup);
						//真处理个数
						_AddToByteArray(&vRet,(long)(*Dit)->ltgTrueDoProcess.size());
						AddDoProcessToByteArray(vRet,(*Dit)->ltgTrueDoProcess);
						//假处理
						_AddToByteArray(&vRet,(long)(*Dit)->ltgFalseDoProcess.size());
						AddDoProcessToByteArray(vRet,(*Dit)->ltgFalseDoProcess);						
					}			


				}	

			}

		}
	}
	return true;
}
/*
NAME:AddDoProcessToByteArray()
DESC:DoProcess编码
PARA:vRet数组
	 ltgDoProcess过程列表
*/
void CNewSkill::AddDoProcessToByteArray(vector<BYTE>& vRet,list<tagDoProcess*> &ltgDoProcess)
{


	for (list<tagDoProcess*>::iterator DFPit=ltgDoProcess.begin();
		DFPit!=ltgDoProcess.end();++DFPit)
	{
		//条件组个数
		_AddToByteArray(&vRet,(long)(*DFPit)->ltgCDo.size());

		for (list<tagCDo*>::iterator Dit=(*DFPit)->ltgCDo.begin();
			Dit!=(*DFPit)->ltgCDo.end();++Dit)
		{
			//真结果
			_AddToByteArray(&vRet,(char*)(*Dit)->tgCondition.strTrueRet.c_str());
			//假结果
			_AddToByteArray(&vRet,(char*)(*Dit)->tgCondition.strFalseRet.c_str());
			//条件组ID
			_AddToByteArray(&vRet,(*Dit)->tgCondition.dwConditionGroup);
			//真处理个数
			_AddToByteArray(&vRet,(long)(*Dit)->ltgTrueDoProcess.size());
			AddDoProcessToByteArray(vRet,(*Dit)->ltgTrueDoProcess);
			//假处理
			_AddToByteArray(&vRet,(long)(*Dit)->ltgFalseDoProcess.size());
			AddDoProcessToByteArray(vRet,(*Dit)->ltgFalseDoProcess);						
		}		


		//个数
		_AddToByteArray(&vRet,(long)(*DFPit)->ltgBaseChange.size());
		for (list<tagBaseChange*>::iterator it=(*DFPit)->ltgBaseChange.begin();
			it!=(*DFPit)->ltgBaseChange.end();++it)
		{
			_AddToByteArray(&vRet,(char*)(*it)->strChangeType.c_str());
			_AddToByteArray(&vRet,(char*)(*it)->strObj.c_str());
			
			if ((*it)->strChangeType=="Script")
			{
				//脚本
				tagScriptID *pTemp = dynamic_cast<tagScriptID*>(*it);				
				_AddToByteArray(&vRet,pTemp->dwScriptID);				
			} 
			else if((*it)->strChangeType=="ObjectPropertyChange")
			{
				//属性
				tagChangeProperty *pTemp = dynamic_cast<tagChangeProperty*>(*it);				
				_AddToByteArray(&vRet,pTemp->dwValue);
				_AddToByteArray(&vRet,(char*)pTemp->strName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strOperator.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strScriptVarName.c_str());

			}
			else if ((*it)->strChangeType=="BuffChange")
			{
				//状态
				tagChangeBuff *pTemp = dynamic_cast<tagChangeBuff*>(*it);			
				_AddToByteArray(&vRet,pTemp->dwID);
				_AddToByteArray(&vRet,pTemp->dwLevel);

				_AddToByteArray(&vRet,pTemp->dwCount);
				_AddToByteArray(&vRet,(char*)pTemp->strSIDName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSLvName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSCountName.c_str());



				_AddToByteArray(&vRet,(char*)pTemp->strOperator.c_str());

			}
			else if ((*it)->strChangeType=="PosChange")
			{
				//位置
				tagChangePos *pTemp = dynamic_cast<tagChangePos*>(*it);				
				_AddToByteArray(&vRet,(char*)pTemp->strDir.c_str());				
				_AddToByteArray(&vRet,(char*)pTemp->strRefObject.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strStyle.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSpeed.c_str());

			} 
			else if ((*it)->strChangeType=="AttakObjectChange")
			{
				tagAttackChangeObject *pTemp = dynamic_cast<tagAttackChangeObject*>(*it);
				_AddToByteArray(&vRet,(char*)pTemp->strAttackChangeObject.c_str());

			}
			else if ((*it)->strChangeType=="CooldownChange")
			{
				//时间
				tagCoolDownChange *pTemp = dynamic_cast<tagCoolDownChange*>(*it);
				_AddToByteArray(&vRet,pTemp->dwValue);
				_AddToByteArray(&vRet,(char*)pTemp->strType.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSVarianame.c_str());

			} 
			else if ((*it)->strChangeType=="DestObjsChange")
			{
				//目标对象
				tagDestObjChange *pTemp = dynamic_cast<tagDestObjChange*>(*it);
				_AddToByteArray(&vRet,(char*)pTemp->strDestObjChange.c_str());
			}
			else if ((*it)->strChangeType=="RegisterTimer")
			{
				//注册定时器
				tagRegisterTimer *pTemp = dynamic_cast<tagRegisterTimer*>(*it);
				_AddToByteArray(&vRet,pTemp->dwCount);
				_AddToByteArray(&vRet,pTemp->dwFlag);
				_AddToByteArray(&vRet,pTemp->dwFuture);
				_AddToByteArray(&vRet,pTemp->dwIntelTime);
				_AddToByteArray(&vRet,(char*)pTemp->strSCountName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSFutvarname.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSIntelName.c_str());
			} 
			else if ((*it)->strChangeType=="RegisterEvent")
			{
				//注册事件
				tagRegisterEvent *pTemp = dynamic_cast<tagRegisterEvent*>(*it);
				_AddToByteArray(&vRet,pTemp->dwFlag);
				_AddToByteArray(&vRet,(char*)pTemp->strName.c_str());
			}
			else if ((*it)->strChangeType=="UnRegisterTimer")
			{
				//注消时间
				tagUnRegisterTimer *pTemp = dynamic_cast<tagUnRegisterTimer*>(*it);
				_AddToByteArray(&vRet,pTemp->dwUnRegisterTimer);
			} 
			else if ((*it)->strChangeType=="UnRegisterEvent")
			{
				//注消事件
				tagUnRegisterEvent *pTemp = dynamic_cast<tagUnRegisterEvent*>(*it);
				_AddToByteArray(&vRet,pTemp->dwUnRegisterEvent);
			}
			else if ((*it)->strChangeType=="SendMsg")
			{
				//发送消息
				tagSendMsg *pTemp = dynamic_cast<tagSendMsg*>(*it);
				_AddToByteArray(&vRet,pTemp->dwValue);
				_AddToByteArray(&vRet,(char*)pTemp->strType.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strRange.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSVName.c_str());
				_AddToByteArray(&vRet,(BYTE)pTemp->nAdd);
			} 
			else if ((*it)->strChangeType=="DamageChange")
			{
				//伤害计算
				tagDamageChange *pTemp = dynamic_cast<tagDamageChange*>(*it);
				_AddToByteArray(&vRet,pTemp->dwValue);			
				_AddToByteArray(&vRet,(char*)pTemp->strOperator.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strType.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSVarianame.c_str());
			}
			else if ((*it)->strChangeType=="DoDamage")
			{
				//执行伤害
				tagDoDamage *pTemp = dynamic_cast<tagDoDamage*>(*it);
				_AddToByteArray(&vRet,pTemp->dwBlastAttack);
				_AddToByteArray(&vRet,pTemp->dwResult);
				_AddToByteArray(&vRet,(char*)pTemp->strSBlastVariName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strSResVariName.c_str());
			} 
			else if ((*it)->strChangeType=="GoodChange")
			{
				//物品
				tagGoodChange *pTemp = dynamic_cast<tagGoodChange*>(*it);
				_AddToByteArray(&vRet,pTemp->dwPlace);
				_AddToByteArray(&vRet,pTemp->dwPos);
				_AddToByteArray(&vRet,pTemp->dwValue);
				_AddToByteArray(&vRet,(char*)pTemp->strName.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strOperator.c_str());
				_AddToByteArray(&vRet,pTemp->dwGoodsType);
				_AddToByteArray(&vRet,pTemp->dwWeaponType);

			}
			else if ((*it)->strChangeType=="DirecChange")
			{
				//方向改变
				tagDirection *pTemp = dynamic_cast<tagDirection*>(*it);
				_AddToByteArray(&vRet,(char*)pTemp->strDir.c_str());
			}
			else if ((*it)->strChangeType=="SkillChange")
			{
				//技能操作
				tagSkillChange *pTemp = dynamic_cast<tagSkillChange*>(*it);
				_AddToByteArray(&vRet,(char*)pTemp->strObj.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strType.c_str());
				_AddToByteArray(&vRet,(char*)pTemp->strOpera.c_str());
				_AddToByteArray(&vRet,pTemp->lID);
				_AddToByteArray(&vRet,pTemp->lLv);
			}
		}

	}

}
/*
NAME:Clear()
DESC:释放条件
PARA:pCondition条件指针
	 strType条件类型
*/

void CNewSkill::ClearCondition(void *pCondition,string strType)
{
	if(strType=="BaseProperty")
	{
		tagConProperty *p = reinterpret_cast<tagConProperty*>(pCondition);
		for (list<tagPropertyCondition*>::iterator it=p->ltgProtertyConditon.begin();
			it!=p->ltgProtertyConditon.end();++it)
		{
			OBJ_RELEASE(tagPropertyCondition,*it);
		}
		p->ltgProtertyConditon.clear();
		OBJ_RELEASE(tagConProperty,p);
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
				OBJ_RELEASE(tagGoodAddProperty,*git);
			}
			(*it)->ltgGoodAddProperty.clear();
			OBJ_RELEASE(tagGoodCondition,*it);
		}

		p->ltgGoodCondition.clear();
		OBJ_RELEASE(tagConGood,p);
	}
	else if (strType=="Buff")
	{
		tagConBuff *p = reinterpret_cast<tagConBuff*>(pCondition);
		for (list<tagBuffCondition*>::iterator it=p->ltgBuffCondition.begin();
			it!=p->ltgBuffCondition.end();++it)
		{
			OBJ_RELEASE(tagBuffCondition,*it);
		}
		p->ltgBuffCondition.clear();
		OBJ_RELEASE(tagConBuff,p);
	}
	else if (strType=="Skill")
	{
		tagConSkill *p = reinterpret_cast<tagConSkill*>(pCondition);
		for (list<tagSkillCondition*>::iterator it=p->ltgSkillCondition.begin();
			it!=p->ltgSkillCondition.end();++it)
		{
			OBJ_RELEASE(tagSkillCondition,*it);
		}
		p->ltgSkillCondition.clear();
		OBJ_RELEASE(tagConSkill,p);
	}
	else if (strType=="Distance")
	{
		tagConDistance *p = reinterpret_cast<tagConDistance*>(pCondition);
		for (list<tagDistanceCondition*>::iterator it=p->ltgDistanceCondition.begin();
			it!=p->ltgDistanceCondition.end();++it)
		{
			OBJ_RELEASE(tagDistanceCondition,*it);
		}
		p->ltgDistanceCondition.clear();
		OBJ_RELEASE(tagConDistance,p);
	}
	else if (strType=="ValidObject")
	{
		tagConValidObject *p = reinterpret_cast<tagConValidObject*>(pCondition);
		for (list<tagValidObjectCondition*>::iterator it=p->ltgValidObjectCondition.begin();
			it!=p->ltgValidObjectCondition.end();++it)
		{
			OBJ_RELEASE(tagValidObjectCondition,*it);
		}
		p->ltgValidObjectCondition.clear();
		OBJ_RELEASE(tagConValidObject,p);
	}
	else if (strType=="Event")
	{
		tagConEvent *p = reinterpret_cast<tagConEvent*>(pCondition);
		for (list<tagEventCondition*>::iterator it=p->ltgEventCondition.begin();
			it!=p->ltgEventCondition.end();++it)
		{
			OBJ_RELEASE(tagEventCondition,*it);
		}
		p->ltgEventCondition.clear();
		OBJ_RELEASE(tagConEvent,p);
	}
	else if (strType=="Block")
	{
		tagConBlock *p = reinterpret_cast<tagConBlock*>(pCondition);
		list<tagBlockCondition*>::iterator it=p->ltgBlockCondition.begin();
		for (;it!=p->ltgBlockCondition.end();++it)
		{
			OBJ_RELEASE(tagBlockCondition,*it);
		}
		p->ltgBlockCondition.clear();
		OBJ_RELEASE(tagConBlock,p);
	}
	else if (strType=="Script")
	{
		tagConScript *p = reinterpret_cast<tagConScript*>(pCondition);
		p->strScript.clear();		
		OBJ_RELEASE(tagConScript,p);
	}
	else if (strType=="Timer")
	{
		tagConTimer *p = reinterpret_cast<tagConTimer*>(pCondition);
		OBJ_RELEASE(tagConTimer,p);
	}
	else if (strType=="Face")
	{
		tagConFace *p = reinterpret_cast<tagConFace*>(pCondition);
		for (list<tagFaceCondition*>::iterator it=p->ltgFaceCondition.begin();
			it!=p->ltgFaceCondition.end();++it)
		{
			OBJ_RELEASE(tagFaceCondition,*it);
		}
		p->ltgFaceCondition.clear();
		OBJ_RELEASE(tagConFace,p);
	}
	else if (strType=="Region")
	{
		tagConRegion *p = reinterpret_cast<tagConRegion*>(pCondition);
		list<tagRegionCondition*>::iterator it=p->ltgRegionCondition.begin();
		for (;it!=p->ltgRegionCondition.end();++it)
		{
			OBJ_RELEASE(tagRegionCondition,*it);
		}
		p->ltgRegionCondition.clear();
		OBJ_RELEASE(tagConRegion,p);		
	}


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
		OBJ_RELEASE(tagCondition,it->second);
	}
	m_mapCondition.clear();

	//条件组
	for (map<DWORD,tagConditionGroup*>::iterator it=m_mapConditionGroup.begin();
		it!=m_mapConditionGroup.end();++it)
	{
		//条件组
		OBJ_RELEASE(tagConditionGroup,it->second);
	}
	m_mapConditionGroup.clear();

	//作用对象
	for(map<DWORD,tagOperateObject*>::iterator it=m_mapOperateObject.begin();
		it!=m_mapOperateObject.end();++it)
	{
		OBJ_RELEASE(tagOperateObject,it->second);
	}
	m_mapOperateObject.clear();

	for (list<tagSkill*>::iterator it=m_lSkill.begin();
		it!=m_lSkill.end(); ++it)
	{
		if((*it)->pStaticParam)
			OBJ_RELEASE(stStaticParam,(*it)->pStaticParam);
	
		for (map<DWORD,tagCondition*>::iterator cit=(*it)->mapCondition.begin();
			cit!=(*it)->mapCondition.end();++cit)
		{	
			ClearCondition((cit->second)->pVoidCondition,(cit->second)->strType);		
			OBJ_RELEASE(tagCondition,cit->second);
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
					OBJ_RELEASE(tagDo,*SDit);
				}
				(*Sit)->ltgDo.clear();
				OBJ_RELEASE(tagStep,*Sit);

			}
			(*Pit)->ltgStep.clear();
			OBJ_RELEASE(tagProcess,*Pit);

		}
		(*it)->ltgExecuteStep.clear();
		OBJ_RELEASE(tagSkill,*it);
	}

	m_lSkill.clear();
}
//释放Do
void CNewSkill::ClearDo(tagBaseChange *ptgBaseChange)
{
	if (!stricmp(ptgBaseChange->strChangeType.c_str(),"ObjectPropertyChange"))
	{
		//对象基本属性
		tagChangeProperty *ptgChange = dynamic_cast<tagChangeProperty*>(ptgBaseChange);
		OBJ_RELEASE(tagChangeProperty,ptgChange);
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"BuffChange"))
	{
		//状态
		tagChangeBuff *ptgChange = dynamic_cast<tagChangeBuff*>(ptgBaseChange);
		OBJ_RELEASE(tagChangeBuff,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"PosChange"))
	{
		//位置
		tagChangePos *ptgChange = dynamic_cast<tagChangePos*>(ptgBaseChange);
		OBJ_RELEASE(tagChangePos,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"AttakObjectChange"))
	{
		//改变攻击对象
		tagAttackChangeObject *ptgChange = dynamic_cast<tagAttackChangeObject*>(ptgBaseChange);
		OBJ_RELEASE(tagAttackChangeObject,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"Script"))
	{			
		//脚本
		tagScriptID *ptgChange = dynamic_cast<tagScriptID*>(ptgBaseChange);
		OBJ_RELEASE(tagScriptID,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"CooldownChange"))
	{
		//冷却时间
		tagCoolDownChange *ptgChange = dynamic_cast<tagCoolDownChange*>(ptgBaseChange);
		OBJ_RELEASE(tagCoolDownChange,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DestObjsChange"))
	{
		//对象列表
		tagDestObjChange *ptgChange = dynamic_cast<tagDestObjChange*>(ptgBaseChange);
		OBJ_RELEASE(tagDestObjChange,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"RegisterTimer"))
	{
		//注册时间
		tagRegisterTimer  *ptgChange = dynamic_cast<tagRegisterTimer*>(ptgBaseChange);
		OBJ_RELEASE(tagRegisterTimer,ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"RegisterEvent"))
	{
		//注册事件
		tagRegisterEvent  *ptgChange = dynamic_cast<tagRegisterEvent*>(ptgBaseChange);
		OBJ_RELEASE(tagRegisterEvent,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"UnRegisterTimer"))
	{
		//注消时间
		tagUnRegisterTimer  *ptgChange = dynamic_cast<tagUnRegisterTimer*>(ptgBaseChange);
		OBJ_RELEASE(tagUnRegisterTimer,ptgChange);
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"UnRegisterEvent"))
	{
		//注消事件
		tagUnRegisterEvent  *ptgChange = dynamic_cast<tagUnRegisterEvent*>(ptgBaseChange);
		OBJ_RELEASE(tagUnRegisterEvent,ptgChange);		
	}

	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"SendMsg"))
	{
		//发送消息
		tagSendMsg  *ptgChange = dynamic_cast<tagSendMsg*>(ptgBaseChange);
		OBJ_RELEASE(tagSendMsg,ptgChange);		
		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DamageChange"))
	{
		//伤害计算
		tagDamageChange  *ptgChange = dynamic_cast<tagDamageChange*>(ptgBaseChange);
		OBJ_RELEASE(tagDamageChange,ptgChange);		

		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"DoDamage"))
	{
		tagDoDamage  *ptgChange = dynamic_cast<tagDoDamage*>(ptgBaseChange);
		OBJ_RELEASE(tagDoDamage,ptgChange);			
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"GoodChange"))
	{
		tagGoodChange *ptgChange = dynamic_cast<tagGoodChange*>(ptgBaseChange);
		OBJ_RELEASE(tagGoodChange,ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(), "DirecChange"))
	{
		tagDirection *ptgChange = dynamic_cast<tagDirection*>(ptgBaseChange);
		OBJ_RELEASE(tagDirection,ptgChange);		
	}
	else if (!stricmp(ptgBaseChange->strChangeType.c_str(),"SkillChange"))
	{
		tagSkillChange *ptgChange = dynamic_cast<tagSkillChange*>(ptgBaseChange);
		OBJ_RELEASE(tagSkillChange,ptgChange);
	}
	
}
//释放CDo
void	CNewSkill::ClearCDo(tagCDo* ptgCDo)
{
	for (list<tagDoProcess*>::iterator TDPit=ptgCDo->ltgTrueDoProcess.begin();
		TDPit!=ptgCDo->ltgTrueDoProcess.end();++TDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*TDPit)->ltgBaseChange.begin();
			BCit!=(*TDPit)->ltgBaseChange.end();++BCit)
		{
			ClearDo(*BCit);
		}
		(*TDPit)->ltgBaseChange.clear();	

		for (list<tagCDo*>::iterator Dit=(*TDPit)->ltgCDo.begin();
			Dit!=(*TDPit)->ltgCDo.end();++Dit)
		{
			ClearCDo((*Dit));
		}					
		(*TDPit)->ltgCDo.clear();


		OBJ_RELEASE(tagDoProcess,*TDPit);					
	}
	ptgCDo->ltgTrueDoProcess.clear();

	for (list<tagDoProcess*>::iterator FDPit=ptgCDo->ltgFalseDoProcess.begin();
		FDPit!=ptgCDo->ltgFalseDoProcess.end();++FDPit)
	{
		for (list<tagBaseChange*>::iterator BCit=(*FDPit)->ltgBaseChange.begin();
			BCit!=(*FDPit)->ltgBaseChange.end();++BCit)
		{
			ClearDo(*BCit);
		}
		(*FDPit)->ltgBaseChange.clear();

		for (list<tagCDo*>::iterator Dit=(*FDPit)->ltgCDo.begin();
			Dit!=(*FDPit)->ltgCDo.end();++Dit)
		{
			ClearCDo((*Dit));
		}					
		(*FDPit)->ltgCDo.clear();



		OBJ_RELEASE(tagDoProcess,*FDPit);	
	}
	ptgCDo->ltgFalseDoProcess.clear();
	OBJ_RELEASE(tagCDo,ptgCDo);
}