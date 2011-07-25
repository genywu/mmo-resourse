/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeColor*** helper classes
*/


#ifndef HGECOLOR_H
#define HGECOLOR_H


#include "hge.h"


#define hgeColor hgeColorRGB

inline VOID ColorClamp(FLOAT &x) { if(x<0.0f) x=0.0f; if(x>1.0f) x=1.0f; }


class hgeColorRGB
{
public:
    FLOAT        r,g,b,a;

    hgeColorRGB(FLOAT _r, FLOAT _g, FLOAT _b, FLOAT _a) { r=_r; g=_g; b=_b; a=_a; }
    hgeColorRGB(DWORD col) { SetHWColor(col); }
    hgeColorRGB() { r=g=b=a=0; }

    hgeColorRGB        operator-  (CONST hgeColorRGB &c) CONST { return hgeColorRGB(r-c.r, g-c.g, b-c.b, a-c.a); }
    hgeColorRGB        operator+  (CONST hgeColorRGB &c) CONST { return hgeColorRGB(r+c.r, g+c.g, b+c.b, a+c.a); }
    hgeColorRGB        operator*  (CONST hgeColorRGB &c) CONST { return hgeColorRGB(r*c.r, g*c.g, b*c.b, a*c.a); }
    hgeColorRGB&    operator-= (CONST hgeColorRGB &c)        { r-=c.r; g-=c.g; b-=c.b; a-=c.a; return *this;   }
    hgeColorRGB&    operator+= (CONST hgeColorRGB &c)        { r+=c.r; g+=c.g; b+=c.b; a+=c.a; return *this;   }
    BOOL            operator== (CONST hgeColorRGB &c) CONST { return (r==c.r && g==c.g && b==c.b && a==c.a);  }
    BOOL            operator!= (CONST hgeColorRGB &c) CONST { return (r!=c.r || g!=c.g || b!=c.b || a!=c.a);  }

    hgeColorRGB        operator/  (CONST FLOAT scalar) CONST { return hgeColorRGB(r/scalar, g/scalar, b/scalar, a/scalar); }
    hgeColorRGB        operator*  (CONST FLOAT scalar) CONST { return hgeColorRGB(r*scalar, g*scalar, b*scalar, a*scalar); }
    hgeColorRGB&    operator*= (CONST FLOAT scalar)          { r*=scalar; g*=scalar; b*=scalar; a*=scalar; return *this;   }

    VOID            Clamp() { ColorClamp(r); ColorClamp(g); ColorClamp(b); ColorClamp(a); }
    VOID            SetHWColor(DWORD col) {    a = (col>>24)/255.0f; r = ((col>>16) & 0xFF)/255.0f; g = ((col>>8) & 0xFF)/255.0f; b = (col & 0xFF)/255.0f;    }
    DWORD            GetHWColor() CONST { return (DWORD(a*255.0f)<<24) + (DWORD(r*255.0f)<<16) + (DWORD(g*255.0f)<<8) + DWORD(b*255.0f);    }
};

inline hgeColorRGB operator* (CONST FLOAT sc, CONST hgeColorRGB &c) { return c*sc; }


class hgeColorHSV
{
public:
    FLOAT        h,s,v,a;

    hgeColorHSV(FLOAT _h, FLOAT _s, FLOAT _v, FLOAT _a) { h=_h; s=_s; v=_v; a=_a; }
    hgeColorHSV(DWORD col) { SetHWColor(col); }
    hgeColorHSV() { h=s=v=a=0; }

    hgeColorHSV     operator-  (CONST hgeColorHSV &c) CONST { return hgeColorHSV(h-c.h, s-c.s, v-c.v, a-c.a); }
    hgeColorHSV     operator+  (CONST hgeColorHSV &c) CONST { return hgeColorHSV(h+c.h, s+c.s, v+c.v, a+c.a); }
    hgeColorHSV     operator*  (CONST hgeColorHSV &c) CONST { return hgeColorHSV(h*c.h, s*c.s, v*c.v, a*c.a); }
    hgeColorHSV&    operator-= (CONST hgeColorHSV &c)        { h-=c.h; s-=c.s; v-=c.v; a-=c.a; return *this;   }
    hgeColorHSV&    operator+= (CONST hgeColorHSV &c)        { h+=c.h; s+=c.s; v+=c.v; a+=c.a; return *this;   }
    BOOL            operator== (CONST hgeColorHSV &c) CONST { return (h==c.h && s==c.s && v==c.v && a==c.a);  }
    BOOL            operator!= (CONST hgeColorHSV &c) CONST { return (h!=c.h || s!=c.s || v!=c.v || a!=c.a);  }

    hgeColorHSV     operator/  (CONST FLOAT scalar) CONST { return hgeColorHSV(h/scalar, s/scalar, v/scalar, a/scalar); }
    hgeColorHSV     operator*  (CONST FLOAT scalar) CONST { return hgeColorHSV(h*scalar, s*scalar, v*scalar, a*scalar); }
    hgeColorHSV&    operator*= (CONST FLOAT scalar)          { h*=scalar; s*=scalar; v*=scalar; a*=scalar; return *this;   }

    VOID    Clamp() { ColorClamp(h); ColorClamp(s); ColorClamp(v); ColorClamp(a); }
    VOID    SetHWColor(DWORD col);
    DWORD   GetHWColor() CONST;
};

inline hgeColorHSV operator* (CONST FLOAT sc, CONST hgeColorHSV &c) { return c*sc; }


#endif
