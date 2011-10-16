//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月9日 10:29:31
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#include "StdAfx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include ".\Cmdmapterrainraiseheight.h"

#include "CommDB.h"
#include "EditWnd.h"
#include "CommUI.h"

CmdMapTerrainRaiseHeight::CmdMapTerrainRaiseHeight(void)
{
	m_ptProcessData   = NULL;
}

CmdMapTerrainRaiseHeight::~CmdMapTerrainRaiseHeight(void)
{
	SAFEDELETE(m_ptProcessData);
	m_vOldVertexArray.clear();
}

void CmdMapTerrainRaiseHeight::Execute(void)
{
	Cmd::Execute();

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

	static TerrainMesh::_tagProcessData pCurrentData;
	// 保存当前的操作参数
	pTerrainMesh->GetProcessData(&pCurrentData);

	pTerrainMesh->SetProcessData(m_ptProcessData);

	static FLOAT fOffset = 0.05f;
	switch(m_eEditStatus)
	{
	case HEST_IDLE:
		break;
	case HEST_RAISE:     // 提高
		{
			pTerrainMesh->ModifyTerrainHeight(m_vOldVertexArray, m_ptProcessData->dwSelectedGridIndex, 
				                              m_ptProcessData->dwSelectedVertexIndex, fOffset, ET_ADD);
		}
		break;
	case HEST_LOWER:     // 压低
		{
			pTerrainMesh->ModifyTerrainHeight(m_vOldVertexArray, m_ptProcessData->dwSelectedGridIndex, 
				                              m_ptProcessData->dwSelectedVertexIndex, -fOffset, ET_ADD);
		}
		break;
	case HEST_PLATEAU:   // 平台
		{
			pTerrainMesh->ModifyTerrainHeight(m_vOldVertexArray, m_ptProcessData->dwSelectedGridIndex, 
				                              m_ptProcessData->dwSelectedVertexIndex, 0.0f, ET_REPLACE);
		}
		break;
	case HEST_NOISE:     // 随机
		{
			pTerrainMesh->ModifyTerrainHeight(m_vOldVertexArray, m_ptProcessData->dwSelectedGridIndex, 
				                              m_ptProcessData->dwSelectedVertexIndex, 0.0f, ET_NOISE);
		}
		break;
	case HEST_SMOOTH:    // 平滑
		{
			pTerrainMesh->ModifyTerrainHeight(m_vOldVertexArray, m_ptProcessData->dwSelectedGridIndex, 
				                              m_ptProcessData->dwSelectedVertexIndex, 0.0f, ET_SMOOTH);
		}
		break;
	default:
		break;				
	}

	// 设回当前的操作参数
	pTerrainMesh->SetProcessData(&pCurrentData);
}

void CmdMapTerrainRaiseHeight::UnExecute(void)
{
	Cmd::UnExecute();

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	static TerrainMesh::_tagProcessData pCurrentData;

	// 保存当前的操作参数
	pTerrainMesh->GetProcessData(&pCurrentData);
	// 回填上一次操作时的操作参数
	pTerrainMesh->SetProcessData(m_ptProcessData);
	// 恢复上一次操作前的网格数据
	pTerrainMesh->FillVertex(m_vOldVertexArray, FALSE);
	// 设回当前的操作参数
	pTerrainMesh->SetProcessData(&pCurrentData);
}

void CmdMapTerrainRaiseHeight::Dump(void)
{
	// 输出错误数据
	OutputConsole("Map_Raise_Height error: %d, %d", m_ptProcessData->dwSelectedGridIndex, m_ptProcessData->dwSelectedVertexIndex);
}




//---------------------------------------------------------------------------------
// Function name   : CmdMapTerrainRaiseHeight::GetProcessData
// Description     : 获取操作参数
// Return type     : VOID 
// Argument        : TerrainMesh::_tagProcessData* pData
//---------------------------------------------------------------------------------
VOID CmdMapTerrainRaiseHeight::GetProcessData(TerrainMesh::_tagProcessData* pData)
{
	ASSERT(pData);

	if(m_ptProcessData)
		memcpy(pData, m_ptProcessData, sizeof(TerrainMesh::_tagProcessData));
}



//---------------------------------------------------------------------------------
// Function name   : CmdMapTerrainRaiseHeight::SetProcessData
// Description     : 设置操作参数
// Return type     : VOID 
// Argument        : TerrainMesh::_tagProcessData* pData
//---------------------------------------------------------------------------------
VOID CmdMapTerrainRaiseHeight::SetProcessData(TerrainMesh::_tagProcessData* pData)
{
	ASSERT(pData);

	if(!m_ptProcessData)
	{
		m_ptProcessData = new TerrainMesh::_tagProcessData;
		memcpy(m_ptProcessData, pData, sizeof(TerrainMesh::_tagProcessData));
	}
}