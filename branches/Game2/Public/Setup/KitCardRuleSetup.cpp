#include "StdAfx.h"
#include "KitCardRuleSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
KitCardRuleSetup::MAP_KITCARDRULE KitCardRuleSetup::m_mRules;
vector<KitCardRuleSetup::tagCardPrice> KitCardRuleSetup::m_vecCardPrice;

KitCardRuleSetup::KitCardRule::KitCardRule()
{	
	
}

KitCardRuleSetup::KitCardRule::KitCardRule(DWORD dwId,uchar btHoleNum,uchar btCardNum)
{
	tagKitCardRule.RuleId = dwId;	
	tagKitCardRule.RequireHoleNum = btHoleNum;
	tagKitCardRule.RequireCardNum = btCardNum;
	tagKitCardRule.CardIdArray.reserve( btCardNum );
}

KitCardRuleSetup::KitCardRule::~KitCardRule()
{	

}

BOOL KitCardRuleSetup::KitCardRule::Serialize(vector<uchar>* pStream, BOOL b)
{		
	_AddToByteArray(pStream,tagKitCardRule.RuleId);
	_AddToByteArray(pStream,tagKitCardRule.RequireHoleNum);	
	_AddToByteArray(pStream,tagKitCardRule.RequireCardNum);
	for(int i=0;i<tagKitCardRule.RequireCardNum;i++)
	{
		_AddToByteArray(pStream,tagKitCardRule.CardIdArray[i]);
	}	
	_AddToByteArray(pStream,(WORD)(tagKitCardRule.AttrList.size()));
	vector<tagRuleValue>::iterator tagIter=tagKitCardRule.AttrList.begin();
	for(;tagIter!=tagKitCardRule.AttrList.end();tagIter++)
	{
		_AddToByteArray(pStream,tagIter->strType.c_str());
		_AddToByteArray(pStream,tagIter->lVal);
	}
	return TRUE;
}

BOOL KitCardRuleSetup::KitCardRule::Unserialize(uchar* pStream, long& lOffset, BOOL b)
{	
	tagKitCardRule.RuleId=_GetDwordFromByteArray(pStream,lOffset);
	tagKitCardRule.RequireHoleNum=_GetByteFromByteArray(pStream,lOffset);
	tagKitCardRule.RequireCardNum=_GetByteFromByteArray(pStream,lOffset);

	tagKitCardRule.CardIdArray.clear();
	tagKitCardRule.CardIdArray.reserve( tagKitCardRule.RequireCardNum );
	for(int i=0;i<tagKitCardRule.RequireCardNum;i++)
	{
		tagKitCardRule.CardIdArray.push_back(_GetLongFromByteArray(pStream,lOffset));
	}

	tagKitCardRule.AttrList.clear();
	WORD wSize=_GetWordFromByteArray(pStream,lOffset);
	char strName[256];
	for(int i=0;i<wSize;i++)
	{
		tagRuleValue stRuleValue;
		ZeroMemory(strName,256);
		_GetStringFromByteArray(pStream, lOffset, strName);
		stRuleValue.strType=strName;
		stRuleValue.lVal=_GetLongFromByteArray(pStream,lOffset);
		tagKitCardRule.AttrList.push_back(stRuleValue);
	}
	return TRUE;
}


KitCardRuleSetup::KitCardRuleSetup()
{	
}

KitCardRuleSetup::~KitCardRuleSetup()
{	
}

VOID KitCardRuleSetup::Release()
{
	ITR_KITCARDRULE ruleIter=m_mRules.begin();
	for(;ruleIter!=m_mRules.end();ruleIter++)
	{
		SAFE_DELETE(ruleIter->second);
	}
	m_mRules.clear();
}

BOOL KitCardRuleSetup::Serialize(vector<uchar>* pStream, BOOL b)
{	
	_AddToByteArray(pStream,static_cast<DWORD>(m_vecCardPrice.size()));
	for(size_t i=0; i<m_vecCardPrice.size(); i++)
	{
		_AddToByteArray(pStream,&m_vecCardPrice[i],sizeof(tagCardPrice));
	}

	_AddToByteArray( pStream, static_cast<DWORD>( m_mRules.size() ) );
	ITR_KITCARDRULE it = m_mRules.begin();
	for( ; it != m_mRules.end(); it ++ )
	{
		_AddToByteArray( pStream, (long)(it->first) );
		it->second->Serialize( pStream );
	}
	return TRUE;	
}

BOOL KitCardRuleSetup::Unserialize(uchar* pStream, long& lOffset, BOOL b)
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
		KitCardRule* pRule=new KitCardRule();
		pRule->Unserialize(pStream,lOffset);
		m_mRules[lVal]=pRule;
	}	
	return TRUE;
}

DWORD KitCardRuleSetup::GetOpenHolePrice(DWORD dwLev)
{	
	for(size_t i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwOpenHolePrice;
	}
	return -1;
}

DWORD KitCardRuleSetup::GetEnchasePrice(DWORD dwLev)
{
	for(size_t i=0;i<m_vecCardPrice.size();i++)
	{
		if(dwLev>0 && dwLev<=m_vecCardPrice[i].dwEquipLev)
			return m_vecCardPrice[i].dwEnchasePrice;
	}
	return -1;
}

KitCardRuleSetup::MAP_KITCARDRULE* KitCardRuleSetup::GetKitCardList()
{
	return &m_mRules;
}

BOOL KitCardRuleSetup::Load(const char* strPath)
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
		KitCardRuleSetup::tagCardPrice stPrice;
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
		KitCardRule* pRule = new KitCardRule(nKitCardId,nHoleNumRequire,nCardNumRequire);
		m_mRules[nKitCardId] = pRule;
		//要求的各卡片ID
		int nId;
		//int* pCardIds=new int[nCardNumRequire];
		ReadTo(stream,"CARD_ID");
		for(int i=0;i<nCardNumRequire;i++)
		{
			stream>>nId;
			pRule->tagKitCardRule.CardIdArray.push_back(nId);
			//pCardIds[i]=nId;
		}		
		//pRule->SetCardIds(pCardIds);
		//SAFE_DELETE(pCardIds);
		//读属性修正值
		ReadTo(stream,"<ATTR>");		
		while(TRUE)
		{			
			stream>>strVal;
			if(strVal=="</ATTR>")
			{
				break;
			}			
			tagRuleValue stRuleValue;
			stRuleValue.strType=strVal;
			stream>>stRuleValue.lVal;
			pRule->tagKitCardRule.AttrList.push_back(stRuleValue);
		}
	}
	return TRUE;
}