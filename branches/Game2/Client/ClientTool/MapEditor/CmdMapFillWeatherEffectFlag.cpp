#include "StdAfx.h"
#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "commdb.h"
#include "map.h"
#include "CmdMapFillWeatherEffectFlag.h"

CmdMapFillWeatherEffectFlag::CmdMapFillWeatherEffectFlag(void)
{
}

CmdMapFillWeatherEffectFlag::~CmdMapFillWeatherEffectFlag(void)
{
}

void CmdMapFillWeatherEffectFlag::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);

}

bool CmdMapFillWeatherEffectFlag::Execute()
{
	Cmd::Execute();

	//填充所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_vGridOldWeatherEffectFlagArray.push_back(pGrid->GetWeatherEffectFlag());
		pGrid->SetWeatherEffectFlag(m_bWeatherEffectFlag);
	}

	return true;
}																

void CmdMapFillWeatherEffectFlag::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldWeatherEffectFlag = m_vGridOldWeatherEffectFlagArray[i];
		pGrid->SetWeatherEffectFlag(dwOldWeatherEffectFlag);
	}
	m_vGridOldWeatherEffectFlagArray.clear();
}

void CmdMapFillWeatherEffectFlag::Dump()
{
	OutputConsole("Map_Fill_WeatherEffectFlag: %d %d",m_vGridIndexArray.size(),m_bWeatherEffectFlag);
}