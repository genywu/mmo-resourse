#include "StdAfx.h"
#include "WeaponEffectXml.h"
#include <sstream>
#include "../../../Public/Common/CStrConverter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<DWORD, DWORD> CWeaponEffectXml::m_mapWeaponEffectXml;

CWeaponEffectXml::CWeaponEffectXml(void)
{
}
CWeaponEffectXml::~CWeaponEffectXml(void)
{
	Release();
}
BOOL CWeaponEffectXml::LoadWeaponEffect(const char* filename)
{
	Release();
	//=================================================================//
	CRFile *prfile = rfOpen(filename);
	if(NULL == prfile)
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		return false;
	}
	TiXmlNode *pNode = NULL; 		
	TiXmlDocument doc(filename);
	//载入技能配置文件
	if( !doc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		char strError[128];
		wsprintf(strError,"LoadFileError:%s",filename);
		MessageBox(NULL,strError,"",MB_OK); 
		rfClose(prfile);
		return false;
	}
	rfClose(prfile);
	//=================================================================//
	TiXmlElement	*m_pElem = NULL;
	pNode = doc.FirstChild("WeaponEffect");
	if(NULL == pNode)
	{
		return FALSE;
	}
	m_pElem = pNode->ToElement();

	for (TiXmlElement *pChildElem = m_pElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
	{
		if (!strcmp(pChildElem->Value(),"Weapon"))
		{
			m_mapWeaponEffectXml[CStrConverter::parseLong(pChildElem->Attribute("weaponid"))] = 
				CStrConverter::parseLong(pChildElem->Attribute("effectid"));
		}
	}
	return TRUE;
}
DWORD CWeaponEffectXml::GetWeaponEffect(DWORD dwWeaponID)
{
	if (m_mapWeaponEffectXml.find(dwWeaponID)!=m_mapWeaponEffectXml.end())
	{
		return m_mapWeaponEffectXml[dwWeaponID];
	}
	return NULL;
}
void CWeaponEffectXml::Release()
{
	m_mapWeaponEffectXml.clear();
}