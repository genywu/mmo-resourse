#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper class implementation
*/


#include "hgegui.h"


HGE *hgeGUI::hge=0;
HGE *hgeGUIObject::hge=0;


hgeGUI::hgeGUI()
{
    hge=hgeCreate(HGE_VERSION);

    ctrls=0;
    ctrlLock=0;
    ctrlFocus=0;
    ctrlOver=0;
    navmode=HGEGUI_NONAVKEYS;
    bLPressed=bLReleased=FALSE;
    bRPressed=bRReleased=FALSE;
    nWheel=0;
    mx=my=0.0f;
    nEnterLeave=0;
    sprCursor=0;
}

hgeGUI::~hgeGUI()
{
    hgeGUIObject *ctrl=ctrls, *nextctrl;

    while(ctrl)
    {
        nextctrl=ctrl->next;
        delete ctrl;
        ctrl=nextctrl;
    }

    hge->Release();
}

VOID hgeGUI::AddCtrl(hgeGUIObject *ctrl)
{
    hgeGUIObject *last=ctrls;

    ctrl->gui=this;

    if(!ctrls)
    {
        ctrls=ctrl;
        ctrl->prev=0;
        ctrl->next=0;
    }
    else
    {
        while(last->next) last=last->next;
        last->next=ctrl;
        ctrl->prev=last;
        ctrl->next=0;
    }
}

VOID hgeGUI::DelCtrl(INT id)
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        if(ctrl->id == id)
        {
            if(ctrl->prev) ctrl->prev->next = ctrl->next;
            else ctrls = ctrl->next;
            if(ctrl->next) ctrl->next->prev = ctrl->prev;
            delete ctrl;
            return;
        }
        ctrl=ctrl->next;
    }
}

hgeGUIObject* hgeGUI::GetCtrl(INT id) CONST
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        if(ctrl->id == id) return ctrl;
        ctrl=ctrl->next;
    }

    return NULL;
}

VOID hgeGUI::MoveCtrl(INT id, FLOAT x, FLOAT y)
{
    hgeGUIObject *ctrl=GetCtrl(id);
    ctrl->rect.x2=x + (ctrl->rect.x2 - ctrl->rect.x1);
    ctrl->rect.y2=y + (ctrl->rect.y2 - ctrl->rect.y1);
    ctrl->rect.x1=x;
    ctrl->rect.y1=y;
}

VOID hgeGUI::ShowCtrl(INT id, BOOL bVisible)
{
    GetCtrl(id)->bVisible=bVisible;
}

VOID hgeGUI::EnableCtrl(INT id, BOOL bEnabled)
{
    GetCtrl(id)->bEnabled=bEnabled;
}

VOID hgeGUI::SetNavMode(INT mode)
{
    navmode=mode;
}

VOID hgeGUI::SetCursor(hgeSprite *spr)
{
    sprCursor=spr;
}


VOID hgeGUI::SetColor(DWORD color)
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        ctrl->SetColor(color);
        ctrl=ctrl->next;
    }
}


VOID hgeGUI::Reset()
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        ctrl->Reset();
        ctrl=ctrl->next;
    }

    ctrlLock=0;
    ctrlOver=0;
    ctrlFocus=0;
}


VOID hgeGUI::Move(FLOAT dx, FLOAT dy)
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        ctrl->rect.x1 += dx;
        ctrl->rect.y1 += dy;
        ctrl->rect.x2 += dx;
        ctrl->rect.y2 += dy;

        ctrl=ctrl->next;
    }
}


VOID hgeGUI::SetFocus(INT id)
{
    hgeGUIObject *ctrlNewFocus=GetCtrl(id);

    if(ctrlNewFocus==ctrlFocus) return;
    if(!ctrlNewFocus)
    {
        if(ctrlFocus) ctrlFocus->Focus(FALSE);
        ctrlFocus=0;
    }
    else if(!ctrlNewFocus->bStatic && ctrlNewFocus->bVisible && ctrlNewFocus->bEnabled)
    {
        if(ctrlFocus) ctrlFocus->Focus(FALSE);
        if(ctrlNewFocus) ctrlNewFocus->Focus(TRUE);
        ctrlFocus=ctrlNewFocus;
    }
}

INT hgeGUI::GetFocus() CONST
{
    if(ctrlFocus) return ctrlFocus->id;
    else return 0;
}

VOID hgeGUI::Enter()
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        ctrl->Enter();
        ctrl=ctrl->next;
    }

    nEnterLeave=2;
}

VOID hgeGUI::Leave()
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        ctrl->Leave();
        ctrl=ctrl->next;
    }

    ctrlFocus=0;
    ctrlOver=0;
    ctrlLock=0;
    nEnterLeave=1;
}

VOID hgeGUI::Render()
{
    hgeGUIObject *ctrl=ctrls;

    while(ctrl)
    {
        if(ctrl->bVisible) ctrl->Render();
        ctrl=ctrl->next;
    }

    if(hge->Input_IsMouseOver() && sprCursor) sprCursor->Render(mx,my);
}

INT hgeGUI::Update(FLOAT dt)
{
    BOOL bDone;
    INT key;
    hgeGUIObject *ctrl;

// Update the mouse variables

    hge->Input_GetMousePos(&mx, &my);
    bLPressed  = hge->Input_KeyDown(HGEK_LBUTTON);
    bLReleased = hge->Input_KeyUp(HGEK_LBUTTON);
    bRPressed  = hge->Input_KeyDown(HGEK_RBUTTON);
    bRReleased = hge->Input_KeyUp(HGEK_RBUTTON);
    nWheel=hge->Input_GetMouseWheel();

// Update all controls

    ctrl=ctrls;
    while(ctrl)
    {
        ctrl->Update(dt);
        ctrl=ctrl->next;
    }

// Handle Enter/Leave

    if(nEnterLeave)
    {
        ctrl=ctrls; bDone=TRUE;
        while(ctrl)
        {
            if(!ctrl->IsDone()) { bDone=FALSE; break; }
            ctrl=ctrl->next;
        }
        if(!bDone) return 0;
        else
        {
            if(nEnterLeave==1) return -1;
            else nEnterLeave=0;
        }
    }

// Handle keys    

    key=hge->Input_GetKey();
    if(((navmode & HGEGUI_LEFTRIGHT) && key==HGEK_LEFT) ||
        ((navmode & HGEGUI_UPDOWN) && key==HGEK_UP))
    {
        ctrl=ctrlFocus;
        if(!ctrl)
        {
            ctrl=ctrls;
            if(!ctrl) return 0;
        }
        do {
            ctrl=ctrl->prev;
            if(!ctrl && ((navmode & HGEGUI_CYCLED) || !ctrlFocus))
            {
                ctrl=ctrls;
                while(ctrl->next) ctrl=ctrl->next;
            }
            if(!ctrl || ctrl==ctrlFocus) break;
        } while(ctrl->bStatic==TRUE || ctrl->bVisible==FALSE || ctrl->bEnabled==FALSE);

        if(ctrl && ctrl!=ctrlFocus)
        {
            if(ctrlFocus) ctrlFocus->Focus(FALSE);
            if(ctrl) ctrl->Focus(TRUE);
            ctrlFocus=ctrl;
        }
    }
    else if(((navmode & HGEGUI_LEFTRIGHT) && key==HGEK_RIGHT) ||
        ((navmode & HGEGUI_UPDOWN) && key==HGEK_DOWN))
    {
        ctrl=ctrlFocus;
        if(!ctrl)
        {
            ctrl=ctrls;
            if(!ctrl) return 0;
            while(ctrl->next) ctrl=ctrl->next;
        }
        do {
            ctrl=ctrl->next;
            if(!ctrl && ((navmode & HGEGUI_CYCLED) || !ctrlFocus)) ctrl=ctrls;
            if(!ctrl || ctrl==ctrlFocus) break;
        } while(ctrl->bStatic==TRUE || ctrl->bVisible==FALSE || ctrl->bEnabled==FALSE);

        if(ctrl && ctrl!=ctrlFocus)
        {
            if(ctrlFocus) ctrlFocus->Focus(FALSE);
            if(ctrl) ctrl->Focus(TRUE);
            ctrlFocus=ctrl;
        }
    }
    else if(ctrlFocus && key && key!=HGEK_LBUTTON && key!=HGEK_RBUTTON)
    {
        if(ctrlFocus->KeyClick(key, hge->Input_GetChar())) return ctrlFocus->id;
    }

// Handle mouse

    BOOL bLDown = hge->Input_GetKeyState(HGEK_LBUTTON);
    BOOL bRDown = hge->Input_GetKeyState(HGEK_RBUTTON);

    if(ctrlLock)
    {
        ctrl=ctrlLock;
        if(!bLDown && !bRDown) ctrlLock=0;
        if(ProcessCtrl(ctrl)) return ctrl->id;
    }
    else
    {
        // Find last (topmost) control

        ctrl=ctrls;
        if(ctrl)
            while(ctrl->next) ctrl=ctrl->next;

        while(ctrl)
        {
            if(ctrl->rect.TestPoint(mx,my) && ctrl->bEnabled)
            {
                if(ctrlOver != ctrl)
                {
                    if(ctrlOver) ctrlOver->MouseOver(FALSE);
                    ctrl->MouseOver(TRUE);
                    ctrlOver=ctrl;
                }

                if(ProcessCtrl(ctrl)) return ctrl->id;
                else return 0;
            }
            ctrl=ctrl->prev;
        }

        if(ctrlOver) {ctrlOver->MouseOver(FALSE); ctrlOver=0;}

    }

    return 0;
}

BOOL hgeGUI::ProcessCtrl(hgeGUIObject *ctrl)
{
    BOOL bResult=FALSE;

    if(bLPressed)    { ctrlLock=ctrl;SetFocus(ctrl->id);bResult=bResult || ctrl->MouseLButton(TRUE); }
    if(bRPressed)    { ctrlLock=ctrl;SetFocus(ctrl->id);bResult=bResult || ctrl->MouseRButton(TRUE); }
    if(bLReleased)    { bResult=bResult || ctrl->MouseLButton(FALSE); }
    if(bRReleased)    { bResult=bResult || ctrl->MouseRButton(FALSE); }
    if(nWheel)        { bResult=bResult || ctrl->MouseWheel(nWheel); }
    bResult=bResult || ctrl->MouseMove(mx-ctrl->rect.x1,my-ctrl->rect.y1);

    return bResult;
}



