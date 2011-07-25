#include "Precompiled.h"
/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeFont helper class implementation
*/


#include "hgefont.h"

CONST TCHAR FNTHEADERTAG[] = _T("[HGEFONT]");
CONST TCHAR FNTBITMAPTAG[] = _T("Bitmap");
CONST TCHAR FNTCHARTAG[]   = _T("Char");


HGE *hgeFont::hge=0;
TCHAR hgeFont::buffer[1024];


hgeFont::hgeFont(CONST TCHAR *szFont, BOOL bMipmap)
{
    VOID    *data;
    DWORD    size;
    TCHAR    *desc, *pdesc;
    TCHAR    linebuf[256];
    TCHAR    buf[MAX_PATH], *pbuf;
    TCHAR    chr;
    INT        i, x, y, w, h, a, c;

    // Setup variables
    
    hge=hgeCreate(HGE_VERSION);

    fHeight=0.0f;
    fScale=1.0f;
    fProportion=1.0f;
    fRot=0.0f;
    fTracking=0.0f;
    fSpacing=1.0f;
    hTexture=0;

    fZ=0.5f;
    nBlend=BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE;
    dwCol=0xFFFFFFFF;

    ZeroMemory( &letters, sizeof(letters) );
    ZeroMemory( &pre, sizeof(letters) );
    ZeroMemory( &post, sizeof(letters) );
    
    // Load font description

    data=hge->Resource_Load(szFont, &size);
    if(!data) return;

    desc = new TCHAR[size+1];
    memcpy(desc,data,size);
    desc[size]=0;
    hge->Resource_Free(data);

    pdesc=_get_line(desc,linebuf);
    if(_tcscmp(linebuf, FNTHEADERTAG))
    {
        hge->System_Log(_T("Font %s has incorrect format."), szFont);
        delete[] desc;    
        return;
    }

    // Parse font description

    while(pdesc = _get_line(pdesc,linebuf))
    {
        if(!_tcsncicmp(linebuf, FNTBITMAPTAG, sizeof(FNTBITMAPTAG)-1 ))
        {
            _tcscpy(buf,szFont);
            pbuf=_tcsrchr(buf,_T('\\'));
            if(!pbuf) pbuf=_tcsrchr(buf,_T('/'));
            if(!pbuf) pbuf=buf;
            else pbuf++;
            if(!_stscanf(linebuf, _T("Bitmap = %s"), pbuf)) continue;

            hTexture=hge->Texture_Load(buf, 0, bMipmap);
            if(!hTexture)
            {
                delete[] desc;    
                return;
            }
        }

        else if(!_tcsncicmp(linebuf, FNTCHARTAG, sizeof(FNTCHARTAG)-1 ))
        {
            pbuf=_tcsrchr(linebuf,_T('='));
            if(!pbuf) continue;
            pbuf++;
            while(*pbuf==_T(' ')) pbuf++;
            if(*pbuf==_T('\"'))
            {
                pbuf++;
                i=(BYTE)*pbuf++;
                pbuf++; // skip "
            }
            else
            {
                i=0;
                while((*pbuf>='0' && *pbuf<='9') || (*pbuf>='A' && *pbuf<='F') || (*pbuf>='a' && *pbuf<='f'))
                {
                    chr=*pbuf;
                    if(chr >= 'a') chr-='a'-':';
                    if(chr >= 'A') chr-='A'-':';
                    chr-='0';
                    if(chr>0xF) chr=0xF;
                    i=(i << 4) | chr;
                    pbuf++;
                }
                if(i<0 || i>255) continue;
            }
            _stscanf(pbuf, _T(" , %d , %d , %d , %d , %d , %d"), &x, &y, &w, &h, &a, &c);

            letters[i] = new hgeSprite(hTexture, (FLOAT)x, (FLOAT)y, (FLOAT)w, (FLOAT)h);
            pre[i]=(FLOAT)a;
            post[i]=(FLOAT)c;
            if(h>fHeight) fHeight=(FLOAT)h;
        }
    }

    delete[] desc;    
}


hgeFont::~hgeFont()
{
    for(INT i=0; i<256; i++)
        if(letters[i]) delete letters[i];
    if(hTexture) hge->Texture_Free(hTexture);
    hge->Release();
}

VOID hgeFont::Render(FLOAT x, FLOAT y, INT align, CONST TCHAR *string)
{
    INT i;
    FLOAT    fx=x;

    align &= HGETEXT_HORZMASK;
    if(align==HGETEXT_RIGHT) fx-=GetStringWidth(string, FALSE);
    if(align==HGETEXT_CENTER) fx-=INT(GetStringWidth(string, FALSE)/2.0f);

    while(*string)
    {
        if(*string=='\n')
        {
            y += INT(fHeight*fScale*fSpacing);
            fx = x;
            if(align == HGETEXT_RIGHT)  fx -= GetStringWidth(string+1, FALSE);
            if(align == HGETEXT_CENTER) fx -= INT(GetStringWidth(string+1, FALSE)/2.0f);
        }
        else
        {
            i=(BYTE)*string;
            if(!letters[i]) i='?';
            if(letters[i])
            {
                fx += pre[i]*fScale*fProportion;
                letters[i]->RenderEx(fx, y, fRot, fScale*fProportion, fScale);
                fx += (letters[i]->GetWidth()+post[i]+fTracking)*fScale*fProportion;
            }
        }
        string++;
    }
}

VOID hgeFont::printf(FLOAT x, FLOAT y, INT align, CONST TCHAR *format, ...)
{
    TCHAR    *pArg=(TCHAR *) &format+sizeof(format);

    _vsntprintf(buffer, sizeof(buffer)-1, format, (va_list)pArg);
    DWORD dwSize = sizeof(buffer);
    buffer[dwSize - 1]=0;
    //vsprintf(buffer, format, pArg);

    Render(x,y,align,buffer);
}

VOID hgeFont::printfb(FLOAT x, FLOAT y, FLOAT w, FLOAT h, INT align, CONST TCHAR *format, ...)
{
    TCHAR    chr, *pbuf, *prevword, *linestart;
    INT        i,lines=0;
    FLOAT    tx, ty, hh, ww;
    TCHAR    *pArg=(TCHAR *) &format+sizeof(format);

    _vsntprintf(buffer, sizeof(buffer)-1, format, (va_list)pArg);
    DWORD dwSize = sizeof(buffer);
    buffer[dwSize - 1]=0;
    //vsprintf(buffer, format, pArg);

    linestart=buffer;
    pbuf=buffer;
    prevword=0;

    for(;;)
    {
        i=0;
        while(pbuf[i] && pbuf[i]!=' ' && pbuf[i]!='\n') i++;

        chr=pbuf[i];
        pbuf[i]=0;
        ww=GetStringWidth(linestart);
        pbuf[i]=chr;

        if(ww > w)
        {
            if(pbuf==linestart)
            {
                pbuf[i]='\n';
                linestart=&pbuf[i+1];
            }
            else
            {
                *prevword='\n';
                linestart=prevword+1;
            }

            lines++;
        }

        if(pbuf[i]=='\n')
        {
            prevword=&pbuf[i];
            linestart=&pbuf[i+1];
            pbuf=&pbuf[i+1];
            lines++;
            continue;
        }

        if(!pbuf[i]) {lines++;break;}

        prevword=&pbuf[i];
        pbuf=&pbuf[i+1];
    }
    
    tx=x;
    ty=y;
    hh=fHeight*fSpacing*fScale*lines;

    switch(align & HGETEXT_HORZMASK)
    {
        case HGETEXT_LEFT: break;
        case HGETEXT_RIGHT: tx+=w; break;
        case HGETEXT_CENTER: tx+=INT(w/2); break;
    }

    switch(align & HGETEXT_VERTMASK)
    {
        case HGETEXT_TOP: break;
        case HGETEXT_BOTTOM: ty+=h-hh; break;
        case HGETEXT_MIDDLE: ty+=INT((h-hh)/2); break;
    }

    Render(tx,ty,align,buffer);
}

FLOAT hgeFont::GetStringWidth(CONST TCHAR *string, BOOL bMultiline) CONST
{
    INT i;
    FLOAT linew, w = 0;

    while(*string)
    {
        linew = 0;

        while(*string && *string != '\n')
        {
            i=(BYTE)*string;
            if(!letters[i]) i='?';
            if(letters[i])
                linew += letters[i]->GetWidth() + pre[i] + post[i] + fTracking;

            string++;
        }

        if(!bMultiline) return linew*fScale*fProportion;

        if(linew > w) w = linew;

        while (*string == '\n' || *string == '\r') string++;
    }

    return w*fScale*fProportion;
}

VOID hgeFont::SetColor(DWORD col)
{
    dwCol = col;

    for(INT i=0; i<256; i++)
        if(letters[i])
            letters[i]->SetColor(col);
}

VOID hgeFont::SetZ(FLOAT z)
{
    fZ = z;

    for(INT i=0; i<256; i++)
        if(letters[i])
            letters[i]->SetZ(z);
}

VOID hgeFont::SetBlendMode(INT blend)
{
    nBlend = blend;

    for(INT i=0; i<256; i++)
        if(letters[i])
            letters[i]->SetBlendMode(blend);
}

TCHAR *hgeFont::_get_line(TCHAR *file, TCHAR *line)
{
    INT i=0;

    if(!file[i]) return 0;

    while(file[i] && file[i]!='\n' && file[i]!='\r')
    {
        line[i]=file[i];
        i++;
    }
    line[i]=0;

    while(file[i] && (file[i]=='\n' || file[i]=='\r')) i++;

    return file + i;
}