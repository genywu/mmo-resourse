#include "StdAfx.h"
#include "SuitRuleSetup.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
SuitRuleSetup::MAP_SUITRULE SuitRuleSetup::m_mRules;


/// 套装属性构造
SuitRuleSetup::SuitRule::SuitRule()
{

}

/// 套装属性析构
SuitRuleSetup::SuitRule::~SuitRule()
{
	SuitRule::Release();
}

/// 套装属性清理
void SuitRuleSetup::SuitRule::Release()
{
	map<int,vector<tagRuleValue*>*>::iterator vecIter =
		SuitRule::mapAttr.begin();
	for(; vecIter != SuitRule::mapAttr.end(); ++vecIter)
	{
		vector<tagRuleValue*>::iterator tagIter = 
			vecIter->second->begin();
		for(;tagIter !=vecIter->second->end(); ++tagIter)
		{
			SAFE_DELETE(*tagIter);
		}
		vecIter->second->clear();
		SAFE_DELETE(vecIter->second);
	}
	SuitRule::mapAttr.clear();
	SuitRule::mapEquip.clear();
}

/// 套装属性编码
BOOL SuitRuleSetup::SuitRule::Serialize(vector<uchar>* pStream, BOOL b)
{
	_AddToByteArray(pStream, static_cast<DWORD>(SuitRule::mapEquip.size()));
	map<int,string>::iterator it = SuitRule::mapEquip.begin();
	for(; it!=SuitRule::mapEquip.end(); ++it)
	{
		_AddToByteArray(pStream,(long)(it->first));
		_AddToByteArray(pStream,it->second.c_str());
	}

	_AddToByteArray(pStream, static_cast<DWORD>( SuitRule::mapAttr.size()));
	map<int,vector<tagRuleValue*>*>::iterator it2 = 
		SuitRule::mapAttr.begin();
	for(; it2!=SuitRule::mapAttr.end(); ++it2)
	{
		_AddToByteArray(pStream,(long)(it2->first));

		_AddToByteArray(pStream,(WORD)(it2->second->size()));
		for(size_t i=0;i<it2->second->size();i++)
		{
			_AddToByteArray(pStream,(*it2->second)[i]->strType.c_str());
			_AddToByteArray(pStream,(*it2->second)[i]->lVal);
		}
	}
	return TRUE;
}

/// 套装属性解码
BOOL SuitRuleSetup::SuitRule::Unserialize(uchar* pStream, long& lOffset, BOOL b)
{
	SuitRule::Release();

	//m_mEquip
	DWORD dwNum = _GetDwordFromByteArray(pStream, lOffset);
	char strName[256];
	ZeroMemory(strName,256);
	for(DWORD i = 0; i<dwNum; i++)
	{
		long lVal = _GetLongFromByteArray(pStream, lOffset);
		_GetStringFromByteArray(pStream, lOffset, strName);		
		SuitRule::mapEquip[lVal] = strName;
		ZeroMemory(strName, 256);
	}

	dwNum = _GetDwordFromByteArray(pStream, lOffset);
	for(DWORD i=0; i<dwNum; i++)
	{
		long lVal = _GetLongFromByteArray(pStream, lOffset);
		WORD wVal = _GetWordFromByteArray(pStream, lOffset);
		vector<tagRuleValue*>* pVec = new vector<tagRuleValue*>;
		for(WORD j=0; j<wVal; j++)
		{
			ZeroMemory(strName, 256);
			tagRuleValue* pValue = new tagRuleValue;
			_GetStringFromByteArray(pStream, lOffset, strName);
			pValue->strType = strName;
			pValue->lVal = _GetLongFromByteArray(pStream, lOffset);
			pVec->push_back(pValue);

		}
		SuitRule::mapAttr[lVal] = pVec;
	}
	return TRUE;
}


/// 构造
SuitRuleSetup::SuitRuleSetup()
{

}

/// 析构
SuitRuleSetup::~SuitRuleSetup()
{
	Release();
}

/// 清理
void SuitRuleSetup::Release()
{
	map<int,SuitRule*>::iterator ruleIter = m_mRules.begin();
	for(; ruleIter!=m_mRules.end(); ++ruleIter)
	{
		SAFE_DELETE(ruleIter->second);
	}
	m_mRules.clear();
}

/// 编码
BOOL SuitRuleSetup::Serialize(vector<uchar>* pStream, BOOL b)
{
	_AddToByteArray(pStream, static_cast<DWORD>(m_mRules.size()));
	map<int,SuitRule*>::iterator it = m_mRules.begin();
	for(; it!=m_mRules.end(); ++it)
	{
		_AddToByteArray(pStream, (long)(it->first));
		it->second->Serialize(pStream);
	}
	return TRUE;
}

/// 解码
BOOL SuitRuleSetup::Unserialize(uchar* pStream, long& lOffset, BOOL b)
{
	DWORD dwNum = _GetDwordFromByteArray(pStream, lOffset);
	for( DWORD i=0; i<dwNum; i++)
	{
		long lVal = _GetLongFromByteArray(pStream, lOffset);
		SuitRule* pRule = new SuitRule();
		pRule->Unserialize(pStream, lOffset);
		m_mRules[lVal] = pRule;
	}
	return TRUE;
}

/// 加载套装配置
BOOL SuitRuleSetup::Load(const char* strPath)
{
	m_mRules.clear();

	CRFile* prfile = rfOpen(strPath);
	if(prfile == NULL)
	{		
		return FALSE;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);	
	string strVal = "";	
	while(ReadTo(stream, "SUIT_ID"))
	{
		int suit_id;
		stream >> suit_id;
		ReadTo(stream, "<SUIT_GOODS>");

		SuitRule* pRule = new SuitRule();
		m_mRules[suit_id] = pRule;
		while(TRUE)
		{
			stream >> strVal;
			if(strVal == "</SUIT_GOODS>")
			{
				break;
			}
			int goods_id = atoi(strVal.c_str());
			stream >> strVal;//原始名
			pRule->mapEquip[goods_id] = strVal;
		}
		bool bLoop = TRUE;
		while(bLoop)
		{
			ReadTo(stream, "NUM");
			int num;
			stream >> num;//件数
			pRule->mapAttr[num] = new vector<tagRuleValue*>;
			ReadTo(stream, "<ATTR>");
			while(TRUE)
			{
				stream >> strVal;
				if(strVal == "</ATTR>")
				{
					break;
				}
				else if(strVal == "</ATTR_END>")
				{
					bLoop = FALSE;
					break;
				}
				tagRuleValue* pRuleValue = new tagRuleValue;
				pRuleValue->strType = strVal;
				stream >> pRuleValue->lVal;
				pRule->mapAttr[num]->push_back(pRuleValue);
			}
		}		
	}	
	return TRUE;
}