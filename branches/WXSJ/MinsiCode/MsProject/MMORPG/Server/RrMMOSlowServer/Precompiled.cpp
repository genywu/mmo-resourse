#include "Precompiled.h"
INT MSCALL SlowServerMainThread(INT, LPTCHAR*);
CMsBaseRun s_CMsBaseRun(_T("%s/Log/Exe_SlowServer_Assertx.log"), SlowServerMainThread);
