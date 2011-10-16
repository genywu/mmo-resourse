#include "StdAfx.h"
#include "organizingparam.h"
#include "../public/tinystr.h"
#include "../public/tinyxml.h"
#include "../goods/CGoodsFactory.h"

//Function:	帮会及相关组织的参数类
//Author:	Joe
//Create Time:2005.3.15



extern HWND g_hWnd;

WSOrganizingParam	g_OrganizingParam;

WSOrganizingParam::WSOrganizingParam(void)
:m_LoadSucceed(FALSE)
{
	memset(m_FunctionWithLevel, 0xff, sizeof(LONG) * MAXNUM_FactionPurview);
}

WSOrganizingParam::~WSOrganizingParam(void)
{
}

//得到唯一实例句柄
//该函数不支持多线程操作
WSOrganizingParam& WSOrganizingParam:: getInstance()
{
	return g_OrganizingParam;
}

BOOL	WSOrganizingParam::Initialize()
{
	return ReLoad();
}

void	WSOrganizingParam::Release()
{
}

BOOL	WSOrganizingParam::ReLoad()
{
	m_LoadSucceed = FALSE;
	//! ----------------------------根节点----------------------------
	TiXmlNode *pNode = NULL;

	const char *pFilename = "data/Faction.xml";
	CRFile *prfile = rfOpen(pFilename);
	if(NULL == prfile)
	{
		//输出错误信息
		return FALSE;
	}

	TiXmlDocument m_Tdoc(pFilename);
	//载入技能配置文件
	if( !m_Tdoc.LoadData(prfile->GetData(),prfile->GetDatalen()) )
	{
		//输出错误信息
		rfClose(prfile);
		return FALSE;
	}
	rfClose(prfile);

	pNode=m_Tdoc.FirstChild("Faction");
	if (pNode==NULL) 
		return FALSE;

	TiXmlElement* pFaction = pNode->ToElement();//获取node 的指针
	if (pFaction==NULL) 
		return FALSE;

	//! ----------------------------获取配置----------------------------

	try
	{
		//! 工会全局配置
		TiXmlElement* pFacGlobe = pFaction->FirstChildElement("FacGlobe"); 
		m_tagGlobeSetup.lDemiseTime					= atoi(pFacGlobe->Attribute("DemiseTime"));
		m_tagGlobeSetup.lDisbandFactionTime			= atoi(pFacGlobe->Attribute("auto_disband_term"));
		m_tagGlobeSetup.lNegativeTime				= atoi(pFacGlobe->Attribute("auto_degrade_term"));
		m_tagGlobeSetup.lBaseBattleStart			= atoi(pFacGlobe->Attribute("BaseBattleStart"));
		m_tagGlobeSetup.lBaseBattleLast				= atoi(pFacGlobe->Attribute("BaseBattleLast"));
		m_tagGlobeSetup.byClearBaseTime				= (BYTE)atoi(pFacGlobe->Attribute("ClearBaseTime"));

		//! 帮会相关的等级参数配置
		m_vFacLevelSetup.clear();
		TiXmlElement* pFactionLevel = pFaction->FirstChildElement("FactionLevel");
		for (; pFactionLevel!=NULL; pFactionLevel = pFactionLevel->NextSiblingElement("FactionLevel"))
		{
			tagFacLevelSetup FacLevelSetup;
			FacLevelSetup.lLevel			= atoi(pFactionLevel->Attribute("level"));
			FacLevelSetup.lMaxMemNum		= atoi(pFactionLevel->Attribute("maxMemberNum"));
			FacLevelSetup.lDisbandMemNum	= atoi(pFactionLevel->Attribute("DisbandMemNum"));

			TiXmlElement* pUpgradeCondition = pFactionLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "LeaderLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_LeaderLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else if (0 == stricmp(pTypeTxt, "FacExp"))
				{
					UpgradeCondition.lConditionType = eUCT_FacExp;
				}
				else if (0 == stricmp(pTypeTxt, "BaseNum"))
				{
					UpgradeCondition.lConditionType = eUCT_BaseNum;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));

				FacLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_vFacLevelSetup.push_back(FacLevelSetup);
		}

		//! 帮会经验奖励等级的配置
		m_vFacExpLevelSetup.clear();
		TiXmlElement* pExpLevel = pFaction->FirstChildElement("ExpLevel");
		for (; pExpLevel!=NULL; pExpLevel = pExpLevel->NextSiblingElement("ExpLevel"))
		{
			tagFacExpLevelSetup FacExpLevelSetup;
			FacExpLevelSetup.lLevel		= atoi(pExpLevel->Attribute("level"));
			FacExpLevelSetup.fBaseExp	= atof(pExpLevel->Attribute("baseExp"));

			TiXmlElement* pUpgradeCondition = pExpLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "FacLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_FacLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));
				//添加福利最高等级
				if (UpgradeCondition.lConditionType == eUCT_FacLevel)
				{
					AddLevel(UpgradeCondition.lValue,eUT_ExpLevel,FacExpLevelSetup.lLevel);
				}
				FacExpLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_vFacExpLevelSetup.push_back(FacExpLevelSetup);
		}

		//! 帮会专业等级的配置
		m_vFacDepotLevelSetup.clear();
		TiXmlElement* pDepotLevel = pFaction->FirstChildElement("DepotLevel");
		for (; pDepotLevel!=NULL; pDepotLevel = pDepotLevel->NextSiblingElement("DepotLevel"))
		{
			tagFacDepotLevelSetup FacExpLevelSetup;
			FacExpLevelSetup.lLevel		= atoi(pDepotLevel->Attribute("level"));
			FacExpLevelSetup.lCellNum	= atoi(pDepotLevel->Attribute("CellNum"));

			TiXmlElement* pUpgradeCondition = pDepotLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "FacLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_FacLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));
				//添加专业最高等级
				if (UpgradeCondition.lConditionType == eUCT_FacLevel)
				{
					AddLevel(UpgradeCondition.lValue,eUT_SpecialityLevel,FacExpLevelSetup.lLevel);
				}
				FacExpLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_vFacDepotLevelSetup.push_back(FacExpLevelSetup);
		}

		//! 战争等级配置
		m_SpecialLevelSetup.clear();
		TiXmlElement* pSmithingLevel = pFaction->FirstChildElement("SmithingLevel");
		for (; pSmithingLevel!=NULL; pSmithingLevel = pSmithingLevel->NextSiblingElement("SmithingLevel"))
		{
			tagFacSubLevelSetup FacSubLevelSetup;
			FacSubLevelSetup.lLevel		= atoi(pSmithingLevel->Attribute("level"));

			TiXmlElement* pUpgradeCondition = pSmithingLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "FacLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_FacLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));
				//添加战争最高等级
				if (UpgradeCondition.lConditionType == eUCT_FacLevel)
				{
					AddLevel(UpgradeCondition.lValue,eUT_BattleLevel,FacSubLevelSetup.lLevel);
				}
				FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_SpecialLevelSetup.push_back(FacSubLevelSetup);
		}



		//! 商业等级配置
		m_BussinessLevelSetup.clear();
		TiXmlElement* pMetallurgyLevel = pFaction->FirstChildElement("MetallurgyLevel");
		for (; pMetallurgyLevel!=NULL; pMetallurgyLevel = pMetallurgyLevel->NextSiblingElement("MetallurgyLevel"))
		{
			tagFacSubLevelSetup FacSubLevelSetup;
			FacSubLevelSetup.lLevel		= atoi(pMetallurgyLevel->Attribute("level"));

			TiXmlElement* pUpgradeCondition = pMetallurgyLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "FacLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_FacLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));
				//添加战争最高等级
				if (UpgradeCondition.lConditionType == eUCT_FacLevel)
				{
					AddLevel(UpgradeCondition.lValue,eUT_BussinessLevel,FacSubLevelSetup.lLevel);
				}
				FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_BussinessLevelSetup.push_back(FacSubLevelSetup);
		}

		//! 贵族等级配置
		m_NobleLevelSetup.clear();
		TiXmlElement* pSewingLevel = pFaction->FirstChildElement("SewingLevel");
		for (; pSewingLevel!=NULL; pSewingLevel = pSewingLevel->NextSiblingElement("SewingLevel"))
		{
			tagFacSubLevelSetup FacSubLevelSetup;
			FacSubLevelSetup.lLevel		= atoi(pSewingLevel->Attribute("level"));

			TiXmlElement* pUpgradeCondition = pSewingLevel->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "FacLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_FacLevel;
				}
				else if (0 == stricmp(pTypeTxt, "FacRes"))
				{
					UpgradeCondition.lConditionType = eUCT_FacRes;
				}
				else 
					throw;//! 条件类型错误

				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));
				//添加贵族最高等级
				if (UpgradeCondition.lConditionType == eUCT_FacLevel)
				{
					AddLevel(UpgradeCondition.lValue,eUT_NobleLevel,FacSubLevelSetup.lLevel);
				}
				FacSubLevelSetup.vCondition.push_back(UpgradeCondition);
			}
			m_NobleLevelSetup.push_back(FacSubLevelSetup);
		}

		
		//! 联盟创建条件
		TiXmlElement* pUnionCreate = pFaction->FirstChildElement("UnionCreate");
		{
			m_vUnionUpgradeCondition.clear();

			TiXmlElement* pUpgradeCondition = pUnionCreate->FirstChildElement("UpgradeCondition");
			for (; pUpgradeCondition!=NULL; pUpgradeCondition = pUpgradeCondition->NextSiblingElement())
			{
				tagUpgradeCondition UpgradeCondition;
				//（金钱），Gem（宝石），（会长等级），（任务ID），
				const char *pTypeTxt = pUpgradeCondition->Attribute("type");
				if (0 == stricmp(pTypeTxt, "Money"))
				{
					UpgradeCondition.lConditionType = eUCT_Money;
				}
				else if (0 == stricmp(pTypeTxt, "LeaderLevel"))
				{
					UpgradeCondition.lConditionType = eUCT_LeaderLevel;
				}
				else 
					throw;//! 条件类型错误


				UpgradeCondition.lValue = atoi(pUpgradeCondition->Attribute("value"));

				m_vUnionUpgradeCondition.push_back(UpgradeCondition);
			}
		}

		//! 工会功能开放的等级配置
		TiXmlElement* pLevelFunction = pFaction->FirstChildElement("LevelFunction");
		{
			static char arr_szPurviewType[MAXNUM_FactionPurview][64] = 
			{
				"eFPI_SetAffiche",
				"eFPI_Authorize",
				"eFPI_Fire",
				"eFPI_TakeOut",
				"eFPI_OpenQuest",
				"eFPI_Invite",
				"eFPI_EditTitle",
				"eFPI_EditIcon",
				"eFPI_BeachheadWar",
				"eFPI_ResWar",

				"eFPI_Disband",
				"eFPI_Demise",
				"eFPI_SetMemberPurview",
			};

			TiXmlElement* pLF = pLevelFunction->FirstChildElement("LF");
			for (LONG i = 0; pLF!=NULL || i < MAXNUM_FactionPurview; pLF = pLF->NextSiblingElement("LF"), ++i)
			{
				const char* pTxtType = pLF->Attribute("type");
				for (LONG lFPI = eFPI_SetAffiche; lFPI < MAXNUM_FactionPurview; ++lFPI )
				{
					if (0 == stricmp(arr_szPurviewType[lFPI], pTxtType))
					{
						m_FunctionWithLevel[lFPI] = atoi(pLF->Attribute("level"));
						break;
					}
				}
			}
		}

		TiXmlElement* pConsume=pFaction->FirstChildElement("DailyConsume");
		TiXmlElement* pLv=pConsume->FirstChildElement("Level");
		tagBaseConsume tgConsume;
		LONG lBussLv=0,lBaseVal=0;
		for (;pLv!=NULL;pLv=pLv->NextSiblingElement())
		{
			if (0==stricmp("Level",pLv->Value()) )
			{
				lBussLv=atoi(pLv->Attribute("Lv"));
				tgConsume._BaseVal=atoi(pLv->Attribute("BaseVal"));
				tgConsume._Critical=atoi(pLv->Attribute("Critical"));
				m_BaseConsume[lBussLv]=tgConsume;
			}
			else if (0==stricmp("BusinessLv",pLv->Value()) )
			{
				lBussLv=atoi(pLv->Attribute("Lv"));
				lBaseVal=atoi(pLv->Attribute("BaseVal"));
				m_BussinessBase[lBussLv]=lBaseVal;
			}
		}
	}
	catch (...)
	{
		AddErrorLogText("在读取工会配置的时候发生了错误，读取失败，可能的原因是配置不当。");
		return FALSE;
	}
	

	//! ----------------------------清除对象----------------------------
	m_LoadSucceed = TRUE;
	return m_LoadSucceed;
}

//! 添加到消息缓存
BOOL WSOrganizingParam::CodeToMsgSet(DBWriteSet &DbWriteSet)
{
	if (m_LoadSucceed)
	{
		//! 工会全局配置
		DbWriteSet.AddToByteArray(&m_tagGlobeSetup, sizeof(tagGlobeSetup));

		//! 帮会相关的等级参数配置
		DbWriteSet.AddToByteArray((LONG)m_vFacLevelSetup.size());
		for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_vFacLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray(m_vFacLevelSetup[i].lMaxMemNum);
			DbWriteSet.AddToByteArray(m_vFacLevelSetup[i].lDisbandMemNum);
			DbWriteSet.AddToByteArray((LONG)m_vFacLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_vFacLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_vFacLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}
		//! 联盟创建条件
		DbWriteSet.AddToByteArray((LONG)m_vUnionUpgradeCondition.size());
		for (LONG i = 0; i < m_vUnionUpgradeCondition.size(); ++i)
		{
			DbWriteSet.AddToByteArray(&m_vUnionUpgradeCondition[i], sizeof(tagUpgradeCondition));
		}

		//! 工会功能开放的等级配置;
		DbWriteSet.AddToByteArray(&m_FunctionWithLevel, sizeof(LONG) * MAXNUM_FactionPurview);

		//! 帮会经验奖励等级的配置
		DbWriteSet.AddToByteArray((LONG)m_vFacExpLevelSetup.size());
		for (LONG i = 0; i < m_vFacExpLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_vFacExpLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray(m_vFacExpLevelSetup[i].fBaseExp);
			DbWriteSet.AddToByteArray((LONG)m_vFacExpLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_vFacExpLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_vFacExpLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}
		//! 专业等级的配置
		DbWriteSet.AddToByteArray((LONG)m_vFacDepotLevelSetup.size());
		for (LONG i = 0; i < m_vFacDepotLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_vFacDepotLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray(m_vFacDepotLevelSetup[i].lCellNum);
			DbWriteSet.AddToByteArray((LONG)m_vFacDepotLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_vFacDepotLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_vFacDepotLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}

		//! 商业等级配置
		DbWriteSet.AddToByteArray((LONG)m_BussinessLevelSetup.size());
		for (LONG i = 0; i < m_BussinessLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_BussinessLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray((LONG)m_BussinessLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_BussinessLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_BussinessLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}

		//! 战争等级配置
		DbWriteSet.AddToByteArray((LONG)m_SpecialLevelSetup.size());
		for (LONG i = 0; i < m_SpecialLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_SpecialLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray((LONG)m_SpecialLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_SpecialLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_SpecialLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}

		//! 贵族等级配置
		DbWriteSet.AddToByteArray((LONG)m_NobleLevelSetup.size());
		for (LONG i = 0; i < m_NobleLevelSetup.size(); ++i)
		{
			DbWriteSet.AddToByteArray(m_NobleLevelSetup[i].lLevel);
			DbWriteSet.AddToByteArray((LONG)m_NobleLevelSetup[i].vCondition.size());
			for (LONG j = 0; j < m_NobleLevelSetup[i].vCondition.size(); ++j)
			{
				DbWriteSet.AddToByteArray(&m_NobleLevelSetup[i].vCondition[j], sizeof(tagUpgradeCondition));
			}
		}
		//家族日常消耗
		DbWriteSet.AddToByteArray((long)m_BaseConsume.size());
		map<LONG,tagBaseConsume>::iterator itBase=m_BaseConsume.begin();
		for (;itBase!=m_BaseConsume.end();itBase++)
		{
			DbWriteSet.AddToByteArray(itBase->first);
			DbWriteSet.AddToByteArray(&(itBase->second),sizeof(tagBaseConsume));
		}
		//商业等级对应的基数
		DbWriteSet.AddToByteArray((long)m_BussinessBase.size());
		map<LONG,LONG>::iterator itBussBase=m_BussinessBase.begin();
		for (;itBussBase!=m_BussinessBase.end();itBussBase++)
		{
			DbWriteSet.AddToByteArray(itBussBase->first);
			DbWriteSet.AddToByteArray(itBussBase->second);
		}
	}
	return m_LoadSucceed;
}

//! 获得等级配置
const tagFacLevelSetup* WSOrganizingParam::GetLevelSetup(LONG lLevel)
{
	if (m_LoadSucceed)
	{
		for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
		{
			if (lLevel == m_vFacLevelSetup[i].lLevel)
			{
				return &m_vFacLevelSetup[i];
			}
		}
	}
	
	return NULL;
}

//! 得到一个工会等级开放的所有会员权限
LONG WSOrganizingParam::GetCurrLvlAllPurview(LONG lCurrLvl)
{
	LONG lRe = 0;

	for (LONG i = 0; i < MAXNUM_MemberPurview; ++i)
	{
		if(m_FunctionWithLevel[i] <= lCurrLvl)
		{
			lRe |= (1 << i);
		}
	}

	return lRe;
}

//! 得到一个升级类型的条件
const VecCondition* WSOrganizingParam::GetUpgradeVecCondition(DWORD dwUpgradeType, LONG lAimLevel)
{
	if (m_LoadSucceed)
	{
		switch(dwUpgradeType)
		{
		case eUT_FactionLevel:		//! 工会等级
			{
				for (LONG i = 0; i < m_vFacLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacLevelSetup[i].lLevel)
					{
						return &(m_vFacLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_ExpLevel:			//! 经验福利等级
			{
				for (LONG i = 0; i < m_vFacExpLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacExpLevelSetup[i].lLevel)
					{
						return &(m_vFacExpLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_SpecialityLevel:		//! 专业等级
			{
				for (LONG i = 0; i < m_vFacDepotLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_vFacDepotLevelSetup[i].lLevel)
					{
						return &(m_vFacDepotLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_BattleLevel:		//! 战争等级
			{
				for (LONG i = 0; i < m_SpecialLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_SpecialLevelSetup[i].lLevel)
					{
						return &(m_SpecialLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_BussinessLevel:	//! 商业等级
			{
				for (LONG i = 0; i < m_BussinessLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_BussinessLevelSetup[i].lLevel)
					{
						return &(m_BussinessLevelSetup[i].vCondition);
					}
				}
			}
			break;
		case eUT_NobleLevel:		//! 贵族等级
			{
				for (LONG i = 0; i < m_NobleLevelSetup.size(); ++i)
				{
					if (lAimLevel == m_NobleLevelSetup[i].lLevel)
					{
						return &(m_NobleLevelSetup[i].vCondition);
					}
				}
			}
			break;
		}
	}

	return NULL;
}

void WSOrganizingParam::AddLevel(DWORD dwFacLv,eUpgradeType eType,DWORD dwLv)
{
	map<LONG,tagTechInfo>::iterator it=m_TechInfo.find(dwFacLv);
	if (it!=m_TechInfo.end())
	{
		switch(eType)
		{
		case eUT_ExpLevel:
			it->second._lSubExpLv=max(it->second._lSubExpLv,dwLv);
			break;
		case eUT_SpecialityLevel:													
			it->second._lSpecialityLv=max(it->second._lSpecialityLv,dwLv);
			break;
		case eUT_BattleLevel:	
			it->second._lBattleLv=max(it->second._lBattleLv,dwLv);
			break;
		case eUT_BussinessLevel:	
			it->second._lBussinessLv=max(it->second._lBussinessLv,dwLv);	
			break;
		case eUT_NobleLevel:
			it->second._lNoblelv=max(it->second._lNoblelv,dwLv);
			break;
		}
	}
	else
	{
		tagTechInfo techinfo;
		switch(eType)
		{
		case eUT_ExpLevel:
			techinfo._lSubExpLv=dwLv;
			break;
		case eUT_SpecialityLevel:													
			techinfo._lSpecialityLv=dwLv;
			break;
		case eUT_BattleLevel:	
			techinfo._lBattleLv=dwLv;
			break;
		case eUT_BussinessLevel:	
			techinfo._lBussinessLv=dwLv;	
			break;
		case eUT_NobleLevel:
			techinfo._lNoblelv=dwLv;
			break;
		}
		m_TechInfo[dwFacLv]=techinfo;
	}
}

const tagBaseConsume* WSOrganizingParam::GetConsume(DWORD dwLv)
{
	tagBaseConsume* pC=NULL;
	map<LONG,tagBaseConsume>::iterator it=m_BaseConsume.find(dwLv);
	if (it!=m_BaseConsume.end())
	{
		pC=&(it->second);
	}
	return pC;
}

float WSOrganizingParam::GetBussBase(DWORD dwLv)
{
	LONG lBase=0;
	if(m_BussinessBase.find(dwLv)!=m_BussinessBase.end())
		lBase=m_BussinessBase[dwLv];
	return lBase;
}