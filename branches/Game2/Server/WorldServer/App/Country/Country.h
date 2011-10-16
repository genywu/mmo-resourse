// 国家类
// 主要是构成国家对象
// 包括国家基本属性和操作
// bugs
// 06/6/27

#pragma once
#ifndef _Country_h_
#define _Country_h_

#include "CountryParam.h"

class CCountry
{
public:
	CCountry(void);

	virtual ~CCountry(void);


public:
	enum eState
	{
		PEACE,									// 和平
		WAR,									// 战争
	};

private:
	BYTE			m_lID;						// 国家ID  // 这个ID是固定了的。
												// 可能从配置文件里获取，可能在数据库字典表里获取。
	long			m_lPower;					// 国力 上线为1亿
	CGUID			m_King;					// 国王ID
	long			m_lTechLevel;				// 科技等级

	eState			m_eState;					// 国家当前状态

	//vector<long>	m_lRegions;					// 国家领地场景ID
	//vector<long>	m_lFactions;				// 国家拥有帮会
public:
	void			SetID(BYTE id)				{ m_lID = id; }
	BYTE			GetID()						{ return m_lID; }

	void			SetPower(long power)
	{
		if( power > GetCountryParam()->m_lMaxCountryPower ) m_lPower = GetCountryParam()->m_lMaxCountryPower;
		else m_lPower = power;
	}
	long			GetPower()					{ return m_lPower; }

	void			SetKing(const CGUID& king)	{ m_King = king; }
	const CGUID&	GetKing()					{ return m_King; }

	void			SetTechLevel(long level)	{ m_lTechLevel = level; }
	long			GetTechLevel()				{ return m_lTechLevel;}

	void			SetState(eState state)		{ m_eState = state; }
	eState			GetState()					{ return m_eState; }

public:
	bool			IsWar()						{ return m_eState == WAR; }

	CCountry*		CloneSaveData();													//克隆保存数据

	bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};

#endif