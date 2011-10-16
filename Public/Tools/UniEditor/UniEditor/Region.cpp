#include "StdAfx.h"
#include "Region.h"

CRegion::CRegion(void)
{
	m_strCls_Rgn	= _T("");				// 7 Characters
	m_lVersion		= 0;					// Version
	m_lType			= 0;					// Type
	m_lWidth		= 0;					// Region width
	m_lHeight		= 0;					// Region Height
	m_pCell			= NULL;					// Region cell pointer
	m_lSwitchCount	= 0;					// Switch count
	m_ptSwitch		= NULL;					// Switch coordinate
	m_lSwitchPoint	= 0;					// Switch point
	m_pSwitch		= NULL;					// Switch pointer
}

CRegion::~CRegion(void)
{
 	for(VECNPCITR itr=m_vecNPC.begin(); itr!=m_vecNPC.end(); ++itr)
	{
		delete *itr;
	}

	for(VECMONSTERITR itr=m_vecMonster.begin(); itr!=m_vecMonster.end(); ++itr)
	{
		delete *itr;
	}

	for (VECCOLLECTIONITR it = m_vecCollection.begin();it != m_vecCollection.end();it++)
	{
		delete *it;
	}
	for (VECSIGNPOSITR it = m_vecSignPos.begin();it != m_vecSignPos.end();it++)
	{
		delete *it;
	}
	for (MAPROADSITR it = m_mapRoads.begin();it != m_mapRoads.end();it++)
	{
		delete it->second;
	}
	if(m_pSwitch != NULL)
	{
		delete []m_pSwitch;	
	}

	if(m_ptSwitch != NULL)
	{
		delete []m_ptSwitch;
	}

	if(m_pCell != NULL)
	{
		delete []m_pCell;
	}
}

BOOL CRegion::Release()
{
	for(VECNPCITR itr=m_vecNPC.begin(); itr!=m_vecNPC.end(); ++itr)
	{
		delete *itr;
	}

	for(VECMONSTERITR itr=m_vecMonster.begin(); itr!=m_vecMonster.end(); ++itr)
	{
		delete *itr;
	}

	for (VECCOLLECTIONITR it = m_vecCollection.begin();it != m_vecCollection.end();it++)
	{
		delete *it;
	}
	for (VECSIGNPOSITR it = m_vecSignPos.begin();it != m_vecSignPos.end();it++)
	{
		delete *it;
	}
	for (MAPROADSITR it = m_mapRoads.begin();it != m_mapRoads.end();it++)
	{
		delete it->second;
	}
	if(m_pSwitch != NULL)
	{
		delete []m_pSwitch;	
	}

	if(m_ptSwitch != NULL)
	{
		delete []m_ptSwitch;
	}

	if(m_pCell != NULL)
	{
		delete []m_pCell;
	}
	return TRUE;
}
VECMONSTER &CRegion::GetMonster()
{
	for(VECMONSTERITR itr=m_vecMonster.begin(); itr!=m_vecMonster.end(); ++itr)
	{
		CRect rect((*itr)->GetCoordinate());
		bool bModify(false);

		if(rect.left < 1)
		{
			rect.left = 1;
			bModify = true;
		}

		if(rect.right > m_lWidth)
		{
			rect.right = m_lWidth;
			bModify = true;
		}

		if(rect.top < 1)
		{
			rect.right = 1;
			bModify = true;
		}

		if(rect.bottom > m_lHeight)
		{
			rect.right = m_lHeight;
			bModify = true;
		}

		if(bModify)
		{
			(*itr)->SetCoor(rect, m_lWidth, m_lHeight);
		}
	}

	return m_vecMonster;
}

VECNPC &CRegion::GetNPC()
{
	for(VECNPCITR itr=m_vecNPC.begin(); itr!=m_vecNPC.end(); ++itr)
	{
		CRect rect((*itr)->GetCoordinate());
		bool bModify(false);

		if(rect.left < 1)
		{
			rect.left = 1;
			bModify = true;
		}

		if(rect.right > m_lWidth)
		{
			rect.right = m_lWidth;
			bModify = true;
		}

		if(rect.top < 1)
		{
			rect.right = 1;
			bModify = true;
		}

		if(rect.bottom > m_lHeight)
		{
			rect.right = m_lHeight;
			bModify = true;
		}

		if(bModify)
		{
			(*itr)->SetCoor(rect, m_lWidth, m_lHeight);
		}
	}

	return m_vecNPC;
}
VECCOLLECTION &CRegion::GetCollection()
{
	for(VECCOLLECTIONITR itr=m_vecCollection.begin(); itr!=m_vecCollection.end(); ++itr)
	{
		CRect rect((*itr)->GetCoordinate());
		bool bModify(false);

		if(rect.left < 1)
		{
			rect.left = 1;
			bModify = true;
		}

		if(rect.right > m_lWidth)
		{
			rect.right = m_lWidth;
			bModify = true;
		}

		if(rect.top < 1)
		{
			rect.right = 1;
			bModify = true;
		}

		if(rect.bottom > m_lHeight)
		{
			rect.right = m_lHeight;
			bModify = true;
		}

		if(bModify)
		{
			(*itr)->SetCoor(rect, m_lWidth, m_lHeight);
		}
	}

	return m_vecCollection;
}
VECSIGNPOS &CRegion::GetSignPos()
{
	return m_vecSignPos;
}
MAPROADS &CRegion::GetRoads()
{
	return m_mapRoads;
}

// 校验MonsterNo信息
bool CRegion::CheckMonsterNo(const unsigned uNo, unsigned &uInd)
{
	uInd = 0;
	for (VECMONSTERITR itr = m_vecMonster.begin(); itr != m_vecMonster.end(); ++itr, ++uInd)
	{
		if (uNo == (*itr)->GetNo())
		{
			return true;
		}
	}

	return false;
}

// 校验采集物信息
bool CRegion::CheckCollectionNo(const unsigned uNo, unsigned &uInd)
{
	for (VECCOLLECTIONITR itr = m_vecCollection.begin(); itr != m_vecCollection.end(); ++itr,++uInd)
	{
		if (uNo == (*itr)->GetNo())
		{
			return true;
		}
	}

	return false;
}
// 读单元格
ifstream &operator>>(ifstream &rstream, Cell &rcell)
{
	rstream.read(reinterpret_cast<char*>(&rcell),sizeof(Cell));
	return rstream;
}

// 读出入口
ifstream &operator>>(ifstream &rstream, Switch &rswitch)
{
	rstream.read(reinterpret_cast<char*>(&rswitch),sizeof(Switch));
	return rstream;
}

// 读地图场景
ifstream &operator>>(ifstream &rstream, CRegion *pregion)
{
	char str[7];
	rstream.read(str, 7);
	pregion->m_strCls_Rgn = string(str);

	rstream.read(reinterpret_cast<char *>(&(pregion->m_lVersion)), sizeof(long));
	rstream.read(reinterpret_cast<char *>(&(pregion->m_lType)), sizeof(long));
	rstream.read(reinterpret_cast<char *>(&(pregion->m_lWidth)), sizeof(long));
	rstream.read(reinterpret_cast<char *>(&(pregion->m_lHeight)), sizeof(long));

	// 计算格子数目
	long lCellCount = pregion->CellCount();
	pregion->m_pCell = new Cell[lCellCount];
	rstream.read(reinterpret_cast<char *>(pregion->m_pCell),lCellCount*sizeof(Cell));

	//rstream.read(reinterpret_cast<char *>(&(pregion->m_lSwitchCount)), sizeof(long));
	//pregion->m_pSwitch = new Switch[pregion->m_lSwitchCount];
	//rstream.read(reinterpret_cast<char *>(pregion->m_pSwitch), pregion->m_lSwitchCount*sizeof(Switch));

	return rstream;
}

void CRegion::WriteToVecDisPlay(long lType,long lValue)
{
	switch(lType)
	{
	case 0:
		{
			m_vecMDisplay.push_back(lValue);
		}
		break;
	case 1:
		{
			m_vecCDisplay.push_back(lValue);
		}
		break;
	}
}