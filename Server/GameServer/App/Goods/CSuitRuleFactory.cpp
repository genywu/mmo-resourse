#include "StdAfx.h"
#include "CSuitRuleFactory.h"
#include "CGoods.h"
#include "../Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CSuitRuleFactory::CSuitRuleFactory()
{
	// TODO: Add your specialized code here.
}

CSuitRuleFactory::~CSuitRuleFactory()
{
	// TODO: Add your specialized code here.
}


BOOL CSuitRuleFactory::RuleActive(CPlayer* pPlayer,int suit_num,SuitRuleSetup::SuitRule* Suit)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	map<int,vector<SuitRuleSetup::tagRuleValue*>*>::iterator attrIter =
		Suit->mapAttr.find(suit_num);
	if(attrIter != Suit->mapAttr.end())
	{
		SuitRuleSetup::tagRuleValue* pValue=NULL;
		for(int i=0;i<(int)attrIter->second->size();i++)
		{
			pValue=(*(attrIter->second))[i];
			if(pValue)
			{
				if(pValue->strType=="GAP_ADD_SKILL")
				{
					//添加技能
				}
				else
				{
					pPlayer->ChangeAttribute(pValue->strType,pValue->lVal);
				}				
			}

		}
		bRet=TRUE;		
	}
	return bRet;
}

BOOL CSuitRuleFactory::RuleInvalidate(CPlayer* pPlayer,int suit_num,SuitRuleSetup::SuitRule* Suit)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	map<int,vector<SuitRuleSetup::tagRuleValue*>*>::iterator attrIter = Suit->mapAttr.find(suit_num);
	if(attrIter != Suit->mapAttr.end())
	{
		SuitRuleSetup::tagRuleValue* pValue=NULL;
		for(int i=0;i<(int)attrIter->second->size();i++)
		{
			pValue=(*(attrIter->second))[i];
			pPlayer->ChangeAttribute(pValue->strType,-pValue->lVal);
		}
		bRet=TRUE;
	}
	return bRet;
}


//套装效果激活
BOOL CSuitRuleFactory::SuitActive(CPlayer* pPlayer,int nNum,CGoods* pGoods)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	LONG lSuitId=pGoods->GetAddonPropertyValues(GAP_SUITID,1);
	LONG lSuitNumer=pGoods->GetAddonPropertyValues(GAP_SUIT_NUMBER,1);
	if(lSuitId==0)
		return bRet;

	SuitRuleSetup::MAP_SUITRULE* mapRules = SuitRuleSetup::GetSuitRules();
	SuitRuleSetup::ITR_SUITRULE ruleIter = mapRules->find(lSuitId);
	if(ruleIter != mapRules->end())
	{	
		RuleInvalidate(pPlayer, nNum, ruleIter->second);
		nNum++;
		bRet = RuleActive(pPlayer,nNum, ruleIter->second);
		if(bRet)
		{/*
			CMessage msg(MSG_S2C_SUIT_ACTIVE);
			msg.Add(lSuitId);
			msg.Add((BYTE)nNum);
			msg.SendToPlayer(pPlayer->GetID());*/
		}
		
		//attr update
	}
	return bRet;	
}


//套装效果取消
BOOL CSuitRuleFactory::SuitInvalidate(CPlayer* pPlayer,int nNum,CGoods* pGoods)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	LONG lSuitId=pGoods->GetAddonPropertyValues(GAP_SUITID,1);
	LONG lSuitNumer=pGoods->GetAddonPropertyValues(GAP_SUIT_NUMBER,1);
	if(lSuitId==0)
		return bRet;

	SuitRuleSetup::MAP_SUITRULE* mapRules = SuitRuleSetup::GetSuitRules();
	SuitRuleSetup::ITR_SUITRULE ruleIter = mapRules->find(lSuitId);
	{
		bRet = RuleInvalidate(pPlayer,nNum,ruleIter->second);
		nNum--;
		BOOL bActived = RuleActive(pPlayer,nNum,ruleIter->second);
		if(bActived)
		{/*
			CMessage msg(MSG_S2C_SUIT_ACTIVE);
			msg.Add(lSuitId);
			msg.Add((BYTE)nNum);
		    msg.SendToPlayer(pPlayer->GetID());*/
		}
		
		//attr update
	}
	return bRet;
}
