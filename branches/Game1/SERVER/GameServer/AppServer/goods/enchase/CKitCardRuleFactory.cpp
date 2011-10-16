#include "stdafx.h"
#include "CKitCardRuleFactory.h"
#include "CKitCardRule.h"
#include "../CGoods.h"

map<int,CKitCardRule*> CKitCardRuleFactory::m_mRules;

vector<CKitCardRuleFactory::tagCardPrice> CKitCardRuleFactory::m_vecCardPrice;

CKitCardRuleFactory::CKitCardRuleFactory()
{	
}

CKitCardRuleFactory::~CKitCardRuleFactory()
{	
}

VOID CKitCardRuleFactory::Release()
{
	map<int,CKitCardRule*>::iterator ruleIter=m_mRules.begin();
	for(;ruleIter!=m_mRules.end();ruleIter++)
	{
		MP_DELETE(ruleIter->second);
	}
	m_mRules.clear();
}

DWORD CKitCardRuleFactory::GetOpenHolePrice(DWORD dwLev)
{	
	for(int i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>=0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwOpenHolePrice;
	}
	return 0;
}

DWORD CKitCardRuleFactory::GetEnchasePrice(DWORD dwLev)
{
	for(int i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>=0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwEnchasePrice;
	}
	return 0;
}

BOOL CKitCardRuleFactory::Serialize(vector<BYTE>* pStream, BOOL b)
{	
	_AddToByteArray(pStream,static_cast<DWORD>(m_vecCardPrice.size()));
	for(int i=0;i<m_vecCardPrice.size();i++)
	{
		_AddToByteArray(pStream,&m_vecCardPrice[i],sizeof(tagCardPrice));
	}

	_AddToByteArray( pStream, static_cast<DWORD>( m_mRules.size() ) );
	map<int,CKitCardRule*>::iterator it = m_mRules.begin();
	for( ; it != m_mRules.end(); it ++ )
	{
		_AddToByteArray( pStream, (long)(it -> first) );
		it -> second -> Serialize( pStream );
	}
	return TRUE;	
}

BOOL CKitCardRuleFactory::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	m_vecCardPrice.clear();
	DWORD dwNum=0;
	dwNum=_GetDwordFromByteArray(pStream,lOffset);
	for(DWORD i=0;i<dwNum;i++)
	{
		tagCardPrice stPrice;
		_GetBufferFromByteArray(pStream,lOffset,&stPrice,sizeof(stPrice));
		m_vecCardPrice.push_back(stPrice);
	}

	dwNum = _GetDwordFromByteArray( pStream, lOffset );
	for( DWORD i = 0; i < dwNum; i ++ )
	{
		long lVal=_GetLongFromByteArray(pStream,lOffset);
		CKitCardRule* pRule=MP_NEW CKitCardRule;
		pRule->Unserialize(pStream,lOffset);
		m_mRules[lVal]=pRule;
	}	
	return TRUE;
}

BOOL CKitCardRuleFactory::Load(const CHAR* strPath)
{	
	return TRUE;
}

//套卡效果激活
BOOL CKitCardRuleFactory::KitCardActive(CPlayer* pPlayer,CGoods* pGoods)
{
	//激活孔数
	DWORD dwHoleNum=pGoods->GetActivedHoleNum();
	//当前卡片镶嵌数量	
	vector<DWORD> vecCardIds;	
	pGoods->GetEnchasedCard(vecCardIds);
	
	map<int,CKitCardRule*>::iterator ruleIter=m_mRules.begin();
	for(;ruleIter!=m_mRules.end();ruleIter++)
	{
		BOOL bRet=ruleIter->second->RuleActive(pPlayer,pGoods,dwHoleNum,vecCardIds);
		if(bRet)
		{
			pGoods->SetActivedKitCardId(ruleIter->first);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CKitCardRuleFactory::KitCardInvalidate(CPlayer* pPlayer,CGoods* pGoods,DWORD dwKitCardId)
{
	if(pGoods)
	{
		map<int,CKitCardRule*>::iterator ruleIter=m_mRules.begin();
		for(;ruleIter!=m_mRules.end();ruleIter++)
		{
			if(ruleIter->first==dwKitCardId)
			{
				ruleIter->second->RuleInvalidate(pPlayer,pGoods);
			}		
		}
	}
	return FALSE;
}
