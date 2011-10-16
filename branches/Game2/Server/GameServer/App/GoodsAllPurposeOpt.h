

#pragma once

const LONG STR_INFO_SIZE = 256;
const LONG STR_TXT_SIZE = 1024 * 15;

class GoodsAllPurposeOpt
{
public:
	GoodsAllPurposeOpt(void);
	~GoodsAllPurposeOpt(void);

public:
	//! 操作返回值
	enum eOptResult
	{
		//! 失败
		eOR_Failed,
		//! 成功
		eOR_Succeed,
		//! 非法（需要清除对象）
		eOR_Lawless,
	};

	//!					开始物品操作
	BOOL				BeginOpt(
							const CGUID	&OwnerGuid,			//! 所属玩家GUID
							DWORD		dwTitleID,			//! 标题ID
							DWORD		dwButtonID,			//! 按钮ID
							LPCSTR		pText,				//! 基本说明文字
							LPCSTR		pOnPushScript,		//! 响应物品放入的脚本名
							LPCSTR		pOnFinishScript,	//! 相应按钮按下的脚本名
							LONG		lRadius,			//! 会话时允许的活动半径
							LONG		lPosX,				//! 活动范围中心X坐标
							LONG		lPosY,				//! 活动范围中心Y坐标
							const CGUID	&RegionGuid,		//! 所在地图GUID
							DWORD		dwMaxPushNum		//! 最大放置物品的数量（不超过MAX_ALLPURPOSE_CONTAINER_SIZE）
							);

	//!					响应物品放入
	eOptResult			OnGoodsPut(const CGUID	&GoodsGuid, DWORD dwIdx);

	//!					响应物品移除
	eOptResult			OnGoodsPop(DWORD dwIdx);

	//!					响应开始物品操作
	eOptResult			FinishOpt(void);

public:
	//!					得到容器中的物品GUID
	const	CGUID&		GetOptGoodsGuid(DWORD dwIdx){if(m_dwMaxPushNum > dwIdx)return m_arrGoodsGuid[dwIdx];return NULL_GUID;}
	//!					刷新容器显示内容
	eOptResult			SetTxt(LPCSTR pTxt);
	//!					刷新容器显示金钱数
	eOptResult			SetGold(DWORD dwGoldNum);
	//!
	void				Release(BOOL bSendCloseMsg = TRUE);

private:
	//!					判断玩家操作的合法性
	BOOL				CanOpt(CPlayer *pPlayer);

private:
	//!					所属玩家GUID
	CGUID				m_OwnerGuid;
	//!					标题ID
	DWORD				m_dwTitleID;
	//!					按钮ID
	DWORD				m_dwButtonID;
	//!					基本说明文字
	CHAR				m_szText[STR_TXT_SIZE];
	//!					响应物品放入的脚本名
	CHAR				m_szOnPushScript[STR_INFO_SIZE];
	//!					相应按钮按下的脚本名
	CHAR				m_szOnFinishScript[STR_INFO_SIZE];
	//!					会话时允许的活动半径
	LONG				m_lRadius;
	//!					活动范围中心X坐标
	LONG				m_lPosX;
	//!					活动范围中心Y坐标
	LONG				m_lPosY;
	//!					对话所在地图GUID
	CGUID				m_RegionGuid;

	//!					放入物品的GUID
	CGUID				m_arrGoodsGuid[MAX_ALLPURPOSE_CONTAINER_SIZE];
	//!					最大放入物品的数量
	DWORD				m_dwMaxPushNum;
};