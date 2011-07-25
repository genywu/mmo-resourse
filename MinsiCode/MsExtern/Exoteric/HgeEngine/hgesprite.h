/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeSprite helper class header
*/


#ifndef HGESPRITE_H
#define HGESPRITE_H


#include "hge.h"
#include "hgerect.h"


/*
** HGE Sprite class
*/
class hgeSprite
{
public:
    hgeSprite(HTEXTURE tex, FLOAT x, FLOAT y, FLOAT w, FLOAT h);
    hgeSprite(CONST hgeSprite &spr);
    ~hgeSprite() { hge->Release(); }
    
    
    VOID        Render(FLOAT x, FLOAT y);
    VOID        RenderEx(FLOAT x, FLOAT y, FLOAT rot, FLOAT hscale=1.0f, FLOAT vscale=1.0f);
    VOID        RenderStretch(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2);
    VOID        Render4V(FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT x3, FLOAT y3);

    VOID        SetTexture(HTEXTURE tex);
    VOID        SetTextureRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h, BOOL adjSize = TRUE);
    VOID        SetColor(DWORD col, INT i=-1);
    VOID        SetZ(FLOAT z, INT i=-1);
    VOID        SetBlendMode(INT blend) { quad.blend=blend; }
    VOID        SetHotSpot(FLOAT x, FLOAT y) { hotX=x; hotY=y; }
    VOID        SetFlip(BOOL bX, BOOL bY, BOOL bHotSpot = FALSE);

    HTEXTURE    GetTexture() CONST { return quad.tex; }
    VOID        GetTextureRect(FLOAT *x, FLOAT *y, FLOAT *w, FLOAT *h) CONST { *x=tx; *y=ty; *w=width; *h=height; }
    DWORD       GetColor(INT i=0) CONST { return quad.v[i].col; }
    FLOAT       GetZ(INT i=0) CONST { return quad.v[i].z; }
    INT         GetBlendMode() CONST { return quad.blend; }
    VOID        GetHotSpot(FLOAT *x, FLOAT *y) CONST { *x=hotX; *y=hotY; }
    VOID        GetFlip(BOOL *bX, BOOL *bY) CONST { *bX=bXFlip; *bY=bYFlip; }

    FLOAT       GetWidth() CONST { return width; }
    FLOAT       GetHeight() CONST { return height; }
    hgeRect*    GetBoundingBox(FLOAT x, FLOAT y, hgeRect *rect) CONST { rect->Set(x-hotX, y-hotY, x-hotX+width, y-hotY+height); return rect; }
    hgeRect*    GetBoundingBoxEx(FLOAT x, FLOAT y, FLOAT rot, FLOAT hscale, FLOAT vscale,  hgeRect *rect) CONST;

protected:
    hgeSprite();
    static HGE  *hge;

    hgeQuad     quad;
    FLOAT       tx, ty, width, height;
    FLOAT       tex_width, tex_height;
    FLOAT       hotX, hotY;
    BOOL        bXFlip, bYFlip, bHSFlip;
};


#endif
