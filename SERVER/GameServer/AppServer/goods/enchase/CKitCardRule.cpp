#include "stdafx.h"
#include "CKitCardRule.h"
#include "../cgoods.h"
#include "../cgoodsfactory.h"
#include "../../player.h"



CKitCardRule::CKitCardRule()
{	
	m_dwRuleId=0;	
	m_btRequireHoleNum=0;
	m_btRequireCardNum=0;
	m_pCardIdArray=NULL;
}

CKitCardRule::~CKitCardRule()
{	
	m_vAttr.clear();
	M_FREE(m_pCardIdArray, sizeof(LONG)*m_btRequireCardNum);
}

CKitCardRule::CKitCardRule(DWORD dwId,BYTE btHoleNum,BYTE btCardNum)
{	
	m_dwRuleId=dwId;	
	m_btRequireHoleNum=btHoleNum;
	m_btRequireCardNum=btCardNum;
	m_pCardIdArray=(LONG*)M_ALLOC(sizeof(LONG)*btCardNum);
}

BOOL CKitCardRule::Serialize(vector<BYTE>* pStream, BOOL b)
{	
	_AddToByteArray(pStream,m_dwRuleId);
	_AddToByteArray(pStream,m_btRequireHoleNum);	
	_AddToByteArray(pStream,m_btRequireCardNum);
	for(int i=0;i<m_btRequireCardNum;i++)
	{
		_AddToByteArray(pStream,m_pCardIdArray[i]);
	}	
	_AddToByteArray(pStream,(WORD)(m_vAttr.size()));
	vector<CKitCardRule::tagRuleValue>::iterator tagIter=m_vAttr.begin();
	for(;tagIter!=m_vAttr.end();tagIter++)
	{
		_AddToByteArray(pStream,tagIter->strType.c_str());
		_AddToByteArray(pStream,tagIter->lVal);
	}
	return TRUE;
}

BOOL CKitCardRule::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{	
	m_dwRuleId=_GetDwordFromByteArray(pStream,lOffset);
	m_btRequireHoleNum=_GetByteFromByteArray(pStream,lOffset);
	m_btRequireCardNum=_GetByteFromByteArray(pStream,lOffset);
	M_FREE(m_pCardIdArray, m_btRequireCardNum);
	m_pCardIdArray=(LONG*)M_ALLOC(sizeof(LONG)*m_btRequireCardNum);
	for(int i=0;i<m_btRequireCardNum;i++)
	{
		m_pCardIdArray[i]=::_GetLongFromByteArray(pStream,lOffset);
	}	
	m_vAttr.clear();
	WORD wSize=_GetWordFromByteArray(pStream,lOffset);
	char strName[256];
	for(int i=0;i<wSize;i++)
	{
		CKitCardRule::tagRuleValue stRuleValue;
		ZeroMemory(strName,256);
		_GetStringFromByteArray(pStream, lOffset, strName);
		stRuleValue.strType=strName;
		stRuleValue.lVal=_GetLongFromByteArray(pStream,lOffset);
		m_vAttr.push_back(stRuleValue);
	}
	return TRUE;
}

BOOL CKitCardRule::RuleActive(CPlayer* pPlayer,CGoods* pGoods,DWORD dwHoleNum,vector<DWORD>& vecIds)
{
	int nSize=vecIds.size();
	//要求开孔数量或要求卡片数量不匹配
	if(dwHoleNum!=m_btRequireHoleNum || nSize!=m_btRequireCardNum)
		return FALSE;
	int nEqualNum=0;
	for(int i=0;i<nSize;i++)
	{
		for(int j=0;j<m_btRequireCardNum;j++)
		{
			if(m_pCardIdArray[j]==vecIds[i])
			{
				nEqualNum++;
				break;
			}
		}
	}
	if(nEqualNum==m_btRequireCardNum)
	{
		//Actived KitCard!		
		vector<CKitCardRule::tagRuleValue>::iterator ruleValueIter=m_vAttr.begin();		
		for(;ruleValueIter!=m_vAttr.end();ruleValueIter++)
		{
			DWORD dwAddonEnum=CGoodsFactory::GetAddonByName(ruleValueIter->strType);
			if(dwAddonEnum>0)
			{
				BOOL bExist=pGoods->HasAddonProperty(static_cast<GOODS_ADDON_PROPERTIES>(dwAddonEnum));		
				
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
		return TRUE;
	}
	return FALSE;
}

BOOL CKitCardRule::RuleInvalidate(CPlayer* pPlayer,CGoods* pGoods)
{
	
	if(pGoods)
	{
		vector<CKitCardRule::tagRuleValue>::iterator ruleValueIter=m_vAttr.begin();		
		for(;ruleValueIter!=m_vAttr.end();ruleValueIter++)
		{				
			//装备上是否有该属性
			DWORD dwAddonEnum=CGoodsFactory::GetAddonByName(ruleValueIter->strType);
			if(dwAddonEnum>0)
			{
				BOOL bExist=pGoods->HasAddonProperty(static_cast<GOODS_ADDON_PROPERTIES>(dwAddonEnum));		
				
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
	return FALSE;
}