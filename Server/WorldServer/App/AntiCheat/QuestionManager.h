///
/// @file QuestionManager.h
/// @author Kevin Lynx
/// @brief manage the questions generation and cache details.
///
#ifndef ___QUESTION_MANAGER_H_
#define ___QUESTION_MANAGER_H_

namespace AntiCheat
{
	class CQuestionManager : public CGameEvent
	{
	private:
		struct BufferInfo
		{
			void *buf;
			size_t size;
		};

		struct Question
		{
			BufferInfo data;
			int answer;
		};
		typedef std::list<Question> QuestionList;

		/// event types
		enum
		{
			ET_QUIT = 0,
			ET_GEN,
			ET_COUNT
		};

		typedef CTimerQueue<CGameEvent*> TimerType;

	public:
		CQuestionManager() : m_Timer( 1024, 1000 )
		{
			m_pQuestions = NULL;
			m_UpdateTimer = -1;
		}

		/// 开启功能，会生成第一批题目，开启线程，注册定时器等。
		bool Startup();

		/// 关闭操作，清除题目缓存，关闭线程，注销定时器。
		void Shutdown();

		/// 每帧调用
		void Run();

		/// 发送题目数据给指定GS，GS连接上WS时调用此函数。
		void SendQuestionsToGS( long lGSSocketID );

	private:
		long OnTimeOut( DWORD timerid, DWORD curtime, const void *var ) ;
		void OnTimerCancel( DWORD timerid, const void *var ) {}
		void OnMessage( CMessage* ) {}
		long Proc( eEventType type, const tagProcVar *pVar ) { return 0; }
	private:

		/// 线程回调，准备生成题目并更新题目缓存
		void OnThreadCall();

		/// 生成一个题目，返回的内存需要自己释放 : free
		Question GenOneQuestion();

		/// 根据配置生成一批题目，返回的容器对象需要自己释放 : delete
		QuestionList *GenQuestions();

		/// 更新题目缓存，此函数会进行资源互斥
		void UpdateQuestionCache( QuestionList *pQuestions );

		/// 编码题目缓存，此函数会进行资源互斥
		void EncodeQuestionCache( DBWriteSet &db );

		/// 更新题目缓存到所有GS
		void UpdateQuestionsToAll();

		/// 清除题目缓存
		void ReleaseQuestionCache( QuestionList *pQuestions );

		/// 更新题目线程
		static  unsigned int __stdcall threadFunc( void *p );
	private:
		QuestionList *m_pQuestions;
		long m_UpdateTimer;
		CRITICAL_SECTION m_QuesMutex;
		HANDLE m_Events[ET_COUNT];
		HANDLE m_Thread;
		TimerType m_Timer;
	};
}

#endif