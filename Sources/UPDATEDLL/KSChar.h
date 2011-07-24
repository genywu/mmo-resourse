//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KSChar.h
//  Creater     :   Li Yu Xiong
//  Date        :   2002-12-21 20:30:35
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KSCHAR_H
#define _KSCHAR_H

extern unsigned char LowerToUpperTable[256 + 4];
extern unsigned char UpperToLowerTable[256 + 4];

inline int ToUpper(char ch)
{
    return (unsigned char)LowerToUpperTable[(unsigned char)ch];
}

inline int ToLower(char ch)
{
    return (unsigned char)UpperToLowerTable[(unsigned char)ch];
}

#endif // _KSCHAR_H