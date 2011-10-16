#ifndef SDOADX9_H
#define SDOADX9_H


#include <d3d9.h>

typedef interface ISDOADx9 ISDOADx9;
typedef ISDOADx9 *PSDOADx9, *LPSDOADx9;

MIDL_INTERFACE("59B789AB-BEE9-4C68-AA23-BCA9AB1A3E50")
ISDOADx9 : public IUnknown
{
public:
	/* 初始化d3d设备 */
	STDMETHOD_(HRESULT,Initialize)(THIS_ IDirect3DDevice9* pDev9, D3DPRESENT_PARAMETERS* pParams, bool bHookGameWnd) PURE;	
	/* 渲染界面内部已经调用BeginScene和EndScene，确认在EndScene之后调用它 */
	STDMETHOD_(HRESULT,Render)(THIS) PURE;	
	/* 确保在BeginScene、EndScene之间调用RenderEx */
	STDMETHOD_(HRESULT,RenderEx)(THIS) PURE;	
	/* 释放d3d相关资源 */
	STDMETHOD_(HRESULT,Finalize)(THIS) PURE;
	/* 重置设备 */
	STDMETHOD_(void,OnDeviceReset)(THIS_ D3DPRESENT_PARAMETERS* pParams) PURE;
	/* 设备丢失 */
	STDMETHOD_(void,OnDeviceLost)(THIS) PURE;
	/* 传入消息 */
	STDMETHOD_(HRESULT,OnWindowProc)(THIS_ HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lResult) PURE;
};

#endif /* SDOADX9_H */