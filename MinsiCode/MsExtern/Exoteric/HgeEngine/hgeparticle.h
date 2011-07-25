/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeParticleSystem helper class header
*/


#ifndef HGEPARTICLE_H
#define HGEPARTICLE_H


#include "hge.h"
#include "hgesprite.h"
#include "hgevector.h"
#include "hgecolor.h"
#include "hgerect.h"


#define MAX_PARTICLES    500
#define MAX_PSYSTEMS    100

struct hgeParticle
{
    hgeVector    vecLocation;
    hgeVector    vecVelocity;

    FLOAT        fGravity;
    FLOAT        fRadialAccel;
    FLOAT        fTangentialAccel;

    FLOAT        fSpin;
    FLOAT        fSpinDelta;

    FLOAT        fSize;
    FLOAT        fSizeDelta;

    hgeColor    colColor;        // + alpha
    hgeColor    colColorDelta;

    FLOAT        fAge;
    FLOAT        fTerminalAge;
};

struct hgeParticleSystemInfo
{
    hgeSprite*    sprite;    // texture + blend mode
    INT            nEmission; // particles per sec
    FLOAT        fLifetime;

    FLOAT        fParticleLifeMin;
    FLOAT        fParticleLifeMax;

    FLOAT        fDirection;
    FLOAT        fSpread;
    BOOL        bRelative;

    FLOAT        fSpeedMin;
    FLOAT        fSpeedMax;

    FLOAT        fGravityMin;
    FLOAT        fGravityMax;

    FLOAT        fRadialAccelMin;
    FLOAT        fRadialAccelMax;

    FLOAT        fTangentialAccelMin;
    FLOAT        fTangentialAccelMax;

    FLOAT        fSizeStart;
    FLOAT        fSizeEnd;
    FLOAT        fSizeVar;

    FLOAT        fSpinStart;
    FLOAT        fSpinEnd;
    FLOAT        fSpinVar;

    hgeColor    colColorStart; // + alpha
    hgeColor    colColorEnd;
    FLOAT        fColorVar;
    FLOAT        fAlphaVar;
};

class hgeParticleSystem
{
public:
    hgeParticleSystemInfo info;
    
    hgeParticleSystem(CONST TCHAR *filename, hgeSprite *sprite);
    hgeParticleSystem(hgeParticleSystemInfo *psi);
    hgeParticleSystem(CONST hgeParticleSystem &ps);
    ~hgeParticleSystem() { hge->Release(); }

    hgeParticleSystem&    operator= (CONST hgeParticleSystem &ps);


    VOID                Render();
    VOID                FireAt(FLOAT x, FLOAT y);
    VOID                Fire();
    VOID                Stop(BOOL bKillParticles=FALSE);
    VOID                Update(FLOAT fDeltaTime);
    VOID                MoveTo(FLOAT x, FLOAT y, BOOL bMoveParticles=FALSE);
    VOID                Transpose(FLOAT x, FLOAT y) { fTx=x; fTy=y; }
    VOID                SetScale(FLOAT scale) { fScale = scale; }
    VOID                TrackBoundingBox(BOOL bTrack) { bUpdateBoundingBox=bTrack; }

    INT                    GetParticlesAlive() CONST { return nParticlesAlive; }
    FLOAT                GetAge() CONST { return fAge; }
    VOID                GetPosition(FLOAT *x, FLOAT *y) CONST { *x=vecLocation.x; *y=vecLocation.y; }
    VOID                GetTransposition(FLOAT *x, FLOAT *y) CONST { *x=fTx; *y=fTy; }
    FLOAT                GetScale() { return fScale; }
    hgeRect*            GetBoundingBox(hgeRect *rect) CONST;

private:
    hgeParticleSystem();

    static HGE            *hge;

    FLOAT                fAge;
    FLOAT                fEmissionResidue;

    hgeVector            vecPrevLocation;
    hgeVector            vecLocation;
    FLOAT                fTx, fTy;
    FLOAT                fScale;

    INT                    nParticlesAlive;
    hgeRect                rectBoundingBox;
    BOOL                bUpdateBoundingBox;

    hgeParticle            particles[MAX_PARTICLES];
};

class hgeParticleManager
{
public:
    hgeParticleManager();
    ~hgeParticleManager();

    VOID                Update(FLOAT dt);
    VOID                Render();

    hgeParticleSystem*    SpawnPS(hgeParticleSystemInfo *psi, FLOAT x, FLOAT y);
    BOOL                IsPSAlive(hgeParticleSystem *ps) CONST;
    VOID                Transpose(FLOAT x, FLOAT y);
    VOID                GetTransposition(FLOAT *dx, FLOAT *dy) CONST {*dx=tX; *dy=tY;}
    VOID                KillPS(hgeParticleSystem *ps);
    VOID                KillAll();

private:
    hgeParticleManager(CONST hgeParticleManager &);
    hgeParticleManager&    operator= (CONST hgeParticleManager &);

    INT                    nPS;
    FLOAT                tX;
    FLOAT                tY;
    hgeParticleSystem*    psList[MAX_PSYSTEMS];
};


#endif
