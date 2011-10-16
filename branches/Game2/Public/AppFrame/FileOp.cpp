#include "stdafx.h"
#include "FileOp.h"

#include <commdlg.h>
#include "AppFrameDef.h"

namespace AppFrame
{

static OPENFILENAME ofn;

void FileOpInit(const char* filter)
{
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetWnd();
    ofn.hInstance = NULL;
    ofn.lpstrFilter = filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = NULL;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;
    ofn.Flags = 0;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "xml";
    ofn.lCustData = 0L;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;
    ofn.pvReserved = NULL;
    ofn.dwReserved = 0;
    ofn.FlagsEx = 0;
}

uint FileOpenDlg(char* files)
{
    ofn.lpstrFile = files;
    ofn.Flags |= OFN_ALLOWMULTISELECT|OFN_EXPLORER;
    if (!::GetOpenFileName(&ofn))
        return 0;
    return ofn.nFileOffset;
}

}; // end AppFrame
