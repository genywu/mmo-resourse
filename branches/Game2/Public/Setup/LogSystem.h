//======================================================================================
/**
*	@file		LogSystem.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once

/**
*	@class LogSystem
*
*	@brief 日志系统配置类
*/
class LogSystem
{
public:
	/// 构造
	LogSystem(void);

	/// 析构
	~LogSystem(void);

private:
	/// 日志系统数据结构定义
	struct tagLogSystem
	{
		//物品相关
		bool	bGoodsTradeLog;			//交易日志
		bool	bGoodsShopLog;			//商店交易

		bool	bGoodsGetFromRegion;	//拾取
		bool	bGoodsBuyFromNPC;		//从NPC购买

		bool	bGoodsSellToNPC;		//出售到NPC
		bool	bGoodsDropToRegion;		//丢弃消失
		bool	bGoodsLostByBroken;		//损坏消失
		bool	bGoodsLostByUpgrade;	//升级消失
		bool	bGoodsLostByDead;		//死亡消失
		bool	bGoodsDepotSet;
		bool	bGoodsDepotGet;
		bool	bGoodsBankSet;
		bool	bGoodsBankGet;

		bool	bGoodsSplit;			// 拆分
		bool	bGoodsEnhange;			// 炼制
		bool	bGoodsMake;				// 合成
		bool	bGoodsUesed;			// 使用

		bool	bGoodsUpgradeSuccess;	// 升级成功
		bool	bGoodsUpgradeFailure;	// 升级失败

		bool	bGoodsGemExchange;		// 宝石兑换
		bool	bGoodsJewelryMade;		// 制作首饰


		bool	bGoodsLargess;			// 赠送物品

		//经验等级
		bool	bExpDecrease;			//经验增加
		bool	bExpIncrease;			//经验减少

		bool	bLevelLog;				//等级变化

		bool	bTitileLog;				//身份变化

		bool	bDeleteLog;				//删除日志

		bool	bDiedLog;				//死亡日志

		//队伍相关
		bool	bTeamJion;				//加入队员
		bool	bTeamLeave;				//离开队员

		//pk相关
		bool	bPlayerKiller;			//先手攻击
		bool	bPlayerKilled;			//pk死亡

		// 帮会相关
		bool	bFactionCreate;			//创建帮会
		bool	bFactionDisband;		//解散帮会
		bool	bFactionApply;			//申请入会
		bool	bFactionQuit;			//离开帮会

		bool	bFactionJoin;			//同意加入
		bool	bFactionFireOut;		//开除帮会

		bool	bFactionTitleChanged;	//成员头衔变更
		bool	bFactionPurviewAdd;		//成员权限变更
		bool	bFactionPurviewSub;		//减去全县
		bool	bFactionMasterChnaned;	//帮主变更日志

		bool	bFactionExp;			//帮会经验日志
		bool	bFactionLel;			//帮会等级日志

		bool	bNormalChat;			//屏幕聊天
		bool	bRegionChat;			//区域聊天
		bool	bFactionChat;			//帮会聊天
		bool	bUnionChat;				//联盟聊天
		bool	bTeamChat;				//队伍聊天
		bool	bPrivateChat;			//私聊
		bool	bGMCommand;				//GM指令

		bool	bChMap0;				//场景内切换地图
		bool	bChMap1;				//服务器内切换地图
		bool	bChMap2;				//服务器间切换地图

		bool	bCardDealing;			//点卡寄售日志

		bool	bIncrement;				//增值日志
	};

	/// 日志系统结构
	static tagLogSystem m_stSetting;

public:
	/// 从文件加载日志系统配置
	static bool Load(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);
	
	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 取得日志系统配置
	static tagLogSystem* GetSetting()	{return &m_stSetting;}
};
