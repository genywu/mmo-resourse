#include "stdafx.h"
#include "Pet.h"
#include "Player.h"
#include "ClientRegion.h"
#include "../GameClient/GameControl.h"
#include "Skills/SkillXml.h"
#include "../GameClient/Game.h"
#include "Other/GlobeSetup.h"
#include "../../Input/KeyBoard.h"
#include "../../../UI/UIDef.h"

const float CPet::FAR_FROM_TARGET_DISTANCE = 1.0f;
vector<CPet::tagStandInfo>	CPet::vecPetStandDirPos;
CPet::CPet()
{
	m_tagPetDiff = new tagPetDiff;			//宠物属性数据
	m_tagPetDetail = new tagPetTemp;		//宠物逻辑操作
	
	m_bIsActived = false;		//是否被激活

	m_lType = TYPE_PET;
	m_lDistanceFromTarget = 0;

	m_MoveMsgNum = 0;
	m_dwStopMoveMsgNum = 0;
	m_dwSkillMsgSendNum = 0;
	m_dwSkillSuccMsgRecvNum = 0;
	m_dwSkillFailMsgRecvNum = 0;

	m_lStartDestX = 0;
	m_lStartDestY = 0;

	m_lLastPosX = 0;
	m_lLastPosY = 0;
	m_lLastDestX = 0;
	m_lLastDestY = 0;
	m_pCurAttackShape = 0;
	m_pPassiveAtkTarget = 0;

	m_lStayModePosX = 0.0f;	//目标的上一次X位置
	m_lStayModePosY = 0.0f;

	m_bPassiveForceToFight = false;
	m_bPetIsRetaliateUponNextShape = false;
	m_dwSkillPublicBeginCoolTime = 0;
	m_lCurUseSkillPos = 0;
	m_lLastUseSkillPos = 0;
	dwFightBackMsgNum = 0;
	m_bPetSkillBechanged = false;
	m_bIsInBornState = true;
	m_PetType = PET_TYPE_NULL;

	if (vecPetStandDirPos.empty())
	{
		for (long i= CShape::DIR_UP; i<=CShape::DIR_LEFTUP; ++i)
		{
			tagStandInfo	sInfo;
			sInfo.bOccupy = false;
			sInfo.eDirPos = (CShape::eDIR)i;
			vecPetStandDirPos.push_back(sInfo);
		}
	}
    m_FloatMove = new CFloatMove;
}

CPet::~CPet()
{
	SAFE_DELETE(m_tagPetDiff);
	SAFE_DELETE(m_tagPetDetail);
    SAFE_DELETE(m_FloatMove);
}

/// 解码激活宠物属性
bool CPet::DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData)
{
	bool ret = CMoveShape::DecodeFromDataBlock(setReadDB, bExData);
	setReadDB.GetBufferFromByteArray(m_MasterGUID);

	if (!ret)
	{
		assert(0);//base pet decode failed
		return false;
	}
	else
	{
		if(bExData)
		{
			long lNum = setReadDB.GetLongFromByteArray();
			tagPetSkillInfo skillInfo;

			for (long i = 0;i<lNum;++i)
			{
				long lPos = setReadDB.GetLongFromByteArray();
				map<long,tagPetSkillInfo>::iterator it = m_PetSkillMap.find(lPos);
				if (it == m_PetSkillMap.end())
					return false;

				tagPetSkillInfo& skillInfo = it->second;
				skillInfo.lRestorTimeLength = setReadDB.GetLongFromByteArray();
                // 技能范围改为float TODO:修改客户端技能范围相关逻辑
				skillInfo.lMinAtkDistance	= (long)setReadDB.GetFloatFromByteArray();
				skillInfo.lMaxAtkDistance	= (long)setReadDB.GetFloatFromByteArray();
				skillInfo.lCostMp			= setReadDB.GetLongFromByteArray();
				skillInfo.lStartCoolTime	= setReadDB.GetLongFromByteArray();
			}
			//m_lStayModePosX = m_Host->GetTileX();
			//m_lStayModePosY = m_Host->GetTileY();
			m_lStayModePosX = m_Host->GetPosX();
			m_lStayModePosY = m_Host->GetPosY();
		}
		else
		{
			long lPetType = setReadDB.GetLongFromByteArray();
			SetPetType((PET_TYPE)lPetType);
			long lLevel = setReadDB.GetLongFromByteArray();
			SetPetLevel(lLevel);
			long lHp    = setReadDB.GetLongFromByteArray();
			SetHp(lHp);
			long lMp = setReadDB.GetLongFromByteArray();
			SetMp(lMp);
			long lEnergy = setReadDB.GetLongFromByteArray();
			SetEnergy(lEnergy);
			long lMaxHp	= setReadDB.GetLongFromByteArray();
			SetMaxHp(lMaxHp);
			long lMaxMp	= setReadDB.GetLongFromByteArray();
			SetMaxMp(lMaxMp);
			long lMaxEnergy	= setReadDB.GetLongFromByteArray();
			SetMaxEnergy(lMaxEnergy);
		}
	}
	return ret;
}

/// 解码宠物属性
bool CPet::DecodeAttrFromDataBlock(DBReadSet& setReadDB)
{
	setReadDB.GetBufferFromByteArray(m_tagPetDiff, sizeof(tagPetDiff));
	setReadDB.GetBufferFromByteArray(m_tagPetDetail, sizeof(tagPetTemp));
	long lNum = setReadDB.GetLongFromByteArray();
	tagPetSkillInfo skillInfo;

	for (long i = 0;i<lNum;++i)
	{
		setReadDB.GetBufferFromByteArray(&skillInfo,sizeof(tagSkill));
		m_PetSkillMap[skillInfo.lPos] = skillInfo;
	}
	SetPetType((PET_TYPE)m_tagPetDetail->lPetType);
	return true;
}

long CPet::AutoMoveAI_A(float fDestX,float fDestY)				//自动寻路到目标
{
	CClientRegion	*pRegion = GetGame()->GetRegion();
	if(!pRegion)
		return 0;

	m_FindPath.SetRegion(pRegion);

	if (abs(m_fPosX) <= 0.001 && abs(m_fPosY) <= 0.001)
	{
		return 0;
	}
    //如果是移动状态
    if (GetMaster()->GetAction() == ACT_RUN ||
        GetMaster()->GetAction() == ACT_WALK)
    {
        SetIsArrive(false);
        if (abs(m_fPosX - fDestX) >  FAR_FROM_TARGET_DISTANCE||
            abs(m_fPosY - fDestY) > FAR_FROM_TARGET_DISTANCE)
        {
            m_FindPath.GetPathList()->clear();
            ContinueMoveTo(fDestX,fDestY);
        }
    }
    else
    {
        if(abs(m_fPosX - fDestX) > m_FloatMove->MINIMUS_OP_DISTANCE ||
            abs(m_fPosY - fDestY) > m_FloatMove->MINIMUS_OP_DISTANCE)
        {
            SetIsArrive(false);
            m_FindPath.GetPathList()->clear();
            ContinueMoveTo(fDestX,fDestY);
        }
    }
    
    //更新当前转向移动点表
    if (IsArrive())
    {
        CFloatMove::tagMovPos CurDestPoint;
        if(!m_FloatMove->LineMoveList.empty())
        {
            CurDestPoint.x = m_FloatMove->LineMoveList.begin()->x;
            CurDestPoint.y = m_FloatMove->LineMoveList.begin()->y;
            MoveToCell(CurDestPoint.x,CurDestPoint.y);    
            m_FloatMove->LineMoveList.pop_front();
            m_FloatMove->bIsPreviousMoveByA_Star = false;
        }
        else
        {
            if(!m_FloatMove->A_MoveList.empty())
            {
                if(m_FloatMove->bIsPreviousMoveByA_Star)
                    m_FloatMove->A_MoveList.pop_front();
                if(!m_FloatMove->A_MoveList.empty())
                {
                    CurDestPoint.x = m_FloatMove->A_MoveList.begin()->x;
                    CurDestPoint.y = m_FloatMove->A_MoveList.begin()->y;
                    m_FloatMove->bIsPreviousMoveByA_Star = true;
                    m_FloatMove->UpdatelinearMoveList(m_fPosX,m_fPosY,CurDestPoint.x,CurDestPoint.y);
                    if (!m_FloatMove->LineMoveList.empty())
                    {
                        CurDestPoint.x = m_FloatMove->LineMoveList.begin()->x;
                        CurDestPoint.y = m_FloatMove->LineMoveList.begin()->y;
                        m_FloatMove->LineMoveList.pop_front();
                        m_FloatMove->bIsPreviousMoveByA_Star = false;
                    }
                    MoveToCell(CurDestPoint.x,CurDestPoint.y);
                }
            }
            else
            {
                //到达目标 停止移动
                return 0;
            }
        }
    }
	return 1;
}

long	CPet::ContinueMoveTo(float fDestX, float fDestY)
{
	m_fDestX = fDestX;
	m_fDestY = fDestY;

	// 寻路
	m_FindPath.AddPathPos(GetGame()->GetRegion()->GetResourceID(),
        (long)m_fPosX, (long)m_fPosY, (long)m_fDestX, (long)m_fDestY,15,15);
	
	// 去掉第一个点,因为就是自己站的点
	size_t size = m_FindPath.GetPathList()->size();
	if(size > 0)
    {
        m_FindPath.GetPathList()->pop_front();
        CClientRegion *pRegion = GetGame()->GetRegion();
        if(pRegion)
            m_FloatMove->UpdateA_MoveList(pRegion->GetFindPath(),
            m_fPosX,m_fPosY,m_fDestX,m_fDestY);
        if (!m_FloatMove->A_MoveList.empty())
        {
            CFloatMove::tagMovPos firstStep;
            firstStep.x = m_FloatMove->A_MoveList.begin()->x;
            firstStep.y = m_FloatMove->A_MoveList.begin()->y;
            MoveToCell(firstStep.x, firstStep.y);
        }
    }
	if(size == 0)
    {
        m_FloatMove->A_MoveList.clear();
        return 0;
    }
	return 1;
}

void	CPet::MoveToCell(float fDestX, float fDestY)	//移动到指定格子
{
	if (GetAction() == ACT_PERFORM || GetAction() == ACT_ATTACK)
	{
		return;
	}
	float fRadianDir =  GetRadianDir(m_fPosX,m_fPosY,fDestX,fDestY);
	SetDirEx(fRadianDir);
	SetDestXY(fDestX,fDestY);
	SetIsArrive(false);

	//得出移动时间
	float fTotalDistance = RealDistance(fDestX,fDestY);
	float fMoveTime = fTotalDistance/GetSpeed();
	DWORD dwCurTime = GetCurTickCount();
	SetEndMoveTime(dwCurTime+(DWORD)fMoveTime);
    SetMoveOffset((fDestX-m_fPosX)/fMoveTime, (fDestY-m_fPosY)/fMoveTime);
	if(m_wAction != CShape::ACT_WALK &&
		m_wAction != CShape::ACT_RUN)
	{
		//如果现在的动作不是行走动作,重置上次的更新时间
		SetLastUpdatePosTime(dwCurTime);
	}

	WORD action = CShape::ACT_WALK;
	if(GetIsRun())
		action = CShape::ACT_RUN;
	if(m_wAction != action)
	{
		SetAction(action);
	}

	if((m_lLastPosX != (long)m_fPosX || m_lLastPosY != (long)m_fPosY ||
		m_lLastDestX != (long)fDestX || m_lLastDestY != (long)fDestY
		) && GetPetState() == PET_STATE_FIGHT)
	{
		m_lLastPosX = (long)m_fPosX;m_lLastPosY = (long)m_fPosY;
		m_lLastDestX = (long)fDestX; m_lLastDestY = (long)fDestY;
		m_MoveMsgNum++;
		CMessage msg(MSG_C2S_PET_MOVE);
		msg.Add(GetExID());
		msg.Add(m_fPosX);
		msg.Add(m_fPosY);
		msg.Add(fDestX);
		msg.Add(fDestY);
		msg.Send();
	}
}
void	CPet::PetStopMove(bool Flag)
{
	m_FindPath.GetPathList()->clear();
	++m_dwStopMoveMsgNum;
	SetEndMoveTime(GetCurTickCount());
    sprintf_s<256>(strMoveInfo,"宠物停止移动 当前位置X:%f Y:%f\n",m_fPosX,m_fPosY);
    OutputDebugString(strMoveInfo);

    sprintf_s<256>(strMoveInfo,"---------------------------------\n");
    OutputDebugString(strMoveInfo);
	if(!Flag)
        return;
    CMessage msg(MSG_C2S_PET_STOP);
	msg.Add(GetExID());
	msg.Add(m_fPosX);
	msg.Add(m_fPosY);
	msg.Add((short)GetDir());
	msg.Send();
}
bool CPet::Display()
{	
	if (!CMoveShape::Display())
		return false;
	return true;
}
long	CPet::FollowObjectAI(long lDx , long lDy)
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	long lSrcX = GetTileX();
	long lSrcY = GetTileY();
	long lDestX = lDx;
	long lDestY = lDy;

	// 和目标距离已经在最大允许范围之内，跟踪结束
	if ( abs(lSrcX-lDestX) <= FAR_FROM_TARGET_DISTANCE
		&& abs(lSrcY-lDestY) <= FAR_FROM_TARGET_DISTANCE )
	{
		return 0;
	}
	long lDir = GetLineDir(lSrcX, lSrcY, lDestX, lDestY);
	pRegion->GetLeastShapesCellFromSpace(lDestX, lDestY, 0, 0,
		1, lDir,lDestX,lDestY);
	m_fDestX = lDestX + 0.5f;
	m_fDestY = lDestY + 0.5f;

	//如果目标点有Shape,并且和Shape之间的距离为1是就停止移动
	// 到达目标坐标，停止自动移动
	if ( abs(lSrcX-(long)m_fDestX) < 1 && abs(lSrcY-(long)m_fDestY) < 1 )
	{
		return 0;
	}
	float fDestX = m_fPosX;
	float fDestY = m_fPosY;
	if(GetNextMoveDestCell(m_fDestX,m_fDestY,2,fDestX,fDestY))
	{
		MoveToCell(fDestX, fDestY);
		return 1;
	}
	return -1;
}

long CPet::FollowObjectAI(CShape *pTarget)
{
	if (pTarget)
	{
		CClientRegion *pRegion = GetGame()->GetRegion();
		long lSrcX = GetTileX();
		long lSrcY = GetTileY();
		long lDestX = pTarget->GetTileX();
		long lDestY = pTarget->GetTileY();
		if( m_lStartDestX != lDestX || m_lStartDestY != lDestY )
		{
			// 和目标距离已经在最大允许范围之内，跟踪结束
			if ( abs(lSrcX-lDestX) <= FAR_FROM_TARGET_DISTANCE
				&& abs(lSrcY-lDestY) <= FAR_FROM_TARGET_DISTANCE )
			{
				return 0;
			}
			long lDir = GetLineDir(lSrcX, lSrcY, lDestX, lDestY);
			pRegion->GetLeastShapesCellFromSpace(lDestX, lDestY, 0, 0,
				1, lDir,lDestX,lDestY);
			m_fDestX = lDestX + 0.5f;
			m_fDestY = lDestY + 0.5f;
			//m_lStartDestX = pTarget->GetTileX();
			//m_lStartDestY = pTarget->GetTileY();
		}

		//如果目标点有Shape,并且和Shape之间的距离为1是就停止移动
		// 到达目标坐标，停止自动移动
		if ( abs(lSrcX-(long)m_fDestX) < 1 && abs(lSrcY-(long)m_fDestY) < 1 )
		{
			return 0;
		}
		float fDestX = m_fPosX;
		float fDestY = m_fPosY;
		if(GetNextMoveDestCell(m_fDestX,m_fDestY,2,fDestX,fDestY))
		{
			MoveToCell(fDestX, fDestY);
			return 1;
		}
	}
	return -1;
}

// 添加移动过的格子
//返回值:false:表示已经存在,true:不存在该格子
bool	CPet::AddMovedCell(long lTileX,long lTileY)
{
    //return TRUE;
    static long lLastAddX = 0;
    static long lLastAddY = 0;
    if(lLastAddX == lTileX && lLastAddY == lTileY)
    {
        return true;
    }
    lLastAddX = lTileX;lLastAddY=lTileY;
    POINT point = {lTileX,lTileY};
    itMovedCell it = m_MovedCell.find(point);
    if(it != m_MovedCell.end())
    {
        if( (*it).second >= 2)
            return false;
        (*it).second++;
    }
    else
    {
        m_MovedCell[point]=1;
    }
    return true;
}
long	CPet::GetNextMoveDestCell(float fX,float fY,long lMaxDistance,float &fDestX,float &fDestY)
{
	float fDir = CShape::GetLineDirF(fDestX, fDestY, fX, fY);
	float fDistance = RealDistance(fX,fY);
	if( 0.0f==fDistance )
		return FALSE;
	if( fDistance > lMaxDistance )
	{
		fDistance = (float)lMaxDistance;
	}
	return GetNextMoveDestCell(fDir, fDistance, fDestX, fDestY);
}

long	CPet::GetNextMoveDestCell(float fRadianDir,float fMaxDistance,float &fDestX,float &fDestY)
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	if(pRegion == NULL)	return 0;
	//滑动的8个方向加减值
	static int SlipOrder[8] = {0,1,-1,2,-2,3,-3,4};
	//随机的初始滑动正负方向
	static int SlipRandom[] = {1,-1};

	//在初始方向上移动到阻挡为止
	float fCount = 0.0f;
	float fEndX = fDestX,fEndY = fDestY;
	int nBlockNum = 0;
	while(fCount <= fMaxDistance)
	{
		float fx = fDestX + fCount * cosf(fRadianDir);
		float fy = fDestY + fCount * sinf(fRadianDir);
		if( pRegion->GetBlock((long)fx, (long)fy) != CRegion::BLOCK_NO && 
			pRegion->GetBlock((long)fx, (long)fy) != CRegion::BLOCK_AIM)
			nBlockNum++;
		else
		{
			nBlockNum=0;
			fEndX = fx;fEndY=fy;
		}
		//连续有5个点存在阻挡，则表示不能移动通过
		if(nBlockNum >= 1)
		{
			break;
		}
		fCount += 0.1f;
	}

	if( nBlockNum < 1 && ( (long)fDestX != (long)fEndX || (long)fDestY != (long)fEndY ))
	{
		fDestX = fEndX;
		fDestY = fEndY;
		if(pRegion->GetBlock((long)fDestX, (long)fDestY) == CRegion::BLOCK_UNFLY)
		{
			int x = 8;
		}
		return TRUE;
	}

	//没有移动成功，则在产生一格的滑动	
	long nRandom=random(2);
	long lDir = CShape::RadianDirTo8Dir(fRadianDir);
	int i=0;
	long lDestX = 0;
	long lDextY = 0;
	for(; i<8; i++)
	{
		long lMoveDir = lDir+SlipOrder[i]*SlipRandom[nRandom];
		if(lMoveDir>7)	lMoveDir = lMoveDir%8;
		else if(lMoveDir<0) lMoveDir = lMoveDir+8;
		lDestX = (long)fDestX + _dir[lMoveDir][0];
		lDextY = (long)fDestY + _dir[lMoveDir][1];
		if( pRegion->GetBlock(lDestX, lDextY) == CRegion::BLOCK_NO ||
			pRegion->GetBlock(lDestX, lDextY) == CRegion::BLOCK_AIM)
		{
			break;
		}
	}
	if(i < 8)
	{
		fDestX = (float)lDestX+0.5f;
		fDestY = (float)lDextY+0.5f;
		return TRUE;
	}
	return FALSE;
}

void CPet::OnDied(void)
{
	SetState(CShape::STATE_DIED);
	m_bIsActived = false;
	//SetIsDeleted(true);
	//CAlphaFadeObjectList & ObjectList = GetGame()->GetRegion()->GetAlphaFadeObjectList();
	//ObjectList.AddFadeOutObject(this);
	if (m_MasterGUID == GetGame()->GetMainPlayer()->GetExID() /*&& m_PetType != PET_TYPE_TRAP*/)
	{
		//更新血条
		//CPetNameHpPageEx	*pPetHpBarPage = GetGame()->GetCGuiEx()->GetPetNamHpPage();
		//if(pPetHpBarPage)
		//{
		//	pPetHpBarPage->RemoveListBar(m_PetType);
		//	pPetHpBarPage->AutoSortPetNameBars();
		//}
		//操作界面打开或关闭
		GetGame()->GetMainPlayer()->ValidatePetInterface();
	}
}

void CPet::UpdateProperty(CMessage *pMsg)
{
	DBReadSet setReadDB;
	pMsg->GetDBReadSet(setReadDB);
	DecodeChangePropertyFromDataBlock(setReadDB);
}

void CPet::DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB)
{
	WORD wNum = setReadDB.GetWordFromByteArray();
	WORD wAttrType = 0;
	BYTE bData_Type = 0;

	long lData = 0;

	for (int i = 0;i<wNum;i++)
	{	
		wAttrType = setReadDB.GetWordFromByteArray();
		bData_Type = setReadDB.GetByteFromByteArray();

		//根据数据类型获取具体的数据值
		switch(bData_Type)
		{
		case ATTR_DATA_BYTE:
			lData = setReadDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_WORD:
			lData = setReadDB.GetWordFromByteArray();
			break;
		case ATTR_DATA_DWORD:
			lData = setReadDB.GetDwordFromByteArray();
			break;
		case ATTR_DATA_FLOAT:
			lData = (long)setReadDB.GetFloatFromByteArray();
			break;
		case ATTR_DATA_LONG:
			lData = setReadDB.GetLongFromByteArray();
			break;
		case ATTR_DATA_SHORT:
			lData = setReadDB.GetShortFromByteArray();
			break;
		case ATTR_DATA_BOOL:
			lData = setReadDB.GetByteFromByteArray();
			break;

		default:
			break;
		}						

		//设置相应属性值
		SetPetAttibute(wAttrType,lData);
	}
}

void	CPet::SetPetAttibute(WORD wAttrType,long lData)
{
	switch (wAttrType)
	{
	case PET_ATTR_Hp:					// HP
		m_tagPetDiff->lHp = lData;
		break;
	case PET_ATTR_MaxHp:				// HP上限
		m_tagPetDetail->lMaxHp = lData;
		break;
	case PET_ATTR_Mp:					// MP
		m_tagPetDiff->lMp = lData;
		break;
	case PET_ATTR_MaxMp:				// MP上限
		m_tagPetDetail->lMaxMp = lData;
		break;
	case PET_ATTR_Energy:				// 内力
		m_tagPetDiff->lEnergy = lData;
		break;
	case PET_ATTR_MaxEnergy:			// 内力上限
		m_tagPetDetail->lMaxEnergy = lData;
		break;
	case PET_ATTR_Life:					// 寿命
		m_tagPetDiff->lLife = lData;
		break;
	case PET_ATTR_Loyalty:				// 忠诚度
		m_tagPetDiff->lLoyalty = lData;
		break;
	case PET_ATTR_Exp:					// 经验值
		m_tagPetDiff->lExp = lData;
		break;
	case PET_ATTR_MaxExp:				// 经验值上限
		m_tagPetDetail->lMaxExp = lData;
		break;
	case PET_ATTR_Level:				// 等级
		m_tagPetDiff->lLevel = lData;
		break;
	case PET_ATTR_DExp:					// 修为经验
		m_tagPetDiff->ServantAttr.lDExp = lData;
		break;
	case PET_ATTR_MaxDExp:				// 修为经验上限
		m_tagPetDetail->lMaxDExp = lData;
		break;
	case PET_ATTR_DLevel:				// 道行等级
		m_tagPetDiff->ServantAttr.lDLevel = lData;
		break;
	case PET_ATTR_Strenth:
		m_tagPetDetail->lStrenth = lData;
		break;
	case PET_ATTR_Dexterity:
		m_tagPetDetail->lDexterity = lData;
		break;
	case PET_ATTR_Con:
		m_tagPetDetail->lCon = lData;
		break;
	case PET_ATTR_Intellect:
		m_tagPetDetail->lIntellect = lData;
		break;
	case PET_ATTR_Spiritualism:
		m_tagPetDetail->lSpiritualism = lData;
		break;

	case PET_ATTR_Hit:
		m_tagPetDetail->lHit = lData;
		break;
	case PET_ATTR_Dodge:
		m_tagPetDetail->lDodge = lData;
		break;
	case PET_ATTR_MAtkSpeed:
		m_tagPetDetail->lMAtkSpeed = lData;
		break;
	case PET_ATTR_PhysicsAtk:
		m_tagPetDetail->lPhysicsAtk = lData;
		break;
	case PET_ATTR_PhysicsDef:
		m_tagPetDetail->lPhysicsDef = lData;
		break;
	case PET_ATTR_MAtk:
		m_tagPetDetail->lMAtk = lData;
		break;
	case PET_ATTR_MDef:
		m_tagPetDetail->lMDef = lData;
		break;
	case PET_ATTR_ElemDef0:
		m_tagPetDetail->lElemDef0 = lData;
		break;
	case PET_ATTR_ElemDef1:
		m_tagPetDetail->lElemDef1 = lData;
		break;
	case PET_ATTR_ElemDef2:
		m_tagPetDetail->lElemDef2 = lData;
		break;
	case PET_ATTR_ElemDef3:
		m_tagPetDetail->lElemDef3 = lData;
		break;
	case PET_ATTR_ElemDef4:
		m_tagPetDetail->lElemDef4 = lData;
		break;

	case PET_ATTR_Race:					// 种族
		m_tagPetDetail->lRace = lData;
		break;
	case PET_ATTR_TakeLev:				// 携带等级
		m_tagPetDetail->lTakeLev = lData;
		break;
	case PET_ATTR_StateType:			// 状态类型
		{

		}
		break;
	case PET_ATTR_FightType:			// 战斗模式
		{
			if (lData == PET_FIGHETYPE_DEFEND && m_pCurAttackShape )
			{
				UpdateTargetPos(GetGame()->GetMainPlayer());	// 设置跟随目标是主人
				m_pCurAttackShape = 0;
				m_pPassiveAtkTarget = 0;
			}
			m_tagPetDetail->lFightType = lData;
		}
		break;
	case PET_ATTR_MoveType:				// 移动模式
		{
			m_tagPetDetail->lMoveType = lData;
			if (lData == PET_MOVETYPE_STOP)
			{
				//m_lStayModePosX = GetTileX();
				//m_lStayModePosY = GetTileY();
				m_lStayModePosX = GetPosX();
				m_lStayModePosY = GetPosY();
			}
			else if (lData == PET_MOVETYPE_FOLLOW && m_pCurAttackShape)
			{
				UpdateTargetPos(GetGame()->GetMainPlayer());	// 设置跟随目标是主人
				m_pCurAttackShape = 0;
				m_pPassiveAtkTarget = 0;
			}
		}
		break;
	default:
		break;
	}
	if (IsMainPlayerPet())
	{
		// 更新界面 @todo 王晓轩
		//CPetPageEx *pPetPage = GetGame()->GetCGuiEx()->GetPetPageEx();
		//CPetNameHpPageEx	*pPetHpBarPage = GetGame()->GetCGuiEx()->GetPetNamHpPage();
		//CPetHotKeyBarPageEx	*pPetHotKeyBar = GetGame()->GetCGuiEx()->GetPetHotKeyBarPage();
		//if(pPetHpBarPage && (wAttrType == PET_ATTR_Hp || 
		//	wAttrType == PET_ATTR_MaxHp ||
		//	wAttrType == PET_ATTR_Mp ||
		//	wAttrType == PET_ATTR_MaxMp ||
		//	wAttrType == PET_ATTR_Level))
		//	pPetHpBarPage->UpdatePetListBar(m_PetType);//更新宠物血条界面
		//if (pPetHotKeyBar && (wAttrType == PET_ATTR_FightType ||
		//	wAttrType == PET_ATTR_MoveType	))
		//{
		//	pPetHotKeyBar->UpdateHotKeyChooseState();
		//}
		//if(pPetPage)
		//	pPetPage->UpdateProperty(this);
	}
}

bool CPet::IsMainPlayerPet()
{
	if (m_MasterGUID == GetGame()->GetMainPlayer()->GetExID())
		return true;
	else
		return false;
}

void	CPet::EnterRegion(bool isSelf)				//进入场景
{
	CPlayer *pMaster =  dynamic_cast<CPlayer*>(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER, m_MasterGUID));
	float fPosX = 0;
	float fPosY = 0;
	if (pMaster)
	{
		fPosX = pMaster->GetPosX();
		fPosY = pMaster->GetPosY();
		// 加载宠物Mini界面放在设置宠物状态处
		SetPosXY(fPosX,fPosY);
		SetIsNeedAlphaFade(true);
		SetAlphaFadeInTimeLength(CGlobeSetup::GetSetup()->dwMonsterAlphaFadeTimeLength);
		SetAlphaFadeMode(CAlphaFadeObjectList::ALPHA_FADE_IN);

		SetDisplayModel();
		SetDisplayModelGroup();
		SetEndMoveTime(GetCurTickCount());
		extern	bool g_bForceUpdate;
		g_bForceUpdate = true;
		m_FindPath.GetPathList()->clear();
	}
}



void CPet::AddSkill(tagPetSkillInfo &skill)	//添加技能
{
	long lPos = skill.lPos;
	m_PetSkillMap[skill.lPos] = skill;
}


tagPetSkillInfo * CPet::GetPetSkillInfoByPos(long lPos)
{
	map<long ,tagPetSkillInfo>::iterator IterSkill = m_PetSkillMap.find(lPos);
	if(IterSkill != m_PetSkillMap.end())
		return &(IterSkill->second);
	return NULL;
}

tagPetSkillInfo * CPet::GetPetSkillInfoByID(long lID)
{
	if (m_PetSkillMap.empty())
		return NULL;
	map<long ,tagPetSkillInfo>::iterator IterSkill = m_PetSkillMap.begin();
	for(;IterSkill != m_PetSkillMap.end();++IterSkill)
	{
		tagPetSkillInfo	*pSkillInfo = &(IterSkill->second);
		if (pSkillInfo->lID == lID)
		{
			return pSkillInfo;
		}
	}
	return NULL;
}

void	CPet::SetCurAttackShape(CGUID & guid)
{
	m_curAtkID_Not_Find = guid;
}

void	CPet::SetCurAttackShape(CMoveShape *pTarget)
{
	m_pCurAttackShape = pTarget;
}

void CPet::SetPetBehavior(PET_MOVE_STYLE  pms)
{
	m_tagPetDetail->lMoveType = pms;
	if (pms == PET_MOVETYPE_STOP)
	{
		//m_lStayModePosX = GetTileX();
		//m_lStayModePosY = GetTileY();
		m_lStayModePosX = GetPosX();
		m_lStayModePosY = GetPosY();

	}
	else if (pms == PET_MOVETYPE_FOLLOW)
	{
		UpdateTargetPos(GetGame()->GetMainPlayer());	// 设置跟随目标是主人
		m_pCurAttackShape = 0;
	}
	CMessage  msg(MSG_C2S_PET_CHANGE_MODE);
	msg.Add((long)m_PetType);
	msg.Add((long)pms);
	msg.Send();
}

void CPet::SetPetFightMode(PET_FIGHT_STYLE pfs)
{
	if (pfs == PET_FIGHETYPE_DEFEND && m_pCurAttackShape)
	{
		UpdateTargetPos(GetGame()->GetMainPlayer());	// 设置跟随目标是主人
		m_pCurAttackShape = 0;
	}
	//m_tagPetDetail->lFightType = pfs;
	CMessage  msg(MSG_C2S_PET_CHANGE_ACTION);
	msg.Add((long)m_PetType);
	msg.Add((long)pfs);
	msg.Send();
}

void CPet::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
	CMessage  msg(MSG_C2S_PET_CHANGE_SPEED);
	msg.Add((long)m_PetType);
	msg.Add(m_fSpeed);
	msg.Send();
}

//自动还击
void CPet::AutoRetaliateUponAI()
{
	//如果自己或者主人是战斗状态。  两者战斗状态 可能是被挨打导致 ，促使反击
	if (m_bPetIsRetaliateUponNextShape)
	{
		//如果不存在没死的攻击目标 则找当前选中的目标去反击（不论宠物或主人受到伤害时，会选中发起攻击的目标）
		if (!(m_pCurAttackShape && m_pCurAttackShape->IsDied() == false))
		{
			CShape *pAttckingShape = GetGame()->GetGameControl()->GetSelectedShape();
			if(pAttckingShape == NULL)	return;
			if (pAttckingShape->GetType() == TYPE_MONSTER
				|| pAttckingShape->GetType() == TYPE_PLAYER
				|| pAttckingShape->GetType() == TYPE_PET)
			{
				CMoveShape *pAtkShape = (CMoveShape*)pAttckingShape;
				if (pAtkShape->IsDied() == false)
				{
					m_pCurAttackShape = pAtkShape;
					UpdateTargetPos(m_pCurAttackShape);
				}
			}
		}
		m_bPetIsRetaliateUponNextShape = false;
	}
}

void	CPet::UpdateTargetPos(long lPosX,long lPosY)
{
	//更新自动更随
	//m_lStartDestX = lPosX;
	//m_lStartDestY = lPosY;
	m_fDestX = GetPosX() + 0.5f;
	m_fDestY = GetPosY() + 0.5f;
	float fTotalDistance = RealDistance(m_fDestX,m_fDestY);
	DWORD dwMoveTime = (DWORD)(fTotalDistance/GetSpeed());
	DWORD dwCurTime = GetCurTickCount();
	SetEndMoveTime(dwCurTime+dwMoveTime);
	//清空A*路径表
	m_FindPath.GetPathList()->clear();
}

void	CPet::UpdateTargetPos(CMoveShape *pTarget)
{
	//更新自动更随
	m_fDestX = GetTileX() + 0.5f;
	m_fDestY = GetTileY() + 0.5f;
	float fTotalDistance = RealDistance(m_fDestX,m_fDestY);
	DWORD dwMoveTime = (DWORD)(fTotalDistance/GetSpeed());
	DWORD dwCurTime = GetCurTickCount();
	SetEndMoveTime(dwCurTime+dwMoveTime);
	//清空A*路径表
	m_FindPath.GetPathList()->clear();

}

bool CPet::CheckAttackTargetIsValidate(CMoveShape * pAtkShape)		//检查攻击目标是否合法
{

	if(pAtkShape == NULL)	
		return false;
	if(pAtkShape->IsDied())					//如果死亡
		return false;
	if(pAtkShape == GetMaster())			//目标不是主人
		return false;
	if(pAtkShape == this)					//目标不是宠物自己
		return false;

	if (pAtkShape->GetType() == TYPE_PET)	//如果目标是一个宠物
	{
		CPet *pAtkPet = (CPet*)pAtkShape;
		if (pAtkPet->GetMasterGUID() == GetMaster()->GetExID())	//检查当前攻击宠物是否 属于自己主人的
			return false;
	}
	return true;
}

CPet::eFightModeRet	CPet::PetFightAI()
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	if (pRegion == NULL)
		return	FMR_Other;

	//先处理有攻击guid但没找到对应shape的情况
	if (m_pCurAttackShape == 0 && m_curAtkID_Not_Find != NULL_GUID)
	{
		CBaseObject *pAtkObj = pRegion->FindChildObject(m_curAtkID_Not_Find);
		if (pAtkObj && (pAtkObj->GetType() == TYPE_MONSTER || pAtkObj->GetType() == TYPE_PLAYER||
						pAtkObj->GetType() == TYPE_PET))
		{
			m_pCurAttackShape = (CMoveShape*)pAtkObj;
			m_curAtkID_Not_Find = NULL_GUID;
		}
	}
	//验证当前攻击目标是否是有效场景下目标
	if (m_pCurAttackShape && GetGame()->GetRegion()->FindChildObject(m_pCurAttackShape) == false)
	{
		m_pCurAttackShape = 0;
	}

	//如果当前目标死亡，自动回到主人身边
	if (m_pCurAttackShape && m_pCurAttackShape->IsDied() == true)
	{
		m_pCurAttackShape = 0;
		UpdateTargetPos(GetGame()->GetMainPlayer());
	}
	//攻击AI
	switch(m_tagPetDetail->lFightType)
	{
		// @todo 王晓轩
	//case PET_AS_PASSIVE:			//被动模式
	//	{
	//		if (m_bPassiveForceToFight)		// 玩家强制设置攻击目标 
	//		{
	//			m_pPassiveAtkTarget = m_pCurAttackShape;
	//			m_bPassiveForceToFight = false;
	//		}
	//		//如果场景上找不到目标 或者目标已经死亡 则重置为空
	//		if (GetGame()->GetRegion()->FindChildObject(m_pPassiveAtkTarget) == false || m_pPassiveAtkTarget->IsDied())
	//		{
	//			m_pPassiveAtkTarget = 0;
	//		}
	//		m_pCurAttackShape = m_pPassiveAtkTarget;
	//	}
	//	break;
	case PET_FIGHETYPE_DEFEND:				//防御模式
		{
			//自动反击
			AutoRetaliateUponAI();
		}	
		break;
	case PET_FIGHETYPE_FIGHT:				//主动模式
		{
			//自动反击
			AutoRetaliateUponAI();
		}
		break;
	default:
		break;
	}
	if (m_pCurAttackShape)
	{
		eFightModeRet fmr = SkillAI();
		return fmr;
	}
	else
		return	FMR_No_TARGET;
}
CPet::eFightModeRet	CPet::PetSkillIsCanRelease(tagPetSkillInfo *pSkillInfo)
{
	if (pSkillInfo == NULL)					
		return	FMR_NO_SKILL;

	if(pSkillInfo->bAuto == false && pSkillInfo->bIsUsingByHotKey == false)				//判断是自动释放技能
		return	FMR_SKILL_IS_NOT_AUTO;

	long	dwTimeElaps = GetCurTickCount() - pSkillInfo->lStartCoolTime;
	if ((long)dwTimeElaps < pSkillInfo->lRestorTimeLength )	//判断当前技能冷却时间到没
		return	FMR_SKILL_CD_NOT_ARRIVAL;

	if (m_tagPetDiff->lMp < pSkillInfo->lCostMp)			//所需要的魔法值是否够
		return	FMR_SKILL_NOT_ENOUGH_MP;

	//CSkillXml skill;
	/***********************************************************************/
	/* 赵航 fix */
	////CSkillXml skill;
	/***********************************************************************/
	ConcreteSkillComp SkillCom ;
	//SkillCom的内存交给了CSkillXml的析构来管理
	CSkillXml skill(&SkillCom); 
	long x = m_pCurAttackShape->GetTileX();
	long y = m_pCurAttackShape->GetTileY();

	if(skill.StepSend(pSkillInfo->lID ,pSkillInfo->lLevel,this,m_pCurAttackShape,x,y) == false)
		return FMR_SKILL_LEVEL_FAIL;

	// 释放条件满足
	long lRangMax = pSkillInfo->lMaxAtkDistance;
	long lRangMin = pSkillInfo->lMinAtkDistance;

	if((lRangMin >1 || lRangMax >= 2) && IsSkillBeBlcoked())	//判断技能是否被阻挡了
	{
		return  FMR_Trace_Far;
	}
	FLOAT fXOffset = m_fPosX - m_pCurAttackShape->GetPosX();
	FLOAT fYOffset = m_fPosY - m_pCurAttackShape->GetPosY();
	m_lDistanceFromTarget = (long)(sqrt( fYOffset * fYOffset + fXOffset * fXOffset ));

	if(m_lDistanceFromTarget < lRangMin)
	{
		return	FMR_Trace_Near;
	}
	else if(m_lDistanceFromTarget > lRangMax)
	{
		return  FMR_Trace_Far;//追踪攻击目标
	}
	else
	{
		if (IsPetActWithCurAction() == false)		//判断动作是否完成。
			return	FMR_SKILL_ACTION_NOT_FINISH;
		return	FMR_Attack;//攻击距离合法
	}
}
CPet::eFightModeRet CPet::SkillAI()
{
	if (m_PetSkillMap.empty())
		return FMR_NO_SKILL;
	
	if (m_lCurUseSkillPos >= (long)m_PetSkillMap.size())
		m_lCurUseSkillPos = 0;

	tagPetSkillInfo * pLastSkill = &m_PetSkillMap[m_lLastUseSkillPos];
	
	//if (pLastSkill && pLastSkill->bIsSkillUsingEnd == false)
	//	return	FMR_SKILL_NOT_FINISH;		//上一个技能尚未结束
	
	tagPetSkillInfo * pSkillInfo = &m_PetSkillMap[m_lCurUseSkillPos];
	
	FLOAT fXOffset = m_fPosX - m_pCurAttackShape->GetPosX();
	FLOAT fYOffset = m_fPosY - m_pCurAttackShape->GetPosY();
	m_lDistanceFromTarget = (long)sqrt( fYOffset * fYOffset + fXOffset * fXOffset );
	//优先处理攻击距离长的技能。
	if (m_lDistanceFromTarget >= 2)
	{
		map<long ,tagPetSkillInfo>::iterator  IterSkill = m_PetSkillMap.begin();
		long lLongDisSkillPos = -1;
		for (;IterSkill != m_PetSkillMap.end();++IterSkill)
		{
			tagPetSkillInfo * pSkillInfo = &(*IterSkill).second;
			long lRangMax = pSkillInfo->lMaxAtkDistance;
			long lRangMin = pSkillInfo->lMinAtkDistance;
			if (lRangMin>=1 || lRangMax>=2)
			{
				eFightModeRet ret =  PetSkillIsCanRelease(pSkillInfo);
				if (ret == FMR_Attack)
				{
					if (lLongDisSkillPos < pSkillInfo->lPos)
					{
						lLongDisSkillPos = pSkillInfo->lPos;
					}
				}
			}
		}
		if (lLongDisSkillPos != -1)
		{
			m_lCurUseSkillPos = lLongDisSkillPos;
			return FMR_Attack;
		}
	}
	eFightModeRet ret = PetSkillIsCanRelease(pSkillInfo);
	return ret;
	
}
bool	CPet::IsPreviousSkillActionFinish()
{
	tagPetSkillInfo * pLastSkill = &m_PetSkillMap[m_lLastUseSkillPos];
	if(pLastSkill->lSkillStartTime == -1 && pLastSkill->lSkillActTime == -1)
		return true;
	else
	{
		long dwTime = timeGetTime() - pLastSkill->lSkillStartTime;
		if((long)dwTime >= pLastSkill->lSkillActTime)
		{
			pLastSkill->lSkillStartTime = GetCurTickCount();
			if (IsCurSkillActTimeFinished())
				return true;
			else
				return false;
		}
		return false;
	}
}
void	CPet::UsePetSkill(long lSkillPos)	//使用技能
{
	if (lSkillPos < 0 && lSkillPos > 11)
		return;
	tagPetSkillInfo	*pSkillInfo = &m_PetSkillMap[lSkillPos];
	if (pSkillInfo)
	{
		pSkillInfo->bIsSkillUsingEnd = false;
		if(pSkillInfo->bIsUsingByHotKey)	
			pSkillInfo->bIsUsingByHotKey = false;
	}

	CMessage msg(MSG_C2S_SKILL_USE_PET);
	msg.Add(GetExID());
	msg.Add(lSkillPos);
	msg.Add( m_pCurAttackShape->GetType() );	//目标的类型
	msg.Add( m_pCurAttackShape->GetExID() );
	msg.Add( m_pCurAttackShape->GetTileX() );
	msg.Add( m_pCurAttackShape->GetTileY() );	
	msg.Send();

	m_dwSkillPublicBeginCoolTime = GetCurTickCount();
	++m_dwSkillMsgSendNum;
	ChangeToNextSkill();
	Log4c::Trace(ROOT_MODULE,"%d",pSkillInfo->lID);
}

CPlayer *CPet::GetMaster()
{
	CClientRegion *pRegion = GetGame()->GetRegion();
	if (pRegion)
	{
		CPlayer *pMaster = (CPlayer*)pRegion->FindChildObject(TYPE_PLAYER,m_MasterGUID);
		return pMaster;
	}
	return NULL;
}

bool	CPet::IsPetMoveWithCurAction()
{
	if (m_wAction == CShape::ACT_STAND || 
		m_wAction == CShape::ACT_IDLE || 
		m_wAction == CShape::ACT_BEATTACK ||
		m_wAction == CShape::ACT_BLOCK ||
		m_wAction == CShape::ACT_PARRY ||
		m_wAction == CShape::ACT_MISS || 
		m_wAction == CShape::ACT_INTONATE ||
		(m_wAction >= CShape::ACT_FACEACTION && GetAction() <= CShape::ACT_FACEACTIONMAX) //仪态动作移动可以
		|| GetIntonateKey())
	{
		return true;
	}
	return false;
}

bool	CPet::IsPetActWithCurAction()
{
	if (IsPetMoveWithCurAction())
	{
		return true;
	}
	// 处于施法动作状态，如果该施法动作的规定时间达到，可以攻击
	if(m_wAction >= CShape::ACT_PERFORM && m_wAction <= CShape::ACT_MAX_PERFORM)
	{
		// 规定动作达到
		if(IsCurSkillActTimeFinished())
			return true;
	}
	return false;
}

bool	CPet::IsSkillBeBlcoked()
{
	if(m_pCurAttackShape == NULL)
		return false;
	CClientRegion	*pRegion = GetGame()->GetRegion();
	list<POINT> listPos;
	long lx = GetTileX();
	long ly = GetTileY();
	long lDestX = m_pCurAttackShape->GetTileX();
	long lDestY = m_pCurAttackShape->GetTileY();
	long lDistance = max( abs(lx - lDestX), abs(ly - lDestY) );
	GetLinePointToDest(listPos, lx, ly, lDestX, lDestY, lDistance);

	list<POINT>::iterator	listIter = listPos.begin();
	BYTE bBlock = 0;
	for(;listIter != listPos.end(); ++listIter)
	{
		POINT& pos = *listIter;
		bBlock = pRegion->GetBlock(pos.x, pos.y);
		if( !(bBlock == CRegion::BLOCK_CANFLY ||
			bBlock == CRegion::BLOCK_NO) )
		{
			return true;
		}
	}
	return false;
}

void	CPet::LetMeAttack()
{
	CShape *pShape = GetGame()->GetGameControl()->GetSelectedShape();	
	if (pShape && (pShape->GetType() == TYPE_PLAYER || pShape->GetType() == TYPE_PET || pShape->GetType() == TYPE_MONSTER))
	{
		CMoveShape *pCurAttackingShape = (CMoveShape*)pShape;
		if(CheckAttackTargetIsValidate(pCurAttackingShape))
		{
			SetPetCurUseSkillPos(0);
			UpdateTargetPos(pCurAttackingShape);
			SetCurAttackShape(pCurAttackingShape);
			SetForceToAttack();
			GetPetSkillInfoByPos(0)->bIsUsingByHotKey = true;
		}
	}
}

void	CPet::ProcessPetHotKey()
{
 	if(GetKeyboard()->IsKeyDown(DIK_LCONTROL))
	{
		CKeyBoard	*pKeyBoard = GetGame()->GetKeyboard();
		unsigned char Keycode = pKeyBoard->GetCurrentKey();
		//CSummonPetHotKeyPage	*pPage = GetGame()->GetCGuiEx()->GetPetHotKeyBarPage()->GetSummonHotKeyPage();
		//switch (Keycode)
		//{
		//case	DIK_1:
		//	LetMeAttack();
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_2:
		//	SetPetBehavior(PET_AS_FOLLOW);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_3:
		//	SetPetBehavior(PET_AS_STOP);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_4:
		//	pPage->UsePetSkillOnHotKey(0);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_5:
		//	pPage->UsePetSkillOnHotKey(1);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_6:
		//	pPage->UsePetSkillOnHotKey(2);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_7:
		//	pPage->UsePetSkillOnHotKey(3);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_8:
		//	SetPetFightMode(PET_AS_ACTIVE);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_9:
		//	SetPetFightMode(PET_AS_DEFEND);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//case	DIK_0:
		//	SetPetFightMode(PET_AS_PASSIVE);
		//	pKeyBoard->SetCurrentKey(0);
		//	break;
		//}
	}
}

void CPet::RendShadow(bool bsimple)
{
	if (m_bIsActived)
	{
		CMoveShape::RendShadow(bsimple);
	}
}

bool CPet::CheckBornIsComplete()				//宠物出生是否结束
{
	if (m_bIsInBornState)	//出生状态不执行AI
	{
		long loopCount = m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount;
		DWORD dwCurAct = m_pAnimInfo->GetActionInfo()->dwCurAction;
		if ( loopCount == 0 && dwCurAct == MAKEFOURCC('B','O','R','N'))
		{
			return false;
		}
		else
		{
			SetAction(CMoveShape::ACT_STAND);
			m_bIsInBornState = false;
			return true;
		}
	}
	return true;
}

void	CPet::ChangeToNextSkill()
{
	m_lLastUseSkillPos = m_lCurUseSkillPos;
	m_lCurUseSkillPos++;
}

CShape::eDIR	CPet::GetRandomPlaceBesideMaster()
{
	CPlayer *pMaster = GetMaster();
	CClientRegion *pRegion = GetGame()->GetRegion();
	if(pMaster == NULL || pRegion == NULL)
		return CShape::DIR_DOWN;
	long lDestX = pMaster->GetTileX();
	long lDestY = pMaster->GetTileY();
	bool bFindPetPos = false;
	long lFindPos = -1;
	
	vector<tagStandInfo>::iterator IterDir = vecPetStandDirPos.begin();
	for (int iStep = 0;IterDir != vecPetStandDirPos.end();++IterDir,++iStep)
	{
		if ((*IterDir).ePetType == m_PetType)
		{
			bFindPetPos = true;
			lFindPos = iStep;
			break;
		}
	}
	if(bFindPetPos)
	{
		return vecPetStandDirPos[lFindPos].eDirPos;
	}
	else
	{
		for(long i = 0; i<7; ++i)
		{
			tagStandInfo *sInfo = &vecPetStandDirPos[i];
			if (sInfo->bOccupy == false)
			{
				CShape::eDIR ePosDir = (CShape::eDIR)i;
				pMaster->GetPosFromRelativeDir(ePosDir,lDestX,lDestY);
				BYTE bBlock = pRegion->GetBlock(lDestX, lDestY);
				if(bBlock == CRegion::BLOCK_NO)
				{
					sInfo->bOccupy  = true;
					sInfo->ePetType = m_PetType;
					return sInfo->eDirPos;
				}
			}
		}
	}
	return CShape::DIR_DOWN;
}

void	CPet::ConfusePetStandInfo()
{
	std::random_shuffle(vecPetStandDirPos.begin(),vecPetStandDirPos.end());
	vector<tagStandInfo>::iterator IterDir = vecPetStandDirPos.begin();
	for (;IterDir != vecPetStandDirPos.end();++IterDir)
	{
		(*IterDir).bOccupy = false;
		(*IterDir).ePetType = PET_TYPE_NULL;
	}
}

/// 设置主人
void CPet::SetHost(CPlayer* pPlayer)
{
	if( NULL == pPlayer )
	{
		m_MasterGUID = NULL_GUID;
	}
	else
	{
		m_MasterGUID = pPlayer->GetExID();
	}
	m_Host = pPlayer;
}

void CPet::SetPetState(PET_STATE s)
{
    if (m_tagPetDiff->lStateType == s)
        return;

    //激活的宠物改成非激活状态，Mini宠物界面要隐藏
    if (PET_STATE_ACTIVE == m_tagPetDiff->lStateType)
        FireUIEvent("MiniPetPage", "MyHide");

    //非激活的宠物改成激活状态，Mini宠物界面要显示
    if (PET_STATE_ACTIVE == s)
    {
        GetMaster()->SetActivePetGUID(GetExID());
        FireUIEvent("MiniPetPage", "MyShow");
    }

    m_tagPetDiff->lStateType = s;

    //改变界面显示宠物状态的图标
    FireUIEvent("Servant/Pets", "ChangePetState");

    //改变按钮状态
    switch(m_tagPetDiff->lStateType)
    {
    case PET_STATE_SLEEP:
        FireUIEvent("Servant/Pet/Await", "MyDisabled");
        FireUIEvent("Servant/Pet/Fight", "MyDisabled");
        FireUIEvent("Servant/Pet/Active", "MyDisabled");
        break;

    case PET_STATE_AWAIT:
        FireUIEvent("Servant/Pet/Await", "MyDisabled");
        FireUIEvent("Servant/Pet/Fight", "MyEnabled");
        FireUIEvent("Servant/Pet/Active", "MyDisabled");
        break;

    case PET_STATE_FIGHT:
        FireUIEvent("Servant/Pet/Await", "MyEnabled");
        FireUIEvent("Servant/Pet/Fight", "MyDisabled");
        FireUIEvent("Servant/Pet/Active", "MyEnabled");
        break;

    case PET_STATE_ACTIVE:
        FireUIEvent("Servant/Pet/Await", "MyEnabled");
        FireUIEvent("Servant/Pet/Fight", "MyEnabled");
        FireUIEvent("Servant/Pet/Active", "MyDisabled");
        break;

    default:
        break;
    }
}

void CPet::AI(void)
{
	CMoveShape::AI();
	if (m_MasterGUID != GetGame()->GetMainPlayer()->GetExID())
		return;
	CPlayer	*pMaster = GetGame()->GetMainPlayer();
	CMoveShape *pTarget = pMaster;

	int iRet = -1;
	m_lFightAImode = PetFightAI();

	//long lX = GetTileX();
	//long lY = GetTileY();
	//long lDestX = pTarget->GetTileX();
	//long lDestY = pTarget->GetTileY();
	float lX = GetPosX();
	float lY = GetPosY();
	float lDestX = pTarget->GetPosX();
	float lDestY = pTarget->GetPosY();
	switch (m_lFightAImode)
	{
	case FMR_Trace_Far:		//追踪目标
		if((IsArrive() == true || GetAction() == ACT_STAND || GetAction() == ACT_IDLE))
		{	
			float fPosX = m_pCurAttackShape->GetDestX();
			float fPosY = m_pCurAttackShape->GetDestY();
			iRet = AutoMoveAI_A(fPosX+1.0f,fPosY+1.0f);
		}
		break;
	case FMR_Trace_Near:;
		m_lCurUseSkillPos++;
		break;

	case FMR_Attack:
		{
			if ( !(GetAction() == ACT_RUN || GetAction() == ACT_WALK) )
			{
				PetStopMove();
				SetAction(ACT_STAND);
				SetState(STATE_FIGHT);
			}


			//释放技能更新方向
			float fDestX = m_pCurAttackShape->GetPosX();
			float fDestY = m_pCurAttackShape->GetPosY();
			float fRadianDir =  GetRadianDir(m_fPosX,m_fPosY,fDestX,fDestY);
			SetDirEx(fRadianDir);

			UsePetSkill(m_lCurUseSkillPos);
		}
		break;
	case FMR_No_TARGET:
		{
			switch (m_tagPetDetail->lMoveType)
			{	
			case PET_MOVETYPE_FOLLOW:
				{
					pMaster->GetPosFromRelativeDir(CShape::DIR_LEFT,lDestX,lDestY);
                    CClientRegion *pRegion = GetGame()->GetRegion();
                    BYTE bBlock = pRegion->GetBlock((int)lDestX,(int)lDestY);
                    if(bBlock != CRegion::BLOCK_NO)
                    {
                        //lDestX = pMaster->GetTileX();
                        //lDestY = pMaster->GetTileY();
						lDestX = pMaster->GetPosX();
						lDestY = pMaster->GetPosY();
                        pMaster->GetPosFromRelativeDir(CShape::DIR_DOWN,lDestX,lDestY);
                        bBlock = pRegion->GetBlock((int)lDestX,(int)lDestY);
                        if(bBlock != CRegion::BLOCK_NO)
                        {
                            //lDestX = pMaster->GetTileX();
                            //lDestY = pMaster->GetTileY();
							lDestX = pMaster->GetPosX();
							lDestY = pMaster->GetPosY();
                        }
                    }
				}
				break;
			case PET_MOVETYPE_STOP:
				{
					lDestX = m_lStayModePosX;
					lDestY = m_lStayModePosY;
				}
				break;
			default:
				break;
			}
            //自动移动到格子中央
	        iRet = AutoMoveAI_A(lDestX+0.5f,lDestY+0.5f);
            if (iRet == 0)
            {
                if (GetAction() != ACT_STAND && GetAction() != ACT_IDLE && pMaster->GetAction() == ACT_STAND)
                {	
                    PetStopMove();
                    SetAction(ACT_STAND);
                    //SetDestXY(GetTileX(),GetTileY());
					SetDestXY(GetPosX(),GetPosY());
                }
            }   
		}
		break;
	case	FMR_NO_SKILL:
		break;
	case	FMR_SKILL_CD_NOT_ARRIVAL:
		break;
	case	FMR_SKILL_NOT_ENOUGH_MP:
		break;
	case	FMR_SKILL_ACTION_NOT_FINISH:
		break;
	case	FMR_SKILL_IS_NOT_AUTO:
	case	FMR_SKILL_LEVEL_FAIL:
		if ((GetAction() == ACT_RUN || GetAction() == ACT_WALK))
		{	
			PetStopMove();
			SetAction(ACT_STAND);
			//SetDestXY(GetTileX(),GetTileY());
			SetDestXY(GetPosX(),GetPosY());
		}
		m_lCurUseSkillPos++;

		break;
	default:
		break;
	}
}

void CPet::AskforChangeName(const char* newName)
{
    CMessage msg(MSG_C2S_PET_CHANG_NAME);
    msg.Add(GetExID());
    msg.Add(newName);
    msg.Send();

	SetPetName(newName);
}

void CPet::AskforDelete() const
{
    CMessage msg(MSG_C2S_PET_CANCEL);
    msg.Add(GetExID());
    msg.Send();
}

void CPet::AskforLvUp() const
{
	CMessage msg(MSG_C2S_PET_UPGRADE);
	msg.Add(GetExID());
	msg.Send();
}

void CPet::AskforDLvUp() const
{
	CMessage msg(MSG_C2S_PET_UPGRADE_DLEV);
	msg.Add(GetExID());
	msg.Send();
}

void CPet::AskforChangeState(PET_STATE newState) const
{
    CMessage msg(MSG_C2S_PET_CHANGE_STATE);
	msg.Add(GetExID());
	msg.Add((long)newState);
	msg.Send();
}

/// 主人死亡
void CPet::OnHostDied(void)
{
	SetState(CShape::STATE_PEACE);
	m_bIsActived = false;
}