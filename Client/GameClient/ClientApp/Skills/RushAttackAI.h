#pragma once
#include "SkillAI.h"

class CRushAttackAI : public CSkillAI
{
public:
	CRushAttackAI();
	CRushAttackAI(CSkillXml* pSkill);
	virtual ~CRushAttackAI();

	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);	// 执行具体功能(返回false表示删除)
	virtual bool StepEndAI();	// 执行结束处理(返回false表示删除)

protected:
	struct tagTrail
	{
		DWORD dwStartTime;
		DWORD dwLifeTime;
		DWORD dwCurColor;
		DWORD dwDefaultColor;
		float x, y, fHeight;

		tagTrail *pNext;
	};
	tagTrail *m_pTrail;

	CSkillXml *m_pSkill;
	float m_fStartX;
	float m_fStartY;
	float m_fEndX;
	float m_fEndY;

	DWORD m_dwFlyingTime;	// 飞行时间
	DWORD m_dwStartTime;	// 开始时间
};