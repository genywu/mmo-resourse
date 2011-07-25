#include "Precompiled.h"
/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: input
*/


#include "hge_impl.h"


TCHAR *KeyNames[] =
{
    _T("?"),
    _T("Left Mouse Button"), _T("Right Mouse Button"), _T("?"), _T("Middle Mouse Button"),
    _T("?"), _T("?"), _T("?"), _T("Backspace"), _T("Tab"), _T("?"), _T("?"), _T("?"), _T("Enter"), _T("?"), _T("?"),
    _T("Shift"), _T("Ctrl"), _T("Alt"), _T("Pause"), _T("Caps Lock"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Escape"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Space"), _T("Page Up"), _T("Page Down"), _T("End"), _T("Home"),
    _T("Left Arrow"), _T("Up Arrow"), _T("Right Arrow"), _T("Down Arrow"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("Insert"), _T("Delete"), _T("?"),
    _T("0"), _T("1"), _T("2"), _T("3"), _T("4"), _T("5"), _T("6"), _T("7"), _T("8"), _T("9"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("A"), _T("B"), _T("C"), _T("D"), _T("E"), _T("F"), _T("G"), _T("H"), _T("I"), _T("J"), _T("K"), _T("L"), _T("M"),
    _T("N"), _T("O"), _T("P"), _T("Q"), _T("R"), _T("S"), _T("T"), _T("U"), _T("V"), _T("W"), _T("X"), _T("Y"), _T("Z"),
    _T("Left Win"), _T("Right Win"), _T("Application"), _T("?"), _T("?"),
    _T("NumPad 0"), _T("NumPad 1"), _T("NumPad 2"), _T("NumPad 3"), _T("NumPad 4"),
    _T("NumPad 5"), _T("NumPad 6"), _T("NumPad 7"), _T("NumPad 8"), _T("NumPad 9"),
    _T("Multiply"), _T("Add"), _T("?"), _T("Subtract"), _T("Decimal"), _T("Divide"),
    _T("F1"), _T("F2"), _T("F3"), _T("F4"), _T("F5"), _T("F6"), _T("F7"), _T("F8"), _T("F9"), _T("F10"), _T("F11"), _T("F12"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Num Lock"), _T("Scroll Lock"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Semicolon"), _T("Equals"), _T("Comma"), _T("Minus"), _T("Period"), _T("Slash"), _T("Grave"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("Left bracket"), _T("Backslash"), _T("Right bracket"), _T("Apostrophe"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"), _T("?"),
    _T("?"), _T("?"), _T("?")
};

BOOL CALL HGE_Impl::Input_GetEvent(hgeInputEvent *event)
{
    CInputEventList *eptr;

    if(queue)
    {
        eptr=queue;
        memcpy(event, &eptr->event, sizeof(hgeInputEvent));
        queue=eptr->next;
        delete eptr;
        return TRUE;
    }
    
    return FALSE;
}

VOID CALL HGE_Impl::Input_GetMousePos(FLOAT *x, FLOAT *y)
{
    *x=Xpos; *y=Ypos;
}


VOID CALL HGE_Impl::Input_SetMousePos(FLOAT x, FLOAT y)
{
    POINT pt;
    pt.x=(long)x; pt.y=(long)y;
    ClientToScreen(hwnd, &pt);
    SetCursorPos(pt.x,pt.y);
}

INT CALL HGE_Impl::Input_GetMouseWheel()
{
    return Zpos;
}

BOOL CALL HGE_Impl::Input_IsMouseOver()
{
    return bMouseOver;
}

BOOL CALL HGE_Impl::Input_GetKeyState(INT key)
{
    return ((GetKeyState(key) & 0x8000) != 0);
}

BOOL CALL HGE_Impl::Input_KeyDown(INT key)
{
    return (keyz[key] & 1) != 0;
}

BOOL CALL HGE_Impl::Input_KeyUp(INT key)
{
    return (keyz[key] & 2) != 0;
}

TCHAR* CALL HGE_Impl::Input_GetKeyName(INT key)
{
    return KeyNames[key];
}

INT CALL HGE_Impl::Input_GetKey()
{
    return VKey;
}

INT CALL HGE_Impl::Input_GetChar()
{
    return Char;
}


//////// Implementation ////////


VOID HGE_Impl::_InputInit()
{
    POINT    pt;
    GetCursorPos(&pt);
    ScreenToClient(hwnd, &pt);
    Xpos = (FLOAT)pt.x;
    Ypos = (FLOAT)pt.y;

    memset(&keyz, 0, sizeof(keyz));
}

VOID HGE_Impl::_UpdateMouse()
{
    POINT    pt;
    RECT    rc;

    GetCursorPos(&pt);
    GetClientRect(hwnd, &rc);
    MapWindowPoints(hwnd, NULL, (LPPOINT)&rc, 2);

    if(bCaptured || (PtInRect(&rc, pt) && WindowFromPoint(pt)==hwnd))
        bMouseOver=TRUE;
    else
        bMouseOver=FALSE;
}

VOID HGE_Impl::_BuildEvent(INT type, INT key, INT scan, INT flags, INT x, INT y)
{
    CInputEventList *last, *eptr=new CInputEventList;
    BYTE kbstate[256];
    POINT pt;

    eptr->event.type=type;
    eptr->event.chr=0;
    pt.x=x; pt.y=y;

    GetKeyboardState(kbstate);
    if(type==INPUT_KEYDOWN)
    {
        if((flags & HGEINP_REPEAT) == 0) keyz[key] |= 1;
        ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
    }
    if(type==INPUT_KEYUP)
    {
        keyz[key] |= 2;
        ToAscii(key, scan, kbstate, (unsigned short *)&eptr->event.chr, 0);
    }
    if(type==INPUT_MOUSEWHEEL)
    {
        eptr->event.key=0; eptr->event.wheel=key;
        ScreenToClient(hwnd,&pt);
    }
    else { eptr->event.key=key; eptr->event.wheel=0; }

    if(type==INPUT_MBUTTONDOWN)
    {
        keyz[key] |= 1;
        SetCapture(hwnd);
        bCaptured=TRUE;
    }
    if(type==INPUT_MBUTTONUP)
    {
        keyz[key] |= 2;
        ReleaseCapture();
        Input_SetMousePos(Xpos, Ypos);
        pt.x=(INT)Xpos; pt.y=(INT)Ypos;
        bCaptured=FALSE;
    }
    
    if(kbstate[VK_SHIFT] & 0x80) flags|=HGEINP_SHIFT;
    if(kbstate[VK_CONTROL] & 0x80) flags|=HGEINP_CTRL;
    if(kbstate[VK_MENU] & 0x80) flags|=HGEINP_ALT;
    if(kbstate[VK_CAPITAL] & 0x1) flags|=HGEINP_CAPSLOCK;
    if(kbstate[VK_SCROLL] & 0x1) flags|=HGEINP_SCROLLLOCK;
    if(kbstate[VK_NUMLOCK] & 0x1) flags|=HGEINP_NUMLOCK;
    eptr->event.flags=flags;

    if(pt.x==-1) { eptr->event.x=Xpos;eptr->event.y=Ypos; }
    else
    {
        if(pt.x<0) pt.x=0;
        if(pt.y<0) pt.y=0;
        if(pt.x>=nScreenWidth) pt.x=nScreenWidth-1;
        if(pt.y>=nScreenHeight) pt.y=nScreenHeight-1;

        eptr->event.x=(FLOAT)pt.x;
        eptr->event.y=(FLOAT)pt.y;
    }

    eptr->next=0; 

    if(!queue) queue=eptr;
    else
    {
        last=queue;
        while(last->next) last=last->next;
        last->next=eptr;
    }

    if(eptr->event.type==INPUT_KEYDOWN || eptr->event.type==INPUT_MBUTTONDOWN)
    {
        VKey=eptr->event.key;Char=eptr->event.chr;
    }
    else if(eptr->event.type==INPUT_MOUSEMOVE)
    {
        Xpos=eptr->event.x;Ypos=eptr->event.y;
    }
    else if(eptr->event.type==INPUT_MOUSEWHEEL)
    {
        Zpos+=eptr->event.wheel;
    }
}

VOID HGE_Impl::_ClearQueue()
{
    CInputEventList *nexteptr, *eptr=queue;

    memset(&keyz, 0, sizeof(keyz));
    
    while(eptr)
    {
        nexteptr=eptr->next;
        delete eptr;
        eptr=nexteptr;
    }

    queue=0; VKey=0; Char=0; Zpos=0;
}
