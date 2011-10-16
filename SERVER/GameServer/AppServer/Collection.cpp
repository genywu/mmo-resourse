#include "stdafx.h"
#include "Collection.h"
#include "../GameServer/GameManager.h"
#include "ServerRegion.h"
#include "script/Script.h"



CCollection::CCollection()
{
	//m_guidMoneter= NULL_GUID;
	//m_dwID=0;
	//m_dwCollectTimes=0;
	m_dwRefreshIndex=0;
	//m_bIsBeenCollect=0;
	//m_bCanBeenCollect=0;
	m_lTimerID=-1;
	m_lDiedEvent=-1;
}
CCollection::~CCollection()
{
	DelectAllScripByShape(GetExID());
}

BOOL CCollection::Init(const CCollectionLayout* p)
{
	if (NULL == p) return FALSE;

	SetGraphicsID(p->GetPicID());//;
	SetName(p->GetCollectionName().c_str());
	SetCollectionName(p->GetCollectionName().c_str());
	//m_strOrginName=p->GetOrginName();
	SetCollectionName(p->GetOrginName());
	SetCollectionID(p->GetCollectionID());
	SetBlockType(p->GetBringBlock());
	int mintime=p->GetMinTimes();
	int maxtime=p->GetMaxTime();
	if (mintime>=maxtime)
	{
		SetCollectionTimes(maxtime);
	}
	else 
	{
		SetCollectionTimes(random(maxtime-mintime+1)+mintime);
	}
	
	if (GetCollectionTimes() <= 0)
	{
		SetCollectionTimes(1);
	}
	SetIsBeenCollect(FALSE);
	SetCanBeenCollect(TRUE);

	//m_dwID=p->GetCollectionID();
	//m_dwCollectTimes=random(p->GetMaxTime()-p->GetMinTimes())+p->GetMinTimes()+1;
	//if (m_dwCollectTimes<=0)
	//{
	//	m_dwCollectTimes=1;
	//}
	//m_dwRefreshIndex;
	//m_bIsBeenCollect=FALSE;
	//m_bCanBeenCollect=TRUE;
	return TRUE;
}

//bool CCollection::AddToByteArray(vector<BYTE>* pByteArray, bool bExData )
//{
//	CMoveShape::AddToByteArray_ForClient( pByteArray, bExData );
//	//_AddToByteArray(pByteArray,m_dwCollectionID);
//	//_AddToByteArray(pByteArray,(BYTE)m_bCanBeenCollect);
//	//受益保护
//	_AddToByteArray(pByteArray,(long)GetBeneficiaryType());//long
//	_AddToByteArray(pByteArray,GetBeneficiaryID());//CGUID
//
//	return true;
//}
//bool CCollection::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
//{
//	CMoveShape::DecodeBuffFromByteArray(pByte,pos);
//	//m_dwCollectionID=_GetLongFromByteArray(pByte,pos);
//	//m_bCanBeenCollect= _GetByteFromByteArray(pByte,pos);
//	return true;
//}

// 添加到CByteArray
bool CCollection::CodeToDataBlock(DBWriteSet& setWriteDB, bool bExData)
{
	CMoveShape::CodeToDataBlock_ForClient( setWriteDB, bExData );
	//受益保护
	setWriteDB.AddToByteArray((long)GetBeneficiaryType());//long
	setWriteDB.AddToByteArray(GetBeneficiaryID());//CGUID

	setWriteDB.AddToByteArray(m_dwCollectionID);							//采集物id
	setWriteDB.AddToByteArray(static_cast<BYTE>(m_bIsBeenCollect));			//是否正在被采集
	setWriteDB.AddToByteArray(static_cast<BYTE>(m_bCanBeenCollect));		//能否被采集

	return true;
}

// 解码
bool CCollection::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData )
{
	CMoveShape::DecodeFromDataBlock(setReadDB);
	
	return true;
}

//BOOL CCollection::AddDeleteEvent(DWORD dwTime)
//{
//	//return AddEvent(ChildObjType_DeleteObj,dwTime);
//	AddDelEvent(dwTime);
//	return TRUE;
//}

//BOOL CCollection::AddReliveEvent(DWORD dwTime)
//{
//	return AddEvent(CE_RELIVE,dwTime);
//}

//long CCollection::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
//{
//	//int type= (int)var;
//	const tagTimerVar* pTimerVar = (const tagTimerVar*)var;
//	if (pTimerVar->TimerType== ChildObjType_DeleteObj)
//	{
//		//OnDelete();
//		OnDelEvent();
//		return 0;
//	}
//
//	return CMoveShape::OnTimeOut(timerid,curtime,var);
//}


BOOL CCollection::BeenCollectedAll()
{
	LogoutDelEvent();
	CleanBeneficiary();
	AddDiedEvent();

	CMessage msg( MSG_S2C_SHAPE_ATK_DIED );
	msg.Add( 0L );
	msg.Add( NULL_GUID );
	msg.Add( this -> GetType() );
	msg.Add( this -> GetExID() );

	msg.SendToAround(this);
	return TRUE;
}

void CCollection::OnLoseBeneficiary()			//丢失受益者事件
{
	if (GetRoleProtect()==false)
	{
		//OnDelEvent();
		AddDelEvent(0);
	}
}
void CCollection::StartRecordBeneficiary(CGUID guidFirst,OBJECT_TYPE typeFirst,DWORD dwBeneficiaryTime)				//开始记录受益者
{
	SetBeneficiary(typeFirst,guidFirst);
	RecordBeneficiaryTime();
	AddBeneficiaryEvent(dwBeneficiaryTime);
	//AddDeleteEvent(dwBeneficiaryTime);
}

void CCollection::SetPosXY(float fX,float fY)
{
	long lNewTileX = static_cast<long>(fX);
	long lNewTileY = static_cast<long>(fY);
	CMoveShape::SetPosXY(fX,fY);
	//取消老阻挡
	if( !IsDied())
		SetBlock(lNewTileX,lNewTileY,CRegion::BLOCK_AIM);
}

void CCollection::SetBlock(int x, int y, unsigned short block)
{
	assert(GetFather());
	//如果没有阻挡,则不设置任何阻挡变化
	//if( !GetBringBlock())	return;
	BYTE byShapeBlockType=GetBlockType();
	if (byShapeBlockType==0)return;
	//if (block==CRegion::BLOCK_NO || block==CRegion::BLOCK_SHAPE)	return;
	//if (block==CRegion::BLOCK_CANFLY || block== CRegion::BLOCK_UNFLY)	return;

	switch(block) 
	{
	case CRegion::BLOCK_AIM:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && pCell->bBlock == CRegion::BLOCK_NO )
				{
					pCell->bBlock = CRegion::BLOCK_AIM;;//CRegion::BLOCK_AIM;
				}
			}
		}
		break;

	case CRegion::BLOCK_NO:
		for (int i=x-GetFigure(DIR_RIGHT); i<=x+GetFigure(DIR_RIGHT); i++)
		{
			for (int j=y-GetFigure(DIR_UP); j<=y+GetFigure(DIR_UP); j++)
			{
				CRegion::tagCell* pCell = ((CRegion*)GetFather())->GetCell(i,j);
				if( pCell && (pCell->bBlock == CRegion::BLOCK_AIM))// || pCell->bBlock== CRegion::BLOCK_SHAPE
				{
					pCell->bBlock = CRegion::BLOCK_NO;
				}
			}
		}
		break;
	}
}

void CCollection::AddDiedEvent()
{
	//m_lDiedEvent
	if (m_lDiedEvent!=-1)	return ;

	tagTimerVar* pTimvar =  CreateTimerVar(2);
	pTimvar->TimerType = ChildObjType_CollectionDied;
	//pTimvar->lvar= (long)this;
	m_lDiedEvent=GameManager::GetInstance()->Schedule(this,(void*)pTimvar,timeGetTime()+CGlobeSetup::GetSetup()->dwCollectionKeepDeadTime);
	return ;
}

bool CCollection::LogoutDiedEvent()
{
	if (m_lDiedEvent!=-1)
	{
		GameManager::GetInstance()->Cancel(m_lDiedEvent);
		m_lDiedEvent=-1;
		return true;
	}
	return false;
}

void CCollection::DelDiedEvent()
{
	m_lDiedEvent=-1;
}

void CCollection::OnDiedEvent()
{
	DelDiedEvent();
	AddDelEvent(0);
}

//void CCollection::OnDelEvent()
//{
//	ChangeStateTo(CShape::STATE_DIED);
//	SetBlock(GetTileX(),GetTileY(),CRegion::BLOCK_NO);
//	CServerRegion* pServerRegion = dynamic_cast<CServerRegion*>(GetFather());
//
//	if(pServerRegion)
//	{
//		CServerRegion::tagCollection* pTc= pServerRegion->GetCollectionRefeash(GetRefreshIndex());
//		if (pTc!=NULL)
//		{
//			pServerRegion->OnRemoveObject(this,pTc->lResetTime);
//		}
//		else
//		{
//			pServerRegion->OnDeleteObject(this);
//		}
//	}
//	CleanDelEvent();
//	return ;
//}

BOOL CCollection::OnRelive()
{
	dynamic_cast<CServerRegion*>(GetFather())->RefreshCollection(this);
	return TRUE;
}