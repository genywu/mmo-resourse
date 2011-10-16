#pragma once
#include "MoveShape.h"
#include "../../../Public/Common/SkillRelated.h"
//##召唤的没有生命的召唤物,比如毒雾,魔法阵
class CSummonShape : public CMoveShape
{
protected:
	//##生命长度，0表示不消失
	DWORD	m_dwLifeTime;

	//##诞生时间
	DWORD	m_dwTimeStamp;

	tagSkillID	m_tSkillID;
	long	m_lSkillLevel;
	long	m_lHostType;	// 主人类型
	long	m_lHostID;		// 主人id

public:
	//##构造/析构函数
    CSummonShape();
	virtual ~CSummonShape();

	DWORD	GetRemainedTime();
	DWORD	GetLifeTime()			{return m_dwLifeTime;}
	void	SetLifeTime(DWORD dw)	{m_dwLifeTime=dw;}

	//##返回召唤技能的类型
	virtual tagSkillID	GetSkillID()	{return m_tSkillID;}
	void SetSkillID(tagSkillID tID)		{m_tSkillID=tID;}
	long GetSkillLevel()				{return m_lSkillLevel;}
	void SetSkillLevel(long l)			{m_lSkillLevel=l;}

	void SetHostType(long lType)		{m_lHostType = lType;}
	void SetHostID(long lID)			{m_lHostID = lID;}
	long GetHostType()					{return m_lHostType;}
	long GetHostID()					{return m_lHostID;}
	
	virtual BOOL		DecodeFromByteArray( BYTE* pByte, LONG& lPos, bool bExData ) {return true;}
};
