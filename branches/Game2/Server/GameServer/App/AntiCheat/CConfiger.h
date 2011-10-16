#pragma once

namespace AntiCheat
{
	typedef std::pair<int, int> Range;
	class CPlayerProperty;

	class CConfiger
	{
	public:
		enum TalkCheck
		{
			eTC_NONE = 0x0000,
			eTC_NORMAL = 0x0001,
			eTC_REGION = 0x0002,
			eTC_PRIVATE = 0x0004,
			eTC_TEAM = 0x0008,
			eTC_FACTION = 0x0010,
		};
		struct ScriptCfg
		{
			std::string file;
			int limit;
		};
	public:
		struct System
		{
			System()
			{
				memset(this,0,sizeof(System));
			}
			Range CheckIntervalRange;
			Range QuitActivityRange;
			int RefuseMax;
			int BonusMax;
			int AnswerInterval;
			int PkTimeInterval;
			int ActivityMax;
			int PkAddValue;
			int PkDropValue;
			int PkDropInterval;
			int AtkActivityAddValue;
			int AtkActivityDropValue;
			int AtkActivityDropInterval;
			Range FightActivityRange;
			Range TalkActivityRange;
			Range TalkSensActivityRange;
			Range RushHourRange;
			int AnswerTimeOutInterval;
			int DropCurWrongValue;
			int CheckSensWords;
			int TalkCheckFlag;

			int UpdateQuestionInterval;
			int GenQuestionCount;
			Range AnswerAlpha;
			int BanHours;
			int LevelFilter;
			int Enable;
		}; 

		typedef std::map<int, ScriptCfg> ScriptsTable;
		typedef ScriptsTable::iterator     SCon_LIST;

	public:

		/// 从消息解码配置  
		void DecodeFromDataBlock( DBReadSet &db );

		/// 回答正确脚本触发  
		void OnAnswerRight( const CPlayer *player, int rightTimes, bool bLimit );

		/// 回答错误脚本触发  
		void OnAnswerWrong( const CPlayer *player, int wrongTimes );

		/// 获取全局配置(System域)  
		const System &GetSystem() const   {    return m_sys;   }

		/// 是否开启反外挂功能
		bool IsEnable() const { return m_sys.Enable != 0; }

		/// 检查指定字符串是否包含敏感词
		bool HasSensWord( const std::string &content );

		/// 是否检测指定频道发言 
		bool IsCheckTalk( TalkCheck flag )
		{
			return ( m_sys.TalkCheckFlag & flag ) != 0;
		}

	private:
		/// 解码敏感词列表
		void DecodeSensWords( DBReadSet &db );
	private:
		System m_sys;
		ScriptsTable m_RightScripts;
		ScriptsTable m_WrongScripts;
	private:
		typedef std::list<std::string> SensWordsList;
		SensWordsList m_SensWords;
	};

}