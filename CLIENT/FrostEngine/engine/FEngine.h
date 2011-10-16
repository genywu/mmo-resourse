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

描述: 主模块
修改: 2003/10/31
*/
#pragma once

#include "frender.h"
#include "fsingleton.h"


class engine
{
public:
	//配置文件
	class Manager
	{
		DECLARE_SINGLETON(Manager);

		BOOL m_bWndActive;									//窗口激活标志
		//窗口句柄副本
		HWND m_hWnd;		

		//数据
		unsigned __int64 m_qwUpdateFrame;
		unsigned __int64 m_qwRenderFrame;

		//frustum of current display frame 
		//render::Frustum * m_pFrustum;

		Manager(void);
		~Manager(void);

	public:
		//__初始化__
		BOOL  Create(HWND hWnd);										//
		void  Destroy();

		//消息处理部分
		BOOL  WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);

		//刷新，渲染
		void Update(void);
		void Render(void);
		//设置渲染主窗口
		void SetWnd(void * pWnd);
		//设置当前渲染摄像机
		void SetActiveCamera(render::Camera * pCamera);
	//	render::Frustum * GetFrustum() { return m_pFrustum;}
	};

};
