#include "Precompiled.h"
INT MSCALL LogServerMainThread(INT, LPTCHAR*);
CMsBaseRun s_CMsBaseRun(_T("%s/Log/Exe_LogServer_Assertx.log"), LogServerMainThread);
