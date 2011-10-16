#pragma once
#include "../../../../Public/Common/BaseDef.h"
// 聊天
class CChat
{
public:
	static string m_strCommand[TYPE_NUM];	// 命令
	static long m_lInterleave[TYPE_NUM];	// 发言间隔
	static long m_lLastSendTime[TYPE_NUM];	// 上次发言时间

	static list<string> m_dirtyWords;			// 将被过滤的脏话关键字

public:
	static void Init();
	static long GetWord(char* strSource, char* strDest, long num, long lStartPos=0);	// 获取第几个单词

	static eChatType GetChatType( LPCTSTR szInput, long& lPos );		// 获取聊天的模式

public:
	CChat(void);
	virtual ~CChat(void);
};
