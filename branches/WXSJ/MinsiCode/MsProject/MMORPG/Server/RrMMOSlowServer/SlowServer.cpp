#include "Precompiled.h"

INT MSCALL SlowServerMainThread(INT, LPTCHAR*)
{
    _tsystem(_T("Pause"));
    return 0;
}
