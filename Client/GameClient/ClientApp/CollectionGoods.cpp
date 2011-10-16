////////////////////////////////////////////////////////////////////////////
//CollectionGoods.cpp///////////////////////////////////////////////////////////////////
//采集类，此物体无生命和防御
//Author:肖杨
//Create Time:2008.2.3
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\CollectionGoods.h"
#include ".\Other\DeputyOccuSystemClient.h"
#include "../../EngineExtend\DisplayModel.h"
#include "../../EngineExtend\GameModelManager.h"
#include "../../../Public/Common/Public.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CGameModelManager;

CCollectionGoods::CCollectionGoods()
{
	m_lCreatorType = 0;			// 创建关联者的类型
	m_CreatorID = CGUID::GUID_INVALID;
	//m_dwCollectionGoodsID = 0;
	//m_bCanBeCollect = false;
	//m_strCollectionGoodsShowName = "";		//采集物的显示名字
	//m_strCollectionGoodsOriName = "";		//采集物的原始名字
	//m_eOccuID = DOccu_Num;					//副职业id
	//m_dwPicID = 0;							//图形id
	//m_dwRadarID = 0;						//小地图标志(雷达id)
	//m_dwMixTimes = 0;						//最小采集次数
	//m_dwMaxTimes = 0;						//最大采集次数
	//m_dwCollectLevel = 0;					//采集等级
	//m_dwRoleID = 0;							//需要任务ID
	//m_dwRoleStep = 0;						//需要任务步骤
}

//设置采集物的状态
void CCollectionGoods::SetAction(WORD l)
{

	if( m_wAction != l )
	{
		m_wAction = l;
		OnActionChange();
	}
}



/*
 * 功能: 初始化显示模型
 * 摘要: -
 * 参数: bAsyn - 是否同步显示
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.10.27 - lpf
 *		采集物对象设定显示模型为同步显示(修正第一次进游戏,由于采集物可随机设定等级,而造成了无法显示模型问题);
 */
void CCollectionGoods::SetDisplayModelGroup(bool bAsyn)
{
	if (this->GetDisplayModel()!=NULL)
	{
		this->GetDisplayModel()->ShowGroup(MAKEFOURCC('0','0','0','0'),MAKEFOURCC('m','e','s','h'),0xffffffff,false);
	}
}

////设置等级
//void CCollectionGoods::SetModelLevel()
//{
//	if (this->GetDisplayModel()!=NULL)
//	{
//		DWORD dwGroupName;
//		DWORD dwPartName;
//		DWORD dwLevelNum;
//		DWORD dwLevel;
//		CDisplayModel::CDisplayGroup* pGroup;
//		CDisplayModel::itDisplayGroup it = this->GetDisplayModel()->GetGroups().begin();
//		for (;it != this->GetDisplayModel()->GetGroups().end();it++ )
//		{
//			dwGroupName = (*it).first;
//			pGroup = (*it).second;
//			dwPartName = pGroup->GetPartName();
//			GameModel* pgamemodel =  CGameModelManager::GetInstance()->GetGameModel(this->GetDisplayModel()->GetGameModelID());
//			if(pgamemodel!=NULL)
//			{
//				std::map<DWORD,GameModel::Group*>::iterator GropIter = pgamemodel->GetGroupArray()->find(dwGroupName);
//				GameModel::Group* pgroup = GropIter->second;
//				if (pgroup!=NULL)
//				{
//					std::map<DWORD,GameModel::Group::Part*>::iterator PartIter = pgroup->GetPartArray()->find(dwPartName);
//					GameModel::Group::Part* pPart = PartIter->second;
//					if (pPart!=NULL)
//					{
//						dwLevelNum = pPart->GetLevelCount();
//						dwLevel = random((int)dwLevelNum);
//						this->GetDisplayModel()->ShowGroup(dwGroupName,dwPartName,dwLevel);
//					}
//				}
//			}
//		}
//	}
//}

//采集物的显示
bool CCollectionGoods::Display(void)
{
	CMoveShape::Display();
	return true;
}


//切换采集物的状态
void CCollectionGoods::OnActionChange()
{
	static char str[32];

	AnimInfo::tagActionInfo *pActionInfo = m_pAnimInfo->GetActionInfo();
	//---------------------------------------------
	pActionInfo->dwCurActionStartTime = GetCurTickCount();
	pActionInfo->dwCurActionLoopCount = 0;

	if (m_wAction == CShape::ACT_STAND || m_wAction == CShape::ACT_INTONATE
		|| m_wAction == CShape::ACT_WALK || m_wAction == CShape::ACT_RUN 
		|| m_wAction == CShape::ACT_MEGRIM || m_wAction == CShape::ACT_ALREADYDIED )
	{
		pActionInfo->bCurActionLooped = TRUE;
	}
	else
	{
		pActionInfo->bCurActionLooped = FALSE;
	}

	// 进入非站立动作
	if (m_wAction!=ACT_STAND)
	{
		m_lIdleCount = 0;
	}

	static DWORD dwSTND = MAKEFOURCC('S','T','N','D');
	static DWORD dwDIED = MAKEFOURCC('D','I','E','D');
	//static DWORD dwOPEN = MAKEFOURCC('O','P','E','N');
	static DWORD dwHURT = MAKEFOURCC('H','U','R','T');
	static DWORD dwDIE1 = MAKEFOURCC('D','I','E','1');

	switch (m_wAction)
	{
	case CShape::ACT_STAND:
		pActionInfo->dwCurAction = dwSTND;
		break;

	case CShape::ACT_DIED:
		pActionInfo->dwCurAction= dwDIED;
		break;

	// 王峰：已经死亡
	case ACT_ALREADYDIED:
			pActionInfo->dwCurAction = dwDIE1;
		break;

	case CShape::ACT_BEATTACK:
		pActionInfo->dwCurAction= dwHURT;
		break;

	}
}

// 解码
bool CCollectionGoods::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CMoveShape::DecordFromByteArray(pByte, pos, bExData);
	// 任务保护信息(类型和GUID)
	m_lCreatorType = _GetLongFromByteArray(pByte,pos);
	_GetBufferFromByteArray(pByte,pos,m_CreatorID);
	return true;
}

bool CCollectionGoods::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CMoveShape::DecodeFromDataBlock(setReadDB, bExData);

	// 任务保护信息(类型和GUID)
	m_lCreatorType = setReadDB.GetLongFromByteArray();
	setReadDB.GetBufferFromByteArray(m_CreatorID);
	m_dwCollectID		= setReadDB.GetDwordFromByteArray();
	if (m_dwCollectID!=0)
	{
		const CCollectionLayout* CollectLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
		if (CollectLayout!=NULL)
		{
			m_strCollectionName = CollectLayout->GetCollectionName();
		}
	}
	m_bIsBeenCollect        = (setReadDB.GetByteFromByteArray() == 0)?false : true;
	m_bCanBeenCollect		= (setReadDB.GetByteFromByteArray() == 0)?false : true;
	return true;
}

//采集物的AI
void CCollectionGoods::AI()
{
	CShape::AI();

	switch( GetAction() )
	{
	case ACT_STAND:
	case ACT_IDLE:
		if( AI_Stand() == RET_AI_BREAK ) return;
		break;
	case ACT_BEATTACK:
		if( AI_BeAttack() == RET_AI_BREAK ) return;
		break;
	case ACT_DIED:
		if( AI_Died() == RET_AI_BREAK ) return;
		break;
	}
}

//获取物品的显示名字
string CCollectionGoods::GetColletctionGoodsShowName()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetCollectionName();	//采集物的显示名字
	}
	return "";
}

//获取物品的原始名
string CCollectionGoods::GetCollectionGoodsOriName()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetOrginName();	//采集物的原始名字
	}
	return "";
}

//获取副职业的ID
DWORD  CCollectionGoods::GetOccuID()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetOccuID();		//副职业id
	}
	return 0;
}

//获取采集物显示模型的ID
DWORD  CCollectionGoods::GetPicID()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetPicID();		//模型id
	}
	return 0;
}

//获取雷达地图的ID
DWORD  CCollectionGoods::GetRadarID()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetRadarID();		//小地图标志(雷达id)
	}
	return 0;
}

//获取最小可被采集的次数
DWORD  CCollectionGoods::GetMixTimes()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{	
		return pCollectionLayout->GetMinTimes();	//最小采集次数
	}
	return 0;
}

//获取最大可被采集的次数
DWORD  CCollectionGoods::GetMaxTimes()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetMaxTime();	//最大采集次数
	}
	return 0;
}

//获取采集物要求的采集等级
DWORD  CCollectionGoods::GetCollectLevel()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetCollectLevel();	//采集等级
	}
	return 0;
}

//获取任务ID
int  CCollectionGoods::GetRoleID()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetRoleID();   //需要任务ID
	}
	return 0;
}

//获取任务ID列表
const vector<tagQuestGoods>* CCollectionGoods::GetRoleInfo()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return &(pCollectionLayout->GetRoleInfo());
	}
	return NULL;
}

//获取采集物的被采集的条件列表
const list<tgConditionGoods>* CCollectionGoods::GetConditionList()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return &(pCollectionLayout->GetConditionList());
	}
	return NULL;
}

//获取任务阶段
DWORD  CCollectionGoods::GetRoleStep()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetRoleStep();//需要任务步骤
	}
	return 0;
}

///获取采集此采集物时使用的动作ID
DWORD  CCollectionGoods::GetActionID()
{
	const CCollectionLayout* pCollectionLayout = CDeputyOccuSystemClient::GetCollectionLayout(m_dwCollectID);
	if (pCollectionLayout!=NULL)
	{
		return pCollectionLayout->GetActionID();
	}
	return 0;
}