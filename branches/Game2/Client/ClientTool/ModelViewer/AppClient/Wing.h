#pragma once
#include "specialequip.h"

// 动作
enum eACTION
{
	ACT_NO = -1,			// 没有任何动作
	ACT_STAND,				// 站(建筑静止)
	ACT_IDLE,				// 站立idle
	ACT_WALK,				// 走
	ACT_RUN,				// 跑
	ACT_ATTACK,				// 攻擊
	ACT_BEATTACK,			// 被攻擊
	ACT_DIED,				// 死亡(建筑击毁)
	ACT_INTONATE,			// 吟唱
	ACT_BLOCK,				// 格档
	ACT_PARRY,				// 招架
	ACT_MISS,				// 闪避

	ACT_OPEN,				// 城门打开

	// 选人界面动作
	ACT_SEL_X0,				// 空手站立
	ACT_SEL_X1,				// 拔剑
	ACT_SEL_X2,				// 挥剑x
	ACT_SEL_X3,				// 持剑站立x
	ACT_SEL_X4,				// 收剑x
	ACT_SEL_X5,				// 恢复站立

	ACT_PERFORM = 50,		// 施法
	ACT_MAX_PERFORM = 70,	// 最大施法动作

	ACT_MAX = 100			// 最多多少个动作
};



class CWing : public CSpecialEquip
{
public:
	enum WING_STATUS 
	{
		WING_IDLE  = ACT_DIED,
		WING_STAND = ACT_STAND,
		WING_WALK  = ACT_WALK,
		WING_RUN   = ACT_RUN,
		//WING_HURT = CShape::ACT_BEATTACK,
	};

	CWing();
	~CWing();
	void ChangeStatus(DWORD eStaus);
};