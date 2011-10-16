#include "stdafx.h"
#include "CKitCardRule.h"


CKitCardRule::CKitCardRule()
{	
	m_dwRuleId=0;	
	m_btRequireHoleNum=0;
	m_btRequireCardNum=0;
	m_pCardIdArray=NULL;
}

CKitCardRule::CKitCardRule(DWORD dwId,BYTE btHoleNum,BYTE btCardNum)
{	
	m_dwRuleId=dwId;	
	m_btRequireHoleNum=btHoleNum;
	m_btRequireCardNum=btCardNum;
	m_pCardIdArray=(LONG*)M_ALLOC(btCardNum*sizeof(long));
}

CKitCardRule::~CKitCardRule()
{	
	m_vAttr.clear();
	M_FREE(m_pCardIdArray,m_btRequireCardNum*sizeof(long));
}

VOID CKitCardRule::SetCardIds(int* pIds)
{
	if(m_pCardIdArray)
	{
		for(int i=0;i<m_btRequireCardNum;i++)
		{
			m_pCardIdArray[i]=pIds[i];
		}
	}
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
	M_FREE(m_pCardIdArray,m_btRequireCardNum*sizeof(long));
	m_pCardIdArray=(LONG*)M_ALLOC(m_btRequireCardNum*sizeof(LONG));
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