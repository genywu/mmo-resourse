#ifndef _OP_H_
#define _OP_H_

enum SUB_WINDOW_ID
{
    ID_SAVEBTN,

    ID_PREFIXLIST,
    ID_PREFIXEDIT,

    ID_TEXTLIST,
    ID_TEXTEDIT,

    ID_SEARCHEDIT,
};

void CreateSubWindows(HWND hWnd);

void OnPrefixListSelectChange();
void OnTextListSelectChange();

#endif//#ifndef _OP_H_