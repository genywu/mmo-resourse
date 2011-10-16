#include "StdAfx.h"
#include "PetCtrl.h"
#include "Pet.h"
#include "Player.h"
#include "Monster.h"
#include "DBEntity/EntityGroup.h"
#include "DBEntity/EntityManager.h"
#include "../../Public/ConfigParse.h"
#include "../../NETS/NetWorld/Message.h"



CPetCtrl::CPetCtrl(void)
{
}

CPetCtrl::~CPetCtrl(void)
{
}

// Fox@20081120 宠物配置及属性列表编解码
bool CPetCtrl::LoadConfig(void)
{
	CConfigParse config("Data/PetConfig.xml");
	if( config.IsLoaded() )
	{
		// 功能开关
		config.SetCursor(1, "FuncSwitch");
		config.GetAttr("val", &m_ConfPet.nFuncSwitch, 0);

		// 最大距离
		config.SetNextSibling("MaxDistance");
		config.SetFirstChild("Dis");
		long l = 0;
		do
		{
			config.GetAttr("val", &m_ConfPet.MaxDistances[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();

		// 宠物移动速度
		config.SetNextSibling("Speeds");
		config.SetFirstChild("Speed");
		l=0;
		do
		{
			config.GetAttr("val", &m_ConfPet.Speeds[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();

		// AI触发间隔(目前仅对陷阱适用)
		config.SetNextSibling("AISpringTime");
		config.GetAttr("int", &m_ConfPet.nAISpringTime, 0);

		// HPMP回复时间间隔
		config.SetNextSibling("HPMPAutoRenew");
		config.GetAttr("int", &m_ConfPet.nHPMPInt, 0);

		// 饱食度时间衰减
		config.SetNextSibling("GorgeAutoRenew");
		config.GetAttr("int", &m_ConfPet.nGorgeInt, 0);
		config.SetFirstChild("Gorge");
		l=0;
		do
		{
			config.GetAttr("delta", &m_ConfPet.GorgeAutoRenew[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();

		// 饱食度等级分级
		config.SetNextSibling("Gorges");
		config.GetAttr("max", &m_ConfPet.nMaxGorge, 0);
		config.SetFirstChild("Gorge");
		l=0;
		do
		{
			config.GetAttr("val", &m_ConfPet.Gorges[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lGorgeLvNum = l;

		// 忠诚度等级分级
		config.SetNextSibling("Loyalties");
		config.GetAttr("max", &m_ConfPet.nMaxLoyalty, 0);
		config.SetFirstChild("Loy");
		l=0;
		do
		{
			config.GetAttr("val", &m_ConfPet.Loyalties[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lLoyaltyLvNum = l;

		// 饱食度影响属性
		config.SetNextSibling("GorgeEffs");
		config.SetFirstChild("Eff");
		tagGorge tag;
		l=0;
		do
		{
			config.GetAttr("loy", &tag.nLoyalty, 0);
			config.GetAttr("hpmp", &tag.fHpMp, 0);
			config.GetAttr("foc", &tag.nAbsorb, 0);
			config.GetAttr("int", &tag.nInterval, 0);
			config.GetAttr("miss", &tag.fMiss, 0);
			config.GetAttr("eff", &tag.fEffect, 0);
			m_ConfPet.GorgeEffs[l++] = tag;
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lFeedLvNum = l;

		// 喂食策略
		config.SetNextSibling("Foods");
		config.GetAttr("int", &m_ConfPet.nFooddLvInt, 0);
		config.SetFirstChild("Food");
		l=0;
		do
		{
			config.GetAttr("eff", &m_ConfPet.Feed[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();

		// 天赋技能开启条件
		config.SetNextSibling("GeniusLevels");
		config.GetAttr("imin", &m_ConfPet.nInitGeniusSkillMin, 0);
		config.GetAttr("imax", &m_ConfPet.nInitGeniusSkillMax, 0);
		config.SetFirstChild("Lev");
		tagSkillOpen tagTSO;
		l=0;
		do
		{
			config.GetAttr("lv", &tagTSO.nLevel, 0);
			config.GetAttr("rat", &tagTSO.nRate, 0);
			config.GetAttr("class", &tagTSO.nClass, 0);
			m_ConfPet.GeniusSkillOpen[l++] = tagTSO;
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lGeniusSkillNum = l;

		// 天赋技能列表
		config.SetNextSibling("GeniusSkills");
		config.SetFirstChild("Gen");
		l=0;
		do
		{
			config.GetAttr("id", &m_ConfPet.GeniusSkills[l++], 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lGeniusSkillsSize = l;

		// 插槽技能开启条件
		config.SetNextSibling("HoleSkills");
		config.SetFirstChild("Hol");
		l=0;
		do
		{
			config.GetAttr("lv", &tagTSO.nLevel, 0);
			config.GetAttr("rat", &tagTSO.nRate, 0);
			config.GetAttr("class", &tagTSO.nClass, 0);
			m_ConfPet.StudySkillOpen[l++] = tagTSO;
		}
		while( config.SetNextSibling() );
		config.SetParent();
		m_ConfPet.lStudySkillNum = l;

		// 宠物原始属性换算
		config.SetNextSibling("AttrExchange");
		config.SetFirstChild("Attr");
		double* pExchange = &m_ConfPet.fConsti2MaxHp;
		do
		{
			config.GetAttr("val", pExchange++, 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();
	}
	else
	{
		PutoutLog("Pet", LT_ERROR, "Can't find file Data/PetConfig.xml.");
		return false;
	}

	// 读取宠物经验配置
	string strTemp;
	CRFile* pRFile = rfOpen("Data/PetExp.ini");
	if( NULL == pRFile )
	{
		PutoutLog("Pet", LT_ERROR, "Can't find file Data/PetExp.ini.");
		return false;
	}
	else
	{
		stringstream iFS;
		pRFile->ReadToStream(iFS);
		rfClose(pRFile);

		m_ConfPets.clear();
		m_ConfPet.lMaxLevel = 0;
		while( ReadTo(iFS, "#") )
		{
			iFS >> strTemp
				>> m_ConfPet.Exps[m_ConfPet.lMaxLevel++];
		}
	}

	// 读取各种宠物基本属性配置
	pRFile = rfOpen("Data/PetList.ini");
	if( NULL == pRFile )
	{
		PutoutLog("Pet", LT_ERROR, "Can't find file Data/PetList.ini.");
		return false;
	}
	else
	{
		tagPetBase tPetConf;
		stringstream iFS;
		pRFile->ReadToStream(iFS);
		rfClose(pRFile);

		m_ConfPets.clear();
		long lType = 0;
		while( ReadTo(iFS, "#") )
		{
			iFS >> tPetConf.lIndex
				>> tPetConf.lPetType
				>> tPetConf.lClass
				>> tPetConf.lPicID
				>> tPetConf.szOriName
				>> tPetConf.lExistTime
				>> tPetConf.lInvisible
				>> tPetConf.lMoveType
				>> tPetConf.lSelAtk
				>> tPetConf.lGorge
				>> tPetConf.lLoyalty

				>> tPetConf.lMaxHp
				>> tPetConf.lHpRecoverSpeed
				>> tPetConf.lMaxMp
				>> tPetConf.lMpRecoverSpeed
				>> tPetConf.lStrenth
				>> tPetConf.lAgility
				>> tPetConf.lConsti
				>> tPetConf.lIntell
				>> tPetConf.lWisdom
				>> tPetConf.lSpirit
				>> tPetConf.lCharm
				>> tPetConf.lLuck
				>> tPetConf.lParry
				>> tPetConf.lBlock
				>> tPetConf.lPierce
				>> tPetConf.lAbsorb
				>> tPetConf.lHit
				>> tPetConf.lDodge
				>> tPetConf.lImmunity
				>> tPetConf.lMAtkSpeed
				>> tPetConf.lMinAtk
				>> tPetConf.lMaxAtk
				>> tPetConf.lDef
				>> tPetConf.lMAtk
				>> tPetConf.lMDef
				>> tPetConf.lCri
				>> tPetConf.lCriDef
				>> tPetConf.lMCri
				>> tPetConf.lMCriDef
				>> tPetConf.lWillDef
				>> tPetConf.lElemDef0
				>> tPetConf.lElemDef1
				>> tPetConf.lElemDef2
				>> tPetConf.lElemDef3
				>> tPetConf.lElemDef4
				>> tPetConf.lElemDef5
				>> tPetConf.lSkillNum;
			for (long i=0; i<tPetConf.lSkillNum; ++i)
			{
				iFS >> tPetConf.GeniusSkills[i].lID
					>> tPetConf.GeniusSkills[i].lLevel;
			}

			m_ConfPets[tPetConf.lIndex] = tPetConf;
		}
	}

	// 读取宠物成长配置
	pRFile = rfOpen("Data/PetGrow.ini");
	if( NULL == pRFile )
	{
		PutoutLog("Pet", LT_ERROR, "Can't find file Data/PetGrow.ini.");
		return false;
	}
	else
	{
		stringstream iFS;
		pRFile->ReadToStream(iFS);
		rfClose(pRFile);

		m_GrowConfPets.clear();
		long lPrevID = 0;
		long lCurID = 0;
		tagPetOrig tPetOrig;
		vector<tagPetOrig> vecPetOrig;
		while( ReadTo(iFS, "*") )
		{
			iFS >> lCurID
				>> tPetOrig.lLevel
				>> tPetOrig.fMaxHp
				>> tPetOrig.fMaxMp
				>> tPetOrig.fStrenth
				>> tPetOrig.fAgility
				>> tPetOrig.fConsti
				>> tPetOrig.fIntell
				>> tPetOrig.fWisdom
				>> tPetOrig.fSpirit
				>> tPetOrig.fCharm
				>> tPetOrig.fLuck;
			if( lCurID != lPrevID )
			{
				m_GrowConfPets[lPrevID] = vecPetOrig;
				lPrevID = lCurID;
				vecPetOrig.clear();
			}
			vecPetOrig.push_back(tPetOrig);
		}
		m_GrowConfPets[lCurID] = vecPetOrig;
	}

	return true;
}

// 配置编码
bool CPetCtrl::CodeToDataBlock(DBWriteSet& setWriteDB)
{
	setWriteDB.AddToByteArray(&m_ConfPet, sizeof(m_ConfPet));
	long lNum = (long)m_ConfPets.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_PET_LIST itr = m_ConfPets.begin();
	for( long l=0; l<lNum; ++l, ++itr )
	{
		setWriteDB.AddToByteArray(&itr->second, sizeof(itr->second));
	}

	lNum = (long)m_GrowConfPets.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_GROW_CONF itrG = m_GrowConfPets.begin();
	for( ; itrG != m_GrowConfPets.end(); ++itrG )
	{
		setWriteDB.AddToByteArray(itrG->first);
		GROW_LIST& rList = itrG->second;
		lNum = (long)rList.size();
		setWriteDB.AddToByteArray(lNum);
		for( long m=0; m<lNum; ++m )
		{
			setWriteDB.AddToByteArray(&rList[m], sizeof(tagPetOrig));
		}
	}

	return true;
}

// WS==>DB编码
bool CPetCtrl::CodeToDataBlock(CPlayer* pPlayer,
							   CEntityGroup* pDBPetList)
{
	char szLog[512];
	char szGUID[128];
	CPet* pPet = NULL;
	CBaseEntity* pDBPet = NULL;
	C_PETS rPets = pPlayer->GetPets();
	C_ITR_PET itrPet = rPets.begin();
	for( ; itrPet!=rPets.end(); ++itrPet )
	{
		if( NULL == (pPet = itrPet->second) )
		{
			itrPet->first.tostring(szGUID);
			sprintf(szLog, "异常: 玩家%s没有GUID为%s的宠物.",
				pPlayer->GetName(),
				szGUID);
			PutoutLog("Pet", LT_ERROR, szLog);
			continue;
		}

		if( NOT_BE_SAVED == pPet->m_TempAttrs.cSaveFlag )
		{
			continue;
		}

		pDBPet = GetGame()->GetEntityManager()->NewBaseEntity(string("[pet]"), pPet->GetExID());
		pDBPet->SetCurDbOperFlag(1);
		pDBPet->SetCurDbOperType(DB_OPER_DELETE_INSERT);

		pDBPet->SetGuidAttr("ID", 
			pPet->GetExID());
		pDBPet->SetLongAttr("Idx", 
			pPet->m_DiffAttrs.lIndex);
		pDBPet->SetStringAttr("Name", 
			pPet->m_DiffAttrs.szName);
		pDBPet->SetGuidAttr("HostID", 
			pPet->m_HostID);
		pDBPet->SetStringAttr("HostName", 
			pPet->m_DiffAttrs.szHostName);
		pDBPet->SetLongAttr("Active", 
			pPet->m_DiffAttrs.lActive);
		pDBPet->SetLongAttr("Pos", 
			pPet->m_DiffAttrs.lPos);
		pDBPet->SetLongAttr("Lvl", 
			pPet->m_DiffAttrs.lLevel);
		pDBPet->SetLongAttr("Exp", 
			pPet->m_DiffAttrs.lExp);
		pDBPet->SetLongAttr("HP", 
			pPet->m_DiffAttrs.lHp);
		pDBPet->SetLongAttr("MP", 
			pPet->m_DiffAttrs.lMp);
		pDBPet->SetLongAttr("NameChgTimes", 
			pPet->m_DiffAttrs.lNameChgTimes);
		pDBPet->SetLongAttr("Gorge", 
			pPet->m_DiffAttrs.lGorge);
		pDBPet->SetLongAttr("Loyalty", 
			pPet->m_DiffAttrs.lLoyalty);
		pDBPet->SetLongAttr("SP", 
			pPet->m_DiffAttrs.lSP);
		pDBPet->SetTimeAttr("Birthday", 
			pPet->m_DiffAttrs.tBirthday,
			sizeof(pPet->m_DiffAttrs.tBirthday));

		// 技能存储
		SKILL_LIST& rSkills = pPet->m_Skills;
		long lSkillNum = (long)rSkills.size();
		long lSize = lSkillNum * sizeof(tagSkill) + sizeof(long);
		BYTE* pBuf = (BYTE*)M_ALLOC(lSize);
		memcpy(pBuf, &lSkillNum, sizeof(long));
		ITR_SKILL_LIST itrSkill = rSkills.begin();
		long lCurTime = timeGetTime();
		long lAbCurTime = (long)time(NULL);
		long lRestTime = 0;
		for( long i=0; itrSkill != rSkills.end(); ++itrSkill, ++i )
		{
			lRestTime = itrSkill->second.lEndCooldown - lCurTime;
			if( lRestTime > 2000 )
			{
				itrSkill->second.lEndCooldown = lAbCurTime + lRestTime;
			}
			else
			{
				itrSkill->second.lEndCooldown = 0;
			}
			memcpy(pBuf+sizeof(long)+sizeof(tagSkill)*i, &(itrSkill->second), sizeof(tagSkill));
		}
		pDBPet->SetBufAttr("Skills",  pBuf, lSize);
		M_FREE(pBuf,lSize);

		pDBPetList->AddChild(pDBPet);
	}

	return true;
}

// DB==>WS解码
// 将宠物加入宠物管理序列, 并初始化玩家宠物列表
bool CPetCtrl::DecodeFromDataBlock(CEntityGroup* pDBPetList,
								   CPlayer* pPlayer)
{
	char szLog[512];
	char szGUID[128];
	CGUID guID;
	CPet* pPet = NULL;
	CEntityProperty* pEP = NULL;
	CBaseEntity* pDBPet = NULL;
	pPlayer->ReleaseAllPets();	// 清空宠物列表
	EntityGroupMap& rDBPetList = pDBPetList->GetEntityGroupMap();
	EntityGroupMapItr itrPet = rDBPetList.begin();
	for( ; itrPet!= rDBPetList.end(); ++itrPet )
	{
		pDBPet = itrPet->second;
		if( NULL == pDBPet )
		{
			guID.tostring(szGUID);
			sprintf(szLog, "异常: 玩家%s宠物数据为NULL.",
				pPlayer->GetName());
			PutoutLog("Pet", LT_ERROR, szLog);
			return false;
		}

		pDBPet->GetGuidAttr("ID",  guID);
		pPet = pPlayer->CreatePet(guID);

		// 设置宠物数据库属性
		// 创建宠物
		pPet->m_DiffAttrs.lIndex = pDBPet->GetLongAttr("Idx");
		strcpy(pPet->m_DiffAttrs.szName,
			pDBPet->GetStringAttr("Name"));
		pPet->SetName(pPet->m_DiffAttrs.szName);
		pDBPet->GetGuidAttr("HostID",  pPet->m_HostID);
		strcpy(pPet->m_DiffAttrs.szHostName,
			pDBPet->GetStringAttr("HostName"));
		pPet->m_DiffAttrs.lActive		= pDBPet->GetLongAttr("Active");
		pPet->m_DiffAttrs.lPos			= pDBPet->GetLongAttr("Pos");
		pPet->m_DiffAttrs.lLevel		= pDBPet->GetLongAttr("Lvl");
		pPet->m_DiffAttrs.lExp			= pDBPet->GetLongAttr("Exp");
		pPet->m_DiffAttrs.lHp			= pDBPet->GetLongAttr("HP");
		pPet->m_DiffAttrs.lMp			= pDBPet->GetLongAttr("MP");
		pPet->m_DiffAttrs.lNameChgTimes	= pDBPet->GetLongAttr("NameChgTimes");
		pPet->m_DiffAttrs.lGorge		= pDBPet->GetLongAttr("Gorge");
		pPet->m_DiffAttrs.lLoyalty		= pDBPet->GetLongAttr("Loyalty");
		pPet->m_DiffAttrs.lSP			= pDBPet->GetLongAttr("SP");
		pDBPet->GetTimeAttr("Birthday", 
			pPet->m_DiffAttrs.tBirthday, 
			sizeof(pPet->m_DiffAttrs.tBirthday));

		// 技能加载
		string strBuf = "Skills"; 
		long lAttrEnum = pDBPet->GetBufSize(strBuf);
		if( lAttrEnum > 0 )
		{
			BYTE* pBuf = (BYTE*)M_ALLOC(lAttrEnum);
			pDBPet->GetBufAttr(strBuf, pBuf, lAttrEnum);
			SKILL_LIST& rSkills = pPet->m_Skills;
			rSkills.clear();
			long lSkillNum = (long)pBuf[0];
			tagSkill tSkill;
			long lCurTime = timeGetTime();
			long lAbCurTime = (long)time(NULL);
			long lRestTime = 0;
			for( long i=0; i<lSkillNum; ++i )
			{
				memcpy(&tSkill, pBuf+sizeof(long)+sizeof(tagSkill)*i, sizeof(tagSkill));
				lRestTime = tSkill.lEndCooldown - lAbCurTime;
				if( lRestTime > 0 )
				{
					tSkill.lEndCooldown = lCurTime + lRestTime;
				}
				else
				{
					tSkill.lEndCooldown = 0;
				}
				rSkills[i] = tSkill;
			}
			M_FREE(pBuf,lAttrEnum);
		}
	}

	return true;
}

// WS==>GS编码
bool CPetCtrl::CodeToDataBlock(DBWriteSet& setWriteDB,
							   CPlayer* pPlayer)
{
	CONDITION_CHECK(m_ConfPet.nFuncSwitch);

	char szLog[512];
	char szGUID[128];
	CPet* pPet = NULL;
	C_PETS rPets = pPlayer->GetPets();
	C_ITR_PET itrPet = rPets.begin();
	long lNum = (long)rPets.size();
	setWriteDB.AddToByteArray(lNum);
	for( ; itrPet != rPets.end(); ++itrPet )
	{
		if( NULL == (pPet = itrPet->second) )
		{
			setWriteDB.AddToByteArray(char(0));
			itrPet->first.tostring(szGUID);
			sprintf(szLog, "异常: 玩家%s没有GUID为%s的宠物.",
				pPlayer->GetName(),
				szGUID);
			PutoutLog("Pet", LT_ERROR, szLog);
			continue;
		}
		setWriteDB.AddToByteArray(char(1));
		pPet->CodeToDataBlock(setWriteDB);
	}
	return true;
}

// GS==>WS解码
bool CPetCtrl::DecodeFromDataBlock(DBReadSet& setReadDB,
								   CPlayer* pPlayer)
{
	CONDITION_CHECK(m_ConfPet.nFuncSwitch);

	pPlayer->ReleaseAllPets();	// 清空宠物列表
	long lNum = setReadDB.GetLongFromByteArray();
	if( 0 == lNum ) return true;
	CGUID guID;
	char cFlag = 0;
	CPet* pPet = NULL;
	for( long i=0; i<lNum; ++i )
	{
		cFlag = setReadDB.GetCharFromByteArray();
		if( 0 == cFlag )
			continue;

		pPet = pPlayer->CreatePet(NULL_GUID);
		pPet->DecodeFromDataBlock(setReadDB);
		pPlayer->AddPet(pPet);
	}

	return true;
}