#include "StdAfx.h"
#include ".\logsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HWND g_hWnd;
CLogSystem::tagLogSystem CLogSystem::m_stSetting;

CLogSystem::CLogSystem(void)
{
}

CLogSystem::~CLogSystem(void)
{
}



// 从文件读取列表
bool CLogSystem::Load(const char* filename)
{
	ifstream stream;
	stream.open(filename);
	if( !stream.is_open() )
	{
		char str[256];
		_snprintf(str, 256, "file '%s' can't found!", filename);
		MessageBox(g_hWnd, str, "error", MB_OK);
		return FALSE;
	}

	string strTemp;
	stream	
		// 物品相关
		>> strTemp >> GetSetting()->bGoodsTradeLog			//交易日志
		>> strTemp >> GetSetting()->bGoodsShopLog			//商店交易

		>> strTemp >> GetSetting()->bGoodsGetFromRegion		//拾取
		>> strTemp >> GetSetting()->bGoodsBuyFromNPC		//从NPC购买

		>> strTemp >> GetSetting()->bGoodsSellToNPC			//出售给NPC
		>> strTemp >> GetSetting()->bGoodsDropToRegion		//丢弃消失
		>> strTemp >> GetSetting()->bGoodsLostByBroken		//损坏消失
		>> strTemp >> GetSetting()->bGoodsLostByUpgrade		//升级消失
		>> strTemp >> GetSetting()->bGoodsLostByDead		//死亡消失
		//仓库银行
		>> strTemp >> GetSetting()->bGoodsDepotSet
		>> strTemp >> GetSetting()->bGoodsDepotGet
		>> strTemp >> GetSetting()->bGoodsBankSet
		>> strTemp >> GetSetting()->bGoodsBankGet

		>> strTemp >> GetSetting()->bGoodsSplit
		>> strTemp >> GetSetting()->bGoodsEnhange
		>> strTemp >> GetSetting()->bGoodsMake
		>> strTemp >> GetSetting()->bGoodsUesed

		>> strTemp >> GetSetting()->bGoodsUpgradeSuccess	// 升级成功
		>> strTemp >> GetSetting()->bGoodsUpgradeFailure	// 升级失败

		>> strTemp >> GetSetting()->bGoodsGemExchange		// 宝石兑换
		>> strTemp >> GetSetting()->bGoodsJewelryMade		// 制作首饰

		>> strTemp >> GetSetting()->bGoodsLargess			// 赠送物品


		//经验等级
		>> strTemp >> GetSetting()->bExpDecrease			//经验增加
		>> strTemp >> GetSetting()->bExpIncrease			//经验减少

		>> strTemp >> GetSetting()->bLevelLog				//等级变化

		>> strTemp >> GetSetting()->bTitileLog				//身份变化

		>> strTemp >> GetSetting()->bDeleteLog				//删除日志

		>> strTemp >> GetSetting()->bDiedLog				//死亡日志

		//队伍相关
		>> strTemp >> GetSetting()->bTeamJion				//加入队员
		>> strTemp >> GetSetting()->bTeamLeave				//队员离开

		//pk相关
		>> strTemp >> GetSetting()->bPlayerKiller
		>> strTemp >> GetSetting()->bPlayerKilled

		// 帮会相关
		>> strTemp >> GetSetting()->bFactionCreate
		>> strTemp >> GetSetting()->bFactionDisband
		>> strTemp >> GetSetting()->bFactionApply
		>> strTemp >> GetSetting()->bFactionQuit

		>> strTemp >> GetSetting()->bFactionJoin
		>> strTemp >> GetSetting()->bFactionFireOut

		>> strTemp >> GetSetting()->bFactionTitleChanged
		>> strTemp >> GetSetting()->bFactionPurviewAdd
		>> strTemp >> GetSetting()->bFactionPurviewSub
		>> strTemp >> GetSetting()->bFactionMasterChnaned

		>> strTemp >> GetSetting()->bFactionExp
		>> strTemp >> GetSetting()->bFactionLel

		// 聊天
		>> strTemp >> GetSetting()->bNormalChat				//屏幕聊天
		>> strTemp >> GetSetting()->bRegionChat				//区域聊天
		>> strTemp >> GetSetting()->bFactionChat			//帮会聊天
		>> strTemp >> GetSetting()->bUnionChat				//联盟聊天
		>> strTemp >> GetSetting()->bTeamChat				//队伍聊天
		>> strTemp >> GetSetting()->bPrivateChat			//私聊
		>> strTemp >> GetSetting()->bGMCommand				//GM指令

		>> strTemp >> GetSetting()->bChMap0
		>> strTemp >> GetSetting()->bChMap1
		>> strTemp >> GetSetting()->bChMap2

		>> strTemp >> GetSetting()->bCardDealing

		>> strTemp >> GetSetting()->bIncrement
		; // end ;


	stream.close();
	return true;
}

// 添加到CByteArray
bool CLogSystem::AddToByteArray(vector<BYTE>* pByteArray)
{
	_AddToByteArray(pByteArray, GetSetting(), sizeof(tagLogSystem));
	return true;
}

// 解码
bool CLogSystem::DecordFromByteArray(BYTE* pByte, long& pos)
{
	_GetBufferFromByteArray(pByte, pos, GetSetting(), sizeof(tagLogSystem));
	return true;
}
