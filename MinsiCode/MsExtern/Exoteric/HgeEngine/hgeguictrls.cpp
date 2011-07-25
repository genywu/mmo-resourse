#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeGUI default controls implementation
*/


#include "hgeguictrls.h"


/*
** hgeGUIText
*/

hgeGUIText::hgeGUIText(INT _id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, hgeFont *fnt)
{
    id=_id;
    bStatic=TRUE;
    bVisible=TRUE;
    bEnabled=TRUE;
    rect.Set(x, y, x+w, y+h);

    font=fnt;
    tx=x;
    ty=y+(h-fnt->GetHeight())/2.0f;

    text[0]=0;
}

VOID hgeGUIText::SetMode(INT _align)
{
    align=_align;
    if(align==HGETEXT_RIGHT) tx=rect.x2;
    else if(align==HGETEXT_CENTER) tx=(rect.x1+rect.x2)/2.0f;
    else tx=rect.x1;
}

VOID hgeGUIText::SetText(CONST TCHAR *_text)
{
    _tcscpy(text, _text);
}

VOID hgeGUIText::printf(CONST TCHAR *format, ...)
{
    _vstprintf(text, format, (va_list)&format+sizeof(format));
}

VOID hgeGUIText::Render()
{
    font->SetColor(color);
    font->Render(tx,ty,align,text);
}

/*
** hgeGUIButton
*/

hgeGUIButton::hgeGUIButton(INT _id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, HTEXTURE tex, FLOAT tx, FLOAT ty)
{
    id=_id;
    bStatic=FALSE;
    bVisible=TRUE;
    bEnabled=TRUE;
    rect.Set(x, y, x+w, y+h);

    bPressed=FALSE;
    bTrigger=FALSE;

    sprUp = new hgeSprite(tex, tx, ty, w, h);
    sprDown = new hgeSprite(tex, tx+w, ty, w, h);
}

hgeGUIButton::~hgeGUIButton()
{
    if(sprUp) delete sprUp;
    if(sprDown) delete sprDown;
}

VOID hgeGUIButton::Render()
{
    if(bPressed) sprDown->Render(rect.x1, rect.y1);
    else sprUp->Render(rect.x1, rect.y1);
}

BOOL hgeGUIButton::MouseLButton(BOOL bDown)
{
    if(bDown)
    {
        bOldState=bPressed; bPressed=TRUE;
        return FALSE;
    }
    else
    {
        if(bTrigger) bPressed=!bOldState;
        else bPressed=FALSE;
        return TRUE; 
    }
}

/*
** hgeGUISlider
*/

hgeGUISlider::hgeGUISlider(INT _id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, HTEXTURE tex, FLOAT tx, FLOAT ty, FLOAT sw, FLOAT sh, BOOL vertical)
{
    id=_id;
    bStatic=FALSE;
    bVisible=TRUE;
    bEnabled=TRUE;
    bPressed=FALSE;
    bVertical=vertical;
    rect.Set(x, y, x+w, y+h);

    mode=HGESLIDER_BAR;
    fMin=0; fMax=100; fVal=50;
    sl_w=sw; sl_h=sh;

    sprSlider=new hgeSprite(tex, tx, ty, sw, sh);
}

hgeGUISlider::~hgeGUISlider()
{
    if(sprSlider) delete sprSlider;
}

VOID hgeGUISlider::SetValue(FLOAT _fVal)
{
    if(_fVal<fMin) fVal=fMin;
    else if(_fVal>fMax) fVal=fMax;
    else fVal=_fVal;
}

VOID hgeGUISlider::Render()
{
    FLOAT xx, yy;
    FLOAT x1,y1,x2,y2;

    xx=rect.x1+(rect.x2-rect.x1)*(fVal-fMin)/(fMax-fMin);
    yy=rect.y1+(rect.y2-rect.y1)*(fVal-fMin)/(fMax-fMin);
    
    if(bVertical)
        switch(mode)
        {
            case HGESLIDER_BAR: x1=rect.x1; y1=rect.y1; x2=rect.x2; y2=yy; break;
            case HGESLIDER_BARRELATIVE: x1=rect.x1; y1=(rect.y1+rect.y2)/2; x2=rect.x2; y2=yy; break;
            case HGESLIDER_SLIDER: x1=(rect.x1+rect.x2-sl_w)/2; y1=yy-sl_h/2; x2=(rect.x1+rect.x2+sl_w)/2; y2=yy+sl_h/2; break;
        }
    else
        switch(mode)
        {
            case HGESLIDER_BAR: x1=rect.x1; y1=rect.y1; x2=xx; y2=rect.y2; break;
            case HGESLIDER_BARRELATIVE: x1=(rect.x1+rect.x2)/2; y1=rect.y1; x2=xx; y2=rect.y2; break;
            case HGESLIDER_SLIDER: x1=xx-sl_w/2; y1=(rect.y1+rect.y2-sl_h)/2; x2=xx+sl_w/2; y2=(rect.y1+rect.y2+sl_h)/2; break;
        }

    sprSlider->RenderStretch(x1, y1, x2, y2);
}

BOOL hgeGUISlider::MouseLButton(BOOL bDown)
{
    bPressed=bDown;
    return FALSE;
}

BOOL hgeGUISlider::MouseMove(FLOAT x, FLOAT y)
{
    if(bPressed)
    {
        if(bVertical)
        {
            if(y>rect.y2-rect.y1) y=rect.y2-rect.y1;
            if(y<0) y=0;
            fVal=fMin+(fMax-fMin)*y/(rect.y2-rect.y1);
        }
        else
        {
            if(x>rect.x2-rect.x1) x=rect.x2-rect.x1;
            if(x<0) x=0;
            fVal=fMin+(fMax-fMin)*x/(rect.x2-rect.x1);
        }
        return TRUE;
    }

    return FALSE;
}


/*
** hgeGUIListbox
*/

hgeGUIListbox::hgeGUIListbox(INT _id, FLOAT x, FLOAT y, FLOAT w, FLOAT h, hgeFont *fnt, DWORD tColor, DWORD thColor, DWORD hColor)
{
    id=_id;
    bStatic=FALSE;
    bVisible=TRUE;
    bEnabled=TRUE;
    rect.Set(x, y, x+w, y+h);
    font=fnt;
    sprHighlight=new hgeSprite(0, 0, 0, w, fnt->GetHeight());
    sprHighlight->SetColor(hColor);
    textColor=tColor;
    texthilColor=thColor;
    pItems=0;
    nItems=0;

    nSelectedItem=0;
    nTopItem=0;
    mx=0; my=0;
}

hgeGUIListbox::~hgeGUIListbox()
{
    Clear();
    if(sprHighlight) delete sprHighlight;
}


INT hgeGUIListbox::AddItem(TCHAR *item)
{
    hgeGUIListboxItem *pItem=pItems, *pPrev=0, *pNew;

    pNew = new hgeGUIListboxItem;
    memcpy(pNew->text, item, min(sizeof(pNew->text), _tcslen(item)+1));
    DWORD dwSize = sizeof(pNew->text);
    pNew->text[dwSize - 1]='\0';
    pNew->next=0;

    while(pItem) { pPrev=pItem;    pItem=pItem->next; }

    if(pPrev) pPrev->next=pNew;
    else pItems=pNew;
    nItems++;

    return nItems-1;
}

VOID hgeGUIListbox::DeleteItem(INT n)
{
    INT i;
    hgeGUIListboxItem *pItem=pItems, *pPrev=0;

    if(n<0 || n>=GetNumItems()) return;

    for(i=0;i<n;i++) { pPrev=pItem;    pItem=pItem->next; }

    if(pPrev) pPrev->next=pItem->next;
    else pItems=pItem->next;

    delete pItem;
    nItems--;
}

TCHAR *hgeGUIListbox::GetItemText(INT n)
{
    INT i;
    hgeGUIListboxItem *pItem=pItems;

    if(n<0 || n>=GetNumItems()) return 0;

    for(i=0;i<n;i++) pItem=pItem->next;

    return pItem->text;
}

VOID hgeGUIListbox::Clear()
{
    hgeGUIListboxItem *pItem=pItems, *pNext;

    while(pItem)
    {
        pNext=pItem->next;
        delete pItem;
        pItem=pNext;
    }
    
    pItems=0;
    nItems=0;
}

VOID hgeGUIListbox::Render()
{
    INT i;
    hgeGUIListboxItem *pItem=pItems;

    for(i=0;i<nTopItem;i++) pItem=pItem->next;
    for(i=0;i<GetNumRows();i++)
    {
        if(i>=nItems) return;

        if(nTopItem+i == nSelectedItem)
        {
            sprHighlight->Render(rect.x1,rect.y1+i*font->GetHeight());
            font->SetColor(texthilColor);
        }
        else
            font->SetColor(textColor);

        font->Render(rect.x1+3, rect.y1+i*font->GetHeight(), HGETEXT_LEFT, pItem->text);
        pItem=pItem->next;
    }
}

BOOL hgeGUIListbox::MouseLButton(BOOL bDown)
{
    INT nItem;

    if(bDown)
    {
        nItem=nTopItem+INT(my)/INT(font->GetHeight());
        if(nItem<nItems)
        {
            nSelectedItem=nItem;
            return TRUE;
        }
    }
    return FALSE;
}


BOOL hgeGUIListbox::MouseWheel(INT nNotches)
{
    nTopItem-=nNotches;
    if(nTopItem<0) nTopItem=0;
    if(nTopItem>GetNumItems()-GetNumRows()) nTopItem=GetNumItems()-GetNumRows();

    return TRUE;
}

BOOL hgeGUIListbox::KeyClick(INT key, INT chr)
{
    switch(key)
    {
        case HGEK_DOWN:
            if(nSelectedItem < nItems-1)
            {
                nSelectedItem++;
                if(nSelectedItem > nTopItem+GetNumRows()-1) nTopItem=nSelectedItem-GetNumRows()+1;
                return TRUE;
            }
            break;

        case HGEK_UP:
            if(nSelectedItem > 0)
            {
                nSelectedItem--;
                if(nSelectedItem < nTopItem) nTopItem=nSelectedItem;
                return TRUE;
            }
            break;
    }
    return FALSE;
}
