#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeResourceManager helper class implementation
*/


#include "hgeresource.h"
#include "parser.h"
#include "resources.h"


HGE *hgeResourceManager::hge=0;


hgeResourceManager::hgeResourceManager(CONST TCHAR *scriptname)
{
    hge=hgeCreate(HGE_VERSION);

    for(INT i=0;i<RESTYPES;i++) res[i]=0;
    _parse_script(scriptname);
}

hgeResourceManager::~hgeResourceManager()
{
    _remove_all();
    hge->Release();
}

VOID hgeResourceManager::_parse_script(CONST TCHAR *scriptname)
{
    ResDesc *rc, *rcnext;

    if(scriptname)
    {
        RScript::Parse(this, NULL, scriptname, NULL);
        
        rc=res[RES_SCRIPT];
        while(rc)
        {
            rc->Free();
            rcnext=rc->next;
            delete rc;
            rc=rcnext;
        }
        res[RES_SCRIPT]=0;
    }
}

VOID hgeResourceManager::_remove_all()
{
    INT i;
    ResDesc *rc, *rcnext;

    for(i=0;i<RESTYPES;i++)
    {
        rc=res[i];
        while(rc)
        {
            rc->Free();
            rcnext=rc->next;
            delete rc;
            rc=rcnext;
        }
        res[i]=0;
    }
}

VOID hgeResourceManager::ChangeScript(CONST TCHAR *scriptname)
{
    _remove_all();
    _parse_script(scriptname);
}

BOOL hgeResourceManager::Precache(INT groupid)
{
    INT i;
    ResDesc *rc;
    BOOL bResult=TRUE;

    for(i=0;i<RESTYPES;i++)
    {
        rc=res[i];
        while(rc)
        {
            if(!groupid || groupid==rc->resgroup) bResult=bResult && (rc->Get(this)!=0);
            rc=rc->next;
        }
    }

    return bResult;
}

VOID hgeResourceManager::Purge(INT groupid)
{
    INT i;
    ResDesc *rc;

    for(i=0;i<RESTYPES;i++)
    {
        rc=res[i];
        while(rc)
        {
            if(!groupid || groupid==rc->resgroup) rc->Free();
            rc=rc->next;
        }
    }
}

VOID* hgeResourceManager::GetResource(CONST TCHAR *name, INT resgroup)
{
    VOID *reshandle;
    RResource *resource;
    ResDesc *Res=FindRes(this, RES_RESOURCE, name);

    if(Res) return (VOID *)Res->Get(this);
    else
    {
        reshandle=hge->Resource_Load(name);
        if(reshandle)
        {
            resource=new RResource();
            resource->handle=(DWORD)reshandle;
            resource->resgroup=resgroup;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_RESOURCE, resource);
            
            return reshandle;
        }
    }

    return 0;
}

HTEXTURE hgeResourceManager::GetTexture(CONST TCHAR *name, INT resgroup)
{
    HTEXTURE reshandle;
    RTexture *resource;
    ResDesc *Res=FindRes(this, RES_TEXTURE, name);
    if(Res) return (HTEXTURE)Res->Get(this);
    else
    {
        reshandle=hge->Texture_Load(name);
        if(reshandle)
        {
            resource=new RTexture();
            resource->handle=reshandle;
            resource->resgroup=resgroup;
            resource->mipmap=FALSE;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_TEXTURE, resource);

            return reshandle;
        }
    }

    return 0;
}

HEFFECT hgeResourceManager::GetEffect(CONST TCHAR *name, INT resgroup)
{
    HEFFECT reshandle;
    REffect *resource;
    ResDesc *Res=FindRes(this, RES_EFFECT, name);
    if(Res) return (HEFFECT)Res->Get(this);
    else
    {
        reshandle=hge->Effect_Load(name);
        if(reshandle)
        {
            resource=new REffect();
            resource->handle=reshandle;
            resource->resgroup=resgroup;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_EFFECT, resource);

            return reshandle;
        }
    }

    return 0;
}

HMUSIC hgeResourceManager::GetMusic(CONST TCHAR *name, INT resgroup)
{
    HMUSIC reshandle;
    RMusic *resource;
    ResDesc *Res=FindRes(this, RES_MUSIC, name);
    if(Res) return (HMUSIC)Res->Get(this);
    else
    {
        reshandle=hge->Music_Load(name);
        if(reshandle)
        {
            resource=new RMusic();
            resource->handle=reshandle;
            resource->resgroup=resgroup;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_MUSIC, resource);

            return reshandle;
        }
    }

    return 0;
}

HSTREAM hgeResourceManager::GetStream(CONST TCHAR *name, INT resgroup)
{
    HSTREAM reshandle;
    RStream *resource;
    ResDesc *Res=FindRes(this, RES_STREAM, name);
    if(Res) return (HSTREAM)Res->Get(this);
    else
    {
        reshandle=hge->Stream_Load(name);
        if(reshandle)
        {
            resource=new RStream();
            resource->handle=reshandle;
            resource->resgroup=resgroup;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_STREAM, resource);

            return reshandle;
        }
    }

    return 0;
}

HTARGET hgeResourceManager::GetTarget(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_TARGET, name);
    if(Res) return (HTARGET)Res->Get(this);
    else return 0;
}

hgeSprite* hgeResourceManager::GetSprite(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_SPRITE, name);
    if(Res) return (hgeSprite *)Res->Get(this);
    else return 0;
}

hgeAnimation* hgeResourceManager::GetAnimation(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_ANIMATION, name);
    if(Res) return (hgeAnimation *)Res->Get(this);
    else return 0;
}

hgeFont* hgeResourceManager::GetFont(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_FONT, name);
    if(Res) return (hgeFont *)Res->Get(this);
    else return 0;
}

hgeParticleSystem* hgeResourceManager::GetParticleSystem(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_PARTICLE, name);
    if(Res) return (hgeParticleSystem *)Res->Get(this);
    else return 0;
}

hgeDistortionMesh* hgeResourceManager::GetDistortionMesh(CONST TCHAR *name)
{
    ResDesc *Res=FindRes(this, RES_DISTORT, name);
    if(Res) return (hgeDistortionMesh *)Res->Get(this);
    else return 0;
}

hgeStringTable* hgeResourceManager::GetStringTable(CONST TCHAR *name, INT resgroup)
{
    hgeStringTable *strtable;
    RStringTable *resource;
    ResDesc *Res=FindRes(this, RES_STRTABLE, name);
    if(Res) return (hgeStringTable*)Res->Get(this);
    else
    {
        strtable=new hgeStringTable(name);
        if(strtable)
        {
            resource=new RStringTable();
            resource->handle=(DWORD)strtable;
            resource->resgroup=resgroup;
            _tcscpy(resource->name, name);
            _tcscpy(resource->filename, name);
            AddRes(this, RES_STRTABLE, resource);

            return strtable;
        }
    }

    return 0;
}
