#include "Precompiled.h"

extern LPTCHAR szMsErrorCodeName[MS_ERROR_MAX_COUNT+2] =
{
    _T("未知错误"),     _T("操作成功"),     _T("发生异常"),     _T("权限错误"),
    _T("空闲不足"),   _T("数据不足"),       _T("未初始化"),
    //_T("?"),            _T("?"),            _T("?"),            _T("?"),


    _T("ERROR_END")
};

extern LPTCHAR szMsErrorCodeInfo[MS_ERROR_MAX_COUNT+2] =
{
    _T("未定义的错误类型"),
    _T("操作成功,没有记录任何错误"),
    _T("可能是系统访问冲突,或程序自身的错误检查触发异常"),
    _T("一般为访问系统权限错误"),
    _T("缓冲区中没有足够的空余空间"),
    _T("缓冲区中没有足够的数据可用"),
    _T("调用或使用到的对象未进行初始化"),
    //_T("?"),            _T("?"),            _T("?"),            _T("?"),


    _T("ERROR_END")
};

TCHAR* GetErrorCodeName(E_MS_ERROR eMsError)
{
    return szMsErrorCodeName[eMsError+1];
}

TCHAR* GetErrorCodeInfo(E_MS_ERROR eMsError)
{
    return szMsErrorCodeInfo[eMsError+1];
}
