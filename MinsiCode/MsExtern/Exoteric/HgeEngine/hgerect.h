/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeRect helper class
*/


#ifndef HGERECT_H
#define HGERECT_H


class hgeRect
{
public:
    FLOAT    x1, y1, x2, y2;

    hgeRect(FLOAT _x1, FLOAT _y1, FLOAT _x2, FLOAT _y2) {x1=_x1; y1=_y1; x2=_x2; y2=_y2; bClean=FALSE; }
    hgeRect() {bClean=TRUE;}

    VOID    Clear() {bClean=TRUE;}
    BOOL    IsClean() CONST {return bClean;}
    VOID    Set(FLOAT _x1, FLOAT _y1, FLOAT _x2, FLOAT _y2) { x1=_x1; x2=_x2; y1=_y1; y2=_y2; bClean=FALSE; }
    VOID    SetRadius(FLOAT x, FLOAT y, FLOAT r) { x1=x-r; x2=x+r; y1=y-r; y2=y+r; bClean=FALSE; }
    VOID    Encapsulate(FLOAT x, FLOAT y);
    BOOL    TestPoint(FLOAT x, FLOAT y) CONST;
    BOOL    Intersect(CONST hgeRect *rect) CONST;

private:
    BOOL    bClean;
};


#endif
