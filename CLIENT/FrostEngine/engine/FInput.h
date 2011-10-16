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

描述: 输入模块
修改: 2003/10/31
*/
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "fsingleton.h"

enum INPUT_EVENT_TYPE{
		IET_NONE,
	//键盘
		IET_KB_KEYDOWN,//DATA IS DIK CODE
		IET_KB_KEYUP,  //DATA IS DIK CODE
		IET_KB_CHAR,   //DATA IS VK CHAR
	//鼠标
		IET_MS_BTNDOWN,//DATA IS DOWN BUTTON MASK
		IET_MS_BTNUP,  //DATA IS DOWN BUTTON MASK
		IET_MS_MOVE,   //DATA IS MOUSE BUTTON MASK
		IET_MS_WHEEL,  //DATA IS SCROLL VALUE
};

#define MAX_EVENT_QUEUE_SIZE 32

class input
{
public:
	class Interface
	{
		DECLARE_SINGLETON(Interface);

		int m_iQueueCount;	
		//CRITICAL_SECTION m_csCriticalSectionEvent;

		//KEYBOARD
		IDirectInput8	 	* m_pDInput;
		IDirectInputDevice8 * m_pDInputKeyboardDevice;

		

		POINT m_ptPreMousePosition;							//上一个鼠标位置
		POINT m_ptCurMousePosition;							//当前鼠标位置
		DWORD m_dwMouseButtonStatus;						//鼠标状态

		HWND m_hWnd;										//窗口句柄

		int m_iEventQueueFront;
		int m_iEventQueueRear;

		BYTE m_pPreKeyStateBuffer[256];
		BYTE m_pCurKeyStateBuffer[256];

	public:
		struct _tagEVENT									//事件
		{
			INPUT_EVENT_TYPE eType;
			DWORD dwData;
			WPARAM wParam;
			LPARAM lParam;
		};
	private:
		_tagEVENT m_eEventQueue[MAX_EVENT_QUEUE_SIZE];

		BOOL CreateDevice();
		void DestroyDevice();

		Interface(void);
		~Interface(void);

	public:

		void UpdateInput();									//刷新输入

		//
		BOOL ParseMsg(UINT uiMessage,WPARAM wParam,LPARAM lParam);

		void PushEvent(INPUT_EVENT_TYPE eType,DWORD dwData,WPARAM wParam,LPARAM lParam);
		BOOL PopEvent(_tagEVENT *pstEvent);

		//接口：
		//取得键盘状态
		BOOL GetKeyState(BYTE byKey) const { return m_pCurKeyStateBuffer[byKey];}
		//取得键盘状态缓冲区[256]
		const BYTE * GetKeyStateBuffer() const { return m_pCurKeyStateBuffer;}
		//取得鼠标状态
		DWORD GetMouseBtnStatus() const { return m_dwMouseButtonStatus;}
		//取得鼠标坐标
		void  GetMousePosition(POINT *ptCur,POINT *ptPre);
		
		//初始化
		BOOL Create(HWND hWnd);
		//销毁
		void Destroy();
	};
};