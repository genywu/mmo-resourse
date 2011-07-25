#include "Precompiled.h"

INT MSCALL Test_MsAccessMainThread(INT,LPTCHAR*)
{
    CMsAccess MsAccess;
    BOOL bRet = MsAccess.Open(_T("D:/我的桌面/1.mdb"));
    if (!bRet)
    {
        std::cout << "打开数据库失败" << std::endl;
    }

    MsAccess.ExecuteSQL(_T("SELECT 测试表.* FROM 测试表 WHERE Name like '*A'"));
    DWORD dwResultCount = MsAccess.GetResultCount();
    COleVariant* pCOleVariant = NULL;

    FOR_x_TO(i, dwResultCount)
    {
        pCOleVariant = MsAccess.GetResultValue(0);
        std::cout << pCOleVariant->lVal << std::endl;
        pCOleVariant = MsAccess.GetResultValue(1);
        std::wcout << pCOleVariant->bstrVal << std::endl;
        MsAccess.MoveNext();
    }

    _tsystem(_T("Pause"));
    return 0;
}
