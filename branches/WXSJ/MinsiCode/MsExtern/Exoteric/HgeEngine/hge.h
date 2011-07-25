/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** System layer API
*/


#ifndef HGE_H
#define HGE_H


#include <windows.h>
//#pragma warning(disable : 4996)
#pragma comment(lib, "Winmm.lib")
#define HGE_VERSION 0x180

#ifdef HGEDLL
#define EXPORT  __declspec(dllexport)
#else
#define EXPORT
#endif

#define CALL  __stdcall

#ifdef __BORLANDC__
 #define floorf (FLOAT)floor
 #define sqrtf (FLOAT)sqrt
 #define acosf (FLOAT)acos
 #define atan2f (FLOAT)atan2
 #define cosf (FLOAT)cos
 #define sinf (FLOAT)sin
 #define powf (FLOAT)pow
 #define fabsf (FLOAT)fabs

 #define min(x,y) ((x) < (y)) ? (x) : (y)
 #define max(x,y) ((x) > (y)) ? (x) : (y)
#endif


/*
** Common data types
*/
#ifndef DWORD
typedef unsigned long   DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;
#endif


/*
** Common math constants
*/
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#define M_PI_2  1.57079632679489661923f
#define M_PI_4  0.785398163397448309616f
#define M_1_PI  0.318309886183790671538f
#define M_2_PI  0.636619772367581343076f
#endif


/*
** HGE Handle types
*/
typedef DWORD HTEXTURE;
typedef DWORD HTARGET;
typedef DWORD HEFFECT;
typedef DWORD HMUSIC;
typedef DWORD HSTREAM;
typedef DWORD HCHANNEL;


/*
** Hardware color macros
*/
#define ARGB(a,r,g,b)   ((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)       ((col)>>24)
#define GETR(col)       (((col)>>16) & 0xFF)
#define GETG(col)       (((col)>>8) & 0xFF)
#define GETB(col)       ((col) & 0xFF)
#define SETA(col,a)     (((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)     (((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)     (((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)     (((col) & 0xFFFFFF00) + DWORD(b))


/*
** HGE Blending constants
*/
#define    BLEND_COLORADD   1
#define    BLEND_COLORMUL   0
#define    BLEND_ALPHABLEND 2
#define    BLEND_ALPHAADD   0
#define    BLEND_ZWRITE     4
#define    BLEND_NOZWRITE   0

#define BLEND_DEFAULT   (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)
#define BLEND_DEFAULT_Z (BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)


/*
** HGE System state constants
*/
enum hgeBoolState
{
    HGE_WINDOWED        = 1,    // BOOL         是否窗口化运行           (默认: FALSE)
    HGE_ZBUFFER         = 2,    // BOOL         是否使用Z缓冲            (默认: FALSE)
    HGE_TEXTUREFILTER   = 3,    // BOOL         是否使用纹理过滤         (默认: TRUE)
    
    HGE_USESOUND        = 4,    // BOOL         是否使用BASS为声音引擎   (默认: TRUE)
    
    HGE_DONTSUSPEND     = 5,    // BOOL         失去焦点时是否不暂停     (默认: FALSE)
    HGE_HIDEMOUSE       = 6,    // BOOL         是否隐藏系统光标         (默认: TRUE)

    HGE_SHOWSPLASH      = 7,    // BOOL         是否显示启动LOGO?        (默认: TRUE)

    HGEBOOLSTATE_FORCE_DWORD = 0x7FFFFFFF
};

enum hgeFuncState
{
    HGE_FRAMEFUNC       = 8,    // hgeCallback  框架回调函数            (默认: NULL) (此项必须设置)
    HGE_RENDERFUNC      = 9,    // hgeCallback  渲染回调函数            (默认: NULL)
    HGE_FOCUSLOSTFUNC   = 10,   // hgeCallback  失去焦点处理函数        (默认: NULL)
    HGE_FOCUSGAINFUNC   = 11,   // hgeCallback  获取焦点处理函数        (默认: NULL)
    HGE_GFXRESTOREFUNC  = 12,   // hgeCallback  重建窗口处理函数        (默认: NULL)
    HGE_EXITFUNC        = 13,   // hgeCallback  退出处理函数            (默认: NULL)
    
    HGEFUNCSTATE_FORCE_DWORD = 0x7FFFFFFF
};

enum hgeHwndState
{
    HGE_HWND            = 15,    // INT         获取窗口句柄             (只读)
    HGE_HWNDPARENT      = 16,    // INT         设置父窗口句柄           (默认: NULL)
    
    HGEHWNDSTATE_FORCE_DWORD = 0x7FFFFFFF
};

enum hgeIntState
{
    HGE_SCREENWIDTH     = 17,   // INT          窗口宽度                  (默认: 800)
    HGE_SCREENHEIGHT    = 18,   // INT          窗口高度                  (默认: 600)
    HGE_SCREENBPP       = 19,   // INT          窗口颜色深度              (默认: 32位) (窗口模式时使用桌面颜色深度)
    
    HGE_SAMPLERATE      = 20,   // INT          声音比特率                (默认: 44100)
    HGE_FXVOLUME        = 21,   // INT          全局音量                  (默认: 100)
    HGE_MUSVOLUME       = 22,   // INT          音乐音量                  (默认: 100)
    HGE_STREAMVOLUME    = 23,   // INT          流音量                    (默认: 100)
    
    HGE_FPS             = 24,   // INT          固定帧率                  (默认: HGEFPS_UNLIMITED 无限制)

    HGE_POWERSTATUS     = 25,   // INT          battery life percent + status
    
    HGEINTSTATE_FORCE_DWORD = 0x7FFFFFF
};

enum hgeStringState
{
    HGE_ICON            = 26,   // TCHAR*        图标资源                    (默认: NULL)
    HGE_TITLE           = 27,   // TCHAR*        窗口标题                    (默认: "HGE")
    
    HGE_INIFILE         = 28,   // TCHAR*        INI配置文件                 (默认: NULL)(表示没有文件)
    HGE_LOGFILE         = 29,   // TCHAR*        LOG日志文件                 (默认: NULL)(表示没有文件)

    HGESTRINGSTATE_FORCE_DWORD = 0x7FFFFFFF
};

/*
** Callback protoype used by HGE
*/
typedef BOOL (*hgeCallback)();


/*
** HGE_FPS system state special constants
*/
#define HGEFPS_UNLIMITED    0
#define HGEFPS_VSYNC        -1


/*
** HGE_POWERSTATUS system state special constants
*/
#define HGEPWR_AC           -1
#define HGEPWR_UNSUPPORTED  -2


/*
** HGE Primitive type constants
*/
#define HGEPRIM_LINES   2
#define HGEPRIM_TRIPLES 3
#define HGEPRIM_QUADS   4


/*
** HGE Vertex structure
*/
struct hgeVertex
{
    FLOAT   x, y;   // screen position    
    FLOAT   z;      // Z-buffer depth 0..1
    DWORD   col;    // color
    FLOAT   tx, ty; // texture coordinates
};


/*
** HGE Triple structure
*/
struct hgeTriple
{
    hgeVertex   v[3];
    HTEXTURE    tex;
    INT         blend;
};


/*
** HGE Quad structure
*/
struct hgeQuad
{
    hgeVertex   v[4];
    HTEXTURE    tex;
    INT         blend;
};


/*
** HGE Input Event structure
*/
struct hgeInputEvent
{
    INT     type;   // event type
    INT     key;    // key code
    INT     flags;  // event flags
    INT     chr;    // character code
    INT     wheel;  // wheel shift
    FLOAT   x;      // mouse cursor x-coordinate
    FLOAT   y;      // mouse cursor y-coordinate
};


/*
** HGE Input Event type constants
*/
#define INPUT_KEYDOWN       1
#define INPUT_KEYUP         2
#define INPUT_MBUTTONDOWN   3
#define INPUT_MBUTTONUP     4
#define INPUT_MOUSEMOVE     5
#define INPUT_MOUSEWHEEL    6


/*
** HGE Input Event flags
*/
#define HGEINP_SHIFT        1
#define HGEINP_CTRL         2
#define HGEINP_ALT          4
#define HGEINP_CAPSLOCK     8
#define HGEINP_SCROLLLOCK   16
#define HGEINP_NUMLOCK      32
#define HGEINP_REPEAT       64


/*
** HGE Interface class
*/
class HGE
{
public:
    virtual VOID        CALL    Release() = 0;

    virtual BOOL        CALL    System_Initiate() = 0;
    virtual VOID        CALL    System_Shutdown() = 0;
    virtual BOOL        CALL    System_Start() = 0;
    virtual TCHAR*       CALL    System_GetErrorMessage() = 0;
    virtual VOID        CALL    System_Log(CONST TCHAR *format, ...) = 0;
    virtual BOOL        CALL    System_Launch(CONST TCHAR *url) = 0;
    virtual VOID        CALL    System_Snapshot(CONST TCHAR *filename=0) = 0;

private:
    virtual VOID        CALL    System_SetStateBool  (hgeBoolState   state, BOOL        value) = 0;
    virtual VOID        CALL    System_SetStateFunc  (hgeFuncState   state, hgeCallback value) = 0;
    virtual VOID        CALL    System_SetStateHwnd  (hgeHwndState   state, HWND        value) = 0;
    virtual VOID        CALL    System_SetStateInt   (hgeIntState    state, INT         value) = 0;
    virtual VOID        CALL    System_SetStateString(hgeStringState state, CONST TCHAR *value) = 0;
    virtual BOOL        CALL    System_GetStateBool  (hgeBoolState   state) = 0;
    virtual hgeCallback CALL    System_GetStateFunc  (hgeFuncState   state) = 0;
    virtual HWND        CALL    System_GetStateHwnd  (hgeHwndState   state) = 0;
    virtual INT         CALL    System_GetStateInt   (hgeIntState    state) = 0;
    virtual CONST TCHAR* CALL    System_GetStateString(hgeStringState state) = 0;

public:
    inline VOID         System_SetState(hgeBoolState   state, BOOL        value) { System_SetStateBool  (state, value); }
    inline VOID         System_SetState(hgeFuncState   state, hgeCallback value) { System_SetStateFunc  (state, value); }
    inline VOID         System_SetState(hgeHwndState   state, HWND        value) { System_SetStateHwnd  (state, value); }
    inline VOID         System_SetState(hgeIntState    state, INT         value) { System_SetStateInt   (state, value); }
    inline VOID         System_SetState(hgeStringState state, CONST TCHAR *value) { System_SetStateString(state, value); }
    inline BOOL         System_GetState(hgeBoolState   state) { return System_GetStateBool  (state); }
    inline hgeCallback  System_GetState(hgeFuncState   state) { return System_GetStateFunc  (state); }
    inline HWND         System_GetState(hgeHwndState   state) { return System_GetStateHwnd  (state); }
    inline INT          System_GetState(hgeIntState    state) { return System_GetStateInt   (state); }
    inline CONST TCHAR*  System_GetState(hgeStringState state) { return System_GetStateString(state); }
    
    virtual VOID*       CALL    Resource_Load(CONST TCHAR *filename, DWORD *size=0) = 0;
    virtual VOID        CALL    Resource_Free(VOID *res) = 0;
    virtual BOOL        CALL    Resource_AttachPack(CONST TCHAR *filename, CONST TCHAR *password=0) = 0;
    virtual VOID        CALL    Resource_RemovePack(CONST TCHAR *filename) = 0;
    virtual VOID        CALL    Resource_RemoveAllPacks() = 0;
    virtual TCHAR*       CALL    Resource_MakePath(CONST TCHAR *filename=0) = 0;
    virtual TCHAR*       CALL    Resource_EnumFiles(CONST TCHAR *wildcard=0) = 0;
    virtual TCHAR*       CALL    Resource_EnumFolders(CONST TCHAR *wildcard=0) = 0;

    virtual VOID        CALL    Ini_SetInt(CONST TCHAR *section, CONST TCHAR *name, INT value) = 0;
    virtual INT         CALL    Ini_GetInt(CONST TCHAR *section, CONST TCHAR *name, INT def_val) = 0;
    virtual VOID        CALL    Ini_SetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT value) = 0;
    virtual FLOAT       CALL    Ini_GetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT def_val) = 0;
    virtual VOID        CALL    Ini_SetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *value) = 0;
    virtual TCHAR*       CALL    Ini_GetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *def_val) = 0;

    virtual VOID        CALL    Random_Seed(INT seed=0) = 0;
    virtual INT         CALL    Random_Int(INT min, INT max) = 0;
    virtual FLOAT       CALL    Random_Float(FLOAT min, FLOAT max) = 0;

    virtual FLOAT       CALL    Timer_GetTime() = 0;
    virtual FLOAT       CALL    Timer_GetDelta() = 0;
    virtual INT         CALL    Timer_GetFPS() = 0;

    virtual HEFFECT     CALL    Effect_Load(CONST TCHAR *filename, DWORD size=0) = 0;
    virtual VOID        CALL    Effect_Free(HEFFECT eff) = 0;
    virtual HCHANNEL    CALL     Effect_Play(HEFFECT eff) = 0;
    virtual HCHANNEL    CALL    Effect_PlayEx(HEFFECT eff, INT volume=100, INT pan=0, FLOAT pitch=1.0f, BOOL loop=FALSE) = 0;

    virtual HMUSIC      CALL    Music_Load(CONST TCHAR *filename, DWORD size=0) = 0;
    virtual VOID        CALL    Music_Free(HMUSIC mus) = 0;
    virtual HCHANNEL    CALL    Music_Play(HMUSIC mus, BOOL loop, INT volume = 100, INT order = -1, INT row = -1) = 0;
    virtual VOID        CALL    Music_SetAmplification(HMUSIC music, INT ampl) = 0;
    virtual INT         CALL    Music_GetAmplification(HMUSIC music) = 0;
    virtual INT         CALL    Music_GetLength(HMUSIC music) = 0;
    virtual VOID        CALL    Music_SetPos(HMUSIC music, INT order, INT row) = 0;
    virtual BOOL        CALL    Music_GetPos(HMUSIC music, INT *order, INT *row) = 0;
    virtual VOID        CALL    Music_SetInstrVolume(HMUSIC music, INT instr, INT volume) = 0;
    virtual INT         CALL    Music_GetInstrVolume(HMUSIC music, INT instr) = 0;
    virtual VOID        CALL    Music_SetChannelVolume(HMUSIC music, INT channel, INT volume) = 0;
    virtual INT         CALL    Music_GetChannelVolume(HMUSIC music, INT channel) = 0;

    virtual HSTREAM     CALL    Stream_Load(CONST TCHAR *filename, DWORD size=0) = 0;
    virtual VOID        CALL    Stream_Free(HSTREAM stream) = 0;
    virtual HCHANNEL    CALL    Stream_Play(HSTREAM stream, BOOL loop, INT volume = 100) = 0;

    virtual VOID        CALL    Channel_SetPanning(HCHANNEL chn, INT pan) = 0;
    virtual VOID        CALL     Channel_SetVolume(HCHANNEL chn, INT volume) = 0;
    virtual VOID        CALL     Channel_SetPitch(HCHANNEL chn, FLOAT pitch) = 0;
    virtual VOID        CALL     Channel_Pause(HCHANNEL chn) = 0;
    virtual VOID        CALL     Channel_Resume(HCHANNEL chn) = 0;
    virtual VOID        CALL     Channel_Stop(HCHANNEL chn) = 0;
    virtual VOID        CALL     Channel_PauseAll() = 0;
    virtual VOID        CALL     Channel_ResumeAll() = 0;
    virtual VOID        CALL     Channel_StopAll() = 0;
    virtual BOOL        CALL    Channel_IsPlaying(HCHANNEL chn) = 0;
    virtual FLOAT       CALL    Channel_GetLength(HCHANNEL chn) = 0;
    virtual FLOAT       CALL    Channel_GetPos(HCHANNEL chn) = 0;
    virtual VOID        CALL    Channel_SetPos(HCHANNEL chn, FLOAT fSeconds) = 0;
    virtual VOID        CALL    Channel_SlideTo(HCHANNEL channel, FLOAT time, INT volume, INT pan = -101, FLOAT pitch = -1) = 0;
    virtual BOOL        CALL    Channel_IsSliding(HCHANNEL channel) = 0;

    virtual VOID        CALL    Input_GetMousePos(FLOAT *x, FLOAT *y) = 0;
    virtual VOID        CALL    Input_SetMousePos(FLOAT x, FLOAT y) = 0;
    virtual INT         CALL    Input_GetMouseWheel() = 0;
    virtual BOOL        CALL    Input_IsMouseOver() = 0;
    virtual BOOL        CALL    Input_KeyDown(INT key) = 0;
    virtual BOOL        CALL    Input_KeyUp(INT key) = 0;
    virtual BOOL        CALL    Input_GetKeyState(INT key) = 0;
    virtual TCHAR*       CALL    Input_GetKeyName(INT key) = 0;
    virtual INT         CALL    Input_GetKey() = 0;
    virtual INT         CALL    Input_GetChar() = 0;
    virtual BOOL        CALL    Input_GetEvent(hgeInputEvent *event) = 0;

    virtual BOOL        CALL    Gfx_BeginScene(HTARGET target=0) = 0;
    virtual VOID        CALL    Gfx_EndScene() = 0;
    virtual VOID        CALL    Gfx_Clear(DWORD color) = 0;
    virtual VOID        CALL    Gfx_RenderLine(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD color=0xFFFFFFFF, FLOAT z=0.5f) = 0;
    virtual VOID        CALL    Gfx_RenderTriple(CONST hgeTriple *triple) = 0;
    virtual VOID        CALL    Gfx_RenderQuad(CONST hgeQuad *quad) = 0;
    virtual hgeVertex*  CALL    Gfx_StartBatch(INT prim_type, HTEXTURE tex, INT blend, INT *max_prim) = 0;
    virtual VOID        CALL    Gfx_FinishBatch(INT nprim) = 0;
    virtual VOID        CALL    Gfx_SetClipping(INT x=0, INT y=0, INT w=0, INT h=0) = 0;
    virtual VOID        CALL    Gfx_SetTransform(FLOAT x=0, FLOAT y=0, FLOAT dx=0, FLOAT dy=0, FLOAT rot=0, FLOAT hscale=0, FLOAT vscale=0) = 0; 

    virtual HTARGET     CALL    Target_Create(INT width, INT height, BOOL zbuffer) = 0;
    virtual VOID        CALL    Target_Free(HTARGET target) = 0;
    virtual HTEXTURE    CALL    Target_GetTexture(HTARGET target) = 0;

    virtual HTEXTURE    CALL    Texture_Create(INT width, INT height) = 0;
    virtual HTEXTURE    CALL    Texture_Load(CONST TCHAR *filename, DWORD size=0, BOOL bMipmap=FALSE) = 0;
    virtual VOID        CALL    Texture_Free(HTEXTURE tex) = 0;
    virtual INT         CALL    Texture_GetWidth(HTEXTURE tex, BOOL bOriginal=FALSE) = 0;
    virtual INT         CALL    Texture_GetHeight(HTEXTURE tex, BOOL bOriginal=FALSE) = 0;
    virtual DWORD*      CALL    Texture_Lock(HTEXTURE tex, BOOL bReadOnly=TRUE, INT left=0, INT top=0, INT width=0, INT height=0) = 0;
    virtual VOID        CALL    Texture_Unlock(HTEXTURE tex) = 0;
};

extern "C" { EXPORT HGE * CALL hgeCreate(INT ver); }


/*
** HGE Virtual-key codes
*/
#define HGEK_LBUTTON    0x01
#define HGEK_RBUTTON    0x02
#define HGEK_MBUTTON    0x04

#define HGEK_ESCAPE     0x1B
#define HGEK_BACKSPACE  0x08
#define HGEK_TAB        0x09
#define HGEK_ENTER      0x0D
#define HGEK_SPACE      0x20

#define HGEK_SHIFT      0x10
#define HGEK_CTRL       0x11
#define HGEK_ALT        0x12

#define HGEK_LWIN       0x5B
#define HGEK_RWIN       0x5C
#define HGEK_APPS       0x5D

#define HGEK_PAUSE      0x13
#define HGEK_CAPSLOCK   0x14
#define HGEK_NUMLOCK    0x90
#define HGEK_SCROLLLOCK 0x91

#define HGEK_PGUP       0x21
#define HGEK_PGDN       0x22
#define HGEK_HOME       0x24
#define HGEK_END        0x23
#define HGEK_INSERT     0x2D
#define HGEK_DELETE     0x2E

#define HGEK_LEFT       0x25
#define HGEK_UP         0x26
#define HGEK_RIGHT      0x27
#define HGEK_DOWN       0x28

#define HGEK_0          0x30
#define HGEK_1          0x31
#define HGEK_2          0x32
#define HGEK_3          0x33
#define HGEK_4          0x34
#define HGEK_5          0x35
#define HGEK_6          0x36
#define HGEK_7          0x37
#define HGEK_8          0x38
#define HGEK_9          0x39

#define HGEK_A          0x41
#define HGEK_B          0x42
#define HGEK_C          0x43
#define HGEK_D          0x44
#define HGEK_E          0x45
#define HGEK_F          0x46
#define HGEK_G          0x47
#define HGEK_H          0x48
#define HGEK_I          0x49
#define HGEK_J          0x4A
#define HGEK_K          0x4B
#define HGEK_L          0x4C
#define HGEK_M          0x4D
#define HGEK_N          0x4E
#define HGEK_O          0x4F
#define HGEK_P          0x50
#define HGEK_Q          0x51
#define HGEK_R          0x52
#define HGEK_S          0x53
#define HGEK_T          0x54
#define HGEK_U          0x55
#define HGEK_V          0x56
#define HGEK_W          0x57
#define HGEK_X          0x58
#define HGEK_Y          0x59
#define HGEK_Z          0x5A

#define HGEK_GRAVE      0xC0
#define HGEK_MINUS      0xBD
#define HGEK_EQUALS     0xBB
#define HGEK_BACKSLASH  0xDC
#define HGEK_LBRACKET   0xDB
#define HGEK_RBRACKET   0xDD
#define HGEK_SEMICOLON  0xBA
#define HGEK_APOSTROPHE 0xDE
#define HGEK_COMMA      0xBC
#define HGEK_PERIOD     0xBE
#define HGEK_SLASH      0xBF

#define HGEK_NUMPAD0    0x60
#define HGEK_NUMPAD1    0x61
#define HGEK_NUMPAD2    0x62
#define HGEK_NUMPAD3    0x63
#define HGEK_NUMPAD4    0x64
#define HGEK_NUMPAD5    0x65
#define HGEK_NUMPAD6    0x66
#define HGEK_NUMPAD7    0x67
#define HGEK_NUMPAD8    0x68
#define HGEK_NUMPAD9    0x69

#define HGEK_MULTIPLY   0x6A
#define HGEK_DIVIDE     0x6F
#define HGEK_ADD        0x6B
#define HGEK_SUBTRACT   0x6D
#define HGEK_DECIMAL    0x6E

#define HGEK_F1         0x70
#define HGEK_F2         0x71
#define HGEK_F3         0x72
#define HGEK_F4         0x73
#define HGEK_F5         0x74
#define HGEK_F6         0x75
#define HGEK_F7         0x76
#define HGEK_F8         0x77
#define HGEK_F9         0x78
#define HGEK_F10        0x79
#define HGEK_F11        0x7A
#define HGEK_F12        0x7B


#endif

