///
/// @file FightObj.h
/// @author Kevin Lynx
///
#ifndef ___FIGHT_OBJ_H_
#define ___FIGHT_OBJ_H_

#include "../GlobalDef.h"

class CMonster;

namespace StateAI
{
	class MonsterAI;
	///
	/// 战斗状态辅助类，用于提供公共数据/接口。
	///
	class FightObj
	{
	public:
		FightObj( MonsterAI *ai ) ;

		void Reset();

        /// 检查是否该归位
		bool CheckReturnPeace();

        /// 受伤触发
		void OnHurted();

        /// 脱离战斗触发
		void LoseTarget();

        /// 进入战斗触发
		void EnterFight();

        void ReturnPeace();
	private:
		unsigned long m_HurtTime;
		unsigned long m_GuardTime;
		MonsterAI *m_AI;
		CMonster *m_Owner;
		Point m_EnterFightPos;
		// may keep the target info.
	};
}

#endif
