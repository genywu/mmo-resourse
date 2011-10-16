///
/// @file BaseAI.h
/// @author Kevin Lynx
/// 
#ifndef ___BASE_AI_H_
#define ___BASE_AI_H_

#include "AIStateDefs.h"
#include "GlobalDef.h"
#include "StateSet.h"
#include "ShareObjTable.h"

class CMoveShape;
class CRegion;
struct stModuParam;

#define OPER_FAILED_TIME (1000)
#define CONTINUE_TIME (100)

namespace StateAI
{
	class AIDriver;

	///
	/// AI基类，封装状态机管理，提供公共数据/操作。
	///
	class BaseAI
	{
	public:
		BaseAI( CMoveShape *owner );

		virtual ~BaseAI();

		virtual bool Init( const ImplConfig &impl, long init_state );

		virtual void Reset();

		virtual void Release();

		void ChangeState( long state );

		void ChangeState( long state, const AITransition &tran );

		void ChangeState( long state, const AIEvent &ev );

		AIState *GetState( long type );

		bool HasState( long state );

		virtual void Run( long timerID, long timerType );
		
		bool Resume( unsigned long trigerTime );

		void Stop();

        bool SetTimer( long type, ulong trigerTime, ulong interTime = 0 );

        void StopTimer( long type );

		void SendEvent( const AIEvent &event );

		bool HasTarget() const;

		virtual void SetTarget( const CGUID &targetID, long targetType );

		CMoveShape *GetTarget() const;

		bool BeginSkill( stModuParam *moduParam, long x, long y, CMoveShape* target );

		/// AI Operations
        void RunScript( const char *file );

		void Stand( unsigned long time );

        /// 按照指定方向移动一个格子，该函数会根据目标点情况自动调整方向
		bool Move( long dir );

        /// 向指定坐标移动一格（移动结果坐标不一定等于目标坐标）
		bool Move( long destX, long destY );

        /// 响应移动
		virtual bool OnMove( float destX, float destY );

        /// 检查指定格子是否可到
		bool IsCanMoveTo( CRegion *region, long x, long y ) const ;

        /// 在指定方向获取指定距离的下一个坐标，该坐标是一个大概值。如果指定方向上的坐标
        /// 有阻挡，则这个方向会做调整。
		bool GetNextMoveDestCell( long dir, long dis, long &x, long &y ) const;

        /// 取得当前状态类型
        long CurStateType();

		ShareObjTable &GetShareObjTable()
		{
			return m_ShareObjs;
		}

		CMoveShape *GetOwner()
		{
			return m_Owner;
		}
	protected:
		long ToShapeState( long aiState );

        AIState *CurState();

		virtual void OnEvent( const AIEvent &event );
	protected:
        /// store states
		StateSet m_States;
        /// state machine
		MachineType *m_Machine;
        /// not used
		ShareObjTable m_ShareObjs;
        /// a timer wrapper, to drive the AI
		AIDriver *m_AIDriver;
		CMoveShape *m_Owner;
        /// attack target info
		CGUID m_TargetID;
		long m_TargetType;
        /// event script handles ai events
        std::string m_EventScript;
	};
}
#endif
