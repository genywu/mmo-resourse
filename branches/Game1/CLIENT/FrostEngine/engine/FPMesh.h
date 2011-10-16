/*========================================================================
	ProgressiveMesh:模型简化算法,为LOD提供支持
	采取顶点删除法，一次从模型网格中删除一个顶点（U）。删除方法如下：
	该顶点和另外一个顶点（V）构成一条边，把同时拥有这两个顶点三角形删除，改变只拥有这个顶点的三角形的结构，把三角形的顶点U换成V。
	确定一个顶点是否可以删除的原则是根据它在模型中的位置以及它周围的三角型所形成的曲率的大小（曲率可以反映面与面的共面接近程度），
	一般情况下，处于边缘上的顶点不要删除，所以它具有较大的删除代价；曲率越小，删除代价就越小。
========================================================================*/
#pragma once

#ifndef _PMESH_H_
#define _PMESH_H_

#include <windows.h>
#include <set>
#include <algorithm>
#include <fmodel.h>

using namespace std;

class PMVertex;
class PMTriangle;
class CollapseMap;
class ProgressiveMesh;

typedef vector<PMTriangle*> TriangleList;
typedef set<PMVertex*> VertexSet;
typedef set<PMTriangle*> TriangleSet;

/*========================================================================
	顶点类
========================================================================*/
class PMVertex
{
public:
    int                 id;			//ID，索引
    D3DXVECTOR3         pos;		//坐标数据
    BOOL                bCollapsed;	//是否塌过
	float				collapseCost;
	PMVertex			*collapseTo;
	bool				bRemoved;
	bool				bToBeRemoved;

    set<PMVertex*>		neighbors;//邻顶点
    set<PMTriangle*>	faces;	//拥有的面

    PMVertex(const D3DXVECTOR3 & v, int _id);
	bool	IsBorder();
	bool	IsBorderEdgeWith(PMVertex *);
	void	NotifyRemoved();
	void	RemoveNeighbor(PMVertex *);
};
/*========================================================================
	三角形类
========================================================================*/
class PMTriangle
{
public:
    int                 id;
    PMVertex*           vertices[3];
	D3DXVECTOR3         normal;
	int					indexes[6];
	bool				bRemoved;

    PMTriangle(PMVertex *v0, PMVertex *v1, PMVertex *v2, int _id);

    void	ReplaceVertex(PMVertex *u, PMVertex *v);
    void	ComputeNormal();
    int     FindVertex(PMVertex *v);
	void	NotifyRemoved();

};
/*========================================================================
	塌值表类
========================================================================*/
class CollapseMap
{
public:
    CollapseMap(int size);
    ~CollapseMap();	
	int GetNextCollapser();
	void SetCollapse(int index,float cost);
private:
    float   *costArray;
    int     size;
};
/*========================================================================
	模型简化类
========================================================================*/
class PMesh
{
public:
	PMesh();
	~PMesh();
	//构造一个模型简化对象,v--顶点坐标表,nV--顶点数量,poly--原始三角形表,nP--三角形数量
	void Create(const D3DXVECTOR3 *v, int nV, const model::Mesh::tagTriangle *pTriangleBuffer, int nP);
	//生成层次细节,countList存储所对应的三角形数量列表,dataList存储三角形表
	//numLevels指定层次数目,reducutionValues是各层细节简化的参数,长度不能小于numLevels
    void Build(vector<int> *countList,vector<int *> *dataList,int numLevels,float *reductionValues);

private:
    void AddVertex(PMVertex* v);
    void AddFace(PMTriangle* face);
    float CalculateEdgeCollapseCost(PMVertex *u, PMVertex *v);
    float CalculateVertexCollapseCost(PMVertex *u);
    void CalculateCollapseMap();
    void CollapseVertex(PMVertex *u);
	int* MakeLOD();

private:
    CollapseMap*          _collapseMap;
    int                   _currentNumIndexes;
    vector<PMVertex*>       _vertices;
    vector<PMTriangle*>     _poly;
};

#endif