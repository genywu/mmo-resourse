
#pragma once

//!		物品扩展操作窗口类型
enum ePageType{
	PT_DisassembleWindow,		//! 分解窗口
	PT_PrescriptionWindow,	//! 合成窗口
	PT_BindWindow,			//! 绑定窗口
	PT_FrostWindow,			//! 冻结窗口
	PT_UpgradeWindow,			//! 升级窗口
	PT_PlusEvilWindow,		//! 魔化窗口
	PT_EvilBackoutWindow,		//! 魔化拆除窗口
	PT_EvilSynthesizeWindow,	//! 魔化材料合成窗口
	PT_DepotWindow,			//! 仓库窗口
	/////XY 2008.1.21/////
	PT_CardFuntionWindow,		//!	镶卡主功能页面	
	PT_PlayerTradeWindow,		//! 玩家交易功能页面
	//////////////////////
	PT_Number,
	PT_NULL,
};

struct tagEnchaseSession
{
	CGUID	m_SessionGuid;
	long	lPlayerID;
	BYTE	iPageType;
	tagEnchaseSession() : m_SessionGuid(CGUID::GUID_INVALID), lPlayerID(0), iPageType(0)
	{}
};

struct tagEnchaseGoods 
{
	CGoods* pGoods;
	CGoods* pBaoShi;
	CGoods* pCard[5];
	tagEnchaseGoods()
	{
		pGoods = NULL;
		pBaoShi = NULL;
		for(int i=0;i<5;i++)
		{
			pCard[i] = NULL;
		}
	}
};

enum CardEnchasePage  
{
	CardEnchasePage_BROACH = 1,					//钻孔页面
	CardEnchasePage_JAM,							//堵孔页面
	CardEnchasePage_ENCHASE,						//镶嵌页面
	CardEnchasePage_BACKOUT,						//拆卡页面

	CardEnchasePage_COUNT							//页面数量
};

// 仓库的子背包栏位总数
#define		DEPOT_SUBPACK_MAXNUM			4U
// 仓库的扩展栏位 首位置
#define		DEPOT_SUBPACK_BASEPOS			10001U

