#pragma once                    //┃//┃
#ifndef __GAMETREECAPTION_H__   //┃//┃
#define __GAMETREECAPTION_H__   //┃//┃
//━━━━━━━━━━━━━━━━┛//┃
//文件名称：GameTreeCaption.h       //┃
//功能描述：游戏树标题              //┃
//平台相关：Windows                 //┃
//━━━━━━━━━━━━━━━━━━┛

// 游戏树标题
class CGameTreeCaption
{
public:
    CGameTreeCaption(CMsGdiBuffImage* pBuffImage, HFONT hFont = NULL);
    ~CGameTreeCaption(VOID);

    BOOL SetFont(INT nSize, DWORD dwWeight, BOOL bItalic, BOOL bUnderline, TCHAR* lpszFontName);
    BOOL PrintText(RECT* lpRc, TCHAR* szText, INT nTextLen, COLORREF cColor, DWORD dwFormat);

private:

    HFONT               m_hFont;
    CMsGdiBuffImage*    m_pBuffImage;
};

//━━━━━━━━━━┓
#endif//#pragma once  ┃
//━━━━━━━━━━┛
