#include "stdafx.h"
#include "CSuitRule.h"


CSuitRule::CSuitRule()
{	
}

CSuitRule::~CSuitRule()
{	
	map<int,vector<tagRuleValue*>*>::iterator vecIter=m_mAttr.begin();
	for(;vecIter!=m_mAttr.end();vecIter++)
	{
		vector<tagRuleValue*>::iterator tagIter=vecIter->second->begin();
		for(;tagIter!=vecIter->second->end();tagIter++)
		{
			OBJ_RELEASE(tagRuleValue,*tagIter);
		}
		vecIter->second->clear();
		SAFE_DELETE(vecIter->second);
	}
	m_mAttr.clear();
}

BOOL CSuitRule::Serialize(vector<BYTE>* pStream, BOOL b)
{	
	_AddToByteArray( pStream, static_cast<DWORD>( m_mEquip.size() ) );
	map<int,string>::iterator it = m_mEquip.begin();
	for( ; it != m_mEquip.end(); it ++ )
	{
		_AddToByteArray( pStream, (long)(it -> first) );
		_AddToByteArray(pStream,it->second.c_str());
	}
	_AddToByteArray( pStream, static_cast<DWORD>( m_mAttr.size() ) );
	map<int,vector<tagRuleValue*>*>::iterator it2 = m_mAttr.begin();
	for( ; it2 != m_mAttr.end(); it2 ++ )
	{
		_AddToByteArray( pStream, (long)(it2 -> first) );

		_AddToByteArray(pStream,(WORD)(it2->second->size()));
		for(int i=0;i<it2->second->size();i++)
		{
			_AddToByteArray(pStream,(*it2->second)[i]->strType.c_str());
			_AddToByteArray(pStream,(*it2->second)[i]->lVal);
		}
	}
	return TRUE;
}

BOOL CSuitRule::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{	
	DWORD dwNum = _GetDwordFromByteArray( pStream, lOffset );
	char strName[256];
	ZeroMemory(strName,256);
	for( DWORD i = 0; i < dwNum; i ++ )
	{
		long lVal=_GetLongFromByteArray(pStream,lOffset);
		_GetStringFromByteArray(pStream, lOffset, strName);		
		m_mEquip[lVal]=strName;
		ZeroMemory(strName,256);
	}

	dwNum = _GetDwordFromByteArray( pStream, lOffset );
	for(DWORD i=0;i<dwNum;i++)
	{
		long lVal=_GetLongFromByteArray(pStream,lOffset);
		WORD wVal=_GetWordFromByteArray(pStream,lOffset);
		
		vector<tagRuleValue*>* pVec=new vector<tagRuleValue*>;
		for(WORD j=0;j<wVal;j++)
		{
			ZeroMemory(strName,256);
			tagRuleValue* pValue=OBJ_CREATE(tagRuleValue);
			_GetStringFromByteArray(pStream, lOffset, strName);
			pValue->strType=strName;
			pValue->lVal=_GetLongFromByteArray(pStream,lOffset);
			pVec->push_back(pValue);

		}
		m_mAttr[lVal]=pVec;

	}
	return TRUE;
}