/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeStringTable helper class header
*/


#ifndef HGESTRINGS_H
#define HGESTRINGS_H


#include "hge.h"


#define MAXSTRNAMELENGTH 64


struct NamedString
{
    TCHAR            name[MAXSTRNAMELENGTH];
    TCHAR            *string;
    NamedString        *next;
};

/*
** HGE String table class
*/
class hgeStringTable
{
public:
    hgeStringTable(CONST TCHAR *filename);
    ~hgeStringTable();

    TCHAR            *GetString(CONST TCHAR *name);

private:
    hgeStringTable(CONST hgeStringTable &);
    hgeStringTable& operator= (CONST hgeStringTable &);

    NamedString     *strings;

    static HGE      *hge;
};


#endif
