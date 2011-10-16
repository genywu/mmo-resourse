//======================================================================================
/**
*	@file		MailList.h
*	@data		09/19/2008
*	@author		陈先进
*/
//======================================================================================
#pragma once

#include "../Common/MailDef.h"
/**
*	@class MailList
*
*	@brief 系统邮件配置类
*/
class MailList
{
public:
	/// 构造
	MailList(void);

	/// 析构
	~MailList(void);

	/// 从文件加载统邮件配置
	static bool LoadSysMailList(const char* pFileName);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码	
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 通过ID获取系统邮件
	static tagMailParam* GetSysMail(DWORD dwID);

	/// 获取系统邮件
	static map<DWORD,tagMailParam*> GetSysMail();

	/// 释放
	static void Release();
private:									
	/// 载入系统邮件参数
	static bool	LoadSysMailList(TiXmlElement* pElem);

	/// 载入系统邮件参数
	static void LoadSysMailList(TiXmlElement* pElem,tagMailParam* ptgMailParam);

	/// 载入时间配置
	static void LoadSysMailTimeConfig(TiXmlElement* pElem,struct tm* timeinfo);

	/// 载入事件产生的时间区
	static void LoadSysMailEventTime(TiXmlElement* pElem,tagMailEvent* ptgMailEvent);

	/// 获取DWORD	
	static DWORD GetDwordAttribute(TiXmlElement* pElem,string strName);

	/// 获取字符串	
	static string GetStringAttribute(TiXmlElement* pElem,string strName);

	/// 获取整型	
	static uchar GetIntAttribute(TiXmlElement* pElem,string strName);

	/// 获取文本
	static string GetTextAttribute(TiXmlElement* pElem);
		
private:
	/// 系统邮件结构
	static map<DWORD,tagMailParam*> m_mapMail;
};
