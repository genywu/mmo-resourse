///
/// @file QuestionManager.cpp
/// @author Kevin Lynx
/// @brief manage the questions cache 
///
#include "stdafx.h"
#include "CQuestionManager.h"

namespace AntiCheat
{
	void CQuestionManager::UpdateQuestions( DBReadSet &db )
	{
		ClearQuestions();
		long lCount = db.GetLongFromByteArray();
		m_Questions.reserve( lCount );
		for( long i = 0; i < lCount; ++ i )
		{
			Question ques;
			ques.answer = db.GetByteFromByteArray();
			ques.data.size = (size_t) db.GetLongFromByteArray();
			ques.data.buf = M_ALLOC( sizeof(char)*(ques.data.size) );
			db.GetBufferFromByteArray( ques.data.buf, ques.data.size );

			m_Questions.push_back( ques );
		}
	}

	void CQuestionManager::ClearQuestions()
	{
		for( QuestionList::iterator it = m_Questions.begin(); it != m_Questions.end(); ++ it )
		{
			Question &ques = *it;
			M_FREE( ques.data.buf, sizeof(char)*(ques.data.size) );
		}
		m_Questions.clear();
	}

	bool CQuestionManager::GetRandQuestion( void **pBuf, size_t *size, int *answer )
	{
		if( m_Questions.size() == 0 )
		{
			return false;
		}
		Question &ques = m_Questions[ random( m_Questions.size() ) ];
		*pBuf = ques.data.buf;
		*size = ques.data.size;
		*answer = ques.answer;
		return true;
	}
}