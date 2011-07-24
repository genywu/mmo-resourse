//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KwString.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   1998-10-16
//  Comment     :   String Rounte from Delphi 4.0
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KWSTRING_H_
#define _KWSTRING_H_     1


#ifdef _XP_LINUX_

#include <ctype.h>
#include <string.h>
#include <wchar.h>

#pragma pack(push, 1)

extern unsigned char LowerToUpperTable[256 + 4];
extern unsigned char UpperToLowerTable[256 + 4];

#pragma pack(pop)


inline int WcsLen(const unsigned short *Str1)
{
    return wcslen((const wchar_t *)Str1);
}

inline unsigned short * WcsCpy(unsigned short *Dest, const unsigned short *Source)
{
    return (unsigned short *)wcscpy((wchar_t *)Dest, (const wchar_t *)Source);
}

inline int WcsCmp(const unsigned short *Str1, const unsigned short *Str2)
{
    return wcscmp((const wchar_t *)Str1, (const wchar_t *)Str2);
}

inline int WcsNCmp(const unsigned short *Str1, const unsigned short *Str2, unsigned MaxLen)
{
    return wcsncmp((const wchar_t *)Str1, (const wchar_t *)Str2, MaxLen);
}


inline unsigned short * WcsCat(unsigned short *DestStr, const unsigned short *SrcStr)
{
    return (unsigned short *)wcscat((wchar_t *)DestStr, (const wchar_t *)SrcStr);
}

#else   // not _XP_LINUX_                   

#pragma pack(push, 1)

extern unsigned char LowerToUpperTable[256 + 4];
extern unsigned char UpperToLowerTable[256 + 4];

#pragma pack(pop)

inline int _fastcall WcsLen(const unsigned short *Str1)
{
    int Len;

    __asm cld
    __asm MOV    EDI,[Str1]
    __asm MOV    ECX,0xFFFFFFFF
    __asm XOR    ax, ax
    __asm REPNE  SCASW
    __asm MOV    EAX,0xFFFFFFFE
    __asm SUB    EAX,ECX


    __asm MOV    [Len], EAX

    return Len;
}

inline unsigned short * _fastcall WcsCpy(unsigned short *Dest, const unsigned short *Source)
{
    unsigned short *RetStr;

    __asm cld
    __asm MOV    ESI,[Source]
    __asm MOV    EDX,[Dest]
    __asm MOV    EDI,ESI
    __asm MOV    ECX,0xFFFFFFFF
    __asm XOR    EAX,EAX
    __asm REPNE  SCASW
    __asm NOT    ECX
    __asm MOV    EDI,EDX
    __asm MOV    EDX,ECX
    __asm MOV    EAX,EDI
    __asm SHR    ECX,1
    __asm REP    MOVSD
    __asm MOV    ECX,EDX
    __asm AND    ECX,1
    __asm REP    MOVSW
    __asm MOV    [RetStr], EAX

    return RetStr;
}

inline int _fastcall WcsCmp(const unsigned short *Str1, const unsigned short *Str2)
{
    int RetCode;

    __asm cld
    __asm MOV    EDI,[Str1]
    __asm MOV    ESI,[Str2]
    __asm mov    edx,edi
    __asm MOV    ECX, -1
    __asm XOR    EAX,EAX
    __asm REPNE  SCASW
    __asm NOT    ECX
    __asm MOV    EDI,EDX
    __asm XOR    EDX,EDX
    __asm REPE   CMPSW
    __asm MOV    AX, [EDI-2]
    __asm MOV    DX, [ESI-2]
    __asm SUB    EAX, EDX
    __asm mov    [RetCode], EAX

    return RetCode;
}

inline int _fastcall WcsNCmp(const unsigned short *Str1, const unsigned short *Str2, unsigned MaxLen)
{
    int RetCode;
    
    __asm MOV     EDI,[Str1]
    __asm MOV     ESI,[Str2]
    __asm mov     ECX,[MaxLen]
    __asm mov     EDX,EDI

    __asm MOV     EBX,ECX
    __asm XOR     EAX,EAX
    __asm OR      ECX,ECX
    __asm JE      Label1
    __asm REPNE   SCASW
    __asm SUB     EBX,ECX
    __asm MOV     ECX,EBX
    __asm MOV     EDI,EDX
    __asm XOR     EDX,EDX
    __asm REPE    CMPSW
    __asm MOV     AX, [EDI-2]
    __asm MOV     DX, [ESI-2]
    __asm SUB     EAX,EDX

Label1:        
    __asm mov [RetCode], eax

    return RetCode;
}

inline unsigned short * _fastcall wStrEnd(unsigned short *Str1)
{
    unsigned short *RetStr;

    __asm MOV     ECX, 0xFFFFFFFF
    __asm XOR     EAX, EAX
    __asm MOV     EDI, [Str1]
    __asm REPNE   SCASW
    __asm LEA     EAX, [EDI - 2]
    __asm MOV     [RetStr], EAX

    return RetStr;
}

inline unsigned short * _fastcall WcsCat(unsigned short *DestStr, const unsigned short *SrcStr)
{
    return WcsCpy(wStrEnd(DestStr), SrcStr);
}

#endif  // _XP_LINUX_
#endif  // _KWSTRING_H_

