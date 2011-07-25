#include "Precompiled.h"

INT __cdecl _tmain(INT argc, LPTCHAR* argv)
{
    //_ENTER_FUN_C;

    if (g_pCMsConsole)
    {
        g_pCMsConsole->Init(argc, argv);
        return g_pCMsConsole->Run();
    }
    else
    {
        AssertEx(
			AL_NORMAL,
            g_pCMsConsole,
            _T("请在预编译头cpp文件中定义全局【CMsBaseRun】对象【使用控制台处理方式】\n")
            _T("CMsBaseRun s_CMsBaseRun(_T(\"%s/Log/********_Assertx.log\"), ******Fun);")
            );
    }

    //_DISPOSE_END_C
    //{
    //    MESSAGEBOX(_T("最后的异常处理,程序即将结束!"));
    //}
    //_LEAVE_FUN;
    return INVALID_NID;
}

#ifdef UNICODE
INT __cdecl main(INT argc, LPCHAR* argv)
{
    //_ENTER_FUN_C;

    static WCHAR wszCmdLine[KB_SIZE] = {0};
    static LPWCHAR lpargv[MAX_PATH] = {NULL};
    static size_t sTempStrLen = 0;

    //sTempStrLen = strlen(argv[0]);

    //::MultiByteToWideChar(
    //    CP_ACP, 
    //    0, 
    //    argv[0],
    //    (INT)sTempStrLen + 1,
    //    lpCmdLine,
    //    KB_SIZE
    //    );

    //lpCmdLine += sTempStrLen;

    for (INT i = 0; i < argc; i++)
    {
        size_t sTempLen = strlen(argv[i]);
        ACharToWChar(argv[i], &wszCmdLine[sTempStrLen], MAX_PATH);
        lpargv[i] = &wszCmdLine[sTempStrLen];
        sTempStrLen += sTempLen;
        wszCmdLine[sTempStrLen] = _T('\0');
        sTempStrLen++;
    }
    return _tmain(argc, lpargv);

    //_DISPOSE_END_C
    //{
    //    MESSAGEBOX(_T("最后的异常处理,程序即将结束!"));
    //}
    //_LEAVE_FUN;
    //return INVALID_NID;
}
#endif

CMsConsole* g_pCMsConsole = NULL;

CMsConsole::CMsConsole(RUNCALLBACK pRunCallBack)
: m_pRunCallBack        (pRunCallBack)
, m_nArgc               (0)
, m_strArgv             (NULL)
, m_hConsoleOut         (NULL)
, m_hConsoleErr         (NULL)
, m_hConsoleIn          (NULL)
, m_lpwAttributesBuff   (NULL)
, m_sScreenBuffSize     (0)
{
    _ENTER_FUN_C;

    ZeroMemoryStruct(m_cfi);
    m_cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);

    //BOOL bRet = this->Init(nArgc, strArgv);
    //AssertExf(bRet, _T("%s构造失败"), m_lpClassName);

    _DISPOSE_END_C
    {

    }
    _LEAVE_FUN
}

CMsConsole::~CMsConsole()
{
    this->Release();
}

BOOL CMsConsole::Init(INT nArgc, LPTCHAR* strArgv)
{
    m_nArgc = nArgc;
    m_strArgv = NEW LPTCHAR[m_nArgc];
    if (m_strArgv)
    {
        for (INT i = 0; i < m_nArgc; i++)
        {
            m_strArgv[i] = NEW TCHAR[MAX_PATH];
            if (m_strArgv[i])
            {
                _tcscpy_s(m_strArgv[i], MAX_PATH, strArgv[i]);
            }
            else
            {
                AssertEx(AL_CATAST, m_strArgv[i], _T("内存不足"));
            }
        }
    }
    else
    {
        AssertEx(AL_CATAST, m_strArgv, _T("内存不足"));
    }

    // 获取标准输出设备句柄
   m_hConsoleOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
   m_hConsoleErr = ::GetStdHandle(STD_ERROR_HANDLE);
   m_hConsoleIn  = ::GetStdHandle(STD_INPUT_HANDLE);

   ::GetConsoleScreenBufferInfo(m_hConsoleOut, &m_csbi);
   m_sScreenBuffSize = m_csbi.dwSize.X * m_csbi.dwSize.Y;
   m_lpwAttributesBuff = NEW WORD[m_sScreenBuffSize * 2];
   AssertEx(AL_CATAST, m_lpwAttributesBuff, _T("m_lpwAttributesBuff为空,内存不足"));

    return TRUE;
}

// 释放
BOOL CMsConsole::Release(VOID)
{
    for (INT i = 0; i < m_nArgc; i++)
    {
        SAFE_DELETE_ARRAY(m_strArgv[i]);
    }
    SAFE_DELETE_ARRAY(m_strArgv);
    SAFE_DELETE_ARRAY(m_lpwAttributesBuff);
    return TRUE;
}

BOOL CMsConsole::Run(VOID)
{
    if (!m_pRunCallBack(m_nArgc, m_strArgv))
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CMsConsole::SetConsoleSize(DWORD dwSize)
{
    CONSOLE_CURSOR_INFO cci;

    BOOL bRet = TRUE;

    bRet = ::GetConsoleCursorInfo(m_hConsoleOut, &cci);
    if (!bRet)return FALSE;
    cci.dwSize = dwSize;
    cci.bVisible = FALSE;
    bRet = ::SetConsoleCursorInfo(m_hConsoleOut, &cci);
    if (!bRet)return FALSE;

    bRet = ::GetConsoleCursorInfo(m_hConsoleErr, &cci);
    if (!bRet)return FALSE;
    cci.dwSize = dwSize;
    cci.bVisible = FALSE;
    bRet = ::SetConsoleCursorInfo(m_hConsoleErr, &cci);
    if (!bRet)return FALSE;

    bRet = ::GetConsoleCursorInfo(m_hConsoleIn, &cci);
    if (!bRet)return FALSE;
    cci.dwSize = dwSize;
    cci.bVisible = FALSE;
    bRet = ::SetConsoleCursorInfo(m_hConsoleIn, &cci);
    if (!bRet)return FALSE;

    return TRUE;
}


BOOL CMsConsole::SetConsoleTitle(LPTCHAR szTitle)
{
    return ::SetConsoleTitle(szTitle);
}

BOOL CMsConsole::SetSithTextColor(
    E_FOREGOUND_TEXT_COLOR fColor,
    E_BACKGOUND_TEXT_COLOR bColor,
    BOOL bfIntensity,
    BOOL bbIntensity
    )
{
    WORD wfColor = (WORD)fColor;
    WORD wbColor = (WORD)bColor;
    if (bfIntensity)
    {
        wfColor |= FOREGROUND_INTENSITY;
    }
    if (bbIntensity)
    {
        wbColor |= BACKGROUND_INTENSITY;
    }
    return ::SetConsoleTextAttribute(m_hConsoleOut, (WORD)(wfColor | wbColor));
}

DWORD CMsConsole::GetSithColor(VOID)
{
    //CONSOLE_SCREEN_BUFFER_INFOEX csbi;
    //csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
    ////::GetConsoleScreenBufferInfoEx(m_hConsoleOut, &csbi);
    //return csbi.wAttributes;
    return 0;
}

BOOL CMsConsole::SetSceneColor(
    E_BACKGOUND_TEXT_COLOR bColor,
    BOOL bbIntensity
    )
{
    COORD cd;
    cd.X = 0;
    cd.Y = 0;
    DWORD dwOut;
    WORD wbColor = (WORD)bColor;
    if (bbIntensity)
    {
        wbColor |= BACKGROUND_INTENSITY;
    }
    size_t BuffLen = m_sScreenBuffSize*sizeof(WORD);
    wmemset(
        (wchar_t*)m_lpwAttributesBuff,
        wbColor,
        BuffLen
        );

    return ::WriteConsoleOutputAttribute(
        m_hConsoleOut,
        m_lpwAttributesBuff,
        m_csbi.dwSize.X * m_csbi.dwSize.Y,
        cd, &dwOut
        );
}
//
//BOOL CMsConsole::SetConsoleTextFont(COORD dwFontSize, DWORD dwFontIndex, UINT FontFamily, UINT FontWeight, WCHAR* FaceName)
//{
//    m_cfi.nFont         = dwFontIndex;
//    m_cfi.dwFontSize    = dwFontSize;
//    m_cfi.FontFamily    = FontFamily;
//    m_cfi.FontWeight    = FontWeight;
//    wcscpy_s(m_cfi.FaceName, LF_FACESIZE, FaceName);
//    return ::SetCurrentConsoleFontEx(m_hConsoleOut, FALSE, &m_cfi);
//}

//COORD CMsConsole::GetCursorPosition(VOID)
//{
//    CONSOLE_SCREEN_BUFFER_INFOEX csbi;
//    csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
//    ::GetConsoleScreenBufferInfoEx(m_hConsoleOut, &csbi);
//    return csbi.dwCursorPosition;
//}
//
//BOOL CMsConsole::SetCursorPosition(COORD dwCursorPosition)
//{
//    return ::SetConsoleCursorPosition(m_hConsoleOut, dwCursorPosition);
//}

//VOID CMsConsole::OutText(LPTCHAR strText, DWORD dwLength ,COORD dwCoord)
//{
//#ifdef UNICODE
//    return this->OutTextW(strText, dwLength, dwCoord);
//#else
//    return this->OutTextA(strText, dwLength, dwCoord);
//#endif
//
//}

//VOID CMsConsole::OutTextW(LPWCHAR strText, DWORD dwLength, COORD dwCoord)
//{
//    if ((dwCoord.X == INVALID_NID) && (dwCoord.Y == INVALID_NID))
//    {
//        CONSOLE_SCREEN_BUFFER_INFOEX csbi;
//        csbi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
//        ::GetConsoleScreenBufferInfoEx(m_hConsoleOut, &csbi);
//        ::WriteConsoleOutputCharacterW(m_hConsoleOut, strText, dwLength, csbi.dwCursorPosition, &dwLength);
//        csbi.dwCursorPosition.Y += (SHORT)dwLength*sizeof(WCHAR)/csbi.dwSize.X;
//        csbi.dwCursorPosition.X += (SHORT)dwLength*sizeof(WCHAR)%csbi.dwSize.X;
//        while (csbi.dwCursorPosition.X >= csbi.dwSize.X)
//        {
//            csbi.dwCursorPosition.Y++;
//            csbi.dwCursorPosition.X -= csbi.dwSize.X;
//        }
//        this->SetCursorPosition(csbi.dwCursorPosition);
//    }
//    else
//    {
//        ::WriteConsoleOutputCharacterW(m_hConsoleOut, strText, dwLength, dwCoord, NULL);
//    }
//
//
//    //dwCoord.X += 
//    //this->SetCursorPosition()
//}
//
//VOID CMsConsole::OutTextA(LPCHAR strText, DWORD dwLength, COORD dwCoord)
//{
//    if (dwCoord.X == 255 && dwCoord.Y)
//    {
//        ::WriteConsoleOutputCharacterA(m_hConsoleOut, strText, dwLength, this->GetCursorPosition(), NULL);
//    }
//    else
//    {
//        ::WriteConsoleOutputCharacterA(m_hConsoleOut, strText, dwLength, dwCoord, NULL);
//    }
//}