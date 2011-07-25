/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class header
*/


#ifndef HGEFONT_H
#define HGEFONT_H


#include "hge.h"
#include "hgesprite.h"


#define HGETEXT_LEFT        0
#define HGETEXT_RIGHT        1
#define HGETEXT_CENTER        2
#define HGETEXT_HORZMASK    0x03

#define HGETEXT_TOP            0
#define HGETEXT_BOTTOM        4
#define HGETEXT_MIDDLE        8
#define HGETEXT_VERTMASK    0x0C

/*
** HGE Font class
*/
class hgeFont
{
public:
    hgeFont(CONST TCHAR *filename, BOOL bMipmap=FALSE);
    ~hgeFont();

    VOID        Render(FLOAT x, FLOAT y, INT align, CONST TCHAR *string);
    VOID        printf(FLOAT x, FLOAT y, INT align, CONST TCHAR *format, ...);
    VOID        printfb(FLOAT x, FLOAT y, FLOAT w, FLOAT h, INT align, CONST TCHAR *format, ...);

    VOID        SetColor(DWORD col);
    VOID        SetZ(FLOAT z);
    VOID        SetBlendMode(INT blend);
    VOID        SetScale(FLOAT scale) {fScale=scale;}
    VOID        SetProportion(FLOAT prop) { fProportion=prop; }
    VOID        SetRotation(FLOAT rot) {fRot=rot;}
    VOID        SetTracking(FLOAT tracking) {fTracking=tracking;}
    VOID        SetSpacing(FLOAT spacing) {fSpacing=spacing;}

    DWORD       GetColor() CONST {return dwCol;}
    FLOAT       GetZ() CONST {return fZ;}
    INT         GetBlendMode() CONST {return nBlend;}
    FLOAT       GetScale() CONST {return fScale;}
    FLOAT       GetProportion() CONST { return fProportion; }
    FLOAT       GetRotation() CONST {return fRot;}
    FLOAT       GetTracking() CONST {return fTracking;}
    FLOAT       GetSpacing() CONST {return fSpacing;}

    hgeSprite*  GetSprite(TCHAR chr) CONST { return letters[(BYTE)chr]; }
    FLOAT       GetPreWidth(TCHAR chr) CONST { return pre[(BYTE)chr]; }
    FLOAT       GetPostWidth(TCHAR chr) CONST { return post[(BYTE)chr]; }
    FLOAT       GetHeight() CONST { return fHeight; }
    FLOAT       GetStringWidth(CONST TCHAR *string, BOOL bMultiline=TRUE) CONST;

private:
    hgeFont();
    hgeFont(CONST hgeFont &fnt);
    hgeFont&    operator= (CONST hgeFont &fnt);

    TCHAR*       _get_line(TCHAR *file, TCHAR *line);

    static HGE  *hge;

    static TCHAR buffer[1024];

    HTEXTURE    hTexture;
    hgeSprite*  letters[256];
    FLOAT       pre[256];
    FLOAT       post[256];
    FLOAT       fHeight;
    FLOAT       fScale;
    FLOAT       fProportion;
    FLOAT       fRot;
    FLOAT       fTracking;
    FLOAT       fSpacing;

    DWORD       dwCol;
    FLOAT       fZ;
    INT         nBlend;
};


#endif
