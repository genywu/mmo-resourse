//=============================================================================
/**
 *  file: CSetup.h
 *
 *  Brief:配置类
 *
 *  Authtor:wangqiao
 *	
 *	Datae:2007-7-30
 */
//=============================================================================
#include "StdAfx.h"
#include ".\setup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSetup* CSetup::instance=NULL;

CSetup::CSetup(void)
{
}

CSetup::~CSetup(void)
{
}

CSetup* CSetup::getInstance()
{
	if(NULL == instance)
		instance = new CSetup;
	return instance;
}

void CSetup::Release()
{
	if(instance)
		SAFE_DELETE(instance);
}

void CSetup::Load(const char* file)
{
 	char strfile[512] = "setup\\Initial.xml";
 	if(file)
 		strncpy(strfile,file,510);

	TiXmlNode *pNode = NULL;
	TiXmlDocument doc(strfile);
	///载入配置文件
	if(!doc.LoadFile())
	{
		PutDebugString("装载配置文件<<Initial.xml>>出错！");
		return;
	}

	pNode = doc.FirstChild("Setup");
	if(pNode)
	{
		TiXmlElement *pChildElement = pNode->FirstChildElement();
		for(;pChildElement!=NULL;pChildElement=pChildElement->NextSiblingElement())
		{
			if(!strcmp(pChildElement->Value(),"Server"))
			{
				m_strServerIP = string(pChildElement->Attribute("ip"));
				pChildElement->Attribute("port",&m_nPort);
				//设置IP
			}
			else if(!strcmp(pChildElement->Value(),"ScriptFile"))
			{
				//设置初始化的时候要读的脚本文件
				m_strIniScript = string(pChildElement->Attribute("name"));
			}
		}
	}
}
