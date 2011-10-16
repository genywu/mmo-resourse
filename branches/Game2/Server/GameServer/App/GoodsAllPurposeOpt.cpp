

#include "StdAfx.h"
#include "GoodsAllPurposeOpt.h"
#include "Player.h"
//#include "Script/Script.h"



GoodsAllPurposeOpt::GoodsAllPurposeOpt(void)
:m_OwnerGuid(NULL_GUID)
,m_dwTitleID(0)
,m_dwButtonID(0)
,m_lRadius(0L)
,m_lPosX(0L)
,m_lPosY(0L)
,m_RegionGuid(NULL_GUID)
,m_dwMaxPushNum(0)
{
	memset(m_szText, 0, STR_TXT_SIZE);
	memset(m_szOnPushScript, 0, STR_INFO_SIZE);
	memset(m_szOnFinishScript, 0, STR_INFO_SIZE);
	memset(m_arrGoodsGuid, 0, sizeof(CGUID) * MAX_ALLPURPOSE_CONTAINER_SIZE);
}
GoodsAllPurposeOpt::~GoodsAllPurposeOpt(void)
{
	Release(FALSE);
}

//!
void GoodsAllPurposeOpt::Release(BOOL bSendCloseMsg)
{
	memset(m_arrGoodsGuid, 0, sizeof(CGUID) * MAX_ALLPURPOSE_CONTAINER_SIZE);
	m_RegionGuid	= NULL_GUID;
	m_dwMaxPushNum	= 0;

	memset(m_szText, 0, STR_TXT_SIZE);
	memset(m_szOnPushScript, 0, STR_INFO_SIZE);
	memset(m_szOnFinishScript, 0, STR_INFO_SIZE);

	if(bSendCloseMsg)
	{
		CMessage msg(MSG_S2C_GOODS_AllPurpose_Close);
		msg.SendToPlayer(m_OwnerGuid);
	}
}

//! 开始物品操作
BOOL GoodsAllPurposeOpt::BeginOpt(
	 const	CGUID	&OwnerGuid,			//! 所属玩家GUID
	 DWORD			dwTitleID,			//! 标题ID
	 DWORD			dwButtonID,			//! 按钮ID
	 LPCSTR			pText,				//! 基本说明文字
	 LPCSTR			pOnPushScript,		//! 响应物品放入的脚本名
	 LPCSTR			pOnFinishScript,	//! 相应按钮按下的脚本名
	 LONG			lRadius,			//! 会话时允许的活动半径
	 LONG			lPosX,				//! 活动范围中心X坐标
	 LONG			lPosY,				//! 活动范围中心Y坐标
	 const	CGUID	&RegionGuid,		//! 所在地图GUID
	 DWORD			dwMaxPushNum			//! 最大放置物品的数量（不超过MAX_ALLPURPOSE_CONTAINER_SIZE）
)
{
	Release(FALSE);

	if(NULL == pText || NULL == pOnPushScript || NULL == pOnFinishScript || 0 >= lRadius || 0 > lPosX || 0 > lPosY || MAX_ALLPURPOSE_CONTAINER_SIZE < dwMaxPushNum || 0 == dwMaxPushNum)
		return FALSE;

	//if(NULL == GetGame()->GetScriptFileData(pOnPushScript) || GetGame()->GetScriptFileData(pOnFinishScript))
		//return FALSE;

	m_OwnerGuid			= OwnerGuid;
	m_dwTitleID			= dwTitleID;
	m_dwButtonID		= dwButtonID;
	m_dwMaxPushNum		= dwMaxPushNum;

	memcpy(m_szText, pText, min(STR_TXT_SIZE - 1, strlen(pText)));
	memcpy(m_szOnPushScript, pOnPushScript, min(STR_INFO_SIZE - 1, strlen(pOnPushScript)));
	memcpy(m_szOnFinishScript, pOnFinishScript, min(STR_INFO_SIZE - 1, strlen(pOnFinishScript)));

	m_lRadius			= lRadius;
	m_lPosX				= lPosX;
	m_lPosY				= lPosY;
	m_RegionGuid		= RegionGuid;

	//! 
	CMessage msgOpen(MSG_S2C_GOODS_AllPurpose_Open);

	DBWriteSet OpendbWriteSet;
	msgOpen.GetDBWriteSet(OpendbWriteSet);
	OpendbWriteSet.AddToByteArray(dwMaxPushNum);
	OpendbWriteSet.AddToByteArray(m_dwTitleID);
	OpendbWriteSet.AddToByteArray(m_dwButtonID);
	OpendbWriteSet.AddToByteArray(m_lRadius);
	OpendbWriteSet.AddToByteArray(m_lPosX);
	OpendbWriteSet.AddToByteArray(m_lPosY);

	msgOpen.SendToPlayer(OwnerGuid, false);

	//! 
	CMessage msgTxt(MSG_S2C_GOODS_AllPurpose_UpdateTxt);

	DBWriteSet TxtdbWriteSet;
	msgTxt.GetDBWriteSet(TxtdbWriteSet);
	TxtdbWriteSet.AddToByteArray(m_szText);

	msgTxt.SendToPlayer(m_OwnerGuid, false);

	return TRUE;
}

//! 响应物品放入
GoodsAllPurposeOpt::eOptResult GoodsAllPurposeOpt::OnGoodsPut(const CGUID &GoodsGuid, DWORD dwIdx)
{
	if(m_dwMaxPushNum > dwIdx)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(m_OwnerGuid);
		if(NULL != pPlayer)
		{
			if(CanOpt(pPlayer))
			{
				//! 找到该物品
				if(NULL != pPlayer->FindGoodsByGuidInPack(GoodsGuid))
				{
					m_arrGoodsGuid[dwIdx] = GoodsGuid;

					//! 执行脚本
					pPlayer->PlayerRunScript(m_szOnPushScript);

					CMessage msg(MSG_S2C_GOODS_AllPurpose_RePush);
					DBWriteSet dbWriteSet;
					msg.GetDBWriteSet(dbWriteSet);
					dbWriteSet.AddToByteArray(dwIdx);
					dbWriteSet.AddToByteArray(GoodsGuid);
					msg.SendToPlayer(m_OwnerGuid, false);

					return eOR_Succeed;
				}
				else
				{
					return eOR_Failed;
				}
			}
		}
	}

	return eOR_Lawless;
}

//! 响应物品移除
GoodsAllPurposeOpt::eOptResult GoodsAllPurposeOpt::OnGoodsPop(DWORD dwIdx)
{
	if(m_dwMaxPushNum > dwIdx || 0xFFFFFFFF == dwIdx)
	{
		CPlayer *pPlayer = GetGame()->FindPlayer(m_OwnerGuid);
		if(NULL != pPlayer)
		{
			if(CanOpt(pPlayer))
			{
				if(0xFFFFFFFF == dwIdx)
					memset(m_arrGoodsGuid, 0, sizeof(CGUID) * MAX_ALLPURPOSE_CONTAINER_SIZE);
				else
					m_arrGoodsGuid[dwIdx] = NULL_GUID;
				CMessage msg(MSG_S2C_GOODS_AllPurpose_RePop);
				DBWriteSet dbWriteSet;
				msg.GetDBWriteSet(dbWriteSet);
				dbWriteSet.AddToByteArray(dwIdx);
				msg.SendToPlayer(m_OwnerGuid, false);

				CMessage msgTxt(MSG_S2C_GOODS_AllPurpose_UpdateTxt);
				DBWriteSet TxtdbWriteSet;
				msgTxt.GetDBWriteSet(TxtdbWriteSet);
				TxtdbWriteSet.AddToByteArray(m_szText);
				msgTxt.SendToPlayer(m_OwnerGuid, false);

				return eOR_Succeed;
			}
		}
	}

	return eOR_Lawless;
}

//! 响应开始物品操作
GoodsAllPurposeOpt::eOptResult GoodsAllPurposeOpt::FinishOpt(void)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(m_OwnerGuid);
	if(NULL != pPlayer)
	{
		if(CanOpt(pPlayer))
		{
			BOOL bPass = TRUE;
			//! 找到物品
			for (DWORD i = 0; i < m_dwMaxPushNum; ++i)
			{
				if(NULL == pPlayer->FindGoodsByGuidInPack(m_arrGoodsGuid[i]))
				{
					bPass = FALSE;
					break;
				}
			}
			
			if(bPass)
			{
				//! 执行脚本
				pPlayer->PlayerRunScript(m_szOnFinishScript);

				return eOR_Succeed;
			}
			else
			{
				memset(m_arrGoodsGuid, 0, sizeof(CGUID) * MAX_ALLPURPOSE_CONTAINER_SIZE);
				CMessage msg(MSG_S2C_GOODS_AllPurpose_RePop);
				DBWriteSet dbWriteSet;
				msg.GetDBWriteSet(dbWriteSet);
				dbWriteSet.AddToByteArray((DWORD)0xFFFFFFFF);
				msg.SendToPlayer(m_OwnerGuid, false);

				CMessage msgTxt(MSG_S2C_GOODS_AllPurpose_UpdateTxt);
				DBWriteSet TxtdbWriteSet;
				msgTxt.GetDBWriteSet(TxtdbWriteSet);
				TxtdbWriteSet.AddToByteArray(m_szText);
				msgTxt.SendToPlayer(m_OwnerGuid, false);

				return eOR_Failed;
			}
		}
	}

	return eOR_Lawless;
}

//! 刷新容器显示内容
GoodsAllPurposeOpt::eOptResult GoodsAllPurposeOpt::SetTxt(LPCSTR pTxt)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(m_OwnerGuid);
	if(NULL != pPlayer)
	{
		if(CanOpt(pPlayer))
		{
			if(NULL == pTxt)
				return eOR_Failed;
			else
			{
				CMessage msgTxt(MSG_S2C_GOODS_AllPurpose_UpdateTxt);
				DBWriteSet TxtdbWriteSet;
				msgTxt.GetDBWriteSet(TxtdbWriteSet);
				TxtdbWriteSet.AddToByteArray(pTxt);
				msgTxt.SendToPlayer(m_OwnerGuid, false);

				return eOR_Succeed;
			}
		}
	}

	return eOR_Lawless;
}

//! 刷新容器显示金钱数
GoodsAllPurposeOpt::eOptResult GoodsAllPurposeOpt::SetGold(DWORD dwGoldNum)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(m_OwnerGuid);
	if(NULL != pPlayer)
	{
		if(CanOpt(pPlayer))
		{
			CMessage msgTxt(MSG_S2C_GOODS_AllPurpose_UpdateGold);
			DBWriteSet TxtdbWriteSet;
			msgTxt.GetDBWriteSet(TxtdbWriteSet);
			TxtdbWriteSet.AddToByteArray(dwGoldNum);
			msgTxt.SendToPlayer(m_OwnerGuid, false);

			return eOR_Succeed;
		}
	}

	return eOR_Lawless;
}

//! 判断玩家操作的合法性
BOOL GoodsAllPurposeOpt::CanOpt(CPlayer *pPlayer)
{
	assert(NULL != pPlayer);

	if(m_RegionGuid != pPlayer->GetRegionGUID())
		return FALSE;

	if (abs(pPlayer->GetPosX() - m_lPosX) > m_lRadius || abs(pPlayer->GetPosY() - m_lPosY) > m_lRadius)
		return FALSE;

	return TRUE;
}