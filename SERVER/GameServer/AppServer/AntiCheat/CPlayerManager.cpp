#include "StdAfx.h"
#include "CPlayerManager.h"
#include "PlayerProperty.h"
#include "CConfiger.h"
#include "CGUID.h"
#include "../Player.h"

namespace AntiCheat
{
	
#define CHECK_ENABLE if( !GetInst( CConfiger ).IsEnable() ) return

	void CPlayerManager::Release()
	{
		for( PlayersTable::iterator it = m_playerTable.begin();
			it != m_playerTable.end(); ++ it )
		{
			it->second->CancelTimers();
			MP_DELETE( it->second );
		}
		m_playerTable.clear();
	}

	/// 玩家进入游戏
	bool CPlayerManager::PlayerEnter(CPlayer *player )
	{		
		if( !GetInst( CConfiger ).IsEnable() )
		{
			return true;
		}

		const CGUID &playerid = player->GetExID();
		CPlayerProperty * playerpro = FindPlayerProPerty(playerid);
		
		if (playerpro !=NULL)
		{
			/// 玩家进入游戏，启动定时器
			return playerpro->OnEnter();
		}
		return false;
	}

	/// 开启玩家反外挂定时器
	void CPlayerManager::StartCheck( const CGUID& PlayerId)
	{
		CHECK_ENABLE;
		CPlayerProperty * playerpro = FindPlayerProPerty(PlayerId);
	
		if ( playerpro != NULL)
		{
			 playerpro->StartCheck();
		}
		return ;
	}

	/// 玩家退出,清除列表
	void CPlayerManager::PlayerExit( const CGUID &playerID )
	{	
		CHECK_ENABLE;

		CPlayerProperty * playerpro = FindPlayerProPerty(playerID);
		if (playerpro != NULL)
		{
			//playerpro->OnExit( bChangeServer );
			/// 注销玩家所有的定时器
			playerpro->CancelTimers();
			/// 释放对象空间
			MP_DELETE(playerpro);
			/// 删除map资源
			m_playerTable.erase(playerID);
		}
		
	}

	/// 调用其EncodeToDataBlock编码数据。
	void CPlayerManager::PlayerEncode( CPlayer *player , DBWriteSet &db )
	{
		CHECK_ENABLE;

		CPlayerProperty * playerpro = FindPlayerProPerty(player->GetExID());
		if (playerpro != NULL)
		{
			db.AddToByteArray((BYTE)1);
			playerpro->EncodeToDataBlock(db);
		}
		else
		{
			db.AddToByteArray((BYTE)0);
		}
	}

	void CPlayerManager::PlayerDecode( CPlayer *player, DBReadSet &db )
	{
		CHECK_ENABLE;
		
		const CGUID &playerid = player->GetExID();
		CPlayerProperty *playerpro = MP_NEW CPlayerProperty(player->GetExID());
		playerpro->DecodeFromDataBlock(db);
	
		//! 原有数据可以覆盖吗？--------------
		PlayersTable::iterator itePlayer = m_playerTable.find(playerid);
		if (m_playerTable.end() != itePlayer)
		{
			MP_DELETE(itePlayer->second);
			m_playerTable.erase(itePlayer);
		}
		//! --------------
		m_playerTable[playerid] = playerpro;

	}
	/// 回应客户端的答题消息
	void CPlayerManager::PlayerAnswer(CPlayer *player, int sel )
	{
		const CGUID &playerid = player->GetExID();
		CPlayerProperty * playerpro = FindPlayerProPerty(playerid);
		if (playerpro != NULL)
		{
			/// 用户选择的答案
			playerpro->OnAntiCheatAnswer(sel);
		}
	}

	/// Pk触发事件
	void CPlayerManager::PlayerFight(CPlayer *player)
	{
		const CGUID &playerid = player->GetExID();
		CPlayerProperty * playerpro = FindPlayerProPerty(playerid);
		if (playerpro != NULL)
		{
			/// 改变玩家的Pk值
			playerpro->OnFight();
		}
	}

	void CPlayerManager::PlayerAttack( CPlayer *player )
	{
		const CGUID &playerid = player->GetExID();
		CPlayerProperty *playerpro = FindPlayerProPerty( playerid );
		if( playerpro != NULL )
		{
			playerpro->OnAttack();
		}
	}

	void CPlayerManager::PlayerTalk( CPlayer *talker, const std::string &content, 
			const std::string &listener, long talk_type )
	{
		CPlayerProperty *playerpro = FindPlayerProPerty( talker->GetExID() );
		if( playerpro != NULL )
		{
			playerpro->OnTalk( content, listener, talk_type );
		}
	}

	/// 活跃度脚本接口函数
	int CPlayerManager::PlayerChangeActivity(const CGUID &playerID, int changeValue, bool bSend )
	{
		CPlayerProperty * playerpro = FindPlayerProPerty(playerID);
		if (playerpro != NULL)
		{
			return playerpro->ChangeActivity(changeValue, bSend);
		}
		return 0;
	}

	/// 查找元素
	CPlayerProperty *CPlayerManager::FindPlayerProPerty(const CGUID & guid )
	{
		map<CGUID, CPlayerProperty*>::iterator it = m_playerTable.find(guid);
		if (it != m_playerTable.end())
		{
			return it->second;
		}
		return NULL;
	}

	/// 踢玩家下线
	void CPlayerManager::KickOnlinePlayer(const CGUID &PlayerId  )
	{
		CPlayerProperty * playerpro = FindPlayerProPerty(PlayerId);
		
		if (playerpro != NULL)
		{
			playerpro->KickPlayer();
		}
	}

	void CPlayerManager::StopCheck(const CGUID & PlayerId)
	{
		CPlayerProperty * playerpro = FindPlayerProPerty(PlayerId);
		if (playerpro != NULL)
		{
			playerpro->StopCheck();
		}
	}

	void CPlayerManager::CheckRefuseAnswer( const CGUID &playerID, bool bChangeServer )
	{
		CPlayerProperty * playerpro = FindPlayerProPerty(playerID);
		if (playerpro != NULL)
		{
			playerpro->CheckRefuseAnswer( bChangeServer );
		}
	}

	long CPlayerManager::ScriptQueryProperty( const CGUID &playerID, const std::string &var_name )
	{
#ifdef SCRIPT_QUERY
		CPlayerProperty * playerpro = FindPlayerProPerty(playerID);
		if (playerpro != NULL)
		{
			return playerpro->GetAttr( var_name );
		}
#endif
		return 0;
	}
}