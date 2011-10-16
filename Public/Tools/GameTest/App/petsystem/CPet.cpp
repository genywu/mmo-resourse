#include "stdafx.h"
#include "CPet.h"
#include "../PetMgr.h"
//#include "../../Utility/MsgType.h"
#include "../../GameTest/resource.h"
#include "../../GameTest/GameTestDlg.h"
#include "../../Script/ScriptSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
CPet::CPet()
{
	m_tagPetAttr = new tagPetDiff;
	m_tagPetLogic = new tagPetDetail;

	m_lType = TYPE_PET;
	m_pCurAtkShape = NULL;
	m_pPassiveAtkShape = NULL;

	m_pMaster = NULL;
	m_pRegion = NULL;
	m_bActive = false;

	m_lCurUseSkillPos = 0;
	m_lLastUseSkillPos = 0;
}
CPet::~CPet()
{
	SAFE_DELETE(m_tagPetAttr);
	SAFE_DELETE(m_tagPetLogic);
}

CPlayer* CPet::GetMaster()
{
	CPlayer* pMaster = (CPlayer*)m_pRegion->FindChildObject(TYPE_PLAYER,m_MasterGuid);
	return pMaster;
}

bool CPet::DecodePetFromDataBlock(DBReadSet& setReadDB, bool bExData /* = true */,CPlayer* pMaster,CClientRegion* pRegion)
{
	bool bRet = CMoveShape::DecordFromDataBlock( setReadDB,bExData );
	setReadDB.GetBufferFromByteArray( m_MasterGuid );
	if( !bRet )
	{
		return false;
	}
	else
	{
		if( bExData )
		{
			SetMaster(pMaster);
			SetPetRegion(pRegion);

			long lNum = setReadDB.GetLongFromByteArray();
			tagPetSkill skillinfo;
			for(long i=0;i<lNum;i++)
			{
				setReadDB.GetBufferFromByteArray(&skillinfo,sizeof(tagPetSkill));
				for (int j =0;j<5;j++)
				{
					setReadDB.GetLongFromByteArray();
				}
				m_mapSkillList[skillinfo.lPos] = skillinfo;
			}
		}
		else
		{
			long lLev = setReadDB.GetLongFromByteArray();
			long lHp = setReadDB.GetLongFromByteArray();
			long lMaxHp = setReadDB.GetLongFromByteArray();
		}
	}
	return bRet;
}

void CPet::DecordChangePropertyFromDataBlock(DBReadSet& readDB)
{
	WORD wNum = readDB.GetWordFromByteArray();
	WORD wAttrType = 0;
	BYTE bDataType = 0;
	long lData = 0;
	for (int i = 0;i<wNum;i++)
	{	
		wAttrType = readDB.GetWordFromByteArray();
		bDataType = readDB.GetByteFromByteArray();
		//根据数据类型获取具体的数据值
		switch(bDataType)
		{
		case ATTR_DATA_BYTE:
			lData = readDB.GetByteFromByteArray();
			break;
		case ATTR_DATA_WORD:
			lData = readDB.GetWordFromByteArray();
			break;
		case ATTR_DATA_DWORD:
			lData = readDB.GetDwordFromByteArray();
			break;
		case ATTR_DATA_FLOAT:
			lData = readDB.GetFloatFromByteArray();
			break;
		case ATTR_DATA_LONG:
			lData = readDB.GetLongFromByteArray();
			break;
		case ATTR_DATA_SHORT:
			lData = readDB.GetShortFromByteArray();
			break;
		case ATTR_DATA_BOOL:
			lData = readDB.GetByteFromByteArray();
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
	case PET_ATTR_Hp:
		m_tagPetAttr->lHp = lData;
		break;
	case PET_ATTR_MaxHp:
		m_tagPetLogic->lMaxHp = lData;
		break;
	case PET_ATTR_Mp:
		m_tagPetAttr->lMp = lData;
		break;
	case PET_ATTR_MaxMp:
		m_tagPetLogic->lMaxMp = lData;
		break;
	case PET_ATTR_Gorge:
		m_tagPetAttr->lGorge = lData;
		break;
	case PET_ATTR_GorgeLevel:
		m_tagPetLogic->lGorgeLevel = lData;
		break;
	case PET_ATTR_Loyalty:
		m_tagPetAttr->lLoyalty = lData;
		break;
	case PET_ATTR_Exp:
		m_tagPetAttr->lExp = lData;
		break;
	case PET_ATTR_MaxExp:
		m_tagPetLogic->lMaxExp = lData;
		break;
	case PET_ATTR_Level:
		m_tagPetAttr->lLevel = lData;
		break;
	case PET_ATTR_Absorb:
		m_tagPetLogic->lAbsorb = lData;
		break;
	case PET_ATTR_Dodge:
		m_tagPetLogic->lDodge = lData;
		break;
	case PET_ATTR_Agility:
		m_tagPetLogic->lAgility = lData;
		break;
	case PET_ATTR_Block:
		m_tagPetLogic->lBlock = lData;
		break;
	case PET_ATTR_Charm:
		m_tagPetLogic->lCharm = lData;
		break;
	case PET_ATTR_Consti:
		m_tagPetLogic->lConsti = lData;
		break;
	case PET_ATTR_ElemDef0:
		m_tagPetLogic->lElemDef0 = lData;
		break;
	case PET_ATTR_ElemDef1:
		m_tagPetLogic->lElemDef1 = lData;
		break;
	case PET_ATTR_ElemDef2:
		m_tagPetLogic->lElemDef2 = lData;
		break;
	case PET_ATTR_ElemDef3:
		m_tagPetLogic->lElemDef3 = lData;
		break;
	case PET_ATTR_ElemDef4:
		m_tagPetLogic->lElemDef4 = lData;
		break;
	case PET_ATTR_ElemDef5:
		m_tagPetLogic->lElemDef5 = lData;
		break;
	case PET_ATTR_Immunity:
		m_tagPetLogic->lImmunity = lData;
		break;
	case PET_ATTR_Intell:
		m_tagPetLogic->lIntell = lData;
		break;
	case PET_ATTR_MAtkSpeed:
		m_tagPetLogic->lMAtkSpeed = lData;
		break;
	case PET_ATTR_Luck:
		m_tagPetLogic->lLuck = lData;
		break;
	case PET_ATTR_MAtk:
		m_tagPetLogic->lMAtk = lData;
		break;
	case PET_ATTR_MCri:
		m_tagPetLogic->lMCri = lData;
		break;
	case PET_ATTR_MCriDef:
		m_tagPetLogic->lMCriDef = lData;
		break;
	case PET_ATTR_MDef:
		m_tagPetLogic->lMDef = lData;
		break;
	case PET_ATTR_MaxAtk:
		m_tagPetLogic->lMaxAtk = lData;
		break;
	case PET_ATTR_MinAtk:
		m_tagPetLogic->lMinAtk = lData;
		break;
	case PET_ATTR_Parry:
		m_tagPetLogic->lParry = lData;
		break;
	case PET_ATTR_Cri:
		m_tagPetLogic->lCri = lData;
		break;
	case PET_ATTR_CriDef:
		m_tagPetLogic->lCriDef = lData;
		break;
	case PET_ATTR_Def:
		m_tagPetLogic->lDef = lData;
		break;
	case PET_ATTR_Pierce:
		m_tagPetLogic->lPierce = lData;
		break;
	case PET_ATTR_Spirit:
		m_tagPetLogic->lSpirit = lData;
		break;
	case PET_ATTR_Strenth:
		m_tagPetLogic->lStrenth = lData;
		break;
	case PET_ATTR_WillDef:
		m_tagPetLogic->lWillDef = lData;
		break;
	case PET_ATTR_Wisdom:
		m_tagPetLogic->lWisdom = lData;
		break;
	case PET_ATTR_Hit:
		m_tagPetLogic->lHit = lData;
		break;
	case PET_ATTR_FightType:
		{
			if (lData == PET_AS_DEFEND || lData == PET_AS_PASSIVE && m_pCurAtkShape )
			{
				if( m_pMaster )
					UpdateAimPos( m_pMaster );	// 设置跟随目标是主人
				m_pCurAtkShape = NULL;
				m_pPassiveAtkShape = NULL;
			}
			m_tagPetLogic->lFightType = lData;
		}
		break;
	case PET_ATTR_MoveType:
		{
			m_tagPetLogic->lMoveType = lData;
			if (lData == PET_AS_FOLLOW && m_pCurAtkShape)
			{
				if( m_pMaster )
					UpdateAimPos(m_pMaster);	// 设置跟随目标是主人
				m_pCurAtkShape = 0;
				m_pPassiveAtkShape = 0;
			}
		}
		break;
	default:
		break;
	}
}

void CPet::UpdateProperty(CMessage *pMsg)
{
	DBReadSet dbr;
	pMsg->GetDBReadSet(dbr);
	DecordChangePropertyFromDataBlock(dbr);
}

void CPet::EnterRegion(bool bSelf /* = false  */)
{
	CPlayer* pMaster = dynamic_cast<CPlayer*>(m_pRegion->FindChildObject(TYPE_PLAYER,m_MasterGuid));
	float fPosX = 0, fPosY = 0;
	long lPetCount = 0;
	if( pMaster )
	{
		fPosX = pMaster->GetPosX();
		fPosY = pMaster->GetPosY();

		CPetMgr* pPetMgr = pMaster->GetPetMgr();
		if( pPetMgr == NULL )	return;

		pPetMgr->AddPet(this,bSelf);
		lPetCount = pPetMgr->GetActivePetCount();
		
		SetPosXY(fPosX+lPetCount,fPosY);
	}
}

//	自动寻路到目标
long CPet::AutoMoveAI(long lPosX,long lPosY)
{
	if( m_pRegion )
	{
		if( m_FindPath.GetRegion() != m_pRegion )
			m_FindPath.SetRegion( m_pRegion );
		//	已经到达目标
		float fx = GetPosX();
		float fy = GetPosY();

		float lAimX = lPosX;
		float lAimY = lPosY;

		if( abs(fx) <= 0.001 && abs(fy) <= 0.001 )
			return 0;
		long lDistance = 2;
		//	按路径走
		if( m_FindPath.GetPathList()->size() )
		{
			//	计算第一步方向
			list<CFindPath::stPos>::iterator it = m_FindPath.GetPathList()->begin();
			long lAX = (*it).x;
			long lAY = (*it).y;
			//	阻断，重新寻路
			if( m_pRegion->GetBlock(lAX,lAY) != CRegion::BLOCK_NO && 
				m_pRegion->GetBlock(lAX,lAY) != CRegion::BLOCK_AIM )
			{
				//	如果是目标格子，就停止移动
				ContinueMoveTo(lPosX,lPosY);
				return 1;
			}

			int dir1 = GetLineDir( (long)fx, (long)fy, lAX, lAY );
			m_FindPath.GetPathList()->pop_front();
			lDistance = 1;
			long lAX2 = lAX;
			long lAY2 = lAY;
			//	计算第二步
			if( m_FindPath.GetPathList()->size() )
			{
				list<CFindPath::stPos>::iterator it2 = m_FindPath.GetPathList()->begin();
				lAX2 = (*it2).x;
				lAY2 = (*it2).y;
				int dir2 = GetLineDir( lAX, lAY, lAX2, lAY2 );
				//	放心相同
				if( dir1 == dir2 )
				{
					//	阻断，重新寻路
					if (m_pRegion->GetBlock(lAX2, lAY2) != CRegion::BLOCK_NO 
						&& m_pRegion->GetBlock(lAX2, lAY2) != CRegion::BLOCK_AIM)
					{

						ContinueMoveTo(lPosX, lPosY);
						return 1;
					}
					lDistance = 2;
					m_FindPath.GetPathList()->pop_front();
				}
			}
			float fXOffset = m_fPosX - lAX;
			float fYOffset = m_fPosY - lAY;
			if(lDistance == 1)
			{
				if ( max(abs(fXOffset), abs(fYOffset))  > 2)
				{
					int	a = 0;
				}
				MoveToCell(lAX+0.5f,lAY+0.5f);
			}
			else
			{
				fXOffset = m_fPosX - lAX2;
				fYOffset = m_fPosY - lAY2;

				MoveToCell(lAX2+0.5f,lAY2+0.5f);
			}
		}
		else
		{
			if( ContinueMoveTo( lPosX, lPosY) == 0 )
				return 0;
		}
	}
	return 0;
}

//	继续移动
long CPet::ContinueMoveTo(long lAimX,long lAimY)
{
	if( m_pRegion == NULL )
		return 0;
	m_fDestX = lAimX;
	m_fDestY = lAimY;
	//	寻路
	m_FindPath.AddPathPos( m_pRegion->GetResourceID(), (long)m_fPosX,(long)m_fPosY,lAimX,lAimY,15,15);

	int size = m_FindPath.GetPathList()->size();
	if( size > 0 )
		m_FindPath.GetPathList()->pop_front();
	if( size == 0 )
		return 0;
	return 1;
}

//	移动到指定格子
void CPet::MoveToCell(float fAimX,float fAimY)
{
	if( m_pRegion )
	{
		CBaseMessage* pMsg = CBaseMessage::AllocMsg();
		pMsg->Reset();
		pMsg->Init(MSG_C2S_PET_MOVE);
		pMsg->Add((long)m_lPetType);
		pMsg->Add(m_fPosX);
		pMsg->Add(m_fPosY);
		pMsg->Add(fAimX);
		pMsg->Add(fAimY);
		((CMessage*)pMsg)->SendBySocket( m_lSocketID );
		CBaseMessage::FreeMsg(pMsg);

		SetDestXY( fAimX,fAimY);
		SetPosXY(fAimX,fAimY);
	}
}

long CPet::PetFightAI()
{
	if( !m_pMaster && !m_pRegion )
		return FMR_Other;
	//	先处理有攻击对象guid，但shape不在当前场景
	if( m_pCurAtkShape == NULL && m_guidCurAtk != CGUID::GUID_INVALID )
	{
		CBaseObject * pAtkObj = m_pRegion->FindChildObject(m_guidCurAtk);
		if( pAtkObj &&
			(pAtkObj->GetType() == TYPE_MONSTER ||
			pAtkObj->GetType() == TYPE_PLAYER || 
			pAtkObj->GetType() == TYPE_PET ) )
		{
			m_pCurAtkShape = (CMoveShape*)pAtkObj;
			m_guidCurAtk = CGUID::GUID_INVALID;
		}
	}

	if ( m_pCurAtkShape && m_pRegion->FindChildObject(m_pCurAtkShape) == false )
		m_pCurAtkShape = NULL;

	if( m_pCurAtkShape && m_pCurAtkShape->IsDied() == true )
	{
		m_pCurAtkShape = NULL;
		UpdateAimPos( m_pMaster );
	}

	switch( m_tagPetLogic->lFightType )
	{
		//	被动攻击
	case PET_AS_PASSIVE:
		{
			//	打回来,	暂时不考虑被多个目标先后攻击的情况
			if( m_pRegion->FindChildObject(m_pPassiveAtkShape) == false ||
				m_pPassiveAtkShape->IsDied() )
				m_pPassiveAtkShape = NULL;
			m_pCurAtkShape = m_pPassiveAtkShape;
		}
		break;
		//	防御模式
	case PET_AS_DEFEND:
		//	主动模式
	case PET_AS_ACTIVE:
		//AutoRetaliateAI();
		break;
	default:
		break;
	}
	if( m_pCurAtkShape )
		return SkillAI();
	else
		return	FMR_No_TARGET;
}

void CPet::UpdateAimPos(CMoveShape* pAimShape )
{
	m_fDestX = GetTileX() + 0.5f;
	m_fDestY = GetTileY() + 0.5f;
	m_FindPath.GetPathList()->clear();
}

long CPet::SkillAI()
{
	if( m_mapSkillList.empty() )
		return FMR_NO_SKILL;
	if( m_pCurAtkShape == NULL )
		return FMR_Other;

	if( m_lCurUseSkillPos >= m_mapSkillList.size() )
		return 0;

	
	tagPetSkill* pSkillInfo = &m_mapSkillList[m_lCurUseSkillPos];
	if (pSkillInfo == NULL)					
		return	FMR_NO_SKILL;

	m_lCurUseSkillPos = pSkillInfo->lPos;
	long x = m_pCurAtkShape->GetPosX();
	long y = m_pCurAtkShape->GetPosY();
	if( abs(GetPosX() - x) < 3 && abs(GetPosY() - y) <3 )
		return FMR_Attack;
	return FMR_Trace_Far;
}

void CPet::AutoRetaliateAI()
{
	if( m_pCurAtkShape&&!m_pCurAtkShape->IsDied())
	{
		UpdateAimPos(m_pCurAtkShape);
	}
}

//	使用技能
void CPet::UsePetSkill(long lSkill)
{
	if( lSkill <0 && lSkill > 11)
		return;
	tagPetSkill* pSkillInfo = &m_mapSkillList[lSkill];

	CBaseMessage* pMsg = CBaseMessage::AllocMsg();
	pMsg->Reset();
	pMsg->Init(MSG_C2S_SKILL_USE_PET);
	pMsg->Add((long)m_lPetType);
	pMsg->Add(lSkill);
	pMsg->Add(m_pCurAtkShape->GetType());
	pMsg->Add(m_pCurAtkShape->GetExID());
	pMsg->Add(m_pCurAtkShape->GetTileX());
	pMsg->Add(m_pCurAtkShape->GetTileY());
	((CMessage*)pMsg)->SendByPlayer( m_pMaster->GetExID() );
	CBaseMessage::FreeMsg(pMsg);
}


void CPet::AI()
{
	CGameClient* pClient = CScriptSys::getInstance()->GetTestDialog()->GetGameClient(m_lSocketID);
	if( pClient )
	{
		CPlayer* pPlayer = pClient->GetMainPlayer();
		CClientRegion* pRegion = pClient->GetClientRegion();
		if( pPlayer && pRegion &&
			m_pMaster == pPlayer &&
			m_pRegion == pRegion )
		{
			CMoveShape* pAimShape = m_pMaster;
			long lX = GetTileX();
			long lY = GetTileY();
			long lAimX = pAimShape->GetTileX();
			long lAimY = pAimShape->GetTileY();

			//	跟随主人移动
			if( abs(lX-lAimX) > 1 || abs(lY-lAimY) > 1 )
			{
				//if( IsActive())
				AutoMoveAI(lAimX,lAimY);
			}
		}
	}
}

void CPet::AddSkill(tagPetSkill& skillinfo)
{
	m_mapSkillList[skillinfo.lPos] = skillinfo;
}