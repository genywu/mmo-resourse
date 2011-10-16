/*

○○○○○○
○                                      ○
○○○    ○○ ○     ○○    ○○○  ○○○
○  ○      ○○    ○    ○  ○        ○
○          ○      ○    ○     ○     ○
○○○      ○○○       ○○    ○○○    ○○

"FROST" 3D GRAPHICS RENDERING ENGINE.

WRITTEN BY JIATAO.
--------------------------------------------------------------------
COPYRIGHT 2002-2003. 

描述: 粒子系统模块
作者: 贾涛
建立: 2004/02/14
最后修改: 2005/01/15
*/
#pragma once

#include <list>
#include <d3d9.h>
#include <d3dx9.h>
#include "fsingleton.h"

class particle
{

public:
	class Manager;

public:
	class Emitter
	{
		enum PARTICLE_TYPE { PARTICLE_QUAD = 0,PARTICLE_BELT};
		friend class Manager;

		static const int MAX_EMITTER_NAME = 64;

		struct tagParticle
		{
			//-----------------------------------------
			//运算数据
			D3DXVECTOR3 vPosition;			    //当前位置()
			float		fVisibility;		    //当前可见度	
			DWORD       dwColor;	  		    //当前颜色
			int			iAnimTextureRectIndex;	//当前UV动画在贴图中的位置0..n ; n = rows * columns
			DWORD		dwAnimTextureStartTime;	//这个纹理动画循环开始时间
			float		fScaleWidth;
			float		fScaleHeight;

			//-----------------------------------------
			//生成时数据（不变）
			D3DXVECTOR3 vPosition0;
			D3DXVECTOR3 vVelocity0;			//初速度
			DWORD		dwLifeTimeStart;	//生命时间开始(单位: 毫秒)

			//-----------------------------------------
			//节点
			tagParticle * pNext;				//节点
			//渲染
			tagParticle * pNextRender;			//渲染链
		};

		struct tagParameter
		{
			char		    szName[MAX_EMITTER_NAME];//发射器名称

			PARTICLE_TYPE   eParticleType;			  //粒子类型

			float		  	fInitWidth;				  //宽度
			float		  	fInitHeight;			  //高度
			float			fInitWidthRange;				
			float			fInitHeightRange;

			float			*pfVisibilityFrameArray;  //亮度值列表 0 ... 1.0f
			DWORD			dwVisibilityFrameCount;	  //帧数列表 ...

			DWORD			*pdwColorFrameArray;	  //颜色值列表 0 ... 
			DWORD			dwColorFrameCount;		  //颜色个数 ...

			DWORD			dwInitColor;			  //粒子颜色

			DWORD			dwLifeTimeLength;		  //初始生命时间起始(毫秒)

			int				iInitT;				      //起始参数(参数方程起始时间0参数)毫秒

			D3DXVECTOR3  	vAcceleration;			  //加速度

			float        	fInitVelocity;		      //初始速度

			float        	fPitchAngleStart;		  //开始角度
			float        	fPitchAngleRange;	      //垂直范围

			DWORD			dwEmitInterval;			  //发射间隔
			DWORD			dwEmitNumber;			  //每次发射数量

			DWORD			dwAnimTextureRows;		  //纹理行数
			DWORD			dwAnimTextureColumns;	  //纹理列数			 
			DWORD			dwAnimTextureTimeLength;  //纹理动画循环时间

			BOOL			bBillboard;				  //billboard标志
			BOOL			bBillboardY;			  //billboardY标志	
			float			fAngleX;				  //绕X旋转

			BOOL            bfollowedowner;              //是否跟随宿主
		}m_tParameter;

		DWORD m_dwShaderGroupHandle;				  //渲染组

		tagParticle * m_pParticleUsed;
		tagParticle * m_pParticleFree;

		//渲染头
		tagParticle * m_pParticleRenderHead;

		//函数
		tagParticle * NewParticle(void);
		void  Update(void);
		void  Render(void);

		//中间数据

		//处理中间数据
		D3DXVECTOR3 m_vPosition;		//发射器当前位置
		D3DXMATRIX  m_matBillboard;	
		D3DXMATRIX  m_matBillboardY;	

		BOOL	    m_bEmited;			//是否发射例子
		//目前的变换矩阵
		D3DXMATRIX m_matNowMatrix;

	public:
		void  CreateFromFile(const char *pszFileName);
		void  Create(const char *pszParameter,const char *pszResourcePath);
		void  Destroy();

		void  SetBillboard(const D3DXMATRIX *pBillboard);
		void  SetBillboardY(const D3DXMATRIX *pBillboardY);
		void  SetPosition(const D3DXVECTOR3 * pPosition);

		float InterpolationVisibility(DWORD dwTimeElapse);
		DWORD InterpolationColor(DWORD dwTimeElapse);
		void  SetNowMatrix(D3DXMATRIX* mat){m_matNowMatrix = *mat;}
		D3DXMATRIX& GetNowMatrix() {return m_matNowMatrix;}
		BOOL  UpdateTime(DWORD dwLastTime,DWORD dwCurTime);		
		void  Emission(void);

		void  Clear(void);
	};                        

	class Manager
	{
		DECLARE_SINGLETON(Manager);

		//Emitter列表
		std::list<Emitter *> m_lEmitterList;

		DWORD m_dwEmitterCount;

	public:

		BOOL Create();
		void Destroy();

		void Add(Emitter *pEmitter);
		void Remove(Emitter *pEmitter);

		void Update(void);
		void Render(void);
	};
};
