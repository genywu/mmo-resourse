/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeDistortionMesh helper class header
*/


#ifndef HGEDISTORT_H
#define HGEDISTORT_H


#include "hge.h"

#define HGEDISP_NODE    0
#define HGEDISP_TOPLEFT 1
#define HGEDISP_CENTER  2

/*
** HGE Distortion mesh class
*/
class hgeDistortionMesh
{
public:
     hgeDistortionMesh(INT cols, INT rows);
     hgeDistortionMesh(CONST hgeDistortionMesh &dm);
     ~hgeDistortionMesh();

     hgeDistortionMesh&    operator= (CONST hgeDistortionMesh &dm);

     VOID        Render(FLOAT x, FLOAT y);
     VOID        Clear(DWORD col=0xFFFFFFFF, FLOAT z=0.5f);

     VOID        SetTexture(HTEXTURE tex);
     VOID        SetTextureRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h);
     VOID        SetBlendMode(INT blend);
     VOID        SetZ(INT col, INT row, FLOAT z);
     VOID        SetColor(INT col, INT row, DWORD color);
     VOID        SetDisplacement(INT col, INT row, FLOAT dx, FLOAT dy, INT ref);

     HTEXTURE   GetTexture() CONST {return quad.tex;}
     VOID       GetTextureRect(FLOAT *x, FLOAT *y, FLOAT *w, FLOAT *h) CONST { *x=tx; *y=ty; *w=width; *h=height; }
     INT        GetBlendMode() CONST { return quad.blend; }
     FLOAT      GetZ(INT col, INT row) CONST;
     DWORD      GetColor(INT col, INT row) CONST;
     VOID        GetDisplacement(INT col, INT row, FLOAT *dx, FLOAT *dy, INT ref) CONST;

     INT        GetRows() { return nRows; }
     INT        GetCols() { return nCols; }

private:
    hgeDistortionMesh();

    static HGE  *hge;

    hgeVertex   *disp_array;
    INT         nRows, nCols;
    FLOAT       cellw,cellh;
    FLOAT       tx,ty,width,height;
    hgeQuad     quad;
};

#endif
