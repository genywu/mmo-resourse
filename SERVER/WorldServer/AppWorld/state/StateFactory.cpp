///////////////////////////////////////////////////////////////////////////////
///  StateFactory.cpp
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\WorldServer\AppWorld\State
///
///  CREATED: 06/08/2007 9:50:01 PM by 陈先进
///
///  PURPOSE:   实现状态工厂类
///
///  COPYRIGHT NOTICE:   Copyright (C) Aurora Game
///
///  LAST CHANGED: $Date$
///
///  REVISION HISTORY:
///  $Log$
/// 

// StateFactory.cpp: 实现 CStateFactory class类.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StateFactory.h"
//#include "State.h"
#include "../../../../public/CStrConverter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<DWORD,CState*>  CStateFactory::g_mStateMap;

/*
NAME:ClearStateCache
DESC:清除状态缓冲
PARA:无
*/
void CStateFactory::ClearStateCache()
{
	//清除状态缓冲
	for( map<DWORD, CState*>::iterator it = g_mStateMap.begin(); 
		it != g_mStateMap.end(); it ++ )
	{
		SAFE_DELETE( it -> second );
	}
	g_mStateMap.clear();
}


/*
NAME:LoadStateCache
DESC:载入状态缓冲
PARA:无
*/
bool CStateFactory::LoadStateCache()
{
	ClearStateCache();

	// 读取状态配置文件
	//LoadStateConfig("states/StateConfig.ini");
	//从指定文件夹从取得所有的状态配置文件，并压入文件列表中
	list<string> listFileName;
	string strTemptFolder("states/*.*");
	CheckRFileStr(strTemptFolder);
	string strTempt(strTemptFolder);
	int nPos = strTemptFolder.find_last_of("\\");
	strTempt.erase(0,nPos+1);

	if(strTempt=="*.*")
		strTemptFolder.erase(nPos,strTemptFolder.length()-nPos);

	if(CClientResource::GetInstance()->IsFileExist(strTemptFolder.c_str()) )
	{
		string strTemptExt(".xml");
		GetDefaultClientResource()->FindFileList(strTemptFolder,strTemptExt,listFileName);
	}
	else
	{
		FindFile( ".\\states\\*.*", ".xml", &listFileName );
	}


	bool bResult = true;
	for( list<string>::iterator it = listFileName.begin(); it != listFileName.end(); it++ )
	{
		const char* pFileName = it -> c_str();
		//载入一个配置文件
		if( FAILED( LoadConfigration( pFileName ) ) )
		{
			bResult = true;
			ClearStateCache();
			break;
		}
	}
	return bResult;
}


/*
NAME:LoadConfigration
DESC:读取状态配置文件
PARA:pFileName,配置文件名称
*/
HRESULT CStateFactory::LoadConfigration( const string pFileName )
{
	if( pFileName.empty()) 
		return E_FAIL;
	
	TiXmlNode *pNode = NULL;
	
	TiXmlDocument doc(pFileName.c_str());
	//载入状态配置文件
	doc.LoadFile();					

	//取得第一个节点
	pNode = doc.FirstChild("State");	

	if (NULL == pNode)
	{
		return E_FAIL;
	}
	
	TiXmlElement *pElem = pNode->ToElement();
	
	LoadConfigration(pElem);
	
	return S_OK;	
}

/*
NAME:LoadConfigration
DESC:通过节点载入
PARA:pElem节点元素
*/
HRESULT	CStateFactory::LoadConfigration	( TiXmlElement *pElem )
{
	string strName;
	DWORD dwID;	
	BYTE byOnly;
	DWORD dwDeathIsClear;
	static CState *pState	= NULL;

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Buff"))
		{

			strName = pChildElem->Attribute("name");			
			dwID =  CStrConverter::parseLong(pChildElem->Attribute("ID"));
			byOnly = CStrConverter::parseInt(pChildElem->Attribute("Buffonly"));
			dwDeathIsClear = CStrConverter::parseLong(pChildElem->Attribute("DiedClear"));

			

			pState = new CState(dwID);			//建立状态
			g_mStateMap[dwID] = pState;
			pState->SetStateName(strName);		//设置状态名称
			pState->SetStateOnly(byOnly);
			pState->SetStateDeathIsClear(dwDeathIsClear);

			LoadConfigration(pChildElem);		
		}
		else if (!stricmp(pChildElem->Value(),"Level"))
		{
			LoadLevelConfigration(pChildElem,pState);		//某个等级的状态	
		}
		else if (!stricmp(pChildElem->Value(),"Condition"))
		{			
			LoadConditionConfigration(pChildElem,pState);	//状态条件
		}
	}

	return S_OK;
}

/*
NAME:LoadConditionConfigration
DESC:载入条件判断
PARA:pElem节点元素
pState状态
*/
HRESULT CStateFactory::LoadConditionConfigration(TiXmlElement *pElem, CState *pState )
{

	DWORD dwConditionID;
	if (NULL!=pElem->Attribute("ID"))
	{
		dwConditionID = CStrConverter::parseLong(pElem->Attribute("ID"));	
	} 
	else
	{
		dwConditionID = 0;
	}

	
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		CState::tagCondition tgCondition;// = new CState::tagCondition;

		if (!stricmp(pChildElem->Value(),"Good"))
		{
			CState::tagGoodCondition tgGoodCondition ;//*= new CState::tagGoodCondition;
			if (NULL!=pChildElem->Attribute("Place"))
			{
				tgGoodCondition.dwPlace = CStrConverter::parseLong(pChildElem->Attribute("Place"));
			} 
			else
			{
				tgGoodCondition.dwPlace = 0;
			}
			
			if (NULL!=pChildElem->Attribute("Pos"))
			{
				tgGoodCondition.dwPos   = CStrConverter::parseLong(pChildElem->Attribute("Pos"));
			} 
			else
			{
				tgGoodCondition.dwPos = 0;
			}
			
			if (NULL!=pChildElem->Attribute("GoodType"))
			{
				tgGoodCondition.dwType = CStrConverter::parseLong(pChildElem->Attribute("GoodType"));
			} 
			else
			{
				tgGoodCondition.dwType = 0;
			}

			tgCondition.vtgGoodCondition.push_back(tgGoodCondition);			

		}
		else if (!stricmp(pChildElem->Value(),"Skill"))
		{
			DWORD dwSkillID = CStrConverter::parseLong(pChildElem->Attribute("ID"));
			tgCondition.vtgSkillID.push_back(dwSkillID);
		}

		pState->SetCondition(dwConditionID,tgCondition);
	}
	
	return S_OK;
}


/*
NAME:LoadLevelConfigration
DESC:载入各个等级的状态
PARA:pElem节点元素
	 pState状态
*/
HRESULT CStateFactory::LoadLevelConfigration(TiXmlElement *pElem, CState *pState)
{
	DWORD dwLevel;
//	DWORD dwTime;
//	DWORD dwBfettleID;
//	float fBfettleValue;
	CState::tagState *ptgState = new CState::tagState;
	//状态的等级
	dwLevel = CStrConverter::parseLong(pElem->Attribute("value"));		
	ptgState->dwLevel = dwLevel;


	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{

		if (!stricmp(pChildElem->Value(),"Begin"))
		{
			LoadBeginConfig(pChildElem,ptgState);
		} 
		else if(!stricmp(pChildElem->Value(),"Event"))
		{
			LoadEventConfig(pChildElem,ptgState);
		}
		else if (!stricmp(pChildElem->Value(),"Timeout"))
		{
			LoadTimeOutConfig(pChildElem,ptgState);
		}
		else if (!stricmp(pChildElem->Value(),"End"))
		{			
			LoadEndConfig(pChildElem,ptgState);
		}	
		
	}
	pState->AddState(ptgState);

	return S_OK;

}
//载入事件驱动脚本
HRESULT	CStateFactory::LoadEventConfig(TiXmlElement *pElem,CState::tagState *ptgState)
{
	
	//ptgEvent = NULL;

	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		CState::tagEvent *ptgEvent = new CState::tagEvent;
		if(!stricmp(pChildElem->Value(),"EventName"))
		{
			//事件名称
			string strEventName = pChildElem->Attribute("value");

			DWORD dwCondition;
			if (NULL != pChildElem->Attribute("ConditionID"))
			{
				dwCondition = CStrConverter::parseLong(pChildElem->Attribute("ConditionID"));
			} 
			else
			{
				dwCondition = 0;
			}
			ptgEvent->dwCondition = dwCondition;


			for (TiXmlElement *pSubElem=pChildElem->FirstChildElement(); pSubElem!=NULL; 
				pSubElem=pSubElem->NextSiblingElement())
			{
				if (!stricmp(pSubElem->Value(),"Script"))
				{
					//执行脚本
					string strscript;
					if (NULL != pSubElem->GetText())
					{
						strscript = pSubElem->GetText();						
					} 
					else
					{
						strscript = "";
					}			
					ptgEvent->strScripte = strscript;
					
					ptgState->mapEvent[strEventName] = ptgEvent;//trscript;
				}
			}		
			
		}
	}
	return S_OK;
}
//载入更新状态配置
HRESULT CStateFactory::LoadTimeOutConfig(TiXmlElement *pElem,CState::tagState *ptgState)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Property"))
		{
			//载入基本状态
			CState::tagFettle *tgFettle = LoadBaseProperty(pChildElem);
			ptgState->vtgUpdateFettle.push_back(tgFettle);		
		}
		else if (!stricmp(pChildElem->Value(),"Script"))
		{
			if(NULL != pChildElem->GetText())
			{
				ptgState->strUpdateScript = pChildElem->GetText();
			}
			else
			{
				 ptgState->strUpdateScript = "";
			}
		}
	}
	return S_OK;
}


//载入结束状态配置
HRESULT CStateFactory::LoadEndConfig(TiXmlElement *pElem,CState::tagState *ptgState)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"Property"))
		{
			//载入基本状态
			CState::tagFettle *tgFettle = LoadBaseProperty(pChildElem);
			ptgState->vtgEndFettle.push_back(tgFettle);		
		}
		else if (!stricmp(pChildElem->Value(),"Script"))
		{
			if(NULL != pChildElem->GetText())
			{
				ptgState->strEndScript = pChildElem->GetText();
			}
			else
			{
				 ptgState->strEndScript = "";
			}
		}
	}
	return S_OK;
}

//载入开始状态
HRESULT CStateFactory::LoadBeginConfig(TiXmlElement *pElem,CState::tagState *ptgState)
{
	for (TiXmlElement *pChildElem=pElem->FirstChildElement(); pChildElem!=NULL; 
		pChildElem=pChildElem->NextSiblingElement())
	{
		if (!stricmp(pChildElem->Value(),"InteTime"))
		{
			//间隔时间
			DWORD dwInteTime = CStrConverter::parseLong(pChildElem->Attribute("value"));
			ptgState->dwTime = dwInteTime;
		}
		else if(!stricmp(pChildElem->Value(),"Count"))
		{
			//次数
			DWORD dwCount = CStrConverter::parseLong(pChildElem->Attribute("value"));
			ptgState->dwCount = dwCount;
		}
		else if (!stricmp(pChildElem->Value(),"Property"))
		{
			//载入基本状态
			CState::tagFettle *tgFettle = LoadBaseProperty(pChildElem);
			ptgState->vtgBegingFettle.push_back(tgFettle);

		}
		else if (!stricmp(pChildElem->Value(),"Script"))
		{
			if(NULL != pChildElem->GetText())
			{
				
				ptgState->strBeginScript = pChildElem->GetText();
				
			}
			else
			{
				ptgState->strBeginScript = "";
			}
		}
	}
	return S_OK;
}

//载入基本状态
CState::tagFettle *CStateFactory::LoadBaseProperty(TiXmlElement *pElem)//,CState::tagState *ptgState
{
	CState::tagFettle *tgFettle = new CState::tagFettle;
	string strName;
	string strOpera;
	DWORD  dwConId;
	DWORD  dwValue;

	//名称
	if (NULL != pElem->Attribute("Name"))
	{
		strName = pElem->Attribute("Name");
		
	}
	else
	{
		strName = "";
	}
	//操作符
	if (NULL != pElem->Attribute("Operator"))
	{
		strOpera = pElem->Attribute("Operator");

	}
	else
	{
		strOpera = "";
	}
	//条件
	if (NULL != pElem->Attribute("ConditionID"))
	{
		dwConId = CStrConverter::parseLong(pElem->Attribute("ConditionID"));

	}
	else
	{
		dwConId = 0;
	}
	//修正值
	if (NULL != pElem->Attribute("value"))
	{
		dwValue = CStrConverter::parseLong(pElem->Attribute("value"));

	}
	else
	{
		dwValue = 0;
	}
	
	//公式
	tgFettle->strFettleName = strName;
	tgFettle->strOpera = strOpera;
	tgFettle->dwValue  = dwValue;
	tgFettle->dwCondition  = dwConId;
	tgFettle->strFormat = "";

	return tgFettle;
}


/*
NAME:CodeState
DESC:状态编码
PARA:vRet缓冲
*/
bool CStateFactory::CodeState( vector<BYTE>& vRet )
{
	//多少个状态/等级
	//可能需要事先确定有没有null的指针
	
	_AddToByteArray( &vRet, static_cast<DWORD>( g_mStateMap.size() ) );

	size_t	lLength	= 0;
	BYTE*	pBuffer	= NULL;
	for( map<DWORD, CState*>::iterator it = g_mStateMap.begin(); 
		it != g_mStateMap.end(); it ++ )
	{
		if( it -> second )
		{			
			it ->second ->AddStateToByteArray(vRet);		
		}
		else 
		{
			_AddToByteArray( &vRet, static_cast<DWORD>( 0 ) );
		}
	}

	
	return true;
}