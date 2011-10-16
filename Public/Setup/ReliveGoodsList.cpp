#include "StdAfx.h"
#include "ReliveGoodsList.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
list<ReLiveGoodsList::tagReLiveGoods*>  ReLiveGoodsList::m_lReLiveGoods;

/// 构造
ReLiveGoodsList::ReLiveGoodsList()
{

}

/// 析构
ReLiveGoodsList::~ReLiveGoodsList()
{

}

/// 从文件加载复活物品列表
bool ReLiveGoodsList::Init(const char* filename)
{
	if (NULL==filename)
	{
		return false;
	}

	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		Log4c::Error(ROOT_MODULE, "%-15s LoadFileError:%s", __FUNCTION__,filename);

		return false;
	}
	TiXmlNode* pNode = NULL; 		
	TiXmlDocument m_Tdoc(filename);
	//载入复活物品配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(), prfile->GetDatalen()) )
	{
		//输出错误信息
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,FormatText("WS_SKILL_FOURTH", filename));
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);

	//找到根节点，如果无节点，输出错误提示
	pNode = m_Tdoc.FirstChild("ReLiveGoods");
	if (pNode == NULL)
	{
        Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,GetText("WS_SKILL_FIFTH"));
		return false;
	}
	TiXmlElement* pElem = pNode->ToElement();

	for (TiXmlElement* pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(), "Goods"))
		{
			tagReLiveGoods *ptgReliveGoods = new tagReLiveGoods;
			ptgReliveGoods->strOrignalName = pChildElem->Attribute("OrignalName");
			ptgReliveGoods->dwIndex = (ulong)atol(pChildElem->Attribute("Index"));
			ptgReliveGoods->dwNum = (ulong)atol( pChildElem->Attribute("GoodsNum"));
			ptgReliveGoods->dwRecoverRatio = (ulong)atol(pChildElem->Attribute("RecoverRatio"));
			m_lReLiveGoods.push_back(ptgReliveGoods);		
		}
	}
	return true;
}

/// 清理	
void ReLiveGoodsList::Release()
{
	list<tagReLiveGoods*>::iterator it = m_lReLiveGoods.begin();
	for (; it!=m_lReLiveGoods.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_lReLiveGoods.clear();
}

/// 解码
bool ReLiveGoodsList::AddToByteArray(vector<uchar>& pByteArray)
{
	//复活物品个数		
	_AddToByteArray(&pByteArray,static_cast<long>(m_lReLiveGoods.size()));

	list<tagReLiveGoods *>::iterator it = m_lReLiveGoods.begin();
	for (; it!=m_lReLiveGoods.end(); ++it)
	{
		// 原始名
		_AddToByteArray(&pByteArray, (char*)((*it)->strOrignalName.c_str()));			
		//索引
		_AddToByteArray(&pByteArray, (*it)->dwIndex);
		//数量
		_AddToByteArray(&pByteArray, (*it)->dwNum);
		//比例值
		_AddToByteArray(&pByteArray, (*it)->dwRecoverRatio);	
	}
	return true;
}

/// 解码	
 bool ReLiveGoodsList::DecordFromByteArray(uchar* pByte, long& pos)
{
	//复活物品数量
	char strTemp[256];
	long lNum = _GetLongFromByteArray(pByte, pos);
	for(long l=0; l<lNum; ++l)
	{
		tagReLiveGoods *ptgReliveGoods  = new tagReLiveGoods;
		_GetStringFromByteArray(pByte, pos, strTemp);
		ptgReliveGoods->strOrignalName = strTemp;
		ptgReliveGoods->dwIndex = _GetDwordFromByteArray(pByte,pos);
		ptgReliveGoods->dwNum = _GetDwordFromByteArray(pByte,pos);
		ptgReliveGoods->dwRecoverRatio = _GetDwordFromByteArray(pByte,pos);
		m_lReLiveGoods.push_back(ptgReliveGoods);
	}
	return true;
}
