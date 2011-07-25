#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeAnimation helper class implementation
*/


#include "hgeanim.h"


hgeAnimation::hgeAnimation(HTEXTURE tex, INT nframes, FLOAT FPS, FLOAT x, FLOAT y, FLOAT w, FLOAT h)
    : hgeSprite(tex, x, y, w, h)
{
    orig_width = hge->Texture_GetWidth(tex, TRUE);

    fSinceLastFrame=-1.0f;
    fSpeed=1.0f/FPS;
    bPlaying=FALSE;
    nFrames=nframes;

    Mode=HGEANIM_FWD | HGEANIM_LOOP;
    nDelta=1;
    SetFrame(0);
}

hgeAnimation::hgeAnimation(CONST hgeAnimation & anim)
: hgeSprite(anim)
{ 
    // Copy hgeAnimation parameters: 
    this->orig_width      = anim.orig_width;
    this->bPlaying        = anim.bPlaying; 
    this->fSpeed          = anim.fSpeed; 
    this->fSinceLastFrame = anim.fSinceLastFrame;
    this->Mode            = anim.Mode;
    this->nDelta          = anim.nDelta;
    this->nFrames         = anim.nFrames;
    this->nCurFrame       = anim.nCurFrame;
}

VOID hgeAnimation::SetMode(INT mode)
{
    Mode=mode;

    if(mode & HGEANIM_REV)
    {
        nDelta = -1;
        SetFrame(nFrames-1);
    }
    else
    {
        nDelta = 1;
        SetFrame(0);
    }
}


VOID hgeAnimation::Play()
{
    bPlaying=TRUE;
    fSinceLastFrame=-1.0f;
    if(Mode & HGEANIM_REV)
    {
        nDelta = -1;
        SetFrame(nFrames-1);
    }
    else
    {
        nDelta = 1;
        SetFrame(0);
    }
}


VOID hgeAnimation::Update(FLOAT fDeltaTime)
{
    if(!bPlaying) return;

    if(fSinceLastFrame == -1.0f)
        fSinceLastFrame=0.0f;
    else
        fSinceLastFrame += fDeltaTime;

    while(fSinceLastFrame >= fSpeed)
    {
        fSinceLastFrame -= fSpeed;

        if(nCurFrame + nDelta == nFrames)
        {
            switch(Mode)
            {
                case HGEANIM_FWD:
                case HGEANIM_REV | HGEANIM_PINGPONG:
                    bPlaying = FALSE;
                    break;

                case HGEANIM_FWD | HGEANIM_PINGPONG:
                case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
                case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
                    nDelta = -nDelta;
                    break;
            }
        }
        else if(nCurFrame + nDelta < 0)
        {
            switch(Mode)
            {
                case HGEANIM_REV:
                case HGEANIM_FWD | HGEANIM_PINGPONG:
                    bPlaying = FALSE;
                    break;

                case HGEANIM_REV | HGEANIM_PINGPONG:
                case HGEANIM_REV | HGEANIM_PINGPONG | HGEANIM_LOOP:
                case HGEANIM_FWD | HGEANIM_PINGPONG | HGEANIM_LOOP:
                    nDelta = -nDelta;
                    break;
            }
        }

        if(bPlaying) SetFrame(nCurFrame+nDelta);
    }
}

VOID hgeAnimation::SetFrame(INT n)
{
    FLOAT tx1, ty1, tx2, ty2;
    BOOL bX, bY, bHS;
    INT ncols = INT(orig_width) / INT(width);


    n = n % nFrames;
    if(n < 0) n = nFrames + n;
    nCurFrame = n;

    // calculate texture coords for frame n
    ty1 = ty;
    tx1 = tx + n*width;

    if(tx1 > orig_width-width)
    {
        n -= INT(orig_width-tx) / INT(width);
        tx1 = width * (n%ncols);
        ty1 += height * (1 + n/ncols);
    }

    tx2 = tx1 + width;
    ty2 = ty1 + height;

    tx1 /= tex_width;
    ty1 /= tex_height;
    tx2 /= tex_width;
    ty2 /= tex_height;

    quad.v[0].tx=tx1; quad.v[0].ty=ty1;
    quad.v[1].tx=tx2; quad.v[1].ty=ty1;
    quad.v[2].tx=tx2; quad.v[2].ty=ty2;
    quad.v[3].tx=tx1; quad.v[3].ty=ty2;

    bX=bXFlip; bY=bYFlip; bHS=bHSFlip;
    bXFlip=FALSE; bYFlip=FALSE;
    SetFlip(bX,bY,bHS);
}

