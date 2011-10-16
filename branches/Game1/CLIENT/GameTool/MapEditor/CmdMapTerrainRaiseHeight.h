//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年1月9日 10:29:35
// 更新时间 : 
// 摘要     :
//---------------------------------------------------------------------------------
#pragma once
#include "cmd.h"
#include "TerrainMesh.h"
#include "ToolTerrainHeight.h"

class CmdMapTerrainRaiseHeight :
	public Cmd
{
	std::vector<TerrainMesh::_tagVertex> m_vOldVertexArray;

	TerrainMesh::_tagProcessData* m_ptProcessData; // 本次操作的操作参数

	HEIGHTEDITSTATUETYPE m_eEditStatus;
public:
	CmdMapTerrainRaiseHeight(void);
	~CmdMapTerrainRaiseHeight(void);

	virtual void Execute(void);
	virtual void UnExecute(void);
	virtual void Dump(void);

	// 获取处理数据
	VOID GetProcessData(TerrainMesh::_tagProcessData* pData);
	// 设置处理数据
	VOID SetProcessData(TerrainMesh::_tagProcessData* pData);
	// 设置编辑状态
	VOID SetEditStatusType(HEIGHTEDITSTATUETYPE editstatus) { m_eEditStatus = editstatus; }
};
