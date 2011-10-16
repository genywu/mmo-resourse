//======================================================================================
/**
*	@file		PlayerList.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

#include "../Common/BaseDef.h"
#include "../Common/PlayerDef.h"

/**
*	@class PlayerList
*
*	@brief 
*/

class PlayerList
{
public:
	/// 构造
	PlayerList(void);

	/// 析构
	~PlayerList(void);

public:
	/// 性别
	enum eSex
	{
		SEX_MALE,							// 男
		SEX_FEMALE,							// 女
		SEX_NUMS							// 性别数量
	};

	static string s_strOccupation[OCC_Max];
	static string s_strSex[SEX_NUMS];

	/// 升级属性
	struct tagLevelUp
	{
		DWORD	dwExp;						// 升级所需经验
		DWORD	dwExpMultiple;				// 升级经验倍数
		long	lPresentExpPerMinute;		// 每分钟赠送经验
		long	lMaxPresentExp;				// 最大赠送经验
		float	fFacBoonExpScale;			// 获得家族经验的比例
		long	lBaseCP;					// CP基础值
	};

	/// 玩家职业升级属性
	struct tagOccuLevelUp
	{
		DWORD dwExp;						//升级所需的经验
	};

	/// 玩家职业修改属性
	struct tagOccupModifyProperty
	{
		float		fStr;				    //力量 
		float		fDex;				    //身法 
		float		fCon;				    //根骨 
		float		fIntellect;				//意志 
		float		fSpiritualism;			//灵性 

		float		fStrAttRatio;			//力攻比
		float		fAgiAttRatio;			//敏攻比
		float		fConDefRatio;			//体防比

		float		fBlockStrRatio;			//格挡力比
		float		fBlockValueRatio;		//格挡值比
		float		fParryStrRatio;			//招架力比		
		float		fParryAgiRatio;			//招架敏比
		float		fParryValueRatio;		//招架值比

		float		fDodgeAgiRatio;			//闪避敏比
		float		fDodgeValueRatio;		//闪避值比
		float		fCriAgiRatio;			//敏物爆比
		float		fCriValueRatio;			//物爆值比
		float		fHitAgiRatio;			//敏命中比
		float		fHitValueRatio;			//命中值比

		float		fBaseHit;				//基本命中
		WORD		fBaseHp;				//职业基本生命
		WORD		fLvlUpHp;				//每升级生命

		WORD		fBaseMp;				//职业基本法力
		WORD		fLvlUpMp;				//每升级法力

		DWORD		fMaxBaseEnergy;			//职业基本能量上限
		float		fBaseEnergyRecoverValue;//基本能量恢复
		DWORD		fBaseEnergyRecoverTime;	//基本能量恢复间隔时间
		float		fFightEnergyRecoverValue;//战斗能量恢复
		DWORD		fFightEnergyRecoverTime; //战斗能量恢复间隔时间
		DWORD		fAttackEnergyValue;		//普通攻击能量值修正
		DWORD		fBlastEnertyValue;		//暴击能量值修正
		DWORD		fBeenAttackEnergyValue;	//被攻击能量值修正
		DWORD		fBeenBlastEnergyValue;	//被暴击能量值修正
		
		float		fCriDamageStrRatio;		//物爆伤害力比
		float		fCriDamageValueRatio;	//物爆伤害值比
		float		fCriDefConstiRatio;		//物理暴击抗性体质比
		float		fCriDefValueRatio;		//物理暴击抗性值比
		float		fConstiDefRatio;		//体质抗性比
		float		fConstiDefValueRatio;	//体质抗性值比
		float		fConstiHpRatio;			//体质生命比
		float		fMAtkIntellRatio;		//智力魔攻比
		float		fMCriDamageIntellRatio;	//魔暴伤害智力比
		float		fMCriDamageValueRatio;	//魔暴伤害值%比
		float		fWillDefSpiritRatio;	//精神意志抗性比
		float		fWillMpRatio;			//精神法力比
		float		fWillDefValueRatio;		//意志抗性值%比
		float		fMCriDefSpiritRatio;	//魔法暴击抗性精神比
		float		fMCriDefValueRatio;		//魔法暴击抗性值%比
		float		fMCriWisdomRatio;		//智慧魔暴比
		float		fMCriValueRatio;		//魔暴值%比
		float		fMDefSpiritRatio;		//精神魔法抗性比
		float		fMDefValueRatio;		//魔法抗性值%比
		float		fAbsorbWisdomRatio;		//专注智慧比
		float		fAbsorbValueRatio;		//专注值%比
		float		fBuffPowerIntellRatio;	//状态攻击强度智力比
		float		fBuffPowerValueRatio;	//状态攻击强度%比
		float		fMAtkSpeedWisdomRatio;	//施法速度智慧比
		float		fMAtkSpeedValueRatio;	//施法速度值%比
		long		lBasePKCP;				//职业基本CP值
		long		lLvlupAddPKCP;			//职业升级CP值
	};

	/// 初始物品和装备列表
	struct tagOrginEquip
	{
		uchar	lOccupation;			// 职业
		WORD	wPlacePos;				// 放置位置
		string	strOrginName;			// 物品原始名
	};

	/// 职业的魔防和物防参数设置 
	struct tagOccuDefParam
	{
		float Def[OCC_Max];				//各职业的物理防御
		float MDef[OCC_Max];			//各职业的魔法防御
	};

	/// 职业的魔攻和物攻参数设置 
	struct tagOccuAtkParam
	{	
		float Atk[OCC_Max];				//各职业的物理攻击		
		float MAtk[OCC_Max];			//各职业的魔法攻击
	};

private:
	
	static vector<tagLevelUp>				m_vectorPlayerExp;	// 玩家升级经验值属性
	static DWORD							m_dwLevelPoint;		// 每升一级给玩家分配的点数
	static list<tagOrginEquip>				m_listOrginEquip;	// 初始装备列表

	static vector<tagOccuLevelUp>	m_vcOccuLvlExp[OCC_Max];//每个职业等级经验列表


	static long m_lBasePropertyValue;//基本属性值
	static float m_fPropertyLvlParam;//属性等级系数
	static map<DWORD, tagOccupModifyProperty>	m_mapOccuModifyPropertyList;     //职业修正表
	static map<DWORD,tagOccuDefParam>	m_mapOccuDefParam;//各职业的防御修正值

	static map<DWORD,tagOccuAtkParam>	m_mapOccuAtkParam;//各职业的攻击修正值

public:
	static tagOccupModifyProperty*	GetOccuModifyProperty(WORD level);         //
	static DWORD	GetPlayerExp(uchar lLevel)	{return (m_vectorPlayerExp.size()>lLevel && lLevel>=0)?m_vectorPlayerExp[lLevel].dwExp:0;}
	static DWORD	GetPlayerExpMultiple(uchar lLevel)	{return (m_vectorPlayerExp.size()>lLevel && lLevel>=0)?m_vectorPlayerExp[lLevel].dwExpMultiple:0;}
	static LONG64	GetPlayerTotalExp(uchar lLevel) 
	{ 
		LONG64 llRet = 0;
		if(m_vectorPlayerExp.size()>lLevel && lLevel>=0)
		{
			LONG64 llTmp = (LONG64)dwExpMutipleValue*m_vectorPlayerExp[lLevel].dwExpMultiple;
			llRet = llTmp + m_vectorPlayerExp[lLevel].dwExp;
		};
		return llRet; 
	}
	static LONG64   GetMaxExp(void)
	{
		LONG64 llRet = 0;
		if(m_vectorPlayerExp.size()>0)
		{
			LONG64 llTmp = (LONG64)dwExpMutipleValue*m_vectorPlayerExp[m_vectorPlayerExp.size()-1].dwExpMultiple;
			llRet = llTmp + m_vectorPlayerExp[m_vectorPlayerExp.size()-1].dwExp;
		};
		return llRet; 
	}
	static DWORD	GetLevelNum()	{return (DWORD)m_vectorPlayerExp.size();}
	static DWORD	GetOccuLevelNum(eOccupation byOccu)
	{
		if(byOccu>=0 && byOccu <OCC_Max)
			return (DWORD)(m_vcOccuLvlExp[byOccu].size());
		return 0;
	}

	static DWORD	GetPresentExpPerMinute(uchar lLevel) {return (m_vectorPlayerExp.size()>lLevel && lLevel>=0)?m_vectorPlayerExp[lLevel].lPresentExpPerMinute:0;}
	static DWORD	GetMaxPresentExp(uchar lLevel) {return (m_vectorPlayerExp.size()>lLevel && lLevel>=0)?m_vectorPlayerExp[lLevel].lMaxPresentExp:0;}
	static float	GetFacBoonExpScale(long lLevel){return ((long)(m_vectorPlayerExp.size())>=lLevel && lLevel>0)?m_vectorPlayerExp[lLevel - 1].fFacBoonExpScale:0.0f;}
	static float	GetBaseCP(long lLevel);
	
	static list<tagOrginEquip>*		GetOrginEquipList()	{return &m_listOrginEquip;}

public:
	static bool LoadOccuModifyProperty(const char* filename);            //玩家职业修正属性表
	//
	static bool LoadPlayerExpList(const char* filename);		// 从文件读取exp列表

	static bool LoadOccuLvlExpList();//装载职业等级经验列表

	static bool LoadOccuDefParam(const char* filename);		// 从文件读取职业防御修正
	static bool LoadOccuAtkParam(const char* filename);		// 从文件读取职业攻击修正
	
	static bool AddToByteArray(vector<uchar>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(uchar* pByte, long& pos);	// 解码

	//得到职业等级系数
	static float GetOccuLvlCoe(eOccupation byOccu,WORD wOccuLevel);
	static void GetDefParam(eOccupation byOccu,WORD wLeve,float& fDef,float& fMDef);
	static void GetAtkParam(eOccupation byOccu,WORD wLeve,float& fAtk,float& fMAtk);
	static void SetBaseFightProperty(tagPPro *pPPro,tagPFPro *pPFPro);
	static void SetFightProperty(tagPPro *pPPro,tagPFPro *pBasePFPro,tagPFPro *pExPFPro);

    // 得到当前职业等级可分配的点数
    static WORD GetOccuLvlPoint(eOccupation byOccu, WORD wOccuLevel);
	

	static double GetOccuValueRatio(const char* pszName,int nOccu);
};
