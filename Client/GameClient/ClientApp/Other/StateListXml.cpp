#include "stdAfx.h"
#include "StateListXml.h"
#include <sstream>
#include "../../../Public/Common/CStrConverter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<long,CStateListXml::tagStateList>	CStateListXml::m_mapStateList;
CStateListXml::CStateListXml(void)
{

}
CStateListXml::~CStateListXml(void)
{
	Release();
}

BOOL CStateListXml::Load(const char* filename)
{
	m_mapStateList.clear();
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
	pNode = doc.FirstChild("State");
	if (NULL==pNode)
	{
		return false;
	}

	m_pElem = pNode->ToElement();

	for (TiXmlElement *pChildElem = m_pElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
	{
		if (!strcmp(pChildElem->Value(),"BUFF"))
		{
			tagStateList	stStateList;
			 if (NULL!= pChildElem->Attribute("name"))
				 stStateList.strAfettleName = pChildElem->Attribute("name");
			 if(NULL!= pChildElem->Attribute("id"))
				 stStateList.lAfettleID = CStrConverter::parseInt(pChildElem->Attribute("id"));
			 if(NULL!= pChildElem->Attribute("icon"))
				 stStateList.lAfettleICON = CStrConverter::parseInt(pChildElem->Attribute("icon"));
			 if(NULL!= pChildElem->Attribute("stateonly"))
				 stStateList.AfettleStateOnly = CStrConverter::parseInt(pChildElem->Attribute("stateonly"));
			 if(NULL!= pChildElem->Attribute("isbuff"))
				 stStateList.AfettleIsBuff = CStrConverter::parseInt(pChildElem->Attribute("isbuff"));
			 if(NULL!= pChildElem->Attribute("isstopmove"))
				 stStateList.bStopMove = CStrConverter::parseInt(pChildElem->Attribute("isstopmove"));
			 if(NULL!= pChildElem->Attribute("action"))
				 stStateList.wAction = CStrConverter::parseInt(pChildElem->Attribute("action"));

			 for (TiXmlElement *pLevelElem = pChildElem->FirstChildElement(); pLevelElem!=NULL; pLevelElem = pLevelElem->NextSiblingElement())
			 {
				 
				 if(!strcmp(pLevelElem->Value(),"Level"))
				 {
					 tagStateLevel stStateLevel;
					 if(NULL!= pLevelElem->Attribute("id"))
					 {
						 stStateLevel.dwAfettleLevel = CStrConverter::parseInt(pLevelElem->Attribute("id"));
					 }
					 if(NULL!= pLevelElem->Attribute("continuetime"))
					 {
						 stStateLevel.dwAfettleContinueTime = CStrConverter::parseInt(pLevelElem->Attribute("continuetime"));
					 }
					 if (NULL!= pLevelElem->Attribute("desc"))
					 {
						 stStateLevel.strAfettleDesc = pLevelElem->Attribute("desc");
					 }
					 if(NULL !=pLevelElem->Attribute("afettledeathisclear"))
					 {
						 stStateLevel.AfettleDeathIsClear = CStrConverter::parseInt(pLevelElem->Attribute("afettledeathisclear"));
					 }
					 if(NULL !=pLevelElem->Attribute("colorfade"))
					 {
						 stStateLevel.bColorFade = CStrConverter::parseBool(pLevelElem->Attribute("colorfade"));
					 }
					 //------------------开始阶段特效------------------
					 //if (NULL!= pLevelElem->Attribute("begineffect"))
					 //{
						// stStateLevel.AfettleBeginEffect = CStrConverter::parseInt(pLevelElem->Attribute("begineffect"));
					 //}
					 //if (NULL != pLevelElem->Attribute("begineffectindex"))
					 //{
						// stStateLevel.AfettleBeginEffectIndex = CStrConverter::parseInt(pLevelElem->Attribute("begineffectindex"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("begineffectposition"))
					 //{
						// stStateLevel.AfettleBeginEffectPosition = CStrConverter::parseInt(pLevelElem->Attribute("begineffectposition"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("begineffectcycle"))
					 //{
						// stStateLevel.AfettleBeginEffectCycle = CStrConverter::parseInt(pLevelElem->Attribute("begineffectcycle"));
					 //}
					 ////------------------中间阶段特效------------------
					 //if (NULL!= pLevelElem->Attribute("effect"))
					 //{
						// stStateLevel.AfettleEffect = CStrConverter::parseInt(pLevelElem->Attribute("effect"));
					 //}
					 //if (NULL != pLevelElem->Attribute("effectindex"))
					 //{
						// stStateLevel.AfettleEffectIndex = CStrConverter::parseInt(pLevelElem->Attribute("effectindex"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("effectposition"))
					 //{
						// stStateLevel.AfettleEffectPosition = CStrConverter::parseInt(pLevelElem->Attribute("effectposition"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("effectcycle"))
					 //{
						// stStateLevel.AfettleEffectCycle = CStrConverter::parseInt(pLevelElem->Attribute("effectcycle"));
					 //}
					 ////------------------结束阶段特效------------------
					 //if (NULL!= pLevelElem->Attribute("endeffect"))
					 //{
						// stStateLevel.AfettleEndEffect = CStrConverter::parseInt(pLevelElem->Attribute("endeffect"));
					 //}
					 //if (NULL != pLevelElem->Attribute("endeffectindex"))
					 //{
						// stStateLevel.AfettleEndEffectIndex = CStrConverter::parseInt(pLevelElem->Attribute("endeffectindex"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("endeffectposition"))
					 //{
						// stStateLevel.AfettleEndEffectPosition = CStrConverter::parseInt(pLevelElem->Attribute("endeffectposition"));
					 //}
					 //if (NULL!= pLevelElem->Attribute("endeffectcycle"))
					 //{
						// stStateLevel.AfettleEndEffectCycle = CStrConverter::parseInt(pLevelElem->Attribute("endeffectcycle"));
					 //}
					 //------------------------------------------------
					 if (NULL!= pLevelElem->Attribute("colorinterval"))
					 {
						 stStateLevel.dwInterval = CStrConverter::parseInt(pLevelElem->Attribute("colorinterval"));
					 }
					 for(TiXmlElement *pChildElem = pLevelElem->FirstChildElement(); pChildElem!=NULL; pChildElem = pChildElem->NextSiblingElement())
					 {
						 // 颜色
						 if(!strcmp(pChildElem->Value(),"Color"))
						 {
							 if(NULL!= pChildElem->Attribute("value"))
								 stStateLevel.vecBuffColor.push_back(CStrConverter::parseStringToDec(pChildElem->Attribute("value")));
						 }
						 // 开始阶段特效
						 else if(!strcmp(pChildElem->Value(),"BeginEffect"))
						 {
							 if( NULL!= pChildElem->Attribute("id") && 
								 NULL!= pChildElem->Attribute("index") && 
								 NULL!= pChildElem->Attribute("pos") && 
								 NULL!= pChildElem->Attribute("cycle"))
							 {
								 tagStateEffect stEffect;
								 stEffect.dwEffectID		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("id"));
								 stEffect.dwEffectIndex		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("index"));
								 stEffect.dwEffectPosition	= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("pos"));
								 stEffect.dwEffectCycle		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("cycle"));
								 if(pChildElem->Attribute("isorderplay"))
									 stEffect.bIsOrderPlay		= (DWORD)CStrConverter::parseBool(pChildElem->Attribute("isorderplay"));
								 if(stEffect.dwEffectID > 0)
									 stStateLevel.vecBeginEffect.push_back(stEffect);
							 }
						 }
						 // 中间阶段特效
						 else if(!strcmp(pChildElem->Value(),"MidEffect"))
						 {
							 if( NULL!= pChildElem->Attribute("id") && 
								 NULL!= pChildElem->Attribute("index") && 
								 NULL!= pChildElem->Attribute("pos") && 
								 NULL!= pChildElem->Attribute("cycle"))
							 {
								 tagStateEffect stEffect;
								 stEffect.dwEffectID		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("id"));
								 stEffect.dwEffectIndex		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("index"));
								 stEffect.dwEffectPosition	= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("pos"));
								 stEffect.dwEffectCycle		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("cycle"));
								 if(pChildElem->Attribute("isorderplay"))
									 stEffect.bIsOrderPlay		= (DWORD)CStrConverter::parseBool(pChildElem->Attribute("isorderplay"));
								 if(stEffect.dwEffectID > 0)
									 stStateLevel.vecMidEffect.push_back(stEffect);
							 }
						 }
						 // 结束阶段特效
						 else if(!strcmp(pChildElem->Value(),"EndEffect"))
						 {
							 if( NULL!= pChildElem->Attribute("id") && 
								 NULL!= pChildElem->Attribute("index") && 
								 NULL!= pChildElem->Attribute("pos") && 
								 NULL!= pChildElem->Attribute("cycle"))
							 {
								 tagStateEffect stEffect;
								 stEffect.dwEffectID		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("id"));
								 stEffect.dwEffectIndex		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("index"));
								 stEffect.dwEffectPosition	= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("pos"));
								 stEffect.dwEffectCycle		= (DWORD)CStrConverter::parseInt(pChildElem->Attribute("cycle"));
								 if(pChildElem->Attribute("isorderplay"))
									 stEffect.bIsOrderPlay		= (DWORD)CStrConverter::parseBool(pChildElem->Attribute("isorderplay"));
								 if(stEffect.dwEffectID > 0)
									 stStateLevel.vecEndEffect.push_back(stEffect);
							 }
						 }
					 }
					 stStateList.vecStateLevel.push_back(stStateLevel);
				 }
			 }
			 m_mapStateList[stStateList.lAfettleID] = stStateList;
		}
	}
	return true;
}

CStateListXml::tagStateList *CStateListXml::GetState(long lID)
{
	if (m_mapStateList.find(lID) != m_mapStateList.end())
	{
		return &m_mapStateList[lID];
	}
	return NULL;
}
void CStateListXml::Release()
{
	m_mapStateList.clear();
}