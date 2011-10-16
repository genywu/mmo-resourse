#pragma once

// 通用设置
class CGlobeSetup
{
public:
	struct tagSetup
	{
		float		fMoneyBaseRate;			// 物品买卖商店价格基本折旧率
		float		fMoneyRate;				// 物品买卖商店价格折旧率
		BYTE		btMaxCharactersNum;		// 角色最大数
		DWORD		dwMaxPlayerLevel;		// 角色最大等级		
		DWORD		dwMoneyIndex;			// 金币的物编号
		DWORD		dwSilverMoneyIndex;		// 银币的物编号
		D3DCOLOR	colorWarningText;		// 警告信息颜色
		D3DCOLOR	colorMouseShape;		// 鼠标选中的shape
		D3DCOLOR	colorWhiteName;			// 白名
		D3DCOLOR	colorWhiteNameBack;		// 白名边框
		D3DCOLOR	colorPurpleName;		// 紫名
		D3DCOLOR	colorPurpleNameBack;	// 紫名边框
		D3DCOLOR	colorRedName;			// 红名
		D3DCOLOR	colorRedNameBack;		// 红名边框
		D3DCOLOR	colorOrangeName;		// 橙名
		D3DCOLOR	colorOrangeNameBack;	// 橙名边框
		D3DCOLOR	colorBlueName;			// 蓝名
		D3DCOLOR	colorBlueNameBack;		// 蓝名边框
		D3DCOLOR	colorYellowName;		// 黄名
		D3DCOLOR	colorYellowNameBack;	// 黄名边框
		D3DCOLOR	colorGrayName;			// 灰名
		D3DCOLOR	colorGrayNameBack;		// 灰名边框

		D3DCOLOR	colorNoShowCountryName;		//没有打开国家标志时,其他玩家客户端应该显示的名字颜色
		D3DCOLOR	colorNoShowCountryNameBack;	//没有打开国家标志时,其他玩家客户端应该显示的名字阴影颜色
		D3DCOLOR	colorAllianceName;		// 同阵营名
		D3DCOLOR	colorAllianceNameBack;	// 同阵营边框
		D3DCOLOR	colorEnemyName;			// 非同阵营名
		D3DCOLOR	colorEnemyNameBack;		// 非同阵营边框
		D3DCOLOR	colorTeammateName;		// 队友名
		D3DCOLOR	colorTeammateNameBack;	// 队友名边框
		D3DCOLOR	colorBossName;			// BOSS名字
		D3DCOLOR	colorBossNameBack;		// BOSS名字边框

		D3DCOLOR	colorNpcName;			// NPC名字
		D3DCOLOR	colorNpcNameBack;		// NPC名字边框
		D3DCOLOR	colorMonsterName[7];	// Monster名字
		D3DCOLOR	colorMonsterNameBack[7];// Monster名字边框
		D3DCOLOR	colorMonsterTitelName[8];	// Monster的称谓名字颜色
		D3DCOLOR	colorMonsterTitelNameBack[8];// Monster的称谓名字边框颜色
		D3DCOLOR	colorDiedMonsterName;		// 死亡的Monster名字
		D3DCOLOR	colorDiedMonsterNameBack;	// 死亡的Monster名字边框
		D3DCOLOR	colorGoodsName;			// Goods名字
		D3DCOLOR	colorGoodsNameBack;		// Goods名字边框
		D3DCOLOR	colorCityGateName;		// CityGate名字
		D3DCOLOR	colorCityGateNameBack;	// CityGate名字边框

		D3DCOLOR	colorKnight;				//爵位称号颜色
		D3DCOLOR	colorFaction;				//帮会称号颜色
		D3DCOLOR	colorMedal;					//勋章称号颜色

		D3DCOLOR	colorBloodTextMonster;			// 怪物减血数值
		D3DCOLOR	colorBloodTextMonsterBack;
		D3DCOLOR	colorBloodTextPlayer;			// 玩家减血数值
		D3DCOLOR	colorBloodTextPlayerBack;
		D3DCOLOR	colorAddBloodTextPlayer;		// 玩家加血数值颜色
		D3DCOLOR	colorAddBloodTextPlayerBack;	

		D3DCOLOR	colorExpTipText;				//经验值字体提示颜色
		D3DCOLOR	colorExpTipTextBack;			//经验值字体提示背景颜色
		D3DCOLOR	colorConTipText;				//功勋值字体提示颜色
		D3DCOLOR	colorConTipTextBack;			//功勋值字体提示背景颜色
		DWORD		dwExpTipTextTimeLength;			//经验值显示持续时间
		DWORD		dwExpTipTextLineMax;			//经验值提示最多显示行数
		float		fExpTipTextLineSpace;			//经验值提示行间距
		float		fExpTipTextNumberScale;			//经验值提示数字缩放比例

		float fPlayerInfoHeight;				//玩家头顶信息绘制高度增量
		float fMonsterInfoHeight;				//怪物头顶信息绘制高度增量(包括NPC)

		// 同屏聊天		0
		// 同场景聊天	1
		// 联盟聊天		2
		// 行会聊天		3
		// 队伍聊天		4
		// 两人私聊		5
		// 国家聊天		6
		// 盟国聊天		7
		// 世界聊天		8
		// 系统信息		9
		// 广播			10
		// GM命令		11

		DWORD		dwChatItemNum;		// 聊天缓存条目数量

		D3DCOLOR	colorChatBackBar;		// 背景条颜色
		D3DCOLOR	colorChatText[12];		// 聊天颜色
		D3DCOLOR	colorChatTextBack[12];
		D3DCOLOR	colorChatTextShadow[12];

		D3DCOLOR	colorTopChat;				// 置顶聊天颜色
		D3DCOLOR	colorTopChatShadow;			// 置顶聊天描边颜色
		DWORD		dwTopChatPropIndex;			// 置顶聊天道具
		DWORD		dwTopChatMaxNum;			// 置顶聊天字数限制
		DWORD		dwTopChatTime;				// 置顶聊天停留时间
		LONG		lTopChatY;					// 置顶聊天中部Y坐标

		D3DCOLOR	colorTopChatInBox;			// 置顶在左下聊天框颜色
		D3DCOLOR	colorTopChatInBoxBack;
		D3DCOLOR	colorTopChatInBoxShadow;	

		D3DCOLOR	colorBroadcast;				// 跑马灯 颜色
		D3DCOLOR	colorBroadcastShadow;		// 跑马灯 描边颜色
		DWORD		dwBroadcastSpeed;			// 跑马灯 速度
		DWORD		dwBroadcastLoop;			// 跑马灯 循环次数
		LONG		lBroadcastY;				// 跑马灯 Y坐标

		D3DCOLOR	colorScriptBroadcast;		// 脚本公告 颜色
		D3DCOLOR	colorScriptBroadcastBack;	// 脚本公告 背景颜色
		D3DCOLOR	colorScriptBroadcastShadow;	// 脚本公告 描边颜色

		//物品说明颜色
		D3DCOLOR	colorGoodsWhite;		// 白色
		D3DCOLOR	colorGoodsBlue;			// 蓝色
		D3DCOLOR	colorGoodsRed;			// 红色
		D3DCOLOR	colorGoodsDrop;			// 物品的下降属性颜色
		D3DCOLOR	colorGoodsGold;			// 金色(附加属性超过4项)
		D3DCOLOR	colorGoodsDarkGold;		// 暗金色(特殊属性)
		D3DCOLOR	colorGoodsGod;			// 极品 100项附加属性!!!!????
		D3DCOLOR	colorGoodsFuMo;			// 附魔
		D3DCOLOR	colorGoodsDesc;			// 描述
		D3DCOLOR	colorGoodsPrice;		// 售价
		D3DCOLOR	colorGoodsPurpleBaseDesp;		// 基本属性描述紫色
		D3DCOLOR	colorGoodsPurpleAddtionDesp;		// 追加属性描述紫色
		D3DCOLOR	colorGoodsOrangeBaseDesp;		// 基本属性描述橙色
		D3DCOLOR	colorGoodsOrangeAddtionDesp;		// 追加属性描述橙色

		D3DCOLOR	colorShopBar;			// 个人商店底板颜色
		D3DCOLOR	colorShopWireBar;		// 个人商店底板边框颜色
		D3DCOLOR	colorShopName;			// 个人商店名字
		D3DCOLOR	colorShopNameBack;		// 个人商店名字边框

		D3DCOLOR	colorCriticalBloodText;		// 致命一击减血数值
		D3DCOLOR	colorCriticalBloodTextBack;
		float		fCriticalTextScale;			// 致命一击的数字大小比例

		D3DCOLOR	colorFriendOnlineOffline;	// 好友上下线提示颜色
		D3DCOLOR	colorFriendOnlineOfflineBack;
		D3DCOLOR	colorFriendOnlineOfflineShadow;

		D3DCOLOR	colorLowerLevel;
		D3DCOLOR	colorMiddleLevel;
		D3DCOLOR	colorUpperLevel;

		D3DCOLOR	colorItemBlueBack;		// 物品的蓝色背景（可用）
		D3DCOLOR	colorItemRedBack;		// 物品的红色背景（不可用）
		D3DCOLOR	colorHandItemCanPlaceBack;		// 手上物品能放置遮罩色
		D3DCOLOR	colorHandItemCanChangeBack;		// 手上物品能交换遮罩色
		D3DCOLOR	colorHandItemCannotChangeBack;	// 手上物品不能交换遮罩色
		D3DCOLOR	colorInShopMirrorBack;	// 放置在个人商店或交易框中的镜像背景色
		D3DCOLOR	colorEnemyPlayerNameGreen;	//敌国玩家绿色名字
		D3DCOLOR	colorEnemyPlayerNameGray;	//敌国玩家灰色名字
		D3DCOLOR	colorEnemyPlayerNamePurple;	//敌国玩家紫色名字
		D3DCOLOR	colorEnemyNameInBattleZone;		//竞技区敌对阵营名字颜色
		D3DCOLOR	colorEnemyNameInBattleZoneBack;	//竞技区敌对阵营名字颜色背景色
		D3DCOLOR	colorEnemyNameInWarZone;		//战争属性敌对阵营名字颜色
		D3DCOLOR	colorEnemyNameInWarZoneBack;	//战争属性敌对阵营名字颜色背景色

		DWORD		dwLevelUpEffectID;				// 玩家升级特效
		DWORD		dwSkillLevelUpEffectID;			// 玩家技能升级特效
		DWORD		dwReputeLevelUpEffectID;		// 玩家声望升级特效
		DWORD       dwOccuLevelUpEffectID;          // 玩家职业升级特效

		D3DCOLOR	colorQuestMstByCreator;			// 任务保护怪的名字颜色（创建者）
		D3DCOLOR	colorQuestMstByOther;			// 任务保护怪的名字颜色（非创建者）

		DWORD dwPlayerAlphaFadeTimeLength;			//玩家透明渐变时间长度(为0则表示没有透明渐变)
		DWORD dwMonsterAlphaFadeTimeLength;			//怪物透明渐变时间长度(为0则表示没有透明渐变)
		DWORD dwCollectionAlphaFadeTimeLength;		//采集物透明渐变时间长度(为0则表示没有透明渐变)
		DWORD dwGoodsAlphaFadeTimeLength;			//物品透明渐变时间长度(为0则表示没有透明渐变)

		DWORD dwNormalMailPrice;					// 普通邮件的邮资
		DWORD dwNormalGoodsMailPrice;				// 普通附件邮件的邮资
		DWORD dwPresentMailPrice;					// 赠送邮件的邮资
		DWORD dwRequestMailPrice;					// 索要邮件的邮资
		DWORD dwPayGoldMailPrice;					// 付费收信邮件的邮资

		long lAreaByTab;							// Tab键查找对象的范围
		DWORD dwLoadingDelayTime;					// loading的延迟时间
		long	lUseSkillMsgCount;					// 连续使用技能发送消息次数
		long	lMoveMsgCount;						// 连续移动发送消息次数

		DWORD dwSendFinishBagDelayTime;				// 发送背包整理的消息的延迟时间
		DWORD dwAnswerTime;							// 防外挂答题时间
		DWORD dwAutoOpenTime;						// 防外挂界面自动打开时间
		DWORD dwSonudID;							// 防外挂精灵消息提示音ID
		long	lEncryptType;						// 通信加密开关
		DWORD dwLimit;								// 好友搜索功能的等级限制
		long	lUseSkillTime;						// 技能使用频率
		long   lReliveTime;                         // 复活使用道具时间
		long   lReliveItem;                         // 默认使用复活物品的id
		long   lPKCPShowLevel;                      // CP值的显示等级限制
		DWORD  dwPrivateChatHistoryMaxItem;			// 私聊历史每页显示条目数
		DWORD  dwMaxNumPrivateChat;					// 私聊任务栏最大栏位数目
		DWORD  dwMaxPriChatMsgBoxTotalItem;			//  私聊消息盒子最大条目总数
		DWORD  dwFBCoinID;							 // 家族跑商货币的ID
        float  fminimumOpDistance;                  // 最小操作距离
        float  fLineMaxDistanceAllowToMove;         // 直线上最大有效移动距离
		/***********************************************************************/
		/* zhaohang fix 2010-9-3
		/* 添加仙贝寄售的配置
		/***********************************************************************/
		char   strWeiMianName[256];						// 仙贝寄售时位面的名称
		float  agentExtracust;							// 仙贝寄售手续费费率
		int    oderPerNum;								// 仙贝寄售一单对应的位面数量
		/***********************************************************************/
		/* zhaohang add */
		char   strSkillDocCon[256];					///技能文案配置文件完整路径名称
		char   strSkillShowIni[256];				///技能显示配置文件完整路径名称
		char   strSkillPoseIni[256];				///技能姿态配置文件完整路径名称
		char   strSkillAttrIniSetup[256];			///技能属性配置文件完整路径名称
		/***********************************************************************/
	};

protected:
	static tagSetup	m_tagSetup;

public:
	static bool		Load(const char* filename);			// 读取
	static tagSetup*	GetSetup()	{return &m_tagSetup;}

public:
	CGlobeSetup(void);
	virtual ~CGlobeSetup(void);
};
