#pragma once
#ifndef _CContributeSetup_h_
#define _CContributeSetup_h_
class CContributeSetup
{
public:
	struct tagContributeItem
	{
		DWORD	dwLoValue;		// 物品下限
		DWORD	dwHiValue;		// 物品上限
		DWORD	dwNum;			// 物品数量
		string	strName;		// 怪物原始名
	};


public:
	CContributeSetup(void);
	virtual ~CContributeSetup(void);


//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:

	static long	lCombatLevel1;
	static long	lCombatLevel2;

	static long	lContribute1;
	static long	lContribute2;

	static long	lContributeLevel;
	static long	lContributeMultiple;

	static long	lContributeBaseValue;
	static long	lContributeMax;

	static long	lCountryCityContributeModifierLose;
	static long	lCountryCityContributeModifierGain;
	static long	lCountryCityContributeItemModifier;


	static vector<tagContributeItem>	m_vContributeItems;	// 物品列表

public:
	static BOOL LoadContributeSetup(const char* filename);			// 从文件读取列表

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};

#endif