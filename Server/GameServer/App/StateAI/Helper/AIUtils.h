///
/// @file AIUtilities.h
/// @author Kevin Lynx
/// @brief To provide some useful utilities for AI module.
///
#pragma once

#include <vector>
#include "../../Monster.h"
#include "../../PhaseSystem/PhaseManager.h"

namespace AIUtils
{
	///
	/// 生物列表（玩家、怪物等）
	///
	typedef std::vector<CShape*> ShapeListType;

	///
	/// 获取某个shape周围的shape列表（玩家、怪物等）
	/// 
	/// @return 查找到的shape数量
	///
	size_t FindAroundShape( CServerRegion *pRegion, CShape *pFinder, ShapeListType &outList );

	///
	/// 获取指定GUID的怪物或玩家
	///
	CMoveShape *FindShape( CServerRegion *pRegion, const CGUID &target_id );

	///
	/// 对shape列表进行对象过滤，常用于AI索敌
	///
	/// @param filter 过滤条件functor，返回true表示需要过滤此shape
	///
	template <typename Filter>
	size_t FilterShapeList( ShapeListType &shapes, Filter filter )
	{
		for( ShapeListType::iterator it = shapes.begin(); it != shapes.end(); )
		{
			if( filter( *it ) )
			{
				it = shapes.erase( it );
			}
			else
			{
				++ it;
			}
		}

		return shapes.size();
	}

	///
	/// To provide some pre-define filter
	///
	namespace FilterSet
	{
		///
		/// 过滤死亡shape
		///
		struct Died
		{
			bool operator()( CShape *pShape )
			{
				CMoveShape *pTarget = static_cast<CMoveShape*>( pShape );
				return pTarget->IsDied() == TRUE;
			}
		};

		///
		/// 过滤隐形相关shape
		///
		struct Invisible
		{
			Invisible( CMoveShape *pOwner )
			{
				m_pOwner = pOwner;
			}
			bool operator()( CShape *pShape )
			{
				CMoveShape *pTarget = static_cast<CMoveShape*>( pShape );
				if ( !pTarget->IsVisible(m_pOwner) )	
				{
					return true;
				}
				return false;
			}

			CMoveShape *m_pOwner;
		};

		///
		/// 过滤超出警戒范围的shape
		///
		struct GuardDis
		{
			GuardDis( CMonster *pMonster )
			{
				m_pOwner = pMonster;
			}
			bool operator()( CShape *pShape )
			{
				float guardRange = m_pOwner->GetBaseGuardRange();
				return m_pOwner->RealDistance( pShape ) > guardRange;
			}
	
			CMonster *m_pOwner;
		};

        ///
        /// 过滤超出追击范围的shape
        ///
		struct TrackDis
		{
			TrackDis( CMonster *pMonster )
			{
				m_pOwner = pMonster;
			}
			bool operator()( CShape *pShape )
			{
				float trackRange = m_pOwner->GetBaseChaseRange();
				return m_pOwner->RealDistance( pShape ) > trackRange;
			}
	
			CMonster *m_pOwner;
		};

		///
		/// 愚弄
		///
		struct Fool
		{
			Fool( const CGUID &id ) : m_FoolID( id )
			{
			}
			bool operator()( CShape *pShape )
			{
				return m_FoolID == pShape->GetExID();
			}
			const CGUID &m_FoolID;
		};

		///
		/// 无敌状态
		///
		struct GodState
		{
			bool operator()( CShape *pShape )
			{
				CMoveShape *pMoveShape = static_cast<CMoveShape*>( pShape );
				return pMoveShape->GetStateParamByID( STATE_BASE_GOD, "Num" ) != 0;
			}
		};

        /// 
        /// 过滤相位处理
        /// 
        struct PhaseShape 
        {
            PhaseShape(CMoveShape* Owner) : m_pOwner ( Owner )
            {     
            }

            bool operator() ( CShape* shape)
            {
                CMoveShape* pShape = static_cast<CMoveShape*>(shape);
                if ( PHASEMAN.PhasePlayerFilter(pShape ,m_pOwner))
                    return true;
                return false;
            }

            CMoveShape* m_pOwner;
        };

		///
		/// default filter
		///
		struct Default
		{
			Default( CMonster *pOwner )
			{
				m_pOwner = pOwner;
			}
			bool operator()( CShape *pShape )
			{
				Died died;
				Invisible invi( m_pOwner );
				GuardDis dis( m_pOwner );
				Fool fool( m_pOwner->GetBefoolID() );
				GodState god;
                PhaseShape phase( m_pOwner );
				return  phase( pShape) ||  died( pShape ) || invi( pShape ) || dis( pShape ) || fool( pShape ) || god( pShape );
			}
			CMonster *m_pOwner;
		};
	}

	///
	/// Some wrapper functions of the filter
	///
	inline bool IsInGodState( CShape *pShape )
	{
		FilterSet::GodState god_s;
		return god_s( pShape );
	}

	/// TODO : add more
}