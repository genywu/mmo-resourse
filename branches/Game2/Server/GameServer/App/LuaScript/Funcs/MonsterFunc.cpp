#include "stdafx.h"
#include "lua.hpp"
#include "MonsterFunc.h"
#include "../ScriptSys.h"		// for ScriptSys
#include "../../RgnManager.h"	// for CRgnManager
#include "../../Monster.h"		// for CMonster
#include "../../StateAI/MonsterAI.h"
#include "../../StateAI/EventImpl/Event.h"

namespace ScriptFuncs
{
	///
	/// \brief 创建怪物
	/// \detail CGUIDs CreateMonster(monsterOriginalName, monsterAmount, coordinateX, coordinateY, width, height, rgnId, scriptFilename, talkFilename, dir)
	///
	static int CreateMonster(lua_State* L)
	{
		Script* script = GetInst(ScriptSys).GetScript(L);
		if(script==NULL)
			return 0;

		int luaIdx=1;
		const char* monsterOriginalName = lua_tostring(L, luaIdx++);
		if(monsterOriginalName==NULL)
			return 0;

		int monsterAmount = (int) tolua_tonumber(L, luaIdx++, 1);
		if(monsterAmount <= 0)
			return 0;

		int coordinateX = (int)tolua_tonumber(L, luaIdx++, 0);
		int coordinateY = (int)tolua_tonumber(L, luaIdx++, 0);
		int width = (int)tolua_tonumber(L, luaIdx++, 1);
		int height = (int)tolua_tonumber(L, luaIdx++, 1);
		int rgnId = (int)tolua_tonumber(L, luaIdx++, -1);
		const char* scriptFilename = TOLUA_TOSTRING( scriptFilename,L, luaIdx++, NULL);
		const char* talkFilename = TOLUA_TOSTRING( talkFilename,L, luaIdx++, NULL);
		int dir = (int)tolua_tonumber(L, luaIdx++, -1);

		CServerRegion* region = NULL;
		if(rgnId != -1)
			region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnId);
		else
			region = dynamic_cast<CServerRegion*>(script->GetEnv().region);

		if(region != NULL)
		{
			long x, y;
			vector<CGUID> guids;
			for(int i=0; i<monsterAmount; ++i)
			{
				region->GetRandomPosInRange(x, y, coordinateX, coordinateY, width, height);
				CMonster* monster = region->AddMonster(monsterOriginalName, x, y, dir);
				if(monster != NULL) 
				{
					guids.push_back(monster->GetExID());
					if(scriptFilename != NULL)
						monster->SetScriptFile(scriptFilename);
					if(talkFilename != NULL)
						monster->SetTalkScriptFile(talkFilename);
				}
			}
			for(vector<CGUID>::iterator it = guids.begin(); it != guids.end(); ++it)
			{
                Script::PushGCGUID( L, *it );
			}
			return (int) guids.size();
		}
		else//地图在其他服务器
		{
			//CMessage msg(MSG_S2W_SERVER_CREATEMONSTER);
			//if(region)
			//	msg.Add(region->GetExID());
			//else
			//	msg.Add(NULL_GUID);
			//msg.Add(p);
			//msg.Add((long)nCounts);
			//msg.Add((long)left);
			//msg.Add((long)top);
			//msg.Add((long)right);
			//msg.Add((long)bottom);
			//if(strScript)
			//{
			//	msg.Add(BYTE(1));
			//	msg.Add(strScript);
			//}
			//else 
			//{
			//	msg.Add(BYTE(0));
			//}
			//msg.Send();
			//return lua_yield(L, 0);
            return 0;
		}
	}

    /// 
    /// 对指定目标释放技能
    /// ret = UseSkill( id, level, targetID [,trace], [traceTime] )
    /// ret = UseSkill( id, level, cellX, cellY )
    /// @param id 技能ID
    /// @param level 技能等级
    /// @param targetID 目标GUID
    /// @param trace true表示自动追踪目标
    /// @param traceTime 追踪时间（毫秒）
    /// @param cellX 目标格子X坐标
    /// @param cellY 目标格子Y坐标
    /// @return 成功返回0，失败错误码：1:超时；2:错误
    ///
    static int UseSkill( lua_State *L )
    {
        Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
        CMonster *monster = dynamic_cast<CMonster*>( env.srcShape );
        if( monster == NULL )
        {
            tolua_pushnumber( L, 0 );
            return 1;
        }
        using namespace StateAI;
        MonsterAI *ai = static_cast<MonsterAI*>( monster->GetAI() );
        if( ai->CurStateType() != ST_FIGHT )
        {
            tolua_pushnumber( L, 0 );
            return 1;
        }
        long id = (long) tolua_tonumber( L, 1, 0 );
        long lvl = (long) tolua_tonumber( L, 2, 0 );
        long x, y;
        const CGUID *targetID = NULL;
        tolua_Error err;
        if( tolua_isnumber( L, 3, 0, &err ) )
        {
            x = (long) tolua_tonumber( L, 3, 0 );
            y = (long) tolua_tonumber( L, 4, 0 );
            ai->SendEvent( ScriptUseSkillEvent( script->ID(), x, y, id, lvl ) );
        }
        else
        {
            targetID = TOLUA_TOGUID( targetID, L, 3, (void*) &NULL_GUID );
            int trace = tolua_toboolean( L, 4, 0 );
            ulong traceTime = (ulong) tolua_tonumber( L, 5, 0 );
            ai->SendEvent( ScriptUseSkillEvent( script->ID(), *targetID, id, lvl, trace != 0, traceTime ) );
        }

        return lua_yield( L, 0 );
    }

	void OpenMonsterLibs(lua_State* L)
	{
		lua_register(L, "CreateMonster", CreateMonster);
		lua_register(L, "UseSkill", UseSkill);

        using namespace StateAI;
        REGISTER_ENUM( L, ST_FIGHT );
        REGISTER_ENUM( L, ST_DEAD );
        REGISTER_ENUM( L, ST_RETURN );
        REGISTER_ENUM( L, ST_ESCAPE );
        REGISTER_ENUM( L, ET_INIT );
        REGISTER_ENUM( L, ET_HURT );
        REGISTER_ENUM( L, ET_KILL );
        REGISTER_ENUM( L, ET_BORN );
        REGISTER_ENUM( L, ET_REBORN );
	}
}