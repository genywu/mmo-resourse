#pragma once
class CMoveShape;
#include "SkillAI.h"

class CNormalLongDistanceAttackAI : public CSkillAI
{
public:	
	enum FLY_MODE
	{
		FLY_TRACK = 0,		//跟踪飞行
		FLY_LOCK_LINE = 1	//锁定线
	};
	CNormalLongDistanceAttackAI();
	CNormalLongDistanceAttackAI(CSkillXml* pSkill);
	CNormalLongDistanceAttackAI(CSkillXml* pSkill, FLY_MODE mode);
	virtual ~CNormalLongDistanceAttackAI();

	virtual void AI();
	virtual bool Display();
	virtual bool StepBeginAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepRunAI(vector<long> vecType, vector<CGUID> vecID);
	virtual bool StepEndAI();

	struct tagObjectInfo
	{
		long lType;
		CGUID guid;
	};

protected:
	CSkillXml *m_pSkill;
	CMoveShape *m_pMoveshape;
	FLY_MODE	m_emFlyMode;
	void Release();

	struct tagEffect
	{
		tagEffect()
		{
			bgone = false;
			barrived = false;
			bloadinregion = false;
			fstartx = 0.0f;
			fstarty = 0.0f;
			fstarth = 0.0f;
			llasttime = 0;
			ldelaytime = 0;
			lstarttime = 0;
			lspeed = 0;
			dwlocator = 0;
			dweffectID = 0;
			dweffectindex = 0;	
			D3DXMatrixIdentity(&matrot);
		}
		tagObjectInfo objinfo;
		float fstartx;
		float fstarty;
		float fstarth;
		long  llasttime;
		long  ldelaytime;
		long  lspeed;
		long  lstarttime;
		bool  bgone;
		bool  barrived;
		bool  bloadinregion;
		DWORD dwlocator;
		DWORD dweffectID;
		DWORD dweffectindex;
		CGUID guidmyguid;
		D3DXMATRIX matrot;
	};
	vector<tagEffect> m_veceffect;
	/*vector<tagObjectInfo> m_vecObjInfo;	// 所有目标列表

	vector<float> m_vecFStartX;			// 每种飞行特效开始位置x坐标列表
	vector<float> m_vecFStartY;			// 每种飞行特效开始位置y坐标列表
	vector<float> m_vecFStartHeight;		// 每种飞行特效开始位置height坐标列表

	vector<float> m_vecFEndX;				// 每种飞行特效结束位置x坐标列表
	vector<float> m_vecFEndY;				// 每种飞行特效结束位置y坐标列表
	vector<float> m_vecFEndHeight;			// 每种飞行特效结束位置height坐标列表

	vector<DWORD> m_vecDwFlyingTime;		// 每种飞行特效飞行时间

	vector<DWORD> m_vecDwStartTime;			// 飞行开始时间
	vector<DWORD> m_vecLastTime;			// 飞行上周期时间

	bool m_bIsRun;							// 开始飞行的标志*/
};