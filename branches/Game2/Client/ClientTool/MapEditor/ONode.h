#pragma once

#include "..\..\engine\render.h"


class QNode;
class EditMap;
class Terrain;

#define MAX_GRID_IN_NODE  64
#define MAX_BUILD_IN_NODE 16

class ONodePool;

class ONode
{
	render::BoundingBox * m_pBoundingBox;

	ONode    * m_pChildNodeArray[8];

	std::vector<int>m_vGridIndexArray;//

	std::vector<int>m_vBuildGridIndexArray;//

public:
	ONode(void);
	~ONode(void);

	render::BoundingBox * GetBoundingBox() { return m_pBoundingBox;}
	BOOL   IsLeaf();

	//void Build(std::vector<int> &vIndexArray,Terrain *pTerrain,EditMap *pMap,DWORD &dwNodeCount);
	//void BuildFast(std::vector<int> &vIndexArray,Terrain *pTerrain,EditMap *pMap,DWORD &dwNodeCount);
	void OptimizeBuild(std::vector<int> &vParentNodeGridIndexArray,std::vector<int> &vParentNodeBuildGridIndexArray,DWORD &dwNodeCount);
	
	void SetBoundingBox(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);

	void Write(FILE * file);


};
