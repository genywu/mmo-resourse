///
/// @file ScriptSys.h
/// @author Kevin Lynx
///
#ifndef ___SCRIPT_SYS_H_
#define ___SCRIPT_SYS_H_

#include "Script.h"
#include "ScriptHolder.h"
#include <hash_map>
#include <vector>

struct lua_State;
class VariableList;
class CMoveShape;

/// singleton
class ScriptSys 
{
public:
	typedef stdext::hash_map<CGUID, Script*, hash_guid_compare> ScriptTableT;
	typedef stdext::hash_map<ulong, Script*> State2ScriptTableT;
	typedef std::vector<CGUID> DelScriptListT;
public:
	ScriptSys();

	bool Init();

	void Release();

	///
	/// Run a script file which was added into ScryptHolder before.
	///
	/// @return true if there's no error.
	///
	bool RunScript( const Script::RunEnv &env, const char *file );

	///
	/// Run a lua string.
	///
	/// @return true if there's no error.
	///
	bool RunStr( const Script::RunEnv &env, const char *str );

	/// Resume a yield script.
	bool ResumeScript( const CGUID &scriptID, int retCount = 0 );
	bool ResumeScript( Script *script, int retCount = 0 );

	/// Get the script from lua state.
	Script *GetScript( lua_State *L );

	/// Get the script from its id.
	Script *GetScript( const CGUID &scriptID );

	/// Decode globale variable list.
	void UpdateGeneralVarList( DBReadSet &db );

	/// Create a new script.
	Script *NewScript( const Script::RunEnv &env );

	/// Del a script immediately.
	void DelScript( Script *s );

	/// Push a script into the del cache list.
	/// Scripts in the cache will be deleted in the next game frame.
	void PushDelScript( const CGUID &scriptID );

	/// Push all the scripts assoicated with the shape into the cache.
	void PushDelScript( CMoveShape *shape );

	/// Del scripts in the cache.
	void DelScripts();

	ScriptHolder &GetHolder()
	{
		return *m_Holder;
	}

	VariableList *GetGeneralVarList()
	{
		return m_GeneralVarList;
	}

private:

    int RunCustomScript();

	void OpenLibs();

    /// Thread table is a table to store Lua thread, to prevent GC.
    void CreateThreadTable();

    /// Set the new thread in the thread table, the new thread must on the top stack
    void NewThread( lua_State *thread );

    /// Mark the thread nil to be GC later
    void DelThread( lua_State *thread );
private:
	lua_State *m_MainState;
	ScriptHolder *m_Holder;
	ScriptTableT m_Scripts;
	State2ScriptTableT m_State2ScriptTable;
	VariableList *m_GeneralVarList;
	DelScriptListT m_DelScripts;
};

#endif
