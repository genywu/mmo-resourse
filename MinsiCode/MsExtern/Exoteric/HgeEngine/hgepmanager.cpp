#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleManager helper class implementation
*/


#include "hgeparticle.h"


hgeParticleManager::hgeParticleManager()
{
    nPS=0;
    tX=tY=0.0f;
}

hgeParticleManager::~hgeParticleManager()
{
    INT i;
    for(i=0;i<nPS;i++) delete psList[i];
}

VOID hgeParticleManager::Update(FLOAT dt)
{
    INT i;
    for(i=0;i<nPS;i++)
    {
        psList[i]->Update(dt);
        if(psList[i]->GetAge()==-2.0f && psList[i]->GetParticlesAlive()==0)
        {
            delete psList[i];
            psList[i]=psList[nPS-1];
            nPS--;
            i--;
        }
    }
}

VOID hgeParticleManager::Render()
{
    INT i;
    for(i=0;i<nPS;i++) psList[i]->Render();
}

hgeParticleSystem* hgeParticleManager::SpawnPS(hgeParticleSystemInfo *psi, FLOAT x, FLOAT y)
{
    if(nPS==MAX_PSYSTEMS) return 0;
    psList[nPS]=new hgeParticleSystem(psi);
    psList[nPS]->FireAt(x,y);
    psList[nPS]->Transpose(tX,tY);
    nPS++;
    return psList[nPS-1];
}

BOOL hgeParticleManager::IsPSAlive(hgeParticleSystem *ps) CONST
{
    INT i;
    for(i=0;i<nPS;i++) if(psList[i]==ps) return TRUE;
    return FALSE;
}

VOID hgeParticleManager::Transpose(FLOAT x, FLOAT y)
{
    INT i;
    for(i=0;i<nPS;i++) psList[i]->Transpose(x,y);
    tX=x; tY=y;
}

VOID hgeParticleManager::KillPS(hgeParticleSystem *ps)
{
    INT i;
    for(i=0;i<nPS;i++)
    {
        if(psList[i]==ps)
        {
            delete psList[i];
            psList[i]=psList[nPS-1];
            nPS--;
            return;
        }
    }
}

VOID hgeParticleManager::KillAll()
{
    INT i;
    for(i=0;i<nPS;i++) delete psList[i];
    nPS=0;
}
