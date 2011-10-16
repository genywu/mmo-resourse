#include "StdAfx.h"
#include ".\globesetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hWnd;

CGlobeSetup::tagSetup CGlobeSetup::m_stSetup;

CGlobeSetup::CGlobeSetup(void)
{
	memset(&m_stSetup, 0, sizeof(tagSetup));
}

CGlobeSetup::~CGlobeSetup(void)
{
}

// 从文件读取列表
BOOL CGlobeSetup::Load(const char* filename)
{
	CRFile* prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}


	stringstream stream;
	prfile->ReadToStream(stream);
	ZeroMemory(&m_stSetup, sizeof(tagSetup));

	string strTemp;
	stream	
		>> strTemp >> m_stSetup.btMaxCharactersNum
		>> strTemp >> m_stSetup.fStr2MaxAtk[0]
		>> strTemp >> m_stSetup.fStr2MaxAtk[1]
		>> strTemp >> m_stSetup.fStr2MaxAtk[2]
		>> strTemp >> m_stSetup.fStr2Burden[0]
		>> strTemp >> m_stSetup.fStr2Burden[1]
		>> strTemp >> m_stSetup.fStr2Burden[2]
		>> strTemp >> m_stSetup.fDex2MinAtk[0]
		>> strTemp >> m_stSetup.fDex2MinAtk[1]
		>> strTemp >> m_stSetup.fDex2MinAtk[2]
		>> strTemp >> m_stSetup.fDex2Stiff[0]
		>> strTemp >> m_stSetup.fDex2Stiff[1]
		>> strTemp >> m_stSetup.fDex2Stiff[2]
		>> strTemp >> m_stSetup.fCon2MaxHp[0]
		>> strTemp >> m_stSetup.fCon2MaxHp[1]
		>> strTemp >> m_stSetup.fCon2MaxHp[2]
		>> strTemp >> m_stSetup.fCon2Def[0]
		>> strTemp >> m_stSetup.fCon2Def[1]
		>> strTemp >> m_stSetup.fCon2Def[2]
		>> strTemp >> m_stSetup.fInt2MaxMp[0]
		>> strTemp >> m_stSetup.fInt2MaxMp[1]
		>> strTemp >> m_stSetup.fInt2MaxMp[2]
		>> strTemp >> m_stSetup.fInt2Element[0]
		>> strTemp >> m_stSetup.fInt2Element[1]
		>> strTemp >> m_stSetup.fInt2Element[2]
		>> strTemp >> m_stSetup.fInt2Resistant[0]
		>> strTemp >> m_stSetup.fInt2Resistant[1]
		>> strTemp >> m_stSetup.fInt2Resistant[2]

		>> strTemp >> m_stSetup.lBaseHit
		>> strTemp >> m_stSetup.fBaseAtcHitcMagnify
		>> strTemp >> m_stSetup.fHitLvRate
		>> strTemp >> m_stSetup.lMinHitMonster
		>> strTemp >> m_stSetup.lMaxHitMonster
		>> strTemp >> m_stSetup.lMinHit[0]
		>> strTemp >> m_stSetup.lMinHit[1]
		>> strTemp >> m_stSetup.lMinHit[2]
		>> strTemp >> m_stSetup.lMaxHit[0]
		>> strTemp >> m_stSetup.lMaxHit[1]
		>> strTemp >> m_stSetup.lMaxHit[2]
		>> strTemp >> m_stSetup.fCriticalRate
		>> strTemp >> m_stSetup.fLossExp_Normal
		>> strTemp >> m_stSetup.fLossExp_Game
		>> strTemp >> m_stSetup.fLossExp_War		

		// 掉物品几率(蓝、黄、红、灰)		
		>>strTemp>>m_stSetup.fDropEquip_Head[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Wing[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Body[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Back[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Glove[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Boot[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Frock[0][0]
		>>strTemp>>m_stSetup.fDropEquip_LRing[0][0]
		>>strTemp>>m_stSetup.fDropEquip_RRing[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Horse[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Bag[0][0]
		>>strTemp>>m_stSetup.fDropEquip_Money[0][0]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[0][0]

		>>strTemp>>m_stSetup.fDropEquip_Head[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Wing[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Body[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Back[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Glove[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Boot[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Frock[0][1]
		>>strTemp>>m_stSetup.fDropEquip_LRing[0][1]
		>>strTemp>>m_stSetup.fDropEquip_RRing[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Horse[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Bag[0][1]
		>>strTemp>>m_stSetup.fDropEquip_Money[0][1]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[0][1]

		>>strTemp>>m_stSetup.fDropEquip_Head[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Wing[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Body[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Back[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Glove[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Boot[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Frock[0][2]
		>>strTemp>>m_stSetup.fDropEquip_LRing[0][2]
		>>strTemp>>m_stSetup.fDropEquip_RRing[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Horse[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Bag[0][2]
		>>strTemp>>m_stSetup.fDropEquip_Money[0][2]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[0][2]

		>>strTemp>>m_stSetup.fDropEquip_Head[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Wing[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Body[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Back[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Glove[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Boot[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Frock[0][3]
		>>strTemp>>m_stSetup.fDropEquip_LRing[0][3]
		>>strTemp>>m_stSetup.fDropEquip_RRing[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Horse[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Bag[0][3]
		>>strTemp>>m_stSetup.fDropEquip_Money[0][3]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[0][3]
		// 掉物品几率(蓝、黄、红、灰)(城战)
		>>strTemp>>m_stSetup.fDropEquip_Head[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Wing[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Body[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Back[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Glove[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Boot[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Frock[1][0]
		>>strTemp>>m_stSetup.fDropEquip_LRing[1][0]
		>>strTemp>>m_stSetup.fDropEquip_RRing[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Horse[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Bag[1][0]
		>>strTemp>>m_stSetup.fDropEquip_Money[1][0]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[1][0]

		>>strTemp>>m_stSetup.fDropEquip_Head[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Wing[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Body[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Back[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Glove[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Boot[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Frock[1][1]
		>>strTemp>>m_stSetup.fDropEquip_LRing[1][1]
		>>strTemp>>m_stSetup.fDropEquip_RRing[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Horse[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Bag[1][1]
		>>strTemp>>m_stSetup.fDropEquip_Money[1][1]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[1][1]

		>>strTemp>>m_stSetup.fDropEquip_Head[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Wing[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Body[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Back[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Glove[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Boot[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Frock[1][2]
		>>strTemp>>m_stSetup.fDropEquip_LRing[1][2]
		>>strTemp>>m_stSetup.fDropEquip_RRing[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Horse[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Bag[1][2]
		>>strTemp>>m_stSetup.fDropEquip_Money[1][2]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[1][2]

		>>strTemp>>m_stSetup.fDropEquip_Head[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Necklace[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Wing[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Body[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Back[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Glove[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Boot[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Headgear[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Frock[1][3]
		>>strTemp>>m_stSetup.fDropEquip_LRing[1][3]
		>>strTemp>>m_stSetup.fDropEquip_RRing[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal1[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal2[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Medal3[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Fairy[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Weapon[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Weapon2[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Cloak[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Horse[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Bag[1][3]
		>>strTemp>>m_stSetup.fDropEquip_Money[1][3]
		>>strTemp>>m_stSetup.fDropEquip_MoneyPercent[1][3]

		//物品保值比和价格修理系数
		>> strTemp >> m_stSetup.fBasePriceRate
		>> strTemp >> m_stSetup.fTradeInRate
		>> strTemp >> m_stSetup.fRepairFactor
		>> strTemp >> m_stSetup.fRepairFactor2

		//##物品损耗
		>> strTemp >> m_stSetup.lGoodsDurFray
		>> strTemp >> m_stSetup.lGoodsDurSpecialFray
		>> strTemp >> m_stSetup.lHeadWaste
		>> strTemp >> m_stSetup.lBodyWaste
		>> strTemp >> m_stSetup.lGloveWaste
		>> strTemp >> m_stSetup.lBootWaste

		>> strTemp >> m_stSetup.lBackWaste
		>> strTemp >> m_stSetup.lDecorateWaste
		>> strTemp >> m_stSetup.lHeadgearWaste
		>> strTemp >> m_stSetup.lTalismanWaste
		>> strTemp >> m_stSetup.lFrockWaste
		>> strTemp >> m_stSetup.lMedalWaste
		>> strTemp >> m_stSetup.lWingWaste
		>> strTemp >> m_stSetup.lManteauWaste
		>> strTemp >> m_stSetup.lFairyWaste
		>> strTemp >> m_stSetup.lNeckWaste
		>> strTemp >> m_stSetup.lRingWaste
		>> strTemp >> m_stSetup.lShieldWaste

		//物品升级阶段系数和成功几率
		>> strTemp >> m_stSetup.wGoodsBlessPhase[0]
		>> strTemp >> m_stSetup.wGoodsBlessPhase[1]
		>> strTemp >> m_stSetup.wGoodsBlessPhase[2]
		>> strTemp >> m_stSetup.fGoodsUpgradeOdds[0]
		>> strTemp >> m_stSetup.fGoodsUpgradeOdds[1]
		>> strTemp >> m_stSetup.fGoodsUpgradeOdds[2]
		>> strTemp >> m_stSetup.fGoodsUpgradeOdds[3]

		// 其他
		>> strTemp >> m_stSetup.dwGoodsDisappearTimer
		>> strTemp >> m_stSetup.dwGoodsProtectedTimer
		>> strTemp >> m_stSetup.lResumeTimer
		>> strTemp >> m_stSetup.lResumeFactor
		>> strTemp >> m_stSetup.fResumeFactorMini

		>> strTemp >> m_stSetup.lMonsterResumeTimer

		>> strTemp >> m_stSetup.lFightStateTimer
		>> strTemp >> m_stSetup.lSinStateTimer
		>> strTemp >> m_stSetup.dwAttackMonsterProtectedTimer
		>> strTemp >> m_stSetup.dwMonsterKeepDeadTime
		>> strTemp >> m_stSetup.dwCanCollectMonsterKeepDeadTime
		>> strTemp >> m_stSetup.dwBeneficiaryTime
		>> strTemp >> m_stSetup.dwCollectionKeepDeadTime
		>> strTemp >> m_stSetup.dwMonsterTransitionTime
		>> strTemp >> m_stSetup.dwMonsterKindlyTime
		>> strTemp >> m_stSetup.dwReturnPeaceEffectTime
		>> strTemp >> m_stSetup.fReturnPeaceSpeed
		>> strTemp >> m_stSetup.dwReturnPeaceStandTime
		>> strTemp >> m_stSetup.dwFightStateTime
		>> strTemp >> m_stSetup.dwDHFQueueSize
		// 怒值和连击
		>> strTemp >> m_stSetup.wBaseRp_Lv1
		>> strTemp >> m_stSetup.wBaseRp_Lv2
		>> strTemp >> m_stSetup.wBaseMaxRp_Lv1
		>> strTemp >> m_stSetup.wBaseMaxRp_Lv2
		>> strTemp >> m_stSetup.wBaseAtcRp_Sword

		>> strTemp >> m_stSetup.fRpAdd_Lv1[0]	>>	m_stSetup.wRpAdd_Lv1[0]
		>> strTemp >> m_stSetup.fRpAdd_Lv1[1]	>>	m_stSetup.wRpAdd_Lv1[1]
		>> strTemp >> m_stSetup.fRpAdd_Lv1[2]	>>	m_stSetup.wRpAdd_Lv1[2]
		>> strTemp >> m_stSetup.fRpAdd_Lv1[3]	>>	m_stSetup.wRpAdd_Lv1[3]
		>> strTemp >> m_stSetup.fRpAdd_Lv1[4]	>>	m_stSetup.wRpAdd_Lv1[4]
		>> strTemp >> m_stSetup.fRpAdd_Lv1[5]	>>	m_stSetup.wRpAdd_Lv1[5]

		>> strTemp >> m_stSetup.lHitBaseLv
		>> strTemp >> m_stSetup.lHitTime
		>> strTemp >> m_stSetup.fHitExpPrize
		>> strTemp >> m_stSetup.fMaxHitExpPrize

		>> strTemp >> m_stSetup.dwHitTopLogNum[0]	>>	m_stSetup.dwHitTopLogPrizeExp[0]
		>> strTemp >> m_stSetup.dwHitTopLogNum[1]	>>	m_stSetup.dwHitTopLogPrizeExp[1]
		>> strTemp >> m_stSetup.dwHitTopLogNum[2]	>>	m_stSetup.dwHitTopLogPrizeExp[2]
		>> strTemp >> m_stSetup.dwHitTopLogNum[3]	>>	m_stSetup.dwHitTopLogPrizeExp[3]
		>> strTemp >> m_stSetup.dwHitTopLogNum[4]	>>	m_stSetup.dwHitTopLogPrizeExp[4]
		>> strTemp >> m_stSetup.dwHitTopLogNum[5]	>>	m_stSetup.dwHitTopLogPrizeExp[5]
		>> strTemp >> m_stSetup.dwHitTopLogNum[6]	>>	m_stSetup.dwHitTopLogPrizeExp[6]
		>> strTemp >> m_stSetup.dwHitTopLogNum[7]	>>	m_stSetup.dwHitTopLogPrizeExp[7]
		>> strTemp >> m_stSetup.dwHitTopLogNum[8]	>>	m_stSetup.dwHitTopLogPrizeExp[8]
		>> strTemp >> m_stSetup.dwHitTopLogNum[9]	>>	m_stSetup.dwHitTopLogPrizeExp[9]
		>> strTemp >> m_stSetup.dwHitTopLogNum[10]	>>	m_stSetup.dwHitTopLogPrizeExp[10]
		>> strTemp >> m_stSetup.dwHitTopLogNum[11]	>>	m_stSetup.dwHitTopLogPrizeExp[11]

		>> strTemp >> m_stSetup.lHpLiveUp[0]
		>> strTemp >> m_stSetup.lHpLiveUp[1]
		>> strTemp >> m_stSetup.lHpLiveUp[2]
		>> strTemp >> m_stSetup.lMpLiveUp[0]
		>> strTemp >> m_stSetup.lMpLiveUp[1]
		>> strTemp >> m_stSetup.lMpLiveUp[2]
		>> strTemp >> m_stSetup.lBurdenLiveUp[0]
		>> strTemp >> m_stSetup.lBurdenLiveUp[1]
		>> strTemp >> m_stSetup.lBurdenLiveUp[2]

		//##硬直的参数
		>> strTemp >> m_stSetup.wNumStiffSetup
		>> strTemp >> m_stSetup.fStiff_Damage[0] >> m_stSetup.wStiff_Probability[0]
		>> strTemp >> m_stSetup.fStiff_Damage[1] >> m_stSetup.wStiff_Probability[1]
		>> strTemp >> m_stSetup.fStiff_Damage[2] >> m_stSetup.wStiff_Probability[2]
		>> strTemp >> m_stSetup.fStiff_Damage[3] >> m_stSetup.wStiff_Probability[3]

		>> strTemp >> m_stSetup.dwStiffDelayTime
		>> strTemp >> m_stSetup.dwStiffBoundTime
		>> strTemp >> m_stSetup.dwStiffLimit

		//##时间减免
		>> strTemp >> m_stSetup.dwCriminalTime
		>> strTemp >> m_stSetup.dwMurdererTime	// 废弃
		>> strTemp >> m_stSetup.dwRipperTime	// 废弃
		>> strTemp >> m_stSetup.dwPkCountPerKill	// 杀一个增加的PK值
		>> strTemp >> m_stSetup.dwOnePkCountTime	// 消除一个PK值的时间

		//拾取物品时间间隔
		>> strTemp >> m_stSetup.dwPickGoodsInterval
		//##金币上限
		>> strTemp >> m_stSetup.dwGoldCoinLimit
		//银币上限
		>> strTemp >> m_stSetup.dwSilverLimit
		//点券上限
		>> strTemp >> m_stSetup.dwTicketLimit

		//##道具加血允许的时间间隔。
		>> strTemp >> m_stSetup.dwHealTimeInterval

		//##道具加MP允许的时间间隔。
		>> strTemp >> m_stSetup.dwMpTimeInterval

		>> strTemp >> m_stSetup.fMonsterNumScale

		>> strTemp >> strTemp	//m_stSetup.bAllowClientRunScript

		>> strTemp >> strTemp	//m_stSetup.bAllowClientChangePos

		>> strTemp >> strTemp	//m_stSetup.dwSavePointTime

		>> strTemp >> m_stSetup.lAreaWidth
		>> strTemp >> m_stSetup.lAreaHeight

		// 角色保留天数
		>> strTemp >> strTemp	//m_stSetup.dwDelDays
		>> strTemp >> strTemp	//m_stSetup.strSpeStr	// 含有该特殊字符，可以更名
		>> strTemp >> m_stSetup.lRegionChatLevelLimit

		>> strTemp >> m_stSetup.lOfferExpPercent
		>> strTemp >> m_stSetup.fDecTimeParam
		>> strTemp >> m_stSetup.lDiedStateTime

		>> strTemp >> m_stSetup.m_fPvPDamageFactor
		>> strTemp >> m_stSetup.m_btNewbieLevelLimit
		>> strTemp >> m_stSetup.m_btNewSoldierLevel
		
		>> strTemp >> m_stSetup.m_fWeaponPowerModifier
		>> strTemp >> m_stSetup.m_fWeaponMinPowerModifier


		>> strTemp >> m_stSetup.m_dwCard6Hours
		>> strTemp >> m_stSetup.m_dwCard12Hours
		>> strTemp >> m_stSetup.m_dwCard24Hours

		>> strTemp >> m_stSetup.m_dwCard6HoursChargePrice
		>> strTemp >> m_stSetup.m_dwCard12HoursChargePrice
		>> strTemp >> m_stSetup.m_dwCard24HoursChargePrice

		>> strTemp >> m_stSetup.m_dwCardLowerLimitPrice
		>> strTemp >> m_stSetup.m_dwCardUpperLimitPrice
		

		>> strTemp >> m_stSetup.m_dwCardMaxNum
		>> strTemp >> m_stSetup.m_dwCardWorldMaxNum

		>> strTemp >> m_stSetup.m_dwCardRetryNum
		>> strTemp >> m_stSetup.m_dwCardDenyTime

		>> strTemp >> m_stSetup.fBlastAttackScale
		>> strTemp >> m_stSetup.fBlastDefendScale
		>> strTemp >> m_stSetup.fElementBlastAttackScale
		>> strTemp >> m_stSetup.fElementBlastDefendScale
		>> strTemp >> m_stSetup.fElementFullMissScale
	
		>> strTemp >> m_stSetup.szTalkWorldGoodsName		// 世界喊话物品原始名
		>> strTemp >> m_stSetup.nTalkWorldGoodsNum			// 世界喊话物品数量
		>> strTemp >> m_stSetup.lTalkWorldGold				// 世界喊话金币数量
		>> strTemp >> m_stSetup.lTalkWorldSilver			// 世界喊话银币数量

		>> strTemp >> m_stSetup.szTalkUnionGoodsName		// 阵营喊话物品原始名
		>> strTemp >> m_stSetup.nTalkUnionGoodsNum			// 阵营喊话物品数量
		>> strTemp >> m_stSetup.lTalkUnionGold				// 阵营喊话金钱数量
		>> strTemp >> m_stSetup.lTalkUnionSilver			// 阵营喊话银币数量

		>> strTemp >> m_stSetup.szTalkCountryGoodsName		// 国家喊话物品原始名
		>> strTemp >> m_stSetup.nTalkCountryGoodsNum		// 国家喊话物品数量
		>> strTemp >> m_stSetup.lTalkCountryGold			// 国家喊话金钱数量
		>> strTemp >> m_stSetup.lTalkCountrySilver			// 国家喊话银币数量

		>> strTemp >> m_stSetup.szTalkTopWorldGoodsName		// 置顶世界喊话物品原始名
		>> strTemp >> m_stSetup.nTalkTopWorldGoodsNum		// 置顶世界喊话物品数量
		>> strTemp >> m_stSetup.lTalkTopWorldGold			// 置顶世界喊话金币数量
		>> strTemp >> m_stSetup.lTalkTopWorldSilver			// 置顶世界喊话银币数量

		>> strTemp >> m_stSetup.szReliveGoodsName
		>> strTemp >> m_stSetup.lRelivePrice


		>> strTemp >> m_stSetup.fPlayerWalkSpeed
		>> strTemp >> m_stSetup.fPlayerRunSpeed

		>> strTemp >> m_stSetup.fRidePlayerWalkSpeed
		>> strTemp >> m_stSetup.fRidePlayerRunSpeed

		>> strTemp >> m_stSetup.lExpBallFactor1
		>> strTemp >> m_stSetup.lExpBallFactor2

		>> strTemp >> m_stSetup.dwAutoProtectTime

		>> strTemp >> m_stSetup.dwIncrementLogDays
		>> strTemp >> m_stSetup.lMaxHelpHintNum

		>> strTemp >> m_stSetup.pszPlayerLoginScript
		>> strTemp >> m_stSetup.pszPlayerDiedScript
		>> strTemp >> m_stSetup.pszPlayerUpLevelScript
		>> strTemp >> m_stSetup.pszPlayerChangeOccuScript
		>> strTemp >> m_stSetup.pszPlayerUseDiamondScript
		>> strTemp >> m_stSetup.pszPVPKilled
		>> strTemp >> m_stSetup.pszPVPBeKilled

		>> strTemp >> m_stSetup.pszConValueNegative
		>> strTemp >> m_stSetup.pszConValueNotNegative

		>> strTemp >> m_stSetup.szReliveScriptName	// 玩家复活触发脚本名
		>> strTemp >> m_stSetup.pszCalendar
		>> strTemp >> m_stSetup.pszMagicBox
		>> strTemp >> m_stSetup.pszAnswerRequest
		>> strTemp >> m_stSetup.pszChangeLineRgn
		>> strTemp >> m_stSetup.pszHelpRequest

		//队伍设置
		>> strTemp >> m_stSetup.lCorrelativeSkillID			//与组队关联的技能ID
		>> strTemp >> m_stSetup.lMinSkillLevel				//可以组队的最小技能等级
		>> strTemp >> m_stSetup.lMinTradeLevel

		>> strTemp >> m_stSetup.new_lGoodsAllot				//默认分配方式
		>> strTemp >> m_stSetup.new_lMaxMemberNum			//默认最大人数
		>> strTemp >> m_stSetup.new_lSessionTimeoutS		//会话超时秒数

		>> strTemp >> m_stSetup.new_fIncreasedExp_2			//2人经验加成比例
		>> strTemp >> m_stSetup.new_fIncreasedExp_3			//3人经验加成比例
		>> strTemp >> m_stSetup.new_fIncreasedExp_4			//4人经验加成比例
		>> strTemp >> m_stSetup.new_fIncreasedExp_5			//5人经验加成比例
		>> strTemp >> m_stSetup.new_fIncreasedExp_6			//6人经验加成比例
		>> strTemp >> m_stSetup.new_lExperienceRadii		//经验分配半径（单位：格）
		>> strTemp >> m_stSetup.new_lExpDiff				//经验差
		>> strTemp >> m_stSetup.new_lExpLimit				//玩家经验值份额基本保障比例
		>> strTemp >> m_stSetup.new_fExpAmerce				//经验惩罚系数
		>> strTemp >> m_stSetup.new_fExpHortation			//经验奖励系数
		>> strTemp >> m_stSetup.new_fExpMinLvlDiff			//经验奖励开始的最小等级差

		>> strTemp >> m_stSetup.vigour_fIncreasedExp_2		//2人元气加成比例
		>> strTemp >> m_stSetup.vigour_fIncreasedExp_3		//3人元气加成比例
		>> strTemp >> m_stSetup.vigour_fIncreasedExp_4		//4人元气加成比例
		>> strTemp >> m_stSetup.vigour_fIncreasedExp_5		//5人元气加成比例
		>> strTemp >> m_stSetup.vigour_fIncreasedExp_6		//6人元气加成比例
		>> strTemp >> m_stSetup.vigour_lExpDiff				//元气差
		>> strTemp >> m_stSetup.vigour_lExpLimit			//元气经验值份额基本保障
		>> strTemp >> m_stSetup.vigour_fExpAmerce			//元气惩罚系数
		>> strTemp >> m_stSetup.vigour_fExpHortation		//元气奖励系数
		>> strTemp >> m_stSetup.vigour_fExpMinLvlDiff		//元气奖励开始的最小等级差
		>> strTemp >> m_stSetup.vigour_fOccExpSpRatio		//职业经验和SP比值
		

		>> strTemp >> m_stSetup.lCommoCoolDownTime		//公共冷却时间
		>> strTemp >> m_stSetup.fPkToPvpValue			//pk/pvp比例值
		>> strTemp >> m_stSetup.pszPKToPvPScript		//pk/pvp比例触发的脚本
		>> strTemp >> m_stSetup.dwPkCountPerAttack		//
		>> strTemp >> m_stSetup.dwActTime				//施放动作时间
		>> strTemp >> m_stSetup.dwHpMpRestoreTime
		>> strTemp >> m_stSetup.lPostage[0]				//系统邮件
		>> strTemp >> m_stSetup.lPostage[1]				//普通邮件
		>> strTemp >> m_stSetup.lPostage[2]				//普通附件邮件
		>> strTemp >> m_stSetup.lPostage[3]				//赠送邮件邮资
		>> strTemp >> m_stSetup.lPostage[4]				//索取邮件邮资
		>> strTemp >> m_stSetup.lPostage[5]				//付费邮件邮资

		>> strTemp >> m_stSetup.lMaxName				//名字字数限制
		>> strTemp >> m_stSetup.lMaxSubject				//主题最大字数
		>> strTemp >> m_stSetup.lMaxText				//邮件内容最大字数
		>> strTemp >> m_stSetup.lMaxExText				//邮件附加文本的最大字数
		>> strTemp >> m_stSetup.lMaxMoney				//最大金钱

		>> strTemp >> m_stSetup.fMaxRemainTime[0]				//系统邮件
		>> strTemp >> m_stSetup.fMaxRemainTime[1]				//普通邮件
		>> strTemp >> m_stSetup.fMaxRemainTime[2]				//普通附件邮件
		>> strTemp >> m_stSetup.fMaxRemainTime[3]				//赠送邮件
		>> strTemp >> m_stSetup.fMaxRemainTime[4]				//索取邮件
		>> strTemp >> m_stSetup.fMaxRemainTime[5]				//付费邮件
		>> strTemp >> m_stSetup.lPrisonIslandPkValue			//被放到流亡岛的PK值
		>> strTemp >> m_stSetup.lMailDelayTime					//邮件延时时间
	
		//>> strTemp >> m_stSetup.lMaxMailGroup					 //群发收信人的最大数量







		>> strTemp >> m_stSetup.dwGoldGoodsId
		>> strTemp  >> m_stSetup.dwSilverGoodsId

		>> strTemp  >> m_stSetup.dwWeimianGoodsId		//位面精华物编ID

		>> strTemp >> m_stSetup.dwNpcSellGoldToSilver

		>> strTemp >> m_stSetup.lMaxGoodInfoNum			//最大[聊天中物品信息]保存数量

		>> strTemp >> m_stSetup.lReliveDebuffId		// 玩家复活DebuffID
		>> strTemp >> m_stSetup.lReliveDebuffLevel  // 玩家复活DebuffLevel	
		>> strTemp >> m_stSetup.pszLottery          // 彩票系统调用脚本
		;
		// end ;

		LoadGameSetup("setup/gamesetup.ini");
	
	rfClose(prfile);
	return true;
}

// 从setup\gamesetup.ini读取
BOOL CGlobeSetup::LoadGameSetup(const char* filename)
{
	//////////////////////////////////////////////////////////////////////////
	// 从 setup\GameSetup.ini 读取
	//////////////////////////////////////////////////////////////////////////
	
	CRFile *prfile = rfOpen(filename);
	if(prfile == NULL)
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	stringstream stream;
	prfile->ReadToStream(stream);
	rfClose(prfile);

	string strTemp;
	stream	
		>> strTemp >> m_stSetup.fExpScale
		>> strTemp >> m_stSetup.fExpScriptScale
		>> strTemp >> m_stSetup.fOccuExpScale
		>> strTemp >> m_stSetup.fOccuExpScriptScale
		>> strTemp >> m_stSetup.fSpScale
		>> strTemp >> m_stSetup.fSpScriptScale
		>> strTemp >> m_stSetup.fMonsterDropGoodsScale

		//赠送经验相关
		>> strTemp >> m_stSetup.dwPresentExpCooldown
		>> strTemp >> m_stSetup.fPresentExpScaleInPersonalHouse
		>> strTemp >> m_stSetup.fPresentExpScaleInSafeArea
		>> strTemp >> m_stSetup.fPresentExpScale
		>> strTemp >> m_stSetup.lPresentExpBuffId >> m_stSetup.lPresentExpBuffLvl

		>> strTemp >> m_stSetup.bAllowClientRunScript

		>> strTemp >> m_stSetup.bAllowClientChangePos

		>> strTemp >> m_stSetup.lShowSpouseNameStep

		>> strTemp >> m_stSetup.dwSavePointTime

		// 角色保留天数
		>> strTemp >> m_stSetup.dwDelDays
		>> strTemp >> m_stSetup.dwDelLevelCondition		//　删除角色需要达到的等级
		>> strTemp >> m_stSetup.strSpeStr	// 含有该特殊字符，可以更名


		//限制发消息的最大玩家数量
		>> strTemp >> m_stSetup.lSendMsgLimitPlayerNum
		//限制发消息时的最大距离
		>> strTemp >> m_stSetup.lSendMsgLimitDistance
		//限制发技能消息时最大玩家数量
		>> strTemp >> m_stSetup.lSendSkillMsgLimitPlayerNum
		//更新属性到周围的延迟时间
		>> strTemp >> m_stSetup.lUpdateOtherPropertyDelayTime
		//发送AddShape消息时的间隔时间
		>> strTemp >> m_stSetup.lSendAddShapeMsgInteTime
		//立即发送AddShape消息的距离限制
		>> strTemp >> m_stSetup.lSendAddSahpeMsgDistance
		//技能状态超时检测时间
		>> strTemp >> m_stSetup.lBaseModuleTime	
		//总技能越界个数
		>> strTemp >> m_stSetup.lTotalMaxSkillNum	
		//每秒技能越界个数
		>> strTemp >> m_stSetup.fOneSecMaxSkillNum
		//总NPC脚本越界个数
		>> strTemp >> m_stSetup.lTotalMaxNpcScriptsNum	
		//每秒NPC脚本越界个数
		>> strTemp >> m_stSetup.fOneSecMaxNpcScriptsNum	

		>> strTemp >> m_stSetup.lCheckMsgCount
		>> strTemp >> m_stSetup.fMaxMsgCountPerSecond
		>> strTemp >> m_stSetup.fMaxMistakeAverSpeed
		>> strTemp >> m_stSetup.fMaxMistakeMaxSpeed
		>> strTemp >> m_stSetup.lPingChekTime
		>> strTemp >> m_stSetup.lPingErrorBanTime
		>> strTemp >> m_stSetup.lReLiveTime

		//! 游戏对象分配策略设置
		>> strTemp //配置注释

		>> strTemp >> m_stSetup.Goods_Policy.ObjNum				>> m_stSetup.Goods_Policy.blockNum
		>> strTemp >> m_stSetup.Monster_Policy.ObjNum			>> m_stSetup.Monster_Policy.blockNum
		>> strTemp >> m_stSetup.Collection_Policy.ObjNum		>> m_stSetup.Collection_Policy.blockNum
		>> strTemp >> m_stSetup.Npc_Policy.ObjNum				>> m_stSetup.Npc_Policy.blockNum
		>> strTemp >> m_stSetup.Player_Policy.ObjNum			>> m_stSetup.Player_Policy.blockNum
		>> strTemp >> m_stSetup.Region_Policy.ObjNum			>> m_stSetup.Region_Policy.blockNum
		>> strTemp >> m_stSetup.SummonedCreature_Policy.ObjNum	>> m_stSetup.SummonedCreature_Policy.blockNum
		>> strTemp >> m_stSetup.Build_Policy.ObjNum				>> m_stSetup.Build_Policy.blockNum
		>> strTemp >> m_stSetup.Citygate_Policy.ObjNum			>> m_stSetup.Citygate_Policy.blockNum
		
		>> strTemp >> m_stSetup.lTimerVarNum	//预分配定时器参数的数量
		>> strTemp >> m_stSetup.lPreAllocedScriptNum // 预分配的脚本内存块数量
		>> strTemp >> m_stSetup.lMaxPreAllocedScriptNum// 预分配的脚本内存块最大数量
		>> strTemp >> m_stSetup.lPDupRgnMaxNum  // 创建个人副本最大数
		>> strTemp >> m_stSetup.lPHDupRgnMaxNum  // 创建个人房屋副本最大数
		>> strTemp >> m_stSetup.lTDupRgnMaxNum  // 创建组队副本最大数
		>> strTemp >> m_stSetup.lPDupRgnDelTime			// 个人副本删除时间
		>> strTemp >> m_stSetup.lCreatePDupRgnMaxNum	//玩家创建个人副本最大数
		>> strTemp >> m_stSetup.lCreateTDupRgnMaxNum	//玩家创建组队副本最大数
		>> strTemp >> m_stSetup.lCreateDupRgnDeltaTime //玩家创建副本时间间隔
		>> strTemp >> m_stSetup.lSaveTime				// 玩家保存数据的时间间隔
		>> strTemp >> m_stSetup.lWSSaveTime			// WS保存数据的时间间隔
		>> strTemp >> m_stSetup.lCreateRgnPlayerLoadMax	//GS创建副本玩家数上限
		>> strTemp >> m_stSetup.lCreateRgnMonsterLoadMax	//GS创建副本怪物数上限
		>> strTemp >> m_stSetup.lPDupRgnMaxPlayerNum  // 个人副本最大人数
		>> strTemp >> m_stSetup.lPHDupRgnMaxPlayerNum  // 个人房屋副本最大人数
		>> strTemp >> m_stSetup.lTDupRgnMaxPlayerNum  // 组队副本最大人数
		>> strTemp >> m_stSetup.lRgnKickPlayerDeltaTime  // 副本清人时间
		>> strTemp >> m_stSetup.szHideName				// 玩家隐匿名
		>> strTemp >> m_stSetup.lMLevelEnablePlayerSearch // 根据等级开启搜索玩家功能
		>> strTemp >> m_stSetup.uUpgradeTo10FontColor		//! 武器成功到达10以上，公告显示的字体颜色
		>> strTemp >> m_stSetup.uUpgradeTo20BackIdx		//! 武器成功到达20以上，走马灯显示的背景索引
		>> strTemp >> m_stSetup.uUpgrade10LostFontColor	//! 10以上武器升级失败，公告显示的字体颜色
		>> strTemp >> m_stSetup.lWarningDeltaTime	//! 遇敌报警时间间隔
		>> strTemp >> m_stSetup.lOperBounsTime		// 玩家限制操作赠送间隔时间(毫秒)
		>> strTemp >> m_stSetup.dwReportTime		// 举报时间间隔	

		; // end ;

	return true;
}

// 保存到文件列表
BOOL CGlobeSetup::Save(const char* filename)
{
	return true;
}

// 添加到CByteArray
bool CGlobeSetup::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, &m_stSetup, sizeof(tagSetup));
	return true;
}

// 解码
bool CGlobeSetup::DecordFromByteArray(BYTE* pByte, long& pos)
{
	_GetBufferFromByteArray(pByte, pos, &m_stSetup, sizeof(tagSetup));
	return true;
}

// 获取最大怒值上限
WORD CGlobeSetup::GetBaseMaxRp(BYTE lOccupation, BYTE lLevel)
{
	if (lOccupation==0)	// 武士
	{
		if (lLevel>=GetSetup()->wBaseRp_Lv1 && lLevel<GetSetup()->wBaseRp_Lv2)
		{
			return GetSetup()->wBaseMaxRp_Lv1;	// 第一段
		}
		else if (lLevel>=GetSetup()->wBaseRp_Lv2)
		{
			return GetSetup()->wBaseMaxRp_Lv2;	// 第二段
		}
	}

	return 0;
}


