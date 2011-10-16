///
/// @file Guard.h
/// @brief To implement the Guard AI
/// @history 10.31.2008 built by kl
///
#ifndef ___GUARD_H_
#define ___GUARD_H_

#include "MonsterAI.h"
#include "../MoveShape.h"
#include <vector>

class Guard : public CMonsterAI
{
public:
	/// the move shape list
	typedef std::vector<CShape*> ShapeListType;
protected:
	///
	/// Search enemy ruled by the document. 
	///
	virtual BOOL SearchEnemy();

private:
	/// check whether the owner can attack the current target
	BOOL CanAttackCurrent();

	/// search enemy by ai reference 1 
	CMoveShape *SearchByAIRef1( const ShapeListType &player_list, DWORD ai_ref );
	
	/// search enemy by ai reference 2
	CMoveShape *SearchByAIRef2( const ShapeListType &player_list, DWORD ai_ref );
	
	/// search enemy by ai reference 3
	CMoveShape *SearchByAIRef3( const ShapeListType &monster_list, DWORD ai_ref );

	/// search enemy by ai reference 4
	CMoveShape *SearchByAIRef4( const ShapeListType &monster_list, DWORD ai_ref );

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

#endif ___GUARD_H_ // ___GUARD_H_
