

#include "stdafx.h"
#include "PetMgr.h"
#include "PetSystem/CPet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPetMgr::CPetMgr()
{

}

CPetMgr::~CPetMgr()
{
	Release();
}

void CPetMgr::Release()
{

}

void CPetMgr::AddPet(CPet *pPet,bool bExtra)
{
	if(pPet)
	{
		m_mapActivePet[pPet->GetExID()] = pPet;
		pPet->SetActive(true);
	}
	else
		ASSERT(0);
}

void CPetMgr::DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData)
{
	if( bExData )
	{
		long lCount = setReadDB.GetLongFromByteArray();	//获得所有宠物的数量
		if (lCount == 0)
			return ;

		for(int i=0;i<lCount;i++)
		{
			tagPetInfo tagInfo;
			tagInfo.byType = setReadDB.GetLongFromByteArray();
			if (tagInfo.byType == 0)
			{
				continue;
			}

			setReadDB.GetBufferFromByteArray(tagInfo.guid);
			/*tagInfo.lGrahpicsID = */setReadDB.GetLongFromByteArray();
			setReadDB.GetStringFromByteArray(tagInfo.szName,16);	//设置名字
			if (strcmp(tagInfo.szName,"") == 0)
			{
#ifdef _DEBUG
				OutputDebugString("...断言:宠物解码....\n");
#endif // _DEBUG
				ASSERT(0);
			}

			m_listAllPet.push_back(tagInfo);
		}
	}
}

void CPetMgr::ActivePet(CPet *pPet)
{
	if(!pPet)
	{
		ASSERT(0);
	}
	else
	{
		map<CGUID,CPet*>::iterator it = m_mapActivePet.begin(); 
		for (;it != m_mapActivePet.end();++it)
		{
			CPet *pPetOp = it->second;
			if (pPetOp->IsActive())
			{
				pPetOp->SetActive(false);
			}
			if (pPetOp == pPet)
			{
				pPetOp->SetActive(true);
			}
		}
	}
}
void CPetMgr::PetEnterRegion()
{
	if(m_mapActivePet.empty())
		true;
	int i=1;
	map<CGUID,CPet*>::iterator it = m_mapActivePet.begin();
	for (;it!=m_mapActivePet.end();++it,i++)
	{
		CPet	*pPet	= it->second;
		if (pPet && pPet->IsActive())
		{
			float fPosX = m_pOwner->GetPosX();
			float fPosY = m_pOwner->GetPosY();
			pPet->SetPosXY(fPosX+i,fPosY-i);

			extern	bool g_bForceUpdate;
			g_bForceUpdate = true;
			pPet->SetState(CMoveShape::STATE_PEACE);
			pPet->SetAction(CMoveShape::ACT_STAND);
			i+=1.0f;
		}
	}
}


CPet	*CPetMgr::GetActivePet(long  pt)
{
	if(m_mapActivePet.empty())
		true;
	map<CGUID,CPet*>::iterator it = m_mapActivePet.begin();
	for (;it!=m_mapActivePet.end();++it)
	{
		CPet	*pPet	= it->second;
		if (pPet && pPet->IsActive() && pPet->GetPetType() == pt)
		{
			return pPet;
		}
	}
	return NULL;
}

void CPetMgr::ProcessPetDied(CPet	*pDiedPet)		//宠物宠物死亡
{
	if (!pDiedPet)
	{
		ASSERT(0);	//
		return;
	}
}
long	CPetMgr::GetActivePetCount()
{
	long count = 0;
	map<CGUID,CPet*>::iterator it = m_mapActivePet.begin();
	for (;it != m_mapActivePet.end();++it)
	{
		CPet *pPet = it->second;
		if (pPet && pPet->IsActive() && pPet->GetPetType() != PET_TYPE_TRAP )
		{
			count++;
		}
	}
	return	count;
}