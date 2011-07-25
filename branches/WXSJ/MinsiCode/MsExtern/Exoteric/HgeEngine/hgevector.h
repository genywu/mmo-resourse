/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeVector helper class
*/


#ifndef HGEVECTOR_H
#define HGEVECTOR_H


#include "hge.h"
#include <math.h>


/*
** Fast 1.0/sqrtf(FLOAT) routine
*/
FLOAT InvSqrt(FLOAT x);

class hgeVector
{
public:
    FLOAT    x,y;

    hgeVector(FLOAT _x, FLOAT _y)    { x=_x; y=_y; }
    hgeVector()                        { x=0; y=0; }

    hgeVector   operator-  ()                    CONST { return hgeVector(-x, -y);        }
    hgeVector   operator-  (CONST hgeVector &v) CONST { return hgeVector(x-v.x, y-v.y); }
    hgeVector   operator+  (CONST hgeVector &v) CONST { return hgeVector(x+v.x, y+v.y); }
    hgeVector&  operator-= (CONST hgeVector &v)          { x-=v.x; y-=v.y; return *this;    }
    hgeVector&  operator+= (CONST hgeVector &v)          { x+=v.x; y+=v.y; return *this;    }
    BOOL        operator== (CONST hgeVector &v)    CONST { return (x==v.x && y==v.y);        }
    BOOL        operator!= (CONST hgeVector &v)    CONST { return (x!=v.x || y!=v.y);        }

    hgeVector   operator/  (CONST FLOAT scalar)    CONST { return hgeVector(x/scalar, y/scalar); }
    hgeVector   operator*  (CONST FLOAT scalar) CONST { return hgeVector(x*scalar, y*scalar); }
    hgeVector&  operator*= (CONST FLOAT scalar)          { x*=scalar; y*=scalar; return *this;   }

    FLOAT       Dot(CONST hgeVector *v) CONST { return x*v->x + y*v->y; }
    FLOAT       Length() CONST { return sqrtf(Dot(this)); }
    FLOAT       Angle(CONST hgeVector *v = 0) CONST;
    
    VOID        Clamp(CONST FLOAT max) { if(Length() > max)    { Normalize(); x *= max; y *= max; } }
    hgeVector*  Normalize() { FLOAT rc=InvSqrt(Dot(this)); x*=rc; y*=rc; return this; }
    hgeVector*  Rotate(FLOAT a);
};

inline hgeVector operator* (CONST FLOAT s, CONST hgeVector &v)        { return v*s; }
inline FLOAT     operator^ (CONST hgeVector &v, CONST hgeVector &u) { return v.Angle(&u); }
inline FLOAT     operator% (CONST hgeVector &v, CONST hgeVector &u) { return v.Dot(&u); }


#endif
