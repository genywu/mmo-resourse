///
/// @file PetAI.h
/// @brief a traditional AI implementation for Pet, i copied codes from the original place.
///
#ifndef ___PET_AI_H_
#define ___PET_AI_H_

#include "BaseAI.h"

class CMoveShape;
namespace StateAI
{
    class PetAI : public BaseAI
    {
    public:
        PetAI( CMoveShape *owner );

        ~PetAI();

        void Run( long timerID, long timerType );

        bool OnMove( float destX, float destY );

        void Attack( long actID, long x = 0, long y = 0, CMoveShape *target = NULL );

        void SetTarget( const CGUID &targetID, long targetType );

        void SetHost( CMoveShape *host );

        /// shape state
        void ChangeStateTo( long state );

    private: 
        bool SearchEnemy();

        void AddRetPeaceAct();

        void LogoutRetPeaceAct();

        void OnReturnPeace();

        void OnDel();

        void OnDead();

        void OnHurt();

        void OnSkillEnd( long subType );

        void OnEvent( const AIEvent &event );

    private:
        CMoveShape *m_Host;
    };
}

#endif

