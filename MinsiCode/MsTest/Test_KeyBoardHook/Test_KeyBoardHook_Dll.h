#pragma once
#define I_KNOW_THE_BASE_TYPE
#define DLL_EXPORTS
#include "MsBaseDef.h"

BOOL WINAPI DllMain(
    IN HINSTANCE hDllHandle, 
    IN DWORD     nReason,    
    IN LPVOID    Reserved    
    );

USE_DLL extern HHOOK g_hHook;
