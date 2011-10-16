#include "StdAfx.h"
#include "CKitCardRuleFactory.h"
#include "../CGoods.h"

#include "../../../../../Public/Setup/GoodsSetup.h"
#include "../../../../../Public/Setup/KitCardRuleSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CKitCardRuleFactory::CKitCardRuleFactory()
{	
}

CKitCardRuleFactory::~CKitCardRuleFactory()
{	
}

//套卡效果激活
bool CKitCardRuleFactory::KitCardActive(CPlayer* pPlayer, CGoods* pGoods)
{
	//激活孔数
	ulong dwHoleNum = pGoods->GetActivedHoleNum();

	//当前卡片镶嵌数量	
	vector<ulong> vecCardIds;	
	pGoods->GetEnchasedCard(vecCardIds);

	KitCardRuleSetup::MAP_KITCARDRULE* mapRules = KitCardRuleSetup::GetKitCardList();
	KitCardRuleSetup::ITR_KITCARDRULE ruleIter = mapRules->begin();
	for(; ruleIter!=mapRules->end(); ++ruleIter)
	{
		bool bRet = RuleActive(pPlayer,pGoods,dwHoleNum,vecCardIds,ruleIter->second);
		if(bRet)
		{
			pGoods->SetActivedKitCardId(ruleIter->first);
			return true;
		}
	}
	return false;
}

bool CKitCardRuleFactory::KitCardInvalidate(CPlayer* pPlayer,CGoods* pGoods,ulong dwKitCardId)
{
	if(pGoods)
	{
		KitCardRuleSetup::MAP_KITCARDRULE* mapRules = KitCardRuleSetup::GetKitCardList();
		KitCardRuleSetup::ITR_KITCARDRULE ruleIter = mapRules->begin();
		for(; ruleIter!=mapRules->end(); ++ruleIter)
		{
			if(ruleIter->first == dwKitCardId)
			{
				RuleInvalidate(pPlayer,pGoods,ruleIter->second);
			}		
		}
	}
	return false;
}

bool CKitCardRuleFactory::RuleActive(CPlayer* pPlayer,CGoods* pGoods,ulong dwHoleNum,vector<ulong>& vecIds,
									 KitCardRuleSetup::KitCardRule* kitCard)
{
	int nSize=(int)vecIds.size();
	//要求开孔数量或要求卡片数量不匹配
	if(dwHoleNum!=kitCard->tagKitCardRule.RequireHoleNum 
		|| nSize!=kitCard->tagKitCardRule.RequireCardNum)
		return false;
	int nEqualNum=0;
	for(int i=0;i<nSize;i++)
	{
		for(int j=0;j<kitCard->tagKitCardRule.RequireCardNum;j++)
		{
			if(kitCard->tagKitCardRule.CardIdArray[j]==vecIds[i])
			{
				nEqualNum++;
				break;
			}
		}
	}
	if(nEqualNum==kitCard->tagKitCardRule.RequireCardNum)
	{
		vector<KitCardRuleSetup::tagRuleValue>::iterator ruleValueIter=kitCard->tagKitCardRule.AttrList.begin();		
		for(;ruleValueIter!=kitCard->tagKitCardRule.AttrList.end();ruleValueIter++)
		{
			ulong dwAddonEnum=GoodsSetup::GetAddonByName(ruleValueIter->strType);
			if(dwAddonEnum>0)
			{
				bool bExist=pGoods->HasAddonProperty(static_cast<eGoodsAddonProperties>(dwAddonEnum));		

				if(bExist)
				{
					//已经有该属性，直接更新
					pGoods->ChangeAttribute(dwAddonEnum,ruleValueIter->lVal,1);
				}
				else
				{
					//不存在该属性，添加			
					pGoods->SetAttribute(dwAddonEnum,ruleValueIter->lVal,1);
				}
			}
		}
		if(pPlayer)
			pGoods->UpdateAttribute(pPlayer->GetExID());
		return true;
	}
	return false;
}

bool CKitCardRuleFactory::RuleInvalidate(CPlayer* pPlayer,CGoods* pGoods,KitCardRuleSetup::KitCardRule* kitCard)
{
	if(pGoods)
	{
		vector<KitCardRuleSetup::tagRuleValue>::iterator ruleValueIter=kitCard->tagKitCardRule.AttrList.begin();		
		for(;ruleValueIter!=kitCard->tagKitCardRule.AttrList.end();ruleValueIter++)
		{				
			//装备上是否有该属性
			ulong dwAddonEnum=GoodsSetup::GetAddonByName(ruleValueIter->strType);
			if(dwAddonEnum>0)
			{
				bool bExist=pGoods->HasAddonProperty(static_cast<eGoodsAddonProperties>(dwAddonEnum));		

				if(bExist)
				{
					//已经有该属性，直接更新
					pGoods->ChangeAttribute(dwAddonEnum,-(ruleValueIter->lVal),1);
				}
				else
				{
					//不存在该属性，添加			
					pGoods->SetAttribute(dwAddonEnum,-(ruleValueIter->lVal),1);
				}
			}
		}
		if(pPlayer)
			pGoods->UpdateAttribute(pPlayer->GetExID());
	}
	return false;
}