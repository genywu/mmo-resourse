/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI default controls header
*/


#ifndef HGEGUICTRLS_H
#define HGEGUICTRLS_H


#include "hge.h"
#include "hgesprite.h"
#include "hgefont.h"
#include "hgerect.h"
#include "hgegui.h"


#define hgeButtonGetState(gui,id)        ((hgeGUIButton*)gui->GetCtrl(id))->GetState()
#define hgeButtonSetState(gui,id,b)        ((hgeGUIButton*)gui->GetCtrl(id))->SetState(b)
#define hgeSliderGetValue(gui,id)        ((hgeGUISlider*)gui->GetCtrl(id))->GetValue()
#define hgeSliderSetValue(gui,id,f)        ((hgeGUISlider*)gui->GetCtrl(id))->SetValue(f)
#define hgeGetTextCtrl(gui,id)            ((hgeGUIText*)gui->GetCtrl(id))
#define hgeGetListboxCtrl(gui,id)        ((hgeGUIListbox*)gui->GetCtrl(id))


/*
** hgeGUIText
*/
class hgeGUIText : public hgeGUIObject
{
public:
    hgeGUIText(INT id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, hgeFont *fnt);

    VOID            SetMode(INT _align);
    VOID            SetText(CONST TCHAR *_text);
    VOID            printf(CONST TCHAR *format, ...);

    virtual VOID    Render();

private:
    hgeFont*        font;
    FLOAT            tx, ty;
    INT                align;
    TCHAR            text[256];
};


/*
** hgeGUIButton
*/
class hgeGUIButton : public hgeGUIObject
{
public:
    hgeGUIButton(INT id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, HTEXTURE tex, FLOAT tx, FLOAT ty);
    virtual            ~hgeGUIButton();

    VOID            SetMode(BOOL _bTrigger) { bTrigger=_bTrigger; }
    VOID            SetState(BOOL _bPressed) { bPressed=_bPressed; }
    BOOL            GetState() CONST { return bPressed; }

    virtual VOID    Render();
    virtual BOOL    MouseLButton(BOOL bDown);

private:
    BOOL            bTrigger;
    BOOL            bPressed;
    BOOL            bOldState;
    hgeSprite        *sprUp, *sprDown;
};


/*
** hgeGUISlider
*/
#define HGESLIDER_BAR            0
#define HGESLIDER_BARRELATIVE    1
#define HGESLIDER_SLIDER        2

class hgeGUISlider : public hgeGUIObject
{
public:
    hgeGUISlider(INT id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, HTEXTURE tex, FLOAT tx, FLOAT ty, FLOAT sw, FLOAT sh, BOOL vertical=FALSE);
    virtual            ~hgeGUISlider();

    VOID            SetMode(FLOAT _fMin, FLOAT _fMax, INT _mode) { fMin=_fMin; fMax=_fMax; mode=_mode; }
    VOID            SetValue(FLOAT _fVal);
    FLOAT            GetValue() CONST { return fVal; }

    virtual VOID    Render();
    virtual BOOL    MouseMove(FLOAT x, FLOAT y);
    virtual BOOL    MouseLButton(BOOL bDown);

private:
    BOOL            bPressed;
    BOOL            bVertical;
    INT                mode;
    FLOAT            fMin, fMax, fVal;
    FLOAT            sl_w, sl_h;
    hgeSprite        *sprSlider;
};


/*
** hgeGUIListbox
*/
struct hgeGUIListboxItem
{
    TCHAR                text[64];
    hgeGUIListboxItem    *next;
};

class hgeGUIListbox : public hgeGUIObject
{
public:
    hgeGUIListbox(INT id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, hgeFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor);
    virtual            ~hgeGUIListbox();

    INT                AddItem(TCHAR *item);
    VOID            DeleteItem(INT n);
    INT                GetSelectedItem() { return nSelectedItem; }
    VOID            SetSelectedItem(INT n) { if(n>=0 && n<GetNumItems()) nSelectedItem=n; }
    INT                GetTopItem() { return nTopItem; }
    VOID            SetTopItem(INT n) { if(n>=0 && n<=GetNumItems()-GetNumRows()) nTopItem=n; }

    TCHAR            *GetItemText(INT n);
    INT                GetNumItems() { return nItems; }
    INT                GetNumRows() { return INT((rect.y2-rect.y1)/font->GetHeight()); }
    VOID            Clear();

    virtual VOID    Render();
    virtual BOOL    MouseMove(FLOAT x, FLOAT y) { mx=x; my=y; return FALSE; }
    virtual BOOL    MouseLButton(BOOL bDown);
    virtual BOOL    MouseWheel(INT nNotches);
    virtual BOOL    KeyClick(INT key, INT chr);

private:
    hgeSprite        *sprHighlight;
    hgeFont            *font;
    DWORD            textColor, texthilColor;

    INT                    nItems, nSelectedItem, nTopItem;
    FLOAT                mx, my;
    hgeGUIListboxItem    *pItems;
};


#endif
