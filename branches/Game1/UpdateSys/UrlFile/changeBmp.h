#pragma once

#define BTN_NOR 0
#define BTN_SEL 1
#define BTN_FOC 2
#define BTN_DIS 3

BOOL ChangeBitmapBtnImages(CBitmapButton &button,LPCTSTR lpszFilename);

BOOL SetBitmapBtnImage(CBitmapButton &button,LPCTSTR lpszFilenameNor,int state=0);

