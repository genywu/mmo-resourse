/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager helper class header
*/


#ifndef HGERESOURCE_H
#define HGERESOURCE_H


#include "hge.h"
#include "hgesprite.h"
#include "hgeanim.h"
#include "hgefont.h"
#include "hgeparticle.h"
#include "hgedistort.h"
#include "hgestrings.h"


#define RESTYPES 13
#define MAXRESCHARS 128


class hgeResourceManager;

struct ResDesc
{
    TCHAR        name[MAXRESCHARS];
    INT            resgroup;
    DWORD        handle;
    ResDesc*    next;

    ResDesc()    { hge=hgeCreate(HGE_VERSION); }
    ~ResDesc()    { hge->Release(); }

    virtual DWORD Get(hgeResourceManager *rm) = 0;
    virtual VOID  Free() = 0;

protected:
    static HGE    *hge;
};

/*
** HGE Resource manager class
*/
class hgeResourceManager
{
public:
    hgeResourceManager(CONST TCHAR *scriptname=0);
    ~hgeResourceManager();

    VOID                ChangeScript(CONST TCHAR *scriptname=0);
    BOOL                Precache(INT groupid=0);
    VOID                Purge(INT groupid=0);

    VOID*                GetResource(CONST TCHAR *name, INT resgroup=0);
    HTEXTURE            GetTexture(CONST TCHAR *name, INT resgroup=0);
    HEFFECT                GetEffect(CONST TCHAR *name, INT resgroup=0);
    HMUSIC                GetMusic(CONST TCHAR *name, INT resgroup=0);
    HSTREAM                GetStream(CONST TCHAR *name, INT resgroup=0);
    HTARGET                GetTarget(CONST TCHAR *name);

    hgeSprite*            GetSprite(CONST TCHAR *name);
    hgeAnimation*        GetAnimation(CONST TCHAR *name);
    hgeFont*            GetFont(CONST TCHAR *name);
    hgeParticleSystem*    GetParticleSystem(CONST TCHAR *name);
    hgeDistortionMesh*    GetDistortionMesh(CONST TCHAR *name);
    hgeStringTable*        GetStringTable(CONST TCHAR *name, INT resgroup=0);

    ResDesc*            res[RESTYPES];

private:
    hgeResourceManager(CONST hgeResourceManager &);
    hgeResourceManager&    operator= (CONST hgeResourceManager&);
    VOID                _remove_all();
    VOID                _parse_script(CONST TCHAR *scriptname=0);

    static HGE            *hge;
};


#endif
