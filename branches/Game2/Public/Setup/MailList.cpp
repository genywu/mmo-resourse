#include "StdAfx.h"
#include "MailList.h"
#include "../Common/Public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// 静态成员初始化
map<DWORD,tagMailParam*> MailList::m_mapMail;

/// 从文件加载统邮件配置
bool MailList::LoadSysMailList(const char* pFileName)
{
	Release();
	CRFile *prfile = rfOpen(pFileName);
	if(NULL == prfile)	
		return false;

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(pFileName);

	//载入技能配置文件
	doc.LoadData(prfile->GetData(), prfile->GetDatalen());
	rfClose(prfile);

	//取得第一个节点
	pNode = doc.FirstChild("Mail");	

	if (NULL == pNode)
	{
		return false;
	}

	TiXmlElement *pElem = pNode->ToElement();

	if(LoadSysMailList(pElem))
	{
		return true;	
	}
	else
	{
		return false;
	}
}

/// 载入系统邮件参数
bool MailList::LoadSysMailList(TiXmlElement* pElem)
{
	tagMailParam *ptgMailParam = NULL;
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"SysMail"))
		{
			DWORD dwID = GetDwordAttribute(pChildElem, "id");

			map<DWORD, tagMailParam*>::iterator it = m_mapMail.find(dwID);
			if (it!=m_mapMail.end())
			{				
				continue;
			} 
			else
			{
				ptgMailParam = new tagMailParam;
				m_mapMail[dwID] = ptgMailParam;
			}
			LoadSysMailList(pChildElem,ptgMailParam);			
		}
	}
	return true;
}

/// 载入系统邮件参数
void MailList::LoadSysMailList(TiXmlElement* pElem,tagMailParam* ptgMailParam)	
{
	for (TiXmlElement* pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"BaseParam"))
		{
			//主题
			ptgMailParam->strSubject = GetStringAttribute(pChildElem, "subject");
			ptgMailParam->strText = GetStringAttribute(pChildElem, "text");
			ptgMailParam->strExText = GetStringAttribute(pChildElem, "extext");
			ptgMailParam->strWriter = GetStringAttribute(pChildElem, "writer");			

		} 
		else if(!stricmp(pChildElem->Value(), "Receiver"))
		{
			//邮件内容
			string strReceiver = GetStringAttribute(pChildElem, "name");
			if (strReceiver.size()>0)
			{
				ptgMailParam->strReceiverList.push_back(strReceiver); 
			}			
		}
// 		else if(!stricmp(pChildElem->Value(), "Good"))
// 		{
// 			//物品
// 			tagSGoods* ptgSGoods = new tagSGoods;
// 			ptgSGoods->lIndex = GetDwordAttribute(pChildElem, "id");
// 			ptgSGoods->lNum = GetDwordAttribute(pChildElem, "num");
// 			ptgMailParam->lSGoods.push_back(ptgSGoods);
// 		}
		else if (!stricmp(pChildElem->Value(), "Gold"))
		{
			ptgMailParam->lGold = GetDwordAttribute(pChildElem, "value");
		}

		else if (!stricmp(pChildElem->Value(), "Time"))
		{
			tagTimer* ptgTimer = new tagTimer;
			struct tm* timeinfo  = new tm;			
			LoadSysMailTimeConfig(pChildElem, timeinfo);
			ptgTimer->lIntev = GetDwordAttribute(pChildElem, "intev");
			ptgTimer->lCount = GetDwordAttribute(pChildElem, "count");
			ptgTimer->lTimer = (long)mktime(timeinfo);	
			ptgMailParam->ltgTimer.push_back(ptgTimer);
			SAFE_DELETE(timeinfo);
		}
		else if (!stricmp(pChildElem->Value(), "Condition"))
		{
			tagMailCondition* pMailCondition = new tagMailCondition;
			pMailCondition->strName = GetStringAttribute(pChildElem, "name");
			pMailCondition->lValue = GetDwordAttribute(pChildElem, "value");
			pMailCondition->strOperator = GetStringAttribute(pChildElem, "cmpfun");			
			ptgMailParam->lMailCondition.push_back(pMailCondition);
		}
		else if (!stricmp(pChildElem->Value(), "Event"))
		{
			tagMailEvent* ptgMailEvent = new tagMailEvent;
			ptgMailEvent->strEventName= GetStringAttribute(pChildElem, "name");
			LoadSysMailEventTime(pChildElem, ptgMailEvent);
			ptgMailParam->ltgMailEvent.push_back(ptgMailEvent);
		}
	}	
}

/// 载入事件产生的时间区
void MailList::LoadSysMailEventTime(TiXmlElement* pElem,tagMailEvent* ptgMailEvent)
{
	for (TiXmlElement* pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(), "Begin"))
		{
			struct tm* timeinfo  = new tm;	
			LoadSysMailTimeConfig(pChildElem, timeinfo);
			ptgMailEvent->lBeginTime = (long)mktime(timeinfo);
			SAFE_DELETE(timeinfo);
		}
		else if (!stricmp(pChildElem->Value(), "End"))
		{
			struct tm* timeinfo  = new tm;	
			LoadSysMailTimeConfig(pChildElem, timeinfo);
			ptgMailEvent->lEndTime = (long)mktime(timeinfo);
			SAFE_DELETE(timeinfo);
		}
	}
}

/// 载入时间配置	
void MailList::LoadSysMailTimeConfig(TiXmlElement* pElem,struct tm* timeinfo)
{
	timeinfo->tm_year = GetDwordAttribute(pElem, "year") - 1900;
	timeinfo->tm_mon = GetDwordAttribute(pElem, "month") - 1;
	timeinfo->tm_mday = GetDwordAttribute(pElem, "day") ;
	timeinfo->tm_hour = GetDwordAttribute(pElem, "hour");
	timeinfo->tm_min = GetDwordAttribute(pElem, "minute");
	timeinfo->tm_sec = GetDwordAttribute(pElem, "second");
}


/// 编码
bool MailList::AddToByteArray(vector<uchar>* pByteArray)
{
	long lNum = (long)m_mapMail.size();
	_AddToByteArray(pByteArray, lNum);

	map<DWORD,tagMailParam*>::iterator it = m_mapMail.begin();
	for (; it!=m_mapMail.end(); ++it)
	{
		_AddToByteArray(pByteArray, (*it).first);
		_AddToByteArray(pByteArray, (*it).second->strSubject.c_str());		
		_AddToByteArray(pByteArray, (*it).second->strWriter.c_str());
		_AddToByteArray(pByteArray, (*it).second->strText.c_str());
		_AddToByteArray(pByteArray, (*it).second->lGold);

		//定时器
		long lTNum = (long)((*it).second->ltgTimer.size());
		_AddToByteArray(pByteArray, lTNum);

		list<tagTimer*>::iterator tit = (*it).second->ltgTimer.begin();
		for (; tit!=(*it).second->ltgTimer.end(); ++tit)
		{
			_AddToByteArray(pByteArray, (*tit),sizeof(tagTimer));
		}	
		//事件
		long lENum = (long)((*it).second->ltgMailEvent.size());
		_AddToByteArray(pByteArray,lENum);
		list<tagMailEvent*>::iterator eit = (*it).second->ltgMailEvent.begin();
		for (; eit!=(*it).second->ltgMailEvent.end(); ++eit)
		{
			_AddToByteArray(pByteArray, (char*)(*eit)->strEventName.c_str());
			_AddToByteArray(pByteArray, (*eit)->lBeginTime);
			_AddToByteArray(pByteArray, (*eit)->lEndTime);
		}

		//收信人
		long lRNum = (long)((*it).second->strReceiverList.size());
		_AddToByteArray(pByteArray, lRNum);
		list<string>::iterator rit=(*it).second->strReceiverList.begin();
		for (; rit!=(*it).second->strReceiverList.end(); ++rit)
		{
			_AddToByteArray(pByteArray, (*rit).c_str());
		}
		//附加物品
//		long lGNum = (long)((*it).second->lSGoods.size());
//		_AddToByteArray(pByteArray, lGNum);
// 		list<tagSGoods*>::iterator git = (*it).second->lSGoods.begin();
// 		for (; git!=(*it).second->lSGoods.end(); ++git)
// 		{
// 			_AddToByteArray(pByteArray, (*git)->lIndex);
// 			_AddToByteArray(pByteArray, (*git)->lNum);
// 		}
		//发信条件
		long lCNum = (long)((*it).second->lMailCondition.size());
		_AddToByteArray(pByteArray, lCNum);
		list<tagMailCondition*>::iterator cit = (*it).second->lMailCondition.begin();
		for (; cit!=(*it).second->lMailCondition.end(); ++cit)
		{
			_AddToByteArray(pByteArray, (*cit)->strName.c_str());
			_AddToByteArray(pByteArray, (*cit)->lValue);
			_AddToByteArray(pByteArray, (*cit)->strOperator.c_str());
		}
	}
	return true;
}

/// 解码
bool MailList::DecordFromByteArray(uchar* pByte, long& pos)
{
	Release();
	char strTemp[102400];
	long lNum = _GetLongFromByteArray(pByte,pos);
	for (int i=0; i<lNum; ++i)
	{
		tagMailParam* ptgMailParam = new tagMailParam;
		DWORD dwID = _GetDwordFromByteArray(pByte,pos);

		_GetStringFromByteArray(pByte, pos, strTemp);
		ptgMailParam->strSubject = strTemp;

		_GetStringFromByteArray(pByte, pos, strTemp);
		ptgMailParam->strWriter = strTemp;

		_GetStringFromByteArray(pByte, pos, strTemp);
		ptgMailParam->strText = strTemp;
		ptgMailParam->lGold = _GetLongFromByteArray(pByte, pos);

		long lTNum = _GetLongFromByteArray(pByte, pos);
		for (int k=0; k<lTNum; ++k)
		{
			tagTimer* ptgTimer = new tagTimer;
			_GetBufferFromByteArray(pByte, pos, ptgTimer, sizeof(tagTimer));
			ptgMailParam->ltgTimer.push_back(ptgTimer);
		}
		long lENum = _GetLongFromByteArray(pByte, pos);
		for (int l=0; l<lENum; ++l)
		{
			tagMailEvent* ptgMailEvent = new tagMailEvent;

			_GetStringFromByteArray(pByte, pos, strTemp);
			ptgMailEvent->strEventName = strTemp;
			ptgMailEvent->lBeginTime = _GetLongFromByteArray(pByte, pos);
			ptgMailEvent->lEndTime = _GetLongFromByteArray(pByte, pos);
			ptgMailParam->ltgMailEvent.push_back(ptgMailEvent);
		}


		long lRNum = _GetLongFromByteArray(pByte, pos);
		for (int j=0; j<lRNum; ++j)
		{
			_GetStringFromByteArray(pByte, pos, strTemp);
			string strReName = strTemp;
			ptgMailParam->strReceiverList.push_back(strReName);
		}

//		long lGNum = _GetLongFromByteArray(pByte, pos);
// 		for (int index=0; index<lGNum; ++index)
// 		{
// 			tagSGoods* ptgSGoods = new tagSGoods;
// 			ptgSGoods->lIndex = _GetLongFromByteArray(pByte, pos);
// 			ptgSGoods->lNum = _GetLongFromByteArray(pByte, pos);
// 			ptgMailParam->lSGoods.push_back(ptgSGoods);
// 		}

		long lCNum = _GetLongFromByteArray(pByte, pos);
		for (int index=0; index<lCNum; ++index)
		{
			tagMailCondition* ptgMailCondition = new tagMailCondition;
			_GetStringFromByteArray(pByte, pos, strTemp);
			ptgMailCondition->strName = strTemp;
			ptgMailCondition->lValue = _GetLongFromByteArray(pByte, pos);
			_GetStringFromByteArray(pByte, pos, strTemp);
			ptgMailCondition->strOperator = strTemp;
			ptgMailParam->lMailCondition.push_back(ptgMailCondition);
		}

		m_mapMail[dwID] = ptgMailParam;
	}	
	return true;
}

/// 通过ID获取系统邮件
tagMailParam* MailList::GetSysMail(DWORD dwID)
{
	map<DWORD,tagMailParam*>::iterator it = m_mapMail.find(dwID);
	if (it!=m_mapMail.end())
	{
		return (*it).second;
	}
	return NULL;
}

/// 获取双字
DWORD MailList::GetDwordAttribute(TiXmlElement* pElem, string strName)
{
	double dValue;
	if(pElem->QueryDoubleAttribute(strName.c_str(), &dValue))
	{
		dValue = 0;
	}
	return (DWORD)dValue;
}

/// 获取位型
uchar MailList::GetIntAttribute(TiXmlElement *pElem,string strName)
{
	int iValue;
	if(pElem->QueryIntAttribute(strName.c_str(),&iValue))
	{
		iValue = 0;
	}
	return iValue;
}

/// 获取字符串
string MailList::GetStringAttribute(TiXmlElement* pElem, string strName)
{
	string newStr;
	if (NULL != pElem->Attribute(strName.c_str()))
	{
		newStr = pElem->Attribute(strName.c_str());
	} 
	else
	{
		newStr = "";
	}
	return newStr;
}
/// 获取文本
string MailList::GetTextAttribute(TiXmlElement* pElem)
{
	string strValue;
	if (NULL != pElem->GetText())
	{
		strValue = pElem->GetText();
	} 
	else
	{
		strValue = "";
	}
	return strValue;
}
/// 获取系统邮件
map<DWORD,tagMailParam*> MailList::GetSysMail()																
{
	return m_mapMail;
}

/// 释放
void MailList::Release()
{
	map<DWORD,tagMailParam*>::iterator it=m_mapMail.begin();
	for (; it!=m_mapMail.end(); ++it)
	{
// 		for (list<tagSGoods*>::iterator git=(*it).second->lSGoods.begin(); git!=(*it).second->lSGoods.end(); ++git)
// 		{
// 			SAFE_DELETE(*git);
// 		}
// 		(*it).second->lSGoods.clear();	

		for (list<tagMailEvent*>::iterator git=(*it).second->ltgMailEvent.begin(); git!=(*it).second->ltgMailEvent.end(); ++git)
		{
			SAFE_DELETE(*git);
		}
		(*it).second->ltgMailEvent.clear();	

		for (list<tagTimer*>::iterator git=(*it).second->ltgTimer.begin(); git!=(*it).second->ltgTimer.end(); ++git)
		{
			SAFE_DELETE(*git);
		}
		(*it).second->ltgTimer.clear();		

		for (list<tagMailCondition*>::iterator cit=(*it).second->lMailCondition.begin(); cit!=(*it).second->lMailCondition.end(); ++cit)
		{
			SAFE_DELETE(*cit);
		}
		(*it).second->lMailCondition.clear();

		SAFE_DELETE((*it).second);
	}
	m_mapMail.clear();	
}