/*
** $Id: lfunc.h,v 2.4.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#ifndef lfunc_h
#define lfunc_h


#include "lobject.h"


#define sizeCclosure(n)    (cast(int, sizeof(CClosure)) + \
                         cast(int, sizeof(TValue)*((n)-1)))

#define sizeLclosure(n)    (cast(int, sizeof(LClosure)) + \
                         cast(int, sizeof(TValue *)*((n)-1)))


LUAI_FUNC Proto * LUACALL luaF_newproto (lua_State *L);
LUAI_FUNC Closure * LUACALL luaF_newCclosure (lua_State *L, int nelems, Table *e);
LUAI_FUNC Closure * LUACALL luaF_newLclosure (lua_State *L, int nelems, Table *e);
LUAI_FUNC UpVal * LUACALL luaF_newupval (lua_State *L);
LUAI_FUNC UpVal * LUACALL luaF_findupval (lua_State *L, StkId level);
LUAI_FUNC void LUACALL luaF_close (lua_State *L, StkId level);
LUAI_FUNC void LUACALL luaF_freeproto (lua_State *L, Proto *f);
LUAI_FUNC void LUACALL luaF_freeclosure (lua_State *L, Closure *c);
LUAI_FUNC void LUACALL luaF_freeupval (lua_State *L, UpVal *uv);
LUAI_FUNC const char * LUACALL luaF_getlocalname (const Proto *func, int local_number,
                                         int pc);


#endif
