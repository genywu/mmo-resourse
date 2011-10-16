#pragma  once
#include "SkillAI.h"
class CFixSummonAI : public CSkillAI
{
public:
	CFixSummonAI();
	CFixSummonAI(CSkillXml *pSkill);
	virtual ~CFixSummonAI();

	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepEndAI();

	DWORD	GetRemainedTime();
protected:
	CSkillXml *m_pSkill;

	long	m_lHostType;		// 目标对象类型
	long	m_lHostID;			// 目标编号
	DWORD	m_dwLifeTime;		// 残留时间
	DWORD	m_dwTimeStamp;		// 诞生时间标记

	vector<long> m_vecSummonEffect;		// 召唤特效列表
	vector<long> m_vecSummonSound;		// 召唤声效
private:
};