///
/// @file ClientFunc.cpp
/// @brief 与客户端交互的全局函数
/// @see ClientFunc.h
///
#include "stdafx.h"
#include "lua.hpp"
#include "../ScriptSys.h"
#include "../../Player.h"

namespace ScriptFuncs
{
	///
	/// 弹出信息对话框
	///
	/// talkRet, chooseRet = TalkBox( str, btn1, btn1Enable, btn2, btn2Enable, 
	/// resID, closeWin, picID, picX, picY, musicID, sndID )
	///
	/// @param str 信息字符串
	/// @return talkRet 玩家对按钮的操作
	/// @return chooseRet 玩家对物品等的选择
	///
	static int TalkBox( lua_State *L )
	{
		const char *str = TOLUA_TOSTRING( str, L, 1, "" );
		const char *btn1 = TOLUA_TOSTRING( btn1, L, 2, "" );
		uchar btn1Enable = (uchar) tolua_tonumber( L, 3, 0 );
		const char *btn2 = TOLUA_TOSTRING( btn2, L, 4, "" );
		uchar btn2Enable = (uchar) tolua_tonumber( L, 5, 0 );
		long resID = (long) tolua_tonumber( L, 6, 1 );
		uchar closeWin = (uchar) tolua_tonumber( L, 7, 0 );
		long picID = (long) tolua_tonumber( L, 8, 0 );
		long picX = (long) tolua_tonumber( L, 9, 0 );
		long picY = (long) tolua_tonumber( L, 10, 0 );
		long musicID = (long) tolua_tonumber( L, 11, 0 );
		long sndID = (long) tolua_tonumber( L, 12, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENTALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( str );	

			msg.Add( btn1 );
			msg.Add( btn1Enable );
			msg.Add( btn2 );
			msg.Add( btn2Enable );

			msg.Add( resID );
			msg.Add( closeWin );
			msg.Add( picID );
			msg.Add( picX );	
			msg.Add( picY );	
			msg.Add( sndID );	
			msg.Add( musicID );
			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出信息对话框，文字资源从客户端资源中获取
	///
	/// talkRet, chooseRet = TalkBoxByIndex( textType, textID, btn1, btn1Enable, 
	/// btn2, btn2Enable, resID, closeWin, picID, picX, picY, musicID, sndID )
	///
	/// @see TalkBox
	///
	static int TalkBoxByIndex( lua_State *L )
	{
        long i = 0;
        const char* textid = TOLUA_TOSTRING( textid, L, ++i, "" );
		const char *btn1 = TOLUA_TOSTRING( btn1, L, ++i, "" );
		uchar btn1Enable = (uchar) tolua_tonumber( L, ++i, 0 );
		const char *btn2 = TOLUA_TOSTRING( btn2, L, ++i, "" );
		uchar btn2Enable = (uchar) tolua_tonumber( L, ++i, 0 );
		long resID = (long) tolua_tonumber( L, ++i, 1 );
		uchar closeWin = (uchar) tolua_tonumber( L, ++i, 0 );
		long picID = (long) tolua_tonumber( L, ++i, 0 );
		long picX = (long) tolua_tonumber( L, ++i, 0 );
		long picY = (long) tolua_tonumber( L, ++i, 0 );
		long musicID = (long) tolua_tonumber( L, ++i, 0 );
		long sndID = (long) tolua_tonumber( L, ++i, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENTALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( textid );	
			msg.Add( btn1 );
			msg.Add( btn1Enable );
			msg.Add( btn2 );
			msg.Add( btn2Enable );

			msg.Add( resID );
			msg.Add( closeWin );
			msg.Add( picID );
			msg.Add( picX );	
			msg.Add( picY );	
			msg.Add( sndID );	
			msg.Add( musicID );
			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出图片对话框
	///
	/// talkRet, chooseRet = PicTalkBox( str, topPic, backPic, btn1, btn1Enable, 
	/// btn2, btn2Enable, resID, closeWin, picID, picX, picY, musicID, sndID )
	///
	/// @see TalkBox
	///
	static int PicTalkBox( lua_State *L )
	{
		const char *str = TOLUA_TOSTRING( str, L, 1, "" );
		const char *topPic = TOLUA_TOSTRING( topPic, L, 2, "" );
		const char *backPic = TOLUA_TOSTRING( backPic, L, 3, "" );
		const char *btn1 = TOLUA_TOSTRING( btn1, L, 4, "" );
		uchar btn1Enable = (uchar) tolua_tonumber( L, 5, 0 );
		const char *btn2 = TOLUA_TOSTRING( btn2, L, 6, "" );
		uchar btn2Enable = (uchar) tolua_tonumber( L, 7, 0 );
		long resID = (long) tolua_tonumber( L, 8, 1 );
		uchar closeWin = (uchar) tolua_tonumber( L, 9, 0 );
		long picID = (long) tolua_tonumber( L, 10, 0 );
		long picX = (long) tolua_tonumber( L, 11, 0 );
		long picY = (long) tolua_tonumber( L, 12, 0 );
		long musicID = (long) tolua_tonumber( L, 13, 0 );
		long sndID = (long) tolua_tonumber( L, 14, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPEN_PIC_TALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( str );	
			msg.Add( topPic );
			msg.Add( backPic );

			msg.Add( btn1 );
			msg.Add( btn1Enable );
			msg.Add( btn2 );
			msg.Add( btn2Enable );

			msg.Add( resID );
			msg.Add( closeWin );
			msg.Add( picID );
			msg.Add( picX );	
			msg.Add( picY );	
			msg.Add( sndID );	
			msg.Add( musicID );
			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出情节描述框
	///
	/// talkRet, chooseRet = StoryTalkBox( str, pointContinu, endTalkBox, sndID, musicID,
	/// headID, npcName, faceID1, faceTime1, faceID2, faceTime2, faceID3, faceTime3,
	/// faceID4, faceTime4, faceID5, faceTime5 )
	///
	/// @see TalkBox
	///
	static int StoryTalkBox( lua_State *L )
	{
		const char *str = TOLUA_TOSTRING( str, L, 1, "" );
		uchar pointContinu = (uchar) tolua_tonumber( L, 2, 0 );
		uchar endTalkBox = (uchar) tolua_tonumber( L, 3, 0 );
		long sndID = (long) tolua_tonumber( L, 4, 0 );
		long musicID = (long) tolua_tonumber( L, 5, 0 );
		long headID = (long) tolua_tonumber( L, 6, 0 );
		const char *npcName = TOLUA_TOSTRING( npcName, L, 7, 0 );
		long faceID1 = (long) tolua_tonumber( L, 8, 0 );
		long faceTimes1 = (long) tolua_tonumber( L, 9, 0 );
		long faceID2 = (long) tolua_tonumber( L, 10, 0 );
		long faceTimes2 = (long) tolua_tonumber( L, 11, 0 );
		long faceID3 = (long) tolua_tonumber( L, 12, 0 );
		long faceTimes3 = (long) tolua_tonumber( L, 13, 0 );
		long faceID4 = (long) tolua_tonumber( L, 14, 0 );
		long faceTimes4 = (long) tolua_tonumber( L, 15, 0 );
		long faceID5 = (long) tolua_tonumber( L, 16, 0 );
		long faceTimes5 = (long) tolua_tonumber( L, 17, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENSTORYTALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( str );	
			msg.Add( pointContinu );
			msg.Add( endTalkBox );
			msg.Add( sndID );
			msg.Add( musicID );
			msg.Add( headID );
			msg.Add( npcName );
			msg.Add( faceID1 );
			msg.Add( faceTimes1 );
			msg.Add( faceID2 );
			msg.Add( faceTimes2 );
			msg.Add( faceID3 );
			msg.Add( faceTimes3 );
			msg.Add( faceID4 );
			msg.Add( faceTimes4 );
			msg.Add( faceID5 );
			msg.Add( faceTimes5 );

			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出故事描述框
	///
	/// talkRet, chooseRet = StoryTalkBoxByIndex( textType, textID, pointContinu, endTalkBox, sndID, musicID,
	/// headID, npcName, faceID1, faceTime1, faceID2, faceTime2, faceID3, faceTime3,
	/// faceID4, faceTime4, faceID5, faceTime5 )
	///
	/// @see StoryTalkBox
	///
	static int StoryTalkBoxByIndex( lua_State *L )
	{
		long textType = (long) tolua_tonumber( L, 1, 0 );
		long textID = (long) tolua_tonumber( L, 2, 0 );
		uchar pointContinu = (uchar) tolua_tonumber( L, 3, 0 );
		uchar endTalkBox = (uchar) tolua_tonumber( L, 4, 0 );
		long sndID = (long) tolua_tonumber( L, 5, 0 );
		long musicID = (long) tolua_tonumber( L, 6, 0 );
		long headID = (long) tolua_tonumber( L, 7, 0 );
		const char *npcName = TOLUA_TOSTRING( npcName, L, 8, 0 );
		long faceID1 = (long) tolua_tonumber( L, 9, 0 );
		long faceTimes1 = (long) tolua_tonumber( L, 10, 0 );
		long faceID2 = (long) tolua_tonumber( L, 11, 0 );
		long faceTimes2 = (long) tolua_tonumber( L, 12, 0 );
		long faceID3 = (long) tolua_tonumber( L, 13, 0 );
		long faceTimes3 = (long) tolua_tonumber( L, 14, 0 );
		long faceID4 = (long) tolua_tonumber( L, 15, 0 );
		long faceTimes4 = (long) tolua_tonumber( L, 16, 0 );
		long faceID5 = (long) tolua_tonumber( L, 17, 0 );
		long faceTimes5 = (long) tolua_tonumber( L, 18, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENSTORYTALKBOX );
			msg.Add( (long)1 );
			msg.Add( script->ID() );
			msg.Add( textType );	
			msg.Add( textID );
			msg.Add( pointContinu );
			msg.Add( endTalkBox );
			msg.Add( sndID );
			msg.Add( musicID );
			msg.Add( headID );
			msg.Add( npcName );
			msg.Add( faceID1 );
			msg.Add( faceTimes1 );
			msg.Add( faceID2 );
			msg.Add( faceTimes2 );
			msg.Add( faceID3 );
			msg.Add( faceTimes3 );
			msg.Add( faceID4 );
			msg.Add( faceTimes4 );
			msg.Add( faceID5 );
			msg.Add( faceTimes5 );

			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出小精灵
	///
	/// talkRet, chooseRet = PixyTalkBox( str, sndID, headID, 
	/// faceID1, faceTime1, faceID2, faceTime2, faceID3, faceTime3,
	/// faceID4, faceTime4, faceID5, faceTime5 )
	///
	static int PixyTalkBox( lua_State *L )
	{
		const char *str = TOLUA_TOSTRING( str, L, 1, "" );
		long sndID = (long) tolua_tonumber( L, 2, 0 );
		long headID = (long) tolua_tonumber( L, 3, 0 );
		long faceID1 = (long) tolua_tonumber( L, 4, 0 );
		long faceTimes1 = (long) tolua_tonumber( L, 5, 0 );
		long faceID2 = (long) tolua_tonumber( L, 6, 0 );
		long faceTimes2 = (long) tolua_tonumber( L, 7, 0 );
		long faceID3 = (long) tolua_tonumber( L, 8, 0 );
		long faceTimes3 = (long) tolua_tonumber( L, 9, 0 );
		long faceID4 = (long) tolua_tonumber( L, 10, 0 );
		long faceTimes4 = (long) tolua_tonumber( L, 11, 0 );
		long faceID5 = (long) tolua_tonumber( L, 12, 0 );
		long faceTimes5 = (long) tolua_tonumber( L, 13, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENPIXYTALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( str );	
			msg.Add( sndID );
			msg.Add( headID );
			msg.Add( faceID1 );
			msg.Add( faceTimes1 );
			msg.Add( faceID2 );
			msg.Add( faceTimes2 );
			msg.Add( faceID3 );
			msg.Add( faceTimes3 );
			msg.Add( faceID4 );
			msg.Add( faceTimes4 );
			msg.Add( faceID5 );
			msg.Add( faceTimes5 );

			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 弹出小精灵
	///
	/// talkRet, chooseRet = PixyTalkBoxByIndex( textType, textID, sndID, headID, 
	/// faceID1, faceTime1, faceID2, faceTime2, faceID3, faceTime3,
	/// faceID4, faceTime4, faceID5, faceTime5 )
	///
	/// @see PixyTalkBox
	///
	static int PixyTalkBoxByIndex( lua_State *L )
	{
		long textType = (long) tolua_tonumber( L, 1, 0 );
		long textID = (long) tolua_tonumber( L, 2, 0 );
		long sndID = (long) tolua_tonumber( L, 3, 0 );
		long headID = (long) tolua_tonumber( L, 4, 0 );
		long faceID1 = (long) tolua_tonumber( L, 5, 0 );
		long faceTimes1 = (long) tolua_tonumber( L, 6, 0 );
		long faceID2 = (long) tolua_tonumber( L, 7, 0 );
		long faceTimes2 = (long) tolua_tonumber( L, 8, 0 );
		long faceID3 = (long) tolua_tonumber( L, 9, 0 );
		long faceTimes3 = (long) tolua_tonumber( L, 10, 0 );
		long faceID4 = (long) tolua_tonumber( L, 11, 0 );
		long faceTimes4 = (long) tolua_tonumber( L, 12, 0 );
		long faceID5 = (long) tolua_tonumber( L, 13, 0 );
		long faceTimes5 = (long) tolua_tonumber( L, 14, 0 );

		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENPIXYTALKBOX );
			msg.Add( (long)1 );
			msg.Add( script->ID() );
			msg.Add( textType );	
			msg.Add( textID );
			msg.Add( sndID );
			msg.Add( headID );
			msg.Add( faceID1 );
			msg.Add( faceTimes1 );
			msg.Add( faceID2 );
			msg.Add( faceTimes2 );
			msg.Add( faceID3 );
			msg.Add( faceTimes3 );
			msg.Add( faceID4 );
			msg.Add( faceTimes4 );
			msg.Add( faceID5 );
			msg.Add( faceTimes5 );

			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return lua_yield( L, 0 );
	}

	///
	/// 关闭对话框，在TalkBox系列函数调用后都需要调用此函数
	///
	/// EndTalkBox()
	///
	/// @see TalkBox
	///
	static int EndTalkBox( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENTALKBOX );// --让客户端弹出TalkBox
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( "" );	
			msg.Add( (uchar)0 );
			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return 0;
	}

	///
	/// 关闭对话框
	///
	/// EndPixyTalkBox()
	///
	static int EndPixyTalkBox( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			CMessage msg( MSG_S2C_OTHER_OPENPIXYTALKBOX );
			msg.Add( (long)0 );
			msg.Add( script->ID() );
			msg.Add( "" );	
			msg.Add( (uchar)0 );
			msg.SendToPlayer( env.srcShape->GetExID() );
		}
		return 0;
	}

	///
	/// 弹出提示框
	///
	/// ret, chooseRet = GameMessage( msg, [type] )
	/// @param msg 提示框文字
	/// @param type [可选]提示框类型：MB_YESNO或者MB_OK
	/// @return ret 玩家对对话框的选择，对于MB_YESNO对话框，1表示选择了YES，0表示选择了NO，MB_OK始终返回0
    /// @return chooseRet 始终为0，不使用
	///
	static int GameMessage( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		if( env.srcShape != NULL &&
			env.srcShape->GetType() == TYPE_PLAYER )
		{
			const char *str = TOLUA_TOSTRING( str, L, 1, "" );
			long type = (long) tolua_tonumber( L, 2, 1 );
			CMessage msg( MSG_S2C_OTHER_GAMEMESSAGE );
			msg.Add( str );
			msg.Add( type );
			msg.Add( script->ID() );
			msg.SendToPlayer( env.srcShape->GetExID() );
			return lua_yield( L, 0 );
		}
		tolua_pushnumber( L, 0 );
		return 1;
	}

	/// 
	/// 打开复活窗口
	///
	/// ret, chooseRet = OpenReliveWindow( type )
	/// @param type 为0时只有"回城复活"，为1时加上"原地复活"
	/// @return
	///
	static int OpenReliveWindow( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		CPlayer *player = dynamic_cast<CPlayer*>( env.srcShape );
		if( player == NULL )
		{
			return 0;
		}
		uchar type = (uchar) tolua_tonumber( L, 1, 0 );
		player->SetReliveScript( script->ID() );
		CMessage msg( MSG_S2C_OTHER_OPEN_RELIVEWINDOW );
		msg.Add( script->ID() );
		msg.Add( type );
		msg.SendToPlayer( player->GetExID() );

		return lua_yield( L, 0 );
	}

	///
	/// 打开原地复活结果窗口
	///
	/// OpenReliveByGoodsWindow( ret, index, costNum )
	/// @param ret 原地复活结果标志符1为符合条件，0是不符合条件
	/// @param index 物品索引
	/// @param costNum 物品消耗数量
	///
	static int OpenReliveByGoodsWindow( lua_State *L )
	{
		Script *script = GetInst( ScriptSys ).GetScript( L );
		const Script::RunEnv &env = script->GetEnv();
		CPlayer *player = dynamic_cast<CPlayer*>( env.srcShape );
		if( player == NULL )
		{
			return 0;
		}
		long ret = (long) tolua_tonumber( L, 1, 0 );
		long index = (long) tolua_tonumber( L, 2, 0 );
		long costNum = (long) tolua_tonumber( L, 3, 0 );

		CMessage msg( MSG_S2C_PLAYER_BUYRELIVEITEM_RESULT );
		msg.Add( script->ID() );
		msg.Add( ret );
		msg.Add( index );
		msg.Add( costNum );
		msg.SendToPlayer( player->GetExID() );

		return lua_yield( L, 0 );
	}
	

	void OpenClientLibs( lua_State *L )
	{
		lua_register( L, "TalkBox", TalkBox );
		lua_register( L, "EndTalkBox", EndTalkBox );
		lua_register( L, "GameMessage", GameMessage );
		lua_register( L, "PicTalkBox", PicTalkBox );
		lua_register( L, "TalkBoxByIndex", TalkBoxByIndex );
		lua_register( L, "StoryTalkBox", StoryTalkBox );
		lua_register( L, "StoryTalkBoxByIndex", StoryTalkBoxByIndex );
		lua_register( L, "PixyTalkBox", PixyTalkBox );
		lua_register( L, "PixyTalkBoxByIndex", PixyTalkBoxByIndex );
		lua_register( L, "EndPixyTalkBox", EndPixyTalkBox );
		lua_register( L, "OpenReliveWindow", OpenReliveWindow );
		lua_register( L, "OpenReliveByGoodsWindow", OpenReliveByGoodsWindow );

		lua_pushnumber( L, 1 );
		lua_setglobal( L, "MB_YESNO" );
		lua_pushnumber( L, 4 );
		lua_setglobal( L, "MB_OK" );
	}
}
