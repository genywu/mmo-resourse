#include "StdAfx.h"
#include "Do.h"
#include "CondiGroup.h"
#include "EffectObjects.h"
#include "Operator.h"
#include "../MoveShape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CDo::CDo(stModuParam *pParam)
:m_pParam(pParam)
{
}
CDo::CDo(void *pParam)
:m_pParam((stModuParam*)pParam)
{

}
CDo::~CDo(void)
{	
	OBJ_RELEASE(CEffectObjects, m_pEffObjects);
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
			OBJ_RELEASE(COperator, *OTit);
		}
		group->pTrueOperators.clear();
		for(vector<COperator*>::iterator OTit=group->pFalseOperators.begin();
			OTit!=group->pFalseOperators.end();++OTit)
		{
			OBJ_RELEASE(COperator, *OTit);
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
		OBJ_RELEASE(CCondiGroup, group->pCondiGroup);
		OBJ_RELEASE(tagGroup, group);
}


//对作用对象作操作
void CDo::DoOperator(list<CMoveShape*>& listObjs,vector<COperator*>& Operators)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()1 begin");
#endif
	for(itObj it = listObjs.begin();it!=listObjs.end();it++)
	{
		itOper itOper = Operators.begin();
		for(;itOper != Operators.end(); itOper++)
		{
			(*itOper)->SetParam(m_pParam);
			(*itOper)->Execute((*it));
		}
	}
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()1 end");
#endif
}

void CDo::SendSKillFailMsg(int nValue)
{
	CMessage msg(MSG_S2C_SKILL_USE);
	msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
	msg.Add( m_pParam->pUser -> GetExID() );
	//id和等级
	msg.Add((long)m_pParam->nID);
	msg.Add((BYTE)m_pParam->nLvl );
	//全局id
	msg.Add(m_pParam->dwGlobleID);
	//错误类型
	msg.Add((BYTE)nValue);
	msg.SendToPlayer(m_pParam->pUser -> GetExID(),false);

}

void CDo::DoOperator(CMoveShape* pObj,vector<COperator*>& Operators)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()2 begin");
#endif
	itOper itOper = Operators.begin();
	for(;itOper != Operators.end(); itOper++)
	{
		(*itOper)->SetParam(m_pParam);
		(*itOper)->Execute(pObj);
	}
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::DoOperator()2 end");
#endif
}

void   CDo::AnnexGeniusStates(stModuParam *pParam)
{
#ifdef _RUNSTACKINFO1_
    CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step 1");
#endif
    m_pParam = pParam;

    if(NULL==m_pEffObjects)
    {	
#ifdef _RUNSTACKINFO1_
        CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step end 1");
#endif
        return ;
    }
    //得到作用对象
    typedef list<CMoveShape*>::iterator itShape;
    list<CMoveShape*> listObjs;

    listObjs.clear();	
    m_pEffObjects->SetParam(m_pParam);
    unsigned int nNum = m_pEffObjects->GetEffectObjs(listObjs);
    if(nNum <= 0 )	
    {
#ifdef _SKILL_INFO_D_      
        Log4c::Trace(SKILL_MODULE, "AnnexGeniusStates:天赋附加状态作用对象EffObjects <= 0 \n");    
#endif
        return ;
    }
    itShape itS = listObjs.begin();
    for(;itS!=listObjs.end();itS++)
    {
        if( !(*itS) && m_pParam->SendGeniusList.find((*itS)->GetExID()) != m_pParam->SendGeniusList.end())
            return ;
        m_pParam->pFirer->AddGeniusStates( m_pParam->nID , *itS , 0 );
        m_pParam->SendGeniusList.insert( (*itS)->GetExID() );
    }
}

//执行
eModuleRet CDo::Execute(stModuParam *pParam)
{
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step 1");
#endif
	eModuleRet eRet = MR_Modu_End;

	m_pParam = pParam;
	if (m_Ret!=MR_No)
	{
		eRet = m_Ret;
	}


	if(NULL==m_pEffObjects)
	{	
		SendSKillFailMsg(SKILL_USE_FAILED_INVALID_UNKNOWN);
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step end 1");
#endif
		return eRet;
	}
	//得到作用对象
	typedef list<CMoveShape*>::iterator itShape;
	list<CMoveShape*> listObjs;

	listObjs.clear();	
	m_pEffObjects->SetParam(m_pParam);
	unsigned int nNum = m_pEffObjects->GetEffectObjs(listObjs);
	if(nNum <= 0 )	
	{

		if (m_pParam->eTagTarget==eYesTarget)
		{
			SendSKillFailMsg(SKILL_USE_FAILED_NOOBJECT);
		}
#ifdef _RUNSTACKINFO1_
		CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step end 2");
#endif
		return eRet;	
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step 2");
#endif
	itShape itS = listObjs.begin();
	for(;itS!=listObjs.end();itS++)
	{
		itGroup it = m_Groups.begin();
		for(;it != m_Groups.end();it++)
		{
			tagGroup  *group = (*it);
			DoTrueOrFalseOperator(eRet,group,(*itS));

			if(eRet != MR_Continue)
				break;

		}
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(), "CDo::Execute step end 3");
#endif
	return eRet; 
}

void  CDo::DoTrueOrFalseOperator(eModuleRet &eRet,tagGroup *ptgGroup,CMoveShape *pMoveShape)
{
	bool bRet = false;
	if (ptgGroup->pCondiGroup)
	{
		
		bRet = ptgGroup->pCondiGroup->Result(pMoveShape,m_pParam);  
		if(!bRet)
		{
			DoOperator(pMoveShape,ptgGroup->pFalseOperators);
			eRet = ptgGroup->Ret.m_efalseRet;
			if (ptgGroup->pFalseGroup)
			{
				DoTrueOrFalseOperator(eRet,ptgGroup->pFalseGroup,pMoveShape);
			}
		}
		else
		{

			DoOperator(pMoveShape,ptgGroup->pTrueOperators);
			eRet = ptgGroup->Ret.m_etrueRet;
			if (ptgGroup->pTrueGroup)
			{
				DoTrueOrFalseOperator(eRet,ptgGroup->pTrueGroup,pMoveShape);
			}
		}
	}
	else
	{
		SendSKillFailMsg(SKILL_USE_FAILED_INVALID_UNKNOWN);
	}
}