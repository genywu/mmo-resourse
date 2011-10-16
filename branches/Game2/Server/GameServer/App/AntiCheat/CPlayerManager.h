#pragma once
 
class CPlayer;

namespace AntiCheat
{
	class CPlayerProperty;
	class CPlayerManager
	{
	public:
		typedef std::map<CGUID, CPlayerProperty*> PlayersTable;

	public:

		void Release();

		///  玩家进入游戏，启动玩家的定时器
		bool PlayerEnter( CPlayer *player );

		///  玩家退出，只需要清除玩家列表里的数据即可，数据存档需要
		///  使用另一个接口。
		void PlayerExit( const CGUID &playerID );

		/// 编码某个玩家的数据，该函数从列表中查找到PlayerProperty，并
		/// 调用其EncodeToDataBlock编码数据。调用该函数表示该玩家可能切服，
		/// 或者退出游戏准备保存。(CPlayer::CodeToDataBlock)
		void PlayerEncode( CPlayer *player , DBWriteSet &db );

		///  玩家进入新的GS（第一次进或者切换GS）时调用此函数
		///  该函数负责解码玩家的存档数据(CPlayerProperty::DecodeFromDataBlock)
		void PlayerDecode( CPlayer *player, DBReadSet &db );

		///  客户端回应某个答题，查找列表并调用其PlayerProperty::OnAntiCheatAnswer
		void PlayerAnswer( CPlayer *player, int sel );

		///  玩家进行Pk
		void PlayerFight(CPlayer *player);

		/// 玩家主动攻击
		void PlayerAttack( CPlayer *player );

		/// 玩家发言，可能会增长活跃度
		void PlayerTalk( CPlayer *talker, const std::string &content, 
			const std::string &listener, long talk_type );

		///  改变玩家活跃度
		int  PlayerChangeActivity( const CGUID &playerID, int changeValue, bool bSend = true );

		///  反外挂踢玩家下线脚本
		void KickOnlinePlayer( const CGUID &PlayerId );

		///  停止玩家反外挂模块
		void StopCheck(const CGUID & PlayerId);

		/// 提供给脚本查询属性的能力
		long ScriptQueryProperty( const CGUID &playerID, const std::string &var_name );

		/// 检查玩家是否有拒绝回答题目的行为（切服拒绝）
		void CheckRefuseAnswer( const CGUID &playerID, bool bChangeServer );

		/// 开启玩家反外挂定时器
		void StartCheck(const CGUID & PlayerId);
	private:
		///  返回查找结果
		inline CPlayerProperty *FindPlayerProPerty(const CGUID & guid );

	private:
		PlayersTable m_playerTable;
	};
}

