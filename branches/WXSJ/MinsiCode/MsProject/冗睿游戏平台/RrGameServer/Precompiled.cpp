#include "Precompiled.h"
CMsLog* g_pMsLog = NULL;
DWORD g_dwMainThreadId = INVALID_UID;
INT MSCALL RrGameServerMainThread(INT, LPTCHAR*);
CMsBaseRun s_CMsBaseRun(_T("%s/Log/Exe_RrGameServer_Assertx.log"), RrGameServerMainThread);
