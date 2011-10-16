/**
	对模型实现布的摆动效果
	摆动是由风吹或者主物体的移动引起的
	布的运动是用质点-弹簧模型来模拟,如下:
	a----b----c
	|\../|\../|
	|.\/.|.\/.|
	|./\.|./\.|
	|/..\|/..\|
	e----O----f
	|\../|\../|
	|.\/.|.\/.|
	|./\.|./\.|
	|/..\|/..\|
	g----h----i	
	那么认为存在下面三种弹力:
	1.点的结构弹力,指点和上下左右邻点的弹力,如oe,of,ob,oh.保持质点间初始状态时的距离
	2.点的剪切弹力,指点和斜向上邻点的弹力,如oc,oa,og,oi.防止自身平面过渡
	3.点的弯曲弹力,指点和上下左右相隔一点的弹力，如ac,ag.防止变形和皱纹
	各种不同弹力,它们的弹簧系数各不一样
	在建立布的模型时,其顶点排列应该满足以下要求:
	1.成矩形网络排列,w*h个顶点,h行,w列
	2.所有顶点最好在同一个平面
	3.顶点最好等距排列
	4.从上到小排列,上端固定
*/

#pragma once

#ifndef _FCLOTHMESH_H_
#define _FCLOTHMESH_H_

#include <d3dx9math.h>
#include "fmodel.h"

using namespace std;

typedef D3DXVECTOR3 Vector3;

//布中的节点
struct CMVertex{
	Vector3 pos;//坐标
	Vector3 force;//受力
	Vector3 velocity;//速度
	float oneOverMass;//质量的倒数,如果为0,表示该点质量无穷大,不可移动
};

//弹簧的弹力
struct CMSpring{
	int v1,v2;//弹簧的两个顶点索引
	float restLen;//弹簧静止时候的长度
	float ks;//弹簧的弹力常数
	float kd;//弹簧的阻力系数
};
//和碰撞面发生接触的接触点
struct CMCollisionPoint{
	int v;
	Vector3 normal;
};

class ClothyMesh{
public:
	ClothyMesh();
	virtual ~ClothyMesh();
public:
	//由一个模型来构造布对象,这个模型负责布的显示
	void Create(const model::Mesh *mesh,unsigned int w,unsigned int h);
	//用顶点数组构造一个布的对象,顶点必须是成矩形排列而且数量为w*h,最好是这些顶点初始时位于同一个平面
	void Create(const Vector3 *pPosBuf,unsigned int w,unsigned int h);
	
	//设置碰撞面的信息,每个碰撞面由一个三角形规定,pIndexBuff是个三角形结构的指针(同model::Mesh::tagTriangle)
	void SetCollisionMesh(Vector3 *pVertexBuff,unsigned int *pIndexBuff,unsigned int indexCount);
	//void SetCollisionMesh(model::Mesh *mesh);

	void SetClothy(float arg);//设置布的质地,指定布的变形能力,越大变形越困难
	void SetDampingFactor(float damp);//设置空气对布的阻挡系数(0-1),抵消布的速度的比例
	void SetGravity(const Vector3& g);//设置重力加速度的方向和大小,重力大小影响布趋于静止的速度
	void SetFixedPoint(const Vector3 &pos);//指定需要固定不动的点

	void Move(float time);//使布做一次运动处理,变化时间范围是time
	void Move(Vector3 &pos1,Vector3 &pos2,float time);//主物体在time时间内从pos1移动到pos2
	void Move(Vector3 &offset,float time);//主物体在time时间内发生位移offset
	void Reset();//使所有点回到最开始的状态
	void MoveAndStand(const Vector3 &offset);//平移布,不改变布的局部运动趋势
private:
	void AddSpring(int v1,int v2,float ks,float kd);
	void Simulate(float deltaTime);
	int  CheckCollisionState();
	void ResolveCollisions();
	void CalcVelocity();
	void CalcPosition(float deltaTime);
	void CopyPositions(Vector3 *pOut,int count);
private:
	bool IsPointInLineSegment(const Vector3 &p,const Vector3 &p1,const Vector3 &p2);
	bool IsPointInTriangle(const Vector3 &p,const Vector3 &p1,const Vector3 &p2,const Vector3 &p3);
	float GetAngleBetweenVectors(const Vector3 &v1,const Vector3 &v2);
private:
	model::Mesh *clothMesh;//布的模型
	float damping;//阻力的大小
	Vector3 gravity;//重力
	CMVertex *vertexArray[3];//顶点数组的数组,第一二个数组做当前位置交替变化的计算,第三个数组存储初始状态的位置信息
	unsigned int vertexCount;//顶点数量
	unsigned int width,height;
	CMSpring *springArray;//弹力数组
	unsigned int springCount;//弹力数量
	Vector3 *collisionVertexArray;
	unsigned int *collisionIndexArray;
	unsigned int collisionIndexCount;
	CMCollisionPoint *collisionPointArray;
	unsigned int collisionCount;
};
#endif