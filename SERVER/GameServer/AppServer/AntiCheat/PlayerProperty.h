#pragma once 

#include "..\..\..\..\public\GameEvent.h"
#include "..\..\..\..\public\MsgType.h"
#include "..\..\GameServer\GameManager.h"
#include "..\public\Singleton.h"

class CPlayer;
class CRegion;

namespace AntiCheat
{

	// 允许脚本查询相关属性
#define SCRIPT_QUERY
#ifdef SCRIPT_QUERY
	class CScriptQuery
	{
	private:
		typedef void* VarPtr;
		typedef std::map<std::string, VarPtr> VarTable;
	public:
		template <typename _Tp>
		void AddVar( const std::string &name, _Tp &var )
		{
			m_Vars[name] = &var;
		}

		VarPtr GetVar( const std::string &name ) const
		{
			VarTable::const_iterator it = m_Vars.find( name );
			if( it == m_Vars.end() )
			{
				return VarPtr();
			}
			return it->second;
		}

		template <typename _Tp>
		_Tp GetVarValue( const std::string &name ) const
		{
			VarPtr v = GetVar( name );
			return v == VarPtr() ? _Tp() : *( (_Tp*)v );
		}

	private:
		VarTable m_Vars;
	};
#endif

	class CQusetionManager;
	class CPlayerProperty : public BaseMemObj, public CGameEvent
	{
	private:
		/// 玩家的存档数据
		struct SavedProperty 
		{
			SavedProperty() 
			{
				memset(this,0,sizeof(SavedProperty));
			};
			int nActFactor;							/// 活跃度
			int nAccWrongTimes;						/// 累计答错次数
			int nCurWrongTimes;						/// 当前答错次数
			int nCurRightTimes;						/// 连续答对次数
			int nLastGameTime;						/// 上次游戏时间
			unsigned char bBanFlag;					/// 封号标志，为1时表示被封号
			unsigned char m_bScriptExecuted;		/// 为1时表示之前的错误脚本未触发
			int nBonusTimes;						/// 当前奖励次数，第二天清零
			unsigned char m_bRHAnswered;			/// 是否在黄金时间答题正确过
		};

		/// 需要编码到WS的数据
		struct CachedProperty
		{
			SavedProperty saved;
			long nAntiCheatPK;
			unsigned long LastAnswerTime;	
			long nRefuseTimes;
			long nAtkAct;
			CachedProperty()
			{
				memset( this, 0, sizeof( *this ) );
			}
		};

		/// 定时器类型
		enum Anti_Type
		{
			ANTICHEAT=1,							/// 答题定时器
			PKTIMER,								/// PK定时器
			ANSTIMER,								/// 回答问题超时定时器
			ATKACT_TIMER
		};

		/// 答题反馈类型
		enum AnswerRet
		{
			AR_RIGHT = 1,
			AR_WRONG = 2,
			AR_TIMEOUT = 3,
		};

		/// 定时器状态
		enum TimerStatus
		{
			TS_VALID = 0,
			TS_INVALID = -1,
			TS_STOP = -2
		};
	public:
		CPlayerProperty( const CGUID &OwnerID ) : m_OwnerID( OwnerID )
		{
			m_lAnswerTimer= m_lCheckTimer = 
				m_lPKTimer = m_lAtkActTimer = TS_INVALID;
			m_CheckFinish= false;
			m_pOwner = NULL;
#ifdef SCRIPT_QUERY
			InitVarTable();
#endif
		}
 
	public:

		///  编码配置信息到消息  
		void EncodeToDataBlock( DBWriteSet &db );

		///  从消息解码配置  
		void DecodeFromDataBlock( DBReadSet &db );
		
		///  消息
		///  注销所有定时器资源
		void CancelTimers( bool bStop = false );

		///  玩家刚进入GS时，需要做一些逻辑判断，例如疑似外挂值过高做
		///  掉线处理等，这里主要是做脚本触发。
		bool OnEnter();

		/// 玩家退出服务器（退出游戏或切服）
		/// 在未答题状态下退出游戏算错。
		void OnExit( bool bChangeServer );

		///  客户端回应消息处理函数，当客户端发回答题消息后，调用此函数
		void OnAntiCheatAnswer( int sel );

		///  玩家PK触发
		void OnFight();

		/// 玩家主动攻击触发
		void OnAttack();

		/// 玩家发言触发
		void OnTalk( const std::string &content, const std::string &listener, long talk_type );

		///  改变活跃度值，返回改变后的活跃度
		int  ChangeActivity( int changeValue, bool bSend = true );

		///  踢在线玩家下线，
		void KickPlayer();  

		/// 检查玩家是否有拒绝回答题目的行为（切服拒绝），有则增加拒绝回答次数
		/// 当拒绝回答次数增加到一定值会直接归为错误。
		/// 如果是退出游戏拒绝，则直接计算错误一次
		void CheckRefuseAnswer( bool bChangeServer );

		/// 开启玩家的反外挂检测
		bool StartCheck();
		/// 关闭玩家所有反外挂定时器
		bool StopCheck();
	private:
		///  注册答题定时器
		bool ReginsterAntiCheatTimer( bool bFirst = false );			
		///  注册答题超时定时器，当发送题目给客户端后，注册该定时器
		bool RegisterAnswerTimeOutTimer();
		///  注册Pk定时器
		bool RegisterPkTimer();					
		/// 注册战斗活跃度定时器
		bool RegisterAtkActTimer();

		///  超时，注销，回调函数
		long OnTimeOut( DWORD timerid, DWORD curtime, const void *var );
		///  定时器注销时调用的函数，可用于删除一些附加参数
		void OnTimerCancel( DWORD timerid, const void *var );
		///  GameEvent中部分纯虚函数需要在此重写，只需要写个空函数即可
		void OnMessage( CMessage* ) {}
		long Proc( eEventType type, const tagProcVar *pVar ) { return 0; }
		///  注销定时器ID
		void CancelTimerId(long &TimerId, TimerStatus s );
		
		///  答题判断定时器触发回调函数，该函数主要做是否发送题目的逻辑检测
		bool OnAntiCheatTimeOut();
		///  答题超时定时器触发函数
		void OnAnswerTimeOut();
		///  活跃度判断函数，在答题判断逻辑中（即是否向客户端发送题目的判断）调用此函数
		bool OnChangeActivity();
		///  PK值改变判断函数
		void OnPKDropTimeOut();
		/// 战斗活跃度衰减函数，同PK值一样一直衰减
		void OnAtkActDropTimeOut();

		///  答案相关
		///  发送题目到客户端
		void SendQuestionToClient();
		///  发送用户答题信息
		void SendResultToClient();

		///  是否封号
		bool IsKickPlayer();

		/// 答题错误的处理
		void OnAnswerWrong();

		/// 检查当前奖励次数，超过一定值后得不到奖励；
		/// 如果当前时间已经是第二天，自动清除该值
		bool CheckBonusTimes();

		/// 检查是否可以向玩家发送题目
		bool CheckSendQuestion();

		/// 检查是否可以改变活跃度
		bool CheckChangeActivity();

		/// 根据玩家ID获取玩家，需要对返回值做NULL判断
		CPlayer *GetPlayer() const;

		/// 根据玩家ID获取玩家所在场景，需要对返回值做NULL判断
		CRegion *GetPlayerRegion() const;

		/// 返回客户端答题结果
		void SendAnswerRetToClient( AnswerRet ret );

		/// 添加答题情况日志
		void AppendAnswerLog( const char *log_type );
	private:
		///  答题判断定时器ID
		long m_lCheckTimer;
		///  答题超时定时器ID
		long m_lAnswerTimer;
		///  PK定时器ID
		long m_lPKTimer;
		/// 战斗活跃度定时器
		long m_lAtkActTimer;
		///  该PlayerProperty对应的玩家
		CGUID m_OwnerID;	
		/// 玩家指针，在每次定时器触发时/客户端发来消息/脚本函数调用时重新查找并更新此值
		CPlayer *m_pOwner;

		/// 玩家缓存属性，包括存档属性
		CachedProperty m_cachedProper;

		///  为答题增加辅助变量,主要是防止重复答题，超时后也不能答题
		bool  m_CheckFinish;
		///  题目答案
		int   m_Answer;

		// 提供给脚本查询相关属性的能力
#ifdef SCRIPT_QUERY
	public:
		long GetAttr( const std::string &name )
		{
			return m_scriptQueryer.GetVarValue<long>( name );
		}
	private:
		void InitVarTable();
	private:
		CScriptQuery m_scriptQueryer;
#endif
	};
}
