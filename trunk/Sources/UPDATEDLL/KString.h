//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KString.h
//  Version     :   1.0
//  Creater     :   Freeway Chen
//  Date        :   1998-10-16
//  Comment     :   String Rounte from Delphi 4.0
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KSTRING_H_
#define _KSTRING_H_     1

#include "KSChar.h"

#ifdef _XP_LINUX_

#include <ctype.h>
#include <string.h>


inline int StrLen(const char *Str1)
{
    return strlen(Str1);
}

inline char * StrCpy(char *Dest, const char *Source)
{
    return strcpy(Dest, Source);
}

inline char * StrNCpy(char *Dest,const char *Source, int MaxLen)
{
    return strncpy(Dest, Source, MaxLen);
}

inline int StrCmp(const char *Str1, const char *Str2)
{
    return strcmp(Str1, Str2);
}

inline int StrNCmp(const char *Str1, const char *Str2, unsigned MaxLen)
{
    return strncmp(Str1, Str2, MaxLen);
}

inline int StrICmp(const char *Str1, const char *Str2)
{
    int f, l;

    do {
        f = tolower(*Str1);
        Str1++;
        l = tolower(*Str2);
        Str2++;
    } while ( f && (f == l) );

    return (f - l);
}

inline int StrNICmp(const char *Str1, const char *Str2, unsigned Len)
{
    int f, l;

    if (!Len)
        return 0;

    do {
        f = tolower(*Str1);
        Str1++;
        l = tolower(*Str2);
        Str2++;
    } while (--Len && f && (f == l));

    return (f - l);
}


inline char * StrChr(const char *Str1, const char Chr)
{
    return strchr(Str1, Chr);
}

inline char *StrStr(const char *Str1, const char *Str2)
{
    return strstr(Str1, Str2);
}

inline char * StrRChr(const char *Str1, const char Chr)
{
    return strrchr(Str1, Chr);
}

inline void MemCpy(void *Dst, const void *Src, unsigned int tSize)
{
    memcpy(Dst, Src, tSize);
}

inline void MemMove(void *Dst, const void *Src, unsigned int tSize)
{
    memmove(Dst, Src, tSize);
}


// return 0 equal
// return 1 not equal
inline int  FastMemCmp(const void *Dst, const void *Src, unsigned int tSize)
{
    int nRetCode = memcmp(Dst, Src, tSize);

    return !(nRetCode == 0);
}

inline int MemCmp(const void *Dst, const void *Src, unsigned int tSize)
{
    return memcmp(Dst, Src, tSize);
}

inline void MemSet(void *Buffer, char ch1, size_t Count)
{ 
    memset(Buffer, ch1, Count);
}

inline void MemZero(void *Buffer, size_t Count)
{ 
    memset(Buffer, 0, Count);
}


inline void * MemChr(const void *Buffer, char Chr, size_t Count)
{
    return memchr(Buffer, Chr, Count);
}

// search a Integer
inline void * MemInt(const void *Buffer, int nInt, size_t Count)
{
    int *pnBuffer = (int *)Buffer;

    while (Count--)
    {
        if ((*pnBuffer) == nInt)
            return (void *)pnBuffer;
        
        pnBuffer++;
    }

    return NULL;
}


inline char * StrCat(char *DestStr, const char *SrcStr)
{
    return strcat(DestStr, SrcStr);
}

inline char * StrUpper(char *SrcStr)
{
    char * cp;

    for (cp = SrcStr; *cp; ++cp)
    {
        if (
            ('a' <= *cp) && 
            (*cp <= 'z')
        )
            *cp += 'A' - 'a';
    }

    return SrcStr;
}

inline char * StrSetEmpty(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int StrIsEmpty(const char *pszStr)
{
    return ('\0' == *pszStr);
}

inline char * StrSetEnd(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int StrIsEnd(const char *pszStr)
{
    return ('\0' == *pszStr);
}

#else   // not _XP_LINUX_                   

// ------------------------------------------------------

// VC Complier general :
// push ebx esi edi
// pop edi esi ebx
// code when One Function

// -------------------------------------------------------

//#ifdef __BORLANDC__
//#pragma inline
//#define inline static
//#define __asm _asm
//#endif

#include "KWString.h"

inline int _fastcall StrLen(const char *Str1)
{
    int Len;

    __asm cld
    __asm MOV    edi, [Str1]
    __asm MOV    ECX,0xFFFFFFFF
    __asm XOR    AL,AL
    __asm REPNE  SCASB
    __asm MOV    EAX,0xFFFFFFFE
    __asm SUB    EAX,ECX
    __asm MOV    [Len], EAX

    return Len;

}

inline char * _fastcall StrCpy(char *Dest, const char *Source)
{
    char *RetStr;

    __asm cld
    __asm MOV    ESI,[Source]
    __asm MOV    EDX,[Dest]
    __asm MOV    EDI,ESI
    __asm MOV    ECX,0xFFFFFFFF
    __asm XOR    AL,AL
    __asm REPNE  SCASB
    __asm NOT    ECX
    __asm MOV    EDI,EDX
    __asm MOV    EDX,ECX
    __asm MOV    EAX,EDI
    __asm SHR    ECX,2
    __asm REP    MOVSD
    __asm MOV    ECX,EDX
    __asm AND    ECX,3
    __asm REP    MOVSB
    __asm MOV    [RetStr], EAX

    return RetStr;
}

inline char * _fastcall StrNCpy(char *Dest,const char *Source, int MaxLen)
{
    char *RetStr;

    __asm cld
    __asm MOV    ESI,[Dest]
    __asm MOV    EDX,[Source]
    __asm MOV    EDI, EDX
    __asm MOV    ECX,[MaxLen]
    __asm MOV    EBX,ECX
    __asm XOR    AL,AL
    __asm TEST   ECX, ECX
    __asm JZ     Label2
    __asm REPNE  SCASB
    __asm JNE    Label1
    __asm INC    ECX
    Label1:
    __asm SUB    EBX,ECX
    __asm MOV    EDI,ESI
    __asm MOV    ESI,EDX
    __asm MOV    EDX,EDI
    __asm MOV    ECX,EBX
    __asm SHR    ECX,2
    __asm REP    MOVSD
    __asm MOV    ECX,EBX
    __asm AND    ECX,3
    __asm REP    MOVSB
    __asm STOSB
    __asm MOV    ESI,EDX

    Label2://Exit
    __asm mov [RetStr], ESI

    return RetStr;
}

inline int _fastcall StrCmp(const char *Str1, const char *Str2)
{
    int RetCode;

    __asm cld
    __asm MOV    EDI,[Str1]
    __asm MOV    ESI,[Str2]
    __asm mov    edx,edi
    __asm MOV    ECX, -1
    __asm XOR    EAX,EAX
    __asm REPNE  SCASB
    __asm NOT    ECX
    __asm MOV    EDI,EDX
    __asm XOR    EDX,EDX
    __asm REPE   CMPSB
    __asm MOV    AL,[EDI-1]
    __asm MOV    DL,[ESI-1]
    __asm SUB    EAX, EDX
    __asm mov    [RetCode], eax

    return RetCode;
}

inline int _fastcall StrNCmp(const char *Str1, const char *Str2, unsigned MaxLen)
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
    __asm REPNE   SCASB
    __asm SUB     EBX,ECX
    __asm MOV     ECX,EBX
    __asm MOV     EDI,EDX
    __asm XOR     EDX,EDX
    __asm REPE    CMPSB
    __asm MOV     AL, [EDI-1]
    __asm MOV     DL, [ESI-1]
    __asm SUB     EAX,EDX

Label1:        
    __asm mov [RetCode], eax

    return RetCode;
}

inline int _fastcall StrICmp(const char *Str1, const char *Str2)
{
    int RetCode;

    __asm cld
    __asm MOV    EDI, [Str1]
    __asm MOV    ESI, [Str2]
    __asm mov    edx, edi
    __asm mov    ebx, offset LowerToUpperTable
    __asm MOV    ECX, -1
    __asm XOR    EAX,EAX
    __asm REPNE  SCASB
    __asm NOT    ECX
    __asm MOV    EDI,EDX
    __asm XOR    EDX,EDX

    Label1:
    __asm REPE   CMPSB
    __asm JE     Lable4 // exit


    __asm MOVZX  EAX, byte ptr [EDI-1]
    __asm MOVZX  EDX, byte ptr [ESI-1]
    __asm MOVZX  EAX, byte ptr [EBX + EAX]
    __asm MOVZX  EDX, byte ptr [EBX + EDX]
    __asm SUB    EAX, EDX
    __asm JE     Label1

    Lable4:
    __asm mov    [RetCode], eax

    return RetCode;
}

inline int _fastcall StrNICmp(const char *Str1, const char *Str2, unsigned Len)
{
    int RetCode;

    __asm cld
    __asm MOV    EDI, [Str1]
    __asm MOV    ESI, [Str2]
    __asm MOV    ECX, [Len]
    __asm mov    edx, edi
    __asm mov    ebx, ecx
    //__asm MOV    ECX, -1
    __asm XOR    EAX,EAX
    __asm REPNE  SCASB
    __asm sub    ebx, ecx
    __asm MOV    EDI,EDX
    __asm mov    ecx, ebx
    __asm XOR    EDX,EDX
    __asm mov    ebx, offset LowerToUpperTable

    Label1:
    __asm REPE   CMPSB
    __asm JE     Lable4 // exit EAX = 0


    __asm MOVZX  EAX, byte ptr [EDI - 1]
    __asm MOVZX  EDX, byte ptr [ESI - 1]
    __asm MOVZX  EAX, byte ptr [EBX + EAX]
    __asm MOVZX  EDX, byte ptr [EBX + EDX]
    __asm SUB    EAX, EDX
    __asm JE     Label1

    Lable4:
    __asm mov    [RetCode], eax

    return RetCode;
}


inline char * _fastcall StrChr(const char *Str1, const char Chr)
{
    char *RetStr;

    __asm cld
    __asm MOV    EAX, [Str1]
    __asm MOV    DL, [Chr]
    __asm PUSH   EAX
    __asm MOV    EDI, EAX
    __asm MOV    ECX,0xFFFFFFFF
    __asm XOR    AL,AL
    __asm REPNE  SCASB
    __asm NOT    ECX
    __asm POP    EDI
    __asm MOV    AL,DL
    __asm REPNE  SCASB
    __asm MOV    EAX,0
    __asm JNE    Label1
    __asm MOV    EAX,EDI
    __asm DEC    EAX
    Label1:
    __asm MOV    [RetStr], EAX

    return RetStr;
}

inline char * _fastcall StrStr(const char *Str1, const char *Str2)
{
    char *RetStr;

    __asm CLD
    __asm MOV     EDX,[Str2]
    __asm MOV     EBX,[Str1]

    __asm MOV     EDI,EDX
    __asm XOR     AL,AL
    __asm MOV     ECX,0FFFFFFFFH
    __asm REPNE   SCASB
    __asm NOT     ECX
    __asm DEC     ECX
    __asm JE      Lable2

    __asm MOV     ESI,ECX
    __asm MOV     EDI,EBX
    __asm MOV     ECX,0FFFFFFFFH
    __asm REPNE   SCASB
    __asm NOT     ECX
    __asm SUB     ECX,ESI
    __asm JBE     Lable2

    __asm MOV     EDI,EBX
    __asm LEA     EBX,[ESI-1]
Lable1:    
    __asm MOV     ESI,EDX
    __asm LODSB
    __asm REPNE   SCASB
    __asm JNE     Lable2

    __asm MOV     EAX,ECX
    __asm PUSH    EDI
    __asm MOV     ECX,EBX
    __asm REPE    CMPSB
    __asm POP     EDI
    __asm MOV     ECX,EAX
    __asm JNE     Lable1

    __asm LEA     EAX,[EDI-1]
    __asm JMP     Lable3
Lable2:    
    __asm XOR     EAX,EAX
Lable3:    
    __asm MOV    [RetStr], EAX

    return RetStr;
}

inline char * _fastcall StrRChr(const char *Str1, const char Chr)
{
    char *RetStr;

    __asm MOV     EDI,[Str1]
    __asm MOV     DL, [Chr]
    __asm MOV     ECX,0xFFFFFFFF
    __asm XOR     AL,AL
    __asm REPNE   SCASB
    __asm NOT     ECX
    __asm STD
    __asm DEC     EDI
    __asm MOV     AL, DL
    __asm REPNE   SCASB
    __asm MOV     EAX,0
    __asm JNE     Lable1
    __asm MOV     EAX,EDI
    __asm INC     EAX
    Lable1:
    __asm CLD

    __asm mov   [RetStr], eax

    return RetStr;
}

/*
#ifdef _INTEL_P4

inline void _fastcall MemCpy(void *Dst, const void *Src, unsigned int tSize)
{
    
    __asm cld
    __asm mov esi, [Src]
    __asm mov edi, [Dst]
    __asm prefetchnta [esi]
    __asm prefetchnta [esi + 64]
    __asm prefetchnta [esi + 128]
    __asm prefetchnta [esi + 192]
    __asm prefetchnta [esi + 256]
    __asm mov ecx, [tSize]
    __asm mov edx, ecx
    __asm shr ecx, 7
    __asm jz Label_Last

Label_Loop:
    __asm prefetchnta [esi + 320]

    __asm movss xmm0, [esi]
    __asm movss xmm1, [esi + 16]
    __asm movss xmm2, [esi + 32]
    __asm movss xmm3, [esi + 48]
    __asm movss xmm4, [esi + 64]
    __asm movss xmm5, [esi + 80]
    __asm movss xmm6, [esi + 96]
    __asm movss xmm7, [esi + 112]

    __asm movss [edi], xmm0
    __asm add esi, 128
    __asm movss [edi + 16], xmm1
    __asm movss [edi + 32], xmm2
    __asm movss [edi + 48], xmm3
    __asm movss [edi + 64], xmm4
    __asm movss [edi + 80], xmm5
    __asm movss [edi + 96], xmm6
    __asm movss [edi + 112], xmm7
    __asm add edi, 128

    __asm dec ecx
    __asm jnz Label_Loop

Label_Last:
    __asm and edx, 0x7f

    __asm mov ecx, edx
    __asm shr ecx, 2
    __asm rep movsd

    __asm mov ecx, edx
    __asm and ecx, 0x3
    __asm rep movsb          // mov BYTE
}


#else
*/
inline void _fastcall MemCpy(void *Dst, const void *Src, unsigned int tSize)
{
    //Touch_Register(edx)
    //Touch_Register(ecx)
    //Touch_Register(esi)
    //Touch_Register(edi)

    __asm cld
    __asm mov esi, [Src]
    __asm mov edi, [Dst]
    __asm mov ecx, [tSize]
    __asm mov edx, ecx
    __asm shr ecx, 2   
    __asm rep movsd          // mov DWORD
    __asm mov ecx, edx 
    __asm and ecx, 0x03
    __asm rep movsb          // mov BYTE
}
//#endif

// Get Following Code From C++Builder 5.0
inline void _fastcall MemMove(void *Dst, const void *Src, unsigned int tSize)
{
    __asm mov esi, [Src]
    __asm mov edi, [Dst]
    __asm mov ecx, [tSize]

    __asm cmp edi, esi              // dest < source ?
    __asm mov edx, ecx
    __asm jb  MOV_UP                // yes, do a simple forward move
    __asm je  MOV_END               // dest == source ? do nothing
    
    // MOV_DOWN
    __asm STD                       // force esi and edi to decrement
    __asm lea esi, [esi + ecx - 1]  // point to last byte of source buffer
    __asm lea edi, [edi + ecx - 1]  // point to last byte of dest buffer
    __asm and ecx, 3                // copy trailing bytes
    __asm rep movsb
    __asm mov ecx, edx              // get original count back
    __asm shr ecx, 2                // calculate no. of longwords
    __asm sub esi, 3                // point to previous longword in source
    __asm sub edi, 3                // point to previous longword in dest
    __asm rep movsd                 // copy longwords
    __asm cld                       // clear direction flag
    __asm jmp short MOV_END

    MOV_UP:
    __asm cld
    __asm shr ecx, 2
    __asm rep movsd                 // copy longwords
    __asm mov ecx, edx              // get original count back
    __asm and ecx, 3                // calculate remainder byte count (0-3)
    __asm rep movsb                 // copy remaining bytes

    MOV_END:
    return;
}


// return 0 equal
// return 1 not equal
inline int _fastcall FastMemCmp(const void *Dst, const void *Src, unsigned int tSize)
{
    //Touch_Register(edx)
    //Touch_Register(ecx)
    //Touch_Register(esi)
    //Touch_Register(edi)
    int RetCode;

    __asm cld
    __asm mov esi, [Src]
    __asm mov edi, [Dst]
    __asm mov ecx, [tSize]

    __asm mov eax, 1      // not equal

    __asm mov edx, ecx
    __asm shr ecx, 2
    __asm and edx, 0x03
    __asm rep cmpsd          // cmp DWORD
    __asm jne Exit2
    __asm mov ecx, edx 
    __asm rep cmpsb          // cmp BYTE
    __asm jne Exit2

    __asm dec eax           // equal  0
    Exit2:
    __asm mov [RetCode], eax

    return RetCode;
}

inline int _fastcall MemCmp(const void *Dst, const void *Src, unsigned int tSize){
    //Touch_Register(edx)
    //Touch_Register(ecx)
    //Touch_Register(esi)
    //Touch_Register(edi)
    int RetCode;

    __asm cld
    __asm mov esi, [Src]
    __asm mov edi, [Dst]
    __asm mov ecx, [tSize]
    __asm xor eax, eax
    __asm xor edx, edx


    __asm rep cmpsb          // mov DWORD
    __asm jz Exit1
    // not equal
    __asm mov al, [edi-1]
    __asm mov dl, [esi-1]
    __asm sub eax, edx

    Exit1:
    __asm mov [RetCode], eax

    return RetCode;
}

//#ifdef _INTEL_P4
//
//inline void _fastcall MemSet(void *Buffer, char ch1, size_t Count)
//{
//    __asm CLD
//    __asm MOV     EDI, [Buffer] // Point EDI to destination
//    __asm MOV     ECX, [Count]
//    __asm MOV     DL, [ch1]
//
//    __asm mov     DH, DL
//    __asm mov     eax, edx
//    __asm shl     eax, 16
//    __asm mov     ax, dx
//    __asm movd    xmm0, eax
//    __asm movd    xmm1, eax
//    __asm pslldq  xmm0, 32
//    __asm por     xmm0, xmm1
//    __asm mov     xmm1, xmm0
//    __asm pslldq  xmm0, 64
//    __asm por     xmm0, xmm1
//
//
//
//
//}
//
//#else

inline void _fastcall MemSet(void *Buffer, char ch1, size_t Count)
{ 
    __asm CLD
    __asm MOV     EDI, [Buffer] // Point EDI to destination    
    __asm MOV     ECX, [Count]
    __asm MOV     DL, [ch1]
    __asm MOV     DH,DL   // Fill EAX with value repeated 4 times
    __asm MOV     EAX,EDX
    __asm SHL     EAX,16
    __asm MOV     AX,DX
    __asm MOV     EDX, ECX
    __asm SAR     ECX,2
    __asm JS      Exit1
    __asm REP     STOSD   // Fill count DIV 4 dwords
    __asm MOV     ECX,EDX
    __asm AND     ECX,3
    __asm REP     STOSB   // Fill count MOD 4 bytes

    Exit1:
    return;
}

inline void _fastcall MemZero(void *Buffer, size_t Count)
{ 
    __asm CLD
    __asm MOV     EDI, [Buffer] // Point EDI to destination    
    __asm MOV     ECX, [Count]
    __asm xor     eax, eax
    __asm MOV     EDX, ECX
    __asm SAR     ECX,2
    __asm JS      Exit1
    __asm REP     STOSD   // Fill count DIV 4 dwords
    __asm MOV     ECX,EDX
    __asm AND     ECX,3
    __asm REP     STOSB   // Fill count MOD 4 bytes

    Exit1:
    return;
}

//#endif

inline void * _fastcall MemChr(const void *Buffer, char Chr, size_t Count)
{
    void *RetBuffer;

    __asm cld
    __asm MOV    EDI, [Buffer]
    __asm MOV   ECX, [Count]
    __asm MOV    AL, [Chr]
    __asm REPNE  SCASB
    __asm MOV    EAX, 0
    __asm JNE    Label1
    __asm MOV    EAX,EDI
    __asm DEC    EAX

    Label1:
    __asm MOV    [RetBuffer], EAX

    return RetBuffer;
}

// search a Integer
inline void * _fastcall MemInt(const void *Buffer, int nInt, size_t Count)
{
    void *RetBuffer;

    __asm MOV    [RetBuffer], 0
    __asm CLD
    __asm MOV    EDI, [Buffer]
    __asm MOV    ECX, [Count]
    __asm SUB    ECX, 4         //
    __asm JB     Label0         // for protect bounds, follow 'cmp eax, [edi]'
    __asm INC    ECX            //
    __asm MOV    EAX, [nInt]
    Loop0:
    __asm REPNE  SCASB
    __asm JNE    Label0

    __asm CMP    EAX, [EDI - 1]
    __asm JNE    Loop0

    __asm LEA    EAX, [EDI - 1]

    __asm MOV    [RetBuffer], EAX
    Label0:
    return RetBuffer;
}

inline char * _fastcall StrEnd(char *Str1)
{
    char *RetStr;

    __asm MOV     ECX,0xFFFFFFFF
    __asm XOR     AL,AL
    __asm MOV     EDI,[Str1]
    __asm REPNE   SCASB
    __asm LEA     EAX,[EDI-1]
    __asm MOV     [RetStr], EAX

    return RetStr;
}

inline char * _fastcall StrCat(char *DestStr, const char *SrcStr)
{
    return StrCpy(StrEnd(DestStr), SrcStr);
}

inline char * _fastcall StrUpper(char *SrcStr)
{
    char *RetStr;

    __asm push esi
    __asm mov esi, [SrcStr]
    __asm mov edx, [SrcStr]

    Label1:
    __asm movzx eax, byte ptr [esi]
    __asm inc esi
    __asm test eax, eax
    __asm jz Label2
    __asm mov al, LowerToUpperTable[eax]
    __asm mov [esi - 1], al
    __asm jmp short Label1

    Label2:
    __asm mov [RetStr], edx
    __asm pop esi

    return RetStr;
}

inline char * _fastcall StrSetEmpty(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int _fastcall StrIsEmpty(const char *pszStr)
{
    return ('\0' == *pszStr);
}

inline char * _fastcall StrSetEnd(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int _fastcall StrIsEnd(const char *pszStr)
{
    return ('\0' == *pszStr);
}


/*
inline char *StrMove(char *Dest, char *Source,int Count)
{
    __asm{
        MOV     ESI,[Source]
        MOV     EDI,[Dest]
        MOV     EDX,[Count]
        CMP     EDI,ESI
        JA      Label1
        JE      Label2
        SHR     ECX,2
        REP     MOVSD
        MOV     ECX,EDX
        AND     ECX,3
        REP     MOVSB
        JMP     Label2
    }
    Label1:    
    __asm{
        LEA     ESI,[ESI+ECX-1]
        LEA     EDI,[EDI+ECX-1]
        AND     ECX,3
        STD
        REP     MOVSB
        SUB     ESI,3
        SUB     EDI,3
        MOV     ECX,EDX
        SHR     ECX,2
        REP     MOVSD
        CLD
    }
    Lable2:
}

function StrCopy(Dest, Source: PChar): PChar; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        MOV     ESI,EAX
        MOV     EDI,EDX
        MOV     ECX,0FFFFFFFFH
        XOR     AL,AL
        REPNE   SCASB
        NOT     ECX
        MOV     EDI,ESI
        MOV     ESI,EDX
        MOV     EDX,ECX
        MOV     EAX,EDI
        SHR     ECX,2
        REP     MOVSD
        MOV     ECX,EDX
        AND     ECX,3
        REP     MOVSB
        POP     ESI
        POP     EDI
end;

function StrECopy(Dest, Source: PChar): PChar; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        MOV     ESI,EAX
        MOV     EDI,EDX
        MOV     ECX,0FFFFFFFFH
        XOR     AL,AL
        REPNE   SCASB
        NOT     ECX
        MOV     EDI,ESI
        MOV     ESI,EDX
        MOV     EDX,ECX
        SHR     ECX,2
        REP     MOVSD
        MOV     ECX,EDX
        AND     ECX,3
        REP     MOVSB
        LEA     EAX,[EDI-1]
        POP     ESI
        POP     EDI
end;

function StrNCpy(Dest, Source: PChar; MaxLen: Cardinal): PChar; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        MOV     ESI,EAX
        MOV     EDI,EDX
        MOV     EBX,ECX
        XOR     AL,AL
        TEST    ECX,ECX
        JZ      @@1
        REPNE   SCASB
        JNE     @@1
        INC     ECX
@@1:    SUB     EBX,ECX
        MOV     EDI,ESI
        MOV     ESI,EDX
        MOV     EDX,EDI
        MOV     ECX,EBX
        SHR     ECX,2
        REP     MOVSD
        MOV     ECX,EBX
        AND     ECX,3
        REP     MOVSB
        STOSB
        MOV     EAX,EDX
        POP     EBX
        POP     ESI
        POP     EDI
end;

function StrPCopy(Dest: PChar; const Source: string): PChar;
begin
  Result := StrNCpy(Dest, PChar(Source), Length(Source));
end;

function StrPLCopy(Dest: PChar; const Source: string;
  MaxLen: Cardinal): PChar;
begin
  Result := StrNCpy(Dest, PChar(Source), MaxLen);
end;

function StrCat(Dest, Source: PChar): PChar;
begin
  StrCopy(StrEnd(Dest), Source);
  Result := Dest;
end;

function StrLCat(Dest, Source: PChar; MaxLen: Cardinal): PChar; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        MOV     EDI,Dest
        MOV     ESI,Source
        MOV     EBX,MaxLen
        CALL    StrEnd
        MOV     ECX,EDI
        ADD     ECX,EBX
        SUB     ECX,EAX
        JBE     @@1
        MOV     EDX,ESI
        CALL    StrNCpy
@@1:    MOV     EAX,EDI
        POP     EBX
        POP     ESI
        POP     EDI
end;

function StrComp(Str1, Str2: PChar): Integer; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        MOV     EDI,EDX
        MOV     ESI,EAX
        MOV     ECX,0FFFFFFFFH
        XOR     EAX,EAX
        REPNE   SCASB
        NOT     ECX
        MOV     EDI,EDX
        XOR     EDX,EDX
        REPE    CMPSB
        MOV     AL,[ESI-1]
        MOV     DL,[EDI-1]
        SUB     EAX,EDX
        POP     ESI
        POP     EDI
end;

function StrIComp(Str1, Str2: PChar): Integer; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        MOV     EDI,EDX
        MOV     ESI,EAX
        MOV     ECX,0FFFFFFFFH
        XOR     EAX,EAX
        REPNE   SCASB
        NOT     ECX
        MOV     EDI,EDX
        XOR     EDX,EDX
@@1:    REPE    CMPSB
        JE      @@4
        MOV     AL,[ESI-1]
        CMP     AL,'a'
        JB      @@2
        CMP     AL,'z'
        JA      @@2
        SUB     AL,20H
@@2:    MOV     DL,[EDI-1]
        CMP     DL,'a'
        JB      @@3
        CMP     DL,'z'
        JA      @@3
        SUB     DL,20H
@@3:    SUB     EAX,EDX
        JE      @@1
@@4:    POP     ESI
        POP     EDI
end;

function StrLComp(Str1, Str2: PChar; MaxLen: Cardinal): Integer; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        MOV     EDI,EDX
        MOV     ESI,EAX
        MOV     EBX,ECX
        XOR     EAX,EAX
        OR      ECX,ECX
        JE      @@1
        REPNE   SCASB
        SUB     EBX,ECX
        MOV     ECX,EBX
        MOV     EDI,EDX
        XOR     EDX,EDX
        REPE    CMPSB
        MOV     AL,[ESI-1]
        MOV     DL,[EDI-1]
        SUB     EAX,EDX
@@1:    POP     EBX
        POP     ESI
        POP     EDI
end;

function StrLIComp(Str1, Str2: PChar; MaxLen: Cardinal): Integer; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        MOV     EDI,EDX
        MOV     ESI,EAX
        MOV     EBX,ECX
        XOR     EAX,EAX
        OR      ECX,ECX
        JE      @@4
        REPNE   SCASB
        SUB     EBX,ECX
        MOV     ECX,EBX
        MOV     EDI,EDX
        XOR     EDX,EDX
@@1:    REPE    CMPSB
        JE      @@4
        MOV     AL,[ESI-1]
        CMP     AL,'a'
        JB      @@2
        CMP     AL,'z'
        JA      @@2
        SUB     AL,20H
@@2:    MOV     DL,[EDI-1]
        CMP     DL,'a'
        JB      @@3
        CMP     DL,'z'
        JA      @@3
        SUB     DL,20H
@@3:    SUB     EAX,EDX
        JE      @@1
@@4:    POP     EBX
        POP     ESI
        POP     EDI
end;

function StrScan(Str: PChar; Chr: Char): PChar; assembler;
asm
        PUSH    EDI
        PUSH    EAX
        MOV     EDI,Str
        MOV     ECX,0FFFFFFFFH
        XOR     AL,AL
        REPNE   SCASB
        NOT     ECX
        POP     EDI
        MOV     AL,Chr
        REPNE   SCASB
        MOV     EAX,0
        JNE     @@1
        MOV     EAX,EDI
        DEC     EAX
@@1:    POP     EDI
end;

function StrRScan(Str: PChar; Chr: Char): PChar; assembler;
asm
        PUSH    EDI
        MOV     EDI,Str
        MOV     ECX,0FFFFFFFFH
        XOR     AL,AL
        REPNE   SCASB
        NOT     ECX
        STD
        DEC     EDI
        MOV     AL,Chr
        REPNE   SCASB
        MOV     EAX,0
        JNE     @@1
        MOV     EAX,EDI
        INC     EAX
@@1:    CLD
        POP     EDI
end;

function StrPos(Str1, Str2: PChar): PChar; assembler;
asm
        PUSH    EDI
        PUSH    ESI
        PUSH    EBX
        OR      EAX,EAX
        JE      @@2
        OR      EDX,EDX
        JE      @@2
        MOV     EBX,EAX
        MOV     EDI,EDX
        XOR     AL,AL
        MOV     ECX,0FFFFFFFFH
        REPNE   SCASB
        NOT     ECX
        DEC     ECX
        JE      @@2
        MOV     ESI,ECX
        MOV     EDI,EBX
        MOV     ECX,0FFFFFFFFH
        REPNE   SCASB
        NOT     ECX
        SUB     ECX,ESI
        JBE     @@2
        MOV     EDI,EBX
        LEA     EBX,[ESI-1]
@@1:    MOV     ESI,EDX
        LODSB
        REPNE   SCASB
        JNE     @@2
        MOV     EAX,ECX
        PUSH    EDI
        MOV     ECX,EBX
        REPE    CMPSB
        POP     EDI
        MOV     ECX,EAX
        JNE     @@1
        LEA     EAX,[EDI-1]
        JMP     @@3
@@2:    XOR     EAX,EAX
@@3:    POP     EBX
        POP     ESI
        POP     EDI
end;

function StrUpper(Str: PChar): PChar; assembler;
asm
        PUSH    ESI
        MOV     ESI,Str
        MOV     EDX,Str
@@1:    LODSB
        OR      AL,AL
        JE      @@2
        CMP     AL,'a'
        JB      @@1
        CMP     AL,'z'
        JA      @@1
        SUB     AL,20H
        MOV     [ESI-1],AL
        JMP     @@1
@@2:    XCHG    EAX,EDX
        POP     ESI
end;

function StrLower(Str: PChar): PChar; assembler;
asm
        PUSH    ESI
        MOV     ESI,Str
        MOV     EDX,Str
@@1:    LODSB
        OR      AL,AL
        JE      @@2
        CMP     AL,'A'
        JB      @@1
        CMP     AL,'Z'
        JA      @@1
        ADD     AL,20H
        MOV     [ESI-1],AL
        JMP     @@1
@@2:    XCHG    EAX,EDX
        POP     ESI
end;

*/

extern "C"{
void *  __cdecl memcpy(void *, const void *, size_t);
int     __cdecl memcmp(const void *, const void *, size_t);
void *  __cdecl memset(void *, int, size_t);
char *  __cdecl strcpy(char *, const char *);
char *  __cdecl strcat(char *, const char *);
int     __cdecl strcmp(const char *, const char *);
size_t  __cdecl strlen(const char *);
}

#pragma function(memcpy, memcmp, memset, strlen, strcpy, strcat, strcmp)

inline void *  __cdecl memcpy(void *pvDst, const void *pvSrc, size_t Count)
{
    MemCpy(pvDst, pvSrc, Count);
    return pvDst;
}

inline void *  __cdecl memmove(void *pvDst, const void *pvSrc, size_t Count)
{
    MemMove(pvDst, pvSrc, Count);
    return pvDst;
}


inline int     __cdecl memcmp(const void *pvDst, const void *pvSrc, size_t Count)
{
    return MemCmp(pvDst, pvSrc, Count);
}

inline void *  __cdecl memset(void *pvDst, int ch, size_t Count)
{
    
    MemSet(pvDst, (char)ch, Count);
    return pvDst;
}

inline void *  __cdecl memchr(const void *pvBuf, int ch, size_t Count)
{
    return MemChr(pvBuf, (char)ch, Count);
}


inline char *  __cdecl strcpy(char *pszDst, const char *pszSrc)
{
    return StrCpy(pszDst, pszSrc);
}

inline char *  __cdecl strcat(char *pszDst, const char *pszSrc)
{
    return StrCat(pszDst, pszSrc);
}

inline int     __cdecl strcmp(const char *pszStr1, const char *pszStr2)
{
    return StrCmp(pszStr1, pszStr2);
}

inline int __cdecl stricmp(const char *pszStr1, const char *pszStr2)
{
    return StrICmp(pszStr1, pszStr2);
}


inline size_t  __cdecl strlen(const char *pszStr)
{
    return StrLen(pszStr);
}

inline char *  __cdecl strncpy(char *pszDst, const char *pszSrc, size_t MaxLen)
{
    return StrNCpy(pszDst, pszSrc, MaxLen);
}

inline int     __cdecl strncmp(const char *pszStr1, const char *pszStr2, size_t MaxLen)
{
    return StrNCmp(pszStr1, pszStr2, MaxLen);
}

inline char *  __cdecl strchr(const char *pszStr, int ch)
{
    return StrChr(pszStr, (char)ch);
}

inline char *  __cdecl strrchr(const char *pszStr, int ch)
{
    return StrRChr(pszStr, (char)ch);
}

inline char * __cdecl strsetempty(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int __cdecl strisempty(const char *pszStr)
{
    return ('\0' == *pszStr);
}


inline char * __cdecl strsetend(char *pszStr)
{
    *pszStr = '\0';
    return pszStr;
}

inline int __cdecl strisend(const char *pszStr)
{
    return ('\0' == *pszStr);
}

#endif  // _XP_LINUX_


#endif  // _KSTRING_H_
