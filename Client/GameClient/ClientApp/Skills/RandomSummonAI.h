#pragma  once
#include "SkillAI.h"
class CRandomSummonAI : public CSkillAI
{
public:
	CRandomSummonAI();
	CRandomSummonAI(CSkillXml *pSkill);
	virtual ~CRandomSummonAI();

	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepEndAI();

	// 取得客户端的残留时间
	DWORD	GetRemainedTime();
protected:
	CSkillXml *m_pSkill;
	
	long  m_lHostType;		// 目标对象类型
	long  m_lHostID;		// 目标编号
	DWORD m_dwLifeTime;		// 残留时间
	DWORD m_dwTimeStamp;		// 诞生时间标记
	DWORD m_dwFrequency;
	DWORD m_dwTotalTime;		// 播放总时间
	long  m_lPreSkillTime;		// 每个召唤物播放的间隔时间
	long  m_lDeleTime;			// 召唤物播放的延迟时间
	vector<long> m_vecSummonEffect; // 召唤特效列表
	vector<long> m_vecSummonSound;	// 召唤声效
	// 攻击点结构
	struct tagCell
	{
		long lX;
		long lY;
	};

	vector<tagCell>	m_vecShotPoint;		// 发射点列表
	long			m_lCurShotPoint;	// 当前的发射点
};