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

描述: 定时器模块
作者: 贾涛
建立: 
修改: 2003/10/31
*/
#pragma once

#include "fsingleton.h"

class time
{
public:
	class Clock
	{
		//DWORD m_dwStartTime;
		DWORD m_dwCurTime;
	public:
		void Update();
	};

	class Manager
	{
		DECLARE_SINGLETON(Manager);

		Clock m_cClock;
	public:
		void Create();
		void Destroy();

		void Update();
	};

};
