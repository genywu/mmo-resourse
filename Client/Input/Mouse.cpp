#include "Mouse.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMouse::CMouse()
{
	m_lX = 0;
	m_lY = 0;
	m_lOldX = 0;
	m_lOldY = 0;
	m_bLeftDown = false;
	m_bRightDown = false;
	m_bMiddleDown = false;
	m_dwDelta = 0;
}

CMouse::~CMouse()
{
	while( m_queueMouseKey.size() != 0 )
	{
		tagMouseKey* p = m_queueMouseKey.front();
		m_queueMouseKey.pop();
		SAFE_DELETE(p);
	}
}

// -------------------------------------------------------
// Name: Create()
// Describe: 初始化鼠标硬件设备(DInput)
// -------------------------------------------------------
bool CMouse::Create(HINSTANCE hInst, HWND hWnd)
{
	return true;
}

// -------------------------------------------------------
// Name: RecieveMouseInput()
// Describe: 获取鼠标输入数据
// -------------------------------------------------------
void CMouse::RecieveMouseInput()
{
	if( m_queueMouseKey.size() != 0 )
	{
		tagMouseKey* p = m_queueMouseKey.front();
		m_queueMouseKey.pop();
		SAFE_DELETE(p);
	}
	return;
}

void CMouse::SetPosition(long x, long y)
{
	if(GetActiveWindow()!=g_hWnd)x=y=-1;
	m_lOldX = m_lX;
	m_lOldY = m_lY;
	m_lX = x;
	m_lY = y;
}

// -------------------------------------------------------
// Name: GetCurrentButton()
// Describe: 获取当前输入
// -------------------------------------------------------
long CMouse::GetCurrentButton()
{
	if( m_queueMouseKey.size() == 0 )
		return 0;
	tagMouseKey* p = m_queueMouseKey.front();
	long lKey = p->lKey;
	return lKey;
}

// -------------------------------------------------------
// Name: SetCurrentButton()
// Describe: 设置当前输入
// -------------------------------------------------------
void CMouse::SetCurrentButton(long value)
{
	if( m_queueMouseKey.size() == 0 )
		return;
	tagMouseKey* p = m_queueMouseKey.front();
	p->lKey = value;
}

void CMouse::ClearMessage()
{
	m_bLeftDown = false;
	m_bRightDown = false;
	m_bMiddleDown = false;
}


// -------------------------------------------------------
// Name: MouseMessageProc()
// Describe: Windows系统鼠标消息响应进程
// -------------------------------------------------------
bool CMouse::MouseMessageProc(long lMessage, POINT pointMousePos, WPARAM wParam)
{
	switch(lMessage)
	{
	case WM_LBUTTONUP:
			if(GetCapture()==g_hWnd)SetCapture(0);
			PushMouseKey(pointMousePos.x, pointMousePos.y, LB_UP);
			m_bLeftDown = false;
		break;
	case WM_LBUTTONDOWN:
			SetCapture(g_hWnd);
			PushMouseKey(pointMousePos.x, pointMousePos.y, LB_DOWN);
			m_bLeftDown = true;
		break;
	case WM_LBUTTONDBLCLK:
			PushMouseKey(pointMousePos.x, pointMousePos.y, LB_DCLICK);
			m_bLeftDown = true;
		break;
	case WM_MBUTTONDBLCLK:
			PushMouseKey(pointMousePos.x, pointMousePos.y, MB_DCLICK);
			m_bMiddleDown = true;
		break;
	case WM_MBUTTONDOWN:
			PushMouseKey(pointMousePos.x, pointMousePos.y, MB_DOWN);
			m_bMiddleDown = true;
		break;
	case WM_MBUTTONUP:
			PushMouseKey(pointMousePos.x, pointMousePos.y, MB_UP);
			m_bMiddleDown = false;
		break;
	case WM_RBUTTONDBLCLK:
			PushMouseKey(pointMousePos.x, pointMousePos.y, RB_DCLICK);
			m_bRightDown = true;
		break;
	case WM_RBUTTONDOWN:
			SetCapture(g_hWnd);
			PushMouseKey(pointMousePos.x, pointMousePos.y, RB_DOWN);
			m_bRightDown = true;
		break;
	case WM_RBUTTONUP:
			if(GetCapture()==g_hWnd)SetCapture(0);
			PushMouseKey(pointMousePos.x, pointMousePos.y, RB_UP);
			m_bRightDown = false;
			break;
	case 0x020a:
			PushMouseKey(pointMousePos.x, pointMousePos.y, MB_WHEEL);
			m_dwDelta = (DWORD)wParam;
			break;
		break;
	default:
			return false;
		break;
	}

	return true;
}

// -------------------------------------------------------
// Name: PushMouseKey()
// Describe: 插入某消息到消息队列(响应Windows消息内部操作队列)中
// -------------------------------------------------------
void CMouse::PushMouseKey(long lMouseX, long lMouseY, long lValue)
{
	tagMouseKey* p = new tagMouseKey;
	p->lX = lMouseX;
	p->lY = lMouseY;
	p->lKey = lValue;
	m_queueMouseKey.push(p);
}
