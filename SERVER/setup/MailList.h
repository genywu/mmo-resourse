///////////////////////////////////////////////////////////////////////////////
///  MailList.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\setup
///
///  CREATED: 09/19/2008  by 陈先进
///
///  PURPOSE:   系统邮件类
//////////////////////////////////////////////////////////////////////
#ifndef _INC_MAILLIST_INCLUDED
#define _INC_MAILLIST_INCLUDED
//#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
//#endif



#include "../../public/tinyxml.h"
#include "../../public/MailDef.h"

using namespace std;
class CMailList
{
public:
	CMailList(void);
	virtual ~CMailList(void);

	static bool 							LoadSysMailList(const char *pFileName);										//载入系统邮件文件
	static bool								LoadSysMailList( TiXmlElement *pElem );										//载入系统邮件参数
	static void								LoadSysMailList( TiXmlElement *pElem,tagMailParam *ptgMailParam);			//载入系统邮件参数
	static void								LoadSysMailTimeConfig(TiXmlElement *pElem,struct tm *timeinfo);				//载入时间配置
	static void								LoadSysMailEventTime(TiXmlElement *pElem,tagMailEvent *ptgMailEvent);		//载入事件产生的时间区
	static bool 							AddToByteArray(vector<BYTE>* pByteArray);									//添加到CByteArray
	static bool 							DecordFromByteArray(BYTE* pByte, long& pos);								//解码	
	static DWORD							GetDwordAttribute(TiXmlElement *pElem,string strName);						//获取DWORD	
	static string							GetStringAttribute(TiXmlElement *pElem,string strName);						//获取字符串	
	static BYTE								GetIntAttribute(TiXmlElement *pElem,string strName);						//获取整型	
	static string							GetTextAttribute(TiXmlElement *pElem);										//获取文本
	static tagMailParam*					GetSysMail(DWORD dwID);														//通过ID获取系统邮件
	static map<DWORD,tagMailParam*>			GetSysMail();																//获取系统邮件
	static void								Release();																	//释放

private:	
	static map<DWORD,tagMailParam*> m_mapMail;
};
#endif//_INC_MAILLIST_INCLUDED