#pragma once

class TiXmlElement ;

namespace AntiCheat
{
	typedef std::pair<int, int> Range;
	class PlayerProperty;

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
			Range AnswerAlphaRange; /* [0-255] */
			int BanHours;
			int LevelFilter;
			int Enable;
		}; 

		typedef std::map<int, ScriptCfg> ScriptsTable;
		typedef ScriptsTable::iterator     SCon_LIST;

	public:
		/// 装载配置文件 
		bool Load();

		/// 编码配置信息到消息  
		void EncodeToDataBlock( DBWriteSet &db );

		/// 获取全局配置(System域)  
		const System &GetSystem() const   {    return m_sys;   }

		/// 是否开启反外挂功能
		bool IsEnable() const { return m_sys.Enable != 0; }

	private:
		/// 编码敏感词列表
		void EncodeSensWords( DBWriteSet &db );

		/// 装载敏感词列表
		bool LoadSensWords( const char *file );

		/// 装载脚本配置块
		bool LoadScriptTable( TiXmlElement *pElem, ScriptsTable &st );
	private:
		System m_sys;
		ScriptsTable m_RightScripts;
		ScriptsTable m_WrongScripts;

	private:
		typedef std::list<std::string> SensWordsList;
		SensWordsList m_SensWords;
	};

}