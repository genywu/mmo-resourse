////////////////////////////////////////////////////////////////////////////////
//
//
//  FileName    :   KAVLangImport.h
//  Creator     :   Shen Min
//  Date        :   2002-12-3
//  Comment     :   Import file of MultiLanguge Module
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KAV_LANG_IMPORT_H_
#define _KAV_LANG_IMPORT_H_


#define defKAVLANG_MODULENAME  "KAVLang.DLL"

typedef enum {enumCHS, enumENU, enumCHT, enumJPN} ML_ENUM_LANG;

int __stdcall ML_Init(ML_ENUM_LANG CurLang);
int __stdcall ML_UnInit();
int __stdcall ML_SetLanguage(ML_ENUM_LANG CurLang);

int __stdcall ML_LoadStringFromName(
    const char cszStringName[],
    char szBuffer[],
    int nBufferSize,
    int *pnReturnSize
);
int __stdcall ML_LoadStringFromID(
    int nID,
    char szBuffer[],
    int nBufferSize,
    int *pnReturnSize
);

int __stdcall ML_LoadBitmapFromName(
    const char cszStringName[],
    HBITMAP *phBitmap
);
int __stdcall ML_LoadBitmapFromID(
    int nID,
    HBITMAP *phBitmap
);

int __stdcall ML_LoadBitmapFileFromName(
    const char cszStringName[],
    char szBuffer[],
    int nBufferSize,
    int *pnReturnSize
);

int __stdcall ML_LoadBitmapFileFromID(
    int nID,
    char szBuffer[],
    int nBufferSize,
    int *pnReturnSize
);


#endif  //_KAV_LANG_IMPORT_H_
