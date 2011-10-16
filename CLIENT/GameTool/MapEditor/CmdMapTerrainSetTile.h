#pragma once
#include "cmd.h"
#include "TerrainMesh.h"

class CmdMapTerrainSetTile :	public Cmd
{
	std::vector<TerrainMesh::_tagVertex> m_vOldVertexArray;
	TerrainMesh::_tagProcessData* m_ptProcessData; // 本次操作的操作参数

public:
	CmdMapTerrainSetTile(void);
	~CmdMapTerrainSetTile(void);

	// 获取处理数据
	VOID GetProcessData(TerrainMesh::_tagProcessData* pData);

	// 设置处理数据
	VOID SetProcessData(TerrainMesh::_tagProcessData* pData);

	VOID Destroy();

	virtual void Execute(void);
	virtual void UnExecute(void);
	virtual void Dump(void);
};
