#include "Precompiled.h"
/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: timer
*/


#include "hge_impl.h"


FLOAT CALL HGE_Impl::Timer_GetTime()
{
    return fTime;
}

FLOAT CALL HGE_Impl::Timer_GetDelta()
{
    return fDeltaTime;
}


INT CALL HGE_Impl::Timer_GetFPS()
{
    return nFPS;
}

