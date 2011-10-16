//Account.h/////////////////////////////////////////////////////////////////////
//’À∫≈¿‡:
//Author:∞≤∫£¥®
//Create Time:2008.11.03
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <list>
#include <iostream>

#include "EntityGroup.h"

class CEntityGroup;
	// ============================================================================
	//  Account Instance Class
	// ============================================================================
class Account : public CEntityGroup
	{
	public:
		Account();
		Account(const CGUID& guid);
		~Account();
	
		virtual Account& operator=(Account& right);

		CEntityGroup* FindLoginPlayer(const CGUID& guid);

		void	AddLoginPlayer(CEntityGroup* lPlayer);
		void	DelLoginPlayer(const CGUID& guid);

		const CGUID& GetPlayerIdByName(const char* szName);

		long GetLastLoginTime(void) { return m_lLastLoginTime; }
		void SetLastLoginTime(long time) { m_lLastLoginTime = time; }

		long m_lLastLoginTime;

	};  // end class Account


#endif
