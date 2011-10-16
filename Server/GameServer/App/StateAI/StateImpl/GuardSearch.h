///
/// @file GuardSearch.h
/// @brief To implement the Guard AI
/// @history 10.31.2008 built by kl
///          4.8.2010 modified for StateAI
///
#ifndef ___GUARD_SEARCH_H_
#define ___GUARD_SEARCH_H_

#include <vector>
#include "../AIStateDefs.h"

class CMoveShape;
class CMonster;

namespace StateAI
{
    class GuardSearch : public AIState
    {
    public:
        /// the move shape list
        typedef std::vector<CShape*> ShapeListType;
    public:
        GuardSearch( MachineType *machine ) : AIState( machine, ST_SEARCH )
        {
        }

        ///
        /// Search enemy ruled by the document. 
        ///
        void Execute( BaseType::EntityType *entity );

        DECL_LIFE_POLICY_FUNC( GuardSearch, AIState );
    private:
        /// check whether the owner can attack the current target
        BOOL CanAttackCurrent( CMonster *owner, BaseType::EntityType *entity );

        /// search enemy by ai reference 1 
        CMoveShape *SearchByAIRef1( CMonster *owner, const ShapeListType &player_list, DWORD ai_ref );

        /// search enemy by ai reference 2
        CMoveShape *SearchByAIRef2( CMonster *owner, const ShapeListType &player_list, DWORD ai_ref );

        /// search enemy by ai reference 3
        CMoveShape *SearchByAIRef3( CMonster *owner, const ShapeListType &monster_list, DWORD ai_ref );

        /// search enemy by ai reference 4
        CMoveShape *SearchByAIRef4( CMonster *owner, const ShapeListType &monster_list, DWORD ai_ref );

        /// check whether the two countrys belong to the same camp
        BOOL IsSameCamp( BYTE country_id1, BYTE country_id2 );
        BOOL IsSameCamp( CMoveShape *pShape1, CMoveShape *pShape2 )
        {
            return IsSameCamp( pShape1->GetCountry(), pShape2->GetCountry() );
        }

        /// check whether the two shapes are the same country.
        BOOL IsSameCountry( CMoveShape *pShape1, CMoveShape *pShape2 )
        {
            return pShape1->GetCountry() == pShape2->GetCountry() ;
        }

        /// a helper function to check the attack condition
        bool _ai_ref3_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref );
        bool _ai_ref4_helper( CMonster *pSelf, CMonster *pOther, DWORD ai_ref );
    };

}
#endif // ___GUARD_H_
