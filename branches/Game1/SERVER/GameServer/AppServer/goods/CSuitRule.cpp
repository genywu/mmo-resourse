#include "stdafx.h"
#include "CSuitRule.h"
#include "..\player.h"
#include "..\script\script.h"


CSuitRule::CSuitRule()
{
	// TODO: Add your specialized code here.
}

CSuitRule::~CSuitRule()
{
	// TODO: Add your specialized code here.
	map<int,vector<tagRuleValue*>*>::iterator vecIter=m_mAttr.begin();
	for(;vecIter!=m_mAttr.end();vecIter++)
	{
		vector<tagRuleValue*>::iterator tagIter=vecIter->second->begin();
		for(;tagIter!=vecIter->second->end();tagIter++)
		{
			SAFE_DELETE(*tagIter);
		}
		vecIter->second->clear();
		SAFE_DELETE(vecIter->second);
	}
	m_mAttr.clear();
}

BOOL CSuitRule::RuleActive(CPlayer* pPlayer,int suit_num)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	map<int,vector<CSuitRule::tagRuleValue*>*>::iterator attrIter=m_mAttr.find(suit_num);
	if(attrIter!=m_mAttr.end())
	{
		

		CSuitRule::tagRuleValue* pValue=NULL;
		for(int i=0;i<attrIter->second->size();i++)
		{
			pValue=(*(attrIter->second))[i];
			if(pValue)
			{
				if(pValue->strType=="GAP_ADD_SKILL")
				{
					//Ìí¼Ó¼¼ÄÜ
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

BOOL CSuitRule::RuleInvalidate(CPlayer* pPlayer,int suit_num)
{
	BOOL bRet=FALSE;
	if(!pPlayer)
		return bRet;
	map<int,vector<CSuitRule::tagRuleValue*>*>::iterator attrIter=m_mAttr.find(suit_num);
	if(attrIter!=m_mAttr.end())
	{
		CSuitRule::tagRuleValue* pValue=NULL;
		for(int i=0;i<attrIter->second->size();i++)
		{
			pValue=(*(attrIter->second))[i];
			pPlayer->ChangeAttribute(pValue->strType,-pValue->lVal);
		}
		bRet=TRUE;
	}
	return bRet;
}


BOOL CSuitRule::Serialize(vector<BYTE>* pStream, BOOL b)
{
	//m_mEquip
	_AddToByteArray( pStream, static_cast<DWORD>( m_mEquip.size() ) );
	map<int,string>::iterator it = m_mEquip.begin();
	for( ; it != m_mEquip.end(); it ++ )
	{
		_AddToByteArray( pStream, (long)(it -> first) );
		_AddToByteArray(pStream,it->second.c_str());
	}

	//map<int ,vector<tagRuleValue*>*> m_mAttr;
	/*
	struct tagRuleValue
	{
		string strType;
		long  lVal;
	};
	*/
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
	//m_mEquip
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
		//map<int ,vector<tagRuleValue*>*> m_mAttr;
		vector<tagRuleValue*>* pVec=new vector<tagRuleValue*>;
		for(WORD j=0;j<wVal;j++)
		{
			ZeroMemory(strName,256);
			tagRuleValue* pValue=MP_NEW tagRuleValue;
			_GetStringFromByteArray(pStream, lOffset, strName);
			pValue->strType=strName;
			pValue->lVal=_GetLongFromByteArray(pStream,lOffset);
			pVec->push_back(pValue);

		}
		m_mAttr[lVal]=pVec;

	}
	return TRUE;
}