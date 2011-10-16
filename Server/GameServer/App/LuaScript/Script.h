///
/// @file Script.h
/// @author Kevin Lynx
/// 
#ifndef ___SCRIPT_H_
#define ___SCRIPT_H_

#include <list>

struct lua_State;
class CMoveShape;
class CRegion;
class VariableList;
class CPlayer;

#define REGISTER_ENUM( L, t ) lua_pushnumber( L, t ); lua_setglobal( L, #t )
#define TOLUA_TOSTRING( var, L, arg, def ) ( var=tolua_tostring( L, arg, def ), \
    var == NULL ? def : var )
#define TOLUA_TOUSERTYPE( var, L, arg, def, type ) (type) ( var=(type)tolua_tousertype( L, arg, (void*) def ), \
    var == NULL ? def : var )
#define TOLUA_TOGUID( var, L, arg, def ) TOLUA_TOUSERTYPE( var, L, arg, def, const CGUID* )

///
/// Wrap lua state as a script running instance.
///
class Script : public kl_common::cached_obj<Script>
{
public:
	enum VarListType
	{
		VT_SCRIPT,
		VT_SRCSHAPE,
		VT_DESTSHAPE,
		VT_REGION,
		VT_GLOBAL
	};

	enum
	{
		EXEC_ERROR = -1,	//executes a script function error
		EXEC_FAIL = 0,		//executes a script function unsuccessful
		EXEC_SUCCESS = 1,	//executes a script function successful
	};

	struct RunEnv
	{
		CMoveShape *srcShape;
		CMoveShape *destShape;
		CRegion *region;
		VariableList *varList;
		CGUID goodsId;

		RunEnv( CMoveShape *src = NULL, CMoveShape *dest = NULL, 
			CRegion *rgn = NULL, VariableList *vars = NULL, 
			const CGUID &goodsID = NULL_GUID ) : goodsId( goodsID )
		{
			srcShape = src; 
			destShape = dest;
			region = rgn;
			varList = vars;
		}
	};
public:
	Script();

	bool Init( const CGUID &id, lua_State *state );

	void InitEnv( const RunEnv &env );

	void Reset();

	/// Run a script which must exists in the script holder.
	int RunScript( const char *file );

	/// Compile the lua string and run it.
	int RunStr( const char *str );

	/// Resume a yield script with some return values.
	int Resume( int retCount = 0 );

	/// Push return value to return to the yield script.
	void PushRetValue( double val );
	void PushRetValue( const char *val );
	void PushRetValue( const CGUID &val );

    /// Push a guid value and register a gc for it.
    static void PushGCGUID( lua_State *L, const CGUID &val );

	/// Get specified variable list in the run env.
	VariableList *GetVarList( long type = VT_SCRIPT );

	/// Check whether the script can be deleted by the ret.
	bool CanDelete( int ret ) const;

	lua_State *QueryState()
	{
		return m_luaState;
	}

	const CGUID &ID() const
	{
		return m_ID;
	}

	const RunEnv &GetEnv() const
	{
		return m_RunEnv;
	}
	/// Some exported (to lua) function declatations
#include "Funcs/ScriptClientFunc.h"
#include "Funcs/ScriptBaseFunc.h"
#include "Funcs/ScriptRgnFunc.h"
#include "Funcs/ScriptGoodsFunc.h"
#include "Funcs/ScriptSkillFunc.h"
#include "Funcs/ScriptQuestFunc.h"
#include "Funcs/ScriptOrganFunc.h"
#include "Funcs/ScriptGlobalFunc.h"
#include "Funcs/ScriptPlayerFunc.h"
#include "Funcs/ScriptShapeFunc.h"
#include "Funcs/ScriptMonsterFunc.h"
#include "Funcs/ScriptPhaseFunc.h"
#include "Funcs/ScriptPetFunc.h"
#include "Funcs/ScriptMailFunc.h"
#include "Funcs/ScriptTauntFunc.h"
private:
	void PreRun();
	int BeginLocalEnv();
	void EndLocalEnv( int envT, int funcIndex );
	
	/// It will check whether the shape is srcShape or destShape first, then 
	/// find in the shape list.
	CMoveShape *FindShape( long type, const CGUID &id );
	CPlayer *FindPlayer( const CGUID &playerID );
	CPlayer *FindPlayer( const char *name );
private:
	lua_State *m_luaState;
	CGUID m_ID;
	RunEnv m_RunEnv;
};

#endif
