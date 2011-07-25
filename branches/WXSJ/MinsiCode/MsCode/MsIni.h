#pragma once            //┃//┃
#ifndef __MSINI_H__     //┃//┃
#define __MSINI_H__     //┃//┃
//━━━━━━━━━━━━┛//┃
//文件名称：MsIni.h         //┃
//功能描述：基本断言定义    //┃
//平台相关：Windows         //┃
//━━━━━━━━━━━━━━┛

class CMsIni
{
public:
    CMsIni(TCHAR* szIniFile);
    ~CMsIni(VOID);

    inline BOOL CheckIniFile(TCHAR* szIniFile);

    VOID MSCALL SetDword(
        CONST TCHAR* section,
        CONST TCHAR* name,
        DWORD value
        );

    DWORD MSCALL GetDword(
        CONST TCHAR* section,
        CONST TCHAR* name,
        DWORD def_val
        );

    VOID MSCALL SetWord(
        CONST TCHAR* section,
        CONST TCHAR* name,
        WORD value
        );

    WORD MSCALL GetWord(
        CONST TCHAR* section,
        CONST TCHAR* name,
        WORD def_val
        );

    VOID MSCALL SetByte(
        CONST TCHAR* section,
        CONST TCHAR* name,
        BYTE value
        );

    BYTE MSCALL GetByte(
        CONST TCHAR* section,
        CONST TCHAR* name,
        BYTE def_val
        );

    VOID MSCALL SetBool(
        CONST TCHAR* section,
        CONST TCHAR* name,
        BOOL value
        );

    BOOL MSCALL GetBool(
        CONST TCHAR* section,
        CONST TCHAR* name,
        BOOL def_val
        );

    VOID MSCALL SetFloat(
        CONST TCHAR* section,
        CONST TCHAR* name,
        FLOAT value
        );

    FLOAT MSCALL GetFloat(
        CONST TCHAR* section,
        CONST TCHAR* name,
        FLOAT def_val
        );

    VOID MSCALL SetString(
        CONST TCHAR* section,
        CONST TCHAR* name,
        CONST TCHAR* value
        );

    TCHAR* MSCALL GetString(
        CONST TCHAR* section,
        CONST TCHAR* name,
        CONST TCHAR* def_val,
        OUT TCHAR* szString = NULL,
        IN DWORD dwMaxLen = 0
        );

    ACHAR* MSCALL GetStringA(
        CONST ACHAR* section,
        CONST ACHAR* name,
        CONST ACHAR* def_val,
        OUT ACHAR* szString = NULL,
        IN DWORD dwMaxLen = 0
        );

    WCHAR* MSCALL GetStringW(
        CONST WCHAR* section,
        CONST WCHAR* name,
        CONST WCHAR* def_val,
        OUT WCHAR* szString = NULL,
        IN DWORD dwMaxLen = 0
        );

    TCHAR* ParseSectionIndex(CONST TCHAR* format, ...);
    ACHAR* ParseSectionIndexA(CONST ACHAR* format, ...);
    WCHAR* ParseSectionIndexW(CONST WCHAR* format, ...);
private:
    ACHAR    m_szIniFileA[MAX_PATH];
    WCHAR   m_szIniFileW[MAX_PATH];

    WCHAR   m_szIniTextW[MAX_PATH];
    ACHAR    m_szIniTextA[MAX_PATH];
};


//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
