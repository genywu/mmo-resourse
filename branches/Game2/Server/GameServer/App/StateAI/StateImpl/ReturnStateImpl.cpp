///
/// @file ReturnStateImpl.cpp
///
#include "stdafx.h"
#include "ReturnStateImpl.h"
#include "../MonsterAI.h"
#include "../../Monster.h"

#include "../../../../../Public/Setup/GlobalSetup.h"

namespace StateAI
{
	IMPL_LIFE_POLICY_FUNC( ReturnStateNormal, AIState );

	void ReturnStateNormal::Enter( BaseType::EntityType *entity, const BaseType::TransitionType &tran )
	{
		const EventTransition &etran = (const EventTransition&) tran;
		assert( etran.event.Type() == ET_RETURN );
		const ReturnEvent &event = (const ReturnEvent&) etran.event;
		m_Data.destX = event.destX;
		m_Data.destY = event.destY;

		CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
        if( monster->GetIsResumeAll() == 0 )
        {		
            entity->Resume( GlobalSetup::GetSetup()->dwMonsterTransitionTime );
            return ;
        }

		long dis = monster->Distance( m_Data.destX, m_Data.destY );
		long time = (long) ( dis / GlobalSetup::GetSetup()->fReturnPeaceSpeed );
		if( time > (long)GlobalSetup::GetSetup()->dwReturnPeaceEffectTime )
		{
			time = GlobalSetup::GetSetup()->dwReturnPeaceEffectTime;
		}
		m_OutTime = time + timeGetTime();
		monster->SendLeaveFightMessage( m_Data.destX, m_Data.destY, time );

		long speed = (long)( 1 / ( GlobalSetup::GetSetup()->fReturnPeaceSpeed - monster->GetSpeed() ) );
		monster->SetChangeSpeed( (short) speed );
		monster->SendChangedSpeedToClients();
		monster->God();

		// no transition state
		entity->Resume( GlobalSetup::GetSetup()->dwMonsterTransitionTime );

		m_OutTime += GlobalSetup::GetSetup()->dwMonsterTransitionTime;
	}
	
	void ReturnStateNormal::Leave( BaseType::EntityType *entity, const BaseType::TransitionType &tran )
	{
		CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
        if( monster->GetIsResumeAll() != 0 )
        {
            monster->SetChangeSpeed( 0 );
            monster->SendChangedSpeedToClients();
            monster->God( false );

            // reset all properties
            monster->ResumeAll();
        }
		entity->Resume( 1000 );
	}

	void ReturnStateNormal::ReceiveEvent( BaseType::EntityType *entity, const BaseType::EventType &ev )
	{
	}

	void ReturnStateNormal::Execute( BaseType::EntityType *entity )
	{
		CMonster *monster = static_cast<CMonster*>( entity->GetOwner() );
        if( monster->GetIsFlyToPeace() == 0 )
        {
            entity->Reset();
            // reset search action
            AIState *searchAction = entity->GetState( ST_SEARCH );
            assert( searchAction );
            searchAction->Reset();
            return ;
        }

		unsigned long thisTime = timeGetTime();
		if( thisTime < (unsigned long)m_OutTime )
		{
			long x = monster->GetTileX();
			long y = monster->GetTileY();
			long dir = GetLineDir( x, y, m_Data.destX, m_Data.destY );
			monster->GetDirPos( dir, x, y );
			float dis = ( dir % 2 != 0 ? 1.414f : 1.0f );
			float time = dis / monster->GetSpeed();

			monster->SetPosXY( (float)x, (float)y);
			monster->OnMove( x + 0.5f, y + 0.5f );
			
			entity->Resume( (unsigned long) time );
		}
		else
		{
			monster->SetPosition( m_Data.destX, m_Data.destY );

            // set position failed ? ( to aviod a bug )
            if( monster->GetTileX() != m_Data.destX || monster->GetTileY() != m_Data.destY )
            {
                // find another position to set
                CRegion *region = static_cast<CRegion*>( monster->GetFather() );
                assert( region );
                long rx, ry;
                region->GetRandomPosInRange( rx, ry, m_Data.destX - 3, m_Data.destY - 3, 6, 6 );
                monster->SetPosition( rx, ry );
            }
			// change state to peace state, and reset all properties
			entity->Reset();
            // reset search action
            AIState *searchAction = entity->GetState( ST_SEARCH );
            assert( searchAction );
            searchAction->Reset();
		}
	}
}
