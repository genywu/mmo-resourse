/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class header
*/


#ifndef HGEANIM_H
#define HGEANIM_H


#include "hgesprite.h"


#define HGEANIM_FWD            0
#define HGEANIM_REV            1
#define HGEANIM_PINGPONG    2
#define HGEANIM_NOPINGPONG    0
#define HGEANIM_LOOP        4
#define HGEANIM_NOLOOP        0


/*
** HGE Animation class
*/
class hgeAnimation : public hgeSprite
{
public:
    hgeAnimation(HTEXTURE tex, INT nframes, FLOAT FPS, FLOAT x, FLOAT y, FLOAT w, FLOAT h);
    hgeAnimation(CONST hgeAnimation &anim);
    
    VOID        Play();
    VOID        Stop() { bPlaying=FALSE; }
    VOID        Resume() { bPlaying=TRUE; }
    VOID        Update(FLOAT fDeltaTime);
    BOOL        IsPlaying() CONST { return bPlaying; }

    VOID        SetTexture(HTEXTURE tex) { hgeSprite::SetTexture(tex); orig_width = hge->Texture_GetWidth(tex, TRUE); }
    VOID        SetTextureRect(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2) { hgeSprite::SetTextureRect(x1,y1,x2,y2); SetFrame(nCurFrame); }
    VOID        SetMode(INT mode);
    VOID        SetSpeed(FLOAT FPS) { fSpeed=1.0f/FPS; }
    VOID        SetFrame(INT n);
    VOID        SetFrames(INT n) { nFrames=n; }

    INT         GetMode() CONST { return Mode; }
    FLOAT       GetSpeed() CONST { return 1.0f/fSpeed; }
    INT         GetFrame() CONST { return nCurFrame; }
    INT         GetFrames() CONST { return nFrames; }

private:
    hgeAnimation();

    INT         orig_width;

    BOOL        bPlaying;

    FLOAT       fSpeed;
    FLOAT       fSinceLastFrame;

    INT         Mode;
    INT         nDelta;
    INT         nFrames;
    INT         nCurFrame;
};


#endif
