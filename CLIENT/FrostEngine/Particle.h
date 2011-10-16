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
#include "singleton.h"

class particle
{

public:
	class Manager;

public:
	class Emitter:public LoadState
	{
		enum PARTICLE_TYPE { PARTICLE_QUAD = 0,PARTICLE_BELT};
		friend class Manager;

		static const int MAX_EMITTER_NAME = 64;

		struct tagParticle
		{
			tagParticle()
			{
				ZeroMemory(this,sizeof(tagParticle));
			}
			~tagParticle()
			{

			}
			//-----------------------------------------
			//运算数据
			D3DXVECTOR3 vPosition;			    //当前位置()
			float		fVisibility;		    //当前可见度	
			DWORD       dwColor;	  		    //当前颜色
			int			iAnimTextureRectIndex;	//当前UV动画在贴图中的位置0..n ; n = rows * columns
			DWORD		dwAnimTextureStartTime;	//这个纹理动画循环开始时间
			float		fScaleWidth;
			float		fScaleHeight;
			float       frotex;
			float       frotey;
			float       frotez;

			//-----------------------------------------
			//生成时数据（不变）
			D3DXVECTOR3 vPosition0;
			D3DXVECTOR3 vVelocity0;			//初速度
			DWORD		dwLifeTimeStart;	//生命时间开始(单位: 毫秒)
			D3DXVECTOR3 vSrcPosition;		//没有任何骨骼变化时的位子
			D3DXMATRIX  matstartstate;

			//-----------------------------------------
			//节点
			tagParticle * pNext;				//节点
			//渲染
			tagParticle * pNextRender;			//渲染链
			bool m_bDrawToScteen;
			bool m_bRenderToTarger;
			D3DXMATRIX m_matOwnZoneMatrix;

		};

		struct tagParameter
		{
			tagParameter()
			{
				pfVisibilityFrameArray = NULL;
				pdwColorFrameArray = NULL;

			}
			~tagParameter()
			{

			}
			char		    szName[MAX_EMITTER_NAME];//发射器名称

			PARTICLE_TYPE   eParticleType;			  //粒子类型

			float		  	fInitWidth;				  //宽度
			float		  	fInitHeight;			  //高度
			float			fInitWidthRange;				
			float			fInitHeightRange;
			int m_iroatdir;
			int m_iroatspeed;

			float			*pfVisibilityFrameArray;  //亮度值列表 0 ... 1.0f
			DWORD			dwVisibilityFrameCount;	  //帧数列表 ...

			DWORD			*pdwColorFrameArray;	  //颜色值列表 0 ... 
			DWORD			dwColorFrameCount;		  //颜色个数 ...

			DWORD			dwInitColor;			  //粒子颜色

			DWORD			dwLifeTimeLength;		  //初始生命时间起始(毫秒)

			int				iInitT;				      //起始参数(参数方程起始时间0参数)毫秒

			D3DXVECTOR3  	vAcceleration;			  //加速度

			float        	fInitVelocity;		      //初始速度

			float        	fPitchAngleStartx;		  //开始PITCH角度
			float        	fPitchAngleStarty;		  //开始YAW角度
			float        	fPitchAngleRange;	      //垂直范围

			DWORD			dwEmitInterval;			  //发射间隔
			DWORD			dwEmitNumber;			  //每次发射数量

			DWORD			dwAnimTextureRows;		  //纹理行数
			DWORD			dwAnimTextureColumns;	  //纹理列数			 
			DWORD			dwAnimTextureTimeLength;  //纹理动画循环时间

			BOOL			bBillboard;				  //billboard标志
			BOOL			bBillboardY;			  //billboardY标志	
			float			fAngleX;				  //绕X旋转
			float			fAngleY;				  //绕Y旋转
			float			fAngleZ;				  //绕Z旋转


			float           fSelfRoteSpeedX;		  //绕X旋转速度	
			float           fSelfRoteSpeedY;		  //绕Y旋转速度
			float           fSelfRoteSpeedZ;		  //绕Z旋转速度

			BOOL            bfollowedowner;              //是否跟随宿主
			BOOL            bfollowedEmitter;              //是否跟随发射器
			BOOL            btransinownzone;              //是否只在自身空间变化
			BOOL            bstaytofirstframe;              //是否只在自身空间变化
			std::vector<float> vfsize;				  //大小
		}m_tParameter;

		char m_pszShaderFileName[MAX_PATH];
		DWORD m_dwShaderGroupHandle;				  //渲染组

		tagParticle * m_pParticleUsed;
		tagParticle * m_pParticleFree;

		//渲染头
		tagParticle * m_pParticleRenderHead;

		//函数
		tagParticle * NewParticle(void);
		void  Update(float fscal = 1.0f);
		void  Render(float fscal = 1.0f,bool bsendtobuffer = true);


		//中间数据

		//处理中间数据
		D3DXVECTOR3 m_vPosition;		//发射器当前位置
		D3DXVECTOR3 m_vSrcPosition;     //发射器不随骨骼运动的位子
		D3DXMATRIX  m_matBillboard;	
		D3DXMATRIX  m_matBillboardY;	

		BOOL	    m_bEmited;			//是否发射例子
		//目前的变换矩阵
		D3DXMATRIX m_matNowMatrix;
		//固定在自身空间变换矩阵
		D3DXMATRIX m_matOwnZoneMatrix;
		//是否在界面上显示
		bool m_bDrawToScreen;
		bool m_bInDraw;
		bool m_bRenderToTarger;
		bool m_bReadyToDie;
		bool m_bDied;


	public:
		Emitter();
		~Emitter();
		void  CloneEmitter(Emitter* pEmiiter);
		void  SetDrawToScreen(bool bdrawtoscreen){m_bDrawToScreen = bdrawtoscreen;}
		void  SetRenderToTarget(bool brendertotarget){m_bRenderToTarger = brendertotarget;}
		void  Draw(float fscal,bool bdrawtoscreen = false,bool brendertotarget = false);
		const char*GetShaderFileName()	{return m_pszShaderFileName;}
		DWORD GetShaderGroupHandle()	{return m_dwShaderGroupHandle;}
		tagParameter* GetParameter()	{return &m_tParameter;}
		void  CreateFromFile(const char *pszFileName);
		void  Create(CRFile *pFile);
		void  Create(const char *pszParameter,const char *pszResourcePath);
		void  CreateParam(const char *pszParameter);
		BOOL CreateShaderGroup(CRFile* pFile,DWORD dwTextureProcision=0);

		void  Destroy();

		void  SetBillboard(const D3DXMATRIX *pBillboard);
		void  SetBillboardY(const D3DXMATRIX *pBillboardY);
		void  SetPosition(const D3DXVECTOR3 * pPosition);
		void  SetSrcPosition(const D3DXVECTOR3 * pPosition);
		void  SetReadyToDie(){m_bReadyToDie = true;}
		bool  GetIsDead(){return m_bDied;}

		float InterpolationVisibility(DWORD dwTimeElapse);
		float InterpolationSize(DWORD dwTimeElapse);
		DWORD InterpolationColor(DWORD dwTimeElapse);
		void  SetNowMatrix(D3DXMATRIX* mat){m_matNowMatrix = *mat;}
		D3DXMATRIX& GetNowMatrix() {return m_matNowMatrix;}
		void  SetOwnZoneMatrix(D3DXMATRIX* mat){m_matOwnZoneMatrix = *mat;}
		D3DXMATRIX& GetOwnZoneMatrix() {return m_matOwnZoneMatrix;}
		BOOL  UpdateTime(DWORD dwLastTime,DWORD dwCurTime);		
		void  Emission(void);
		void  Clear(void);
	};                        

	class Manager
	{
		DECLARE_SINGLETON(Manager);

		//Emitter列表for displaymodel
		std::list<Emitter *> m_lEmitterList;

		//Emitter列表for gamemodel
		std::list<Emitter *> m_lEmitterListSource;

		DWORD m_dwEmitterCount;

	public:

		BOOL Create();
		void Destroy();

		void Add(Emitter *pEmitter);
		void AddSourcelist(Emitter *pEmitter);
		void Remove(Emitter *pEmitter);

		void Update(float fscal = 1.0f,bool bdrawtoscreen = false,bool brendertotarget = false);
		void Render(float fscal = 1.0f,bool bsendtobuffer = true);
		void Draw(float fscal = 1.0f,bool bdrawtoscreen = false,bool brendertotarget = false);
	};
};
