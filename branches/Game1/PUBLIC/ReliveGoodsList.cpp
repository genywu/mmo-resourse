/*
*	file:		ReliveGoodsList.cpp
*	Brief:		复活物品列表类
*	detail:		
*	Authtor:	陈先进
*	Datae:		2009-08-27
*/
#include "StdAfx.h"
#include "ReliveGoodsList.h"
#include "StrUnifyInput.h"
#include "public.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

list<CReLiveGoodsList::tagReLiveGoods*>  CReLiveGoodsList::m_lReLiveGoods;
map<string,CReLiveGoodsList::tagExpGoods*>  CReLiveGoodsList::m_mpExpGoods;

CReLiveGoodsList::CReLiveGoodsList()
{

}


CReLiveGoodsList::~CReLiveGoodsList()
{

}

// 解析XML中关于复活物品的配置
bool CReLiveGoodsList::Init(const char *filename)
{
	if (NULL==filename)
	{
		return false;
	}
	char strError[128];
	//=================================================================//
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		_snprintf(strError, 128, "LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument m_Tdoc(filename);
	//载入复活物品配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		_snprintf(strError, 128, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_FOURTH),filename);
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("Goods");
	if (pNode==NULL)
	{
		string strNode = "Goods";
		_snprintf(strError, 128, CStringReading::LoadString(IDS_WS_SKILL,STR_WS_SKILL_FIFTH),filename,strNode.c_str());
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"ReLiveGoods"))
		{
			tagReLiveGoods *ptgReliveGoods  = new tagReLiveGoods;
			ptgReliveGoods->strOrignalName  = pChildElem->Attribute("OrignalName");
			ptgReliveGoods->dwIndex			= (DWORD)atol(pChildElem->Attribute("Index"));
			ptgReliveGoods->dwNum			= (DWORD)atol( pChildElem->Attribute("GoodsNum"));
			ptgReliveGoods->dwRecoverRatio  = (DWORD)atol(pChildElem->Attribute("RecoverRatio"));
			m_lReLiveGoods.push_back(ptgReliveGoods);		
		}
		else if(!stricmp(pChildElem->Value(),"ExpGoods"))
		{
			tagExpGoods *ptgExpGoods = new tagExpGoods;
			string strOrgName		 = pChildElem->Attribute("OrignalName");
			ptgExpGoods->dwExpRatio  = atoi(pChildElem->Attribute("ExpRatio"));			
			if (pChildElem->Attribute("Script"))
			{
				ptgExpGoods->strScript   = pChildElem->Attribute("Script");
			}
			if (pChildElem->Attribute("Hint"))
			{
				ptgExpGoods->strHint	 = pChildElem->Attribute("Hint");
			}
			
			
			map<string,tagExpGoods*>::iterator it = m_mpExpGoods.find(strOrgName);
			if (it!=m_mpExpGoods.end())
			{
				SAFE_DELETE(ptgExpGoods);
			}
			else
			{
				m_mpExpGoods[strOrgName] = ptgExpGoods;
			}
			
		}
	}
	return true;
}

// 释放资源	
void CReLiveGoodsList::Release()
{
	list<tagReLiveGoods*>::iterator it = m_lReLiveGoods.begin();
	for (;it!=m_lReLiveGoods.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_lReLiveGoods.clear();

	Eit eit = m_mpExpGoods.begin();
	for (; eit!=m_mpExpGoods.end(); ++eit)
	{
		SAFE_DELETE((*eit).second);
	}
	m_mpExpGoods.clear();
}
// 添加到CByteArray
bool CReLiveGoodsList::AddToByteArray(vector<BYTE> &pByteArray)
{
	//复活物品个数		
	_AddToByteArray(&pByteArray,static_cast<long>(m_lReLiveGoods.size()));

	for (list<tagReLiveGoods *>::iterator it = m_lReLiveGoods.begin();it!=m_lReLiveGoods.end();++it)
	{
		// 原始名
		_AddToByteArray(&pByteArray,(char*)((*it)->strOrignalName.c_str()));			
		//索引
		_AddToByteArray(&pByteArray,(*it)->dwIndex);
		//数量
		_AddToByteArray(&pByteArray,(*it)->dwNum);
		//比例值
		_AddToByteArray(&pByteArray,(*it)->dwRecoverRatio);	
	}
	//吸取经验物品个数
	_AddToByteArray(&pByteArray,static_cast<long>(m_mpExpGoods.size()));
	for (Eit it=m_mpExpGoods.begin(); it!=m_mpExpGoods.end(); ++it)
	{
		//原始名
		_AddToByteArray(&pByteArray,(char*)((*it).first.c_str()));
		//比例
		_AddToByteArray(&pByteArray,(*it).second->dwExpRatio);
		//脚本
		_AddToByteArray(&pByteArray,(*it).second->strScript.c_str());
		//提示
		_AddToByteArray(&pByteArray,(*it).second->strHint.c_str());
	}


	return true;
}

// 解码	
 bool CReLiveGoodsList::DecordFromByteArray(BYTE* pByte, long& pos)
{
	//复活物品数量
	
	long lNum = _GetLongFromByteArray(pByte,pos);
	for(long l=0; l<lNum; ++l )
	{
		char strTemp[256];
		tagReLiveGoods *ptgReliveGoods  = new tagReLiveGoods;
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgReliveGoods->strOrignalName = strTemp;
		ptgReliveGoods->dwIndex			 = _GetDwordFromByteArray(pByte,pos);
		ptgReliveGoods->dwNum			 = _GetDwordFromByteArray(pByte,pos);
		ptgReliveGoods->dwRecoverRatio   = _GetDwordFromByteArray(pByte,pos);
		m_lReLiveGoods.push_back(ptgReliveGoods);

	}
	//吸收经验物品
	long lENum = _GetLongFromByteArray(pByte,pos);
	for (long i=0; i<lENum; ++i)
	{	
		char strOrgName[256];
		char strTemp[1024];
		tagExpGoods *ptgExpGoods = new tagExpGoods;
		_GetStringFromByteArray(pByte,pos,strOrgName);
		ptgExpGoods->dwExpRatio = _GetLongFromByteArray(pByte,pos);
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgExpGoods->strScript = strTemp;
		_GetStringFromByteArray(pByte,pos,strTemp);
		ptgExpGoods->strHint = strTemp;
		m_mpExpGoods[strOrgName] = ptgExpGoods;
	}
	return true;
}

//根据原始名获取物品附加经验的相关参数
CReLiveGoodsList::tagExpGoods* CReLiveGoodsList::GetExpGoods(string strName)
 {
	 Eit it = m_mpExpGoods.find(strName);
		 if (it!=m_mpExpGoods.end())
		 {
			 return (*it).second;
		 }
		 return NULL;
 }
