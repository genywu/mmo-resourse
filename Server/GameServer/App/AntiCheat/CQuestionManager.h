///
/// @file QuestionManager.h
/// @author Kevin Lynx
/// @brief manage the questions cache 
///
#ifndef ___QUESTION_MANAGER_H_
#define ___QUESTION_MANAGER_H_


namespace AntiCheat
{
	class CQuestionManager
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
		typedef std::vector<Question> QuestionList;

	public:
		CQuestionManager()
		{
		}

		~CQuestionManager()
		{
			ClearQuestions();
		}

		/// 更新题目缓存
		void UpdateQuestions( DBReadSet &db );

		/// 清除题目缓存
		void ClearQuestions();

		/// 随机获取一个题目，不能删除返回的内存
		bool GetRandQuestion( void **pBuf, size_t *size, int *answer );

	private:
		QuestionList m_Questions;
	};
}

#endif