#pragma once

// 数学运算数值常数
#ifndef M_PI
#define M_PI    3.141592653589793238460f
#define M_PI_2  1.570796326794896619230f
#define M_PI_4  0.785398163397448309616f
#define M_1_PI  0.318309886183790671538f
#define M_2_PI  0.636619772367581343076f
#endif

// 硬件颜色相关宏定义
#define ARGB(a,r,g,b) ((DWORD(a)<<24) + \
                        (DWORD(r)<<16) + \
                        (DWORD(g)<<8) + \
                        DWORD(b))

#define GETA(col)       ((col)>>24)
#define GETR(col)       (((col)>>16) & 0xFF)
#define GETG(col)       (((col)>>8) & 0xFF)
#define GETB(col)       ((col) & 0xFF)
#define SETA(col,a)     (((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)     (((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)     (((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)     (((col) & 0xFFFFFF00) + DWORD(b))


// Ms2DEngine混合常数
#define    BLEND_COLORADD   1   // 加
#define    BLEND_COLORMUL   0   // 乘
#define    BLEND_ALPHABLEND 2   // 透明混合
#define    BLEND_ALPHAADD   0   // 透明加
#define    BLEND_ZWRITE     4   // 写Z
#define    BLEND_NOZWRITE   0   // 不写Z

// 默认无Z混合
#define     BLEND_DEFAULT (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)

// 默认有Z混合
#define     BLEND_DEFAULT_Z (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)


// 引擎回调函数
class CMs2DEngine;
typedef BOOL(*Ms2DCBFun)(CMs2DEngine*);

// Ms2DEngine系统状态常量

// 布尔类型状态
enum E_MS2DENGINE_STATE_BOOL
{
    M2DE_WINDOWED       = 1,    // BOOL 是否窗口化运行          (默认: FALSE)
    M2DE_ZBUFFER        = 2,    // BOOL 是否使用Z缓冲           (默认: FALSE)
    M2DE_TEXTUREFILTER  = 3,    // BOOL 是否使用纹理过滤        (默认: TRUE)
    M2DE_USESOUND       = 4,    // BOOL 是否使用声音            (默认: TRUE)
    M2DE_DONTSUSPEND    = 5,    // BOOL 失去焦点时是否不暂停    (默认: FALSE)
    M2DE_HIDEMOUSE      = 6,    // BOOL 是否隐藏系统光标        (默认: TRUE)
    M2DE_SHOWSPLASH     = 7,    // BOOL 是否显示启动LOGO        (默认: TRUE)

    M2DEBOOLSTATE_FORCE_DWORD = 0x7FFFFFFF
};

// 回调函数类型状态
enum E_MS2DENGINE_STATE_FUNC
{

    M2DE_FRAMEFUNC      = 8,    // Ms2DCBFun  框架回调函数      (默认: NULL) 
                                // (此项必须设置)

    M2DE_RENDERFUNC     = 9,    // Ms2DCBFun  渲染回调函数      (默认: NULL)
    M2DE_FOCUSLOSTFUNC  = 10,   // Ms2DCBFun  失去焦点处理函数  (默认: NULL)
    M2DE_FOCUSGAINFUNC  = 11,   // Ms2DCBFun  获取焦点处理函数  (默认: NULL)
    M2DE_GFXRESTOREFUNC = 12,   // Ms2DCBFun  重建窗口处理函数  (默认: NULL)
    M2DE_EXITFUNC       = 13,   // Ms2DCBFun  退出处理函数      (默认: NULL)

    M2DEFUNCSTATE_FORCE_DWORD = 0x7FFFFFFF
};

// 句柄类型状态
enum E_MS2DENGINE_STATE_HWND
{
    M2DE_HWND           = 15,    // HWND    获取窗口句柄        (只读)
    M2DE_HWNDPARENT     = 16,    // HWND    设置父窗口句柄      (默认: NULL)

    M2DEHWNDSTATE_FORCE_DWORD = 0x7FFFFFFF
};

// 整型类型状态
enum E_MS2DENGINE_STATE_INT
{
    M2DE_SCREENWIDTH    = 17,   // INT  窗口宽度                (默认: 800)
    M2DE_SCREENHEIGHT   = 18,   // INT  窗口高度                (默认: 600)
    M2DE_SCREENBPP      = 19,   // INT  窗口颜色深度            (默认: 32bit) 
                                // (窗口模式时使用桌面颜色深度)

    M2DE_SAMPLERATE     = 20,   // INT  声音比特率              (默认: 44100)
    M2DE_FXVOLUME       = 21,   // INT  全局音量                (默认: 100)
    M2DE_MUSVOLUME      = 22,   // INT  音乐音量                (默认: 100)
    M2DE_STREAMVOLUME   = 23,   // INT  流音量                  (默认: 100)
    M2DE_FPS            = 24,   // INT  固定帧(默认: M2DEFPS_UNLIMITED 无限制)
    M2DE_POWERSTATUS    = 25,   // INT  电池容量与电源状态      (只读)

    M2DEINTSTATE_FORCE_DWORD = 0x7FFFFFF
};

// 字符串类型状态
enum E_MS2DENGINE_STATE_STRING
{
    M2DE_ICON           = 26,   // LPTCHAR  图标资源            (默认: NULL)
    M2DE_TITLE          = 27,   // LPTCHAR  窗口标题            (默认: "M2DE")
    M2DE_INIFILE        = 28,   // LPTCHAR  INI配置文件(默认: NULL)(表示没有)
    M2DE_LOGFILE        = 29,   // LPTCHAR  LOG日志文件(默认: NULL)(表示没有)

    M2DESTRINGSTATE_FORCE_DWORD = 0x7FFFFFFF
};

//━━━〖MS2DEngine输入事件类型常量〗━━━━━━┓
// 输入类型                                     //┃
enum E_MS2D_INPUT_TYPE                          //┃
{                                               //┃
    INPUT_UNKNOW = 0,   // 未知类型             //┃
    INPUT_KEYDOWN,      // 按键按下             //┃
    INPUT_KEYUP,        // 按键松开             //┃
    INPUT_MBUTTONDOWN,  // 鼠标键按下           //┃
    INPUT_MBUTTONUP,    // 鼠标键松开           //┃
    INPUT_MOUSEMOVE,    // 鼠标移动             //┃
    INPUT_MOUSEWHEEL,   // 鼠标滚轮             //┃
};                                              //┃
// 输入标记                                     //┃
enum E_MS2D_INPUT_FLAG                          //┃
{                                               //┃
    INPUT_FLAG_NO_FLAG      = 0,  // 无标记     //┃
    INPUT_FLAG_SHIFT        = 1,  // 按下Shift  //┃
    INPUT_FLAG_CTRL         = 2,  // 按下Ctrl   //┃
    INPUT_FLAG_ALT          = 4,  // 按下Alt    //┃
    INPUT_FLAG_CAPSLOCK     = 8,  // 启用大写   //┃
    INPUT_FLAG_SCROLLLOCK   = 16, // 启用滚动   //┃
    INPUT_FLAG_NUMLOCK      = 32, // 启用小区   //┃
    INPUT_FLAG_REPEAT       = 64, // 重复       //┃
};                                              //┃
// 2D引擎输入事件最大个数                       //┃
#define MS2D_INPUT_EVENT_MAX_COUNT  1024        //┃
// 2D引擎输入按键个数                           //┃
#define MS2D_INPUT_MAX_KEY_COUNT    256         //┃
                                                //┃
// CMs2DEngine输入事件结构体                    //┃
struct CMs2DInputEvent                          //┃
{                                               //┃
    E_MS2D_INPUT_TYPE   eType;  // 事件类型     //┃
    DWORD               dwKey;  // 按键虚码     //┃
    DWORD               dwFlags;// 事件标记     //┃
    WORD                wChar;  // 按键字符码   //┃
    INT                 nWheel; // 滚轮移动     //┃
    LONG                lX;     // 鼠标x坐标    //┃
    LONG                lY;     // 鼠标y坐标    //┃
};                                              //┃
                                                //┃
// CMs2DEngine输入事件队列                      //┃
struct Ms2DInputEventQueue                      //┃
{                                               //┗━━━━━━━━━━━━┓
    Ms2DInputEventQueue(VOID)                                             //┃
        : m_CMsQueueManager (MS2D_INPUT_EVENT_MAX_COUNT)                  //┃
    {                                                                     //┃
        ZeroMemoryArray(m_pInputEventQueue);                              //┃
    }                                                                     //┃
                                                                          //┃
    // 输入事件队列                                                       //┃
    CMs2DInputEvent m_pInputEventQueue[MS2D_INPUT_EVENT_MAX_COUNT];       //┃
                                                                          //┃
    // 队列                                                               //┃
    CMsQueueManager m_CMsQueueManager;                                    //┃
    CMs2DInputEvent* GetNextFreeEvent(VOID)                               //┃
    {                                                                     //┃
        return &m_pInputEventQueue[m_CMsQueueManager.GetNextIndex(TRUE)]; //┃
    }                                                                     //┃
};                                                                        //┃
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

// Ms2D顶点结构体
struct S_MS2D_VERTEX
{
    FLOAT   fX, fY;     // 屏幕坐标    
    FLOAT   fZ;         // Z缓冲范围0.0f ~ 1.0f
    DWORD   dwColor;    // 颜色
    FLOAT   ftx, fty;   // 纹理坐标
};

// 输入按键
enum E_INPUT_KEY
{
    MS2DK_EBUTTON   = 0x00,
    MS2DK_LBUTTON   = 0x01,
    MS2DK_RBUTTON   = 0x02,
    MS2DK_MBUTTON   = 0x04,

    MS2DK_ESCAPE    = 0x1B,
    MS2DK_BACKSPACE = 0x08,
    MS2DK_TAB       = 0x09,
    MS2DK_ENTER     = 0x0D,
    MS2DK_SPACE     = 0x20,

    MS2DK_SHIFT = 0x10,
    MS2DK_CTRL  = 0x11,
    MS2DK_ALT   = 0x12,

    MS2DK_LWIN  = 0x5B,
    MS2DK_RWIN  = 0x5C,
    MS2DK_APPS  = 0x5D,

    MS2DK_PAUSE     = 0x13,
    MS2DK_CAPSLOCK  = 0x14,
    MS2DK_NUMLOCK   = 0x90,
    MS2DK_SCROLLLOCK= 0x91,

    MS2DK_PGUP  = 0x21,
    MS2DK_PGDN  = 0x22,
    MS2DK_HOME  = 0x24,
    MS2DK_END   = 0x23,
    MS2DK_INSERT= 0x2D,
    MS2DK_DELETE= 0x2E,

    MS2DK_LEFT  = 0x25,
    MS2DK_UP    = 0x26,
    MS2DK_RIGHT = 0x27,
    MS2DK_DOWN  = 0x28,

    MS2DK_0 = 0x30,
    MS2DK_1 = 0x31,
    MS2DK_2 = 0x32,
    MS2DK_3 = 0x33,
    MS2DK_4 = 0x34,
    MS2DK_5 = 0x35,
    MS2DK_6 = 0x36,
    MS2DK_7 = 0x37,
    MS2DK_8 = 0x38,
    MS2DK_9 = 0x39,

    MS2DK_A = 0x41,
    MS2DK_B = 0x42,
    MS2DK_C = 0x43,
    MS2DK_D = 0x44,
    MS2DK_E = 0x45,
    MS2DK_F = 0x46,
    MS2DK_G = 0x47,
    MS2DK_H = 0x48,
    MS2DK_I = 0x49,
    MS2DK_J = 0x4A,
    MS2DK_K = 0x4B,
    MS2DK_L = 0x4C,
    MS2DK_M = 0x4D,
    MS2DK_N = 0x4E,
    MS2DK_O = 0x4F,
    MS2DK_P = 0x50,
    MS2DK_Q = 0x51,
    MS2DK_R = 0x52,
    MS2DK_S = 0x53,
    MS2DK_T = 0x54,
    MS2DK_U = 0x55,
    MS2DK_V = 0x56,
    MS2DK_W = 0x57,
    MS2DK_X = 0x58,
    MS2DK_Y = 0x59,
    MS2DK_Z = 0x5A,

    MS2DK_GRAVE     = 0xC0,
    MS2DK_MINUS     = 0xBD,
    MS2DK_EQUALS    = 0xBB,
    MS2DK_BACKSLASH = 0xDC,
    MS2DK_LBRACKET  = 0xDB,
    MS2DK_RBRACKET  = 0xDD,
    MS2DK_SEMICOLON = 0xBA,
    MS2DK_APOSTROPHE= 0xDE,
    MS2DK_COMMA     = 0xBC,
    MS2DK_PERIOD    = 0xBE,
    MS2DK_SLASH     = 0xBF,

    MS2DK_NUMPAD0   = 0x60,
    MS2DK_NUMPAD1   = 0x61,
    MS2DK_NUMPAD2   = 0x62,
    MS2DK_NUMPAD3   = 0x63,
    MS2DK_NUMPAD4   = 0x64,
    MS2DK_NUMPAD5   = 0x65,
    MS2DK_NUMPAD6   = 0x66,
    MS2DK_NUMPAD7   = 0x67,
    MS2DK_NUMPAD8   = 0x68,
    MS2DK_NUMPAD9   = 0x69,

    MS2DK_MULTIPLY  = 0x6A,
    MS2DK_DIVIDE    = 0x6F,
    MS2DK_ADD       = 0x6B,
    MS2DK_SUBTRACT  = 0x6D,
    MS2DK_DECIMAL   = 0x6E,

    MS2DK_F1    = 0x70,
    MS2DK_F2    = 0x71,
    MS2DK_F3    = 0x72,
    MS2DK_F4    = 0x73,
    MS2DK_F5    = 0x74,
    MS2DK_F6    = 0x75,
    MS2DK_F7    = 0x76,
    MS2DK_F8    = 0x77,
    MS2DK_F9    = 0x78,
    MS2DK_F10   = 0x79,
    MS2DK_F11   = 0x7A,
    MS2DK_F12   = 0x7B,
};

// D3D渲染格式
#define MS_D3DFORMAT D3DFMT_A8R8G8B8