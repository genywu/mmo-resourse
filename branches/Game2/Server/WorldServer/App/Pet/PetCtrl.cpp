#include "StdAfx.h"
#include "PetCtrl.h"
#include "Pet.h"
#include "../Player.h"
#include "../Monster.h"
#include "../DBEntity/EntityGroup.h"
#include "../DBEntity/EntityManager.h"
#include "../../../../Public/Common/PetSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPetCtrl::CPetCtrl(void)
{
}

CPetCtrl::~CPetCtrl(void)
{
}

// Fox@20081120 宠物配置及属性列表编解码
bool CPetCtrl::LoadConfig(void)
{
	// 读取宠物基本配置
	if (!LoadPetBaseConfig())
		return false;

	// 读取宠物强化材料配置
	if (!GetInst(PetSetup).LoadPetMaterialConfig())
		return false;

	// 读取宠物经验配置
	if (!LoadPetExpConfig())
		return false;

	// 读取各种宠物基本属性配置
	if (!LoadPetList())
		return false;

	// 读取宠物成长配置
	if (!LoadPetGrowList())
		return false;

	return true;
}

bool CPetCtrl::LoadPetBaseConfig(void)
{
	ConfigParse config("Data/PetConfig.xml");
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

		// HP\MP\内力自动回复间隔
		config.SetNextSibling("HPMPAutoRenew");
		config.GetAttr("int", &m_ConfPet.nHPMPInt, 0);

		// 忠诚度自动回复间隔
		config.SetNextSibling("LoyaltyAutoRenew");
		config.GetAttr("int", &m_ConfPet.nLoyaltyInt, 0);

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

		// 忠诚度等级分级
		config.SetNextSibling("Loyalties");
		config.GetAttr("max", &m_ConfPet.loyaltyMax, 0);
		config.GetAttr("OutMin", &m_ConfPet.loyaltyMin, 0);
		config.GetAttr("int", &m_ConfPet.loyaDownUnit, 0);
		config.GetAttr("AutoRenewdelta", &m_ConfPet.loyaDownVal, 0);
		config.GetAttr("UpGrageAdd", &m_ConfPet.loyaUpGrageAdd, 0);
		config.GetAttr("TradeDownRate", &m_ConfPet.loyaTradeRate, 0);
		config.GetAttr("DieDownRate", &m_ConfPet.loyaDieRate, 0);
		config.SetFirstChild("Loy");
		l=0;
		do
		{
			config.GetAttr("val", &m_ConfPet.loyalties[l], 0);
			config.GetAttr("Expdelta", &m_ConfPet.layaExp[l], 0);
			config.GetAttr("Transrate", &m_ConfPet.layaHurt[l], 0);
			l++;
		}
		while(config.SetNextSibling());
		m_ConfPet.loyaltyLvNum = l;
		config.SetParent();

		// 宠物原始属性换算
		config.SetNextSibling("AttrExchange");
		config.SetFirstChild("Attr");
		double* pExchange = &m_ConfPet.fStrenth2Hit;
		do
		{
			config.GetAttr("val", pExchange++, 0);
		}
		while( config.SetNextSibling() );
		config.SetParent();

		// 侍从品质（以千分之作为分母）
		config.SetNextSibling("Quality");
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			config.GetAttr("rate", &m_ConfPet.Quality[l++], 0);
		} while(config.SetNextSibling());
		config.SetParent();

		// 品级对应融合系数
		config.SetNextSibling("Grade");
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			config.GetAttr("syncretize", &m_ConfPet.syncretismRate[l++], 0);
		} while(config.SetNextSibling());
		m_ConfPet.gradeNum = l;
		config.SetParent();

		// 侍从特性偏好每级额外提升点
		config.SetNextSibling("Speciality");
		config.SetFirstChild("Nature");
		l=0;
		do 
		{
			config.GetAttr("val", &m_ConfPet.Speciality[l++], 0);
		} while(config.SetNextSibling());
		config.SetParent();

		// 聊天泡泡

		// 侍从公共配置
		config.SetNextSibling("Common");
		config.GetAttr("MoreHostLevel", &m_ConfPet.moreHostLevel, 0);
		config.GetAttr("MaxTake", &m_ConfPet.maxTakeNum, 0);
		config.GetAttr("MaxTake", &m_ConfPet.maxExpMultiple, 0);
		config.GetAttr("MaxTake", &m_ConfPet.maxDExpMultiple, 0);

		// 侍从融合配置
		config.SetNextSibling("Syncretism");
		config.GetAttr("MinLev", &m_ConfPet.syncretismMinLev, 0);
		config.GetAttr("MaxCount", &m_ConfPet.syncretismMaxCount, 0);
		config.GetAttr("Den", &m_ConfPet.syncretismMaxDen, 0);

		// 侍从寿命
		config.SetNextSibling("Life");
		config.GetAttr("MaxLife", &m_ConfPet.maxLife, 0);
		config.GetAttr("SubRat", &m_ConfPet.lifeSubRate, 0);
		config.GetAttr("Timer", &m_ConfPet.lifeSubUnit, 0);
		config.GetAttr("DieDownRate", &m_ConfPet.lifeDieDownRate, 0);

		// 侍从经验、修为配置
		config.SetNextSibling("Exps");
		config.GetAttr("NormalExp", &m_ConfPet.normalExp, 0);
		config.GetAttr("NormalDExp", &m_ConfPet.normalDExp, 0);
		config.GetAttr("BaseLevDiff", &m_ConfPet.expsBaseLevDiff, 0);
		config.GetAttr("MaxLevDiff", &m_ConfPet.expsMaxLevDiff, 0);
		config.SetFirstChild("ExpRate");
		l=0;
		do 
		{
			config.GetAttr("ExpRate", &m_ConfPet.levDiffExpRate[l], 0);
			config.GetAttr("DExpRate", &m_ConfPet.levDiffDExpRate[l], 0);
			l++;
		} while(config.SetNextSibling());
		config.SetParent();


		// 侍从阴阳八卦阵配置
		config.SetNextSibling("Lunar");
		config.GetAttr("MaxLunarUseCnt", &m_ConfPet.lunarMaxUseCnt, 0);
		config.GetAttr("MaxLunarUpCnt", &m_ConfPet.lunarMaxUpCnt, 0);
		config.GetAttr("LunarBaseNum", &m_ConfPet.lunarBase, 0);
		config.GetAttr("ReversionRate", &m_ConfPet.lunarReverRate, 0);
		config.GetAttr("Timer", &m_ConfPet.lunarReverUnit, 0);

		// 侍从强化概率配置
		config.SetNextSibling("Intensify");

		// 天资随机算法
		config.SetFirstChild("TalentVal");
		config.GetAttr("min", &m_ConfPet.talentMinVal, 0);
		config.GetAttr("max", &m_ConfPet.talentMaxVal, 0);
		config.GetAttr("ResetLev", &m_ConfPet.talentResetLev, 0);
		config.GetAttr("Numismatics", &m_ConfPet.talentNumismatics, 0);
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			config.GetAttr("rate", &m_ConfPet.talentAlogRate[l][0], 0);
			config.GetAttr("fminval", &m_ConfPet.talentAlogRate[l][1], 0);
			config.GetAttr("fmaxval", &m_ConfPet.talentAlogRate[l][2], 0);
			l++;
		} while(config.SetNextSibling());
		config.SetParent();

		// 品质强化
		config.SetNextSibling("Quality");
		config.GetAttr("MinLevel", &m_ConfPet.qualityMinLev, 0);
		config.GetAttr("Loyalties", &m_ConfPet.qualityLoyaProp, 0);
		config.GetAttr("DropRate", &m_ConfPet.qualityDropProp, 0);
		config.GetAttr("CostLevExp", &m_ConfPet.qualityExpProp, 0);
		config.GetAttr("MaxCostGoods", &m_ConfPet.qualityMaxCostGoodsCnt, 0);
		config.GetAttr("Numismatics", &m_ConfPet.qualityNumismatics, 0);
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			config.GetAttr("rate", &m_ConfPet.qualitySuccessRate[l++], 0);
		} while(config.SetNextSibling());
		config.SetParent();

		// 魂石强化
		config.SetNextSibling("FStone");
		config.GetAttr("MinLevel", &m_ConfPet.fStoneMinLev, 0);
		config.GetAttr("MaxLevel", &m_ConfPet.fStoneMaxLev, 0);
		config.GetAttr("CostLevExp", &m_ConfPet.fStoneExpProp, 0);
		config.GetAttr("FStoneNum", &m_ConfPet.fStoneN, 0);
		config.GetAttr("Numismatics", &m_ConfPet.fStoneNumismatics, 0);
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			//config.GetAttr("lev", &m_ConfPet.fStoneSuccesRate[l][0], 0);
			config.GetAttr("rate", &m_ConfPet.fStoneSuccesRate[l++], 0);
			//l++;
		} while(config.SetNextSibling());
		config.SetParent();

		// 顿悟神位
		config.SetNextSibling("ExclamaLev");
		config.GetAttr("MinLevel", &m_ConfPet.exclamaMinLev, 0);
		config.GetAttr("Loyalties", &m_ConfPet.exclamaLoyaProp, 0);
		config.GetAttr("DropRate", &m_ConfPet.exclamaDropProp, 0);
		config.GetAttr("CostLevExp", &m_ConfPet.exclamaExpProp, 0);
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			//config.GetAttr("lev", &m_ConfPet.exclamaSuccesRate[l][0], 0);
			config.GetAttr("rate", &m_ConfPet.exclamaSuccesRate[l++], 0);
			//l++;
		} while(config.SetNextSibling());
		config.SetParent();

		// 星等强化
		config.SetNextSibling("StarLev");
		config.GetAttr("MinLev", &m_ConfPet.starMinLev, 0);
		config.GetAttr("MaxLev", &m_ConfPet.starMaxLev, 0);
		config.GetAttr("Numismatics", &m_ConfPet.starNumismatics, 0);
		config.SetFirstChild("Eff");
		l=0;
		do 
		{
			//config.GetAttr("lev", &m_ConfPet.starSuccessRate[l][0], 0);
			config.GetAttr("rate", &m_ConfPet.starSuccessRate[l], 0);
			config.GetAttr("DropValue", &m_ConfPet.starDropVal[l], 0);
			l++;
		} while(config.SetNextSibling());
		config.SetParent();

		// 后天资质强化
		config.SetNextSibling("DAfterStart");
		config.GetAttr("DAfterStartValue", &m_ConfPet.dAfterStartRate, 0);
		config.GetAttr("DAfterDropRate", &m_ConfPet.dAfterDropRate, 0);
		config.GetAttr("MaxValue", &m_ConfPet.dAfterMaxVal, 0);
		config.GetAttr("Numismatics", &m_ConfPet.dAfterNumismatics, 0);

		// 重置后天资质强化次数
		config.SetNextSibling("ReSetDAfterCnt");
		config.GetAttr("Numismatics", &m_ConfPet.ReSetDAfterNumismatics, 0);

		// 悟性点
		config.SetNextSibling("Savvy");
		config.GetAttr("savvyCnt", &m_ConfPet.savvyCnt, 0);
		config.GetAttr("Numismatics", &m_ConfPet.savvyNumismatics, 0);
		config.SetParent();
		return true;
	}
	else
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Can't find file Data/PetConfig.xml.");
		return false;
	}
}

bool CPetCtrl::LoadPetExpConfig(void)
{
	string strTemp;
	CRFile* pRFile = rfOpen("Data/PetExp.ini");
	if( NULL == pRFile )
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Can't find file Data/PetExp.ini.");
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
				>> m_ConfPet.Exps[m_ConfPet.lMaxLevel]
				>> m_ConfPet.DExps[m_ConfPet.lMaxLevel];
			m_ConfPet.lMaxLevel++;
		}
	}
	return true;
}

bool CPetCtrl::LoadPetList(void)
{
	CRFile* pRFile = rfOpen("Data/PetList.ini");
	if( NULL == pRFile )
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Can't find file Data/PetList.ini.");
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
			iFS >> tPetConf.lIndex						// 索引编号
				>> tPetConf.lPetType					// 宠物类型
				>> tPetConf.lClass						// 宠物职业
				>> tPetConf.lPicID						// 图档编号
				>> tPetConf.szOriName					// 原始名
				>> tPetConf.lExistTime					// 生存时间
				>> tPetConf.lInvisible					// 隐形类型
				>> tPetConf.lMoveType					// 移动类型
				>> tPetConf.lSelAtk						// 被选择/被攻击
				>> tPetConf.lGrade						// 品级
				>> tPetConf.lLife						// 寿命
				>> tPetConf.lLoyalty					// 忠诚度
				>> tPetConf.lRace						// 种族
				>> tPetConf.lTakeLevel					// 携带等级

				>> tPetConf.lMaxHp						// 生命
				>> tPetConf.lHpRecoverSpeed				// 基本生命回复速度
				>> tPetConf.lMaxMp						// 法力
				>> tPetConf.lMpRecoverSpeed				// 基本法力回复速度
				>> tPetConf.lMaxEnergy					// 内力
				>> tPetConf.lEnergyRecoverSpeed			// 基本内力回复速度
				>> tPetConf.lStrenth					// 力量
				>> tPetConf.lDexterity					// 身法
				>> tPetConf.lCon						// 根骨
				>> tPetConf.lIntellect					// 意志
				>> tPetConf.lSpiritualism				// 灵性
				>> tPetConf.lHit						// 命中
				>> tPetConf.lDodge						// 闪避
				>> tPetConf.lMAtkSpeed					// 吟唱速度
				>> tPetConf.lPhysicsAtk					// 物理伤害
				>> tPetConf.lPhysicsDef					// 物理防御
				>> tPetConf.lMAtk						// 魔法攻击
				>> tPetConf.lMDef						// 魔法防御
				>> tPetConf.lMinStrenth					// 力量天资下限
				>> tPetConf.lMaxStrenth					// 力量天资上限
				>> tPetConf.lMinDexterity				// 身法天资下限
				>> tPetConf.lMaxDexterity				// 身法天资上限
				>> tPetConf.lMinCon						// 根骨天资下限
				>> tPetConf.lMaxCon						// 根骨天资上限
				>> tPetConf.lMinIntellect				// 意志天资下限
				>> tPetConf.lMaxIntellect				// 意志天资上限
				>> tPetConf.lMinSpiritualism			// 灵性天资下限
				>> tPetConf.lMaxSpiritualism			// 灵性天资上限
				>> tPetConf.lNominateMode;				// 推荐加点

			for (int i=0; i<PET_QUALITY_NUM; ++i)
			{
				iFS >> tPetConf.lQualityRate[i];		// 品质机率
			}
			iFS	>> tPetConf.lElemDef0					// 水抗
				>> tPetConf.lElemDef1					// 土抗
				>> tPetConf.lElemDef2					// 木抗
				>> tPetConf.lElemDef3					// 金抗
				>> tPetConf.lElemDef4					// 火抗
				>> tPetConf.GeniusSkill.lID				// 天赋技能ID
				>> tPetConf.GeniusSkill.lLevel			// 天赋技能等级
				>> tPetConf.OrdinarilySkill.lID			// 普通技能ID
				>> tPetConf.OrdinarilySkill.lLevel;		// 普通技能等级
			for (int j=0; j<MAX_HOLE_SKILL; ++j)
			{
				iFS >> tPetConf.lSkillHole[j];			// 技能槽数量
			}

			m_ConfPets[tPetConf.lIndex] = tPetConf;
		}
	}
	return true;
}

bool CPetCtrl::LoadPetGrowList(void)
{
	CRFile* pRFile = rfOpen("Data/PetGrow.ini");
	if( NULL == pRFile )
	{
		Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Can't find file Data/PetGrow.ini.");
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
				>> tPetOrig.fMaxEnergy				// 内力
				>> tPetOrig.fStrenth				// 力量
				>> tPetOrig.fDexterity				// 身法
				>> tPetOrig.fCon					// 根骨
				>> tPetOrig.fIntellect				// 意志
				>> tPetOrig.fSpiritualism;			// 灵性
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
	// 宠物系统统一配置
	setWriteDB.AddToByteArray(&m_ConfPet, sizeof(m_ConfPet));

	// 宠物列表配置
	long lNum = (long)m_ConfPets.size();
	setWriteDB.AddToByteArray(lNum);
	ITR_PET_LIST itr = m_ConfPets.begin();
	for( long l=0; l<lNum; ++l, ++itr )
	{
		setWriteDB.AddToByteArray(&itr->second, sizeof(itr->second));
	}

	// 宠物成长
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

	// 侍从强化配置
	GetInst(PetSetup).CodeToDataBlock(setWriteDB);

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
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, szLog);
			continue;
		}

		pDBPet = GetGame().GetEntityManager()->NewBaseEntity(string("[pet]"), pPet->GetExID());
		pDBPet->SetCurDbOperFlag(1);
		pDBPet->SetCurDbOperType(DB_OPER_DELETE_INSERT);

		pDBPet->SetGuidAttr("ID", pPet->GetExID());
		pDBPet->SetLongAttr("Idx", pPet->m_DiffAttrs.lIndex);
		pDBPet->SetStringAttr("Name", pPet->m_DiffAttrs.szName);
		pDBPet->SetGuidAttr("HostID", pPet->m_HostID);
		pDBPet->SetStringAttr("HostName", pPet->m_DiffAttrs.szHostName);
		pDBPet->SetLongAttr("StateType", pPet->m_DiffAttrs.lStateType);
		pDBPet->SetLongAttr("Pos", pPet->m_DiffAttrs.lPos);
		pDBPet->SetLongAttr("Lev", pPet->m_DiffAttrs.lLevel);
		pDBPet->SetLongAttr("Exp", pPet->m_DiffAttrs.lExp);
		pDBPet->SetLongAttr("HP", pPet->m_DiffAttrs.lHp);
		pDBPet->SetLongAttr("MP", pPet->m_DiffAttrs.lMp);
		pDBPet->SetLongAttr("Energy",pPet->m_DiffAttrs.lEnergy);
		pDBPet->SetLongAttr("Life",pPet->m_DiffAttrs.lLife);
		pDBPet->SetLongAttr("NameChgTimes", pPet->m_DiffAttrs.lNameChgTimes);
		pDBPet->SetLongAttr("Loyalty", pPet->m_DiffAttrs.lLoyalty);

		// 侍从专用存储属性
		pDBPet->SetLongAttr("DLvl",pPet->m_DiffAttrs.ServantAttr.lDLevel);
		pDBPet->SetLongAttr("DExp",pPet->m_DiffAttrs.ServantAttr.lDExp);
		pDBPet->SetLongAttr("Wild",pPet->m_DiffAttrs.ServantAttr.lWild);
		pDBPet->SetLongAttr("Ages",pPet->m_DiffAttrs.ServantAttr.lAges);
		pDBPet->SetLongAttr("Quality", pPet->m_DiffAttrs.ServantAttr.lQuality);
		pDBPet->SetLongAttr("Speciality", pPet->m_DiffAttrs.ServantAttr.lSpeciality);
		pDBPet->SetLongAttr("DAfterValCnt", pPet->m_DiffAttrs.ServantAttr.lDAfterValCount);
		pDBPet->SetLongAttr("FStone", pPet->m_DiffAttrs.ServantAttr.lFStoneUpgradeFlag);
		pDBPet->SetLongAttr("DStone", pPet->m_DiffAttrs.ServantAttr.lFUpgradeCount);
		pDBPet->SetLongAttr("StarLev",pPet->m_DiffAttrs.ServantAttr.lStarLev);
		pDBPet->SetLongAttr("StarCnt",pPet->m_DiffAttrs.ServantAttr.lStarLevCount);

#define PET_ATTR_DB(TYPE) \
	pDBPet->SetLongAttr(#TYPE"Strenth", pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lStrenth); \
	pDBPet->SetLongAttr(#TYPE"Dexterity", pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lDexterity); \
	pDBPet->SetLongAttr(#TYPE"Con", pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lCon); \
	pDBPet->SetLongAttr(#TYPE"Intellect", pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lIntellect); \
	pDBPet->SetLongAttr(#TYPE"Spiritualism", pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lSpiritualism);

		PET_ATTR_DB(talent);
		PET_ATTR_DB(dAfter);
		PET_ATTR_DB(syncretize);
		PET_ATTR_DB(stoneUp);
		PET_ATTR_DB(savvy);

		// 技能存储
		SKILL_LIST& rSkills = pPet->m_Skills;
		long lSkillNum = (long)rSkills.size();
		long lSize = lSkillNum * sizeof(tagSkill) + sizeof(long);
		BYTE* pBuf = new BYTE[lSize];
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
		SAFE_DELETE_ARRAY(pBuf);

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
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, szLog);
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
		pPet->m_DiffAttrs.lStateType	= pDBPet->GetLongAttr("StateType");
		pPet->m_DiffAttrs.lPos			= pDBPet->GetLongAttr("Pos");
		pPet->m_DiffAttrs.lLevel		= pDBPet->GetLongAttr("Lev");
		pPet->m_DiffAttrs.lExp			= pDBPet->GetLongAttr("Exp");
		pPet->m_DiffAttrs.lHp			= pDBPet->GetLongAttr("HP");
		pPet->m_DiffAttrs.lMp			= pDBPet->GetLongAttr("MP");
		pPet->m_DiffAttrs.lEnergy = pDBPet->GetLongAttr("Energy");
		pPet->m_DiffAttrs.lLife = pDBPet->GetLongAttr("Life");
		pPet->m_DiffAttrs.lNameChgTimes	= pDBPet->GetLongAttr("NameChgTimes");
		pPet->m_DiffAttrs.lLoyalty		= pDBPet->GetLongAttr("Loyalty");


		// 侍从专用存储属性
		pPet->m_DiffAttrs.ServantAttr.lDLevel = pDBPet->GetLongAttr("DLvl");
		pPet->m_DiffAttrs.ServantAttr.lDExp = pDBPet->GetLongAttr("DExp");
		pPet->m_DiffAttrs.ServantAttr.lWild = pDBPet->GetLongAttr("Wild");
		pPet->m_DiffAttrs.ServantAttr.lAges = pDBPet->GetLongAttr("Ages");
		pPet->m_DiffAttrs.ServantAttr.lQuality = pDBPet->GetLongAttr("Quality");
		pPet->m_DiffAttrs.ServantAttr.lSpeciality = pDBPet->GetLongAttr("Speciality");
		pPet->m_DiffAttrs.ServantAttr.lDAfterValCount = pDBPet->GetLongAttr("DAfterValCnt");
		pPet->m_DiffAttrs.ServantAttr.lFStoneUpgradeFlag = pDBPet->GetLongAttr("FStone");
		pPet->m_DiffAttrs.ServantAttr.lFUpgradeCount = pDBPet->GetLongAttr("DStone");
		pPet->m_DiffAttrs.ServantAttr.lStarLev = pDBPet->GetLongAttr("StarLev");
		pPet->m_DiffAttrs.ServantAttr.lStarLevCount = pDBPet->GetLongAttr("StarCnt");

#define PET_ATTR_WS(TYPE) \
	pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lStrenth = pDBPet->GetLongAttr(#TYPE"Strenth"); \
	pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lDexterity = pDBPet->GetLongAttr(#TYPE"Dexterity"); \
	pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lCon = pDBPet->GetLongAttr(#TYPE"Con"); \
	pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lIntellect = pDBPet->GetLongAttr(#TYPE"Intellect"); \
	pPet->m_DiffAttrs.ServantAttr.##TYPE##Val.lSpiritualism = pDBPet->GetLongAttr(#TYPE"Spiritualism"); \

		PET_ATTR_WS(talent);
		PET_ATTR_WS(dAfter);
		PET_ATTR_WS(syncretize);
		PET_ATTR_WS(stoneUp);
		PET_ATTR_WS(savvy);

		// 技能加载
		string strBuf = "Skills"; 
		long lAttrEnum = pDBPet->GetBufSize(strBuf);
		if( lAttrEnum > 0 )
		{
			BYTE* pBuf = new BYTE[lAttrEnum];
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
			SAFE_DELETE_ARRAY(pBuf);
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
			Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, szLog);
			continue;
		}
		setWriteDB.AddToByteArray(char(1));
		setWriteDB.AddToByteArray(itrPet->first);
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
	CGUID guid;
	char cFlag = 0;
	CPet* pPet = NULL;
	for( long i=0; i<lNum; ++i )
	{
		cFlag = setReadDB.GetCharFromByteArray();
		if( 0 == cFlag )
			continue;
		setReadDB.GetBufferFromByteArray(guid);
		pPet = pPlayer->CreatePet(guid);
		if (pPet==NULL)
		{
			return false;
		}
		pPet->DecodeFromDataBlock(setReadDB);
		pPlayer->AddPet(pPet);
	}

	return true;
}