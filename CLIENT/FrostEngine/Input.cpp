
#include "stdafx.h"
#include "utility.h"
#include "console.h"
#include "input.h"

IMPLEMENT_SINGLETON(input::Interface);

input::Interface::Interface(void)
{
	m_iEventQueueFront = 0;
	m_iEventQueueRear  = 0;

	m_ptPreMousePosition.x = 0;
	m_ptPreMousePosition.y = 0;
	m_ptCurMousePosition.x = 0;
	m_ptCurMousePosition.y = 0;

	m_pDInput = NULL;
	m_pDInputKeyboardDevice = NULL;

	m_iQueueCount = 0;
}

input::Interface::~Interface(void)
{
}


BOOL input::Interface::ParseMsg(UINT uiMessage,WPARAM wParam,LPARAM lParam)
{
	switch (uiMessage) 
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:		
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case 0x020a:
	case WM_CHAR:		
		{
			_tagEVENT stEvent;
			stEvent.wParam = wParam;
			stEvent.lParam = lParam;
			switch(uiMessage)
			{
			case WM_LBUTTONDOWN:
				stEvent.eType =  IET_MS_BTNDOWN;
				stEvent.dwData = MK_LBUTTON;
				m_dwMouseButtonStatus =  (DWORD)wParam;
				break;
			case WM_MBUTTONDOWN:
				stEvent.eType = IET_MS_BTNDOWN;
				stEvent.dwData = MK_MBUTTON;
				m_dwMouseButtonStatus =  (DWORD)wParam;
				break;
			case WM_RBUTTONDOWN:
				stEvent.eType = IET_MS_BTNDOWN;
				stEvent.dwData = MK_RBUTTON;
				m_dwMouseButtonStatus = (DWORD)wParam;
				break;
			case WM_LBUTTONUP:
				stEvent.eType = IET_MS_BTNUP;
				stEvent.dwData = MK_LBUTTON;
				m_dwMouseButtonStatus = (DWORD)wParam;
				break;
			case WM_MBUTTONUP:
				stEvent.eType = IET_MS_BTNUP;
				stEvent.dwData= MK_MBUTTON;
				m_dwMouseButtonStatus = (DWORD)wParam;
				break;
			case WM_MOUSEMOVE:
				stEvent.eType = IET_MS_MOVE;
				stEvent.dwData= (DWORD)wParam;
				m_dwMouseButtonStatus = (DWORD)wParam;
				break;
			case WM_RBUTTONUP:
				stEvent.eType = IET_MS_BTNUP;
				stEvent.dwData= MK_RBUTTON;
				m_dwMouseButtonStatus = (DWORD)wParam;
				break;
			case 0x020a:
				stEvent.eType = IET_MS_WHEEL;
				stEvent.dwData= (DWORD)wParam;
				break;
			case WM_CHAR:
				stEvent.eType = IET_KB_CHAR;
				stEvent.dwData	= (DWORD)wParam;
				break;
			default:
				return FALSE;
			}

			PushEvent(stEvent.eType,stEvent.dwData,stEvent.wParam,stEvent.lParam);
		}
		break;
	default:
		return FALSE;//未处理任何消息
	}

	return TRUE;
	
}

void input::Interface::PushEvent(INPUT_EVENT_TYPE eType,DWORD dwData,WPARAM wParam,LPARAM lParam)
{
//	EnterCriticalSection(&m_csCriticalSectionEvent);

	if ((m_iEventQueueRear + 1) % MAX_EVENT_QUEUE_SIZE == m_iEventQueueFront) 
		return ;//忽略多的输入;
	
	_tagEVENT &stEvent = m_eEventQueue[m_iEventQueueRear];

	stEvent.eType = eType;
	stEvent.dwData= dwData;
	stEvent.wParam = wParam;
	stEvent.lParam = lParam;

	m_iEventQueueRear++;
	m_iEventQueueRear = m_iEventQueueRear % MAX_EVENT_QUEUE_SIZE;

	m_iQueueCount++;

}

BOOL input::Interface::PopEvent(_tagEVENT *pstEvent)
{
	pstEvent->eType = IET_NONE;

	BOOL bResult = FALSE;
	if (m_iEventQueueFront != m_iEventQueueRear)//not empty
	{
		bResult = TRUE;
		//出列
		_tagEVENT &stEvent = m_eEventQueue[m_iEventQueueFront];
		pstEvent->eType = stEvent.eType;
		pstEvent->dwData= stEvent.dwData;
		pstEvent->wParam= stEvent.wParam;
		pstEvent->lParam= stEvent.lParam;


		m_iQueueCount--;

		m_iEventQueueFront++;
		m_iEventQueueFront = m_iEventQueueFront % MAX_EVENT_QUEUE_SIZE;
	}

	return bResult;
}

void input::Interface::UpdateInput(void)
{

	//鼠标
	m_ptPreMousePosition = m_ptCurMousePosition;
	::GetCursorPos(&m_ptCurMousePosition); 
	ScreenToClient(m_hWnd,&m_ptCurMousePosition);

	//键盘
	HRESULT  hResult; 
	if (m_pDInputKeyboardDevice)
	{
		memcpy(m_pPreKeyStateBuffer,m_pCurKeyStateBuffer,256);
		hResult = m_pDInputKeyboardDevice->GetDeviceState(256,(LPVOID)&m_pCurKeyStateBuffer); 
	
		for(DWORD dwKey = 0; dwKey < 256; dwKey++)
		{
			BYTE byKeyStatus = m_pCurKeyStateBuffer[dwKey];
			if (m_pPreKeyStateBuffer[dwKey] != byKeyStatus)
			{
				//此处要过滤IME
                if (byKeyStatus & 0x80)
				{
					//Down
					PushEvent(IET_KB_KEYDOWN,dwKey,0,0);
				}
				else
				{
					PushEvent(IET_KB_KEYUP,dwKey,0,0); 
				}
			}
		}

        if FAILED(hResult) 
		{ 
			// If it failed, the device has probably been lost. 
			while(hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED) 
			{
				hResult = m_pDInputKeyboardDevice->Acquire();
			}
		}
	}

}

BOOL input::Interface::Create(HWND hWnd)
{
	m_hWnd = hWnd;
	OutputConsole("HWND-create:%x\n",m_hWnd);

	BOOL bResult = CreateDevice();
	ASSERT(bResult);

	return bResult;
}

void input::Interface::Destroy()
{
	DestroyDevice();
}

BOOL input::Interface::CreateDevice()
{	
	#pragma warning( disable : 4312)

	//get INSTANCE 
    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(m_hWnd, GWL_HINSTANCE );
	//create dinput8 object for keyboard
	HRESULT hResult;
	hResult  = DirectInput8Create(hInstance,DIRECTINPUT_VERSION, IID_IDirectInput8,
			   (void **)&m_pDInput, NULL); 
    
	if (FAILED(hResult))  
	{
		OutputConsole("error: FInput:Create DInput 8 Failed.\n");
		return FALSE;
	}

	// Crreate device
	hResult = m_pDInput->CreateDevice( GUID_SysKeyboard, &m_pDInputKeyboardDevice, NULL );
    if ( FAILED(hResult))  
	{
		OutputConsole("error: FInput:Create keyboard input device failed.\n");
		return FALSE;
	}

	//setup Format
	hResult = m_pDInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard); 
	if (FAILED(hResult))
	{ 
		OutputConsole("error: FInput:Set keyboard input data format failed.\n");
		ASSERT(0);
		return FALSE; 
	} 

	// Set the cooperative level Only Use Default to DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
	//hResult = m_pDInputKeyboardDevice->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 
	hResult = m_pDInputKeyboardDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY); 
	if (FAILED(hResult))
	{
		OutputConsole("error: FInput:Set keyboard copperative level failed.\n");
		ASSERT(0);
		return FALSE; 
	} 

	//占有设备
	hResult = m_pDInputKeyboardDevice->Acquire();
	if FAILED(hResult) 
	{ 
		// If it failed, the device has probably been lost. 
		while(hResult == DIERR_INPUTLOST || hResult == DIERR_NOTACQUIRED) 
		{
			hResult = m_pDInputKeyboardDevice->Acquire();
		}
	}

	return TRUE;	
}

void input::Interface::DestroyDevice()
{
	if (m_pDInputKeyboardDevice)
		m_pDInputKeyboardDevice->Unacquire();
	
	SAFERELEASE(m_pDInputKeyboardDevice);
	SAFERELEASE(m_pDInput);
}

void  input::Interface::GetMousePosition(POINT *ptCur,POINT *ptPre)
{
	if (ptCur)
	{
		ptCur->x = m_ptCurMousePosition.x;
		ptCur->y = m_ptCurMousePosition.y;
	}

	if (ptPre)
	{
		ptPre->x = m_ptPreMousePosition.x;
		ptPre->y = m_ptPreMousePosition.y;
	}
}