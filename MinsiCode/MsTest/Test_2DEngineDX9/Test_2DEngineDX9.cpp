#include "Precompiled.h"
#include "Test_2DEngineDX9.h"
#include "Ms2DEngine/Ms2DEngineDef.h"
#include "Ms2DEngine/Ms2DTextureManager.h"
#include "Ms2DEngine/Ms2DEngine.h"
#include "Ms2DEngine/Ms2DRender_DX9.h"

CMs2DRender_DX9* g_pCMs2DRender_DX9 = NULL;

LRESULT CALLBACK PlaneEngineWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = ::GetDC(hWnd);

	switch (uMsg)
	{
    case WM_CREATE:
        {
            CMs2DEngine::s_CreateD3D9(hWnd, 800, 600);
            g_pCMs2DRender_DX9 = NEW CMs2DRender_DX9;
            AssertEx(g_pCMs2DRender_DX9, _T("内存不足"));
        }break;

    case MSM_INIT_MESSAGE:
        {
            g_pCMsWinMain->SetIcon(IDI_SMALL, TRUE);
            g_pCMsWinMain->SetWndName(_T("付家的小家"));
            g_pCMsWinMain->SetMaximize(FALSE);
            g_pCMsWinMain->SetMinimize(FALSE);
            g_pCMsWinMain->SetLockSize(800, 600);
            g_pCMs2DRender_DX9->Run();
        }break;

    case WM_PAINT:
        {
            hdc = ::BeginPaint(hWnd, &ps);
            ::EndPaint(hWnd, &ps);
        }break;

	case WM_DESTROY:
        {
            g_pCMs2DRender_DX9->Stop(TRUE);
            CMs2DEngine::s_DeleteD3D9();
            SAFE_DELETE(g_pCMs2DRender_DX9);
            ::PostQuitMessage(0);
        }break;

	default:
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
