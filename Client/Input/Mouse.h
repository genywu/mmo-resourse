#pragma once
#include "stdafx.h"
#define MOUSEBUFFERSIZE			16

extern HANDLE              g_hMouseEvent;

enum {
	NONE = 0,
	LB_DOWN = 2,
	LB_UP = 3,					// 左鍵
	LB_DCLICK = 6,

	RB_DOWN = 4,
	RB_UP = 5,					// 右鍵
	RB_DCLICK = 7,

	MB_DOWN = 8,
	MB_UP = 9,					// 中鍵
	MB_DCLICK = 10,

	MB_WHEEL,					// 滚轮
};

struct tagMouseKey
{
	long lX, lY;
	long lKey;
};


class CMouse  
{
public:
	CMouse();
	virtual ~CMouse();

public:
	bool m_bLeftDown;										// 鼠标左键是否按下
	bool m_bRightDown;										// 鼠标右键是否按下
	bool m_bMiddleDown;										// 鼠标中键是否按下

	long					m_lX;							// 鼠标移动时候的X
	long					m_lY;							// 鼠标移动时候的Y
	long					m_lOldX;						// 鼠标上一桢时候的X
	long					m_lOldY;						// 鼠标上一桢时候的Y
	DWORD					m_dwDelta;						// 滚轮偏移

private:
	queue <tagMouseKey*>		m_queueMouseKey;			// 鼠标消息的内部队列

public:

	bool Create(HINSTANCE hInst, HWND hWnd);
	void RecieveMouseInput();
	void SetPosition(long x, long y);

	long GetMouseX(void) { return m_lX; }
	long GetMouseY(void) { return m_lY; }
	long GetOldMouseX(void) { return m_lOldX; }
	long GetOldMouseY(void) { return m_lOldY; }
	long GetMouseMoveX()	{return m_lX-m_lOldX;}
	long GetMouseMoveY()	{return m_lY-m_lOldY;}

	bool IsLeftButtonDown(void) { return m_bLeftDown; }
	bool IsRightButtonDown(void) { return m_bRightDown; }
	bool IsMiddleButtonDown(void) { return m_bMiddleDown; }
	void SetCurrentButton(long value);
	long GetCurrentButton(void);
	DWORD GetWheelDelta()	{return m_dwDelta;}

// 接收/處理 window系統的系統鼠標消息
public:
	bool MouseMessageProc(long lMessage, POINT pointMousePos, WPARAM wParam);
	void PushMouseKey(long lMouseX, long lMouseY, long lValue);

	void ClearMessage();
};
