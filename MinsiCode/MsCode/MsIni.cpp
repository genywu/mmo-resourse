#include "Precompiled.h"

CMsIni::CMsIni(TCHAR* szIniFile)
{
    _ENTER_FUN_C;

#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
#else
    TCHAR* strFile = m_szIniFileA;
#endif

    if (szIniFile)
    {
        BOOL bRet = CreateFullPath(strFile, szIniFile);
        AssertEx(AL_NORMAL, bRet, _T("解析配置文件路径失败"));
    }
    else
    {
        AssertEx(AL_NORMAL, szIniFile, _T("日志信息不能为空"));
    }

    if (szIniFile && strFile)
    {
        HANDLE hIniFile = ::CreateFile(
            strFile,
            FILE_READ_DATA,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
        if (INVALID_HANDLE_VALUE == hIniFile)
        {
            fAssertEx(AL_CATAST, FALSE, _T("%s\n\n此目录不存在"), strFile);
        }
        else
        {
            ::CloseHandle(hIniFile);
            hIniFile = INVALID_HANDLE_VALUE;

            BOOL bRet = this->CheckIniFile(strFile);

            if (!bRet)
            {
#ifdef UNICODE
                strFile = m_szIniFileW;
#else
                strFile = m_szIniFileA;
#endif
                ::GetCurrentDirectory(MAX_PATH, strFile);
                size_t sRunPathlen = _tcslen(strFile);
                if (strFile[sRunPathlen-1] != _T('\\') &&
                    strFile[sRunPathlen-1] != _T('/'))
                {
                    strFile[sRunPathlen] = _T('\\');
                    strFile[sRunPathlen+1] = _T('\0');
                }
                _tcscat_s(strFile, MAX_PATH, szIniFile);

                bRet = this->CheckIniFile(strFile);
                fAssertEx(
					AL_NORMAL, 
                    bRet,
                    _T("配置文件%s读取失败\n")
                    _T("【1、文件不存在】\n")
                    _T("【2、路径不对】\n")
                    _T("【3、选项被禁用】"),
                    szIniFile);
            }
        }
#ifdef UNICODE
        wcsncpy_s(m_szIniFileW, strFile, MAX_PATH);
        m_szIniFileW[MAX_PATH-1] = _T('\0');
        ::WideCharToMultiByte(
            CP_ACP, 
            0, 
            m_szIniFileW,
            (INT)_tcslen(m_szIniFileW) + 1,
            m_szIniFileA,
            MAX_PATH,
            NULL,
            NULL
            );
#else
        strncpy(m_szIniFileA, strFile, MAX_PATH);
        ACharToWChar(m_szIniFileA, m_szIniFileW, MAX_PATH);
#endif
    }

    _LEAVE_FUN_DISPOSE_END_C
}

CMsIni::~CMsIni(VOID)
{
}

inline BOOL CMsIni::CheckIniFile(TCHAR* szIniFile)
{
#ifdef UNICODE
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(::GetPrivateProfileString(
        _T("MsConfig"),
        _T("MsConfig"),
        _T(""),
        strTemp,
        MAX_PATH,
        szIniFile))
    {
        _tcslwr(strTemp);
        if (!_tcscmp(strTemp, _T("false")))
        {
            return FALSE;
        }
        else if (!_tcscmp(strTemp, _T("true")))
        {
            return TRUE;
        }
        else
        {
            DWORD dwVal = _ttoi(strTemp);
            if (1 == dwVal)
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }
        }
    }
    else
    {
        return FALSE;
    }
}


// 写配置文件DWORD
VOID MSCALL CMsIni::SetDword(
    CONST TCHAR* section,
    CONST TCHAR* name,
    DWORD value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        _stprintf(strTemp, _T("%d"), value);
        ::WritePrivateProfileString(section, name, strTemp, strFile);
    }
}

// 读配置文件DWORD
DWORD MSCALL CMsIni::GetDword(
    CONST TCHAR* section,
    CONST TCHAR* name,
    DWORD def_val
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if(::GetPrivateProfileString(
            section,
            name,
            _T(""),
            strTemp,
            MAX_PATH,
            strFile))
        {
            return _ttoi(strTemp);
        }
        else
        {
            return def_val;
        }
    }
    return def_val;
}

VOID MSCALL CMsIni::SetWord(
    CONST TCHAR* section,
    CONST TCHAR* name,
    WORD value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        _stprintf(strTemp, _T("%d"), value);
        ::WritePrivateProfileString(section, name, strTemp, strFile);
    }
}

WORD MSCALL CMsIni::GetWord(
    CONST TCHAR* section,
    CONST TCHAR* name,
    WORD def_val
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if(::GetPrivateProfileString(
            section,
            name,
            _T(""),
            strTemp,
            MAX_PATH,
            strFile))
        {
            return (WORD)_ttoi(strTemp);
        }
        else
        {
            return def_val;
        }
    }
    return def_val;
}

VOID MSCALL CMsIni::SetByte(
    CONST TCHAR* section,
    CONST TCHAR* name,
    BYTE value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        _stprintf(strTemp, _T("%d"), value);
        ::WritePrivateProfileString(section, name, strTemp, strFile);
    }
}

BYTE MSCALL CMsIni::GetByte(
    CONST TCHAR* section,
    CONST TCHAR* name,
    BYTE def_val
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if(::GetPrivateProfileString(
            section,
            name,
            _T(""),
            strTemp,
            MAX_PATH,
            strFile))
        {
            return (BYTE)_ttoi(strTemp);
        }
        else
        {
            return def_val;
        }
    }
    return def_val;
}

VOID MSCALL CMsIni::SetBool(
    CONST TCHAR* section,
    CONST TCHAR* name,
    BOOL value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    //TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    //TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if (value)
        {
            ::WritePrivateProfileString(section, name, _T("true"), strFile);
        }
        else
        {
            ::WritePrivateProfileString(section, name, _T("false"), strFile);
        }
    }
}

BOOL MSCALL CMsIni::GetBool(
    CONST TCHAR* section,
    CONST TCHAR* name,
    BOOL def_val
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if(::GetPrivateProfileString(
            section,
            name,
            _T(""),
            strTemp,
            MAX_PATH,
            strFile))
        {
            _tcslwr(strTemp);
            if (!_tcscmp(strTemp, _T("false")))
            {
                return FALSE;
            }
            else if (!_tcscmp(strTemp, _T("true")))
            {
                return TRUE;
            }
            else
            {
                DWORD dwVal = _ttoi(strTemp);
                if (1 == dwVal)
                {
                    return TRUE;
                }
                return def_val;
            }
        }
        else
        {
            return def_val;
        }
    }
    return def_val;
}

// 写配置文件FLOAT
VOID MSCALL CMsIni::SetFloat(
    CONST TCHAR* section,
    CONST TCHAR* name,
    FLOAT value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        _stprintf(strTemp,_T("%f"),value);
        ::WritePrivateProfileString(section, name, strTemp, strFile);
    }
}

// 读配置文件FLOAT
FLOAT MSCALL CMsIni::GetFloat(
    CONST TCHAR* section,
    CONST TCHAR* name,
    FLOAT def_val
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if(::GetPrivateProfileString(
            section,
            name,
            _T(""),
            strTemp,
            MAX_PATH,
            strFile))
        {
            return (FLOAT)(_tstof(strTemp));
        }
        else
        {
            return def_val;
        }
    }
    return def_val;
}

// 写配置文件字符串
VOID MSCALL CMsIni::SetString(
    CONST TCHAR* section,
    CONST TCHAR* name,
    CONST TCHAR* value
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
#else
    TCHAR* strFile = m_szIniFileA;
#endif
    if(strFile[0])
    {
        ::WritePrivateProfileString(section, name, value, strFile);
    }
}

// 读配置文件字符串
TCHAR* MSCALL CMsIni::GetString(
    CONST TCHAR* section,
    CONST TCHAR* name,
    CONST TCHAR* def_val,
    OUT TCHAR* szString,
    IN DWORD dwMaxLen
    )
{
#ifdef UNICODE
    TCHAR* strFile = m_szIniFileW;
    TCHAR* strTemp = m_szIniTextW;
#else
    TCHAR* strFile = m_szIniFileA;
    TCHAR* strTemp = m_szIniTextA;
#endif
    if(strFile[0])
    {
        if (szString)
        {
            ::GetPrivateProfileString(
                section,
                name,
                def_val,
                szString,
                dwMaxLen,
                strFile
                );

            return szString;
        }
        else
        {
            ::GetPrivateProfileString(
                section,
                name,
                def_val,
                strTemp,
                MAX_PATH,
                strFile
                );

            return strTemp;
        }
    }
    return NULL;
}


ACHAR* MSCALL CMsIni::GetStringA(
    CONST ACHAR* section,
    CONST ACHAR* name,
    CONST ACHAR* def_val,
    OUT ACHAR* szString,
    IN DWORD dwMaxLen
    )
{
    if(m_szIniFileA[0])
    {
        if (szString)
        {
            ::GetPrivateProfileStringA(
                section,
                name,
                def_val,
                szString,
                dwMaxLen,
                m_szIniFileA
                );

            return szString;
        }
        else
        {
            ::GetPrivateProfileStringA(
                section,
                name,
                def_val,
                m_szIniTextA,
                MAX_PATH,
                m_szIniFileA
                );

            return m_szIniTextA;
        }
    }
    return NULL;
}

WCHAR* MSCALL CMsIni::GetStringW(
    CONST WCHAR* section,
    CONST WCHAR* name,
    CONST WCHAR* def_val,
    OUT WCHAR* szString,
    IN DWORD dwMaxLen
    )
{
    if(m_szIniFileW[0])
    {
        if (szString)
        {
            ::GetPrivateProfileStringW(
                section,
                name,
                def_val,
                szString,
                dwMaxLen,
                m_szIniFileW
                );

            return szString;
        }
        else
        {
            ::GetPrivateProfileStringW(
                section,
                name,
                def_val,
                m_szIniTextW,
                MAX_PATH,
                m_szIniFileW
                );

            return m_szIniTextW;
        }
    }
    return NULL;
}

TCHAR* CMsIni::ParseSectionIndex(CONST TCHAR* format, ...)
{
    static MsLock s_Lock;
    _ENTER_FUN_C;
    AUTO_LOCK(s_Lock);
    va_list ap;
    va_start(ap, format);
    static TCHAR szRealsection[MAX_PATH] = {0};
    _vsntprintf_s(szRealsection, MAX_PATH, MAX_PATH-1, format, ap);
    va_end(ap);
    return szRealsection;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}

ACHAR* CMsIni::ParseSectionIndexA(CONST ACHAR* format, ...)
{
    static MsLock s_Lock;
    _ENTER_FUN_C;
    AUTO_LOCK(s_Lock);
    va_list ap;
    va_start(ap, format);
    static ACHAR szRealsection[MAX_PATH] = {0};
    _vsnprintf_s(szRealsection, MAX_PATH, MAX_PATH-1, format, ap);
    va_end(ap);
    return szRealsection;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}

WCHAR* CMsIni::ParseSectionIndexW(CONST WCHAR* format, ...)
{
    static MsLock s_Lock;
    _ENTER_FUN_C;
    AUTO_LOCK(s_Lock);
    va_list ap;
    va_start(ap, format);
    static WCHAR szRealsection[MAX_PATH] = {0};
    _vsnwprintf_s(szRealsection, MAX_PATH, MAX_PATH-1, format, ap);
    va_end(ap);
    return szRealsection;
    _LEAVE_FUN_DISPOSE_CONTINUE_C;
    return NULL;
}
