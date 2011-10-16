#include "stdafx.h"
#include <stdio.h>
#include "fcloth.h"
#include "futility.h"

const float ST_K=4.0f;//结构弹力的弹力系数
const float ST_D=0.6f;//结构弹力的阻力系数
const float SH_K=4.0f;//剪切弹力的弹力系数
const float SH_D=0.6f;//剪切弹力的阻力系数
const float BD_K=2.4f;//弯曲弹力的弹力系数
const float BD_D=0.8f;//弯曲弹力的阻力系数
const float GRAVITY_NUM=0.8;//重力大小
const float DAMPING_FACTOR=0.2;//空气阻力系数

ClothyMesh::ClothyMesh()
{
	this->clothMesh=0;
}

void ClothyMesh::Create(const model::Mesh *mesh,unsigned int w,unsigned int h)
{
	this->Create(mesh->GetPosBuffer(),w,h);
	this->clothMesh=(model::Mesh *)mesh;
}

void ClothyMesh::Create(const Vector3 *pPosBuf,unsigned int w,unsigned int h)
{
	int count=w*h;
	this->width=w;
	this->height=h;
	this->vertexCount=count;
	for(int i=0;i<3;i++)
	{
		this->vertexArray[i]=new CMVertex[count];
	}	
	for(int i=0;i<count;i++)
	{
		this->vertexArray[2][i].pos=pPosBuf[i];
		this->vertexArray[2][i].force=Vector3(0,0,0);
		this->vertexArray[2][i].velocity=Vector3(0,0,0);
		this->vertexArray[2][i].oneOverMass=1.0f;
	}
	//this->collisionPointArray=new CMCollisionPoint[1];
	//计算会有多少组弹力
	this->springCount=0;
	int springCount=(w-2)*h+(h-2)*w;
	springCount+=count*2-w-1-h+1;
	springCount+=count*2-(h-1)*2-w*2;
	this->springArray=new CMSpring[springCount];
	//构造弹力
	int i1=0,i2=0;
	for(int i=0;i<h;i++)
	{
		for(int j=0;j<w;j++,i1++)
		{
			if(j<w-1)
			{
				i2=i1+1;
				AddSpring(i1,i2,ST_K,ST_D);
			}
			if(i<h-1)
			{
				i2=i1+w;
				AddSpring(i1,i2,ST_K,ST_D);
				if(j==0)
				{
					i2=i1+w+1;
					AddSpring(i1,i2,SH_K,SH_D);
				}
				else if(j==w-1)
				{
					i2=i1+w-1;
					AddSpring(i1,i2,SH_K,SH_D);
				}
				else
				{
					i2=i1+w+1;
					AddSpring(i1,i2,SH_K,SH_D);
					i2=i1+w-1;
					AddSpring(i1,i2,SH_K,SH_D);
				}
			}
			if(j<w-2)
			{
				i2=i1+2;
				AddSpring(i1,i2,BD_K,BD_D);//
			}
			if(i<h-2)
			{
				i2=i1+w*2;
				AddSpring(i1,i2,BD_K,BD_D);//
			}
		}
	}
	this->gravity=Vector3(0,-GRAVITY_NUM,0);
	this->damping=DAMPING_FACTOR;
	this->collisionIndexCount=0;
	this->collisionCount=0;
	this->Reset();
}

void ClothyMesh::AddSpring(int i1,int i2,float ks,float kd)
{	
	CMVertex *v1=&this->vertexArray[2][i1];
	CMVertex *v2=&this->vertexArray[2][i2];
	Vector3 v;
	D3DXVec3Subtract(&v,&v1->pos,&v2->pos);
	this->springArray[springCount].v1=i1;
	this->springArray[springCount].v2=i2;
	this->springArray[springCount].restLen=D3DXVec3Length(&v);
	this->springArray[springCount].kd=kd;
	this->springArray[springCount].ks=ks;
	springCount++;
}

void ClothyMesh::Reset()
{	
	for(int i=0;i<this->vertexCount;i++)
	{
		this->vertexArray[0][i]=this->vertexArray[2][i];
		this->vertexArray[1][i]=this->vertexArray[2][i];
	}
	if(clothMesh){
		this->CopyPositions(clothMesh->GetPosBuffer(),clothMesh->GetPNCCount());
	}
}

void ClothyMesh::SetGravity(const Vector3 &g)
{
	gravity=g;
}

void ClothyMesh::SetDampingFactor(float damp)
{
	this->damping=damp;
}

void ClothyMesh::SetClothy(float arg)
{
	for(int i=0;i<this->springCount;i++)
	{
		this->springArray[i].kd*=arg;
		this->springArray[i].ks*=arg;
	}
}

void ClothyMesh::SetCollisionMesh(Vector3 *vertexArray,unsigned int *indexArray,unsigned int indexCount)
{
	if(!vertexArray||!indexArray)return;
	this->collisionVertexArray=vertexArray;
	this->collisionIndexArray=indexArray;
	this->collisionIndexCount=indexCount;
}

void ClothyMesh::CalcVelocity()
{
	static Vector3 deltaP,deltaV,springForce;
	CMVertex *vertices=this->vertexArray[0];
	for(int i=0;i<this->vertexCount;i++)
	{
		vertices[i].force.x=vertices[i].force.y=vertices[i].force.z=0.0f;
		if(vertices[i].oneOverMass!=0)
		{
			vertices[i].force.x+=gravity.x/vertices[i].oneOverMass;
			vertices[i].force.y+=gravity.y/vertices[i].oneOverMass;
			vertices[i].force.z+=gravity.z/vertices[i].oneOverMass;
		}
		vertices[i].force.x-=vertices[i].velocity.x*damping;
		vertices[i].force.y-=vertices[i].velocity.y*damping;
		vertices[i].force.z-=vertices[i].velocity.z*damping;
	}
	for(int i=0;i<this->springCount;i++)
	{
		CMVertex *v1=&vertices[springArray[i].v1];
		CMVertex *v2=&vertices[springArray[i].v2];

		D3DXVec3Subtract(&deltaP,&v1->pos,&v2->pos);
		float dist=D3DXVec3Length(&deltaP);
		float Hterm =(dist-springArray[i].restLen) * springArray[i].ks;
		
		D3DXVec3Subtract(&deltaV,&v1->velocity,&v2->velocity);
		float Dterm = (D3DXVec3Dot(&deltaV,&deltaP) * springArray[i].kd) / dist;
		
		D3DXVec3Scale(&springForce,&deltaP,1.0f / dist);
		D3DXVec3Scale(&springForce,&springForce,-(Hterm + Dterm));
		D3DXVec3Add(&v1->force,&springForce,&v1->force);
		springForce=-springForce;
		D3DXVec3Add(&v2->force,&springForce,&v2->force);
	}
}

void ClothyMesh::CalcPosition(float deltaTime)
{
	static Vector3 tmp;
	float deltaTimeMass;
	CMVertex *source=this->vertexArray[0];
	CMVertex *target=this->vertexArray[1];
	for(int i=0;i<this->vertexCount;i++)
	{		
		deltaTimeMass = deltaTime * source[i].oneOverMass;

		target[i].velocity.x=source[i].velocity.x+source[i].force.x*deltaTimeMass;
		target[i].velocity.y=source[i].velocity.y+source[i].force.y*deltaTimeMass;
		target[i].velocity.z=source[i].velocity.z+source[i].force.z*deltaTimeMass;

		target[i].pos.x=source[i].pos.x+deltaTime*source[i].velocity.x;
		target[i].pos.y=source[i].pos.y+deltaTime*source[i].velocity.y;
		target[i].pos.z=source[i].pos.z+deltaTime*source[i].velocity.z;

		for(int j=0;j<collisionIndexCount;j++)
		{
			static D3DXPLANE plane;
			static Vector3 normal,vN,vT,intersection,tmp;
			Vector3 *v1=collisionVertexArray+collisionIndexArray[j*9];
			Vector3 *v2=collisionVertexArray+collisionIndexArray[j*9+1];
			Vector3 *v3=collisionVertexArray+collisionIndexArray[j*9+2];
			D3DXPlaneFromPoints(&plane,v1,v2,v3);
			normal.x=plane.a;
			normal.y=plane.b;
			normal.z=plane.c;
			float axbyczd=D3DXVec3Dot(&target[i].pos,&normal)+plane.d;
			int count=0;
			while(axbyczd<0.1&&axbyczd>0&&count++<100)
			{
				float relativeVelocity = D3DXVec3Dot(&normal,&target[i].velocity);
				if(relativeVelocity<0.0f)
				{
					D3DXVec3Scale(&tmp,&target[i].velocity,deltaTime);
					D3DXVec3Add(&tmp,&target[i].pos,&tmp);
					if(!D3DXPlaneIntersectLine(&intersection,&plane,&target[i].pos,&tmp))break;
					if(this->IsPointInTriangle(intersection,*v1,*v2,*v3))
					{
						float vdotn=D3DXVec3Dot(&normal,&target[i].velocity);
						D3DXVec3Scale(&vN,&normal,vdotn);
						D3DXVec3Subtract(&vT,&target[i].velocity,&vN);
						D3DXVec3Subtract(&target[i].velocity,&vT,&vN);

						axbyczd=D3DXVec3Dot(&target[i].pos,&normal)+plane.d;
					}
				}
			}
		}
	}
}

float ClothyMesh::GetAngleBetweenVectors(const Vector3 &v1, const Vector3 &v2)
{
	//return FM_AngleBetweenVectors(v1,v2);
	float dotProduct=D3DXVec3Dot(&v1,&v2);
	float vectorsMagnitude=D3DXVec3Length(&v2)*D3DXVec3Length(&v1);
	float angle=acosf(dotProduct/vectorsMagnitude);
	if(_isnan(angle))return 0;
	return(angle);
}

bool ClothyMesh::IsPointInLineSegment(const Vector3 &p,const Vector3 &p1,const Vector3 &p2)
{
	static Vector3 v1,v2,v3;
	if(p==p1||p==p2)return false;
	v1=p2-p1;
	v2=p-p1;
	v3=p-p2;
	return D3DXVec3Length(&v1)>D3DXVec3Length(&v2)&&D3DXVec3Length(&v3);
}

bool ClothyMesh::IsPointInTriangle(const Vector3 &p,const Vector3 &p1,const Vector3 &p2,const Vector3 &p3)
{	
	float angle=0;
	static Vector3 triangle[3];
	static Vector3 v1,v2;
	triangle[0]=p1;
	triangle[1]=p2;
	triangle[2]=p3;
	for(int i=0;i<3;i++)
	{
		if(triangle[i]==p)return true;
		v1=triangle[i]-p;
		v2=triangle[(i+1)%3]-p;
		angle+=GetAngleBetweenVectors(v1,v2);
	}
	return (angle>=2.0f*3.1415926);
}

void ClothyMesh::SetFixedPoint(const Vector3 &pos)
{
	for(int i=0;i<this->vertexCount;i++)
	{
		if(this->vertexArray[2][i].pos==pos)
		{
			this->vertexArray[0][i].oneOverMass=0.0f;
			this->vertexArray[1][i].oneOverMass=0.0f;
			this->vertexArray[2][i].oneOverMass=0.0f;
			break;
		}
	}
}
void ClothyMesh::Move(Vector3 &offset,float deltaTime)
{
	float dx=offset.x/height,dy=offset.y/height,dz=offset.z/height;
	int count=(int)D3DXVec3Length(&offset);
	if(count<1)count=1;
	dx*=this->damping;
	dy*=this->damping;
	dz*=this->damping;
	dx/=count;
	dy/=count;
	dz/=count;
	int index=0;
	for(int i=0;i<height;i++)
	{
		for(int j=0;j<width;j++,index++)
		{
			vertexArray[2][index].pos+=offset;
			if(this->vertexArray[2][index].oneOverMass!=0.0f)
			{
				vertexArray[0][index].velocity.x-=dx*i;
				vertexArray[0][index].velocity.y-=dy*i;
				vertexArray[0][index].velocity.z-=dz*i;
			}
			else
			{
				vertexArray[0][index].pos+=offset;
			}
		}
	}	
	Move(1);
}
void ClothyMesh::Move(Vector3 &pos1,Vector3 &pos2,float deltaTime)
{	
	this->Move(pos2-pos1,deltaTime);
}
void ClothyMesh::Move(float deltaTime)
{
	int count=(int)(deltaTime*10);
	for(int i=0;i<count;i++){
		this->CalcVelocity();
		this->CalcPosition(0.01);
		CMVertex *tmp=this->vertexArray[0];
		this->vertexArray[0]=this->vertexArray[1];
		this->vertexArray[1]=tmp;
	}
	//this->Simulate(deltaTime);
	if(clothMesh){
		this->CopyPositions(clothMesh->GetPosBuffer(),clothMesh->GetPNCCount());
	}
}
void ClothyMesh::MoveAndStand(const Vector3 &offset)
{
	for(int i=0;i<this->vertexCount;i++)
	{
		this->vertexArray[0][i].pos+=offset;
	}
	for(int i=0;i<this->vertexCount;i++)
	{
		this->vertexArray[1][i].pos+=offset;
	}
	for(int i=0;i<this->vertexCount;i++)
	{
		this->vertexArray[2][i].pos+=offset;
	}
	if(clothMesh){
		this->CopyPositions(clothMesh->GetPosBuffer(),clothMesh->GetPNCCount());
	}
}
void ClothyMesh::Simulate(float deltaTime)
{
	float currentTime=0;
	float targetTime=deltaTime;
	while(currentTime<deltaTime)
	{
		this->CalcVelocity();
		this->CalcPosition(targetTime-currentTime);
		int state=this->CheckCollisionState();
		if(state==1)
		{
			targetTime=(targetTime+currentTime)/2.0f;
		}
		else
		{
			if(state==2)
			{
				int counter=0;
				do
				{
					this->ResolveCollisions();
					counter++;
				}
				while(this->CheckCollisionState()==2&&counter<100);
			}
			currentTime=targetTime;
			targetTime=deltaTime;
			CMVertex *tmp=this->vertexArray[0];
			this->vertexArray[0]=this->vertexArray[1];
			this->vertexArray[1]=tmp;
		}
	}
}
void ClothyMesh::ResolveCollisions()
{
	static Vector3 vN,vT;
	CMCollisionPoint *points=this->collisionPointArray;
	CMVertex *vertices=this->vertexArray[1];
	for(int i=0;i<collisionCount;i++)
	{
		CMVertex *vertex=&vertices[points[i].v];
		float vdotn=D3DXVec3Dot(&points[i].normal,&vertex->velocity);
		D3DXVec3Scale(&vN,&points[i].normal,vdotn);
		vT=vertex->velocity-vN;
		//D3DXVec3Scale(&vN,&vN,0.95);
		vertex->velocity=vT-vN;
	}
	collisionCount=0;
}
int ClothyMesh::CheckCollisionState()
{
	static D3DXPLANE plane;
	static Vector3 normal,intersection,tmp;
	static Vector3 *v1,*v2,*v3;
	int state=0;
	CMVertex *source=this->vertexArray[0];
	CMVertex *target=this->vertexArray[1];
	collisionCount=0;
	if(1+1)return 0;
	for(int i=0;i<vertexCount&&state!=1;i++)
	{
		for(int j=0;j<this->collisionIndexCount;j++)
		{
			v1=collisionVertexArray+collisionIndexArray[j*9];
			v2=collisionVertexArray+collisionIndexArray[j*9+1];
			v3=collisionVertexArray+collisionIndexArray[j*9+2];
			D3DXPlaneFromPoints(&plane,v1,v2,v3);
			normal.x=plane.a;
			normal.y=plane.b;
			normal.z=plane.c;
			float axbyczd=D3DXVec3Dot(&target[i].pos,&normal)+plane.d;
			if(axbyczd<0.1&&axbyczd>0)
			{
				float relativeVelocity = D3DXVec3Dot(&normal,&target[i].velocity);
                if(relativeVelocity<0.0f)
                {
					tmp=target[i].pos+target[i].velocity*0.1;
					if(!D3DXPlaneIntersectLine(&intersection,&plane,&target[i].pos,&tmp))continue;
					if(this->IsPointInTriangle(intersection,*v1,*v2,*v3))
					{
						state=2;
						this->collisionPointArray[collisionCount].v=i;
						this->collisionPointArray[collisionCount++].normal=normal;
					}
                }
			}
		}
	}
	return state;
}
void ClothyMesh::CopyPositions(Vector3 *pOut,int count)
{
	count=min(count,this->vertexCount);
	for(int i=0;i<count;i++)
	{
		pOut[i]=this->vertexArray[0][i].pos;
	}
}
ClothyMesh::~ClothyMesh()
{
	delete[] this->vertexArray[0];
	delete[] this->vertexArray[1];
	delete[] this->vertexArray[2];
	delete[] this->springArray;
	//delete[] this->collisionPointArray;
}
