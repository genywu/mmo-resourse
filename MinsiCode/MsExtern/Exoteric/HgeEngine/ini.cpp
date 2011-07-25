#include "Precompiled.h"
/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: ini file
*/


#include "hge_impl.h"
//#pragma warning(disable : 4996)


VOID CALL HGE_Impl::Ini_SetInt(CONST TCHAR *section, CONST TCHAR *name, INT value)
{
    TCHAR buf[256];

    if(szIniFile[0]) {
        _stprintf(buf,_T("%d"),value);
        WritePrivateProfileString(section, name, buf, szIniFile);
    }
}


INT CALL HGE_Impl::Ini_GetInt(CONST TCHAR *section, CONST TCHAR *name, INT def_val)
{
    TCHAR buf[256];

    if(szIniFile[0]) {
        if(GetPrivateProfileString(section, name, _T(""), buf, sizeof(buf), szIniFile))
        { return _ttoi(buf); }
        else { return def_val; }
    }
    return def_val;
}


VOID CALL HGE_Impl::Ini_SetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT value)
{
    TCHAR buf[256];

    if(szIniFile[0]) {
        _stprintf(buf,_T("%f"),value);
        WritePrivateProfileString(section, name, buf, szIniFile);
    }
}


FLOAT CALL HGE_Impl::Ini_GetFloat(CONST TCHAR *section, CONST TCHAR *name, FLOAT def_val)
{
    TCHAR buf[256];

    if(szIniFile[0]) {
        if(GetPrivateProfileString(section, name, _T(""), buf, sizeof(buf), szIniFile))
        { return (FLOAT)_tstof(buf); }
        else { return def_val; }
    }
    return def_val;
}


VOID CALL HGE_Impl::Ini_SetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *value)
{
    if(szIniFile[0]) WritePrivateProfileString(section, name, value, szIniFile);
}


TCHAR* CALL HGE_Impl::Ini_GetString(CONST TCHAR *section, CONST TCHAR *name, CONST TCHAR *def_val)
{
    if(szIniFile[0]) GetPrivateProfileString(section, name, def_val, szIniString, sizeof(szIniString), szIniFile);
    else _tcscpy(szIniString, def_val);
    return szIniString;
}

