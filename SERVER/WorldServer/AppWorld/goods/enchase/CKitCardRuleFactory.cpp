#include "stdafx.h"
#include "CKitCardRuleFactory.h"
#include "CKitCardRule.h"



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
		CKitCardRule* pRule=MP_NEW CKitCardRule();
		pRule->Unserialize(pStream,lOffset);
		m_mRules[lVal]=pRule;
	}	
	return TRUE;
}

DWORD CKitCardRuleFactory::GetOpenHolePrice(DWORD dwLev)
{	
	for(int i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwOpenHolePrice;
	}
	return -1;
}

DWORD CKitCardRuleFactory::GetEnchasePrice(DWORD dwLev)
{
	for(int i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwEnchasePrice;
	}
	return -1;
}

BOOL CKitCardRuleFactory::Load(const CHAR* strPath)
{	
	m_mRules.clear();
	CRFile* prfile=rfOpen(strPath);
	if(prfile==NULL)
	{
		return FALSE;
	}
	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);
	string strVal="";

	ReadTo(stream,"<ENCHASE_PRICE>");
	m_vecCardPrice.clear();
	while(TRUE)
	{
		stream>>strVal;
		if(strVal=="</ENCHASE_PRICE>")
		{			
			break;
		}
		CKitCardRuleFactory::tagCardPrice stPrice;
		ZeroMemory(&stPrice,sizeof(stPrice));
		stPrice.dwEquipLev=atoi(strVal.c_str());
		stream>>stPrice.dwOpenHolePrice>>stPrice.dwEnchasePrice;
		m_vecCardPrice.push_back(stPrice);
	}

	while(ReadTo(stream,"<KIT_CARD>"))
	{
		//套卡ID
	    int nKitCardId;
		ReadTo(stream,"KIT_CARD_ID");
		stream>>nKitCardId;
		//要求开孔数量
		int nHoleNumRequire;
		ReadTo(stream,"HOLE_NUM_REQUIRE");
		stream>>nHoleNumRequire;
		//要求卡片数量
		int nCardNumRequire;
		ReadTo(stream,"CARD_NUM_REQUIRE");
		stream>>nCardNumRequire;
		CKitCardRule* pRule=MP_NEW CKitCardRule(nKitCardId,nHoleNumRequire,nCardNumRequire);
		m_mRules[nKitCardId]=pRule;
		//要求的各卡片ID
		int nId;
		int* pCardIds=(int*)M_ALLOC(nCardNumRequire*sizeof(int));
		ReadTo(stream,"CARD_ID");
		for(int i=0;i<nCardNumRequire;i++)
		{
			stream>>nId;
			pCardIds[i]=nId;
		}		
		pRule->SetCardIds(pCardIds);
		M_FREE(pCardIds,nCardNumRequire*sizeof(int));
		//读属性修正值
		ReadTo(stream,"<ATTR>");		
		while(TRUE)
		{			
			stream>>strVal;
			if(strVal=="</ATTR>")
			{
				break;
			}			
			CKitCardRule::tagRuleValue stRuleValue;
			stRuleValue.strType=strVal;
			stream>>stRuleValue.lVal;
			pRule->m_vAttr.push_back(stRuleValue);
		}
	}
	return TRUE;
}