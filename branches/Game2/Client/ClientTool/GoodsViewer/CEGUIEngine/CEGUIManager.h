/**
* @filename:CEGUIManager.h
* @date: 2009/12/17
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 
      
* @purpose: CEGUI的管理器
*/
#pragma  once

#include <CEGUI.h>
#include "RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h"

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include <dinput.h>
#include <string>

#if defined(_WIN32)
#pragma  comment(lib,"dinput8.lib")
#pragma  comment(lib,"dxguid.lib")
#endif

#include "../CEGUIEngine/BaseUIClass.h"
class UIClass::BaseUIClass;

class CEGUIManager
{
	DECLARE_SINGLETON(CEGUIManager);
/////////////////////////////////////////////////
	struct DirectInputState 
	{
		DirectInputState():directInput(0),KeyboardDevice(0)
		{}

		LPDIRECTINPUT8	directInput;
		LPDIRECTINPUTDEVICE8 KeyboardDevice;
	};
public:
	
	///@brief
	///CEGUI管理器构造函数
	CEGUIManager();

	/// @brief
	/// 析构函数，释放CEGUI DX9渲染库，CEGUI System 库
	~CEGUIManager();

	/// @brief
	/// 获取CEGUI的系统变量
	CEGUI::System* GetSystem() { return m_SysCEGUI; }

	/// @brief
	/// 初始化CEGUI
	/// @param pd3ddevice
	/// d3d9设备指针
	/// @return
	/// 成功返回true,失败返回false
	bool InitCEGUI(LPDIRECT3DDEVICE9 pd3ddevice);


	/// @brief
	/// 释放CEGUI 使用的资源等
	void ReleaseCEGUI();

	/// @brief
	/// 进行CEGUI的渲染
	void RenderCEGUI();

	/// @brief
	/// 更新CEGUI的相关项：时间，帧数，DInput
	void UpDate();


	/// @brief
	/// 因为调用CGUI渲染会修改D3D渲染的状态，所以，在渲染完成后需要重置D3D的渲染状态
	void ResetD3DRenderState();

	/// @brief
	/// 初始化资源组路径
	/// @param szPath
	/// 默认为"datafiles",确保szPath !=NULL
	void InitiliseResourceDirectories(const char *szPath="datafiles");

	/// @brief
	/// 初始化默认资源组:Imageset,Font,Scheme,looknfeel,layout,lua_script,xmlparse,mousecursor
	void InitialiseDefaultResourceGroups();

	/// @brief
	/// 是否启用显示FPS
	/// @param Enabble
	/// true = 启用，false = 不启用
	void EnableDisplayFPS(bool Enable);

	/// @brief
	/// 进行帧数更新
	void DoUpdateFPS();

	/// @brief
	/// 进行CEGUI System 时间更新
	void DoUpdateTime();

	/// @brief
	/// 帧数更新事件处理函数
	/// @param args
	/// CEGUI事件参数
	/// @return
	/// 成功返回true，失败返回false
	bool OverlayHandler(const CEGUI::EventArgs &args);


	/// @brief
	/// 加载脚本文件
	/// @param ScriptFile
	/// 脚本文件名称，不包含路径
	void ExecuteScriptFile(const char *ScriptFile);

	/// @brief
	/// 加载脚本文件数组
	/// @param：in &vecScrFiles
	/// 由vector<string>定义的脚本文件数组的引用，参数作为传入的值
	/// @return
	/// 
	void ExecuteScriptFiles(std::vector<std::string>& vecScrFiles);


	/// @brief
	/// 初始化DInput
	/// @param：in hWnd
	/// 传入的窗口句柄
	/// @param: in &dis
	/// 封装的Dinput输入设备结构体，用于存储创建好的Dinput对象
	/// @return
	/// 初始化成功返回true，失败返回false
	static bool InitialiseDirectInput(HWND hWnd,DirectInputState& dis);

	/// @brief
	/// 处理Dinput事件的函数
	/// @param &dis
	/// 封装的Dinput输入设备结构体的引用
	static void DoDirectInputEvents(const CEGUIManager::DirectInputState& dis);

	/// @brief
	/// 清除Dinput设备
	/// @param &dis
	/// 封装的Dinput输入设备结构体
	static void CleanupDirectInput(CEGUIManager::DirectInputState& dis);

	/// @brief
	/// 封装好的CEGUI自己的windows消息处理函数
	/// @param message
	/// @param wParam
	/// @param lParam
	/// @return
	/// 如果没有处理到指定的消息，就返回false，其他情况返回true
	bool ParseMsg(UINT message,WPARAM wParam,LPARAM lParam);


	/// @brief
	/// 获取封装的Dinput结构成员变量的引用
	DirectInputState& GetDirectInputState(){ return m_DirectInputState; }


	/// @brief
	/// 保存渲染前的D3D状态
	/// @remark
	/// 因为CEGUI渲染会改变D3D设备的渲染状态，所以渲染CEGUI前要保存渲染状态，以便完成CEGUI渲染后，恢复D3D的状态
	/// 此函数需要和ResetRenderSate()成对出现
	void SaveRenderState();

	/// @brief
	/// 恢复D3D到CEGUI渲染前时的状态
	/// @remark
    /// 因为CEGUI渲染会改变D3D设备的渲染状态，所以完成CEGUI渲染后，要恢复D3D的状态
	/// 此函数需要和SaveRenderState()成对出现
	void ResetRenderState();

	CEGUI::Window* CreateRootWnd();
	CEGUI::Window* GetRootWindow(){return m_RootWindow;};
	void RegisterUIClassInList();
	UIClass::BaseUIClass* GetUIClass(const CEGUI::String ClassName);
/////////////////////////////////////////////////
protected:

/////////////////////////////////////////////////
private:

	
	bool						m_ShowFPS;///< 是否显示帧数
	DWORD						m_LastFrameTime;///< 上一帧的时间
	DWORD						m_FPS_LastTime;
	int							m_FPS_Frames;
	int							m_FPS_Value;
	char						m_FPS_TextBuffer[16];///< 保存显示帧数的字符串
	CEGUI::GeometryBuffer		*m_FPS_geometry;///< 用于显示帧数的缓冲区

	CEGUI::Direct3D9Renderer	*m_RenderCEGUI; ///< CEGUI DX9 渲染库
	CEGUI::System				*m_SysCEGUI;    ///< CEGUI 系统库

	DirectInputState		m_DirectInputState;///< DirectInput 设备对象结构体

	typedef std::map<const CEGUI::String,UIClass::BaseUIClass*>UIClassList;

	UIClassList m_UIClassList;
	CEGUI::Window* m_RootWindow;
	/////////////////////////////////////////////////
	/**
	*以下变量用于存储CEGUI渲染之前的D3D设备渲染状态
	*使用者不必细究,只需成对使用SaveRenderState()和ResetRenderState()函数即可
	*/
	LPDIRECT3DDEVICE9 m_d3ddevice;
	DWORD	m_FVF;
	IDirect3DVertexShader9		*m_VertexShader; ///< 顶点shader
	IDirect3DPixelShader9		*m_PixelShader; ///< 像素shader
	DWORD m_LIGHTING;
	DWORD m_FOGENABLE;
	DWORD m_ZENABLE;
	DWORD m_ALPHATESTENABLE;
	DWORD m_CULLMODE;
	DWORD m_FILLMODE;

	DWORD m_SCISSORTESTENABLE;
	DWORD m_ZWRITENABLE;

	
	DWORD m_SAMPADDRESSU;
	DWORD m_SAMPADDRESSV;

	
	DWORD m_COLORARG1;
	DWORD m_COLORARG2;
	DWORD m_COLOROP;

	DWORD m_ALPHAARG1;
	DWORD m_ALPHAARG2;
	DWORD m_ALPHAOP;

	DWORD m_MINFILTER;
	DWORD m_MAGFILTER;

	DWORD m_TSS1_COLOROP;

	DWORD m_RS_ALPHABLENDENABLE;
	DWORD m_RS_SRCBLEND;
	DWORD m_RS_DESTBLEND;
	/////////////////////////////////////////////////

};