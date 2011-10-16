///
/// @file ScriptHolder.h
/// @author Kevin Lynx
///
#ifndef ___SCRIPT_HOLDER_H_
#define ___SCRIPT_HOLDER_H_

struct lua_State;

///
/// Hold the scripts chunk in lua state.
///
class ScriptHolder
{
public:
	ScriptHolder( lua_State *s )
	{
		m_luaState = s;
	}

	~ScriptHolder()
	{
	}

	bool Init();

	///
	/// Add a new script chunk in lua. It will compile the lua script and push the chunk in
	/// out table.
	///
	bool Add( const char *file, const char *buf, size_t size );

	/// Get a compiled lua chunk and push it onto the stack.
	bool Get( const char *file );

	/// Push onto the stack of the specified lua state.
	bool Get( const char *file, lua_State *L );

private:
	bool GetScriptTable( lua_State *L );
private:
	lua_State *m_luaState;
};

#endif
