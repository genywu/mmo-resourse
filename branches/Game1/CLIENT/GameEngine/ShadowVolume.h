//---------------------------------------------------------------------------------
// 创建     : 范骁
// 创建时间 : 2004年4月2日 15:04:51
// 更新时间 : 
// 摘要     : 计算体积阴影的相关数据
//---------------------------------------------------------------------------------
#pragma once

#include "..\frostengine\render.h"
#include "gamemodel.h"

//模型中的边的表示结构
struct MeshEdge 
{
	//顶点索引
	int v[2];
	//三角形索引
	int face[2];
};

class CShadowVolume
{
private:
	static const int MAX_VERTICES = 2048 * 3 * 6;
	static const int MAX_EDGES    = 2048 * 3;
	static const int MAX_TRIANGLES= 2048;
    D3DXVECTOR3	*m_pVertices;
    DWORD		 m_dwNumVertices;
	MeshEdge	*m_pEdges;
	DWORD		 m_dwNumEdges;
	BOOL		*m_pFaceLights;

	D3DXMATRIX	 m_matWorld;
	BOOL		 m_bAvailable;
	BOOL		 m_bOccluded;
public:
	CShadowVolume(void);
	~CShadowVolume(void);

    void    Reset();
	//从模型计算影子体积，如果模型是封闭的，采用封闭模型影子体积计算方法；否则采用非封闭模型的计算方法
    BOOL    BuildFromMesh(model::Buffer *pBuffer, D3DXVECTOR3 &vLight);
    void	Render();
	void	SetWorldMatrix(D3DXMATRIX *pm);
	DWORD	GetTriangleNum() { return (m_dwNumVertices / 3); };
private:
	BOOL    AnalyseMesh(const model::Buffer *pBuffer);//预分析模型特征
	bool	AddFaceEdge(int face,int v1,int v2);//添加统计一个边,如果这个边不满足封闭网络的特点，返回false
	bool	IsOccludedMesh(const model::Buffer *);//计算模型是否是封闭的三角形网络
	void	BuildFromAccludedMesh(const model::Buffer *pBuffer,const D3DXVECTOR3 &vLight);//从封闭的模型构造影子体积
	void	BuildFromNonAccludedMeshMesh(const model::Buffer *pBuffer,const D3DXVECTOR3 &vLight);//从非封闭的模型构造影子体积
};
