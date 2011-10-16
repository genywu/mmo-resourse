#pragma once

// 聊天
class CChat
{
public:

	enum eChatType
	{
		CHAT_NORMAL = 0,		// 同屏聊天
		CHAT_REGION,			// 同场景聊天
		CHAT_FACTION,			// 帮会聊天
		CHAT_UNION,				// 联盟聊天
		CHAT_TEAM,				// 队伍聊天
		CHAT_PRIVATE,			// 两人私聊
		CHAT_COUNTRY,			// 国家聊天
		CHAT_WOLRD,				// 世界聊天

		CHAT_BOARDCAST,			// 广播

		GM_CMD,					// GM命令
		RUN_SCRIPT,				// 运行脚本
		RUN_HELP,				// 帮助

		TYPE_NUM,
	};

public:
	CChat(void);
	virtual ~CChat(void);
};
