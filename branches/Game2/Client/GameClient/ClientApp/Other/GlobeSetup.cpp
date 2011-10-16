#include "StdAfx.h"
#include ".\globesetup.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGlobeSetup::tagSetup	CGlobeSetup::m_tagSetup;
CGlobeSetup::CGlobeSetup(void)
{
}

CGlobeSetup::~CGlobeSetup(void)
{
}

// 读取
bool CGlobeSetup::Load(const char* filename)
{
	stringstream stream;	
	CRFile* prfile = rfOpen(filename);
	if( !prfile )
	{
		char str[256];
		sprintf(str, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	prfile->ReadToStream(stream);
	rfClose(prfile);

	char strTemp[256];
	long a,r,g,b;

	// 物品买卖商店价格基本折旧率
	stream >> strTemp >> GetSetup()->fMoneyBaseRate;

	// 物品买卖商店价格折旧率
	stream >> strTemp >> GetSetup()->fMoneyRate;

	// 角色最大数
	stream >> strTemp >> a;
	GetSetup()->btMaxCharactersNum = (BYTE)a;
	
	//角色最大等级
	stream >> strTemp >> a;
	GetSetup()->dwMaxPlayerLevel = (DWORD)a;
	// 金币的物编号
	stream >> strTemp >> a;
	GetSetup()->dwMoneyIndex = (DWORD)a;

	// 银币的物编号
	stream >> strTemp >> a;
	GetSetup()->dwSilverMoneyIndex = (DWORD)a;

	// 警告信息颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorWarningText = D3DCOLOR_ARGB(a,r,g,b);

	// 鼠标选中的shape
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorMouseShape = D3DCOLOR_ARGB(a,r,g,b);

	// 白名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorWhiteName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorWhiteNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 紫名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorPurpleName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorPurpleNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 红名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorRedName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorRedNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 橙名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorOrangeName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorOrangeNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 蓝名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBlueName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBlueNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 黄名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorYellowName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorYellowNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 灰名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGrayName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGrayNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 没有打开国家标志时,其他玩家客户端应该显示的名字颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorNoShowCountryName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorNoShowCountryNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 同阵营名
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorAllianceName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorAllianceNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 非同阵营名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 队友名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTeammateName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTeammateNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// Boss名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBossName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBossNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// NPC名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorNpcName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorNpcNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// Monster名字
	for (int i=0;i<7;i++)
	{
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorMonsterName[i] = D3DCOLOR_ARGB(a,r,g,b);
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorMonsterNameBack[i] = D3DCOLOR_ARGB(a,r,g,b);
	}
	// Monster的称谓名字
	for (int i=0;i<8;i++)
	{
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorMonsterTitelName[i] = D3DCOLOR_ARGB(a,r,g,b);
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorMonsterTitelNameBack[i] = D3DCOLOR_ARGB(a,r,g,b);
	}

	// DiedMonster名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorDiedMonsterName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorDiedMonsterNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// Goods名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// CityGate名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorCityGateName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorCityGateNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 爵位称号颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorKnight = D3DCOLOR_ARGB(a,r,g,b);

	// 帮会称号颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorFaction = D3DCOLOR_ARGB(a,r,g,b);

	// 勋章称号颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorMedal = D3DCOLOR_ARGB(a,r,g,b);

	// 怪物减血数值
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBloodTextMonster = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBloodTextMonsterBack = D3DCOLOR_ARGB(a,r,g,b);

	// 玩家减血数值
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBloodTextPlayer = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBloodTextPlayerBack = D3DCOLOR_ARGB(a,r,g,b);

	// 玩家加血数值
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorAddBloodTextPlayer = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorAddBloodTextPlayerBack = D3DCOLOR_ARGB(a,r,g,b);

	// 经验值字体提示颜色
	stream >> strTemp >> r >> g >> b;
	GetSetup()->colorExpTipText = D3DCOLOR_ARGB(0, r, g, b);

	// 经验值字体提示背景颜色
	stream >> strTemp >> r >> g >> b;
	GetSetup()->colorExpTipTextBack = D3DCOLOR_ARGB(0, r, g, b);

	// 功勋值字体提示颜色
	stream >> strTemp >> r >> g >> b;
	GetSetup()->colorConTipText = D3DCOLOR_ARGB(0, r, g, b);

	// 功勋值字体提示背景颜色
	stream >> strTemp >> r >> g >> b;
	GetSetup()->colorConTipTextBack = D3DCOLOR_ARGB(0, r, g, b);

	// 经验值显示持续时间
	stream >> strTemp >> GetSetup()->dwExpTipTextTimeLength;

	// 经验值提示最多显示行数
	stream >> strTemp >> GetSetup()->dwExpTipTextLineMax;

	// 经验值提示行间距
	stream >> strTemp >> GetSetup()->fExpTipTextLineSpace;

	// 经验值提示数字缩放比例
	stream >> strTemp >> GetSetup()->fExpTipTextNumberScale;

	// 玩家头顶信息绘制高度增量
	stream >> strTemp >> GetSetup()->fPlayerInfoHeight;
	stream >> strTemp >> GetSetup()->fMonsterInfoHeight;

	// 聊天缓存条目数量
	stream >> strTemp >> a;
	GetSetup()->dwChatItemNum = (DWORD)a+1;

	// 聊天颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorChatBackBar = D3DCOLOR_ARGB(a,r,g,b);

	for (int i=0;i<12;i++)
	{
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorChatText[i] = D3DCOLOR_ARGB(a,r,g,b);
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorChatTextBack[i] = D3DCOLOR_ARGB(a,r,g,b);
		stream >> strTemp >> a >> r >> g >> b;
		GetSetup()->colorChatTextShadow[i] = D3DCOLOR_ARGB(a,r,g,b);
	}

	// 置顶聊天颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTopChat = D3DCOLOR_ARGB(a,r,g,b);

	// 置顶聊天描边颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTopChatShadow = D3DCOLOR_ARGB(a,r,g,b);

	// 置顶聊天道具
	stream >> strTemp >> a;
	GetSetup()->dwTopChatPropIndex = (DWORD)a;

	// 置顶聊天字数限制
	stream >> strTemp >> a;
	GetSetup()->dwTopChatMaxNum = (DWORD)a;

	// 置顶聊天停留时间
	stream >> strTemp >> a;
	GetSetup()->dwTopChatTime = (DWORD)a;

	// 置顶聊天中部Y坐标
	stream >> strTemp >> a;
	GetSetup()->lTopChatY = a;

	// 置顶在左下聊天框颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTopChatInBox = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTopChatInBoxBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorTopChatInBoxShadow = D3DCOLOR_ARGB(a,r,g,b);

	// 跑马灯 颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBroadcast = D3DCOLOR_ARGB(a,r,g,b);

	// 跑马灯 描边颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorBroadcastShadow = D3DCOLOR_ARGB(a,r,g,b);

	// 跑马灯 速度
	stream >> strTemp >> a;
	GetSetup()->dwBroadcastSpeed = (DWORD)a;

	// 跑马灯 循环次数
	stream >> strTemp >> a;
	GetSetup()->dwBroadcastLoop = (DWORD)a;

	// 跑马灯 Y坐标
	stream >> strTemp >> a;
	GetSetup()->lBroadcastY = a;

	// 脚本公告 颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorScriptBroadcast = D3DCOLOR_ARGB(a,r,g,b);

	// 脚本公告 背景颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorScriptBroadcastBack = D3DCOLOR_ARGB(a,r,g,b);

	// 脚本公告 描边颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorScriptBroadcastShadow = D3DCOLOR_ARGB(a,r,g,b);

	// 白色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsWhite = D3DCOLOR_ARGB(a,r,g,b);

	// 蓝色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsBlue = D3DCOLOR_ARGB(a,r,g,b);

	// 红色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsRed = D3DCOLOR_ARGB(a,r,g,b);

	// 物品的下降属性颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsDrop = D3DCOLOR_ARGB(a,r,g,b);

	// 金色(附加属性超过4项)
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsGold = D3DCOLOR_ARGB(a,r,g,b);

	// 暗金色(特殊属性)
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsDarkGold = D3DCOLOR_ARGB(a,r,g,b);

	// 极品 100项附加属性!!!!????
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsGod = D3DCOLOR_ARGB(a,r,g,b);

	// 附魔
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsFuMo = D3DCOLOR_ARGB(a,r,g,b);

	// 附魔
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsDesc = D3DCOLOR_ARGB(a,r,g,b);

	// 附魔
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsPrice = D3DCOLOR_ARGB(a,r,g,b);

	//基本属性描述紫色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsPurpleBaseDesp = D3DCOLOR_ARGB(a,r,g,b);

	//追加属性描述紫色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsPurpleAddtionDesp = D3DCOLOR_ARGB(a,r,g,b);

	//基本属性描述橙色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsOrangeBaseDesp = D3DCOLOR_ARGB(a,r,g,b);

	//追加属性描述橙色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorGoodsOrangeAddtionDesp = D3DCOLOR_ARGB(a,r,g,b);

	// 个人商店底板颜色
	// 个人商店底板边框颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorShopBar = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorShopWireBar = D3DCOLOR_ARGB(a,r,g,b);

	// 个人商店名字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorShopName = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorShopNameBack = D3DCOLOR_ARGB(a,r,g,b);

	// 致命一击减血数值
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorCriticalBloodText = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorCriticalBloodTextBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> GetSetup()->fCriticalTextScale;

	// 好友上下线提示文字
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorFriendOnlineOffline = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorFriendOnlineOfflineBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorFriendOnlineOfflineShadow = D3DCOLOR_ARGB(a,r,g,b);

	// 等级差颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorLowerLevel = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorMiddleLevel = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorUpperLevel = D3DCOLOR_ARGB(a,r,g,b);

	// 物品的背景
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorItemBlueBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorItemRedBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorHandItemCanPlaceBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorHandItemCanChangeBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorHandItemCannotChangeBack = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorInShopMirrorBack = D3DCOLOR_ARGB(a,r,g,b);
	
	//敌国玩家名字颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyPlayerNameGreen = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyPlayerNameGray = D3DCOLOR_ARGB(a,r,g,b);
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyPlayerNamePurple = D3DCOLOR_ARGB(a,r,g,b);

	// 竞技区敌对阵营名字颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyNameInBattleZone = D3DCOLOR_ARGB(a,r,g,b);

	// 竞技区敌对阵营名字颜色背景色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyNameInBattleZoneBack = D3DCOLOR_ARGB(a,r,g,b);

	// 战争属性敌对阵营名字颜色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyNameInWarZone = D3DCOLOR_ARGB(a,r,g,b);

	// 战争属性敌对阵营名字颜色背景色
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorEnemyNameInWarZoneBack = D3DCOLOR_ARGB(a,r,g,b);

	// 玩家升级特效
	stream >> strTemp >> a;
	GetSetup()->dwLevelUpEffectID = (DWORD)a;

	// 玩家技能升级特效
	stream >> strTemp >> a;
	GetSetup()->dwSkillLevelUpEffectID = (DWORD)a;

	// 玩家声望升级特效
	stream >> strTemp >> a;
	GetSetup()->dwReputeLevelUpEffectID = (DWORD)a;

	//玩家职业升级特效
    stream >> strTemp >> a;
    GetSetup()->dwOccuLevelUpEffectID = (DWORD)a;

	// 任务保护怪的名字颜色（创建者）
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorQuestMstByCreator = D3DCOLOR_ARGB(a,r,g,b);

	// 任务保护怪的名字颜色（非创建者）
	stream >> strTemp >> a >> r >> g >> b;
	GetSetup()->colorQuestMstByOther = D3DCOLOR_ARGB(a,r,g,b);

	// 玩家透明渐变时间长度(为0则表示没有透明渐变)
	stream >> strTemp >> a;
	GetSetup()->dwPlayerAlphaFadeTimeLength = (DWORD)a;

	// 怪物透明渐变时间长度(为0则表示没有透明渐变)
	stream >> strTemp >> a;
	GetSetup()->dwMonsterAlphaFadeTimeLength = (DWORD)a;

	// 采集物透明渐变时间长度(为0则表示没有透明渐变)
	stream >> strTemp >> a;
	GetSetup()->dwCollectionAlphaFadeTimeLength = (DWORD)a;

	// 物品透明渐变时间长度(为0则表示没有透明渐变)
	stream >> strTemp >> a;
	GetSetup()->dwGoodsAlphaFadeTimeLength = (DWORD)a;

	// 普通邮件的邮资
	stream >> strTemp >> a;
	GetSetup()->dwNormalMailPrice = (DWORD)a;

	// 普通附件邮件的邮资
	stream >> strTemp >> a;
	GetSetup()->dwNormalGoodsMailPrice = (DWORD)a;

	// 赠送邮件的邮资
	stream >> strTemp >> a;
	GetSetup()->dwPresentMailPrice = (DWORD)a;

	// 索要邮件的邮资
	stream >> strTemp >> a;
	GetSetup()->dwRequestMailPrice = (DWORD)a;

	// 付费收信邮件的邮资
	stream >> strTemp >> a;
	GetSetup()->dwPayGoldMailPrice = (DWORD)a;

	// Tab键查找对象的范围
	stream >> strTemp >> a;
	GetSetup()->lAreaByTab = (long)a;

	// loading的延迟时间
	stream >> strTemp >> a;
	GetSetup()->dwLoadingDelayTime = (DWORD)a;

	// 连续使用技能发送消息次数
	stream >> strTemp >> a;
	GetSetup()->lUseSkillMsgCount = (long)a;

	// 连续移动发送消息次数
	stream >> strTemp >> a;
	GetSetup()->lMoveMsgCount = (long)a;

	// 背包整理的消息发送延迟时间
	stream >> strTemp >> a;
	GetSetup()->dwSendFinishBagDelayTime = (DWORD)a;

	//防外挂界面答题时间
	stream >> strTemp >> a;
	GetSetup()->dwAnswerTime = (DWORD)a;
	//防外挂界面自动打开时间
	stream >> strTemp >> a;
	GetSetup()->dwAutoOpenTime = (DWORD)a;
	//防外挂精灵消息提示音ID
	stream >> strTemp >> a;
	GetSetup()->dwSonudID	= (DWORD)a;

	stream >> strTemp >> m_tagSetup.lEncryptType;

	//好友搜索功能的等级限制
	stream >> strTemp >> a;
	GetSetup()->dwLimit=(DWORD)a;
	//好友搜索功能的等级限制
	stream >> strTemp >> a;
	GetSetup()->lUseSkillTime = (long)a;
	//复活道具使用限制的时间
	stream >> strTemp >> a;
	GetSetup()->lReliveTime = (long)a;
	//默认使用复活物品的id
	stream >> strTemp >> a;
	GetSetup()->lReliveItem = (long)a;
	//CP值的显示等级限制
	stream >> strTemp >> a;
	GetSetup()->lPKCPShowLevel = (long)a;
	//私聊任务栏最大栏位数目
	stream >> strTemp >> a;
	GetSetup()->dwMaxNumPrivateChat = (DWORD)a;
	//私聊历史每页显示条目数
	stream >> strTemp >> a;
	GetSetup()->dwPrivateChatHistoryMaxItem = (DWORD)a;
	//私聊消息盒子最大条目总数
	stream >> strTemp >> a;
	GetSetup()->dwMaxPriChatMsgBoxTotalItem = (DWORD)a;

	//跑商货币的ID
	stream >> strTemp >> a;
	GetSetup()->dwFBCoinID = (DWORD)a;
    //最小操作距离
    stream >> strTemp >> GetSetup()->fminimumOpDistance;

    //直线方向上最大允许移动距离
    stream >> strTemp >> GetSetup()->fLineMaxDistanceAllowToMove;
    float abb = GetSetup()->fLineMaxDistanceAllowToMove;
	/***********************************************************************/
	/* zhaohang add */
	//程序里标识位面名称的变量
	stream >> strTemp >> GetSetup()->strWeiMianName;
	//仙贝寄售手续费费率
	stream >> strTemp >> GetSetup()->agentExtracust;
	//仙贝寄售一单对应的位面数
	stream >> strTemp >> GetSetup()->oderPerNum;
	//技能文案配置文件名称
	stream >> strTemp >> GetSetup()->strSkillDocCon;
	//技能显示配置文件名称
	stream >> strTemp >> GetSetup()->strSkillShowIni;
	//技能姿态配置文件名称
	stream >> strTemp >> GetSetup()->strSkillPoseIni;
	//技能属性配置文件名称
	stream >> strTemp >> GetSetup()->strSkillAttrIniSetup;
	/***********************************************************************/
	return true;
}
