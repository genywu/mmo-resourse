#pragma once
//服务器信息类型
enum eInfoType
{
	CONNECTS=0,								//连接数
	AITICKS,								//循环次数

	//LS
	LS_PLAYER=			10,					//在线人数
	LS_WRONGCDKEY,							//冻结CDKey
	//LS end---

	//DBS
	DBS_LASTSAVETIME	=	40,		        //最后一次保存用时
	DBS_MEMORY,						        //内存使用
	DBS_OPERMAIL,		                    //42	邮件操作数
	DBS_OPERFACTION,				        //帮会操作数
	DBS_OPERPLAYER,		                    //44	角色操作数
	DBS_LOADENTITY,					        //载入实体数
	DBS_SAVEENTITY,		                    //46	保存实体数
	DBS_LOADNUM,					        //载入数
	DBS_SAVENUM,		                    //48	保存数
	DBS_LOG,						        //日志
	DBS_REMSG,			                    //50	返回消息
	DBS_LOADTHREAD,					        //载入使用进程数
	DBS_SAVETHREAD,		                    //52	保存使用进程数
	DBS_SAVE_ALL,		                    //保存所有数据
	//DBS end---

	//WS
	WS_PLAYER=70,		//	70	在线人数
	WS_REGION,			//		场景及副本数
	WS_W2L,				//		WS到LS消息量
	WS_L2W,				//		LS到WS消息量
	WS_W2DB,			//		WS到DBS消息量
	WS_DB2W,			//	75	DBS到WS消息量
	WS_W2S,				//		WS到GS消息量
	WS_S2W,				//		GS到WS消息量
	WS_RELOADFILE,		//		重读资源文件
	WS_TOPNEWS,			//		置顶公告
	WS_KICK,			//	80	踢人
	WS_KICK_ALL,		//		踢世界人
	WS_SAVE_ALL,		//      保存游戏数据
	WS_SAVE_ALL_NOW,	//		立即存当前数据
	//WS end---

	//GS
	GS_PALYER=			100,//玩家数
	GS_TEAM,				//队伍数
	GS_SCRIPTS,		        //102	脚本数
	GS_SERVERID,			//服务器ID
	GS_S2C,			        //104	GS到GC消息量
	GS_C2S,					//GC到GS消息量
	GS_W2S,			        //105	WS到GS消息量
	GS_S2W,					//GS到WS消息量
	GS_TEAMDUP,		        //107	队伍副本数
	GS_ONEDUP,				//个人副本数
	//GS end---

	//LOGS
	LOGS_CREATETABLE=	140,//创建表
};