#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\render.h"

#include "visibleset.h"
#include "map.h"
#include "commdb.h"

VisibleSet::VisibleSet(void)
{
	m_dwCurFlagValue = 0;
	m_dwVisibleIndexCount = 0;
}

VisibleSet::~VisibleSet(void)
{
}

void VisibleSet::Create(EditMap * pMap)
{
	m_dwMapWidth  = pMap->GetWidth();
	m_dwMapDepth  = pMap->GetDepth();

	m_dwGridCount =  m_dwMapWidth * m_dwMapDepth;

	m_pGridFlagArray = new DWORD[m_dwGridCount];
	ZeroMemory(m_pGridFlagArray,sizeof(DWORD) * m_dwGridCount);

	m_pGridIndexArray= new DWORD[m_dwGridCount];

}

void VisibleSet::Destroy()
{
	SAFEDELETEARRAY(m_pGridFlagArray);
	SAFEDELETEARRAY(m_pGridIndexArray);
}

void VisibleSet::SetVisible(int iGridIndex)
{
	DWORD &dwFlag = m_pGridFlagArray[iGridIndex];
	if ( dwFlag != m_dwCurFlagValue)	
	{
		dwFlag = m_dwCurFlagValue;
		
		m_pGridIndexArray[m_dwVisibleIndexCount++] = iGridIndex;
	}
}

void VisibleSet::Reset()
{
	m_dwCurFlagValue++;
	m_dwVisibleIndexCount = 0;
}

void VisibleSet::_DEBUG_Render(void)
{
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	//

	for(DWORD n = 0;n < m_dwVisibleIndexCount; n++)
	{
		DWORD dwGridIndex = m_pGridIndexArray[n];

		D3DXVECTOR3 pos[4];
		D3DXVECTOR3 vCenter;
		pMap->GetGridPosition(dwGridIndex,pos,&vCenter);
		/*float x = (float)(dwGridIndex % m_dwMapWidth);
		float z = (float)(dwGridIndex / m_dwMapWidth);

		//
		//[Z]     
		//2------3
		//|      |
		//|      |
		//0------1[X]

		pos[0] = D3DXVECTOR3(x,0,z);
		pos[1] = D3DXVECTOR3(x+1,0,z);
		pos[2] = D3DXVECTOR3(x,0,z+1);
		pos[3] = D3DXVECTOR3(x+1,0,z+1);*/
		
		pLayer3D->_DEBUG_DrawLine(&pos[0],&pos[1],D3DCOLOR_ARGB(192,125,125,125));	
		pLayer3D->_DEBUG_DrawLine(&pos[1],&pos[3],D3DCOLOR_ARGB(192,125,125,125));	
		pLayer3D->_DEBUG_DrawLine(&pos[2],&pos[3],D3DCOLOR_ARGB(192,125,125,125));	
		pLayer3D->_DEBUG_DrawLine(&pos[2],&pos[0],D3DCOLOR_ARGB(192,125,125,125));	
	}
}

