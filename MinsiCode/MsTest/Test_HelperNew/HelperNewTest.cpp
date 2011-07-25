#include "Precompiled.h"
#include "MsNode.h"
#include "MsStaticTree.h"

// 最大队列长度
#define MAX_QUEUE_LEN   1024

INT __fastcall MainCall(INT,LPTCHAR*)
{
    _ENTER_FUN_C;

    MSLOG.SetSaveLog(FALSE);
    MSLOG.SetEnableLog(FALSE);

    CMsStaticTree tree(8, 1000);

    _tsystem(_T("pause"));
    return 0;
    _LEAVE_FUN_DISPOSE_END_C;
    return INVALID_ID;
}
