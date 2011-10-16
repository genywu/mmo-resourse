///
/// @file MonsterAI.h
/// @author Kevin Lynx
///
#ifndef ___MONSTERAI_H_
#define ___MONSTERAI_H_

#include "BaseAI.h"

class CMonster;

namespace StateAI
{
	class FightObj;
    class RunByRoad;
	///
	/// 怪物AI控制类
	///
	class MonsterAI : public BaseAI
	{
	public:
		MonsterAI( CMonster *owner );

		~MonsterAI();

		void Reset();

        void Run( long timerID, long timerType );

		bool BeginSkill( long skillId, long skillLvl, long x, long y, CMoveShape *target );

        bool BeginSkill( long skillId, long skillLvl, long x, long y );

        bool BeginSkill( long skillId, long skillLvl, CMoveShape *target );

        bool StartCycle( ulong trigerTime, ulong interTime );

        void StopCycle();

		bool RandomRun();

		void SetBornPos( long x, long y );

		void EscapeSpring();

        void GBHSpring( long hurt );

        bool IsInKindlyTime();

		Point GetBornPos() const
		{
			return m_BornPos;
		}

		FightObj &GetFightObj()
		{
			return *m_FightObj;
		}

        RunByRoad &GetRunByRoad()
        {
            return *m_RunByRoad;
        }
	private:
		virtual void OnEvent( const AIEvent &event );
	private:
        /// 出生点
		Point m_BornPos;
        /// 封装战斗状态特有的数据/操作
		FightObj *m_FightObj;
        /// 重生时间
        ulong m_RebornTime;
        /// 封装沿配置路径移动数据/操作
        RunByRoad *m_RunByRoad;
	};
}

#endif
