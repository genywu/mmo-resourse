#ifndef SDOA4CLIENT_H
#define SDOA4CLIENT_H


#include <objbase.h>
#include "windows.h"

#pragma pack(push,1)

// SDOA_SDK_VERSION 表示一套接入的SDK版本号
#define SDOA_SDK_VERSION                 0x0200

// SDOA 返回值
#define SDOA_OK                      0
#define SDOA_FALSE                   1

// 内存不足
#define SDOA_INSUFFICIENT_BUFFER     122  

// SDOA开始工作后发出的消息
#define SDOA_WM_CLIENT_RUN			0xA604// WM_USER + 0xA204

// 游戏图形引擎类型
#define SDOA_RENDERTYPE_UNKNOWN             0x00
#define SDOA_RENDERTYPE_GDI                 0x01
#define SDOA_RENDERTYPE_DDRAW               0x04
#define SDOA_RENDERTYPE_D3D7                0x08
#define SDOA_RENDERTYPE_D3D8                0x10
#define SDOA_RENDERTYPE_D3D9                0x20
#define SDOA_RENDERTYPE_D3D10               0x40
#define SDOA_RENDERTYPE_OPENGL              0x80

// 外部控制Screen状态 
#define SDOA_SCREEN_NONE				0		// Screen没有被创建
#define SDOA_SCREEN_NORMAL				1		// 正常显示状态
#define SDOA_SCREEN_MINI				2		// 最小化状态


// 内部IWidgetWindows状态控制 
#define SDOA_WINDOWSSTATUS_NONE			0		// 窗口没有被创建
#define SDOA_WINDOWSSTATUS_SHOW			1		// 窗口显示状态
#define SDOA_WINDOWSSTATUS_HIDE			2		// 窗口隐藏
// 窗口类型
#define SDOA_WINDOWSTYPE_NONE		0       // 未知类型
#define SDOA_WINDOWSTYPE_TOOLBAR	1		// 工具栏
#define SDOA_WINDOWSTYPE_WIDGET		2		// Widget窗口
#define SDOA_WINDOWSTYPE_WINDOWS	3		// 普通窗口

// 打开Widget的选项
#define SDOA_OPENWIDGET_DEFAULT			0		// 默认
#define SDOA_OPENWIDGET_NOBARICON		1		// 仅打开Widget，不自动添加到工具栏上

// 接口SetSelfCursor用到的常量
#define SDOA_CURSOR_AUTO		0	// 自动检测是否自绘鼠标指针
#define SDOA_CURSOR_SELF		1	// 始终自绘制鼠标指针
#define SDOA_CURSOR_HIDESELF	2	// 始终不自绘制鼠标指针

// 登录方法
#define SDOA_LOGINMETHOD_SSO        0   // 登录方法：SSO
#define SDOA_LOGINMETHOD_SDO        1   // 登录方法：SDO(CAS)

// 登录状态
#define SDOA_LOGINSTATE_UNKNOWN     -1  // 未知，参数不正确或获取状态异常
#define SDOA_LOGINSTATE_NONE        0   // 还没有调用过登录方法
#define SDOA_LOGINSTATE_OK          1   // 登录成功
#define SDOA_LOGINSTATE_CONNECTING  2   // 正在登录
#define SDOA_LOGINSTATE_FAILED      3   // 登录失败
#define SDOA_LOGINSTATE_LOGOUT      4   // 已登出

// 圈圈工具条展开模式
#define SDOA_BARMODE_ICONBOTTOM		0   // 工具条在圈圈图标下面
#define SDOA_BARMODE_ICONTOP		1   // 工具条在圈圈图标上面

struct AppInfo {
	DWORD		cbSize;				// = sizeof(AppInfo)
	int			nAppID;				// 盛大统一游戏ID
	LPCWSTR		pwcsAppName;		// 游戏名称
	LPCWSTR		pwcsAppVer;		   	// 游戏版本
	int			nRenderType;		// 游戏图形引擎类型 (取值SDOA_RENDERTYPE_UNKNOWN..SDOA_RENDERTYPE_OPENGL)
	int			nMaxUser;			// 游戏可同时在一台机器上游戏的最大人数（通常为1，例如:KOF应该是2）
	int			nAreaId;			// 游戏区ID	(注意：不可用时传-1)
	int			nGroupId;           // 游戏组ID	(注意：不可用时传-1)
};


struct RoleInfo{
	DWORD		cbSize;                 // sizeof(RoleInfo)
	LPCWSTR		pwcsRoleName;			// 昵称
	int			nSex;                   // 性别(0: 女; 1: 男)
	// more
};

struct SDOAWinProperty{
	int nWinType;				// 窗口类型(只用于标识窗口类型，设置无效)
	int nStatus;				// 窗口状态
	// 以下是位置、宽高信息
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;	
};


struct LoginResult {
	DWORD  cbSize;			// sizeof(LoginResult)，为以后扩充提供可能
	LPCSTR szSessionId;		// 用于后台验证的token串
	LPCSTR szSndaid;		// 用户ID
	LPCSTR szIdentityState;	// 是否成年标识，0未成年，1成年
	LPCSTR szAppendix;		// 附加字段，保留

};

 struct LoginMessage {
   DWORD  dwSize;
   int    nErrorCode;   // 登录组件返回的错误码,仅供参考，详细内容请用pbstrContent
   BSTR*  pbstrTitle;	// 提示标题，可以调用SysReallocString修改其中的内容，注意，回调结果后IGW将调用SysFreeString释放已分配的内存
   BSTR*  pbstrContent;	// 提示内容，可以调用SysReallocString修改其中的内容，注意，回调结果后IGW将调用SysFreeString释放已分配的内存
 };

// LPLOGINCALLBACKPROC中回调参数ErrorCode值定义
#define SDOA_ERRORCODE_OK               0    // 登录成功
#define SDOA_ERRORCODE_CANCEL           -1   // 用户按取消关闭了登录对话框
#define SDOA_ERRORCODE_UILOST           -2   // 加载对话框UI文件缺失或无效
#define SDOA_ERRORCODE_FAILED           -3   // 登录失败
#define SDOA_ERRORCODE_UNKNOWN          -4   // 未知错误
#define SDOA_ERRORCODE_INVALIDPARAM     -5   // 参数无效
#define SDOA_ERRORCODE_UNEXCEPT         -6   // 未知异常
#define SDOA_ERRORCODE_ALREAYEXISTS     -7   // 已经存在
#define SDOA_ERRORCODE_SHOWMESSAGE      -10  // 显示对示对话框


// 登录回调函数
// nErrorCode:     登录返回错误代码
// pLoginResult:   登录返回结果，只在 nErrorCode = SDOA_ERRORCODE_OK 时才有效
// nUserData:      在调用ShowLoginDialog时传入的用户数据
// nReserved:      保留，暂不使用，目前总是为0(不等于ShowLoginDialog中传入的Reserved参数)
// 返回值:        为TRUE将关闭对话框，为FALSE不关闭
//
// 2009.03.30
// 扩展功能，当nErrorCode = ERRORCODE_SHOWMESSAGE时，Reserved返回LoginMessage*结果
// 返回TRUE时，IGW将不再弹出对话框，游戏可以自行弹出提示对话框等
// 返回FALSE时，IGW继续弹出对话框，可以调用SysReallocString修改Title,Content其中的内容，以便游戏显示自定义的错误提示
//              注意：回调结果后IGW将调用SysFreeString释放已分配的内存
typedef BOOL (CALLBACK *LPLOGINCALLBACKPROC)(int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved);

// 应用接口
typedef interface ISDOAApp ISDOAApp;
typedef ISDOAApp *PSDOAApp, *LPSDOAApp;

MIDL_INTERFACE("C8187484-1C5F-48DF-8497-2136A72A5733")
ISDOAApp : public IUnknown
{
public:
	// 显示登录对话框
	STDMETHOD_(int,ShowLoginDialog)(THIS_ LPLOGINCALLBACKPROC fnCallback, int nUserData, int nReserved) PURE;

	STDMETHOD_(void,ModifyAppInfo)(THIS_ const AppInfo* pAppInfo) PURE;
	STDMETHOD_(void,Logout)(THIS) PURE;
	STDMETHOD_(int,GetLoginState)(THIS_ int nLoginMethod) PURE;
	STDMETHOD_(void,SetRoleInfo)(THIS_ const RoleInfo* pRoleInfo) PURE;
	

	// 显示充值对话框 
	STDMETHOD_(int,ShowPaymentDialog)(THIS_ LPCWSTR pwcsSrc) PURE;

	// 配置SDOA UI相关
	STDMETHOD_(int,GetScreenStatus)(THIS) PURE;
	STDMETHOD_(void,SetScreenStatus)(THIS_ int nValue) PURE;
	STDMETHOD_(bool,GetScreenEnabled)(THIS) PURE;
	STDMETHOD_(void,SetScreenEnabled)(THIS_ bool bValue) PURE;
	STDMETHOD_(bool,GetTaskBarPosition)(THIS_ PPOINT ptPosition) PURE;
	STDMETHOD_(bool,SetTaskBarPosition)(THIS_ const PPOINT ptPosition) PURE;
	STDMETHOD_(bool,GetFocus)(THIS) PURE;
	STDMETHOD_(void,SetFocus)(THIS_ bool bValue) PURE;
	STDMETHOD_(bool,HasUI)(THIS_ const PPOINT ptPosition) PURE;
	
	STDMETHOD_(int,GetTaskBarMode)(THIS) PURE;
	STDMETHOD_(void,SetTaskBarMode)(THIS_ int nBarMode) PURE;
	STDMETHOD_(int,GetSelfCursor)(THIS) PURE;
	STDMETHOD_(void,SetSelfCursor)(THIS_ int nValue) PURE;

	// 功能相关
	STDMETHOD_(int,OpenWidget)(THIS_ LPCWSTR pwcsWidgetNameSpace) PURE;
	STDMETHOD_(int,OpenWidgetEx)(THIS_ LPCWSTR pwcsWidgetNameSpace, LPCWSTR pwcsParam, int nFlag) PURE;
	STDMETHOD_(int,CloseWidget)(THIS_ LPCWSTR pwcsWidgetNameSpace) PURE;
	STDMETHOD_(int,ExecuteWidget)(THIS_ LPCWSTR pwcsWidgetNameSpaceOrGUID, LPCWSTR pwcsParam) PURE;
	STDMETHOD_(int,WidgetExists)(THIS_ LPCWSTR pwcsWidgetNameSpaceOrGUID) PURE;
	STDMETHOD_(int,OpenWindow)(THIS_ LPCWSTR pwcsWinType, LPCWSTR pwcsWinName, LPCWSTR pwcsSrc, int nLeft, int nTop, int nWidth, int nHeight, LPCWSTR pwcsMode) PURE;
	STDMETHOD_(int,CloseWindow)(THIS_ LPCWSTR pwcsWinName) PURE;

	STDMETHOD_(int,GetWinProperty)(THIS_ LPCWSTR pwcsWinName, SDOAWinProperty* pWinProperty) PURE;
	STDMETHOD_(int,SetWinProperty)(THIS_ LPCWSTR pwcsWinName, SDOAWinProperty* pWinProperty) PURE;
};


// 游戏实现的声音控制回调
// 注意音量值的有效范围在0-100
typedef DWORD (CALLBACK *LPGETAUDIOSOUNDVOLUME)();
typedef void  (CALLBACK *LPSETAUDIOSOUNDVOLUME)(DWORD nNewVolume);
typedef DWORD (CALLBACK *LPGETAUDIOEFFECTVOLUME)();
typedef void  (CALLBACK *LPSETAUDIOEFFECTVOLUME)(DWORD nNewVolume);

// 通用数据传输接口
class ICommonChannel
{
public:
	// 下列三个方法是为兼容Delphi的接口而设置，在C++中不需要实现，但需要保留在此位置
	virtual HRESULT _stdcall QueryInterface(REFIID riid, void **ppvObject) = 0;
	virtual ULONG _stdcall AddRef(void) = 0;
	virtual ULONG _stdcall Release(void) = 0;

	virtual void _stdcall SendData(BSTR Request, BSTR* Response) = 0;
	virtual void _stdcall Set_RevertChannel(ICommonChannel *pReverter) = 0;
	virtual ICommonChannel* _stdcall Get_RevertChannel(void) = 0;
	virtual void _stdcall Close(void) = 0;
	virtual void _stdcall Get_ChannelType(LPSTR szChannelType, DWORD nBufferLen) = 0; // max BufferLen = 64
};
typedef ICommonChannel *PCommonChannel;

typedef BOOL (WINAPI *LPCREATECHANNEL)(LPCSTR szChannelType, ICommonChannel **pChannel);


typedef interface ISDOAAppUtils ISDOAAppUtils;
typedef ISDOAAppUtils *PSDOAAppUtils, *LPSDOAAppUtils;

MIDL_INTERFACE("1170C2F9-28AD-4EA8-8392-E9A219C8FF65")
ISDOAAppUtils : public IUnknown
{
public:
	// 声音控制
	// 设置声音控制相关回调
	STDMETHOD_(void,SetAudioSetting)(THIS_ 
		LPGETAUDIOSOUNDVOLUME  fnGetAudioSoundVolume,
		LPSETAUDIOSOUNDVOLUME  fnSetAudioSoundVolume,
		LPGETAUDIOEFFECTVOLUME fnGetAudioEffectVolume,
		LPSETAUDIOEFFECTVOLUME fnSetAudioEffectVolume ) PURE;
	// 通知SDOA游戏声音有变化                          
	STDMETHOD_(void,NodifyAudioChanged)(THIS) PURE;	
	// 设置创建数据通道的回调
	STDMETHOD_(void,SetCreateChannelCallback)(THIS_ const LPCREATECHANNEL fnCreateChannel) PURE;	
};

// 异步回调
typedef void (CALLBACK *LPQUERYCALLBACK)(int nRetCode, int nUserData);

typedef interface ISDOAClientService ISDOAClientService;
typedef ISDOAClientService *PSDOAClientService, *LPSDOAClientService;

MIDL_INTERFACE("AF56D291-823A-41AA-85A0-EBE5C6163425")
ISDOAClientService : public IUnknown
{
public:
	STDMETHOD_(int, SetValue)(THIS_ const BSTR bstrKey, const BSTR bstrValue ) PURE;
	STDMETHOD_(int, Query)(THIS_ const BSTR bstrService) PURE;	
	STDMETHOD_(int, AsyncQuery)(THIS_ const BSTR bstrService, LPQUERYCALLBACK fnCallback, int nUserData) PURE;	
	STDMETHOD_(int, GetValue)(THIS_ const BSTR bstrKey, BSTR* bstrValue) PURE;	
};

/* 注意igwInitialize中dwSdkVersion参数使用宏 SDOA_SDK_VERSION */
typedef int  (WINAPI* LPigwInitialize)(DWORD dwSdkVersion, const AppInfo* pAppInfo);
typedef bool (WINAPI* LPigwGetModule)(REFIID riid, void** intf);
typedef int  (WINAPI* LPigwTerminal)();

#pragma pack(pop)
#endif /* SDOA4CLIENT_H */