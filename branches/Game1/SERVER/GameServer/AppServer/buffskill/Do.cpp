#include "StdAfx.h"
#include "Do.h"
#include "CondiGroup.h"
#include "EffectObjects.h"
#include "Operator.h"
#include "..\MoveShape.h"


CDo::CDo(stModuParam *pParam)
{
}
CDo::~CDo(void)
{	
	MP_DELETE(m_pEffObjects);
	itGroup it=m_Groups.begin();
	for (;it!=m_Groups.end();++it)
	{
		tagGroup *group = (*it);
		ClearGroup(group);		
	}
	m_Groups.clear();

}
//释放
void  CDo::ClearGroup(tagGroup *group)
{
		for(vector<COperator*>::iterator OTit=group->pTrueOperators.begin();
			OTit!=group->pTrueOperators.end();++OTit)
		{
			MP_DELETE(*OTit);
		}
		group->pTrueOperators.clear();
		for(vector<COperator*>::iterator OTit=group->pFalseOperators.begin();
			OTit!=group->pFalseOperators.end();++OTit)
		{
			MP_DELETE(*OTit);
		}
		group->pFalseOperators.clear();	

		if (group->pFalseGroup)
		{
			ClearGroup(group->pFalseGroup);
		}
		if (group->pTrueGroup)
		{
			ClearGroup(group->pTrueGroup);
		}
		MP_DELETE(group->pCondiGroup);
		MP_DELETE(group);
}


//对作用对象作操作
void CDo::DoOperator(list<CMoveShape*>& listObjs,vector<COperator*>& Operators,stModuParam *pParam)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()1 begin");
#endif
	for(itObj it = listObjs.begin();it!=listObjs.end();it++)
	{
		itOper itOper = Operators.begin();
		for(;itOper != Operators.end(); itOper++)
		{
			(*itOper)->SetParam(pParam);
			(*itOper)->Execute((*it));
		}
	}
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()1 end");
#endif
}

void CDo::SendSKillFailMsg(int nValue,stModuParam *pParam)
{
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
	msg.Add( pParam->pUser -> GetExID() );
	//id和等级
	msg.Add((long)pParam->nID);
	msg.Add((BYTE)pParam->nLvl );
	//全局id
	msg.Add(pParam->dwGlobleID);
	//错误类型
	msg.Add((BYTE)nValue);
	msg.SendToPlayer(pParam->pUser -> GetExID(),false);

}

void CDo::DoOperator(CMoveShape* pObj,vector<COperator*>& Operators,stModuParam *pParam)
{
#ifdef _RUNSTACKINFO1_
	char szInfo[128];
	_snprintf(szInfo, 128, "ID:%d ,CDo::DoOperator()2 begin",pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);
#endif
	itOper itOper = Operators.begin();
	for(;itOper != Operators.end(); itOper++)
	{
		(*itOper)->SetParam(pParam);
		(*itOper)->Execute(pObj);
	}
#ifdef _RUNSTACKINFO1_
	_snprintf(szInfo, 128, "ID:%d, CDo::DoOperator()2 end",pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);
#endif
}

//执行
eModuleRet CDo::Execute(stModuParam *pParam)
{
	
#ifdef _RUNSTACKINFO1_
	char szInfo[128];
	_snprintf(szInfo, 128, "ID:%d ,CDo::Execute step 1",pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);
#endif
	eModuleRet eRet = MR_Modu_End;


	if (m_Ret!=MR_No)
	{
		eRet = m_Ret;
	}

	if(NULL==m_pEffObjects)
	{	
		SendSKillFailMsg(SKILL_USE_FAILED_INVALID_UNKNOWN,pParam);
#ifdef _RUNSTACKINFO1_
		char szInfo[128];
		_snprintf(szInfo, 128, "ID:%d ,CDo::Execute step end 1",pParam->nID);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);	
#endif
		return eRet;
	}
	//得到作用对象
	typedef list<CMoveShape*>::iterator itShape;
	list<CMoveShape*> listObjs;

	listObjs.clear();	
	m_pEffObjects->SetParam(pParam);
	unsigned int nNum = m_pEffObjects->GetEffectObjs(listObjs);
	if(nNum <= 0 )	
	{

		if (pParam->eTagTarget==eYesTarget)
		{
			SendSKillFailMsg(SKILL_USE_FAILED_NOOBJECT,pParam);
		}
#ifdef _RUNSTACKINFO1_
		char szInfo[128];
		_snprintf(szInfo, 128, "ID:%d ,CDo::Execute step end 2",pParam->nID);
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);			
#endif
		return eRet;	
	}

#ifdef _RUNSTACKINFO1_
	_snprintf(szInfo, 128, "ID:%d ,CDo::Execute step 2",pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);	
#endif
	itShape itS = listObjs.begin();
	for(;itS!=listObjs.end();itS++)
	{
		itGroup it = m_Groups.begin();
		for(;it != m_Groups.end();it++)
		{
			tagGroup  *group = (*it);
			DoTrueOrFalseOperator(eRet,group,(*itS),pParam);
			
			if(eRet != MR_Continue)
				break;
			
		}
	}

#ifdef _RUNSTACKINFO1_
	_snprintf(szInfo, 128, "ID:%d ,CDo::Execute step end 3",pParam->nID);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), szInfo);	
#endif
	return eRet; 
}

void  CDo::DoTrueOrFalseOperator(eModuleRet &eRet,tagGroup *ptgGroup,CMoveShape *pMoveShape,stModuParam *pParam)
{
	bool bRet = false;
	if (ptgGroup->pCondiGroup)
	{
		
		bRet = ptgGroup->pCondiGroup->Result(pMoveShape,pParam);  
		if(!bRet)
		{
			DoOperator(pMoveShape,ptgGroup->pFalseOperators,pParam);
			eRet = ptgGroup->Ret.m_efalseRet;
			if (ptgGroup->pFalseGroup)
			{
				DoTrueOrFalseOperator(eRet,ptgGroup->pFalseGroup,pMoveShape,pParam);
			}
		}
		else
		{

			DoOperator(pMoveShape,ptgGroup->pTrueOperators,pParam);
			eRet = ptgGroup->Ret.m_etrueRet;
			if (ptgGroup->pTrueGroup)
			{
				DoTrueOrFalseOperator(eRet,ptgGroup->pTrueGroup,pMoveShape,pParam);
			}
		}
	}
	else
	{
		SendSKillFailMsg(SKILL_USE_FAILED_INVALID_UNKNOWN,pParam);
	}
}