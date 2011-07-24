//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   LoadHostList.h
//  Version     :   1.0
//  Creater     :   Cheng Bitao
//  Date        :   2002-1-7 16:26:11
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATE_LOAD_HOST_LIST_H_
#define _UPDATE_LOAD_HOST_LIST_H_

#define defHOST_LIST_FILE_SVR       "Update.SVR"
#define defHOST_LIST_FILE_EXT       "Update.EXT"
#define defHOST_LIST_FILE_ORDER     "Update.ord"


typedef struct tagKHOST_LIST
{
    char szName[MAX_PATH];
    char szURL[MAX_PATH];
    int nIndex;
    tagKHOST_LIST *pNext;
} KHOST_LIST;

class KHostList
{
private:
    char m_szFileName[MAX_PATH];
    int m_nSVRChangedFlag;
    int m_nEXTChangedFlag;

    int LoadList();

    int LoadOrdHostList(const char cszFileName[], KHOST_LIST **ppListHost);
    int LoadSrcHostList(const char cszFileName[], KHOST_LIST **ppHostList);
    void ReleaseHostList(KHOST_LIST **ppHostList);
    void AddHostList(KHOST_LIST *pSrcHostList, KHOST_LIST **ppDestHostList);
    int SaveHostListFileInfo(const char cszDestFileName[], const char cszSrcFileName[]);
    int IsHostListFileChanged(const char cszDestFileName[], const char cszSrcFileName[]);

public:
    KHOST_LIST *m_pHostList;

    KHostList();
    KHostList(const char cszFileName[]);    
    ~KHostList();
    int LoadList(const char cszFileName[]);
    int SaveList(char *pszFileName = NULL);
    void ReleaseList();    
    void AddList(KHOST_LIST *pHostList);
    int IsHostListChanged();

protected:

};


#endif   //_UPDATE_LOAD_HOST_LIST_H_ 
