/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/


#ifndef HGE_IMPL_H
#define HGE_IMPL_H

#include "hge.h"
#include <stdio.h>
#include "d3d8.h"
#include "d3dx8.h"
#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

#define DEMO

#define D3DFVF_HGEVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define VERTEX_BUFFER_SIZE 4000


typedef BOOL (WINAPI *GetSystemPowerStatusFunc)(LPSYSTEM_POWER_STATUS);


struct CRenderTargetList
{
    INT                    width;
    INT                    height;
    IDirect3DTexture8*    pTex;
    IDirect3DSurface8*    pDepth;
    CRenderTargetList*    next;
};

struct CTextureList
{
    HTEXTURE            tex;
    INT                    width;
    INT                    height;
    CTextureList*        next;
};

struct CResourceList
{
    TCHAR                filename[_MAX_PATH];
    TCHAR                password[64];
    CResourceList*        next;
};

struct CStreamList
{
    HSTREAM                hstream;
    VOID*                data;
    CStreamList*        next;
};

struct CInputEventList
{
    hgeInputEvent        event;
    CInputEventList*    next;
};


VOID DInit();
VOID DDone();
BOOL DFrame();


/*
** HGE Interface implementation
*/
class HGE_Impl : public HGE
{
public:
    virtual    VOID        CALL    Release();

    virtual BOOL        CALL    System_Initiate();
    virtual VOID        CALL    System_Shutdown();
    virtual BOOL        CALL    System_Start();
    virtual VOID        CALL    System_SetStateBool  (hgeBoolState   state, BOOL        value);
    virtual VOID        CALL    System_SetStateFunc  (hgeFuncState   state, hgeCallback value);
    virtual VOID        CALL    System_SetStateHwnd  (hgeHwndState   state, HWND        value);
    virtual VOID        CALL    System_SetStateInt   (hgeIntState    state, INT         value);
    virtual VOID        CALL    System_SetStateString(hgeStringState state, CONST TCHAR *value);
    virtual BOOL        CALL    System_GetStateBool  (hgeBoolState  );
    virtual hgeCallback    CALL    System_GetStateFunc  (hgeFuncState  );
    virtual HWND        CALL    System_GetStateHwnd  (hgeHwndState  );
    virtual INT            CALL    System_GetStateInt   (hgeIntState   );
    virtual CONST TCHAR*    CALL    System_GetStateString(hgeStringState);
    virtual TCHAR*        CALL    System_GetErrorMessage();
    virtual    VOID        CALL    System_Log(CONST TCHAR *format, ...);
    virtual BOOL        CALL    System_Launch(CONST TCHAR *url);
    virtual VOID        CALL    System_Snapshot(CONST TCHAR *filename=0);

    virtual VOID*        CALL    Resource_Load(CONST TCHAR *filename, DWORD *size=0);
    virtual VOID        CALL    Resource_Free(VOID *res);
    virtual BOOL        CALL    Resource_AttachPack(CONST TCHAR *filename, CONST TCHAR *password=0);
    virtual VOID        CALL    Resource_RemovePack(CONST TCHAR *filename);
    virtual VOID        CALL    Resource_RemoveAllPacks();
    virtual TCHAR*        CALL    Resource_MakePath(CONST TCHAR *filename=0);
    virtual TCHAR*        CALL    Resource_EnumFiles(CONST TCHAR *wildcard=0);
    virtual TCHAR*        CALL    Resource_EnumFolders(CONST TCHAR *wildcard=0);

    virtual    VOID        CALL    Ini_SetInt(CONST TCHAR *section, CONST TCHAR *name, INT value);
    virtual    INT         CALL    Ini_GetInt(CONST TCHAR *section, CONST TCHAR *name, INT def_val);
    virtual    VOID        CALL    Ini_SetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT value);
    virtual    FLOAT        CALL    Ini_GetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT def_val);
    virtual    VOID        CALL    Ini_SetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *value);
    virtual    TCHAR*        CALL    Ini_GetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *def_val);

    virtual VOID        CALL    Random_Seed(INT seed=0);
    virtual INT            CALL    Random_Int(INT min, INT max);
    virtual FLOAT        CALL    Random_Float(FLOAT min, FLOAT max);

    virtual FLOAT        CALL    Timer_GetTime();
    virtual FLOAT        CALL    Timer_GetDelta();
    virtual INT            CALL    Timer_GetFPS();

    virtual HEFFECT        CALL    Effect_Load(CONST TCHAR *filename, DWORD size=0);
    virtual VOID        CALL    Effect_Free(HEFFECT eff);
    virtual HCHANNEL    CALL     Effect_Play(HEFFECT eff);
    virtual HCHANNEL    CALL     Effect_PlayEx(HEFFECT eff, INT volume=100, INT pan=0, FLOAT pitch=1.0f, BOOL loop=FALSE);

    virtual HMUSIC        CALL     Music_Load(CONST TCHAR *filename, DWORD size=0);
    virtual VOID        CALL    Music_Free(HMUSIC mus);
    virtual HCHANNEL    CALL     Music_Play(HMUSIC mus, BOOL loop, INT volume = 100, INT order = 0, INT row = 0);
    virtual VOID        CALL    Music_SetAmplification(HMUSIC music, INT ampl);
    virtual INT            CALL    Music_GetAmplification(HMUSIC music);
    virtual INT            CALL    Music_GetLength(HMUSIC music);
    virtual VOID        CALL    Music_SetPos(HMUSIC music, INT order, INT row);
    virtual BOOL        CALL    Music_GetPos(HMUSIC music, INT *order, INT *row);
    virtual VOID        CALL    Music_SetInstrVolume(HMUSIC music, INT instr, INT volume);
    virtual INT            CALL    Music_GetInstrVolume(HMUSIC music, INT instr);
    virtual VOID        CALL    Music_SetChannelVolume(HMUSIC music, INT channel, INT volume);
    virtual INT            CALL    Music_GetChannelVolume(HMUSIC music, INT channel);

    virtual HSTREAM        CALL    Stream_Load(CONST TCHAR *filename, DWORD size=0);
    virtual VOID        CALL    Stream_Free(HSTREAM stream);
    virtual HCHANNEL    CALL    Stream_Play(HSTREAM stream, BOOL loop, INT volume = 100);

    virtual VOID        CALL     Channel_SetPanning(HCHANNEL chn, INT pan);
    virtual VOID        CALL     Channel_SetVolume(HCHANNEL chn, INT volume);
    virtual VOID        CALL     Channel_SetPitch(HCHANNEL chn, FLOAT pitch);
    virtual VOID        CALL     Channel_Pause(HCHANNEL chn);
    virtual VOID        CALL     Channel_Resume(HCHANNEL chn);
    virtual VOID        CALL     Channel_Stop(HCHANNEL chn);
    virtual VOID        CALL     Channel_PauseAll();
    virtual VOID        CALL     Channel_ResumeAll();
    virtual VOID        CALL     Channel_StopAll();
    virtual BOOL        CALL    Channel_IsPlaying(HCHANNEL chn);
    virtual FLOAT        CALL    Channel_GetLength(HCHANNEL chn);
    virtual FLOAT        CALL    Channel_GetPos(HCHANNEL chn);
    virtual VOID        CALL    Channel_SetPos(HCHANNEL chn, FLOAT fSeconds);
    virtual VOID        CALL    Channel_SlideTo(HCHANNEL channel, FLOAT time, INT volume, INT pan = -101, FLOAT pitch = -1);
    virtual BOOL        CALL    Channel_IsSliding(HCHANNEL channel);

    virtual VOID        CALL    Input_GetMousePos(FLOAT *x, FLOAT *y);
    virtual VOID        CALL    Input_SetMousePos(FLOAT x, FLOAT y);
    virtual INT            CALL    Input_GetMouseWheel();
    virtual BOOL        CALL    Input_IsMouseOver();
    virtual BOOL        CALL    Input_KeyDown(INT key);
    virtual BOOL        CALL    Input_KeyUp(INT key);
    virtual BOOL        CALL    Input_GetKeyState(INT key);
    virtual TCHAR*        CALL    Input_GetKeyName(INT key);
    virtual INT            CALL    Input_GetKey();
    virtual INT            CALL    Input_GetChar();
    virtual BOOL        CALL    Input_GetEvent(hgeInputEvent *event);

    virtual BOOL        CALL    Gfx_BeginScene(HTARGET target=0);
    virtual VOID        CALL    Gfx_EndScene();
    virtual VOID        CALL    Gfx_Clear(DWORD color);
    virtual VOID        CALL    Gfx_RenderLine(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD color=0xFFFFFFFF, FLOAT z=0.5f);
    virtual VOID        CALL    Gfx_RenderTriple(CONST hgeTriple *triple);
    virtual VOID        CALL    Gfx_RenderQuad(CONST hgeQuad *quad);
    virtual hgeVertex*    CALL    Gfx_StartBatch(INT prim_type, HTEXTURE tex, INT blend, INT *max_prim);
    virtual VOID        CALL    Gfx_FinishBatch(INT nprim);
    virtual VOID        CALL    Gfx_SetClipping(INT x=0, INT y=0, INT w=0, INT h=0);
    virtual VOID        CALL    Gfx_SetTransform(FLOAT x=0, FLOAT y=0, FLOAT dx=0, FLOAT dy=0, FLOAT rot=0, FLOAT hscale=0, FLOAT vscale=0); 

    virtual HTARGET        CALL    Target_Create(INT width, INT height, BOOL zbuffer);
    virtual VOID        CALL    Target_Free(HTARGET target);
    virtual HTEXTURE    CALL    Target_GetTexture(HTARGET target);

    virtual HTEXTURE    CALL    Texture_Create(INT width, INT height);
    virtual HTEXTURE    CALL    Texture_Load(CONST TCHAR *filename, DWORD size=0, BOOL bMipmap=FALSE);
    virtual VOID        CALL    Texture_Free(HTEXTURE tex);
    virtual INT            CALL    Texture_GetWidth(HTEXTURE tex, BOOL bOriginal=FALSE);
    virtual INT            CALL    Texture_GetHeight(HTEXTURE tex, BOOL bOriginal=FALSE);
    virtual DWORD*        CALL    Texture_Lock(HTEXTURE tex, BOOL bReadOnly=TRUE, INT left=0, INT top=0, INT width=0, INT height=0);
    virtual VOID        CALL    Texture_Unlock(HTEXTURE tex);

    //////// Implementation ////////

    static HGE_Impl*    _Interface_Get();
    VOID                _FocusChange(BOOL bAct);
    VOID                _PostError(TCHAR *error);


    HINSTANCE            hInstance;
    HWND                hwnd;

    BOOL                bActive;
    TCHAR                szError[256];
    TCHAR                szAppPath[_MAX_PATH];
    TCHAR                szIniString[256];


    // System States
    BOOL                (*procFrameFunc)();
    BOOL                (*procRenderFunc)();
    BOOL                (*procFocusLostFunc)();
    BOOL                (*procFocusGainFunc)();
    BOOL                (*procGfxRestoreFunc)();
    BOOL                (*procExitFunc)();
    CONST TCHAR*            szIcon;
    TCHAR                szWinTitle[256];
    INT                    nScreenWidth;
    INT                    nScreenHeight;
    INT                    nScreenBPP;
    BOOL                bWindowed;
    BOOL                bZBuffer;
    BOOL                bTextureFilter;
    TCHAR                szIniFile[_MAX_PATH];
    TCHAR                szLogFile[_MAX_PATH];
    BOOL                bUseSound;
    INT                    nSampleRate;
    INT                    nFXVolume;
    INT                    nMusVolume;
    INT                    nStreamVolume;
    INT                    nHGEFPS;
    BOOL                bHideMouse;
    BOOL                bDontSuspend;
    HWND                hwndParent;

    #ifdef DEMO
    BOOL                bDMO;
    #endif


    // Power
    INT                            nPowerStatus;
    HMODULE                        hKrnl32;
    GetSystemPowerStatusFunc    lpfnGetSystemPowerStatus;

    VOID                _InitPowerStatus();
    VOID                _UpdatePowerStatus();
    VOID                _DonePowerStatus();


    // Graphics
    D3DPRESENT_PARAMETERS*  d3dpp;

    D3DPRESENT_PARAMETERS   d3dppW;
    RECT                    rectW;
    LONG                    styleW;

    D3DPRESENT_PARAMETERS   d3dppFS;
    RECT                    rectFS;
    LONG                    styleFS;

    IDirect3D8*                pD3D;
    IDirect3DDevice8*        pD3DDevice;
    IDirect3DVertexBuffer8*    pVB;
    IDirect3DIndexBuffer8*    pIB;

    IDirect3DSurface8*    pScreenSurf;
    IDirect3DSurface8*    pScreenDepth;
    CRenderTargetList*    pTargets;
    CRenderTargetList*    pCurTarget;

    D3DXMATRIX            matView;
    D3DXMATRIX            matProj;

    CTextureList*        textures;
    hgeVertex*            VertArray;

    INT                    nPrim;
    INT                    CurPrimType;
    INT                    CurBlendMode;
    HTEXTURE            CurTexture;

    BOOL                _GfxInit();
    VOID                _GfxDone();
    BOOL                _GfxRestore();
    VOID                _AdjustWindow();
    VOID                _Resize(INT width, INT height);
    BOOL                _init_lost();
    VOID                _render_batch(BOOL bEndScene=FALSE);
    INT                    _format_id(D3DFORMAT fmt);
    VOID                _SetBlendMode(INT blend);
    VOID                _SetProjectionMatrix(INT width, INT height);
    

    // Audio
    HINSTANCE            hBass;
    BOOL                bSilent;
    CStreamList*        streams;
    BOOL                _SoundInit();
    VOID                _SoundDone();
    VOID                _SetMusVolume(INT vol);
    VOID                _SetStreamVolume(INT vol);
    VOID                _SetFXVolume(INT vol);


    // Input
    INT                    VKey;
    INT                    Char;
    INT                    Zpos;
    FLOAT                Xpos;
    FLOAT                Ypos;
    BOOL                bMouseOver;
    BOOL                bCaptured;
    TCHAR                keyz[256];
    CInputEventList*    queue;
    VOID                _UpdateMouse();
    VOID                _InputInit();
    VOID                _ClearQueue();
    VOID                _BuildEvent(INT type, INT key, INT scan, INT flags, INT x, INT y);


    // Resources
    TCHAR                szTmpFilename[_MAX_PATH];
    CResourceList*        res;
    HANDLE                hSearch;
    WIN32_FIND_DATA     SearchData;


    // Timer
    FLOAT                fTime;
    FLOAT                fDeltaTime;
    DWORD                nFixedDelta;
    INT                    nFPS;
    DWORD                t0, t0fps, dt;
    INT                    cfps;


private:
    HGE_Impl();
};

extern HGE_Impl*        pHGE;

#endif

