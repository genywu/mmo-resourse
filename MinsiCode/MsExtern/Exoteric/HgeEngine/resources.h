/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager resources header
*/

#ifndef HGERESOURCES_H
#define HGERESOURCES_H


#include "hgeresource.h"
#include "parser.h"


#define RES_SCRIPT        0

#define RES_RESOURCE    1
#define RES_TEXTURE        2
#define RES_EFFECT        3
#define RES_MUSIC        4
#define RES_STREAM        5
#define RES_TARGET        6
#define RES_SPRITE        7
#define RES_ANIMATION    8
#define RES_FONT        9
#define RES_PARTICLE    10
#define RES_DISTORT        11
#define RES_STRTABLE    12


VOID        AddRes(hgeResourceManager *rm, INT type, ResDesc *resource);
ResDesc*    FindRes(hgeResourceManager *rm, INT type, CONST TCHAR *name);


struct RScript : public ResDesc
{
    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm) {return 0;}
    virtual VOID  Free() {}
};

struct RResource : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RTexture : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];
    BOOL          mipmap;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct REffect : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RMusic : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];
    INT              amplify;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RStream : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RTarget : public ResDesc
{
    INT            width;
    INT            height;
    BOOL        zbuffer;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RSprite : public ResDesc
{
    TCHAR        texname[MAXRESCHARS];
    FLOAT        tx, ty, w, h;
    FLOAT        hotx, hoty;
    INT            blend;
    DWORD        color;
    FLOAT        z;
    BOOL        bXFlip, bYFlip;
//    FLOAT        x,y;
//    FLOAT        scale;
//    FLOAT        rotation;
//    INT            collision;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RAnimation : public RSprite
{
    INT            frames;
    FLOAT        fps;
    INT            mode;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RFont : public ResDesc
{
    TCHAR        filename[MAXRESCHARS];
    BOOL        mipmap;
    INT            blend;
    DWORD        color;
    FLOAT        z;
    FLOAT        scale;
    FLOAT        proportion;
    FLOAT        tracking;
    FLOAT        spacing;
    FLOAT        rotation;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RParticle : public ResDesc
{
    TCHAR        filename[MAXRESCHARS];
    TCHAR        spritename[MAXRESCHARS];

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

struct RDistort : public ResDesc
{
    TCHAR        texname[MAXRESCHARS];
    FLOAT        tx, ty, w, h;
    INT            cols, rows;
    INT            blend;
    DWORD        color;
    FLOAT        z;

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};


struct RStringTable : public ResDesc
{
    TCHAR          filename[MAXRESCHARS];

    static  VOID  Parse(hgeResourceManager *rm, RScriptParser *sp, CONST TCHAR *name, CONST TCHAR *basename);
    virtual DWORD Get(hgeResourceManager *rm);
    virtual VOID  Free();
};

#endif
