///
/// @file MonsterAISetup.h
/// @author Kevin Lynx
/// @brief Load Monster AI setup file. "data/MonsterAI.xml"
///
#ifndef ___MONSTER_AI_SETUP_H_
#define ___MONSTER_AI_SETUP_H_

#include <map>

class TiXmlElement;

namespace StateAI
{
	/// A config table to reprensent AI state implementations.
	class ImplConfig
	{
	public:
        enum { SCRIPT_LEN = 256 };
        struct Impl
        {
            /// impl type
            long impl;
            /// script file name if has
            char script[SCRIPT_LEN];
        };
		// <type, impl>
		typedef std::map<long, Impl> ImplTableT;

	public:

		void Encode( DBWriteSet &db ) const;

		void Decode( DBReadSet &db );

		Impl &operator[] ( long type )
		{
			return m_Impl[type];
		}

        void AddImpl( long type, long impl );

		const ImplTableT &Raw() const 
		{
			return m_Impl;
		}
	public:
		ImplTableT m_Impl;
        char m_EventScript[SCRIPT_LEN];
	};

	/// Singleton 
	class MonsterAISetup
	{
	public:
		typedef std::map<long, ImplConfig> AITableT;
	public:
		void Encode( DBWriteSet &db ) const;

		void Decode( DBReadSet &db );

		bool Load();

		void Clear();

		const ImplConfig *GetImpl( long aiType ) const;

	private:
		bool LoadImpls( TiXmlElement *elem );
	private:
		AITableT m_AITable;
	};
}

#endif
