#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeDistortionMesh helper class implementation
*/

#include "hgedistort.h"


HGE *hgeDistortionMesh::hge=0;


hgeDistortionMesh::hgeDistortionMesh(INT cols, INT rows)
{
    INT i;
    
    hge=hgeCreate(HGE_VERSION);

    nRows=rows;
    nCols=cols;
    cellw=cellh=0;
    quad.tex=0;
    quad.blend=BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE;
    disp_array=new hgeVertex[rows*cols];

    for(i=0;i<rows*cols;i++)
    {
        disp_array[i].x=0.0f;
        disp_array[i].y=0.0f;
        disp_array[i].tx=0.0f;
        disp_array[i].ty=0.0f;
        
        disp_array[i].z=0.5f;
        disp_array[i].col=0xFFFFFFFF;
    }
}

hgeDistortionMesh::hgeDistortionMesh(CONST hgeDistortionMesh &dm)
{
    hge=hgeCreate(HGE_VERSION);

    nRows=dm.nRows;
    nCols=dm.nCols;
    cellw=dm.cellw;
    cellh=dm.cellh;
    tx=dm.tx;
    ty=dm.ty;
    width=dm.width;
    height=dm.height;
    quad=dm.quad;

    disp_array=new hgeVertex[nRows*nCols];
    memcpy(disp_array, dm.disp_array, sizeof(hgeVertex)*nRows*nCols);
}

hgeDistortionMesh::~hgeDistortionMesh()
{
    delete[] disp_array;
    hge->Release();
}

hgeDistortionMesh& hgeDistortionMesh::operator= (CONST hgeDistortionMesh &dm)
{
    if(this!=&dm)
    {
        nRows=dm.nRows;
        nCols=dm.nCols;
        cellw=dm.cellw;
        cellh=dm.cellh;
        tx=dm.tx;
        ty=dm.ty;
        width=dm.width;
        height=dm.height;
        quad=dm.quad;

        delete[] disp_array;
        disp_array=new hgeVertex[nRows*nCols];
        memcpy(disp_array, dm.disp_array, sizeof(hgeVertex)*nRows*nCols);
    }

    return *this;
    
}

VOID hgeDistortionMesh::SetTexture(HTEXTURE tex)
{
    quad.tex=tex;
}

VOID hgeDistortionMesh::SetTextureRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h)
{
    INT i,j;
    FLOAT tw,th;

    tx=x; ty=y; width=w; height=h;

    if (quad.tex)
    {
        tw=(FLOAT)hge->Texture_GetWidth(quad.tex);
        th=(FLOAT)hge->Texture_GetHeight(quad.tex);
    }
    else
    {
        tw = w;
        th = h;
    }

    cellw=w/(nCols-1);
    cellh=h/(nRows-1);

    for(j=0; j<nRows; j++)
        for(i=0; i<nCols; i++)
        {
            disp_array[j*nCols+i].tx=(x+i*cellw)/tw;
            disp_array[j*nCols+i].ty=(y+j*cellh)/th;

            disp_array[j*nCols+i].x=i*cellw;
            disp_array[j*nCols+i].y=j*cellh;
        }
}

VOID hgeDistortionMesh::SetBlendMode(INT blend)
{
    quad.blend=blend;
}

VOID hgeDistortionMesh::Clear(DWORD col, FLOAT z)
{
    INT i,j;

    for(j=0; j<nRows; j++)
        for(i=0; i<nCols; i++)
        {
            disp_array[j*nCols+i].x=i*cellw;
            disp_array[j*nCols+i].y=j*cellh;
            disp_array[j*nCols+i].col=col;
            disp_array[j*nCols+i].z=z;
        }
}

VOID hgeDistortionMesh::Render(FLOAT x, FLOAT y)
{
    INT i,j,idx;

    for(j=0; j<nRows-1; j++)
        for(i=0; i<nCols-1; i++)
        {
            idx=j*nCols+i;

            quad.v[0].tx=disp_array[idx].tx;
            quad.v[0].ty=disp_array[idx].ty;
            quad.v[0].x=x+disp_array[idx].x;
            quad.v[0].y=y+disp_array[idx].y;
            quad.v[0].z=disp_array[idx].z;
            quad.v[0].col=disp_array[idx].col;

            quad.v[1].tx=disp_array[idx+1].tx;
            quad.v[1].ty=disp_array[idx+1].ty;
            quad.v[1].x=x+disp_array[idx+1].x;
            quad.v[1].y=y+disp_array[idx+1].y;
            quad.v[1].z=disp_array[idx+1].z;
            quad.v[1].col=disp_array[idx+1].col;

            quad.v[2].tx=disp_array[idx+nCols+1].tx;
            quad.v[2].ty=disp_array[idx+nCols+1].ty;
            quad.v[2].x=x+disp_array[idx+nCols+1].x;
            quad.v[2].y=y+disp_array[idx+nCols+1].y;
            quad.v[2].z=disp_array[idx+nCols+1].z;
            quad.v[2].col=disp_array[idx+nCols+1].col;

            quad.v[3].tx=disp_array[idx+nCols].tx;
            quad.v[3].ty=disp_array[idx+nCols].ty;
            quad.v[3].x=x+disp_array[idx+nCols].x;
            quad.v[3].y=y+disp_array[idx+nCols].y;
            quad.v[3].z=disp_array[idx+nCols].z;
            quad.v[3].col=disp_array[idx+nCols].col;

            hge->Gfx_RenderQuad(&quad);
        }
}

VOID hgeDistortionMesh::SetZ(INT col, INT row, FLOAT z)
{
    if(row<nRows && col<nCols) disp_array[row*nCols+col].z=z;
}

VOID hgeDistortionMesh::SetColor(INT col, INT row, DWORD color)
{
    if(row<nRows && col<nCols) disp_array[row*nCols+col].col=color;
}

VOID hgeDistortionMesh::SetDisplacement(INT col, INT row, FLOAT dx, FLOAT dy, INT ref)
{
    if(row<nRows && col<nCols)
    {
        switch(ref)
        {
            case HGEDISP_NODE:        dx+=col*cellw; dy+=row*cellh; break;
            case HGEDISP_CENTER:    dx+=cellw*(nCols-1)/2;dy+=cellh*(nRows-1)/2; break;
            case HGEDISP_TOPLEFT:    break;
        }

        disp_array[row*nCols+col].x=dx;
        disp_array[row*nCols+col].y=dy;
    }
}

FLOAT hgeDistortionMesh::GetZ(INT col, INT row) CONST
{
    if(row<nRows && col<nCols) return disp_array[row*nCols+col].z;
    else return 0.0f;
}

DWORD hgeDistortionMesh::GetColor(INT col, INT row) CONST
{
    if(row<nRows && col<nCols) return disp_array[row*nCols+col].col;
    else return 0;
}

VOID hgeDistortionMesh::GetDisplacement(INT col, INT row, FLOAT *dx, FLOAT *dy, INT ref) CONST
{
    if(row<nRows && col<nCols)
    {
        switch(ref)
        {
            case HGEDISP_NODE:        *dx=disp_array[row*nCols+col].x-col*cellw;
                                    *dy=disp_array[row*nCols+col].y-row*cellh;
                                    break;

            case HGEDISP_CENTER:    *dx=disp_array[row*nCols+col].x-cellw*(nCols-1)/2;
                                    *dy=disp_array[row*nCols+col].x-cellh*(nRows-1)/2;
                                    break;

            case HGEDISP_TOPLEFT:    *dx=disp_array[row*nCols+col].x;
                                    *dy=disp_array[row*nCols+col].y;
                                    break;
        }
    }
}

