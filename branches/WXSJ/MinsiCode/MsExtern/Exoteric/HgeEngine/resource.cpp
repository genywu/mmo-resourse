#include "Precompiled.h"
/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: resources management
*/


#include "hge_impl.h"

#define NOCRYPT
//#define NOUNCRYPT
#include "unzip.h"
//#pragma warning(disable : 4996)


BOOL CALL HGE_Impl::Resource_AttachPack(CONST TCHAR *filename, CONST TCHAR *password)
{
    TCHAR *szName;
    CResourceList *resItem=res;
    unzFile zip;

    szName=Resource_MakePath(filename);
    _tcsupr(szName);

    while(resItem)
    {
        if(!_tcscmp(szName,resItem->filename)) return FALSE;
        resItem=resItem->next;
    }
    
    zip=unzOpen(szName);
    if(!zip) return FALSE;
    unzClose(zip);

    resItem=new CResourceList;
    _tcscpy(resItem->filename, szName);
    if(password) _tcscpy(resItem->password, password);
    else resItem->password[0]=0;
    resItem->next=res;
    res=resItem;

    return TRUE;
}

VOID CALL HGE_Impl::Resource_RemovePack(CONST TCHAR *filename)
{
    TCHAR *szName;
    CResourceList *resItem=res, *resPrev=0;

    szName=Resource_MakePath(filename);
    _tcsupr(szName);

    while(resItem)
    {
        if(!_tcscmp(szName,resItem->filename))
        {
            if(resPrev) resPrev->next=resItem->next;
            else res=resItem->next;
            delete resItem;
            break;
        }

        resPrev=resItem;
        resItem=resItem->next;
    }
}

VOID CALL HGE_Impl::Resource_RemoveAllPacks()
{
    CResourceList *resItem=res, *resNextItem;

    while(resItem)
    {
        resNextItem=resItem->next;
        delete resItem;
        resItem=resNextItem;
    }

    res=0;
}

VOID* CALL HGE_Impl::Resource_Load(CONST TCHAR *filename, DWORD *size)
{
    static TCHAR *res_err = _T("Can't load resource: %s");

    CResourceList *resItem=res;
    TCHAR szName[_MAX_PATH];
    TCHAR szZipName[_MAX_PATH];
    unzFile zip;
    unz_file_info file_info;
    INT done, i;
    VOID *ptr;
    HANDLE hF;

    if(filename[0]=='\\' || filename[0]=='/' || filename[1]==':') goto _fromfile; // skip absolute paths

    // Load from pack
 
    _tcscpy(szName,filename);
    _tcsupr(szName);
    for(i=0; szName[i]; i++) { if(szName[i]=='/') szName[i]='\\'; }

    while(resItem)
    {
        zip=unzOpen(resItem->filename);
        done=unzGoToFirstFile(zip);
        while(done==UNZ_OK)
        {
            unzGetCurrentFileInfo(zip, &file_info, szZipName, sizeof(szZipName), NULL, 0, NULL, 0);
            _tcsupr(szZipName);
            for(i=0; szZipName[i]; i++)    { if(szZipName[i]=='/') szZipName[i]='\\'; }
            if(!_tcscmp(szName,szZipName))
            {
                if(unzOpenCurrentFilePassword(zip, resItem->password[0] ? resItem->password : 0) != UNZ_OK)
                {
                    unzClose(zip);
                    _stprintf(szName, res_err, filename);
                    _PostError(szName);
                    return 0;
                }

                ptr = malloc(file_info.uncompressed_size);
                if(!ptr)
                {
                    unzCloseCurrentFile(zip);
                    unzClose(zip);
                    _stprintf(szName, res_err, filename);
                    _PostError(szName);
                    return 0;
                }

                if(unzReadCurrentFile(zip, ptr, file_info.uncompressed_size) < 0)
                {
                    unzCloseCurrentFile(zip);
                    unzClose(zip);
                    free(ptr);
                    _stprintf(szName, res_err, filename);
                    _PostError(szName);
                    return 0;
                }
                unzCloseCurrentFile(zip);
                unzClose(zip);
                if(size) *size=file_info.uncompressed_size;
                return ptr;
            }
            
            done=unzGoToNextFile(zip);
        }
        
        unzClose(zip);
        resItem=resItem->next;
    }

    // Load from file
_fromfile:

    hF = CreateFile(Resource_MakePath(filename), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
    if(hF == INVALID_HANDLE_VALUE)
    {
        _stprintf(szName, res_err, filename);
        _PostError(szName);
        return 0;
    }
    file_info.uncompressed_size = GetFileSize(hF, NULL);
    ptr = malloc(file_info.uncompressed_size);
    if(!ptr)
    {
        CloseHandle(hF);
        _stprintf(szName, res_err, filename);
        _PostError(szName);
        return 0;
    }
    if(ReadFile(hF, ptr, file_info.uncompressed_size, &file_info.uncompressed_size, NULL ) == 0)
    {
        CloseHandle(hF);
        free(ptr);
        _stprintf(szName, res_err, filename);
        _PostError(szName);
        return 0;
    }
    
    CloseHandle(hF);
    if(size) *size=file_info.uncompressed_size;
    return ptr;
}


VOID CALL HGE_Impl::Resource_Free(VOID *res)
{
    if(res) free(res);
}


TCHAR* CALL HGE_Impl::Resource_MakePath(CONST TCHAR *filename)
{
    INT i;

    if(!filename)
        _tcscpy(szTmpFilename, szAppPath);
    else if(filename[0]=='\\' || filename[0]=='/' || filename[1]==':')
        _tcscpy(szTmpFilename, filename);
    else
    {
        _tcscpy(szTmpFilename, szAppPath);
        if(filename) _tcscat(szTmpFilename, filename);
    }

    for(i=0; szTmpFilename[i]; i++) { if(szTmpFilename[i]=='/') szTmpFilename[i]='\\'; }
    return szTmpFilename;
}

TCHAR* CALL HGE_Impl::Resource_EnumFiles(CONST TCHAR *wildcard)
{
    if(wildcard)
    {
        if(hSearch) { FindClose(hSearch); hSearch=0; }
        hSearch=FindFirstFile(Resource_MakePath(wildcard), &SearchData);
        if(hSearch==INVALID_HANDLE_VALUE) { hSearch=0; return 0; }

        if(!(SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return SearchData.cFileName;
        else return Resource_EnumFiles();
    }
    else
    {
        if(!hSearch) return 0;
        for(;;)
        {
            if(!FindNextFile(hSearch, &SearchData))    { FindClose(hSearch); hSearch=0; return 0; }
            if(!(SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return SearchData.cFileName;
        }
    }
}

TCHAR* CALL HGE_Impl::Resource_EnumFolders(CONST TCHAR *wildcard)
{
    if(wildcard)
    {
        if(hSearch) { FindClose(hSearch); hSearch=0; }
        hSearch=FindFirstFile(Resource_MakePath(wildcard), &SearchData);
        if(hSearch==INVALID_HANDLE_VALUE) { hSearch=0; return 0; }

        if((SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
           _tcscmp(SearchData.cFileName,_T(".")) && _tcscmp(SearchData.cFileName,_T("..")))
                return SearchData.cFileName;
        else return Resource_EnumFolders();
    }
    else
    {
        if(!hSearch) return 0;
        for(;;)
        {
            if(!FindNextFile(hSearch, &SearchData))    { FindClose(hSearch); hSearch=0; return 0; }
            if((SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
               _tcscmp(SearchData.cFileName,_T(".")) && _tcscmp(SearchData.cFileName,_T("..")))
                    return SearchData.cFileName;
        }
    }
}
