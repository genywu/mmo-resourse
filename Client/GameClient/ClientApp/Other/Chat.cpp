#include "stdafx.h"
#include "chat.h"
#include <strstream>
#include "../../../Public/Common/LinkmanDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 命令
string CChat::m_strCommand[TYPE_NUM] =
							{ "/s",		// 普通
							 "/y",		// 场景
							  "/a",		// 联盟
							  "/g",		// 行会
							  "/p",		// 队伍
							  "/w",		// 私聊
							  "/c",		// 国家
							  "/u",		// 盟国
							  "/世界",	// 世界

							  "/sys/",	// 系统

							 "!!!",		// 广播
							 "@",		//GM命令
							 "//",		//脚本-调试用
							 "/?",		// 聊天帮助
							 "~",		//玩家命令
							 "/npc/",	//npc命令
							};


// 发言间隔
long CChat::m_lInterleave[TYPE_NUM] = { 2000,		// 普通
										10000,		// 场景
										1000,		// 联盟
										100,		// 行会
										1000,		// 队伍
										0,			// 私聊
										10000,		// 国家
										10000,		// 盟国
										0,			// 世界
										0,			// 系统
										10000,		// 广播
										0,			//GM命令
										0,			//脚本-调试用
										0			// 聊天帮助
										};

// 上次发言时间
long CChat::m_lLastSendTime[TYPE_NUM] = {0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0};

//list<string*>CChat:: m_dirtyWords;

CChat::CChat(void)
{

}

CChat::~CChat(void)
{
}

void CChat::Init()
{
}


// 获取聊天的类型
eChatType CChat::GetChatType( LPCTSTR szInput, long& lPos )
{
	if( szInput )
	{
		TCHAR szInputCmd[256];

		for( long c = 0; c < TYPE_NUM; c++ )
		{
			// 匹配开头的命令字符串
			memcpy( szInputCmd, szInput, m_strCommand[ c ].size() );
			szInputCmd[ m_strCommand[ c ].size() ] = 0;
			CharLower( szInputCmd );

			if( ! lstrcmp( szInputCmd, m_strCommand[ c ].c_str() ) )
			{
				// 返回 命令字符串之后的位置 和 类型
				lPos = (long)m_strCommand[c].size();

				// 去除空格
				while( szInput[lPos] )
				{
					if( ' ' != szInput[lPos] )
						break;
					lPos++;
				}

				// 如果是密聊要去名字
				if( CHAT_PRIVATE == c )
				{
					TCHAR szName[ MAX_PLAYER_NAME_SIZE + 1 ];
					ZeroMemory( szName, sizeof(szName) );

					// 获取名字
					for( int i = 0; szInput[lPos] && i < MAX_PLAYER_NAME_SIZE; i++, lPos++ )
					{
						if( ' ' == szInput[lPos] )
							break;

						szName[i] = szInput[lPos];
					}

					//
					//GetGame()->GetCGuiEx()->GetMainBarPageEx()->m_strPrivateChatName = szName;
				}

				return (eChatType)c;
			}
		}
	}

	lPos = 0;
	return TYPE_NUM;
}


// 获取第几个单词
long CChat::GetWord(char* strSource, char* strDest, long num, long lStartPos)
{
	strstream stream(&strSource[lStartPos], lstrlen(strSource)-lStartPos, ios::in);
	long l = 0;

	while( stream >> strDest )
	{
		if( stream.eof() )
		{
			strDest[0] = 0;
			return 0;
		}
		
		if( l == num )
		{
			long lPos = stream.tellg() + lStartPos;

			// 获取具体聊天数据的位置	
			for(lPos; lPos<lstrlen(strSource); lPos++)
			{
				if( strSource[lPos] != ' ' )
				{
					break;
				}
			}
			return lPos;
		}

		l++;
	}

	strDest[0] = 0;
	return 0;
}
