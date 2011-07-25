/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI helper classes header
*/


#ifndef HGEGUI_H
#define HGEGUI_H


#include "hge.h"
#include "hgesprite.h"
#include "hgerect.h"


#define HGEGUI_NONAVKEYS        0
#define HGEGUI_LEFTRIGHT        1
#define HGEGUI_UPDOWN            2
#define HGEGUI_CYCLED            4

class hgeGUI;

/*
** hgeGUIObject
*/
class hgeGUIObject
{
public:
    hgeGUIObject()    { hge=hgeCreate(HGE_VERSION); color=0xFFFFFFFF; }
    virtual            ~hgeGUIObject() { hge->Release(); }

    virtual VOID    Render() = 0;
    virtual VOID    Update(FLOAT dt) {}

    virtual VOID    Enter() {}
    virtual VOID    Leave() {}
    virtual VOID    Reset() {}
    virtual BOOL    IsDone() { return TRUE; }
    virtual VOID    Focus(BOOL bFocused) {}
    virtual VOID    MouseOver(BOOL bOver) {}

    virtual BOOL    MouseMove(FLOAT x, FLOAT y) { return FALSE; }
    virtual BOOL    MouseLButton(BOOL bDown) { return FALSE; }
    virtual BOOL    MouseRButton(BOOL bDown) { return FALSE; }
    virtual BOOL    MouseWheel(INT nNotches) { return FALSE; }
    virtual BOOL    KeyClick(INT key, INT chr) { return FALSE; }

    virtual VOID    SetColor(DWORD _color) { color=_color; }
    
    INT                id;
    BOOL            bStatic;
    BOOL            bVisible;
    BOOL            bEnabled;
    hgeRect            rect;
    DWORD            color;

    hgeGUI            *gui;
    hgeGUIObject    *next;
    hgeGUIObject    *prev;

protected:
    hgeGUIObject(CONST hgeGUIObject &go);
    hgeGUIObject&    operator= (CONST hgeGUIObject &go);

    static HGE        *hge;
};


/*
** hgeGUI
*/
class hgeGUI
{
public:
    hgeGUI();
    ~hgeGUI();

    VOID            AddCtrl(hgeGUIObject *ctrl);
    VOID            DelCtrl(INT id);
    hgeGUIObject*    GetCtrl(INT id) CONST;

    VOID            MoveCtrl(INT id, FLOAT x, FLOAT y);
    VOID            ShowCtrl(INT id, BOOL bVisible);
    VOID            EnableCtrl(INT id, BOOL bEnabled);

    VOID            SetNavMode(INT mode);
    VOID            SetCursor(hgeSprite *spr);
    VOID            SetColor(DWORD color);
    VOID            SetFocus(INT id);
    INT                GetFocus() CONST;
    
    VOID            Enter();
    VOID            Leave();
    VOID            Reset();
    VOID            Move(FLOAT dx, FLOAT dy);

    INT                Update(FLOAT dt);
    VOID            Render();

private:
    hgeGUI(CONST hgeGUI &);
    hgeGUI&            operator= (CONST hgeGUI&);
    BOOL            ProcessCtrl(hgeGUIObject *ctrl);

    static HGE        *hge;

    hgeGUIObject    *ctrls;
    hgeGUIObject    *ctrlLock;
    hgeGUIObject    *ctrlFocus;
    hgeGUIObject    *ctrlOver;

    INT                navmode;
    INT                nEnterLeave;
    hgeSprite        *sprCursor;

    FLOAT            mx,my;
    INT                nWheel;
    BOOL            bLPressed, bLReleased;
    BOOL            bRPressed, bRReleased;
};


#endif
